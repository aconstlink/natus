
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

    struct buffer_config
    {
        natus::ntd::string_t name ;

        // a source id used for purly playing the buffer
        // it is generated along with the buffer
        ALuint sid = ALuint( -1 ) ;

        // the buffer id
        ALuint id = ALuint( -1 ) ;

        ALCenum format = 0 ;
        ALvoid const* data = nullptr ;
        ALsizei sib = 0 ;
        ALsizei samplerate = 0 ;
    };
    natus_typedef( buffer_config ) ;
}

struct natus::audio::oal_backend::pimpl
{
    natus_this_typedefs( pimpl ) ;

    size_t _captures = 0 ;
    size_t _do_captures = 0 ;

    this_file::global_capture_ptr_t _gc ;
    natus::ntd::vector< this_file::buffer_config_t > buffers ;

    ALCdevice * ddev = nullptr ;
    ALCcontext * dctx = nullptr ;

    bool_t init( void_t ) noexcept
    {
        {
            auto const res = alcIsExtensionPresent( NULL, "ALC_ENUMERATION_EXT" ) ;
            if( res != ALC_TRUE ) return false ;
        }

        {
            auto const* list = alcGetString( NULL, ALC_DEVICE_SPECIFIER ) ;
            size_t sib = 0 ;
            while( true )
            {
                natus::ntd::string_t item = list + sib  ;

                if( item.size() == 0 ) break ;

                natus::log::global_t::status( item ) ;
                sib += item.size() + 1 ;
            }
        }

        {
            ddev = alcOpenDevice( NULL ) ;
            if( ddev == nullptr )
            {
                natus::log::global::error( "[OpenAL Backend] : alcOpenDevice for default device" ) ;
                return false ;
            }
        }
        {
            dctx = alcCreateContext( ddev, NULL ) ;
            auto const res = alcGetError( ddev ) ;
            if( res != AL_NO_ERROR )
            {
                natus::log::global::error( "[OpenAL Backend] : alcCreateContext for default device context" ) ;
                return false ;
            }
        }

        {
            alcMakeContextCurrent( dctx ) ;
            auto const res = alcGetError( ddev ) ;
            if( res != AL_NO_ERROR )
            {
                natus::log::global::error( "[OpenAL Backend] : alcMakeContextCurrent for default device context" ) ;
                return false ;
            }
        }

        return true ;
    }

    void_t release( void_t ) 
    {
        if( _gc != nullptr )
        {
            alcCaptureCloseDevice( _gc->dev ) ;
            _gc->dev = nullptr ;
            natus::memory::global_t::dealloc( _gc ) ;
        }

        for( auto& b : buffers )
        {
            if( b.id != ALuint( -1 ) )
            {
                alDeleteBuffers( 1, &b.id ) ;
                auto const res = alGetError() ;
                natus::log::global::error( res != AL_NO_ERROR, "[OpenAL Backend] : alDeleteBuffers" ) ;
                b.id = ALuint( -1 ) ;
            }
            natus::memory::global_t::dealloc_raw( (void_ptr_t)b.data ) ;

            b.data = nullptr ;
            b.format = 0 ;
            b.name = "" ;
            b.samplerate = 0 ;
            b.sib = 0 ;
        }

        if( dctx != nullptr )
        {
            {
                auto const res = alcMakeContextCurrent( NULL ) ;
                natus::log::global::error( res != AL_TRUE, "[OpenAL Backend] : alcMakeContextCurrent(NULL)" ) ;
            }
            {
                alcDestroyContext( dctx ) ;
                auto const res = alcGetError( ddev ) ;
                natus::log::global::error( res != AL_NO_ERROR, "[OpenAL Backend] : alcDestroyContext" ) ;
            }
        }

        if( ddev != nullptr )
        {
            auto const res = alcCloseDevice( ddev ) ;
            natus::log::global::error( res != AL_TRUE, "[OpenAL Backend] : alcCloseDevice" ) ;
        }
    }

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

        for( size_t i = 0; i < (size_t)count; ++i )
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
            //float_cptr_t values = fbuffer.data() ;

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
                //_gc->frequencies[ i ] = (_gc->frequencies[ i ] < (1.0f * div)) ? 0.0f : _gc->frequencies[ i ] ;

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

    size_t construct_capture_object( natus::audio::capture_object_ref_t /*cap*/ )
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

    size_t construct_buffer_object( size_t oid, natus::ntd::string_cref_t name, natus::audio::buffer_object_ref_t )
    {
        // the name must be unique
        {
            auto iter = std::find_if( buffers.begin(), buffers.end(),
                [&] ( this_file::buffer_config_cref_t c )
            {
                return c.name == name ;
            } ) ;

            if( iter != buffers.end() )
            {
                size_t const i = std::distance( buffers.begin(), iter ) ;
                if( natus::log::global_t::error( i != oid && oid != size_t( -1 ),
                    natus_log_fn( "name and id do not match" ) ) )
                {
                    return oid ;
                }
            }
        }

        // make oid
        if( oid == size_t( -1 ) )
        {
            // must stay here because of 0 size buffers
            size_t i = 0 ;
            for( ; i < buffers.size(); ++i )
            {
                if( natus::core::is_not( buffers[ i ].id != ALuint( -1 ) ) ) break ;
            }
            oid = i ;
        }

        if( oid >= buffers.size() )
            buffers.resize( oid + 1 ) ;

        // create buffer
        if( buffers[ oid ].id == ALuint( -1 ) )
        {
            {
                ALCuint buffer = 0 ;
                alGenBuffers( 1, &buffer ) ;
                auto const res = alGetError() ;
                natus::log::global_t::error( res != ALC_NO_ERROR,
                    "[OpenAL Backend] : alGenBuffers" ) ;

                buffers[ oid ].id = buffer ;
            }

            {
                ALuint sid = ALuint( -1 ) ;
                {
                    alGenSources( 1, &sid ) ;
                    auto const res = alGetError() ;
                    natus::log::global_t::error( res != ALC_NO_ERROR,
                        "[OpenAL Backend] : alGenSources" ) ;

                    buffers[ oid ].sid = sid ;
                }

                {
                    alSourcef( sid, AL_GAIN, 0.1f ) ;
                    auto const res = alGetError() ;
                    natus::log::global_t::error( res != ALC_NO_ERROR,
                        "[OpenAL Backend] : alSourcef[GAIN]" ) ;}
            }
        }

        return oid ;
    }

    bool_t update( size_t const oid, natus::audio::buffer_object_ref_t buffer ) noexcept
    {
        auto & alb = buffers[ oid ] ;

        alb.samplerate = ALsizei( buffer.get_sample_rate() ) ;

        // reallocate the buffer
        {
            if( buffer.get_sib() != size_t( alb.sib ) )
            {
                natus::memory::global_t::dealloc_raw<ALCbyte>( (ALCbyte*)alb.data ) ;
                alb.data = natus::memory::global_t::alloc_raw<ALCbyte>( buffer.get_sib() ) ;
                alb.sib = ALsizei( buffer.get_sib() ) ;
            }
        }

        // determine format
        {
            ALCenum format = 0 ;
            if( buffer.get_num_channels() == 1 )
                format = AL_FORMAT_MONO16  ;
            else if( buffer.get_num_channels() == 2 )
                format = AL_FORMAT_STEREO16 ;

            if( format == 0 )
            {
                natus::log::global_t::error( "[OpenAL Backend] : invalid number of channels for "
                    "[" + buffer.get_name() + "]" ) ;
                return false ;
            }
            alb.format = format ;
        }

        // convert float to 16 data
        {
            auto const & floats = buffer.get_samples() ;
            ALshort* ptr = ( ALshort* ) alb.data ;
            size_t const channels = buffer.get_num_channels() ;
            for( size_t i = 0; i < floats.size(); i+=channels )
            {
                size_t const idx = i ;
                for( size_t c = 0; c < channels; ++c )
                {
                    *ptr = ALshort( floats[ idx + c ] * float_t( (1<<15)-1 ) ) ; ++ptr ;
                }
            }
        }

        // 
        {
            alSourcei( alb.sid, AL_BUFFER, 0 ) ;
            auto const res = alGetError() ;
            natus::log::global_t::error( res != ALC_NO_ERROR,
                "[OpenAL Backend] : alGenSources" ) ;
        }

        // copy the buffer to al
        {
            ALuint const id = alb.id ;
            ALCenum const format = alb.format ;
            ALvoid const* data = alb.data ;
            ALsizei const sib = alb.sib ;
            ALsizei const samplerate = alb.samplerate ;

            alBufferData( id, format, data, sib, samplerate );
            auto const res = alGetError() ;
            natus::log::global_t::error( res != ALC_NO_ERROR,
                "[OpenAL Backend] : alBufferData" ) ;
        }

        {
            alSourcei( alb.sid, AL_BUFFER, ( ALint ) alb.id ) ;
            auto const res = alGetError() ;
            natus::log::global_t::error( res != ALC_NO_ERROR,
                "[OpenAL Backend] : alGenSources" ) ;
        }

        return true ;
    }

    void_t execute( size_t const oid, natus::audio::buffer_object_ref_t obj, 
        natus::audio::backend::execute_detail_cref_t det ) noexcept
    {
        auto& bo = buffers[ oid ] ;

        if( det.to == natus::audio::execution_options::stop )
        {
            alSourceStop( bo.sid ) ;
            auto const res = alGetError() ;
            natus::log::global_t::error( res != ALC_NO_ERROR,
                "[OpenAL Backend] : alSourceStop" ) ;
        }
        else if( det.to == natus::audio::execution_options::pause )
        {
            alSourcePause( bo.sid ) ;
            auto const res = alGetError() ;
            natus::log::global_t::error( res != ALC_NO_ERROR,
                "[OpenAL Backend] : alSourcePause" ) ;
        }
        else if( det.to == natus::audio::execution_options::replay )
        {
            alSourceRewind( bo.sid ) ;
            auto const res = alGetError() ;
            natus::log::global_t::error( res != ALC_NO_ERROR,
                "[OpenAL Backend] : alSourceRewind" ) ;
        }
        else if( det.to == natus::audio::execution_options::play )
        {
            if( det.sample != size_t(-1) )
            {
                alSourcei( bo.sid, AL_SAMPLE_OFFSET, (ALint)det.sample ) ;
                auto const res = alGetError() ;
                natus::log::global_t::error( res != ALC_NO_ERROR,
                    "[OpenAL Backend] : alSourcei" ) ;
            }
            else if( det.sec > 0.0f )
            {
                alSourcef( bo.sid, AL_SEC_OFFSET, (ALfloat) det.sec ) ;
                auto const res = alGetError() ;
                natus::log::global_t::error( res != ALC_NO_ERROR,
                    "[OpenAL Backend] : alSourcef" ) ;
            }

            alSourcePlay( bo.sid );
            auto const res = alGetError() ;
            natus::log::global_t::error( res != ALC_NO_ERROR,
                "[OpenAL Backend] : alSourceRewind" ) ;
        }

        {
            ALenum state ;
            alGetSourcei( bo.sid, AL_SOURCE_STATE, &state ) ;
            if( state == AL_PLAYING )
            {
                obj.set_execution_state( natus::audio::execution_state::playing ) ;
            }
            else if( state == AL_INITIAL || state == AL_STOPPED )
            {
                obj.set_execution_state( natus::audio::execution_state::stopped ) ;
            }
            else if( state == AL_PAUSED )
            {
                obj.set_execution_state( natus::audio::execution_state::paused ) ;
            }
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
}

oal_backend::oal_backend( this_rref_t rhv ) noexcept : backend( std::move( rhv ) )
{
    natus_move_member_ptr( _pimpl, rhv ) ;
}

oal_backend::~oal_backend( void_t ) noexcept
{
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

natus::audio::result oal_backend::configure( natus::audio::buffer_object_res_t obj ) noexcept
{
    natus::audio::id_res_t id = obj->get_id() ;

    {
        id = natus::audio::id_t( this_t::get_bid(),
            _pimpl->construct_buffer_object( id->get_oid( this_t::get_bid() ), obj->name(), *obj ) ) ;
    }

    {
        auto const res = _pimpl->update( id->get_oid( this_t::get_bid() ), *obj ) ;
        if( natus::core::is_not( res ) ) return natus::audio::result::failed ;
    }

    return natus::audio::result::ok ;
}

natus::audio::result oal_backend::update( natus::audio::buffer_object_res_t obj ) noexcept
{
    natus::audio::id_res_t id = obj->get_id() ;
    if( id->is_not_valid() )
    {
        natus::log::global_t::error( "[OpenAL Backend] : invalid buffer id for [" + obj->get_name() + "]" ) ;
        return natus::audio::result::invalid_argument ;
    }

    {
        auto const res = _pimpl->update( id->get_oid( this_t::get_bid() ), *obj ) ;
        if( natus::core::is_not( res ) ) return natus::audio::result::failed ;
    }

    return natus::audio::result::ok ;
}

natus::audio::result oal_backend::execute( natus::audio::buffer_object_res_t obj, natus::audio::backend::execute_detail_cref_t det ) noexcept
{
    natus::audio::id_res_t id = obj->get_id() ;
    if( id->is_not_valid() )
    {
        natus::log::global_t::error( "[OpenAL Backend] : invalid buffer id for [" + obj->get_name() + "]" ) ;
        return natus::audio::result::invalid_argument ;
    }

    _pimpl->execute( id->get_oid(), *obj, det ) ;

    return natus::audio::result::ok ;
}

void_t oal_backend::init( void_t ) noexcept 
{
    if( _pimpl == nullptr )
    {
        _pimpl = natus::memory::global_t::alloc( pimpl(), "[natus::audio::oal_backend::pimpl]" ) ;
        _pimpl->init() ;
    }
}

void_t oal_backend::release( void_t ) noexcept 
{
    if( _pimpl != nullptr )
    {
        _pimpl->release() ;
    }
    natus::memory::global_t::dealloc( _pimpl ) ;
}

void_t oal_backend::begin( void_t ) noexcept
{
    
}

void_t oal_backend::end( void_t ) noexcept
{
    _what_u_hear_count = 0 ;
}
