#pragma once

#include "typedefs.h"

#include <natus/concurrent/mrsw.hpp>

namespace natus
{
    namespace nsl
    {
        class database
        {
            natus_this_typedefs( database ) ;

        private:

            mutable natus::concurrent::mrsw_t _ac ;

            natus::nsl::post_parse::configs_t _configs ;

            natus::nsl::post_parse::library_t::variables_t _vars ;
            natus::nsl::post_parse::library_t::fragments_t _fragments ;
            
            

            #if 0
            template< typename T >
            struct data
            {
                natus::ntd::vector< natus::nsl::symbol_t > deps ;
                T data ;
            };
            natus_typedefs( data< natus::nsl::pregen::config_t >, config_data ) ;
            natus_typedefs( data< natus::nsl::pregen::library_t >, lib_data ) ;

            natus::ntd::vector< config_data_t > _configs ;
            natus::ntd::vector< config_data_t > _libs ;
            #endif

        public:

            database( void_t ) noexcept
            {
            }


            database( this_cref_t ) = delete ;

            database( this_rref_t rhv ) noexcept
            {
                *this = std::move( rhv ) ;
            }

            ~database( void_t ) noexcept
            {
            }

            this_ref_t operator = ( this_cref_t ) = delete ;
            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _vars = std::move( rhv._vars ) ;
                _fragments = std::move( rhv._fragments ) ;
                _configs = std::move( rhv._configs ) ;
                return *this ;
            }

        public:


            void_t insert( natus::nsl::post_parse::document_rref_t doc ) noexcept
            {
                natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;

                for( auto const & c : doc.configs )
                {
                    auto iter = std::find_if( _configs.begin(), _configs.end(), 
                        [&] ( natus::nsl::post_parse::config_cref_t c1 ) { return c1.name == c.name ; } ) ;
                    if( iter != _configs.end() ) continue ;

                    _configs.emplace_back( c ) ;
                }

                for( auto const & l : doc.libraries )
                {
                    for( auto const & f : l.fragments )
                    {
                        auto iter = std::find( _fragments.begin(), _fragments.end(), f ) ;
                        if( iter != _fragments.end() ) continue ;
                        _fragments.emplace_back( f ) ;
                    }

                    for( auto const & v : l.variables )
                    {
                        auto iter = std::find( _vars.begin(), _vars.end(), v ) ;
                        if( iter != _vars.end() ) continue ;
                        _vars.emplace_back( v ) ;
                    }
                }
            }

        public:

            

        public:

            bool_t find_variable( natus::nsl::symbol_cref_t sym, natus::nsl::post_parse::library_t::variable_out_t res ) const 
            {
                natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;
                for( auto const& v : _vars ) if( v.sym_long == sym ) { res = v ; return true ;  }
                return false ;
            }

            bool_t find_fragments( natus::nsl::symbol_cref_t sym, natus::nsl::post_parse::library_t::fragments_out_t frags ) const
            {
                natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;

                for( auto const& f : _fragments ) if( f.sym_long == sym ) frags.emplace_back( f ) ;

                return frags.size() != 0 ;
            }

            bool_t find_config( natus::nsl::symbol_cref_t sym, natus::nsl::post_parse::config_out_t cfg ) const
            {
                natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;
                for( auto const & c : _configs )
                {
                    if( sym == c.name ) { cfg = c; return true ; }
                }
                return false ;
            }

        };
        natus_res_typedef( database ) ;
    }
}