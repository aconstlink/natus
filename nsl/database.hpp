#pragma once

#include "typedefs.h"
#include "parser_structs.hpp"

namespace natus
{
    namespace nsl
    {
        class database
        {
            natus_this_typedefs( database ) ;

        private:

            natus::nsl::post_parse::library_t::variables_t _vars ;
            natus::nsl::post_parse::library_t::fragments_t _shaders ;

            #if 0
            template< typename T >
            struct data
            {
                natus::ntd::vector< natus::nsl::symbol_t > deps ;
                T data ;
            };
            natus_typedefs( data< natus::nsl::pregen::config_t >, config_data ) ;
            natus_typedefs( data< natus::nsl::pregen::library_t >, lib_data ) ;

            natus::ntd::vector< config_data_t > _configs ;
            natus::ntd::vector< config_data_t > _libs ;
            #endif

        public:

            database( void_t ) noexcept
            {
            }


            database( this_cref_t ) = delete ;

            database( this_rref_t ) noexcept
            {
            }

            ~database( void_t ) noexcept
            {
            }

        public:

            void_t insert( natus::nsl::post_parse::document_rref_t doc ) noexcept
            {

                for( auto & l : doc.libraries )
                {

                }
            }

        };
        natus_res_typedef( database ) ;
    }
}