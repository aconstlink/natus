#pragma once

#include "typedefs.h"

#include <natus/ntd/vector.hpp>
#include <natus/ntd/string.hpp>
#include <natus/concurrent/mutex.hpp>

namespace natus
{
    namespace io
    {
        class monitor
        {
            natus_this_typedefs( monitor ) ;

        public:

            enum class notify
            {
                none,
                deletion,
                change,
                num_values
            };
            static natus::ntd::string_cref_t to_string( this_t::notify const n ) noexcept
            {
                static natus::ntd::string_t __strings[] = { "none", "deletion", "change", "invalid" } ;
                return __strings[ size_t(n) >= size_t(notify::num_values) ? size_t(notify::num_values) : size_t(n) ] ;
            }

        private:

            natus::concurrent::mutex_t _mtx ;

            struct data
            {
                notify n ;
                natus::ntd::string_t loc ;
            };
            natus_typedef( data ) ;

            natus::ntd::vector< data_t > _changed ;

        public:

            monitor( void_t ){}

            monitor( this_cref_t ) = delete ;

            monitor( this_rref_t rhv ) 
            {
                _changed = ::std::move( rhv._changed ) ;
            }

            ~monitor( void_t ) {}

            this_ref_t operator = ( this_cref_t rhv ) = delete ;

            this_ref_t operator = ( this_rref_t rhv ) 
            {
                _changed = ::std::move( rhv._changed ) ;
                return *this ;
            }

        public:

            void_t trigger_changed( natus::ntd::string_cref_t loc, notify const n ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                this_t::insert( { n, loc } ) ;
            }

            typedef ::std::function< void_t ( natus::ntd::string_cref_t, this_t::notify const ) > foreach_funk_t ;
            void_t for_each_and_swap( foreach_funk_t funk )
            {
                natus::ntd::vector< this_t::data_t > tmp ;
                {
                    natus::concurrent::lock_guard_t lk( _mtx ) ;
                    tmp = ::std::move( _changed ) ;
                }

                for( auto const & item : tmp ) 
                {
                    funk( item.loc, item.n ) ;
                }
            }

        private:

            bool_t has( natus::ntd::string_cref_t loc ) noexcept
            {
                for( auto const & item : _changed )
                {
                    if( item.loc == loc ) return true ;
                }
                return false ;
            }

            void_t insert( data_cref_t d ) noexcept
            {
                if( natus::core::is_not( this_t::has( d.loc ) ) )
                {
                    _changed.emplace_back( d ) ;
                }
            }
        };
        natus_res_typedef( monitor ) ;
    }
}