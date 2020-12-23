
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
    ss.name = "natus.tool.sprite_editor." + name ;
    ss.loc = loc ;

    natus::format::module_registry_res_t mod_reg = natus::format::global_t::registry() ;
    ss.fitem = mod_reg->import_from( loc, _db ) ;

    _loads.emplace_back( std::move( ss ) ) ;
}

// ****
void_t sprite_editor::render( natus::tool::imgui_view_t imgui ) noexcept 
{
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
                ss.name = item.name ;
                ss.img_loc = item.loc ;
                ss.dims = natus::math::vec2ui_t( img.get_dims() ) ;

                ss.img = natus::graphics::image_object_t( ss.name, std::move( img ) )
                    .set_wrap( natus::graphics::texture_wrap_mode::wrap_s, natus::graphics::texture_wrap_type::clamp_border )
                    .set_wrap( natus::graphics::texture_wrap_mode::wrap_t, natus::graphics::texture_wrap_type::clamp_border )
                    .set_filter( natus::graphics::texture_filter_mode::min_filter, natus::graphics::texture_filter_type::nearest )
                    .set_filter( natus::graphics::texture_filter_mode::mag_filter, natus::graphics::texture_filter_type::nearest );

                imgui.async().configure( ss.img ) ;

                _sprite_sheets.emplace_back( std::move( ss ) ) ;
            }
        }
    }

    ImGui::Begin( "Sprite Editor" ) ;

    if( _sprite_sheets.size() != 0 )
    {
        ImGuiIO& io = ImGui::GetIO();
        auto & ss = _sprite_sheets[0] ;

        natus::math::vec2f_t const crdims = natus::math::vec2f_t( 
            ImGui::GetContentRegionAvail().x, 
            ImGui::GetContentRegionAvail().y ) * natus::math::vec2f_t( 0.99f, 0.99f ) ;

        natus::math::vec2f_t const idims( ss.dims ) ;

        // world space window rect bottom left, top right
        natus::math::vec4f_t const wrect = natus::math::vec4f_t( crdims, crdims ) * 
            natus::math::vec4f_t(0.5f) * natus::math::vec4f_t( -1.0f, -1.0f, 1.0f, 1.0f ) ;

        // world space image rect bottom left, top right
        natus::math::vec4f_t const irect = natus::math::vec4f_t( idims, idims ) * 
            natus::math::vec4f_t(0.5f) * natus::math::vec4f_t( -1.0f, -1.0f, 1.0f, 1.0f ) ;

        auto const dif = natus::math::vec4f_t( 0.0f,0.0f,1.0f,1.0f ) + 
            (wrect - irect) / natus::math::vec4f_t( idims, idims ) ;

        ImGui::Image( imgui.texture( ss.name ), 
            ImVec2( crdims.x(), crdims.y() ), 
            ImVec2( dif.x(), dif.y()), 
            ImVec2( dif.z(), dif.w()),
            ImVec4( 1, 1, 1, 1),
            ImVec4( 1, 1, 1, 1) ) ;

        ImGui::BeginTooltip();

        ImGui::Text("dif: (%.2f, %.2f, %.2f, %.2f)", dif.x(), dif.y(), dif.z(), dif.w() );
        //ImGui::Text("wuv: (%.2f, %.2f)", window_zo.x(), window_zo.y() );
        //ImGui::Text("iuv: (%.2f, %.2f)", img_pos.x(), img_pos.y() );
        //ImGui::Text("iuv: (%.2f, %.2f)", image_uv.x(), image_uv.y() );
        //ImGui::Text("img: (%d, %d)", img_pos.x(), img_pos.y() );

        

        ImGui::EndTooltip();

        #if 0
        ImVec2 pos = ImGui::GetCursorScreenPos();

        natus::math::vec2f_t const window_pos(
            io.MousePos.x - pos.x,
            io.MousePos.y - pos.y ) ;
        
        // [0,1] value origin top left
        natus::math::vec2f_t const window_zo = window_pos / dims ;

        // number of window pixels per image pixel
        natus::math::vec2f_t const wppi = dims / idims ;

        float_t const aspect = std::min( wppi.x(), wppi.y() ) ;

        natus::math::vec2i_t img_pos ;

        {
            natus::math::vec2f_t const orig_zo( 0.5f, 0.5f ) ;
            natus::math::vec2f_t const origin = dims * orig_zo ;
            auto const iwh = dims / (wppi/natus::math::vec2f_t(aspect)) ;
        
            // top left to bottom left
            auto const win_zo_inv = natus::math::vec2f_t(0.0f, 1.0f) - 
                window_zo * natus::math::vec2f_t(-1.0f, 1.0f ) ;

            auto const bl = origin - iwh * 0.5f ; 
            img_pos = win_zo_inv * dims - bl ;
            img_pos /= wppi ;
        }

        // compute uv coords
        {
        }

        natus::math::vec2f_t const uv_zoom = natus::math::vec2f_t(1.5f) ;
        natus::math::vec2f_t const uv_center = natus::math::vec2f_t(0.5f) ;

        // uv dimensions
        natus::math::vec2f_t const uvd = uv_zoom * (wppi/natus::math::vec2f_t(aspect)) ;
        natus::math::vec2f_t const uvh = uvd * natus::math::vec2f_t( 0.5f ) ;
        natus::math::vec2f_t const uv_min = uv_center - uvh ;
        natus::math::vec2f_t const uv_max = uv_center + uvh ;

        natus::math::vec2f_t const wuv_inv = natus::math::vec2f_t(1.0f) - window_zo ;
        natus::math::vec2f_t const rel = window_zo - uv_center ;

        natus::math::vec2i_t const image_uv = 
            wuv_inv - uv_min ;
        
        ImGui::Image( imgui.texture( ss.name ), 
            ImVec2( dims.x(), dims.y() ), 
            ImVec2(uv_min.x(), uv_min.y()), 
            ImVec2(uv_max.x(), uv_max.y()),
            ImVec4( 1, 1, 1, 1),
            ImVec4( 1, 1, 1, 1) ) ;

        ImGui::BeginTooltip();

        //ImGui::Text("PP: (%.2f, %.2f)", window_pos.x(), window_pos.y() );
        //ImGui::Text("wuv: (%.2f, %.2f)", window_zo.x(), window_zo.y() );
        //ImGui::Text("iuv: (%.2f, %.2f)", img_pos.x(), img_pos.y() );
        //ImGui::Text("iuv: (%.2f, %.2f)", image_uv.x(), image_uv.y() );
        //ImGui::Text("img: (%d, %d)", img_pos.x(), img_pos.y() );

        

        ImGui::EndTooltip();
        #endif
    }

    ImGui::End() ;
}