
#include "wav_module.h"
#include "../future_items.hpp"

#include <natus/io/database.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/math/vector/vector4.hpp>

using namespace natus::format ;

namespace this_file
{
    /// informational chunk
    struct chunk
    {
        char_t id[ 5 ] = { 'x', 'x', 'x', 'x', '\0' } ;
        uint32_t size = 0 ;
    };
    natus_typedef( chunk ) ;
    natus_typedefs( natus::ntd::vector<chunk_t>, chunks ) ;

    struct riff_chunk
    {
        uint32_t size = 0 ;
    };

    struct fmt_chunk
    {
        // 16, 18, 40
        uint32_t size ;

        // pcm : 1
        // ieee_float : 3
        // a-law : 6
        // u-law : 7
        // 0xfffe : extensible 
        uint16_t format ;

        uint16_t channels ;
        uint32_t samples_per_sec ;
        uint32_t avg_bytes_per_sec ;
        uint16_t block_align ;
        uint16_t bits_per_sample ;
        uint16_t ext_size ;
        uint16_t valid_bits_per_sample ;
        uint32_t channel_mask ;
    };

    struct list_chunk
    {
        uint32_t size = 0 ;
    };

    struct data_chunk
    {
        uint32_t size = 0 ;
    };

    struct wav_file
    {
        chunks_t chunks ;

        riff_chunk riff ;
        fmt_chunk fmt ;
        list_chunk list ;
        data_chunk data ;
    };
}

// ***
void_t wav_module_register::register_module( natus::format::module_registry_res_t reg )
{
    reg->register_import_factory( { "wav" }, wav_audio_factory_res_t( wav_audio_factory_t() ) ) ;
    reg->register_export_factory( { "wav" }, wav_audio_factory_res_t( wav_audio_factory_t() ) ) ;
}

// ***
natus::format::future_item_t wav_audio_module::import_from( natus::io::location_cref_t loc, natus::io::database_res_t db ) noexcept
{
    return wav_audio_module::import_from( loc, db, natus::property::property_sheet_t() ) ;
}

// ***
natus::format::future_item_t wav_audio_module::import_from( natus::io::location_cref_t loc, 
                natus::io::database_res_t db, natus::property::property_sheet_res_t ) noexcept 
{
    return std::async( std::launch::async, [=] ( void_t )
    {
        natus::memory::malloc_guard<char_t> data_buffer ;

        natus::io::database_t::cache_access_t ca = db->load( loc ) ;
        auto const res = ca.wait_for_operation( [&] ( char_cptr_t data, size_t const sib )
        {
            data_buffer = natus::memory::malloc_guard<char_t>( data, sib ) ;
        } ) ;

        if( !res )
        {
            natus::log::global_t::warning( "[wav_import] : can not load location " + loc.as_string() ) ;
            return natus::format::item_res_t( natus::format::status_item_t( "error" ) ) ;
        }

        this_file::wav_file wf ;

        // only RIFF and WAVE supported
        {
            uint32_t const fmt = uint32_t( *uint32_ptr_t( data_buffer.get() ) ) ;
            uint32_t const ft = uint32_t( *uint32_ptr_t( &data_buffer[ sizeof( uint32_t ) << 1 ] ) ) ;

            char_t fmt_buffer[ 5 ] = { 'x', 'x', 'x', 'x', '\0' } ;
            char_t ft_buffer[ 5 ] = { 'x', 'x', 'x', 'x', '\0' } ;

            for( size_t i = 0; i < 4; ++i ) {
                fmt_buffer[ i ] = ( fmt >> ( i << 3 ) ) & 255 ;
                ft_buffer[ i ] = ( ft >> ( i << 3 ) ) & 255 ;
            }

            if( fmt_buffer[ 0 ] != 'R' || fmt_buffer[ 1 ] != 'I' || fmt_buffer[ 2 ] != 'F' )
            {
                natus::log::global_t::warning( "[wav_import] : unsupported format" ) ;
                return natus::format::item_res_t( natus::format::status_item_t( "error" ) ) ;
            }

            if( ft_buffer[ 0 ] != 'W' || ft_buffer[ 1 ] != 'A' || ft_buffer[ 2 ] != 'V' )
            {
                natus::log::global_t::error( "[wav_import] : unsupported format" ) ;
                return natus::format::item_res_t( natus::format::status_item_t( "error" ) ) ;
            }
        }

        uint32_t const sib = uint32_t( *uint32_ptr_t( &data_buffer[ sizeof( uint32_t ) ] ) ) -
            sizeof( uint32_t ) ;

        wf.riff.size = sib ;

        // strip of riff header
        size_t pos = sizeof( uint32_t ) * 3 ;

        natus::ntd::string_t fmt_tmp = "xxxx" ;

        natus::audio::buffer_t bo ;

        while( pos < sib )
        {
            size_t local_pos = pos ;

            this_file::chunk_t cur_chunk ;

            uint32_t const chunk_id = uint32_t( *uint32_ptr_t( &data_buffer[ local_pos ] ) ) ;
            local_pos += sizeof( uint32_t ) ; // id

            uint32_t const chunk_size = uint32_t( *uint32_ptr_t( &data_buffer[ local_pos ] ) ) ;
            local_pos += sizeof( uint32_t ) ; // size

            {
                for( size_t i = 0; i < 4; ++i )
                {
                    fmt_tmp[ i ] = ( chunk_id >> ( i << 3 ) ) & 255 ;
                    cur_chunk.id[ i ] = ( chunk_id >> ( i << 3 ) ) & 255 ;
                }
            }

            // LIST
            if( fmt_tmp == "LIST" )
            {
                wf.list.size = chunk_size ;
            }
            // "fmt " chunk : specifies the format of the data
            else if( fmt_tmp == "fmt " )
            {
                uint16_t const chunk_afmt = uint16_t( *uint16_ptr_t( &data_buffer[ local_pos ] ) ) ;
                local_pos += sizeof( uint16_t ) ; // audio format

                uint16_t const chunk_chan = uint16_t( *uint16_ptr_t( &data_buffer[ local_pos ] ) ) ;
                local_pos += sizeof( uint16_t ) ; // channels

                uint32_t const samples_per_sec = uint32_t( *uint32_ptr_t( &data_buffer[ local_pos ] ) ) ;
                local_pos += sizeof( uint32_t ) ; // samples per second

                uint32_t const avg_bytes_per_sec = uint32_t( *uint32_ptr_t( &data_buffer[ local_pos ] ) ) ;
                local_pos += sizeof( uint32_t ) ; // samples per second

                uint16_t const block_align = uint16_t( *uint16_ptr_t( &data_buffer[ local_pos ] ) ) ;
                local_pos += sizeof( uint16_t ) ; // block align

                uint16_t const bits_per_sample = uint16_t( *uint16_ptr_t( &data_buffer[ local_pos ] ) ) ;
                local_pos += sizeof( uint16_t ) ; // bits per sample

                uint16_t const cb_size = uint16_t( *uint16_ptr_t( &data_buffer[ local_pos ] ) ) ;
                local_pos += sizeof( uint16_t ) ; // extension size

                uint16_t const valid_bits = uint16_t( *uint16_ptr_t( &data_buffer[ local_pos ] ) ) ;
                local_pos += sizeof( uint16_t ) ; // valid bits per sample

                uint32_t const dw_channel_mask = uint32_t( *uint32_ptr_t( &data_buffer[ local_pos ] ) ) ;
                local_pos += sizeof( uint32_t ) ; // channel mask

                wf.fmt.size = chunk_size ;
                wf.fmt.format = chunk_afmt ;
                wf.fmt.channels = chunk_chan ;
                wf.fmt.samples_per_sec = samples_per_sec ;
                wf.fmt.avg_bytes_per_sec = avg_bytes_per_sec ;
                wf.fmt.block_align = block_align ;
                wf.fmt.bits_per_sample = bits_per_sample ;
                wf.fmt.ext_size = cb_size ;
                wf.fmt.valid_bits_per_sample = valid_bits ;
                wf.fmt.channel_mask = dw_channel_mask ;
            }
            else if( fmt_tmp == "data" )
            {
                wf.data.size = chunk_size ;

                // pcm
                if( wf.fmt.format == 1 )
                {
                    if( wf.fmt.bits_per_sample == 8 )
                    {
                        typedef natus::ntd::vector< float_t > floats_t ;

                        size_t const num_samples = chunk_size / sizeof( uint8_t ) ;
                        size_t const num_channels = wf.fmt.channels ;
                        size_t const sample_rate = wf.fmt.samples_per_sec ;
                        
                        floats_t samples( num_samples ) ;

                        int8_ptr_t dptr = int8_ptr_t( &data_buffer[ local_pos ] ) ;

                        for( size_t i = 0; i < num_samples; ++i )
                        {
                            samples[ i ] = float_t( dptr[ i ] ) / 255.0f ;
                        }

                        bo.set_samples( natus::audio::to_channels( num_channels ) , sample_rate, samples ) ;
                    }
                    else if( wf.fmt.bits_per_sample == 16 )
                    {
                        typedef natus::ntd::vector< float_t > floats_t ;

                        size_t const num_samples = chunk_size / sizeof( uint16_t ) ;
                        size_t const num_channels = wf.fmt.channels ;
                        size_t const sample_rate = wf.fmt.samples_per_sec ;

                        floats_t samples( num_samples ) ;

                        int16_ptr_t dptr = int16_ptr_t( &data_buffer[ local_pos ] ) ;

                        for( size_t i = 0; i < num_samples; ++i )
                        {
                            samples[ i ] = float_t( double_t( dptr[ i ] ) / double_t( ( 1 << 15 ) - 1 ) ) ;
                        }

                        bo.set_samples( natus::audio::to_channels( num_channels ) , sample_rate, samples ) ;
                    }
                    else if( wf.fmt.bits_per_sample == 24 )
                    {
                        struct uint24
                        {
                            uint8_t s[ 3 ] ;
                            
                            #if 0
                            float_t as_float( void_t ) const noexcept
                            {
                                int_t const trans = ( 1 << 23 ) - 1 ;
                                int_t const v = s[ 0 ] << 16 | s[ 1 ] << 8 | s[ 2 ] << 0 ;
                                return float_t( double_t( v - trans ) / double_t( trans ) ) ;
                            }
                            #elif 1
                            float_t as_float( void_t ) const noexcept
                            {
                                int_t const v0 = s[ 2 ] << 16 | s[ 1 ] << 8 | s[ 0 ] << 0 ;

                                double_t const div = double_t( ( 1 << 23 ) - 1 ) ;
                                return float_t( double_t( v0 ) / double_t( div ) ) ;
                            }
                            #else
                            float_t as_float( void_t ) const noexcept
                            {
                                int_t const sign = ( s[ 2 ] & ( 1 << 7 ) ) ;
                                int_t const b0 = s[ 2 ] & ( ( 1 << 7 ) - 1 ) ;
                                int_t const b1 = s[ 1 ] ;
                                int_t const b2 = s[ 0 ] ;

                                int_t const v0 = sign << 31 | b0 << 16 | b1 << 8 | b2 << 0 ;

                                double_t const div = double_t( ( 1 << 23 ) - 1 ) ;
                                return float_t( double_t( v0 ) / double_t( div ) ) ;
                            }
                            #endif
                        };
                        typedef uint24 uint24_t ;
                        typedef uint24 * uint24_ptr_t ;

                        typedef natus::ntd::vector< float_t > floats_t ;

                        size_t const num_samples = chunk_size / sizeof( uint24_t ) ;
                        size_t const num_channels = wf.fmt.channels ;
                        size_t const sample_rate = wf.fmt.samples_per_sec ;

                        floats_t samples( num_samples ) ;

                        uint24_ptr_t dptr = uint24_ptr_t( &data_buffer[ local_pos ] ) ;

                        for( size_t i = 0; i < num_samples; ++i )
                        {
                            samples[ i ] = dptr[i].as_float() ;
                        }

                        bo.set_samples( natus::audio::to_channels( num_channels ) , sample_rate, samples ) ;
                    }
                    else
                    {
                        natus::log::global_t::error( "[so_imex::wav_module::import_audio] : "
                            "bits per sample not supported." ) ;
                    }
                }
                // ieee float
                else if( wf.fmt.format == 3 )
                {
                    if( wf.fmt.bits_per_sample == 32 )
                    {
                        typedef natus::ntd::vector< float_t > floats_t ;

                        size_t const num_samples = chunk_size / sizeof( float_t ) ;
                        size_t const num_channels = wf.fmt.channels ;
                        size_t const sample_rate = wf.fmt.samples_per_sec ;

                        floats_t samples( num_samples ) ;

                        float_ptr_t dptr = float_ptr_t( &data_buffer[ local_pos ] ) ;

                        for( size_t i = 0; i < num_samples; ++i )
                        {
                            samples[ i ] = dptr[ i ] ;
                        }

                        bo.set_samples( natus::audio::to_channels( num_channels ) , sample_rate, samples ) ;
                    }
                    else
                    {
                        natus::log::global::error( "[so_imex::wav_module::import_audio] : "
                            "bits per sample not supported." ) ;
                    }
                }
                else
                {
                    natus::log::global::error( "[so_imex::wav_module::import_audio] : "
                        "wav format not supported." ) ;
                }
            }

            cur_chunk.size = chunk_size ;
            wf.chunks.push_back( cur_chunk ) ;

            // next chunk pos = cur_pos + chunk_id(4 bytes) + chunk_size(4 bytes)
            pos += chunk_size + ( sizeof( uint32_t ) << 1 ) ;
        }

        return natus::format::item_res_t( natus::format::audio_item_res_t(
            natus::format::audio_item_t( std::move( bo ) ) ) ) ;
    } ) ;
}

// ***
natus::format::future_item_t wav_audio_module::export_to( natus::io::location_cref_t loc, 
                natus::io::database_res_t, natus::format::item_res_t ) noexcept 
{
    return std::async( std::launch::async, [=] ( void_t )
    {
        return natus::format::item_res_t( natus::format::status_item_res_t(
                natus::format::status_item_t( "Export not implemented" ) ) ) ;
    } ) ;
}