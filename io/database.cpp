#include "database.h"
#include "global.h"

#include <natus/std/filesystem.hpp>
#include <natus/concurrent/mutex.hpp>

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
    this_t::spawn_monitor() ;
}

//***
database::database( this_rref_t rhv )
{
    this_t::spawn_monitor() ;
    _db = ::std::move( rhv._db ) ;
}

//***
database::~database( void_t )
{
    this_t::join_monitor() ;
}

//***
bool_t database::init( natus::io::path_cref_t base, natus::io::path_cref_t name, natus::io::path_cref_t working )
{
    this_t::db_t db ;

    // this is where the db is located
    db.base = base ;
    // this is where the data stored should be located
    db.working = db.base / working ;
    // this is the db name
    db.name = name ;

    // look for db file
    {
        natus::io::path_t const loc = base / natus::io::path_t(name).replace_extension( natus::io::path_t( ".natus" ) ) ;
        auto const res = natus::std::filesystem::exists( loc ) ;
        
        if( res )
        {
            this_t::load_db_file( loc ) ;
        }
    }

    // look for db file system
    if( natus::std::filesystem::exists( base ) )
    {
        for( auto& i : natus::std::filesystem::recursive_directory_iterator( base ) )
        {
            // do not go into .xyz directories
            if( i.is_directory() && i.path().stem().string().find_first_of(".",0) == 0 )
            {
                continue ;
            }

            // files
            else if( i.is_regular_file() )
            {
                // do not track self
                if( i.path().stem() == name ) continue ;

                auto const fr = this_t::create_file_record( base, i.path() ) ;
                
                // check other files' existence
                // file names must be unique!
                {
                    this_t::file_record_t fr2 ;
                    if( this_t::lookup( db, fr.location, fr2 ) )
                    {
                        ::std::stringstream ss ;
                        ss 
                            << "[" << name << ".natus] : "
                            << "Only unique file names supported. See [" << fr.location << "] with extensions "
                            << "[" << fr.extension << ", " << fr2.extension << "] " 
                            << "where [" << fr2.extension << "] already stored" ;

                        natus::log::global_t::error( ss.str() ) ;
                        continue ;
                    }
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
        _monitor_thread = natus::concurrent::thread_t([=](){}) ;
    }

    {
        natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;
        _db = ::std::move( db ) ;
    }

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
        natus::io::path_t path ;
        bool_t external ;

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
        fr__.path = _db.base / fr.rel ;
        fr__.external = fr.offset == uint64_t( -2 ) ;

        first_data += fr__.size() ;

        offset += fr.sib ;

        records.push_back( fr__ ) ;
    }


    natus::io::path_t db_new = _db.base / natus::io::path_t( _db.name ).replace_extension( ".tmp.natus" ) ;
    natus::io::path_t db_old = _db.base / natus::io::path_t( _db.name ).replace_extension( ".natus" ) ;
    
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

        // for testing purposes, do not write content
        #if 0
        // file content 
        {
            for( auto & fr : records )
            {
                if( fr.external )
                {
                    this_t::load( fr.location ).wait_for_operation( 
                        [&]( char_cptr_t data, size_t const sib, natus::io::result const res )
                    {
                        if( res == natus::io::result::ok  )
                        {
                            outfile.seekp( first_data + fr.offset ) ;
                            outfile.write( data, sib ) ;
                        }
                    } ) ;
                }
                else
                {
                    // read from db file and store in tmp file
                }
                

                outfile.flush() ;
            }
        }

        outfile.flush() ;
        #endif
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
void_t database::load_db_file( natus::io::path_cref_t p ) 
{
    ::std::ifstream ifs( p, ::std::ifstream::binary ) ;

    
    {
        natus::std::string_t buffer ;
        ::std::getline( ifs, buffer ) ;

        uint64_t const offset = ::std::stol( buffer ) ;
        int const bp = 0 ;
    }
}

//***
bool_t database::unpack( void_t )
{
    return true ;
}

//***
natus::io::store_handle_t database::store( natus::std::string_cref_t /*location*/, char_cptr_t, size_t const )
{
    natus::io::store_handle_t h ;


    return ::std::move( h ) ;
}

//***
natus::io::load_handle_t database::load( natus::std::string_cref_t loc ) const
{
    this_t::file_record_t fr ;
    if( natus::core::is_not( this_t::lookup( loc, fr ) ) )
    {
        natus::log::global_t::warning( "resource location not found : " + loc ) ;
        return natus::io::load_handle_t() ;
    }
    
    natus::io::load_handle_t lh ;
    if( fr.offset == uint64_t(-2) )
    {
        lh = natus::io::global_t::load( _db.base / fr.rel ) ; ;
    }
    else if( fr.offset != uint64_t(-1) )
    {

    }

    return ::std::move( lh ) ;
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

//***
void_t database::attach( natus::std::string_cref_t loc, natus::io::monitor_res_t mon ) noexcept 
{
    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;
    
    for( auto & fr : _db.records )
    {
        if( fr.location == loc )
        {
            fr.monitors.emplace_back( mon ) ;
        }
    }
}

//***
void_t database::detach( natus::std::string_cref_t loc, natus::io::monitor_res_t mon ) noexcept 
{
    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;

    this_t::file_record_t fr ;
    if( this_t::lookup( _db, loc, fr ) )
    {
        auto iter = ::std::remove_if( fr.monitors.begin(), fr.monitors.end(),
            [&] ( natus::io::monitor_res_t const& r )
        {
            return r.get_sptr() == mon.get_sptr() ;
        } ) ;
        if( iter != fr.monitors.end() ) fr.monitors.erase( iter ) ;
    }
}

//***
void_t database::attach( natus::io::monitor_res_t moni ) noexcept 
{
    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;
    auto iter = ::std::find_if( _db.monitors.begin(), _db.monitors.end(), [&]( natus::io::monitor_res_t const & m )
    {
        return m.get_sptr() == moni.get_sptr() ;
    } ) ;

    if( iter != _db.monitors.end() ) return ;
        
    _db.monitors.emplace_back( moni ) ;
}

//***
void_t database::detach( natus::io::monitor_res_t moni ) noexcept 
{
    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;

    // remove from global monitors
    {
        auto iter = ::std::remove_if( _db.monitors.begin(), _db.monitors.end(),
            [&] ( natus::io::monitor_res_t const& r )
        {
            return r.get_sptr() == moni.get_sptr() ;
        } ) ;

        if( iter != _db.monitors.end() ) _db.monitors.erase( iter ) ;
    }
    
    // remove from local monitors
    {
        for( auto& fr : _db.records )
        {
            auto iter = ::std::remove_if( fr.monitors.begin(), fr.monitors.end(),
                [&] ( natus::io::monitor_res_t const& r )
            {
                return r.get_sptr() == moni.get_sptr() ;
            } ) ;

            if( iter != fr.monitors.end() ) fr.monitors.erase( iter ) ;
        }
    }
}

//***
database::file_record_t database::create_file_record( natus::io::path_cref_t base, natus::io::path_cref_t path ) const noexcept
{
    this_t::file_record_t fr ;
 
    // the files' extension
    {
        fr.extension = path.extension() ;
    }

    // determine file locator
    {
        // the relative path to the base path
        auto const p = natus::std::filesystem::relative( path, base ) ;

        natus::std::string_t loc ;
        for( auto j : p.parent_path() )
        {
            loc += j ;
            loc += "." ;
        }
        loc += path.stem().string() ;

        fr.location = loc ;
        fr.rel = p ;
    }

    {
        // external
        fr.offset = uint64_t( -2 ) ;

        {
            ::std::error_code ec ;
            fr.sib = natus::std::filesystem::file_size( path, ec ) ;
            natus::log::global_t::error( ( bool_t ) ec,
                natus_log_fn( "file_size with [" + ec.message() + "]" ) ) ;
        }
    }

    // store the last write time so 
    // monitoring can  take palce.
    {
        fr.stamp = natus::std::filesystem::last_write_time( path ) ;
    }

    return ::std::move( fr ) ;
}

//***
bool_t database::lookup( natus::std::string_cref_t loc, this_t::file_record_out_t fro ) const noexcept
{
    natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;
    return this_t::lookup( _db, loc, fro ) ;
}

//***
bool_t database::lookup( this_t::db const & db_, natus::std::string_cref_t loc, file_record_out_t fro ) const noexcept
{
    for( auto& fr : db_.records )
    {
        if( fr.location == loc )
        {
            fro = fr ;
            return true ;
        }
    }

    return false ;
}

//***
void_t database::file_change_stamp( this_t::file_record_cref_t fri ) noexcept 
{
    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;
    auto iter = ::std::find_if( _db.records.begin(), _db.records.end(), [&]( this_t::file_record_cref_t fr )
    {
        return fr.location == fri.location ;
    } ) ;

    if( iter == _db.records.end() ) return ;
    iter->stamp = fri.stamp ;
}

//***
void_t database::file_remove( this_t::file_record_cref_t fri ) noexcept 
{
    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;

    auto iter = ::std::remove_if( _db.records.begin(), _db.records.end(), [&] ( this_t::file_record_cref_t fr )
    {
        return fr.location == fri.location ;
    } ) ;
    if( iter != _db.records.end() ) _db.records.erase( iter ) ;
}

//***
void_t database::spawn_monitor( void_t ) noexcept
{
    this_t::join_monitor() ;

    _monitor_thread = natus::concurrent::thread_t( [&] ( void_t )
    { 
        size_t const ms = 500 ;

        natus::log::global_t::status("[db] : monitor thread going up @ " + ::std::to_string(ms) + " ms") ;

        while( !_isleep.sleep_for( ::std::chrono::milliseconds(ms) ) )
        {
            this_t::db_t db ;
            {
                natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;
                db = _db ;
            }

            for( auto & fr : db.records )
            {
                natus::io::path_t const p = db.base / fr.rel ;
                if( !natus::std::filesystem::exists( p ) )
                {
                    for( auto & r : db.monitors ) 
                    {
                        r->trigger_changed( fr.location, natus::io::monitor_t::notify::deletion ) ;
                    }

                    for( auto & r : fr.monitors )
                    {
                        r->trigger_changed( fr.location, natus::io::monitor_t::notify::deletion ) ;
                    }

                    this_t::file_remove( fr ) ;
                    continue ;
                }
                
                auto const tp = natus::std::filesystem::last_write_time( p ) ;
                if( fr.stamp != tp )
                {
                    for( auto& r : db.monitors )
                    {
                        r->trigger_changed( fr.location, natus::io::monitor_t::notify::change ) ;
                    }

                    for( auto& r : fr.monitors )
                    {
                        r->trigger_changed( fr.location, natus::io::monitor_t::notify::change ) ;
                    }

                    fr.stamp = tp ;

                    this_t::file_change_stamp( fr ) ;
                    continue ;
                }
            }
        }

        natus::log::global_t::status("[db] : monitor thread shutting down") ;
    } ) ;
}

//***
void_t database::join_monitor( void_t ) noexcept 
{
    if( _monitor_thread.joinable() ) 
    { 
        _isleep.interrupt() ; 
        _monitor_thread.join() ; 
        _isleep.reset() ;
    }
}

