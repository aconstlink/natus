
#pragma once

#include "../backend.h"

namespace natus
{
    namespace audio
    {
        class NATUS_AUDIO_API xaudio2_backend : public natus::audio::backend
        {
            natus_this_typedefs( xaudio2_backend ) ;


            struct pimpl ;
            pimpl* _pimpl = nullptr ;

        public:

            xaudio2_backend( void_t ) noexcept ;
            xaudio2_backend( this_cref_t ) = delete ;
            xaudio2_backend( this_rref_t ) noexcept ;
            virtual ~xaudio2_backend( void_t ) noexcept ;

            this_ref_t operator = ( this_rref_t ) noexcept ;

        public:

            virtual natus::audio::result configure( natus::audio::capture_type const, 
                natus::audio::capture_object_res_t ) noexcept ;

            virtual natus::audio::result capture( natus::audio::capture_object_res_t, bool_t const b = true ) noexcept ;
            virtual natus::audio::result release( natus::audio::capture_object_res_t ) noexcept ;

            virtual natus::audio::result configure( natus::audio::buffer_object_res_t ) noexcept ;
            virtual natus::audio::result update( natus::audio::buffer_object_res_t ) noexcept ;
            virtual natus::audio::result execute( natus::audio::buffer_object_res_t, natus::audio::backend::execute_detail_cref_t ) noexcept ;
            virtual natus::audio::result release( natus::audio::buffer_object_res_t ) noexcept ;

        public:

            virtual void_t init( void_t ) noexcept ;
            virtual void_t release( void_t ) noexcept ;
            virtual void_t begin( void_t ) noexcept ;
            virtual void_t end( void_t ) noexcept ;

        private:

            static size_t create_backend_id(  void_t ) noexcept ;
        };
        natus_res_typedef( xaudio2_backend ) ;
    }
}