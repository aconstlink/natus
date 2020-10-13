
#include "oal.h"

#include "../../id.hpp"
#include "../../enums.h"


#include <natus/ntd/vector.hpp>
#include <natus/math/dsp/fft.hpp>
#include <natus/math/vector/vector2.hpp>
#include <natus/math/interpolation/interpolate.hpp>

#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>

#include <limits>

using namespace natus::audio ;

namespace this_file
{
    struct global_capture
    {
        ALCdevice* dev = nullptr ;

        size_t num_channels = 0 ;
        size_t frame_size = 0 ;

        natus::ntd::vector< ALbyte > raw_bytes ;
        natus::ntd::vector< float_t > raw_samples ;

        natus::ntd::vector< float_t > samples ;
        natus::ntd::vector< float_t > frequencies ;

        typedef natus::math::dsp::fft<float_t> fft_t ;
        fft_t::complexes_t complex_frequencies ;

        natus::audio::frequency frequency ;
        natus::math::vec2f_t mm ;
    };
    natus_typedef( global_capture ) ;
}

struct natus::audio::oal_backend::pimpl
{
    natus_this_typedefs( pimpl ) ;

    size_t _captures = 0 ;
    size_t _do_captures = 0 ;

    this_file::global_capture_ptr_t _gc ;

    void_t create_global_what_u_hear_capture_object( void_t )
    {
        this_file::global_capture gc ;

        natus::ntd::string_t whatuhear ;

        // Get all capture devices
        {
            auto const* list = alcGetString( NULL, ALC_CAPTURE_DEVICE_SPECIFIER ) ;
            size_t sib = 0 ;
            while( true )
            {
                natus::ntd::string_t item = list + sib  ;

                if( item.size() == 0 ) break ;

                sib += item.size() + 1 ;

                if( item.find( "What U Hear" ) != std::string::npos )
                {
                    whatuhear = item ;
                    break ;
                }
            }

            if( whatuhear.empty() )
            {
                natus::log::global_t::error( "[OpenAL backend] : no \"What U Hear\" device available." ) ;
                return ;
            }
        }

        {
            natus::audio::frequency const frequency = natus::audio::frequency::freq_48k ;
            natus::audio::channels const channels = natus::audio::channels::mono ;

            {
                ALCenum fmt = 0 ;

                switch( channels )
                {
                case natus::audio::channels::mono: fmt = AL_FORMAT_MONO16 ;  break ;
                case natus::audio::channels::stereo: fmt = AL_FORMAT_STEREO16 ; break ;
                default: break ;
                }

                gc.dev = alcCaptureOpenDevice( whatuhear.c_str(),
                    ALCuint( natus::audio::to_number( frequency ) ), fmt, ALCuint( 1 << 15 ) ) ;
            }

            if( gc.dev == nullptr )
            {
                natus::log::global_t::error( "[OpenAL backend] : failed to open capture device." ) ;
                return ;
            }

            gc.frame_size = natus::audio::to_number( channels ) * 16 / 8 ;
            gc.num_channels = natus::audio::to_number( channels ) ;
            gc.frequency = frequency ;
        }

        {
            // frequency window
            size_t const size = 1 << 12 ;

            gc.samples.resize( size ) ;
            gc.frequencies.resize( size >> 1 ) ;
            std::memset( gc.frequencies.data(), 0, sizeof( float_t ) * size >> 1 ) ;
            gc.complex_frequencies.resize( size ) ;
        }

        _gc = natus::memory::global_t::alloc( this_file::global_capture_t( std::move( gc ) ),
            "[OpenAL Backend] : global_capture" ) ;
    }

    pimpl( void_t )
    {
        create_global_what_u_hear_capture_object() ;
    }

    ~pimpl( void_t )
    {
    }

    bool_t control_what_u_hear_capturing( bool_t const do_capture )
    {
        ALCdevice* dev = _gc->dev ;

        if( _do_captures == 0 && do_capture )
        {
            alcCaptureStart( dev ) ;
            ++_do_captures ;
        }
        else if( _do_captures == 1 && !do_capture )
        {
            alcCaptureStop( dev ) ;
            _do_captures = 0 ;
        }

        return _do_captures != 0 ;
    }

    void_t capture_what_u_hear_samples( void_t )
    {
        ALCdevice* dev = _gc->dev ;

        if( _do_captures == 0 ) return ;

        ALCint count = 0 ;
        alcGetIntegerv( dev, ALC_CAPTURE_SAMPLES, 1, &count ) ;

        if( count == 0 ) return ;

        natus::ntd::vector< ALbyte >& buffer = _gc->raw_bytes ;
        natus::ntd::vector< float_t >& fbuffer = _gc->raw_samples ;

        buffer.resize( size_t( count ) * _gc->frame_size ) ;
        fbuffer.resize( size_t( count ) ) ;

        alcCaptureSamples( dev, buffer.data(), count ) ;
        if( alcGetError( dev ) != AL_NO_ERROR )
        {
            natus::log::global_t::error( "[OpenAL] : capturing samples failed" ) ;
            return ;
        }

        //natus::log::global_t::status( "Count : " + std::to_string( count ) ) ;



        double_t const dfrequency = double_t( natus::audio::to_number( _gc->frequency ) ) ;

        for( size_t i = 0; i < count; ++i )
        {
            // the index into the buffer
            size_t const idx = i * _gc->frame_size ;

            // reconstruct the 16 bit value
            #if !NATUS_BIG_ENDIAN
            ushort_t const p1 = ushort_t( ( ushort_t( buffer[ idx + 1 ] ) & 255 ) << 8 )  ;
            ushort_t const p0 = ushort_t( ( ushort_t( buffer[ idx + 0 ] ) & 255 ) << 0 )  ;
            short_t const ivalue = short_t( p0 | p1 ) ;// &( ( 1 << 16 ) - 1 ) ;
            #else
            int_t const ivalue = int_t( buffer[ idx + 0 ] << 8 ) | int_t( buffer[ idx + 1 ] << 0 );
            #endif

            size_t const index = i ;
            fbuffer[ index ] = float_t( double_t( ivalue ) / dfrequency ) ;
            //fbuffer[ index ] = natus::math::fn<float_t>::smooth_step( fbuffer[ index ] * 0.5f + 0.5f ) * 2.0f - 1.0f ;
        }

        // test sine wave
        #if 0
        {
            static bool_t swap = true ;
            count = swap ? 100 : 50;
            //swap = !swap ;

            static float_t t0 = 0.0f ;
            t0 += 0.001f ;
            t0 = t0 >= 1.0f ? 0.0f : t0 ;

            //natus::log::global_t::status( "t0 : " + std::to_string( t0 ) ) ;

            float_t const t1 = 1.0f - std::abs( t0 * 2.0f - 1.0f ) ;

            static float_t freqz = 0.0f ;
            float_t freq0 = natus::math::interpolation<float_t>::linear( 1.0f, 100.0f, t1 ) ;
            fbuffer.resize( size_t( count ) ) ;

            #if 0 
            {
                for( size_t i = 0; i < fbuffer.size(); ++i )
                {
                    float_t const s = float_t( i ) / float_t( count - 1 ) ;
                    fbuffer[ i ] = 20.0f * std::sin( freq0 * s * 2.0f * natus::math::constants<float_t>::pi() )
                        ;// +10.0f * std::sin( 150.0f * s * 2.0f * natus::math::constants<float_t>::pi() );

                    // saw-tooth
                    //float_t const ss = 1.0f - std::abs( natus::math::fn<float_t>::mod( s*freq0, 1.0f ) * 2.0f - 1.0f ) ;
                    //fbuffer[ i ] = 10.0f * ss ;
                }
            }
            #endif
            #if 1
            {
                static size_t j = 0 ;
                static size_t num_samples = 10100 ;

                freq0 = 780.0f ;
                for( size_t i = 0; i < count; ++i )
                {
                    float_t freq = natus::math::interpolation<float_t>::linear( freqz, freq0, float_t( i ) / float_t( count - 1 ) ) ;
                    size_t const idx = ( j + i ) % num_samples ;
                    float_t const s = float_t( idx ) / float_t( num_samples - 1 ) ;
                    fbuffer[ i ] = 20.0f * std::sin( freq * s * 2.0f * natus::math::constants<float_t>::pi() )
                        ;// +10.0f * std::sin( 150.0f * s * 2.0f * natus::math::constants<float_t>::pi() );

                    // saw-tooth
                    //float_t const ss = 1.0f - std::abs( natus::math::fn<float_t>::mod( s*freq0, 1.0f ) * 2.0f - 1.0f ) ;
                    //fbuffer[ i ] = 10.0f * ss ;
                }
                j = ( j + count ) % num_samples ;
            }
            #endif

            freqz = freq0 ;
        }
        #endif

        // shift in new values and min/max
        {
            size_t const n = fbuffer.size() ;
            float_cptr_t values = fbuffer.data() ;

            // shift and copy
            {
                size_t const nn = std::min( _gc->samples.size(), n ) ;

                size_t const n0 = _gc->samples.size() - nn ;
                size_t const n1 = n - nn ;

                float_ptr_t samples = _gc->samples.data() ;

                // shift by n values
                {
                    //std::memcpy( samples, samples + nn, n0 * sizeof( float_t ) ) ;
                    for( size_t i = 0; i < n0; ++i ) samples[ i ] = samples[ nn + i ] ;
                }
                // copy the new values
                {
                    //std::memcpy( samples + n0, values + n1, nn * sizeof( float_t ) ) ;
                    for( size_t i = 0; i < nn; ++i ) samples[ n0 + i ] = fbuffer[ n1 + i ] ;
                }
            }

            // calc new min/max
            {
                natus::math::vec2f_t mm = natus::math::vec2f_t(
                    std::numeric_limits<float_t>::max(),
                    std::numeric_limits<float_t>::min() ) ;

                for( float_t const s : _gc->samples )
                {
                    mm = natus::math::vec2f_t( std::min( mm.x(), s ), std::max( mm.y(), s ) ) ;
                }

                _gc->mm = mm ;
            }
        }

        // compute the frequency bands using the fft
        {
            size_t const num_samples = _gc->samples.size() ;
            for( size_t i = 0; i < _gc->samples.size(); ++i )
            {
                _gc->complex_frequencies[ i ] = this_file::global_capture::fft_t::complex_t( _gc->samples[ i ], 0.0f ) ;
            }
            this_file::global_capture::fft_t::compute( _gc->complex_frequencies ) ;

            float_t const div = 2.0f / float_t( num_samples ) ;

            for( size_t i = 0; i < num_samples >> 1; ++i )
            {
                float_t const a = std::abs( _gc->complex_frequencies[ i ] ) ;

                _gc->frequencies[ i ] = a * div ;
                _gc->frequencies[ i ] *= _gc->frequencies[ i ] ;

                // for db calculation
                //_frequencies[ i ] = 10.0f * std::log10( _frequencies[ i ] ) ;
            }
            // the zero frequency should not receive the multiplier 2
            _gc->frequencies[ 0 ] /= 2.0f ;

            // band width
            {
                //float_t const sampling_rate = float_t( natus::audio::to_number( _gc->frequency ) ) ;
                //float_t const buffer_window = float_t( _gc->samples.size() ) ;
                //float_t const mult = float_t( sampling_rate ) / float_t( buffer_window ) ;
            }
        }

        //natus::log::global_t::status("Count : " + std::to_string(count) ) ;
    }

    size_t construct_capture_object( natus::audio::capture_object_ref_t cap )
    {
        return _captures++ ;
    }

    void_t capture_samples( natus::audio::capture_object_ref_t cap )
    {
        cap.copy_samples_from( _gc->samples ) ;
        cap.copy_frequencies_from( _gc->frequencies ) ;
        cap.set_minmax( _gc->mm ) ;
        cap.set_channels( natus::audio::channels::mono ) ;

        // band width
        {
            float_t const sampling_rate = float_t( natus::audio::to_number( _gc->frequency ) ) ;
            float_t const buffer_window = float_t( _gc->samples.size() ) ;
            float_t const band_width = float_t( sampling_rate ) / float_t( buffer_window ) ;

            cap.set_band_width( size_t( band_width ) ) ;
        }
    }
};

size_t oal_backend::create_backend_id( void_t ) noexcept
{
    static size_t const id = size_t( natus::audio::backend_type::openal ) ;
    return id ;
}

oal_backend::oal_backend( void_t ) noexcept : backend( natus::audio::backend_type::openal )
{
    this_t::set_bid( this_t::create_backend_id() ) ;
    _pimpl = natus::memory::global_t::alloc( pimpl(), "[natus::audio::oal_backend::pimpl]" ) ;
}

oal_backend::oal_backend( this_rref_t rhv ) noexcept : backend( std::move( rhv ) )
{
    natus_move_member_ptr( _pimpl, rhv ) ;
}

oal_backend::~oal_backend( void_t ) noexcept
{
    natus::memory::global_t::dealloc( _pimpl ) ;
}

oal_backend::this_ref_t oal_backend::operator = ( this_rref_t rhv ) noexcept
{
    natus_move_member_ptr( _pimpl, rhv ) ;
    return *this ;
}

natus::audio::result oal_backend::configure( natus::audio::capture_type const ct,
    natus::audio::capture_object_res_t cap ) noexcept
{
    if( ct != natus::audio::capture_type::what_u_hear )
    {
        natus::log::global_t::warning( "[OpenAL Backend] : No other than a what u hear device supported." ) ;
        return natus::audio::result::invalid_argument;
    }

    natus::audio::id_res_t id = cap->get_id() ;

    if( id->get_bid() == size_t( -1 ) )
    {
        id = natus::audio::id_t( this_t::get_bid(), _pimpl->construct_capture_object( *cap ) ) ;
    }

    return id->get_oid() != size_t( -1 ) ? natus::audio::result::ok : natus::audio::result::failed ;
}

natus::audio::result oal_backend::capture( natus::audio::capture_object_res_t cap, bool_t const b ) noexcept
{
    if( cap->get_id()->get_oid() == size_t( -1 ) )
        return natus::audio::result::invalid_argument ;

    if( _pimpl->control_what_u_hear_capturing( b ) )
    {
        if( _what_u_hear_count++ == 0 )
            _pimpl->capture_what_u_hear_samples() ;

        _pimpl->capture_samples( *cap ) ;
    }

    return natus::audio::result::ok ;
}

void_t oal_backend::begin( void_t ) noexcept
{
}

void_t oal_backend::end( void_t ) noexcept
{
    _what_u_hear_count = 0 ;
}
