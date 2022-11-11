#pragma once

#include "../types.h"
#include "../object.hpp"

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API streamout_object : public object
        {
            natus_this_typedefs( streamout_object ) ;
           
        private:

            natus::ntd::string_t _name ;

            typedef natus::ntd::vector< natus::graphics::data_buffer_t > buffers_t ;
            buffers_t _dbs ;

            // used for gpu only memory. So buffers do not require any CPU memory.
            size_t _ne = 0 ;

        public:

            streamout_object( void_t ) {}

            streamout_object( natus::ntd::string_cref_t name ) : _name(name)
            {}

            streamout_object( natus::ntd::string_cref_t name, natus::graphics::data_buffer_cref_t db ) : _name(name)
            {
                _dbs.emplace_back( db ) ;
            }

            streamout_object( natus::ntd::string_cref_t name, natus::graphics::data_buffer_rref_t db ) : _name(name)
            {
                _dbs.emplace_back( std::move( db ) ) ;
            }

            streamout_object( this_cref_t rhv ) noexcept
            {
                *this = rhv ;
            }

            streamout_object( this_rref_t rhv ) noexcept
            {
                *this = std::move( rhv ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                object::operator=( rhv ) ;

                _dbs = rhv._dbs ;
                _name = rhv._name ;
                _ne = rhv._ne ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                object::operator=( std::move( rhv ) ) ;

                _dbs = std::move( rhv._dbs ) ;
                _name = std::move( rhv._name ) ;
                _ne = rhv._ne ;

                return *this ;
            }

        public:

            size_t num_buffers( void_t ) const noexcept{ return _dbs.size() ; }
            natus::graphics::data_buffer_ref_t buffer( size_t const i ) noexcept{ return _dbs[i] ; } 
            natus::graphics::data_buffer_cref_t buffer( size_t const i ) const noexcept{ return _dbs[i] ; } 

            natus::ntd::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }

            // allows to resize the GPU memory without resizing the buffers gpu memory.
            // @param ne number of elements defined by the buffers layout.
            this_ref_t resize( size_t const ne ) noexcept { _ne = ne ; return *this ; }

            // @return number of elementy. The elements structure is defined by the buffers layout.
            size_t size( void_t ) const noexcept { return _ne ; }

        };
        natus_res_typedef( streamout_object ) ;
    }
}