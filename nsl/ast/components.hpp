
#pragma once

#include "interfaces.hpp"

namespace natus
{
    namespace nsl
    {
        class version_component : public icomponent
        {
            natus_this_typedefs( version_component ) ;

        private:

            natus_typedefs( natus::ntd::vector< natus::ntd::string_t >, versions ) ;
            versions_t _versions ;

        public:

            version_component( void_t )
            {}

            version_component( this_cref_t rhv ) 
            {
                _versions = rhv._versions ;
            }

            version_component( this_rref_t rhv )
            {
                _versions = std::move( rhv._versions ) ;
            }

        public:

            this_ref_t add( versions_cref_t versions ) noexcept
            {
                for( auto const & v : versions )
                {
                    _versions.emplace_back( v ) ;
                }
                return *this ;
            }

            this_ref_t add( versions_rref_t versions ) noexcept
            {
                for( auto && v : versions )
                {
                    _versions.emplace_back( std::move( v ) ) ;
                }
                return *this ;
            }

        };
        natus_typedef( version_component ) ;
    }
}
