#pragma once

#include "api.h"
#include "typedefs.h"

namespace natus
{
    namespace nsl
    {
        class node
        {
        };
        natus_res_typedef( node ) ;

        class group
        {
        };
        natus_res_typedef( group ) ;

        class leaf
        {
        };
        natus_res_typedef( leaf ) ;

        class statement : public group
        {
            natus::ntd::string_t _statement ;
        };
        natus_res_typedef( statement ) ;

        class variable : public leaf
        {
            // type
            // name
        };
        natus_res_typedef( variable ) ;
        
        class constant : public leaf
        {
            // type
            // name
            // value
        };
        natus_res_typedef( variable ) ;

        class function : public group
        {
            // buildin, operator or custom funk
            // signatue
            // arguments as children
        };
        natus_res_typedef( function ) ;

        
    }
}