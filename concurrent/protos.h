#pragma once

#include <natus/core/macros/typedef.h>

namespace natus
{
    namespace concurrent
    {
        natus_class_proto_typedefs( itask_scheduler ) ;
        natus_class_proto_typedefs( task_scheduler ) ;
        natus_class_proto_typedefs( job_scheduler ) ;
        natus_class_proto_typedefs( itask ) ;
        natus_class_proto_typedefs( task_graph ) ;

        natus_class_proto_typedefs( worker_thread ) ;
        natus_class_proto_typedefs( sync_object ) ;

        natus_class_proto_typedefs( ithread_task_scheduler ) ;
        natus_class_proto_typedefs( thread_task_scheduler ) ;
        natus_class_proto_typedefs( iserial_task_scheduler ) ;
        natus_class_proto_typedefs( serial_task_scheduler ) ;

        natus_class_proto_typedefs( ijob ) ;
    }
}
