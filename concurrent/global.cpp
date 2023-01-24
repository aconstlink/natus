
#include "global.h"

#include "scheduler.h"
#include "thread_pool.hpp"
#include "task/task.hpp"

#include <natus/memory/arena.hpp>
#include <natus/log/global.h>

using namespace natus::concurrent ;

mutex_t global::_mtx ;
global::singleton_data * global::_dptr = nullptr ;

//*************************************************************************************************
struct global::singleton_data
{
    natus::concurrent::thread_pool_t tp ;
    natus::concurrent::loose_thread_scheduler_t lts ;
    natus::memory::arena< natus::concurrent::task_t > arena = 
        natus::memory::arena< natus::concurrent::task_t >(50000) ;

    singleton_data( void_t ) noexcept{}

    singleton_data( singleton_data && rhv ) noexcept
    {
        tp = std::move( rhv.tp ) ;
        lts = std::move( rhv.lts ) ;
        arena = std::move( rhv.arena ) ;
    }

    ~singleton_data( void_t ) noexcept
    {}
};

//*************************************************************************************************
global::singleton_data * global::init( void_t ) noexcept
{
    natus::concurrent::lock_guard_t lk( this_t::_mtx ) ;
    if( this_t::_dptr != nullptr ) return this_t::_dptr ;

    _dptr = natus::memory::global_t::alloc( this_t::singleton_data(),
        "[natus::concurrent::global::init] : global singleton lazy initialization" ) ;
    _dptr->tp.init() ;
    _dptr->lts.init() ;

    natus::log::global_t::status( "[online] : natus concurrent" ) ;

    return this_t::_dptr ;
}

//*************************************************************************************************
void_t global::deinit( void_t )
{
    natus::concurrent::lock_guard_t lk( this_t::_mtx ) ;
    if( this_t::_dptr == nullptr ) return ;

    natus::memory::global_t::dealloc( _dptr ) ;
}

//*************************************************************************************************
void_t global::update( void_t )
{
    this_t::init()->lts.update() ;
}

//*************************************************************************************************
void_t global::yield( std::function< bool_t ( void_t ) > funk ) noexcept 
{
    if( !_dptr->tp.yield( funk ) ) _dptr->lts.yield( funk ) ;
}

//*************************************************************************************************
void_t global::schedule( natus::concurrent::task_res_t t, natus::concurrent::schedule_type const st ) noexcept 
{
    if( st == natus::concurrent::schedule_type::pool )
    {
        this_t::init()->tp.schedule( t ) ;
    }
    else if( st == natus::concurrent::schedule_type::loose )
    {
        this_t::init()->lts.schedule( t ) ;
    }
}

//*************************************************************************************************
task_res_t global::make_task( natus::concurrent::task_t::task_funk_t f ) noexcept
{
    return task_res_t( this_t::init()->arena.alloc( natus::concurrent::task_t(f) ),
        [&]( natus::concurrent::task_ptr_t ptr ){ this_t::init()->arena.dealloc(ptr) ; } ) ;
}