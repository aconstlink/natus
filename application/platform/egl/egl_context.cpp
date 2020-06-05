#include "egl_context.h"
#include "egl_window.h"

//#include <natus/ogl/es/es.h>
#include <natus/ogl/egl/egl.h>
#include <natus/std/string/split.hpp>

using namespace natus::application ;
using namespace natus::application::egl ;

//***********************n*****************************************
context::context( void_t )
{
}

//****************************************************************
context::context( gl_info_in_t gli, EGLNativeWindowType wnd, EGLNativeDisplayType disp ) 
{
    _display = disp ;
    _wnd = wnd ;
    this_t::create_the_context( gli ) ;
}

//****************************************************************
context::context( this_rref_t rhv )
{
    _display = rhv._display ;
    rhv._display = NULL ;

    _wnd = rhv._wnd ;
    rhv._wnd = 0 ;

    _context = rhv._context ;
    rhv._context = 0 ;
}

//****************************************************************
context::~context( void_t )
{
    this_t::deactivate() ;
}

//***************************************************************
natus::application::result context::activate( void_t ) 
{
    //glXMakeCurrent( _display, _wnd, NULL ) ;
    //XLockDisplay( _display ) ;
    //auto const res = glXMakeCurrent( _display, _wnd, _context ) ;
    //XUnlockDisplay( _display ) ;
    //natus::log::global_t::warning( natus::core::is_not(res), 
      //      natus_log_fn( "glXMakeCurrent" ) ) ;

    return natus::application::result::ok ;
}

//***************************************************************
natus::application::result context::deactivate( void_t ) 
{
    //auto const res = glXMakeCurrent( _display, 0, 0 ) ;
    //natus::log::global_t::warning( natus::core::is_not(res), 
      //      natus_log_fn( "glXMakeCurrent" ) ) ;
    return natus::application::result::ok ;
}

//***************************************************************
natus::application::result context::vsync( bool_t const on_off ) 
{
    eglSwapInterval( _display, on_off ? 1 : 0 ) ;
    return natus::application::result::ok ;
}

//**************************************************************
natus::application::result context::swap( void_t ) 
{
    //glXSwapBuffers( _display, _wnd ) ;
    //const GLenum glerr = natus::ogl::gl::glGetError( ) ;
    //natus::log::global_t::warning( glerr != GL_NO_ERROR, 
      //      natus_log_fn( "glXSwapBuffers" ) ) ;
    return natus::application::result::ok ;
}

//**************************************************************
/*natus::application::result context::create_context( 
     Display* display, Window wnd, GLXContext context ) 
{
    _display = display ;
    _wnd = wnd ;

    // the context comes in already create
    _context = context ;

    return natus::application::result::ok ;
}*/

//***************************************************************
natus::application::result context::is_extension_supported( 
    natus::std::string_cref_t extension_name ) 
{
    /*this_t::strings_t ext_list ;
    if( natus::application::no_success( get_glx_extension(ext_list) ) ) 
        return natus::application::result::failed_wgl ;

    this_t::strings_t::iterator iter = ext_list.begin() ;
    while( iter != ext_list.end() )
    {
        if( *iter == extension_name ) 
            return natus::application::result::ok ;
        ++iter ;
    }*/
    return natus::application::result::invalid_extension ;
}

//*****************************************************************
natus::application::result context::get_egl_extension( this_t::strings_out_t /*ext_list*/ )
{
    return natus::application::result::ok ;
}

//****************************************************************
natus::application::result context::get_es_extension( this_t::strings_out_t ext_list )
{
    /*const GLubyte * ch = natus::ogl::gl::glGetString( GL_EXTENSIONS ) ;
    if( !ch ) return natus::application::result::failed ;

    natus::std::string_t extension_string( (const char*)ch) ;
    natus::std::string_ops::split( extension_string, ' ', ext_list ) ;*/
    return natus::application::result::ok ;
}

//****************************************************************
natus::application::result context::get_es_version( natus::application::gl_version & version ) const 
{
    /*const GLubyte* ch = natus::ogl::gl::glGetString(GL_VERSION) ;
    if( !ch ) return natus::application::result::failed ;

    natus::std::string_t version_string = natus::std::string((const char*)ch) ;

    GLint major = 0;//boost::lexical_cast<GLint, std::string>(*token) ;
    GLint minor = 0;//boost::lexical_cast<GLint, std::string>(*(++token));

    {
        natus::ogl::gl::glGetIntegerv( GL_MAJOR_VERSION, &major ) ;
        GLenum err = natus::ogl::gl::glGetError() ;
        if( err != GL_NO_ERROR )
        {
            natus::std::string_t const es = ::std::to_string(err) ;
            natus::log::global::error( 
                "[context::get_gl_version] : get gl major <"+es+">" ) ;
        }
    }
    {
        natus::ogl::gl::glGetIntegerv( GL_MINOR_VERSION, &minor) ;
        GLenum err = natus::ogl::gl::glGetError() ;
        if( err != GL_NO_ERROR )
        {
            natus::std::string_t es = ::std::to_string(err) ;
            natus::log::global::error( "[context::get_gl_version] : get gl minor <"+es+">" ) ;
        }
    }

    version.major = major ;
    version.minor = minor ;
*/
    return natus::application::result::ok ;
}

//****************************************************************
void_t context::clear_now( natus::math::vec4f_t const & vec ) 
{
    //natus::ogl::gl::glClearColor( vec.x(), vec.y(), vec.z(), vec.w() ) ;
    //natus::ogl::gl::glClear( GL_COLOR_BUFFER_BIT ) ;
    
    //GLenum const gler = natus::ogl::gl::glGetError() ;
    //natus::log::global_t::error( gler != GL_NO_ERROR, "[context::clear_now] : glClear" ) ;
}

//***************************************************************
natus::application::result context::create_the_context( gl_info_cref_t gli ) 
{
    EGLDisplay display = eglGetDisplay( _display ) ;

    {
        EGLint major = 0 ;
        EGLint minor = 0 ;
        
        auto const res = eglInitialize( display , &major, &minor ) ;
        if( res != EGL_TRUE )
        {
            natus::log::global_t::error( natus_log_fn("eglInitialize") ) ;
            return natus::application::result::failed ;
        }
    }

    return natus::application::result::ok ;
}

//****************************************************************
bool_t context::determine_es_version( gl_version & gl_out ) const 
{
    
    return true ;
}
