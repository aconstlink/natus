

#include "gl3.h"
#include "gl3_convert.h"

#include "../../buffer/vertex_buffer.hpp"
#include "../../buffer/index_buffer.hpp"

#include <natus/ogl/gl/gl.h>
#include <natus/ogl/gl/convert.hpp>
#include <natus/ogl/gl/error.hpp>
#include <natus/memory/global.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/ntd/vector.hpp>
#include <natus/ntd/stack.hpp>
#include <natus/ntd/string/split.hpp>

using namespace natus::graphics ;
using namespace natus::ogl ;

struct gl3_backend::pimpl
{
    natus_this_typedefs( pimpl ) ;

    struct geo_data
    {
        bool_t valid = false ;
        natus::ntd::string_t name ;

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
            natus::graphics::vertex_attribute va ;
            natus::graphics::type type ;
            natus::graphics::type_struct type_struct ;

            GLuint sib( void_t ) const noexcept
            {
                return GLuint( natus::graphics::size_of( type ) *
                    natus::graphics::size_of( type_struct ) ) ;
            }
        };
        natus::ntd::vector< layout_element > elements ;

        // per vertex sib
        GLuint stride = 0 ;

        GLenum ib_type ;
        size_t ib_elem_sib = 0  ;
        GLenum pt ;
    };
    natus_typedef( geo_data ) ;

    struct shader_data
    {
        bool_t valid = false ;

        natus::ntd::string_t name ;

        GLuint vs_id = GLuint( -1 ) ;
        GLuint gs_id = GLuint( -1 ) ;
        GLuint ps_id = GLuint( -1 ) ;
        GLuint pg_id = GLuint( -1 ) ;

        struct vertex_input_binding
        {
            natus::graphics::vertex_attribute va ;
            natus::ntd::string_t name ;
        };
        natus::ntd::vector< vertex_input_binding > vertex_inputs ;

        bool_t find_vertex_input_binding_by_name( natus::ntd::string_cref_t name_,
            natus::graphics::vertex_attribute& va ) const noexcept
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
            natus::graphics::vertex_attribute va ;
            natus::ntd::string_t name ;
            GLuint loc ;
            GLenum type ;
        };
        natus_typedef( attribute_variable ) ;

        natus::ntd::vector< attribute_variable_t > attributes ;

        struct uniform_variable
        {
            natus::ntd::string_t name ;
            GLuint loc ;
            GLenum type ;

            // this variable's memory location
            void_ptr_t mem = nullptr ;

            // the GL uniform function
            natus::ogl::uniform_funk_t uniform_funk ;

            // set by the user in user-space
            //natus::graphics::ivariable_ptr_t var ;

            void_t do_uniform_funk( void_t )
            {
                uniform_funk( loc, 1, mem ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glUniform" ) ) ;
            }

            void_t do_copy_funk( natus::graphics::ivariable_ptr_t var )
            {
                ::std::memcpy( mem, var->data_ptr(), natus::ogl::uniform_size_of( type ) ) ;
            }
        };
        natus_typedef( uniform_variable ) ;

        natus::ntd::vector< uniform_variable > uniforms ;

        void_ptr_t uniform_mem = nullptr ;
    } ;
    natus_typedef( shader_data ) ;

    struct render_state_sets
    {
        struct clear_state_set
        {
            bool_t change = false ;
            bool_t enable = false ;
            natus::math::vec4f_t color ;
            GLenum clear_bits ;
        } ;
        clear_state_set clear_s ;

        struct view_state_set
        {
            bool_t change = false ;
            bool_t enable = false ;
            natus::math::vec4ui_t vp ;
        } ;
        view_state_set view_s ;

        struct blend_state_set
        {
            bool_t change = false ;
            bool_t enable = false ;
            GLenum src_blend_fac ;
            GLenum dst_blend_fac ;
            GLenum blend_func ;
        };
        blend_state_set blend_s ;

        struct depth_state_set
        {
            bool_t change = false ;
            bool_t enable = false ;
            bool_t write = false ;
        } ;
        depth_state_set depth_s ;

        struct scissor_state_set
        {
            bool_t change = false ;
            bool_t enable = false ;
            natus::math::vec4ui_t rect ;
        } ;
        scissor_state_set scissor_s ;

        struct polygon_state_set
        {
            bool_t change = false ;
            bool_t enable ;
            GLenum cull_mode ;
            GLenum front_face ;
            GLenum fill_mode ;
        };
        polygon_state_set polygon_s ;
    };
    natus_typedef( render_state_sets ) ;

    struct state_data
    {
        bool_t valid = false ;
        natus::ntd::string_t name ;
        natus::ntd::vector< render_state_sets > states ;
    } ;
    natus_typedef( state_data ) ;

    struct render_data
    {
        bool_t valid = false ;

        natus::ntd::string_t name ;

        size_t geo_id = size_t( -1 ) ;
        size_t shd_id = size_t( -1 ) ;

        struct uniform_variable_link
        {
            // the index into the shader_config::uniforms array
            size_t uniform_id ;
            // the user variable holding the data.
            natus::graphics::ivariable_ptr_t var ;
        };

        // user provided variable set
        natus::ntd::vector< std::pair<
            natus::graphics::variable_set_res_t,
            natus::ntd::vector< uniform_variable_link > > > var_sets_data ;

        struct uniform_texture_link
        {
            // the index into the shader_config::uniforms array
            size_t uniform_id ;
            GLint tex_id ;
            size_t img_id ;
        };
        natus::ntd::vector< std::pair<
            natus::graphics::variable_set_res_t,
            natus::ntd::vector< uniform_texture_link > > > var_sets_texture ;
        
        natus::ntd::vector< render_state_sets > rss ;
    };
    natus_typedef( render_data ) ;

    struct image_config
    {
        natus::ntd::string_t name ;

        GLuint tex_id = GLuint( -1 ) ;
        size_t sib = 0 ;

        GLenum wrap_types[ 3 ] ;
        GLenum filter_types[ 2 ] ;

        // sampler ids for gl>=3.3
    };

    struct framebuffer_data
    {
        bool_t valid = false ;

        natus::ntd::string_t name ;

        GLuint gl_id = GLuint( -1 ) ;

        size_t nt = 0 ;
        GLuint colors[ 8 ] = {
            GLuint( -1 ), GLuint( -1 ), GLuint( -1 ), GLuint( -1 ),
            GLuint( -1 ), GLuint( -1 ), GLuint( -1 ), GLuint( -1 ) } ;

        GLuint depth = GLuint( -1 ) ;

        void_ptr_t mem_ptr = nullptr ;

        natus::math::vec2ui_t dims ;

    };
    natus_typedef( framebuffer_data ) ;

    typedef natus::ntd::vector< this_t::shader_data > shaders_t ;
    shaders_t shaders ;

    typedef natus::ntd::vector< this_t::render_data > rconfigs_t ;
    rconfigs_t rconfigs ;

    typedef natus::ntd::vector< this_t::geo_data > geo_configs_t ;
    geo_configs_t geo_configs ;

    typedef natus::ntd::vector< this_t::image_config > image_configs_t ;
    image_configs_t img_configs ;

    typedef natus::ntd::vector< this_t::framebuffer_data_t > framebuffers_t ;
    framebuffers_t _framebuffers ;

    typedef natus::ntd::vector< this_t::state_data_t > states_t ;
    states_t _states ;
    natus::ntd::stack< std::pair<size_t,size_t>, 10 > _state_id_stack ;

    GLsizei vp_width = 0 ;
    GLsizei vp_height = 0 ;

    natus::graphics::backend_type const bt = natus::graphics::backend_type::gl3 ;

    // the current render state set
    natus::graphics::render_state_sets_t render_states ;

    natus::graphics::gl_context_ptr_t _ctx ;

    pimpl( natus::graphics::gl_context_ptr_t ctx ) 
    {
        _ctx = ctx ;

        {
            auto new_states = *natus::graphics::backend_t::default_render_states() ;

            natus::graphics::render_state_sets old_states = new_states ;
            old_states.blend_s.do_change = !new_states.blend_s.do_change ;
            old_states.clear_s.do_change = !new_states.clear_s.do_change ;
            old_states.depth_s.do_change = !new_states.depth_s.do_change ;
            old_states.polygon_s.do_change = !new_states.polygon_s.do_change ;
            old_states.scissor_s.do_change = !new_states.scissor_s.do_change ;
            old_states.view_s.do_change = !new_states.view_s.do_change ;

            old_states.blend_s.do_activate = !new_states.blend_s.do_activate ;
            old_states.clear_s.do_activate = !new_states.clear_s.do_activate ;
            old_states.depth_s.do_activate = !new_states.depth_s.do_activate ;
            old_states.polygon_s.do_activate = !new_states.polygon_s.do_activate ;
            old_states.scissor_s.do_activate = !new_states.scissor_s.do_activate ;
            old_states.view_s.do_activate = !new_states.view_s.do_activate ;
            
            natus::graphics::state_object_t obj( "gl3_default_states" ) ;
            
            // store the invalid state at the bottom
            // store the opposed default state at position 1
            // so during frame begin, all default states can be
            // restored useing those two state sets.
            obj.add_render_state_set( old_states ) ;
            obj.add_render_state_set( new_states ) ;

            size_t const oid = this_t::construct_state( size_t( -1 ), obj ) ;
            _state_id_stack.push( std::make_pair( oid, size_t( 0 ) ) ) ;
            _state_id_stack.push( std::make_pair( oid, size_t( 1 ) ) ) ;

            update_state( oid, obj ) ;
        }
    }

    template< typename T >
    static size_t determine_oid( natus::ntd::string_cref_t name, natus::ntd::vector< T >& v ) noexcept
    {
        size_t oid = size_t( -1 ) ;

        {
            auto iter = std::find_if( v.begin(), v.end(), [&] ( T const& c )
            {
                return c.name == name ;
            } ) ;

            if( iter != v.end() )
            {
                oid = std::distance( v.begin(), iter ) ;
            }
        }

        if( oid == size_t( -1 ) )
        {
            for( size_t i = 0 ; i < v.size(); ++i )
            {
                if( !v[ i ].valid )
                {
                    oid = i ;
                    break ;
                }
            }
        }

        if( oid >= v.size() )
        {
            oid = v.size() ;
            v.resize( oid + 1 ) ;
        }

        v[ oid ].valid = true ;
        v[ oid ].name = name ;

        return oid ;
    }

    size_t construct_state( size_t oid, natus::graphics::state_object_ref_t obj ) noexcept
    {
        oid = determine_oid( obj.name(), _states ) ;

        auto& states = _states[ oid ] ;

        states.states.resize( obj.size() ) ;

        // @note
        // set all enables. Enable/Disable only possible during construction
        // values are assigned in the update function for the render states
        obj.for_each( [&] ( size_t const i, natus::graphics::render_state_sets_cref_t rs )
        {
            this_t::render_state_sets rs_ ;

            rs_.view_s.change = rs.view_s.do_change ;
            rs_.blend_s.change = rs.blend_s.do_change ;
            rs_.clear_s.change = rs.clear_s.do_change ;
            rs_.depth_s.change = rs.depth_s.do_change ;
            rs_.polygon_s.change = rs.polygon_s.do_change  ;
            rs_.scissor_s.change = rs.scissor_s.do_change ;
            states.states[ i ] = rs_ ;
        } ) ;

        return oid ;
    }

    size_t update_state( size_t const oid, natus::graphics::state_object_ref_t obj ) noexcept
    {
        auto& states = _states[ oid ] ;

        // @note
        // set all values
        obj.for_each( [&] ( size_t const i, natus::graphics::render_state_sets_cref_t rs )
        {
            this_t::render_state_sets & rs_ = states.states[i] ;

            if( rs_.blend_s.change )
            {
                rs_.blend_s.enable = rs.blend_s.do_activate ;
                rs_.blend_s.blend_func = natus::graphics::gl3::convert( rs.blend_s.blend_func ) ;
                rs_.blend_s.src_blend_fac = natus::graphics::gl3::convert( rs.blend_s.src_blend_factor ) ;
                rs_.blend_s.dst_blend_fac = natus::graphics::gl3::convert( rs.blend_s.dst_blend_factor  ) ;
            }
            if( rs_.clear_s.change )
            {
                rs_.clear_s.enable = rs.clear_s.do_activate ;
                rs_.clear_s.color = rs.clear_s.clear_color ;
                rs_.clear_s.clear_bits =
                    ( rs.clear_s.do_color ? GL_COLOR_BUFFER_BIT : 0 ) |
                    ( rs.clear_s.do_depth ? GL_DEPTH_BUFFER_BIT : 0 ) ;
            }
            if( rs_.view_s.change )
            {
                rs_.view_s.enable = rs.view_s.do_activate ;
                rs_.view_s.vp = rs.view_s.vp ;
            }
            if( rs_.depth_s.change )
            {
                rs_.depth_s.write = rs.depth_s.do_depth_write ;
                rs_.depth_s.enable = rs.depth_s.do_activate ;
                rs_.depth_s.write = rs.depth_s.do_depth_write ;
            }
            if( rs_.polygon_s.change )
            {
                rs_.polygon_s.enable = rs.polygon_s.do_activate ;

                // glCullFace
                rs_.polygon_s.cull_mode = natus::graphics::gl3::convert( rs.polygon_s.cm ) ;

                // glFrontFace
                rs_.polygon_s.front_face = natus::graphics::gl3::convert( rs.polygon_s.ff ) ;

                // glPolygonMode
                rs_.polygon_s.fill_mode = natus::graphics::gl3::convert( rs.polygon_s.fm ) ;
            }
            if( rs_.scissor_s.change )
            {
                rs_.scissor_s.enable = rs.scissor_s.do_activate ;
            }
        } ) ;
        return oid ;
    }

    // @param new_state states to be activated
    // @param old_state states that were just activated
    void_t handle_render_state( render_state_sets const & new_states, render_state_sets const & old_states, bool_t const revert = false )
    {
        // depth test
        {
            if( new_states.depth_s.change )
            {
                if( new_states.depth_s.enable )
                {
                    glEnable( GL_DEPTH_TEST );
                    natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;

                    glDepthMask( GL_TRUE );
                    natus::ogl::error::check_and_log( natus_log_fn( "glDepthMask" ) ) ;

                    glDepthFunc( GL_LESS ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glDepthFunc" ) ) ;
                }
                else
                {
                    glDisable( GL_DEPTH_TEST ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glDisable( GL_DEPTH_TEST )" ) ) ;

                    glDepthMask( GL_FALSE ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glDepthMask" ) ) ;
                }
            }
            // the new states do not change, so the old stats need to be reverted.
            else if( revert )
            {
                if( old_states.depth_s.change )
                {
                    if( old_states.depth_s.enable )
                    {
                        glDisable( GL_DEPTH_TEST ) ;
                        natus::ogl::error::check_and_log( natus_log_fn( "glDisable( GL_DEPTH_TEST )" ) ) ;

                        glDepthMask( GL_FALSE ) ;
                        natus::ogl::error::check_and_log( natus_log_fn( "glDepthMask" ) ) ;

                        glDepthFunc( GL_LESS ) ;
                        natus::ogl::error::check_and_log( natus_log_fn( "glDepthFunc" ) ) ;
                    }
                    else
                    {
                        glEnable( GL_DEPTH_TEST );
                        natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;

                        glDepthMask( GL_TRUE );
                        natus::ogl::error::check_and_log( natus_log_fn( "glDepthMask" ) ) ;
                    }
                }
            }

            
        }

        // clear always if true.
        {
            if( new_states.clear_s.change && !revert )
            {
                if( new_states.clear_s.enable )
                {
                    glClearColor( new_states.clear_s.color.x(), new_states.clear_s.color.y(),
                        new_states.clear_s.color.z(), new_states.clear_s.color.w() ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glClearColor" ) ) ;

                    glClear( new_states.clear_s.clear_bits );
                    natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;
                }
            }
        }

        // blend mode
        {
            if( new_states.blend_s.change )
            {
                if( new_states.blend_s.enable )
                {
                    glEnable( GL_BLEND ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;
                }
                else
                {
                    glDisable( GL_BLEND ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
                }

                if( new_states.blend_s.enable )
                {
                    GLenum const glsrc = new_states.blend_s.src_blend_fac ;
                    GLenum const gldst = new_states.blend_s.dst_blend_fac ;
                    glBlendFunc( glsrc, gldst ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glBlendFunc" ) ) ;
                }
            }
        }

        // polygon mode
        {
            if( new_states.polygon_s.change )
            {
                if( new_states.polygon_s.enable )
                {
                    glEnable( GL_CULL_FACE ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;
                }
                else
                {
                    glDisable( GL_CULL_FACE ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
                }

                if( new_states.polygon_s.enable )
                {
                    glCullFace( new_states.polygon_s.cull_mode ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glCullFace" ) ) ;

                    glFrontFace( new_states.polygon_s.front_face ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glFrontFace" ) ) ;

                    glPolygonMode( GL_FRONT_AND_BACK, new_states.polygon_s.fill_mode ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glPolygonMode" ) ) ;
                }
            }
        }

        // scissor
        {
            if( new_states.scissor_s.change )
            {
                if( new_states.scissor_s.enable )
                {
                    glEnable( GL_SCISSOR_TEST ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;
                }
                else
                {
                    glDisable( GL_SCISSOR_TEST ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
                }

                if( new_states.scissor_s.enable )
                {
                    glScissor(
                        GLint( new_states.scissor_s.rect.x() ), GLint( new_states.scissor_s.rect.y() ),
                        GLsizei( new_states.scissor_s.rect.z() ), GLsizei( new_states.scissor_s.rect.w() ) ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glScissor" ) ) ;
                }
            }
        }

        // always set the viewport if enabled
        {
            if( new_states.view_s.change )
            {
                if( new_states.view_s.enable )
                {
                    auto const& vp = new_states.view_s.vp  ;
                    glViewport( GLint( vp.x() ), GLint( vp.y() ), GLsizei( vp.z() ), GLsizei( vp.w() ) ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glViewport" ) ) ;
                }
            }
        }
    }

    // if oid == -1, the state is popped.
    void_t handle_render_state( size_t const oid, size_t const rs_id ) noexcept
    {
        auto new_id = std::make_pair( oid, rs_id ) ;
        auto old_id = _state_id_stack.top() ;

        // pop state
        if( oid == size_t(-1) )
        {
            if( _state_id_stack.size() == 2 )
            {
                natus::log::global_t::error( natus_log_fn( "no more render states to pop" ) ) ;
                return ;
            }
            // was active
            old_id = _state_id_stack.pop() ;
            // will be activated
            new_id = _state_id_stack.top() ;
        }

        {
            auto const & new_states = _states[ new_id.first ].states[ new_id.second ] ;
            auto const & old_states = _states[ old_id.first ].states[ old_id.second ] ;

            this_t::handle_render_state( new_states, old_states, oid == size_t(-1) ) ;
        }

        if( oid != size_t( -1 ) )
        {
            _state_id_stack.push( new_id ) ;
        }
    }

    size_t construct_framebuffer( size_t oid, natus::graphics::framebuffer_object_ref_t obj ) noexcept
    {
        oid = determine_oid( obj.name(), _framebuffers ) ;

        framebuffer_data_ref_t fb = _framebuffers[ oid ] ;

        if( fb.gl_id == GLuint( -1 ) )
        {
            glGenFramebuffers( 1, &fb.gl_id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenFramebuffers" ) ) ;
        }

        if( fb.gl_id == GLuint( -1 ) )return oid ;
        
        // bind
        {
            glBindFramebuffer( GL_FRAMEBUFFER, fb.gl_id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenFramebuffers" ) ) ;
        }

        size_t const nt = obj.get_num_color_targets() ;
        auto const ctt = obj.get_color_target() ;
        auto const dst = obj.get_depth_target();
        natus::math::vec2ui_t dims = obj.get_dims() ;
        
        // fix dims
        {
            dims.x( dims.x() + dims.x() % 2 ) ;
            dims.y( dims.y() + dims.y() % 2 ) ;
        }

        // construct color textures
        {
            glDeleteTextures( GLsizei( fb.nt ), fb.colors ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDeleteTextures" ) ) ;

            glGenTextures( GLsizei( nt ), fb.colors ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenTextures" ) ) ;

            for( size_t i=0; i<nt; ++i )
            {
                GLuint const tid = fb.colors[i] ;

                glBindTexture( GL_TEXTURE_2D, tid ) ;
                if( natus::ogl::error::check_and_log( natus_log_fn( "glBindTexture" ) ) )
                    continue ;

                GLenum const target = GL_TEXTURE_2D ;
                GLint const level = 0 ;
                GLsizei const width = dims.x() ;
                GLsizei const height = dims.y() ;
                GLenum const format = GL_RGBA ;
                GLenum const type = natus::graphics::gl3::to_pixel_type( ctt ) ;
                GLint const border = 0 ;
                GLint const internal_format = natus::graphics::gl3::to_gl_format( ctt ) ;

                // maybe required for memory allocation
                // at the moment, render targets do not have system memory.
                #if 0
                size_t const sib = natus::graphics::gl3::calc_sib( dims.x(), dims.y(), ctt ) ;
                #endif
                void_cptr_t data = nullptr ;

                glTexImage2D( target, level, internal_format, width, height, border, format, type, data ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glTexImage2D" ) ) ;
            }

            // attach
            for( size_t i = 0; i < nt; ++i )
            {
                GLuint const tid = fb.colors[ i ] ;
                GLenum const att = GLenum( size_t( GL_COLOR_ATTACHMENT0 ) + i ) ;
                glFramebufferTexture2D( GL_FRAMEBUFFER, att, GL_TEXTURE_2D, tid, 0 ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glFramebufferTexture2D" ) ) ;
            }
        }

        // depth/stencil
        if( dst != natus::graphics::depth_stencil_target_type::unknown )
        {
            glDeleteTextures( GLsizei( 1 ), &fb.depth ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDeleteTextures" ) ) ;

            glGenTextures( GLsizei( 1 ), &fb.depth ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenTextures" ) ) ;

            {
                GLuint const tid = fb.depth ;

                glBindTexture( GL_TEXTURE_2D, tid ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glBindTexture" ) ) ;

                GLenum const target = GL_TEXTURE_2D ;
                GLint const level = 0 ;
                GLsizei const width = dims.x() ;
                GLsizei const height = dims.y() ;
                GLenum const format = natus::graphics::gl3::to_gl_format( dst ) ;
                GLenum const type = natus::graphics::gl3::to_gl_type( dst ) ;
                GLint const border = 0 ;
                GLint const internal_format = natus::graphics::gl3::to_gl_format( dst ) ;

                // maybe required for memory allocation
                // at the moment, render targets do not have system memory.
                #if 0
                size_t const sib = natus::graphics::gl3::calc_sib( dims.x(), dims.y(), ctt ) ;
                #endif
                void_cptr_t data = nullptr ;

                glTexImage2D( target, level, internal_format, width, height, border, format, type, data ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glTexImage2D" ) ) ;
            }

            // attach
            {
                GLuint const tid = fb.depth ;
                GLenum const att = natus::graphics::gl3::to_gl_attachment(dst) ;
                glFramebufferTexture2D( GL_FRAMEBUFFER, att, GL_TEXTURE_2D, tid, 0 ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glFramebufferTexture2D" ) ) ;
            }
        }


        GLenum status = 0 ;
        // validate
        {
            status = glCheckFramebufferStatus( GL_FRAMEBUFFER ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glCheckFramebufferStatus" ) ) ;

            natus::log::global_t::warning( status != GL_FRAMEBUFFER_COMPLETE, 
                "Incomplete framebuffer : [" + obj.name() + "]" ) ;
        }

        // unbind
        {
            glBindFramebuffer( GL_FRAMEBUFFER, 0 ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenFramebuffers" ) ) ;
        }

        // remember data
        if( status == GL_FRAMEBUFFER_COMPLETE )
        {
            // color type maybe?
            fb.nt = nt ;
            fb.dims = dims ;
        }

        // store images
        {
            size_t const id = img_configs.size() ;
            img_configs.resize( img_configs.size() + nt ) ;

            for( size_t i = 0; i < nt; ++i )
            {
                size_t const idx = id + i ;
                img_configs[ idx ].name = fb.name + "." + std::to_string( i ) ;
                img_configs[ idx ].tex_id = fb.colors[ i ] ;

                for( size_t j = 0; j < ( size_t ) natus::graphics::texture_wrap_mode::size; ++j )
                {
                    img_configs[ idx ].wrap_types[ j ] = GL_CLAMP_TO_BORDER ;
                }

                for( size_t j = 0; j < ( size_t ) natus::graphics::texture_filter_mode::size; ++j )
                {
                    img_configs[ idx ].filter_types[ j ] = GL_LINEAR ;
                }
            }
        }

        // store depth/stencil
        {
            size_t const id = img_configs.size() ;
            img_configs.resize( img_configs.size() + 1 ) ;

            {
                size_t const idx = id + 0 ;
                img_configs[ idx ].name = fb.name + ".depth" ;
                img_configs[ idx ].tex_id = fb.depth ;

                for( size_t j = 0; j < ( size_t ) natus::graphics::texture_wrap_mode::size; ++j )
                {
                    img_configs[ idx ].wrap_types[ j ] = GL_REPEAT ;
                }

                for( size_t j = 0; j < ( size_t ) natus::graphics::texture_filter_mode::size; ++j )
                {
                    img_configs[ idx ].filter_types[ j ] = GL_NEAREST ;
                }
            }
        }

        return oid ;
    }

    bool_t activate_framebuffer( size_t const oid )
    {
        framebuffer_data_ref_t fb = _framebuffers[ oid ] ;

        // bind
        {
            glBindFramebuffer( GL_FRAMEBUFFER, fb.gl_id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenFramebuffers" ) ) ;
        }

        // setup color
        {
            GLenum attachments[ 15 ] ;
            size_t const num_color = fb.nt ;
            
            for( size_t i = 0; i < num_color; ++i )
            {
                attachments[ i ] = GLenum( size_t( GL_COLOR_ATTACHMENT0 ) + i ) ;
            }

            glDrawBuffers( GLsizei( num_color ), attachments ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenFramebuffers" ) ) ;
        }

        // there is only one dimension at the moment
        {
            glViewport( 0, 0, GLsizei( fb.dims.x() ), GLsizei( fb.dims.y() ) ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glViewport" ) ) ;
        }

        return true ;
    }

    void_t deactivate_framebuffer( void_t )
    {
        // unbind
        {
            glBindFramebuffer( GL_FRAMEBUFFER, 0 ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenFramebuffers" ) ) ;
        }

        // reset viewport
        {
            glViewport( 0, 0, vp_width, vp_height ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glViewport" ) ) ;
        }

        glDisable( GL_DEPTH_TEST );
        natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;
    }

    size_t construct_shader_config( size_t oid,
        natus::graphics::shader_object_ref_t obj ) noexcept
    {
        oid = determine_oid( obj.name(), shaders ) ;

        //
        // Do Configuration
        //

        // program
        if( shaders[ oid ].pg_id == GLuint( -1 ) )
        {
            GLuint const id = glCreateProgram() ;
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
            GLuint const id = glCreateShader( GL_VERTEX_SHADER ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "Vertex Shader creation" ) ) ;

            shaders[ oid ].vs_id = id ;
        }
        {
            glAttachShader( shaders[ oid ].pg_id, shaders[oid].vs_id ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "Attaching vertex shader" ) ) ;
        }

        natus::graphics::shader_set_t ss ;
        {
            auto const res = obj.shader_set( this_t::bt, ss ) ;
            if( natus::core::is_not(res) )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "config [" + obj.name() + "] has no shaders for " + 
                    natus::graphics::to_string( this_t::bt ) ) ) ;
                return oid ;
            }
        }

        // geometry shader
        if( ss.has_geometry_shader() )
        {
            GLuint id = shaders[ oid ].gs_id ;

            if( id == GLuint(-1) )
            {
                id = glCreateShader( GL_GEOMETRY_SHADER ) ;
                natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader creation" ) ) ;

                shaders[ oid ].gs_id = id ;
            }

            GLuint const pid = shaders[ oid ].pg_id ;

            glAttachShader( pid, id ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "Attaching geometry shader" ) ) ;

            // check max output vertices
            {
                GLint max_out = 0 ;
                glGetIntegerv( GL_MAX_GEOMETRY_OUTPUT_VERTICES, &max_out ) ;
                natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader Max Output Vertices" ) ) ;
                ( void_t ) max_out ;
            }

            // @todo geometry shader program parameters
            {
                /*
                glProgramParameteri( pid, GL_GEOMETRY_INPUT_TYPE, 
                    ( GLint ) so_gl::convert( shd_ptr->get_input_type() ) ) ;
                error = natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader Input Type" ) ) ;

                glProgramParameteri( pid, GL_GEOMETRY_OUTPUT_TYPE, 
                    ( GLint ) so_gl::convert( shd_ptr->get_output_type() ) ) ;
                error = natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader Output Type" ) ) ;

                glProgramParameteri( pid, GL_GEOMETRY_VERTICES_OUT, 
                    ( GLint ) shd_ptr->get_num_output_vertices() ) ;
                error = natus::ogl::error::check_and_log(
                    natus_log_fn( "Geometry Shader Vertices Out" ) ) ;
                    */
            }
        }
        else if( shaders[ oid ].gs_id != GLuint(-1) )
        {
            glDeleteShader( shaders[ oid ].gs_id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDeleteShader" ) ) ;
            shaders[ oid ].gs_id = GLuint( -1 ) ;
        }

        // pixel shader
        if( ss.has_pixel_shader() )
        {
            GLuint id = shaders[ oid ].ps_id ;
            if( id == GLuint(-1) )
            {
                id = glCreateShader( GL_FRAGMENT_SHADER ) ;
                natus::ogl::error::check_and_log(
                    natus_log_fn( "Fragment Shader creation" ) ) ;
            }

            glAttachShader( shaders[ oid ].pg_id, id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "Attaching pixel shader" ) ) ;

            shaders[ oid ].ps_id = id ;
        }
        else if( shaders[ oid ].ps_id != GLuint( -1 ) )
        {
            glDeleteShader( shaders[ oid ].ps_id ) ;
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

        glGetAttachedShaders( program_id, 10, &count, shaders_ ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glGetAttachedShaders" ) ) ;

        for( GLsizei i = 0; i < count; ++i )
        {
            glDetachShader( program_id, shaders_[ i ] ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glDetachShader" ) ) ;
        }
    }

    //***********************
    void_t delete_all_variables( this_t::shader_data & config )
    {
        config.vertex_inputs.clear() ;

        //for( auto & v : config.attributes )
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
    bool_t compile_shader( GLuint const id, natus::ntd::string_cref_t code )
    {
        if( code.empty() ) return true ;

        GLchar const* source_string = ( GLchar const* ) ( code.c_str() ) ;

        glShaderSource( id, 1, &source_string, 0 ) ;
        if( natus::ogl::error::check_and_log( natus_log_fn( "glShaderSource" ) ) )
            return false ;

        glCompileShader( id ) ;
        if( natus::ogl::error::check_and_log( natus_log_fn( "glCompileShader" ) ) )
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

            natus::ntd::vector< natus::ntd::string_t > tokens ;
            natus::ntd::string_ops::split( natus::ntd::string_t( info_log ), '\n', tokens ) ;

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
        if( natus::ogl::error::check_and_log( natus_log_fn( "glLinkProgram" ) ) )
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
            if( natus::ogl::error::check_and_log( natus_log_fn( "glGetProgramInfoLog" ) ) )
                return false ;

            std::string info_log_string = std::string( ( const char* ) info_log ) ;

            {
                natus::ntd::vector< natus::ntd::string_t > tokens ;
                natus::ntd::string_ops::split( natus::ntd::string_t( info_log ), '\n', tokens ) ;

                for( auto token : tokens )
                {
                    natus::log::global_t::error( token ) ;
                }
            }
        }
        return false ;
    }

    //***********************
    void_t post_link_attributes( this_t::shader_data & config )
    {
        GLuint const program_id = config.pg_id ;

        GLint num_active_attributes = 0 ;
        GLint name_length = 0 ;

        glGetProgramiv( program_id, GL_ACTIVE_ATTRIBUTES, &num_active_attributes ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glGetProgramiv(GL_ACTIVE_ATTRIBUTES)" ) ) ;

        if( num_active_attributes == 0 ) return ;

        config.attributes.resize( num_active_attributes ) ;

        glGetProgramiv( program_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &name_length ) ;
        natus::ogl::error::check_and_log( natus_log_fn( 
            "glGetProgramiv(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH)" ) ) ;

        GLint size ;
        GLenum gl_attrib_type ;

        natus::memory::malloc_guard<char> buffer( name_length ) ;
        
        for( GLint i = 0; i < num_active_attributes; ++i )
        {
            glGetActiveAttrib( program_id, i, name_length, 0, 
                &size, &gl_attrib_type, buffer ) ;

            if( natus::ogl::error::check_and_log( "glGetActiveAttrib failed. continue loop." ) ) continue ;

            // build-ins are not needed in the shader variables container.
            if( name_length >= 3 && buffer.equals( 0, 'g' ) && buffer.equals( 1, 'l' ) && buffer.equals( 2, '_' ) ) continue ;

            GLuint const location_id = glGetAttribLocation( program_id, buffer ) ;
            if( natus::ogl::error::check_and_log( "glGetAttribLocation failed. continue loop." ) ) continue ;

            natus::ntd::string_t const variable_name = natus::ntd::string_t( ( const char* ) buffer ) ;

            this_t::shader_data::attribute_variable_t vd ;
            vd.name = ::std::move( variable_name ) ;
            vd.loc = location_id ;
            vd.type = gl_attrib_type ;
            
            {
                natus::graphics::vertex_attribute va = natus::graphics::vertex_attribute::undefined ;
                auto const res = config.find_vertex_input_binding_by_name( vd.name, va ) ;
                natus::log::global_t::error( natus::core::is_not( res ), 
                    natus_log_fn("can not find vertex attribute - " + vd.name ) ) ;
                vd.va = va ;
            }
            config.attributes[i] = vd ;
        }
    }

    //***********************
    bool_t bind_attributes( this_t::shader_data & sconfig, this_t::geo_data & gconfig )
    {
        // bind vertex array object
        {
            glBindVertexArray( gconfig.va_id ) ;
            if( natus::ogl::error::check_and_log(
                natus_log_fn( "glBindVertexArray" ) ) )
                return false ;
        }

        // bind vertex buffer
        {
            glBindBuffer( GL_ARRAY_BUFFER, gconfig.vb_id ) ;
            if( natus::ogl::error::check_and_log( 
                natus_log_fn("glBindBuffer(GL_ARRAY_BUFFER)") ) ) 
                return false ;
        }

        // bind index buffer
        {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gconfig.ib_id ) ;
            if( natus::ogl::error::check_and_log( 
                natus_log_fn( "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER)" ) ) )
                return false ;
        }

        // disable old locations
        for( size_t i = 0; i < sconfig.attributes.size(); ++i )
        {
            glDisableVertexAttribArray( sconfig.attributes[ i ].loc ) ;
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
                [&]( this_t::shader_data::attribute_variable_cref_t av )
            {
                return av.va == e.va ;
            } ) ;

            if( iter == sconfig.attributes.end() ) 
            {
                natus::log::global_t::warning( natus_log_fn("invalid vertex attribute") ) ;
                continue ;
            }
            glEnableVertexAttribArray( iter->loc ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "glEnableVertexAttribArray" ) ) ;

            glVertexAttribPointer(
                iter->loc,
                GLint( natus::graphics::size_of(e.type_struct) ),
                natus::ogl::complex_to_simple_type( iter->type ),
                GL_FALSE,
                ( GLsizei ) uiStride,
                (const GLvoid*)(size_t)uiBegin 
                ) ;

            natus::ogl::error::check_and_log( natus_log_fn( "glVertexAttribPointer" ) ) ;
        }

        // unbind everything
        {
            glBindVertexArray( 0 ) ;
            glBindBuffer( GL_ARRAY_BUFFER, 0 ) ;
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "unbind" ) ) ;
        }
        return true ;
    }

    //***********************
    void_t post_link_uniforms( this_t::shader_data & config )
    {
        GLuint const program_id = config.pg_id ;

        GLint num_active_uniforms = 0 ;
        GLint name_length = 0 ;

        glGetProgramiv( program_id, GL_ACTIVE_UNIFORMS, &num_active_uniforms ) ;
        natus::ogl::error::check_and_log( "[glGetProgramiv] : GL_ACTIVE_UNIFORMS" ) ;

        if( num_active_uniforms == 0 ) return ;

        config.uniforms.resize( num_active_uniforms ) ;

        glGetProgramiv( program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_length ) ;
        natus::ogl::error::check_and_log( "[glGetProgramiv] : GL_ACTIVE_UNIFORM_MAX_LENGTH" ) ;

        GLint size ;
        GLenum gl_uniform_type ;

        natus::memory::malloc_guard<char> buffer( name_length ) ;

        for( GLint i = 0; i < num_active_uniforms; ++i )
        {
            glGetActiveUniform( program_id, i, name_length, 0, 
                &size, &gl_uniform_type, buffer ) ;
            if( natus::ogl::error::check_and_log( "[gl_33_api] : glGetActiveUniform" ) ) continue ;

            // build-ins are not needed in the shader variables container.
            if( name_length >= 3 && buffer.equals( 0, 'g' ) && buffer.equals( 1, 'l' ) && buffer.equals( 2, '_' ) ) continue ;

            GLuint const location_id = glGetUniformLocation( program_id, buffer ) ;
            if( natus::ogl::error::check_and_log( "[glGetUniformLocation]" ) ) continue ;

            if( natus::log::global_t::error( location_id == GLuint( -1 ), 
                natus_log_fn( "invalid uniform location id." ) ) ) continue ;

            natus::ntd::string const variable_name = natus::ntd::string( char_cptr_t( buffer ) ) ;

            this_t::shader_data::uniform_variable_t vd ;
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
    void_t update_all_uniforms( this_t::shader_data & config )
    {
        GLuint const program_id = config.pg_id ;

        {
            glUseProgram( program_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glUseProgram" ) ) )
                return ;
        }

        for( auto & v : config.uniforms )
        {
            v.do_uniform_funk() ;
        }
    }

    //***********************
    size_t construct_image_config( size_t /*oid*/, natus::ntd::string_cref_t name, 
        natus::graphics::image_object_ref_t config )
    {
        // the name is unique
        {
            auto iter = ::std::find_if( img_configs.begin(), img_configs.end(),
                [&] ( this_t::image_config const& config )
            {
                return config.name == name ;
            } ) ;

            if( iter != img_configs.end() )
                return iter - img_configs.begin() ;
        }

        size_t i = 0 ;
        for( ; i < img_configs.size(); ++i )
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
            glGenTextures( 1, &id ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glGenSamplers" ) ) ;

            img_configs[ i ].tex_id = id ;
        }

        {
            img_configs[ i ].name = name ;
        }

        {
            for( size_t j=0; j<(size_t)natus::graphics::texture_wrap_mode::size; ++j )
            {
                img_configs[ i ].wrap_types[ j ] = natus::graphics::gl3::convert(
                    config.get_wrap( ( natus::graphics::texture_wrap_mode )j ) );
            }

            for( size_t j = 0; j < ( size_t ) natus::graphics::texture_filter_mode::size; ++j )
            {
                img_configs[ i ].filter_types[ j ] = natus::graphics::gl3::convert(
                    config.get_filter( ( natus::graphics::texture_filter_mode )j ) );
            }
        }
        return i ;
    }

    //***********************
    size_t construct_render_data( size_t oid, natus::graphics::render_object_ref_t obj )
    {
        oid = determine_oid( obj.name(), rconfigs ) ;

        {
            rconfigs[ oid ].name = obj.name() ;
            rconfigs[ oid ].var_sets_data.clear() ;
            rconfigs[ oid ].var_sets_texture.clear() ;
        }

        return oid ;
    }

    bool_t update( size_t const id, natus::graphics::shader_object_cref_t sc )
    {
        auto& sconfig = shaders[ id ] ;

        {
            sc.for_each_vertex_input_binding( [&]( 
                natus::graphics::vertex_attribute const va, natus::ntd::string_cref_t name )
            {
                sconfig.vertex_inputs.emplace_back( this_t::shader_data::vertex_input_binding 
                    { va, name } ) ;
            } ) ;
        }

        // compile
        {
            natus::graphics::shader_set_t ss ;
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

    bool_t update( size_t const id, natus::graphics::render_object_ref_t rc )
    {
        auto& config = rconfigs[ id ] ;

        // find geometry
        {
            auto const iter = ::std::find_if( geo_configs.begin(), geo_configs.end(),
                [&] ( this_t::geo_data const& d )
            {
                return d.name == rc.get_geometry() ;
            } ) ;
            
            if( iter == geo_configs.end() )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "no geometry with name [" + rc.get_geometry() + "] for render_data [" + rc.name() + "]" ) ) ;
                return false ;
            }

            config.geo_id = std::distance( geo_configs.begin(), iter ) ;
        }

        // find shader
        {
            auto const iter = ::std::find_if( shaders.begin(), shaders.end(),
                [&] ( this_t::shader_data const& d )
            {
                return d.name == rc.get_shader() ;
            } ) ;
            if( iter == shaders.end() )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "no shader with name [" + rc.get_shader() + "] for render_data [" + rc.name() + "]" ) ) ;
                return false ;
            }

            config.shd_id = std::distance( shaders.begin(), iter ) ;
        }
        
        // for binding attributes, the shader and the geometry is required.
        {
            this_t::shader_data_ref_t shd = shaders[ config.shd_id ] ;
            this_t::geo_data_ref_t geo = geo_configs[ config.geo_id ] ;
            this_t::bind_attributes( shd, geo ) ;
        }

        {
            rc.for_each( [&] ( size_t const /*i*/, natus::graphics::variable_set_res_t vs )
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

    size_t construct_geo( size_t oid, natus::graphics::geometry_object_ref_t obj ) 
    {
        oid = determine_oid( obj.name(), geo_configs ) ;

        bool_t error = false ;
        auto& config = geo_configs[ oid ] ;

        // vertex array object
        if( config.va_id == GLuint( -1 ) )
        {
            GLuint id = GLuint( -1 ) ;
            glGenVertexArrays( 1, &id ) ;
            natus::ogl::error::check_and_log(
                natus_log_fn( "Vertex Array creation" ) ) ;

            config.va_id = id ;
        }

        // vertex buffer
        if( config.vb_id == GLuint(-1) )
        {
            GLuint id = GLuint( -1 ) ;
            glGenBuffers( 1, &id ) ;
            error = natus::ogl::error::check_and_log( 
                natus_log_fn( "Vertex Buffer creation" ) ) ;

            config.vb_id = id ;
        }

        // index buffer
        if( config.ib_id == GLuint(-1) )
        {
            GLuint id = GLuint( -1 ) ;
            glGenBuffers( 1, &id ) ;
            error = natus::ogl::error::check_and_log(
                natus_log_fn( "Index Buffer creation" ) ) ;

            config.ib_id = id ;
        }

        {
            config.name = obj.name() ;
            config.stride = GLuint( obj.vertex_buffer().get_layout_sib() ) ;
            obj.vertex_buffer().for_each_layout_element( 
                [&]( natus::graphics::vertex_buffer_t::data_cref_t d )
            {
                this_t::geo_data::layout_element le ;
                le.va = d.va ;
                le.type = d.type ;
                le.type_struct = d.type_struct ;
                config.elements.push_back( le ) ;
            }) ;
        }

        natus::log::global_t::error( error, natus_log_fn("Error ocurred for ["+ obj.name() +"]") ) ;

        return oid ;
    }

    bool_t update( size_t const id, natus::graphics::geometry_object_res_t geo )
    {
        auto& config = geo_configs[ id ] ;

        {
            //#error "set all these new values"
            config.num_elements_ib = geo->index_buffer().get_num_elements() ;
            config.num_elements_vb = geo->vertex_buffer().get_num_elements() ;
            config.ib_elem_sib = geo->index_buffer().get_element_sib() ;
            config.ib_type = GL_UNSIGNED_INT ;
            config.pt = natus::graphics::gl3::convert( geo->primitive_type() ) ;
        }

        // bind vertex buffer
        {
            glBindBuffer( GL_ARRAY_BUFFER, config.vb_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn("glBindBuffer - vertex buffer") ) )
                return false ;
        }

        // allocate buffer memory
        // what about mapped memory?
        {
            GLuint const sib = GLuint( geo->vertex_buffer().get_sib() ) ;
            if( sib > config.sib_vb )
            {
                glBufferData( GL_ARRAY_BUFFER, sib,
                    geo->vertex_buffer().data(), GL_STATIC_DRAW ) ;
                if( natus::ogl::error::check_and_log( natus_log_fn( "glBufferData - vertex buffer" ) ) )
                    return false ;
                config.sib_vb = sib ;
            }
            else
            {
                glBufferSubData( GL_ARRAY_BUFFER, 0, sib, geo->vertex_buffer().data() ) ;
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
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, config.ib_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBindBuffer - index buffer" ) ) )
                return false ;
        }

        // allocate buffer memory
        // what about mapped memory?
        {
            GLuint const sib = GLuint( geo->index_buffer().get_sib() ) ;
            if( sib > config.sib_ib )
            {
                glBufferData( GL_ELEMENT_ARRAY_BUFFER, sib,
                    geo->index_buffer().data(), GL_STATIC_DRAW ) ;
                if( natus::ogl::error::check_and_log( natus_log_fn( "glBufferData - index buffer" ) ) )
                    return false ;
                config.sib_ib = sib ;
            }
            else
            {
                glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, sib, geo->index_buffer().data() ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glBufferSubData - index buffer" ) ) ;
            }
        }

        return true ;
    }

    bool_t update( size_t const id, natus::graphics::image_object_ref_t confin )
    {
        this_t::image_config& config = img_configs[ id ] ;

        glBindTexture( GL_TEXTURE_2D, config.tex_id ) ;
        if( natus::ogl::error::check_and_log( natus_log_fn( "glBindTexture" ) ) )
            return false ;

        size_t const sib = confin.image().sib() ;
        GLenum const target = GL_TEXTURE_2D ;
        GLint const level = 0 ;
        GLsizei const width = GLsizei( confin.image().get_dims().x() ) ;
        GLsizei const height = GLsizei( confin.image().get_dims().y() ) ;
        GLenum const format = natus::graphics::gl3::convert_to_gl_pixel_format( confin.image().get_image_format() ) ;
        GLenum const type = natus::graphics::gl3::convert_to_gl_pixel_type( confin.image().get_image_element_type() ) ;
        void_cptr_t data = confin.image().get_image_ptr() ;

        // determine how to unpack the data
        // ideally, should be divideable by 4
        {
            GLint unpack = 0 ;

            if( width % 4 == 0 ) unpack = 4 ;
            else if( width % 3 == 0 ) unpack = 3 ;
            else if( width % 3 == 0 ) unpack = 2 ;
            else unpack = 1 ;

            glPixelStorei( GL_UNPACK_ALIGNMENT, unpack ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glPixelStorei" ) ) ;
        }

        if( sib == 0 || config.sib < sib )
        {
            GLint const border = 0 ;
            GLint const internal_format = natus::graphics::gl3::convert_to_gl_format( confin.image().get_image_format(), confin.image().get_image_element_type() ) ;

            glTexImage2D( target, level, internal_format, width, height,
                border, format, type, data ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glTexImage2D" ) ) ;
        }
        else
        {
            GLint const xoffset = 0 ;
            GLint const yoffset = 0 ;

            glTexSubImage2D( target, level, xoffset, yoffset, width, height,
                format, type, data ) ;
            natus::ogl::error::check_and_log( natus_log_fn( "glTexSubImage2D" ) ) ;
        }

        config.sib = confin.image().sib() ;

        return false ;
    }

    bool_t connect( size_t const id, natus::graphics::variable_set_res_t vs )
    {
        auto& config = rconfigs[ id ] ;

        this_t::shader_data_ref_t shd = shaders[ config.shd_id ] ;

        if( this_t::connect( config, vs ) )
            this_t::update_all_uniforms( shd ) ;

        return true ;
    }

    bool_t connect( this_t::render_data & config, natus::graphics::variable_set_res_t vs )
    {
        auto item_data = ::std::make_pair( vs,
            natus::ntd::vector< this_t::render_data::uniform_variable_link >() ) ;

        auto item_tex = ::std::make_pair( vs,
            natus::ntd::vector< this_t::render_data::uniform_texture_link >() ) ;

        this_t::shader_data_ref_t shd = shaders[ config.shd_id ] ;

        size_t id = 0 ;
        for( auto& uv : shd.uniforms )
        {
            // is it a data uniform variable?
            if( natus::ogl::uniform_is_data( uv.type ) )
            {
                auto const types = natus::graphics::gl3::to_type_type_struct( uv.type ) ;
                auto* var = vs->data_variable( uv.name, types.first, types.second ) ;
                if( natus::core::is_nullptr( var ) )
                {
                    natus::log::global_t::error( natus_log_fn( "can not claim variable " + uv.name ) ) ;
                    continue ;
                }
                
                this_t::render_data::uniform_variable_link link ;
                link.uniform_id = id++ ;
                link.var = var ;

                item_data.second.emplace_back( link ) ;
            }
            else if( natus::ogl::uniform_is_texture( uv.type ) )
            {
                //auto const types = natus::graphics::gl3::to_type_type_struct( uv.type ) ;
                auto* var = vs->texture_variable( uv.name ) ;

                if( natus::core::is_nullptr( var ) )
                {
                    natus::log::global_t::error( natus_log_fn( "can not claim variable " + uv.name ) ) ;
                    continue ;
                }

                // looking for image
                {
                    size_t i = 0 ;
                    auto const& tx_name = var->get() ;
                    for( auto& cfg : img_configs )
                    {
                        if( cfg.name == tx_name ) break ;
                        ++i ;
                    }

                    if( i >= img_configs.size() )
                    {
                        natus::log::global_t::error( natus_log_fn( "Could not find image [" +
                            tx_name + "]" ) ) ;
                        continue ;
                    }

                    this_t::render_data::uniform_texture_link link ;
                    link.uniform_id = id++ ;
                    link.tex_id = img_configs[ i ].tex_id ;
                    link.img_id = i ;
                    item_tex.second.emplace_back( link ) ;
                }
            }
        }

        config.var_sets_data.emplace_back( item_data ) ;
        config.var_sets_texture.emplace_back( item_tex ) ;

        return true ;
    }

    bool_t render( size_t const id, size_t const varset_id = size_t(0), GLsizei const start_element = GLsizei(0), 
        GLsizei const num_elements = GLsizei(-1) )
    {
        this_t::render_data & config = rconfigs[ id ] ;
        this_t::shader_data & sconfig = shaders[ config.shd_id ] ;
        this_t::geo_data & gconfig = geo_configs[ config.geo_id ] ;

        {
            glBindVertexArray( gconfig.va_id ) ;
            if( natus::ogl::error::check_and_log( natus_log_fn( "glBindVertexArray" ) ) )
                return false ;
        }

        {
            glUseProgram( sconfig.pg_id ) ;
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
                    glActiveTexture( GLenum(GL_TEXTURE0 + tex_unit) ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glActiveTexture" ) ) ;
                    glBindTexture( GL_TEXTURE_2D, item.tex_id ) ;
                    natus::ogl::error::check_and_log( natus_log_fn( "glBindTexture" ) ) ;

                    {
                        auto const& ic = img_configs[ item.img_id ] ;
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ic.wrap_types[0] ) ;
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ic.wrap_types[1] ) ;
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, ic.wrap_types[2] ) ;
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ic.filter_types[0] ) ;
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ic.filter_types[1] ) ;
                        natus::ogl::error::check_and_log( natus_log_fn( "glTexParameteri" ) ) ;
                    }

                    auto var = natus::graphics::data_variable< int_t >( tex_unit ) ;
                    auto & uv = sconfig.uniforms[ item.uniform_id ] ;
                    
                    uv.do_copy_funk( &var ) ;
                    uv.do_uniform_funk() ;

                    ++tex_unit ;
                }
            }
        }

        // render section
        {
            GLenum const pt = gconfig.pt ;
            GLuint const ib = gconfig.ib_id ;
            //GLuint const vb = config.geo->vb_id ;

            if( ib != GLuint(-1) )
            {
                GLsizei const max_elems = GLsizei( gconfig.num_elements_ib ) ;
                GLsizei const ne = std::min( num_elements>=0?num_elements:max_elems, max_elems ) ;

                GLenum const glt = gconfig.ib_type ;
                
                void_cptr_t offset = void_cptr_t( byte_cptr_t( nullptr ) + 
                    start_element * GLsizei( gconfig.ib_elem_sib ) ) ;
                
                glDrawElements( pt, ne, glt, offset ) ;

                natus::ogl::error::check_and_log( natus_log_fn( "glDrawElements" ) ) ;
            }
            else
            {
                GLsizei const max_elems = GLsizei( gconfig.num_elements_vb ) ;
                GLsizei const ne = std::min( num_elements, max_elems ) ;

                glDrawArrays( pt, start_element, ne ) ;

                natus::ogl::error::check_and_log( natus_log_fn( "glDrawArrays" ) ) ;
            }
        }
        return true ;
    }

    void_t begin_frame( void_t ) 
    {
        auto const ids_old = _state_id_stack[0] ;
        auto const ids_new = _state_id_stack[1] ;

        // set the viewport to the default new state, 
        // so the correct viewport is set automatically.
        {
            natus::math::vec4ui_t vp = _states[ ids_new.first ].states[ ids_new.second ].view_s.vp ;
            vp.z( uint_t( vp_width ) ) ;
            vp.w( uint_t( vp_height ) ) ;
            _states[ ids_new.first ].states[ ids_new.second ].view_s.vp = vp ;
            _states[ ids_new.first ].states[ ids_new.second ].view_s.enable = true ;
            _states[ ids_new.first ].states[ ids_new.second ].view_s.change = true ;
        }

        this_t::handle_render_state( 
            _states[ ids_new.first ].states[ ids_new.second ],
            _states[ ids_old.first ].states[ ids_old.second ] ) ;
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
gl3_backend::gl3_backend( natus::graphics::gl_context_ptr_t ctx ) noexcept : 
    backend( natus::graphics::backend_type::gl3 )
{
    _pimpl = natus::memory::global_t::alloc( pimpl( ctx ), 
        natus_log_fn("gl3_backend::pimpl") ) ;

    _context = ctx ;
}

//****
gl3_backend::gl3_backend( this_rref_t rhv ) noexcept : backend( ::std::move( rhv ) )
{
    natus_move_member_ptr( _pimpl, rhv ) ;
    natus_move_member_ptr( _context, rhv ) ;
}

//****
gl3_backend::~gl3_backend( void_t ) 
{
    natus::memory::global_t::dealloc( _pimpl ) ;
}

//****
void_t gl3_backend::set_window_info( window_info_cref_t wi ) noexcept 
{
    {
        //bool_t change = false ;
        if( wi.width != 0 )
        {
            _pimpl->vp_width = GLsizei( wi.width ) ;
            //change = true ;
        }
        if( wi.height != 0 )
        {
            _pimpl->vp_height = GLsizei( wi.height ) ;
            //change = true ;
        }
    }
}

//****
natus::graphics::result gl3_backend::configure( natus::graphics::geometry_object_res_t gconf ) noexcept 
{
    natus::graphics::id_res_t id = gconf->get_id() ;

    {
        id->set_oid( this_t::get_bid(),
            _pimpl->construct_geo( id->get_oid(this_t::get_bid() ), *gconf ) ) ;
    }

    {
        auto const res = _pimpl->update( id->get_oid( this_t::get_bid() ), gconf ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result gl3_backend::configure( natus::graphics::render_object_res_t config ) noexcept 
{
    natus::graphics::id_res_t id = config->get_id() ;

    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_render_data( 
            id->get_oid( this_t::get_bid() ), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }

    return natus::graphics::result::ok ;
}

//***
natus::graphics::result gl3_backend::configure( natus::graphics::shader_object_res_t config ) noexcept
{
    natus::graphics::id_res_t id = config->get_id() ;

    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_shader_config( 
            id->get_oid( this_t::get_bid() ), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }

    return natus::graphics::result::ok ;
}

//***
natus::graphics::result gl3_backend::configure( natus::graphics::image_object_res_t config ) noexcept 
{
    natus::graphics::id_res_t id = config->get_id() ;

    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_image_config( 
            id->get_oid( this_t::get_bid() ), config->name(), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }

    return natus::graphics::result::ok ;
}

//***
natus::graphics::result gl3_backend::configure( natus::graphics::framebuffer_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;

    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_framebuffer(
            id->get_oid( this_t::get_bid() ), *obj ) ) ;
    }

    return natus::graphics::result::ok ;
}

//***
natus::graphics::result gl3_backend::configure( natus::graphics::state_object_res_t obj ) noexcept
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;

    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_state(
            id->get_oid( this_t::get_bid() ), *obj ) ) ;
    }

    {
        _pimpl->update_state( id->get_oid( this_t::get_bid() ), *obj ) ;
    }

    return natus::graphics::result::ok ;
}

//***
natus::graphics::result gl3_backend::connect( natus::graphics::render_object_res_t config, natus::graphics::variable_set_res_t vs ) noexcept
{
    natus::graphics::id_res_t id = config->get_id() ;

    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid render configuration id" ) ) ;
        return natus::graphics::result::failed ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    auto const res = _pimpl->connect( oid, vs ) ;
    natus::log::global_t::error( natus::graphics::is_not( res ), 
        natus_log_fn( "connect variable set" ) ) ;
   
    return natus::graphics::result::ok ;
}

//****
natus::graphics::result gl3_backend::update( natus::graphics::geometry_object_res_t config ) noexcept 
{
    natus::graphics::id_res_t id = config->get_id() ;

    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid geometry configuration id" ) ) ;
        return natus::graphics::result::failed ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    auto const res = _pimpl->update( oid, config ) ;
    natus::log::global_t::error( natus::graphics::is_not( res ),
        natus_log_fn( "update geometry" ) ) ;

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result gl3_backend::use( natus::graphics::framebuffer_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() )
    {
        _pimpl->deactivate_framebuffer() ;
        return natus::graphics::result::ok ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;

    if( id->is_not_valid( this_t::get_bid() ) )
    {
        _pimpl->deactivate_framebuffer() ;
        return natus::graphics::result::ok ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;
    auto const res = _pimpl->activate_framebuffer( oid ) ;
    if( !res ) return natus::graphics::result::failed ;

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result gl3_backend::use( natus::graphics::state_object_res_t obj, size_t const sid, bool_t const ) noexcept 
{
    if( !obj.is_valid() )
    {
        _pimpl->handle_render_state( size_t( -1 ), size_t( -1 ) ) ;
        return natus::graphics::result::ok ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;

    if( id->is_not_valid( this_t::get_bid() ) )
    {
        _pimpl->handle_render_state( size_t( -1 ), size_t( -1 ) ) ;
        return natus::graphics::result::ok ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;
    _pimpl->handle_render_state( oid, sid ) ;

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result gl3_backend::render( natus::graphics::render_object_res_t config, natus::graphics::backend::render_detail_cref_t detail ) noexcept 
{ 
    natus::graphics::id_res_t id = config->get_id() ;

    //natus::log::global_t::status( natus_log_fn("render") ) ;

    if( id->is_not_valid( this_t::get_bid() )  )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return natus::graphics::result::failed ;
    }

    // @todo
    // change per object render states here.

    _pimpl->render( id->get_oid( this_t::get_bid() ), detail.varset, (GLsizei)detail.start, (GLsizei)detail.num_elems ) ;

    return natus::graphics::result::ok ;
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
