
#pragma once

#include "typedefs.h"
#include <natus/soil/macros.h>

namespace natus
{
    namespace gpu
    {
        natus_class_proto_typedefs( render_config ) ;
        typedef natus::soil::rptr< render_config_ptr_t > render_config_rptr_t ;

        natus_class_proto_typedefs( vertex_shader ) ;
        typedef natus::soil::rptr< vertex_shader_ptr_t > vertex_shader_rptr_t ;

        natus_class_proto_typedefs( geometry_shader ) ;
        typedef natus::soil::rptr< geometry_shader_ptr_t > geometry_shader_rptr_t ;

        natus_class_proto_typedefs( pixel_shader ) ;
        typedef natus::soil::rptr< pixel_shader_ptr_t > pixel_shader_rptr_t ;

        natus_class_proto_typedefs( render_configuration ) ;
        typedef natus::soil::rptr< render_configuration_ptr_t > render_configuration_rptr_t ;

        natus_class_proto_typedefs( backend ) ;
        typedef natus::soil::rptr< backend_ptr_t > backend_rptr_t ;

        natus_soil_class_proto_typedefs( async ) ;
        natus_soil_class_proto_typedefs( async_id ) ;
    }
}