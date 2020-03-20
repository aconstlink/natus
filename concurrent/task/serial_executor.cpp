
#include "serial_executor.h"

using namespace natus::concurrent ;

//*************************************************************************************
void_t serial_executor::consume( itask_ptr_t root_ptr ) 
{
    if(natus::core::is_nullptr( root_ptr ))
        return ;

    natus::concurrent::itask::locked_tasks_t following_tasks ;
    following_tasks.push_back( root_ptr ) ;

    while( following_tasks.size() != 0 )
    {
        natus::concurrent::itask::tasks_t cur_tasks = following_tasks.move_out() ;
        for( auto * task_ptr : cur_tasks )
        {
            task_ptr->execute( following_tasks ) ;
            task_ptr->destroy() ;
        }
    }
}

//*************************************************************************************
void_t serial_executor::consume( tasks_rref_t tasks ) 
{
    for( auto * task_ptr : tasks )
    {
        this_t::consume( task_ptr ) ;
    }
}
