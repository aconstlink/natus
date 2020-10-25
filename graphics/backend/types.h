#pragma once

namespace natus
{
    namespace graphics
    {
        enum class backend_type
        {
            unknown,
            gl3,
            es3,
            d3d11
        };
        static size_t const max_backends = 3 ;

        namespace detail
        {
            natus::ntd::string_t const __backend_type_names[] = {
                "unknown", "gl3", "es3", "d3d11" } ;
        }

        static natus::ntd::string_cref_t to_string( natus::graphics::backend_type const bt )
        {
            return detail::__backend_type_names[ size_t( bt ) ] ;
        }
    }
}
