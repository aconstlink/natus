
#include "res.hpp"
#include "rptr.hpp"

#include <natus/core/macros/typedef.h>

#define natus_soil_typedefs( from_, to_ )                   \
    natus_typedefs( from_, to_ ) ;                   \
    typedef natus::soil::res< from_ > to_ ## _res_t ;        \
    typedef natus::soil::rptr< from_ * > to_ ## _rptr_t 

#define natus_soil_typedef( what_ ) natus_soil_typedefs( what_, what_ )


#define natus_soil_class_proto_typedefs(x)          \
    natus_class_proto_typedefs(x) ;                 \
    typedef natus::soil::rptr< x * > x ## _rptr_t 