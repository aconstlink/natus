
#include "global.h"
#include "stb/stb_module.h"
#include "nsl/nsl_module.h"
#include "wav/wav_module.h"
#include "natus/natus_module.h"

#include <natus/memory/global.h>

using namespace natus::format ;

global_t::this_ptr_t global_t::_singleton = natus::memory::global_t::alloc( global_t(), "natus::format::global" ) ;

//***
global::global( void_t )
{
    _mr = natus::format::module_registry_t() ;
    this_t::register_default_registries() ;
}

//***
global::global( this_rref_t rhv ) 
{
    _mr = std::move( rhv._mr ) ;
}

//***
global::~global( void_t ) 
{
}

//***
global::this_ptr_t global::get( void_t ) 
{
    return _singleton ;
}

//***
natus::format::module_registry_res_t global::registry( void_t ) noexcept 
{
    return this_t::get()->_mr ;
}

//***
void_t global::register_default_registries( void_t ) 
{
    natus::format::stb_module_register::register_module( _mr ) ;
    natus::format::nsl_module_register::register_module( _mr ) ;
    natus::format::wav_module_register::register_module( _mr ) ;
    natus::format::natus_module_register::register_module( _mr ) ;
}