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

            natus_typedefs( natus::ntd::vector< float_t >, floats ) ;
            floats_t _buffer ;

            // the frequency per second. how many samples are taken per second per channel.
            size_t _sample_rate = 0 ;
            size_t _channels = 0 ;

        public:

            buffer( void_t ) 
            {}

            buffer( size_t const s )
            {
                this_t::resize( s ) ;
            }

            buffer( this_cref_t rhv )
            {
                _sample_rate = rhv._sample_rate ;
                _channels = rhv._channels ;
                _buffer = rhv._buffer ;
            }

            buffer( this_rref_t rhv )
            {
                _sample_rate = rhv._sample_rate ;
                _channels = rhv._channels ;
                _buffer = std::move( rhv._buffer ) ;
            }

            ~buffer( void_t ) 
            {}

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _sample_rate = rhv._sample_rate ;
                _channels = rhv._channels ;
                _buffer = rhv._buffer ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _sample_rate = rhv._sample_rate ;
                _channels = rhv._channels ;
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
                        std::memcpy( samples, samples + nn, n0 * sizeof( float_t ) ) ;
                        //for( size_t i = 0; i < n0; ++i ) samples[ i ] = samples[ nn + i ] ;
                    }
                    // copy the new values
                    {
                        std::memcpy( samples + n0, values + n1, nn * sizeof( float_t ) ) ;
                        //for( size_t i = 0; i < nn; ++i ) samples[ n0 + i ] = values[ n1 + i ] ;
                    }
                }

                return *this ;
            }

            this_cref_t copy_buffer( floats_out_t outs ) const noexcept
            {
                outs = _buffer ;
                return *this ;
            }

            // set the samples and the sample rate per channel
            // if 44.1 kHz is set, this is per channel.
            void_t set_samples( natus::audio::channels const channels, size_t const sample_rate, this_t::floats_cref_t buffer ) noexcept
            {
                _channels = natus::audio::to_number( channels ) ;
                _sample_rate = sample_rate ;
                _buffer = buffer ;
            }

        public:

            natus::audio::channels get_channels( void_t ) const noexcept
            {
                return natus::audio::to_enum( _channels ) ;
            }

            size_t get_sample_rate( void_t ) const noexcept
            {
                return _sample_rate ;
            }

            this_t::floats_cref_t get_samples( void_t ) const noexcept
            {
                return _buffer ;
            }
        };
        natus_res_typedef( buffer ) ;
    }
}