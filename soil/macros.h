
#include "res.hpp"

#include <natus/core/macros/typedef.h>

#define natus_soil_typedefs( from_, to_ )                   \
    natus_typedefs( from_, to_ ) ;                   \
    typedef natus::soil::res< from_ > to_ ## _res_t 

#define natus_soil_typedef( what_ ) natus_soil_typedefs( what_, what_ )