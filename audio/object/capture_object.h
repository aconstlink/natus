
#pragma once

#include "../typedefs.h"
#include "../object.hpp"
#include "../enums.h"

#include <natus/ntd/vector.hpp>
#include <natus/math/vector/vector2.hpp>

#include <algorithm>
#include <limits>

namespace natus
{
    namespace audio
    {
        enum class capture_type
        {
            undefined,
            what_u_hear,
            microphone
        };

        class capture_object : public object
        {
            natus_this_typedefs( capture_object ) ;

        private:

            natus::audio::capture_type _ct = natus::audio::capture_type::undefined ;
            natus::audio::channels _ch = natus::audio::channels::undefined ;
            natus::audio::frequency _freq = natus::audio::frequency::undefined ;

            natus_typedefs( natus::ntd::vector< float_t >, samples ) ;
            float_t* _samples = nullptr ;

            size_t _size = 0 ;

            natus::math::vec2f_t _mm = natus::math::vec2f_t(
                std::numeric_limits<float_t>::max(),
                std::numeric_limits<float_t>::min() ) ;

        public:

            capture_object( void_t )
            {}

            capture_object( natus::audio::capture_type const ct, natus::audio::channels const ch,
                natus::audio::frequency const f = natus::audio::frequency::freq_96k )
            {
                _ct = ct ;
                _ch = ch ;
                _freq = f ;

                _size = 1 << 14 ;
                _samples = natus::memory::global_t::alloc_raw<float_t>( _size ) ;
            }

            capture_object( this_cref_t ) = delete ;
            capture_object( this_rref_t rhv )
            {
                _ct = rhv._ct ;
                _ch = rhv._ch ;
                _freq = rhv._freq ;
                natus_move_member_ptr( _samples, rhv ) ;
                _size = rhv._size ;
            }

            virtual ~capture_object( void_t )
            {
                natus::memory::global_t::dealloc_raw( _samples ) ;
            }

        public:

            natus::audio::capture_type get_capture_type( void_t ) const noexcept
            {
                return _ct ;
            }

            natus::audio::channels get_channels( void_t ) const noexcept
            {
                return _ch ;
            }

            natus::audio::frequency get_frequency( void_t ) const noexcept
            {
                return _freq ;
            }

            /*samples_cref_t get_samples( void_t ) const noexcept
            {
                return _samples ;
            }*/

        public:

            natus::math::vec2f_t minmax( void_t ) const noexcept
            {
                return _mm ;
            }

            size_t size( void_t ) const noexcept
            {
                return _size ;
            }

            void_t resize( size_t const n ) noexcept
            {
                if( _samples != nullptr ) natus::memory::global_t::dealloc_raw( _samples ) ;
                _samples = natus::memory::global_t::alloc_raw<float_t>( n ) ;
            }

            void_t resize_by( size_t const n ) noexcept
            {
                this_t::resize( _size + n ) ;
            }

            float_t operator[] ( size_t const i ) const noexcept
            {
                return _samples[ std::min( _size - 1, i ) ] ;
            }

            float_ref_t operator[] ( size_t const i ) noexcept
            {
                return _samples[ std::min( _size - 1, i ) ] ;
            }

            typedef std::function< void_t ( size_t const, float_t const ) > for_each_sample_read_t ;
            void_t for_each( for_each_sample_read_t funk ) const noexcept
            {
                for( size_t i = 0; i < _size; ++i )
                {
                    funk( i, _samples[ i ] ) ;
                }
            }

            typedef std::function< void_t ( size_t const, float_ref_t ) > for_each_sample_write_t ;
            void_t for_each( for_each_sample_write_t funk, size_t const offset = 0 ) noexcept
            {
                for( size_t i = offset; i < _size; ++i )
                {
                    funk( i, _samples[ i ] ) ;
                }
            }

            // quite complex function
            // - does a lot of copying
            // - computes the new min/max values
            // - computes the new frequencies via the fft
            this_ref_t shift_and_copy_from( size_t const n, float_cptr_t values ) noexcept
            {
                if( n == 0 ) return *this ;

                // shift and copy
                {
                    size_t const nn = std::min( _size, n ) ;

                    size_t const n0 = _size - nn ;
                    size_t const n1 = n - nn ;

                    // shift by n values
                    std::memcpy( _samples, _samples + nn, n0 * sizeof( float_t ) ) ;
                    // copy the new values
                    std::memcpy( _samples + n0, values + n1, nn * sizeof( float_t ) ) ;
                }

                // calc new min/max
                {
                    natus::math::vec2f_t mm = natus::math::vec2f_t(
                        std::numeric_limits<float_t>::max(),
                        std::numeric_limits<float_t>::min() ) ;

                    for( size_t i = 0; i < _size; ++i )
                    {
                        mm = natus::math::vec2f_t( std::min( mm.x(), _samples[ i ] ), std::max( mm.y(), _samples[ i ] ) ) ;
                    }

                    _mm = mm ;
                }

                // compute the frequencies
                {
                }

                return  *this ;
            }

            void_t copy_samples_to( natus::ntd::vector< float_t > & smps ) const noexcept
            {
                smps.resize( _size ) ;
                std::memcpy( smps.data(), _samples, _size * sizeof( float_t ) ) ;
            }
        };
        natus_res_typedef( capture_object ) ;
    }
}