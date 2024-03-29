
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
        class NATUS_GRAPHICS_API render_object : public object
        {
            natus_this_typedefs( render_object ) ;

        private:

            natus::ntd::string_t _name ;
            natus::ntd::vector< natus::ntd::string_t > _geo ;
            natus::ntd::string_t _shader ;
            natus::ntd::vector< natus::ntd::string_t > _soo ;

            natus::ntd::vector< natus::graphics::variable_set_res_t > _vars ;
            natus::ntd::vector< natus::graphics::render_state_sets_t > _states ;

        public:
            
            render_object( void_t ) {}
            render_object( natus::ntd::string_cref_t name ) 
                : _name( name ) {}

            render_object( this_cref_t rhv ) : object( rhv )
            {
                _name = rhv._name ;
                _geo = rhv._geo;
                _shader = rhv._shader ;
                _vars = rhv._vars ;
                _states = rhv._states ;
                _soo = rhv._soo ;
            }

            render_object( this_rref_t rhv ) : object( std::move( rhv ) )
            {
                _name = std::move( rhv._name ) ;
                _geo = std::move( rhv._geo ) ;
                _shader = std::move( rhv._shader ) ;
                _vars = std::move( rhv._vars ) ;
                _states = std::move( rhv._states ) ;
                _soo = std::move( rhv._soo ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                object::operator=( rhv ) ;

                _name = rhv._name ;
                _geo = rhv._geo;
                _shader = rhv._shader ;
                _vars = rhv._vars ;
                _states = rhv._states ;
                _soo = rhv._soo ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                object::operator=( ::std::move( rhv ) ) ;

                _name = std::move( rhv._name ) ;
                _geo = std::move( rhv._geo ) ;
                _shader = std::move( rhv._shader ) ;
                _vars = std::move( rhv._vars ) ;
                _states = std::move( rhv._states ) ;
                _soo = std::move( rhv._soo ) ;

                return *this ;
            }

        public:
            
            this_ref_t link_geometry( natus::ntd::string_cref_t name ) noexcept 
            {
                _geo.emplace_back( name ) ;
                return *this ;
            }

            this_ref_t link_geometry( std::initializer_list< natus::ntd::string_t > const & names ) noexcept 
            {
                for( auto const & name : names ) _geo.emplace_back( name ) ;
                return *this ;
            }

            // link to stream out object so geometry can be fed from there.
            // the geometry is then mainly used for geometry layout.
            this_ref_t link_geometry( natus::ntd::string_cref_t name, natus::ntd::string_cref_t soo_name ) noexcept 
            {
                _geo.emplace_back( name ) ;
                _soo.emplace_back( soo_name ) ;
                return *this ;
            }

            size_t get_num_geometry( void_t ) const noexcept{ return _geo.size() ; }
            size_t get_num_streamout( void_t ) const noexcept{ return _soo.size() ; }

            natus::ntd::string_cref_t get_geometry( size_t const i = 0 ) const noexcept
            {
                return _geo[i] ;
            }

            natus::ntd::string_cref_t get_streamout( size_t const i = 0 ) const noexcept
            {
                return _soo[i] ;
            }

            bool_t has_streamout_link( void_t ) const noexcept
            {
                return !_soo.empty() ;
            }

            this_ref_t link_shader( natus::ntd::string_cref_t name ) noexcept
            {
                _shader = name ;
                return *this ;
            }

            natus::ntd::string_cref_t get_shader( void_t ) const noexcept
            {
                return _shader ;
            }

        public: // variable sets

            this_ref_t add_variable_set( natus::graphics::variable_set_res_t vs )
            {
                _vars.emplace_back( ::std::move( vs ) ) ;
                return *this ;
            }

            this_ref_t remove_variable_sets( void_t ) noexcept 
            {
                _vars.clear() ;
                return *this ;
            }

            typedef std::function< void_t ( size_t const i, natus::graphics::variable_set_res_t const & ) > for_each_var_funk_t ;

            void_t for_each( for_each_var_funk_t funk )
            {
                size_t i = 0 ;
                for( auto const & v : _vars )
                {
                    funk( i++, v ) ;
                }
            }

            size_t get_num_variable_sets( void_t ) const noexcept { return _vars.size() ; }
            natus::graphics::variable_set_res_t get_variable_set( size_t const i ) noexcept 
            {
                if( _vars.size() <= i ) return natus::graphics::variable_set_res_t() ;
                return _vars[i] ;
            }

        public: // render state sets

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
        };
        natus_res_typedef( render_object ) ;
    }
}
