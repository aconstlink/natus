#pragma once

#include "typedefs.h"
#include "parser_structs.hpp"

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

            database( this_rref_t ) noexcept
            {
            }

            ~database( void_t ) noexcept
            {
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

        class dependency_resolver
        {
            natus_this_typedefs( dependency_resolver ) ;

            natus::nsl::database_res_t _db ;

        public:

            dependency_resolver( database_res_t db ) : _db( db ) {}

        public:

            struct resolver_result
            {
                natus::nsl::post_parse::library_t::variables_t vars ;
                natus::nsl::post_parse::library_t::fragments_t frags ;
                natus::nsl::post_parse::config_t config ;
            };

        public:

            resolver_result resolve( natus::nsl::symbol_cref_t sym ) noexcept
            {
                resolver_result res ;
                natus::nsl::symbols_t syms( { sym } ) ;
                
                {
                    natus::nsl::symbols_t tmp = std::move( syms ) ;

                    for( auto const& s : tmp )
                    {
                        natus::nsl::post_parse::config_t c ;
                        if( _db->find_config( s, c ) )
                        {
                            res.config = c ;
                            for( auto const s : c.shaders )
                            {
                                for( auto const& d : s.deps )
                                {
                                    syms.emplace_back( d ) ;
                                }
                            }
                        }
                    }
                }

                while( syms.size() != 0 )
                {
                    natus::nsl::symbols_t tmp = std::move( syms ) ;

                    for( auto const& s : tmp )
                    {
                        // check variable first
                        {
                            natus::nsl::post_parse::library_t::variable_t var ;
                            if( _db->find_variable( s, var ) )
                            {
                                auto const iter = std::find( res.vars.begin(), res.vars.end(), var ) ;
                                if( iter == res.vars.end() ) res.vars.emplace_back( var ) ;
                                continue ;
                            }
                        }

                        {
                            natus::nsl::post_parse::library_t::fragments_t frgs ;
                            if( !_db->find_fragments( s, frgs ) ) continue ;

                            for( auto const& frg : frgs )
                            {
                                auto iter = std::find( res.frags.begin(), res.frags.end(), frg ) ;
                                if( iter != res.frags.end() ) continue ;

                                res.frags.emplace_back( frg ) ;

                                for( auto const& s : frg.deps )
                                {
                                    syms.emplace_back( s ) ;
                                }
                            }
                        }
                    }
                }

                return std::move( res ) ;
            }
        };
        natus_typedef( dependency_resolver ) ;
    }
}