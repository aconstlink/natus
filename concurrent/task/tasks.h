#pragma once

#include "task.h"

namespace natus
{
    namespace concurrent
    {
        template< typename funk_t >
        class funk_task : public task
        {
            natus_this_typedefs( funk_task<funk_t> ) ;

        private:

            funk_t _funk ;

        public:

            funk_task( void_t ) : _funk( [=] ( void_t ) { } )
            {}

            funk_task( funk_t f )
            {
                _funk = f ;
            }

            funk_task( this_rref_t rhv ) : task( ::std::move( rhv ) )
            {
                _funk = ::std::move( rhv._funk ) ;
            }

            virtual ~funk_task( void_t )
            {}

        public:

            static this_ptr_t create( funk_t f )
            {
                return natus::concurrent::memory::alloc( this_t( f ),
                    "[natus::concurrent::funk_task::create] : unknown function task" ) ;
            }

            static this_ptr_t create( funk_t f, natus::memory::purpose_cref_t p )
            {
                return natus::concurrent::memory::alloc( this_t( f ), p ) ;
            }

            static this_ptr_t create_noop( natus::memory::purpose_cref_t p )
            {
                return natus::concurrent::memory::alloc( this_t(), p ) ;
            }

            static this_ptr_t create( this_rref_t rhv, natus::memory::purpose_cref_t p )
            {
                return natus::concurrent::memory::alloc( ::std::move( rhv ), p ) ;
            }

            static void_t destroy( this_ptr_t ptr )
            {
                natus::concurrent::memory::dealloc( ptr ) ;
            }

        public:

            virtual void_t run( void_t )
            {
                _funk() ;
            }

            virtual void_t destroy( void_t )
            {
                this_t::destroy( this ) ;
            }
        };
        natus_typedefs( funk_task< ::std::function< void_t( void_t ) > >, void_funk_task ) ;

        /// like a void funk except that it passes itself into the funk call
        /// this allows to then() dynamically. => dyn_task
        class dyn_task : public task
        {
            natus_this_typedefs( dyn_task ) ;

            typedef ::std::function< void_t ( natus::concurrent::itask_ptr_t ) > funk_t ;

        private:

            funk_t _funk ;

        public:

            dyn_task( void_t ) : _funk( [=] ( natus::concurrent::itask_ptr_t ) { } )
            {}

            dyn_task( funk_t f )
            {
                _funk = f ;
            }

            dyn_task( this_rref_t rhv ) : task( ::std::move( rhv ) )
            {
                _funk = ::std::move( rhv._funk ) ;
            }

            virtual ~dyn_task( void_t )
            {}

        public:

            static this_ptr_t create( funk_t f )
            {
                return natus::concurrent::memory::alloc( this_t( f ),
                    "[natus::concurrent::dyn_task::create] : unknown function task" ) ;
            }

            static this_ptr_t create( funk_t f, natus::memory::purpose_cref_t p )
            {
                return natus::concurrent::memory::alloc( this_t( f ), p ) ;
            }

            static this_ptr_t create_default( natus::memory::purpose_cref_t p )
            {
                return natus::concurrent::memory::alloc( this_t(), p ) ;
            }

            static this_ptr_t create( this_rref_t rhv, natus::memory::purpose_cref_t p )
            {
                return natus::concurrent::memory::alloc( ::std::move( rhv ), p ) ;
            }

            static void_t destroy( this_ptr_t ptr )
            {
                natus::concurrent::memory::dealloc( ptr ) ;
            }

        public:

            virtual void_t run( void_t )
            {
                _funk( this ) ;
            }

            virtual void_t destroy( void_t )
            {
                this_t::destroy( this ) ;
            }
        };
        natus_typedef( dyn_task ) ;
    }
}