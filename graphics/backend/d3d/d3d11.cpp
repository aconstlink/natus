

#include "d3d11.h"
//#include "gl3_convert.h"

#include "../../buffer/vertex_buffer.hpp"
#include "../../buffer/index_buffer.hpp"

#include <natus/memory/global.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/ntd/vector.hpp>
#include <natus/ntd/string.hpp>
#include <natus/ntd/string/split.hpp>

#include <d3d11.h>
#include <d3dcompiler.h>

using namespace natus::graphics ;

struct d3d11_backend::pimpl
{
    natus_this_typedefs( d3d11_backend::pimpl ) ;

    struct geo_data
    {
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

        #if 0
        GLenum ib_type ;
        size_t ib_elem_sib = 0  ;
        GLenum pt ;
        #endif
    };
    natus_typedef( geo_data ) ;

    struct shader_data
    {
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

        #if 0
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
            //GLuint loc ;
            //GLenum type ;
        };
        natus_typedef( attribute_variable ) ;

        natus::ntd::vector< attribute_variable_t > attributes ;
        #endif

        struct uniform_variable
        {
            natus::ntd::string_t name ;
            //GLuint loc ;
            //GLenum type ;

            // this variable's memory location
            void_ptr_t mem = nullptr ;

            #if 0
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
            #endif
        };
        natus_typedef( uniform_variable ) ;

        natus::ntd::vector< uniform_variable > uniforms ;

        void_ptr_t uniform_mem = nullptr ;
    } ;
    natus_typedef( shader_data ) ;

    struct render_data
    {
        natus::ntd::string_t name ;

        geo_data* geo = nullptr ;
        shader_data* shaders_ptr = nullptr ;

        // the layout requires information about the layout elements' type
        // in order to determine the proper d3d enumeration
        D3D11_INPUT_ELEMENT_DESC layout[ size_t( natus::graphics::vertex_attribute::num_attributes ) ] ;
        ID3D11InputLayout* vertex_layout = nullptr ;

        ID3D11RasterizerState* raster_state = nullptr ;

        #if 0
        struct uniform_variable_link
        {
            // the index into the shader_config::uniforms array
            size_t uniform_id ;
            // the user variable holding the data.
            natus::graphics::ivariable_ptr_t var ;
        };

        // user provided variable set
        natus::ntd::vector< ::std::pair<
            natus::graphics::variable_set_res_t,
            natus::ntd::vector< uniform_variable_link > > > var_sets_data ;

        struct uniform_texture_link
        {
            // the index into the shader_config::uniforms array
            size_t uniform_id ;
            GLint tex_id ;
            size_t img_id ;
        };
        natus::ntd::vector< ::std::pair<
            natus::graphics::variable_set_res_t,
            natus::ntd::vector< uniform_texture_link > > > var_sets_texture ;

        #endif

    };
    natus_typedef( render_data ) ;

    natus::graphics::backend_type const bt = natus::graphics::backend_type::d3d11 ;
    natus::graphics::d3d11_context_ptr_t _ctx ;

    typedef natus::ntd::vector< this_t::geo_data > geo_configs_t ;
    geo_configs_t geo_datas ;


    typedef natus::ntd::vector< this_t::shader_data > shader_datas_t ;
    shader_datas_t shaders ;

    typedef natus::ntd::vector< this_t::render_data > render_datas_t ;
    render_datas_t renders ;

    FLOAT vp_width = FLOAT( 0 ) ;
    FLOAT vp_height = FLOAT( 0 ) ;

    pimpl( natus::graphics::d3d11_context_ptr_t ctx ) noexcept
    {
        _ctx = ctx ;
        geo_datas.resize( 10 ) ;
    }

    pimpl( pimpl && rhv ) noexcept
    {
        natus_move_member_ptr( _ctx, rhv ) ;
        geo_datas = std::move( rhv.geo_datas ) ;
    }

    size_t construct_geo( natus::ntd::string_cref_t name, natus::graphics::geometry_object_ref_t geo )
    {
        // the name is unique
        // do not reconstruct if configs already exists.
        // we just reuse the d3d buffers objects and do update the data.
        {
            auto iter = std::find_if( geo_datas.begin(), geo_datas.end(),
                [&] ( this_t::geo_data const& config )
            {
                return config.name == name ;
            } ) ;

            if( iter != geo_datas.end() )
                return iter - geo_datas.begin() ;
        }

        // the name does not exist, so look for some released 
        // config objects.
        size_t i = 0 ;
        for( ; i < geo_datas.size(); ++i )
        {
            if( geo_datas[ i ].name.empty() )
            {
                break ;
            }
        }

        if( i == geo_datas.size() ) {
            geo_datas.emplace_back( this_t::geo_data() ) ;
        }

        auto & config = geo_datas[ i ] ;
        config.name = name ;

        // vertex buffer object
        {
            geo.vertex_buffer().for_each_layout_element(
                [&] ( natus::graphics::vertex_buffer_t::data_cref_t d )
            {
                this_t::geo_data::layout_element le ;
                le.va = d.va ;
                le.type = d.type ;
                le.type_struct = d.type_struct ;
                config.elements.push_back( le ) ;
            } ) ;

            config.stride = UINT( geo.vertex_buffer().get_layout_sib() ) ;

            // = number of vertices * sizeof( vertex )
            // if there are vertices already setup, take the numbers
            size_t const byte_width = geo.vertex_buffer().get_sib() ;

            D3D11_BUFFER_DESC bd = { } ;
            bd.Usage = D3D11_USAGE_DEFAULT ;
            bd.ByteWidth = uint_t( std::max( size_t(1), byte_width ) ) ;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER ;
            bd.CPUAccessFlags = 0 ;

            natus::memory::global_t::dealloc_raw( config.vb_mem ) ;
            config.vb_mem = natus::memory::global_t::alloc_raw< uint8_t >( 
                byte_width, "[d3d11] : vertex buffer memory" ) ;

            D3D11_SUBRESOURCE_DATA init_data = { } ;
            init_data.pSysMem = (void_cptr_t)config.vb_mem ;
            
            std::memcpy( config.vb_mem, geo.vertex_buffer().data(), byte_width ) ;

            HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, &config.vb ) ;
            if( FAILED( hr ) )
            {
                natus::log::global_t::error( natus_log_fn( "D3D11_BIND_VERTEX_BUFFER" ) ) ;
            }
        }

        // index buffer object
        if( geo.index_buffer().get_sib() > 0 )
        {
            // = number of vertices * sizeof( index_t )
            size_t const byte_width = geo.index_buffer().get_sib() ;

            D3D11_BUFFER_DESC bd = { } ;
            bd.Usage = D3D11_USAGE_DEFAULT ;
            bd.ByteWidth = uint_t( byte_width ) ;
            bd.BindFlags = D3D11_BIND_INDEX_BUFFER ;
            bd.CPUAccessFlags = 0 ;

            natus::memory::global_t::dealloc_raw( config.ib_mem ) ;
            config.ib_mem = natus::memory::global_t::alloc_raw< uint8_t >(
                byte_width, "[d3d11] : index buffer memory" ) ;

            D3D11_SUBRESOURCE_DATA init_data = { } ;
            init_data.pSysMem = (void_cptr_t)config.ib_mem ;

            std::memcpy( config.ib_mem, geo.index_buffer().data(), byte_width ) ;

            HRESULT const hr = _ctx->dev()->CreateBuffer( &bd, &init_data, &config.ib ) ;
            if( FAILED( hr ) )
            {
                natus::log::global_t::error( natus_log_fn( "D3D11_BIND_INDEX_BUFFER" ) ) ;
            }
        }

        return i ;
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

                //_ctx->ctx()->UpdateSubresource( config.vb, 0, &box, data, lsib, ne ) ;
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
            }
        }

        return true ;
    }

    size_t construct_shader_config( size_t oid, natus::ntd::string_cref_t name,
        natus::graphics::shader_object_ref_t config )
    {
        //
        // Array Management
        //

        // the name must be unique
        {
            auto iter = ::std::find_if( shaders.begin(), shaders.end(),
                [&] ( this_t::shader_data_cref_t  const& c )
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
                // here the vertex shader is used for validity check.
                // if invalid, that object in the array will be repopulated
                if( shaders[ i ].vs == nullptr )
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
        auto & shd = shaders[ oid ] ;

        {
            shd.name = name ;
        }

        // shader code
        natus::graphics::shader_set_t ss ;
        {
            auto const res = config.shader_set( this->bt, ss ) ;
            if( natus::core::is_not(res) )
            {
                natus::log::global_t::warning( natus_log_fn(
                    "config [" + config.name() + "] has no shaders for " + 
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
                auto const hr = D3DCompile( cptr, sib, name.c_str(), Shader_Macros, nullptr, "VS", "vs_5_0",0,0, &pVSBlob, &errblob );

                if( FAILED( hr ) )
                {
                    natus::log::global_t::warning( natus_log_fn(
                        "vertex shader [" + config.name() + "] failed " +
                        natus::graphics::to_string( this->bt ) ) ) ;

                    if( errblob != nullptr )
                    {
                        char_cptr_t ptr = ( char_cptr_t ) errblob->GetBufferPointer() ;
                        size_t const ssib = ( size_t ) errblob->GetBufferSize() ;
                        natus::ntd::string_t s( ptr, ssib ) ;

                        auto const pos = s.find( config.name() ) ;
                        s = s.substr( pos, s.size() - pos ) ;
                        natus::log::global_t::error( s ) ;
                        errblob->Release() ;
                    }
                    pVSBlob->Release() ;

                    return oid ;
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
                auto const hr = D3DCompile( cptr, sib, name.c_str(), Shader_Macros, nullptr, "GS", "gs_5_0", 0, 0, &pVSBlob, &errblob );

                if( FAILED( hr ) )
                {
                    natus::log::global_t::warning( natus_log_fn(
                        "geometry shader [" + config.name() + "] failed " +
                        natus::graphics::to_string( this->bt ) ) ) ;

                    if( errblob != nullptr )
                    {
                        char_cptr_t ptr = ( char_cptr_t ) errblob->GetBufferPointer() ;
                        size_t const ssib = ( size_t ) errblob->GetBufferSize() ;
                        natus::ntd::string_t s( ptr, ssib ) ;

                        auto const pos = s.find( config.name() ) ;
                        s = s.substr( pos, s.size() - pos ) ;
                        natus::log::global_t::error( s ) ;
                        errblob->Release() ;
                    }
                    pVSBlob->Release() ;

                    return oid ;
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
                auto const hr = D3DCompile( cptr, sib, name.c_str(), Shader_Macros, nullptr, "PS", "ps_5_0", 0, 0, &pPSBlob, &errblob );

                if( FAILED( hr ) )
                {
                    natus::log::global_t::warning( natus_log_fn(
                        "pixel shader [" + config.name() + "] failed " +
                        natus::graphics::to_string( this->bt ) ) ) ;

                    if( errblob != nullptr )
                    {
                        char_cptr_t ptr = ( char_cptr_t ) errblob->GetBufferPointer() ;
                        size_t const ssib = ( size_t ) errblob->GetBufferSize() ;
                        natus::ntd::string_t s( ptr, ssib ) ;

                        auto const pos = s.find( config.name() ) ;
                        s = s.substr( pos, s.size() - pos ) ;
                        natus::log::global_t::error( s ) ;
                        errblob->Release() ;
                    }
                    pPSBlob->Release() ;

                    return oid ;
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
            config.for_each_vertex_input_binding( [&] (
                natus::graphics::vertex_attribute const va, natus::ntd::string_cref_t name )
            {
                shd.vertex_inputs.emplace_back( this_t::shader_data::vertex_input_binding
                    { va, name } ) ;
            } ) ;
        }

        return oid ;
    }

    //***********************
    size_t construct_render_config( size_t oid, natus::ntd::string_cref_t name,
        natus::graphics::render_object_ref_t /*config*/ )
    {
        // the name must be unique
        {
            auto iter = std::find_if( renders.begin(), renders.end(),
                [&] ( this_t::render_data_cref_t c )
            {
                return c.name == name ;
            } ) ;

            if( iter != renders.end() )
            {
                size_t const i = std::distance( renders.begin(), iter ) ;
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
            for( ; i < renders.size(); ++i )
            {
                if( natus::core::is_not( renders[ i ].name.empty() ) )
                {
                    break ;
                }
            }
            oid = i ;
        }

        if( oid >= renders.size() ) {
            renders.resize( oid + 1 ) ;
        }

        this_t::render_data_ref_t rd = renders[ oid ] ;

        {
            rd.name = name ;
        }

        if( rd.vertex_layout != nullptr )
        {
            rd.vertex_layout->Release() ;
            rd.vertex_layout = nullptr ;
        }

        std::memset( rd.layout, 0, ARRAYSIZE( rd.layout ) ) ;

        // raster state
        {
            if( rd.raster_state != nullptr )
            {
                rd.raster_state->Release() ;
            }
            D3D11_RASTERIZER_DESC desc = { } ;
            desc.CullMode = D3D11_CULL_NONE ;
            _ctx->dev()->CreateRasterizerState( &desc, &rd.raster_state ) ;
        }

        return oid ;
    }

    bool_t update( size_t const id, natus::graphics::render_object_ref_t rc )
    {
        auto& rd = renders[ id ] ;
        rd.geo = nullptr ;

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

            rd.geo = &geo_datas[ std::distance( geo_datas.begin(), iter ) ] ;
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

            rd.shaders_ptr = &shaders[ std::distance( shaders.begin(), iter ) ] ;
        }
        
        // for binding attributes, the shader and the geometry is required.
        {
            this_t::shader_data_ref_t shd = *rd.shaders_ptr ;
            for( auto const & b : shd.vertex_inputs )
            {
                char_cptr_t name = b.name.c_str() ;
                UINT const semantic_index = 0 ;
                DXGI_FORMAT fmt = DXGI_FORMAT_R32G32B32_FLOAT ;
                UINT input_slot = 0 ;
                UINT aligned_byte_offset = 0 ;
                D3D11_INPUT_CLASSIFICATION const iclass = D3D11_INPUT_PER_VERTEX_DATA ;
                UINT instance_data_step_rate = 0 ;
                
                rd.layout[ size_t( b.va ) ] = { name, semantic_index, fmt, 
                    input_slot, aligned_byte_offset, iclass, instance_data_step_rate } ;
            }

            // test, we know the layout
            {
                rd.layout[ 0 ] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } ;
                //rd.layout[ 1 ] = { "TEXCOORD0", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(natus::math::vec3f_t), D3D11_INPUT_PER_VERTEX_DATA, 0 } ;
            }

            UINT const num_elements = UINT( shd.vertex_inputs.size() ) ;

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

        #if 0
        {
            rc.for_each( [&] ( size_t const /*i*/, natus::graphics::variable_set_res_t vs )
            {
                auto const res = this_t::connect( id, vs ) ;
                natus::log::global_t::warning( natus::core::is_not( res ),
                    natus_log_fn( "connect" ) ) ;
            } ) ;
        }
        #endif

        return true ;
    }

    bool_t render( size_t const id, size_t const varset_id = size_t( 0 ), UINT const start_element = UINT( 0 ),
        UINT const num_elements = UINT( -1 ) )
    {
        this_t::render_data_cref_t rnd = renders[ id ] ;
        this_t::shader_data_cref_t shd = *rnd.shaders_ptr ;
        this_t::geo_data_ref_t geo = *rnd.geo ;

        UINT const stride = geo.stride ;
        UINT const offset = 0 ;
        ID3D11DeviceContext * ctx = _ctx->ctx() ;

        ctx->RSSetState( rnd.raster_state ) ; 

        ctx->IASetInputLayout( rnd.vertex_layout );
        ctx->IASetVertexBuffers( 0, 1, &geo.vb, &stride, &offset );
        
        ctx->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

        ctx->VSSetShader( shd.vs, nullptr, 0 );
        //ctx->VSSetConstantBuffers( 0, 1, &g_pConstantBuffer );
        ctx->PSSetShader( shd.ps, nullptr, 0 );

        if( geo.ib != nullptr )
        {
            ctx->IASetIndexBuffer( geo.ib, DXGI_FORMAT_R32_UINT, 0 );

            UINT const max_elems = num_elements == UINT( -1 ) ? UINT( geo.num_elements_ib ) : num_elements ;
            ctx->DrawIndexed( max_elems, start_element, 0 ) ;
            
        }
        else
        {
            //ctx->Draw( 3, 0 ) ;
        }
        
        
        
        return true ;
    }


    void_t begin_frame( void_t )
    {

        #if 0
        glClearColor( 0.4f, 0.1f, 0.1f, 1.0f ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glClearColor" ) ) ;

        glClear( GL_COLOR_BUFFER_BIT ) ;
        natus::ogl::error::check_and_log( natus_log_fn( "glClear" ) ) ;

        // reset render states
        this_t::set_render_states( *natus::graphics::backend_t::default_render_states() ) ;

        
        #endif

        // Setup the viewport
        D3D11_VIEWPORT vp ;
        vp.Width = ( FLOAT ) vp_width ;
        vp.Height = ( FLOAT ) vp_height ;
        vp.MinDepth = 0.0f ;
        vp.MaxDepth = 1.0f ;
        vp.TopLeftX = 0 ;
        vp.TopLeftY = 0 ;
        _ctx->ctx()->RSSetViewports( 1, &vp );
    }

    void_t end_frame( void_t )
    {
        #if 0
        glFlush() ;
        glFinish() ;
        #endif
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
} ;

//************************************************************************************************
//
//
//************************************************************************************************

//****
d3d11_backend::d3d11_backend( natus::graphics::d3d11_context_ptr_t ctx ) noexcept : 
    backend( natus::graphics::backend_type::gl3 )
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
    
    if( id->is_not_valid( this_t::get_bid() ) )
    {
        id = natus::graphics::id_t( this_t::get_bid(),
            _pimpl->construct_geo( gconf->name(), *(gconf.get_sptr()) ) ) ;
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
        id = natus::graphics::id_t( this_t::get_bid(),
            _pimpl->construct_render_config( id->get_oid( this_t::get_bid() ), config->name(), *config ) ) ;
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
        id = natus::graphics::id_t( this_t::get_bid(),
            _pimpl->construct_shader_config( id->get_oid( this_t::get_bid() ), config->name(), *config ) ) ;
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

    #if 0
    {
        id = natus::graphics::id_t( this_t::get_bid(), _pimpl->construct_image_config( 
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
    #endif
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
natus::graphics::result d3d11_backend::render( natus::graphics::render_object_res_t config, natus::graphics::backend::render_detail_cref_t detail ) noexcept 
{ 
    natus::graphics::id_res_t id = config->get_id() ;

    //natus::log::global_t::status( natus_log_fn("render") ) ;
    
    if( id->is_not_bid( this_t::get_bid() ) || id->is_not_valid() )
    {
        natus::log::global_t::error( natus_log_fn( "invalid id" ) ) ;
        return natus::graphics::result::failed ;
    }

    if( detail.render_states.is_valid() )
    {
        //_pimpl->set_render_states( *( detail.render_states ) ) ;
    }

    _pimpl->render( id->get_oid(), detail.varset, (UINT)detail.start, (UINT)detail.num_elems ) ;
    
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