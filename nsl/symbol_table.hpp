#pragma once

#include "typedefs.h"
#include <natus/ntd/vector.hpp>
#include <natus/ntd/string/split.hpp>
#include <natus/concurrent/mrsw.hpp>

namespace natus
{
    namespace nsl
    {
        class symbol_table
        {
            natus_this_typedefs( symbol_table ) ;

        public:

            class symbol
            {
                natus_this_typedefs( symbol ) ;

            private:

                // the namespace like liba.libb.libc
                natus::ntd::vector< natus::ntd::string_t > _spaces ;

                // the symbols' name like xyz
                natus::ntd::string_t _name ;

                // shader version
                natus::ntd::string_t _version ;

            public:
                
                symbol( void_t ) {}
                symbol( this_cref_t rhv ) 
                {
                    _spaces = rhv._spaces ;
                    _name = rhv._name ;
                    _version = rhv._version ;
                }

                symbol( this_rref_t rhv ) 
                {
                    _spaces = std::move( rhv._spaces ) ;
                    _name = std::move( rhv._name ) ;
                    _version = std::move( rhv._version ) ;
                }

                // @param name something like liba.libb.libc.name
                // @param version something like nsl, gl3, es3, hlsl9...
                symbol( natus::ntd::string_cref_t name, natus::ntd::string_cref_t version ) noexcept
                {
                    size_t off = 0 ;
                    size_t n = name.find_first_of( '.' ) ;
                    while( n != std::string::npos )
                    {
                        _spaces.emplace_back( name.substr( off, n - off ) ) ;
                        off += n + 1 ;
                        n = name.find_first_of( '.' ) ;
                    }
                }

                this_ref_t operator = ( this_cref_t ) noexcept
                {
                    return *this ;
                }

                this_ref_t operator = ( this_rref_t ) noexcept
                {
                    return *this ;
                }

            public:

                bool_t operator == ( this_cref_t s ) const noexcept
                {
                    return s.full_symbol_name() == this_t::full_symbol_name() ;
                }

                bool_t operator == ( natus::ntd::string_cref_t s ) const noexcept
                {
                    return s == this_t::namespace_name() ;
                }

            public:

                // only returns the namespace name in dot notation like
                // liba.libb.libc
                natus::ntd::string_t namespace_name( void_t ) const noexcept
                {
                    natus::ntd::string_t ret ;
                    for( auto const & s : _spaces )
                    {
                        ret += s + "." ;
                    }
                    ret = ret.substr( 0, ret.size() - 1 ) ;

                    return std::move( ret ) ;
                }

                // returns the full qualified name like
                // liba.libb.libc.name
                natus::ntd::string_t symbol_name( void_t ) const noexcept
                {
                    natus::ntd::string_t ret ;
                    ret = this_t::namespace_name() + "." + _name ;
                    return std::move( ret ) ;
                }

                // returns the full qualified name like
                // liba.libb.libc.name
                natus::ntd::string_t full_symbol_name( void_t ) const noexcept
                {
                    return this_t::symbol_name() + "." + _version ;
                }
            };
            natus_typedef( symbol ) ;

            typedef natus::ntd::vector< symbol_t > symbols_t ;
            symbols_t _syms ;

            mutable natus::concurrent::mrsw_t _ac ;

        public:

            symbol_table( void_t ) noexcept
            {}

            symbol_table( this_cref_t ) noexcept
            {}

            symbol_table( this_rref_t ) noexcept
            {}

            ~symbol_table( void_t ) noexcept
            {}

            this_ref_t operator = ( this_cref_t ) noexcept
            {
                return *this ;
            }

            this_ref_t operator = ( this_rref_t ) noexcept
            {
                return *this ;
            }

        public:

            this_ref_t insert( this_t::symbol_cref_t s ) noexcept
            {
                natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;

                auto iter = std::find_if( _syms.begin(), _syms.end(), [&] ( this_t::symbol_cref_t ls ) 
                { 
                    return ls == s ;
                } ) ;

                if( iter == _syms.end() )
                {
                    _syms.emplace_back( s ) ;
                }

                return *this ;
            }

            bool_t find( natus::ntd::string_cref_t, this_t::symbol_out_t ) noexcept
            {
                natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;

                return false;
            }

            typedef std::function< bool_t ( this_t::symbol_cref_t ) > find_funk_t ;
            bool_t find_if( find_funk_t funk ) noexcept
            {
                natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;

                for( auto const & s : _syms )
                {
                    if( funk( s ) ) return true ;
                }

                return false;
            }
        };
        natus_typedef( symbol_table ) ;
    }
}