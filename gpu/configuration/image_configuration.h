#pragma once

#include "../object.hpp"

#include "../texture/image.hpp"

#include <natus/std/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace gpu
    {
        class image_configuration : public object
        {
            natus_this_typedefs( image_configuration ) ;

        private:

            natus::std::string_t _name ;
            natus::gpu::image_t _img ;

        public:

            image_configuration( void_t ) {}
            image_configuration( natus::std::string_in_t name ) : _name( name ){}
            image_configuration( natus::std::string_in_t name, natus::gpu::image_rref_t img) :
                image_configuration( name )
            {
                
                _img = ::std::move( img ) ;
            }

            image_configuration( this_cref_t rhv )
            {
                *this = rhv ;
            }
            
            image_configuration( this_rref_t rhv )
            {
                *this = ::std::move( rhv ) ;
            }

            virtual ~image_configuration( void_t ){}

            this_ref_t operator = ( this_cref_t rhv )
            {
                object::operator=( rhv ) ;

                _name = rhv._name ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                object::operator=( ::std::move( rhv ) ) ;

                _name = ::std::move( rhv._name ) ;

                return *this ;
            }

            natus::std::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }

        };
        natus_soil_typedef( image_configuration ) ;
    }
}