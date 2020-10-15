#pragma once

#include "typedefs.h"

#include <natus/ntd/vector.hpp>
#include <algorithm>

namespace natus
{
    namespace audio
    {
        class buffer
        {
            natus_this_typedefs( buffer ) ;

        private:

            natus::ntd::vector< float_t > _buffer ;

        public:

            buffer( void_t ) 
            {}

            buffer( size_t const s )
            {
                this_t::resize( s ) ;
            }

            buffer( this_cref_t rhv )
            {
                _buffer = rhv._buffer ;
            }

            buffer( this_rref_t rhv )
            {
                _buffer = std::move( rhv._buffer ) ;
            }

            ~buffer( void_t ) 
            {}

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _buffer = rhv._buffer ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _buffer = std::move( rhv._buffer ) ;
                return *this ;
            }

        public:

            size_t size( void_t ) const noexcept
            {
                return _buffer.size() ;
            }

            this_ref_t resize( size_t const s ) noexcept
            {
                _buffer.resize( s ) ;
                return *this ;
            }

            float_cptr_t data( void_t ) const noexcept
            {
                return _buffer.data() ;
            }

            this_ref_t append( natus::ntd::vector< float_t > const & values_ ) noexcept
            {
                size_t const n = values_.size() ;
                float_cptr_t values = values_.data() ;

                // shift and copy
                {
                    size_t const nn = std::min( _buffer.size(), n ) ;

                    size_t const n0 = _buffer.size() - nn ;
                    size_t const n1 = n - nn ;

                    float_ptr_t samples = _buffer.data() ;

                    // shift by n values
                    {
                        //std::memcpy( samples, samples + nn, n0 * sizeof( float_t ) ) ;
                        for( size_t i = 0; i < n0; ++i ) samples[ i ] = samples[ nn + i ] ;
                    }
                    // copy the new values
                    {
                        //std::memcpy( samples + n0, values + n1, nn * sizeof( float_t ) ) ;
                        for( size_t i = 0; i < nn; ++i ) samples[ n0 + i ] = values[ n1 + i ] ;
                    }
                }

                return *this ;
            }
        };
        natus_typedef( buffer ) ;
    }
}