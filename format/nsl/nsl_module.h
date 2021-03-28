#pragma once

#include "../imodule.h"
#include "../module_factory.hpp"
#include "../module_registry.hpp"

#include <natus/nsl/parser_structs.hpp>
#include <natus/graphics/object/shader_object.h>

namespace natus
{
    namespace format
    {
        struct NATUS_FORMAT_API nsl_module_register
        {
            static void_t register_module( natus::format::module_registry_res_t reg ) ;
        };

        class NATUS_FORMAT_API nsl_module : public imodule
        {
            natus_this_typedefs( nsl_module ) ;

        public:

            virtual ~nsl_module( void_t ) {}

            virtual natus::format::future_item_t import_from( natus::io::location_cref_t loc, 
                natus::io::database_res_t ) noexcept ;

            virtual natus::format::future_item_t import_from( natus::io::location_cref_t loc, 
                natus::io::database_res_t, natus::property::property_sheet_res_t ) noexcept ;

        };
        natus_typedef( nsl_module ) ;
        typedef natus::format::module_factory<nsl_module> nsl_factory_t ;
        typedef natus::memory::res_t< nsl_factory_t > nsl_factory_res_t ;

        struct nsl_item : public item
        {
            nsl_item( void_t ) {}
            nsl_item( natus::nsl::post_parse::document_t g ) : doc( g ){}

            natus::nsl::post_parse::document_t doc ;
        };
        natus_res_typedef( nsl_item ) ;
    }
}
