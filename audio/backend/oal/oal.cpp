
#include "oal.h"

#include "../../id.hpp"
#include "../../enums.h"

#include <natus/math/vector/vector2.hpp>
#include <natus/ntd/vector.hpp>

#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>

#include <limits>

using namespace natus::audio ;

namespace this_file
{
    struct capture_obj
    {
        ALCdevice* dev = nullptr ;

        size_t num_channels = 0 ;
        size_t frame_size = 0 ;

        bool_t started = false ;

        // here we reuse the buffer for later usage so
        // be do not have to reallocate memory.
        natus::ntd::vector< ALbyte > buffer ;
        natus::ntd::vector< float_t > fbuffer ;
    };
    natus_typedef( capture_obj ) ;
}

struct natus::audio::oal_backend::pimpl
{
    natus_this_typedefs( pimpl ) ;

    typedef natus::ntd::vector< this_file::capture_obj > captures_t ;
    captures_t captures ;

    pimpl( void_t ) {}
    ~pimpl( void_t )
    {
        for( auto& c : captures )
        {
            if( c.dev != nullptr )
            {
                alcCaptureCloseDevice( c.dev ) ;
            }
        }
    }

    size_t construct_capture_object( natus::audio::capture_object_ref_t cap )
    {
        auto const iter = std::find_if( captures.begin(), captures.end(), [&] ( this_file::capture_obj_cref_t co )
        {
            return co.dev == nullptr ;
        } ) ;

        size_t idx = 0 ;

        if( iter == captures.end() )
        {
            idx = captures.size() ;
            captures.resize( +5 ) ;
        }
        else
        {
            idx = std::distance( captures.begin(), iter ) ;
        }

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
                return size_t( -1 ) ;
            }
        }

        // default is 16 bit and 96k for now
        {
            ALCenum fmt = 0 ;
            switch( cap.get_channels() )
            {
            case natus::audio::channels::mono: fmt = AL_FORMAT_MONO16 ;  break ;
            case natus::audio::channels::stereo: fmt = AL_FORMAT_STEREO16 ; break ;
            default: break ;
            }

            captures[ idx ].dev = alcCaptureOpenDevice( whatuhear.c_str(),
                ALCuint( natus::audio::to_number( natus::audio::frequency::freq_48k ) ), fmt, ALCuint( 1 << 15 ) ) ;

            if( captures[ idx ].dev == nullptr )
            {
                natus::log::global_t::error( "[OpenAL backend] : failed to open capture device." ) ;
                return size_t( -1 ) ;
            }

            captures[ idx ].frame_size = natus::audio::to_number( cap.get_channels() ) * 16 / 8 ;
            captures[ idx ].num_channels = natus::audio::to_number( cap.get_channels() ) ;
        }

        return idx ;
    }

    void_t capture_samples( natus::audio::capture_object_ref_t cap, bool_t const do_capture )
    {
        this_file::capture_obj& co = captures[ cap.get_id()->get_oid() ] ;

        ALCdevice* dev = co.dev ;

        if( co.started != do_capture )
        {
            if( !do_capture )
            {
                if( co.started ) alcCaptureStop( dev ) ;
                return ;
            }
            else
            {
                if( !co.started ) alcCaptureStart( dev ) ;
            }
        }

        ALCint count = 0 ;
        alcGetIntegerv( dev, ALC_CAPTURE_SAMPLES, 1, &count ) ;

        natus::ntd::vector< ALbyte >& buffer = co.buffer ;
        auto& fbuffer = co.fbuffer ;

        buffer.resize( size_t( count ) * co.frame_size ) ;
        fbuffer.resize( size_t( count ) * co.frame_size ) ;

        alcCaptureSamples( dev, buffer.data(), count ) ;
        if( alcGetError( dev ) != AL_NO_ERROR )
        {
            natus::log::global_t::error( "[OpenAL] : capturing samples failed" ) ;
            return ;
        }

        double_t dfrequency = float_t( natus::audio::to_number( natus::audio::frequency::freq_48k ) ) ;


        for( size_t i = 0; i < count; ++i )
        {
            for( size_t j = 0; j < co.num_channels ; ++j )
            {
                // the index into the buffer
                size_t const idx = i * co.frame_size + j  ;

                // reconstruct the 16 bit value
                #if !NATUS_BIG_ENDIAN
                int_t const ivalue = int_t( buffer[ idx + 1 ] << 8 ) | int_t( buffer[ idx + 0 ] << 0 );
                #else
                int_t const ivalue = int_t( buffer[ idx + 0 ] << 8 ) | int_t( buffer[ idx + 1 ] << 0 );
                #endif

                size_t const index = i * natus::audio::to_number( cap.get_channels() ) + j ;
                fbuffer[ index ] = float_t( double_t( ivalue ) / dfrequency ) ;
            }
        }

        cap.shift_and_copy_from( size_t( count ) * natus::audio::to_number( cap.get_channels() ), fbuffer.data(),
            natus::audio::to_number( natus::audio::frequency::freq_48k ) ) ;
        //natus::log::global_t::status("Count : " + std::to_string(count) ) ;
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

natus::audio::result oal_backend::configure( natus::audio::capture_object_res_t cap ) noexcept
{
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

    _pimpl->capture_samples( *cap, b ) ;
    return natus::audio::result::ok ;
}

void_t oal_backend::begin( void_t ) noexcept
{
}

void_t oal_backend::end( void_t ) noexcept
{
}
