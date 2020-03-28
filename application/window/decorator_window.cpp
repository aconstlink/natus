
#include "decorator_window.h"

using namespace natus::application ;

//***********************************************************************
decorator_window::decorator_window( void_t ) 
{

}

//***********************************************************************
decorator_window::decorator_window( iwindow_ptr_t wnd_ptr ) 
{
    _decorated_ptr = wnd_ptr ;
}

//***********************************************************************
decorator_window::decorator_window( this_rref_t rhv ) 
{
    _decorated_ptr = rhv._decorated_ptr ;
    rhv._decorated_ptr = nullptr ;
}

//***********************************************************************
decorator_window::~decorator_window( void_t )
{

}

//***********************************************************************
natus::application::result decorator_window::subscribe( iwindow_message_listener_ptr_t ptr ) 
{
    if( this_t::has_no_decorated() )
        return natus::application::no_decorated ;

    return _decorated_ptr->subscribe( ptr ) ;
}

//***********************************************************************
natus::application::result decorator_window::unsubscribe( iwindow_message_listener_ptr_t ptr ) 
{
    if( this_t::has_no_decorated() )
        return natus::application::no_decorated ;

    return _decorated_ptr->unsubscribe( ptr ) ;
}

//***********************************************************************
natus::application::result decorator_window::destroy( void_t )
{
    if( this_t::has_no_decorated() )
        return natus::application::ok ;

    auto tmp = _decorated_ptr ;
    _decorated_ptr = nullptr ;

    return tmp->destroy() ;
}

//***********************************************************************
iwindow_handle_ptr_t decorator_window::get_handle( void_t )
{
    if( this_t::has_no_decorated() ) 
        return nullptr ;

    return _decorated_ptr->get_handle() ;
}

//***********************************************************************
natus_std::string_cref_t decorator_window::get_name( void_t ) const
{
    static const std::string invalid_name("") ;

    if( this_t::has_no_decorated() )
        return invalid_name ;

    return _decorated_ptr->get_name() ;
}

//***********************************************************************
void_t decorator_window::send_close( void_t )
{
    return _decorated_ptr->send_close() ;
}

//***********************************************************************
void_t decorator_window::send_toggle( natus::application::toggle_window_in_t di ) 
{
    return _decorated_ptr->send_toggle( di ) ;
}

//***********************************************************************
bool_t decorator_window::has_decorated( void_t ) const 
{
    return _decorated_ptr != nullptr ;
}

//***********************************************************************
bool_t decorator_window::has_no_decorated( void_t ) const 
{
    return _decorated_ptr == nullptr ;
}

//***********************************************************************
natus::application::result decorator_window::set_decorated( iwindow_ptr_t wnd_ptr ) 
{
    _decorated_ptr = wnd_ptr ;

    return natus::application::ok ;
}

//***********************************************************************
iwindow_ptr_t decorator_window::get_decorated( void_t ) 
{
    return _decorated_ptr ;
}

