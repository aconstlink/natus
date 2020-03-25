#pragma once

#include "../../typedefs.h"

#include <natus/core/types.hpp>
#include <pthread.h>

namespace natus
{
    namespace concurrent
    {
        namespace osunix
        {
            using namespace natus::core::types ;

            class pthread_helper
            {
            public:

                static void_t set_thread_name( std::string const& /*name*/ )
                {
                    //@todo: what is the correct way to set the thread's name
                    //pthread_setname_np(tptr->native_handle(), name.c_str()) ;
                }

                /// set caller thread's name 
                static void_t set_thread_name( natus::concurrent::thread_ref_t tptr, std::string const& /*name*/ )
                {
                    //if( tptr == nullptr ) return ;
                    //@todo: what is the correct way to set the thread's name
                    //pthread_setname_np(tptr->native_handle(), name.c_str()) ;
                }
            } ;
        }
    }
}