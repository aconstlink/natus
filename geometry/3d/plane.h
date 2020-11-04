
#pragma once

#include "../result.h"
#include "../typedefs.h"
#include "../protos.h"
#include "../api.h"

#include <natus/math/vector/vector2.hpp>
#include <natus/math/vector/vector3.hpp>

namespace natus
{
    namespace geometry
    {
        struct NATUS_GEOMETRY_API plane
        {
            natus_this_typedefs( plane ) ;

        private:

            struct data
            {
                floats_t positions ;
                floats_t normals ;
                more_floats_t texcoords ;

                /// quads per dim
                size_t qpd = 0 ;
                /// vertices per dim
                size_t vpd = 0 ;
                /// num vertices
                size_t nv = 0 ;
            };
            natus_typedef( data ) ;

        public:

            struct input_params
            {
                /// per dimension scale in object space!
                natus::math::vec2f_t scale = natus::math::vec2f_t( 1.0f );
                /// per dimension rotation - euler angles in object space!
                natus::math::vec3f_t euler = natus::math::vec3f_t( 0.0f );

                /// number of quads per side
                /// => vertices per side = tess + 1 
                size_t tess = 1 ;
            };
            natus_typedef( input_params ) ;

            static natus::geometry::result make( polygon_mesh_ptr_t, input_params_cref_t ) ;
            
        private:

            static void_t construct_vertices( data_ref_t, input_params_cref_t ) ;
            
        };
        natus_typedef( plane ) ;
    }
}

