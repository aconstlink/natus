#pragma once

#include "../api.h"
#include "../typedefs.h"

#include <natus/gpu/async.h>
#include <natus/gpu/variable/variable_set.hpp>
#include <natus/gpu/configuration/render_configuration.h>
#include <natus/gpu/configuration/geometry_configuration.h>

#include <natus/soil/macros.h>
#include <imgui/imgui.h>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API imgui
        {
            natus_this_typedefs( imgui ) ;

        private:

            ImGuiContext* _ctx ;

            natus::gpu::async_id_res_t _rc_id = natus::gpu::async_id_t() ;
            natus::gpu::async_id_res_t _sc_id = natus::gpu::async_id_t() ;
            natus::gpu::render_configuration_res_t _rc ;
            natus::gpu::shader_configuration_res_t _sc ;

            struct vertex 
            { 
                natus::math::vec2f_t pos ; 
                natus::math::vec2f_t uv ; 
                natus::math::vec4f_t color ; 
            } ;
            natus::gpu::async_id_res_t _gc_id = natus::gpu::async_id_t() ;
            natus::gpu::geometry_configuration_res_t _gc ;

            natus::gpu::variable_set_res_t _vars = natus::gpu::variable_set_t() ;

        public:

            imgui( void_t ) ;
            imgui( this_cref_t ) = delete ;
            imgui( this_rref_t ) ;
            ~imgui( void_t ) ;

            void_t init( natus::gpu::async_view_ref_t ) ;
            void_t render( natus::gpu::async_view_ref_t ) ;

            typedef ::std::function< void_t ( ImGuiContext* ) > exec_funk_t ;
            void_t execute( exec_funk_t ) ;

        public:

            struct window_data
            {
                int_t width ;
                int_t height ;
            };
            natus_typedef( window_data ) ;

            void_t change( window_data_cref_t ) ;

        private:

            void_t do_default_imgui_init( void_t ) ;
        };
        natus_soil_typedef( imgui ) ;
    }
}