
#pragma once

#include "../object.hpp"

#include "../shader/shader_set.hpp"
#include "../buffer/vertex_attribute.h"

#include "../backend/types.h"

#include <natus/std/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API shader_configuration : public object
        {
            natus_this_typedefs( shader_configuration ) ;

        private: // vertex input

            struct vertex_input_binding
            {
                natus_this_typedefs( vertex_input_binding ) ;

                vertex_input_binding( void_t ) {}
                vertex_input_binding( natus::std::string_cref_t name_,
                    natus::gpu::vertex_attribute const va_ ) : name( name_ ), va( va_ ) {}
                vertex_input_binding( vertex_input_binding const& rhv )
                {
                    *this = rhv ;
                }
                vertex_input_binding( vertex_input_binding&& rhv )
                {
                    *this = rhv ;
                }
                ~vertex_input_binding( void_t ) {}

                this_ref_t operator = ( this_cref_t rhv )
                {
                    name = rhv.name; va = rhv.va ;
                    return *this ;
                }

                this_ref_t operator = ( this_rref_t rhv )
                {
                    name = ::std::move( rhv.name ) ; va = rhv.va ;
                    return *this ;
                }

                natus::std::string name ;
                natus::gpu::vertex_attribute va ;
            };
            natus::std::vector< vertex_input_binding > _vertex_inputs ;

        private: // input bindings

            struct variable_binding
            {
                natus::std::string_t name ;
                natus::gpu::binding_point bp ;
            };
            natus_typedef( variable_binding ) ;
            natus::std::vector< variable_binding_t > _bindings ;

        public: 

            typedef ::std::function < void_t ( 
                natus::gpu::vertex_attribute const va, natus::std::string_cref_t name ) > for_each_vab_funk_t ;
            void_t for_each_vertex_input_binding( for_each_vab_funk_t funk ) const
            {
                for( auto const & vib : _vertex_inputs )
                {
                    funk( vib.va, vib.name ) ;
                }
            }

            this_ref_t add_vertex_input_binding( natus::gpu::vertex_attribute const va,
                natus::std::string_cref_t name )
            {
                auto iter = ::std::find_if( _vertex_inputs.begin(), _vertex_inputs.end(),
                    [&] ( vertex_input_binding const& b )
                {
                    return b.name == name ;
                } ) ;

                if( iter == _vertex_inputs.end() )
                {
                    iter = ::std::find_if( _vertex_inputs.begin(), _vertex_inputs.end(),
                        [&] ( vertex_input_binding const& b )
                    {
                        return b.va == va ;
                    } ) ;
                }

                if( iter == _vertex_inputs.end() )
                {
                    _vertex_inputs.push_back( vertex_input_binding( name, va ) ) ;
                    return *this ;
                }
                iter->va = va ;
                return *this ;
            }

            bool_t find_vertex_input_binding_by_name( natus::std::string_cref_t name,
                natus::gpu::vertex_attribute& va ) const noexcept
            {
                auto iter = ::std::find_if( _vertex_inputs.begin(), _vertex_inputs.end(),
                    [&] ( vertex_input_binding const& b )
                {
                    return b.name == name ;
                } ) ;
                if( iter == _vertex_inputs.end() ) return false ;

                va = iter->va ;

                return true ;
            }

            bool_t find_vertex_input_binding_by_attribute( natus::gpu::vertex_attribute const va,
                natus::std::string_out_t name ) const noexcept
            {
                auto iter = ::std::find_if( _vertex_inputs.begin(), _vertex_inputs.end(),
                    [&] ( vertex_input_binding const& b )
                {
                    return b.va == va ;
                } ) ;
                if( iter == _vertex_inputs.end() ) return false ;

                name = iter->name ;

                return true ;
            }

            this_ref_t add_input_binding( natus::gpu::binding_point const bp,
                natus::std::string_cref_t name )
            {
                variable_binding vb ;
                vb.name = name ;
                vb.bp = bp ;

                _bindings.push_back( vb ) ;

                return *this ;
            }

        private:

            typedef ::std::pair< natus::gpu::backend_type, natus::gpu::shader_set > ss_item_t ;
            typedef natus::std::vector< ss_item_t > shader_sets_t ;

            shader_sets_t _shader_sets ;

            natus::std::string_t _name ;

        public:
            
            shader_configuration( void_t ) {}
            shader_configuration( natus::std::string_cref_t name ) 
                : _name( name ) {}
            shader_configuration( this_cref_t rhv )
            {
                *this = rhv  ;
            }

            shader_configuration( this_rref_t rhv )
            {
                *this = ::std::move( rhv ) ;
            }

        public:
           
            this_ref_t insert( natus::gpu::backend_type const bt, natus::gpu::shader_set_in_t ss )
            {
                auto iter = ::std::find_if( _shader_sets.begin(), _shader_sets.end(),
                    [&] ( this_t::ss_item_t const& item )
                {
                    return item.first == bt ;
                } ) ;

                if( iter == _shader_sets.end() )
                {
                    _shader_sets.emplace_back( ::std::make_pair( bt, ss ) ) ;
                }
                else
                {
                    iter->second = ss ;
                }

                return *this ;
            }

            bool_t shader_set( natus::gpu::backend_type const bt, natus::gpu::shader_set_out_t ss ) const
            {
                auto const iter = ::std::find_if( _shader_sets.begin(), _shader_sets.end(), 
                    [&] ( this_t::ss_item_t const & item ) 
                { 
                    return item.first == bt ;
                } ) ;

                if( iter == _shader_sets.end() ) return false ;

                ss = iter->second ;

                return true ;
            }

        public:

            this_ref_t operator = ( this_cref_t rhv )
            {
                object::operator=( rhv ) ;

                _name = rhv._name ;
                _vertex_inputs = rhv._vertex_inputs ;
                _shader_sets = rhv._shader_sets ;
                _bindings = rhv._bindings ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                object::operator=( ::std::move( rhv ) ) ;

                _name = ::std::move( rhv._name ) ;
                _vertex_inputs = ::std::move( rhv._vertex_inputs ) ;
                _shader_sets = ::std::move( rhv._shader_sets ) ;
                _bindings = ::std::move( rhv._bindings ) ;

                return *this ;
            }

            natus::std::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }
        };
        natus_soil_typedef( shader_configuration ) ;
    }
}
