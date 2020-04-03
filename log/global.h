#pragma once

#include "typedefs.h"
#include "protos.h"
#include "api.h"
#include "function.h"

#include "log_level.h"

#include <mutex>

namespace natus
{
    namespace log
    {
        class NATUS_LOG_API global
        {
            natus_this_typedefs( global ) ;

        private:

            /// should be of type "system", so the log system could be exchanged
            /// by any other system. But since the system stores modules, that is
            /// currently not a big problem.
            natus::log::system_ptr_t __default_log_system = nullptr ;

        private: // singleton

            static ::std::mutex _mtx ;
            static this_ptr_t _ptr ;

        public:

            global( void_t ) ;
            global( this_cref_t ) = delete ;
            global( this_rref_t ) ;
            ~global( void_t ) ;

        public: // singleton functions

            static this_ptr_t init( void_t ) ;
            static void_t deinit( void_t ) ;

            static this_ptr_t get( void_t ) ;

            static void_t add_logger( natus::log::ilogger_ptr_t ) ;
            static natus::log::store_logger_cptr_t get_store( void_t ) ;

        public:

            static void_t message( log_level level, string_cref_t msg ) ;
            static bool_t message( bool_t condition, log_level level, string_cref_t msg ) ;
            static void_t status( string_cref_t msg ) ;
            static bool_t status( bool_t condition, string_cref_t msg ) ;
            static void_t warning( string_cref_t msg ) ;
            static bool_t warning( bool_t condition, string_cref_t msg ) ;
            static void_t error( string_cref_t msg ) ;
            static void_t error_and_exit( string_cref_t msg ) ;
            static void_t error_and_exit( bool_t condition, string_cref_t msg ) ;
            static bool_t error( bool_t condition, string_cref_t msg );
            static void_t critical( string_cref_t msg );
            static bool_t critical( bool_t condition, string_cref_t msg );

        };
        natus_typedef( global ) ;
    }
}
