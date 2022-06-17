
#include "generic_camera.h"
#include "ilens.h"
#include "pinhole_lens.h"

using namespace natus ;
using namespace natus::gfx ;

//*********************************************
generic_camera::generic_camera( void_t ) noexcept {}

//*********************************************
generic_camera::generic_camera( natus::gfx::lens_res_t lens ) noexcept
{
    this_t::add_lens( lens ) ;
}

//*********************************************
generic_camera::generic_camera( this_rref_t rhv ) noexcept
{
    _trafo = ::std::move( rhv._trafo ) ;
    _lenses = ::std::move( rhv._lenses ) ;
}

//*********************************************
generic_camera::~generic_camera( void_t ) noexcept {}

 //*********************************************
 natus::gfx::result generic_camera::add_lens( natus::gfx::lens_res_t lens ) noexcept
 {
     _lenses.push_back( lens ) ;
     return natus::gfx::result::ok ;
 }

 //*********************************************
 void_t generic_camera::replace_lens( size_t const i, lens_res_t lens ) noexcept  
 {
     if( _lenses.size() <= i ) return ;

     _lenses[i] = lens ;
 }

 //*********************************************
 size_t generic_camera::get_num_lenses( void_t ) const noexcept
 {
     return _lenses.size() ;
 }

  //*********************************************
 lens_res_t generic_camera::get_lens( size_t const i ) noexcept
 {
     return i < _lenses.size() ? _lenses[ i ] : lens_res_t() ;
 }

  //*********************************************
 void_t generic_camera::transform_by( natus::math::m3d::trafof_cref_t trafo ) noexcept
 {
     _trafo = _trafo * trafo ;
     for( lens_res_t& res : _lenses )
     {
         res->update_view_matrix( trafo.get_transformation() ) ;
     }
 }

  //*********************************************
 void_t generic_camera::set_transformaion( natus::math::m3d::trafof_cref_t trafo ) noexcept
 {
     _trafo = trafo ;
     for( lens_res_t& res : _lenses )
     {
         res->update_view_matrix( trafo.get_transformation() ) ;
     }
 }

  //*********************************************
 natus::math::m3d::trafof_cref_t generic_camera::get_transformation( void_t ) const noexcept
 {
     return _trafo ;
 }

  //********************************************* 
 void_t generic_camera::for_each_lens( lens_funk_t funk ) noexcept
 {
     for( auto& res : _lenses )
     {
         funk( *this, res ) ;
     }
 }