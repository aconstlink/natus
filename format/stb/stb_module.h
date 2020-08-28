#pragma once

#include "../imodule.h"
#include "../module_factory.hpp"
#include "../module_registry.hpp"

namespace natus
{
    namespace format
    {
        struct stb_module_register
        {
            static void_t register_module( natus::format::module_registry_res_t reg ) ;
        };

        class NATUS_FORMAT_API stb_image_module : public imodule
        {
        public:

            virtual ~stb_image_module( void_t ) {}
            virtual bool_t import_from( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept ;
            
        };
        natus_typedef( stb_image_module ) ;
        typedef natus::format::module_factory<stb_image_module> stb_image_factory_t ;
        typedef natus::memory::res< stb_image_factory_t > stb_image_factory_res_t ;

        class NATUS_FORMAT_API stb_audio_module // : public imodule
        {
        };
        natus_typedef( stb_audio_module ) ;
        typedef natus::format::module_factory<stb_audio_module_t> stb_audio_factory_t ;
        typedef natus::memory::res< stb_audio_factory_t > stb_audio_factory_res_t ;

        class NATUS_FORMAT_API stb_font_module //: public imodule
        {
        };
        natus_typedef( stb_font_module ) ;
        typedef natus::format::module_factory<stb_font_module_t> stb_font_factory_t ;
        typedef natus::memory::res< stb_font_factory_t > stb_font_factory_res_t ;
    }
}
