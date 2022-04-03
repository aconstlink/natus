#pragma once

#include "api.h"
#include "typedefs.h"

#include "module_factory.hpp"

#include <natus/log/global.h>
#include <natus/concurrent/mrsw.hpp>
#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace format
    {
        class NATUS_FORMAT_API module_registry
        {
            natus_this_typedefs( module_registry ) ;

        private: 

            struct data
            {
                data( void_t ) {}
                data( data const & rhv ) : ext( rhv.ext ), fac(rhv.fac) {}
                data( data && rhv ) : ext( std::move(rhv.ext) ), fac(std::move(rhv.fac)) {}
                ~data( void_t ) {}
                data& operator = ( data const& rhv ) { ext = rhv.ext ; fac = rhv.fac ; return *this ; }
                data& operator = ( data && rhv ) { ext = std::move(rhv.ext) ; fac = std::move(rhv.fac) ; return *this ; }
                natus::ntd::string_t ext ;
                natus::format::imodule_factory_res_t fac ;
            };
            natus_typedef( data ) ;
            natus_typedefs( natus::ntd::vector< data_t >, factories ) ;
            factories_t _imports ;
            factories_t _exports ;

            natus::concurrent::mrsw_t _ac ;

        public:

            module_registry( void_t ) {}
            module_registry( this_cref_t ) = delete ;
            module_registry( this_rref_t rhv ) 
            {
                _imports = std::move( rhv._imports ) ;
                _exports = std::move( rhv._exports ) ;
            }
            this_ref_t operator = ( this_cref_t ) = delete ;
            this_ref_t operator = ( this_rref_t rhv ) 
            {
                _imports = std::move( rhv._imports ) ;
                _exports = std::move( rhv._exports ) ;
                return *this ;
            }
            ~module_registry( void_t ) {}

        public:

            bool_t register_import_factory( natus::ntd::vector< natus::ntd::string_t > const & exts, natus::format::imodule_factory_res_t fac ) noexcept
            {
                for( auto const & ext : exts )
                {
                    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;

                    this_t::data_t d ;
                    d.ext = ext ;
                    d.fac = fac ;

                    // add to the front so newer modules will be preferred.
                    _imports.insert( _imports.begin(), std::move( d ) ) ;
                }

                return true ;
            }

            bool_t register_export_factory( natus::ntd::vector< natus::ntd::string_t > const& exts, natus::format::imodule_factory_res_t fac ) noexcept
            {
                for( auto const& ext : exts )
                {
                    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;
                    
                    this_t::data_t d ;
                    d.ext = ext ;
                    d.fac = fac ;

                    // add to the front so newer modules will be preferred.
                    _exports.insert( _exports.begin(), std::move( d ) ) ;
                }

                return true ;
            }

        public:

            natus::format::future_item_t import_from( natus::io::location_cref_t loc, natus::io::database_res_t db ) noexcept
            {
                natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;

                auto fac = this_t::get_import_factory( loc ) ;
                if( !fac.is_valid() )
                {
                    return std::async( std::launch::deferred, [&] ( void_t )
                    {
                        natus::log::global_t::warning( "Can not create factory for extension : " + loc.extension() ) ;
                        return natus::format::item_res_t( natus::format::status_item_t( "Can not create factory for extension : " + loc.extension() ) ) ;
                    } ) ;
                }
                
                return fac->create_module( loc.extension() )->import_from( loc, db ) ;
            }

            natus::format::future_item_t import_from( natus::io::location_cref_t loc, natus::io::database_res_t db, natus::property::property_sheet_res_t ps ) noexcept
            {
                natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;

                auto fac = this_t::get_import_factory( loc ) ;
                if( !fac.is_valid() )
                {
                    return std::async( std::launch::deferred, [&] ( void_t )
                    {
                        natus::log::global_t::warning( "Can not create factory for extension : " + loc.extension() ) ;
                        return natus::format::item_res_t( natus::format::status_item_t( "Can not create factory for extension : " + loc.extension() ) ) ;
                    } ) ;
                }
                
                return fac->create_module( loc.extension() )->import_from( loc, db, ps ) ;
            }

            natus::format::future_item_t export_to( natus::io::location_cref_t loc, natus::io::database_res_t db, natus::format::item_res_t what ) noexcept
            {
                natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;

                auto fac = this_t::get_export_factory( loc ) ;
                if( !fac.is_valid() )
                {
                    return std::async( std::launch::deferred, [&] ( void_t )
                    {
                        natus::log::global_t::warning( "Can not create factory for extension : " + loc.extension() ) ;
                        return natus::format::item_res_t( natus::format::status_item_t( "Can not create factory for extension : " + loc.extension() ) ) ;
                    } ) ;
                }
                
                return fac->create_module( loc.extension() )->export_to( loc, db, what ) ;
            }

        private:

            natus::format::imodule_factory_res_t get_import_factory( natus::io::location_cref_t loc ) const noexcept
            {
                auto iter = std::find_if( _imports.begin(), _imports.end(), [&] ( this_t::data_cref_t d ) 
                { 
                    return d.ext == loc.extension(false) ;
                } ) ;
                if( iter == _imports.end() )
                    return natus::format::imodule_factory_res_t() ;

                return iter->fac ;
            }

            natus::format::imodule_factory_res_t get_export_factory( natus::io::location_cref_t loc ) const noexcept
            {
                auto iter = std::find_if( _exports.begin(), _exports.end(), [&] ( this_t::data_cref_t d ) 
                { 
                    return d.ext == loc.extension(false) ;
                } ) ;
                if( iter == _exports.end() )
                    return natus::format::imodule_factory_res_t() ;

                return iter->fac ;
            }
        };
        natus_res_typedef( module_registry ) ;
    }
}