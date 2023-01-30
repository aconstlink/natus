#pragma once

#include "range_1d.hpp"

#include "global.h"
#include "semaphore.hpp"
#include <thread>

namespace natus
{
    namespace concurrent
    {
        template< typename T >
        using funk_t = std::function< void_t ( natus::concurrent::range_1d< T > const & ) > ;

        template< typename T >
        void_t parallel_for( natus::concurrent::range_1d< T > const& r, natus::concurrent::funk_t<T> f )
        {
            typedef natus::concurrent::range_1d<T> range_t ;

            //
            // make simple for if input space is too small
            //
            if( size_t( r.difference() ) < 100 )
            {
                f(r) ;
                return ;
            }

            // 
            // do parallel_for
            //

            size_t const num_splits = std::thread::hardware_concurrency() ;

            T const per_task = r.difference() / T( num_splits ) ;
            T const rest_task = r.difference() - per_task * T( num_splits ) ;
            
            natus::concurrent::semaphore_t sem_counter( num_splits + (rest_task > 0 ? 1 : 0) ) ;

            T start = r.begin() ;

            for( size_t i=0; i<num_splits; ++i ) 
            {
                T end = start + per_task ;
                range_t const lr( start, end ) ;

                natus::concurrent::global_t::schedule( natus::concurrent::global_t::make_task( [&, lr]( natus::concurrent::task_res_t )
                {
                    f( lr ) ;
                    --sem_counter ;
                } ), natus::concurrent::schedule_type::pool ) ;

                start = end ;
            }

            if( rest_task > 0 )
            {
                T end = start + rest_task ;
                range_t lr( start, end ) ;

                natus::concurrent::global_t::schedule( natus::concurrent::global_t::make_task( [&]( natus::concurrent::task_res_t )
                {
                    f( lr ) ;
                    --sem_counter ;
                } ), natus::concurrent::schedule_type::pool );
            }

            natus::concurrent::global_t::yield( [&]( void_t )
            {
                return sem_counter != 0 ;
            } ) ;

            #if 0
            size_t const num_threads = ::std::thread::hardware_concurrency() ;

            natus_typedefs( natus::concurrent::job_1d<T>, job ) ;

            if( size_t( r.difference() ) < num_threads * 50 )
            {
                job_1d< T >( r, f ).execute() ;
                return ;
            }

            natus_typedefs( natus::concurrent::range_1d<T>, range ) ;
            natus_typedefs( natus::memory::experimental::arena< job_t >, job_arena ) ;

            static job_arena_t local_arena ;

            T const per_job = r.difference() / T( num_threads ) ;
            T const rest_job = r.difference() - per_job * T( num_threads ) ;

            T start = r.begin() ;

            natus::concurrent::mutex_t mtx ;
            natus::concurrent::condition_variable_t cv ;

            size_t num_jobs = num_threads + ( rest_job > 0 ? 1 : 0 ) ;

            // 1. issue number of threads jobs
            {
                for( size_t i = 0; i < num_threads; ++i )
                {
                    T end = start + per_job ;
                    range_t lr( start, end ) ;

                    job_ptr_t jptr = local_arena.alloc( job_t( lr, f ) ) ;

                    natus::concurrent::global::job_scheduler()->schedule( jptr, [&] ( natus::concurrent::ijob_ptr_t j )
                    {
                        {
                            natus::concurrent::lock_guard_t lk( mtx ) ;
                            --num_jobs ;
                        }
                        cv.notify_all() ;

                        local_arena.dealloc( static_cast< job_ptr_t >( j ) ) ;

                    } ) ;

                    start = end ;
                }
            }

            // 2. if required jobs do not divide equally, issue rest job
            {
                if( rest_job > 0 )
                {
                    T end = start + rest_job ;
                    range_t lr( start, end ) ;

                    job_ptr_t jptr = local_arena.alloc( job_t( lr, f ) ) ;

                    natus::concurrent::global::job_scheduler()->schedule( jptr, [&] ( natus::concurrent::ijob_ptr_t j )
                    {
                        {
                            natus::concurrent::lock_t lk( mtx ) ;
                            --num_jobs ;
                        }
                        cv.notify_all() ;

                        local_arena.dealloc( static_cast< job_ptr_t >( j ) ) ;

                    } ) ;
                }
            }

            // 3. wait for all jobs to be finished
            {
                natus::concurrent::lock_t lk( mtx ) ;
                while( num_jobs > 0 ) cv.wait( lk ) ;
            }

            #endif
        }
        
    }
}
