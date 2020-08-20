#pragma once

#include "../protos.h"
#include "../api.h"
#include "../typedefs.h"
#include "../result.h"

#include "../container/vector.hpp"

#include <vector>

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API itask
        {
            natus_this_typedefs( itask ) ;

        public:

            natus_typedefs( natus::ntd::vector<itask_ptr_t>, tasks ) ;
            natus_typedefs( natus::concurrent::vector<itask_ptr_t>, locked_tasks ) ;

        protected:

            struct connection_accessor
            {
                static size_t connect( itask_ptr_t from, itask_ptr_t to )
                {
                    return to->connect_incoming( from ) ;
                }

                static size_t disconnect( itask_ptr_t from, itask_ptr_t to )
                {
                    return to->disconnect_incoming( from ) ;
                }
            };

        public: // dependencies

            virtual itask_ptr_t then( itask_ptr_t ) = 0 ;

        public: // run-time

            virtual void_t execute( locked_tasks_ref_t ) = 0 ;
            virtual void_t destroy( void_t ) = 0 ;

        protected:

            virtual void_t run( void_t ) = 0 ;

        private:

            virtual size_t connect_incoming( itask_ptr_t ) = 0 ;
            virtual size_t disconnect_incoming( itask_ptr_t ) = 0 ;
        };

        natus_typedef( itask ) ;
    }
}
