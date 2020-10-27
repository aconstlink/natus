
#pragma once

#include "d3d11_context.h"
#include "../backend.h"

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API d3d11_backend : public natus::graphics::backend
        {
            natus_this_typedefs( d3d11_backend ) ;

        private:

            struct pimpl ;
            pimpl* _pimpl ;

            natus::graphics::d3d11_context_ptr_t _context = nullptr ;

        public:

            d3d11_backend( natus::graphics::d3d11_context_ptr_t ) noexcept ;
            d3d11_backend( this_cref_t ) = delete ;
            d3d11_backend( this_rref_t ) noexcept ;
            virtual ~d3d11_backend( void_t ) ;

        public:

            virtual void_t set_window_info( window_info_cref_t ) noexcept ;

            virtual natus::graphics::result configure( natus::graphics::geometry_object_res_t ) noexcept override ;
            virtual natus::graphics::result configure( natus::graphics::render_object_res_t ) noexcept override ;
            virtual natus::graphics::result configure( natus::graphics::shader_object_res_t ) noexcept override ;
            virtual natus::graphics::result configure( natus::graphics::image_object_res_t ) noexcept override ;

            virtual natus::graphics::result connect( natus::graphics::render_object_res_t, natus::graphics::variable_set_res_t ) noexcept ;
            virtual natus::graphics::result update( natus::graphics::geometry_object_res_t ) noexcept ;

            virtual natus::graphics::result render( natus::graphics::render_object_res_t, natus::graphics::backend::render_detail_cref_t ) noexcept override ;

            virtual void_t render_begin( void_t ) noexcept ;
            virtual void_t render_end( void_t ) noexcept ;

        private:

            //
        };
        natus_res_typedef( d3d11_backend ) ;
    }
}