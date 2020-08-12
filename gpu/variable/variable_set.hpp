
#pragma once

#include "variables.hpp"
#include "type_traits.hpp"

#include <natus/std/string.hpp>
#include <natus/std/vector.hpp>
#include <natus/concurrent/mutex.hpp>

namespace natus
{
    namespace gpu
    {
        class variable_set
        {
            natus_this_typedefs( variable_set ) ;
        
        private:

            struct data
            {
                natus::std::string_t name ;
                natus::gpu::type type ;
                natus::gpu::type_struct type_struct ;
                natus::gpu::ivariable_ptr_t var ;
            };
            natus_typedef( data ) ;
            natus::std::vector< data > _variables ;

            struct texture_data
            {
                natus::std::string_t name ;
                natus::gpu::ivariable_ptr_t var ;
            };
            natus_typedef( texture_data ) ;
            natus::std::vector< texture_data > _textures ;

            natus::concurrent::mutex_t _mtx ;

        public:

            variable_set( void_t ) noexcept {}
            variable_set( this_cref_t ) = delete ;
            variable_set( this_rref_t rhv ) noexcept
            {
                *this = ::std::move( rhv )  ;
            }

            virtual ~variable_set( void_t ) noexcept
            {
                for( auto & d : _variables )
                {
                    natus::memory::global_t::dealloc( d.var ) ;
                }
            }

            this_ref_t operator = ( this_rref_t rhv ) noexcept
            {
                _variables = ::std::move( rhv._variables ) ;
                _textures = ::std::move( rhv._textures ) ;
                return *this ;
            }

            this_ref_t operator = ( this_cref_t ) = delete ;

        public:

            template< class T >
            natus::gpu::data_variable< T > * data_variable( natus::std::string_cref_t name ) noexcept
            {
                auto const type = natus::gpu::type_traits< T >::gpu_type ;
                auto const type_struct = natus::gpu::type_traits< T >::gpu_type_struct ;

                return static_cast< natus::gpu::data_variable<T>*>( 
                    this_t::data_variable( name, type, type_struct ) ) ;
            }

            natus::gpu::ivariable_ptr_t data_variable( natus::std::string_cref_t name,
                natus::gpu::type const t, natus::gpu::type_struct const ts ) noexcept
            {
                natus::gpu::ivariable_ptr_t var = nullptr ;

                switch( t )
                {
                case natus::gpu::type::tchar: var = this_t::from_type_struct<char_t>( name, ts ) ; break ;
                case natus::gpu::type::tuchar: var = this_t::from_type_struct<uchar_t>( name, ts ) ; break ; 
                case natus::gpu::type::tshort: var = this_t::from_type_struct<short_t>( name, ts ) ; break ;
                case natus::gpu::type::tushort: var = this_t::from_type_struct<ushort_t>( name, ts ) ; break ;
                case natus::gpu::type::tint: var = this_t::from_type_struct<int_t>( name, ts ) ; break ;
                case natus::gpu::type::tuint: var = this_t::from_type_struct<uint_t>( name, ts ) ; break ;
                case natus::gpu::type::tfloat: var = this_t::from_type_struct<float_t>( name, ts ) ; break ;
                case natus::gpu::type::tdouble: var = this_t::from_type_struct<double_t>( name, ts ) ; break ;
                case natus::gpu::type::tbool: var = this_t::from_type_struct<bool_t>( name, ts ) ; break ;
                default: break ;
                }

                if( natus::core::is_nullptr( var ) ) 
                {
                    natus::log::global_t::error( natus_log_fn( "invalid type for variable " + name ) ) ;
                    return var ;
                }

                // before inserting, check if name and type match
                {
                    natus::concurrent::lock_guard_t lk( _mtx ) ;

                    auto iter = ::std::find_if( _variables.begin(), _variables.end(),
                        [&] ( this_t::data const& d )
                    {
                        return d.name == name ;
                    } ) ;

                    if( iter != _variables.end() )
                    {
                        natus::memory::global_t::dealloc( var ) ;

                        if( iter->type != t || iter->type_struct != ts )
                        {
                            natus::log::global_t::error( natus_log_fn( "type mismatch for " + name ) ) ;
                            return nullptr ;
                        }

                        return iter->var ;
                    }

                    this_t::data_t d ;
                    d.name = name ;
                    d.type = t ;
                    d.type_struct = ts ;
                    d.var = var ;

                    _variables.emplace_back( d ) ;
                }

                return var ;
            }

            natus::gpu::data_variable< natus::std::string_t > * texture_variable( 
                natus::std::string_in_t name ) noexcept
            {
                natus::gpu::ivariable_ptr_t var = natus::memory::global_t::alloc(
                    natus::gpu::data_variable<natus::std::string_t>(), natus_log_fn( "texture variable : " + name ) ) ;

                // before inserting, check if name and type match
                {
                    natus::concurrent::lock_guard_t lk( _mtx ) ;

                    auto iter = ::std::find_if( _textures.begin(), _textures.end(),
                        [&] ( this_t::texture_data const& d )
                    {
                        return d.name == name ;
                    } ) ;

                    if( iter != _textures.end() )
                    {
                        natus::memory::global_t::dealloc( var ) ;

                        return static_cast< natus::gpu::data_variable< natus::std::string_t >* >( iter->var ) ;
                    }

                    this_t::texture_data_t d ;
                    d.name = name ;
                    d.var = var ;

                    _textures.emplace_back( d ) ;
                }

                return static_cast< natus::gpu::data_variable< natus::std::string_t >* >( var ) ;
            }

        private:

            template< typename T >
            natus::gpu::ivariable_ptr_t from_type_struct( 
                natus::std::string_cref_t name, natus::gpu::type_struct const ts ) noexcept
            {
                switch( ts )
                {
                case natus::gpu::type_struct::vec1: 
                    return natus::memory::global_t::alloc( 
                        natus::gpu::data_variable<T>(), natus_log_fn( "vec1 variable " + name ) ) ;
                case natus::gpu::type_struct::vec2: 
                    return natus::memory::global_t::alloc( 
                        natus::gpu::data_variable<natus::math::vector2<T>>(), natus_log_fn( "vec2 variable " + name ) ) ;
                case natus::gpu::type_struct::vec3:
                    return natus::memory::global_t::alloc( 
                        natus::gpu::data_variable<natus::math::vector3<T>>(), natus_log_fn( "vec3 variable " + name ) ) ;
                case natus::gpu::type_struct::vec4:
                    return natus::memory::global_t::alloc( 
                        natus::gpu::data_variable<natus::math::vector4<T>>(), natus_log_fn( "vec4 variable " + name ) ) ;
                case natus::gpu::type_struct::mat2:
                    return natus::memory::global_t::alloc( 
                        natus::gpu::data_variable<natus::math::matrix2<T>>(natus::math::matrix2<T>().identity()), natus_log_fn( "mat2 variable " + name ) ) ;
                case natus::gpu::type_struct::mat3:
                    return natus::memory::global_t::alloc( 
                        natus::gpu::data_variable<natus::math::matrix3<T>>(natus::math::matrix3<T>().identity()), natus_log_fn( "mat2 variable " + name ) ) ;
                case natus::gpu::type_struct::mat4:
                    return natus::memory::global_t::alloc( 
                        natus::gpu::data_variable<natus::math::matrix4<T>>(natus::math::matrix4<T>().identity()), natus_log_fn( "mat2 variable " + name ) ) ;
                default:
                    break ;
                }
                return nullptr ;
            }
        };
        natus_res_typedef( variable_set ) ;
    }
}
