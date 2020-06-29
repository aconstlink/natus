#include "egl_context.h"
#include "egl_window.h"

#include <natus/std/string/split.hpp>
#include <GLES3/gl3.h>

using namespace natus::application ;
using namespace natus::application::egl ;

//***********************n*****************************************
context::context( void_t )
{
}

//****************************************************************
context::context( gl_info_in_t gli, EGLNativeWindowType wnd, EGLNativeDisplayType disp ) 
{
    _ndt = disp ;
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

    _surface = rhv._surface ;
    rhv._surface = 0 ;

    _ndt = rhv._ndt ;
    rhv._ndt = 0 ;
}

context::this_ref_t context::operator = ( this_rref_t rhv ) 
{
    _display = rhv._display ;
    rhv._display = NULL ;

    _wnd = rhv._wnd ;
    rhv._wnd = 0 ;

    _context = rhv._context ;
    rhv._context = 0 ;

    _surface = rhv._surface ;
    rhv._surface = 0 ;

    _ndt = rhv._ndt ;
    rhv._ndt = 0 ;
    return *this ;
}

//****************************************************************
context::~context( void_t )
{
    this_t::deactivate() ;
}

//***************************************************************
natus::application::result context::activate( void_t ) 
{
    auto const res = eglMakeCurrent( _display, _surface, _surface, _context ) ;
    if( res == EGL_FALSE )
        return natus::application::result::failed ;

    return natus::application::result::ok ;
}

//***************************************************************
natus::application::result context::deactivate( void_t ) 
{
    auto const res = eglMakeCurrent( _display, EGL_NO_SURFACE, 
               EGL_NO_SURFACE, EGL_NO_CONTEXT ) ;
    if( res == EGL_FALSE )
        return natus::application::result::failed ;
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
    eglSwapBuffers( _display, _surface ) ;
    return natus::application::result::ok ;
}

//***************************************************************
natus::application::result context::is_extension_supported( 
    natus::std::string_cref_t extension_name ) 
{
    this_t::strings_t ext_list ;
    if( natus::application::no_success( get_egl_extension(ext_list) ) ) 
        return natus::application::result::failed_wgl ;

    this_t::strings_t::iterator iter = ext_list.begin() ;
    while( iter != ext_list.end() )
    {
        if( *iter == extension_name ) 
            return natus::application::result::ok ;
        ++iter ;
    }
    return natus::application::result::invalid_extension ;
}

//*****************************************************************
natus::application::result context::get_egl_extension( this_t::strings_out_t ext_list )
{
    char_cptr_t ch = eglQueryString( _display, EGL_EXTENSIONS ) ;
    if( !ch ) return natus::application::result::failed ;
    
    natus::std::string_t extension_string( (const char*)ch) ;
    natus::std::string_ops::split( extension_string, ' ', ext_list ) ;

    return natus::application::result::ok ;
}

//****************************************************************
natus::application::result context::get_es_extension( this_t::strings_out_t ext_list )
{
    const GLubyte * ch = glGetString( GL_EXTENSIONS ) ;
    if( !ch ) return natus::application::result::failed ;

    natus::std::string_t extension_string( (const char*)ch) ;
    natus::std::string_ops::split( extension_string, ' ', ext_list ) ;
    return natus::application::result::ok ;
}

//****************************************************************
natus::application::result context::get_es_version( natus::application::gl_version & version ) const 
{
    const GLubyte* ch = glGetString(GL_VERSION) ;
    if( !ch ) return natus::application::result::failed ;

    natus::std::string_t version_string = natus::std::string((const char*)ch) ;

    GLint major = 0;//boost::lexical_cast<GLint, std::string>(*token) ;
    GLint minor = 0;//boost::lexical_cast<GLint, std::string>(*(++token));

    {
        glGetIntegerv( GL_MAJOR_VERSION, &major ) ;
        GLenum err = glGetError() ;
        if( err != GL_NO_ERROR )
        {
            natus::std::string_t const es = ::std::to_string(err) ;
            natus::log::global::error( 
                "[context::get_gl_version] : get gl major <"+es+">" ) ;
        }
    }
    {
        glGetIntegerv( GL_MINOR_VERSION, &minor) ;
        GLenum err = glGetError() ;
        if( err != GL_NO_ERROR )
        {
            natus::std::string_t es = ::std::to_string(err) ;
            natus::log::global::error( "[context::get_gl_version] : get gl minor <"+es+">" ) ;
        }
    }

    version.major = major ;
    version.minor = minor ;

    return natus::application::result::ok ;
}


//****************************************************************
void_t context::clear_now( natus::math::vec4f_t const & vec ) 
{
    {
        glClearColor( vec.x(), vec.y(), vec.z(), vec.w() ) ;
        glGetError() ;
    }
    {
        glClear( GL_COLOR_BUFFER_BIT ) ;
        glGetError() ;
    }
}

//***************************************************************
natus::application::result context::create_the_context( gl_info_cref_t /*gli*/ ) 
{
    EGLConfig config ;
    EGLDisplay display = eglGetDisplay( _ndt ) ;

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

    {
        auto const res = this_t::is_extension_supported("EGL_KHR_create_context") ;

        int const es3_supported = res == natus::application::result::ok ?
            EGL_OPENGL_ES3_BIT_KHR : EGL_OPENGL_ES2_BIT ;
        EGLint numConfigs = 0 ;
        EGLint const  attribList[] = 
        {
            EGL_RED_SIZE, 5,
            EGL_GREEN_SIZE, 6,
            EGL_BLUE_SIZE, 5,
            EGL_ALPHA_SIZE, 1,
            EGL_DEPTH_SIZE, 1,
            EGL_STENCIL_SIZE, 1,
            EGL_SAMPLE_BUFFERS, 0,
            EGL_RENDERABLE_TYPE, es3_supported,
            EGL_NONE
        } ;

        if( !eglChooseConfig( display, attribList, &config, 1, &numConfigs ) )
        {
            natus::log::global_t::warning( natus_log_fn("eglChooseConfig") ) ;
            return natus::application::result::failed ;
        }

        if( numConfigs < 1 ) 
        {
            natus::log::global_t::warning( natus_log_fn("numConfigs < 1") ) ;
            return natus::application::result::failed ;
        }
    }

    {
        EGLSurface surface = eglCreateWindowSurface( 
                 display, config, _wnd, NULL ) ;
        if( surface == EGL_NO_SURFACE )
        {
            natus::log::global_t::warning( 
                natus_log_fn("eglCreateWindowSurface") ) ;
            return natus::application::result::failed ;
        }
        _surface = surface ;
    }

    {
        EGLint const  attribList[] = 
        {
            EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE
        } ;

        EGLContext context = eglCreateContext( display, config, 
                     EGL_NO_CONTEXT, attribList ) ;

        if( context == EGL_NO_CONTEXT )
        {
            natus::log::global_t::warning( natus_log_fn("eglCreateContext") ) ;
            return natus::application::result::failed ;
        }
        _context = context ;
    }

    {
        eglMakeCurrent( display, _surface, _surface, _context ) ;
        this_t::strings_t list ;
        this_t::get_es_extension( list ) ;
        glClearColor(1.0f,1.0f,1.0f,1.0f);
        glClear( GL_COLOR_BUFFER_BIT ) ;
        eglSwapBuffers( display, _surface ) ;
        glClearColor(1.0f,0.0f,1.0f,1.0f);
        glClear( GL_COLOR_BUFFER_BIT ) ;
        eglSwapBuffers( display, _surface ) ;

        eglMakeCurrent( display, EGL_NO_SURFACE, 
               EGL_NO_SURFACE, EGL_NO_CONTEXT ) ;
    }

    _display = display ;

    return natus::application::result::ok ;
}

