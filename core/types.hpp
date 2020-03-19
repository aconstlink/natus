#pragma once

#include "macros/typedef.h"

#include <cstddef>

// including for typedefs of float_t/double_t
#include <cmath>
#include <cstdint>

namespace natus
{
    namespace core
    {
        namespace types
        {
            so_typedefs( unsigned char, byte ) ;
            so_typedefs( char, char ) ;
            so_typedefs( unsigned char, uchar ) ;

            so_typedefs( short, short ) ;
            so_typedefs( unsigned short, ushort ) ;
            so_typedefs( int, int ) ;
            so_typedefs( unsigned int, uint ) ;
            so_typedefs( bool, bool ) ;

            using ::float_t ;
            using ::double_t ;
            so_typedef_extend( float ) ;
            so_typedef_extend( double ) ;


            typedef unsigned long long ulonglong_t ;

            typedef void void_t ;
            typedef void_t* void_ptr_t ;
            typedef void_t const* void_cptr_t ;

            using ::int8_t ;
            so_typedef_extend( int8 ) ;

            using ::int16_t ;
            so_typedef_extend( int16 ) ;

            using ::int32_t ;
            so_typedef_extend( int32 ) ;

            using ::int64_t ;
            so_typedef_extend( int64 ) ;

            using ::uint8_t ;
            so_typedef_extend( uint8 ) ;

            using ::uint16_t ;
            so_typedef_extend( uint16 ) ;

            using ::uint32_t ;
            so_typedef_extend( uint32 ) ;

            using ::uint64_t ;
            so_typedef_extend( uint64 ) ;
        }
    }    
} 


