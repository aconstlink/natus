
#include "async.h"

using namespace natus::gpu ;

//****
natus::gpu::result async_backend::prepare( natus::gpu::render_configuration_in_t, 
    natus::gpu::completion_rptr_t ) noexcept 
{
    return natus::gpu::result::failed ;
}

//****
natus::gpu::result async_backend::prepare( natus::gpu::id_rref_t, 
    natus::gpu::render_configuration_in_t, natus::gpu::completion_rptr_t ) noexcept
{
    return natus::gpu::result::failed ;
}

//****
natus::gpu::result async_backend::render( natus::gpu::id_rref_t,
    natus::gpu::completion_rptr_t ) noexcept 
{
    return natus::gpu::result::failed ;
}