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
            natus_typedefs( unsigned char, byte ) ;
            natus_typedefs( char, char ) ;
            natus_typedefs( unsigned char, uchar ) ;

            natus_typedefs( short, short ) ;
            natus_typedefs( unsigned short, ushort ) ;
            natus_typedefs( int, int ) ;
            natus_typedefs( unsigned int, uint ) ;
            natus_typedefs( bool, bool ) ;

            using ::float_t ;
            using ::double_t ;
            natus_typedef_extend( float ) ;
            natus_typedef_extend( double ) ;


            typedef unsigned long long ulonglong_t ;

            typedef void void_t ;
            typedef void_t* void_ptr_t ;
            typedef void_t const* void_cptr_t ;

            using ::int8_t ;
            natus_typedef_extend( int8 ) ;

            using ::int16_t ;
            natus_typedef_extend( int16 ) ;

            using ::int32_t ;
            natus_typedef_extend( int32 ) ;

            using ::int64_t ;
            natus_typedef_extend( int64 ) ;

            using ::uint8_t ;
            natus_typedef_extend( uint8 ) ;

            using ::uint16_t ;
            natus_typedef_extend( uint16 ) ;

            using ::uint32_t ;
            natus_typedef_extend( uint32 ) ;

            using ::uint64_t ;
            natus_typedef_extend( uint64 ) ;
        }
    }    
} 


