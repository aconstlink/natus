
#include "itask.h"
#include "async_executor.h"

#include "../semaphore.hpp"

using namespace natus::concurrent ;

//*************************************************************************************
void_t async_executor::consume_and_wait( natus::concurrent::itask_ptr_t root_ptr ) 
{    
    if( natus::core::is_nullptr(root_ptr) ) 
        return ;

    natus::concurrent::itask::locked_tasks_t following_tasks ;
    following_tasks.push_back( root_ptr ) ;

    natus::ntd::vector< ::std::future<void_t> > futures ;

    while( following_tasks.size() != 0 )
    {
        natus::concurrent::itask::tasks_t cur_tasks = following_tasks.move_out() ;

        natus::concurrent::semaphore_t sem( cur_tasks.size() ) ;
        
        for( auto * task_ptr : cur_tasks )
        {
            //@todo we can give a hint how the task should be executed
            // async or deferred
            futures.push_back( ::std::async( ::std::launch::async, [task_ptr,&following_tasks,&sem]( void_t )
            {
                task_ptr->execute( following_tasks ) ;
                task_ptr->destroy() ;

                sem.decrement() ;
            } ) ) ;            
        }
        sem.wait() ;
    }
}

//*************************************************************************************
void_t async_executor::consume_and_wait( tasks_rref_t tasks ) 
{
    natus::concurrent::semaphore_t sem( tasks.size() ) ;
    natus::ntd::vector< ::std::future<void_t> > futures ;

    for( auto * task_ptr : tasks )
    {
        auto fut = ::std::async( ::std::launch::async, [task_ptr, this, &sem]( void_t )
        {
            this->consume_and_wait( task_ptr ) ;
            sem.decrement() ;
        } ) ;

        futures.push_back( ::std::move(fut) ) ;
    }

    sem.wait() ;
}
