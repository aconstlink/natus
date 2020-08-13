#include "database.h"

#include <natus/std/filesystem.hpp>

#include <sstream>
#include <iomanip>

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
    this_t::db_t db ;

    // this is where the db is located
    db.base = base ;

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
                // do not track self
                if( i.path().stem() == "db" ) continue ;

                // the complete path
                // i.path()

                this_t::file_record_t fr ;
                
                // the files' extension
                {
                    fr.extension = i.path().extension() ;
                }
                
                // determine file locator
                {
                    // the relative path to the base path
                    auto const p = natus::std::filesystem::relative( i.path(), base ) ;

                    natus::std::string_t loc ;
                    for( auto j : p.parent_path() )
                    {
                        loc += j ;
                        loc += "." ;
                    }
                    loc += i.path().stem().string() ;

                    fr.location = loc ;
                    fr.rel = p ;
                }

                {
                    // external
                    fr.offset = uint64_t( -2 ) ;

                    {
                        ::std::error_code ec ;
                        fr.sib = i.file_size( ec ) ;
                        natus::log::global_t::error( (bool_t)ec, 
                            natus_log_fn( "file_size with [" + ec.message() + "]" ) ) ;
                    }
                }

                // store the last write time so 
                // monitoring can  take palce.
                {
                    fr.stamp = i.last_write_time() ;
                }

                db.records.emplace_back( fr ) ;
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

    _db = ::std::move( db ) ;

    return true ;
}

//***
bool_t database::pack( this_t::encryption const )
{
    struct __file_record
    {
        natus::std::string_t location ;
        natus::std::string_t extension ;
        uint64_t offset ;
        uint64_t sib ;

        natus::std::string_t to_string( void_t ) const 
        {
            ::std::stringstream ss ;
            ss  << location << ":" << extension << ":"
                << ::std::to_string( offset ) << ":" << ::std::to_string( sib ) << "\n" ;
            return ss.str() ;
        }

        size_t size( void_t ) const 
        {
            return to_string().size() ;
        }
    };

    natus::std::vector< __file_record > records ;
    
    uint64_t first_data = 0 ;
    uint64_t offset = 0 ;
    for( auto& fr : _db.records )
    {
        __file_record fr__ ;
        fr__.location = fr.location ;
        fr__.extension = fr.extension ;
        fr__.offset = offset ;
        fr__.sib = fr.sib ;

        first_data += fr__.size() ;

        offset += fr.sib ;

        records.push_back( fr__ ) ;
    }


    natus::io::path_t db_new = _db.base / "db.tmp.natus" ;
    natus::io::path_t db_old = _db.base / "db.natus" ;
    
    // write file
    {
        ::std::ofstream outfile ( db_new, ::std::ofstream::binary ) ;

        // header info
        {
            natus::std::string_t s = ::std::to_string( first_data ) ;
            first_data += s.size() + 1 ;
            s = ::std::to_string( first_data ) ;
            outfile << s << "\n" ;
        }
        
        // records
        {
            for( auto& fr : records )
            {
                outfile << fr.to_string() ;
            }
        }

        // file content 
        {
            // for each file record, load data and write it to the db.
        }

        outfile.flush() ;
        outfile.close() ;
    }

    if( natus::std::filesystem::exists( db_old ) )
    {
        natus::std::filesystem::remove( db_old ) ;
    }
    natus::std::filesystem::rename( db_new, db_old ) ;
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
void_t database::dump_to_std( void_t ) const noexcept 
{
    natus::log::global_t::status( "Printing Database" ) ;
    natus::log::global_t::status( "***************************************************" ) ;
    
    // db infos
    {
        natus::log::global_t::status( "Database Infos" ) ;
        natus::log::global_t::status( "**************" ) ;
        natus::log::global_t::status( "@" + _db.base.string() ) ;
    }

    // file records
    {
        natus::log::global_t::status( "File Records" ) ;
        natus::log::global_t::status( "**************" ) ;
        for( auto& fr : _db.records )
        {
            ::std::stringstream ss ;
            
            ss << "[" << ::std::to_string(fr.sib) << " @ " 
                << (fr.offset != uint64_t(-2) ? ::std::to_string(fr.offset) : "extern") 
                << "] "
                << fr.location + fr.extension ;

            natus::log::global_t::status( ss.str() ) ;
        }
    }

    natus::log::global_t::status( "***************************************************" ) ;
}