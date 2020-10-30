
#pragma once

#include "../../buffer/vertex_attribute.h"

namespace natus
{
    namespace graphics
    {
        namespace d3d11
        {
            static natus::ntd::string_cref_t vertex_binding_to_semantic( natus::graphics::vertex_attribute const va ) noexcept
            {
                static natus::ntd::string_t const _semantics[] = { 
                    "INVALID", "POSITION", "NORMAL", "TANGENT", 
                    "COLOR", "COLOR1", "COLOR2", "COLOR3", "COLOR4", "COLOR5", 
                    "TEXCOORD","TEXCOORD1", "TEXCOORD2", "TEXCOORD3", 
                    "TEXCOORD4", "TEXCOORD5", "TEXCOORD6", "TEXCOORD7"} ;

                switch( va )
                {
                case natus::graphics::vertex_attribute::position: return _semantics[1] ;
                case natus::graphics::vertex_attribute::normal: return _semantics[2] ;
                case natus::graphics::vertex_attribute::tangent: return _semantics[3] ;
                case natus::graphics::vertex_attribute::color0: return _semantics[4] ;
                case natus::graphics::vertex_attribute::color1: return _semantics[5] ;
                case natus::graphics::vertex_attribute::color2: return _semantics[6] ;
                case natus::graphics::vertex_attribute::color3: return _semantics[7] ;
                case natus::graphics::vertex_attribute::color4: return _semantics[8] ;
                case natus::graphics::vertex_attribute::color5: return _semantics[9] ;
                case natus::graphics::vertex_attribute::texcoord0: return _semantics[10] ;
                case natus::graphics::vertex_attribute::texcoord1: return _semantics[11] ;
                case natus::graphics::vertex_attribute::texcoord2: return _semantics[12] ;
                case natus::graphics::vertex_attribute::texcoord3: return _semantics[13] ;
                case natus::graphics::vertex_attribute::texcoord4: return _semantics[14] ;
                case natus::graphics::vertex_attribute::texcoord5: return _semantics[15] ;
                case natus::graphics::vertex_attribute::texcoord6: return _semantics[16] ;
                case natus::graphics::vertex_attribute::texcoord7: return _semantics[17] ;
                default: break ;
                }

                return _semantics[ 0 ] ;
            }

            static DXGI_FORMAT convert_type_to_vec_format( natus::graphics::type const t, natus::graphics::type_struct const ts ) noexcept
            {
                if( ts == natus::graphics::type_struct::vec1 )
                {
                    if( t == natus::graphics::type::tfloat )
                    {
                        return DXGI_FORMAT_R32_FLOAT ;
                    }
                    else if( t == natus::graphics::type::tdouble )
                    {
                        return DXGI_FORMAT_R32_FLOAT ;
                    }
                    else if( t == natus::graphics::type::tuint )
                    {
                        return DXGI_FORMAT_R32_UINT ;
                    }
                    else if( t == natus::graphics::type::tint )
                    {
                        return DXGI_FORMAT_R32_SINT ;
                    }
                }
                else if( ts == natus::graphics::type_struct::vec2 )
                {
                    if( t == natus::graphics::type::tfloat )
                    {
                        return DXGI_FORMAT_R32G32_FLOAT ;
                    }
                    else if( t == natus::graphics::type::tdouble )
                    {
                        return DXGI_FORMAT_R32G32_FLOAT ;
                    }
                    else if( t == natus::graphics::type::tuint )
                    {
                        return DXGI_FORMAT_R32G32_UINT ;
                    }
                    else if( t == natus::graphics::type::tint )
                    {
                        return DXGI_FORMAT_R32G32_SINT ;
                    }
                }
                else if( ts == natus::graphics::type_struct::vec3 )
                {

                    if( t == natus::graphics::type::tfloat )
                    {
                        return DXGI_FORMAT_R32G32B32_FLOAT ;
                    }
                    else if( t == natus::graphics::type::tdouble )
                    {
                        return DXGI_FORMAT_R32G32B32_FLOAT ;
                    }
                    else if( t == natus::graphics::type::tuint )
                    {
                        return DXGI_FORMAT_R32G32B32_UINT ;
                    }
                    else if( t == natus::graphics::type::tint )
                    {
                        return DXGI_FORMAT_R32G32B32_SINT ;
                    }
                }
                else if( ts == natus::graphics::type_struct::vec4 )
                {
                    if( t == natus::graphics::type::tfloat )
                    {
                        return DXGI_FORMAT_R32G32B32A32_FLOAT ;
                    }
                    else if( t == natus::graphics::type::tdouble )
                    {
                        return DXGI_FORMAT_R32G32B32A32_FLOAT ;
                    }
                    else if( t == natus::graphics::type::tuint )
                    {
                        return DXGI_FORMAT_R32G32B32A32_UINT ;
                    }
                    else if( t == natus::graphics::type::tint )
                    {
                        return DXGI_FORMAT_R32G32B32A32_SINT ;
                    }
                }

                return DXGI_FORMAT_UNKNOWN ;
            }

        }
    }
}