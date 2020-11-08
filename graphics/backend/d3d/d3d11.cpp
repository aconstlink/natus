

#include "d3d11.h"
#include "d3d11_convert.h"

#include "../../buffer/vertex_buffer.hpp"
#include "../../buffer/index_buffer.hpp"
#include "../../texture/image.hpp"

#include <natus/memory/global.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/ntd/vector.hpp>
#include <natus/ntd/string.hpp>
#include <natus/ntd/string/split.hpp>
#include <natus/ntd/stack.hpp>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>

using namespace natus::graphics ;

template< typename T >
class guard
{
private:

    T * _ptr = nullptr ;

public:

    guard( void_t ) noexcept {}
    ~guard( void_t ) noexcept
    {
        if( _ptr != nullptr ) _ptr->Release() ;
    }

    operator T* ( void_t ) noexcept
    {
        return _ptr ;
    }

    operator T** ( void_t ) noexcept
    {
        return &_ptr ;
    }

    operator void** ( void_t ) noexcept
    {
        return (void**)&_ptr ;
    }

    T* operator -> ( void_t ) noexcept
    {
        return _ptr ;
    }

    T* move_out( void_t ) noexcept 
    {
        T* ret = _ptr ;
        _ptr = nullptr ;
        return ret ;
    }
} ;

struct d3d11_backend::pimpl
{
    natus_this_typedefs( d3d11_backend::pimpl ) ;

    struct geo_data
    {
        bool_t valid = false ;

        // empty names indicate free configs
        natus::ntd::string_t name ;

        void_ptr_t vb_mem = nullptr ;
        void_ptr_t ib_mem = nullptr ;

        size_t num_elements_vb = 0 ;
        size_t num_elements_ib = 0 ;

        size_t sib_vb = 0 ;
        size_t sib_ib = 0 ;

        ID3D11Buffer* vb = nullptr ;
        ID3D11Buffer* ib = nullptr ;

        // per vertex sib
        UINT stride = 0 ;

        natus::graphics::primitive_type pt ;

        struct layout_element
        {
            natus::graphics::vertex_attribute va ;
            natus::graphics::type type ;
            natus::graphics::type_struct type_struct ;

            size_t sib( void_t ) const noexcept
            {
                return size_t( natus::graphics::size_of( type ) *
                    natus::graphics::size_of( type_struct ) ) ;
            }
        };
        natus::ntd::vector< layout_element > elements ;

        DXGI_FORMAT get_format_from_element( natus::graphics::vertex_attribute const va ) const noexcept
        {
            for( auto const & e : elements )
            {
                if( e.va == va )
                {
                    return natus::graphics::d3d11::convert_type_to_vec_format( e.type, e.type_struct ) ;
                }
            }

            return DXGI_FORMAT_UNKNOWN ;
        }

        UINT get_sib( natus::graphics::vertex_attribute const va ) const noexcept
        {
            size_t ret = 0 ;
            for( auto const& e : elements )
            {
                if( e.va == va )
                {
                    ret = natus::graphics::size_of( e.type ) * natus::graphics::size_of( e.type_struct ) ;
                }
            }
            return UINT( ret ) ;
        }
    };
    natus_typedef( geo_data ) ;

    struct shader_data
    {
        bool_t valid = false ;
        natus::ntd::string_t name ;

        // vs, gs, ps, else?
        ID3D11VertexShader * vs = nullptr ;
        ID3D11GeometryShader * gs = nullptr ;
        ID3D11PixelShader * ps = nullptr ;

        // required for later vertex layout creation
        ID3DBlob* vs_blob = nullptr;

        struct vertex_input_binding
        {
            natus::graphics::vertex_attribute va ;
            natus::ntd::string_t name ;
        };
        natus::ntd::vector< vertex_input_binding > vertex_inputs ;

        struct data_variable
        {
            natus::ntd::string_t name ;
            natus::graphics::type t ;
            natus::graphics::type_struct ts ;
        };
        natus_typedef( data_variable ) ;

        struct image_variable
        {
            // texture and its sampler must be on the same slot
            natus::ntd::string_t name ;
            UINT slot ;
        } ;
        natus_typedef( image_variable ) ;
        natus_typedefs( natus::ntd::vector< image_variable_t >, image_variables ) ;

        // a constant buffer represent a way to send 
        // variable data to the shader. This struct
        // keeps track of a buffer in the shader and its
        // variables and represents the layout of the buffer.
        struct cbuffer
        {
            // the slot the cbuffer needs to be set to
            // when bound to a shader state
            // done by : register(bn) statement
            UINT slot ;

            natus::ntd::string_t name ;
            natus::ntd::vector< data_variable > datas ;

            size_t sib = 0 ;
        } ;
        natus_typedef( cbuffer ) ;
        natus_typedefs( natus::ntd::vector< cbuffer_t >, cbuffers ) ;
        cbuffers_t vs_cbuffers ;
        cbuffers_t gs_cbuffers ;
        cbuffers_t ps_cbuffers ;

        image_variables_t vs_textures ;
        image_variables_t ps_textures ;
    } ;
    natus_typedef( shader_data ) ;

    struct image_data
    {
        bool_t valid = false ;
        natus::ntd::string_t name ;

        ID3D11ShaderResourceView * view = nullptr ;
        ID3D11Texture2D * texture = nullptr ;
        ID3D11SamplerState * sampler = nullptr ;
    };
    natus_typedef( image_data ) ;

    struct render_state_sets
    {
        struct clear_state_set
        {
            bool_t enable = false ;
            natus::math::vec4f_t color ;
            bool_t do_color = false ;
            bool_t do_depth = false ;
        } ;
        clear_state_set clear_s ;

        struct view_state_set
        {
            bool_t enable = false ;
            natus::math::vec4ui_t vp ;
        } ;
        view_state_set view_s ;

        struct blend_state_set
        {
            bool_t enable = false ;
            //GLenum src_blend_fac ;
            //GLenum dst_blend_fac ;
            //GLenum blend_func ;
        };
        blend_state_set blend_s ;

        struct depth_state_set
        {
            bool_t enable = false ;
            bool_t write = false ;

            ID3D11DepthStencilState* state = nullptr ;
        } ;
        depth_state_set depth_s ;

        struct scissor_state_set
        {
            bool_t enable = false ;
            natus::math::vec4ui_t rect ;
        } ;
        scissor_state_set scissor_s ;

        struct polygon_state_set
        {
            bool_t enable ;
            //GLenum cull_mode ;
            //GLenum front_face ;
            //GLenum fill_mode ;
        };
        polygon_state_set polygon_s ;

        ID3D11RasterizerState * raster_state = nullptr ;
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

        // the layout requires information about the layout elements' type
        // in order to determine the proper d3d enumeration
        D3D11_INPUT_ELEMENT_DESC layout[ size_t( natus::graphics::vertex_attribute::num_attributes ) ] ;
        ID3D11InputLayout* vertex_layout = nullptr ;

        ID3D11RasterizerState* raster_state = nullptr ;
        ID3D11BlendState* blend_state = nullptr ;

        struct data_variable
        {
            natus::ntd::string name ;
            natus::graphics::ivariable_ptr_t ivar = nullptr ;
            natus::graphics::type t ;
            natus::graphics::type_struct ts ;

            size_t sib = 0 ;

            size_t do_copy_funk( void_ptr_t dest )
            {
                if( ivar == 0 ) return 0 ;

                std::memcpy( dest, ivar->data_ptr(), sib ) ;
                return sib ;
            }
        } ;
        natus_typedef( data_variable ) ;
        natus_typedefs( natus::ntd::vector< data_variable_t >, data_variables ) ;

        struct image_variable
        {
            // texture and its sampler must be on the same slot
            natus::ntd::string_t name ;
            UINT slot ;

            // index into the images vector
            size_t id = size_t( -1 ) ;
        } ;
        natus_typedef( image_variable ) ;
        natus_typedefs( natus::ntd::vector< image_variable_t >, image_variables ) ;

        natus::ntd::vector< std::pair< natus::graphics::variable_set_res_t,
            image_variables_t > > var_sets_imgs_vs ;

        natus::ntd::vector< std::pair< natus::graphics::variable_set_res_t,
            image_variables_t > > var_sets_imgs_ps ;

        // represents the cbuffer data of a shader stage.
        // very linked shader in a render data object
        // requires its own constant buffer data.
        struct cbuffer
        {
            UINT slot ;
            void_ptr_t mem = nullptr ;
            ID3D11Buffer * ptr = nullptr ;
            data_variables_t data_variables ;
            // textures
            // ...
        } ;
        natus_typedef( cbuffer ) ;
        natus_typedefs( natus::ntd::vector< cbuffer_t >, cbuffers ) ;

        natus::ntd::vector< std::pair< natus::graphics::variable_set_res_t,
            cbuffers_t > > var_sets_data ;

        natus::ntd::vector< std::pair< natus::graphics::variable_set_res_t,
            cbuffers_t > > var_sets_data_ps ;
    };
    natus_typedef( render_data ) ;

    struct framebuffer_data
    {
        bool_t valid = false ;
        natus::ntd::string_t name ;
        size_t num_color = 0 ;

        FLOAT width = 0.0f ;
        FLOAT height = 0.0f ;

        // 8 color targets + 1 depth stencil
        size_t image_ids[ 9 ] = {
            size_t( -1 ), size_t( -1 ), size_t( -1 ), size_t( -1 ),
            size_t( -1 ), size_t( -1 ), size_t( -1 ), size_t( -1 ), 
            size_t( -1 )} ;

        ID3D11RenderTargetView * rt_view[ 8 ] = 
            { nullptr, nullptr, nullptr, nullptr, 
                nullptr, nullptr, nullptr, nullptr } ;

        ID3D11DepthStencilView * ds_view = nullptr ;

        #if 0

        GLuint gl_id = GLuint( -1 ) ;

        size_t nt = 0 ;
        GLuint colors[ 8 ] = {
            GLuint( -1 ), GLuint( -1 ), GLuint( -1 ), GLuint( -1 ),
            GLuint( -1 ), GLuint( -1 ), GLuint( -1 ), GLuint( -1 ) } ;

        GLuint depth = GLuint( -1 ) ;

        void_ptr_t mem_ptr = nullptr ;

        natus::math::vec2ui_t dims ;
        #endif
    };
    natus_typedef( framebuffer_data ) ;

public: // variables

    natus::graphics::backend_type const bt = natus::graphics::backend_type::d3d11 ;
    natus::graphics::d3d11_context_ptr_t _ctx ;

    typedef natus::ntd::vector< this_t::geo_data > geo_configs_t ;
    geo_configs_t geo_datas ;

    typedef natus::ntd::vector< this_t::shader_data > shader_datas_t ;
    shader_datas_t shaders ;

    typedef natus::ntd::vector< this_t::render_data > render_datas_t ;
    render_datas_t renders ;

    typedef natus::ntd::vector< this_t::image_data > image_datas_t ;
    image_datas_t images ;

    typedef natus::ntd::vector< this_t::framebuffer_data > framebuffer_datas_t ;
    framebuffer_datas_t _framebuffers ;

    natus::graphics::render_state_sets_t render_states ;

    typedef natus::ntd::vector< this_t::state_data_t > states_t ;
    states_t _states ;
    natus::ntd::stack< std::pair<size_t, size_t>, 10 > _state_id_stack ;

    FLOAT vp_width = FLOAT( 0 ) ;
    FLOAT vp_height = FLOAT( 0 ) ;

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

public: // functions

    pimpl( natus::graphics::d3d11_context_ptr_t ctx ) noexcept
    {
        _ctx = ctx ;
        geo_datas.resize( 10 ) ;

        {
            auto new_states = *natus::graphics::backend_t::default_render_states() ;

            natus::graphics::render_state_sets old_states = new_states ;
            old_states.blend_s.do_blend = !new_states.blend_s.do_blend ;
            old_states.clear_s.do_clear = !new_states.clear_s.do_clear ;
            old_states.depth_s.do_depth_test = !new_states.depth_s.do_depth_test ;
            old_states.polygon_s.do_culling = !new_states.polygon_s.do_culling ;
            old_states.scissor_s.do_scissor_test = !new_states.scissor_s.do_scissor_test ;
            old_states.view_s.do_viewport = !new_states.view_s.do_viewport ;

            natus::graphics::state_object_t obj( "d3d11_default_states" ) ;

            // store the invalid state at the bottom
            // store the opposed default state at position 1
            // so during frame begin, all default states can be
            // restored using those two state sets.
            obj.add_render_state_set( old_states ) ;
            obj.add_render_state_set( new_states ) ;

            size_t const oid = this_t::construct_state( size_t( -1 ), obj ) ;
            _state_id_stack.push( std::make_pair( oid, size_t( 0 ) ) ) ;
            _state_id_stack.push( std::make_pair( oid, size_t( 1 ) ) ) ;

            update_state( oid, obj ) ;
        }
    }

    pimpl( pimpl && rhv ) noexcept
    {
        natus_move_member_ptr( _ctx, rhv ) ;
        geo_datas = std::move( rhv.geo_datas ) ;
        _state_id_stack = std::move( rhv._state_id_stack ) ;
        _states = std::move( rhv._states ) ;
    }
    
    ~pimpl( void_t ) 
    {
        for( auto & g : geo_datas )
        {
            natus::memory::global_t::dealloc_raw( g.vb_mem ) ;
            natus::memory::global_t::dealloc_raw( g.ib_mem ) ;
        }
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

            rs_.view_s.enable = rs.view_s.do_viewport ;
            rs_.blend_s.enable = rs.blend_s.do_blend ;
            rs_.clear_s.enable = rs.clear_s.do_clear ;
            rs_.depth_s.enable = rs.depth_s.do_depth_test ;
            rs_.depth_s.write = rs.depth_s.do_depth_write ;
            rs_.polygon_s.enable = rs.polygon_s.do_culling ;
            rs_.scissor_s.enable = rs.scissor_s.do_scissor_test ;

            D3D11_RASTERIZER_DESC raster_desc = { } ;

            if( rs_.blend_s.enable )
            {
                # if 0
                rs.blend_s.blend_func = natus::graphics::gl3::convert( rs_in.blend_s.blend_func ) ;
                rs.blend_s.src_blend_fac = natus::graphics::gl3::convert( rs_in.blend_s.src_blend_factor ) ;
                rs.blend_s.dst_blend_fac = natus::graphics::gl3::convert( rs_in.blend_s.dst_blend_factor ) ;
                #endif
        }
            if( rs_.clear_s.enable )
            {
                rs_.clear_s.color = rs.clear_s.clear_color ;
                rs_.clear_s.do_color = rs.clear_s.do_color ;
                rs_.clear_s.do_depth = rs.clear_s.do_depth ;
            }
            if( rs_.view_s.enable )
            {
                rs_.view_s.vp = rs.view_s.vp ;
            }
            
            // depth state
            {
                rs_.depth_s.write = rs.depth_s.do_depth_write ;

                {
                    D3D11_DEPTH_STENCIL_DESC desc = { } ;
                    desc.DepthEnable = rs_.depth_s.enable  ? TRUE : FALSE ;
                    desc.DepthFunc = D3D11_COMPARISON_LESS ;
                    desc.DepthWriteMask = rs_.depth_s.write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO ;
                    auto const res = _ctx->dev()->CreateDepthStencilState( &desc, &rs_.depth_s.state ) ;
                    if( FAILED( res ) )
                    {
                        natus::log::global_t::error( "CreateDepthStencilState" ) ;
                    }
                }
            }

            if( rs_.polygon_s.enable )
            {
                if( rs.polygon_s.do_culling )
                    raster_desc.CullMode = natus::graphics::d3d11::convert( rs.polygon_s.cm ) ;
                else
                    raster_desc.CullMode = D3D11_CULL_NONE ;
                
                raster_desc.FillMode = natus::graphics::d3d11::convert( rs.polygon_s.fm ) ;
                
                raster_desc.FrontCounterClockwise = rs.polygon_s.ff != natus::graphics::front_face::counter_clock_wise ;
                {

                }
                #if 0
                // glCullFace
                rs.polygon_s.cull_mode = natus::graphics::gl3::convert( rs_in.polygon_s.cm ) ;

                // glFrontFace
                rs.polygon_s.front_face = natus::graphics::gl3::convert( rs_in.polygon_s.ff ) ;

                // glPolygonMode
                rs.polygon_s.fill_mode = natus::graphics::gl3::convert( rs_in.polygon_s.fm ) ;
                #endif
            }
            if( rs_.scissor_s.enable )
            {
            }

            _ctx->dev()->CreateRasterizerState( &raster_desc, &rs_.raster_state ) ;

            states.states[ i ] = rs_ ;
        } ) ;

        return oid ;
    }

    // some values can be updated without newly constructing the d3d11 state
    size_t update_state( size_t const oid, natus::graphics::state_object_ref_t obj ) noexcept
    {
        auto& states = _states[ oid ] ;

        // @note
        // set all values
        obj.for_each( [&] ( size_t const i, natus::graphics::render_state_sets_cref_t rs_in )
        {
            this_t::render_state_sets& rs = states.states[ i ] ;

            if( rs.blend_s.enable )
            {
            }
            if( rs.clear_s.enable )
            {
            }
            if( rs.view_s.enable )
            {
                rs.view_s.vp = rs_in.view_s.vp ;
            }
            if( rs.depth_s.enable )
            {
            }
            if( rs.polygon_s.enable )
            {
            }
            if( rs.scissor_s.enable )
            {
            }
        } ) ;
        return oid ;
    }

    void_t handle_render_state( render_state_sets const& new_states, render_state_sets const& old_states )
    {
        
        if( new_states.depth_s.enable != old_states.depth_s.enable )
        {
            if( new_states.depth_s.enable )
            {
                _ctx->ctx()->OMSetDepthStencilState( new_states.depth_s.state, 0 ) ;
            }
            else
            {
                _ctx->ctx()->OMSetDepthStencilState( NULL,0 ) ;
            }
        }

        #if 0

        if( new_states.blend_s.enable != old_states.blend_s.enable )
        {
            if( new_states.blend_s.enable )
            {
                glEnable( GL_BLEND ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;

                GLenum const glsrc = new_states.blend_s.src_blend_fac ;
                GLenum const gldst = new_states.blend_s.dst_blend_fac ;
                glBlendFunc( glsrc, gldst ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glBlendFunc" ) ) ;
            }
            else
            {
                glDisable( GL_BLEND ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
            }
        }

        if( new_states.polygon_s.enable != old_states.polygon_s.enable )
        {
            if( new_states.polygon_s.enable )
            {
                glEnable( GL_CULL_FACE ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;

                glCullFace( new_states.polygon_s.cull_mode ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glCullFace" ) ) ;

                glFrontFace( new_states.polygon_s.front_face ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glFrontFace" ) ) ;

                glPolygonMode( GL_FRONT_AND_BACK, new_states.polygon_s.fill_mode ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glPolygonMode" ) ) ;
            }
            else
            {
                glDisable( GL_CULL_FACE ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
            }
        }

        if( new_states.scissor_s.enable != old_states.scissor_s.enable )
        {
            if( new_states.scissor_s.enable )
            {
                glEnable( GL_SCISSOR_TEST ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;

                glScissor(
                    GLint( new_states.scissor_s.rect.x() ), GLint( new_states.scissor_s.rect.y() ),
                    GLsizei( new_states.scissor_s.rect.z() ), GLsizei( new_states.scissor_s.rect.w() ) ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glScissor" ) ) ;
            }
            else
            {
                glDisable( GL_SCISSOR_TEST ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
            }
        }
        #endif

        if( new_states.clear_s.enable != old_states.clear_s.enable )
        {
            if( new_states.clear_s.enable )
            {
                // @todo what about user created framebuffers/render targets?
                if( new_states.clear_s.do_color )
                    _ctx->clear_render_target_view( new_states.clear_s.color ) ;
                if( new_states.clear_s.do_depth )
                    _ctx->clear_depth_stencil_view() ;
            }
        }

        if( new_states.view_s.enable != old_states.view_s.enable )
        {
            if( new_states.view_s.enable )
            {
                auto const& vp_ = new_states.view_s.vp  ;

                // Setup the viewport
                D3D11_VIEWPORT vp ;
                vp.Width = FLOAT( vp_.z() ) ;
                vp.Height = FLOAT( vp_.w() ) ;
                vp.MinDepth = 0.0f ;
                vp.MaxDepth = 1.0f ;
                vp.TopLeftX = FLOAT( vp_.x() ) ;
                vp.TopLeftY = FLOAT( vp_.y() ) ;
                _ctx->ctx()->RSSetViewports( 1, &vp );
            }
        }

        _ctx->ctx()->RSSetState( new_states.raster_state ) ;
        
    }

    // if oid == -1, the state is popped.
    void_t handle_render_state( size_t const oid, size_t const rs_id ) noexcept
    {
        auto new_id = std::make_pair( oid, rs_id ) ;
        auto old_id = _state_id_stack.top() ;

        // pop state
        if( oid == size_t( -1 ) )
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
            auto const& new_states = _states[ new_id.first ].states[ new_id.second ] ;
            auto const& old_states = _states[ old_id.first ].states[ old_id.second ] ;

            this_t::handle_render_state( new_states, old_states ) ;
        }

        if( oid != size_t( -1 ) )
        {
            _state_id_stack.push( new_id ) ;
        }
    }

    size_t construct_framebuffer( size_t oid, natus::graphics::framebuffer_object_ref_t obj ) noexcept
    {
        oid = determine_oid( obj.name(), _framebuffers ) ;
        auto & fb = _framebuffers[ oid ] ;

        ID3D11Device * dev = _ctx->dev() ;

        // colors
        {
            for( size_t i=0; i<fb.num_color; ++i )
            {
                auto*& view = fb.rt_view[ i ] ;
                if( view != nullptr )
                {
                    view->Release() ;
                    view = nullptr ;
                }
            }
            // release images
            for( size_t i = 0; i < fb.num_color; ++i )
            {
                if( fb.image_ids[ i ] != size_t( -1 ) )
                {
                    images[ fb.image_ids[ i ] ].view->Release() ;
                    images[ fb.image_ids[ i ] ].texture->Release() ;
                    images[ fb.image_ids[ i ] ].sampler->Release() ;

                    images[ fb.image_ids[ i ] ].view = nullptr  ;
                    images[ fb.image_ids[ i ] ].texture = nullptr ;
                    images[ fb.image_ids[ i ] ].sampler = nullptr ;
                }
            }

            size_t const nt = obj.get_num_color_targets() ;
            auto const ctt = obj.get_color_target() ;
            auto const dst = obj.get_depth_target();

            for( size_t i = 0; i < nt; ++i )
            {
                // sampler
                guard< ID3D11SamplerState > smp ;
                {
                    D3D11_SAMPLER_DESC sampDesc = { } ;
                    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR ;
                    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP ;
                    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP ;
                    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP ;
                    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER ;
                    sampDesc.MinLOD = 0;
                    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

                    auto const hr = dev->CreateSamplerState( &sampDesc, smp ) ;
                    if( FAILED( hr ) )
                    {
                        natus::log::global_t::error( natus_log_fn( "CreateSamplerState" ) ) ;
                        continue ;
                    }
                }

                // texture
                guard< ID3D11Texture2D > tex ;
                {
                    auto const dims = obj.get_dims() ;
                    size_t const width = UINT( dims.x() ) ;
                    size_t const height = UINT( dims.y() ) ;
                    
                    D3D11_TEXTURE2D_DESC desc = { } ;
                    desc.Width = static_cast< UINT >( width ) ;
                    desc.Height = static_cast< UINT >( height ) ;
                    desc.MipLevels = static_cast< UINT >( 1 ) ;
                    desc.ArraySize = static_cast< UINT >( 1 ) ;
                    desc.Format = natus::graphics::d3d11::convert( ctt ) ;
                    desc.SampleDesc.Count = 1 ;
                    desc.SampleDesc.Quality = 0 ;
                    desc.Usage = D3D11_USAGE_DEFAULT ;
                    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE ;
                    desc.CPUAccessFlags = 0 ;
                    desc.MiscFlags = 0 ;

                    // create the texture object
                    {
                        auto const hr = dev->CreateTexture2D( &desc, nullptr, tex ) ;
                        if( FAILED( hr ) )
                        {
                            natus::log::global_t::error( natus_log_fn( "CreateTexture2D" ) ) ;
                            continue ;
                        }
                    }
                }

                // shader resource view
                guard< ID3D11ShaderResourceView > srv ;
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC res_desc = { } ;
                    res_desc.Format = natus::graphics::d3d11::convert( ctt ) ;
                    res_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D ;
                    res_desc.Texture2D.MostDetailedMip = 0 ;
                    res_desc.Texture2D.MipLevels = UINT( 1 ) ;

                    auto const hr = dev->CreateShaderResourceView( tex, &res_desc, srv ) ;
                    if( FAILED( hr ) )
                    {
                        natus::log::global_t::error( natus_log_fn( "CreateShaderResourceView for texture : [" + 
                            obj.name() + "]" ) ) ;
                        continue ;
                    }
                }

                // render target view
                guard< ID3D11RenderTargetView > view ;
                {
                    D3D11_RENDER_TARGET_VIEW_DESC desc = { } ;
                    desc.Format = natus::graphics::d3d11::convert( ctt ) ;
                    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D ;
                    desc.Texture2D.MipSlice = 0 ;

                    auto const res = _ctx->dev()->CreateRenderTargetView( tex, &desc, view ) ;
                    if( FAILED( res ) )
                    {
                        natus::log::global_t::error( natus_log_fn( "CreateDepthStencilView" ) ) ;
                        continue ;
                    }
                }

                // store data
                {
                    size_t const iid = fb.image_ids[ i ] == size_t( -1 ) ?
                        determine_oid( obj.name() + "." + std::to_string( i ), images ) : fb.image_ids[ i ] ;

                    // fill the image so shader variable
                    // lookup can find the render target
                    // for binding to a texture variable
                    {
                        fb.image_ids[ i ] = iid ;
                        images[ iid ].name = obj.name() + "." + std::to_string( i ) ;
                        images[ iid ].sampler = smp.move_out() ;
                        images[ iid ].texture = tex.move_out() ;
                        images[ iid ].valid = true ;
                        images[ iid ].view = srv.move_out() ;
                    }

                    {
                        auto const dims = obj.get_dims() ;
                        fb.width = FLOAT( dims.x() ) ;
                        fb.height = FLOAT( dims.y() ) ;
                    }

                    fb.num_color = nt ;
                    fb.rt_view[ i ] = view.move_out() ;

                    
                }
            }
        }

        // depth stencil
        #if 0
        {
            // texture2d

            if( fb.ds_view != nullptr ) 
            {
                fb.ds_view->Release() ;
                fb.ds_view = nullptr ;
            }

            ID3D11Texture2D * buffer ;
            D3D11_DEPTH_STENCIL_VIEW_DESC desc = { } ;

            auto const res =_ctx->dev()->CreateDepthStencilView( buffer, &desc, &fb.ds_view ) ;
            if( FAILED( res ) )
            {
                natus::log::global_t::error( natus_log_fn( "CreateDepthStencilView" ) ) ;
            }
        }
        #endif

        return oid ;
    }

    bool_t activate_framebuffer( size_t const oid )
    {
        framebuffer_data_ref_t fb = _framebuffers[ oid ] ;

        _ctx->ctx()->OMSetRenderTargets( UINT( fb.num_color ), fb.rt_view, nullptr ) ;

        // Setup the viewport
        D3D11_VIEWPORT vp ;
        vp.Width = fb.width ;
        vp.Height = fb.height ;
        vp.MinDepth = 0.0f ;
        vp.MaxDepth = 1.0f ;
        vp.TopLeftX = FLOAT( 0 ) ;
        vp.TopLeftY = FLOAT( 0 ) ;
        _ctx->ctx()->RSSetViewports( 1, &vp );

        #if 0
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
        #endif
        return true ;
    }

    void_t deactivate_framebuffer( void_t )
    {
        _ctx->activate_framebuffer() ;
        
        // Setup the viewport
        D3D11_VIEWPORT vp ;
        vp.Width = vp_width ;
        vp.Height = vp_height ;
        vp.MinDepth = 0.0f ;
        vp.MaxDepth = 1.0f ;
        vp.TopLeftX = FLOAT( 0 ) ;
        vp.TopLeftY = FLOAT( 0 ) ;
        _ctx->ctx()->RSSetViewports( 1, &vp );

    }

    size_t construct_geo( size_t oid, natus::graphics::geometry_object_ref_t obj )
    {
        oid = this_t::determine_oid( obj.name(), renders ) ;

        auto & config = geo_datas[ oid ] ;
        config.name = obj.name() ;

        config.pt = obj.primitive_type() ;

        // vertex buffer object
        {
            obj.vertex_buffer().for_each_layout_element(
                [&] ( natus::graphics::vertex_buffer_t::data_cref_t d )
            {
                this_t::geo_data::layout_element le ;
                le.va = d.va ;
                le.type = d.type ;
                le.type_struct = d.type_struct ;
                config.elements.push_back( le ) ;
            } ) ;

            config.stride = UINT( obj.vertex_buffer().get_layout_sib() ) ;

            // = number of vertices * sizeof( vertex )
            // if there are vertices already setup, take the numbers
            size_t const byte_width = obj.vertex_buffer().get_sib() ;

            D3D11_BUFFER_DESC bd = { } ;
            bd.Usage = D3D11_USAGE_DYNAMIC ;
            // using max of 1 sib so empty vbs will create and will 
            // be resized in the update function. Important for now
            // is that the buffer will be NOT nullptr.
            bd.ByteWidth = UINT( std::max( byte_width, size_t( 1 ) ) ) ;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER ;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE ;

            natus::memory::global_t::dealloc_raw( config.vb_mem ) ;
            config.vb_mem = natus::memory::global_t::alloc_raw< uint8_t >( 
                byte_width, "[d3d11] : vertex buffer memory" ) ;

            if( config.vb != nullptr ) 
            {
                config.vb->Release() ;
                config.vb = nullptr ;
            }

            if( byte_width != 0 )
            {
                D3D11_SUBRESOURCE_DATA init_data = { } ;
                init_data.pSysMem = ( void_cptr_t ) config.vb_mem ;

                std::memcpy( config.vb_mem, obj.vertex_buffer().data(), byte_width ) ;

                HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, &config.vb ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_BIND_VERTEX_BUFFER" ) ) ;
                }
            }
            else
            {
                HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, nullptr, &config.vb ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_BIND_VERTEX_BUFFER" ) ) ;
                }
            }
        }

        // index buffer object
        {
            // = number of vertices * sizeof( index_t )
            size_t const byte_width = obj.index_buffer().get_sib() ;

            D3D11_BUFFER_DESC bd = { } ;
            bd.Usage = D3D11_USAGE_DYNAMIC ;
            // using max of 1 sib so empty ibs will create and will 
            // be resized in the update function. Important for now
            // is that the buffer will be NOT nullptr.
            bd.ByteWidth = uint_t( std::max( byte_width, size_t( 1 ) ) ) ;
            bd.BindFlags = D3D11_BIND_INDEX_BUFFER ;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE ;

            natus::memory::global_t::dealloc_raw( config.ib_mem ) ;
            config.ib_mem = natus::memory::global_t::alloc_raw< uint8_t >(
                byte_width, "[d3d11] : index buffer memory" ) ;

            if( config.ib != nullptr )
            {
                config.ib->Release() ;
                config.ib = nullptr ;
            }

            if( byte_width != 0 )
            {
                D3D11_SUBRESOURCE_DATA init_data = { } ;
                init_data.pSysMem = ( void_cptr_t ) config.ib_mem ;

                std::memcpy( config.ib_mem, obj.index_buffer().data(), byte_width ) ;

                HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, &config.ib ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_BIND_INDEX_BUFFER" ) ) ;
                }
            }
            else
            {
                HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, nullptr, &config.ib ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_BIND_INDEX_BUFFER" ) ) ;
                }
            }
        }

        return oid ;
    }

    bool_t update( size_t const id, natus::graphics::geometry_object_res_t geo )
    {
        auto& config = geo_datas[ id ] ;

        // vb: check memory space
        {
            D3D11_BUFFER_DESC bd ;
            config.vb->GetDesc( &bd ) ;

            size_t const sib = geo->vertex_buffer().get_sib() ;
            if( sib > bd.ByteWidth )
            {
                bd.ByteWidth = uint_t( sib ) ;

                natus::memory::global_t::dealloc_raw( config.vb_mem ) ;
                config.vb_mem = natus::memory::global_t::alloc_raw< uint8_t >( sib, "[d3d11] : vertex buffer memory" ) ;

                D3D11_SUBRESOURCE_DATA init_data = { } ;
                init_data.pSysMem = ( void_ptr_t ) config.vb_mem ;
                
                config.vb->Release() ;
                {
                    auto const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, &config.vb ) ;
                    if( FAILED( hr ) )
                    {
                        natus::log::global_t::error( natus_log_fn( "CreateBuffer( vertex_buffer )" ) ) ;
                    }
                }
            }

            config.num_elements_vb = geo->vertex_buffer().get_num_elements() ;

            // copy data
            {
                // exactly the size in bytes of the 1d buffer
                LONG const lsib = LONG( geo->vertex_buffer().get_layout_sib() ) ;
                LONG const ne = LONG( geo->vertex_buffer().get_num_elements() ) ;
                void_cptr_t data = geo->vertex_buffer().data() ;
                CD3D11_BOX const box( 0, 0, 0, ne, 1, 1 ) ;

                //_ctx->ctx()->UpdateSubresource( config.vb, 0, nullptr /*&box*/, data, lsib, 0 ) ;
                D3D11_MAPPED_SUBRESOURCE resource;
                _ctx->ctx()->Map( config.vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource ) ;
                std::memcpy( resource.pData, data, lsib * ne ) ;
                _ctx->ctx()->Unmap( config.vb, 0 ) ;
            }
        }

        // ib: check memory space
        {
            D3D11_BUFFER_DESC bd ;
            config.ib->GetDesc( &bd ) ;

            size_t const sib = geo->index_buffer().get_sib() ;
            if( sib > bd.ByteWidth )
            {
                bd.ByteWidth = uint_t( sib ) ;

                natus::memory::global_t::dealloc_raw( config.ib_mem ) ;
                config.ib_mem = natus::memory::global_t::alloc_raw< uint8_t >(
                    sib, "[d3d11] : index buffer memory" ) ;

                D3D11_SUBRESOURCE_DATA init_data = { } ;
                init_data.pSysMem = ( void_ptr_t ) config.ib_mem ;

                config.ib->Release() ;
                {
                    auto const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, &config.ib ) ;
                    if( FAILED( hr ) )
                    {
                        natus::log::global_t::error( natus_log_fn( "CreateBuffer( index_buffer )" ) ) ;
                    }
                }
            }

            config.num_elements_ib = geo->index_buffer().get_num_elements() ;

            // copy data
            {
                // exactly the size in bytes of the 1d buffer
                LONG const lsib = LONG( sizeof( uint32_t ) ) ;
                LONG const ne = LONG( geo->index_buffer().get_num_elements() ) ;
                void_cptr_t data = geo->index_buffer().data() ;
                CD3D11_BOX const box( 0, 0, 0, ne, 1, 1 ) ;

                //_ctx->ctx()->UpdateSubresource( config.ib, 0, &box, data, lsib, ne ) ;
                D3D11_MAPPED_SUBRESOURCE resource;
                _ctx->ctx()->Map( config.ib, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource ) ;
                std::memcpy( resource.pData, data, lsib * ne ) ;
                _ctx->ctx()->Unmap( config.ib, 0 ) ;
            }
        }

        return true ;
    }

    size_t construct_shader_config( size_t oid, natus::graphics::shader_object_ref_t obj )
    {
        oid = this_t::determine_oid( obj.name(), shaders ) ;

        //
        // Do Configuration
        //
        auto & shd = shaders[ oid ] ;
        shd.name = obj.name() ;

        // shader code
        natus::graphics::shader_set_t ss ;
        {
            auto const res = obj.shader_set( this->bt, ss ) ;
            if( natus::core::is_not(res) )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "config [" + obj.name() + "] has no shaders for " + 
                    natus::graphics::to_string( this->bt ) ) ) ;
                return oid ;
            }
        }

        ID3DBlob* pVSBlob = nullptr;
        ID3DBlob* pGSBlob = nullptr;
        ID3DBlob* pPSBlob = nullptr;

        // compile
        {
            ID3DBlob* errblob = nullptr;

            // vertex shader
            {
                D3D_SHADER_MACRO Shader_Macros[] = { NULL, NULL };

                auto code = this->remove_unwanded_characters( ss.vertex_shader().code() ) ;
                void_cptr_t cptr = code.c_str() ;
                size_t const sib = ss.vertex_shader().code().size() ;

                // "vs_5_0" : Direct3D 11 and 11.1
                // "vs_4_1" : Direct3D 10.1
                // "vs_4_0" : Direct3D 10
                {
                    auto const hr = D3DCompile( cptr, sib, obj.name().c_str(), Shader_Macros, nullptr, "VS", "vs_5_0", 0, 0, &pVSBlob, &errblob );

                    if( FAILED( hr ) )
                    {
                        natus::log::global_t::warning( natus_log_fn(
                            "vertex shader [" + obj.name() + "] failed " +
                            natus::graphics::to_string( this->bt ) ) ) ;

                        if( errblob != nullptr )
                        {
                            char_cptr_t ptr = ( char_cptr_t ) errblob->GetBufferPointer() ;
                            size_t const ssib = ( size_t ) errblob->GetBufferSize() ;
                            natus::ntd::string_t s( ptr, ssib ) ;

                            auto const pos = s.find( obj.name() ) ;
                            s = s.substr( pos, s.size() - pos ) ;
                            natus::log::global_t::error( s ) ;
                            errblob->Release() ;
                        }

                        return oid ;
                    }
                }

                // find shader variables in constant buffer
                {
                    shd.vs_cbuffers = this_t::determine_cbuffer( pVSBlob ) ;
                    shd.vs_textures = this_t::determine_texture( pVSBlob ) ;
                }
            }

            // geometry shader
            if( ss.has_geometry_shader() )
            {
                D3D_SHADER_MACRO Shader_Macros[] = { NULL, NULL };

                auto code = this->remove_unwanded_characters( ss.geometry_shader().code() ) ;
                void_cptr_t cptr = code.c_str() ;
                size_t const sib = ss.geometry_shader().code().size() ;

                // "gs_5_0" : Direct3D 11 and 11.1
                // "gs_4_1" : Direct3D 10.1
                // "gs_4_0" : Direct3D 10
                auto const hr = D3DCompile( cptr, sib, obj.name().c_str(), Shader_Macros, nullptr, "GS", "gs_5_0", 0, 0, &pGSBlob, &errblob );

                if( FAILED( hr ) )
                {
                    natus::log::global_t::warning( natus_log_fn(
                        "geometry shader [" + obj.name() + "] failed " +
                        natus::graphics::to_string( this->bt ) ) ) ;

                    if( errblob != nullptr )
                    {
                        char_cptr_t ptr = ( char_cptr_t ) errblob->GetBufferPointer() ;
                        size_t const ssib = ( size_t ) errblob->GetBufferSize() ;
                        natus::ntd::string_t s( ptr, ssib ) ;

                        auto const pos = s.find( obj.name() ) ;
                        s = s.substr( pos, s.size() - pos ) ;
                        natus::log::global_t::error( s ) ;
                        errblob->Release() ;
                    }
                    return oid ;
                }

                // find shader variables in constant buffer
                {
                    shd.gs_cbuffers = this_t::determine_cbuffer( pGSBlob ) ;
                }
            }
            else if( shd.gs != nullptr )
            {
                shd.gs->Release() ;
                shd.gs = nullptr ;
            }

            // pixel shader
            if( ss.has_pixel_shader() )
            {
                D3D_SHADER_MACRO Shader_Macros[] = { NULL, NULL };

                auto code = this->remove_unwanded_characters( ss.pixel_shader().code() ) ;
                void_cptr_t cptr = code.c_str() ;
                size_t const sib = ss.pixel_shader().code().size() ;

                // "ps_5_0" : Direct3D 11 and 11.1
                // "ps_4_1" : Direct3D 10.1
                // "ps_4_0" : Direct3D 10
                auto const hr = D3DCompile( cptr, sib, obj.name().c_str(), Shader_Macros, nullptr, "PS", "ps_5_0", 0, 0, &pPSBlob, &errblob );

                if( FAILED( hr ) )
                {
                    natus::log::global_t::warning( natus_log_fn(
                        "pixel shader [" + obj.name() + "] failed " +
                        natus::graphics::to_string( this->bt ) ) ) ;

                    if( errblob != nullptr )
                    {
                        char_cptr_t ptr = ( char_cptr_t ) errblob->GetBufferPointer() ;
                        size_t const ssib = ( size_t ) errblob->GetBufferSize() ;
                        natus::ntd::string_t s( ptr, ssib ) ;

                        auto const pos = s.find( obj.name() ) ;
                        s = s.substr( pos, s.size() - pos ) ;
                        natus::log::global_t::error( s ) ;
                        errblob->Release() ;
                    }

                    return oid ;
                }

                // find shader variables in constant buffer
                {
                    shd.ps_cbuffers = this_t::determine_cbuffer( pPSBlob ) ;
                    shd.ps_textures = this_t::determine_texture( pPSBlob ) ;
                }
            }
            else if( shd.ps != nullptr )
            {
                shd.ps->Release() ;
                shd.ps = nullptr ;
            }
        }

        // new shaders compiled, so release the old
        {
            if( shd.vs != nullptr )
            {
                shd.vs->Release() ;
                shd.vs = nullptr ;
            }

            if( shd.gs != nullptr )
            {
                shd.gs->Release() ;
                shd.gs = nullptr ;
            }

            if( shd.ps != nullptr )
            {
                shd.ps->Release() ;
                shd.ps = nullptr ;
            }
        }

        // vertex shader
        if( pVSBlob != nullptr )
        {
            auto const hr = _ctx->dev()->CreateVertexShader( 
                pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &shd.vs ) ;
            
            natus::log::global_t::error( FAILED( hr ), "[d3d11] : CreateVertexShader" ) ;

            // the vs blob is required for later layout creation
            if( shd.vs_blob != nullptr ) shd.vs_blob->Release() ;
            shd.vs_blob = pVSBlob ;
        }

        // geometry shader
        if( pGSBlob != nullptr )
        {
            auto const hr = _ctx->dev()->CreateGeometryShader(
                pGSBlob->GetBufferPointer(), pGSBlob->GetBufferSize(), nullptr, &shd.gs ) ;
                
            natus::log::global_t::error( FAILED( hr ), "[d3d11] : CreateGeometryShader" ) ;
            pGSBlob->Release();
            pGSBlob = nullptr ;
        }

        // pixel shader
        if( pPSBlob != nullptr )
        {
            auto const hr = _ctx->dev()->CreatePixelShader(
                pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &shd.ps ) ;

            natus::log::global_t::error( FAILED( hr ), "[d3d11] : CreatePixelShader" ) ;
            pPSBlob->Release();
            pPSBlob = nullptr ;
        }

        // shader input variables
        // the final layout elements for the d3d input layout
        // is done if the render configuration is known.
        {
            obj.for_each_vertex_input_binding( [&] (
                natus::graphics::vertex_attribute const va, natus::ntd::string_cref_t name )
            {
                shd.vertex_inputs.emplace_back( this_t::shader_data::vertex_input_binding
                    { va, name } ) ;
            } ) ;
        }

        return oid ;
    }

    //***********************
    size_t construct_render_config( size_t oid, natus::graphics::render_object_ref_t obj )
    {
        oid = this_t::determine_oid( obj.name(), renders ) ;
        
        this_t::render_data_ref_t rd = renders[ oid ] ;
        rd.name = obj.name() ;

        if( rd.vertex_layout != nullptr )
        {
            rd.vertex_layout->Release() ;
            rd.vertex_layout = nullptr ;
        }

        std::memset( rd.layout, 0, ARRAYSIZE( rd.layout ) ) ;

        return oid ;
    }

    bool_t update( size_t const id, natus::graphics::render_object_ref_t rc )
    {
        auto& rd = renders[ id ] ;
        rd.geo_id = size_t( -1 ) ;
        rd.shd_id = size_t( -1 ) ;

        // find geometry
        {
            auto const iter = std::find_if( geo_datas.begin(), geo_datas.end(),
                [&] ( this_t::geo_data_cref_t d )
            {
                return d.name == rc.get_geometry() ;
            } ) ;
            if( iter == geo_datas.end() )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "no geometry with name [" + rc.get_geometry() + "] for render_config [" + rc.name() + "]" ) ) ;
                return false ;
            }

            rd.geo_id = std::distance( geo_datas.begin(), iter ) ;
        }

        // find shader
        {
            auto const iter = std::find_if( shaders.begin(), shaders.end(),
                [&] ( this_t::shader_data_cref_t d )
            {
                return d.name == rc.get_shader() ;
            } ) ;
            if( iter == shaders.end() )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "no shader with name [" + rc.get_shader() + "] for render_config [" + rc.name() + "]" ) ) ;
                return false ;
            }

            rd.shd_id = std::distance( shaders.begin(), iter ) ;
        }

        // may happen if shaders did not compile properly the first time.
        if( rd.shd_id == size_t(-1) || shaders[ rd.shd_id ].vs_blob == nullptr )
        {
            natus::log::global_t::warning( natus_log_fn(
                "something strange happened to render_config [" + rc.name() + "]" ) ) ;
            return false ;
        }

        
        // for binding attributes, the shader and the geometry is required.
        {
            size_t i = 0 ; 
            this_t::shader_data_ref_t shd = shaders[ rd.shd_id ] ;
            UINT offset = 0 ;
            for( auto const & b : shd.vertex_inputs )
            {
                char_cptr_t name = natus::graphics::d3d11::vertex_binding_to_semantic( b.va ).c_str() ;
                UINT const semantic_index = 0 ;
                DXGI_FORMAT const fmt = geo_datas[rd.geo_id].get_format_from_element( b.va ) ;
                UINT input_slot = 0 ;
                UINT aligned_byte_offset = offset ;
                D3D11_INPUT_CLASSIFICATION const iclass = D3D11_INPUT_PER_VERTEX_DATA ;
                UINT instance_data_step_rate = 0 ;
                
                rd.layout[ i++ ] = { name, semantic_index, fmt, input_slot, 
                    aligned_byte_offset, iclass, instance_data_step_rate } ;

                offset += geo_datas[rd.geo_id].get_sib( b.va ) ;
            }

            UINT const num_elements = UINT( i ) ;

            if( rd.vertex_layout != nullptr )
            {
                rd.vertex_layout->Release() ;
                rd.vertex_layout = nullptr ;
            }

            auto const hr = _ctx->dev()->CreateInputLayout( rd.layout, num_elements, shd.vs_blob->GetBufferPointer(),
                shd.vs_blob->GetBufferSize(), &rd.vertex_layout ) ;

            if( FAILED( hr ) )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "CreateInputLayout for shader [" + shd.name + "] and render object[" + rc.name() + "]" ) ) ;
                return false ;
            }
        }
        
        // release all cbuffers
        {
            for( auto& vsd : rd.var_sets_data )
            {
                for( auto& b : vsd.second )
                {
                    if( b.mem != nullptr )
                    {
                        natus::memory::global_t::dealloc_raw( b.mem ) ;
                        b.mem = nullptr ;
                    }

                    if( b.ptr != nullptr )
                    {
                        b.ptr->Release() ;
                        b.ptr = nullptr ;
                    }
                }
                vsd.second.clear() ;
            }
            rd.var_sets_data.clear() ;
        }

        // release all cbuffers
        {
            for( auto& vsd : rd.var_sets_data_ps )
            {
                for( auto& b : vsd.second )
                {
                    if( b.mem != nullptr )
                    {
                        natus::memory::global_t::dealloc_raw( b.mem ) ;
                        b.mem = nullptr ;
                    }

                    if( b.ptr != nullptr )
                    {
                        b.ptr->Release() ;
                        b.ptr = nullptr ;
                    }
                }
                vsd.second.clear() ;
            }
            rd.var_sets_data_ps.clear() ;
        }

        // constant buffer mapping
        {
            this_t::shader_data_ref_t shd = shaders[ rd.shd_id ] ;
            rc.for_each( [&] ( size_t const /*i*/, natus::graphics::variable_set_res_t vs )
            {
                {
                    this_t::render_data_t::cbuffers_t cbs ;

                    for( auto& c : shd.vs_cbuffers )
                    {
                        this_t::render_data_t::cbuffer_t cb ;
                        cb.mem = natus::memory::global_t::alloc_raw< uint8_t >( c.sib ) ;
                        cb.slot = c.slot ;

                        D3D11_BUFFER_DESC bd = { } ;
                        bd.Usage = D3D11_USAGE_DEFAULT ;
                        bd.ByteWidth = UINT( c.sib ) ;
                        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER ;
                        bd.CPUAccessFlags = 0 ;

                        D3D11_SUBRESOURCE_DATA InitData = { } ;
                        InitData.pSysMem = cb.mem ;
                        auto const hr = _ctx->dev()->CreateBuffer( &bd, &InitData, &cb.ptr ) ;
                        if( FAILED( hr ) )
                        {
                            natus::log::global_t::error( natus_log_fn( "D3D11_BIND_CONSTANT_BUFFER" ) ) ;
                        }

                        for( auto& var : c.datas )
                        {
                            render_data_t::data_variable_t dv ;
                            dv.ivar = vs->data_variable( var.name, var.t, var.ts ) ;
                            dv.sib = natus::graphics::size_of( var.t ) * natus::graphics::size_of( var.ts ) ;
                            dv.name = var.name ;
                            dv.t = var.t ;
                            dv.ts = var.ts ;
                            cb.data_variables.emplace_back( dv ) ;
                        }

                        cbs.emplace_back( std::move( cb ) ) ;
                    }
                    rd.var_sets_data.emplace_back( std::make_pair( vs, std::move( cbs ) ) ) ;
                }

                // gs cbuffers
               
                {
                    this_t::render_data_t::cbuffers_t cbs ;

                    for( auto& c : shd.ps_cbuffers )
                    {
                        this_t::render_data_t::cbuffer_t cb ;
                        cb.mem = natus::memory::global_t::alloc_raw< uint8_t >( c.sib ) ;
                        cb.slot = c.slot ;

                        D3D11_BUFFER_DESC bd = { } ;
                        bd.Usage = D3D11_USAGE_DEFAULT ;
                        bd.ByteWidth = UINT( c.sib ) ;
                        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER ;
                        bd.CPUAccessFlags = 0 ;

                        D3D11_SUBRESOURCE_DATA InitData = { } ;
                        InitData.pSysMem = cb.mem ;
                        auto const hr = _ctx->dev()->CreateBuffer( &bd, &InitData, &cb.ptr ) ;
                        if( FAILED( hr ) )
                        {
                            natus::log::global_t::error( natus_log_fn( "D3D11_BIND_CONSTANT_BUFFER" ) ) ;
                        }

                        for( auto& var : c.datas )
                        {
                            render_data_t::data_variable_t dv ;
                            dv.ivar = vs->data_variable( var.name, var.t, var.ts ) ;
                            dv.sib = natus::graphics::size_of( var.t ) * natus::graphics::size_of( var.ts ) ;
                            dv.name = var.name ;
                            dv.t = var.t ;
                            dv.ts = var.ts ;
                            cb.data_variables.emplace_back( dv ) ;
                        }

                        cbs.emplace_back( std::move( cb ) ) ;
                    }

                    rd.var_sets_data_ps.emplace_back( std::make_pair( vs, std::move( cbs ) ) ) ;
                }

                // texture buffers... some day

                 
            } ) ;
        }

        // texture variable mapping
        {
            this_t::shader_data_ref_t shd = shaders[ rd.shd_id ] ;
            rc.for_each( [&] ( size_t const /*i*/, natus::graphics::variable_set_res_t vs )
            {
                this_t::render_data_t::image_variables_t ivs ;
                for( auto& t : shd.ps_textures )
                {
                    auto * dv = vs->texture_variable( t.name ) ;
                    natus::ntd::string_t const img_name = dv->get() ;

                    size_t i = 0 ;
                    for( ; i < images.size(); ++i ) if( images[ i ].name == img_name ) break ;

                    if( i == images.size() ) continue ;
                    
                    this_t::render_data_t::image_variable_t iv ;
                    iv.id = i ;
                    iv.name = t.name ;
                    iv.slot = t.slot ;
                    ivs.emplace_back( std::move( iv ) ) ;
                }
                rd.var_sets_imgs_ps.emplace_back( std::make_pair( vs, std::move( ivs ) ) ) ;
            } ) ;
        }

        return true ;
    }

    //***********************
    size_t construct_image_config( size_t oid, natus::graphics::image_object_ref_t obj )
    {
        oid = this_t::determine_oid( obj.name(), images ) ;

        this_t::image_data_ref_t img = images[ oid ] ;
        img.name = obj.name() ;

        if( img.texture != nullptr )
        {
            img.texture->Release() ;
            img.sampler->Release() ;
            img.view->Release() ;

            img.texture = nullptr ;
            img.sampler = nullptr ;
            img.view = nullptr ;
        }

        auto * dev = _ctx->dev() ;

        // sampler
        {
            D3D11_SAMPLER_DESC sampDesc = { } ;
            sampDesc.Filter = natus::graphics::d3d11::convert( 
                obj.get_filter( natus::graphics::texture_filter_mode::min_filter ),
                obj.get_filter( natus::graphics::texture_filter_mode::mag_filter ) ) ;
            
            sampDesc.AddressU = natus::graphics::d3d11::convert(
                obj.get_wrap( natus::graphics::texture_wrap_mode::wrap_s ) ) ;

            sampDesc.AddressV = natus::graphics::d3d11::convert(
                obj.get_wrap( natus::graphics::texture_wrap_mode::wrap_t ) ) ;

            sampDesc.AddressW = natus::graphics::d3d11::convert(
                obj.get_wrap( natus::graphics::texture_wrap_mode::wrap_r ) ) ;

            sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER ;
            sampDesc.MinLOD = 0;
            sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

            auto const hr = dev->CreateSamplerState( &sampDesc, &img.sampler );
            if( FAILED( hr ) )
            {
                natus::log::global_t::error( natus_log_fn( "CreateSamplerState" ) ) ;
            }
        }

        // texture
        {
            auto iref = obj.image() ;

            size_t const width = iref.get_dims().x() ;
            size_t const height = iref.get_dims().y() ;
            D3D11_TEXTURE2D_DESC desc = { } ;
            desc.Width = static_cast< UINT >( width ) ;
            desc.Height = static_cast< UINT >( height ) ;
            desc.MipLevels = static_cast< UINT >( 1 );
            desc.ArraySize = static_cast< UINT >( 1 );
            desc.Format = natus::graphics::d3d11::convert( iref.get_image_format(), iref.get_image_element_type() ) ;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT ;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE ;
            desc.CPUAccessFlags = 0 ;
            desc.MiscFlags = 0 ;

            size_t const bpp = natus::graphics::size_of( iref.get_image_format() ) *
                natus::graphics::size_of( iref.get_image_element_type() ) ;

            //uint8_ptr_t mem = natus::memory::global_t::alloc_raw<uint8_t>( width * height * 4 ) ;
            //for( size_t p = 0; p < width * height * 4; ++p ) mem[ p ] = uint8_t( 255 ) ;

            D3D11_SUBRESOURCE_DATA init_data = { } ;
            init_data.pSysMem = iref.get_image_ptr() ;
            init_data.SysMemPitch = UINT( width * bpp ) ;
            init_data.SysMemSlicePitch = UINT( width * height * bpp ) ;

            // create the texture object
            {
                auto const hr = dev->CreateTexture2D( &desc, &init_data, &img.texture ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "CreateTexture2D" ) ) ;
                }
            }

            // create the resource view for the texture in order to be
            // shader variable bindable.
            if( img.texture != nullptr )
            {
                D3D11_SHADER_RESOURCE_VIEW_DESC res_desc = { } ;
                res_desc.Format = desc.Format ;
                res_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D ;
                res_desc.Texture2D.MostDetailedMip = 0 ;
                res_desc.Texture2D.MipLevels = UINT(1) ;

                auto const hr = dev->CreateShaderResourceView( img.texture, &res_desc, &img.view ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "CreateShaderResourceView for texture : [" + img.name + "]" ) ) ;
                }
            }
        }

        return oid ;
    }

    bool_t render( size_t const id, size_t const varset_id = size_t( 0 ), UINT const start_element = UINT( 0 ),
        UINT const num_elements = UINT( -1 ) )
    {
        this_t::render_data_ref_t rnd = renders[ id ] ;

        if( rnd.shd_id == size_t( -1 ) )
        {
            natus::log::global_t::error( natus_log_fn( "shader invalid. First shader compilation failed probably." ) ) ;
            return false ;
        }

        
        this_t::shader_data_cref_t shd = shaders[ rnd.shd_id ] ;
        this_t::geo_data_ref_t geo = geo_datas[ rnd.geo_id ] ;

        ID3D11DeviceContext * ctx = _ctx->ctx() ;

        // vertex shader variables
        for( auto & cb : rnd.var_sets_data[ varset_id ].second )
        {
            size_t offset = 0 ;

            for( auto iter = cb.data_variables.begin(); iter != cb.data_variables.end(); ++iter )
            {
                // if a variable was not there at construction time, 
                // try it once more. If still not found, remove the entry.
                while( iter->ivar == nullptr )
                {
                    auto & var = *iter ;
                    auto * ptr = rnd.var_sets_data[ varset_id ].first->data_variable( var.name, var.t, var.ts ) ;
                    if( ptr == nullptr )
                    {
                        iter = cb.data_variables.erase( iter ) ;
                    }
                    if( iter == cb.data_variables.end() ) break ;
                }
                if( iter == cb.data_variables.end() ) break ;

                iter->do_copy_funk( uint8_ptr_t(cb.mem) + offset ) ;
                offset += iter->sib ;
            }

            ctx->UpdateSubresource( cb.ptr, 0, nullptr, cb.mem, 0, 0 );
            ctx->VSSetConstantBuffers( cb.slot, 1, &cb.ptr ) ;
        }

        // pixel shader variables
        for( auto& cb : rnd.var_sets_data_ps[ varset_id ].second )
        {
            size_t offset = 0 ;

            for( auto iter = cb.data_variables.begin(); iter != cb.data_variables.end(); ++iter )
            {
                // if a variable was not there at construction time, 
                // try it once more. If still not found, remove the entry.
                while( iter->ivar == nullptr )
                {
                    auto& var = *iter ;
                    auto* ptr = rnd.var_sets_data_ps[ varset_id ].first->data_variable( var.name, var.t, var.ts ) ;
                    if( ptr == nullptr )
                    {
                        iter = cb.data_variables.erase( iter ) ;
                    }
                    if( iter == cb.data_variables.end() ) break ;
                }
                if( iter == cb.data_variables.end() ) break ;

                iter->do_copy_funk( uint8_ptr_t( cb.mem ) + offset ) ;
                offset += iter->sib ;
            }

            ctx->UpdateSubresource( cb.ptr, 0, nullptr, cb.mem, 0, 0 );
            ctx->PSSetConstantBuffers( cb.slot, 1, &cb.ptr ) ;
        }

        ctx->IASetInputLayout( rnd.vertex_layout );

        {
            UINT const stride = geo.stride ;
            UINT const offset = 0 ;
            ctx->IASetVertexBuffers( 0, 1, &geo.vb, &stride, &offset );
        }
        
        ctx->IASetPrimitiveTopology( natus::graphics::d3d11::convert( geo.pt ) ) ;

        ctx->VSSetShader( shd.vs, nullptr, 0 ) ;
        ctx->PSSetShader( shd.ps, nullptr, 0 ) ;

        for( auto& img : rnd.var_sets_imgs_ps[ varset_id ].second )
        {
            ctx->PSSetShaderResources( img.slot, 1, &(images[ img.id ].view) ) ;
            ctx->PSSetSamplers( img.slot, 1, &(images[ img.id ].sampler) ) ;
        }

        if( geo.ib != nullptr )
        {
            ctx->IASetIndexBuffer( geo.ib, DXGI_FORMAT_R32_UINT, 0 );

            UINT const max_elems = num_elements == UINT( -1 ) ? UINT( geo.num_elements_ib ) : num_elements ;
            ctx->DrawIndexed( max_elems, start_element, 0 ) ;
        }
        else
        {
            UINT const max_elems = num_elements == UINT( -1 ) ? UINT( geo.num_elements_vb ) : num_elements ;
            ctx->Draw( max_elems, UINT( start_element ) ) ;
        }
        
        return true ;
    }

    void_t do_render_states( natus::graphics::render_state_sets_in_t rs, ID3D11BlendState* d3drs = nullptr,
        ID3D11RasterizerState* d3draster_state = nullptr )
    {
        // blend
        {
            D3D11_BLEND_DESC desc = { } ;

            desc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            if( render_states.blend_s.do_blend != rs.blend_s.do_blend )
            {
                if( rs.blend_s.do_blend )
                {
                    desc.RenderTarget[ 0 ].BlendEnable = true ;

                    desc.RenderTarget[ 0 ].SrcBlend = natus::graphics::d3d11::convert( rs.blend_s.src_blend_factor ) ;
                    desc.RenderTarget[ 0 ].DestBlend = natus::graphics::d3d11::convert( rs.blend_s.dst_blend_factor ) ;
                    desc.RenderTarget[ 0 ].BlendOp = natus::graphics::d3d11::convert( rs.blend_s.blend_func ) ;

                    desc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE ;
                    desc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO ;
                    desc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD ;
                }
                else
                {
                    desc.RenderTarget[ 0 ].BlendEnable = false ;
                }

                ID3D11BlendState* old = nullptr ;
                UINT mask ;
                _ctx->ctx()->OMGetBlendState( &old, 0, &mask ) ;

                if( d3drs != nullptr )
                {
                    _ctx->ctx()->OMSetBlendState( d3drs, 0, 0xffffffff );
                }
                else
                {
                    auto const res = _ctx->dev()->CreateBlendState( &desc, &d3drs ) ;
                    if( SUCCEEDED( res ) )
                    {
                        _ctx->ctx()->OMSetBlendState( d3drs, 0, 0xffffffff );
                        d3drs->Release() ;
                    }
                }

                if( old != nullptr )
                {
                    _ctx->ctx()->OMSetBlendState( old, 0, mask ) ;
                }
            }
        }

        // scissor/culling
        {
            D3D11_RASTERIZER_DESC desc = { } ;
            
            if( !rs.polygon_s.do_culling )
            {
                desc.CullMode = D3D11_CULL_NONE ;
                desc.FillMode = D3D11_FILL_SOLID ;
            }
            else
            {
                desc.CullMode = natus::graphics::d3d11::convert( rs.polygon_s.cm ) ;
                desc.FillMode = natus::graphics::d3d11::convert( rs.polygon_s.fm ) ;
            }

            bool_t const created = d3draster_state == nullptr ;
            if( d3draster_state == nullptr )
            {
                auto const res = _ctx->dev()->CreateRasterizerState( &desc, &d3draster_state ) ;
                if( FAILED( res ) )
                {
                    natus::log::global_t::error( natus_log_fn("CreateRasterizerState") ) ;
                }
            }

            desc.ScissorEnable = rs.scissor_s.do_scissor_test ;

            if( rs.scissor_s.do_scissor_test )
            {
                D3D11_RECT rect ;
                rect.left = rs.scissor_s.rect.x() ;
                rect.right = rs.scissor_s.rect.x() + rs.scissor_s.rect.z() ;
                rect.top = rs.scissor_s.rect.y() ;
                rect.bottom = rs.scissor_s.rect.y() + rs.scissor_s.rect.w() ;

                _ctx->ctx()->RSSetScissorRects( 1, &rect ) ;
            }

            if( d3draster_state )
            {
                _ctx->ctx()->RSSetState( d3draster_state ) ;
            }

            if( created ) d3draster_state->Release() ;
        }

        #if 0
        if( render_states.depth_s.do_depth_test != rs.depth_s.do_depth_test )
        {
            if( rs.depth_s.do_depth_test )
            {
                glEnable( GL_DEPTH_TEST ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glEnable" ) ) ;
            }
            else
            {
                glDisable( GL_DEPTH_TEST ) ;
                natus::ogl::error::check_and_log( natus_log_fn( "glDisable" ) ) ;
            }
        }
       
        #endif

        
    }

    void_t begin_frame( void_t )
    {
        auto const ids_old = _state_id_stack[ 0 ] ;
        auto const ids_new = _state_id_stack[ 1 ] ;

        // set the viewport to the default new state, 
        // so the correct viewport is set automatically.
        {
            natus::math::vec4ui_t vp = _states[ ids_new.first ].states[ ids_new.second ].view_s.vp ;
            vp.z( uint_t( vp_width ) ) ;
            vp.w( uint_t( vp_height ) ) ;
            _states[ ids_new.first ].states[ ids_new.second ].view_s.vp = vp ;
        }

        this_t::handle_render_state(
            _states[ ids_new.first ].states[ ids_new.second ],
            _states[ ids_old.first ].states[ ids_old.second ] ) ;
    }

    void_t end_frame( void_t )
    {
    }

    natus::ntd::string_t remove_unwanded_characters( natus::ntd::string_cref_t code_in ) const noexcept
    {
        auto code = code_in ;

        // remove \r
        {
            size_t pos = 0 ;
            while( pos != code.size() )
            {
                if( code[ pos ] == '\r' ) code[ pos ] = ' ' ; ++pos ;
            }
        }

        // remove preceding, trailing ' '
        {
            size_t a = code.find_first_not_of( ' ' ) ;
            size_t b = code.find_last_not_of( ' ' ) ;
            a = a == std::string::npos ? 0 : a ;
            b = b == std::string::npos ? code.size() - a : b - a ;
            code = code.substr( a, b + 1 ) ;
        }

        return std::move( code ) ;
    }

    static shader_data_t::image_variables_t determine_texture( ID3DBlob * blob ) noexcept
    {
        shader_data_t::image_variables_t ret ;

        guard< ID3D11ShaderReflection >  reflector ;
        D3DReflect( blob->GetBufferPointer(), blob->GetBufferSize(),
            IID_ID3D11ShaderReflection, reflector ) ;

        D3D11_SHADER_DESC shd_desc ;
        {
            auto const hr = reflector->GetDesc( &shd_desc ) ;
            if( FAILED( hr ) )
            {
                natus::log::global_t::error( natus_log_fn( "Can not get shader reflection desc" ) ) ;
                return shader_data_t::image_variables_t() ;
            }
        }

        UINT const num_irb = shd_desc.BoundResources ;
        for( UINT i=0; i<num_irb; ++i )
        {
            D3D11_SHADER_INPUT_BIND_DESC ibd ;
            {
                auto const hr = reflector->GetResourceBindingDesc( i, &ibd ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "GetResourceBindingDesc" ) ) ;
                    return shader_data_t::image_variables_t() ;
                }
            }

            if( (ibd.Type != D3D_SIT_TEXTURE) && (ibd.Type != D3D_SIT_SAMPLER) ) continue ;

            // somehow, we can not do this:
            // ID3D11ShaderReflectionVariable * var = reflector->GetVariableByName( ibd.Name ) ;
            // getting the type from the variable will not work. So we are going through 
            // yet another D3D11 types enum.
            if( ibd.Type == D3D_SIT_SAMPLER )
            {
                // ?
            }
            else if( ibd.Type == D3D_SIT_TEXTURE && ibd.Dimension == D3D_SRV_DIMENSION_TEXTURE2D )
            {
                shader_data_t::image_variable_t ivar ;
                ivar.name = ibd.Name ;
                ivar.slot = ibd.BindPoint ;
                ret.emplace_back( ivar ) ;
            }
            else
            {
                natus::log::global_t::warning( natus_log_fn("Detected texture but type is not supported.") ) ;
            }
        }

        return std::move( ret ) ;
    }

    // performs reflection on the constant buffers present in
    // a shader for user variable binding
    static shader_data_t::cbuffers_t determine_cbuffer( ID3DBlob * blob ) noexcept
    {
        shader_data_t::cbuffers_t ret ;

        ID3D11ShaderReflection * reflector = nullptr ;
        D3DReflect( blob->GetBufferPointer(), blob->GetBufferSize(),
            IID_ID3D11ShaderReflection, ( void** ) &reflector ) ;

        D3D11_SHADER_DESC shd_desc ;
        {
            auto const hr = reflector->GetDesc( &shd_desc ) ;
            if( FAILED( hr ) )
            {
                natus::log::global_t::error( natus_log_fn( "Can not get shader reflection desc" ) ) ;
                return shader_data_t::cbuffers_t() ;
            }
        }
        
        UINT const num_cb = shd_desc.ConstantBuffers ;
        for( UINT i = 0; i < num_cb; ++i )
        {
            shader_data_t::cbuffer cbuffer ;

            ID3D11ShaderReflectionConstantBuffer* cbr = reflector->GetConstantBufferByIndex( i ) ;

            D3D11_SHADER_BUFFER_DESC buf_desc ;
            {
                auto hr = cbr->GetDesc( &buf_desc ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_SHADER_BUFFER_DESC" ) ) ;
                    continue ;
                }
            }
            cbuffer.name = natus::ntd::string_t( buf_desc.Name ) ;
            cbuffer.sib = size_t( buf_desc.Size ) ;

            // figure out the register the resource is bound to
            // this is done through cbuffer xyz : register(bn)
            // where n needs to be determined
            {
                D3D11_SHADER_INPUT_BIND_DESC res_desc ;
                auto const hr = reflector->GetResourceBindingDescByName( buf_desc.Name, &res_desc ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_SHADER_INPUT_BIND_DESC" ) ) ;
                    continue ;
                }
                cbuffer.slot = res_desc.BindPoint ;
            }

            for( UINT j = 0; j < buf_desc.Variables; ++j )
            {
                ID3D11ShaderReflectionVariable* var = cbr->GetVariableByIndex( j ) ;

                D3D11_SHADER_VARIABLE_DESC var_desc ;
                {
                    auto const hr = var->GetDesc( &var_desc ) ;
                    if( FAILED( hr ) )
                    {
                        natus::log::global_t::error( natus_log_fn(
                            "D3D11_SHADER_VARIABLE_DESC" ) ) ;
                        continue ;
                    }
                }

                ID3D11ShaderReflectionType* type = var->GetType() ;
                D3D11_SHADER_TYPE_DESC type_desc ;
                {
                    auto const hr = type->GetDesc( &type_desc ) ;
                    if( FAILED( hr ) )
                    {
                        natus::log::global_t::error( natus_log_fn(
                            "D3D11_SHADER_TYPE_DESC" ) ) ;
                        continue ;
                    }
                }

                if( natus::graphics::d3d11::is_texture_type( type_desc.Type ) )
                {
                    // this is in another function - determine_texture
                }
                else if( natus::graphics::d3d11::is_buffer_type( type_desc.Type ) )
                {
                    // this is in another function - determin_texture
                }
                else
                {
                    UINT const elems = std::max( type_desc.Rows, type_desc.Columns ) ;
                    auto const ts = natus::graphics::d3d11::to_type_struct( type_desc.Class, elems ) ;
                    auto const t = natus::graphics::d3d11::to_type( type_desc.Type ) ;
                    if( ts == natus::graphics::type_struct::undefined ||
                        t == natus::graphics::type::undefined ) continue ;

                    shader_data_t::data_variable_t dv ;
                    dv.name = natus::ntd::string_t( var_desc.Name ) ;
                    dv.t = t ;
                    dv.ts = ts ;

                    cbuffer.datas.emplace_back( dv ) ;
                }
            }

            ret.emplace_back( std::move( cbuffer ) ) ;
        }
        return std::move( ret ) ;
    }
} ;

//************************************************************************************************
//
//
//************************************************************************************************

//****
d3d11_backend::d3d11_backend( natus::graphics::d3d11_context_ptr_t ctx ) noexcept : 
    backend( natus::graphics::backend_type::d3d11 )
{
    _pimpl = natus::memory::global_t::alloc( this_t::pimpl( ctx ), 
        natus_log_fn("d3d11_backend::pimpl") ) ;

    _context = ctx ;
}

//****
d3d11_backend::d3d11_backend( this_rref_t rhv ) noexcept : backend( ::std::move( rhv ) )
{
    natus_move_member_ptr( _pimpl, rhv ) ;
    natus_move_member_ptr( _context, rhv ) ;
}

//****
d3d11_backend::~d3d11_backend( void_t ) 
{
    natus::memory::global_t::dealloc( _pimpl ) ;
}

//****
void_t d3d11_backend::set_window_info( window_info_cref_t wi ) noexcept 
{
    {
        //bool_t change = false ;
        if( wi.width != 0 )
        {
            _pimpl->vp_width = FLOAT( wi.width ) ;
            
        }
        if( wi.height != 0 )
        {
            _pimpl->vp_height = FLOAT( wi.height ) ;
            
        }
    }
}

//****
natus::graphics::result d3d11_backend::configure( natus::graphics::geometry_object_res_t gconf ) noexcept 
{
    natus::graphics::id_res_t id = gconf->get_id() ;

    if( gconf->name().empty() )
        return natus::graphics::result::invalid_argument ;
    
    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_geo( 
            id->get_oid( this_t::get_bid() ), *gconf ) ) ;
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
natus::graphics::result d3d11_backend::configure( natus::graphics::render_object_res_t config ) noexcept 
{
    natus::graphics::id_res_t id = config->get_id() ;
    
    {
        id->set_oid( this_t::get_bid(),
            _pimpl->construct_render_config( id->get_oid( this_t::get_bid() ), *config ) ) ;
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
natus::graphics::result d3d11_backend::configure( natus::graphics::shader_object_res_t config ) noexcept
{
    natus::graphics::id_res_t id = config->get_id() ;

    
    {
        id->set_oid( this_t::get_bid(),
            _pimpl->construct_shader_config( id->get_oid( this_t::get_bid() ), *config ) ) ;
    }

    #if 0
    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }
    #endif
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result d3d11_backend::configure( natus::graphics::image_object_res_t config ) noexcept 
{
    natus::graphics::id_res_t id = config->get_id() ;
    
    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_image_config( 
            id->get_oid( this_t::get_bid() ), *config ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    #if 0
    {
        auto const res = _pimpl->update( oid, *config ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }
    #endif
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result d3d11_backend::configure( natus::graphics::framebuffer_object_res_t obj ) noexcept 
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
natus::graphics::result d3d11_backend::configure( natus::graphics::state_object_res_t ) noexcept
{
    return natus::graphics::result::ok ;
}

//***
natus::graphics::result d3d11_backend::connect( natus::graphics::render_object_res_t config, natus::graphics::variable_set_res_t vs ) noexcept
{
    natus::graphics::id_res_t id = config->get_id() ;

    #if 0
    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid render configuration id" ) ) ;
        return natus::graphics::result::failed ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    auto const res = _pimpl->connect( oid, vs ) ;
    natus::log::global_t::error( natus::graphics::is_not( res ), 
        natus_log_fn( "connect variable set" ) ) ;
   
    #endif

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::update( natus::graphics::geometry_object_res_t config ) noexcept 
{
    natus::graphics::id_res_t id = config->get_id() ;
    
    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid geometry object id" ) ) ;
        return natus::graphics::result::failed ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    auto const res = _pimpl->update( oid, config ) ;
    natus::log::global_t::error( natus::graphics::is_not( res ),
        natus_log_fn( "update geometry" ) ) ;
    

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::use( natus::graphics::framebuffer_object_res_t obj ) noexcept 
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
natus::graphics::result d3d11_backend::use( size_t const, natus::graphics::state_object_res_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::render( natus::graphics::render_object_res_t config, natus::graphics::backend::render_detail_cref_t detail ) noexcept 
{ 
    natus::graphics::id_res_t id = config->get_id() ;

    //natus::log::global_t::status( natus_log_fn("render") ) ;
    
    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return natus::graphics::result::failed ;
    }

    _pimpl->render( id->get_oid( this_t::get_bid() ), 
        detail.varset, (UINT)detail.start, (UINT)detail.num_elems ) ;
    
    return natus::graphics::result::ok ;
}

//****
void_t d3d11_backend::render_begin( void_t ) noexcept 
{
    _pimpl->begin_frame() ;
}

void_t d3d11_backend::render_end( void_t ) noexcept 
{
    _pimpl->end_frame() ;
}