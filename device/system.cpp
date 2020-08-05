
#include "system.h"

using namespace natus::device ;

//***
system::system( void_t ) 
{
    _vdev = natus::device::vdev_module_t() ;
}

//***
system::system( this_rref_t rhv )
{
    _modules = ::std::move( rhv._modules ) ;
    _vdev = ::std::move( rhv._vdev ) ;
}

//***
system::~system( void_t )
{
}

//***
void_t system::add_module( natus::device::imodule_res_t res ) 
{
    _modules.emplace_back( res ) ;

    // every time a new module is added, 
    // all the vdev should check  all mappings
    _vdev->check_devices( res ) ;
}

//***
void_t system::search( natus::device::imodule::search_funk_t funk ) 
{
    for( auto & mod : _modules )
    {
        mod->search( funk ) ;
    }
    _vdev->search( funk ) ;
}

//***
void_t system::update( void_t )
{
    // update all modules which should update all physicals
    for( auto & res : _modules )
    {
        res->update() ;
    }

    // the virtual device module must update 
    // after all physical devices
    _vdev->update() ;
}

//***