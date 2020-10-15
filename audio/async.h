
#pragma once

#include "backend/backend.h"
#include "object/capture_object.h"

#include <natus/concurrent/mutex.hpp>
#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace audio
    {
        class NATUS_AUDIO_API async
        {
            natus_this_typedefs( async ) ;

        private:

            /// decorated backend.
            backend_res_t _backend ;
            
        private:

            struct capture_config_data
            {
                natus::audio::result_res_t res ;
                natus::audio::capture_type ct ;
                natus::audio::capture_object_res_t config ;
            };
            typedef natus::ntd::vector< capture_config_data > capture_configs_t ;
            capture_configs_t _capture_configs ;
            natus::concurrent::mutex_t _capture_configs_mtx ;

            struct capture_data
            {
                natus::audio::result_res_t res ;
                natus::audio::capture_object_res_t config ;
                bool_t begin_capture = false ;
            };
            typedef natus::ntd::vector< capture_data > captures_t ;
            captures_t _captures ;
            natus::concurrent::mutex_t _captures_mtx ;

        private: // sync

            natus::concurrent::mutex_t _frame_mtx ;
            natus::concurrent::condition_variable_t _frame_cv ;
            size_t _num_enter = 0 ;

        public:

            async( void_t ) ;
            async( backend_res_t ) ;
            async( this_cref_t ) = delete ;
            async( this_rref_t ) ;
            ~async( void_t ) ;

        public:

            this_ref_t configure( natus::audio::capture_type const, natus::audio::capture_object_res_t, 
                natus::audio::result_res_t = natus::audio::result_res_t() ) noexcept ;

            this_ref_t capture( natus::audio::capture_object_res_t, bool_t const do_capture = true,
                natus::audio::result_res_t = natus::audio::result_res_t() ) noexcept ;

        public:

            /// render thread update function - DO NOT USE.
            void_t system_update( void_t ) noexcept ;

        public: // sync

            bool_t enter_frame( void_t ) ;
            void_t leave_frame( void_t ) ;
            void_t wait_for_frame( void_t ) ;
        };
        natus_res_typedef( async ) ;

        // restricts access to the async
        class async_access
        {
            natus_this_typedefs( async_access ) ;

        private:

            natus::audio::async_res_t _async ;
            bool_ptr_t _access = nullptr ;

        public:

            async_access( natus::audio::async_res_t const& r, bool_ptr_t b ) :
                _async( r ), _access( b ) {}
            async_access( natus::audio::async_res_t&& r, bool_ptr_t b ) :
                _async( ::std::move( r ) ), _access( b ) {}

        public:

            async_access( void_t ) {}
            async_access( this_cref_t rhv ) :
                _async( rhv._async ), _access( rhv._access ) {}

            async_access( this_rref_t rhv ) :
                _async( ::std::move( rhv._async ) ), _access( rhv._access ) {}

            ~async_access( void_t ) {}

            this_ref_t operator = ( this_cref_t rhv )
            {
                _async = rhv._async ;
                _access = rhv._access ;
                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                _async = ::std::move( rhv._async ) ;
                _access = rhv._access ;
                return *this ;
            }

            /// communication to the audio system is only possible 
            /// during rendering type.
            bool_t is_accessable( void_t ) const noexcept { return *_access ; }

            this_ref_t configure( natus::audio::capture_type const ct, natus::audio::capture_object_res_t config,
                natus::audio::result_res_t res = natus::audio::result_res_t() ) noexcept
            {
                _async->configure( ct, config, res ) ;
                return *this ;
            }

            this_ref_t capture( natus::audio::capture_object_res_t config, bool_t do_capture = true,
                natus::audio::result_res_t res = natus::audio::result_res_t() ) noexcept
            {
                _async->capture( config, do_capture, res ) ;
                return *this ;
            }

        public:

        };
        natus_typedef( async_access ) ;
    }
}