
#include "null_window_handle.h"

using namespace natus::application ;
using namespace natus::application::null ;

//***********************************************************************
null_window_handle::null_window_handle( void_t )
{
}

//***********************************************************************
null_window_handle::null_window_handle( this_rref_t ) 
{
}

//***********************************************************************
null_window_handle::~null_window_handle( void_t )
{

}

//***********************************************************************
null_window_handle::this_ref_t null_window_handle::operator = ( this_rref_t )
{
    return *this ;
}