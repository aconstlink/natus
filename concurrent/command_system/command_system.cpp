//------------------------------------------------------------
// snakeoil (c) Alexis Constantin Link
// Distributed under the MIT license
//------------------------------------------------------------
#include "command_system.h"
#include "../primitive/sync_object.hpp"

using namespace natus::concurrent ;
using namespace natus::core::types ;

//*****************************************************************************************************
command_system::command_system( void_t )
{ 
    _flush = 0 ; 
    _buffers[0] = &_buffer_a ; 
    _buffers[1] = &_buffer_b ; 
    _state = css_wait ; 
}

//*****************************************************************************************************
command_system::command_system( command_system && ) 
{
    

}

//*****************************************************************************************************
natus::concurrent::result command_system::update( void_ptr_t user_ptr ) 
{
    while( true )
    {
        if( _flush == 0 && _state == css_proceed ) break ;

        {
            lock_t lk(_buffer_mtx) ;
            while( _flush == 0 && _state == css_wait ) _wait_for_flush.wait( lk ) ;
        }

        // swap the buffers first, so further commands can be queued.
        // plus the back buffer can be processed without any sync
        {
            command_buffer_ptr_t tmp = _buffers[1] ;
            _buffers[1] = _buffers[0] ;

            lock_t lk(_buffer_mtx) ;
            _buffers[0] = tmp ;
        }

        command_buffer_t & com_buffer = *_buffers[1] ;

        // the front buffer has been swapped in the last step.
        // now it can be analyzed and processed.
        if( com_buffer.cur_pos > 0 )
        {
            for( size_t i=0; i<com_buffer.cur_pos; ++i )
            {
                if( com_buffer.buffer[i]->execute(user_ptr) == natus::concurrent::nonpermanent )
                    natus::concurrent::memory::dealloc<natus::concurrent::command>(com_buffer.buffer[i]) ;
            }
            com_buffer.cur_pos = 0 ;
        }

        // decrement the flush counter.
        // if the flush counter reaches 0 _flush == 0, all waiting 
        // threads need to be notified about that fact. 
        {
            lock_t lk(_buffer_mtx) ;
            --_flush ;					 
        }	
        _wait_for_all.notify_all() ;
    }
    return natus::concurrent::ok ;
}

//*****************************************************************************************************
natus::concurrent::result command_system::flush_commands( command_system_state state ) 
{
    {
        lock_t lk(_buffer_mtx) ;
        _flush = _flush < 2 ? _flush+1 : _flush ;
        _state = state == css_preserve_state ? _state : state ;
    }
    _wait_for_flush.notify_one() ;
    return natus::concurrent::ok ;
}

//*****************************************************************************************************
void_t command_system::wait_for_all_commands( void_t ) 
{
    lock_t lk(_buffer_mtx) ;
    while( _flush != 0 ) _wait_for_all.wait(lk) ;
}		

//*****************************************************************************************************
result command_system::sync( command_system_state state )
{
    struct __sync_command
    {
        natus::concurrent::sync_object_ptr_t _sync ;

        void_t execute( void_ptr_t ) { _sync->set_and_signal() ; }
        
        __sync_command( void_t ){ _sync = nullptr ; }
        __sync_command( natus::concurrent::sync_object_ptr_t sync_ptr ){ _sync = sync_ptr ; }
    };
    typedef natus::concurrent::carrier_command<__sync_command> command_t ;

    natus::concurrent::sync_object s ;
    command_system::put_command( natus::concurrent::memory::alloc<command_t>( command_t(__sync_command(&s)) ) ) ;
    flush_commands(state) ;
    s.wait() ;
    
    return natus::concurrent::ok ;
}

//*****************************************************************************************************
void_t command_system::put_command( command_ptr_t com )
{
    /// ok, if the buffer is not big enough, it is resized. this means long locking.
    /// the solution could be to use a triple buffer approach by swapping with a third buffer
    /// and resizing while the active buffer for other threads is still available.
    /// @todo evaluate triple buffer approach.
    lock_t lk(_buffer_mtx) ;

    command_buffer_t & cb_ref = (*_buffers[0]) ;
    if( cb_ref.cur_pos == cb_ref.buffer.size() )
    {
        cb_ref.buffer.resize( cb_ref.buffer.size() + 100 ) ;
    }

    cb_ref.buffer[cb_ref.cur_pos++] = com ;
}

