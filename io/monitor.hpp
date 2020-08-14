#pragma once

#include "typedefs.h"

#include <natus/std/vector.hpp>
#include <natus/std/string.hpp>
#include <natus/concurrent/mutex.hpp>

namespace natus
{
    namespace io
    {
        class monitor
        {
            natus_this_typedefs( monitor ) ;

        private:

            natus::concurrent::mutex_t _mtx ;
            natus::std::vector< natus::std::string_t > _changed ;

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

            void_t trigger_changed( natus::std::string_cref_t loc ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _mtx ) ;
                this_t::insert( loc ) ;
            }

            typedef ::std::function< void_t ( natus::std::string_cref_t ) > foreach_funk_t ;
            void_t for_each_and_swap( foreach_funk_t funk )
            {
                natus::std::vector< natus::std::string_t > tmp ;
                {
                    natus::concurrent::lock_guard_t lk( _mtx ) ;
                    tmp = ::std::move( _changed ) ;
                }

                for( auto const & item : tmp ) 
                {
                    funk( item ) ;
                }
            }

        private:

            bool_t has( natus::std::string_cref_t loc ) noexcept
            {
                for( auto const & item : _changed )
                {
                    if( item == loc ) return true ;
                }
                return false ;
            }

            void_t insert( natus::std::string_cref_t loc ) noexcept
            {
                if( natus::core::is_not( this_t::has( loc ) ) )
                {
                    _changed.emplace_back( loc ) ;
                }
            }
        };
        natus_res_typedef( monitor ) ;
    }
}