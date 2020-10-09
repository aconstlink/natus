
#pragma once

#include "../typedefs.h"
#include "../object.hpp"
#include "../enums.h"

#include <natus/ntd/vector.hpp>
#include <algorithm>

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
            samples_t _samples ;

        public:

            capture_object( void_t )
            {}

            capture_object( natus::audio::capture_type const ct, natus::audio::channels const ch, 
                natus::audio::frequency const f = natus::audio::frequency::freq_96k ) 
            {
                _ct = ct ;
                _ch = ch ;
                _freq = f ;
            }

            capture_object( this_cref_t ) = delete ;
            capture_object( this_rref_t rhv ) 
            {
                _ct = rhv._ct ;
                _ch = rhv._ch ;
                _freq = rhv._freq ;
                _samples = std::move( rhv._samples ) ;
            }

            virtual ~capture_object( void_t )
            {
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

            samples_cref_t get_samples( void_t ) const noexcept
            {
                return _samples ;
            }

        public:

            size_t size( void_t ) const noexcept
            {
                return _samples.size() ;
            }

            void_t resize( size_t const n ) noexcept
            {
                _samples.resize( n ) ;
            }

            void_t resize_by( size_t const n ) noexcept
            {
                _samples.resize( _samples.size() + n ) ;
            }

            float_t operator[] ( size_t const i ) const noexcept
            {
                return _samples[ std::min( _samples.size()-1, i ) ] ;
            }

            float_ref_t operator[] ( size_t const i ) noexcept
            {
                return _samples[ std::min( _samples.size() - 1, i ) ] ;
            }

            typedef std::function< void_t ( size_t const, float_t const ) > for_each_sample_read_t ;
            void_t for_each( for_each_sample_read_t funk ) const noexcept
            {
                for( size_t i=0; i<_samples.size()-1; ++i )
                {
                    funk( i, _samples[ i ] ) ;
                }
            }

            typedef std::function< void_t ( size_t const, float_ref_t ) > for_each_sample_write_t ;
            void_t for_each( for_each_sample_write_t funk ) noexcept
            {
                for( size_t i = 0; i < _samples.size(); ++i )
                {
                    funk( i, _samples[ i ] ) ;
                }
            }
        };
        natus_res_typedef( capture_object ) ;
    }
}