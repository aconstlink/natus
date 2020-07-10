
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

            natus::std::vector< natus::device::input_component_ptr_t > _inputs ;
            natus::std::vector< natus::device::output_component_ptr_t > _outputs ;

        public:

            device( void_t ) noexcept 
            {
                layout_t::init_components(*this) ;
            }

            device( natus::std::string_cref_t name ) noexcept : _name( name ) 
            {
                layout_t::init_components(*this) ;
            }

            device( this_cref_t rhv ) = delete ;

            device( this_rref_t rhv ) noexcept
            {
                _name = ::std::move( rhv._name ) ;

                _inputs = ::std::move( rhv._inputs ) ;
                _outputs = ::std::move( rhv._outputs ) ;
            }

            virtual ~device( void_t ) 
            {
                for( auto* ptr : _inputs )
                {
                    natus::memory::global_t::dealloc( ptr ) ;
                }

                for( auto* ptr : _outputs )
                {
                    natus::memory::global_t::dealloc( ptr ) ;
                }
            }

            this_ref_t operator = ( this_cref_t rhv ) = delete ;

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _name = ::std::move( rhv._name ) ;

                _inputs = ::std::move( rhv._inputs ) ;
                _outputs = ::std::move( rhv._outputs ) ;
                return *this ;
            }

        public: // add input

            template< typename comp_t >
            this_ref_t add_input_component( void_t )
            {
                _inputs.emplace_back( natus::memory::global_t::alloc<comp_t>(
                        natus_log_fn( "input component" ) ) ) ;
                return *this ;
            }

            template< typename comp_t >
            this_ref_t add_input_component( comp_t const & comp )
            {
                _inputs.emplace_back( 
                    natus::memory::global_t::alloc( comp, 
                        natus_log_fn("input component") ) ) ;
                return *this ;
            }

            template< typename comp_t >
            this_ref_t add_input_component( comp_t && comp )
            {
                _inputs.emplace_back( 
                    natus::memory::global_t::alloc( ::std::move(comp), 
                        natus_log_fn( "input component" ) ) ) ;
                return *this ;
            }

        public: // add output

            template< typename comp_t >
            this_ref_t add_output_component( void_t )
            {
                _outputs.emplace_back(
                    natus::memory::global_t::alloc( comp_t(),
                        natus_log_fn( "output component" ) ) ) ;
                return *this ;
            }

            template< typename comp_t >
            this_ref_t add_output_component( comp_t const& comp )
            {
                _outputs.emplace_back( 
                    natus::memory::global_t::alloc( comp, 
                        natus_log_fn( "output component" ) ) ) ;
                return *this ;
            }

            template< typename comp_t >
            this_ref_t add_output_component( comp_t && comp )
            {
                _outputs.emplace_back( 
                    natus::memory::global_t::alloc( ::std::move( comp ), 
                        natus_log_fn( "output component" ) ) ) ;
                return *this ;
            }

        public:

            natus::device::input_component_ptr_t get_component( typename layout_t::input_component const c ) noexcept
            {
                if( size_t( c ) >= size_t( this_t::input_component::num_components ) ) return nullptr ;
                return _inputs[ size_t( c ) ] ;
            }

            natus::device::output_component_ptr_t get_component( typename layout_t::output_component const c ) noexcept
            {
                if( size_t( c ) >= size_t( this_t::output_component::num_components ) ) return nullptr ;
                return _outputs[ size_t( c ) ] ;
            }

            template< typename T >
            T* get_component( typename layout_t::input_component const c ) noexcept
            {
                if( size_t( c ) >= size_t( typename layout_t::input_component::num_components ) ) return nullptr ;
                return dynamic_cast< T* >( _inputs[ size_t( c ) ] ) ;
            }

            template< typename T >
            T* get_component( typename layout_t::output_component const c ) noexcept
            {
                if( size_t( c ) >= size_t( typename layout_t::output_component::num_components ) ) return nullptr ;
                return dynamic_cast< T* >( _outputs[ size_t( c ) ] ) ;
            }

            typedef ::std::function< void_t ( typename layout_t::input_component const, natus::device::input_component_ptr_t ) > input_funk_t ;

            bool_t foreach_input( input_funk_t funk ) noexcept
            {
                for( size_t i = 0; i < size_t( typename layout_t::input_component::num_components ); ++i )
                {
                    funk( typename layout_t::input_component( i ), _inputs[ i ] ) ;
                }

                return true ;
            }

            typedef ::std::function< void_t ( typename layout_t::output_component const, natus::device::output_component_ptr_t ) > output_funk_t ;

            bool_t foreach_output( output_funk_t funk ) noexcept
            {
                for( size_t i = 0; i < size_t( typename layout_t::output_component::num_components ); ++i )
                {
                    funk( typename layout_t::output_component( i ), _outputs[ i ] ) ;
                }

                return true ;
            }

        public:

            // meant for a module to reset the components
            void_t update_components( void_t ) noexcept
            {
                for( auto* comp : _inputs ) comp->update() ;
                for( auto* comp : _outputs ) comp->update() ;
            }
        };
    }
}