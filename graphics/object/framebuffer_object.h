
#pragma once

#include "../object.hpp"
#include "../types.h"

#include <natus/ntd/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace graphics
    {
        class NATUS_GRAPHICS_API framebuffer_object : public object
        {
            natus_this_typedefs( framebuffer_object ) ;

        private:

            natus::ntd::string_t _name ;

            size_t _width = 0 ;
            size_t _height = 0 ;
            size_t _targets = 0 ;

            natus::graphics::color_target_type _ctt = natus::graphics::color_target_type::unknown ;
            natus::graphics::depth_stencil_target_type _dst = natus::graphics::depth_stencil_target_type::unknown ;

        public:

            framebuffer_object( void_t ) {}
            framebuffer_object( natus::ntd::string_cref_t name ) : _name( name ) {}

            framebuffer_object( size_t const width, size_t const height,
               natus::graphics::color_target_type const t ) : _width( width ), _height( height ), _ctt( t ) {}


            virtual ~framebuffer_object( void_t ) {}

            natus::ntd::string_cref_t name( void_t ) const noexcept { return _name ; }

        public:

            this_ref_t resize( size_t const width, size_t const height ) noexcept
            {
                _width = width ;
                _height = height ;
                return *this ;
            }
            
            this_ref_t set_target( natus::graphics::color_target_type const t, size_t const n = 1 ) noexcept
            {
                _ctt = t ;
                _targets = n ;
                return *this ;
            }

            this_ref_t set_target( natus::graphics::depth_stencil_target_type const t ) noexcept
            {
                _dst = t ;
                return *this ;
            }

        public:

            size_t get_num_color_targets( void_t ) const noexcept { return _targets ; }
            natus::graphics::color_target_type get_color_target( void_t ) const noexcept 
            {
                return _ctt ;
            }
            natus::graphics::depth_stencil_target_type get_depth_target( void_t ) const noexcept
            {
                return _dst ;
            }
            natus::math::vec2ui_t get_dims( void_t ) const noexcept
            {
                return natus::math::vec2ui_t( uint_t( _width ), uint_t( _height ) ) ;
            }
        } ;
        natus_res_typedef( framebuffer_object ) ;
    }
}

