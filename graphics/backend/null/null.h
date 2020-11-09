

#pragma once 

#include "../backend.h"

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API null_backend : public natus::graphics::backend
        {
            natus_this_typedefs( null_backend ) ;

        public:

            null_backend( void_t ) ;
            null_backend( this_cref_t ) = delete ;
            null_backend( this_rref_t ) ;
            virtual ~null_backend( void_t ) ;

        public:

            virtual void_t set_window_info( window_info_cref_t ) noexcept ;

            virtual natus::graphics::result configure( natus::graphics::geometry_object_res_t ) noexcept override ;
            virtual natus::graphics::result configure( natus::graphics::render_object_res_t ) noexcept override ;
            virtual natus::graphics::result configure( natus::graphics::shader_object_res_t ) noexcept override ;
            virtual natus::graphics::result configure( natus::graphics::image_object_res_t ) noexcept override ;
            virtual natus::graphics::result configure( natus::graphics::framebuffer_object_res_t ) noexcept ;
            virtual natus::graphics::result configure( natus::graphics::state_object_res_t ) noexcept ;

            virtual natus::graphics::result connect( natus::graphics::render_object_res_t, natus::graphics::variable_set_res_t ) noexcept ;
            virtual natus::graphics::result update( natus::graphics::geometry_object_res_t ) noexcept ;

            virtual natus::graphics::result use( natus::graphics::framebuffer_object_res_t ) noexcept ;
            virtual natus::graphics::result use( natus::graphics::state_object_res_t, size_t const, bool_t const ) noexcept ;

            virtual natus::graphics::result render( natus::graphics::render_object_res_t, natus::graphics::backend::render_detail_cref_t ) noexcept override ;

            virtual void_t render_begin( void_t ) noexcept ;
            virtual void_t render_end( void_t ) noexcept ;
        };
        natus_res_typedef( null_backend ) ;
    }
}