

#pragma once

#include "../api.h"
#include "../typedefs.h"

#include "image_types.h"

#include <natus/math/vector/vector3.hpp>

namespace natus
{
    namespace gpu
    {
        // default: 2d rgba 8 bit/channel image
        class NATUS_GPU_API image
        {
            natus_this_typedefs( image ) ;

        public:

            natus_typedefs( natus::math::vector3< size_t >, dims ) ;

        private:

            natus::gpu::image_format _if = natus::gpu::image_format::rgba ;
            natus::gpu::image_element_type _iet = natus::gpu::image_element_type::uint8 ;
            
            size_t _width = 0 ;
            size_t _height = 0 ;
            size_t _depth = 1 ;

            void_ptr_t _data = nullptr ;

        public:

            image( void_t ){}

            image( this_t::dims_in_t dims )
            {
                this_t::resize( dims ) ;
            }

            image( natus::gpu::image_format const imf, natus::gpu::image_element_type iet )
                : _if( imf ), _iet( iet ){}

            image( natus::gpu::image_format const imf, natus::gpu::image_element_type iet,
                this_t::dims_in_t dims )
                : _if( imf ), _iet( iet ) 
            {
                this_t::resize( dims ) ;
            }

            image( this_cref_t rhv ) 
            {
                *this = rhv ;
            }
            image( this_rref_t rhv )
            {
                *this = ::std::move( rhv ) ;
            }
            ~image( void_t ) 
            {
                natus::memory::global_t::dealloc( _data ) ;
            }

            this_ref_t operator = ( this_cref_t rhv ) noexcept
            {
                _if = rhv._if ;
                _iet = rhv._iet ;
                _width = rhv._width ;
                _height = rhv._height ;
                _depth = rhv._depth ;
                
                this_t::resize( _width, _height, _depth ) ;
                ::std::memcpy( _data, rhv._data, this_t::sib() ) ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _if = rhv._if ;
                _iet = rhv._iet ;
                _width = rhv._width ;
                _height = rhv._height ;
                _depth = rhv._depth ;

                natus_move_member_ptr( _data, rhv ) ;

                return *this ;
            }


        public:

            size_t sib( void_t ) const noexcept 
            {
                return natus::gpu::size_of( _iet )* natus::gpu::size_of( _if )*
                    _width* _height* _depth ;
            }

            this_ref_t resize( this_t::dims_in_t dims ) noexcept
            {
                return this_t::resize( dims.x(), dims.y(), dims.z() ) ;
            }

            this_ref_t resize( size_t const w, size_t const h ) noexcept
            {
                return this_t::resize( w, h, 1 ) ;
            }

            this_ref_t resize( size_t const w, size_t const h, size_t const d ) noexcept
            {
                _width = w ;
                _height = h ;
                _depth = d ;

                size_t const sib = this_t::sib() ;

                _data = natus::memory::global_t::alloc( sib, 
                    natus::memory::purpose_t( natus_log_fn( "image" ) ) ) ;

                return *this ;
            }

            dims_t get_dims( void_t ) const noexcept
            {
                return natus::math::vector3< size_t >( _width, _height, _depth ) ;
            }
            
            natus::gpu::image_format get_image_format( void_t ) const noexcept 
            {
                return _if ;
            }

            natus::gpu::image_element_type get_image_element_type( void_t ) const noexcept 
            {
                return _iet ;
            }

        public:

            typedef ::std::function< void_t ( this_ptr_t, dims_in_t, void_ptr_t ) > update_funk_t ;
            this_ref_t update( update_funk_t funk )
            {
                funk( this, this_t::get_dims(), _data ) ;
                return *this ;
            }
        };
        natus_soil_typedef( image ) ;
    }
}