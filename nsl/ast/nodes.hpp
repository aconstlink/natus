
#pragma once

#include "interfaces.hpp"

#include <natus/ntd/vector.hpp>
#include <functional>

namespace natus
{
    namespace nsl
    {
        class node : public inode
        {
            natus_this_typedefs( node ) ;

        private:

            natus::ntd::vector< natus::nsl::icomponent_ptr_t > _components ;

        public:

            node( void_t ) noexcept  {}
            node( this_cref_t ) = delete ;
            node( this_rref_t rhv ) noexcept
            {
                _components = std::move( rhv._components ) ;
            }

            virtual ~node( void_t ) noexcept
            {
                for( auto * ptr : _components )
                {
                    natus::memory::global_t::dealloc( ptr ) ;
                }
            }

            size_t get_num_components( void_t ) const noexcept
            {
                return _components.size() ;
            }

            template< typename T >
            this_ref_t add_component( T&& obj ) noexcept
            {
                _components.emplace_back( natus::memory::global_t::alloc( std::move( obj ),
                    natus_log_fn( "node" ) ) ) ;
                return *this ;
            }

            typedef std::function< void_t ( icomponent_ptr_t ) > foreach_comp_funk_t ;
            void_t for_each( foreach_comp_funk_t funk ) noexcept
            {
                for( auto * ptr : _components )
                {
                    funk( ptr ) ;
                }
            }

            virtual void_t apply( ivisitor_ptr_t ) noexcept = 0 ;
        };
        natus_typedef( node ) ;

        // traverses the group with all its child nodes.
        class group : public node
        {
            natus_this_typedefs( group ) ;

        private:

            natus::ntd::vector< node_ptr_t > _nodes ;

        public:

            group( void_t) noexcept {}
            group( this_cref_t ) = delete ;
            group( this_rref_t rhv ) noexcept : node( std::move( rhv ) ) 
            {
                _nodes = std::move( rhv._nodes ) ;
            }
            virtual ~group( void_t ) noexcept
            {
                for( auto * ptr : _nodes ) 
                {
                    natus::memory::global_t::dealloc( ptr ) ;
                }
            }

        public:

            template< typename T >
            this_ref_t add( T && obj ) noexcept
            {
                _nodes.emplace_back( natus::memory::global_t::alloc( std::move( obj ), 
                    natus_log_fn("node") ) ) ;
                return *this ;
            }

            virtual void_t apply( ivisitor_ptr_t vptr ) noexcept 
            {
                vptr->visit( this ) ;
                for( auto * child : _nodes )
                {
                    child->apply( vptr ) ;
                }
                vptr->post_visit( this ) ;
            }
        };
        natus_typedef( group ) ;

        // no further traversal possible
        class leaf : public node
        {
            natus_this_typedefs( leaf ) ;

        private:

        public:

            leaf( void_t ) noexcept{}
            leaf( this_cref_t ) = delete ;
            leaf( this_rref_t rhv ) : node( std::move( rhv ) ) 
            {}
            virtual ~leaf( void_t )
            {}

        public:

            virtual void_t apply( ivisitor_ptr_t vptr ) noexcept 
            {
                vptr->visit( this ) ;
            }
        };
        natus_typedef( leaf ) ;
    }
}