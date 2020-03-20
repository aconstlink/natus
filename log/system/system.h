
#pragma once

#include "isystem.h"

#include "../logger/store_logger.h"

#include <vector>
#include <map>
#include <string>

namespace natus
{
    namespace log
    {
        class system : public isystem
        {
            natus_this_typedefs( system ) ;
            natus_typedefs( ::std::vector< natus::log::ilogger_ptr_t >, loggers ) ;

            loggers_t _loggers ;
            natus::log::store_logger_t _slogger ;

        private:

            natus::log::ilogger_ptr_t _default_logger ;
            natus::log::ilogger_ptr_t _this_default_logger ;

        public:

            static this_ptr_t create( void_t ) ;
            static void_t destroy( this_ptr_t ) ;

        public:

            system( void_t ) ;
            system( this_rref_t ) ;
            virtual ~system( void_t ) ;

            virtual natus::log::result log( natus::log::log_level ll, ::std::string const& msg ) ;
            virtual natus::log::result add_logger( natus::log::ilogger_ptr_t lptr ) ;
            virtual natus::log::ilogger_ptr_t set_default_logger( natus::log::ilogger_ptr_t lptr ) ;

            natus::log::store_logger_cptr_t get_store( void_t ) const ;
        };
        natus_typedef( system ) ;
    }
}