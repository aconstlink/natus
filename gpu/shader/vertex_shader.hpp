
#pragma once

#include "../backend/id.hpp"
#include "../buffer/vertex_attribute.h"
#include <natus/std/string.hpp>

#include <functional>

namespace natus
{
    namespace gpu
    {
        class NATUS_GPU_API vertex_shader
        {
            natus_this_typedefs( vertex_shader ) ;

        private:

            natus::std::string_t _code ;

            size_t _hash = 0 ;

            struct input_binding
            {
                natus_this_typedefs( input_binding ) ;

                input_binding( void_t ) {}
                input_binding( natus::std::string_cref_t name_,
                    natus::gpu::vertex_attribute const va_ ) : name( name_ ), va( va_ ) {}
                input_binding( input_binding const & rhv )
                {
                    *this = rhv ;
                }
                input_binding( input_binding && rhv ) 
                {
                    *this = rhv ;
                }
                ~input_binding( void_t ) {}

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
            natus::std::vector< input_binding > _inputs ;

        public:

            vertex_shader( void_t ) {}
            vertex_shader( natus::std::string_in_t code ) : _code( code ) 
            {
                _hash = ::std::hash<::std::string>{}( code ) ;
            }
            vertex_shader( this_cref_t rhv ) {
                *this = rhv ;
            }
            vertex_shader( this_rref_t rhv )  {
                *this = ::std::move( rhv ) ;
            }
            virtual ~vertex_shader( void_t ) {}

        public:

            this_ref_t operator = ( this_cref_t rhv )
            {
                _code = rhv._code ;
                _hash = rhv._hash ;
                _inputs = rhv._inputs ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _code = ::std::move( rhv._code ) ;
                _hash = rhv._hash ;
                rhv._hash = 0 ;
                _inputs = ::std::move( rhv._inputs ) ;
                return *this ;
            }

            natus::std::string code( void_t ) const { return _code ; }

        public:

            this_ref_t add_input_binding( natus::gpu::vertex_attribute const va, 
                natus::std::string_cref_t name )
            {
                auto iter = ::std::find_if( _inputs.begin(), _inputs.end(), 
                    [&]( input_binding const & b )
                {
                    return b.name == name ;
                } ) ;

                if( iter == _inputs.end() )
                {
                    iter = ::std::find_if( _inputs.begin(), _inputs.end(),
                        [&] ( input_binding const& b )
                    {
                        return b.va == va ;
                    } ) ;
                }

                if( iter == _inputs.end() )
                {
                    _inputs.push_back( input_binding( name, va ) ) ;
                    return *this ;
                }
                iter->va = va ;
                return *this ;
            }

            bool_t find_input_binding_by_name( natus::std::string_cref_t name, 
                natus::gpu::vertex_attribute & va ) const noexcept
            {
                auto iter = ::std::find_if( _inputs.begin(), _inputs.end(),
                    [&] ( input_binding const& b )
                {
                    return b.name == name ;
                } ) ;
                if( iter == _inputs.end() ) return false ;

                va = iter->va ;

                return true ;
            }

            bool_t find_input_binding_by_attribute( natus::gpu::vertex_attribute const va, 
                natus::std::string_out_t name) const noexcept
            {
                auto iter = ::std::find_if( _inputs.begin(), _inputs.end(),
                    [&] ( input_binding const& b )
                {
                    return b.va == va ;
                } ) ;
                if( iter == _inputs.end() ) return false ;

                name = iter->name ;

                return true ;
            }
        };
        natus_soil_typedef( vertex_shader ) ;
    }
}