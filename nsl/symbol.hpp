#pragma once

#include "typedefs.h"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace nsl
    {
        class symbol
        {
            natus_this_typedefs( symbol ) ;

            natus::ntd::vector< natus::ntd::string_t > _spaces ;
            natus::ntd::string_t _sym ;

        public:

            symbol( natus::ntd::string_cref_t s ) noexcept
            {
                this_t::tokenize( s ) ;
                this_t::construct() ;
            }

        public:

            bool_t operator == ( this_cref_t rhv ) const noexcept
            {
                return rhv._sym == _sym ;
            }

            bool_t operator == ( natus::ntd::string_cref_t rhv ) const noexcept
            {
                return rhv == _sym ;
            }

        public:

            natus::ntd::string_t sym_long( void_t ) const noexcept
            {
                return _sym ;
            }

        private:

            void_t tokenize( natus::ntd::string_cref_t s ) noexcept
            {
                size_t p0 = 0 ;
                size_t p1 = s.find_first_of( '.' ) ;
                while( p1 != std::string::npos )
                {
                    _spaces.emplace_back( s.substr( p0, p1 - p0 ) ) ;

                    p0 = p1 + 1 ;
                    p1 = s.find_first_of( '.', p0 ) ;
                }
            }

            void_t construct( void_t ) 
            {
                for( auto const & s : _spaces )
                {
                    _sym += s + "." ;
                }
                _sym = _sym.substr( 0, _sym.size() - 1 ) ;
            }
        };
        natus_typedef( symbol ) ;
    }
}