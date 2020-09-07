#pragma once

#include "typedefs.h"

#include <natus/graphics/texture/image.hpp>
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

        struct error_item : public item
        {
            // error code or message
            natus::ntd::string_t msg ;

            error_item( natus::ntd::string_cref_t msg_ ) : msg( msg_ ){}
        };
        natus_res_typedef( error_item ) ;
        typedef std::future< error_item_res_t > future_error_t ;

    }
}