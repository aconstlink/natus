
#include "stb_module.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace natus::imex ;

natus::ntd::vector< natus::ntd::string_t > const natus::imex::stb_module_t::__formats = 
{ "png", ".png", "jpg", ".jpg", "jpeg", ".jpeg", "gif", ".gif" } ;
natus::ntd::vector< stb_module::import_funk_t > const natus::imex::stb_module_t::__imports = 
{ 
    [=]( stb_module* ptr, natus::ntd::string_cref_t l, natus::io::database_res_t db ) { return ptr->import_image( l, db ) ; },
    [=]( stb_module* ptr, natus::ntd::string_cref_t l, natus::io::database_res_t db ) { return ptr->import_image( l, db ) ; },
    [=] ( stb_module* ptr, natus::ntd::string_cref_t l, natus::io::database_res_t db ) { return ptr->import_image( l, db ) ; },
    [=] ( stb_module* ptr, natus::ntd::string_cref_t l, natus::io::database_res_t db ) { return ptr->import_image( l, db ) ; },
    [=] ( stb_module* ptr, natus::ntd::string_cref_t l, natus::io::database_res_t db ) { return ptr->import_image( l, db ) ; },
    [=] ( stb_module* ptr, natus::ntd::string_cref_t l, natus::io::database_res_t db ) { return ptr->import_image( l, db ) ; },
    [=] ( stb_module* ptr, natus::ntd::string_cref_t l, natus::io::database_res_t db ) { return ptr->import_image( l, db ) ; },
    [=] ( stb_module* ptr, natus::ntd::string_cref_t l, natus::io::database_res_t db ) { return ptr->import_image( l, db ) ; },

    [=] ( stb_module*, natus::ntd::string_cref_t, natus::io::database_res_t ) { return false ; }
} ;

// ***
stb_module::stb_module( void_t ) 
{
}

// ***
stb_module::stb_module( this_cref_t )
{
}

// ***
stb_module::stb_module( this_rref_t )
{
}

// ***
stb_module::~stb_module( void_t )
{
}

// ***
natus::ntd::vector< natus::ntd::string_t > const &  stb_module::supported_formats( void_t ) const noexcept
{
    return __formats ;
}

// ***
bool_t stb_module::is_format_supported( natus::ntd::string_cref_t ext ) const noexcept 
{
    auto const formats = this_t::supported_formats() ;
    auto const iter = ::std::find( formats.begin(), formats.end(), ext ) ;
    return iter != formats.end() ;
}

// ***
bool_t stb_module::import( natus::ntd::string_cref_t loc, natus::io::database_res_t db ) noexcept
{
    natus::ntd::string_t ext ;
    if( !db->lookup_extension( loc, ext ) ) return false ;
    if( !this_t::is_format_supported( ext ) ) return false ;

    return this_t::funk_for_extension( ext )( this, loc, db ) ;
}

// ***
bool_t stb_module::import_audio( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept
{
    return false ;
}

// ***
bool_t stb_module::import_image( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept
{
    return false ;
}

// ***
bool_t stb_module::import_font( natus::ntd::string_cref_t loc, natus::io::database_res_t ) noexcept
{
    return false ;
}

// ***
stb_module::import_funk_t stb_module::funk_for_extension( natus::ntd::string_cref_t ext ) noexcept 
{
    auto const iter = ::std::find( __formats.begin(), __formats.end(), ext ) ;
    if( iter == __formats.end() ) return __imports.back() ;

    return __imports[ ::std::distance( __formats.begin(), iter ) ] ;
}