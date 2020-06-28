

#include "gl3.h"
#include "gl3_convert.h"

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
        GLuint va_id = GLuint( -1 ) ;

        void_ptr_t vb_mem = nullptr ;
        void_ptr_t ib_mem = nullptr ;

        size_t num_elements_vb = 0 ;
        size_t num_elements_ib = 0 ;
        
        size_t sib_vb = 0 ;
        size_t sib_ib = 0 ;

        struct layout_element
        {
            natus::gpu::vertex_attribute va ;
            natus::gpu::type type ;
            natus::gpu::type_struct type_struct ;

            GLuint sib( void_t ) const noexcept 
            {
                return GLuint( natus::gpu::size_of( type ) *
                    natus::gpu::size_of( type_struct ) ) ;
            }
        };
        natus::std::vector< layout_element > elements ;

        // per vertex sib
        GLuint stride = 0 ;

        GLenum ib_type ;
        size_t ib_elem_sib = 0  ;
        GLenum pt ;
    };

    struct shader_config
    {
        natus::std::string_t name ;

        GLuint vs_id = GLuint( -1 ) ;
        GLuint gs_id = GLuint( -1 ) ;
        GLuint ps_id = GLuint( -1 ) ;
        GLuint pg_id = GLuint( -1 ) ;

        struct vertex_input_binding
        {
            natus::gpu::vertex_attribute va ;
            natus::std::string_t name ;
        };
        natus::std::vector< vertex_input_binding > vertex_inputs ;
        
        bool_t find_vertex_input_binding_by_name( natus::std::string_cref_t name_,
            natus::gpu::vertex_attribute & va ) const noexcept
        {
            auto iter = ::std::find_if( vertex_inputs.begin(), vertex_inputs.end(),
                [&] ( vertex_input_binding const& b )
            {
                return b.name == name_ ;
            } ) ;
            if( iter == vertex_inputs.end() ) return false ;

            va = iter->va ;

            return true ;
        }

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
            natus::std::string_t name ;
            GLuint loc ;
            GLenum type ;

            // this variable's memory location
            void_ptr_t mem = nullptr ;

            // the GL uniform function
            natus::ogl::uniform_funk_t uniform_funk ;

            // set by the user in user-space
            //natus::gpu::ivariable_ptr_t var ;

            void_t do_uniform_funk( void_t )
            {
                uniform_funk( loc, 1, mem ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glUniform" ) ) ;
            }

            void_t do_copy_funk( natus::gpu::ivariable_ptr_t var )
            {
                ::std::memcpy( mem, var->data_ptr(), natus::ogl::uniform_size_of( type ) ) ;
            }
        };
        natus_typedef( uniform_variable ) ;

        natus::std::vector< uniform_variable > uniforms ;

        void_ptr_t uniform_mem = nullptr ;
    } ;

    struct render_config
    {
        natus::std::string_t name ;

        geo_config* geo = nullptr ;
        shader_config* shaders_ptr = nullptr ;

        struct uniform_variable_link
        {
            // the index into the shader_config::uniforms array
            size_t uniform_id ;
            // the user variable holding the data.
            natus::gpu::ivariable_ptr_t var ;
        };

        // user provided variable set
        natus::std::vector< ::std::pair< 
            natus::gpu::variable_set_res_t, 
            natus::std::vector< uniform_variable_link > > > var_sets_data ;
        
        struct uniform_texture_link
        {
            // the index into the shader_config::uniforms array
            size_t uniform_id ;
            GLint tex_id ;
            size_t img_id ;
        };
        natus::std::vector< ::std::pair<
            natus::gpu::variable_set_res_t,
            natus::std::vector< uniform_texture_link > > > var_sets_texture ;
        
    };

    struct image_config
    {
        natus::std::string_t name ;

        GLuint tex_id = GLuint( -1 ) ;
        size_t sib = 0 ;

        GLenum wrap_types[ 3 ] ;
        GLenum filter_types[ 2 ] ;

        // sampler ids for gl>=3.3
    };
}

struct gl3_backend::pimpl
{
    natus_this_typedefs( pimpl ) ;

    typedef natus::std::vector< this_file::shader_config > shaders_t ;
    shaders_t shaders ;

    typedef natus::std::vector< this_file::render_config > rconfigs_t ;
    rconfigs_t rconfigs ;

    typedef natus::std::vector< this_file::geo_config > geo_configs_t ;
    geo_configs_t geo_configs ;

    typedef natus::std::vector< this_file::image_config > image_configs_t ;
    image_configs_t img_configs ;

    GLsizei vp_width = 0 ;
    GLsizei vp_height = 0 ;

    natus::gpu::backend_type const bt = natus::gpu::backend_type::gl3 ;

    // the current render state set
    natus::gpu::render_state_sets_t render_states ;

    pimpl( void_t ) 
    {}

    size_t construct_shader_config( size_t oid, natus::std::string_cref_t name,
        natus::gpu::shader_configuration_ref_t config )
    {
        //
        // Array Management
        //

        // the name must be unique
        {
            auto iter = ::std::find_if( shaders.begin(), shaders.end(),
                [&] ( this_file::shader_config const& c )
            {
                return c.name == name ;
            } ) ;

            if( iter != shaders.end() )
            {
                size_t const i = ( iter - shaders.begin() ) ;
                if( natus::log::global_t::error( i != oid && oid != size_t( -1 ),
                    natus_log_fn( "name and id do not fit " ) ) )
                {
                    return oid ;
                }
            }
        }

        if( oid == size_t( -1 ) )
        {
            size_t i = 0 ;
            for( ; i < shaders.size(); ++i )
            {
                // here the vertex shader id is used for validity check.
                // if invalid, that object in the array will be repopulated
                if( natus::core::is_not( shaders[ i ].vs_id != GLuint( -1 ) ) )
                {
                    break ;
                }
            }
            oid = i ;
        }

        if( oid >= shaders.size() ) 
        {
            shaders.resize( oid + 1 ) ;
        }

        //
        // Do Configuration
        //

        {
            shaders[ oid ].name = name ;
        }

        // program
        if( shaders[ oid ].pg_id == GLuint( -1 ) )
        {
            GLuint const id = natus::ogl::gl::glCreateProgram() ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "Shader Program creation" ) ) ;

            shaders[ oid ].pg_id = id ;
        }
        {
            this_t::detach_shaders( shaders[ oid ].pg_id ) ;
            this_t::delete_all_variables( shaders[ oid ] ) ;
        }

        // vertex shader
        if( shaders[oid].vs_id == GLuint(-1) )
        {
            GLuint const id = natus::ogl::gl::glCreateShader( GL_VERTEX_SHADER ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "Vertex Shader creation" ) ) ;

            shaders[ oid ].vs_id = id ;
        }
        {
            natus::ogl::gl::glAttachShader( shaders[ oid ].pg_id, shaders[oid].vs_id ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "Attaching vertex shader" ) ) ;
        }

        natus::gpu::shader_set_t ss ;
        {
            auto const res = config.shader_set( this_t::bt, ss ) ;
            if( natus::core::is_not(res) )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "config [" + config.name() + "] has no shaders for " + 
                    natus::gpu::to_string( this_t::bt ) ) ) ;
                return oid ;
            }
        }

        // geometry shader
        if( ss.has_geometry_shader() )
        {
            GLuint id = shaders[ oid ].gs_id ;

            if( id == GLuint(-1) )
            {
                id = natus::ogl::gl::glCreateShader( GL_GEOMETRY_SHADER ) ;
                natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader creation" ) ) ;

                shaders[ oid ].gs_id = id ;
            }

            GLuint const pid = shaders[ oid ].pg_id ;

            natus::ogl::gl::glAttachShader( pid, id ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "Attaching geometry shader" ) ) ;

            // check max output vertices
            {
                GLint max_out = 0 ;
                natus::ogl::gl::glGetIntegerv( GL_MAX_GEOMETRY_OUTPUT_VERTICES, &max_out ) ;
                natus::ogl::error::check_and_log(
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
        else if( shaders[ oid ].gs_id != GLuint(-1) )
        {
            natus::ogl::gl::glDeleteShader( shaders[ oid ].gs_id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDeleteShader" ) ) ;
            shaders[ oid ].gs_id = GLuint( -1 ) ;
        }

        // pixel shader
        if( ss.has_pixel_shader() )
        {
            GLuint id = shaders[ oid ].ps_id ;
            if( id == GLuint(-1) )
            {
                id = natus::ogl::gl::glCreateShader( GL_FRAGMENT_SHADER ) ;
                natus::ogl::error::check_and_log(
                    natus_log_fn( "Fragment Shader creation" ) ) ;
            }

            natus::ogl::gl::glAttachShader( shaders[ oid ].pg_id, id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "Attaching pixel shader" ) ) ;

            shaders[ oid ].ps_id = id ;
        }
        else if( shaders[ oid ].ps_id != GLuint( -1 ) )
        {
            natus::ogl::gl::glDeleteShader( shaders[ oid ].ps_id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDeleteShader" ) ) ;
            shaders[ oid ].ps_id = GLuint( -1 ) ;
        }

        return oid ;
    }

    //***********************
    void_t detach_shaders( GLuint const program_id )
    {
        GLsizei count = 0 ;
        GLuint shaders_[ 10 ] ;

        natus::ogl::gl::glGetAttachedShaders( program_id, 10, &count, shaders_ ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glGetAttachedShaders" ) ) ;

        for( GLsizei i = 0; i < count; ++i )
        {
            natus::ogl::gl::glDetachShader( program_id, shaders_[ i ] ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDetachShader" ) ) ;
        }
    }

    //***********************
    void_t delete_all_variables( this_file::shader_config & config )
    {
        config.vertex_inputs.clear() ;

        for( auto & v : config.attributes )
        {
            // delete memory
        }
        config.attributes.clear() ;

        for( auto & v : config.uniforms )
        {
            // delete memory. No dealloc. The memory is 
            // a pointer to a memory block.
            v.mem = nullptr ;
        }
        config.uniforms.clear() ;
        natus::memory::global_t::dealloc( config.uniform_mem ) ;
        config.uniform_mem = nullptr ;
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
    void_t post_link_attributes( this_file::shader_config & config )
    {
        GLuint const program_id = config.pg_id ;

        GLint num_active_attributes = 0 ;
        GLint name_length = 0 ;

        natus::ogl::gl::glGetProgramiv( program_id, GL_ACTIVE_ATTRIBUTES, &num_active_attributes ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glGetProgramiv(GL_ACTIVE_ATTRIBUTES)" ) ) ;

        if( num_active_attributes == 0 ) return ;

        config.attributes.resize( num_active_attributes ) ;

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

            GLuint const location_id = natus::ogl::gl::glGetAttribLocation( program_id, buffer ) ;
            if( natus::ogl::error::check_and_log( "glGetAttribLocation failed. continue loop." ) ) continue ;

            natus::std::string_t const variable_name = natus::std::string_t( ( const char* ) buffer ) ;

            this_file::shader_config::attribute_variable_t vd ;
            vd.name = ::std::move( variable_name ) ;
            vd.loc = location_id ;
            vd.type = gl_attrib_type ;
            
            {
                natus::gpu::vertex_attribute va = natus::gpu::vertex_attribute::undefined ;
                auto const res = config.find_vertex_input_binding_by_name( vd.name, va ) ;
                natus::log::global_t::error( natus::core::is_not( res ), 
                    natus_log_fn("can not find vertex attribute - " + vd.name ) ) ;
                vd.va = va ;
            }
            config.attributes[i] = vd ;
        }
    }

    //***********************
    bool_t bind_attributes( this_file::shader_config & sconfig, this_file::geo_config & gconfig )
    {
        // bind vertex array object
        {
            natus::ogl::gl::glBindVertexArray( gconfig.va_id ) ;
            if( natus::ogl::error::check_and_log(
                natus_log_fn( "glBindVertexArray" ) ) )
                return false ;
        }

        // bind vertex buffer
        {
            natus::ogl::gl::glBindBuffer( GL_ARRAY_BUFFER, gconfig.vb_id ) ;
            if( natus::ogl::error::check_and_log( 
                natus_log_fn("glBindBuffer(GL_ARRAY_BUFFER)") ) ) 
                return false ;
        }

        // bind index buffer
        {
            natus::ogl::gl::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gconfig.ib_id ) ;
            if( natus::ogl::error::check_and_log( 
                natus_log_fn( "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)" ) ) )
                return false ;
        }

        // disable old locations
        for( size_t i = 0; i < sconfig.attributes.size(); ++i )
        {
            natus::ogl::gl::glDisableVertexAttribArray( sconfig.attributes[ i ].loc ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDisableVertexAttribArray" ) ) ;
        }

        GLuint uiStride = gconfig.stride ;
        GLuint uiBegin = 0 ;
        GLuint uiOffset = 0 ;

        for( auto & e : gconfig.elements )
        {
            uiBegin += uiOffset ;
            uiOffset = e.sib() ;

            auto iter = ::std::find_if( sconfig.attributes.begin(), sconfig.attributes.end(), 
                [&]( this_file::shader_config::attribute_variable_cref_t av )
            {
                return av.va == e.va ;
            } ) ;

            if( iter == sconfig.attributes.end() ) 
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
    void_t post_link_uniforms( this_file::shader_config & config )
    {
        GLuint const program_id = config.pg_id ;

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

            this_file::shader_config::uniform_variable_t vd ;
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
            natus_assert( config.uniform_mem == nullptr ) ;
            config.uniform_mem = natus::memory::global_t::alloc( sib, 
                natus_log_fn("uniform memory block") ) ;
        }

        // assign ptr
        {
            size_t sib = 0 ;
            for( auto& v : config.uniforms )
            {
                v.mem = void_ptr_t( byte_ptr_t( config.uniform_mem ) + sib ) ;
                natus::ogl::uniform_default_value( v.type )( v.mem ) ;
                sib += natus::ogl::uniform_size_of( v.type ) ;
            }
        }
    }

    //***********************
    void_t update_all_uniforms( this_file::shader_config & config )
    {
        GLuint const program_id = config.pg_id ;

        {
            natus::ogl::gl::glUseProgram( config.pg_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glUseProgram" ) ) )
                return ;
        }

        for( auto & v : config.uniforms )
        {
            v.do_uniform_funk() ;
        }
    }

    //***********************
    size_t construct_image_config( size_t oid, natus::std::string_cref_t name, 
        natus::gpu::image_configuration_ref_t config )
    {
        // the name is unique
        {
            auto iter = ::std::find_if( img_configs.begin(), img_configs.end(),
                [&] ( this_file::image_config const& config )
            {
                return config.name == name ;
            } ) ;

            if( iter != img_configs.end() )
                return iter - img_configs.begin() ;
        }

        size_t i = 0 ;
        for( i; i < img_configs.size(); ++i )
        {
            if( natus::core::is_not( img_configs[ i ].tex_id != GLuint( -1 ) ) )
            {
                break ;
            }
        }

        if( i == img_configs.size() ) {
            img_configs.resize( i + 1 ) ;
        }

        // sampler
        if( img_configs[ i ].tex_id == GLuint( -1 ) )
        {
            GLuint id = GLuint( -1 ) ;
            natus::ogl::gl::glGenTextures( 1, &id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenSamplers" ) ) ;

            img_configs[ i ].tex_id = id ;
        }

        {
            img_configs[ i ].name = name ;
        }

        {
            for( size_t j=0; j<(size_t)natus::gpu::texture_wrap_mode::size; ++j )
            {
                img_configs[ i ].wrap_types[ j ] = natus::gpu::gl3::convert(
                    config.get_wrap( ( natus::gpu::texture_wrap_mode )j ) );
            }

            for( size_t j = 0; j < ( size_t ) natus::gpu::texture_filter_mode::size; ++j )
            {
                img_configs[ i ].filter_types[ j ] = natus::gpu::gl3::convert(
                    config.get_filter( ( natus::gpu::texture_filter_mode )j ) );
            }
        }
        return i ;
    }

    //***********************
    size_t construct_render_config( size_t oid, natus::std::string_cref_t name,
        natus::gpu::render_configuration_ref_t config )
    {
        // the name must be unique
        {
            auto iter = ::std::find_if( rconfigs.begin(), rconfigs.end(),
                [&] ( this_file::render_config const& c )
            {
                return c.name == name ;
            } ) ;

            if( iter != rconfigs.end() )
            {
                size_t const i = ( iter - rconfigs.begin() ) ;
                if( natus::log::global_t::error( i != oid && oid != size_t( -1 ),
                    natus_log_fn( "name and id do not fit " ) ) )
                {
                    return oid ;
                }
            }
        }

        if( oid == size_t( -1 ) )
        {
            size_t i = 0 ;
            for( ; i < rconfigs.size(); ++i )
            {
                if( natus::core::is_not( rconfigs[ i ].name.empty() ) )
                {
                    break ;
                }
            }
            oid = i ;
        }

        if( oid >= rconfigs.size() ) {
            rconfigs.resize( oid + 1 ) ;
        }

        {
            rconfigs[ oid ].name = name ;
        }

        {
            rconfigs[ oid ].var_sets_data.clear() ;
            rconfigs[ oid ].var_sets_texture.clear() ;
        }

        return oid ;
    }

    bool_t update( size_t const id, natus::gpu::shader_configuration_cref_t sc )
    {
        auto& sconfig = shaders[ id ] ;

        {
            sc.for_each_vertex_input_binding( [&]( 
                natus::gpu::vertex_attribute const va, natus::std::string_cref_t name )
            {
                sconfig.vertex_inputs.emplace_back( this_file::shader_config::vertex_input_binding 
                    { va, name } ) ;
            } ) ;
        }

        // compile
        {
            natus::gpu::shader_set_t ss ;
            {
                auto const res = sc.shader_set( this_t::bt, ss ) ;
                if( res )
                {
                    this_t::compile_shader( sconfig.vs_id, ss.vertex_shader().code() ) ;
                    this_t::compile_shader( sconfig.gs_id, ss.geometry_shader().code() ) ;
                    this_t::compile_shader( sconfig.ps_id, ss.pixel_shader().code() ) ;
                    this_t::link( sconfig.pg_id ) ;
                }
            }
        }

        {
            this_t::post_link_attributes( sconfig ) ;
            this_t::post_link_uniforms( sconfig ) ;
        }

        return true ;
    }

    bool_t update( size_t const id, natus::gpu::render_configuration_ref_t rc )
    {
        auto& config = rconfigs[ id ] ;
        config.geo = nullptr ;

        // find geometry
        {
            auto const iter = ::std::find_if( geo_configs.begin(), geo_configs.end(),
                [&] ( this_file::geo_config const& d )
            {
                return d.name == rc.get_geometry() ;
            } ) ;
            if( iter == geo_configs.end() )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "no geometry with name [" + rc.get_geometry() + "] for render_config [" + rc.name() + "]" ) ) ;
                return false ;
            }

            config.geo = &geo_configs[ ::std::distance( geo_configs.begin(), iter ) ] ;
        }

        // find shader
        {
            auto const iter = ::std::find_if( shaders.begin(), shaders.end(),
                [&] ( this_file::shader_config const& d )
            {
                return d.name == rc.get_shader() ;
            } ) ;
            if( iter == shaders.end() )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "no shader with name [" + rc.get_shader() + "] for render_config [" + rc.name() + "]" ) ) ;
                return false ;
            }

            config.shaders_ptr = &shaders[ ::std::distance( shaders.begin(), iter ) ] ;
        }
        
        // for binding attributes, the shader and the geometry is required.
        {
            this_t::bind_attributes( *config.shaders_ptr, *config.geo ) ;
        }

        {
            rc.for_each( [&] ( size_t const i, natus::gpu::variable_set_res_t vs )
            {
                auto const res = this_t::connect( id, vs ) ;
                natus::log::global_t::warning( natus::core::is_not( res ),
                    natus_log_fn( "connect" ) ) ;
            } ) ;
        }

        /*{
            auto vss = ::std::move( config.var_sets ) ;
            for( auto & vs : vss )
            {
                auto const res = this_t::connect( id, vs.first ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), 
                    natus_log_fn( "connect" ) ) ;
            }
        }*/
        
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

        // vertex array object
        if( geo_configs[ i ].va_id == GLuint( -1 ) )
        {
            GLuint id = GLuint( -1 ) ;
            natus::ogl::gl::glGenVertexArrays( 1, &id ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "Vertex Array creation" ) ) ;

            geo_configs[ i ].va_id = id ;
        }

        // vertex buffer
        if( geo_configs[i].vb_id == GLuint(-1) )
        {
            GLuint id = GLuint( -1 ) ;
            natus::ogl::gl::glGenBuffers( 1, &id ) ;
            error = natus::ogl::error::check_and_log( 
                natus_log_fn( "Vertex Buffer creation" ) ) ;
            
            geo_configs[ i ].vb_id = id ;
        }

        // index buffer
        if( geo_configs[i].ib_id == GLuint(-1) )
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

        return i ;
    }

    bool_t update( size_t const id, natus::gpu::geometry_configuration_res_t geo )
    {
        auto& config = geo_configs[ id ] ;

        {
            //#error "set all these new values"
            config.num_elements_ib = geo->index_buffer().get_num_elements() ;
            config.num_elements_vb = geo->vertex_buffer().get_num_elements() ;
            config.ib_elem_sib = geo->index_buffer().get_element_sib() ;
            config.ib_type = GL_UNSIGNED_INT ;
            config.pt = natus::gpu::gl3::convert( geo->primitive_type() ) ;
        }

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
            if( sib > config.sib_vb )
            {
                natus::ogl::gl::glBufferData( GL_ARRAY_BUFFER, sib,
                    geo->vertex_buffer().data(), GL_STATIC_DRAW ) ;
                if( natus::ogl::error::check_and_log( natus_log_fn( "glBufferData - vertex buffer" ) ) )
                    return false ;
                config.sib_vb = sib ;
            }
            else
            {
                natus::ogl::gl::glBufferSubData( GL_ARRAY_BUFFER, 0, sib, geo->vertex_buffer().data() ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glBufferSubData - vertex buffer" ) ) ;
            }
        }

        // there may be no index buffer
        if( config.ib_id == GLuint(-1) )
        {
            return true ;
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
            if( sib > config.sib_ib )
            {
                natus::ogl::gl::glBufferData( GL_ELEMENT_ARRAY_BUFFER, sib,
                    geo->index_buffer().data(), GL_STATIC_DRAW ) ;
                if( natus::ogl::error::check_and_log( natus_log_fn( "glBufferData - index buffer" ) ) )
                    return false ;
                config.sib_ib = sib ;
            }
            else
            {
                natus::ogl::gl::glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, sib, geo->index_buffer().data() ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glBufferSubData - index buffer" ) ) ;
            }
        }

        return true ;
    }

    bool_t update( size_t const id, natus::gpu::image_configuration_ref_t confin )
    {
        this_file::image_config& config = img_configs[ id ] ;

        natus::ogl::gl::glBindTexture( GL_TEXTURE_2D, config.tex_id ) ;
        if( natus::ogl::error::check_and_log( natus_log_fn( "glBindTexture" ) ) )
            return false ;

        size_t const sib = confin.image().sib() ;
        GLenum const target = GL_TEXTURE_2D ;
        GLint const level = 0 ;
        GLsizei const width = GLsizei( confin.image().get_dims().x() ) ;
        GLsizei const height = GLsizei( confin.image().get_dims().y() ) ;
        GLenum const format = natus::gpu::gl3::convert_to_gl_pixel_format( confin.image().get_image_format() ) ;
        GLenum const type = natus::gpu::gl3::convert_to_gl_pixel_type( confin.image().get_image_element_type() ) ;
        void_cptr_t data = confin.image().get_image_ptr() ;

        if( sib == 0 || config.sib < sib )
        {
            GLint const border = 0 ;
            GLint const internal_format = natus::gpu::gl3::convert_to_gl_format( confin.image().get_image_format(), confin.image().get_image_element_type() ) ;

            natus::ogl::gl::glTexImage2D( target, level, internal_format, width, height,
                border, format, type, data ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glTexImage2D" ) ) ;
        }
        else
        {
            GLint const xoffset = 0 ;
            GLint const yoffset = 0 ;

            natus::ogl::gl::glTexSubImage2D( target, level, xoffset, yoffset, width, height,
                format, type, data ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glTexSubImage2D" ) ) ;
        }

        config.sib = confin.image().sib() ;

        return false ;
    }

    bool_t connect( size_t const id, natus::gpu::variable_set_res_t vs )
    {
        auto& config = rconfigs[ id ] ;

        this_file::shader_config & sconfig = *config.shaders_ptr ;

        if( this_t::connect( config, vs ) )
            this_t::update_all_uniforms( sconfig ) ;

        return true ;
    }

    bool_t connect( this_file::render_config & config, natus::gpu::variable_set_res_t vs )
    {
        auto item_data = ::std::make_pair( vs,
            natus::std::vector< this_file::render_config::uniform_variable_link >() ) ;

        auto item_tex = ::std::make_pair( vs,
            natus::std::vector< this_file::render_config::uniform_texture_link >() ) ;

        this_file::shader_config & sconfig = *config.shaders_ptr ;

        size_t id = 0 ;
        for( auto& uv : sconfig.uniforms )
        {
            // is it a data uniform variable?
            if( natus::ogl::uniform_is_data( uv.type ) )
            {
                

                auto const types = natus::gpu::gl3::to_type_type_struct( uv.type ) ;
                auto* var = vs->data_variable( uv.name, types.first, types.second ) ;
                if( natus::core::is_nullptr( var ) )
                {
                    natus::log::global_t::error( natus_log_fn( "can not claim variable " + uv.name ) ) ;
                    continue ;
                }
                
                this_file::render_config::uniform_variable_link link ;
                link.uniform_id = id++ ;
                link.var = var ;

                item_data.second.emplace_back( link ) ;
            }
            else if( natus::ogl::uniform_is_texture( uv.type ) )
            {
                auto const types = natus::gpu::gl3::to_type_type_struct( uv.type ) ;
                auto* var = vs->texture_variable( uv.name ) ;

                if( natus::core::is_nullptr( var ) )
                {
                    natus::log::global_t::error( natus_log_fn( "can not claim variable " + uv.name ) ) ;
                    continue ;
                }

                size_t i = 0 ;
                auto const & tx_name = var->get() ;
                for( auto & cfg : img_configs )
                {
                    if( cfg.name == tx_name ) break ;
                    ++i ;
                }

                if( i >= img_configs.size() ) 
                {
                    natus::log::global_t::warning( natus_log_fn("image not found : " + tx_name ) ) ;
                    continue ;
                }

                this_file::render_config::uniform_texture_link link ;
                link.uniform_id = id++ ;
                link.tex_id = img_configs[ i ].tex_id ;
                link.img_id = i ;
                item_tex.second.emplace_back( link ) ;
            }
        }

        config.var_sets_data.emplace_back( item_data ) ;
        config.var_sets_texture.emplace_back( item_tex ) ;

        return true ;
    }

    bool_t render( size_t const id, size_t const varset_id = size_t(0), GLsizei const start_element = GLsizei(0), 
        GLsizei const num_elements = GLsizei(-1) )
    {
        this_file::render_config & config = rconfigs[ id ] ;
        this_file::shader_config & sconfig = *config.shaders_ptr ;
        this_file::geo_config & gconfig = *config.geo ;

        {
            natus::ogl::gl::glBindVertexArray( gconfig.va_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBindVertexArray" ) ) )
                return false ;
        }

        {
            natus::ogl::gl::glUseProgram( sconfig.pg_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glUseProgram" ) ) )
                return false ;
        }

        if( config.var_sets_data.size() > varset_id )
        {
            // data vars
            {
                auto& varset = config.var_sets_data[ varset_id ] ;
                for( auto& item : varset.second )
                {
                    auto& uv = sconfig.uniforms[ item.uniform_id ] ;
                    uv.do_copy_funk( item.var ) ;
                    uv.do_uniform_funk() ;
                }
            }

            // tex vars
            {
                int_t tex_unit = 0 ;
                auto& varset = config.var_sets_texture[ varset_id ] ;
                for( auto& item : varset.second )
                {
                    natus::ogl::gl::glActiveTexture( GLenum(GL_TEXTURE0 + tex_unit) ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glActiveTexture" ) ) ;
                    natus::ogl::gl::glBindTexture( GL_TEXTURE_2D, item.tex_id ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glBindTexture" ) ) ;

                    {
                        auto const& ic = img_configs[ item.img_id ] ;
                        natus::ogl::gl::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ic.wrap_types[0] ) ;
                        natus::ogl::gl::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ic.wrap_types[1] ) ;
                        natus::ogl::gl::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, ic.wrap_types[2] ) ;
                        natus::ogl::gl::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ic.filter_types[0] ) ;
                        natus::ogl::gl::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ic.filter_types[1] ) ;
                        natus::ogl::error::check_and_log( natus_log_fn( "glTexParameteri" ) ) ;
                    }

                    auto var = natus::gpu::data_variable< int_t >( tex_unit ) ;
                    auto & uv = sconfig.uniforms[ item.uniform_id ] ;
                    
                    uv.do_copy_funk( &var ) ;
                    uv.do_uniform_funk() ;

                    ++tex_unit ;
                }
            }
        }

        // render section
        {
            GLenum const pt = config.geo->pt ;
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

    void_t set_render_states( natus::gpu::render_state_sets_in_t rs ) 
    {
        if( render_states.blend_s.do_blend != rs.blend_s.do_blend )
        {
            if( rs.blend_s.do_blend )
            {
                natus::ogl::gl::glEnable( GL_BLEND ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;

                GLenum const glsrc = natus::gpu::gl3::convert( rs.blend_s.src_blend_factor ) ;
                GLenum const gldst = natus::gpu::gl3::convert( rs.blend_s.dst_blend_factor ) ;
                natus::ogl::gl::glBlendFunc( glsrc, gldst ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glBlendFunc" ) ) ;
            }
            else
            {
                natus::ogl::gl::glDisable( GL_BLEND ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
            }
            
        }

        if( render_states.depth_s.do_depth_test != rs.depth_s.do_depth_test )
        {
            if( rs.depth_s.do_depth_test )
            {
                natus::ogl::gl::glEnable( GL_DEPTH_TEST ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;
            }
            else
            {
                natus::ogl::gl::glDisable( GL_DEPTH_TEST ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
            }
        }
        
        if( render_states.polygon_s.do_culling != rs.polygon_s.do_culling )
        {
            if( rs.polygon_s.do_culling )
            {
                natus::ogl::gl::glEnable( GL_CULL_FACE ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;
            }
            else
            {
                natus::ogl::gl::glDisable( GL_CULL_FACE ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
            }
        }

        if( render_states.scissor_s.do_scissor_test != rs.scissor_s.do_scissor_test )
        {
            if( rs.scissor_s.do_scissor_test )
            {
                natus::ogl::gl::glEnable( GL_SCISSOR_TEST ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;

                natus::ogl::gl::glScissor(
                    GLint( rs.scissor_s.rect.x() ), GLint( rs.scissor_s.rect.y() ),
                    GLsizei( rs.scissor_s.rect.z() ), GLsizei( rs.scissor_s.rect.w() ) ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glScissor" ) ) ;
            }
            else
            {
                natus::ogl::gl::glDisable( GL_SCISSOR_TEST ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
            }
        }


        render_states = rs ;
    }

    void_t begin_frame( void_t ) 
    {
        //natus::ogl::gl::glClearColor( 0.1f, 0.1f, 0.1f, 1.0f ) ;
        //natus::ogl::error::check_and_log( natus_log_fn( "glClearColor" ) ) ;

        //natus::ogl::gl::glClear( GL_COLOR_BUFFER_BIT ) ;
        //natus::ogl::error::check_and_log( natus_log_fn( "glClear" ) ) ;
        
        // reset render states
        this_t::set_render_states( *natus::gpu::backend_t::default_render_states() ) ;

        natus::ogl::gl::glViewport( 0, 0, vp_width, vp_height ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glViewport" ) ) ;
    }

    void_t end_frame( void_t ) 
    {
        natus::ogl::gl::glFlush() ;
        natus::ogl::gl::glFinish() ;
    }
};

//************************************************************************************************
//
//
//************************************************************************************************

//****
gl3_backend::gl3_backend( void_t ) noexcept : backend( natus::gpu::backend_type::gl3 )
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
void_t gl3_backend::set_window_info( window_info_cref_t wi ) noexcept 
{
    {
        bool_t change = false ;
        if( wi.width != 0 )
        {
            _pimpl->vp_width = GLsizei( wi.width ) ;
            change = true ;
        }
        if( wi.height != 0 )
        {
            _pimpl->vp_height = GLsizei( wi.height ) ;
            change = true ;
        }
    }
}

//****
natus::gpu::result gl3_backend::configure( natus::gpu::geometry_configuration_res_t gconf ) noexcept 
{
    natus::gpu::id_res_t id = gconf->get_id() ;

    if( id->is_not_valid( this_t::get_bid() ) )
    {
        id = natus::gpu::id_t( this_t::get_bid(),
            _pimpl->construct_geo( gconf->name(), *(gconf.get_sptr()) ) ) ;
    }

    {
        auto const res = _pimpl->update( id->get_oid( this_t::get_bid() ), gconf ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::gpu::result::failed ;
        }
    }

    return natus::gpu::result::ok ;
}

//****
natus::gpu::result gl3_backend::configure( natus::gpu::render_configuration_res_t config ) noexcept 
{
    natus::gpu::id_res_t id = config->get_id() ;

    {
        id = natus::gpu::id_t( this_t::get_bid(),
            _pimpl->construct_render_config( id->get_oid( this_t::get_bid() ), config->name(), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::gpu::result::failed ;
        }
    }

    return natus::gpu::result::ok ;
}

//***
natus::gpu::result gl3_backend::configure( natus::gpu::shader_configuration_res_t config ) noexcept
{
    natus::gpu::id_res_t id = config->get_id() ;

    {
        id = natus::gpu::id_t( this_t::get_bid(),
            _pimpl->construct_shader_config( id->get_oid( this_t::get_bid() ), config->name(), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::gpu::result::failed ;
        }
    }

    return natus::gpu::result::ok ;
}

//***
natus::gpu::result gl3_backend::configure( natus::gpu::image_configuration_res_t config ) noexcept 
{
    natus::gpu::id_res_t id = config->get_id() ;

    {
        id = natus::gpu::id_t( this_t::get_bid(), _pimpl->construct_image_config( 
            id->get_oid( this_t::get_bid() ), config->name(), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::gpu::result::failed ;
        }
    }

    return natus::gpu::result::ok ;
}

//***
natus::gpu::result gl3_backend::connect( natus::gpu::render_configuration_res_t config, natus::gpu::variable_set_res_t vs ) noexcept
{
    natus::gpu::id_res_t id = config->get_id() ;

    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid render configuration id" ) ) ;
        return natus::gpu::result::failed ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    auto const res = _pimpl->connect( oid, vs ) ;
    natus::log::global_t::error( natus::gpu::is_not( res ), 
        natus_log_fn( "connect variable set" ) ) ;
   
    return natus::gpu::result::ok ;
}

//****
natus::gpu::result gl3_backend::update( natus::gpu::geometry_configuration_res_t config ) noexcept 
{
    natus::gpu::id_res_t id = config->get_id() ;

    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid geometry configuration id" ) ) ;
        return natus::gpu::result::failed ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    auto const res = _pimpl->update( oid, config ) ;
    natus::log::global_t::error( natus::gpu::is_not( res ),
        natus_log_fn( "update geometry" ) ) ;

    return natus::gpu::result::ok ;
}

//****
natus::gpu::result gl3_backend::render( natus::gpu::render_configuration_res_t config, natus::gpu::backend::render_detail_cref_t detail ) noexcept 
{ 
    natus::gpu::id_res_t id = config->get_id() ;

    //natus::log::global_t::status( natus_log_fn("render") ) ;

    if( id->is_not_bid( this_t::get_bid() ) || id->is_not_valid() )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return natus::gpu::result::failed ;
    }

    if( detail.render_states.is_valid() )
    {
        _pimpl->set_render_states( *( detail.render_states ) ) ;
    }

    _pimpl->render( id->get_oid(), detail.varset, (GLsizei)detail.start, (GLsizei)detail.num_elems ) ;

    return natus::gpu::result::ok ;
}

//****
void_t gl3_backend::render_begin( void_t ) noexcept 
{
    _pimpl->begin_frame() ;
}

void_t gl3_backend::render_end( void_t ) noexcept 
{
    _pimpl->end_frame() ;
}
