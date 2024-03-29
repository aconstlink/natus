
#pragma once

#include "../types.h"
#include "../object.hpp"

#include "../shader/shader_set.hpp"
#include "../buffer/vertex_attribute.h"

#include "../backend/types.h"

#include <natus/ntd/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace graphics
    {
        enum class shader_api_type
        {
            unknown,
            glsl_1_4,       // gl 3.1
            glsl_4_0,       // gl 4.0
            glsles_3_0,     // gles 3.1
            hlsl_5_0,        // dx 11
            nsl_1_0
        };

        class NATUS_GRAPHICS_API shader_object : public object
        {
            natus_this_typedefs( shader_object ) ;

        private: // vertex input

            struct vertex_input_binding
            {
                natus_this_typedefs( vertex_input_binding ) ;

                vertex_input_binding( void_t ) noexcept {}
                vertex_input_binding( natus::ntd::string_cref_t name_,
                    natus::graphics::vertex_attribute const va_ ) noexcept : name( name_ ), va( va_ ) {}
                vertex_input_binding( this_cref_t rhv ) noexcept { *this = rhv ; }
                vertex_input_binding( this_rref_t rhv ) noexcept { *this = std::move( rhv ) ; }
                ~vertex_input_binding( void_t ) noexcept {}

                this_ref_t operator = ( this_cref_t rhv ) noexcept
                    { name = rhv.name; va = rhv.va ; return *this ; }

                this_ref_t operator = ( this_rref_t rhv ) noexcept
                    { name = std::move( rhv.name ) ; va = rhv.va ; return *this ; }

                natus::ntd::string name ;
                natus::graphics::vertex_attribute va = natus::graphics::vertex_attribute::undefined ;
            };
            natus::ntd::vector< vertex_input_binding > _vertex_inputs ;

        private: // vertex output

            struct vertex_output_binding
            {
                natus_this_typedefs( vertex_output_binding ) ;

                vertex_output_binding( void_t ) noexcept {}
                vertex_output_binding( natus::ntd::string_cref_t name_,
                    natus::graphics::vertex_attribute const va_ ) noexcept : name( name_ ), va( va_ ), 
                        ct( natus::graphics::deduce_from( va_ ) ) {}
                vertex_output_binding( natus::ntd::string_cref_t name_,
                    natus::graphics::vertex_attribute const va_, natus::graphics::ctype const ct_ ) noexcept : name( name_ ), va( va_ ), ct( ct_ ) {}
                vertex_output_binding( this_cref_t rhv ) noexcept { *this = rhv ; }
                vertex_output_binding( this_rref_t rhv ) noexcept { *this = std::move( rhv ) ; }
                ~vertex_output_binding( void_t ) noexcept {}

                this_ref_t operator = ( this_cref_t rhv ) noexcept
                    { name = rhv.name; va = rhv.va ; ct = rhv.ct ; return *this ; }

                this_ref_t operator = ( this_rref_t rhv ) noexcept
                    { name = std::move( rhv.name ) ; va = rhv.va ; ct = std::move( rhv.ct ) ;return *this ; }

                natus::ntd::string name ;
                natus::graphics::vertex_attribute va = natus::graphics::vertex_attribute::undefined ;
                natus::graphics::ctype ct ;
            };
            natus::ntd::vector< vertex_output_binding > _vertex_outputs ;

        private: // input bindings

            struct variable_binding
            {
                natus::ntd::string_t name ;
                natus::graphics::binding_point bp ;
            };
            natus_typedef( variable_binding ) ;
            natus::ntd::vector< variable_binding_t > _bindings ;

        public: 

            typedef std::function < void_t ( size_t const i,
                natus::graphics::vertex_attribute const va, natus::ntd::string_cref_t name ) > for_each_vib_funk_t ;
            
            //**************************************************************************
            void_t for_each_vertex_input_binding( for_each_vib_funk_t funk ) const noexcept
            {
                size_t i = 0 ;
                for( auto const & vib : _vertex_inputs ) funk( i++, vib.va, vib.name ) ;
                
            }

            typedef std::function < void_t ( size_t const i,
                natus::graphics::vertex_attribute const va, natus::graphics::ctype const, natus::ntd::string_cref_t name ) > for_each_vob_funk_t ;

            //**************************************************************************
            void_t for_each_vertex_output_binding( for_each_vob_funk_t funk ) const noexcept
            {
                size_t i = 0 ;
                for( auto const & vb : _vertex_outputs ) funk( i++, vb.va, vb.ct, vb.name ) ;
            }

            //**************************************************************************
            this_ref_t add_vertex_input_binding( natus::graphics::vertex_attribute const va,
                natus::ntd::string_cref_t name ) noexcept
            {
                auto iter = std::find_if( _vertex_inputs.begin(), _vertex_inputs.end(),
                    [&] ( vertex_input_binding const& b )
                {
                    return b.name == name ;
                } ) ;

                if( iter == _vertex_inputs.end() )
                {
                    iter = std::find_if( _vertex_inputs.begin(), _vertex_inputs.end(),
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

            //**************************************************************************
            this_ref_t add_vertex_output_binding( natus::graphics::vertex_attribute const va,
                natus::ntd::string_cref_t name ) noexcept
            {
                auto iter = std::find_if( _vertex_outputs.begin(), _vertex_outputs.end(),
                    [&] ( vertex_output_binding const& b )
                {
                    return b.name == name ;
                } ) ;

                if( iter == _vertex_outputs.end() )
                {
                    iter = std::find_if( _vertex_outputs.begin(), _vertex_outputs.end(),
                        [&] ( vertex_output_binding const& b )
                    {
                        return b.va == va ;
                    } ) ;
                }

                if( iter == _vertex_outputs.end() )
                {
                    _vertex_outputs.push_back( vertex_output_binding( name, va ) ) ;
                    return *this ;
                }
                iter->va = va ;
                return *this ;
            }

            //**************************************************************************
            this_ref_t add_vertex_output_binding( natus::graphics::vertex_attribute const va, 
                natus::graphics::ctype const ct, natus::ntd::string_cref_t name ) noexcept
            {
                auto iter = std::find_if( _vertex_outputs.begin(), _vertex_outputs.end(),
                    [&] ( vertex_output_binding const& b )
                {
                    return b.name == name ;
                } ) ;

                if( iter == _vertex_outputs.end() )
                {
                    iter = std::find_if( _vertex_outputs.begin(), _vertex_outputs.end(),
                        [&] ( vertex_output_binding const& b )
                    {
                        return b.va == va ;
                    } ) ;
                }

                if( iter == _vertex_outputs.end() )
                {
                    _vertex_outputs.push_back( vertex_output_binding( name, va, ct ) ) ;
                    return *this ;
                }
                iter->va = va ;
                iter->ct = natus::graphics::deduce_from( va ) ;
                return *this ;
            }

            //**************************************************************************
            bool_t find_vertex_input_binding_by_name( natus::ntd::string_cref_t name,
                natus::graphics::vertex_attribute& va ) const noexcept
            {
                auto iter = std::find_if( _vertex_inputs.begin(), _vertex_inputs.end(),
                    [&] ( vertex_input_binding const& b )
                {
                    return b.name == name ;
                } ) ;
                if( iter == _vertex_inputs.end() ) return false ;

                va = iter->va ;

                return true ;
            }

            //**************************************************************************
            bool_t find_vertex_output_binding_by_name( natus::ntd::string_cref_t name,
                natus::graphics::vertex_attribute& va ) const noexcept
            {
                auto iter = std::find_if( _vertex_outputs.begin(), _vertex_outputs.end(),
                    [&] ( vertex_output_binding const& b )
                {
                    return b.name == name ;
                } ) ;
                if( iter == _vertex_outputs.end() ) return false ;

                va = iter->va ;

                return true ;
            }

            //**************************************************************************
            bool_t find_vertex_input_binding_by_attribute( natus::graphics::vertex_attribute const va,
                natus::ntd::string_out_t name ) const noexcept
            {
                auto iter = std::find_if( _vertex_inputs.begin(), _vertex_inputs.end(),
                    [&] ( vertex_input_binding const& b )
                {
                    return b.va == va ;
                } ) ;
                if( iter == _vertex_inputs.end() ) return false ;

                name = iter->name ;

                return true ;
            }

            //**************************************************************************
            bool_t find_vertex_output_binding_by_attribute( natus::graphics::vertex_attribute const va,
                natus::ntd::string_out_t name ) const noexcept
            {
                auto iter = std::find_if( _vertex_outputs.begin(), _vertex_outputs.end(),
                    [&] ( vertex_output_binding const& b )
                {
                    return b.va == va ;
                } ) ;
                if( iter == _vertex_outputs.end() ) return false ;

                name = iter->name ;

                return true ;
            }

            //**************************************************************************
            this_ref_t add_input_binding( natus::graphics::binding_point const bp,
                natus::ntd::string_cref_t name ) noexcept
            {
                variable_binding vb ;
                vb.name = name ;
                vb.bp = bp ;

                _bindings.push_back( vb ) ;

                return *this ;
            }

            //**************************************************************************
            bool_t has_input_binding( natus::graphics::binding_point const bp ) const noexcept
            {
                for( auto const & b : _bindings )
                {
                    if( b.bp == bp ) return true ;
                }
                return false ;
            }

            //**************************************************************************
            this_ref_t set_streamout_mode( natus::graphics::streamout_mode sm ) noexcept
            {
                _sm = sm ;
                return *this ;
            }

            //**************************************************************************
            natus::graphics::streamout_mode get_streamout_mode( void_t ) const noexcept
            {
                return _sm ;
            }

        private:

            typedef ::std::pair< natus::graphics::shader_api_type, natus::graphics::shader_set > ss_item_t ;
            typedef natus::ntd::vector< ss_item_t > shader_sets_t ;

            shader_sets_t _shader_sets ;

            natus::ntd::string_t _name ;

            natus::graphics::streamout_mode _sm = natus::graphics::streamout_mode::unknown ;

        public:
            
            shader_object( void_t ) noexcept {}

            shader_object( natus::ntd::string_cref_t name ) noexcept
                : _name( name ) {}

            shader_object( this_cref_t rhv ) noexcept : object( rhv )
            {
                _name = rhv._name ;
                _vertex_inputs = rhv._vertex_inputs ;
                _vertex_outputs = rhv._vertex_outputs ;
                _shader_sets = rhv._shader_sets ;
                _bindings = rhv._bindings ;
                _sm = rhv._sm ;
            }

            shader_object( this_rref_t rhv ) noexcept : object( ::std::move(rhv) )
            {
                _name = ::std::move( rhv._name ) ;
                _vertex_inputs = ::std::move( rhv._vertex_inputs ) ;
                _vertex_outputs = ::std::move( rhv._vertex_outputs ) ;
                _shader_sets = ::std::move( rhv._shader_sets ) ;
                _bindings = ::std::move( rhv._bindings ) ;
                _sm = rhv._sm ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                object::operator=( rhv ) ;

                _name = rhv._name ;
                _vertex_inputs = rhv._vertex_inputs ;
                _vertex_outputs = rhv._vertex_outputs ;
                _shader_sets = rhv._shader_sets ;
                _bindings = rhv._bindings ;
                _sm = rhv._sm ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                object::operator=( ::std::move( rhv ) ) ;

                _name = std::move( rhv._name ) ;
                _vertex_inputs = std::move( rhv._vertex_inputs ) ;
                _vertex_outputs = std::move( rhv._vertex_outputs ) ;
                _shader_sets = std::move( rhv._shader_sets ) ;
                _bindings = std::move( rhv._bindings ) ;
                _sm = rhv._sm ;

                return *this ;
            }

            this_ref_t set_name( natus::ntd::string_cref_t n ) noexcept 
            {
                _name = n ;
                return *this ;
            }

        public:
           
            this_ref_t insert( natus::graphics::shader_api_type const bt, natus::graphics::shader_set_in_t ss ) noexcept
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

            bool_t shader_set( natus::graphics::shader_api_type const bt, natus::graphics::shader_set_out_t ss ) const noexcept
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

            size_t get_num_output_bindings( void_t ) const noexcept
            {
                return _vertex_outputs.size() ;
            }

            natus::ntd::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }
        };
        natus_res_typedef( shader_object ) ;
    }
}
