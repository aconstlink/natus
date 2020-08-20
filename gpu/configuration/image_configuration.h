#pragma once

#include "../object.hpp"

#include "../texture/image.hpp"

#include <natus/ntd/vector.hpp>

#include <algorithm>

namespace natus
{
    namespace gpu
    {
        class image_configuration : public object
        {
            natus_this_typedefs( image_configuration ) ;

        private:

            natus::ntd::string_t _name ;
            natus::gpu::image_t _img ;

            natus::gpu::texture_wrap_type _wrap_types[ (size_t)natus::gpu::texture_wrap_mode::size ] ;
            natus::gpu::texture_filter_type _filter_types[ (size_t)natus::gpu::texture_filter_mode::size ] ;

        public:

            image_configuration( void_t ) 
            {
                for( size_t i=0; i<(size_t)natus::gpu::texture_wrap_mode::size; ++i )
                {
                    _wrap_types[ i ] = natus::gpu::texture_wrap_type::clamp ;
                }

                for( size_t i = 0; i < ( size_t ) natus::gpu::texture_filter_mode::size; ++i )
                {
                    _filter_types[ i ] = natus::gpu::texture_filter_type::nearest ;
                }
            }
            image_configuration( natus::ntd::string_in_t name ) : _name( name )
            {
                for( size_t i = 0; i < ( size_t ) natus::gpu::texture_wrap_mode::size; ++i )
                {
                    _wrap_types[ i ] = natus::gpu::texture_wrap_type::clamp ;
                }

                for( size_t i = 0; i < ( size_t ) natus::gpu::texture_filter_mode::size; ++i )
                {
                    _filter_types[ i ] = natus::gpu::texture_filter_type::nearest ;
                }
            }
            image_configuration( natus::ntd::string_in_t name, natus::gpu::image_rref_t img) :
                image_configuration( name )
            {
                _img = ::std::move( img ) ;
            }

            image_configuration( this_cref_t rhv ) : object( rhv )
            {
                _name = rhv._name ;
                _img = rhv._img ;

                memcpy( ( void_ptr_t ) _wrap_types, ( void_cptr_t )rhv._wrap_types, sizeof(_wrap_types) ) ;
                memcpy( ( void_ptr_t ) _filter_types, ( void_cptr_t )rhv._filter_types, sizeof(_filter_types) ) ;
            }

            image_configuration( this_rref_t rhv ) : object( ::std::move( rhv ) )
            {
                _name = ::std::move( rhv._name ) ;
                _img = ::std::move( rhv._img ) ;

                memcpy( ( void_ptr_t ) _wrap_types, ( void_cptr_t ) rhv._wrap_types, sizeof( _wrap_types ) ) ;
                memcpy( ( void_ptr_t ) _filter_types, ( void_cptr_t ) rhv._filter_types, sizeof( _filter_types ) ) ;
            }

            virtual ~image_configuration( void_t ){}

            this_ref_t operator = ( this_cref_t rhv )
            {
                object::operator=( rhv ) ;

                _name = rhv._name ;
                _img = rhv._img ;

                memcpy( ( void_ptr_t ) _wrap_types, ( void_cptr_t )rhv._wrap_types, sizeof(_wrap_types) ) ;
                memcpy( ( void_ptr_t ) _filter_types, ( void_cptr_t )rhv._filter_types, sizeof(_filter_types) ) ;

                return *this ;
            }

            this_ref_t operator = ( this_rref_t rhv )
            {
                object::operator=( ::std::move( rhv ) ) ;

                _name = ::std::move( rhv._name ) ;
                _img = ::std::move( rhv._img ) ;

                memcpy( ( void_ptr_t ) _wrap_types, ( void_cptr_t ) rhv._wrap_types, sizeof( _wrap_types ) ) ;
                memcpy( ( void_ptr_t ) _filter_types, ( void_cptr_t ) rhv._filter_types, sizeof( _filter_types ) ) ;

                return *this ;
            }

            natus::ntd::string_cref_t name( void_t ) const noexcept
            {
                return _name ;
            }

        public:

            image_cref_t image( void_t ) const noexcept { return _img ; }
            image_ref_t image( void_t ) noexcept { return _img ; }

            natus::gpu::texture_wrap_type get_wrap( natus::gpu::texture_wrap_mode const wm ) const noexcept
            {
                return _wrap_types[ size_t( wm ) ] ;
            }

            natus::gpu::texture_filter_type get_filter( natus::gpu::texture_filter_mode const fm ) const noexcept
            {
                return _filter_types[ size_t( fm ) ] ;
            }

            this_ref_t set_filter( natus::gpu::texture_filter_mode const fm, natus::gpu::texture_filter_type const ft ) noexcept
            {
                _filter_types[ size_t( fm ) ] = ft ;
                return *this ;
            }

            this_ref_t set_wrap( natus::gpu::texture_wrap_mode const m, natus::gpu::texture_wrap_type const t ) noexcept
            {
                _wrap_types[ size_t( m ) ] = t ;
                return *this ;
            }

        };
        natus_res_typedef( image_configuration ) ;
    }
}
