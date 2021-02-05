
#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../future_item.hpp"

#include <natus/ntd/vector.hpp>
#include <natus/math/vector/vector2.hpp>
#include <natus/math/vector/vector4.hpp>

namespace natus
{
    namespace format
    {
        struct natus_document
        {
            struct sprite_sheet
            {
                struct image
                {
                    natus::ntd::string_t src ;
                };
                natus_typedef( image ) ;
                image_t image ;

                struct sprite
                {
                    struct hit
                    {
                        natus::ntd::string_t name ;
                        natus::math::vec4ui_t rect ;
                    };
                    natus_typedef( hit ) ;

                    struct animation
                    {
                        natus::math::vec4ui_t rect ;
                        natus::math::vec2i_t pivot ;
                    };
                    natus_typedef( animation ) ;

                    struct collision
                    {
                        natus::math::vec4ui_t rect ;
                    };
                    natus_typedef( collision ) ;
                    
                    natus::ntd::string_t name ;
                    
                    animation_t animation ;
                    collision_t collision ;
                    natus::ntd::vector< hit_t > hits ;
                };
                natus_typedef( sprite ) ;
                natus::ntd::vector< sprite_t > sprites ;

                struct animation
                {
                    struct frame
                    {
                        natus::ntd::string_t sprite ;
                        size_t duration ;
                    };
                    natus_typedef( frame ) ;

                    natus::ntd::string_t name ;
                    natus::ntd::vector< frame_t > frames ;
                };
                natus_typedef( animation ) ;
                natus::ntd::vector< animation_t > animations ;

                natus::ntd::string_t name ;
            };
            natus_typedef( sprite_sheet ) ;

            natus::ntd::vector< sprite_sheet_t > sprite_sheets ;
        };
        natus_typedef( natus_document ) ;
    }
}