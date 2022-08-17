#pragma once

#include <natus/math/matrix/matrix4.hpp>
#include <natus/math/vector/vector3.hpp>
#include <natus/math/vector/vector4.hpp>

namespace natus
{
    namespace math
    {
        namespace m3d
        {
            namespace detail
            {
                /// 
                template< typename T >
                struct orthographic_projection
                {
                    natus_this_typedefs( orthographic_projection<T> ) ;
                    natus_typedefs( T, type ) ;
                } ;
            }

            ///
            template< typename T >
            struct orthographic
            {
                natus_this_typedefs( orthographic<T> ) ;

                natus_typedefs( T, type ) ;
                natus_typedefs( natus::math::vector2<type_t>, vec2 ) ;
                natus_typedefs( natus::math::vector4<type_t>, vec4 ) ;
                natus_typedefs( natus::math::matrix4<type_t>, mat4 ) ;

                natus_typedefs( natus::math::m3d::detail::orthographic_projection<type_t>, proj ) ;

            private:

                static void __create( type_t const width, type_t const height,
                    type_t const n, type_t const f, mat4_out_t m )
                {
                    typedef natus::math::vector4< type_t > vec4_t ;

                    // make column vectors
                    // but they are the row vectors of
                    // the dx projection matrix.
                    vec4_t vcX( type_t( 2.0 ) / width, type_t( 0 ), type_t( 0 ), type_t( 0 ) ) ;
                    vec4_t vcY( type_t( 0 ), type_t( 2.0 ) / height, type_t( 0 ), type_t( 0 ) ) ;
                    vec4_t vcZ( type_t( 0 ), type_t( 0 ), type_t( 1.0 ) / ( f - n ), -( n ) / ( f - n ) ) ;
                    vec4_t vcW( type_t( 0 ), type_t( 0 ), type_t( 0 ), type_t( 1 ) ) ;

                    m.set_row( 0, vcX ) ;
                    m.set_row( 1, vcY ) ;
                    m.set_row( 2, vcZ ) ;
                    m.set_row( 3, vcW ) ;
                }

            public:

                static mat4_t create( type_t const width, type_t const height,
                    type_t const n, type_t const f )
                {
                    mat4_t m ;

                    this_t::__create( width, height, n, f, m ) ;

                    return m ;
                }
            } ;
        }
    }
}