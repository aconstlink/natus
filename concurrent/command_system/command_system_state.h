//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#ifndef _NATUS_CONCURRENT_COMMAND_SYSTEM_STATE_H_
#define _NATUS_CONCURRENT_COMMAND_SYSTEM_STATE_H_

namespace natus::concurrent
{
	enum command_system_state
	{
		css_proceed,
		css_wait,
		css_preserve_state
	};
}

#endif

