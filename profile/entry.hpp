
#pragma once

#include "api.h"
#include "typedefs.h"

namespace natus
{
    namespace profile
    {
        class entry
        {
            natus_this_typedefs( entry ) ;
            natus_typedefs( std::chrono::high_resolution_clock, clock ) ;

        private:

            bool_t _started = false ;

            natus::ntd::string_t _name = "noname" ;

            clock_t::time_point _begin ;
            clock_t::time_point _end ;

        public:

            entry( natus::ntd::string_t name  ) noexcept : _name( name )
            {
                this_t::begin() ;
            }

            entry( this_cref_t rhv ) noexcept
            {
                _name = rhv._name ;
                _begin = rhv._begin ;
                _end = rhv._end ;
                _started = rhv._started ;

                if( _started ) this_t::end() ;
            }

            entry( this_rref_t rhv ) noexcept
            {
                _name = std::move( rhv._name ) ;
                _begin = std::move( rhv._begin ) ;
                _end = std::move( rhv._end ) ;
                _started = rhv._started ;

                if( _started ) this_t::end() ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _name = rhv._name ;
                _begin = rhv._begin ;
                _end = rhv._end ;
                _started = rhv._started ;

                if( _started ) this_t::end() ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _name = std::move( rhv._name ) ;
                _begin = std::move( rhv._begin ) ;
                _end = std::move( rhv._end ) ;
                _started = rhv._started ;

                if( _started ) this_t::end() ;

                return *this ;
            }

        private:

            this_ref_t begin( void_t ) noexcept
            {
                _started = true ;
                _begin = clock_t::now() ;
                return *this ;
            }

            this_ref_t end( void_t ) noexcept
            {
                _started = false ;
                _end = clock_t::now() ;
                return *this ;
            }

        public:

            natus::ntd::string_cref_t get_name( void_t ) const noexcept 
            {
                return _name ;
            }

            this_t::clock_t::time_point get_begin( void_t ) const noexcept
            {
                return _begin ;
            }

            this_t::clock_t::time_point get_end( void_t ) const noexcept
            {
                return _end ;
            }

            template< typename D >
            D get_duration( void_t ) const noexcept
            {
                return std::chrono::duration_cast< D >( _end - _begin ) ;
            }
        };
        natus_typedef( entry ) ;
    }
}