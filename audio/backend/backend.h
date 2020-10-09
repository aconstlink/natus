#pragma once

#include "../api.h"
#include "../typedefs.h"
#include "../result.h"

#include "../object/capture_object.h"

namespace natus
{
    namespace audio
    {
        class NATUS_AUDIO_API backend
        {
            natus_this_typedefs( backend ) ;

        private:

            natus::audio::backend_type _bt = backend_type::unknown ;
            size_t _backend_id = size_t( -1 ) ;

        public: // ctor

            backend( natus::audio::backend_type const bt ) ;
            backend( this_cref_t ) = delete ;
            backend( this_rref_t rhv ) ;
            virtual ~backend( void_t ) ;

        public:

            virtual natus::audio::result configure( natus::audio::capture_object_res_t ) noexcept = 0 ;
            virtual natus::audio::result capture( natus::audio::capture_object_res_t, bool_t const b = true ) noexcept = 0 ;
            virtual void_t begin( void_t ) noexcept = 0 ;
            virtual void_t end( void_t ) noexcept = 0 ;


        public:

            natus::audio::backend_type get_type( void_t ) const noexcept { return _bt ; }

        protected:

            
            size_t get_bid( void_t ) const noexcept { return _backend_id ; }
            void_t set_bid( size_t const bid ) { _backend_id = bid ; }
            void_t set_type( natus::audio::backend_type const bt ) { _bt = bt ; }

        };
        natus_res_typedef( backend ) ;
    }
}