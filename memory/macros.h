
#include "res.hpp"

#include <natus/core/macros/typedef.h>

#define natus_res_typedefs( from_, to_ )                   \
    natus_typedefs( from_, to_ ) ;                   \
    typedef natus::memory::res_t< from_ > to_ ## _res_t 

#define natus_res_typedef( what_ ) natus_res_typedefs( what_, what_ )