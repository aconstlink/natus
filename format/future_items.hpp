#pragma once

#include "future_item.hpp"

#include <natus/audio/object/buffer_object.h>
#include <natus/graphics/texture/image.hpp>
#include <natus/font/glyph_atlas.h>

#include <future>

namespace natus
{
    namespace format
    {
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
    }
}