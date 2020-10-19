
#include "stb_module.h"

#include <natus/io/database.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/graphics/texture/image.hpp>
#include <natus/math/vector/vector4.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stb_vorbis.c>
#include <stb_truetype.h>

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

        natus::audio::buffer_object_t bo ;

        {
            int_t error ;
            stb_vorbis * stbv = stb_vorbis_open_memory( uchar_cptr_t( data_buffer.get() ), data_buffer.size(), &error, nullptr ) ;

            if( stbv == nullptr ) 
            {
                natus::log::global_t::error( "[stb_audio_module] : failed to import .ogg file [" + loc.as_string() + "] with error code [" + std::to_string( error ) + "]" ) ;
                return natus::format::item_res_t( natus::format::status_item_res_t(
                    natus::format::status_item_t( "Error loading .ogg file" ) ) ) ;
            }
            int const bp = 0 ;
        }

        return natus::format::item_res_t( natus::format::audio_item_res_t(
            natus::format::audio_item_t( std::move( bo ) ) ) ) ;
    } ) ;
}

// ***
natus::format::future_item_t stb_font_module::import_from( natus::io::location_cref_t /*loc*/, natus::io::database_res_t /*db*/ ) noexcept
{
    return std::async( std::launch::async, [=] ( void_t )
    {
        return natus::format::item_res_t( natus::format::status_item_res_t(
            natus::format::status_item_t( "not implemented" ) ) ) ;
    } ) ;
}