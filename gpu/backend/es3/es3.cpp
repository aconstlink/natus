#include "es3.h"
#include "es3_convert.h"

#include "../../buffer/vertex_buffer.hpp"
#include "../../buffer/index_buffer.hpp"

#include <natus/ogl/es/es.h>
#include <natus/ogl/es/convert.hpp>
//#include <natus/ogl/es/error.hpp>
#include <natus/memory/global.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/std/vector.hpp>
#include <natus/std/string/split.hpp>

using namespace natus::gpu ;

namespace this_file_es
{
    using namespace natus::core::types ;
    struct camera_config
    {
        // proj
        // view
        // cam rot
        // cam pos
        // viewport
    };

    struct geo_config
    {
        natus::std::string_t name ;

        GLuint vb_id = GLuint( -1 ) ;
        GLuint ib_id = GLuint( -1 ) ;
        
        void_ptr_t vb_mem = nullptr ;
        void_ptr_t ib_mem = nullptr ;

        size_t num_elements_vb = 0 ;
        size_t num_elements_ib = 0 ;
        
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

        // per vertex sib
        GLuint stride = 0 ;

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
            natus::std::string_t name ;
            GLuint loc ;
            GLenum type ;

            // this variable's memory location
            void_ptr_t mem = nullptr ;

            // the GL uniform function
            natus::ogl::uniform_funk_t uniform_funk ;

            // set by the user in user-space
            natus::gpu::ivariable_ptr_t var ;

            void_t do_uniform_funk( void_t )
            {
                uniform_funk( loc, 1, mem ) ;
                natus::ogl::es::check_and_log( natus_log_fn( "glUniform" ) ) ;
            }

            void_t do_copy_funk( void_t ) 
            {
                ::std::memcpy( mem, var->data_ptr(), natus::ogl::uniform_size_of( type ) ) ;
            }
        };
        natus_typedef( uniform_variable ) ;

        natus::std::vector< uniform_variable > uniforms ;

        void_ptr_t uniform_mem = nullptr ;

        geo_config* geo = nullptr ;

        // user provided variable set
        natus::std::vector< natus::gpu::variable_set_res_t > var_sets ;
    };
}

struct es3_backend::pimpl
{
    natus_this_typedefs( pimpl ) ;

    typedef natus::std::vector< this_file_es::render_config > rconfigs_t ;
    rconfigs_t rconfigs ;

    typedef natus::std::vector< this_file_es::geo_config > geo_configs_t ;
    geo_configs_t geo_configs ;

    GLsizei vp_width = 0 ;
    GLsizei vp_height = 0 ;

    pimpl( void_t ) 
    {}

    size_t construct_rconfig( size_t oid, natus::std::string_cref_t name, 
        natus::gpu::render_configuration_ref_t config )  
    {
        // the name is unique
        {
            auto iter = ::std::find_if( rconfigs.begin(), rconfigs.end(),
                [&] ( this_file_es::render_config const& c )
            {
                return c.name == name ;
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
            for( ; i < rconfigs.size(); ++i )
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
            glGenVertexArrays( 1, &id ) ;
            error = natus::ogl::es::check_and_log(
                natus_log_fn( "Vertex Array creation" ) ) ;

            rconfigs[ oid ].va_id = id ;
        }

        // program
        if( rconfigs[oid].pg_id == GLuint(-1) )
        {
            GLuint const id = glCreateProgram() ;
            error = natus::ogl::es::check_and_log(
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
            GLuint const id = glCreateShader( GL_VERTEX_SHADER ) ;
            error = natus::ogl::es::check_and_log(
                natus_log_fn( "Vertex Shader creation" ) ) ;

            rconfigs[ oid ].vs_id = id ;
        }
        {
            glAttachShader( rconfigs[ oid ].pg_id, rconfigs[oid].vs_id ) ;
            error = natus::ogl::es::check_and_log(
                natus_log_fn( "Attaching vertex shader" ) ) ;
        }

        // pixel shader
        if( config.has_pixel_shader() )
        {
            GLuint id = rconfigs[ oid ].ps_id ;
            if( id == GLuint(-1) )
            {
                id = glCreateShader( GL_FRAGMENT_SHADER ) ;
                error = natus::ogl::es::check_and_log(
                    natus_log_fn( "Fragment Shader creation" ) ) ;
            }
            
            glAttachShader( rconfigs[ oid ].pg_id, id ) ;
            error = natus::ogl::es::check_and_log( natus_log_fn( "Attaching pixel shader" ) ) ;

            rconfigs[ oid ].ps_id = id ;
        }
        else if( rconfigs[ oid ].ps_id != GLuint( -1 ) )
        {
            glDeleteShader( rconfigs[ oid ].ps_id ) ;
            natus::ogl::es::check_and_log( natus_log_fn( "glDeleteShader" ) ) ;
            rconfigs[ oid ].ps_id = GLuint( -1 ) ;
        }

        return oid ;
    }

    //***********************
    void_t detach_shaders( GLuint const program_id )
    {
        GLsizei count = 0 ;
        GLuint shaders[ 10 ] ;

        glGetAttachedShaders( program_id, 10, &count, shaders ) ;
        natus::ogl::es::check_and_log( natus_log_fn( "glGetAttachedShaders" ) ) ;

        for( GLsizei i = 0; i < count; ++i )
        {
            glDetachShader( program_id, shaders[ i ] ) ;
            natus::ogl::es::check_and_log( natus_log_fn( "glDetachShader" ) ) ;
        }
    }

    //***********************
    void_t delete_all_variables( this_file_es::render_config & config )
    {
        /*for( auto & v : config.attributes )
        {
            // delete memory
        }*/
        config.attributes.clear() ;

        for( auto & v : config.uniforms )
        {
            // delete memory
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

        glShaderSource( id, 1, &source_string, 0 ) ;
        if( natus::ogl::es::check_and_log( natus_log_fn( "glShaderSource" ) ) )
            return false ;

        glCompileShader( id ) ;
        if( natus::ogl::es::check_and_log( natus_log_fn( "glCompileShader" ) ) )
            return false ;

        GLint ret ;
        glGetShaderiv( id, GL_COMPILE_STATUS, &ret ) ;

        GLint length ;
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &length ) ;

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

            glGetShaderInfoLog( id, length, 0, info_log ) ;

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
        glLinkProgram( program_id ) ;
        if( natus::ogl::es::check_and_log( natus_log_fn( "glLinkProgram" ) ) )
            return false ;

        {
            GLint ret ;
            glGetProgramiv( program_id, GL_LINK_STATUS, &ret ) ;

            GLint length ;
            glGetProgramiv( program_id, GL_INFO_LOG_LENGTH, &length ) ;

            if( ret == GL_TRUE && length <= 1 ) 
                return true ;

            if( natus::log::global_t::error( length == 0, natus_log_fn("unknown") ) )
                return false ;

            natus::memory::malloc_guard<char> info_log( length ) ;

            glGetProgramInfoLog( program_id, length, 0, info_log ) ;
            if( natus::ogl::es::check_and_log( natus_log_fn( "glGetProgramInfoLog" ) ) )
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
    void_t post_link_attributes( GLuint const program_id, this_file_es::render_config * config, 
        natus::gpu::render_configuration_cref_t rc )
    {
        GLint num_active_attributes = 0 ;
        GLint name_length = 0 ;

        glGetProgramiv( program_id, GL_ACTIVE_ATTRIBUTES, &num_active_attributes ) ;
        natus::ogl::es::check_and_log( natus_log_fn( "glGetProgramiv(GL_ACTIVE_ATTRIBUTES)" ) ) ;

        if( num_active_attributes == 0 ) return ;

        config->attributes.resize( num_active_attributes ) ;

        glGetProgramiv( program_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &name_length ) ;
        natus::ogl::es::check_and_log( natus_log_fn( 
            "glGetProgramiv(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH)" ) ) ;

        GLint size ;
        GLenum gl_attrib_type ;

        natus::memory::malloc_guard<char> buffer( name_length ) ;
        
        for( GLint i = 0; i < num_active_attributes; ++i )
        {
            glGetActiveAttrib( program_id, i, name_length, 0, 
                &size, &gl_attrib_type, buffer ) ;

            if( natus::ogl::es::check_and_log( "glGetActiveAttrib failed. continue loop." ) ) continue ;

            // build-ins are not needed in the shader variables container.
            if( name_length >= 3 && buffer.equals( 0, 'g' ) && buffer.equals( 1, 'l' ) && buffer.equals( 2, '_' ) ) continue ;

            GLuint const location_id = glGetAttribLocation( program_id, buffer ) ;
            if( natus::ogl::es::check_and_log( "glGetAttribLocation failed. continue loop." ) ) continue ;

            natus::std::string_t const variable_name = natus::std::string_t( ( const char* ) buffer ) ;

            this_file_es::render_config::attribute_variable_t vd ;
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
    bool_t bind_attributes( natus::gpu::render_configuration_cref_t /*rc*/, this_file_es::render_config * config, this_file_es::geo_config * gconfig )
    {
        // bind vertex array object
        {
            glBindVertexArray( config->va_id ) ;
            if( natus::ogl::es::check_and_log(
                natus_log_fn( "glBindVertexArray" ) ) )
                return false ;
        }

        // bind vertex buffer
        {
            glBindBuffer( GL_ARRAY_BUFFER, gconfig->vb_id ) ;
            if( natus::ogl::es::check_and_log( 
                natus_log_fn("glBindBuffer(GL_ARRAY_BUFFER)") ) ) 
                return false ;
        }

        // bind index buffer
        {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gconfig->ib_id ) ;
            if( natus::ogl::es::check_and_log( 
                natus_log_fn( "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)" ) ) )
                return false ;
        }

        // disable old locations
        for( size_t i = 0; i < config->attributes.size(); ++i )
        {
            glDisableVertexAttribArray( config->attributes[ i ].loc ) ;
            natus::ogl::es::check_and_log( natus_log_fn( "glDisableVertexAttribArray" ) ) ;
        }

        GLuint uiStride = gconfig->stride ;
        GLuint uiBegin = 0 ;
        GLuint uiOffset = 0 ;

        for( auto & e : gconfig->elements )
        {
            uiBegin += uiOffset ;
            uiOffset = e.sib() ;

            auto iter = ::std::find_if( config->attributes.begin(), config->attributes.end(), 
                [&]( this_file_es::render_config::attribute_variable_cref_t av )
            {
                return av.va == e.va ;
            } ) ;

            if( iter == config->attributes.end() ) 
            {
                natus::log::global_t::warning( natus_log_fn("invalid vertex attribute") ) ;
                continue ;
            }
            glEnableVertexAttribArray( iter->loc ) ;
            natus::ogl::es::check_and_log(
                natus_log_fn( "glEnableVertexAttribArray" ) ) ;

            glVertexAttribPointer(
                iter->loc,
                GLint( natus::gpu::size_of(e.type_struct) ),
                natus::ogl::complex_to_simple_type( iter->type ),
                GL_FALSE,
                ( GLsizei ) uiStride,
                (const GLvoid*)(size_t)uiBegin 
                ) ;

            natus::ogl::es::check_and_log( natus_log_fn( "glVertexAttribPointer" ) ) ;
        }

        // unbind everything
        {
            glBindVertexArray( 0 ) ;
            glBindBuffer( GL_ARRAY_BUFFER, 0 ) ;
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) ;
            natus::ogl::es::check_and_log( natus_log_fn( "unbind" ) ) ;
        }
        return true ;
    }

    //***********************
    void_t post_link_uniforms( GLuint const program_id, this_file_es::render_config & config )
    {
        GLint num_active_uniforms = 0 ;
        GLint name_length = 0 ;

        glGetProgramiv( program_id, GL_ACTIVE_UNIFORMS, &num_active_uniforms ) ;
        natus::ogl::es::check_and_log( "[glGetProgramiv] : GL_ACTIVE_UNIFORMS" ) ;

        if( num_active_uniforms == 0 ) return ;

        config.uniforms.resize( num_active_uniforms ) ;

        glGetProgramiv( program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_length ) ;
        natus::ogl::es::check_and_log( "[glGetProgramiv] : GL_ACTIVE_UNIFORM_MAX_LENGTH" ) ;

        GLint size ;
        GLenum gl_uniform_type ;

        natus::memory::malloc_guard<char> buffer( name_length ) ;

        for( GLint i = 0; i < num_active_uniforms; ++i )
        {
            glGetActiveUniform( program_id, i, name_length, 0, 
                &size, &gl_uniform_type, buffer ) ;
            if( natus::ogl::es::check_and_log( "[gl_33_api] : glGetActiveUniform" ) ) continue ;

            // build-ins are not needed in the shader variables container.
            if( name_length >= 3 && buffer.equals( 0, 'g' ) && buffer.equals( 1, 'l' ) && buffer.equals( 2, '_' ) ) continue ;

            GLuint const location_id = glGetUniformLocation( program_id, buffer ) ;
            if( natus::ogl::es::check_and_log( "[glGetUniformLocation]" ) ) continue ;

            if( natus::log::global_t::error( location_id == GLuint( -1 ), 
                natus_log_fn( "invalid uniform location id." ) ) ) continue ;

            natus::std::string const variable_name = natus::std::string( char_cptr_t( buffer ) ) ;

            this_file_es::render_config::uniform_variable_t vd ;
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
    void_t update_all_uniforms( GLuint const /*program_id*/, this_file_es::render_config & config )
    {
        {
            glUseProgram( config.pg_id ) ;
            if( natus::ogl::es::check_and_log( natus_log_fn( "glUseProgram" ) ) )
                return ;
        }

        for( auto & v : config.uniforms )
        {
            v.do_uniform_funk() ;
        }
    }

    bool_t update( size_t const id, natus::gpu::render_configuration_cref_t rc )
    {
        auto& config = rconfigs[ id ] ;
        config.geo = nullptr ;

        this_file_es::geo_config* gconfig = nullptr ;

        // search for geometry
        {
            auto iter = ::std::find_if( geo_configs.begin(), geo_configs.end(),
                [&] ( this_file_es::geo_config const& c )
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
        }

        {
            auto vss = ::std::move( config.var_sets ) ;
            for( auto vs : vss )
            {
                auto const res = this_t::connect( id, vs ) ;
                natus::log::global_t::warning( natus::core::is_not( res ), natus_log_fn( "connect" ) ) ;
            }
        }

        {
            this_t::update_all_uniforms( config.pg_id, config ) ;
        }
        return true ;
    }

    size_t construct_geo( natus::std::string_cref_t name, natus::gpu::geometry_configuration_ref_t geo ) 
    {
        // the name is unique
        {
            auto iter = ::std::find_if( geo_configs.begin(), geo_configs.end(), 
                [&]( this_file_es::geo_config const & config )
            {
                return config.name == name ;
            } ) ;

            if( iter != geo_configs.end() )
                return iter - geo_configs.begin() ;
        }

        size_t i = 0 ;
        for( ; i < geo_configs.size(); ++i )
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
            glGenBuffers( 1, &id ) ;
            error = natus::ogl::es::check_and_log( 
                natus_log_fn( "Vertex Buffer creation" ) ) ;
            
            geo_configs[ i ].vb_id = id ;
        }

        // index buffer
        {
            GLuint id = GLuint( -1 ) ;
            glGenBuffers( 1, &id ) ;
            error = natus::ogl::es::check_and_log(
                natus_log_fn( "Index Buffer creation" ) ) ;
            
            geo_configs[ i ].ib_id = id ;
        }

        {
            geo_configs[ i ].name = name ;
            geo_configs[ i ].stride = GLuint( geo.vertex_buffer().get_layout_sib() ) ;
            geo.vertex_buffer().for_each_layout_element( 
                [&]( natus::gpu::vertex_buffer_t::data_cref_t d )
            {
                this_file_es::geo_config::layout_element le ;
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
            geo_configs[ i ].pt = natus::gpu::es3::convert( geo.primitive_type() ) ; 
        }

        return i ;
    }

    bool_t update( size_t const id, natus::gpu::geometry_configuration_res_t geo )
    {
        auto& config = geo_configs[ id ] ;

        // bind vertex buffer
        {
            glBindBuffer( GL_ARRAY_BUFFER, config.vb_id ) ;
            if( natus::ogl::es::check_and_log( natus_log_fn("glBindBuffer - vertex buffer") ) )
                return false ;
        }

        // allocate buffer memory
        // what about mapped memory?
        {
            GLuint const sib = GLuint( geo->vertex_buffer().get_sib() ) ;
            glBufferData( GL_ARRAY_BUFFER, sib, 
                geo->vertex_buffer().data(), GL_STATIC_DRAW ) ;
            if( natus::ogl::es::check_and_log( natus_log_fn( "glBufferData - vertex buffer" ) ) )
                return false ;
        }

        // bind index buffer
        {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, config.ib_id ) ;
            if( natus::ogl::es::check_and_log( natus_log_fn( "glBindBuffer - index buffer" ) ) )
                return false ;
        }

        // allocate buffer memory
        // what about mapped memory?
        {
            GLuint const sib = GLuint( geo->index_buffer().get_sib() ) ;
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, sib, 
                geo->index_buffer().data(), GL_STATIC_DRAW ) ;
            if( natus::ogl::es::check_and_log( natus_log_fn( "glBufferData - index buffer" ) ) )
                return false ;
        }

        return true ;
    }

    bool_t connect( size_t const id, natus::gpu::variable_set_res_t vs )
    {
        auto& config = rconfigs[ id ] ;

        if( this_t::connect( config, vs ) )
            this_t::update_all_uniforms( config.pg_id, config ) ;

        return true ;
    }

    bool_t connect( this_file_es::render_config & config, natus::gpu::variable_set_res_t vs )
    {
        config.var_sets.emplace_back( vs ) ;

        for( auto& uv : config.uniforms )
        {
            // is it a data uniform variable?
            if( natus::ogl::uniform_is_data( uv.type ) )
            {
                auto const types = natus::gpu::es3::to_type_type_struct( uv.type ) ;
                auto* var = vs->data_variable( uv.name, types.first, types.second ) ;
                if( natus::core::is_nullptr( var ) )
                {
                    natus::log::global_t::error( natus_log_fn( "can not claim variable " + uv.name ) ) ;
                    continue ;
                }

                uv.var = var ;
                uv.do_copy_funk() ;
            }
        }

        return true ;
    }

    bool_t render( size_t const id, GLsizei const start_element = GLsizei(0), 
        GLsizei const num_elements = GLsizei(-1) )
    {
        this_file_es::render_config& config = rconfigs[ id ] ;

        {
            glBindVertexArray( config.va_id ) ;
            if( natus::ogl::es::check_and_log( natus_log_fn( "glBindVertexArray" ) ) )
                return false ;
        }

        {
            glUseProgram( config.pg_id ) ;
            if( natus::ogl::es::check_and_log( natus_log_fn( "glUseProgram" ) ) )
                return false ;
        }

        {
            for( auto & uv : config.uniforms )
            {
                uv.do_copy_funk() ;
                uv.do_uniform_funk() ;
            }
        }

        // render section
        {
            GLenum const pt = config.geo->pt ;
            GLuint const ib = config.geo->ib_id ;

            if( ib != GLuint(-1) )
            {
                GLsizei const max_elems = GLsizei( config.geo->num_elements_ib ) ;
                GLsizei const ne = ::std::min( num_elements>=0?num_elements:max_elems, max_elems ) ;

                GLenum const glt = config.geo->ib_type ;
                
                void_cptr_t offset = void_cptr_t( byte_cptr_t( nullptr ) + 
                    start_element * GLsizei( config.geo->ib_elem_sib ) ) ;
                
                glDrawElements( pt, ne, glt, offset ) ;

                natus::ogl::es::check_and_log( natus_log_fn( "glDrawElements" ) ) ;
            }
            else
            {
                GLsizei const max_elems = GLsizei( config.geo->num_elements_vb ) ;
                GLsizei const ne = ::std::min( num_elements, max_elems ) ;

                glDrawArrays( pt, start_element, ne ) ;

                natus::ogl::es::check_and_log( natus_log_fn( "glDrawArrays" ) ) ;
            }
        }
        return true ;
    }

    void_t begin_frame( void_t ) 
    {
        //natus::ogl::gl::glClearColor( 0.1f, 0.1f, 0.1f, 1.0f ) ;
        //natus::ogl::error::check_and_log( natus_log_fn( "glClearColor" ) ) ;

        //natus::ogl::gl::glClear( GL_COLOR_BUFFER_BIT ) ;
        //natus::ogl::error::check_and_log( natus_log_fn( "glClear" ) ) ;
        
        glViewport( 0, 0, vp_width, vp_height ) ;
        natus::ogl::es::check_and_log( natus_log_fn( "glViewport" ) ) ;
    }

    void_t end_frame( void_t ) 
    {
        glFlush() ;
        glFinish() ;
    }
};
//************************************************************************************************
//
//
//************************************************************************************************

//****
es3_backend::es3_backend( void_t ) noexcept : backend( natus::gpu::backend_type::es3 )
{
    _pimpl = natus::memory::global_t::alloc( pimpl(), 
        natus_log_fn("es3_backend::pimpl") ) ;
}

//****
es3_backend::es3_backend( this_rref_t rhv ) noexcept : backend( ::std::move( rhv ) )
{
    natus_move_member_ptr( _pimpl, rhv ) ;
}

//****
es3_backend::~es3_backend( void_t ) 
{
    if( natus::core::is_not_nullptr( _pimpl ) )
        natus::memory::global_t::dealloc( _pimpl ) ;
}

//****
void_t es3_backend::set_window_info( window_info_cref_t wi ) noexcept 
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
natus::gpu::id_t es3_backend::configure( id_rref_t id,
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
natus::gpu::id_t es3_backend::configure( id_rref_t id, natus::gpu::render_configuration_res_t config ) noexcept 
{
    {
        id = natus::gpu::id_t( this_t::get_bid(),
            _pimpl->construct_rconfig( id.get_oid(), config->name(), *config ) ) ;
    }

    if( id.is_not_bid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return ::std::move( id ) ;
    }

    size_t const oid = id.get_oid() ;

    {
        auto const res = _pimpl->update( id.get_oid(), *config ) ;
        if( natus::core::is_not( res ) )
        {
            return ::std::move( id ) ;
        }
    }

    return ::std::move( id ) ;
}

//***
natus::gpu::id_t es3_backend::connect( id_rref_t id, natus::gpu::variable_set_res_t vs ) noexcept
{
    if( id.is_not_valid() )
    {
        natus::log::global_t::error( natus_log_fn( "invalid render configuration id" ) ) ;
        return ::std::move( id ) ;
    }

    auto const res = _pimpl->connect( id.get_oid(), vs ) ;

   
    return ::std::move( id ) ;
}

//****
natus::gpu::id_t es3_backend::render( id_rref_t id ) noexcept 
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

//****
void_t es3_backend::render_begin( void_t ) noexcept 
{
    _pimpl->begin_frame() ;
}

void_t es3_backend::render_end( void_t ) noexcept 
{
    _pimpl->end_frame() ;
}
