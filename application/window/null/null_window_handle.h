#pragma once
#include "../iwindow_handle.h"

namespace natus
{
    namespace application
    {
        namespace null
        {
            class NATUS_APPLICATION_API null_window_handle : public iwindow_handle
            {
                natus_this_typedefs( null_window_handle ) ;

            public:

                null_window_handle( void_t ) ;
                null_window_handle( this_rref_t rhv ) ;
                null_window_handle( this_cref_t rhv ) = delete ;
                virtual ~null_window_handle( void_t ) ;

            public:

                this_ref_t operator = ( this_rref_t ) ;

            };
            natus_typedef( null_window_handle ) ;
            typedef natus::soil::rptr< null_window_handle_ptr_t > null_window_handle_rptr_t ;
        }
    }
}