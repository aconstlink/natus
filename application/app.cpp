
#include "app.h"

#include "platform/gfx_context.h"
#include "platform/window_info.h"

#if defined( NATUS_GRAPHICS_WGL )
#include "platform/wgl/wgl_context.h"
#include "platform/wgl/wgl_window.h"
#include <natus/graphics/backend/gl3/gl3.h>
#endif
#if defined( NATUS_GRAPHICS_GLX )
#include "platform/glx/glx_context.h"
#include "platform/glx/glx_window.h"
#include <natus/graphics/backend/gl3/gl3.h>
#endif
#if defined( NATUS_GRAPHICS_EGL )
#include "platform/egl/egl_context.h"
#include "platform/egl/egl_window.h"
#if defined( NATUS_GRAPHICS_OPENGLES )
#include <natus/graphics/backend/es3/es3.h>
#endif
#endif
#if defined( NATUS_GRAPHICS_DIRECT3D )
#include "platform/d3d/d3d_context.h"
#include "platform/d3d/d3d_window.h"
#endif

#include <natus/graphics/async.h>
#include <natus/graphics/backend/null/null.h>

#include <natus/audio/backend/oal/oal.h>

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
    _audios = std::move( rhv._audios ) ;
    natus_move_member_ptr( _access, rhv ) ;
}

//***
app::~app( void_t )
{
    for( auto & pwi : _windows )
    {
        this_t::destroy_window( pwi ) ;
    }

    for( auto& pwi : _audios )
    {
        this_t::destroy_audio( pwi ) ;
    }
    
    natus::memory::global_t::dealloc( _access ) ;
}

//***
natus::audio::async_access_t app::create_audio_engine( void_t )  noexcept
{
    this_t::per_audio_info_t pai ;
    natus::audio::backend_res_t backend = natus::audio::oal_backend_res_t(
        natus::audio::oal_backend_t() ) ;

    pai.async = natus::audio::async_res_t(
        natus::audio::async_t( backend ) ) ;

    natus::audio::async_res_t async = pai.async ;

    bool_ptr_t run = natus::memory::global_t::alloc<bool_t>(
        natus_log_fn( "bool for render thread while" ) ) ;
    *run = true ;
    pai.run = run ;

    pai.rnd_thread = natus::concurrent::thread_t( [=] ( void_t )
    {
        auto async_ = async ;
        auto run_ = run ;

        async_->enter_thread() ;
        while( *run_ )
        {
            async_->wait_for_frame() ;
            async_->system_update() ;
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ) ;
        }
        async_->leave_thread() ;
        natus::log::global_t::status( "[natus::app] : audio thread end" ) ;
    } ) ;
    pai.async = async ;
    
    natus::concurrent::lock_guard_t lk( _amtx ) ;
    _audios.emplace_back( ::std::move( pai ) ) ;

    return natus::audio::async_access_t( std::move( async ), _access ) ;
}

//***
app::window_async_t app::create_window( 
    natus::ntd::string_cref_t name, this_t::window_info_in_t wi )
{
    this_t::per_window_info_t pwi ;
    natus::graphics::backend_res_t backend = natus::graphics::null_backend_res_t(
        natus::graphics::null_backend_t() ) ;
    natus::application::gfx_context_res_t ctx ;

    auto rnd_msg_recv = natus::application::window_message_receiver_res_t(
        natus::application::window_message_receiver_t() ) ;

    auto gfx_msg_send = natus::application::window_message_receiver_res_t(
        natus::application::window_message_receiver_t() ) ;

    {
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

#if 0 //defined( NATUS_GRAPHICS_DIRECT3D )

        natus::application::d3d_info_t d3di ;
        {
            d3di.vsync_enabled = wi.vsync ;
        }

        natus::application::d3d::window_res_t d3dw =
            natus::application::d3d::window_t( d3di, wii ) ;

        pwi.wnd = d3dw ;

        natus::application::d3d::context_res_t glctx =
            d3dw->get_context() ;

        /*{
            natus::application::d3d_version ver ;
            glctx->get_gl_version( glv ) ;
            if( glv.major >= 3 )
            {
                backend = natus::graphics::gl3_backend_res_t(
                    natus::graphics::gl3_backend_t() ) ;
            }
        }*/

        ctx = glctx ;

        // window -> other entity
        {
            pwi.msg_recv = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            d3dw->get_window()->register_in( pwi.msg_recv ) ; // application
            d3dw->get_window()->register_in( rnd_msg_recv ) ; // render
        }

        // other entity -> window
        {
            pwi.msg_send = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            d3dw->get_window()->register_out( pwi.msg_send ) ; // application
        }

        // other entity -> context
        {
            pwi.gfx_send = gfx_msg_send ;
        }

        // show the window after all listeners have been registered.
        d3dw->get_window()->show_window( wii ) ;

#elif defined( NATUS_GRAPHICS_WGL )

        natus::application::gl_info_t gli ;
        {
            gli.vsync_enabled = wi.vsync ;
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
                backend = natus::graphics::gl3_backend_res_t(
                    natus::graphics::gl3_backend_t() ) ;
            }
        }

        ctx = glctx ;

        // window -> other entity
        {
            pwi.msg_recv = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            wglw->get_window()->register_in( pwi.msg_recv ) ; // application
            wglw->get_window()->register_in( rnd_msg_recv ) ; // render
        }

        // other entity -> window
        {
            pwi.msg_send = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            wglw->get_window()->register_out( pwi.msg_send ) ; // application
        }

        // other entity -> context
        {
            pwi.gfx_send = gfx_msg_send ;
        }

        // show the window after all listeners have been registered.
        wglw->get_window()->show_window( wii ) ;

#elif defined( NATUS_GRAPHICS_EGL )

        natus::application::gl_info_t gli ;
        {
            gli.vsync_enabled = wi.vsync ;
        }

        natus::application::egl::window_res_t eglw =
            natus::application::egl::window_t( gli, wii ) ;
        pwi.wnd = eglw ;

        natus::application::egl::context_res_t glctx =
            eglw->get_context() ;

        {
            natus::application::gl_version glv ;
            glctx->get_es_version( glv ) ;
            if( glv.major >= 3 )
            {
                backend = natus::graphics::es3_backend_res_t(
                    natus::graphics::es3_backend_t() ) ;
            }
        }

        ctx = glctx ;

        // window -> other entity
        {
            pwi.msg_recv = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            eglw->get_window()->register_in( pwi.msg_recv ) ; // application
            eglw->get_window()->register_in( rnd_msg_recv ) ; // render
        }

        // other entity -> window
        {
            pwi.msg_send = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            eglw->get_window()->register_out( pwi.msg_send ) ; // application
        }

        // other entity -> context
        {
            pwi.gfx_send = gfx_msg_send ;
        }

        // show the window after all listeners have been registered.
        eglw->get_window()->show_window( wii ) ;

#elif defined( NATUS_GRAPHICS_GLX )

        natus::application::gl_info_t gli ;
        {
            gli.vsync_enabled = wi.vsync ;
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
                backend = natus::graphics::gl3_backend_res_t(
                    natus::graphics::gl3_backend_t() ) ;
            }
        }

        ctx = glctx ;

        // window -> other entity
        {
            pwi.msg_recv = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            glxw->get_window()->register_in( pwi.msg_recv ) ; // application
            glxw->get_window()->register_in( rnd_msg_recv ) ; // render
        }

        // other entity -> window
        {
            pwi.msg_send = natus::application::window_message_receiver_res_t(
                natus::application::window_message_receiver_t() ) ;
            glxw->get_window()->register_out( pwi.msg_send ) ; // application
        }

        // other entity -> context
        {
            pwi.gfx_send = gfx_msg_send ;
        }

        // show the window after all listeners have been registered.
        glxw->get_window()->show_window( wii ) ;
#endif

        pwi.async = natus::graphics::async_res_t( 
            natus::graphics::async_t( backend ) ) ;
    }
    
    natus::graphics::async_res_t async = pwi.async ;

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
        auto recv2 = gfx_msg_send ;

        ctx_->activate() ;
        while( *run_ ) 
        {
            {
                natus::application::window_message_receiver_t::state_vector sv ;
                if( recv->swap_and_reset( sv ) )
                {

                    natus::graphics::backend_t::window_info_t wi ;
                    if( sv.resize_changed )
                    {
                        wi.width = sv.resize_msg.w ;
                        wi.height = sv.resize_msg.h ;
                    }

                    async_->set_window_info( wi ) ;
                }
            }

            {
                {
                    natus::application::window_message_receiver_t::state_vector sv ;
                    if( recv2->swap_and_reset( sv ) )
                    {
                        if( sv.vsync_msg_changed )
                        {
                            ctx->vsync( sv.vsync_msg.on_off ) ;
                        }
                    }
                }
            }
            
            async_->wait_for_frame() ;
            async_->system_update() ; 
            ctx_->swap() ;
        }
        natus::log::global_t::status( natus_log_fn("thread end") ) ;
        ctx_->deactivate() ;
    } ) ;
    pwi.async = async ;

    auto msg_send = pwi.msg_send ;

    // add per window info
    //here
    natus::concurrent::lock_guard_t lk( _wmtx ) ;
    _windows.emplace_back( ::std::move( pwi ) ) ;
    

    return ::std::make_pair( this_t::window_view_t( _windows.size()-1, msg_send, gfx_msg_send ), 
        natus::graphics::async_view_t( ::std::move( async ), _access ) ) ;
}

//***
void_t app::destroy_window( this_t::per_window_info_ref_t pwi ) 
{
    *( pwi.run ) = false ;
    pwi.async->enter_frame() ;
    pwi.async->leave_frame() ;
    if( pwi.rnd_thread.joinable() ) pwi.rnd_thread.join() ;
    natus::memory::global_t::dealloc( pwi.run ) ;
}

//***
void_t app::destroy_audio( this_t::per_audio_info_ref_t nfo ) 
{
    *( nfo.run ) = false ;
    nfo.async->enter_frame() ;
    nfo.async->leave_frame() ;
    if( nfo.rnd_thread.joinable() )nfo.rnd_thread.join() ;
    natus::memory::global_t::dealloc( nfo.run ) ;
}

//***
natus::application::result app::request_change( this_t::window_info_in_t )
{
    return natus::application::result::ok ;
}

//***
bool_t app::platform_update( void_t ) 
{
    if( this_t::before_update() )
    {
        this_t::update_data_t dat ;
        this->on_update( dat ) ;
        this_t::after_update() ;
    }

    if( this_t::before_audio() )
    {
        this_t::audio_data_t dat ;
        this->on_audio( dat ) ;
        this_t::after_audio() ;
    }
    
    if( this_t::before_render() )
    {
        this_t::render_data_t dat ;
        this->on_graphics( dat ) ;
        this_t::after_render() ;
    }

    return true ;
}

//***
bool_t app::before_update( void_t ) 
{
    *_access = false ;

    size_t id = 0 ;
    for( auto & pwi : _windows )
    {
        // check messages from the window
        {
            natus::application::window_message_receiver_t::state_vector sv ;
            if( pwi.msg_recv->swap_and_reset( sv ) )
            {

                natus::graphics::backend_t::window_info_t wi ;
                if( sv.resize_changed )
                {
                    wi.width = sv.resize_msg.w ;
                    wi.height = sv.resize_msg.h ;
                }

                this_t::window_event_info_t wei ;
                wei.w = uint_t( wi.width ) ;
                wei.h = uint_t( wi.height ) ;
                this->on_event( id++, wei ) ;
            }
        }

        // check and send message to the window
        if( pwi.msg_send->has_any_change() )
        {
            pwi.wnd->check_for_messages() ;
        }
    }

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

    return windows == 0 ;
}

//***
bool_t app::after_render( void_t )
{
    ++_render_count ;
    for( auto& pwi : _windows )
    {
        pwi.async->leave_frame() ;
    }
    return true ;
}

//***
bool_t app::before_audio( void_t ) 
{
    size_t audio = _audios.size() ;

    // check if async system is ready
    for( auto& pwi : _audios )
    {
        if( pwi.async->enter_frame() )
            --audio ;
    }

    return audio == 0 ;
}

//***
bool_t app::after_audio( void_t ) 
{
    ++_audio_count ;
    for( auto& pwi : _audios )
    {
        pwi.async->leave_frame() ;
    }
    return true ;
}

//***
app::window_view::window_view( void_t ) 
{
}

//***
app::window_view::window_view( this_rref_t rhv )
{
    _id = rhv._id ;
    _msg_wnd = ::std::move( rhv._msg_wnd ) ;
    _msg_gfx = ::std::move( rhv._msg_gfx ) ;
}

//***
app::window_view::window_view( this_cref_t rhv )
{
    _id = rhv._id ;
    _msg_wnd = rhv._msg_wnd ;
    _msg_gfx = rhv._msg_gfx ;
}

//***
app::window_view::~window_view( void_t )
{
}

//***
app::window_view::this_ref_t app::window_view::operator = ( this_rref_t rhv ) 
{
    _id = rhv._id ;
    _msg_wnd = ::std::move( rhv._msg_wnd ) ;
    _msg_gfx = ::std::move( rhv._msg_gfx ) ;
    return *this ;
}

//***
app::window_view::this_ref_t app::window_view::operator = ( this_cref_t rhv ) 
{
    _id = rhv._id ;
    _msg_wnd = rhv._msg_wnd ;
    _msg_gfx = rhv._msg_gfx ;
    return *this ;
}

//***
app::window_view::window_view( window_id_t id, natus::application::window_message_receiver_res_t wnd,
    natus::application::window_message_receiver_res_t gfx ) :
    _id( id ), _msg_wnd( wnd ), _msg_gfx( gfx )
{
}

//***
app::window_id_t app::window_view::id( void_t ) const noexcept
{
    return _id ;
}

//***
void_t app::window_view::resize( size_t const w, size_t const h ) noexcept
{
    natus::application::resize_message_t msg ;
    msg.w = w ;
    msg.h = h ;
    _msg_wnd->on_resize( msg ) ;
}

//***
void_t app::window_view::vsync( bool_t const onoff ) noexcept
{
    natus::application::vsync_message_t msg ;
    msg.on_off = onoff ;
    _msg_gfx->on_vsync( msg ) ;
}

//***
void_t app::window_view::fullscreen( bool_t const onoff ) noexcept
{
    natus::application::fullscreen_message_t msg ;
    msg.on_off = onoff ;
    _msg_wnd->on_fullscreen( msg ) ;
}

//***
