#pragma once 

#include "api.h"
#include "typedefs.h"

#include <natus/io/database.h>

namespace natus
{
    namespace imex
    {
        class NATUS_IMEX_API imodule
        {
        public:

            imodule( void_t ) {}
            virtual ~imodule( void_t ) {}

            virtual natus::std::vector< natus::std::string_t > const &  supported_formats( void_t ) const noexcept = 0 ;
            virtual bool_t import( natus::std::string_cref_t loc, natus::io::database_res_t ) noexcept = 0 ;
        };

        class NATUS_IMEX_API imesh_module : public imodule
        {

        public:

            imesh_module( void_t ) {}
            virtual ~imesh_module( void_t ) {}

        };
        natus_res_typedef( imesh_module ) ;

        class NATUS_IMEX_API iscene_module : public imodule
        {

        public:

            iscene_module( void_t ) {}
            virtual ~iscene_module( void_t ) {}

        };
        natus_res_typedef( iscene_module ) ;

        class NATUS_IMEX_API iaudio_module : public imodule
        {

        public:

            iaudio_module( void_t ) {}
            virtual ~iaudio_module( void_t ) {}

            virtual bool_t import_audio( natus::std::string_cref_t loc, natus::io::database_res_t ) noexcept = 0 ;
        };
        natus_res_typedef( iaudio_module ) ;

        class NATUS_IMEX_API iimage_module : public imodule
        {

        public:

            iimage_module( void_t ) {}
            virtual ~iimage_module( void_t ) {}

            virtual bool_t import_image( natus::std::string_cref_t loc, natus::io::database_res_t ) noexcept = 0 ;
        };
        natus_res_typedef( iimage_module ) ;

        class NATUS_IMEX_API ifont_module : public imodule
        {

        public:

            ifont_module( void_t ) {}
            virtual ~ifont_module( void_t ) {}

            virtual bool_t import_font( natus::std::string_cref_t loc, natus::io::database_res_t ) noexcept = 0 ;

        };
        natus_res_typedef( iimage_module ) ;
    }
}