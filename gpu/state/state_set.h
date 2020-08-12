#pragma once

#include "../api.h"
#include "../typedefs.h"

#include "render_states.h"
#include <natus/math/vector/vector4.hpp>

namespace natus
{
    namespace gpu
    {
        struct color_state_set
        {
            natus::math::vec4f_t clear_color = natus::math::vec4f_t( 0.0f ) ;
        };

        struct blend_state_set
        {
            // blend active
            bool_t do_blend = false ;
            blend_factor src_blend_factor = natus::gpu::blend_factor::one ;
            blend_factor dst_blend_factor = natus::gpu::blend_factor::zero ;
            blend_function blend_func = natus::gpu::blend_function::add ;

            // separate section
            /*
            natus::math::vec4f_t constant_blend_color = natus::math::vec4f_t( 0.0f ) ;

            bool_t blend_factor_separate = false ;
            
            blend_factor src_color_blend_factor = natus::gpu::blend_factor::one ;
            blend_factor dst_color_blend_factor = natus::gpu::blend_factor::zero ;
            blend_factor src_alpha_blend_factor = natus::gpu::blend_factor::one ;
            blend_factor dst_alpha_blend_factor = natus::gpu::blend_factor::zero ;


            bool_t blend_func_separate = false ;
            
            blend_function color_blend_func = natus::gpu::blend_function::add ;
            blend_function alpha_blend_func = natus::gpu::blend_function::add ;*/
        };

        struct depth_state_set
        {
            bool_t do_depth_test = false ;
            bool_t do_depth_write = false ;

            // depth buffer test func
        };

        struct stencil_state_set
        {
            // missing
        };

        struct scissor_state_set
        {
            bool_t do_scissor_test = false ;
            natus::math::vec4ui_t rect ;
        };

        struct polygon_state_set
        {
            bool_t do_culling = false ;

            cull_mode cm = natus::gpu::cull_mode::back ;
            front_face ff = natus::gpu::front_face::clock_wise ;

            // fill mode
            cull_mode cm_fill = natus::gpu::cull_mode::front_back ;
            fill_mode fm = natus::gpu::fill_mode::fill ;
        };

        struct viewport_state_set
        {
            //natus::gpu::viewport_2d vp ;
            // depth range
        };

        // something that needs to be tracked and reset
        // e.g. you can not simply turn on/off the viewport
        // it needs to be reset to some previous value
        struct view_state_sets
        {
            natus_this_typedefs( view_state_sets ) ;

            color_state_set color_s ;
            viewport_state_set viewport_s ;

            this_ref_t operator = ( this_cref_t rhv )
            {
                color_s = rhv.color_s ;
                viewport_s = rhv.viewport_s ;

                return *this ;
            }
        };
        natus_res_typedef( view_state_sets ) ;

        // something that can be turned on/off
        struct render_state_sets
        {
            natus_this_typedefs( render_state_sets ) ;

            blend_state_set blend_s ;
            depth_state_set depth_s ;
            stencil_state_set stencil_s ;
            polygon_state_set polygon_s ;
            scissor_state_set scissor_s ;

            render_state_sets( void_t ) {}
            render_state_sets( this_cref_t rhv ) noexcept
            {
                blend_s = rhv.blend_s ;
                depth_s = rhv.depth_s ;
                stencil_s = rhv.stencil_s ;
                polygon_s = rhv.polygon_s ;
                scissor_s = rhv.scissor_s ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                blend_s = rhv.blend_s ;
                depth_s = rhv.depth_s ;
                stencil_s = rhv.stencil_s ;
                polygon_s = rhv.polygon_s ;
                scissor_s = rhv.scissor_s ;

                return *this ;
            }
        };
        natus_res_typedef( render_state_sets ) ;
    }
}
