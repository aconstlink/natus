
#pragma once

#include "variable.hpp"

#include <natus/std/string.hpp>
#include <natus/std/vector.hpp>

namespace natus
{
    namespace gpu
    {
        class variable_set
        {
            natus_this_typedefs( variable_set ) ;
        
        private:


            struct data
            {
                natus::std::string_t name ;
                natus::gpu::ivariable_ptr_t var ;
            };
            natus::std::vector< data > _variables ;

        public:

            template< class T >
            ::std::shared_ptr< natus::gpu::variable< T > > create_variable( natus::std::string_cref_t name ) 
            {
                auto iter = ::std::find_if( _variables.begin(), _variables.end(), 
                    [&] ( this_t::data const & d ) 
                { 
                    return d.name == name ;
                } ) ;

                if( iter != _variables.end() )
                {
                    auto sptr = ::std::dynamic_pointer_cast< natus::gpu::variable< T > > ( iter->var ) ;
                    if(  sptr != nullptr )
                    {
                    }
                    return iter->var ;
                }

                auto var = natus::gpu::variable< T >() ;
                return natus::soil::res<natus::gpu::variable<T>>( natus::gpu::variable<T>( var ) ) ;
            }
        };
        natus_typedef( variable_set ) ;
    }
}