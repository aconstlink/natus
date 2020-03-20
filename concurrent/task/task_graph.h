#pragma once

#include "itask.h"

#include <natus/std/map.hpp>
#include <natus/std/string.hpp>

#include <natus/core/macros/move.h>

namespace natus
{
    namespace concurrent
    {
        class NATUS_CONCURRENT_API task_graph
        {
            natus_this_typedefs( task_graph ) ;

        private:

            natus::concurrent::itask_ptr_t _begin = nullptr ;
            natus::concurrent::itask_ptr_t _end = nullptr ;

        private:

            task_graph( this_cref_t rhv ) = delete ;
            this_ref_t operator = ( this_cref_t ) = delete ;

        public:

            task_graph( void_t ) ;

            task_graph( natus::concurrent::itask_ptr_t begin, natus::concurrent::itask_ptr_t end ) ;

            task_graph( natus::concurrent::itask_ptr_t single ) ;

            task_graph( this_rref_t rhv ) ;

            ~task_graph( void_t ) ;

        public:

            natus::concurrent::itask_ptr_t begin_moved( void_t ) ;
            natus::concurrent::itask_ptr_t end_moved( void_t ) ;
            static void_t dont_destroy( this_rref_t ) ;

        public:

            bool_t operator == ( this_ref_t rhv ) const ;


            this_ref_t operator = ( this_rref_t ) = delete ;

        public:

            this_ref_t then( natus::concurrent::itask_ptr_t ptr ) ;

            this_ref_t then( this_rref_t ) ;

            // with this function, the pair will change to a connected pair.
            this_ref_t in_between( natus::concurrent::itask_ptr_t tptr ) ;

            // with this function, the pair will change to a connected pair.
            this_ref_t in_between( this_rref_t rhv ) ;

        private:

            void_t ensure_not_nullptr_and_connect( void_t ) ;

        };
        natus_typedef( task_graph ) ;


        /// This task graph is designed to be used with dynamic tasks.
        /// A dynamic task is a task that is already in its execution.
        /// The normal task_graph creates begin and end tasks and it can only
        /// be used once. A dynamic task graph does not destroy its tasks
        /// because those are supposed to be user-supplied. 
        /// @note
        /// If no end task is passed, the dyn task creates a new end task and
        /// connects that new end task to the provided begin task.
        class NATUS_CONCURRENT_API task_graph_dyn
        {
            natus_this_typedefs( task_graph_dyn ) ;

        private:

            natus::concurrent::itask_ptr_t _begin = nullptr ;
            natus::concurrent::itask_ptr_t _end = nullptr ;

        public:

            task_graph_dyn( natus::concurrent::itask_ptr_t begin, natus::concurrent::itask_ptr_t end ) ;

            task_graph_dyn( natus::concurrent::itask_ptr_t single ) ;

            task_graph_dyn( this_rref_t rhv ) ;
            task_graph_dyn( task_graph_rref_t ) ;

            task_graph_dyn( this_cref_t rhv ) ;

            ~task_graph_dyn( void_t ) ;

        public:

            natus::concurrent::itask_ptr_t begin( void_t ) ;
            natus::concurrent::itask_ptr_t end( void_t ) ;

        public:

            bool_t operator == ( this_cref_t rhv ) const ;

            this_ref_t operator = ( this_cref_t ) = delete ;
            this_ref_t operator = ( this_rref_t ) = delete ;

        public:

            this_ref_t then( natus::concurrent::itask_ptr_t ptr ) ;

            this_ref_t then( this_rref_t ) ;

            // with this function, the pair will change to a connected pair.
            this_ref_t in_between( natus::concurrent::itask_ptr_t tptr ) ;

            // with this function, the pair will change to a connected pair.
            this_ref_t in_between( this_rref_t rhv ) ;
            this_ref_t in_between( this_ref_t rhv ) ;
            this_ref_t in_betwenn( task_graph_rref_t ) ;

        private:

            void_t ensure_not_nullptr_and_connect( void_t ) ;

        };
        natus_typedef( task_graph_dyn ) ;

        namespace detail
        {
            typedef natus::std::map< natus::std::string, task_graph_t > __task_graph_map_t ;
        }
        natus_typedefs( detail::__task_graph_map_t, task_graph_map ) ;
    }
}