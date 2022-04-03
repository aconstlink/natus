#pragma once

#include "../imodule.h"
#include "../module_factory.hpp"
#include "../module_registry.hpp"

#include "natus_structs.h"

namespace natus
{
    namespace format
    {
        struct NATUS_FORMAT_API natus_module_register
        {
            static void_t register_module( natus::format::module_registry_res_t reg ) ;
        };

        class NATUS_FORMAT_API natus_module : public imodule
        {
            natus_this_typedefs( natus_module ) ;

        public:

            virtual ~natus_module( void_t ) {}

            virtual natus::format::future_item_t import_from( natus::io::location_cref_t loc, 
                natus::io::database_res_t ) noexcept ;

            virtual natus::format::future_item_t import_from( natus::io::location_cref_t loc, 
                natus::io::database_res_t, natus::property::property_sheet_res_t ) noexcept ;

            virtual natus::format::future_item_t export_to( natus::io::location_cref_t loc, 
                natus::io::database_res_t, natus::format::item_res_t ) noexcept ;

        };
        natus_typedef( natus_module ) ;
        typedef natus::format::module_factory<natus_module> natus_factory_t ;
        typedef natus::memory::res_t< natus_factory_t > natus_factory_res_t ;

        struct natus_item : public item
        {
            natus_item( void_t ) {}
            natus_item( natus::format::natus_document_rref_t g ) : doc( std::move( g ) ){}

            natus::format::natus_document_t doc ;
        };
        natus_res_typedef( natus_item ) ;
    }
}
