
#pragma once

#include "../api.h"
#include "../typedefs.h"

#include <natus/ntd/vector.hpp>

#include <atomic>

namespace natus
{
    namespace concurrent
    {
        class task
        {
            natus_this_typedefs( task ) ;
            typedef natus::memory::res< task > this_res_t ;

            friend class task ;

        public:

            typedef std::function< void_t ( this_res_t ) > task_funk_t ;

        private: // user execution funktion

            task_funk_t _funk ;

        private: // refs

            natus::concurrent::mutex_t _incomings_mtx ;
            size_t _incomings = 0 ;

            bool_t dec_incoming( void_t ) noexcept 
            {
                natus::concurrent::lock_guard_t lk( _incomings_mtx ) ;
                if( --_incomings == size_t(-1) ) _incomings = 0 ;
                return _incomings == 0 ;
            }

            void_t inc_incoming( void_t ) noexcept
            {
                natus::concurrent::lock_guard_t lk( _incomings_mtx ) ;
                ++_incomings ;
            }

        private: //

            natus::ntd::vector< this_res_t > _inbetweens ;
            natus::ntd::vector< this_res_t > _thens ;

        public:

            task( void_t ) noexcept
            {
                _funk = [=]( this_res_t ){} ;
            }

            task( task_funk_t f ) noexcept
            {
                _funk = f ;
            }

            task( this_cref_t ) = delete ;
            task( this_rref_t rhv ) noexcept
            {
                _funk = std::move( rhv._funk ) ;
                _inbetweens = std::move( rhv._inbetweens ) ;
                _thens = std::move( rhv._thens ) ;
                _incomings = rhv._incomings ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _funk = std::move( rhv._funk ) ;
                _inbetweens = std::move( rhv._inbetweens ) ;
                _thens = std::move( rhv._thens ) ;
                _incomings = rhv._incomings ;
                return *this ;
            }

        public: // this is what the user should use only

            this_res_t in_between( this_res_t other ) noexcept
            {
                other->inc_incoming() ;
                _inbetweens.emplace_back( other ) ;
                return other ;
            }

            this_res_t then( this_res_t other ) noexcept
            {
                other->inc_incoming() ;
                _thens.emplace_back( other ) ;
                return other ;
            }

        public:

            struct scheduler_accessor
            {
                static void_t schedule( this_res_t t, natus::ntd::vector< this_res_t > & tasks ) noexcept
                {
                    t->schedule( tasks ) ;
                }

                static bool_t will_execute( this_res_t t ) noexcept
                {
                    return t->will_execute() ;
                }

                static bool_t execute( this_res_t t ) noexcept 
                {
                    return t->execute( t ) ;
                }
            };
            friend struct scheduler_accessor ;

        private:

            void_t schedule( natus::ntd::vector< this_res_t > & tasks ) noexcept
            {
                tasks.reserve( tasks.capacity() + _inbetweens.size() + _thens.size() ) ;

                if( _inbetweens.size() == 0 )
                {
                    for( auto & t : _thens ) tasks.emplace_back( t ) ;
                    return ;
                }

                for( auto & c : _inbetweens )
                {
                    for( auto & t : _thens ) 
                    {
                        c->then( t ) ;
                    }
                    tasks.emplace_back( c ) ;
                }

                // t is not scheduled yet it is
                // delayed to the children
                for( auto & t : _thens ) 
                {
                    t->dec_incoming() ; 
                }
            }

            bool_t will_execute( void_t ) noexcept
            {
                return this_t::dec_incoming() ;
            }

            // the schedule will pass the resource handle
            // of this task to this function so the
            // executing function could add futher tasks
            bool_t execute( this_res_t this_res ) noexcept 
            {
                if( _incomings != 0 ) return false ;
                
                 _funk( this_res ) ;
                 return true ;
            }
        };
        natus_res_typedef( task ) ;
    }
}