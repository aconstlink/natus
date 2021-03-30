
#pragma once

#include "task.hpp"

#include <natus/memory/global.h>

#include <future>

namespace natus
{
    namespace concurrent
    {
        class loose_thread_scheduler
        {
            natus_this_typedefs( loose_thread_scheduler ) ;

        public:

            natus_typedefs( natus::ntd::vector< natus::concurrent::task_res_t >, tasks ) ;

        private:

            struct shared_data
            {
                natus::concurrent::mutex_t mtx ;
                this_ptr_t owner = nullptr ;

                shared_data( void_t ) noexcept {}
                shared_data( shared_data && rhv ) noexcept
                {
                    natus_move_member_ptr( owner, rhv ) ;
                }
            } ;
            natus_typedef( shared_data ) ;
            shared_data_ptr_t _sd = nullptr ;

            natus::concurrent::mutex_t _mtx ;
            tasks_t _tasks ;

            natus::concurrent::mutex_t _mtx_f ;
            natus::ntd::vector< std::future<void_t> > _futures ;

        public:

            loose_thread_scheduler( void_t ) noexcept
            {
                shared_data sd ;
                sd.owner = this ;
                
                _sd = natus::memory::global_t::alloc( std::move( sd ), 
                    "[loose_thread_scheduler] : shared data" ) ;
            }

            loose_thread_scheduler( this_cref_t ) = delete ;
            loose_thread_scheduler( this_rref_t rhv ) noexcept
            {
                {
                    natus::concurrent::lock_t lk(rhv._sd->mtx) ;
                    rhv._sd->owner = this ;
                }
                natus_move_member_ptr( _sd, rhv ) ;
                _tasks = std::move( rhv._tasks ) ;
            }

            ~loose_thread_scheduler( void_t ) noexcept
            {
                natus::memory::global_t::dealloc( _sd ) ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                {
                    natus::concurrent::lock_t lk(rhv._sd->mtx) ;
                    rhv._sd->owner = this ;
                }
                natus_move_member_ptr( _sd, rhv ) ;
                _tasks = std::move( rhv._tasks ) ;
                return *this ;
            }

        private:

            void_t place_future( std::future< void_t > && f_ ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx_f ) ;
                for( auto & f : _futures )
                {
                    if( f.wait_for( std::chrono::microseconds(0) ) == std::future_status::ready )
                    {
                        f = std::move( f_ ) ;
                        return ;
                    }
                }
                _futures.emplace_back( std::move( f_ ) ) ;
            }

            void_t execute( task_res_t t ) noexcept
            {
                if( natus::concurrent::task::scheduler_accessor::will_execute( t ) ) 
                {
                    auto f = std::async( std::launch::async, [=]( void_t )
                    {
                        natus::concurrent::task::scheduler_accessor::execute( t ) ;

                        tasks_t tasks ;
                        natus::concurrent::task::scheduler_accessor::schedule( t, tasks ) ;

                        _sd->owner->schedule( std::move( tasks ) ) ;
                    } ) ;
                    this_t::place_future( std::move(f) ) ;
                }
            }

        public:

            void_t schedule( task_res_t t ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                 _tasks.emplace_back( std::move( t ) ) ;
            }

            void_t schedule( tasks_rref_t tasks ) noexcept
            {
                natus::concurrent::lock_t lk( _mtx ) ;
                for( auto t : tasks )
                {
                    _tasks.emplace_back( std::move( t ) ) ;
                }
            }

            void_t update( void_t ) noexcept
            {
                tasks_t tasks ;
                {
                    natus::concurrent::lock_t lk( _mtx ) ;
                    tasks = std::move( _tasks ) ;
                }

                for( auto & t : tasks )
                {
                    this_t::execute( t ) ;
                }
            }

            void_t yield( std::function< bool_t ( void_t ) > funk ) noexcept 
            {
                while( funk() ) std::this_thread::yield() ;
            }
        };
        natus_typedef( loose_thread_scheduler ) ;
    }
}