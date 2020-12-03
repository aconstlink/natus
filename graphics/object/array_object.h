#pragma once

#include "../object.hpp"

#include "../backend/types.h"
#include "../buffer/data_buffer.hpp"
#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace graphics
    {
        class array_object : public object
        {
            natus_this_typedefs( array_object ) ;
           
        private:

            natus::ntd::string_t _name ;
            natus::graphics::data_buffer_t _db ;

        public:

            array_object( void_t ) {}

            array_object( natus::ntd::string_cref_t name ) : _name(name)
            {}

            array_object( natus::ntd::string_cref_t name, natus::graphics::data_buffer_cref_t db )
            {
                _name = name ;
                _db = db ;
            }

            array_object( natus::ntd::string_cref_t name, natus::graphics::data_buffer_rref_t db )
            {
                _name = name ;
                _db = std::move( db ) ;
            }

            array_object( this_cref_t rhv ) : object( rhv ) 
            {
                _db = rhv._db ;
                _name = rhv._name ;
            }

            array_object( this_rref_t rhv ) : object( ::std::move( rhv ) )
            {
                _db = std::move( rhv._db ) ;
                _name = std::move( rhv._name ) ;
            }

            ~array_object( void_t ) 
            {}

            this_ref_t operator = ( this_cref_t rhv )
            {
                object::operator=( rhv ) ;

                _db = rhv._db ;
                _name = rhv._name ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                object::operator=( std::move( rhv ) ) ;

                _db = std::move( rhv._db ) ;
                _name = std::move( rhv._name ) ;

                return *this ;
            }

        public:

            natus::graphics::data_buffer_ref_t data_buffer( void_t ) noexcept
            {
                return _db ;
            }

            natus::graphics::data_buffer_cref_t data_buffer( void_t ) const noexcept
            {
                return _db ;
            }

            natus::ntd::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }

        };
        natus_res_typedef( array_object ) ;
    }
}