
#pragma once

#include "../backend.h"

namespace natus
{
    namespace audio
    {
        class NATUS_AUDIO_API oal_backend : public natus::audio::backend
        {
            natus_this_typedefs( oal_backend ) ;

        private:

            struct pimpl ;
            pimpl* _pimpl ;

            size_t _what_u_hear_count = 0 ;

        public:

            oal_backend( void_t ) noexcept ;
            oal_backend( this_cref_t ) = delete ;
            oal_backend( this_rref_t ) noexcept ;
            virtual ~oal_backend( void_t ) ;

            this_ref_t operator = ( this_rref_t ) noexcept ;

        public:

            virtual natus::audio::result configure( natus::audio::capture_type const, 
                natus::audio::capture_object_res_t ) noexcept ;

            virtual natus::audio::result capture( natus::audio::capture_object_res_t, bool_t const b = true ) noexcept ;

        public:

            virtual void_t begin( void_t ) noexcept ;
            virtual void_t end( void_t ) noexcept ;

        private:

            static size_t create_backend_id(  void_t ) noexcept ;
        };
        natus_res_typedef( oal_backend ) ;
    }
}