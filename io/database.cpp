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

struct database::record_cache
{
    natus_this_typedefs( record_cache ) ;
    natus_res_typedef( record_cache ) ;

    natus::concurrent::mrsw_t ac ;

    natus::io::load_handle_t _lh ;

    char_ptr_t _data = nullptr ;
    size_t _sib = 0 ;

    database* owner = nullptr ;
    size_t _idx = size_t( -1 ) ;
    

    record_cache( void_t )
    {
    }

    record_cache( database* owner_new, size_t const idx )
    {
        owner = owner_new ;
        _idx = idx ;
    }

    record_cache( this_rref_t rhv )
    {
        *this = std::move( rhv ) ;
    }

    ~record_cache( void_t )
    {
    }

    record_cache::this_ref_t operator = ( this_rref_t rhv )
    {
        _idx = rhv._idx ;
        rhv._idx = size_t( -1 ) ;
        owner = rhv.owner ;
        rhv.owner = nullptr ;

        natus_move_member_ptr( _data, rhv ) ;
        _sib = rhv._sib ;

        _lh = std::move( rhv._lh ) ;

        return *this ;
    }

    bool_t can_wait( void_t ) const noexcept { return _lh.can_wait() ; }
    bool_t has_data( void_t ) const noexcept { return natus::core::is_not_nullptr( _data ) ; }

    void_t take_load_handle( natus::io::load_handle_rref_t hnd )
    {
        _lh = std::move( hnd ) ;
    }

    bool_t wait_for_operation( natus::io::database::load_completion_funk_t funk )
    {
        if( _idx == size_t( -1 ) )
        {
            natus::log::global_t::error( "[db cache] : invalid handle" ) ;
            return false ;
        }

        auto const res = _lh.wait_for_operation( [&] ( char_cptr_t data, size_t const sib, natus::io::result const err )
        {
            if( err != natus::io::result::ok )
            {
                natus::log::global_t::error( "[db] : failed to load data loc " + natus::io::to_string( err ) ) ;
                return ;
            }

            // @todo cache data...
            natus::memory::malloc_guard< char_t > const mg( data, sib ) ;

            funk( mg.get(), mg.size() ) ;
        } ) ;

        // there was no load operation, so take data from cache
        if( res == natus::io::result::invalid_handle )
        {
            if( _data == nullptr )
            {
                natus::log::global_t::error( "[db] : no load pending and no data cached. "
                    "This function requires a db load call." ) ;
                return false ;
            }

            funk( _data, _sib ) ;
            return true ;
        }
        return res == natus::io::result::ok ;
    }

    void_t change_database( natus::io::database* owner_new )
    {
        owner = owner_new ;
    }

    natus::io::database::record_cache::record_cache_res_t load(
        natus::io::location_cref_t loc, bool_t const reload ) noexcept
    {
        return owner->load( loc, reload )._res ;
    }

    natus::io::database::record_cache::record_cache_res_t load( bool_t const reload ) noexcept
    {
        natus::ntd::string_t loc = owner->location_for_index( _idx ) ;
        return owner->load( loc, reload )._res ;
    }
};

bool_t database::cache_access::wait_for_operation( natus::io::database::load_completion_funk_t funk ) 
{
    if( !_res.is_valid() ) return false ;
    return _res->wait_for_operation( funk ) ;
}

natus::io::database::cache_access_t::this_t database::cache_access::load( 
    natus::io::location_cref_t loc, bool_t const reload ) noexcept
{
    this->_res = _res->load( loc, reload ) ;
    return std::move( *this ) ;
}

natus::io::database::cache_access_t::this_t database::cache_access::load( bool_t const reload ) noexcept
{
    return this_t( _res->load( reload ) ) ;
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
    this_t::spawn_update() ;
}

//***
database::database( this_rref_t rhv )
{
    *this = std::move( rhv ) ;
}

//***
database::~database( void_t )
{
    this_t::join_update() ;
}

database::this_ref_t database::operator = ( this_rref_t rhv ) noexcept 
{
    this_t::join_update() ;
    _db = ::std::move( rhv._db ) ;

    natus::concurrent::mrsw_t::writer_lock_t lk( _ac ) ;
    for( auto& rec : _db.records )
    {
        rec.cache->change_database( this ) ;
    }

    this_t::spawn_update() ;
    return *this ;
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

                auto fr = this_t::create_file_record( db, i.path() ) ;
                
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
                                << "Only unique file names supported. See [" << fr.location.as_string() << "] with extensions "
                                << "[" << fr.location.extension() << ", " << fr2.location.extension() << "] "
                                << "where [" << fr2.location.extension() << "] already stored" ;

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
        _update_thread = natus::concurrent::thread_t([=](){}) ;
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
        natus::io::location_t location ;
        uint64_t offset ;
        uint64_t sib ;
        natus::io::path_t path ;
        bool_t external ;

        natus::ntd::string_t to_string( void_t ) const 
        {
            ::std::stringstream ss ;
            ss  << location.as_string() << ":" << location.extension() << ":"
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
       
        // file content 
        {
            for( auto & fr : records )
            {
                this_t::load( fr.location ).wait_for_operation( [&] ( char_cptr_t data, size_t const sib )
                {
                    natus::ntd::string_t const wdata = this_t::obfuscator().encode( data, sib ) ;

                    outfile.seekp( fr.offset ) ;
                    outfile.write( wdata.c_str(), sib ) ;

                } ) ;
            }
            outfile.flush() ;
        }
        
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
            fr.location = natus::io::location_t( token[ 0 ] ) ;
            fr.offset = ::std::stol( token[ 2 ] ) ;
            fr.sib = ::std::stol( token[ 3 ] ) ;
            fr.cache = this_t::record_cache( const_cast< this_ptr_t >( this ), db_.records.size() ) ;

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
database::cache_access_t database::load( natus::io::location_cref_t loc, bool_t const reload )
{
    this_t::file_record_t fr ;
    if( natus::core::is_not( this_t::lookup( loc, fr ) ) )
    {
        natus::log::global_t::warning( "resource location not found : " + loc.as_string() ) ;
        return this_t::cache_access_t( this_t::record_cache_res_t() ) ;
    }
    
    this_t::cache_access_t ret = this_t::cache_access_t( fr.cache ) ;

    // need write lock so no concurrent task is doing the load in parallel.
    natus::concurrent::mrsw_t::writer_lock_t lk( fr.cache->ac ) ;

    // is load going on?
    if( fr.cache->can_wait() ) return std::move( ret ) ;

    if( !fr.cache->has_data() || reload )
    {
        natus::io::load_handle_t lh ;
        
        // load from filesystem
        if( fr.offset == uint64_t( -2 ) )
        {
            lh = natus::io::global_t::load( _db.working / fr.rel, natus::io::obfuscator_t() ) ;
        }
        // load from .ndb file
        else if( fr.offset != uint64_t( -1 ) )
        {
            size_t const offset = fr.offset + _db.offset ;
            auto const p = _db.base / natus::io::path_t( _db.name ).replace_extension( natus::io::path_t( ".ndb" ) ) ;
            lh = natus::io::global_t::load( p, offset, fr.sib, this_t::obfuscator() ) ;
        }

        fr.cache->take_load_handle( std::move( lh ) ) ;
    }

    return std::move( ret ) ;
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
                << fr.location.as_string() ;

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

    ext = fr.location.extension() ;

    return true;
}

//***
database::file_record_t database::create_file_record( this_t::db_ref_t db, natus::io::path_cref_t path ) const noexcept
{
    this_t::file_record_t fr ;
 
    // the files' extension
    {
        //fr.extension = path.extension() ;
    }

    // determine file locator
    {
        // the relative path to the base path       
        fr.rel = natus::ntd::filesystem::relative( path, db.working ) ;
        fr.location = natus::io::location_t::from_path( fr.rel ) ;
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
    // monitoring can  take place.
    {
        fr.stamp = natus::ntd::filesystem::last_write_time( path ) ;
    }

    {
        fr.cache = this_t::record_cache( const_cast< this_ptr_t >( this ), db.records.size() ) ;
    }

    return ::std::move( fr ) ;
}

//***
bool_t database::lookup( natus::io::location_cref_t loc, this_t::file_record_out_t fro ) const noexcept
{
    natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;
    return this_t::lookup( _db, loc, fro ) ;
}

//***
bool_t database::lookup( this_t::db const & db_, natus::io::location_cref_t loc, file_record_out_t fro ) const noexcept
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
natus::io::location_t database::location_for_index( size_t const idx ) const 
{
    natus::concurrent::mrsw_t::reader_lock_t lk( _ac ) ;
    if( _db.records.size() <= idx ) return natus::io::location_t( natus::ntd::string_t( "" ) ) ;
    return _db.records[ idx ].location ;
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
void_t database::spawn_update( void_t ) noexcept
{
    this_t::join_update() ;

    _update_thread = natus::concurrent::thread_t( [&] ( void_t )
    { 
        size_t const ms = 500 ;

        natus::log::global_t::status("[db] : update thread going up @ " + std::to_string(ms) + " ms") ;

        while( !_isleep.sleep_for( std::chrono::milliseconds(ms) ) )
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
        }

        natus::log::global_t::status("[db] : update thread shutting down") ;
    } ) ;
}

//***
void_t database::join_update( void_t ) noexcept 
{
    if( _update_thread.joinable() ) 
    { 
        _isleep.interrupt() ; 
        _update_thread.join() ; 
        _isleep.reset() ;
    }
}

