
#pragma once

#include "api.h"
#include "typedefs.h"

#include "component.hpp"

#include <natus/std/vector.hpp>

namespace natus
{
    namespace device
    {
        class NATUS_DEVICE_API idevice
        {
        public:

            virtual ~idevice( void_t ) {}
        };
        natus_soil_typedef( idevice ) ;

        template< typename L >
        class device : public idevice
        {
            natus_this_typedefs( device<L> ) ;
            natus_typedefs( L, layout ) ;

        private:

            natus::std::string_t _name ;
            layout_t _layout ;

        public:

            device( void_t ) noexcept 
            {}

            device( natus::std::string_cref_t name ) noexcept : _name( name ) 
            {}

            device( this_cref_t rhv ) = delete ;

            device( this_rref_t rhv ) noexcept
            {
                _name = ::std::move( rhv._name ) ;
                _layout = ::std::move( rhv._layout ) ;
            }

            virtual ~device( void_t ) 
            {}

            this_ref_t operator = ( this_cref_t rhv ) = delete ;

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _name = ::std::move( rhv._name ) ;
                _layout = ::std::move( rhv._layout ) ;
                return *this ;
            }

        public:
            
            layout_ref_t layout( void_t ) 
            {
                return _layout ;
            }
            
        };
    }
}