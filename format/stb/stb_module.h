#pragma once

#include "../imodule.h"

namespace natus
{
    namespace format
    {
        // stb module can import audio, images and fonts
        class NATUS_FORMAT_API stb_module : public imodule
        {
            natus_this_typedefs( stb_module ) ;

        private:

            static const natus::ntd::vector< natus::ntd::string_t > __formats ;

            typedef ::std::function< bool_t ( this_ptr_t, natus::ntd::string_cref_t loc, natus::io::database_res_t ) > import_funk_t ;
            static const natus::ntd::vector< import_funk_t > __imports ;

        public:

            stb_module( void_t ) ;
            stb_module( this_cref_t ) ;
            stb_module( this_rref_t ) ;
            virtual ~stb_module( void_t ) ;

        public:

            virtual natus::ntd::vector< natus::ntd::string_t > const & supported_formats( void_t ) const noexcept ;
            virtual bool_t is_format_supported( natus::ntd::string_cref_t ) const noexcept ;
            virtual bool_t import( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept ;

        private:

            bool_t import_audio( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept ;
            bool_t import_image( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept ;
            bool_t import_font( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept ;

        private:

            import_funk_t funk_for_extension( natus::ntd::string_cref_t ext ) noexcept ;
        };
        natus_res_typedef( stb_module ) ;
    }
}
