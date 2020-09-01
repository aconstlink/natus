#pragma once

#include "typedefs.h"

#include <natus/ntd/vector.hpp>
#include <natus/memory/res.hpp>

namespace natus
{
    namespace concurrent
    {
        class task
        {
            natus_this_typedefs( task ) ;

        public:

            typedef std::function< void_t ( this_ref_t ) > task_funk_t ;

        private:
            
            typedef natus::memory::res< this_t > this_res_t ;
            typedef natus::ntd::vector< this_res_t > tasks_t ;

            // internal tasks
            tasks_t _tasks ;
            // the follow up task
            this_res_t _then ;
            // the funk to be executed
            task_funk_t _funk ;

        public:

            task( void_t ) noexcept
            {
                _funk = [&] ( this_ref_t ) { } ;
            }

            task( task_funk_t f ) noexcept : _funk( f ) 
            {}

            task( this_cref_t rhv ) noexcept
            {
                _tasks = rhv._tasks ;
                _funk = rhv._funk ;
            }

            task( this_rref_t rhv ) noexcept
            {
                _tasks = std::move( rhv._tasks ) ;
                _funk = std::move( rhv._funk ) ;
            }

            this_ref_t operator = ( task_funk_t funk ) noexcept
            {
                _funk = funk ;
                return *this ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept 
            {
                _tasks = rhv._tasks ;
                _funk = rhv._funk ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _tasks = std::move( rhv._tasks ) ;
                _funk = std::move( rhv._funk ) ;
                return *this ;
            }

            this_ref_t insert( task_funk_t t ) noexcept
            {
                _tasks.insert( _tasks.begin(), this_t( t ) ) ;
                return *this ;
            }

            this_ref_t insert( this_cref_t t ) noexcept
            {
                _tasks.insert( _tasks.begin(), t ) ;
                return *this ;
            }

            this_ref_t insert( this_rref_t t ) noexcept
            {
                _tasks.insert( _tasks.begin(), std::move( t ) ) ;
                return *this ;
            }

            this_ref_t then( task_funk_t t ) noexcept
            {
                _then = this_t( t ) ;
                return *this ;
            }

            this_ref_t then( this_cref_t t ) noexcept
            {
                _then = t ;
                return *this ;
            }

            this_ref_t then( this_rref_t t ) noexcept
            {
                _then = std::move( t ) ;
                return *this ;
            }

            operator task_funk_t( void_t ) noexcept
            {
                return this_t() ;
            }

        public:

            void_t exec( void_t ) noexcept
            {
                _funk( *this ) ;
            }

        public:

            void_t copy_tasks( natus::ntd::vector< this_res_t > & other )
            {
                other.resize( other.size() + _tasks.size() ) ;

                size_t i = other.size() ;
                for( auto & t : _tasks )
                {
                    other[ i++ ] = t ;
                }
            }
        };
        natus_res_typedef( task ) ;
    }
}