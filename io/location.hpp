#pragma once

#include "typedefs.h"

namespace natus
{
    namespace io
    {
        class location
        {
            natus_this_typedefs( location ) ;

        private:

            natus::ntd::string_t _loc ;

        public:

            location( void_t ) noexcept
            {
            }

            location( natus::ntd::string_cref_t loc ) noexcept
            {
                _loc = loc ;
            }

            location( this_cref_t rhv ) noexcept
            {
                _loc = rhv._loc ;
            }

            location( this_rref_t rhv ) noexcept
            {
                _loc = std::move( rhv._loc ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _loc = rhv._loc ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept 
            {
                _loc = std::move( rhv._loc ) ;
                return *this ;
            }

            bool_t operator == ( this_cref_t other ) const noexcept
            {
                return _loc == other._loc ;
            }

            operator natus::ntd::string_t( void_t ) noexcept
            {
                return _loc ;
            }

            operator natus::io::path_t( void_t ) noexcept
            {
                return this_t::as_path() ;
            }
            
            natus::io::path_t as_path( void_t ) const noexcept
            {
                natus::io::path_t pout = "." ;
                
                size_t off = _loc.find_first_of( '.', 0 ) ;
                size_t last = size_t(0) ;
                while( off != std::string::npos )
                {
                    pout /= _loc.substr( last, off - last ) ;

                    last = off +1 ;
                    off = _loc.find_first_of( '.', last ) ;
                }
                pout += "." + _loc.substr( last, _loc.size() - 1 ) ;

                return std::move( pout ) ;
            }

            natus::ntd::string_cref_t as_string( void_t ) const noexcept
            {
                return _loc ;
            }

            natus::ntd::string_t extension( bool_t const dotted = true ) const noexcept
            {
                natus::ntd::string_t ext = this_t::as_path().extension() ;
                if( !dotted )
                {
                    ext = ext.substr( 1, ext.size() - 1 ) ;
                }
                return ext ;
            }

            static this_t from_path( natus::io::path_cref_t p ) noexcept
            {
                natus::ntd::string_t loc ;

                for( auto p_ : p.lexically_normal() )
                {
                    loc += p_.string() + "." ;
                }
                loc = loc.substr( 0, loc.size() - 1 ) ;

                return this_t( loc ) ;
            }
        };
        natus_typedef( location ) ;
    }
}