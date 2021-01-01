
#include "app.h"

#if defined( NATUS_GRAPHICS_WGL )
#include "platform/wgl/wgl_context.h"
#include "platform/wgl/wgl_window.h"
#include <natus/graphics/backend/gl/gl3.h>
#endif
#if defined( NATUS_GRAPHICS_GLX )
#include "platform/glx/glx_context.h"
#include "platform/glx/glx_window.h"
#include <natus/graphics/backend/gl/gl3.h>
#endif
#if defined( NATUS_GRAPHICS_EGL )
#include "platform/egl/egl_context.h"
#include "platform/egl/egl_window.h"
#if defined( NATUS_GRAPHICS_OPENGLES )
#include <natus/graphics/backend/gl/es3.h>
#endif
#endif
#if defined( NATUS_GRAPHICS_DIRECT3D )
#include "platform/d3d/d3d_context.h"
#include "platform/d3d/d3d_window.h"
#endif

#include <natus/graphics/async.h>
#include <natus/graphics/backend/null/null.h>

#include <natus/device/global.h>
#include <natus/device/layouts/three_mouse.hpp>

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
    natus::ntd::string_cref_t name, this_t::window_info_in_t wi, natus::ntd::vector< natus::graphics::backend_type > types )
{
    this_t::per_window_info_t pwi ;
    natus::graphics::backend_res_t backend = natus::graphics::null_backend_res_t(
        natus::graphics::null_backend_t() ) ;
    natus::application::gfx_context_res_t ctx ;

    auto rnd_msg_recv = natus::application::window_message_receiver_res_t(
        natus::application::window_message_receiver_t() ) ;

    auto gfx_msg_send = natus::application::window_message_receiver_res_t(
        natus::application::window_message_receiver_t() ) ;

    // other entity -> context
    {
        pwi.gfx_send = gfx_msg_send ;
    }

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

        if( types.empty()) 
        {
            types = 
            { 
                natus::graphics::backend_type::es3,
                natus::graphics::backend_type::d3d11,
                natus::graphics::backend_type::gl3
            } ;
        }

        for( auto const t : types )
        {
            switch( t )
            {
            case natus::graphics::backend_type::d3d11: 
                ctx = this_t::create_d3d_window( wii, rnd_msg_recv, pwi ) ;
                break ;
            case natus::graphics::backend_type::es3:
                ctx = this_t::create_egl_window( wii, rnd_msg_recv, pwi ) ;
                break ;
            case natus::graphics::backend_type::gl3:
                ctx = this_t::create_wgl_window( wii, rnd_msg_recv, pwi ) ;
                if( !ctx.is_valid() )
                    ctx = this_t::create_glx_window( wii, rnd_msg_recv, pwi ) ;
                break ;
            default: break ;
            } 

            if( ctx.is_valid() ) break ;
        }

        if( !ctx.is_valid() )
        {
            natus::log::global_t::warning( "[create window] : can not create requested window. Using null window." ) ;
            ctx = this_t::create_null_window( wii, rnd_msg_recv, pwi ) ;
        }
    }
    
    natus::graphics::async_res_t async = pwi.async ;
    pwi.imgui = natus::tool::imgui_res_t( natus::tool::imgui_t() ) ;
    pwi.imgui->init( natus::graphics::async_view_t( async ) ) ;

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
    

    return this_t::window_async_t( this_t::window_view_t( _windows.size()-1, msg_send, gfx_msg_send ), 
        natus::graphics::async_view_t( ::std::move( async ) ) ) ;
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

    if( this_t::before_device() )
    {
        if( _windows.size() != 0 )
        {
            _windows[ 0 ].imgui->update( _dev_ascii ) ;
            _windows[ 0 ].imgui->update( _dev_mouse ) ;
        }
        
        {
            this_t::device_data_in_t dat = {} ;
            this->on_device( dat ) ;
        }

        this_t::after_device() ;
    }

    if( this_t::before_render() )
    {
        // do the actual rendering of the app
        
        this_t::render_data_t dat ;
        this->on_graphics( dat ) ;

        // do the tool ui only if rendering is possible
        if( this_t::before_tool() )
        {
            if( _windows.size() != 0 )
            {
                bool_t render = false ;

                _windows[ 0 ].imgui->begin() ;
                

                _windows[ 0 ].imgui->execute( [&] ( ImGuiContext* ctx )
                {
                    if( this->on_tool( natus::tool::imgui_view_t( _windows[ 0 ].imgui ) ) != natus::application::result::no_imgui )
                    {
                        render = true ;
                    }
                } ) ;
                _windows[ 0 ].imgui->end() ;
                

                if( render ) _windows[ 0 ].imgui->render( _windows[ 0 ].async ) ;

            }
            this_t::after_tool() ;
        }

        this_t::after_render() ;
    }

    return true ;
}

//***
bool_t app::before_tool( void_t ) noexcept
{
    natus::device::global_t::system()->search( [&] ( natus::device::idevice_res_t dev_in )
    {
        if( natus::device::three_device_res_t::castable( dev_in ) )
        {
            _dev_mouse = dev_in ;
        }
        else if( natus::device::ascii_device_res_t::castable( dev_in ) )
        {
            _dev_ascii = dev_in ;
        }
    } ) ;

    if( !_dev_mouse.is_valid() )
    {
        return false ;
    }

    if( !_dev_ascii.is_valid() )
    {
        return false ;
    }

    return true ;
}

//***
bool_t app::after_tool( void_t ) noexcept
{
    return true ;
}

//***
bool_t app::before_device( void_t ) noexcept 
{
    size_t const milli_passed = std::chrono::duration_cast< std::chrono::milliseconds > ( this_t::device_clock_t::now() - _device_tp ).count() ;

    if( milli_passed >= _device_milli_update )
    {
        _device_tp = this_t::device_clock_t::now() ;
        natus::device::global_t::system()->update() ;
        return true ;
    }

    return false ;
}

//***
bool_t app::after_device( void_t ) noexcept 
{
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

                    natus::tool::imgui_t::window_data_t wd ;
                    wd.width = int_t( wi.width ) ;
                    wd.height = int_t( wi.height ) ;
                    pwi.imgui->update( wd ) ;
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
void_t app::window_view::position( int_t const x, int_t const y ) noexcept 
{
    natus::application::resize_message_t msg ;
    msg.position = true ;
    msg.x = x ;
    msg.y = y ;
    _msg_wnd->on_resize( msg ) ;
}

//***
void_t app::window_view::resize( size_t const w, size_t const h ) noexcept
{
    natus::application::resize_message_t msg ;
    msg.resize = true ;
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
natus::application::gfx_context_res_t app::create_wgl_window( natus::application::window_info_in_t wii, natus::application::window_message_receiver_res_t rnd_msg_recv, this_t::per_window_info_inout_t pwi ) noexcept
{
    natus::application::gfx_context_res_t ctx ;
    natus::graphics::backend_res_t backend = natus::graphics::null_backend_res_t(
        natus::graphics::null_backend_t() ) ;

#if defined( NATUS_GRAPHICS_WGL )

    natus::application::wgl::window_res_t wglw =
    natus::application::wgl::window_t( natus::application::gl_info_t(), wii ) ;

    pwi.wnd = wglw ;

    {
        ctx = wglw->get_context() ;
        backend = ctx->create_backend() ;
    }

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

    // show the window after all listeners have been registered.
    wglw->get_window()->show_window( wii ) ;

#else
    (void) wii ;
    (void) rnd_msg_recv ;
    (void) pwi ;
#endif

    pwi.async = natus::graphics::async_res_t(
        natus::graphics::async_t( std::move( backend ) ) ) ;

    return std::move( ctx ) ;
}

natus::application::gfx_context_res_t app::create_egl_window( natus::application::window_info_in_t wii, natus::application::window_message_receiver_res_t rnd_msg_recv, this_t::per_window_info_inout_t pwi) noexcept 
{
    natus::application::gfx_context_res_t ctx ;
    natus::graphics::backend_res_t backend = natus::graphics::null_backend_res_t(
        natus::graphics::null_backend_t() ) ;

#if defined( NATUS_GRAPHICS_EGL )

    natus::application::egl::window_res_t eglw =
        natus::application::egl::window_t( natus::application::gl_info_t(), wii ) ;

    pwi.wnd = eglw ;    

    {
        ctx = eglw->get_context() ;
        backend = ctx->create_backend() ;
    }

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

    // show the window after all listeners have been registered.
    eglw->get_window()->show_window( wii ) ;

#else
    ( void ) wii ;
    ( void ) rnd_msg_recv ;
    ( void ) pwi ;
#endif

    pwi.async = natus::graphics::async_res_t(
        natus::graphics::async_t( std::move( backend ) ) ) ;

    return std::move( ctx ) ;
}

natus::application::gfx_context_res_t app::create_d3d_window( natus::application::window_info_in_t wii, natus::application::window_message_receiver_res_t rnd_msg_recv, this_t::per_window_info_inout_t pwi ) noexcept 
{
    natus::application::gfx_context_res_t ctx ;
    natus::graphics::backend_res_t backend = natus::graphics::null_backend_res_t(
        natus::graphics::null_backend_t() ) ;

#if defined( NATUS_GRAPHICS_DIRECT3D )

    natus::application::d3d::window_res_t d3dw =
        natus::application::d3d::window_t( natus::application::d3d_info_t(), wii ) ;

    pwi.wnd = d3dw ;

    ctx = d3dw->get_context() ;
    backend = ctx->create_backend() ;

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

    // show the window after all listeners have been registered.
    d3dw->get_window()->show_window( wii ) ;
#else
    (void) wii ;
    (void) rnd_msg_recv ;
    (void) pwi ;
#endif

    pwi.async = natus::graphics::async_res_t(
        natus::graphics::async_t( std::move( backend ) ) ) ;

    return std::move( ctx ) ;
}

natus::application::gfx_context_res_t app::create_glx_window( natus::application::window_info_in_t wii, natus::application::window_message_receiver_res_t rnd_msg_recv, this_t::per_window_info_inout_t pwi ) noexcept 
{
    natus::application::gfx_context_res_t ctx ;
    natus::graphics::backend_res_t backend = natus::graphics::null_backend_res_t(
        natus::graphics::null_backend_t() ) ;

#if defined( NATUS_GRAPHICS_GLX )

    natus::application::glx::window_res_t glxw =
        natus::application::glx::window_t( natus::application::gl_info_t(), wii ) ;

    pwi.wnd = glxw ;

    {
        ctx = glxw->get_context() ;
        backend = ctx->create_backend() ;
    }

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

    // show the window after all listeners have been registered.
    glxw->get_window()->show_window( wii ) ;

#endif

    pwi.async = natus::graphics::async_res_t(
        natus::graphics::async_t( std::move( backend ) ) ) ;

    return std::move( ctx ) ;
}

natus::application::gfx_context_res_t app::create_null_window( natus::application::window_info_in_t, natus::application::window_message_receiver_res_t, this_t::per_window_info_inout_t pwi ) noexcept 
{
    natus::application::gfx_context_res_t ctx ;
    natus::graphics::backend_res_t backend = natus::graphics::null_backend_res_t(
        natus::graphics::null_backend_t() ) ;

    pwi.async = natus::graphics::async_res_t(
        natus::graphics::async_t( std::move( backend ) ) ) ;

    return std::move( ctx ) ;
}
