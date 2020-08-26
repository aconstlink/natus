#include "database.h"
#include "global.h"
#include "obfuscator.hpp"

#include <natus/ntd/filesystem.hpp>
#include <natus/ntd/string.hpp>
#include <natus/ntd/string/split.hpp>
#include <natus/concurrent/mutex.hpp>
#include <natus/memory/guards/malloc_guard.hpp>

#include <sstream>
#include <streambuf>
#include <iomanip>
#include <array>
#include <fstream>

using namespace natus::io ;

static const natus::ntd::string_t header_desc = "natus db file format 0.1" ;

namespace this_file_db
{
    using namespace natus::core::types ;

    struct length_validator
    {
        static bool_t make_ending_string( natus::ntd::string_cref_t si, natus::ntd::string_out_t so ) noexcept
        {
            ::std::srand( uint_t(si.size()) ) ;

            if( si.size() >= length_validator::fixed_length() ) return false ;

            so = si ;
            so.resize( length_validator::fixed_length(), '-' ) ;
            for( size_t i=si.size(); i<so.size(); ++i )
            {
                so[ i ] = char_t( ::std::rand() ) ;
            }
            return true ;
        }

        static size_t fixed_length( void_t ) noexcept { return 1024 ; }
    };
}

//***
database::database( void_t ) 
{
}

//***
database::database( natus::io::path_cref_t base, natus::io::path_cref_t name,
    natus::io::path_cref_t working_rel )
{
    this_t::init( base, name, working_rel ) ;
    this_t::spawn_monitor() ;
}

//***
database::database( this_rref_t rhv )
{
    this_t::join_monitor() ;
    _db = ::std::move( rhv._db ) ;
    this_t::spawn_monitor() ;
}

//***
database::~database( void_t )
{
    this_t::join_monitor() ;
}

//***
bool_t database::init( natus::io::path_cref_t base, natus::io::path_cref_t working, natus::io::path_cref_t name )
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
        natus::io::path_t const loc = db.base / natus::io::path_t(name).replace_extension( natus::io::path_t( ".ndb" ) ) ;
        auto const res = natus::ntd::filesystem::exists( loc ) ;
        
        if( res )
        {
            this_t::load_db_file( db, loc ) ;
        }
    }

    // look for db file system
    if( natus::ntd::filesystem::exists( db.working ) )
    {
        for( auto& i : natus::ntd::filesystem::recursive_directory_iterator( db.working ) )
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
                if( i.path().stem() == name && i.path().extension() == ".ndb" ) continue ;

                auto const fr = this_t::create_file_record( db.working, i.path() ) ;
                
                // check other files' existence
                {
                    this_t::file_record_t fr2 ;
                    if( this_t::lookup( db, fr.location, fr2 ) )
                    {
                        // only unique data entries
                        if( fr2.offset == size_t(-2) )
                        {
                            ::std::stringstream ss ;
                            ss
                                << "[" << name << ".ndb] : "
                                << "Only unique file names supported. See [" << fr.location << "] with extensions "
                                << "[" << fr.extension << ", " << fr2.extension << "] "
                                << "where [" << fr2.extension << "] already stored" ;

                            natus::log::global_t::error( ss.str() ) ;
                        }

                        // file system data wins
                        else
                        {
                            this_t::file_change_external( db, fr ) ;
                        }
                        continue ;
                    }
                }

                db.records.emplace_back( fr ) ;
            }
        }
    }
    else
    {
        natus::log::global_t::warning( "Path does not exist : " + natus::io::path_t( base / working.string() ).string() ) ;
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
        natus::ntd::string_t location ;
        natus::ntd::string_t extension ;
        uint64_t offset ;
        uint64_t sib ;
        natus::io::path_t path ;
        bool_t external ;

        natus::ntd::string_t to_string( void_t ) const 
        {
            ::std::stringstream ss ;
            ss  << location << ":" << extension << ":"
                << ::std::to_string( offset ) << ":" << ::std::to_string( sib ) << ": " ;
            return ss.str() ;
        }

        size_t size( void_t ) const 
        {
            return to_string().size() ;
        }
    };

    natus::ntd::vector< __file_record > records ;
    
    // base offset to the first data written
    // (number of records + header ) * length
    uint64_t const offset = (_db.records.size() + 1) * this_file_db::length_validator::fixed_length() ;

    {
        uint64_t lo = offset ;

        for( auto& fr : _db.records )
        {
            __file_record fr__ ;
            fr__.location = fr.location ;
            fr__.extension = fr.extension ;
            fr__.offset = lo ;
            fr__.sib = fr.sib ;
            fr__.path = _db.base / fr.rel ;
            fr__.external = fr.offset == uint64_t( -2 ) ;

            lo += fr__.sib ;
            records.push_back( fr__ ) ;
        }
    }

    natus::io::path_t db_new = _db.base / natus::io::path_t( _db.name ).replace_extension( ".tmp.ndb" ) ;
    natus::io::path_t db_old = _db.base / natus::io::path_t( _db.name ).replace_extension( ".ndb" ) ;
    
    // write file
    {
        std::ofstream outfile ( db_new, std::ios::binary ) ;

        // header info
        {
            natus::ntd::string_t so ;
            natus::ntd::string_t const si = this_t::obfuscator().
                encode( header_desc + ":" + ::std::to_string( _db.records.size() ) + ": " ) ;

            auto const res = this_file_db::length_validator::make_ending_string( si, so ) ;
            if( res ) outfile << so ;
            natus::log::global_t::error( !res, "[db] : header does not fit into fixed length." ) ;
            
        }
        
        // records
        {
            natus::ntd::string_t so ;

            for( auto& fr : records )
            {
                natus::ntd::string_t const si = this_t::obfuscator().encode( fr.to_string() ) ;

                auto const res = this_file_db::length_validator::make_ending_string( si, so ) ;
                if( res ) outfile << so ;
                natus::log::global_t::warning( !res, "[db] : file record entry too long. Please reduce name length." ) ;
            }
        }

        // for testing purposes, do not write content
        //#if 0
        // file content 
        {
            for( auto & fr : records )
            {
                this_t::load( fr.location ).wait_for_operation(
                    [&] ( char_cptr_t data, size_t const sib, natus::io::result const res )
                {
                    if( res != natus::io::result::ok ) return ;

                    natus::ntd::string_t const wdata = this_t::obfuscator().encode( data, sib ) ;

                    outfile.seekp( fr.offset ) ;
                    outfile.write( wdata.c_str(), sib ) ;

                } ) ;
            }
            outfile.flush() ;
        }
        
        //#endif
        outfile.flush() ;
        outfile.close() ;
    }

    if( natus::ntd::filesystem::exists( db_old ) )
    {
        natus::ntd::filesystem::remove( db_old ) ;
    }
    natus::ntd::filesystem::rename( db_new, db_old ) ;
    return true ;
}

//***
void_t database::load_db_file( this_t::db_ref_t db_, natus::io::path_cref_t p ) 
{
    ::std::ifstream ifs( p, std::ios::binary ) ;

    size_t num_records = 0 ;

    natus::memory::malloc_guard< char_t > data( this_file_db::length_validator::fixed_length() ) ;

    // check file header description
    {
        ifs.read( data, this_file_db::length_validator::fixed_length() ) ;

        natus::ntd::string_t const buffer = this_t::obfuscator().
            decode( natus::ntd::string_t( data.get(), this_file_db::length_validator::fixed_length() ) ) ;
        
        natus::ntd::vector< natus::ntd::string_t > token ;
        size_t const num_elems = natus::ntd::string_ops::split( buffer, ':', token ) ;

        if( num_elems >= 3 )
        {
            if( token[0] != header_desc )
            {
                natus::log::global_t::error( "[db] : Invalid db file. Header description mismatch. Should be " + header_desc + " for file " + p.string() ) ;
                natus::log::global_t::error( "[db] : Just repack the data with the appropriate packer version." ) ;
                return ;
            }

            num_records = ::std::stol( token[ 1 ] ) ;
        }
        else 
        {
            natus::log::global_t::error( "[db] : can not read db for file " + p.string() ) ;
            return ;
        }
        
        natus::log::global_t::status( "[db] : Loading db file from " + p.string() ) ;
    }

    // add file records
    {
        for( size_t i = 0; i < num_records; ++i )
        {
            ifs.read( data, this_file_db::length_validator::fixed_length() ) ;
            natus::ntd::string_t const buffer = this_t::obfuscator().
                decode( natus::ntd::string_t( data.get(), this_file_db::length_validator::fixed_length() ) ) ;

            natus::ntd::vector< natus::ntd::string_t > token ;
            size_t const num_elems = natus::ntd::string_ops::split( buffer, ':', token ) ;

            if( num_elems < 5 ) 
            {
                natus::log::global_t::warning( "[db] : invalid file record" ) ;
                continue;
            }

            this_t::file_record_t fr ;
            fr.location = token[ 0 ] ;
            fr.extension = token[ 1 ] ;
            fr.offset = ::std::stol( token[ 2 ] ) ;
            fr.sib = ::std::stol( token[ 3 ] ) ;

            db_.records.emplace_back( fr ) ;
        }
    }
}

//***
bool_t database::unpack( void_t )
{
    return true ;
}

//***
natus::io::store_handle_t database::store( natus::ntd::string_cref_t /*location*/, char_cptr_t, size_t const )
{
    natus::io::store_handle_t h ;


    return ::std::move( h ) ;
}

//***
natus::io::load_handle_t database::load( natus::ntd::string_cref_t loc ) const
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
        lh = natus::io::global_t::load( _db.working / fr.rel, natus::io::obfuscator_t() ) ;
    }
    else if( fr.offset != uint64_t(-1) )
    {
        size_t const offset = fr.offset + _db.offset ;
        auto const p = _db.base / natus::io::path_t( _db.name ).replace_extension( natus::io::path_t( ".ndb" ) ) ;
        lh = natus::io::global_t::load( p, offset, fr.sib, this_t::obfuscator() ) ;
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
void_t database::attach( natus::ntd::string_cref_t loc, natus::io::monitor_res_t mon ) noexcept 
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
void_t database::detach( natus::ntd::string_cref_t loc, natus::io::monitor_res_t mon ) noexcept 
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
bool_t database::lookup_extension( natus::ntd::string_cref_t loc, natus::ntd::string_out_t ext ) const noexcept 
{
    this_t::file_record_t fr ;
    if( !this_t::lookup( loc, fr ) ) return false ;

    ext = fr.extension ;

    return true;
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
        auto const p = natus::ntd::filesystem::relative( path, base ) ;

        natus::ntd::string_t loc ;
        for( auto j : p.parent_path() )
        {
            loc += j ;
            loc += "." ;
        }
        loc += path.stem().string() ;
        loc += path.extension().string() ;

        fr.location = loc ;
        fr.rel = p ;
    }

    {
        // external
        fr.offset = uint64_t( -2 ) ;

        {
            ::std::error_code ec ;
            fr.sib = natus::ntd::filesystem::file_size( path, ec ) ;
            natus::log::global_t::error( ( bool_t ) ec,
                natus_log_fn( "file_size with [" + ec.message() + "]" ) ) ;
        }
    }

    // store the last write time so 
    // monitoring can  take palce.
    {
        fr.stamp = natus::ntd::filesystem::last_write_time( path ) ;
    }

    return ::std::move( fr ) ;
}

//***
bool_t database::lookup( natus::ntd::string_cref_t loc, this_t::file_record_out_t fro ) const noexcept
{
    natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;
    return this_t::lookup( _db, loc, fro ) ;
}

//***
bool_t database::lookup( this_t::db const & db_, natus::ntd::string_cref_t loc, file_record_out_t fro ) const noexcept
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
void_t database::file_change_external( this_t::file_record_cref_t fr ) noexcept 
{
    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;
    return this_t::file_change_external( _db, fr ) ;
}

//***
void_t database::file_change_external( this_t::db_t & db,  this_t::file_record_cref_t fri ) noexcept 
{
    auto iter = ::std::find_if( db.records.begin(), db.records.end(), [&] ( this_t::file_record_cref_t fr )
    {
        return fr.location == fri.location ;
    } ) ;

    if( iter == db.records.end() ) return ;
    
    iter->offset = fri.offset ;
    iter->sib = fri.sib ;
    iter->rel = fri.rel ;
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
                // is internal? At the moment, there is no
                // internal db file data change tracking.
                if( fr.offset != uint64_t( -2 ) ) continue ;

                natus::io::path_t const p = db.working / fr.rel ;
                if( !natus::ntd::filesystem::exists( p ) )
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
                
                auto const tp = natus::ntd::filesystem::last_write_time( p ) ;
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
            
            // if no external is registered, change the update rate
            {
                bool_t any_external = false ;
                for( auto& fr : db.records )
                {
                     if( fr.offset == uint64_t( -2 ) ) 
                     {
                         any_external = true ;
                         break ;
                     }
                }

                if( !any_external )
                {
                    natus::log::global_t::status( "[db][Monitor] : no external files to track. exiting early." ) ;
                    break ;
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

