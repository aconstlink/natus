
#include "../types.h"


#include <limits>
#include <cstdlib>

#include <natus/math/vector/vector2.hpp>
#include <natus/math/vector/vector3.hpp>
#include <natus/math/vector/vector4.hpp>

#include <natus/math/matrix/matrix2.hpp>
#include <natus/math/matrix/matrix3.hpp>
#include <natus/math/matrix/matrix4.hpp>


namespace natus
{
    namespace gpu
    {
        template< typename type_t >
        class type_traits
        {
            static const natus::gpu::type gpu_type ;
            static const natus::gpu::type_struct gpu_type_struct ;
        };

        template<>
        class type_traits< ushort_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tshort ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec1 ;
        };

        template<>
        class type_traits< natus::math::vec2us_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tshort ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec2 ;
        };

        template<>
        class type_traits< natus::math::vec3us_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tshort ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec3 ;
        };

        template<>
        class type_traits< natus::math::vec4us_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tshort ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec4 ;
        };

        template<>
        class type_traits< uint_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tuint ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec1 ;
        };

        template<>
        class type_traits< natus::math::vec2ui_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tuint ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec2 ;
        };

        template<>
        class type_traits< natus::math::vec3ui_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tuint ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec3 ;
        };

        template<>
        class type_traits< natus::math::vec4ui_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tuint ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec4 ;
        };

        template<>
        class type_traits< int_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tint ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec1 ;
        };

        template<>
        class type_traits< natus::math::vec2i_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tint ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec2 ;
        };

        template<>
        class type_traits< natus::math::vec3i_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tint ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec3 ;
        };

        template<>
        class type_traits< natus::math::vec4i_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tint ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec4 ;
        };

        template<>
        class type_traits< float_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tfloat ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec1 ;
        };

        template<>
        class type_traits< natus::math::vec2f_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tfloat ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec2 ;
        };

        template<>
        class type_traits< natus::math::vec3f_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tfloat ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec3 ;
        };

        template<>
        class type_traits< natus::math::vec4f_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tfloat ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec4 ;
        };

        template<>
        class type_traits< bool_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tbool ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec1 ;
        };

        template<>
        class type_traits< natus::math::vec2b_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tbool ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec2 ;
        };

        template<>
        class type_traits< natus::math::vec3b_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tbool ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec3 ;
        };

        template<>
        class type_traits< natus::math::vec4b_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tbool ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec4 ;
        };

        template<>
        class type_traits< char_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tchar ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec1 ;
        };

        template<>
        class type_traits< natus::math::vec2c_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tchar ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec2 ;
        };

        template<>
        class type_traits< natus::math::vec3c_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tchar ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec3 ;
        };

        template<>
        class type_traits< natus::math::vec4c_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tchar ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec4 ;
        };

        template<>
        class type_traits< uchar_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tuchar ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec1 ;
        };

        template<>
        class type_traits< natus::math::vec2uc_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tuchar ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec2 ;
        };

        template<>
        class type_traits< natus::math::vec3uc_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tuchar ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec3 ;
        };

        template<>
        class type_traits< natus::math::vec4uc_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tuchar ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::vec4 ;
        };

        template<>
        class type_traits< natus::math::mat3f_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tfloat ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::mat3 ;
        };

        template<>
        class type_traits< natus::math::mat4f_t >
        {
        public:

            static const natus::gpu::type gpu_type = natus::gpu::type::tfloat ;
            static const natus::gpu::type_struct gpu_type_struct = natus::gpu::type_struct::mat4 ;
        };
    }
}