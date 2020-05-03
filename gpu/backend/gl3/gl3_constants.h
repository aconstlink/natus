#pragma once

#include <natus/ogl/gl/gl.h>

namespace natus
{
    namespace gpu
    {
        namespace gl3
        {
            namespace detai
            {
                static const GLenum uniform_types_gl[] =
                {
                    0,
                    GL_FLOAT,
                    GL_FLOAT_VEC2,
                    GL_FLOAT_VEC3,
                    GL_FLOAT_VEC4,
                    GL_INT,
                    GL_INT_VEC2,
                    GL_INT_VEC3,
                    GL_INT_VEC4,
                    GL_UNSIGNED_INT,
                    GL_UNSIGNED_INT_VEC2,
                    GL_UNSIGNED_INT_VEC3,
                    GL_UNSIGNED_INT_VEC4,
                    GL_BOOL,
                    GL_BOOL_VEC2,
                    GL_BOOL_VEC3,
                    GL_BOOL_VEC4,
                    GL_FLOAT_MAT2,
                    GL_FLOAT_MAT3,
                    GL_FLOAT_MAT4,
                    GL_SAMPLER_1D,
                    GL_SAMPLER_2D,
                    GL_SAMPLER_3D,
                    GL_INT_SAMPLER_1D,
                    GL_INT_SAMPLER_2D,
                    GL_INT_SAMPLER_3D,
                    GL_UNSIGNED_INT_SAMPLER_1D,
                    GL_UNSIGNED_INT_SAMPLER_2D,
                    GL_UNSIGNED_INT_SAMPLER_3D,
                    GL_SAMPLER_CUBE,
                    GL_SAMPLER_1D_SHADOW,
                    GL_SAMPLER_2D_SHADOW,
                    GL_SAMPLER_BUFFER,
                    GL_INT_SAMPLER_BUFFER,
                    GL_UNSIGNED_INT_SAMPLER_BUFFER,
                    GL_SAMPLER_2D_ARRAY,
                    GL_INT_SAMPLER_2D_ARRAY,
                    GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
                } ;

                static size_t const uniform_types_gl_size =
                    sizeof ( uniform_types_gl ) / sizeof ( uniform_types_gl[ 0 ] ) ;
            }

            static GLuint uniform_size_of( GLenum const e ) noexcept
            {
                switch( e ) 
                {
                case GL_FLOAT: return sizeof( GLfloat ) ;
                case GL_FLOAT_VEC2: return sizeof( GLfloat ) * 2 ;
                case GL_FLOAT_VEC3: return sizeof( GLfloat ) * 3 ;
                case GL_FLOAT_VEC4: return sizeof( GLfloat ) * 4 ;
                case GL_INT: return sizeof( GLint );
                case GL_INT_VEC2: return sizeof( GLint ) * 2 ;
                case GL_INT_VEC3: return sizeof( GLint ) * 3 ;
                case GL_INT_VEC4: return sizeof( GLint ) * 4 ;
                case GL_UNSIGNED_INT: return sizeof( GLuint );
                case GL_UNSIGNED_INT_VEC2: return sizeof( GLuint ) * 2 ;
                case GL_UNSIGNED_INT_VEC3: return sizeof( GLuint ) * 3 ;
                case GL_UNSIGNED_INT_VEC4: return sizeof( GLuint ) * 4 ;
                case GL_BOOL: return sizeof( GLboolean );
                case GL_BOOL_VEC2: return sizeof( GLboolean ) * 2 ;
                case GL_BOOL_VEC3: return sizeof( GLboolean ) * 3 ;
                case GL_BOOL_VEC4: return sizeof( GLboolean ) * 4 ;
                case GL_FLOAT_MAT2: return sizeof( GLfloat ) * 4 ;
                case GL_FLOAT_MAT3: return sizeof( GLfloat ) * 9 ;
                case GL_FLOAT_MAT4: return sizeof( GLfloat ) * 16 ;
                case GL_SAMPLER_1D: return sizeof( GLint );
                case GL_SAMPLER_2D: return sizeof( GLint );
                case GL_SAMPLER_3D: return sizeof( GLint );
                case GL_INT_SAMPLER_1D: return sizeof( GLint );
                case GL_INT_SAMPLER_2D: return sizeof( GLint );
                case GL_INT_SAMPLER_3D: return sizeof( GLint );
                case GL_UNSIGNED_INT_SAMPLER_1D: return sizeof( GLint );
                case GL_UNSIGNED_INT_SAMPLER_2D: return sizeof( GLint );
                case GL_UNSIGNED_INT_SAMPLER_3D: return sizeof( GLint );
                case GL_SAMPLER_CUBE: return sizeof( GLint );
                case GL_SAMPLER_1D_SHADOW: return sizeof( GLint );
                case GL_SAMPLER_2D_SHADOW: return sizeof( GLint );
                case GL_SAMPLER_BUFFER: return sizeof( GLint );
                case GL_INT_SAMPLER_BUFFER: return sizeof( GLint );
                case GL_UNSIGNED_INT_SAMPLER_BUFFER: return sizeof( GLint );
                case GL_SAMPLER_2D_ARRAY: return sizeof( GLint );
                case GL_INT_SAMPLER_2D_ARRAY: return sizeof( GLint );
                case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return sizeof( GLint );
                default: return 0 ;
                }
                return 0 ;
            }

            static GLenum complex_to_simple_type( GLenum const e ) noexcept
            {
                switch( e )
                {
                case GL_FLOAT: return GL_FLOAT ;
                case GL_FLOAT_VEC2: return GL_FLOAT ;
                case GL_FLOAT_VEC3: return GL_FLOAT ;
                case GL_FLOAT_VEC4: return GL_FLOAT ;
                case GL_INT: return GL_INT ;
                case GL_INT_VEC2: return GL_INT ;
                case GL_INT_VEC3: return GL_INT ;
                case GL_INT_VEC4: return GL_INT ;
                case GL_UNSIGNED_INT: return GL_UNSIGNED_INT ;
                case GL_UNSIGNED_INT_VEC2: return GL_UNSIGNED_INT ;
                case GL_UNSIGNED_INT_VEC3: return GL_UNSIGNED_INT ;
                case GL_UNSIGNED_INT_VEC4: return GL_UNSIGNED_INT ;
                case GL_BOOL: return GL_BOOL ;
                case GL_BOOL_VEC2: return GL_BOOL ;
                case GL_BOOL_VEC3: return GL_BOOL ;
                case GL_BOOL_VEC4: return GL_BOOL ;
                case GL_FLOAT_MAT2: return GL_FLOAT ;
                case GL_FLOAT_MAT3: return GL_FLOAT ;
                case GL_FLOAT_MAT4: return GL_FLOAT ;
                case GL_SAMPLER_1D: 
                case GL_SAMPLER_2D: 
                case GL_SAMPLER_3D: 
                case GL_INT_SAMPLER_1D: 
                case GL_INT_SAMPLER_2D: 
                case GL_INT_SAMPLER_3D: 
                case GL_UNSIGNED_INT_SAMPLER_1D:
                case GL_UNSIGNED_INT_SAMPLER_2D: 
                case GL_UNSIGNED_INT_SAMPLER_3D: 
                case GL_SAMPLER_CUBE: 
                case GL_SAMPLER_1D_SHADOW: 
                case GL_SAMPLER_2D_SHADOW: 
                case GL_SAMPLER_BUFFER: 
                case GL_INT_SAMPLER_BUFFER: 
                case GL_UNSIGNED_INT_SAMPLER_BUFFER:
                case GL_SAMPLER_2D_ARRAY:
                case GL_INT_SAMPLER_2D_ARRAY: 
                case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return GL_INT ;
                default: return 0 ;
                }
                return 0 ;
            }
        }

        namespace gl3
        {
            typedef ::std::function< void_t ( GLuint, GLuint, void_ptr_t ) > uniform_funk_t ;

            static natus::gpu::gl3::uniform_funk_t uniform_funk( GLenum const e ) noexcept
            {
                natus::gpu::gl3::uniform_funk_t empty_funk =
                    [=] ( GLuint, GLuint, void_ptr_t ) { } ;

                switch( e )
                {

                    //************************************************************

                case GL_FLOAT: return [=] ( GLuint loc, GLuint /*count*/, void_ptr_t p )
                { natus::ogl::gl::glUniform1f( loc, *( const GLfloat* ) p ); } ;

                case GL_FLOAT_VEC2: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniform2fv( loc, count, ( const GLfloat* ) p ); } ;

                case GL_FLOAT_VEC3: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniform3fv( loc, count, ( const GLfloat* ) p ); } ;

                case GL_FLOAT_VEC4: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniform4fv( loc, count, ( const GLfloat* ) p ); } ;

                    //************************************************************

                case GL_INT: return [=] ( GLuint loc, GLuint /*count*/, void_ptr_t p )
                { natus::ogl::gl::glUniform1i( loc, *( const GLint* ) p ); } ;

                case GL_INT_VEC2: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniform2iv( loc, count, ( const GLint* ) p ); } ;

                case GL_INT_VEC3: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniform3iv( loc, count, ( const GLint* ) p ); } ;

                case GL_INT_VEC4: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniform4iv( loc, count, ( const GLint* ) p ); } ;

                    //************************************************************

                case GL_UNSIGNED_INT: return [=] ( GLuint loc, GLuint /*count*/, void_ptr_t p )
                { natus::ogl::gl::glUniform1ui( loc, *( const GLuint* ) p ); } ;

                case GL_UNSIGNED_INT_VEC2: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniform2uiv( loc, count, ( const GLuint* ) p ); } ;

                case GL_UNSIGNED_INT_VEC3: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniform3uiv( loc, count, ( const GLuint* ) p ); } ;

                case GL_UNSIGNED_INT_VEC4: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniform4uiv( loc, count, ( const GLuint* ) p ); } ;

                    //************************************************************

                case GL_FLOAT_MAT2: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniformMatrix2fv( loc, count, GL_TRUE, ( const GLfloat* ) p ); } ;

                case GL_FLOAT_MAT3: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniformMatrix3fv( loc, count, GL_TRUE, ( const GLfloat* ) p ); } ;

                case GL_FLOAT_MAT4: return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                { natus::ogl::gl::glUniformMatrix4fv( loc, count, GL_TRUE, ( const GLfloat* ) p ); } ;

                    //************************************************************

                case GL_SAMPLER_1D: 
                case GL_SAMPLER_2D: 
                case GL_SAMPLER_3D: 
                case GL_INT_SAMPLER_1D: 
                case GL_INT_SAMPLER_2D: 
                case GL_INT_SAMPLER_3D: 
                case GL_UNSIGNED_INT_SAMPLER_1D: 
                case GL_UNSIGNED_INT_SAMPLER_2D: 
                case GL_UNSIGNED_INT_SAMPLER_3D: 
                case GL_SAMPLER_CUBE: 
                case GL_SAMPLER_1D_SHADOW: 
                case GL_SAMPLER_2D_SHADOW: 
                case GL_SAMPLER_BUFFER:
                case GL_INT_SAMPLER_BUFFER: 
                case GL_UNSIGNED_INT_SAMPLER_BUFFER: 
                case GL_SAMPLER_2D_ARRAY: 
                case GL_INT_SAMPLER_2D_ARRAY: 
                case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: 
                    return [=] ( GLuint loc, GLuint count, void_ptr_t p )
                    { natus::ogl::gl::glUniform1iv( loc, count, ( const GLint* ) p ); } ;

                default: break ;
                }
                return empty_funk ;
            }
        }

        namespace gl3
        {
            static natus::std::string_t to_string( GLenum const e ) noexcept
            {
                switch( e ) 
                {
                case GL_NO_ERROR: return natus::std::string_t("GL_NO_ERROR") ;
                case GL_INVALID_ENUM: return natus::std::string_t("GL_INVALID_ENUM") ;
                case GL_INVALID_VALUE: return natus::std::string_t("GL_INVALID_VALUE") ;
                case GL_INVALID_OPERATION: return natus::std::string_t("GL_INVALID_OPERATION") ;
                default: break ;
                }
                return ::std::to_string( e ) ;
            }
        }
    }
}