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
                natus::ntd::string_t ext ;
                natus::format::imodule_factory_res_t fac ;
            };
            natus_typedef( data ) ;
            natus_typedefs( natus::ntd::vector< data_t >, factories ) ;
            factories_t _factories ;

            natus::concurrent::mrsw_t _ac ;

        public:

            module_registry( void_t ) {}
            module_registry( this_cref_t ) = delete ;
            module_registry( this_rref_t rhv ) 
            {
                _factories = std::move( rhv._factories ) ;
            }
            this_ref_t operator = ( this_cref_t ) = delete ;
            this_ref_t operator = ( this_rref_t rhv ) 
            {
                _factories = std::move( rhv._factories ) ;
                return *this ;
            }
            ~module_registry( void_t ) {}

        public:

            bool_t register_factory( natus::ntd::string_cref_t ext, natus::format::imodule_factory_res_t fac ) noexcept
            {
                natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;
                auto iter = std::find_if( _factories.begin(), _factories.end(), [&]( this_t::data_cref_t d )
                {
                    return d.ext == ext ;
                } ) ;

                if( iter != _factories.end() ) 
                {
                    natus::log::global_t::warning( "[module_registry] : extension already registered : " + ext ) ;
                    return false ;
                }

                this_t::data_t d ;
                d.ext = ext ;
                d.fac = fac ;

                _factories.emplace_back( std::move( d ) ) ;

                return true ;
            }
        };
        natus_res_typedef( module_registry ) ;
    }
}