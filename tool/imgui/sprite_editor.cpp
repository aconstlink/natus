
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

    if( _sprite_sheets.size() == 0 )
    {
        ImGui::Text( "No Sprite Sheets" ) ;
        ImGui::End() ;
        return ;
    }

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

    static natus::math::vec4f_t rect ;

    // tabs
    {
        ImGui::BeginGroup() ;

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
        {
            _croff = natus::math::vec2f_t( ImGui::GetCursorScreenPos().x,
                        ImGui::GetCursorScreenPos().y ) ;
            
            _crdims = natus::math::vec2f_t( 
                ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y ) * 
                    natus::math::vec2f_t( 0.99f, 0.99f ) ;

            if( ImGui::BeginTabItem("Bounds") )
            {
                auto & ss = _sprite_sheets[_cur_item] ;

                this_t::handle_mouse( imgui, _cur_item ) ;
                
                natus::math::vec4ui_t res ;
                if( this_t::handle_rect( imgui, res ) )
                {
                    res = res ;
                    ss.bounds.emplace_back( res ) ;
                }

                this_t::show_image( imgui, _cur_item ) ;

                // draw rects
                {
                    for( size_t i=0; i<ss.bounds.size(); ++i )
                    {
                        auto rect = this_t::image_rect_to_window_rect( _cur_item, ss.bounds[i] ) ;

                        if( !this_t::is_window_rect_inside_content_region( rect ) ) continue ;

                        this_t::draw_rect( rect, natus::math::vec4ui_t(200+ss.bounds[i].x(), 200+ss.bounds[i].y(), 255, 255) ) ;

                        // if mouse on a rect, draw scales
                        if( this_t::is_ip_mouse_in_bound( ss.bounds[i] ) )
                        {
                            auto r = this_t::image_rect_to_window_rect( _cur_item, ss.bounds[i] ) ;
                            this_t::draw_scales( r ) ;
                        }
                    }
                }

                // draw currently building rect when user
                // presses the mouse button
                if( _mouse_down_rect )
                {
                    auto const rect0 = this_t::rearrange_mouse_rect( _cur_rect ) ;
                    auto const rect = this_t::image_rect_to_window_rect( _cur_item, rect0 ) ;
                    this_t::draw_rect( rect ) ;
                }

                // draw rect for current mouse position
                {
                    natus::math::vec4f_t r( _cur_pixel, _cur_pixel ) ;
                    auto const rect = this_t::image_rect_to_window_rect( _cur_item, r ) ;
                    this_t::draw_rect_for_scale( rect ) ;
                }



                ImGui::EndTabItem() ;
            }

            if( ImGui::BeginTabItem("Animation") )
            {
                this_t::handle_mouse( imgui, _cur_item ) ;
                this_t::show_image( imgui, _cur_item ) ;
                
                ImGui::EndTabItem() ;
            }

            if( ImGui::BeginTabItem("Hit") )
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

            ImGui::EndTabBar() ;
        }

        ImGui::EndGroup() ;
    }


    

    {
        ImGui::BeginTooltip() ;
            
        //ImGui::Text("rect: (%.2f, %.2f), (%.2f, %.2f)", a.x, a.y, b.x, b.y ) ;
        //ImGui::Text("cur mouse: (%.2f, %.2f)", _cur_mouse.x(), _cur_mouse.y() ) ;
        ImGui::Text("image pixel: (%d, %d)", _cur_pixel.x(), _cur_pixel.y() ) ;
        //ImGui::Text("pixel ratio: 1 : %d", ratio ) ;

        ImGui::EndTooltip() ;
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

    // handle mouse dragging
    {
        size_t idx = size_t(-1) ;
        for( size_t i=0; i<ss.bounds.size(); ++i )
        {
            if( this_t::is_ip_mouse_in_bound( ss.bounds[i] ) )
            {
                idx = i ;
                break ;
            }
        }

        if( in_cr && !_mouse_down_rect && idx != size_t(-1) )
        {
            if( io.MouseDown[0] )
            {
                // start dragging
                if( !_mouse_down_drag )
                {
                    _mouse_down_drag = true ;
                    _drag_begin = ip ;
                }
                // do dragging
                else
                {
                    auto dif = natus::math::vec2i_t( ip ) - natus::math::vec2i_t( _drag_begin ) ;

                    auto rect = natus::math::vec4ui_t( ss.bounds[idx] ) ;

                    auto xy0 = natus::math::vec2i_t( rect.xy() ) + dif ;
                    if( xy0.x() < 0 ) dif.x( dif.x() + -xy0.x() ) ;
                    if( xy0.y() < 0 ) dif.y( dif.y() + -xy0.y() ) ;
                    xy0 = natus::math::vec2i_t( rect.xy() ) + dif ;

                    auto const xy1 = rect.zw() + dif ;

                    rect = natus::math::vec4ui_t( xy0, xy1 ) ;

                    ss.bounds[ idx ] = rect ;

                    _drag_begin = ip ;
                }
            }
            else if( io.MouseReleased[0] && _mouse_down_drag )
            {
                _mouse_down_drag = false ;
            }
        }
        else if( io.MouseReleased[0] && _mouse_down_drag ) 
        {
            _mouse_down_drag = false ;
        }
    }
    

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


bool_t sprite_editor::handle_rect( natus::tool::imgui_view_t imgui, natus::math::vec4ui_ref_t res  ) 
{
    if( _mouse_down_drag ) return false;

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
        
    if( in_cr && io.MouseDown[0] && !_mouse_down_rect )
    {
        _mouse_down_rect = true ;
        _cur_rect = natus::math::vec4ui_t( _cur_pixel, _cur_pixel ) ;
    }

    else if( io.MouseDown[0] )
    {        
        _cur_rect.zw( _cur_pixel ) ;
        
        return false ;
    }

    else if( io.MouseReleased[0] && _mouse_down_rect )
    {
        _mouse_down_rect = false ;
        _cur_rect.zw( _cur_pixel ) ;

        res = this_t::rearrange_mouse_rect( _cur_rect ) ;
        _cur_rect = natus::math::vec4ui_t() ;

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
    ImGui::GetWindowDrawList()->AddRectFilled( a_, b_, IM_COL32( color.x(), color.y(), color.z(), color.w() * 0.5f ) ) ;
}

void_t sprite_editor::draw_rect_for_scale( natus::math::vec4f_cref_t rect, natus::math::vec4ui_cref_t color ) 
{
    ImVec2 const a_( rect.x(), rect.y() + 1 ) ;
    ImVec2 const b_( rect.x() + rect.z() + 1, rect.y() - rect.w() + 1 ) ;

    ImGui::GetWindowDrawList()->AddRectFilled( a_, b_, IM_COL32( color.x(), color.y(), color.z(), color.w() ) ) ;
}

void_t sprite_editor::draw_scales( natus::math::vec4f_cref_t rect, natus::math::vec4ui_cref_t color ) 
{
    float_t const wh = float_t( _pixel_ratio ) * 1.0f ;

    // bottom left
    {
        natus::math::vec4f_t r( rect.x(), rect.y(), wh, wh ) ;
        if( this_t::is_window_rect_inside_content_region( r, false ) )
        {
            this_t::draw_rect_for_scale( r, color ) ;
        }
    }
    // top left
    {
        natus::math::vec4f_t r( rect.x(), rect.y()-rect.w()+wh, wh, wh ) ;
        if( this_t::is_window_rect_inside_content_region( r, false ) )
        {
            this_t::draw_rect_for_scale( r, color ) ;
        }
    }

    // top right
    {
        natus::math::vec4f_t r( rect.x()+rect.z()-wh, rect.y()-rect.w()+wh, wh, wh ) ;
        if( this_t::is_window_rect_inside_content_region( r, false ) )
        {
            this_t::draw_rect_for_scale( r, color ) ;
        }
    }

    // bottom right
    {
        natus::math::vec4f_t r( rect.x()+rect.z()-wh, rect.y(), wh, wh ) ;
        if( this_t::is_window_rect_inside_content_region( r, false ) )
        {
            this_t::draw_rect_for_scale( r, color ) ;
        }
    }
}

bool_t sprite_editor::is_ip_mouse_in_bound( natus::math::vec4ui_cref_t rect ) const 
{
    if( uint_t( _cur_pixel.x() ) < rect.x() || uint_t( _cur_pixel.x() ) > rect.z() ) return false ;
    if( uint_t( _cur_pixel.y() ) < rect.y() || uint_t( _cur_pixel.y() ) > rect.w() ) return false ;
    return true ;
}