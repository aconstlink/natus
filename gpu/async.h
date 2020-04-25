
#pragma once

#include "async_id.hpp"
#include "backend/backend.h"

#include <natus/concurrent/typedefs.h>
#include <natus/std/vector.hpp>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API async
        {
            natus_this_typedefs( async ) ;

        private:

            /// decorated backend.
            backend_res_t _backend ;

            struct prepare_data
            {
                natus::gpu::async_id_res_t aid = natus::gpu::async_id_t() ;
                natus::gpu::render_configurations_t config ;
            };
            typedef natus::std::vector< prepare_data > prepares_t ;
            prepares_t _prepares ;
            natus::concurrent::mutex_t _preps_mtx ;
            
            struct render_data
            {
                natus::gpu::async_id_res_t aid ;
            };
            typedef natus::std::vector< render_data > renders_t ;
            renders_t _renders ;
            natus::concurrent::mutex_t _renders_mtx ;


        private: // sync

            natus::concurrent::mutex_t _frame_mtx ;
            natus::concurrent::condition_variable_t _frame_cv ;
            bool_t _frame_ready = false ;
            bool_t _ready = true ;

        public:

            async( void_t ) ;
            async( backend_res_t ) ;
            async( this_cref_t ) = delete ;
            async( this_rref_t ) ;
            ~async( void_t ) ;

        public:

            virtual natus::gpu::result prepare( natus::gpu::async_id_res_t, natus::gpu::render_configurations_in_t ) noexcept ;

            virtual natus::gpu::result render( natus::gpu::async_id_res_t ) noexcept ;

        public:

            /// render thread update function - DO NOT USE.
            void_t system_update( void_t ) noexcept ;

        public: // sync

            bool_t enter_frame( void_t ) ;
            void_t leave_frame( void_t ) ;
            void_t wait_for_frame( void_t ) ;
            void_t set_ready( void_t ) ;
        };
        natus_soil_typedef( async ) ;
    }
}