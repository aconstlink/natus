//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _SNAKEOIL_THREAD_PTHRED_HELPER_H_
#define _SNAKEOIL_THREAD_PTHRED_HELPER_H_

#include <snakeoil/core/types.hpp>
#include <pthread.h>

namespace so_thread
{
    namespace so_unix
    {
        namespace so_pthread
        {
            using namespace so_core::so_types ;

            class helper
            {
            public:

                static void_t set_thread_name( std::string const & /*name*/ )
                {
                    //@todo: what is the correct way to set the thread's name
                    //pthread_setname_np(tptr->native_handle(), name.c_str()) ;
                }

                /// set caller thread's name 
                static void_t set_thread_name( so_thread::thread_ptr_t tptr, std::string const & /*name*/ )
                {
                    if( tptr == nullptr ) return ;
                    //@todo: what is the correct way to set the thread's name
                    //pthread_setname_np(tptr->native_handle(), name.c_str()) ;
                }
                
            } ;
        }

    }
}

#endif

