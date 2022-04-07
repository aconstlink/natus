
#include "natus_module.h"

#include <natus/io/database.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/graphics/texture/image.hpp>
#include <natus/math/vector/vector4.hpp>

#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>

#include <array>
#include <sstream>

using namespace natus::format ;

namespace this_file
{
    template< typename T, size_t N >
    static bool_t parse( natus::ntd::string_cref_t what, std::array< T, N > & ret  ) noexcept
    {
        size_t n = 0 ;
        size_t l = 0 ;
        size_t p = 0 ;  ;
        
        while( ( p = what.find_first_of( ' ', l ) )  != std::string::npos && n < N )
        {
            ret[ n++ ] = T(std::stoi( what.substr( l, p - l ) )) ;
            
            l = p + 1;
            //p = what.find_first_of( ' ', p + 1 ) ;
        }

        if( n < N ) ret[ n ] = T(std::stoi( what.substr( l ) )) ;

        return n == N ;
    }
}

// ***
void_t natus_module_register::register_module( natus::format::module_registry_res_t reg )
{
    reg->register_import_factory( { "natus" }, natus_factory_res_t( natus_factory_t() ) ) ;
    reg->register_export_factory( { "natus" }, natus_factory_res_t( natus_factory_t() ) ) ;
}

// ***
natus::format::future_item_t natus_module::import_from( natus::io::location_cref_t loc, natus::io::database_res_t db ) noexcept
{
    return this_t::import_from( loc, db, natus::property::property_sheet_t() ) ;
}

// ***
natus::format::future_item_t natus_module::import_from( natus::io::location_cref_t loc, 
                natus::io::database_res_t db, natus::property::property_sheet_res_t ) noexcept 
{
    return std::async( std::launch::async, [=] ( void_t )
    {
        natus_document_t nd ;

        natus::memory::malloc_guard<char> content ;
        natus::io::database_t::cache_access_t ca = db->load( loc ) ;
        {
            
            auto const res = ca.wait_for_operation( [&] ( char_cptr_t data, size_t const sib )
            {
                natus::ntd::string_t file = natus::ntd::string_t( data, sib ) ;
                content = natus::memory::malloc_guard<char>( file.c_str(), file.size()+1 ) ;
            } ) ;

            if( !res )
            {
                natus::log::global_t::warning( "[natus_module] : failed to load file [" + loc.as_string() + "]" ) ;

                return natus::format::item_res_t( natus::format::status_item_res_t(
                    natus::format::status_item_t( "failed to load file" ) ) ) ;
            }

            rapidxml::xml_document<> doc ;
            doc.parse<0>( content.get() ) ;

            rapidxml::xml_node<> * root = doc.first_node( "natus" ) ;
            if( root == nullptr )
            {
                natus::ntd::string_t msg = "[natus_module] : can not find node [natus] in document [" +
                    loc.as_string() + "]" ;

                natus::log::global_t::warning( msg ) ;

                return natus::format::item_res_t( natus::format::status_item_res_t(
                    natus::format::status_item_t( msg ) ) ) ;
            }

            // handle sprite sheets
            for( rapidxml::xml_node<> * sss_xml = root->first_node( "sprite_sheets" ); sss_xml != nullptr; 
                    sss_xml = sss_xml->next_sibling() )
            {
                for( rapidxml::xml_node<> * ss_xml = sss_xml->first_node("sprite_sheet");
                    ss_xml != nullptr; ss_xml = ss_xml->next_sibling() )
                {
                    using sprite_sheet_t = natus::format::natus_document_t::sprite_sheet_t ;
                    sprite_sheet_t ss ;

                    // handle attributes
                    {
                        rapidxml::xml_attribute<> * attr = ss_xml->first_attribute( "name" ) ;
                        if( attr == nullptr )
                        {
                            natus::ntd::string_t msg = "[natus_module] : node [sprite_sheets] "
                                "requires [name] attribute in document [" + loc.as_string() + "]" ;

                            natus::log::global_t::warning( msg ) ;
                            continue ;
                        }
                        ss.name = natus::ntd::string_t( attr->value() ) ;
                    }

                    // image
                    {
                        sprite_sheet_t::image_t image ;

                        rapidxml::xml_node<> * image_xml = ss_xml->first_node( "image" ) ;
                        if( image_xml == nullptr )
                        {
                            natus::ntd::string_t msg = "[natus_module] : node [sprite_sheets] "
                                "requires [image] node in document [" + loc.as_string() + "]" ;
                            natus::log::global_t::warning( msg ) ;
                            continue ;
                        }

                        rapidxml::xml_attribute<> * attr = image_xml->first_attribute( "src" ) ;
                        if( attr == nullptr )
                        {
                            natus::ntd::string_t msg = "[natus_module] : node [image] "
                                "requires [src] attribute in document [" + loc.as_string() + "]" ;

                            natus::log::global_t::warning( msg ) ;
                            continue ;
                        }

                        image.src = natus::ntd::string_t( attr->value() ) ;

                        ss.image = std::move( image ) ;
                    }

                    // sprites
                    {
                        rapidxml::xml_node<> * sprites_xml = ss_xml->first_node( "sprites" ) ;
                        if( sprites_xml != nullptr )
                        {
                            for( rapidxml::xml_node<> * sprite_xml = sprites_xml->first_node("sprite"); 
                                sprite_xml != nullptr; sprite_xml = sprite_xml->next_sibling() )
                            {
                                sprite_sheet_t::sprite_t sprite ;

                                // attributes
                                {
                                    rapidxml::xml_attribute<> * attr = sprite_xml->first_attribute( "name" ) ;
                                    if( attr == nullptr ) 
                                    {
                                        natus::ntd::string_t msg = "[natus_module] : node [sprite] "
                                        "requires [name] attribute in document [" + loc.as_string() + "]" ;
                                        continue ;
                                    }
                                    sprite.name = natus::ntd::string_t( attr->value() ) ;
                                }

                                // animation
                                {
                                    sprite_sheet_t::sprite_t::animation_t animation ;
                                    rapidxml::xml_node<> * node = sprite_xml->first_node("animation") ;
                                    if( node != nullptr )
                                    {
                                        // attribute rect
                                        {
                                            rapidxml::xml_attribute<> * attr = node->first_attribute( "rect" ) ;
                                            if( attr == nullptr ) 
                                            {
                                                natus::ntd::string_t msg = "[natus_module] : node [animation] "
                                                "requires [rect] attribute in document [" + loc.as_string() + "]" ;
                                                continue ;
                                            }

                                            std::array< uint_t, 4 > parsed ;
                                            this_file::parse( natus::ntd::string_t(attr->value()), parsed ) ;
                                            animation.rect = natus::math::vec4ui_t( parsed[0], parsed[1],
                                                    parsed[2], parsed[3] ) ;
                                        }

                                        // attribute pivot
                                        {
                                            rapidxml::xml_attribute<> * attr = node->first_attribute( "pivot" ) ;
                                            if( attr != nullptr ) 
                                            {
                                                std::array< int_t, 2 > parsed ;
                                                this_file::parse( natus::ntd::string_t(attr->value()), parsed ) ;
                                                animation.pivot = natus::math::vec2i_t( parsed[0], parsed[1] ) ;
                                            }
                                        }
                                    }
                                    sprite.animation = std::move( animation ) ;
                                }

                                // collision
                                {
                                    sprite_sheet_t::sprite_t::collision_t collision ;
                                    rapidxml::xml_node<> * node = sprite_xml->first_node("collision") ;
                                    if( node != nullptr )
                                    {
                                        // attribute rect
                                        {
                                            rapidxml::xml_attribute<> * attr = node->first_attribute( "rect" ) ;
                                            if( attr != nullptr ) 
                                            {
                                                std::array< uint_t, 4 > parsed ;
                                                this_file::parse( natus::ntd::string_t(attr->value()), parsed ) ;
                                                collision.rect = natus::math::vec4ui_t( parsed[0], parsed[1],
                                                    parsed[2], parsed[3] ) ;
                                            }
                                        }
                                    }
                                    sprite.collision = std::move( collision ) ;
                                }

                                // hits
                                {
                                    rapidxml::xml_node<> * node = sprite_xml->first_node("hits") ;
                                    if( node != nullptr )
                                    {
                                        for( rapidxml::xml_node<> * hit = node->first_node("hit"); hit != nullptr; 
                                            hit = hit->next_sibling() )
                                        {
                                            sprite_sheet_t::sprite_t::hit_t h ;

                                            // attribute: name
                                            {
                                                rapidxml::xml_attribute<> * attr = hit->first_attribute( "name" ) ;
                                                if( attr != nullptr )
                                                {
                                                    h.name = natus::ntd::string_t( attr->value() ) ;
                                                }
                                            }

                                            // attribute: rect
                                            {
                                                rapidxml::xml_attribute<> * attr = hit->first_attribute( "rect" ) ;
                                                if( attr != nullptr )
                                                {
                                                    std::array< uint_t, 4 > parsed ;
                                                    this_file::parse( natus::ntd::string_t(attr->value()), parsed ) ;
                                                    h.rect = natus::math::vec4ui_t( parsed[0], parsed[1],
                                                        parsed[2], parsed[3] ) ;
                                                }
                                            }

                                            sprite.hits.emplace_back( std::move( h ) ) ;
                                        }
                                    }
                                }

                                ss.sprites.emplace_back( std::move( sprite ) ) ;
                            }
                        }
                    }

                    // animations
                    {
                        rapidxml::xml_node<> * animations_xml = ss_xml->first_node( "animations" ) ;
                        if( animations_xml != nullptr ) 
                        {
                            for( rapidxml::xml_node<> * ani_xml = animations_xml->first_node("animation"); 
                                ani_xml != nullptr; ani_xml = ani_xml->next_sibling() )
                            {
                                sprite_sheet_t::animation_t animation ;

                                // attribute: object
                                {
                                    rapidxml::xml_attribute<> * attr = ani_xml->first_attribute( "object" ) ;
                                    if( attr != nullptr )
                                    {
                                        animation.object = natus::ntd::string_t( attr->value() ) ;
                                    }
                                }

                                // attribute: name
                                {
                                    rapidxml::xml_attribute<> * attr = ani_xml->first_attribute( "name" ) ;
                                    if( attr != nullptr )
                                    {
                                        animation.name = natus::ntd::string_t( attr->value() ) ;
                                    }
                                }

                                for( rapidxml::xml_node<> * frame_xml = ani_xml->first_node("frame") ; 
                                frame_xml != nullptr; frame_xml = frame_xml->next_sibling() )
                                {
                                    sprite_sheet_t::animation_t::frame_t frame ;

                                    // attribute: sprite
                                    {
                                        rapidxml::xml_attribute<> * attr = frame_xml->first_attribute( "sprite" ) ;
                                        if( attr != nullptr )
                                        {
                                            frame.sprite = natus::ntd::string_t( attr->value() ) ;
                                        }
                                    }

                                    // attribute: duration
                                    {
                                        rapidxml::xml_attribute<> * attr = frame_xml->first_attribute( "duration" ) ;
                                        if( attr != nullptr )
                                        {
                                            frame.duration = std::stol( attr->value() ) ;
                                        }
                                    }

                                    animation.frames.emplace_back( std::move( frame ) ) ;
                                }

                                ss.animations.emplace_back( std::move( animation ) ) ;
                            }
                        }
                    }

                    nd.sprite_sheets.emplace_back( std::move( ss ) ) ;
                }
            }
        }

        return natus::format::item_res_t( natus::format::natus_item_res_t(
             natus::format::natus_item_t( std::move( nd ) ) ) ) ;
    } ) ;
}

natus::format::future_item_t natus_module::export_to( natus::io::location_cref_t loc, 
                natus::io::database_res_t db, natus::format::item_res_t what ) noexcept 
{
    return std::async( std::launch::async, [=] ( void_t )
    {
        natus::format::natus_item_res_t ntm = what ;
        if( !ntm.is_valid() )
        {
            return natus::format::item_res_t( natus::format::status_item_res_t(
                natus::format::status_item_t( "Invalid item type. Natus item expected." ) ) ) ;
        }

        auto const & ndoc = ntm->doc ;
        rapidxml::xml_document<> doc ;

        auto * root = doc.allocate_node( rapidxml::node_element, "natus" ) ;
        
        {
            auto * rsheets = doc.allocate_node( rapidxml::node_element, "sprite_sheets" ) ;
            
            for( auto const & ss : ndoc.sprite_sheets )
            {
                auto * rsheet = doc.allocate_node( rapidxml::node_element, "sprite_sheet" ) ;
                {
                    rsheet->append_attribute( doc.allocate_attribute( "name", ss.name.c_str() ) ) ;
                }

                {
                    auto * img = doc.allocate_node( rapidxml::node_element, "image" ) ;
                    img->append_attribute( doc.allocate_attribute( "src", ss.image.src.c_str() ) ) ;
                    rsheet->append_node( img ) ;
                }

                // sprites
                {
                    auto * n = doc.allocate_node( rapidxml::node_element, "sprites" ) ;
                    for( auto const & s : ss.sprites )
                    {
                        auto * n2 = doc.allocate_node( rapidxml::node_element, "sprite" ) ;
                        {
                            n2->append_attribute( doc.allocate_attribute( "name", s.name.c_str() ) ) ;
                            {
                                auto * n3 = doc.allocate_node( rapidxml::node_element, "animation" ) ;
                                {
                                    auto const value = std::to_string( s.animation.rect.x() ) + " " +
                                        std::to_string( s.animation.rect.y() ) + " " + 
                                        std::to_string( s.animation.rect.z() ) + " " + 
                                        std::to_string( s.animation.rect.w() ) ;

                                    n3->append_attribute( doc.allocate_attribute( "rect", doc.allocate_string( value.c_str() ) ) ) ;
                                }
                                {
                                    auto const value = std::to_string( s.animation.pivot.x() ) + " " +
                                        std::to_string( s.animation.pivot.y() ) ;

                                    n3->append_attribute( doc.allocate_attribute( "pivot", doc.allocate_string( value.c_str() ) ) ) ;
                                }
                                n2->append_node( n3 ) ;
                            }
                            {
                                auto * n3 = doc.allocate_node( rapidxml::node_element, "collision" ) ;
                                {
                                    auto const value = std::to_string( s.collision.rect.x() ) + " " +
                                        std::to_string( s.collision.rect.y() ) + " " + 
                                        std::to_string( s.collision.rect.z() ) + " " + 
                                        std::to_string( s.collision.rect.w() ) ;

                                    n3->append_attribute( doc.allocate_attribute( "rect", doc.allocate_string( value.c_str() ) ) ) ;
                                }
                                n2->append_node( n3 ) ;
                            }
                            {
                                auto * n3 = doc.allocate_node( rapidxml::node_element, "hits" ) ;
                                for( auto const & h : s.hits )
                                {
                                    auto * n4 = doc.allocate_node( rapidxml::node_element, "hit" ) ;
                                    {
                                        n4->append_attribute( doc.allocate_attribute( "name", h.name.c_str() ) ) ;
                                    }
                                    {
                                        auto const value = std::to_string( h.rect.x() ) + " " +
                                            std::to_string( h.rect.y() ) + " " + 
                                            std::to_string( h.rect.z() ) + " " + 
                                            std::to_string( h.rect.w() ) ;

                                        n4->append_attribute( doc.allocate_attribute( "rect", doc.allocate_string( value.c_str() ) ) ) ;
                                    }
                                    n3->append_node( n4 ) ;
                                }
                                n2->append_node( n3 ) ;
                            }
                            n->append_node( n2 ) ;
                        }
                    }
                    rsheet->append_node( n ) ;
                }

                // animations
                {
                    auto * n = doc.allocate_node( rapidxml::node_element, "animations" ) ;
                    for( auto const & a : ss.animations )
                    {
                        auto * ani = doc.allocate_node( rapidxml::node_element, "animation" ) ;
                        {
                            ani->append_attribute( doc.allocate_attribute( "name", doc.allocate_string( a.name.c_str() ) ) ) ;
                        }

                        for( auto const & f : a.frames )
                        {
                            auto * fr = doc.allocate_node( rapidxml::node_element, "frame" ) ;
                            {
                                fr->append_attribute( doc.allocate_attribute( "sprite", f.sprite.c_str() ) ) ;
                            }
                            {
                                fr->append_attribute( doc.allocate_attribute( "duration", doc.allocate_string( std::to_string( f.duration ).c_str() ) ) ) ;
                            }
                            ani->append_node( fr ) ;
                        }
                        n->append_node( ani ) ;
                    }
                    rsheet->append_node( n ) ;
                }
                rsheets->append_node( rsheet ) ;
            }
            root->append_node( rsheets ) ;
        }

        doc.append_node( root ) ;

        std::stringstream ss ;
        ss << doc ;

        ntd::string_t output = ss.str() ;
        
        // tabs to spaces
        {
            size_t o = 0 ;
            while( true )
            {
                auto const a = output.find_first_of( '\t', o ) ;
                if( a == std::string::npos ) break ;

                output.replace( a, 1, "    " ) ;
                o = a ;
            }
        }
        auto const res = db->store( loc.as_string(), output.c_str(), output.size() ).wait_for_operation() ;

        return natus::format::item_res_t( natus::format::status_item_res_t(
                natus::format::status_item_t( res == natus::io::result::ok ? "Exported natus document" : "Failed to export natus document ") ) ) ;
    } ) ;
}