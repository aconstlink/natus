
#include "stb_module.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace natus::format ;

// ***
void_t stb_module_register::register_module( natus::format::module_registry_res_t reg ) 
{
    reg->register_import_factory( { "png", "jpg" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;
    reg->register_export_factory( { "png" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;

    reg->register_import_factory( { "ogg" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;
    reg->register_import_factory( { "ttf" }, stb_image_factory_res_t( stb_image_factory_t() ) ) ;
}

// ***
bool_t stb_image_module::import_from( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept 
{
    return false ;
}