
#include "app.h"

#include "platform/gfx_context.h"

#if defined( NATUS_GRAPHICS_WGL )
#include "platform/wgl/wgl_context.h"
#include "platform/wgl/wgl_window.h"
#include <natus/gpu/backend/gl3/gl3.h>
#elif defined( NATUS_GRAPHICS_GLX )
#include "platform/glx/glx_context.h"
#include "platform/glx/glx_window.h"
#include <natus/gpu/backend/gl3/gl3.h>
#include <natus/gpu/backend/es3/es3.h>
#endif

#include <natus/gpu/async.h>
#include <natus/gpu/backend/null/null.h>

using namespace natus::application ;

//***
app::app( void_t ) 
{
    _access = natus::memory::global_t::alloc< bool_t >() ;
    *_access = false ;
}

//***
app::app( this_rref_t rhv )
{
    _windows = ::std::move( rhv._windows ) ;
    natus_move_member_ptr( _access, rhv ) ;
}

//***
app::~app( void_t )
{
    for( auto & pwi : _windows )
    {
        this_t::destroy_window( pwi ) ;
    }
    
    natus::memory::global_t::dealloc( _access ) ;
}

//***
app::wid_async_t app::create_window( 
    natus::std::string_cref_t name, this_t::window_info_in_t wi )
{
    this_t::per_window_info_t pwi ;
    natus::gpu::backend_res_t backend = natus::gpu::null_backend_res_t(
        natus::gpu::null_backend_t() ) ;
    natus::application::gfx_context_res_t ctx ;

    auto rnd_msg_recv = natus::application::window_message_receiver_res_t(
        natus::application::window_message_receiver_t() ) ;

    {
#if defined( NATUS_GRAPHICS_WGL )

        natus::application::gl_info_t gli ;
        {
            gli.vsync_enabled = wi.vsync ;
        }

        natus::application::window_info_t wii ;
        {
            wii.x = wi.x ;
            wii.y = wi.y ;
            wii.w = wi.w ;
            wii.h = wi.h ;
            wii.window_name = name ;
            wii.borderless = !wi.borders ;
            wii.fullscreen = wi.fullscreen ;
        }

        natus::application::wgl::window_res_t wglw =
            natus::application::wgl::window_t( gli, wii ) ;

        pwi.wnd = wglw ;

        natus::application::wgl::context_res_t glctx =
            wglw->get_context() ;

        {
            natus::application::gl_version glv ;
            glctx->get_gl_version( glv ) ;
            if( glv.major >= 3 )
            {
                backend = natus::gpu::gl3_backend_res_t(
                    natus::gpu::gl3_backend_t() ) ;
            }
        }

        ctx = glctx ;

        {
            pwi.msg_recv = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            wglw->get_window()->register_listener( pwi.msg_recv ) ; // application
            wglw->get_window()->register_listener( rnd_msg_recv ) ; // render
        }

        // show the window after all listeners have been registered.
        wglw->get_window()->show_window( wii ) ;

#elif defined( NATUS_GRAPHICS_GLX )

        natus::application::gl_info_t gli ;
        {
            gli.vsync_enabled = wi.vsync ;
        }

        natus::application::window_info_t wii ;
        {
            wii.x = wi.x ;
            wii.y = wi.y ;
            wii.w = wi.w ;
            wii.h = wi.h ;
            wii.window_name = name ;
            wii.borderless = !wi.borders ;
            wii.fullscreen = wi.fullscreen ;
        }

        natus::application::glx::window_res_t glxw =
            natus::application::glx::window_t( gli, wii ) ;

        pwi.wnd = glxw ;

        natus::application::glx::context_res_t glctx =
            glxw->get_context() ;

        {
            natus::application::gl_version glv ;
            glctx->get_gl_version( glv ) ;
            if( glv.major >= 3 )
            {
                backend = natus::gpu::gl3_backend_res_t(
                    natus::gpu::gl3_backend_t() ) ;
            }
        }

        ctx = glctx ;

        {
            pwi.msg_recv = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            glxw->get_window()->register_listener( pwi.msg_recv ) ; // application
            glxw->get_window()->register_listener( rnd_msg_recv ) ; // render
        }

        // show the window after all listeners have been registered.
        glxw->get_window()->show_window( wii ) ;
#endif

        pwi.async = natus::gpu::async_res_t( 
            natus::gpu::async_t( backend ) ) ;
    }

    natus::gpu::async_res_t async = pwi.async ;

    bool_ptr_t run = natus::memory::global_t::alloc<bool_t>(
        natus_log_fn( "bool for render thread while") ) ;
    *run = true ;
    pwi.run = run ;

    pwi.rnd_thread = natus::concurrent::thread_t( [=]( void_t )
    {
        auto async_ = async ;
        auto ctx_ = ctx ;
        auto run_ = run ;
        auto recv = rnd_msg_recv ;

        ctx_->activate() ;
        async_->set_ready() ;
        while( *run_ ) 
        {
            {
                natus::application::window_message_receiver_t::state_vector sv ;
                if( recv->swap_and_reset( sv ) )
                {

                    natus::gpu::backend_t::window_info_t wi ;
                    if( sv.resize_changed )
                    {
                        wi.width = sv.resize_msg.w ;
                        wi.height = sv.resize_msg.h ;
                    }

                    async_->set_window_info( wi ) ;
                }
            }
            

            async_->wait_for_frame() ;
            async_->system_update() ;            
            ctx_->swap() ;
            async_->set_ready() ;
            static size_t this_count = 0 ;
            this_count++ ;
        }
        natus::log::global_t::status( natus_log_fn("thread end") ) ;
        ctx_->deactivate() ;
    } ) ;
    pwi.async = async ;

    // add per window info
    //here
    natus::concurrent::lock_guard_t lk( _wmtx ) ;
    _windows.emplace_back( ::std::move( pwi ) ) ;

    return ::std::make_pair( _windows.size()-1, 
        this_t::async_view_t( ::std::move( async ), _access ) ) ;
}

//***
void_t app::destroy_window( this_t::per_window_info_ref_t pwi ) 
{
    *( pwi.run ) = false ;
    pwi.rnd_thread.join() ;
    natus::memory::global_t::dealloc( pwi.run ) ;
}

//***
natus::application::result app::request_change( this_t::window_info_in_t )
{
    return natus::application::result::ok ;
}

//***
bool_t app::before_update( void_t ) 
{
    *_access = false ;
    return true ;
}

//***
bool_t app::after_update( void_t )
{
    *_access = true ;
    ++_update_count ;
    return true ;
}

//***
bool_t app::before_render( void_t )
{
    size_t windows = _windows.size() ;

    // check if async system is ready
    for( auto & pwi : _windows )
    {
        if( pwi.async->enter_frame() )
            --windows ;
    }

    if( windows != 0 )
    {
        for( auto& pwi : _windows )
        {
            pwi.async->leave_frame( false ) ;
        }
        // do not go into on_render
        return false ;
    }

    return true ;
}

//***
bool_t app::after_render( void_t )
{
    ++_render_count ;
    for( auto& pwi : _windows )
    {
        pwi.async->leave_frame( true ) ;
    }
    return true ;
}

//***
