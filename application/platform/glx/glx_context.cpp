#include "glx_context.h"

#include <natus/ogl/gl/gl.h>
#include <natus/ogl/glx/glx.h>
#include <natus/std/string/split.hpp>

using namespace natus::application ;
using namespace natus::application::glx ;

//****************************************************************
context::context( void_t )
{
}

//****************************************************************
context::context( Window wnd, Display * disp ) 
{
    _display = disp ;
    _wnd = wnd ;
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
    glXMakeCurrent( _display, _wnd, NULL ) ;
    //XLockDisplay( _display ) ;
    glXMakeCurrent( _display, _wnd, _context ) ;
    //XUnlockDisplay( _display ) ;

    const GLenum glerr = natus::ogl::gl::glGetError( ) ;
    natus::log::global_t::warning( glerr != GL_NO_ERROR, 
            "[context::activate] : glXMakeCurrent" ) ;

    return natus::application::result::ok ;
}

//***************************************************************
natus::application::result context::deactivate( void_t ) 
{
    return natus::application::result::ok ;
}

//***************************************************************
natus::application::result context::vsync( bool_t /*on_off*/ ) 
{
    return natus::application::result::ok ;
}

//**************************************************************
natus::application::result context::swap( void_t ) 
{
    return natus::application::result::ok ;
}

//**************************************************************
natus::application::result context::create_context( 
     Display* display, Window wnd, GLXContext context ) 
{
    _display = display ;
    _wnd = wnd ;

    // the context comes in already create
    _context = context ;

    return natus::application::result::ok ;
}

//***************************************************************
natus::application::result context::is_extension_supported( 
    natus::std::string_cref_t extension_name ) 
{
    this_t::strings_t ext_list ;
    if( natus::application::no_success( get_glx_extension(ext_list) ) ) 
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
natus::application::result context::get_glx_extension( this_t::strings_out_t /*ext_list*/ )
{
    return natus::application::result::ok ;
}

//****************************************************************
natus::application::result context::get_gl_extension( this_t::strings_out_t ext_list )
{
    const GLubyte * ch = natus::ogl::gl::glGetString( GL_EXTENSIONS ) ;
    if( !ch ) return natus::application::result::failed ;

    natus::std::string_t extension_string( (const char*)ch) ;
    natus::std::string_ops::split( extension_string, ' ', ext_list ) ;
    return natus::application::result::ok ;
}

//****************************************************************
natus::application::result context::get_gl_version( natus::application::gl_version & version ) const 
{
    const GLubyte* ch = natus::ogl::gl::glGetString(GL_VERSION) ;
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

    return natus::application::result::ok ;
}

//****************************************************************
void_t context::clear_now( natus::math::vec4f_t const & vec ) 
{
    natus::ogl::gl::glClearColor( vec.x(), vec.y(), vec.z(), vec.w() ) ;
    natus::ogl::gl::glClear( GL_COLOR_BUFFER_BIT ) ;
    
    GLenum const gler = natus::ogl::gl::glGetError() ;
    natus::log::global_t::error( gler != GL_NO_ERROR, "[context::clear_now] : glClear" ) ;
}

//***************************************************************
natus::application::result context::create_the_context( gl_info_cref_t /*gli*/ ) 
{
    return natus::application::result::ok ;
}
