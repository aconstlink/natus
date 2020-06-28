
#pragma once

#include "../types.h"
#include "../typedefs.h"

#include <functional>

namespace natus
{
    namespace gpu
    {
        class ivariable
        {
        public:

            virtual ~ivariable( void_t ) {}

            virtual void_cptr_t data_ptr( void_t ) const noexcept = 0 ;
        };
        natus_typedef( ivariable ) ;

        template< class T >
        class data_variable : public ivariable
        {
            natus_this_typedefs( data_variable< T > ) ;
            natus_typedefs( T, value ) ;

        private:

            value_t _value ;

        public:

            data_variable( void_t ) 
            {}

            data_variable( value_cref_t v ) : _value(v)
            {}

            data_variable( this_cref_t rhv ) 
            {
                _value = rhv._value ;
            }

            data_variable( this_rref_t rhv )
            {
                _value = ::std::move( rhv._value ) ;
            }

            virtual ~data_variable( void_t ) {}

        public:


            void_t set( value_cref_t v ) noexcept { _value = v ; }
            void_t set( value_rref_t v ) noexcept { _value = v ; }
            value_cref_t get( void_t ) const noexcept { return _value ; }

            virtual void_cptr_t data_ptr( void_t ) const noexcept override
            {
                return reinterpret_cast< void_cptr_t >( &_value ) ;
            }
        };

        template<>
        class data_variable<natus::std::string_t> : public ivariable
        {
            natus_this_typedefs( data_variable< natus::std::string_t > ) ;
            natus_typedefs( natus::std::string_t, value ) ;

        private:

            natus::std::string_t _value ;
            size_t _hash = 0 ;

        public:

            data_variable( void_t )
            {}

            data_variable( this_cref_t rhv )
            {
                _value = rhv._value ;
                _hash = rhv._hash ;
            }

            data_variable( this_rref_t rhv )
            {
                _value = ::std::move( rhv._value ) ;
                _hash = rhv._hash ;
            }

            virtual ~data_variable( void_t ) {}

        public:

            void_t set( value_cref_t v ) noexcept 
            { 
                _value = v ; 
                _hash = ::std::hash<natus::std::string_t>{}(v) ; 
            }

            void_t set( value_rref_t v ) noexcept 
            { 
                _value = ::std::move( v ) ; 
                _hash = ::std::hash<natus::std::string_t>{}(_value) ; 
            }

            value_cref_t get( void_t ) const noexcept { return _value ; }
            size_t hash( void_t ) const noexcept { return _hash ; }

            virtual void_cptr_t data_ptr( void_t ) const noexcept override
            {
                return nullptr ;
            }
        };
    }
}