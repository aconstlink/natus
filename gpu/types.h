#pragma once

#include "typedefs.h"
#include <natus/std/string.hpp>

namespace natus
{
    namespace gpu
    {
        enum class type
        {
            undefined,
            tchar,
            tuchar,
            tshort,
            tushort,
            tint,
            tuint,
            tfloat,
            tdouble,
            tbool,
            num_scalar_types
        } ;

        enum class type_struct
        {
            undefined,
            vec1, // scalar
            vec2,
            vec3,
            vec4,
            mat2,
            mat3,
            mat4,
            num_type_structures
        } ;

        namespace detail
        {
            static natus::std::string_t const type_string_array[] =
            {
                "t_not_implemented"
            } ;

            // this must correspond to the TYPE enum
            static size_t const type_size_array[] =
            {
                0, sizeof ( char ), sizeof ( unsigned char ), sizeof ( short ), sizeof ( unsigned short ),
                sizeof ( int ), sizeof ( unsigned int ), sizeof ( float ), sizeof ( double ), sizeof ( bool )
            } ;

            static natus::std::string_t const type_structure_string_array[] =
            {
                "ts_not_implemented"
            } ;

            // this must correspond to the TYPE_STRUCTURE enum
            static uint_t const _type_struct_size_array[] =
            {
                0, 1, 2, 3, 4, 4, 9, 16
            } ;

            // a vector every has 1 row, so vectors must stored in memory
            // like a row vector, of course.
            static uint_t const type_struct_rows[] =
            {
                0, 1, 1, 1, 1, 2, 3, 4
            } ;

            static uint_t const type_struct_columns[] =
            {
                0, 1, 2, 3, 4, 2, 3, 4
            } ;
        }

        static natus::std::string_cref_t to_string( type const /*t*/ ) noexcept
        {
            return detail::type_string_array[ 0 ] ;
        }

        static natus::std::string_cref_t to_string( type_struct const /*t*/ ) noexcept
        {
            return detail::type_structure_string_array[ 0 ] ;
        }

        static size_t size_of( type const t ) noexcept
        {
            return detail::type_size_array[ size_t( t ) ] ;
        }

        static size_t size_of( type_struct const ts ) noexcept
        {
            return detail::_type_struct_size_array[ size_t( ts ) ] ;
        }

        /// returns the number of rows a type structure has.
        /// Dont forget. Vectors have a row count of 1
        static size_t rows_of( type_struct const ts ) noexcept
        {
            return detail::type_struct_rows[ size_t( ts ) ] ;
        }

        /// returns the number of columns a type struct has.
        static size_t columns_of( type_struct const ts ) noexcept
        {
            return detail::type_struct_columns[ size_t( ts ) ] ;
        }
    }

    namespace gpu
    {
        enum class primitive_type
        {
            undefined,
            points,
            lines,
            triangles,
            num_primitive_types
        } ;

        namespace detail
        {
            static natus::std::string_t const primitive_type_string_array[] =
            {
                "undefined", "points", "lines", "triangles"
            } ;

            static size_t const vertex_count_for_primitive_type[] =
            {
                0,
                1,
                1,
                2,
                2,
                2,
                3,
                3,
                3,
                0
            } ;
        }

        static natus::std::string_cref_t to_string( primitive_type const pt )
        {
            return detail::primitive_type_string_array[ size_t( pt ) ] ;
        }

        static size_t num_vertices( primitive_type const pt )
        {
            return detail::vertex_count_for_primitive_type[ size_t( pt ) ] ;
        }
    }

    namespace gpu
    {
        enum class usage_type
        {
            unknown,
            buffer_static,
            buffer_dynamic
        };
    }
}