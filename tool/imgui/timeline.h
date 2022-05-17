#pragma once

#include "imgui.h"

#include <natus/ntd/string.hpp>
#include <natus/ntd/vector.hpp>

#include <array>

namespace natus
{
    namespace tool
    {
        class NATUS_TOOL_API timeline
        {
            natus_this_typedefs( timeline ) ;

        private:

            // unit: milli_per_pixel
            size_t _zoom = 10 ;

            // in milli seconds
            size_t _hover = 0 ;

            // in milli seconds
            size_t _play = 0 ;

        public:

            timeline( void_t ) noexcept ;
            timeline( this_cref_t ) = delete ;
            timeline( this_rref_t ) noexcept ;
            ~timeline( void_t ) noexcept ;

            this_ref_t operator = ( this_cref_t ) noexcept ;
            this_ref_t operator = ( this_rref_t ) noexcept ;

        public:

            bool_t begin( natus::ntd::string_cref_t label, natus::tool::imgui_view_t ) noexcept ;
            void_t end( void_t ) noexcept ;

        public:

            // add to the top of the timeline(down tri)
            bool_t set_time_marker( size_t const milli, bool_t const center = false ) noexcept ;
            
            // vertical line at milli
            bool_t set_hover_marker( size_t const milli ) noexcept ;

            // add to the bottom of the timeline(up tri)
            bool_t add_marker( size_t const milli, ImColor const & color ) ;

            struct time_region{
                size_t milli_begin ;
                size_t milli_end ;
            } ;
            natus_typedef( time_region ) ;

            // returns the region where the current view
            // begins and ends taking the zoom into account
            time_region_t get_time_region( void_t ) const noexcept ;

        public:

            inline size_t milli_to_pixel( size_t const ) const noexcept ;
            inline size_t pixel_to_milli( size_t const ) const noexcept ;

            size_t mtp( size_t const m ) const noexcept { return this_t::milli_to_pixel( m )  ; }
            size_t ptm( size_t const p ) const noexcept { return this_t::pixel_to_milli( p ) ; }

            natus::ntd::string_t make_time_string( size_t const milli ) const noexcept ;

        } ;
        natus_res_typedef( timeline ) ;
    }
}