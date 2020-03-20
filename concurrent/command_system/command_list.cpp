//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#include "command_list.h"

using namespace natus::concurrent ;

//*****************************************************************************************************
command_list::command_list( void_t ) 
{}

//*****************************************************************************************************
command_list::command_list( command_state s ) : _state(s)
{}

//*****************************************************************************************************
command_list::~command_list( void_t ) 
{}

//*****************************************************************************************************
command_list::command_list( this_t const & rhv ) : 
    _state(rhv._state), _cur_pos(rhv._cur_pos), _commands(rhv._commands)
{}

//*****************************************************************************************************
command_list::command_list( this_t && rhv ) :
    _state(rhv._state), _cur_pos(rhv._cur_pos), _commands( ::std::move( rhv._commands ) )
{}

//*****************************************************************************************************
void_t command_list::put_command( command_ptr_t com_ptr )
{
	if( _cur_pos == _commands.size() )
	{
		_commands.resize(_commands.size()+100) ;
	}
    _commands[_cur_pos++] = com_ptr ;
}

//*****************************************************************************************************
command_state command_list::execute( void_ptr_t user_ptr )
{
    for( size_t i=0; i<_cur_pos; ++i )
    {
        if( _commands[i]->execute(user_ptr) == natus::concurrent::nonpermanent ) 
            natus::concurrent::memory::dealloc<command>( _commands[i] ) ;
    }

	_cur_pos = 0 ;
    return _state ;
}



