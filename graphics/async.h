
#pragma once

#include "async_id.hpp"
#include "backend/backend.h"

#include <natus/concurrent/typedefs.h>
#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API async
        {
            natus_this_typedefs( async ) ;

        private:

            /// decorated backend.
            backend_res_t _backend ;

            bool_t _window_info_set = false  ;
            natus::graphics::backend_t::window_info_t _window_info ;
            natus::concurrent::mutex_t _window_info_mtx ;

            typedef std::function< void_t ( natus::graphics::backend_ptr_t ) > runtime_command_t ;
            natus_typedefs( natus::ntd::vector< runtime_command_t >, commands ) ;
            commands_t _configures ;
            natus::concurrent::mutex_t _configures_mtx ;
            commands_t _runtimes ;
            natus::concurrent::mutex_t _runtimes_mtx ;

        private: // sync

            natus::concurrent::mutex_t _frame_mtx ;
            natus::concurrent::condition_variable_t _frame_cv ;
            size_t _num_enter = 0 ;

        public:

            async( void_t ) ;
            async( backend_res_t ) ;
            async( this_cref_t ) = delete ;
            async( this_rref_t ) ;
            ~async( void_t ) ;

        public:

            natus::graphics::result set_window_info( natus::graphics::backend_t::window_info_cref_t ) noexcept ;

            this_ref_t configure( natus::graphics::geometry_object_res_t, natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;

            this_ref_t configure( natus::graphics::render_object_res_t, natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;

            this_ref_t configure( natus::graphics::shader_object_res_t, natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;

            this_ref_t configure( natus::graphics::image_object_res_t, natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;

            this_ref_t configure( natus::graphics::framebuffer_object_res_t, natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;

            this_ref_t configure( natus::graphics::state_object_res_t, natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;

            
            this_ref_t update( natus::graphics::geometry_object_res_t, natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;
            this_ref_t use( natus::graphics::framebuffer_object_res_t, bool_t const = false,
                bool_t const = false, bool_t const = false, natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;
            this_ref_t use( natus::graphics::state_object_res_t, size_t const = 0, bool_t const = true, natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;
            this_ref_t render( natus::graphics::render_object_res_t, natus::graphics::backend::render_detail_cref_t, 
                natus::graphics::result_res_t = natus::graphics::result_res_t() ) noexcept ;

        public:

            /// render thread update function - DO NOT USE.
            void_t system_update( void_t ) noexcept ;

        public: // sync

            bool_t enter_frame( void_t ) ;
            void_t leave_frame( void_t ) ;
            void_t wait_for_frame( void_t ) ;
        };
        natus_res_typedef( async ) ;

        // restricts access to the async
        class async_view
        {
            natus_this_typedefs( async_view ) ;

        private:

            natus::graphics::async_res_t _async ;

        public: 

            async_view( natus::graphics::async_res_t const& r ) :
                _async( r ) {}
            async_view( natus::graphics::async_res_t&& r ) :
                _async( ::std::move( r ) ){}

        public:

            async_view( void_t ) {}
            async_view( this_cref_t rhv ) :
                _async( rhv._async ){}

            async_view( this_rref_t rhv ) :
                _async( ::std::move( rhv._async ) ){}

            ~async_view( void_t ) {}

            this_ref_t operator = ( this_cref_t rhv )
            {
                _async = rhv._async ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _async = std::move( rhv._async ) ;
                return *this ;
            }

            this_ref_t configure( natus::graphics::geometry_object_res_t config,
                natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                _async->configure( config, res ) ;
                return *this ;
            }

            this_ref_t configure( natus::graphics::render_object_res_t config, 
                natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                _async->configure( config, res ) ;
                return *this ;
            }

            this_ref_t configure( natus::graphics::shader_object_res_t config,
                natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                _async->configure( config, res ) ;
                return *this ;
            }

            this_ref_t configure( natus::graphics::image_object_res_t config,
                natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                _async->configure( config, res ) ;
                return *this ;
            }

            this_ref_t configure( natus::graphics::framebuffer_object_res_t obj, natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept 
            {
                _async->configure( obj, res ) ;
                return *this ;
            }

            this_ref_t configure( natus::graphics::state_object_res_t obj, natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                _async->configure( obj, res ) ;
                return *this ;
            }

            this_ref_t update( natus::graphics::geometry_object_res_t gs,
                natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                _async->update( gs, res ) ;
                return *this ;
            }

            this_ref_t use( natus::graphics::framebuffer_object_res_t obj, bool_t clear_color = false,
                bool_t clear_depth= false, bool_t clear_stencil= false, natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                _async->use( obj, clear_color, clear_depth, clear_stencil, res ) ;
                return *this ;
            }

            this_ref_t use( natus::graphics::state_object_res_t obj, size_t const sid = 0 , bool_t const push = true, natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                _async->use( obj, sid, push, res ) ;
                return *this ;
            }

            this_ref_t render( natus::graphics::render_object_res_t config, 
                natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                natus::graphics::backend_t::render_detail_t detail ;
                _async->render( config, detail, res ) ;
                return *this ;
            }

            this_ref_t render( natus::graphics::render_object_res_t config, natus::graphics::backend::render_detail_cref_t detail, 
                natus::graphics::result_res_t res = natus::graphics::result_res_t() ) noexcept
            {
                _async->render( config, detail, res ) ;
                return *this ;
            }

        public:

        };
        natus_typedef( async_view ) ;

        class NATUS_GRAPHICS_API async_views
        {
            natus_this_typedefs( async_views ) ;

            natus_typedefs( natus::ntd::vector< async_view_t >, asyncs ) ;
            asyncs_t _asyncs ;

        public:

            async_views( void_t ) noexcept {}
            async_views( this_cref_t rhv ) 
            {
                _asyncs = rhv._asyncs ;
            }
            async_views( this_rref_t rhv ) noexcept
            {
                _asyncs = std::move( rhv._asyncs ) ;
            }
            async_views( asyncs_t asyncs ) noexcept
            {
                _asyncs = std::move( asyncs ) ;
            }
            ~async_views( void_t ) noexcept {}

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _asyncs = rhv._asyncs ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _asyncs = std::move( rhv._asyncs ) ;
                return *this ;
            }

        public:

            typedef std::function< void_t ( natus::graphics::async_view_t ) > for_each_async_t ;
            void_t for_each( for_each_async_t funk )
            {
                for( auto & a : _asyncs ) funk( a ) ;
            }
        };
        natus_typedef( async_views ) ;
    }
}
