
#include "sprite_editor.h"
#include "imgui_custom.h"

#include <natus/format/future_items.hpp>
#include <natus/format/global.h>
#include <natus/format/natus/natus_structs.h>
#include <natus/format/natus/natus_module.h>

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
void_t sprite_editor::store( natus::io::database_res_t db ) noexcept 
{
    natus::format::module_registry_res_t mod_reg = natus::format::global_t::registry() ;

    natus::format::natus_document doc ;
        
    for( auto const & tss : _sprite_sheets )
    {
        natus::format::natus_document_t::sprite_sheet_t ss ;
        ss.name = tss.name ;

        {
                natus::format::natus_document_t::sprite_sheet_t::image_t img ;
                img.src = tss.img_loc.as_string() ;
                ss.image = img ;
        }

        ss.sprites.reserve( tss.sprites.size() ) ;
        for( auto const & ts : tss.sprites )
        {
            natus::format::natus_document_t::sprite_sheet_t::sprite_t sp ;
            sp.name = ts.name ;
            sp.animation.rect = tss.bounds[ ts.bound_idx ] ;
            sp.animation.pivot = tss.anim_pivots[ ts.pivot_idx ] ;

            ss.sprites.emplace_back( sp ) ;
        }
        
        for( auto const & ta : tss.animations )
        {
            natus::format::natus_document_t::sprite_sheet_t::animation_t ani ;
            ani.name = ta.name ;

            for( auto const & tf : ta.frames )
            {
                natus::format::natus_document_t::sprite_sheet_t::animation_t::frame_t fr ;
                fr.sprite = tss.sprites[ tf.sidx ].name ;
                fr.duration = tf.duration ;
                ani.frames.emplace_back( std::move( fr ) ) ;
            }

            ss.animations.emplace_back( std::move( ani ) ) ;
        }

        doc.sprite_sheets.emplace_back( ss ) ;
    }
        
    auto item = mod_reg->export_to( ss_loc, db, 
        natus::format::natus_item_res_t( natus::format::natus_item_t( std::move( doc ) ) ) ) ;

    natus::format::status_item_res_t r =  item.get() ;
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

            natus::format::item_res_t ir = item.fitem.get() ;
            if( natus::format::item_res_t::castable<natus::format::image_item_res_t>(ir) )
            {
                natus::format::image_item_res_t ii = ir ;

                natus::graphics::image_t img = *ii->img ;
                
                auto iter = std::find_if( _sprite_sheets.begin(), _sprite_sheets.end(), [&]( this_t::sprite_sheet_cref_t sh )
                {
                    return sh.name == item.name ;
                } ) ;

                if( iter != _sprite_sheets.end() )
                {
                    iter->dims = natus::math::vec2ui_t( img.get_dims() ) ;

                    iter->img = natus::graphics::image_object_t( iter->name, std::move( img ) )
                        .set_wrap( natus::graphics::texture_wrap_mode::wrap_s, natus::graphics::texture_wrap_type::clamp_border )
                        .set_wrap( natus::graphics::texture_wrap_mode::wrap_t, natus::graphics::texture_wrap_type::clamp_border )
                        .set_filter( natus::graphics::texture_filter_mode::min_filter, natus::graphics::texture_filter_type::nearest )
                        .set_filter( natus::graphics::texture_filter_mode::mag_filter, natus::graphics::texture_filter_type::nearest );

                    iter->zoom = -0.5f ;
                    imgui.async().configure( iter->img ) ;
                }
            }
            else if( natus::format::item_res_t::castable<natus::format::natus_item_res_t>(ir) )
            {
                natus::format::natus_item_res_t ni = ir ;
                ss_loc = item.loc ;

                natus::format::natus_document_t doc = ni->doc ;
                
                for( auto const & ss : doc.sprite_sheets )
                {
                    this_t::sprite_sheet_t tss ;
                    tss.name = ss.name ;
                    tss.dname = ss.name ;
                    tss.dims = natus::math::vec2ui_t( 1 ) ;

                    // load image
                    {
                        this_t::load_item_t li ;
                        li.disp_name = ss.name ;
                        li.name = ss.name ;
                        li.loc = ss.image.src ; //natus::io::location_t::from_path( natus::io::path_t( ss.image.src ) ) ; 
                        
                        natus::format::module_registry_res_t mod_reg = natus::format::global_t::registry() ;
                        li.fitem = mod_reg->import_from( li.loc, _db ) ;

                        _loads.emplace_back( std::move( li ) ) ;

                        tss.img_loc = ss.image.src ;
                    }

                    for( auto const & s : ss.sprites )
                    {
                        this_t::sprite_sheet_t::sprite_t ts ;
                        ts.name = s.name ;
                        ts.bound_idx = tss.bounds.size() ;
                        ts.pivot_idx = tss.anim_pivots.size() ;

                        tss.sprites.emplace_back( std::move( ts ) ) ;
                        tss.bounds.emplace_back( s.animation.rect ) ;
                        tss.anim_pivots.emplace_back( s.animation.pivot ) ;
                    }

                    for( auto const & a : ss.animations )
                    {
                        this_t::sprite_sheet::animation_t ta ;
                        ta.name = a.name ;
                        for( auto const & f : a.frames )
                        {
                            this_t::sprite_sheet_t::animation_frame_t taf ;
                            taf.duration = f.duration ;
                            auto const iter = std::find_if( tss.sprites.begin(), tss.sprites.end(), [&]( this_t::sprite_sheet_t::sprite_cref_t s )
                            {
                                return s.name == f.sprite ;
                            }) ;
                            taf.sidx = std::distance( tss.sprites.begin(), iter ) ;

                            if( taf.sidx < tss.sprites.size() ) ta.frames.emplace_back( std::move( taf ) ) ;
                        }
                        tss.animations.emplace_back( std::move( ta ) ) ;
                    }

                    _sprite_sheets.emplace_back( std::move( tss ) ) ;
                }
            }
        }
    }

    ImGui::Begin( "Sprite Editor" ) ;

    if( _sprite_sheets.size() == 0 )
    {
        ImGui::Text( "No Sprite Sheets" ) ;
        ImGui::End() ;
        return ;
    }

    // if width or height is 1, the image is not loaded yet
    if( _sprite_sheets[_cur_item].dims.equal( natus::math::vec2ui_t(1) ).any() )
    {
        ImGui::Text( "Not ready yet" ) ;
        ImGui::End() ;
        return ;
    }

    {
        ImGui::BeginGroup() ;
        
        {
            if( ImGui::Button( "Export" ) )
            {
                this_t::store( _db ) ;
            }

            if( _cur_mode == this_t::mode::pivot )
            {
                ImGui::SameLine() ;
                if( ImGui::Button( "Reset Pivots" ) )
                {
                    for( auto const & s : _sprite_sheets[_cur_item].sprites )
                    {
                        auto const & b = _sprite_sheets[_cur_item].bounds[s.bound_idx] ;
                        _sprite_sheets[_cur_item].anim_pivots[s.pivot_idx] = (b.zw() + b.xy())/2 ;
                    }
                }
            }
        }

        // sprite sheets list box
        {
            ImGui::SetNextItemWidth( ImGui::GetWindowWidth() * 0.3f ) ;
            ImGui::LabelText( "", "Sprite Sheets" ) ;

            natus::ntd::vector< const char * > names( _sprite_sheets.size() ) ;
            for( size_t i=0; i<_sprite_sheets.size(); ++i ) names[i] = _sprite_sheets[i].dname.c_str() ;
        
            ImGui::SetNextItemWidth( ImGui::GetWindowWidth() * 0.3f ) ;
            if( ImGui::ListBox( "sprite sheets", &_cur_item, names.data(), names.size() ) )
            {}
        }

        _cur_hovered = size_t(-1) ;

        if( _cur_mode == this_t::mode::bounds )
        {
            ImGui::SetNextItemWidth( ImGui::GetWindowWidth() * 0.3f ) ;
            ImGui::LabelText( "", "Boxes" ) ;

            size_t i = 0 ;
            natus::ntd::vector< natus::ntd::string_t > names( _sprite_sheets[_cur_item].sprites.size() ) ;
            for( auto const & s : _sprite_sheets[_cur_item].sprites )
            {
                auto const & b = _sprite_sheets[_cur_item].bounds[s.bound_idx] ;
                names[i] = std::to_string(i) + "( " + std::to_string(b.x()) + ", " + std::to_string(b.y()) + ", "
                    + std::to_string(b.z()) + ", " + std::to_string(b.w()) + " )";
                names[i] = s.name ;
                ++i ;
            }

            static int sel = 0 ;
            static size_t double_clicked = size_t(-1) ;
            ImGui::SetNextItemWidth( ImGui::GetWindowWidth() * 0.3f ) ;
            int hovered = -1 ;
            size_t item_edited = size_t(-1);

            {
                if( natus::tool::imgui_custom::ListBox( "bounds", &sel, &hovered, double_clicked, names, item_edited ) )
                {
                }
            }

            if( item_edited != size_t(-1) )
            {
                auto & sprites = _sprite_sheets[_cur_item].sprites ;
                auto const iter = std::find_if( sprites.begin(), sprites.end(), [&]( natus::tool::sprite_editor_t::sprite_sheet_t::sprite_cref_t s )
                {
                    return s.name == names[item_edited] ;
                } ) ;

                // only empty or unique names
                if( !names[item_edited].empty() && iter == sprites.end() )
                {
                    _sprite_sheets[_cur_item].sprites[item_edited].name = names[item_edited] ;
                }
                sel = 0 ;
                double_clicked = size_t(-1) ;
            }
            if( hovered != -1 )
            {
                //natus::log::global_t::status( std::to_string(hovered) ) ;
                _cur_hovered = size_t(hovered) ;
            }
        }
        else if( _cur_mode == this_t::mode::pivot )
        {
        }
        else if( _cur_mode == this_t::mode::hit )
        {
        }
        else if( _cur_mode == this_t::mode::damage )
        {
        }

        ImGui::EndGroup() ;
    }

    
    ImGui::SameLine() ; 

    static natus::math::vec4f_t rect ;

    // tabs
    {
        auto & ss = _sprite_sheets[_cur_item] ;

        ImGui::BeginGroup() ;

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            _croff = natus::math::vec2f_t( ImGui::GetCursorScreenPos().x,
                        ImGui::GetCursorScreenPos().y ) ;
            
            _crdims = natus::math::vec2f_t( 
                ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y ) * 
                    natus::math::vec2f_t( 0.99f, 0.99f ) ;
            
            this_t::handle_mouse( imgui, _cur_item ) ;

            if( ImGui::BeginTabItem("Bounds") )
            {
                this_t::handle_mouse_drag_for_bounds( _bounds_drag_info, ss.bounds ) ;
                _cur_mode = this_t::mode::bounds ;
                this_t::handle_mouse_drag_for_anim_pivot( _cur_item ) ;

                natus::math::vec4ui_t res ;
                if( this_t::handle_rect( _bounds_drag_info, res ) )
                {
                    this_t::sprite_sheet_t::sprite_t ts ;
                    ts.name = std::to_string( ss.sprites.size() ) ;
                    ts.bound_idx = ss.bounds.size() ;
                    ts.pivot_idx = ss.anim_pivots.size() ;
                    ss.sprites.emplace_back( std::move( ts ) ) ;

                    ss.bounds.emplace_back( res ) ;
                    ss.anim_pivots.emplace_back( res.xy() + (res.zw() - res.xy()) / natus::math::vec2ui_t(2) ) ;
                }

                this_t::show_image( imgui, _cur_item ) ;

                // draw rect for current mouse position
                {
                    natus::math::vec4f_t r( _cur_pixel, _cur_pixel ) ;
                    auto const rect = this_t::image_rect_to_window_rect( _cur_item, r ) ;
                    this_t::draw_rect_for_scale( rect ) ;
                }

                // draw rects
                {
                    auto const idx = this_t::draw_rects( ss.bounds, _cur_hovered, natus::math::vec4ui_t(255, 255, 255, 150), 
                        natus::math::vec4ui_t(0, 0, 255, 150) ) ;

                    if( idx != size_t(-1) )
                    {
                        auto const r = this_t::image_rect_to_window_rect( _cur_item, ss.bounds[idx] ) ;
                        this_t::draw_scales( r, ss.bounds[idx] ) ;
                    }
                }

                // draw currently building rect when user
                // presses the mouse button
                if( _bounds_drag_info.mouse_down_rect )
                {
                    auto const rect0 = this_t::rearrange_mouse_rect( _bounds_drag_info.cur_rect ) ;
                    auto const rect = this_t::image_rect_to_window_rect( _cur_item, rect0 ) ;
                    this_t::draw_rect( rect ) ;
                }

                
                ImGui::EndTabItem() ;
            }

            if( ImGui::BeginTabItem("Pivot") )
            {
                _cur_mode = this_t::mode::pivot ;

                this_t::handle_mouse_drag_for_anim_pivot( _cur_item ) ;

                this_t::show_image( imgui, _cur_item ) ;
                
                // draw rect for current mouse position
                {
                    natus::math::vec4f_t r( _cur_pixel, _cur_pixel ) ;
                    auto const rect = this_t::image_rect_to_window_rect( _cur_item, r ) ;
                    this_t::draw_rect_for_scale( rect ) ;
                }

                // draw rects
                {
                    this_t::draw_rects( ss.bounds ) ;
                }

                // draw pivots
                {
                    this_t::draw_points( ss.anim_pivots ) ;
                }
                

                ImGui::EndTabItem() ;
            }

            if( ImGui::BeginTabItem("Hit") )
            {
                _cur_mode = this_t::mode::hit ;

                this_t::handle_mouse_drag_for_bounds( _hits_drag_info, ss.hits ) ;

                natus::math::vec4ui_t res ;
                if( this_t::handle_rect( _hits_drag_info, res ) )
                {
                    res = res ;
                    ss.hits.emplace_back( res ) ;
                }

                this_t::show_image( imgui, _cur_item ) ;
                
                // draw bound rects
                {
                    this_t::draw_rects( ss.bounds ) ;
                }

                // draw bound rects
                {
                    this_t::draw_rects( ss.hits, _cur_hovered, natus::math::vec4ui_t(255, 0, 0, 150), 
                        natus::math::vec4ui_t(0, 255, 255, 150) ) ;
                }

                // draw currently building rect when user
                // presses the mouse button
                if( _hits_drag_info.mouse_down_rect )
                {
                    auto const rect0 = this_t::rearrange_mouse_rect( _hits_drag_info.cur_rect ) ;
                    auto const rect = this_t::image_rect_to_window_rect( _cur_item, rect0 ) ;
                    this_t::draw_rect( rect ) ;
                }

                ImGui::EndTabItem() ;
            }

            if( ImGui::BeginTabItem("Damage") )
            {
                _cur_mode = this_t::mode::damage ;

                this_t::handle_mouse_drag_for_bounds( _damages_drag_info, ss.damages ) ;

                natus::math::vec4ui_t res ;
                if( this_t::handle_rect( _damages_drag_info, res ) )
                {
                    res = res ;
                    ss.damages.emplace_back( res ) ;
                }

                this_t::show_image( imgui, _cur_item ) ;
                
                // draw bound rects
                {
                    this_t::draw_rects( ss.bounds ) ;
                }

                // draw bound rects
                {
                    this_t::draw_rects( ss.damages, _cur_hovered, natus::math::vec4ui_t(255, 0, 0, 150), 
                        natus::math::vec4ui_t(0, 255, 255, 150) ) ;
                }

                // draw currently building rect when user
                // presses the mouse button
                if( _damages_drag_info.mouse_down_rect )
                {
                    auto const rect0 = this_t::rearrange_mouse_rect( _damages_drag_info.cur_rect ) ;
                    auto const rect = this_t::image_rect_to_window_rect( _cur_item, rect0 ) ;
                    this_t::draw_rect( rect ) ;
                }

                ImGui::EndTabItem() ;
            }

            

            ImGui::EndTabBar() ;
        }

        ImGui::EndGroup() ;
    }


    
    #if 0
    {
        ImGui::BeginTooltip() ;
            
        //ImGui::Text("rect: (%.2f, %.2f), (%.2f, %.2f)", a.x, a.y, b.x, b.y ) ;
        //ImGui::Text("cur mouse: (%.2f, %.2f)", _cur_mouse.x(), _cur_mouse.y() ) ;
        ImGui::Text("image pixel: (%d, %d)", _cur_pixel.x(), _cur_pixel.y() ) ;
        //ImGui::Text("pixel ratio: 1 : %d", ratio ) ;

        ImGui::EndTooltip() ;
    }
    #endif

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
        ss.zoom = std::max( tmp.x(), tmp.y() ) ;
    }
    
    natus::math::vec2f_t const mouse_in_window(
        io.MousePos.x - ImGui::GetCursorScreenPos().x,
        io.MousePos.y - ImGui::GetCursorScreenPos().y ) ;

    natus::math::vec2f_t const mouse_centered = 
            (mouse_in_window - crdims * natus::math::vec2f_t( 0.5f )) * natus::math::vec2f_t(1.0f,-1.0f) ;

    auto cur_mouse = ss.origin + mouse_centered * ss.zoom ;
            
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
        float_t const izoom = std::max( tmp.x(), tmp.y() ) ;
                
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

    // image pixel coord under mouse pos + transform y coord
    natus::math::vec2i_t const ip = (cur_mouse + idims * natus::math::vec2f_t( 0.5f, 0.5f )).floored() * natus::math::vec2f_t( 1.0f, 1.0f ) ;
    
    

    // tool tip infos
    // and remember values
    if( in_cr )
    {
        // 0.5f because dimensions are multiplided by 0.5f
        int_t const ratio = std::floor( 1.0f / ss.zoom ) ;
            
        _cur_pixel = ip ;
        _pixel_ratio = ratio ;
        _cur_mouse = cur_mouse ; //(cur_mouse + crdims * natus::math::vec2f_t( 0.5f, 0.5f ) )* natus::math::vec2f_t( 1.0f, 1.0f ) ;
        _cr_mouse = mouse_in_window ;
    }
    
}

void_t sprite_editor::handle_mouse_drag_for_bounds( this_t::rect_drag_info_ref_t info, natus::ntd::vector< natus::math::vec4ui_t > & rects ) 
{
    ImGuiIO& io = ImGui::GetIO() ;
    auto & ss = _sprite_sheets[_cur_item] ;

    natus::math::vec2f_t const crdims = natus::math::vec2f_t( 
        ImGui::GetContentRegionAvail().x, 
        ImGui::GetContentRegionAvail().y ) * natus::math::vec2f_t( 0.99f, 0.99f ) ;
    
    natus::math::vec2f_t const mouse_in_window(
        io.MousePos.x - ImGui::GetCursorScreenPos().x,
        io.MousePos.y - ImGui::GetCursorScreenPos().y ) ;

    bool_t const in_cr = 
        mouse_in_window.greater_than( natus::math::vec2f_t() ).all() &&
        mouse_in_window.less_than( crdims ).all() ;

    // handle mouse dragging
    {
        if( in_cr && !info.mouse_down_rect )
        {
            if( io.MouseDown[0] )
            {
                // start dragging
                if( !info.mouse_down_drag )
                {
                    size_t idx = size_t(-1) ;
                    for( size_t i=0; i<rects.size(); ++i )
                    {
                        if( this_t::is_ip_mouse_in_bound( rects[i] ) )
                        {
                            idx = i ;
                            break ;
                        }
                    }

                    if( idx != size_t(-1) ) 
                    {
                        info.drag_idx = idx ;
                        info.mouse_down_drag = true ;
                        info.drag_begin = _cur_pixel ;
                    }
                }
                // do dragging
                else if( info.drag_idx != size_t(-1) )
                {
                    std::array< bool_t, 8 > corners = {false,false,false,false,false,false,false,false} ;
                    bool_t drag_rect = !this_t::intersect_bound_location( info.drag_begin, rects[info.drag_idx], corners ) ;

                    auto dif = natus::math::vec2i_t( _cur_pixel ) - natus::math::vec2i_t( info.drag_begin ) ;

                    auto rect = natus::math::vec4ui_t( rects[info.drag_idx] ) ;

                    if( drag_rect )
                    {
                        auto xy0 = natus::math::vec2i_t( rect.xy() ) + dif ;
                        if( xy0.x() < 0 ) dif.x( dif.x() + -xy0.x() ) ;
                        if( xy0.y() < 0 ) dif.y( dif.y() + -xy0.y() ) ;
                        xy0 = natus::math::vec2i_t( rect.xy() ) + dif ;

                        auto const xy1 = rect.zw() + dif ;

                        rect = natus::math::vec4ui_t( xy0, xy1 ) ;
                    }
                    else if( corners[0] )
                    {
                        auto const xy0 = natus::math::vec2i_t( rect.xy() ) + dif ;
                        auto const xy1 = rect.zw() ;
                        rect = natus::math::vec4ui_t( xy0, xy1 ) ;
                    }
                    else if( corners[2] )
                    {
                        auto const xy = natus::math::vec2i_t( rect.xw() ) + dif ;
                        auto const xy0 = natus::math::vec2ui_t( xy.x(), rect.y() ) ;
                        auto const xy1 = natus::math::vec2ui_t( rect.z(), xy.y() ) ;
                        rect = natus::math::vec4ui_t( xy0, xy1 ) ;
                    }
                    else if( corners[4] )
                    {
                        auto const xy0 = rect.xy() ;
                        auto const xy1 = natus::math::vec2i_t( rect.zw() ) + dif ;
                        rect = natus::math::vec4ui_t( xy0, xy1 ) ;
                    }
                    else if( corners[6] )
                    {
                        auto const xy = natus::math::vec2i_t( rect.zy() ) + dif ;
                        auto const xy0 = natus::math::vec2ui_t( rect.x(), xy.y() ) ;
                        auto const xy1 = natus::math::vec2ui_t( xy.x(), rect.w() ) ;
                        rect = natus::math::vec4ui_t( xy0, xy1 ) ;
                    }

                    if( (natus::math::vec2i_t(rect.zw()) - natus::math::vec2i_t(rect.xy())).less_than( natus::math::vec2ui_t( 1, 1 ) ).any() ) 
                    {
                        return ;
                    }

                    rects[ info.drag_idx ] = rect ;

                    info.drag_begin = _cur_pixel ;
                }
            }
            else if( io.MouseReleased[0] && info.mouse_down_drag )
            {
                info.drag_idx = size_t(-1 ) ;
                info.mouse_down_drag = false ;
            }
        }
        else if( io.MouseReleased[0] && info.mouse_down_drag ) 
        {
            info.drag_idx = size_t(-1 ) ;
            info.mouse_down_drag = false ;
        }
    }
}

void_t sprite_editor::handle_mouse_drag_for_anim_pivot( int_t const selection ) 
{
    ImGuiIO& io = ImGui::GetIO() ;
    auto & ss = _sprite_sheets[selection] ;

    natus::math::vec2f_t const crdims = natus::math::vec2f_t( 
        ImGui::GetContentRegionAvail().x, 
        ImGui::GetContentRegionAvail().y ) * natus::math::vec2f_t( 0.99f, 0.99f ) ;
    
    natus::math::vec2f_t const mouse_in_window(
        io.MousePos.x - ImGui::GetCursorScreenPos().x,
        io.MousePos.y - ImGui::GetCursorScreenPos().y ) ;

    bool_t const in_cr = 
        mouse_in_window.greater_than( natus::math::vec2f_t() ).all() &&
        mouse_in_window.less_than( crdims ).all() ;

    if( !in_cr ) return ;

    if( io.MouseDown[0] )
    {
        // start dragging
        if( !_mouse_down_drag_anim )
        {
            size_t idx = size_t(-1) ;
            for( size_t i=0; i<ss.anim_pivots.size(); ++i )
            {
                //if( this_t::is_ip_mouse_in_bound( natus::math::vec4ui_t( ss.anim_pivots[i], ss.anim_pivots[i]) ) )
                if( this_t::is_ip_mouse_in_bound( ss.bounds[i] ) )
                {
                    idx = i ;
                    break ;
                }
            }

            if( idx != size_t(-1) ) 
            {
                _drag_idx_anim = idx ;
                _mouse_down_drag_anim = true ;
                _drag_begin_anim = _cur_pixel ;
            }
        }
        // do dragging
        else
        {
            ss.anim_pivots[_drag_idx_anim] += natus::math::vec2i_t(_cur_pixel) - natus::math::vec2i_t(_drag_begin_anim) ;
            _drag_begin_anim = _cur_pixel ;
        }
    }
    else if( _mouse_down_drag_anim )
    {
        _drag_idx_anim = size_t(-1) ;
        _mouse_down_drag_anim = false ;
    }
}

void_t sprite_editor::show_image( natus::tool::imgui_view_t imgui, int_t const selected ) 
{
    _screen_pos_image = natus::math::vec2f_t(
        ImGui::GetCursorScreenPos().x,ImGui::GetCursorScreenPos().y ) ;

    auto & ss = _sprite_sheets[ selected ] ;

    natus::math::vec2f_t const crdims = natus::math::vec2f_t( 
        ImGui::GetContentRegionAvail().x, 
        ImGui::GetContentRegionAvail().y ) * natus::math::vec2f_t( 0.99f, 0.99f ) ;

    natus::math::vec2f_t const idims( ss.dims ) ;

    natus::math::vec2f_t origin( ss.origin.x(), -ss.origin.y() ) ;

    // This is what we want to see!
    // world space window rect bottom left, top right
    natus::math::vec4f_t const wrect = natus::math::vec4f_t( crdims, crdims ) * 
        natus::math::vec4f_t( ss.zoom ) * natus::math::vec4f_t( -0.5f, -0.5f, 0.5f, 0.5f ) +
        natus::math::vec4f_t( origin, origin ) ;

    // world space image rect bottom left, top right
    natus::math::vec4f_t const irect = natus::math::vec4f_t( idims, idims ) * 
        natus::math::vec4f_t( -0.5f, -0.5f, 0.5f, 0.5f ) ;

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


bool_t sprite_editor::handle_rect( this_t::rect_drag_info_ref_t info, natus::math::vec4ui_ref_t res  ) 
{
    if( _bounds_drag_info.mouse_down_drag ) return false;

    ImGuiIO& io = ImGui::GetIO() ;
    auto & ss = _sprite_sheets[_cur_item] ;

    if( !ImGui::IsWindowHovered() ) return false ;
    
    natus::math::vec2f_t const crdims = natus::math::vec2f_t( 
        ImGui::GetContentRegionAvail().x, 
        ImGui::GetContentRegionAvail().y ) * natus::math::vec2f_t( 0.99f, 0.99f ) ;
    
    natus::math::vec2f_t const mouse_in_window(
        io.MousePos.x - ImGui::GetCursorScreenPos().x,
        io.MousePos.y - ImGui::GetCursorScreenPos().y ) ;

    bool_t const in_cr = 
        mouse_in_window.greater_than( natus::math::vec2f_t() ).all() &&
        mouse_in_window.less_than( crdims ).all() ;
        
    if( in_cr && io.MouseDown[0] && !info.mouse_down_rect )
    {
        info.mouse_down_rect = true ;
        info.cur_rect = natus::math::vec4ui_t( _cur_pixel, _cur_pixel ) ;
    }

    else if( io.MouseDown[0] )
    {        
        info.cur_rect.zw( _cur_pixel ) ;
        
        return false ;
    }

    else if( io.MouseReleased[0] && info.mouse_down_rect )
    {
        info.mouse_down_rect = false ;
        info.cur_rect.zw( _cur_pixel ) ;

        if( (info.cur_rect.zw() - info.cur_rect.xy()).less_than( natus::math::vec2ui_t(1,1) ).any() ) 
            return false ;

        res = this_t::rearrange_mouse_rect( info.cur_rect ) ;
        info.cur_rect = natus::math::vec4ui_t() ;

        return true ;
    }
    return false ;
}

natus::math::vec4ui_t sprite_editor::rearrange_mouse_rect( natus::math::vec4ui_cref_t vin ) const 
{
    uint_t const x0 = std::min( vin.x(), vin.z() ) ;
    uint_t const x1 = std::max( vin.x(), vin.z() ) ;

    uint_t const y0 = std::min( vin.y(), vin.w() ) ;
    uint_t const y1 = std::max( vin.y(), vin.w() ) ;

    return natus::math::vec4ui_t( x0, y0, x1, y1 ) ;
}

natus::math::vec4f_t sprite_editor::compute_cur_view_rect( int_t const selection ) const 
{
    auto & ss = _sprite_sheets[ selection ] ;

    // This is what we want to see!
    // world space window rect bottom left, top right
    return natus::math::vec4f_t( _crdims, _crdims ) * 
        natus::math::vec4f_t( ss.zoom ) * natus::math::vec4f_t( -0.5f, -0.5f, 0.5f, 0.5f ) +
        natus::math::vec4f_t( ss.origin, ss.origin ) ;
}

natus::math::vec4f_t sprite_editor::image_rect_to_window_rect( int_t const selection, 
                natus::math::vec4f_cref_t image_rect ) const
{
    auto & ss = _sprite_sheets[selection] ;

    auto const a = image_rect.xy() + natus::math::vec2f_t( ss.dims ) * natus::math::vec2f_t( -0.5f ) ;

    auto const b = a + (image_rect.zw() - image_rect.xy()) + 1 ;

    natus::math::vec4f_t const wrect = this_t::compute_cur_view_rect( selection ) ;
                                        
    auto const y = natus::math::vec2f_t( 0.0f, (wrect.zw() - wrect.xy()).y() ) ;

    natus::math::vec2f_t const xy = (a - wrect.xy() - y ) / ss.zoom ;
    natus::math::vec2f_t const wh = (b - a ).absed() / ss.zoom ;
                   
    natus::math::vec2f_t const xy_ = xy * natus::math::vec2f_t( 1.0f, -1.0f ) + _croff ;

    return natus::math::vec4f_t( xy_, wh ) ;
}

bool_t sprite_editor::is_window_rect_inside_content_region( natus::math::vec4f_ref_t rect, bool_t const fix_coords ) const 
{
    // remember: y grows downwards in imgui window space

    natus::math::vec2f_t const off = _croff ;

    // right/bottom completely outside
    if( rect.x() > (off + _crdims).x() ) return false ;
    if( rect.y() - rect.w() > (off + _crdims).y() ) return false ;
    
    // left/top completly outside
    if( rect.x() + rect.z() < off.x() ) return false ;
    if( rect.y() < off.y() ) return false ;
    
    if( !fix_coords ) return true ;

    float_t const x = std::max( rect.x(), off.x() ) ;
    float_t const y = std::min( rect.y(), off.y() + _crdims.y() ) ;

    float_t const w0 = rect.z() - ( x - rect.x() ) ;
    float_t const w1 = std::min( rect.x() + rect.z(), off.x() + _crdims.x() ) - rect.x() ;

    float_t const h0 = rect.w() - ( rect.y() - y ) ;
    float_t const h1 = rect.y() - std::max( rect.y() - rect.w(), off.y() ) ;

    rect = natus::math::vec4f_t( x, y, std::min( w0, w1 ), std::min( h0, h1 ) ) ;

    return true ;
}

void_t sprite_editor::draw_rect( natus::math::vec4f_cref_t rect, natus::math::vec4ui_cref_t color) 
{
    ImVec2 const a_( rect.x(), rect.y() + 1 ) ;
    ImVec2 const b_( rect.x() + rect.z() + 1, rect.y() - rect.w() + 1 ) ;

    ImGui::GetWindowDrawList()->AddRect( a_, b_, IM_COL32( color.x(), color.y(), color.z(), color.w() ) ) ;
    ImGui::GetWindowDrawList()->AddRectFilled( a_, b_, IM_COL32( color.x(), color.y(), color.z(), color.w() ) ) ;
}

void_t sprite_editor::draw_rect_info( natus::math::vec4f_cref_t rect, natus::math::vec4ui_cref_t img_rect ) noexcept
{
    ImGui::BeginTooltip() ;
    ImGui::Text( "Img Rect: %d %d %d %d", img_rect.x(), img_rect.y(), img_rect.z(), img_rect.w() ) ;
    ImGui::EndTooltip() ;
}

size_t sprite_editor::draw_rects( natus::ntd::vector< natus::math::vec4ui_t > const & rects, size_t const hovered,
        natus::math::vec4ui_cref_t color, natus::math::vec4ui_cref_t over_color) 
{
    size_t ret = size_t( -1 ) ;

    for( size_t i=0; i<rects.size(); ++i )
    {
        auto rect = this_t::image_rect_to_window_rect( _cur_item, rects[i] ) ;

        if( !this_t::is_window_rect_inside_content_region( rect ) ) continue ;

        // if mouse on a rect, draw scales
        if( this_t::is_ip_mouse_in_bound( rects[i] ) )
        {
            this_t::draw_rect( rect, over_color ) ;
            this_t::draw_rect_info( rect, rects[i] ) ;
            ret = i ;
        }
        else if( hovered == i )
            this_t::draw_rect( rect, over_color ) ;
        else
        {
            this_t::draw_rect( rect, color ) ;
        }
    }
    return ret ;
}

void_t sprite_editor::draw_points( natus::ntd::vector< natus::math::vec2ui_t > const & points, 
    natus::math::vec4ui_cref_t color ) 
{
    for( size_t i=0; i<points.size(); ++i )
    {
        natus::math::vec4f_t rect( points[i], points[i] ) ;
        rect = this_t::image_rect_to_window_rect( _cur_item, rect  ) ;

        if( !this_t::is_window_rect_inside_content_region( rect ) ) continue ;

        // if mouse on a rect, draw scales
        if( this_t::is_ip_mouse_in_bound( rect ) )
        {
            this_t::draw_rect( rect, natus::math::vec4ui_t(255,255,0,255) ) ;

            auto r = this_t::image_rect_to_window_rect( _cur_item, rect ) ;
            this_t::draw_scales( r, rect ) ;
        }
        else
        {
            this_t::draw_rect( rect, color ) ;
        }
    }
}

void_t sprite_editor::draw_rect_for_scale( natus::math::vec4f_cref_t rect, natus::math::vec4ui_cref_t color ) 
{
    ImVec2 const a_( rect.x(), rect.y() + 1 ) ;
    ImVec2 const b_( rect.x() + rect.z() + 1, rect.y() - rect.w() + 1 ) ;

    ImGui::GetWindowDrawList()->AddRectFilled( a_, b_, IM_COL32( color.x(), color.y(), color.z(), color.w() ) ) ;
}

void_t sprite_editor::draw_scales( natus::math::vec4f_cref_t rect, natus::math::vec4ui_cref_t prect, natus::math::vec4ui_t color ) 
{
    float_t const wh = float_t( _pixel_ratio ) * 1.0f ;

    std::array< bool_t, 8 > corners = {false,false,false,false,false,false,false,false} ;
    this_t::intersect_bound_location( _cur_pixel, prect, corners ) ;

    std::array< natus::math::vec4ui_t, 8 > colors = 
    {
        corners[0] ? natus::math::vec4ui_t( 255, 0, 0, 255  ) : color,
        corners[1] ? natus::math::vec4ui_t( 255, 0, 0, 255  ) : color,
        corners[2] ? natus::math::vec4ui_t( 255, 0, 0, 255  ) : color,
        corners[3] ? natus::math::vec4ui_t( 255, 0, 0, 255  ) : color,
        corners[4] ? natus::math::vec4ui_t( 255, 0, 0, 255  ) : color,
        corners[5] ? natus::math::vec4ui_t( 255, 0, 0, 255  ) : color,
        corners[6] ? natus::math::vec4ui_t( 255, 0, 0, 255  ) : color,
        corners[7] ? natus::math::vec4ui_t( 255, 0, 0, 255  ) : color,
    } ;

    // bottom left
    {
        natus::math::vec4f_t r( rect.x(), rect.y(), wh, wh ) ;
        if( this_t::is_window_rect_inside_content_region( r, false ) )
        {
            this_t::draw_rect_for_scale( r, colors[0] ) ;
        }
    }
    // top left
    {
        natus::math::vec4f_t r( rect.x(), rect.y()-rect.w()+wh, wh, wh ) ;
        if( this_t::is_window_rect_inside_content_region( r, false ) )
        {
            this_t::draw_rect_for_scale( r, colors[2] ) ;
        }
    }

    // top right
    {
        natus::math::vec4f_t r( rect.x()+rect.z()-wh, rect.y()-rect.w()+wh, wh, wh ) ;
        if( this_t::is_window_rect_inside_content_region( r, false ) )
        {
            this_t::draw_rect_for_scale( r, colors[4] ) ;
        }
    }

    // bottom right
    {
        natus::math::vec4f_t r( rect.x()+rect.z()-wh, rect.y(), wh, wh ) ;
        if( this_t::is_window_rect_inside_content_region( r, false ) )
        {
            this_t::draw_rect_for_scale( r, colors[6] ) ;
        }
    }
}

bool_t sprite_editor::is_ip_mouse_in_bound( natus::math::vec4ui_cref_t rect ) const 
{
    if( uint_t( _cur_pixel.x() ) < rect.x() || uint_t( _cur_pixel.x() ) > rect.z() ) return false ;
    if( uint_t( _cur_pixel.y() ) < rect.y() || uint_t( _cur_pixel.y() ) > rect.w() ) return false ;
    return true ;
}

bool_t sprite_editor::intersect_bound_location( natus::math::vec2ui_cref_t cur_pixel, natus::math::vec4ui_cref_t rect, std::array< bool_t, 8 > & hit ) const 
{
    natus::math::vec2ui_t const cp = cur_pixel ;

    // bottom lect
    if( cp.equal( rect.xy() ).all() )
    {
        hit[0] = true ;
        return true ;
    }
    // top left
    else if( cp.equal( rect.xw() ).all() ) 
    {
        hit[2] = true ;
        return true ;
    }
    // top right
    else if( cp.equal( rect.zw() ).all() ) 
    {
        hit[4] = true ;
        return true ;
    }
    // bottom right
    else if( cp.equal( rect.zy() ).all() ) 
    {
        hit[6] = true ;
        return true ;
    }
    // left
    else if( cp.x() == rect.x() ) 
    {
        hit[1] = true ;
        return true ;
    }
    // top
    else if( cp.y() == rect.w() ) 
    {
        hit[3] = true ;
        return true ;
    }
    // right
    else if( cp.x() == rect.z() ) 
    {
        hit[5] = true ;
        return true ;
    }
    // bottom
    else if( cp.y() == rect.y() ) 
    {
        hit[7] = true ;
        return true ;
    }

    return false ;
}