#pragma once

#include "../result.h"

#include "command.h"
#include "command_system_state.h"

#include <natus/ntd/vector.hpp>

namespace natus
{
    namespace concurrent
    {
        /// supposed to run in one thread and processing commands.
        class command_system
        {
            struct command_buffer
            {

                command_buffer( void_t ) : cur_pos( 0 ), buffer( 100 )
                {
                }

                size_t cur_pos ;
                natus::ntd::vector< command_ptr_t > buffer ;

            } ;

            typedef command_buffer command_buffer_t ;
            typedef command_buffer_t* command_buffer_ptr_t ;

        private:

            command_buffer_t _buffer_a ;
            command_buffer_t _buffer_b ;

            /// double buffer
            /// [0]: put commands into
            /// [1]: process commands
            command_buffer_t* _buffers[ 2 ] ;

            size_t _flush ;

            command_system_state _state ;
            mutex_t _buffer_mtx ;

            /// wait here until a flush comes in.
            condition_variable_t _wait_for_flush ;

            /// wait here until all commands are processed.
            condition_variable_t _wait_for_all ;

        public:

            command_system( void_t ) ;
            command_system( command_system&& ) ;

            natus::concurrent::result update( void_ptr_t user_ptr ) ;

            /// tells the system to process all commands. this function does not
            /// block. it sets the flush count, notifies the system about the work to 
            /// be done and returns immediately.
            /// use wait_for_all_commands if blocking is required.
            /// @see wait_for_all_commands
            natus::concurrent::result flush_commands( command_system_state state ) ;

            /// blocks until the system processed all commands.
            void_t wait_for_all_commands( void_t ) ;

            /// blocking sync command
            /// @note
            /// this function blocks execution until the sync object in the command queue
            /// is processed. for this reason, this function implicitly flushes the command queue
            /// after the sync command is placed.
            result sync( command_system_state state ) ;

            /// allows to place a bare command into the command buffer.
            /// depending on the buffer method used, there is an active buffer that is 
            /// used for this operation. Methods could be single, double, triple buffer...
            /// @note: this function is thread-safe
            /// this function locks the currently used buffer. 
            void_t put_command( command_ptr_t com ) ;
        } ;

        typedef command_system* command_system_ptr_t ;
    }
}