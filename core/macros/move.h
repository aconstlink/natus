#pragma once

/// moves a pointer - reduces code by one line in rval functions
#define natus_move_ptr( to, from ) \
    to = from ; \
    from = nullptr

/// moves a pointer - reduces code by one line in rval functions
#define natus_move_member_ptr( var_name, from ) \
    var_name = from.var_name ; \
    from.var_name = nullptr 

#define natus_move( var_name, from ) \
    var_name = ::std::move( from.var_name ) 



