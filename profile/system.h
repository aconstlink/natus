
#pragma once

#include "api.h"
#include "typedefs.h"

#include "entry.hpp"

#include <natus/concurrent/mutex.hpp>
#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace profile
    {
        class NATUS_PROFILE_API system
        {
            natus_this_typedefs( system ) ;

        public:

            natus_typedefs( natus::ntd::vector< natus::profile::entry_t >, entries ) ;

        private:

            natus::concurrent::mutex_t _mtx ;
            entries_t _entries ;

        public:

            void_t add_entry( natus::profile::entry_cref_t ) noexcept ;
            void_t add_entry( natus::profile::entry_rref_t ) noexcept ;

            void_t unique_entry( natus::profile::entry_cref_t ) noexcept ;
            void_t unique_entry( natus::profile::entry_rref_t ) noexcept ;

            entries_t get_and_reset_entries( void_t ) noexcept ;

            
            void_t reset( void_t ) noexcept ;
        };
        natus_typedef( system ) ;
    }
}