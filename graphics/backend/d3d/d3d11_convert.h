
#pragma once

#include "../../types.h"
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

            static natus::graphics::type_struct to_type_struct( D3D_SHADER_VARIABLE_CLASS const cls, UINT const elems ) noexcept
            {
                switch( cls )
                {
                case D3D_SVC_SCALAR: return natus::graphics::type_struct::vec1 ;
                case D3D_SVC_VECTOR:
                {
                    if( elems == 2 ) return natus::graphics::type_struct::vec2 ;
                    else if( elems == 3 ) return natus::graphics::type_struct::vec3 ;
                    else if( elems == 4 ) return natus::graphics::type_struct::vec4 ;
                }
                case D3D_SVC_MATRIX_COLUMNS:
                case D3D_SVC_MATRIX_ROWS:
                {
                    if( elems == 2 ) return natus::graphics::type_struct::mat2 ;
                    else if( elems == 3 ) return natus::graphics::type_struct::mat3 ;
                    else if( elems == 4 ) return natus::graphics::type_struct::mat4 ;
                }
                }

                return natus::graphics::type_struct::undefined ;
            }

            static natus::graphics::type to_type( D3D_SHADER_VARIABLE_TYPE const t ) noexcept
            {
                switch( t )
                {
                case D3D_SVT_BOOL: return natus::graphics::type::tbool ;
                case D3D_SVT_INT: return natus::graphics::type::tint ;
                case D3D_SVT_MIN16INT: return natus::graphics::type::tshort ;
                case D3D_SVT_FLOAT: return natus::graphics::type::tfloat ;
                // half float not supported
                //case D3D_SVT_MIN16FLOAT: return natus::graphics::type::tfloat ;
                case D3D_SVT_UINT: return natus::graphics::type::tuint ;
                case D3D_SVT_MIN16UINT: return natus::graphics::type::tushort ;
                case D3D_SVT_DOUBLE: return natus::graphics::type::tdouble ;
                default: break;
                }

                return natus::graphics::type::undefined ;
            }

            static bool_t is_texture_type( D3D_SHADER_VARIABLE_TYPE const t ) noexcept
            {
                switch( t ) 
                {
                case D3D_SVT_TEXTURE: return true ;
                case D3D_SVT_TEXTURE1D: return true ;
                case D3D_SVT_TEXTURE2D: return true ;
                case D3D_SVT_TEXTURE3D: return true ;
                case D3D_SVT_TEXTURECUBE: return true ;
                case D3D_SVT_SAMPLER: return true ;
                case D3D_SVT_SAMPLER1D: return true ;
                case D3D_SVT_SAMPLER2D: return true ;
                case D3D_SVT_SAMPLER3D: return true ;
                case D3D_SVT_SAMPLERCUBE: return true ;
                case D3D_SVT_TEXTURE1DARRAY: return true ;
                case D3D_SVT_TEXTURE2DARRAY: return true ;
                case D3D_SVT_DEPTHSTENCIL: return true ;
                default:break ;
                }

                return false ;
            }

            static bool_t is_buffer_type( D3D_SHADER_VARIABLE_TYPE const t ) noexcept
            {
                switch( t )
                {
                case D3D_SVT_BUFFER: return true ;
                case D3D_SVT_CBUFFER: return true ;
                case D3D_SVT_TBUFFER: return true ;
                default:break ;
                }

                return false ;
            }

            static D3D11_FILTER convert( natus::graphics::texture_filter_type const ft_min, natus::graphics::texture_filter_type const ft_mag ) noexcept
            {
                switch( ft_min )
                {
                case natus::graphics::texture_filter_type::linear: 
                    switch( ft_mag )
                    {
                    case natus::graphics::texture_filter_type::linear:
                        return D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR ;
                    case natus::graphics::texture_filter_type::nearest:
                        return D3D11_FILTER::D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR ;
                    default: break ;
                    }
                case natus::graphics::texture_filter_type::nearest:
                    switch( ft_mag )
                    {
                    case natus::graphics::texture_filter_type::linear:
                        return D3D11_FILTER::D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR ;
                    case natus::graphics::texture_filter_type::nearest:
                        return D3D11_FILTER::D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR ;
                    default: break ;
                    }
                default: break ;
                }
                return D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT ;
            }

            static D3D11_TEXTURE_ADDRESS_MODE convert( natus::graphics::texture_wrap_type const wt ) noexcept
            {
                switch( wt ) 
                {
                case natus::graphics::texture_wrap_type::repeat:
                    return D3D11_TEXTURE_ADDRESS_WRAP ;
                case natus::graphics::texture_wrap_type::clamp:
                    return D3D11_TEXTURE_ADDRESS_CLAMP ;
                case natus::graphics::texture_wrap_type::clamp_border:
                case natus::graphics::texture_wrap_type::clamp_edge:
                    return D3D11_TEXTURE_ADDRESS_BORDER ;
                case natus::graphics::texture_wrap_type::mirror :
                    return D3D11_TEXTURE_ADDRESS_MIRROR ;
                default: break ;
                }
                return D3D11_TEXTURE_ADDRESS_WRAP ; ;
            }

            static DXGI_FORMAT convert( natus::graphics::image_format const fmt, 
                natus::graphics::image_element_type const iet ) noexcept
            {
                switch( fmt ) 
                {
                case natus::graphics::image_format::rgb: 
                    switch( iet )
                    {
                    case natus::graphics::image_element_type::float32:
                        return DXGI_FORMAT_R32G32B32_FLOAT ;
                    case natus::graphics::image_element_type::int32:
                        return DXGI_FORMAT_R32G32B32_SINT ;
                    case natus::graphics::image_element_type::uint32:
                        return DXGI_FORMAT_R32G32B32_UINT ;
                    case natus::graphics::image_element_type::uint8:
                        return DXGI_FORMAT_R8G8B8A8_UNORM ;
                    default:
                        break;
                    }
                case natus::graphics::image_format::rgba: 
                    switch( iet )
                    {
                    case natus::graphics::image_element_type::float32:
                        return DXGI_FORMAT_R32G32B32A32_FLOAT ;
                    case natus::graphics::image_element_type::int32:
                        return DXGI_FORMAT_R32G32B32A32_SINT ;
                    case natus::graphics::image_element_type::uint32:
                        return DXGI_FORMAT_R32G32B32A32_UINT ;
                    case natus::graphics::image_element_type::uint8:
                        return DXGI_FORMAT_R8G8B8A8_UNORM ;
                    default:
                        break;
                    }
                case natus::graphics::image_format::depth: 
                    switch( iet )
                    {
                    case natus::graphics::image_element_type::float32:
                        return DXGI_FORMAT_D32_FLOAT ;
                    default:
                        break;
                    }
                case natus::graphics::image_format::intensity: 
                    switch( iet )
                    {
                    case natus::graphics::image_element_type::float32:
                        return DXGI_FORMAT_R32_FLOAT ;
                    case natus::graphics::image_element_type::int32:
                        return DXGI_FORMAT_R32_SINT ;
                    case natus::graphics::image_element_type::uint32:
                        return DXGI_FORMAT_R32_UINT ;
                    case natus::graphics::image_element_type::uint8:
                        return DXGI_FORMAT_R8_UNORM ;
                    default:
                        break;
                    }
                }

                return DXGI_FORMAT_UNKNOWN ;
            }
        }
    }
}