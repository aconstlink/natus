
#pragma once

#include <natus/memory/global.h>
#include <natus/memory/allocator.hpp>

#include <natus/ntd/vector.hpp>
#include <natus/ntd/string.hpp>

#include <natus/math/vector/vector2.hpp>
#include <natus/math/vector/vector3.hpp>
#include <natus/math/vector/vector4.hpp>

#include <natus/core/assert.h>
#include <natus/core/types.hpp>
#include <natus/core/macros/typedef.h>

namespace natus
{
    namespace geometry
    {
        using namespace natus::core ;

        natus_typedefs( natus::ntd::string_t, string ) ;
        natus_typedefs( natus::ntd::string_t, key ) ;

        natus_typedefs( natus::ntd::vector< uint_t >, uints ) ;
        natus_typedefs( natus::ntd::vector< uints_t >, more_uints ) ;

        natus_typedefs( natus::ntd::vector< float_t >, floats ) ;
        natus_typedefs( natus::ntd::vector< floats_t >, more_floats ) ;
    }
     
}