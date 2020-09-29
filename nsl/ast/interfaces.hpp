

#pragma once

#include "../typedefs.h"

namespace natus
{
    namespace nsl
    {
        natus_class_proto_typedefs( inode ) ;
        natus_class_proto_typedefs( node ) ;
        natus_class_proto_typedefs( group ) ;
        natus_class_proto_typedefs( leaf ) ;
        natus_class_proto_typedefs( ivisitor ) ;

        class inode
        {
        public:

            virtual void_t apply( ivisitor_ptr_t ) noexcept = 0 ;
        };

        class ivisitor
        {
        public:

            virtual void_t visit( inode_ptr_t ) noexcept = 0 ;
            virtual void_t visit( node_ptr_t ) noexcept = 0 ;            
            virtual void_t visit( leaf_ptr_t ) noexcept = 0 ;
            virtual void_t visit( group_ptr_t ) noexcept = 0 ;
            virtual void_t post_visit( group_ptr_t ) noexcept = 0 ;
        };

        class icomponent
        {
        public:
        };
        natus_typedef( icomponent ) ;
    }
}