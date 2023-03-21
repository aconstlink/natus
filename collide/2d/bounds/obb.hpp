#pragma once

#include "../../typedefs.h"

#include "aabb.hpp"
#include <natus/math/vector/vector2.hpp>
#include <natus/math/vector/vector3.hpp>
#include <natus/math/matrix/matrix2.hpp>
#include <natus/math/matrix/matrix2x3.hpp>
#include <natus/math/utility/2d/transformation.hpp>

namespace natus
{
    namespace collide
    {
        namespace n2d
        {
            template< typename type_t >
            class obb
            {
                natus_this_typedefs( obb< type_t > ) ;

                natus_typedefs( natus::math::vector2< type_t >, vec2 ) ;
                natus_typedefs( natus::math::vector3< type_t >, vec3 ) ;
                natus_typedefs( natus::math::matrix2<type_t>, mat2 ) ;
                natus_typedefs( natus::math::matrix2x3<type_t>, mat2x3 ) ;
                natus_typedefs( natus::math::m2d::transformation< type_t >, trafo ) ;

                natus_typedefs( natus::collide::n2d::aabb< type_t >, aabb ) ;

            private:

                // the origin
                vec2_t _o ;

                // contains the x basis vector 
                // with -distance from origin
                vec3_t _x ;

                // contains the y basis vector 
                // with -distance from origin
                vec3_t _y ;

            public:

                obb( void_t ) noexcept {}
                obb( vec2_cref_t orig, vec2_cref_t x, vec2_cref_t y, vec2_cref_t ext ) noexcept :
                    _o(orig), _x(vec3_t(x,ext.x())), _y(vec3_t(y,ext.y())){}

                obb( aabb_cref_t b ) noexcept
                {
                    auto const to_max = b.get_max() - b.get_center() ;
                    auto const x = vec2_t( type_t(1), type_t(0) ) ;
                    auto const y = vec2_t( type_t(0), type_t(1) ) ;

                    _o = b.get_center() ;
                    _x = vec3_t( x, x.dot(to_max) ) ;
                    _y = vec3_t( y, y.dot(to_max) ) ;
                }

                obb( this_cref_t rhv ) noexcept : _o(rhv._o), _x(rhv._x), _y(rhv._y) {}
                ~obb( void_t ) noexcept {}

            public:

                // returns a transformation that allows to
                // transform a reference point to the local obb
                // coordinate system. This requires the inverse 
                // of the 2x3 transformation matrix.
                trafo_t ref_to_local( void_t ) const noexcept 
                {
                    auto const mt = mat2_t::from_rows( _x, _y ) ;
                    return trafo_t( mat2x3_t( mt, mt * -_o ) ) ;
                }

                // returns a transformation that allows to 
                // transform a point from the local frame
                // to the reference frame.
                trafo_t local_to_ref( void_t ) const noexcept
                {
                    return trafo_t( mat2x3_t( _x, _y, _o ) ) ;
                }

                // transform the given point p to the local 
                // coordinate system
                vec2_t ref_to_local( vec2_cref_t p ) const noexcept 
                {
                    return this_t::ref_to_local() * p ;
                }

                // transform the given point p from the local
                // to the reference coordinate system
                vec2_t local_to_ref_point( vec2_cref_t p ) const noexcept
                {
                    return this_t::local_to_ref() * vec3_t( p, type_t(1) ) ;
                }

                vec2_t local_to_ref_direction( vec2_cref_t p ) const noexcept
                {
                    return this_t::local_to_ref() * p ;
                }

            public:

                // relative translation
                this_ref_t translate_by( vec2_cref_t rel ) noexcept
                {
                    _o += rel ;
                    return *this ;
                }

                // absolute translation
                this_ref_t translate_to( vec2_cref_t position ) noexcept
                {
                    _o = position ;
                    return *this ;
                }

                // applys t to the current transformation
                // so that this is further translated and rotated
                this_ref_t transform_by( trafo_cref_t t ) noexcept
                {
                    _o += t.get_origin() ;
                    _x = vec3_t( t * _x.xy(), _x.z() ) ;
                    _y = vec3_t( t * _y.xy(), _y.z() ) ;
                    return *this ;
                }

            public:

                vec2_cref_t origin( void_t ) const noexcept{ return _o ; }
                vec3_cref_t x_axis( void_t ) const noexcept{ return _x ; }
                vec3_cref_t y_axis( void_t ) const noexcept{ return _y ; }

                vec2_t get_extend( void_t ) const noexcept { return vec2_t( _x.z(), _y.z() ) ; }

            public:

                // left bottom
                // left top
                // right top
                // right bottom
                std::array< vec2_t, 4 > get_points( void_t ) const noexcept
                {
                    auto const x = vec2_t( _x.z(), type_t(0) ) ;
                    auto const y = vec2_t( type_t(0), _y.z() ) ;

                    auto const ext = x + y ;

                    auto const min = -(ext) ;
                    auto const max = +(ext) ;

                    auto const trafo = this_t::local_to_ref() ;

                    return 
                    {
                        trafo * vec3_t( min.x(), min.y(), type_t(1) ),
                        trafo * vec3_t( min.x(), max.y(), type_t(1) ),
                        trafo * vec3_t( max.x(), max.y(), type_t(1) ),
                        trafo * vec3_t( max.x(), min.y(), type_t(1) )
                    } ;
                }

            } ;
            natus_typedefs( obb< float_t >, obbf ) ;
        }
    }
}