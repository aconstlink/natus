#include "database.h"

#include <natus/std/filesystem.hpp>

using namespace natus::io ;

//***
database::database( void_t ) 
{
}

//***
database::database( natus::io::path_cref_t base )
{
    this_t::init( base ) ;
}

//***
database::database( this_rref_t rhv )
{
}

//***
database::~database( void_t )
{
}

//***
bool_t database::init( natus::io::path_cref_t base )
{
    // look for db file
    {
        natus::io::path_t const loc = base / natus::io::path_t( "db.natus" ) ;
        auto const res = natus::std::filesystem::exists( loc ) ;
        
        if( res )
        {
            // file exists. so load it.
        }
    }

    // look for db file system
    if( natus::std::filesystem::exists( base ) )
    {
        for( auto& i : natus::std::filesystem::recursive_directory_iterator( base ) )
        {
            //natus::log::global_t::status( i.path() ) ;

            // only check for files.
            if( i.is_regular_file() )
            {
                natus::log::global_t::status( "File :" ) ;
                // the complete path
                natus::log::global_t::status( i.path() ) ;
                
                // the file names stem
                natus::log::global_t::status( i.path().stem() ) ;
                // the files' extension
                natus::log::global_t::status( i.path().extension() ) ;

                // the relative path to the base path
                auto const p = natus::std::filesystem::relative( i.path(), base ) ;
                natus::log::global_t::status( p ) ;

                natus::std::string_t loc ;
                for( auto j : p.parent_path() )
                {
                    loc += j ;
                    loc += "." ;
                }
                loc += i.path().stem().string() ;
                natus::log::global_t::status( loc ) ;
                natus::log::global_t::status( "********************************" ) ;
            }
        }
    }
    else
    {
        natus::log::global_t::warning( "Path does not exist : " + base.string() ) ;
    }

    // spawn monitor thread for file system changes
    {
    }

    return true ;
}

//***
bool_t database::pack( this_t::encryption const )
{
    return true ;
}

//***
bool_t database::unpack( void_t )
{
    return true ;
}

//***
bool_t database::store( natus::std::string_cref_t location /*, binary data*/ )
{
    return true ;
}

//***