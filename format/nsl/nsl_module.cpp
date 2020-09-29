
#include "nsl_module.h"

#include <natus/io/database.h>
#include <natus/memory/guards/malloc_guard.hpp>
#include <natus/graphics/texture/image.hpp>
#include <natus/math/vector/vector4.hpp>

#include <natus/nsl/parser.hpp>
#include <natus/graphics/shader/nsl_bridge.hpp>

using namespace natus::format ;


// ***
void_t nsl_module_register::register_module( natus::format::module_registry_res_t reg )
{
    reg->register_import_factory( { "nsl" }, nsl_factory_res_t( nsl_factory_t() ) ) ;
    reg->register_export_factory( { "nsl" }, nsl_factory_res_t( nsl_factory_t() ) ) ;
}

// ***
natus::format::future_item_t nsl_module::import_from( natus::io::location_cref_t loc, natus::io::database_res_t db ) noexcept
{
    return std::async( std::launch::async, [=] ( void_t )
    {
        natus::nsl::post_parse::document_t doc ;

        natus::io::database_t::cache_access_t ca = db->load( loc ) ;
        {
            auto const res = ca.wait_for_operation( [&] ( char_cptr_t data, size_t const sib )
            {
                natus::ntd::string_t file = natus::ntd::string_t( data, sib ) ;
                doc = natus::nsl::parser_t( loc.as_string() ).process( std::move( file ) ) ;
            } ) ;

            if( !res )
            {
                natus::log::global_t::warning( "[nsl_module] : failed to load file [" + loc.as_string() + "]" ) ;

                return natus::format::item_res_t( natus::format::status_item_res_t(
                    natus::format::status_item_t( "failed to load file" ) ) ) ;
            }
        }

        return natus::format::item_res_t( natus::format::nsl_item_res_t(
             natus::format::nsl_item_t( std::move( doc ) ) ) ) ;
    } ) ;
}