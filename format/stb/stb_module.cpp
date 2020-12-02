
#include "stb_module.h"

#include <natus/font/structs.h>
#include <natus/io/database.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/graphics/texture/image.hpp>
#include <natus/math/vector/vector4.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stb_vorbis.c>
#include <stb_truetype.h>

#include <natus/font/stb/stb_glyph_atlas_creator.h>

using namespace natus::format ;

// ***
void_t stb_module_register::register_module( natus::format::module_registry_res_t reg ) 
{
    reg->register_import_factory( { "png", "jpg" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;
    reg->register_export_factory( { "png" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;

    reg->register_import_factory( { "ogg" }, stb_audio_factory_res_t( stb_audio_factory_t() ) ) ;
    reg->register_import_factory( { "ttf" }, stb_font_factory_res_t( stb_font_factory_t() ) ) ;
}

// ***
natus::format::future_item_t stb_image_module::import_from( natus::io::location_cref_t loc, natus::io::database_res_t db ) noexcept 
{
    return stb_image_module::import_from( loc, db, natus::property::property_sheet_t() ) ;
}

// ***
natus::format::future_item_t stb_image_module::import_from( natus::io::location_cref_t loc, 
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
            natus::log::global_t::error( "[wav_import] : can not load location " + loc.as_string() ) ;
            return natus::format::item_res_t( natus::format::status_item_t( "error" ) ) ;
        }

        int width, height, comp ;

        //
        // loading the image in uchar format
        // @todo float images
        //
        
        uchar_ptr_t stb_data_ptr = stbi_load_from_memory( uchar_cptr_t( data_buffer.get() ),
            int_t( data_buffer.size() ), &width, &height, &comp, 0 ) ;

        if( natus::log::global::error( stb_data_ptr == nullptr,
            natus_log_fn( "stbi_load_from_memory" ) ) )
        {
            char_cptr_t stbi_err_msg = stbi_failure_reason() ;
            natus::log::global::error( natus_log_fn( "stb_image" )
                + std::string( stbi_err_msg ) ) ;

            return natus::format::item_res_t( natus::format::status_item_t("Can not load from memory.") ) ;
        }

        natus::graphics::image_format imf = natus::graphics::image_format::unknown ;
        switch( comp )
        {
        case 1: imf = natus::graphics::image_format::intensity ; break ;
        case 3: imf = natus::graphics::image_format::rgba ; break ;
        case 4: imf = natus::graphics::image_format::rgba; break ;
        default: break  ;
        }

        natus::graphics::image_t img( imf, natus::graphics::image_element_type::uint8,
            natus::graphics::image_t::dims_t( size_t( width ), size_t( height ), 1 ) ) ;

        if( comp == 3 )
        {
            img.update( [&] ( natus::graphics::image_ptr_t, natus::graphics::image_t::dims_in_t dims, void_ptr_t data_in )
            {
                typedef natus::math::vector3< uint8_t > rgb_t ;
                typedef natus::math::vector4< uint8_t > rgba_t ;
                auto* dst = reinterpret_cast< rgba_t* >( data_in ) ;
                auto* src = reinterpret_cast< rgb_t* >( stb_data_ptr ) ;

                size_t const ne = dims.x() * dims.y() * dims.z() ;
                for( size_t i=0; i<ne; ++i ) 
                {
                    // mirror y
                    size_t const start = ne - width * ( ( i / width ) + 1 ) ;
                    dst[ i ] = rgba_t( src[ start + i % width ], 255 );
                }
            } ) ;
        }
        else if( comp == 4 )
        {
            img.update( [&] ( natus::graphics::image_ptr_t, natus::graphics::image_t::dims_in_t dims, void_ptr_t data_in )
            {
                typedef natus::math::vector4< uint8_t > rgba_t ;
                auto* dst = reinterpret_cast< rgba_t* >( data_in ) ;
                auto* src = reinterpret_cast< rgba_t* >( stb_data_ptr ) ;

                size_t const ne = dims.x() * dims.y() * dims.z() ;
                for( size_t i = 0; i < ne; ++i )
                {
                    // mirror y
                    size_t const start = ne - width * ( ( i / width ) + 1 ) ;
                    dst[ i ] = rgba_t( src[ start + i % width ] );
                }

            } ) ;
        }
        stbi_image_free( stb_data_ptr ) ;

         ;
        return natus::format::item_res_t( natus::format::image_item_res_t( 
            natus::format::image_item_t( std::move( img ) ) ) ) ;
    } ) ;
}

// ***
natus::format::future_item_t stb_audio_module::import_from( natus::io::location_cref_t loc, natus::io::database_res_t db ) noexcept
{
    return stb_audio_module::import_from( loc, db, natus::property::property_sheet_t() ) ;
}

// ***
natus::format::future_item_t stb_audio_module::import_from( natus::io::location_cref_t loc, 
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
            natus::log::global_t::error( "[wav_import] : can not load location " + loc.as_string() ) ;
            return natus::format::item_res_t( natus::format::status_item_t( "error" ) ) ;
        }

        natus::audio::buffer_t bo ;

        {
            int_t error ;
            stb_vorbis* stbv = stb_vorbis_open_memory( uchar_cptr_t( data_buffer.get() ), data_buffer.size(), &error, nullptr ) ;

            if( stbv == nullptr )
            {
                natus::log::global_t::error( "[stb_audio_module] : failed to import .ogg file [" + loc.as_string() + "] with stb error code [" + std::to_string( error ) + "]" ) ;
                return natus::format::item_res_t( natus::format::status_item_res_t(
                    natus::format::status_item_t( "Error loading .ogg file" ) ) ) ;
            }

            size_t sample_rate = 0 ;
            size_t channels = 0 ;
            size_t max_frames = 0 ;

            {
                stb_vorbis_info const info = stb_vorbis_get_info( stbv ) ;
                sample_rate = info.sample_rate ;
                channels = info.channels ;
                max_frames = info.max_frame_size ;
            }

            {
                natus::ntd::vector< float_t > samples ;
                natus::ntd::vector< float_t > tmp( 1 << 14 ) ;

                int_t num_samples = -1 ;
                size_t s = 0 ;

                while( num_samples != 0 )
                {
                    num_samples = stb_vorbis_get_samples_float_interleaved(
                        stbv, channels, tmp.data(), tmp.size() ) ;

                    size_t const start = samples.size() ;
                    samples.resize( samples.size() + size_t( num_samples * channels) ) ;
                    for( size_t i=0; i<num_samples<<1; i+=channels )
                    {
                        for( size_t j=0; j<channels; ++j )
                        {
                            samples[ start + i + j ] = tmp[ i + j ] ;
                        }
                    }

                    ++s ;
                }
                
                natus::log::global_t::status( "[stb_module] : Loaded (" + std::to_string( s ) + " x " + std::to_string( tmp.size() ) + ") samples : " + std::to_string( samples.size() ) ) ;

                bo.set_samples( natus::audio::to_channels( channels ), sample_rate, std::move( samples ) ) ;
            }
            stb_vorbis_close( stbv ) ;
        }

        return natus::format::item_res_t( natus::format::audio_item_res_t(
            natus::format::audio_item_t( natus::audio::buffer_res_t( std::move( bo ) ) ) ) ) ;
    } ) ;
}

// ***
natus::format::future_item_t stb_font_module::import_from( natus::io::location_cref_t loc, natus::io::database_res_t db ) noexcept
{
    return stb_font_module::import_from( loc, db, natus::property::property_sheet_t() ) ;
}

// ***
natus::format::future_item_t stb_font_module::import_from( natus::io::location_cref_t loc, 
                natus::io::database_res_t db, natus::property::property_sheet_res_t ps ) noexcept 
{
    return std::async( std::launch::async, [=] ( void_t )
    {
        natus::memory::malloc_guard<char_t> data_buffer ;

        natus::font::font_files_t ffs ;

        natus::ntd::vector< natus::io::database_t::cache_access_t > caches = 
        {
            db->load( loc )
        } ;
        
        // check additional locations
        {
            natus::ntd::vector< natus::io::location_t > additional_locations ;
            if( ps->get_value( "additional_locations", additional_locations ) )
            {
                for( auto const & l : additional_locations )
                {
                    caches.emplace_back( db->load( l ) ) ;
                }
            }
        }

        for( auto & cache : caches )
        {
            auto const res = cache.wait_for_operation( [&] ( char_cptr_t data, size_t const sib )
            {
                ffs.emplace_back( natus::font::font_file_t( loc.as_string(), (uchar_cptr_t)data, sib ) ) ;
            }) ;

            if( !res ) 
            {
                return natus::format::item_res_t( natus::format::status_item_res_t(
                    natus::format::status_item_t( "Error loading .ttf file [" + loc.as_string() + "]" ) ) ) ;
            }
        }        

        natus::font::code_points_t pts ;
        if( !ps->get_value( "code_points", pts ) )
        {
            for( uint32_t i=33; i<=126; ++i ) pts.emplace_back( i ) ;
            for( uint32_t i : {uint32_t(0x00003041)} ) pts.emplace_back( i ) ;
        }
        
        pts.emplace_back( natus::font::utf32_t('?') ) ;
        pts.emplace_back( natus::font::utf32_t(' ') ) ;

        size_t dpi ;
        if( !ps->get_value( "dpi", dpi ) ) dpi = 96 ;

        size_t pt ;
        if( !ps->get_value( "point_size", pt ) ) pt = 25 ;

        size_t max_width, max_height ;
        if( !ps->get_value( "atlas_width", max_width ) ) max_width = 2048 ;
        if( !ps->get_value( "atlas_height", max_height ) ) max_height = 2048 ;

        natus::font::glyph_atlas_t ga = natus::font::stb::glyph_atlas_creator_t::create_glyph_atlas( ffs, pt, dpi, pts, 
            max_width, max_height ) ;
                    

        return natus::format::item_res_t( natus::format::glyph_atlas_item_res_t(
            natus::format::glyph_atlas_item_t( std::move( ga ) ) ) ) ;
    } ) ;
}