#pragma once

#include <natus/core/macros/typedef.h>

namespace natus::application
{
    natus_class_proto_typedefs( system ) ;  

    natus_class_proto_typedefs( iapplication ) ;  
    natus_class_proto_typedefs( window_application ) ;  
    natus_class_proto_typedefs( iwindow ) ;
    natus_class_proto_typedefs( iwindow_handle ) ;
    natus_class_proto_typedefs( iwindow_listener ) ;
    natus_class_proto_typedefs( irender_window_listener ) ;
    natus_class_proto_typedefs( irender_context ) ;
    
    natus_class_proto_typedefs( render_window ) ;
    natus_class_proto_typedefs( decorator_window ) ;    


    natus_class_proto_typedefs( iwindow_message_listener ) ;

    natus_class_proto_typedefs( window_message_receiver ) ;
    
}
