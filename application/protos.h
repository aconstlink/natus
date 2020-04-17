#pragma once

#include <natus/core/macros/typedef.h>
#include <natus/soil/rptr.hpp>

namespace natus
{
    namespace application
    {
        natus_class_proto_typedefs( global ) ;

        natus_class_proto_typedefs( iapplication ) ;
        natus_class_proto_typedefs( window_application ) ;
        natus_class_proto_typedefs( iwindow ) ;
        typedef natus::soil::rptr< natus::application::iwindow_ptr_t > iwindow_rptr_t ;

        natus_class_proto_typedefs( iwindow_handle ) ;
        typedef natus::soil::rptr< natus::application::iwindow_handle_ptr_t > iwindow_handle_rptr_t ;

        natus_class_proto_typedefs( iwindow_listener ) ;
        typedef natus::soil::rptr< natus::application::iwindow_listener_ptr_t > iwindow_listener_rptr_t ;

        natus_class_proto_typedefs( irender_window_listener ) ;
        natus_class_proto_typedefs( irender_context ) ;

        natus_class_proto_typedefs( render_window ) ;
        natus_class_proto_typedefs( decorator_window ) ;


        natus_class_proto_typedefs( iwindow_message_listener ) ;
        typedef natus::soil::rptr< natus::application::iwindow_message_listener_ptr_t > iwindow_message_listener_rptr_t ;

        natus_class_proto_typedefs( window_message_receiver ) ;

        natus_class_proto_typedefs( app ) ;
        typedef natus::soil::rptr< app_ptr_t > app_rptr_t ;
    }
}
