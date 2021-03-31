
#include "natus_module.h"

#include <natus/io/database.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/graphics/texture/image.hpp>
#include <natus/math/vector/vector4.hpp>

#include <rapidxml.hpp>
#include <array>

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
                natus::log::global_t::warning( "[nsl_module] : failed to load file [" + loc.as_string() + "]" ) ;

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
                                            if( attr == nullptr ) 
                                            {
                                                natus::ntd::string_t msg = "[natus_module] : node [animation] "
                                                "requires [rect] attribute in document [" + loc.as_string() + "]" ;
                                                continue ;
                                            }

                                            std::array< int_t, 2 > parsed ;
                                            this_file::parse( natus::ntd::string_t(attr->value()), parsed ) ;
                                            animation.pivot = natus::math::vec2i_t( parsed[0], parsed[1] ) ;
                                        }
                                    }
                                    sprite.animation = std::move( animation ) ;
                                }

                                // collision
                                {
                                    rapidxml::xml_node<> * node = sprite_xml->first_node("collision") ;
                                    if( node != nullptr )
                                    {
                                        int bp = 0 ;
                                    }
                                }

                                // hits
                                {
                                    rapidxml::xml_node<> * node = sprite_xml->first_node("hits") ;
                                    if( node != nullptr )
                                    {
                                        for( rapidxml::xml_node<> * hit = node->first_node("hit"); hit != nullptr; 
                                            hit = hit->next_sibling() )
                                        {
                                            // attribute: name
                                            {
                                                rapidxml::xml_attribute<> * attr = hit->first_attribute( "name" ) ;
                                                if( attr != nullptr )
                                                {
                                                }
                                            }

                                            // attribute: rect
                                            {
                                                rapidxml::xml_attribute<> * attr = hit->first_attribute( "rect" ) ;
                                                if( attr != nullptr )
                                                {
                                                }
                                            }
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