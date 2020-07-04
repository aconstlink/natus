
#pragma once

#include "api.h"
#include "typedefs.h"

namespace natus
{
    namespace device
    {
        class component
        {
            natus_this_typedefs( component ) ;

        private:

            natus::std::string_t _name ;
            size_t _id = size_t( -1 ) ;

        public:

            component( void_t ) {}
            component( natus::std::string_cref_t name, size_t const id ) : _name(name), _id( id ) {}
            component( this_cref_t rhv ) { _name = rhv._name ; _id = rhv._id ; }
            component( this_rref_t rhv ) { _name = ::std::move( rhv._name ) ; _id = rhv._id ; rhv._id = size_t( -1 ) ; }
            virtual ~component( void_t ) {}

        public:

            size_t id( void_t ) const { return _id ; }
            natus::std::string_cref_t name( void_t ) const { return _name ; }
        };
        
        class input_component : public component
        {
            natus_this_typedefs( input_component ) ;

        public:

            input_component( void_t ) {}
            input_component( natus::std::string_cref_t name, size_t const id ) : component( name, id ) {}
            input_component( this_cref_t rhv ) : component( rhv ){}
            input_component( this_rref_t rhv ) : component( ::std::move( rhv ) ){}
            virtual ~input_component( void_t ) {}
        };
        natus_typedef( input_component ) ;

        class output_component : public component
        {
            natus_this_typedefs( output_component ) ;

        public:

            output_component( void_t ) {}
            output_component( natus::std::string_cref_t name, size_t const id ) : component( name, id ) {}
            output_component( this_cref_t rhv ) : component( rhv ) {}
            output_component( this_rref_t rhv ) : component( ::std::move( rhv ) ) {}
            virtual ~output_component( void_t ) {}
        };
        natus_typedef( output_component ) ;
    }
}