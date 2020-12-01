
#pragma once

#include "iproperty.hpp"
#include "generic_property.hpp"

#include <natus/ntd/string.hpp>
#include <natus/ntd/map.hpp>

namespace natus
{
    namespace property
    {
        class property_sheet
        {
            natus_this_typedefs( property_sheet ) ;

        private:

            natus::ntd::map< natus::ntd::string_t, iproperty_ptr_t > _properties ;

        public:

            property_sheet( void_t ) noexcept
            {}

            property_sheet( this_cref_t ) = delete ;
            property_sheet( this_rref_t rhv ) noexcept
            {
                _properties = std::move( rhv._properties ) ;
            }
            ~property_sheet( void_t )
            {
                for( auto & i : _properties )
                {
                    if( i.second != nullptr ) 
                    {
                        natus::memory::global_t::dealloc( i.second ) ;
                    }
                }
            }

        public:

            template< typename T >
            bool_t set_value( natus::ntd::string_cref_t name, T const & v ) noexcept
            {
                auto iter = _properties.find( name ) ;
                if( iter == _properties.end() ) 
                {
                    natus::property::generic_property< T > * prop =
                        natus::memory::global_t::alloc< natus::property::generic_property< T > >() ;

                    prop->set( v ) ;
                    _properties[ name ] = prop ;
                    return true ;
                }
                
                auto * ptr = dynamic_cast< generic_property< T >* > ( iter->second ) ;
                if( ptr == nullptr ) return false ;

                ptr->set( v ) ;
                return true ;
            }

            template< typename T >
            bool_t get_value( natus::ntd::string_cref_t name, T & v_out ) const noexcept
            {
                auto iter = _properties.find( name ) ;
                if( iter == _properties.end() ) return false ;

                auto * ptr = dynamic_cast< generic_property< T > const * >( iter->second ) ;
                if( ptr == nullptr ) return false ;

                v_out = ptr->get() ;

                return true ;
            }

            typedef std::function< void_t ( natus::ntd::string_cref_t, natus::property::iproperty_cptr_t ) > for_each_funk_t ;

            void_t read_for_each( for_each_funk_t f ) const noexcept
            {
                for( auto i : _properties )
                {
                    f( i.first, i.second ) ;
                }
            }

        };
        natus_res_typedef( property_sheet ) ;
    }
}