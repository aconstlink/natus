#pragma once

#include <natus/soil/macros.h>
#include <natus/core/macros/typedef.h>

namespace natus
{
    namespace application
    {
        natus_class_proto_typedefs( global ) ;

        natus_class_proto_typedefs( iapplication ) ;
        natus_class_proto_typedefs( window_application ) ;
        natus_class_proto_typedefs( iwindow ) ;
        natus_class_proto_typedefs( iwindow_handle ) ;
        

        natus_class_proto_typedefs( platform_window ) ;
        natus_class_proto_typedefs( platform_application ) ;
        natus_class_proto_typedefs( iwindow_message_listener ) ;
        natus_class_proto_typedefs( iwindow_message_listener ) ;
        natus_class_proto_typedefs( window_message_receiver ) ;
        natus_class_proto_typedefs( app ) ;
        natus_class_proto_typedefs( gfx_context ) ;
    }
}
