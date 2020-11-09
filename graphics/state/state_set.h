#pragma once

#include "../api.h"
#include "../typedefs.h"

#include "render_states.h"
#include <natus/math/vector/vector4.hpp>

namespace natus
{
    namespace graphics
    {
        struct clear_state_set
        {
            bool_t do_change = false ;
            bool_t do_activate = false ;
            natus::math::vec4f_t clear_color = natus::math::vec4f_t( 0.0f ) ;
            bool_t do_color = false ;
            bool_t do_depth = false ;
        };

        struct blend_state_set
        {
            bool_t do_change = false ;
            bool_t do_activate = false ;
            blend_factor src_blend_factor = natus::graphics::blend_factor::one ;
            blend_factor dst_blend_factor = natus::graphics::blend_factor::zero ;
            blend_function blend_func = natus::graphics::blend_function::add ;

            // separate section
            /*
            natus::math::vec4f_t constant_blend_color = natus::math::vec4f_t( 0.0f ) ;

            bool_t blend_factor_separate = false ;
            
            blend_factor src_color_blend_factor = natus::graphics::blend_factor::one ;
            blend_factor dst_color_blend_factor = natus::graphics::blend_factor::zero ;
            blend_factor src_alpha_blend_factor = natus::graphics::blend_factor::one ;
            blend_factor dst_alpha_blend_factor = natus::graphics::blend_factor::zero ;


            bool_t blend_func_separate = false ;
            
            blend_function color_blend_func = natus::graphics::blend_function::add ;
            blend_function alpha_blend_func = natus::graphics::blend_function::add ;*/
        };

        struct depth_state_set
        {
            bool_t do_change = false ;
            bool_t do_activate = false ;
            bool_t do_depth_write = true ;

            // depth buffer test func
        };

        struct stencil_state_set
        {
            bool_t do_change = false ;
            // missing
        };

        struct scissor_state_set
        {
            bool_t do_change = false ;
            bool_t do_activate = false ;
            natus::math::vec4ui_t rect ;
        };

        struct polygon_state_set
        {
            bool_t do_change = false ;
            bool_t do_activate = false ;

            cull_mode cm = natus::graphics::cull_mode::back ;
            front_face ff = natus::graphics::front_face::clock_wise ;
            fill_mode fm = natus::graphics::fill_mode::fill ;
        };

        struct viewport_state_set
        {
            bool_t do_change = false ;
            bool_t do_activate = false ;

            // x, y, width, height
            natus::math::vec4ui_t vp ;
            
            // depth range
        };

        // something that can be turned on/off
        struct render_state_sets
        {
            natus_this_typedefs( render_state_sets ) ;

            clear_state_set clear_s ;
            viewport_state_set view_s ;
            blend_state_set blend_s ;
            depth_state_set depth_s ;
            stencil_state_set stencil_s ;
            polygon_state_set polygon_s ;
            scissor_state_set scissor_s ;

            render_state_sets( void_t ) 
            {
                clear_s.do_activate = false ;
                clear_s.do_color = false ;
                clear_s.do_depth = false ;
                clear_s.clear_color = natus::math::vec4f_t( 0.4f, 0.1f, 0.2f, 1.0f ) ;

                view_s.do_activate = false ;
                depth_s.do_activate = false ;
                depth_s.do_depth_write = false ;
                
                polygon_s.do_activate = false ;
                polygon_s.cm = natus::graphics::cull_mode::back ;
                polygon_s.ff = natus::graphics::front_face::clock_wise ;
                polygon_s.fm = natus::graphics::fill_mode::fill ;
            }

            render_state_sets( this_cref_t rhv ) noexcept
            {
                clear_s = rhv.clear_s ;
                view_s = rhv.view_s ;
                blend_s = rhv.blend_s ;
                depth_s = rhv.depth_s ;
                stencil_s = rhv.stencil_s ;
                polygon_s = rhv.polygon_s ;
                scissor_s = rhv.scissor_s ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                clear_s = rhv.clear_s ;
                view_s = rhv.view_s ;
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
