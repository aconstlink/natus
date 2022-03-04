

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
#include <directxcolors.h>

using namespace natus::graphics ;

template< typename T >
class guard
{
    natus_this_typedefs( guard< T > ) ;

private:

    T * _ptr = nullptr ;

    void_t invalidate( void_t ) noexcept
    {
        if( _ptr != nullptr ) _ptr->Release() ;
        _ptr = nullptr ;
    }

public:

    guard( void_t ) noexcept {}
    ~guard( void_t ) noexcept
    {
        this_t::invalidate() ;
    }

    operator T const * ( void_t ) const noexcept
    {
        return _ptr ;
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

    bool_t operator != ( T * other ) const noexcept 
    {
        return _ptr != other ;
    }

    bool_t operator == ( T * other ) const noexcept 
    {
        return _ptr == other ;
    }

    // just replaces the pointer
    this_ref_t operator = ( T * other ) noexcept
    {
        _ptr = other ;
        return *this ;
    }

    // invalidates this and takes other
    this_ref_t operator = ( this_rref_t other ) noexcept
    {
        this_t::invalidate() ;
        _ptr = other.move_out() ;
        return *this ;
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

        guard< ID3D11Buffer > vb ;
        guard< ID3D11Buffer > ib ;

        // per vertex sib
        UINT stride = 0 ;

        natus::graphics::primitive_type pt = natus::graphics::primitive_type::undefined ;

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

        geo_data( void_t ) noexcept{}
        geo_data( geo_data const & ) = delete ;
        geo_data( geo_data && rhv ) noexcept
        {
            valid = rhv.valid ;
            name = rhv.name ;

            natus_move_member_ptr( vb_mem, rhv ) ;
            natus_move_member_ptr( ib_mem, rhv ) ;

            num_elements_vb = rhv.num_elements_vb ;
            num_elements_ib = rhv.num_elements_ib ;

            sib_vb = rhv.sib_vb ;
            sib_ib = rhv.sib_ib ;

            vb = std::move( rhv.vb ) ;
            ib = std::move( rhv.ib ) ;
            
            stride = rhv.stride ;

            pt = rhv.pt ;

            elements = std::move( rhv.elements ) ;
        }
        ~geo_data( void_t ) noexcept
        {
            invalidate() ;
        }

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

        void_t invalidate( void_t ) noexcept
        {
            valid = false ;
            name = "" ;

            natus::memory::global_t::dealloc_raw( vb_mem ) ;
            natus::memory::global_t::dealloc_raw( ib_mem ) ;

            vb_mem = nullptr ;
            ib_mem = nullptr ;

            num_elements_vb = 0 ;
            num_elements_ib = 0 ;

            sib_vb = 0 ;
            sib_ib = 0 ;

            vb = guard< ID3D11Buffer >() ;
            ib = guard< ID3D11Buffer >() ;

            stride = 0 ;

            pt = natus::graphics::primitive_type::undefined ;

            elements.clear() ;
        }
    };
    natus_typedef( geo_data ) ;

    struct shader_data
    {
        bool_t valid = false ;
        natus::ntd::string_t name ;

        // vs, gs, ps, else?
        guard< ID3D11VertexShader > vs ;
        guard< ID3D11GeometryShader > gs ;
        guard< ID3D11PixelShader > ps ;

        // required for later vertex layout creation
        guard< ID3DBlob > vs_blob ;

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

        struct buffer_variable
        {
            natus::ntd::string_t name ;
            UINT slot ;
        } ;
        natus_typedef( buffer_variable ) ;
        natus_typedefs( natus::ntd::vector< buffer_variable_t >, buffer_variables ) ;

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

        buffer_variables_t vs_buffers ;
        buffer_variables_t ps_buffers ;

        shader_data( void_t ) noexcept {}
        shader_data( shader_data const & ) = delete ;
        shader_data( shader_data && rhv ) noexcept
        {
            valid = rhv.valid ;
            name = std::move( rhv.name ) ;

            vs = std::move( rhv.vs ) ;
            gs = std::move( rhv.gs ) ;
            ps = std::move( rhv.ps ) ;
            vs_blob = std::move( rhv.vs_blob ) ;
            
            vertex_inputs = std::move( rhv.vertex_inputs ) ;
            
            vs_cbuffers = std::move( rhv.vs_cbuffers ) ;
            gs_cbuffers = std::move( rhv.gs_cbuffers ) ;
            ps_cbuffers = std::move( rhv.ps_cbuffers ) ;

            vs_textures = std::move( rhv.vs_textures ) ;
            ps_textures = std::move( rhv.ps_textures ) ;

            vs_buffers = std::move( rhv.vs_buffers ) ;
            ps_buffers = std::move( rhv.ps_buffers ) ;
        }

        void_t invalidate( void_t ) noexcept
        {
            valid = false ;
            name = "" ;

            vs = guard< ID3D11VertexShader >() ;
            gs = guard< ID3D11GeometryShader >() ;
            ps = guard< ID3D11PixelShader >() ;
            vs_blob = guard< ID3DBlob >() ;
            
            vertex_inputs.clear() ;
            vs_cbuffers.clear() ;
            gs_cbuffers.clear() ;
            ps_cbuffers.clear() ;
            vs_textures.clear() ;
            ps_textures.clear() ;
            vs_buffers.clear() ;
            ps_buffers.clear() ;
        }
    } ;
    natus_typedef( shader_data ) ;

    struct image_data
    {
        bool_t valid = false ;
        natus::ntd::string_t name ;

        guard< ID3D11ShaderResourceView > view ;
        guard< ID3D11Texture2D > texture ;
        guard< ID3D11SamplerState > sampler ;

        // 0.0 : do not flip uv.v
        // 1.0 : do flip uv.v
        // render targets can not be flipped. those
        // are rendered upside down, so in the shader
        // the user needs to access this variable for
        // flipping information.
        float_t requires_y_flip = 0.0f ;

        image_data( void_t ) noexcept{}
        image_data( image_data const & ) = delete ;
        image_data( image_data && rhv ) noexcept
        {
            valid = rhv.valid ;
            rhv.valid = false ;
            name = std::move( rhv.name ) ;
            view = std::move( rhv.view ) ;
            texture = std::move( rhv.texture ) ;
            sampler = std::move( rhv.sampler ) ;
            requires_y_flip = rhv.requires_y_flip ;
        }

        ~image_data( void_t ) noexcept
        {
            invalidate() ;
        }

        void_t invalidate( void_t ) noexcept
        {
            valid = false ;
            name = "" ;
            view = guard< ID3D11ShaderResourceView >() ;
            texture = guard< ID3D11Texture2D >() ;
            sampler = guard< ID3D11SamplerState >() ;
            requires_y_flip = 0.0f ;
        }
    };
    natus_typedef( image_data ) ;

    struct array_data
    {
        bool_t valid = false ;
        natus::ntd::string_t name ;
        guard< ID3D11Buffer > buffer ;
        guard< ID3D11ShaderResourceView > view ;

        array_data( void_t ) noexcept{}
        array_data( array_data const & ) = delete ;
        array_data( array_data && rhv ) noexcept
        {
            valid = rhv.valid ;
            rhv.valid = false ;
            name = std::move( rhv.name ) ;
            buffer = std::move( rhv.buffer ) ;
            view = std::move( rhv.view ) ;
        }
        ~array_data( void_t ) noexcept
        {
            invalidate() ;
        }

        void_t invalidate( void_t ) noexcept
        {
            valid = false ;
            name = "" ;
            buffer = guard< ID3D11Buffer >() ;
            view = guard< ID3D11ShaderResourceView >() ;
        }
    };
    natus_typedef( array_data ) ;

    struct render_state_sets
    {
        natus::graphics::render_state_sets_t rss ;
        ID3D11DepthStencilState * depth_stencil_state = nullptr ;
        ID3D11RasterizerState * raster_state = nullptr ;
        ID3D11BlendState * blend_state = nullptr ;

        render_state_sets( void_t ) noexcept {}
        render_state_sets( render_state_sets const & rhv ) noexcept
        {
            rss = rhv.rss ;
            depth_stencil_state = rhv.depth_stencil_state ; 
            raster_state = rhv.raster_state ;
            blend_state = rhv.blend_state ;
        }
        render_state_sets( render_state_sets && rhv ) noexcept
        {
            rss = std::move( rhv.rss ) ;
            natus_move_member_ptr( depth_stencil_state, rhv ) ;
            natus_move_member_ptr( raster_state, rhv ) ;
            natus_move_member_ptr( blend_state, rhv ) ;
        }
        ~render_state_sets( void_t ) noexcept
        {
            // need to copy construct!
            //invalidate() ;
        }
        
        void_t invalidate( void_t ) noexcept
        {
            rss = natus::graphics::render_state_sets_t() ;
            guard< ID3D11DepthStencilState >() = depth_stencil_state ;
            guard< ID3D11RasterizerState >() = raster_state ;
            guard< ID3D11BlendState >() = blend_state ;
        }

        render_state_sets & operator = ( render_state_sets const & rhv ) noexcept
        {
            rss = rhv.rss ;
            depth_stencil_state = rhv.depth_stencil_state ; 
            raster_state = rhv.raster_state ;
            blend_state = rhv.blend_state ;
            return *this ;
        }
    };
    natus_typedef( render_state_sets ) ;

    struct state_data
    {
        bool_t valid = false ;
        natus::ntd::string_t name ;
        natus::ntd::vector< natus::graphics::render_state_sets_t > states ;
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

        struct buffer_variable
        {
            natus::ntd::string_t name ;
            UINT slot ;

            // index into the arrays vector
            size_t id = size_t( -1 ) ;
        } ;
        natus_typedef( buffer_variable ) ;
        natus_typedefs( natus::ntd::vector< buffer_variable_t >, buffer_variables ) ;

        natus::ntd::vector< std::pair< natus::graphics::variable_set_res_t,
            buffer_variables_t > > var_sets_buffers_vs ;

        natus::ntd::vector< std::pair< natus::graphics::variable_set_res_t,
            buffer_variables_t > > var_sets_buffers_ps ;

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

        render_data( void_t ) noexcept {}
        render_data( render_data const & ) = delete ;
        render_data( render_data && rhv ) noexcept
        {
            valid = rhv.valid ;
            name = std::move( rhv.name ) ;
            geo_id = rhv.geo_id ;
            rhv.geo_id = size_t( -1 ) ;
            shd_id = rhv.shd_id ;
            rhv.shd_id = size_t( -1 ) ;

            natus_move_member_ptr( vertex_layout, rhv ) ;
            natus_move_member_ptr( raster_state, rhv ) ;
            natus_move_member_ptr( blend_state, rhv ) ;

            var_sets_imgs_vs = std::move( rhv.var_sets_imgs_vs ) ;
            var_sets_imgs_ps = std::move( rhv.var_sets_imgs_ps ) ;
            var_sets_buffers_vs = std::move( rhv.var_sets_buffers_vs ) ;
            var_sets_buffers_ps = std::move( rhv.var_sets_buffers_ps ) ;
            var_sets_data = std::move( rhv.var_sets_data ) ;
            var_sets_data_ps = std::move( rhv.var_sets_data_ps ) ;
        }
        ~render_data( void_t ) noexcept
        {
            invalidate() ;
        }

        void_t invalidate( void_t ) noexcept
        {
            valid = false ;
            name = "" ;
            geo_id = size_t( -1 ) ;
            shd_id = size_t( -1 ) ;

            std::memset( layout, 0, sizeof( D3D11_INPUT_ELEMENT_DESC ) * size_t( natus::graphics::vertex_attribute::num_attributes ) ) ;
            if( vertex_layout != nullptr )
            {
                vertex_layout->Release() ;
                vertex_layout = nullptr ;
            }

            if( raster_state != nullptr ) 
            {
                raster_state->Release() ;
                raster_state = nullptr ;
            }
            
            if( blend_state != nullptr )
            {
                blend_state->Release() ;
                blend_state = nullptr ;
            }



            var_sets_imgs_vs.clear() ;
            var_sets_imgs_ps.clear() ;
            var_sets_buffers_vs.clear() ;
            var_sets_buffers_ps.clear() ;
            var_sets_data.clear() ;
            var_sets_data_ps.clear() ;
        }
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

        void_t invalidate( void_t ) noexcept
        {
            for( size_t i=0; i<9; ++i ) image_ids[i] = size_t( -1 ) ;
            for( size_t i=0; i<8; ++i ) if( rt_view[i] ) rt_view[i]->Release() ;
            if( ds_view ) ds_view->Release() ;
        }
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

    typedef natus::ntd::vector< this_t::array_data > array_datas_t ;
    array_datas_t arrays ;

    typedef natus::ntd::vector< this_t::framebuffer_data > framebuffer_datas_t ;
    framebuffer_datas_t framebuffers ;
    size_t _cur_fb_active = size_t( -1 ) ;

    natus::graphics::render_state_sets_t render_states ;

    typedef natus::ntd::vector< this_t::state_data_t > states_t ;
    states_t state_sets ;

    natus::ntd::stack< this_t::render_state_sets, 10 > _state_stack ;

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
            natus::graphics::state_object_t obj( "d3d11_default_states" ) ;

            auto new_states = *natus::graphics::backend_t::default_render_states() ;

            new_states.view_s.do_change = true ;
            new_states.view_s.ss.do_activate = true ;
            new_states.scissor_s.do_change = true ;
            new_states.scissor_s.ss.do_activate = false ;
            new_states.blend_s.do_change = true ;
            new_states.blend_s.ss.do_activate = false ;
            new_states.polygon_s.do_change = true ;
            new_states.polygon_s.ss.do_activate = true ;
            new_states.depth_s.do_change = true ;

            obj.add_render_state_set( new_states ) ;

            size_t const oid = this->construct_state( size_t( -1 ), obj ) ;
        }
    }

    // @todo is the move required? pimpl is only used via
    // pointer movement.
    pimpl( pimpl && rhv ) noexcept
    {
        natus_move_member_ptr( _ctx, rhv ) ;
        geo_datas = std::move( rhv.geo_datas ) ;
        images = std::move( rhv.images ) ;
        shaders = std::move( rhv.shaders ) ;
        arrays = std::move( rhv.arrays ) ;
        render_states = std::move( rhv.render_states ) ;
        renders = std::move( renders ) ;

        _state_stack = std::move( rhv._state_stack ) ;
        state_sets = std::move( rhv.state_sets ) ;
        _cur_fb_active = rhv._cur_fb_active ;
    }
    
    ~pimpl( void_t ) 
    {
        for( auto & g : geo_datas ) g.invalidate() ;
        geo_datas.clear() ;

        for( auto & s : shaders ) s.invalidate() ;
        shaders.clear() ;

        for( auto & r : renders ) r.invalidate() ;
        renders.clear() ;

        for( auto & i : images ) i.invalidate() ;
        images.clear() ;

        for( auto & a : arrays ) a.invalidate() ;
        arrays.clear() ;

        for( auto & f : framebuffers ) f.invalidate() ;
        framebuffers.clear() ;

        //for( auto & s : state_sets ) s.invalidate() ;
        state_sets.clear() ;

        _cur_fb_active = size_t( -1 ) ;
    }

    size_t construct_state( size_t oid, natus::graphics::state_object_ref_t obj ) noexcept
    {
        oid = determine_oid( obj.name(), state_sets ) ;

        auto& states = state_sets[ oid ] ;

        states.states.resize( obj.size() ) ;

        // @note
        // set all enables. Enable/Disable only possible during construction
        // values are assigned in the update function for the render states
        obj.for_each( [&] ( size_t const i, natus::graphics::render_state_sets_cref_t rs )
        {
            states.states[ i ] = rs ;
        } ) ;

        return oid ;
    }
    void_t release_state( size_t const oid ) noexcept 
    {
    }

    void_t handle_render_state( this_t::render_state_sets & new_states, bool_t const popped )
    {
        //  viewport
        if( new_states.rss.view_s.do_change )
        {
            if( new_states.rss.view_s.ss.do_activate )
            {
                auto const& vp_ = new_states.rss.view_s.ss.vp  ;

                // Setup the viewport
                D3D11_VIEWPORT vp ;
                vp.Width = FLOAT( vp_.z() ) ;
                vp.Height = FLOAT( vp_.w() ) ;
                vp.MinDepth = 0.0f ;
                vp.MaxDepth = 1.0f ;
                vp.TopLeftX = FLOAT( vp_.x() ) ;
                vp.TopLeftY = FLOAT( vp_.y() ) ;
                
                // this solution here only works because
                // - framebuffers do not stack. They must be used and unused.
                // - render states targeting a framebuffer must be activated 
                //   within the use/unuse functions by the user.
                if( _cur_fb_active == size_t(-1) || popped ) 
                    vp.TopLeftY = vp_height - vp.Height - vp.TopLeftY ;
                else 
                    vp.TopLeftY = framebuffers[_cur_fb_active].height - vp.Height - vp.TopLeftY ;
                

                _ctx->ctx()->RSSetViewports( 1, &vp ) ;
            }
        }

        if( new_states.rss.clear_s.do_change && new_states.rss.clear_s.ss.do_activate && !popped )
        {
            if( _cur_fb_active == size_t(-1) )
            {
                bool_t const clear_color = new_states.rss.clear_s.ss.do_color_clear ;
                bool_t const clear_depth = new_states.rss.clear_s.ss.do_depth_clear ;

                if( clear_color )
                {
                    natus::math::vec4f_t const color = new_states.rss.clear_s.ss.clear_color ;
                    _ctx->clear_render_target_view( color ) ;
                }

                if( clear_depth )
                {
                    _ctx->clear_depth_stencil_view() ;
                }
            }
            else 
            {
                framebuffer_data_ref_t fb = framebuffer_data_ref_t( framebuffers[ _cur_fb_active ] ) ;

                bool_t const clear_color = new_states.rss.clear_s.ss.do_color_clear ;
                bool_t const clear_depth = new_states.rss.clear_s.ss.do_depth_clear ;

                if( clear_color )
                {
                    natus::math::vec4f_t const color = new_states.rss.clear_s.ss.clear_color ;
                    FLOAT const dxcolor[ 4 ] = { color.x(), color.y(), color.z(), color.w() } ;
                    for( size_t i=0; i<fb.num_color; ++i )
                        _ctx->ctx()->ClearRenderTargetView( fb.rt_view[ i ], dxcolor ) ;
                }

                if( clear_depth && fb.ds_view != nullptr )
                {
                    _ctx->ctx()->ClearDepthStencilView( fb.ds_view, D3D11_CLEAR_DEPTH, 1.0f, 0 ) ;
                }
            }
        }

        if( new_states.rss.depth_s.do_change )
        {
            D3D11_DEPTH_STENCIL_DESC desc = { } ;
            desc.DepthEnable = new_states.rss.depth_s.ss.do_activate ? TRUE : FALSE ;
            desc.DepthFunc = D3D11_COMPARISON_LESS ;
            desc.DepthWriteMask = new_states.rss.depth_s.ss.do_depth_write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO ;

            auto const res = _ctx->dev()->CreateDepthStencilState( &desc, &new_states.depth_stencil_state ) ;
            if( FAILED( res ) )
            {
                natus::log::global_t::error( "CreateDepthStencilState" ) ;
            }

            _ctx->ctx()->OMSetDepthStencilState( new_states.depth_stencil_state, 0 ) ;
        }

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

            if( new_states.rss.blend_s.do_change )
            {
                desc.RenderTarget[ 0 ].BlendEnable = new_states.rss.blend_s.ss.do_activate ;

                desc.RenderTarget[ 0 ].SrcBlend = natus::graphics::d3d11::convert( 
                    new_states.rss.blend_s.ss.src_blend_factor ) ;
                desc.RenderTarget[ 0 ].DestBlend = natus::graphics::d3d11::convert( 
                    new_states.rss.blend_s.ss.dst_blend_factor ) ;
                desc.RenderTarget[ 0 ].BlendOp = natus::graphics::d3d11::convert( 
                    new_states.rss.blend_s.ss.blend_func ) ;

                desc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_ONE ;
                desc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_ZERO ;
                desc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD ;

                auto const res = _ctx->dev()->CreateBlendState( &desc, &new_states.blend_state ) ;
                if( SUCCEEDED( res ) )
                {
                    _ctx->ctx()->OMSetBlendState( new_states.blend_state, 0, 0xffffffff );
                }
            }
        }
        // rasterizer state conglomerate
        {
            D3D11_RASTERIZER_DESC raster_desc = { } ;

            if( new_states.rss.polygon_s.do_change )
            {
                if( new_states.rss.polygon_s.ss.do_activate )
                    raster_desc.CullMode = natus::graphics::d3d11::convert( new_states.rss.polygon_s.ss.cm ) ;
                else
                    raster_desc.CullMode = D3D11_CULL_NONE ;

                raster_desc.FillMode = natus::graphics::d3d11::convert( new_states.rss.polygon_s.ss.fm ) ;

                raster_desc.FrontCounterClockwise = new_states.rss.polygon_s.ss.ff == natus::graphics::front_face::counter_clock_wise ;
            }

            if( new_states.rss.scissor_s.do_change )
            {
                raster_desc.ScissorEnable = new_states.rss.scissor_s.ss.do_activate ;
                if( new_states.rss.scissor_s.ss.do_activate )
                {
                    LONG const h = _state_stack.top().rss.view_s.ss.vp.w() ;

                    D3D11_RECT rect ;
                    rect.left = new_states.rss.scissor_s.ss.rect.x() ;
                    rect.right = new_states.rss.scissor_s.ss.rect.x() + new_states.rss.scissor_s.ss.rect.z() ;
                    rect.top = h - (new_states.rss.scissor_s.ss.rect.y()+new_states.rss.scissor_s.ss.rect.w()) ;
                    rect.bottom = rect.top + new_states.rss.scissor_s.ss.rect.w() ;

                    _ctx->ctx()->RSSetScissorRects( 1, &rect ) ;
                }
            }
            auto const res = _ctx->dev()->CreateRasterizerState( &raster_desc, &new_states.raster_state ) ;
            if( FAILED( res ) )
            {
                natus::log::global_t::error( "CreateRasterizerState" ) ;
            }

            _ctx->ctx()->RSSetState( new_states.raster_state ) ;
        }
        
    }

    // if oid == -1, the state is popped.
    void_t handle_render_state( size_t const oid, size_t const rs_id ) noexcept
    {
        auto new_id = std::make_pair( oid, rs_id ) ;

        // pop state
        if( oid == size_t( -1 ) )
        {
            if( _state_stack.size() == 1 )
            {
                natus::log::global_t::error( natus_log_fn( "no more render states to pop" ) ) ;
                return ;
            }
            auto old = _state_stack.pop() ;
            #if 0
            if( old.depth_stencil_state != nullptr )
                old.depth_stencil_state->Release() ;
            if( old.raster_state != nullptr )
                old.raster_state->Release() ;
            if( old.blend_state != nullptr )
                old.blend_state->Release() ;
            #endif

            // undo render state effects 
            {
                old.rss = old.rss - _state_stack.top().rss ;
                this->handle_render_state( old, true ) ;
            }

        }
        else
        {
            this_t::render_state_sets rss ;
            rss.rss = _state_stack.top().rss + state_sets[ new_id.first ].states[ new_id.second ] ;
            
            this->handle_render_state( rss, false ) ;
            _state_stack.push( rss ) ;
        }
    }

    size_t construct_framebuffer( size_t oid, natus::graphics::framebuffer_object_ref_t obj ) noexcept
    {
        oid = determine_oid( obj.name(), framebuffers ) ;
        auto & fb = framebuffers[ oid ] ;

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
                        natus::log::global_t::error( natus_log_fn( "CreateRenderTargetView" ) ) ;
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
                        images[ iid ].requires_y_flip = 1.0f ;
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
        {
            if( fb.ds_view != nullptr )
            {
                fb.ds_view->Release() ;
                fb.ds_view = nullptr ;
            }

            // release images
            if( fb.image_ids[ fb.num_color ] != size_t( -1 ) )
            {
                images[ fb.image_ids[ fb.num_color ] ].view->Release() ;
                images[ fb.image_ids[ fb.num_color ] ].texture->Release() ;
                images[ fb.image_ids[ fb.num_color ] ].sampler->Release() ;

                images[ fb.image_ids[ fb.num_color ] ].view = nullptr  ;
                images[ fb.image_ids[ fb.num_color ] ].texture = nullptr ;
                images[ fb.image_ids[ fb.num_color ] ].sampler = nullptr ;
            }
            
            auto const dst = obj.get_depth_target();
            
            if( dst != natus::graphics::depth_stencil_target_type::unknown )
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
                    sampDesc.MinLOD = 0 ;
                    sampDesc.MaxLOD = D3D11_FLOAT32_MAX ;

                    auto const hr = dev->CreateSamplerState( &sampDesc, smp ) ;
                    if( FAILED( hr ) )
                    {
                        natus::log::global_t::error( natus_log_fn( "CreateSamplerState" ) ) ;
                        return oid ;
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
                    desc.Format = natus::graphics::d3d11::convert_texture( dst ) ;
                    desc.SampleDesc.Count = 1 ;
                    desc.SampleDesc.Quality = 0 ;
                    desc.Usage = D3D11_USAGE_DEFAULT ;
                    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ;
                    desc.CPUAccessFlags = 0 ;
                    desc.MiscFlags = 0 ;

                    // create the texture object
                    {
                        auto const hr = dev->CreateTexture2D( &desc, nullptr, tex ) ;
                        if( FAILED( hr ) )
                        {
                            natus::log::global_t::error( natus_log_fn( "CreateTexture2D" ) ) ;
                            return oid ;
                        }
                    }
                }

                // shader resource view
                guard< ID3D11ShaderResourceView > srv ;
                {
                    D3D11_SHADER_RESOURCE_VIEW_DESC res_desc = { } ;
                    res_desc.Format = natus::graphics::d3d11::convert_shader_resource( dst ) ;
                    res_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D ;
                    res_desc.Texture2D.MostDetailedMip = 0 ;
                    res_desc.Texture2D.MipLevels = UINT( 1 ) ;

                    auto const hr = dev->CreateShaderResourceView( tex, &res_desc, srv ) ;
                    if( FAILED( hr ) )
                    {
                        natus::log::global_t::error( natus_log_fn( "CreateShaderResourceView for texture : [" +
                            obj.name() + "]" ) ) ;
                        return oid ;
                    }
                }

                // render target view
                guard< ID3D11DepthStencilView > view ;
                {
                    D3D11_DEPTH_STENCIL_VIEW_DESC desc = { } ;
                    desc.Format = natus::graphics::d3d11::convert_depth_stencil_view( dst ) ;
                    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D ;
                    desc.Texture2D.MipSlice = 0 ;

                    auto const res = _ctx->dev()->CreateDepthStencilView( tex, &desc, view ) ;
                    if( FAILED( res ) )
                    {
                        natus::log::global_t::error( natus_log_fn( "CreateDepthStencilView" ) ) ;
                        return oid ;
                    }
                }

                // store data
                {
                    size_t const i = obj.get_num_color_targets() ;
                    size_t const iid = fb.image_ids[ i ] == size_t( -1 ) ?
                        determine_oid( obj.name() + ".depth", images ) : fb.image_ids[ i ] ;

                    // fill the image so shader variable
                    // lookup can find the render target
                    // for binding to a texture variable
                    {
                        fb.image_ids[ i ] = iid ;
                        images[ iid ].name = obj.name() + ".depth" ;
                        images[ iid ].sampler = smp.move_out() ;
                        images[ iid ].texture = tex.move_out() ;
                        images[ iid ].valid = true ;
                        images[ iid ].view = srv.move_out() ;
                        images[ iid ].requires_y_flip = 1.0f ;
                    }

                    {
                        auto const dims = obj.get_dims() ;
                        fb.width = FLOAT( dims.x() ) ;
                        fb.height = FLOAT( dims.y() ) ;
                    }

                    fb.ds_view = view.move_out() ;
                }
            }
        }

        return oid ;
    }
    void_t release_framebuffer( size_t const oid ) noexcept 
    {
        auto & fb = framebuffers[ oid ] ;
        fb.invalidate() ;
    }

    bool_t activate_framebuffer( size_t const oid ) noexcept
    {
        framebuffer_data_ref_t fb = framebuffer_data_ref_t( framebuffers[ oid ] ) ;

        _ctx->ctx()->OMSetRenderTargets( UINT( fb.num_color ), fb.rt_view, fb.ds_view ) ;

        _cur_fb_active = oid ;

        return true ;
    }

    void_t deactivate_framebuffer( void_t )
    {
        _ctx->activate_framebuffer() ;
        
        // done by the render states

        #if 0
        // Setup the viewport
        D3D11_VIEWPORT vp ;
        vp.Width = vp_width ;
        vp.Height = vp_height ;
        vp.MinDepth = 0.0f ;
        vp.MaxDepth = 1.0f ;
        vp.TopLeftX = FLOAT( 0 ) ;
        vp.TopLeftY = FLOAT( 0 ) ;
        _ctx->ctx()->RSSetViewports( 1, &vp );
        #endif

        _cur_fb_active = size_t( -1 ) ;

    }

    size_t construct_geo( size_t oid, natus::graphics::geometry_object_ref_t obj )
    {
        oid = this_t::determine_oid( obj.name(), renders ) ;

        auto & config = geo_datas[ oid ] ;
        config.valid = true ;
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

                HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, config.vb ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_BIND_VERTEX_BUFFER" ) ) ;
                }
            }
            else
            {
                HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, nullptr, config.vb ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_BIND_VERTEX_BUFFER" ) ) ;
                }
            }
        }

        // index buffer object
        if( obj.index_buffer().get_num_elements() > 0 )
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

                HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, config.ib ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_BIND_INDEX_BUFFER" ) ) ;
                }
            }
            else
            {
                HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, nullptr, config.ib ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "D3D11_BIND_INDEX_BUFFER" ) ) ;
                }
            }
        }

        return oid ;
    }
    void_t release_geometry( size_t const oid ) noexcept 
    {
        for( auto & r : renders )
        {
            if( r.geo_id == oid ) r.geo_id = size_t( -1 ) ;
        }

        auto & o = geo_datas[ oid ] ;
        o.invalidate() ;
    }

    bool_t update( size_t const id, natus::graphics::geometry_object_res_t geo )
    {
        auto& config = geo_datas[ id ] ;
        if( !config.valid ) return false ;

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
                    auto const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, config.vb ) ;
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
        if( config.ib != nullptr )
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
                    auto const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, config.ib ) ;
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
                    shd.vs_textures.clear() ;
                    shd.vs_buffers.clear() ;
                    shd.vs_cbuffers = this_t::determine_cbuffer( pVSBlob ) ;
                    this_t::determine_texture( pVSBlob, shd.vs_textures, shd.vs_buffers ) ;
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
                    shd.ps_textures.clear() ;
                    shd.ps_buffers.clear() ;
                    shd.ps_cbuffers = this_t::determine_cbuffer( pPSBlob ) ;
                    this_t::determine_texture( pPSBlob, shd.ps_textures, shd.ps_buffers ) ;
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
                pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, shd.vs ) ;
            
            natus::log::global_t::error( FAILED( hr ), "[d3d11] : CreateVertexShader" ) ;

            // the vs blob is required for later layout creation
            if( shd.vs_blob != nullptr ) shd.vs_blob->Release() ;
            shd.vs_blob = pVSBlob ;
        }

        // geometry shader
        if( pGSBlob != nullptr )
        {
            auto const hr = _ctx->dev()->CreateGeometryShader(
                pGSBlob->GetBufferPointer(), pGSBlob->GetBufferSize(), nullptr, shd.gs ) ;
                
            natus::log::global_t::error( FAILED( hr ), "[d3d11] : CreateGeometryShader" ) ;
            pGSBlob->Release();
            pGSBlob = nullptr ;
        }

        // pixel shader
        if( pPSBlob != nullptr )
        {
            auto const hr = _ctx->dev()->CreatePixelShader(
                pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, shd.ps ) ;

            natus::log::global_t::error( FAILED( hr ), "[d3d11] : CreatePixelShader" ) ;
            pPSBlob->Release();
            pPSBlob = nullptr ;
        }

        // shader input variables
        // the final layout elements for the d3d input layout
        // is done if the render configuration is known.
        {
            shd.vertex_inputs.clear() ;
            obj.for_each_vertex_input_binding( [&] (
                natus::graphics::vertex_attribute const va, natus::ntd::string_cref_t name )
            {
                shd.vertex_inputs.emplace_back( this_t::shader_data::vertex_input_binding
                    { va, name } ) ;
            } ) ;
        }

        return oid ;
    }
    void_t release_shader_data( size_t const oid ) noexcept 
    {
        for( auto & r : renders )
        {
            if( r.shd_id == oid ) r.shd_id = size_t( -1 ) ;
        }
        
        auto & o = shaders[ oid ] ;
        o.invalidate() ;

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
    void_t release_render_data( size_t const oid ) noexcept 
    {
        auto & o = renders[ oid ] ;
        o.invalidate() ;
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

        {
            rd.var_sets_imgs_ps.clear() ;
            rd.var_sets_buffers_vs.clear() ;
            rd.var_sets_buffers_ps.clear() ;
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

                    // set y flip for the current texture in the current variable set
                    {
                        auto * var = vs->data_variable< float_t >( "sys_flipv_" + t.name ) ;
                        var->set( images[i].requires_y_flip ) ;
                    }
                }
                rd.var_sets_imgs_ps.emplace_back( std::make_pair( vs, std::move( ivs ) ) ) ;
            } ) ;
        }

        // array variable mapping : vs
        {
            this_t::shader_data_ref_t shd = shaders[ rd.shd_id ] ;
            rc.for_each( [&] ( size_t const /*i*/, natus::graphics::variable_set_res_t vs )
            {
                this_t::render_data_t::buffer_variables_t bvs ;
                for( auto& t : shd.vs_buffers )
                {
                    auto * dv = vs->array_variable( t.name ) ;
                    natus::ntd::string_t const name = dv->get() ;

                    size_t i = 0 ;
                    for( ; i < arrays.size(); ++i ) if( arrays[ i ].name == name ) break ;

                    if( i == arrays.size() ) continue ;
                    
                    this_t::render_data_t::buffer_variable_t bv ;
                    bv.id = i ;
                    bv.name = t.name ;
                    bv.slot = t.slot ;
                    bvs.emplace_back( std::move( bv ) ) ;
                }
                rd.var_sets_buffers_vs.emplace_back( std::make_pair( vs, std::move( bvs ) ) ) ;
            } ) ;
        }

        // array variable mapping : ps
        {
            this_t::shader_data_ref_t shd = shaders[ rd.shd_id ] ;
            rc.for_each( [&] ( size_t const /*i*/, natus::graphics::variable_set_res_t vs )
            {
                this_t::render_data_t::buffer_variables_t bvs ;
                for( auto& t : shd.ps_buffers )
                {
                    auto * dv = vs->array_variable( t.name ) ;
                    natus::ntd::string_t const name = dv->get() ;

                    size_t i = 0 ;
                    for( ; i < arrays.size(); ++i ) if( arrays[ i ].name == name ) break ;

                    if( i == arrays.size() ) continue ;
                    
                    this_t::render_data_t::buffer_variable_t bv ;
                    bv.id = i ;
                    bv.name = t.name ;
                    bv.slot = t.slot ;
                    bvs.emplace_back( std::move( bv ) ) ;
                }
                rd.var_sets_buffers_ps.emplace_back( std::make_pair( vs, std::move( bvs ) ) ) ;
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

            auto const hr = dev->CreateSamplerState( &sampDesc, img.sampler );
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
            size_t const depth = iref.get_dims().z() ;

            D3D11_TEXTURE2D_DESC desc = { } ;
            desc.Width = static_cast< UINT >( width ) ;
            desc.Height = static_cast< UINT >( height ) ;
            desc.ArraySize = static_cast< UINT >( depth ) ;
            desc.MipLevels = static_cast< UINT >( 1 ) ;
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

            // create the texture object
            {
                natus::memory::malloc_guard< D3D11_SUBRESOURCE_DATA > init_datas( depth ) ;
                
                for( size_t i=0; i<depth; ++i )
                {
                    init_datas[i].pSysMem = iref.get_image_ptr(i) ;
                    init_datas[i].SysMemPitch = UINT( width * bpp ) ;
                    init_datas[i].SysMemSlicePitch = UINT( width * height * bpp ) ;
                }

                auto const hr = dev->CreateTexture2D( &desc, init_datas.get(), img.texture ) ;
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
                if( obj.get_type() == natus::graphics::texture_type::texture_2d )
                {
                    res_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D ;
                    res_desc.Texture2D.MostDetailedMip = 0 ;
                    res_desc.Texture2D.MipLevels = UINT(1) ;
                }
                else if( obj.get_type() == natus::graphics::texture_type::texture_2d_array )
                {
                    res_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY ;
                    res_desc.Texture2DArray.ArraySize = UINT( depth ) ;
                    res_desc.Texture2DArray.MostDetailedMip = 0 ;
                    res_desc.Texture2DArray.MipLevels = UINT(1) ;
                    res_desc.Texture2DArray.FirstArraySlice = UINT(0) ;
                }

                auto const hr = dev->CreateShaderResourceView( img.texture, &res_desc, img.view ) ;
                if( FAILED( hr ) )
                {
                    natus::log::global_t::error( natus_log_fn( "CreateShaderResourceView for texture : [" + img.name + "]" ) ) ;
                }
            }
        }

        return oid ;
    }
    void_t release_image_data( size_t const oid ) noexcept 
    {
        auto & o = images[ oid ] ;
        o.invalidate() ;
    }

    size_t construct_array_data( size_t oid, natus::graphics::array_object_ref_t obj ) 
    {
        oid = this_t::determine_oid( obj.name(), arrays ) ;

        // only vec4 float allowed
        {
            bool_t valid = true ;
            obj.data_buffer().for_each_layout_element( [&]( natus::graphics::data_buffer::layout_element_cref_t le )
            {
                if( le.type != natus::graphics::type::tfloat &&
                    le.type_struct != natus::graphics::type_struct::vec4 ) valid = false ;
            } ) ;

            if( !valid )
            {
                natus::log::global_t::error("[d3d11] : data buffer must only consist of vec4f layout elements.") ;
                return oid ;
            }
        }

        this_t::array_data_ref_t data = arrays[ oid ] ;
        data.name = obj.name() ;

        ID3D11Device * dev = _ctx->dev() ;
        
        // = number of vertices * sizeof( vertex )
        // if there are vertices already setup, take the numbers
        size_t const byte_width = obj.data_buffer().get_sib() ;

        if( data.buffer != nullptr ) 
        {
            data.buffer->Release() ;
            data.buffer = nullptr ;
        }

        // this release is important for releasing the
        // buffer reference too.
        if( data.view != nullptr ) 
        {
            data.view->Release() ;
            data.view = nullptr ;
        }
            
        // create the buffer
        {
            D3D11_BUFFER_DESC bd = { } ;
            bd.Usage = D3D11_USAGE_DYNAMIC ;
            // using max of 1 sib so empty vbs will create and will 
            // be resized in the update function. Important for now
            // is that the buffer will be NOT nullptr.
            bd.ByteWidth = UINT( std::max( byte_width, size_t( 1 ) ) ) ;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D10_BIND_SHADER_RESOURCE ;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE ;

            HRESULT const hr = dev->CreateBuffer( &bd, nullptr, data.buffer ) ;
            if( FAILED( hr ) )
            {
                natus::log::global_t::error( natus_log_fn( "D3D11_BIND_DATA_BUFFER" ) ) ;
            }
        }
            
        // create the resource view
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC res_desc = { } ;
            res_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT ;
            res_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER ;

            // must be 16 in order to have access to all data.
            // this is independent from the number of layout elements.
            // latout elements may only have vec4f inserted!
            UINT const elem_sib = 16 ; // sizeof( vec4f )

            res_desc.Buffer.FirstElement= 0 ;
            res_desc.Buffer.NumElements = UINT( byte_width ) / elem_sib ;

            auto const hr = dev->CreateShaderResourceView( data.buffer, &res_desc, data.view ) ;
            if( FAILED( hr ) )
            {
                natus::log::global_t::error( natus_log_fn( "CreateShaderResourceView for buffer : [" + data.name + "]" ) ) ;
            }
        }

        return oid ;
    }
    void_t release_array_data( size_t const oid ) noexcept 
    {
        auto & o = arrays[ oid ] ;
        o.invalidate() ;
    }

    bool_t update( size_t const id, natus::graphics::array_object_ref_t obj, bool_t const is_config )
    {
        auto & data = arrays[ id ] ;
        
        D3D11_BUFFER_DESC bd ;
        data.buffer->GetDesc( &bd ) ;

        size_t const sib = obj.data_buffer().get_sib() ;
        if( sib > bd.ByteWidth )
        {
            this->construct_array_data( id, obj ) ;
        }

        // copy data
        {
            // exactly the size in bytes of the 1d buffer
            LONG const lsib = LONG( obj.data_buffer().get_layout_sib() ) ;
            LONG const ne = LONG( obj.data_buffer().get_num_elements() ) ;
            void_cptr_t data_ptr = obj.data_buffer().data() ;

            D3D11_MAPPED_SUBRESOURCE resource;
            _ctx->ctx()->Map( data.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource ) ;
            std::memcpy( resource.pData, data_ptr, lsib * ne ) ;
            _ctx->ctx()->Unmap( data.buffer, 0 ) ;
        }

        return true ;
    }

    bool_t update( size_t const id, natus::graphics::render_object_ref_t obj, size_t const varset_id )
    {
        this_t::render_data_ref_t rnd = renders[ id ] ;

        if( !rnd.valid ) return false ;

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

            ctx->UpdateSubresource( cb.ptr, 0, nullptr, cb.mem, 0, 0 ) ;
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

            ctx->UpdateSubresource( cb.ptr, 0, nullptr, cb.mem, 0, 0 ) ;
        }

        return true ;
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

        
        this_t::shader_data_ref_t shd = shaders[ rnd.shd_id ] ;
        this_t::geo_data_ref_t geo = geo_datas[ rnd.geo_id ] ;

        if( shd.vs == nullptr )
        {
            natus::log::global_t::error( natus_log_fn( "shader missing" ) ) ;
            return false ;
        }

        ID3D11DeviceContext * ctx = _ctx->ctx() ;

        // vertex shader variables
        for( auto & cb : rnd.var_sets_data[ varset_id ].second )
        {
            ctx->VSSetConstantBuffers( cb.slot, 1, &cb.ptr ) ;
        }

        // pixel shader variables
        for( auto& cb : rnd.var_sets_data_ps[ varset_id ].second )
        {
            ctx->PSSetConstantBuffers( cb.slot, 1, &cb.ptr ) ;
        }

        ctx->IASetInputLayout( rnd.vertex_layout );

        {
            UINT const stride = geo.stride ;
            UINT const offset = 0 ;
            ctx->IASetVertexBuffers( 0, 1, geo.vb, &stride, &offset );
        }
        
        ctx->IASetPrimitiveTopology( natus::graphics::d3d11::convert( geo.pt ) ) ;

        ctx->VSSetShader( shd.vs, nullptr, 0 ) ;
        ctx->PSSetShader( shd.ps, nullptr, 0 ) ;

        for( auto& img : rnd.var_sets_imgs_ps[ varset_id ].second )
        {
            ctx->PSSetShaderResources( img.slot, 1, images[ img.id ].view ) ;
            ctx->PSSetSamplers( img.slot, 1, images[ img.id ].sampler ) ;
        }

        for( auto& buf : rnd.var_sets_buffers_vs[ varset_id ].second )
        {
            ctx->VSSetShaderResources( buf.slot, 1, arrays[ buf.id ].view ) ;
        }

        for( auto& buf : rnd.var_sets_buffers_ps[ varset_id ].second )
        {
            ctx->PSSetShaderResources( buf.slot, 1, arrays[ buf.id ].view ) ;
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

    void_t begin_frame( void_t )
    {
        // set the viewport to the default new state, 
        // so the correct viewport is set automatically.
        {
            auto const ids_new = std::make_pair( size_t( 0 ), size_t( 0 ) ) ;

            natus::math::vec4ui_t vp = state_sets[ ids_new.first ].states[ ids_new.second ].view_s.ss.vp ;
            vp.z( uint_t( vp_width ) ) ;
            vp.w( uint_t( vp_height ) ) ;
            state_sets[ ids_new.first ].states[ ids_new.second ].view_s.ss.vp = vp ;

            {
                this_t::render_state_sets rss ;
                rss.rss = state_sets[ ids_new.first ].states[ ids_new.second ] ;
                _state_stack.push( rss ) ;
                this->handle_render_state( rss, false ) ;
            }
        }

        {
            natus::math::vec4f_t const color = _state_stack.top().rss.clear_s.ss.clear_color ;

            _ctx->clear_render_target_view( color ) ;
            _ctx->clear_depth_stencil_view() ;
        }
    }

    void_t end_frame( void_t )
    {
        this->handle_render_state( size_t( -1 ), size_t( -1 ) ) ;
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

    static bool_t determine_texture( ID3DBlob * blob,
        shader_data_t::image_variables_out_t img_vars, 
        shader_data_t::buffer_variables_out_t buf_vars ) noexcept
    {
        guard< ID3D11ShaderReflection >  reflector ;
        D3DReflect( blob->GetBufferPointer(), blob->GetBufferSize(),
            IID_ID3D11ShaderReflection, reflector ) ;

        D3D11_SHADER_DESC shd_desc ;
        {
            auto const hr = reflector->GetDesc( &shd_desc ) ;
            if( FAILED( hr ) )
            {
                natus::log::global_t::error( natus_log_fn( "Can not get shader reflection desc" ) ) ;
                return false ;
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
                    return false ;
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
                img_vars.emplace_back( ivar ) ;
            }
            else if( ibd.Type == D3D_SIT_TEXTURE && ibd.Dimension == D3D_SRV_DIMENSION_TEXTURE2DARRAY )
            {
                shader_data_t::image_variable_t ivar ;
                ivar.name = ibd.Name ;
                ivar.slot = ibd.BindPoint ;
                img_vars.emplace_back( ivar ) ;
            }
            else if( ibd.Type == D3D_SIT_TEXTURE && ibd.Dimension == D3D_SRV_DIMENSION_BUFFER )
            {
                shader_data_t::buffer_variable_t ivar ;
                ivar.name = ibd.Name ;
                ivar.slot = ibd.BindPoint ;
                buf_vars.emplace_back( ivar ) ;
            }
            else
            {
                natus::log::global_t::warning( natus_log_fn("Detected texture but type is not supported.") ) ;
            }
        }

        return true ;
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
natus::graphics::result d3d11_backend::configure( natus::graphics::geometry_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;

    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_geo( 
            id->get_oid( this_t::get_bid() ), *obj ) ) ;
    }
    
    {
        auto const res = _pimpl->update( id->get_oid( this_t::get_bid() ), obj ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::configure( natus::graphics::render_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;
    
    {
        id->set_oid( this_t::get_bid(),
            _pimpl->construct_render_config( id->get_oid( this_t::get_bid() ), *obj ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *obj ) ;
        if( natus::core::is_not( res ) )
        {
            return natus::graphics::result::failed ;
        }
    }

    return natus::graphics::result::ok ;
}

//***
natus::graphics::result d3d11_backend::configure( natus::graphics::shader_object_res_t obj ) noexcept
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;
    
    {
        id->set_oid( this_t::get_bid(),
            _pimpl->construct_shader_config( id->get_oid( this_t::get_bid() ), *obj ) ) ;
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
natus::graphics::result d3d11_backend::configure( natus::graphics::image_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;
    
    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_image_config( 
            id->get_oid( this_t::get_bid() ), *obj ) ) ;
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
natus::graphics::result d3d11_backend::configure( natus::graphics::state_object_res_t obj ) noexcept
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

    return natus::graphics::result::ok ;
}

//***
natus::graphics::result d3d11_backend::configure( natus::graphics::array_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;

    {
        id->set_oid( this_t::get_bid(), _pimpl->construct_array_data( 
            id->get_oid( this_t::get_bid() ), *obj ) ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *obj, true ) ;
        if( natus::core::is_not( res ) ) return natus::graphics::result::failed ;
    }

    return natus::graphics::result::ok ;
}

natus::graphics::result d3d11_backend::release( natus::graphics::geometry_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;

    _pimpl->release_geometry( id->get_oid( this_t::get_bid() ) ) ;
    id->set_oid( this_t::get_bid(), size_t( -1 ) ) ;

    return natus::graphics::result::ok ;
}

natus::graphics::result d3d11_backend::release( natus::graphics::render_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;
    _pimpl->release_render_data( obj->get_id()->get_oid( this_t::get_bid() ) ) ;
    id->set_oid( this_t::get_bid(), size_t( -1 ) ) ;

    return natus::graphics::result::ok ;
}

natus::graphics::result d3d11_backend::release( natus::graphics::shader_object_res_t obj ) noexcept
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;
    _pimpl->release_shader_data( obj->get_id()->get_oid( this_t::get_bid() ) ) ;
    id->set_oid( this_t::get_bid(), size_t( -1 ) ) ;

    return natus::graphics::result::ok ;
}

natus::graphics::result d3d11_backend::release( natus::graphics::image_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;
    _pimpl->release_image_data( obj->get_id()->get_oid( this_t::get_bid() ) ) ;
    id->set_oid( this_t::get_bid(), size_t( -1 ) ) ;

    return natus::graphics::result::ok ;
}

natus::graphics::result d3d11_backend::release( natus::graphics::framebuffer_object_res_t obj ) noexcept 
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;
    _pimpl->release_framebuffer( obj->get_id()->get_oid( this_t::get_bid() ) ) ;
    id->set_oid( this_t::get_bid(), size_t( -1 ) ) ;

    return natus::graphics::result::ok ;
}

natus::graphics::result d3d11_backend::release( natus::graphics::state_object_res_t obj ) noexcept
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;
    _pimpl->release_state( obj->get_id()->get_oid( this_t::get_bid() ) ) ;
    id->set_oid( this_t::get_bid(), size_t( -1 ) ) ;

    return natus::graphics::result::ok ;
}

natus::graphics::result d3d11_backend::release( natus::graphics::array_object_res_t obj ) noexcept
{
    if( !obj.is_valid() || obj->name().empty() )
    {
        natus::log::global_t::error( natus_log_fn( "Object must be valid and requires a name" ) ) ;
        return natus::graphics::result::invalid_argument ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;
    _pimpl->release_array_data( obj->get_id()->get_oid( this_t::get_bid() ) ) ;
    id->set_oid( this_t::get_bid(), size_t( -1 ) ) ;

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
natus::graphics::result d3d11_backend::update( natus::graphics::array_object_res_t obj ) noexcept 
{
    natus::graphics::id_res_t id = obj->get_id() ;
    size_t const oid = id->get_oid( this_t::get_bid() ) ;

    {
        auto const res = _pimpl->update( oid, *obj, false ) ;
        if( natus::core::is_not( res ) ) return natus::graphics::result::failed ;
    }

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::update( natus::graphics::image_object_res_t ) noexcept 
{
    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::update( natus::graphics::render_object_res_t obj, size_t const varset ) noexcept 
{
    natus::graphics::id_res_t id = obj->get_id() ;
    
    if( id->is_not_valid( this_t::get_bid() ) )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return natus::graphics::result::failed ;
    }

    _pimpl->update( id->get_oid( this_t::get_bid() ), *obj, varset ) ;
    
    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::use( natus::graphics::framebuffer_object_res_t obj ) noexcept
{
    if( !obj.is_valid() )
    {
        return this_t::unuse( natus::graphics::backend::unuse_type::framebuffer ) ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;

    if( id->is_not_valid( this_t::get_bid() ) )
    {
        return this_t::unuse( natus::graphics::backend::unuse_type::framebuffer ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;
    auto const res = _pimpl->activate_framebuffer( oid ) ;
    if( !res ) return natus::graphics::result::failed ;

    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::unuse( natus::graphics::backend::unuse_type const ) noexcept 
{
    _pimpl->deactivate_framebuffer() ;
    return natus::graphics::result::ok ;
}

//****
natus::graphics::result d3d11_backend::push( natus::graphics::state_object_res_t obj, size_t const sid, bool_t const ) noexcept 
{
    if( !obj.is_valid() )
    {
        return this_t::pop( natus::graphics::backend::pop_type::render_state ) ;
    }

    natus::graphics::id_res_t id = obj->get_id() ;

    if( id->is_not_valid( this_t::get_bid() ) )
    {
        return this_t::pop( natus::graphics::backend::pop_type::render_state ) ;
    }

    size_t const oid = id->get_oid( this_t::get_bid() ) ;
    _pimpl->handle_render_state( oid, sid ) ;

    return natus::graphics::result::ok ;
}

natus::graphics::result d3d11_backend::pop( natus::graphics::backend::pop_type const ) noexcept 
{
    _pimpl->handle_render_state( size_t( -1 ), size_t( -1 ) ) ;
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