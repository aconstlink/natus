
#include "generic_camera.h"
#include "ilens.h"
#include "pinhole_lens.h"

#include <natus/math/camera/3d/camera_util.hpp>
#include <natus/math/camera/3d/perspective_fov.hpp>
#include <natus/math/camera/3d/orthographic_projection.hpp>
#include <natus/math/utility/3d/transformation.hpp>

#include <natus/math/primitive/3d/frustum.hpp>

using namespace natus ;
using namespace natus::gfx ;

//*********************************************
generic_camera::generic_camera( void_t ) noexcept 
{
    _cam_frame.identity() ;
    _cam_matrix.identity() ;
    _view_matrix.identity() ;
    _proj_matrix.identity() ;
}

//*********************************************
generic_camera::generic_camera( natus::gfx::lens_res_t lens ) noexcept
{
    _cam_frame.identity() ;
    _cam_matrix.identity() ;
    _view_matrix.identity() ;
    _proj_matrix.identity() ;

    this_t::add_lens( lens ) ;
}

//*********************************************
generic_camera::generic_camera( this_rref_t rhv ) noexcept
{
    _trafo = std::move( rhv._trafo ) ;
    _lenses = std::move( rhv._lenses ) ;
    _cam_matrix = rhv._cam_matrix ;
    _cam_frame = rhv._cam_frame ;
    _view_matrix = rhv._view_matrix ;
    _proj_matrix = rhv._proj_matrix ;
    _projection_mode = rhv._projection_mode ;
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
    this->update_view_matrix( _trafo.get_transformation() ) ;
}

//*********************************************
void_t generic_camera::set_transformaion( natus::math::m3d::trafof_cref_t trafo ) noexcept
{
    _trafo = trafo ;
    this->update_view_matrix( trafo.get_transformation() ) ;
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

//*********************************************
generic_camera::this_ref_t generic_camera::make_orthographic( float_t const w, float_t const h,
    float_t const n, float_t const f ) noexcept 
{
    _proj_matrix = natus::math::m3d::orthographic<float_t>::create(
        w, h, n, f ) ;

    _projection_mode = projection_type::orthographic ;

return *this ;
}

//*********************************************
generic_camera::this_ref_t generic_camera::make_perspective_fov( float_t const fov, float_t const aspect,
float_t const n, float_t const f ) noexcept 
{
natus::math::m3d::perspective<float_t>::create_by_fovv_aspect(
    fov, aspect, n, f, _proj_matrix ) ;

_projection_mode = projection_type::perspective ;

return *this ;
}

//*********************************************
generic_camera::this_t generic_camera::create_orthographic( float_t const w, float_t const h,
float_t const n, float_t const f ) noexcept 
{
return std::move( this_t().make_orthographic( w, h, n, f ) ) ;
}

//*********************************************
generic_camera::this_t generic_camera::create_perspective_fov( float_t const fov, float_t const aspect,
float_t const n, float_t const f ) noexcept 
{
this_t ret ;

natus::math::m3d::perspective<float_t>::create_by_fovv_aspect(
    fov, aspect, n, f, ret._proj_matrix ) ;

ret._projection_mode = projection_type::perspective ;

return std::move( ret ) ;
}

//*********************************************
bool_t generic_camera::is_perspective( void_t ) const noexcept
{
return _projection_mode == projection_type::perspective ;
}

//*********************************************
bool_t generic_camera::is_orthographic( void_t ) const noexcept
{
return _projection_mode == projection_type::orthographic ;
}

//*********************************************
void_t generic_camera::update_view_matrix( natus::math::mat4f_cref_t frame ) noexcept 
{
natus::math::m3d::create_view_matrix( frame, _view_matrix ) ;
}

//*********************************************
natus::math::mat4f_cref_t generic_camera::get_view_matrix( void_t ) const noexcept 
{
return _view_matrix ;
}

//*********************************************
natus::math::mat4f_cref_t generic_camera::get_proj_matrix( void_t ) const noexcept 
{
return _proj_matrix ;
}


generic_camera::vec3_t generic_camera::get_position( void_t ) const
{
vec3_t pos ;
natus::math::m3d::get_pos_from_camera_matrix( pos, _cam_matrix ) ;
return pos ;
}

generic_camera::this_cref_t generic_camera::get_position( vec3_ref_t pos ) const
{
natus::math::m3d::get_pos_from_camera_matrix( pos, _cam_matrix ) ;
return *this ;
}

generic_camera::vec3_t generic_camera::get_direction( void_t ) const
{
vec3_t dir ;
natus::math::m3d::get_dir_from_camera_matrix<float_t>( dir, _cam_matrix ) ;
return dir ;
}

generic_camera::this_cref_t generic_camera::get_direction( vec3_ref_t dir ) const
{
natus::math::m3d::get_dir_from_camera_matrix<float_t>( dir, _cam_matrix ) ;
return *this ;
}

generic_camera::vec3_t generic_camera::get_up( void_t ) const
{
vec3_t up ;
natus::math::m3d::get_up_from_camera_matrix<float_t>( up, _cam_matrix ) ;
return up ;
}

generic_camera::this_cref_t generic_camera::get_up( vec3_ref_t up ) const
{
natus::math::m3d::get_up_from_camera_matrix<float_t>( up, _cam_matrix ) ;
return *this ;
}

generic_camera::vec3_t generic_camera::get_right( void_t ) const
{
vec3_t right ;
natus::math::m3d::get_right_from_camera_matrix<float_t>( right, _cam_matrix ) ;
return right ;
}

generic_camera::this_cref_t generic_camera::get_right( vec3_ref_t right ) const
{
natus::math::m3d::get_right_from_camera_matrix<float_t>( right, _cam_matrix ) ;
return *this ;
}