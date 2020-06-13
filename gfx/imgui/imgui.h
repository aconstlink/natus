#pragma once

#include "../api.h"
#include "../typedefs.h"

#include <natus/soil/macros.h>
#include <imgui/imgui.h>

namespace natus
{
    namespace gfx
    {
        class NATUS_GFX_API imgui
        {
            natus_this_typedefs( imgui ) ;

        private:

            ImGuiContext* _ctx ;

        public:

            imgui( void_t ) ;
            imgui( this_cref_t ) = delete ;
            imgui( this_rref_t ) ;
            ~imgui( void_t ) ;

            void_t init( void_t ) ;
            void_t render( void_t ) ;

            typedef ::std::function< void_t ( ImGuiContext* ) > exec_funk_t ;
            void_t execute( exec_funk_t ) ;
        };
        natus_soil_typedef( imgui ) ;
    }
}