#pragma once

#include "ibuffer.hpp"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace audio
    {
        template< typename T, size_t ch >
        class pcm_buffer : public ibuffer
        {
            typedef pcm_buffer< T, ch > __this_t ;
            natus_this_typedefs( __this_t ) ;

        public:

            template< size_t ch >
            struct sample
            {
                T channels[ ch ] ;

                T operator[] ( size_t const c ) const noexcept
                {
                    return channels[ c ] ;
                }

                T & operator[] ( size_t const c ) noexcept
                {
                    return channels[ c ] ;
                }
            };

            template<>
            struct sample<2>
            {
                natus_this_typedefs( sample<2> ) ;
                T channels[ 2 ] ;

                T left( void_t ) const noexcept { return channels[ 0 ] ; }
                T right( void_t ) const noexcept { return channels[ 1 ] ; }

                this_ref_t left( T const& d ) noexcept { channels[ 0 ] = d ; return *this ; }
                this_ref_t right( T const& d ) noexcept { channels[ 1 ] = d ; return *this ; }
            };
            natus_typedefs( sample< ch >, sample ) ;

        private:

            // sib = ch * _samples * sizeof(T)
            natus::ntd::vector< sample_t > _samples ;

        public:

            pcm_buffer( void_t ) {}
            pcm_buffer( this_cref_t rhv ) 
            {
                _samples = rhv._samples ;
            }
            pcm_buffer( this_rref_t rhv )
            {
                _samples = std::move( rhv._samples ) ;
            }
            virtual ~pcm_buffer( void_t ) {}

            sample_t operator []( size_t const i ) const noexcept
            {
                return _samples[ i ] ;
            }

            sample_ref_t operator []( size_t const i ) noexcept
            {
                return _samples[ i ] ;
            }

            this_ref_t push_back( sample_cref_t smp ) noexcept
            {
                _samples.emplace_back( smp ) ;
                return *this ;
            }
        } ;
        typedef pcm_buffer< uint8_t, 2 > stereo_uint8_pcm_t ;
        typedef pcm_buffer< uint16_t, 2 > stereo_uint16_pcm_t ;
    }
}