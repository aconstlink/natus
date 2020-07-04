
#pragma once

#include "api.h"
#include "typedefs.h"

#include "component.hpp"

#include <natus/std/vector.hpp>

namespace natus
{
    namespace device
    {
        // a physical device that is physically present
        // it has inputs and outputs
        // inputs: components' value coming in, i.e. button pressed
        // output: components' value going out, i.e. led light, motor triggered
        class NATUS_DEVICE_API physical
        {
            natus_this_typedefs( physical ) ;

        private:

            natus::std::string_t _name ;
            natus::std::vector< input_component_ptr_t > _inputs ;
            natus::std::vector< output_component_ptr_t > _outputs ;

        public:

            physical( void_t ) noexcept {}
            physical( natus::std::string_cref_t name ) noexcept : _name( name ) {}
            physical( this_cref_t rhv ) = delete ;
            physical( this_rref_t rhv ) noexcept
            {
                _name = ::std::move( rhv._name ) ;
                _inputs = ::std::move( rhv._inputs ) ;
                _outputs = ::std::move( rhv._outputs ) ;
            }

            ~physical( void_t ) 
            {
                for( auto * ptr : _inputs )
                {
                    natus::memory::global_t::dealloc( ptr ) ;
                }

                for( auto* ptr : _outputs )
                {
                    natus::memory::global_t::dealloc( ptr ) ;
                }
            }

        private:

            template< typename T >
            bool_t has_id( T const & collection, size_t const id ) noexcept
            {
                for( auto* ptr : collection )
                {
                    if( ptr->id() == id ) return true ;
                }
                return false ;
            }
            

        public:

            template< typename T >
            this_ref_t add_input_component( T const & comp )
            {
                if( this_t::has_id( _inputs, comp.id() ) )
                {
                    natus::log::global_t::warning( "Component ID already taken: " +
                        comp.name() + "," + ::std::to_string( comp.id() ) + "]" ) ;
                    return *this ;
                }

                _inputs.emplace_back( natus::memory::global_t::alloc( comp ), 
                    natus_log_fn("input_component") ) ;
            }

            template< typename T >
            this_ref_t add_input_component( T && comp )
            {
                if( this_t::has_id( _inputs, comp.id() ) )
                {
                    natus::log::global_t::warning( "Component ID already taken: [" + 
                        comp.name() + "," + ::std::to_string(comp.id()) + "]"  ) ;
                    return *this ;
                }

                _inputs.emplace_back( natus::memory::global_t::alloc( ::std::move(comp) , 
                    natus_log_fn( "input_component" ) ) ) ;
            }

            template< typename T >
            this_ref_t add_output_component( T const& comp )
            {
                if( this_t::has_id( _outputs, comp.id() ) )
                {
                    natus::log::global_t::warning( "Component ID already taken: " +
                        comp.name() + "," + ::std::to_string( comp.id() ) + "]" ) ;
                    return *this ;
                }

                _outputs.emplace_back( natus::memory::global_t::alloc( comp ),
                    natus_log_fn( "output_component" ) ) ;
            }

            template< typename T >
            this_ref_t add_output_component( T && comp )
            {
                if( this_t::has_id( _outputs, comp.id() ) )
                {
                    natus::log::global_t::warning( "Component ID already taken: " +
                        comp.name() + "," + ::std::to_string( comp.id() ) + "]" ) ;
                    return *this ;
                }

                _outputs.emplace_back( natus::memory::global_t::alloc( ::std::move( comp ) ),
                    natus_log_fn( "output_component" ) ) ;
            }
            
        public:

            template< typename T >
            bool_t get_input_component( size_t const id, T & o ) const
            {
                for( auto * ptr : _inputs )
                {
                    if( ptr->id() == id )
                    {
                        o = *ptr ;
                        return true ;
                    }
                }
                return false ;
            }

            template< typename T >
            bool_t get_output_component( size_t const id, T& o ) const
            {
                for( auto* ptr : _outputs )
                {
                    if( ptr->id() == id )
                    {
                        o = *ptr ;
                        return true ;
                    }
                }
                return false ;
            }
        };
        natus_soil_typedef( physical ) ;
    }
}