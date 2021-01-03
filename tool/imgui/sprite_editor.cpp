
#include "sprite_editor.h"

#include <natus/format/future_items.hpp>
#include <natus/format/global.h>

#include <natus/math/vector/vector2.hpp>

using namespace natus::tool ;

sprite_editor::sprite_editor( void_t ) noexcept 
{
}

sprite_editor::sprite_editor( natus::io::database_res_t db ) noexcept 
{
    _db = db ;
}

sprite_editor::sprite_editor( this_rref_t rhv ) noexcept 
{
    _db = std::move( rhv._db ) ;
    _sprite_sheets = std::move( rhv._sprite_sheets ) ;
    _loads = std::move( rhv._loads ) ;
}

sprite_editor::~sprite_editor( void_t ) noexcept 
{
}

sprite_editor::this_ref_t sprite_editor::operator = ( this_rref_t rhv ) noexcept 
{
    _db = std::move( rhv._db ) ;
    _sprite_sheets = std::move( rhv._sprite_sheets ) ;
    _loads = std::move( rhv._loads ) ;

    return *this ;
}

// ****
void_t sprite_editor::add_sprite_sheet( natus::ntd::string_cref_t name, 
                natus::io::location_cref_t loc ) noexcept 
{
    this_t::load_item_t ss ;
    ss.disp_name = name ;
    ss.name = "natus.tool.sprite_editor." + name ;
    ss.loc = loc ;

    natus::format::module_registry_res_t mod_reg = natus::format::global_t::registry() ;
    ss.fitem = mod_reg->import_from( loc, _db ) ;

    _loads.emplace_back( std::move( ss ) ) ;
}

// ****
void_t sprite_editor::render( natus::tool::imgui_view_t imgui ) noexcept 
{
    {
        bool_t show_demo = true ;
        ImGui::ShowDemoWindow( &show_demo ) ;
    }

    // checking future items
    {
        auto loads = std::move(_loads) ;
        for( auto & item : loads )
        {
            auto const res = item.fitem.wait_for( std::chrono::milliseconds(0) ) ;
            if( res != std::future_status::ready )
            {
                _loads.emplace_back( std::move( item ) ) ;
                continue ;
            }

            natus::format::image_item_res_t ii = item.fitem.get() ;
            if( ii.is_valid() )
            {
                natus::graphics::image_t img = *ii->img ;
                
                this_t::sprite_sheet_t ss ;
                ss.dname = item.disp_name ;
                ss.name = item.name ;
                ss.img_loc = item.loc ;
                ss.dims = natus::math::vec2ui_t( img.get_dims() ) ;

                ss.img = natus::graphics::image_object_t( ss.name, std::move( img ) )
                    .set_wrap( natus::graphics::texture_wrap_mode::wrap_s, natus::graphics::texture_wrap_type::clamp_border )
                    .set_wrap( natus::graphics::texture_wrap_mode::wrap_t, natus::graphics::texture_wrap_type::clamp_border )
                    .set_filter( natus::graphics::texture_filter_mode::min_filter, natus::graphics::texture_filter_type::nearest )
                    .set_filter( natus::graphics::texture_filter_mode::mag_filter, natus::graphics::texture_filter_type::nearest );

                ss.region = natus::math::vec4f_t(ss.dims,ss.dims) * natus::math::vec4f_t( 0.5f ) * natus::math::vec4f_t( -1.0f, -1.0f, 1.0f, 1.0f ) ;

                imgui.async().configure( ss.img ) ;

                _sprite_sheets.emplace_back( std::move( ss ) ) ;
            }
        }
    }

    ImGui::Begin( "Sprite Editor" ) ;

    // list box for sprite sheet selection
    {
        ImGui::BeginGroup() ;
        
        ImGui::SetNextItemWidth( ImGui::GetWindowWidth() * 0.3f ) ;
        ImGui::LabelText( "", "Sprite Sheets" ) ;

        natus::ntd::vector< const char * > names( _sprite_sheets.size() ) ;
        for( size_t i=0; i<_sprite_sheets.size(); ++i ) names[i] = _sprite_sheets[i].dname.c_str() ;
        
        ImGui::SetNextItemWidth( ImGui::GetWindowWidth() * 0.3f ) ;
        if( ImGui::ListBox( "", &_cur_item, names.data(), names.size() ) )
        {
            //origin = natus::math::vec2f_t() ;
            //zoom = -0.5f ;
        }
        ImGui::EndGroup() ;
    }

    
    ImGui::SameLine() ; 

    // tabs
    {
        ImGui::BeginGroup() ;

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            if( ImGui::BeginTabItem("Animation") )
            {
                this_t::handle_mouse( imgui, _cur_item ) ;
                this_t::show_image( imgui, _cur_item ) ;
                ImGui::EndTabItem() ;
            }

            if( ImGui::BeginTabItem("Damage") )
            {
                this_t::handle_mouse( imgui, _cur_item ) ;
                this_t::show_image( imgui, _cur_item ) ;
                ImGui::EndTabItem() ;
            }

            if( ImGui::BeginTabItem("Hit") )
            {
                //this_t::handle_mouse( imgui, _cur_item ) ;
                //this_t::show_image( imgui, _cur_item ) ;
                ImGui::EndTabItem() ;
            }

            ImGui::EndTabBar() ;
        }

        ImGui::EndGroup() ;
    }

    // the image
    if( _sprite_sheets.size() != 0 )
    {
        

        

        
    }

    ImGui::End() ;
}

void_t sprite_editor::handle_mouse( natus::tool::imgui_view_t imgui, int_t const selected ) 
{
    ImGuiIO& io = ImGui::GetIO() ;
    auto & ss = _sprite_sheets[_cur_item] ;
        
    natus::math::vec2f_t const crdims = natus::math::vec2f_t( 
        ImGui::GetContentRegionAvail().x, 
        ImGui::GetContentRegionAvail().y ) * natus::math::vec2f_t( 0.99f, 0.99f ) ;

    natus::math::vec2f_t const idims( ss.dims ) ;

    // compute initial zoom
    // so the image exactly fits the content region
    if( ss.zoom < 0.0f )
    {
        auto const tmp = idims / crdims ;
        ss.zoom = std::max( tmp.x(), tmp.y() ) * 0.5f ;            
    }

    // do mouse click zoom prototype
    if( ImGui::IsWindowHovered() )
    {
        natus::math::vec2f_t const mouse_in_window(
            io.MousePos.x - ImGui::GetCursorScreenPos().x,
            io.MousePos.y - ImGui::GetCursorScreenPos().y ) ;

        natus::math::vec2f_t const mouse_centered = 
                mouse_in_window - crdims * natus::math::vec2f_t( 0.5f ) ;

        auto cur_mouse = ss.origin + mouse_centered * ss.zoom * 2.0f ;
            
        // keep the new origin in the image area
        {
            cur_mouse = cur_mouse.max_ed( idims * natus::math::vec2f_t( -0.5f ) ) ;
            cur_mouse = cur_mouse.min_ed( idims * natus::math::vec2f_t( +0.5f ) ) ;
        }

        bool_t const in_cr = 
            mouse_in_window.greater_than( natus::math::vec2f_t() ).all() &&
            mouse_in_window.less_than( crdims ).all() ;

        if( (io.MouseWheel > 0.0f) && in_cr )
        {
            ss.origin = cur_mouse ;
            ss.zoom *= io.KeyCtrl ? 0.5f : 0.9f ;
            ss.zoom = std::max( ss.zoom, 1.0f/200.0f ) ;
        }
        else if( (io.MouseWheel < 0.0f) && in_cr )
        {
            ss.origin = cur_mouse ;
            ss.zoom *= io.KeyCtrl ? 2.0f : 1.1f ;

            // recompute the initial zoom(izoom) 
            auto const tmp = idims / crdims ;
            float_t const izoom = std::max( tmp.x(), tmp.y() ) * 0.5f ;
                
            // allow out-zooming by only izoom * 2.0f
            // so it is not going too far out
            ss.zoom = std::min( ss.zoom, izoom * 2.0f ) ;

            // reset origin by pixel ratio
            int_t const ratio = std::floor( 1.0f / ss.zoom ) ;
            if( ratio <= std::floor( 1.0f / izoom ) )
            {
                ss.origin = natus::math::vec2f_t() ;
            }
        }

        // tool tip infos
        if( in_cr )
        {
            // image pixel coord under mouse pos + transform y coord
            natus::math::vec2i_t const ip = (cur_mouse + idims * natus::math::vec2f_t( 0.5f, -0.5f )).floored() * natus::math::vec2f_t( 1.0f, -1.0f ) ;
                
            // 0.5f because dimensions are multiplided by 0.5f
            int_t const ratio = std::floor( 0.5f / ss.zoom ) ;
            
            _cur_pixel = ip ;
            _pixel_ratio = ratio ;

            {
                ImGui::BeginTooltip() ;
                
                ImGui::Text("cur mouse: (%.2f, %.2f)", cur_mouse.x(), cur_mouse.y() ) ;
                ImGui::Text("image pixel: (%d, %d)", ip.x(), ip.y() ) ;
                ImGui::Text("pixel ratio: 1 : %d", ratio ) ;

                ImGui::EndTooltip() ;
            }
        }
    }
}

void_t sprite_editor::show_image( natus::tool::imgui_view_t imgui, int_t const selected ) 
{
    auto & ss = _sprite_sheets[ selected ] ;

    natus::math::vec2f_t const crdims = natus::math::vec2f_t( 
        ImGui::GetContentRegionAvail().x, 
        ImGui::GetContentRegionAvail().y ) * natus::math::vec2f_t( 0.99f, 0.99f ) ;

    natus::math::vec2f_t const idims( ss.dims ) ;

    // This is what we want to see!
    // world space window rect bottom left, top right
    natus::math::vec4f_t const wrect = natus::math::vec4f_t( crdims, crdims ) * 
        natus::math::vec4f_t( ss.zoom ) * natus::math::vec4f_t( -1.0f, -1.0f, 1.0f, 1.0f ) +
        natus::math::vec4f_t( ss.origin, ss.origin ) ;

    // world space image rect bottom left, top right
    natus::math::vec4f_t const irect = natus::math::vec4f_t( idims, idims ) * 
        natus::math::vec4f_t(0.5f) * natus::math::vec4f_t( -1.0f, -1.0f, 1.0f, 1.0f ) ;

    // uv rect bottom left, top right
    // one vector from bottom left image to bottom left window
    // one vector from top right image to top right window
    auto const uv_rect = natus::math::vec4f_t( 0.0f,0.0f,1.0f,1.0f ) + 
        (wrect - irect) / natus::math::vec4f_t( idims, idims ) ;

    ImGui::Image( imgui.texture( ss.name ), 
        ImVec2( crdims.x(), crdims.y() ), 
        ImVec2( uv_rect.x(), uv_rect.y()), 
        ImVec2( uv_rect.z(), uv_rect.w()),
        ImVec4( 1, 1, 1, 1),
        ImVec4( 1, 1, 1, 1) ) ;
}