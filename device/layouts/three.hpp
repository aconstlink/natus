
#pragma once

#include "../typedefs.h"
#include "../components/button.hpp"
#include "../components/scroll.hpp"
#include "../components/point.hpp"
#include "../device.hpp"

namespace natus
{
    namespace device
    {
        namespace layouts
        {
            class three
            {
                natus_this_typedefs( three ) ;

            public:

                enum class input_component
                {
                    left_button,
                    right_button,
                    middle_button,
                    scroll_wheel,
                    coord,
                    num_components
                };

                enum class output_component
                {
                    num_components
                };

            private:

                natus::std::vector< natus::device::input_component_ptr_t > _inputs ;
                natus::std::vector< natus::device::output_component_ptr_t > _outputs ;

                void_t init_components( void_t ) 
                {
                    // inputs
                    _inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::button_t() ) ) ;
                    _inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::button_t() ) ) ;
                    _inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::button_t() ) ) ;
                    _inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::scroll_t() ) ) ;
                    _inputs.emplace_back( natus::memory::global_t::alloc( natus::device::components::point_t() ) ) ;

                    // outputs
                }

            public:

                three( void_t ) 
                {
                    this_t::init_components() ;
                }

                three( this_cref_t ) = delete ;
                three( this_rref_t rhv ) 
                {
                    _inputs = ::std::move( rhv._inputs ) ;
                    _outputs = ::std::move( rhv._outputs ) ;
                }

                virtual ~three( void_t ) 
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

                this_ref_t operator = ( this_cref_t ) = delete ;
                this_ref_t operator = ( this_rref_t rhv ) 
                {
                    _inputs = ::std::move( rhv._inputs ) ;
                    _outputs = ::std::move( rhv._outputs ) ;
                    return *this ;
                }

                natus::device::input_component_ptr_t get_component( this_t::input_component const c ) noexcept
                {
                    if( size_t( c ) >= size_t( this_t::input_component::num_components ) ) return nullptr ;
                    return _inputs[ size_t( c ) ] ;
                }

                natus::device::output_component_ptr_t get_component( this_t::output_component const c ) noexcept
                {
                    if( size_t( c ) >= size_t( this_t::output_component::num_components ) ) return nullptr ;
                    return _outputs[ size_t( c ) ] ;
                }

                template< typename T >
                T * get_component(  this_t::input_component const c ) noexcept
                {
                    if( size_t( c ) >= size_t( this_t::input_component::num_components ) ) return nullptr ;
                    return dynamic_cast< T* >( _inputs[ size_t( c ) ] ) ;
                }

                template< typename T >
                T* get_component( this_t::output_component const c ) noexcept
                {
                    if( size_t( c ) >= size_t( this_t::output_component::num_components ) ) return nullptr ;
                    return dynamic_cast< T* >( _outputs[ size_t( c ) ] ) ;
                }

                typedef ::std::function< void_t ( this_t::input_component const, natus::device::input_component_ptr_t ) > input_funk_t ;

                bool_t foreach_input( input_funk_t funk ) noexcept
                {
                    for( size_t i=0; i<size_t(this_t::input_component::num_components); ++i )
                    {
                        funk( this_t::input_component(i), _inputs[ i ] ) ;
                    }

                    return true ;
                }

                typedef ::std::function< void_t ( this_t::output_component const, natus::device::output_component_ptr_t ) > output_funk_t ;

                bool_t foreach_output( output_funk_t funk ) noexcept
                {
                    for( size_t i = 0; i < size_t( this_t::output_component::num_components ); ++i )
                    {
                        funk( this_t::output_component( i ), _outputs[ i ] ) ;
                    }

                    return true ;
                }
            };
            natus_typedef( three ) ;
        }
        natus_soil_typedefs( natus::device::device< natus::device::layouts::three_t>, three_device ) ;
    }
}