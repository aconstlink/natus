
#include "window_message_receiver.h"

using namespace natus::application ;

//*************************************************************************************
window_message_receiver::this_ptr_t window_message_receiver::create( natus::std::string_cref_t purpose ) 
{
    return this_t::create( this_t(), purpose ) ;
}

//*************************************************************************************
window_message_receiver::this_ptr_t window_message_receiver::create( this_rref_t rhv, 
    natus::std::string_cref_t purpose ) 
{
    return natus::memory::global_t::alloc( ::std::move(rhv), purpose ) ;
}

//*************************************************************************************
void_t window_message_receiver::destroy( this_ptr_t ptr ) 
{
    natus::application::memory::dealloc( ptr ) ;
}

//*************************************************************************************
window_message_receiver::window_message_receiver( void_t ) 
{
    _has_any_change.set( false ) ;
}

//*************************************************************************************
window_message_receiver::window_message_receiver( this_rref_t rhv ) :
    _states( ::std::move( rhv._states ) ),
    _has_any_change( ::std::move( rhv._has_any_change ) )
{}

//*************************************************************************************
window_message_receiver::~window_message_receiver( void_t )
{

}

//*************************************************************************************
void_t window_message_receiver::on_screen( screen_dpi_message_cref_t msg ) 
{
    natus::concurrent::lock_t lk(_mtx) ;
    _states.set( [&]( state_vector_ref_t obj )
    {
        obj.dpi_msg_changed = true ;
        obj.dpi_msg = msg ;
    }) ;
    _has_any_change.set(true) ;
}

//*************************************************************************************
void_t window_message_receiver::on_screen( screen_size_message_cref_t msg ) 
{
    natus::concurrent::lock_t lk(_mtx) ;
    _states.set( [&]( state_vector_ref_t obj )
    {
        obj.msize_msg_changed = true ;
        obj.msize_msg = msg ;
    }) ;
    _has_any_change.set(true) ;
}

//*************************************************************************************
void_t window_message_receiver::on_resize( resize_message_cref_t msg )
{
    natus::concurrent::lock_t lk(_mtx) ;
    _states.set( [&]( state_vector_ref_t obj )
    {
        obj.resize_changed = true ;
        obj.resize_msg = msg ;
    }) ;
    _has_any_change.set(true) ;
}

//*************************************************************************************
void_t window_message_receiver::on_visible( show_message_cref_t msg ) 
{
    natus::concurrent::lock_t lk(_mtx) ;
    _states.set( [&]( state_vector_ref_t obj )
    {
        obj.show_changed = true ;
        obj.show_msg = msg ;        
    }) ;
    _has_any_change.set(true) ;
}

//*************************************************************************************
void_t window_message_receiver::on_close( close_message_cref_t msg ) 
{
    natus::concurrent::lock_t lk(_mtx) ;
    _states.set( [&]( state_vector_ref_t obj )
    {
        obj.close_changed = true ;
        obj.close_msg = msg ;
    }) ;
    _has_any_change.set(true) ;
}

//*************************************************************************************
bool_t window_message_receiver::has_screen_changed( screen_dpi_message_ref_t msg_out ) 
{
    msg_out = _states.get().dpi_msg ;
    return _states.get().dpi_msg_changed ;
}

//*************************************************************************************
bool_t window_message_receiver::has_screen_changed( screen_size_message_ref_t msg_out ) 
{
    msg_out = _states.get().msize_msg ;
    return _states.get().msize_msg_changed ;
}

//*************************************************************************************
bool_t window_message_receiver::has_resize_changed( resize_message_ref_t msg_out ) 
{
    msg_out = _states.get().resize_msg ;
    return _states.get().resize_changed ;
}

//*************************************************************************************
bool_t window_message_receiver::has_visible_changed( show_message_ref_t msg_out ) 
{
    msg_out = _states.get().show_msg ;
    return _states.get().show_changed ;
}

//*************************************************************************************
bool_t window_message_receiver::has_close_changed( close_message_ref_t msg_out ) 
{
    msg_out = _states.get().close_msg ;
    return _states.get().close_changed ;
}

//*************************************************************************************
void_t window_message_receiver::destroy( void_t )
{
    this_t::destroy( this ) ;
}

//*************************************************************************************
void_t window_message_receiver::swap( void_t ) 
{
    natus::concurrent::lock_t lk( _mtx ) ;

    _states.swap() ;
    _has_any_change.swap() ;

    this_t::reset_change_flags() ;
}

//*************************************************************************************
bool_t window_message_receiver::has_any_change( void_t ) 
{
    return _has_any_change.get() ;
}

//*************************************************************************************
void_t window_message_receiver::reset_change_flags( void_t ) 
{
    _states.set( [&]( state_vector_ref_t obj )
    {
        obj.resize_changed = false ;
        obj.show_changed = false ;
        obj.close_changed = false ;
        obj.msize_msg_changed = false ;
        obj.dpi_msg_changed = false ;
    }) ;

    _has_any_change.set(false) ;
}

