#pragma once

#include "typedefs.h"
#include "task/task.hpp"

namespace natus
{
    namespace concurrent
    {
        class thread_pool
        {
            natus_this_typedefs( thread_pool ) ;
            natus_typedefs( natus::ntd::vector< natus::concurrent::task_res_t >, tasks ) ;

            struct thread_data
            {
                thread_data( void_t ) noexcept {}
                thread_data( thread_data && rhv ) noexcept
                {
                    thread = std::move( rhv.thread ) ;
                }
                ~thread_data( void_t ) noexcept {}

                std::thread thread ;
                std::mutex mtx ;
            };
            natus_res_typedef( thread_data ) ;

            struct shared_data
            {
                natus_this_typedefs( shared_data ) ;

                natus::concurrent::mutex_t pool_mtx ;
                natus::ntd::vector< thread_data_res_t > pool ;
                std::condition_variable pool_cv ;

                natus::concurrent::mutex_t tasks_mtx ;
                natus::ntd::vector< natus::concurrent::task_res_t > tasks ;
                std::condition_variable tasks_cv ;

                size_t threads_running = 0 ;
                size_t threads_max = 0 ;

                shared_data( void_t ) noexcept {}
                shared_data( this_rref_t rhv ) noexcept
                {
                    pool = std::move( rhv.pool ) ;
                    tasks = std::move( rhv.tasks ) ;
                    threads_running = rhv.threads_running ;
                    threads_max = rhv.threads_max ;
                }

                void_t inc_thread_count( void_t ) noexcept
                {
                    natus::concurrent::lock_guard_t lk( pool_mtx ) ;
                    threads_running++ ;
                }

                void_t dec_thread_count( void_t ) noexcept
                {
                    natus::concurrent::lock_guard_t lk( pool_mtx ) ;
                    threads_running-- ;
                }

                void_t add_task( natus::concurrent::task_res_t t ) noexcept
                {
                    natus::concurrent::lock_guard_t lk( tasks_mtx ) ;
                    tasks.emplace_back( t ) ;
                    tasks_cv.notify_one() ;
                }

                bool_t wait_for_thread_wakeup( void_t ) noexcept
                {
                    natus::concurrent::lock_t lk( pool_mtx ) ;
                    while( threads_running >= threads_max ) 
                    {
                        pool_cv.wait( lk ) ;
                        if( threads_max == 0 ) return false ;
                    }
                    threads_running++ ;

                    return true ;
                }

                bool_t wait_for_tasks( void_t ) noexcept
                {
                    natus::concurrent::lock_t lk( tasks_mtx ) ;
                    while( tasks.size() == 0 ) 
                    {
                        tasks_cv.wait( lk ) ;
                        if( threads_max == 0 ) return false ;
                    }
                    return true ;
                }
                
            } ;
            natus_typedef( shared_data ) ;
            shared_data_ptr_t _sd = nullptr ;

            typedef std::function< void_t ( void_t ) > thread_funk_t ;
            thread_funk_t _thread_funk ;

        public:

            thread_pool( void_t ) 
            {
                size_t const num_threads = std::thread::hardware_concurrency() ;
                size_t const thread_pool_size = num_threads<<2 ;

                {
                    _sd = natus::memory::global_t::alloc( shared_data(), 
                        "[thread_pool] : shared data" ) ;
                
                    _sd->pool.resize( thread_pool_size ) ;
                    _sd->threads_max = num_threads ;
                }

                for( size_t i=0; i<thread_pool_size; ++i )
                {
                    thread_data_res_t res = thread_data() ;

                    auto thd = std::thread( [=]( void_t )
                    {
                        while( _sd->threads_max != 0 )
                        {
                            if( !_sd->wait_for_thread_wakeup() ) break ;
                            if( !_sd->wait_for_tasks() ) break ;

                            // go into taking tasks loop
                            while( true )
                            {
                                natus::concurrent::task_res_t task ;

                                // take task
                                {
                                    natus::concurrent::lock_guard_t lk( _sd->tasks_mtx ) ;
                                    if( _sd->tasks.size() == 0 ) break ;
                                    
                                    task = _sd->tasks[0] ;
                                    _sd->tasks[0] = _sd->tasks[_sd->tasks.size()-1] ;
                                    _sd->tasks.resize( _sd->tasks.size() - 1 ) ;
                                }

                                if( natus::concurrent::task::scheduler_accessor::will_execute( task ) ) 
                                {
                                    natus::concurrent::task::scheduler_accessor::execute( task ) ;

                                    tasks_t tasks ;
                                    natus::concurrent::task::scheduler_accessor::schedule( task, tasks ) ;

                                    for( auto & t : tasks ) this->schedule( t ) ;
                                }
                            }

                            _sd->dec_thread_count() ;
                        }
                    } ) ;

                    _sd->pool[i] = res ;
                    _sd->pool[i]->thread = std::move( thd ) ;
                }
            }

            thread_pool( this_rref_t rhv ) noexcept
            {
                natus_move_member_ptr( _sd, rhv ) ;
                rhv._thread_funk = rhv._thread_funk ;
            }

            ~thread_pool( void_t ) noexcept
            {
            }

        public:

            bool_t yield( natus::concurrent::sync_object_res_t ) noexcept
            {
                return false ;
            }

            void_t schedule( natus::concurrent::task_res_t t ) noexcept
            {
                _sd->add_task( t ) ;
            }

            void_t shutdown( void_t ) noexcept 
            {
                #if 0
                for( auto & ptr : _thread_pool )
                {
                    if( ptr->run )
                    {
                        natus::concurrent::lock_guard_t lk( ptr->mtx ) ;
                        ptr->run = false ;
                        ptr->cv.notify_all() ;
                    }
                }

                for( auto & ptr : _thread_pool )
                {
                    if( ptr->thread.joinable() ) ptr->thread.join() ;
                }
                #endif
            }
        };
        natus_typedef( thread_pool ) ;
    }
}