//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#include "glx_context.h"

#include "../xlib/xlib_window_handle.h"
#include "../../log.h"

#include <snakeoil/gli/gl/gl.h>
#include <snakeoil/core/assert.h>

#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>



using namespace so_app ;
using namespace so_app::so_xlib ;

//***********************************************************************
glx_context::glx_context( void_t )
{  
    // create dummy context
    // init glx functions
#if 0
    GLenum const err = glewInit() ;
    if( so_app::log::error( GLEW_OK != err, 
            "[glx_context::init_glew] : glewInit() failed." ) )
    {
        return so_app::failed ;
    }
    
    glewExperimental = GL_TRUE ;
    GLenum const err = glxewInit() ;
    if( so_app::log::error( GLEW_OK != err, 
            "[glx_context::glx_context] : wglewInit()" ) )
    {
        exit(1) ;
    }    
#endif
}

//***********************************************************************
glx_context::glx_context( this_rref_t rhv )
{
    _display = rhv._display ;
    rhv._display = NULL ;
    
    _wnd = rhv._wnd ;
    rhv._wnd = 0 ;
    
    _context = rhv._context ;
    rhv._context = 0 ;
}

//***********************************************************************
glx_context::~glx_context( void_t )
{
    this_t::deactivate() ;

    
}

//***********************************************************************
so_app::result glx_context::activate( void_t ) 
{
    glXMakeCurrent( _display, _wnd, NULL ) ;
    //XLockDisplay( _display ) ;
    glXMakeCurrent( _display, _wnd, _context ) ;
    //XUnlockDisplay( _display ) ;

    const GLenum glerr = so_gli::gl::glGetError( ) ;
    so_app::log::warning( glerr != GL_NO_ERROR, 
            "[glx_context::activate] : glXMakeCurrent" ) ;
    
    return so_app::ok ;
}

//***********************************************************************
so_app::result glx_context::deactivate( void_t ) 
{
    return so_app::ok ;
}

//***********************************************************************
so_app::result glx_context::vsync( bool_t /*on_off*/ ) 
{
    return so_app::ok ;
}

//***********************************************************************
so_app::result glx_context::swap( void_t ) 
{
    return so_app::ok ;
}

//***********************************************************************
irender_context_ptr_t glx_context::create_shared_context( void_t )
{    
    return this_t::create_shared() ;
}

//***********************************************************************
so_app::result glx_context::create_context( Display* display, Window wnd, GLXContext context ) 
{
    _display = display ;
    _wnd = wnd ; 
    
    // the context comes in already create
    _context = context ;
    
      
    return so_app::ok ;
}

//***********************************************************************
glx_context::this_ptr_t glx_context::create_shared( void_t ) 
{
    return nullptr ;
}

//***********************************************************************
void_t glx_context::destroy_shared( this_ptr_t shared_ptr ) 
{
    so_app::memory::dealloc( shared_ptr ) ;
}

//***********************************************************************
so_app::result glx_context::is_extension_supported( std::string const & extension_name ) 
{
    std::list< std::string > ext_list ;
    if( so_app::no_success( get_glx_extension(ext_list) ) ) 
        return so_app::failed_wgl ;

    std::list< std::string >::iterator iter = ext_list.begin() ;
    while( iter != ext_list.end() )
    {
            if( *iter == extension_name ) return so_app::ok ;
            ++iter ;
    }
    return so_app::invalid_extension ;
}

//***********************************************************************
so_app::result glx_context::get_glx_extension( std::list< std::string > & /*ext_list*/ )
{
    return so_app::ok ;
}

//***********************************************************************
so_app::result glx_context::get_gl_extension( std::list< std::string > & ext_list )
{
    const GLubyte * ch = so_gli::gl::glGetString( GL_EXTENSIONS ) ;
    if( !ch ) return so_app::failed ;

    std::string extension_string( (const char*)ch) ;
    typedef boost::tokenizer< boost::char_separator<char> > tokenizer_t ;
    tokenizer_t tokens( extension_string, boost::char_separator<char>(" ") ) ;
    for( tokenizer_t::const_iterator token=tokens.begin(); token!=tokens.end(); ++token )
        {ext_list.push_back(*token) ;}

    return so_app::ok ;
}

//***********************************************************************
so_app::result glx_context::get_gl_version( so_app::gl_version & version ) const 
{
    const GLubyte* ch = so_gli::gl::glGetString(GL_VERSION) ;
    if( !ch ) return failed ;

    std::string version_string = std::string((const char*)ch) ;

    GLint major = 0;//boost::lexical_cast<GLint, std::string>(*token) ;
    GLint minor = 0;//boost::lexical_cast<GLint, std::string>(*(++token));

    {
        so_gli::gl::glGetIntegerv( GL_MAJOR_VERSION, &major ) ;
        GLenum err = so_gli::gl::glGetError() ;
        if( err != GL_NO_ERROR )
        {
            std::string es = boost::lexical_cast<std::string, GLenum>(err) ;
            so_app::log::error( "[glx_context::get_gl_version] : get gl major <"+es+">" ) ;
        }
    }
    {
        so_gli::gl::glGetIntegerv( GL_MINOR_VERSION, &minor) ;
        GLenum err = so_gli::gl::glGetError() ;
        if( err != GL_NO_ERROR )
        {
            std::string es = boost::lexical_cast<std::string, GLenum>(err) ;
            so_app::log::error( "[glx_context::get_gl_version] : get gl minor <"+es+">" ) ;
        }
    }
    version.major = major ;
    version.minor = minor ;

    return so_app::ok ;
}

//***********************************************************************
void_t glx_context::clear_now( so_math::vec4f_t const & vec ) 
{
    so_gli::gl::glClearColor( vec.x(), vec.y(), vec.z(), vec.w() ) ;
    so_gli::gl::glClear( GL_COLOR_BUFFER_BIT ) ;
    
    GLenum const gler = so_gli::gl::glGetError() ;
    so_app::log::error( gler != GL_NO_ERROR, "[glx_context::clear_now] : glClear" ) ;
}

//***********************************************************************
so_app::result glx_context::create_the_context( gl_info_cref_t /*gli*/ ) 
{
    return so_app::ok ;
}
