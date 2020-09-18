
#pragma once

#include "../interfaces.hpp"

#include <natus/log/global.h>

namespace natus
{
    namespace nsl
    {
        class visitor : public ivisitor
        {
        public:

            virtual void_t visit( inode_ptr_t ) noexcept {}
            virtual void_t visit( node_ptr_t ) noexcept {}
            virtual void_t visit( leaf_ptr_t ) noexcept {}
            virtual void_t visit( group_ptr_t ) noexcept {}
            virtual void_t post_visit( group_ptr_t ) noexcept {}
        };

        class print_visitor : public visitor
        {
            natus_this_typedefs( print_visitor ) ;
        
        private:

            size_t num_tabs = 0 ;
            natus::ntd::string_t spaces ;

            void_t increment( void_t ) noexcept
            {
                spaces = this_t::make_tabs( ++num_tabs ) ;
            }

            void_t decrement( void_t ) noexcept
            {
                spaces = this_t::make_tabs( --num_tabs ) ;
            }

            natus::ntd::string_t make_tabs( size_t const number ) const 
            {
                natus::ntd::string_t ret ;
                for( size_t i=0; i<number; ++i )
                    ret += "    " ;
                return std::move( ret ) ;
            }

            void_t print_components( node_ptr_t nptr ) const 
            {
                size_t num_comps = 0 ;
                nptr->for_each( [&] ( natus::nsl::icomponent_ptr_t cptr )
                {
                    num_comps++ ;
                } ) ;

                natus::log::global_t::status( spaces + "[components] : " + std::to_string( num_comps ) ) ;
            }

        public:

            virtual void_t visit( inode_ptr_t nptr ) noexcept 
            {
                natus::log::global_t::status( spaces + "UNKNOWN") ;
            }

            virtual void_t visit( node_ptr_t nptr ) noexcept 
            {
                natus::log::global_t::status(spaces + "[node]") ;
                this_t::print_components( nptr ) ;
            }

            virtual void_t visit( leaf_ptr_t nptr ) noexcept 
            {
                natus::log::global_t::status(spaces + "[leaf]") ;
                this_t::print_components( nptr ) ;
            }

            virtual void_t visit( group_ptr_t nptr ) noexcept 
            {
                natus::log::global_t::status(spaces + "[group->]") ;
                this_t::increment() ;
                this_t::print_components( nptr ) ;
            }

            virtual void_t post_visit( group_ptr_t nptr ) noexcept 
            {
                this_t::decrement() ;
                natus::log::global_t::status(spaces + "[group<-]") ;
            }
        };
        natus_typedef( print_visitor ) ;
    }
}