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
            natus_typedefs( natus::ntd::vector< this_t >, symbols ) ;

            natus::ntd::vector< natus::ntd::string_t > _spaces ;
            natus::ntd::string_t _sym ;

        public:

            symbol( void_t ){}

            symbol( natus::ntd::string_cref_t s ) noexcept
            {
                *this = this_t::construct( this_t::tokenize( s ) ) ;
            }

            symbol( this_cref_t rhv ) noexcept
            {
                *this = rhv ;
            }

            symbol( this_rref_t rhv ) noexcept
            {
                *this = std::move( rhv ) ;
            }

        public:

            this_ref_t operator = ( this_rref_t rhv ) noexcept 
            {
                _spaces = std::move( rhv._spaces ) ;
                _sym = std::move( rhv._sym ) ;

                return *this ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _spaces = rhv._spaces ;
                _sym = rhv._sym ;

                return *this ;
            }

            bool_t operator == ( this_cref_t rhv ) const noexcept
            {
                return rhv._sym == _sym ;
            }

            bool_t operator == ( natus::ntd::string_cref_t rhv ) const noexcept
            {
                return rhv == _sym ;
            }

            this_ref_t operator = ( natus::ntd::string_cref_t s ) noexcept
            {
                return ( *this = this_t::construct( this_t::tokenize( s ) ) ) ;
            }

            this_ref_t operator + ( this_cref_t rhv ) const noexcept
            {
                return this_t( *this ).construct( this_t::tokenize( _sym + "." + rhv._sym ) ) ;
            }

            this_ref_t operator += ( this_rref_t rhv ) noexcept
            {
                return *this = this_t::construct( this_t::tokenize( _sym + "." + rhv._sym ) ) ;
            }

            this_ref_t operator + ( natus::ntd::string_cref_t rhv ) const noexcept
            {
                return this_t( *this ) += rhv ;
            }

            this_ref_t operator += ( natus::ntd::string_cref_t rhv ) noexcept
            {
                if( rhv.empty() ) return *this ;

                return *this = _sym + "." + rhv ;
            }

        public:

            natus::ntd::string_t sym_long( void_t ) const noexcept
            {
                return _sym ;
            }
            
        private:

            natus::ntd::vector< natus::ntd::string_t > tokenize( natus::ntd::string_cref_t s ) const noexcept
            {
                natus::ntd::vector< natus::ntd::string_t > ret ;

                size_t p0 = 0 ;
                size_t p1 = s.find_first_of( '.' ) ;
                while( p1 != std::string::npos )
                {
                    ret.emplace_back( s.substr( p0, p1 - p0 ) ) ;

                    p0 = p1 + 1 ;
                    p1 = s.find_first_of( '.', p0 ) ;
                }
                ret.emplace_back( s.substr( p0 ) ) ;

                return std::move( ret ) ;
            }

            this_t construct( natus::ntd::vector< natus::ntd::string_t > && spaces ) const noexcept
            {
                
                natus::ntd::string_t sym ;

                for( auto const & s : spaces )
                {
                    sym += s + "." ;
                }

                this_t ret ;
                ret._sym = sym.substr( 0, sym.size() - 1 ) ;
                ret._spaces = spaces ;
                

                return std::move( ret ) ;
            }

        public:

            static this_t::symbols_t find_all_symbols( natus::ntd::string_cref_t start, natus::ntd::string_cref_t s ) noexcept
            {
                this_t::symbols_t ret ;

                size_t p0 = 0 ;
                while( true )
                {
                    p0 = s.find( start, p0 ) ;
                    if( p0 == std::string::npos ) break ;

                    size_t const p1 = s.find_first_of( ' ', p0 ) ;
                    if( p1 == std::string::npos ) break ;

                    ret.emplace_back( s.substr( p0, p1 - p0 ) ) ;
                    p0 = p1 ;
                }

                return std::move( ret ) ;
            }

            static symbols_t merge( symbols_rref_t s1, symbols_rref_t s2 ) noexcept
            {
                s1.reserve( s1.size() + s2.size() ) ;
                for( auto const& s : s2 ) s1.emplace_back( s ) ;
                return s1 ;
            }
        };
        natus_typedef( symbol ) ;
        natus_typedefs( natus::ntd::vector< symbol_t >, symbols ) ;
    }
}