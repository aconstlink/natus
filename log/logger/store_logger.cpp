
#include "store_logger.h"

using namespace natus::log ;

//********************************************************************
store_logger::store_logger( void_t )
{
    _stores.resize( max_elems ) ;
}

//********************************************************************
store_logger::store_logger( this_rref_t rhv )
{
    _stores = ::std::move( rhv._stores ) ;
    _cp_start = rhv._cp_start ;
    _cp_end = rhv._cp_end ;
}

//********************************************************************
store_logger::this_ref_t store_logger::operator = ( this_rref_t rhv )
{
    _stores = ::std::move( rhv._stores ) ;
    _cp_start = rhv._cp_start ;
    _cp_end = rhv._cp_end ;
    return *this ;
}

//********************************************************************
store_logger::~store_logger( void_t )
{}

//********************************************************************
natus::log::result store_logger::log( natus::log::log_level ll, ::std::string const & msg )
{
    this_t::store_data_t sd ;
    sd.ll = ll ;
    sd.msg = msg ;

    ::std::lock_guard< ::std::mutex > lk( _mtx ) ;

    ++_cp_end ;

    _cp_end = _cp_end >= max_elems ? 0 : _cp_end ;
    _cp_start = _cp_start >= _cp_end ? _cp_end + 1 : _cp_start ;

    _cp_start = _cp_start > _stores.size() ? 0 : _cp_start ;

    _stores[ _cp_end ] = sd ;

    return natus::log::result::ok ;
}

//********************************************************************
size_t store_logger::get_max_items( void_t ) const
{
    return max_elems ;
}

//********************************************************************
size_t store_logger::get_num_items( void_t ) const
{
    return _cp_end > _cp_start ? _cp_end - _cp_start : _cp_start - _cp_end ;
}

//********************************************************************
void_t store_logger::for_each( size_t const begin, size_t const end, funk_t f ) const
{
    ::std::lock_guard< ::std::mutex > lk( _mtx ) ;

    auto const a = _cp_start + begin ;
    auto const b = _cp_start + end ;

    for( size_t i = a; i < b; ++i )
    {
        f( _stores[ i % _stores.size() ] ) ;
    }
}

//********************************************************************
void_t store_logger::copy( size_t const begin, size_t const end, stores_ref_t sout ) const
{
    auto const a = _cp_start + begin ;
    auto const b = _cp_start + end ;

    sout.resize( b - a ) ;

    for( size_t i = a; i < b; ++i )
    {
        sout[i] = _stores[ i ]  ;
    }
}