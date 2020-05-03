

#include "gl3.h"

#include "../../buffer/vertex_buffer.hpp"
#include "../../buffer/index_buffer.hpp"

#include <natus/ogl/gl/gl.h>
#include <natus/ogl/gl/convert.hpp>
#include <natus/ogl/gl/error.hpp>
#include <natus/memory/global.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/std/vector.hpp>
#include <natus/std/string/split.hpp>

using namespace natus::gpu ;

namespace this_file
{
    struct geo_config
    {
        natus::std::string_t name ;

        GLuint vb_id = GLuint( -1 ) ;
        GLuint ib_id = GLuint( -1 ) ;
        
        void_ptr_t vb_mem = nullptr ;
        void_ptr_t ib_mem = nullptr ;

        size_t num_elements_vb = 0 ;
        size_t num_elements_ib = 0 ;

        // per vertex sib
        GLuint stride = 0 ;
        struct layout_element
        {
            natus::gpu::vertex_attribute va ;
            natus::gpu::type type ;
            natus::gpu::type_struct type_struct ;

            GLuint sib( void_t ) const noexcept 
            {
                return GLuint( natus::gpu::size_of( type ) +
                    natus::gpu::size_of( type_struct ) ) ;
            }
        };
        natus::std::vector< layout_element > elements ;

        GLenum ib_type ;
        size_t ib_elem_sib = 0  ;
        GLenum pt ;
    };

    struct render_config
    {
        natus::std::string_t name ;

        GLuint vs_id = GLuint( -1 ) ;
        GLuint gs_id = GLuint( -1 ) ;
        GLuint ps_id = GLuint( -1 ) ;
        GLuint pg_id = GLuint( -1 ) ;
        GLuint va_id = GLuint( -1 ) ;

        struct attribute_variable
        {
            natus::gpu::vertex_attribute va ;
            natus::std::string_t name ;
            GLuint loc ;
            GLenum type ;
        };
        natus_typedef( attribute_variable ) ;

        natus::std::vector< attribute_variable_t > attributes ;

        struct uniform_variable
        {
            typedef ::std::function< void_t () > glUniform_funk_t ;

            natus::std::string_t name ;
            GLuint loc ;
            GLenum type ;
            void_ptr_t mem = nullptr ;
            natus::ogl::uniform_funk_t uniform_funk ;
        };
        natus_typedef( uniform_variable ) ;


        natus::std::vector< uniform_variable > uniforms ;

        void_ptr_t uniform_mem = nullptr ;

        geo_config* geo = nullptr ;
    };
}

struct gl3_backend::pimpl
{
    natus_this_typedefs( pimpl ) ;

    typedef natus::std::vector< this_file::render_config > rconfigs_t ;
    rconfigs_t rconfigs ;

    typedef natus::std::vector< this_file::geo_config > geo_configs_t ;
    geo_configs_t geo_configs ;

    pimpl( void_t ) 
    {}

    size_t construct_rconfig( size_t oid, natus::std::string_cref_t name, 
        natus::gpu::render_configuration_ptr_t config )  
    {
        // the name is unique
        {
            auto iter = ::std::find_if( rconfigs.begin(), rconfigs.end(),
                [&] ( this_file::render_config const& config )
            {
                return config.name == name ;
            } ) ;
            
            if( iter != rconfigs.end() )
            {
                size_t const i = (iter - rconfigs.begin()) ;
                if( natus::log::global_t::error( i != oid && oid != size_t( -1 ),
                    natus_log_fn( "name and id do not fit " ) ) )
                {
                    return oid ;
                }
            }
        }

        if( oid == size_t(-1) )
        {
            size_t i = 0 ;
            for( i; i < rconfigs.size(); ++i )
            {
                if( natus::core::is_not( rconfigs[ i ].va_id != GLuint( -1 ) ) )
                {
                    break ;
                }
            }
            oid = i ;
        }

        if( oid >= rconfigs.size() ) {
            rconfigs.resize( oid + 1 ) ;
        }

        bool_t error = false ;

        {
            rconfigs[ oid ].name = name ;
        }

        // vertex array object
        if( rconfigs[oid].va_id == GLuint(-1) )
        {
            GLuint id = GLuint( -1 ) ;
            natus::ogl::gl::glGenVertexArrays( 1, &id ) ;
            error = natus::ogl::error::check_and_log(
                natus_log_fn( "Vertex Array creation" ) ) ;

            rconfigs[ oid ].va_id = id ;
        }

        // program
        if( rconfigs[oid].pg_id == GLuint(-1) )
        {
            GLuint const id = natus::ogl::gl::glCreateProgram() ;
            error = natus::ogl::error::check_and_log(
                natus_log_fn( "Shader Program creation" ) ) ;

            rconfigs[ oid ].pg_id = id ;
        }
        {
            this_t::detach_shaders( rconfigs[ oid ].pg_id ) ;
            this_t::delete_all_variables( rconfigs[ oid ] ) ;
        }

        // vertex shader
        if( rconfigs[oid].vs_id == GLuint(-1) )
        {
            GLuint const id = natus::ogl::gl::glCreateShader( GL_VERTEX_SHADER ) ;
            error = natus::ogl::error::check_and_log(
                natus_log_fn( "Vertex Shader creation" ) ) ;

            rconfigs[ oid ].vs_id = id ;
        }
        {
            natus::ogl::gl::glAttachShader( rconfigs[ oid ].pg_id, rconfigs[oid].vs_id ) ;
            error = natus::ogl::error::check_and_log(
                natus_log_fn( "Attaching vertex shader" ) ) ;
        }

        // geometry shader
        if( config->has_geometry_shader() )
        {
            GLuint id = rconfigs[ oid ].gs_id ;

            if( id == GLuint(-1) )
            {
                id = natus::ogl::gl::glCreateShader( GL_GEOMETRY_SHADER ) ;
                error = natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader creation" ) ) ;

                rconfigs[ oid ].gs_id = id ;
            }

            GLuint const pid = rconfigs[ oid ].pg_id ;

            natus::ogl::gl::glAttachShader( pid, id ) ;
            error = natus::ogl::error::check_and_log(
                natus_log_fn( "Attaching geometry shader" ) ) ;

            // check max output vertices
            {
                GLint max_out = 0 ;
                natus::ogl::gl::glGetIntegerv( GL_MAX_GEOMETRY_OUTPUT_VERTICES, &max_out ) ;
                error = natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader Max Output Vertices" ) ) ;
                ( void_t ) max_out ;
            }

            // @todo geometry shader program parameters
            {
                /*
                natus::ogl::gl::glProgramParameteri( pid, GL_GEOMETRY_INPUT_TYPE, 
                    ( GLint ) so_gl::convert( shd_ptr->get_input_type() ) ) ;
                error = natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader Input Type" ) ) ;

                natus::ogl::gl::glProgramParameteri( pid, GL_GEOMETRY_OUTPUT_TYPE, 
                    ( GLint ) so_gl::convert( shd_ptr->get_output_type() ) ) ;
                error = natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader Output Type" ) ) ;

                natus::ogl::gl::glProgramParameteri( pid, GL_GEOMETRY_VERTICES_OUT, 
                    ( GLint ) shd_ptr->get_num_output_vertices() ) ;
                error = natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader Vertices Out" ) ) ;
                    */
            }
        }
        else if( rconfigs[ oid ].gs_id != GLuint(-1) )
        {
            natus::ogl::gl::glDeleteShader( rconfigs[ oid ].gs_id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDeleteShader" ) ) ;
            rconfigs[ oid ].gs_id = GLuint( -1 ) ;
        }

        // pixel shader
        if( config->has_pixel_shader() )
        {
            GLuint id = rconfigs[ oid ].ps_id ;
            if( id == GLuint(-1) )
            {
                id = natus::ogl::gl::glCreateShader( GL_FRAGMENT_SHADER ) ;
                error = natus::ogl::error::check_and_log(
                    natus_log_fn( "Fragment Shader creation" ) ) ;
            }
            
            natus::ogl::gl::glAttachShader( rconfigs[ oid ].pg_id, id ) ;
            error = natus::ogl::error::check_and_log( natus_log_fn( "Attaching pixel shader" ) ) ;

            rconfigs[ oid ].ps_id = id ;
        }
        else if( rconfigs[ oid ].ps_id != GLuint( -1 ) )
        {
            natus::ogl::gl::glDeleteShader( rconfigs[ oid ].ps_id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDeleteShader" ) ) ;
            rconfigs[ oid ].ps_id = GLuint( -1 ) ;
        }

        return oid ;
    }

    //***********************
    void_t detach_shaders( GLuint const program_id )
    {
        GLsizei count = 0 ;
        GLuint shaders[ 10 ] ;

        natus::ogl::gl::glGetAttachedShaders( program_id, 10, &count, shaders ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glGetAttachedShaders" ) ) ;

        for( GLsizei i = 0; i < count; ++i )
        {
            natus::ogl::gl::glDetachShader( program_id, shaders[ i ] ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDetachShader" ) ) ;
        }
    }

    //***********************
    void_t delete_all_variables( this_file::render_config & config )
    {
        for( auto & v : config.attributes )
        {
            // delete memory
        }
        config.attributes.clear() ;

        for( auto & v : config.uniforms )
        {
            // delete memory
            v.mem = nullptr ;
        }
        config.uniforms.clear() ;
        natus::memory::global_t::dealloc( config.uniform_mem ) ;
    }

    //***********************
    bool_t compile_shader( GLuint const id, natus::std::string_cref_t code )
    {
        if( code.empty() ) return true ;

        GLchar const* source_string = ( GLchar const* ) ( code.c_str() ) ;

        natus::ogl::gl::glShaderSource( id, 1, &source_string, 0 ) ;
        if( natus::ogl::error::check_and_log( natus_log_fn( "glShaderSource" ) ) )
            return false ;

        natus::ogl::gl::glCompileShader( id ) ;
        if( natus::ogl::error::check_and_log( natus_log_fn( "glCompileShader" ) ) )
            return false ;

        GLint ret ;
        natus::ogl::gl::glGetShaderiv( id, GL_COMPILE_STATUS, &ret ) ;

        GLint length ;
        natus::ogl::gl::glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length ) ;

        if( ret == GL_TRUE && length <= 1 ) return true ;

        if( natus::log::global::error( length == 0, 
            natus_log_fn( "shader compilation failed, but info log length is 0." ) ) )
            return false ;

        // print first line for info
        // user can place the shader name or any info there.
        {
            size_t pos = code.find_first_of( '\n' ) ;
            natus::log::global::error( natus_log_fn( "First Line: " + code.substr( 0, pos ) ) ) ;
        }

        // get the error message it is and print it
        {
            natus::memory::malloc_guard<char> info_log( length ) ;

            natus::ogl::gl::glGetShaderInfoLog( id, length, 0, info_log ) ;

            natus::std::vector< natus::std::string_t > tokens ;
            natus::std::string_ops::split( natus::std::string_t( info_log ), '\n', tokens ) ;

            for( auto const & msg : tokens )
            {
                natus::log::global::error( msg ) ;
            }
        }
        return true ;
    }

    //***********************
    bool_t link( GLuint const program_id )
    {
        natus::ogl::gl::glLinkProgram( program_id ) ;
        if( natus::ogl::error::check_and_log( natus_log_fn( "glLinkProgram" ) ) )
            return false ;

        {
            GLint ret ;
            natus::ogl::gl::glGetProgramiv( program_id, GL_LINK_STATUS, &ret ) ;

            GLint length ;
            natus::ogl::gl::glGetProgramiv( program_id, GL_INFO_LOG_LENGTH, &length ) ;

            if( ret == GL_TRUE && length <= 1 ) 
                return true ;

            if( natus::log::global_t::error( length == 0, natus_log_fn("unknown") ) )
                return false ;

            natus::memory::malloc_guard<char> info_log( length ) ;

            natus::ogl::gl::glGetProgramInfoLog( program_id, length, 0, info_log ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glGetProgramInfoLog" ) ) )
                return false ;

            std::string info_log_string = std::string( ( const char* ) info_log ) ;

            {
                natus::std::vector< natus::std::string_t > tokens ;
                natus::std::string_ops::split( natus::std::string_t( info_log ), '\n', tokens ) ;

                for( auto token : tokens )
                {
                    natus::log::global_t::error( token ) ;
                }
            }
        }
        return false ;
    }

    //***********************
    void_t post_link_attributes( GLuint const program_id, this_file::render_config * config, 
        natus::gpu::render_configuration_cref_t rc )
    {
        GLint num_active_attributes = 0 ;
        GLint name_length = 0 ;

        natus::ogl::gl::glGetProgramiv( program_id, GL_ACTIVE_ATTRIBUTES, &num_active_attributes ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glGetProgramiv(GL_ACTIVE_ATTRIBUTES)" ) ) ;

        if( num_active_attributes == 0 ) return ;

        config->attributes.resize( num_active_attributes ) ;

        natus::ogl::gl::glGetProgramiv( program_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &name_length ) ;
        natus::ogl::error::check_and_log( natus_log_fn( 
            "glGetProgramiv(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH)" ) ) ;

        GLint size ;
        GLenum gl_attrib_type ;

        natus::memory::malloc_guard<char> buffer( name_length ) ;
        
        for( GLint i = 0; i < num_active_attributes; ++i )
        {
            natus::ogl::gl::glGetActiveAttrib( program_id, i, name_length, 0, 
                &size, &gl_attrib_type, buffer ) ;

            if( natus::ogl::error::check_and_log( "glGetActiveAttrib failed. continue loop." ) ) continue ;

            // build-ins are not needed in the shader variables container.
            if( name_length >= 3 && buffer.equals( 0, 'g' ) && buffer.equals( 1, 'l' ) && buffer.equals( 2, '_' ) ) continue ;

            GLuint location_id = natus::ogl::gl::glGetAttribLocation( program_id, buffer ) ;
            if( natus::ogl::error::check_and_log( "glGetAttribLocation failed. continue loop." ) ) continue ;

            natus::std::string_t const variable_name = natus::std::string_t( ( const char* ) buffer ) ;

            this_file::render_config::attribute_variable_t vd ;
            vd.name = ::std::move( variable_name ) ;
            vd.loc = location_id ;
            vd.type = gl_attrib_type ;
            
            {
                natus::gpu::vertex_attribute va = natus::gpu::vertex_attribute::undefined ;
                auto const res = rc.get_vertex_shader().find_input_binding_by_name(vd.name, va) ;
                natus::log::global_t::error( natus::core::is_not( res ), 
                    natus_log_fn("can not find vertex attribute - " + vd.name ) ) ;
                vd.va = va ;
            }
            config->attributes[i] = vd ;
        }
    }

    //***********************
    bool_t bind_attributes( natus::gpu::render_configuration_cref_t rc, this_file::render_config * config, this_file::geo_config * gconfig )
    {
        // bind vertex array object
        {
            natus::ogl::gl::glBindVertexArray( config->va_id ) ;
            if( natus::ogl::error::check_and_log(
                natus_log_fn( "glBindVertexArray" ) ) )
                return false ;
        }

        // bind vertex buffer
        {
            natus::ogl::gl::glBindBuffer( GL_ARRAY_BUFFER, gconfig->vb_id ) ;
            if( natus::ogl::error::check_and_log( 
                natus_log_fn("glBindBuffer(GL_ARRAY_BUFFER)") ) ) 
                return false ;
        }

        // bind index buffer
        {
            natus::ogl::gl::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gconfig->ib_id ) ;
            if( natus::ogl::error::check_and_log( 
                natus_log_fn( "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)" ) ) )
                return false ;
        }

        // disable old locations
        for( size_t i = 0; i < config->attributes.size(); ++i )
        {
            natus::ogl::gl::glDisableVertexAttribArray( config->attributes[ i ].loc ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDisableVertexAttribArray" ) ) ;
        }

        GLuint uiStride = gconfig->stride ;
        GLuint uiBegin = 0 ;
        GLuint uiOffset = 0 ;

        for( auto & e : gconfig->elements )
        {
            uiBegin += uiOffset ;
            uiOffset = e.sib() ;

            auto iter = ::std::find_if( config->attributes.begin(), config->attributes.end(), 
                [&]( this_file::render_config::attribute_variable_cref_t av )
            {
                return av.va == e.va ;
            } ) ;

            if( iter == config->attributes.end() ) 
            {
                natus::log::global_t::warning( natus_log_fn("invalid vertex attribute") ) ;
                continue ;
            }
            natus::ogl::gl::glEnableVertexAttribArray( iter->loc ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "glEnableVertexAttribArray" ) ) ;

            natus::ogl::gl::glVertexAttribPointer(
                iter->loc,
                GLint( natus::gpu::size_of(e.type_struct) ),
                natus::ogl::complex_to_simple_type( iter->type ),
                GL_FALSE,
                ( GLsizei ) uiStride,
                (const GLvoid*)(size_t)uiBegin 
                ) ;

            natus::ogl::error::check_and_log( natus_log_fn( "glVertexAttribPointer" ) ) ;
        }

        // unbind everything
        {
            natus::ogl::gl::glBindVertexArray( 0 ) ;
            natus::ogl::gl::glBindBuffer( GL_ARRAY_BUFFER, 0 ) ;
            natus::ogl::gl::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "unbind" ) ) ;
        }
        return true ;
    }

    //***********************
    void_t post_link_uniforms( GLuint const program_id, this_file::render_config & config )
    {
        GLint num_active_uniforms = 0 ;
        GLint name_length = 0 ;

        natus::ogl::gl::glGetProgramiv( program_id, GL_ACTIVE_UNIFORMS, &num_active_uniforms ) ;
        natus::ogl::error::check_and_log( "[glGetProgramiv] : GL_ACTIVE_UNIFORMS" ) ;

        if( num_active_uniforms == 0 ) return ;

        config.uniforms.resize( num_active_uniforms ) ;

        natus::ogl::gl::glGetProgramiv( program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_length ) ;
        natus::ogl::error::check_and_log( "[glGetProgramiv] : GL_ACTIVE_UNIFORM_MAX_LENGTH" ) ;

        GLint size ;
        GLenum gl_uniform_type ;

        natus::memory::malloc_guard<char> buffer( name_length ) ;

        for( GLint i = 0; i < num_active_uniforms; ++i )
        {
            natus::ogl::gl::glGetActiveUniform( program_id, i, name_length, 0, 
                &size, &gl_uniform_type, buffer ) ;
            if( natus::ogl::error::check_and_log( "[gl_33_api] : glGetActiveUniform" ) ) continue ;

            // build-ins are not needed in the shader variables container.
            if( name_length >= 3 && buffer.equals( 0, 'g' ) && buffer.equals( 1, 'l' ) && buffer.equals( 2, '_' ) ) continue ;

            GLuint const location_id = natus::ogl::gl::glGetUniformLocation( program_id, buffer ) ;
            if( natus::ogl::error::check_and_log( "[glGetUniformLocation]" ) ) continue ;

            if( natus::log::global_t::error( location_id == GLuint( -1 ), 
                natus_log_fn( "invalid uniform location id." ) ) ) continue ;

            natus::std::string const variable_name = natus::std::string( char_cptr_t( buffer ) ) ;

            this_file::render_config::uniform_variable_t vd ;
            vd.name = ::std::move( variable_name ) ;
            vd.loc = location_id ;
            vd.type = gl_uniform_type ;
            vd.uniform_funk = natus::ogl::uniform_funk( gl_uniform_type ) ;

            config.uniforms[ i ] = vd ;
        }

        // alloc memory for all uniforms
        {
            size_t sib = 0 ;
            for( auto& v : config.uniforms )
            {
                sib += natus::ogl::uniform_size_of( v.type ) ;
            }
            config.uniform_mem = natus::memory::global_t::alloc( sib, 
                natus_log_fn("uniform memory block") ) ;
        }

        // assign ptr
        {
            size_t sib = 0 ;
            for( auto& v : config.uniforms )
            {
                v.mem = void_ptr_t( byte_ptr_t( config.uniform_mem ) + sib ) ;
                sib += natus::ogl::uniform_size_of( v.type ) ;
            }
        }
    }

    bool_t update( size_t const id, natus::gpu::render_configuration_cref_t rc )
    {
        auto& config = rconfigs[ id ] ;
        config.geo = nullptr ;

        this_file::geo_config* gconfig = nullptr ;

        // search for geometry
        {
            auto iter = ::std::find_if( geo_configs.begin(), geo_configs.end(),
                [&] ( this_file::geo_config const& c )
            {
                return c.name == rc.get_geometry() ;
            } ) ;
            if( iter == geo_configs.end() )
            {
                natus::log::global_t::error( natus_log_fn("geometry does not exist - " + 
                    rc.get_geometry() ) ) ;
                return false ;
            }
            gconfig = &(geo_configs[ iter - geo_configs.begin() ]) ;
        }

        // the geometry configuration is required
        // by a render configuration
        {
            config.geo = gconfig ;
        }

        {
            // pre link
        }

        // compile vertex shader
        {
            this_t::compile_shader( config.vs_id, rc.get_vertex_shader().code() ) ;
            this_t::compile_shader( config.gs_id, rc.get_geometry_shader().code() ) ;
            this_t::compile_shader( config.ps_id, rc.get_pixel_shader().code() ) ;
            this_t::link( config.pg_id ) ;
        }

        // vao
        {
        }
        
        {
            this_t::post_link_attributes( config.pg_id, &config, rc ) ;
            this_t::bind_attributes( rc, &config, gconfig ) ;
        }

        
        {
            this_t::post_link_uniforms( config.pg_id, config ) ;
            // link uniforms with variables
        }

        return true ;
    }

    size_t construct_geo( natus::std::string_cref_t name, natus::gpu::geometry_configuration_ref_t geo ) 
    {
        // the name is unique
        {
            auto iter = ::std::find_if( geo_configs.begin(), geo_configs.end(), 
                [&]( this_file::geo_config const & config )
            {
                return config.name == name ;
            } ) ;

            if( iter != geo_configs.end() )
                return iter - geo_configs.begin() ;
        }

        size_t i = 0 ;
        for( i; i < geo_configs.size(); ++i )
        {
            if( natus::core::is_not( geo_configs[ i ].vb_id != GLuint(-1) ) )
            {
                break ;
            }
        }

        if( i == geo_configs.size() ) {
            geo_configs.resize( i + 1 ) ;
        }

        bool_t error = false ;

        // vertex buffer
        {
            GLuint id = GLuint( -1 ) ;
            natus::ogl::gl::glGenBuffers( 1, &id ) ;
            error = natus::ogl::error::check_and_log( 
                natus_log_fn( "Vertex Buffer creation" ) ) ;
            
            geo_configs[ i ].vb_id = id ;
        }

        // index buffer
        {
            GLuint id = GLuint( -1 ) ;
            natus::ogl::gl::glGenBuffers( 1, &id ) ;
            error = natus::ogl::error::check_and_log(
                natus_log_fn( "Index Buffer creation" ) ) ;
            
            geo_configs[ i ].ib_id = id ;
        }

        {
            geo_configs[ i ].name = name ;
            geo_configs[ i ].stride = GLuint( geo.vertex_buffer().get_layout_sib() ) ;
            geo.vertex_buffer().for_each_layout_element( 
                [&]( natus::gpu::vertex_buffer_t::data_cref_t d )
            {
                this_file::geo_config::layout_element le ;
                le.va = d.va ;
                le.type = d.type ;
                le.type_struct = d.type_struct ;
                geo_configs[ i ].elements.push_back( le ) ;
            }) ;
        }

        {
            //#error "set all these new values"
            geo_configs[ i ].num_elements_ib = geo.index_buffer().get_num_elements() ;
            geo_configs[ i ].num_elements_vb = geo.vertex_buffer().get_num_elements() ;
            geo_configs[ i ].ib_elem_sib = geo.index_buffer().get_element_sib() ;
            geo_configs[ i ].ib_type = GL_UNSIGNED_INT ;
            geo_configs[ i ].pt = GL_TRIANGLE_FAN ; //  geo.primitive_type() ;
        }

        return i ;
    }

    bool_t update( size_t const id, natus::gpu::geometry_configuration_res_t geo )
    {
        auto& config = geo_configs[ id ] ;

        // bind vertex buffer
        {
            natus::ogl::gl::glBindBuffer( GL_ARRAY_BUFFER, config.vb_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn("glBindBuffer - vertex buffer") ) )
                return false ;
        }

        // allocate buffer memory
        // what about mapped memory?
        {
            GLuint const sib = GLuint( geo->vertex_buffer().get_sib() ) ;
            natus::ogl::gl::glBufferData( GL_ARRAY_BUFFER, sib, 
                geo->vertex_buffer().data(), GL_STATIC_DRAW ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBufferData - vertex buffer" ) ) )
                return false ;
        }

        // bind index buffer
        {
            natus::ogl::gl::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, config.ib_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBindBuffer - index buffer" ) ) )
                return false ;
        }

        // allocate buffer memory
        // what about mapped memory?
        {
            GLuint const sib = GLuint( geo->index_buffer().get_sib() ) ;
            natus::ogl::gl::glBufferData( GL_ELEMENT_ARRAY_BUFFER, sib, 
                geo->index_buffer().data(), GL_STATIC_DRAW ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBufferData - index buffer" ) ) )
                return false ;
        }

        return true ;
    }

    bool_t render( size_t const id, GLsizei const start_element = GLsizei(0), 
        GLsizei const num_elements = GLsizei(-1) )
    {
        this_file::render_config& config = rconfigs[ id ] ;

        // bind vertex array object
        {
            natus::ogl::gl::glBindVertexArray( config.va_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBindVertexArray" ) ) )
                return false ;
        }

        {
            natus::ogl::gl::glUseProgram( config.pg_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glUseProgram" ) ) )
                return false ;
        }

        // render section
        {
            GLenum const pt = GL_TRIANGLES ;
            GLuint const vb = config.geo->vb_id ;
            GLuint const ib = config.geo->ib_id ;

            if( ib != size_t(-1) )
            {
                GLsizei const max_elems = GLsizei( config.geo->num_elements_ib ) ;
                GLsizei const ne = ::std::min( num_elements>=0?num_elements:max_elems, max_elems ) ;

                GLenum const glt = config.geo->ib_type ;
                
                void_cptr_t offset = void_cptr_t( byte_cptr_t( nullptr ) + 
                    start_element * GLsizei( config.geo->ib_elem_sib ) ) ;
                
                natus::ogl::gl::glDrawElements( pt, ne, glt, offset ) ;

                natus::ogl::error::check_and_log( natus_log_fn( "glDrawElements" ) ) ;
            }
            else
            {
                GLsizei const max_elems = GLsizei( config.geo->num_elements_vb ) ;
                GLsizei const ne = ::std::min( num_elements, max_elems ) ;

                natus::ogl::gl::glDrawArrays( pt, start_element, ne ) ;

                natus::ogl::error::check_and_log( natus_log_fn( "glDrawArrays" ) ) ;
            }
        }
        return true ;
    }
};

//************************************************************************************************
//
//
//************************************************************************************************

//****
gl3_backend::gl3_backend( void_t ) noexcept
{
    _pimpl = natus::memory::global_t::alloc( pimpl(), 
        natus_log_fn("gl3_backend::pimpl") ) ;
}

//****
gl3_backend::gl3_backend( this_rref_t rhv ) noexcept : backend( ::std::move( rhv ) )
{
    natus_move_member_ptr( _pimpl, rhv ) ;
}

//****
gl3_backend::~gl3_backend( void_t ) 
{
    if( natus::core::is_not_nullptr( _pimpl ) )
        natus::memory::global_t::dealloc( _pimpl ) ;
}

//****
natus::gpu::id_t gl3_backend::configure( id_rref_t id,
    natus::gpu::geometry_configuration_res_t gconf ) noexcept 
{
    if( id.is_not_valid() )
    {
        id = natus::gpu::id_t( this_t::get_bid(),
            _pimpl->construct_geo( gconf->name(), *(gconf.get_sptr()) ) ) ;
    }

    if( id.is_not_bid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "incorrect backend" ) ) ;
        return ::std::move( id ) ;
    }

    {
        auto const res = _pimpl->update( id.get_oid(), gconf ) ;
        if( natus::core::is_not( res ) )
        {
            return ::std::move( id ) ;
        }
    }

    return ::std::move( id ) ;
}

//****
natus::gpu::id_t gl3_backend::configure( id_rref_t id, natus::gpu::render_configurations_res_t configs ) noexcept 
{
    natus::gpu::render_configuration_t config( natus::gpu::backend_type::unknown ) ;
    if( natus::core::is_not(configs->find_configuration( natus::gpu::backend_type::gl3, config )) )
    {
        natus::log::global_t::warning( natus_log_fn("render configuration backend missing - " + 
             natus::gpu::to_string(natus::gpu::backend_type::gl3) ) ) ;
        return natus::gpu::id_t() ;
    }
    
    {
        id = natus::gpu::id_t( this_t::get_bid(),
            _pimpl->construct_rconfig( id.get_oid(), config.name(), &config ) ) ;
    }

    if( id.is_not_bid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return ::std::move( id ) ;
    }

    size_t const oid = id.get_oid() ;

    {
        auto const res = _pimpl->update( id.get_oid(), config ) ;
        if( natus::core::is_not( res ) )
        {
            return ::std::move( id ) ;
        }
    }

    return ::std::move( id ) ;
}

//****
natus::gpu::id_t gl3_backend::render( id_rref_t id ) noexcept 
{ 
    //natus::log::global_t::status( natus_log_fn("render") ) ;

    if( id.is_not_bid( this_t::get_bid() ) || id.is_not_valid() )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return ::std::move( id ) ;
    }

    _pimpl->render( id.get_oid() ) ;

    return ::std::move( id ) ;
}
