#pragma once

#include "../object.hpp"

#include "../object/shader_object.h"
#include "../buffer/vertex_attribute.h"
#include "../variable/variable_set.hpp"
#include "../state/state_set.h"

#include <natus/ntd/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API state_object : public object
        {

            natus_this_typedefs( state_object ) ;

        private:

            natus::ntd::string_t _name ;
            natus::ntd::vector< natus::graphics::render_state_sets_t > _states ;

        public:

            state_object( void_t ) {}
            state_object( natus::ntd::string_cref_t name )
                : _name( name ) {}

            state_object( this_cref_t rhv ) : object( rhv )
            {
                _name = rhv._name ;
                _states = rhv._states ;
            }

            state_object( this_rref_t rhv ) : object( ::std::move( rhv ) )
            {
                _name = std::move( rhv._name ) ;
                _states = std::move( rhv._states ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                object::operator=( rhv ) ;

                _name = rhv._name ;
                _states = rhv._states ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                object::operator=( ::std::move( rhv ) ) ;

                _name = std::move( rhv._name ) ;
                _states = std::move( rhv._states ) ;

                return *this ;
            }

        public: // render state sets

            size_t size( void_t ) const noexcept 
            {
                return _states.size() ;
            }

            this_ref_t add_render_state_set( natus::graphics::render_state_sets_cref_t rs )
            {
                _states.emplace_back( rs ) ;
                return *this ;
            }

            typedef std::function< void_t ( size_t const i, natus::graphics::render_state_sets_cref_t ) > for_each_render_state_funk_t ;

            void_t for_each( for_each_render_state_funk_t funk )
            {
                size_t i = 0 ;
                for( auto const& v : _states )
                {
                    funk( i++, v ) ;
                }
            }

        public:

            natus::ntd::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }
        } ;
        natus_res_typedef( state_object ) ;
    }
}