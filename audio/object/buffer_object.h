#pragma once

#include "../typedefs.h"
#include "../object.hpp"
#include "../enums.h"
#include "../buffer.hpp"

#include <natus/ntd/vector.hpp>
#include <natus/math/vector/vector2.hpp>

#include <algorithm>
#include <limits>

namespace natus
{
    namespace audio
    {
        class buffer_object : public object
        {
            natus_this_typedefs( buffer_object ) ;
            
        private:
            
            // the frequency per second. how many samples are taken per second per channel.
            size_t _sample_rate = 0 ;
            size_t _channels = 0 ;

            natus_typedefs( natus::ntd::vector< float_t >, floats ) ;
            floats_t _samples ;

            natus::ntd::string_t _name ;

            // set by the run-time
            execution_state _es ;
            
        public:

            buffer_object( void_t ) {
            }
            buffer_object( natus::ntd::string_cref_t name ) : _name( name ) {}
            buffer_object( this_cref_t ) = delete ;
            buffer_object( this_rref_t rhv ) : object( std::move( rhv ) )
            {
                _samples = std::move( rhv._samples ) ;
                _name = std::move( rhv._name ) ;
                _channels = rhv._channels;
                _sample_rate = rhv._sample_rate ;
            }

            virtual ~buffer_object( void_t ) {}

        public:

            this_ref_t operator = ( natus::audio::buffer_cref_t b ) 
            {
                this_t::set_samples( b.get_channels(), b.get_sample_rate(), b.get_samples() ) ;
                return *this ;
            }

        public:

            natus::ntd::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }

            natus::ntd::string_cref_t get_name( void_t ) const noexcept
            {
                return _name ;
            }

            // set the samples and the sample rate per channel
            // if 44.1 kHz is set, this is per channel.
            void_t set_samples( natus::audio::channels const channels, size_t const sample_rate, this_t::floats_cref_t buffer ) noexcept
            {
                _channels = natus::audio::to_number( channels ) ;
                _sample_rate = sample_rate ;
                _samples = buffer ;
            }

            void_t set_samples( natus::audio::channels const channels, size_t const sample_rate, natus::audio::buffer_cref_t buffer ) noexcept
            {
                _channels = natus::audio::to_number( channels ) ;
                _sample_rate = sample_rate ;
                buffer.copy_buffer( _samples ) ;
            }

            size_t get_sample_rate( void_t ) const noexcept
            {
                return _sample_rate ;
            }

            floats_cref_t get_samples( void_t ) const noexcept
            {
                return _samples ;
            }

            size_t get_sib( void_t ) const noexcept 
            {
                return _samples.size() * sizeof( float_t ) ;
            }

            size_t get_num_channels( void_t ) const noexcept
            {
                return _channels ;
            }

        public:

            void_t set_execution_state( natus::audio::execution_state const es ) noexcept
            {
                _es = es ;
            }

            natus::audio::execution_state get_state( void_t ) const noexcept
            {
                return _es ;
            }

        } ;
        natus_res_typedef( buffer_object ) ;
    }
}