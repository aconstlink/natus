#pragma once

#include "command.h"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace concurrent
    {
        class command_list : public command
        {
            typedef command_list this_t ;
            typedef natus::ntd::vector< command_ptr_t > commands_t ;

        private:

            command_state _state = permanent ;
            size_t _cur_pos = 0 ;
            commands_t _commands ;

        public:

            command_list( void_t ) ;
            command_list( command_state s ) ;
            command_list( this_t const& rhv ) ;
            command_list( this_t&& rhv ) ;

            virtual ~command_list( void_t ) ;

            void_t put_command( command_ptr_t com_ptr ) ;
            virtual command_state execute( void_ptr_t user_ptr ) ;
        };

        typedef command_list* command_list_ptr_t ;
    }
}
