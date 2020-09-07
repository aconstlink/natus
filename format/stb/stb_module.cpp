
#include "stb_module.h"

#include <natus/io/database.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/graphics/texture/image.hpp>
#include <natus/math/vector/vector4.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace natus::format ;

// ***
void_t stb_module_register::register_module( natus::format::module_registry_res_t reg ) 
{
    reg->register_import_factory( { "png", "jpg" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;
    reg->register_export_factory( { "png" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;

    reg->register_import_factory( { "ogg" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;
    reg->register_import_factory( { "ttf" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;
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
            // funk not called
        }

        int width, height, comp ;

        //
        // loading the image in uchar format
        // @todo float images
        //
        
        uchar_ptr_t stb_data_ptr = stbi_load_from_memory( uchar_cptr_t( data_buffer.get() ),
            int_t( data_buffer.size() ), &width, &height, &comp, 0 ) ;

        if( natus::log::global::error( stb_data_ptr == nullptr,
            "[so_imex::stb_module::import_image] : stbi_load_from_memory" ) )
        {
            char_cptr_t stbi_err_msg = stbi_failure_reason() ;
            natus::log::global::error( "[so_imex::stb_module::import_image] : stb_image says : "
                + std::string( stbi_err_msg ) ) ;

            return natus::format::item_res_t( natus::format::error_item_t("Can not load from memory.") ) ;
        }

        natus::graphics::image_format imf = natus::graphics::image_format::unknown ;
        switch( comp )
        {
        case 1: imf = natus::graphics::image_format::intensity ; break ;
        case 3: imf = natus::graphics::image_format::rgb ; break ;
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
                auto* data = reinterpret_cast< rgb_t* >( data_in ) ;

                size_t i = 0 ;
                for( size_t y = 0; y < dims.y(); ++y )
                {
                    for( size_t x = 0; x < dims.x(); ++x )
                    {
                        rgb_t const pixel = *( ( rgb_t* ) ( &stb_data_ptr[ y * width + x ] ) ) ;
                        data[ i++ ] = pixel;
                    }
                }
            } ) ;
        }
        else if( comp == 4 )
        {
            img.update( [&] ( natus::graphics::image_ptr_t, natus::graphics::image_t::dims_in_t dims, void_ptr_t data_in )
            {
                typedef natus::math::vector4< uint8_t > rgba_t ;
                auto* data = reinterpret_cast< rgba_t* >( data_in ) ;

                size_t i = 0 ;
                for( size_t y = 0; y < dims.y(); ++y )
                {
                    for( size_t x = 0; x < dims.x(); ++x )
                    {
                        rgba_t const pixel = *( ( rgba_t* ) ( &stb_data_ptr[ y * width + x ] ) ) ;
                        data[ i++ ] = pixel;
                    }
                }
            } ) ;
        }

        /*
        int index = 0 ;

        #if 1
        for( int y = height - 1; y >= 0; --y )
        {
            for( int x = 0; x < width; ++x )
            {
                size_t const src_index = y * width + x ;

                for( int c = 0; c < comp; ++c )
                {
                    dest_ptr[ index++ ] = stb_data_ptr[ src_index * comp + c ] ;
                }
            }
        }
        #else
        for( int y = 0; y < height; ++y )
        {
            for( int x = 0; x < width; ++x )
            {
                size_t const src_index = y * width + x ;

                for( int c = 0; c < comp; ++c )
                {
                    dest_ptr[ index++ ] = stb_data_ptr[ src_index * comp + c ] ;
                }
            }
        }
        #endif
        */
        stbi_image_free( stb_data_ptr ) ;

         ;
        return natus::format::item_res_t( natus::format::image_item_res_t( 
            natus::format::image_item_t( std::move( img ) ) ) ) ;
    } ) ;
}