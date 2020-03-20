
#include "task_graph.h"
#include "tasks.h"

#include <natus/log/global.h>
#include <natus/core/assert.h>

using namespace natus::concurrent ;

//*************************************************************************************
task_graph::task_graph( void_t )
{
    this_t::ensure_not_nullptr_and_connect() ;    
}

//*************************************************************************************
task_graph::task_graph( natus::concurrent::itask_ptr_t begin, natus::concurrent::itask_ptr_t end ) :
    _begin(begin), _end(end)
{
    this_t::ensure_not_nullptr_and_connect() ;
}

//*************************************************************************************
task_graph::task_graph( natus::concurrent::itask_ptr_t single ) : _begin( single )
{
    this_t::ensure_not_nullptr_and_connect() ;
}

//*************************************************************************************
task_graph::task_graph( this_rref_t rhv )
{
    natus_move_member_ptr( _begin, rhv ) ;
    natus_move_member_ptr( _end, rhv ) ;

    natus::log::global::error_and_exit( natus::core::is_nullptr(_begin) || natus::core::is_nullptr(_end),
        "[natus::concurrent::task_graph::task_graph] : moved pointer must not be nullptr." ) ;
}

//*************************************************************************************
task_graph::~task_graph( void_t )
{
    if( natus::core::is_not_nullptr(_begin) )
    {
        _begin->destroy() ;
        _begin = nullptr ;
    }

    if( natus::core::is_not_nullptr(_end) )
    {
        _end->destroy() ;
        _end = nullptr ;
    }
}

//*************************************************************************************
natus::concurrent::itask_ptr_t task_graph::begin_moved( void_t ) 
{
    auto tmp = _begin ;
    _begin = nullptr ;
    return tmp ;
}

//*************************************************************************************
natus::concurrent::itask_ptr_t task_graph::end_moved( void_t )
{
    auto tmp = _end ;
    _end = nullptr ;
    return tmp ;
}

//*************************************************************************************
void_t task_graph::dont_destroy( this_rref_t rhv ) 
{
    rhv.begin_moved() ;
    rhv.end_moved() ;
}

//*************************************************************************************
bool_t task_graph::operator == (this_ref_t rhv) const
{
    return _begin == rhv._begin && _end == rhv._end ;
}

//*************************************************************************************
/*task_graph::this_ref_t task_graph::operator = ( this_rref_t rhv ) 
{
    natus_move_member_ptr( _begin, rhv ) ;
    natus_move_member_ptr( _end, rhv ) ;

    return *this ;
}*/

//*************************************************************************************
task_graph::this_ref_t task_graph::then( natus::concurrent::itask_ptr_t ptr )
{
    _end = _begin->then( _end )->then( ptr ) ;

    return *this ;
}

//*************************************************************************************
task_graph::this_ref_t task_graph::then( this_rref_t other ) 
{
    _end = _begin->then( _end )->then( other.begin_moved() )->then( other.end_moved() ) ;

    return *this ;
}

//*************************************************************************************
task_graph::this_ref_t task_graph::in_between( natus::concurrent::itask_ptr_t tptr )
{
    if( natus::core::is_nullptr(tptr) )
        return *this ;

    _end = _begin->then( tptr )->then( _end ) ;

    return *this ;
}

//*************************************************************************************
task_graph::this_ref_t task_graph::in_between( this_rref_t rhv ) 
{
    _end = _begin->then( rhv.begin_moved() )->then( rhv.end_moved() )->then( _end ) ;

    return *this ;
}

//*************************************************************************************
void_t task_graph::ensure_not_nullptr_and_connect( void_t ) 
{
    if( natus::core::is_nullptr( _begin ) )
    {
        _begin = natus::concurrent::void_funk_task_t::create_noop(
            "[natus::concurrent::task_graph::task_graph] : default begin" ) ;
    }

    if( natus::core::is_nullptr( _end ) )
    {
        _end = natus::concurrent::void_funk_task_t::create_noop(
            "[natus::concurrent::task_graph::task_graph] : default end" ) ;
    }

    // security connection so that task processing 
    // does not stop.
    _begin->then( _end ) ;
}


//
// task_graph_dyn
//


//*************************************************************************************
task_graph_dyn::task_graph_dyn( natus::concurrent::itask_ptr_t begin, natus::concurrent::itask_ptr_t end ) :
    _begin(begin), _end(end)
{
    ensure_not_nullptr_and_connect() ;
}

//*************************************************************************************
task_graph_dyn::task_graph_dyn( natus::concurrent::itask_ptr_t single ) : _begin(single)
{
    ensure_not_nullptr_and_connect() ;
}

//*************************************************************************************
task_graph_dyn::task_graph_dyn( this_rref_t rhv )
{
    natus_move_member_ptr( _begin, rhv ) ;
    natus_move_member_ptr( _end, rhv ) ;
}

//*************************************************************************************
task_graph_dyn::task_graph_dyn( task_graph_rref_t other ) : _begin(other.begin_moved()),
    _end(other.end_moved())
{}

//*************************************************************************************
task_graph_dyn::task_graph_dyn( this_cref_t rhv ) : _begin(rhv._begin), _end(rhv._end)
{
    
}

//*************************************************************************************
task_graph_dyn::~task_graph_dyn( void_t )
{
    // dynamic task graph is not destroying the tasks.
}

//*************************************************************************************
natus::concurrent::itask_ptr_t task_graph_dyn::begin( void_t )
{
    return _begin ;
}

//*************************************************************************************
natus::concurrent::itask_ptr_t task_graph_dyn::end( void_t )
{
    return _end ;
}

//*************************************************************************************
bool_t task_graph_dyn::operator == (this_cref_t rhv) const
{
    return _begin == rhv._begin && _end == rhv._end ;
}

//*************************************************************************************
task_graph_dyn::this_ref_t task_graph_dyn::then( natus::concurrent::itask_ptr_t ptr )
{
    _end->then( ptr ) ;
    return *this ;
}

//*************************************************************************************
task_graph_dyn::this_ref_t task_graph_dyn::then( this_rref_t other )
{
    return this_t::then( this_t( ::std::move(other) ).begin() ) ;
}

//*************************************************************************************
task_graph_dyn::this_ref_t task_graph_dyn::in_between( natus::concurrent::itask_ptr_t tptr )
{
    _begin->then(tptr)->then(_end) ;
    return *this ;
}

//*************************************************************************************
task_graph_dyn::this_ref_t task_graph_dyn::in_between( this_rref_t rhv )
{
    this_t tmp( ::std::move(rhv) ) ;
    return this_t::in_between( tmp ) ;
}

//*************************************************************************************
task_graph_dyn::this_ref_t task_graph_dyn::in_betwenn( task_graph_rref_t other ) 
{    
    return this_t::in_between( this_t( ::std::move(other) ) ) ;
}

//*************************************************************************************
task_graph_dyn::this_ref_t task_graph_dyn::in_between( this_ref_t other )
{
    _begin->then( other.begin() )->then( other.end() )->then( _end ) ;
    return *this ;
}

//*************************************************************************************
void_t task_graph_dyn::ensure_not_nullptr_and_connect( void_t )
{
    natus::log::global::error_and_exit( natus::core::is_nullptr( _begin ),
        "[natus::concurrent::task_graph_dyn::ensure_not_nullptr_and_connect] : nullptr not allowed" ) ;

    if(natus::core::is_nullptr( _end ) || (_begin == _end))
    {
        _end = natus::concurrent::void_funk_task_t::create_noop(
            "[natus::concurrent::task_graph_dyn::ensure_not_nullptr_and_connect] : end task" ) ;
    }

    // security connection
    _begin->then( _end ) ;
}

//*************************************************************************************
