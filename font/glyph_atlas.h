#pragma once

#include "api.h"
#include "typedefs.h"
#include "result.h"

#include <natus/ntd/string.hpp>
#include <natus/ntd/vector.hpp>
#include <natus/math/vector/vector2.hpp>

namespace natus
{
    namespace font
    {
        class NATUS_FONT_API glyph_atlas
        {
        natus_this_typedefs( glyph_atlas ) ;

        public: // glyph_info

            /// where to find the glyph in the atlas
            struct glyph_info
            {
                /// the glyph's code point
                utf32_t code_point ;

                /// user defined id
                size_t face_id ;

                /// in which image is the glyph stored
                size_t image_id ;

                /// x: x offset, y: y offset 
                /// in [0,1]
                natus::math::vec2f_t start ;

                /// x: width, y: height
                /// in [0,1]
                natus::math::vec2f_t dims ;

                /// x: horizontal advance
                /// y: vertical advance
                natus::math::vec2f_t adv ;

                float_t bearing ;
            };
            natus_typedef( glyph_info ) ;
            natus_typedefs( natus::ntd::vector< glyph_info_t >, glyph_info_map ) ;

        public: // image

            class image
            {
                natus_this_typedefs( image ) ;

            private:

                uint8_ptr_t _plane_ptr = nullptr ;
                size_t _width = 0 ;
                size_t _height = 0 ;

            public:

                image( void_t ) {}
                image( size_t width, size_t height ) : _width( width ), _height( height )
                {
                    _plane_ptr = natus::memory::global_t::alloc_raw<uint8_t>( width * height ) ;
                    std::memset( ( void_ptr_t ) _plane_ptr, 0, width * height * sizeof( uint8_t ) ) ;
                }
                image( this_cref_t ) = delete ;
                image( this_rref_t rhv )
                {
                    natus_move_member_ptr( _plane_ptr, rhv ) ;
                    _width = rhv._width ;
                    rhv._width = 0 ;
                    _height = rhv._height ;
                    rhv._height = 0 ;
                }
                ~image( void_t )
                {
                    natus::memory::global_t::dealloc_raw<uint8_t>( _plane_ptr ) ;
                }

                this_ref_t operator = ( this_rref_t rhv )
                {
                    natus_move_member_ptr( _plane_ptr, rhv ) ;
                    _width = rhv._width ;
                    rhv._width = 0 ;
                    _height = rhv._height ;
                    rhv._height = 0 ;
                    return *this ;
                }

            public:

                uint8_ref_t operator[]( size_t const i ) {
                    return _plane_ptr[ i ] ;
                }

            public:

                uint8_cptr_t get_plane( void_t ) const { return _plane_ptr ; }
                uint8_ptr_t get_plane( void_t ) { return _plane_ptr ; }

                size_t width( void_t ) const { return _width ; }
                size_t height( void_t ) const { return _height ; }
            };
            natus_typedef( image ) ;

        private:

            glyph_info_map_t _glyph_infos ;
            natus::ntd::vector<image_ptr_t> _atlas ;
            
            typedef std::pair< natus::ntd::string_t, size_t > name_id_t ;
            natus::ntd::vector< name_id_t > _face_name_to_id ;

        public:

            glyph_atlas( void_t ) ;
            glyph_atlas( this_cref_t ) = delete ;
            glyph_atlas( this_rref_t ) ;
            ~glyph_atlas( void_t ) ;

            this_ref_t operator = ( this_rref_t ) noexcept ;

        public: // creator interface

            bool_t add_glyph( glyph_info_cref_t ) ;
            bool_t add_image( this_t::image_rref_t ) ;

        public: // user interface
            
            void_t map_face_id( natus::ntd::string_cref_t name, size_t const id ) noexcept ;
            bool_t face_id_for_name( natus::ntd::string_cref_t name, size_t & id ) const  noexcept ;

            size_t get_num_glyphs( void_t ) const ;
            bool_t get_glyph_info( size_t const i, glyph_info_out_t ) const ;

            size_t get_num_images( void_t ) const ;
            image_cptr_t get_image( size_t const id ) const ;

            bool_t find_glyph( size_t const, utf32_t const, size_t&, this_t::glyph_info_ref_t ) const ;

        public:

            size_t get_width( void_t ) const ;
            size_t get_height( void_t ) const ;


        };
        natus_res_typedef( glyph_atlas ) ;
    }
}