#pragma once

#include "../../typedefs.h"

#include "../../vector/vector2.hpp"
#include "../../vector/vector3.hpp"
#include "../../matrix/matrix3.hpp"

namespace natus
{
    namespace math
    {
        namespace m2d
        {
            template< typename type_t >
            class transformation
            {
            public:

                typedef transformation< type_t > this_t ;
                typedef this_t& this_ref_t ;
                typedef this_t const& this_cref_t ;

                typedef vector2<type_t> vec2_t ;
                typedef vec2_t const& vec2_cref_t ;

                typedef vector3<type_t> vec3_t ;
                typedef vec3_t const& vec3_cref_t ;

                typedef matrix3<type_t> mat3_t ;
                typedef mat3_t const& mat3_cref_t ;

            private:

                // contains rotation and position
                mat3_t _trafo ;

            public:

                transformation( void_t ) : _trafo( natus::math::with_identity() ) {}
                transformation( this_cref_t rhv ) { *this = rhv ; }

                transformation( vec2_cref_t pos ) : _trafo( natus::math::with_identity() )
                {
                    this_t::translate_by( pos ) ;
                }

            public:

                mat3_cref_t get_transformation( void_t ) const { return _trafo ; }

                vec2_t get_translation( void_t ) const { return _trafo.get_column( 2 ).xy() ; }

            public:

                /// 2d rotate this transformation
                this_ref_t rotate_by( type_t angle )
                {
                    // rotation matrix
                    mat3_t rot( natus::math::with_identity() ) ;

                    _trafo = rot * _trafo ;

                    return *this ;
                }

                /// translate this transformation
                this_ref_t translate_by( vec2_cref_t by )
                {
                    _trafo = mat3_t( natus::math::with_identity() )
                        .set_column( 2, vec3_t( by, type_t( 1 ) ) ) * _trafo ;

                    return *this ;
                }

                /// transform this transformation by another transformation
                this_ref_t transform_by( this_cref_t by )
                {
                    _trafo = by._trafo * _trafo ;
                    return *this ;
                }

            public:

                this_ref_t operator = ( this_cref_t rhv )
                {
                    _trafo = rhv._trafo ;
                    return *this ;
                }
            };
        }
    }
    natus_typedefs( natus::math::m2d::transformation< float_t >, trafo2df ) ;
}