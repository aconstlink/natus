
#pragma once

#include "typedefs.h"
#include <natus/memory/guards/malloc_guard.hpp>

namespace natus
{
    namespace io
    {
        // carrier of the result of the en/decryption functions
        class obfuscated
        {
            natus_this_typedefs( obfuscated ) ;

        private:

            natus::memory::malloc_guard< char_t > _mem ;

        public:

            obfuscated( natus::memory::malloc_guard< char_t > && mem )
            {
                _mem = std::move( mem ) ;
            }

            obfuscated( natus::ntd::string_cref_t s )
            {
                _mem = natus::memory::malloc_guard< char_t >( s.c_str(), s.size() ) ;
            }

            obfuscated( char_cptr_t ptr, size_t const sib )
            {
                _mem = natus::memory::malloc_guard< char_t >( ptr, sib ) ;
            }

            obfuscated( char_ptr_t ptr, size_t const sib )
            {
                _mem = natus::memory::malloc_guard< char_t >( ptr, sib ) ;
            }

            obfuscated( this_cref_t ) = delete ;
            obfuscated( this_rref_t rhv ) 
            {
                _mem = std::move( rhv._mem ) ;
            }

            ~obfuscated( void_t ) {}

        public:

            size_t sib( void_t ) const noexcept { return _mem.size() ; }
            char_cptr_t ptr( void_t ) const noexcept { return _mem.get() ; }
            char_ptr_t move_ptr( void_t ) noexcept { return _mem.move_ptr() ;  }

        public:

            operator natus::ntd::string_t( void_t ) noexcept
            {
                return natus::ntd::string_t( this_t::ptr(), this_t::sib() ) ;
            }
                
        };
        natus_typedef( obfuscated ) ;

        // carrier of the en-decryption functions
        class obfuscator
        {
            natus_this_typedefs( obfuscator ) ;

        public:

            typedef std::function< natus::memory::malloc_guard< char_t > ( natus::memory::malloc_guard< char_t > && ) > encode_funk_t ;
            typedef std::function< natus::memory::malloc_guard< char_t > ( natus::memory::malloc_guard< char_t > && ) > decode_funk_t ;

        private:

            encode_funk_t _efunk ;
            decode_funk_t _dfunk ;

        public:

            obfuscator( void_t ) 
            {
                _efunk = [] ( natus::memory::malloc_guard< char_t >&& mem ) { return std::move( mem ); } ;
                _dfunk = [] ( natus::memory::malloc_guard< char_t >&& mem ) { return std::move( mem ); } ;
            }

            obfuscator( this_t::encode_funk_t ef, this_t::decode_funk_t df )
            {
                _efunk = ef ;
                _dfunk = df ;
            }

            obfuscator( this_cref_t rhv ) 
            {
                _efunk = rhv._efunk ;
                _dfunk = rhv._dfunk ;
            }

            obfuscator( this_rref_t rhv )
            {
                _efunk = std::move( rhv._efunk ) ;
                _dfunk = std::move( rhv._dfunk ) ;
            }
            ~obfuscator( void_t ) {}

        public:

            natus::io::obfuscated_t encode( natus::ntd::string_cref_t s ) noexcept
            {
                return this_t::encode( s.c_str(), s.size() ) ;
            }

            // const version does copy
            natus::io::obfuscated_t encode( char_cptr_t ptr, size_t const sib ) noexcept
            {
                return natus::io::obfuscated_t( _efunk( natus::memory::malloc_guard< char_t >( ptr, sib ) ) ) ;
            }

            // non const version does inplace
            natus::io::obfuscated_t encode( char_ptr_t ptr, size_t const sib ) noexcept
            {
                return natus::io::obfuscated_t( _efunk( natus::memory::malloc_guard< char_t >( ptr, sib ) ) ) ;
            }

            natus::io::obfuscated_t decode( natus::ntd::string_cref_t s ) noexcept
            {
                return this_t::decode( s.c_str(), s.size() ) ;
            }

            natus::io::obfuscated_t decode( char_cptr_t ptr, size_t const sib ) noexcept
            {
                return natus::io::obfuscated_t( _dfunk( natus::memory::malloc_guard< char_t >( ptr, sib ) ) ) ;
            }

            natus::io::obfuscated_t decode( char_ptr_t ptr, size_t const sib ) noexcept
            {
                return natus::io::obfuscated_t( _dfunk( natus::memory::malloc_guard< char_t >( ptr, sib ) ) ) ;
            }

        private:

            this_ref_t endecode( this_t::encode_funk_t ef, this_t::decode_funk_t df ) noexcept
            {
                _efunk = ef ;
                _dfunk = df ;
                return *this ;
            }

        public:

            static this_t as_user_funk( this_rref_t other, this_t::encode_funk_t ef, this_t::decode_funk_t df )
            {
                return std::move( std::move( other ).endecode( ef, df ) ) ;
            }

            // null endecode
            static this_t variant_0( this_rref_t other = this_t() ) noexcept
            {
                return this_t::as_user_funk( std::move( other ),
                    [] ( natus::memory::malloc_guard< char_t >&& mem ) { return std::move( mem ); },
                    [] ( natus::memory::malloc_guard< char_t >&& mem ) { return std::move( mem ); } ) ;
            }

            // variant 1 endecode with some xoring.
            static this_t variant_1( this_rref_t other = this_t() ) noexcept
            {
                this_t::encode_funk_t ef = [] ( natus::memory::malloc_guard< char_t > && mem )
                {
                    static const char_t lut[ 8 ] = { 'a', 'f', 'c', 'd', '5', 'c', '\n', '5' } ;

                    for( size_t i = 0; i < mem.size(); ++i )
                    {
                        mem[ i ] = mem[ i ] ^ lut[ i % 8 ] ;
                    }

                    return std::move( mem );
                } ;

                this_t::decode_funk_t df = [] ( natus::memory::malloc_guard< char_t > && mem )
                {
                    static const char_t lut[ 8 ] = { 'a', 'f', 'c', 'd', '5', 'c', '\n', '5' } ;

                    for( size_t i = 0; i < mem.size(); ++i )
                    {
                        mem[ i ] = mem[ i ] ^ lut[ i % 8 ] ;
                    }

                    return std::move( mem );
                } ;

                return this_t::as_user_funk( std::move( other ), ef, df ) ;
            }
        };
        natus_typedef( obfuscator ) ;
    }
}