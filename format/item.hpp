#pragma once

#include "typedefs.h"

#include <natus/audio/object/buffer_object.h>
#include <natus/graphics/texture/image.hpp>
#include <natus/font/glyph_atlas.h>

#include <future>

namespace natus
{
    namespace format
    {
        struct item
        {
            virtual ~item( void_t ) {}
        };
        natus_res_typedef( item ) ;
        typedef std::future< item_res_t > future_item_t ;

        struct image_item : public item
        {
            image_item( natus::graphics::image_res_t&& img_ ) : img( std::move( img_ ) ) {}
            virtual ~image_item( void_t ) {}

            natus::graphics::image_res_t img ;
        };
        natus_res_typedef( image_item ) ;

        struct audio_item : public item
        {
            audio_item( natus::audio::buffer_res_t&& obj_ ) : obj( std::move( obj_ ) ) {}
            virtual ~audio_item( void_t ) {}

            natus::audio::buffer_res_t obj ;
        };
        natus_res_typedef( audio_item ) ;

        struct glyph_atlas_item : public item
        {
            glyph_atlas_item( natus::font::glyph_atlas_rref_t obj_ ) : obj( std::move( obj_ ) ) {}
            virtual ~glyph_atlas_item( void_t ) {}
            
            natus::font::glyph_atlas_res_t obj ;
        };
        natus_res_typedef( glyph_atlas_item ) ;

        struct status_item : public item
        {
            natus::ntd::string_t msg ;

            status_item( natus::ntd::string_cref_t msg_ ) : msg( msg_ ) {}
        };
        natus_res_typedef( status_item ) ;
        typedef std::future< status_item_res_t > future_status_t ;
    }
}