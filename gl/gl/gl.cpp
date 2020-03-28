#include "gl.h"

#include <natus/log/global.h>

#if defined( NATUS_TARGET_OS_WIN )
#include <GL/wglext.h>
#elif defined( NATUS_TARGET_OS_LIN )
//#include <X11/X.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <X11/Xmd.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

using namespace natus::gl ;

/////////////////////////////////////////////////////////////////////////
// some defines
/////////////////////////////////////////////////////////////////////////

#define CHECK_AND_LOAD_COND( fn, name ) \
    !natus::log::global_t::error( \
    (fn = (fn == NULL ? (decltype(fn))(this_t::load_gl_function( name )) : fn)) == NULL, \
    "[CHECK_AND_LOAD_COND] : Failed to load: "  name  )

#define CHECK_AND_LOAD( fn, name ) \
{ \
    if( fn == NULL ) \
    { \
        fn = (decltype(fn))(this_t::load_gl_function( name ) ) ; \
    } \
    \
    natus::log::global_t::error( fn == NULL, "[CHECK_AND_LOAD] : Failed to load: "  name  ) ; \
}


#define CHECK_AND_LOAD_BY_FN( fn ) CHECK_AND_LOAD( fn, #fn )

#define NULL_STATIC_MEMBER( fn ) decltype(natus::gl::bindings::fn) natus::gl::bindings::fn = nullptr

/////////////////////////////////////////////////////////////////////////
// member init
/////////////////////////////////////////////////////////////////////////

natus::gl::bindings::string_list_t bindings::_extensions = natus::gl::bindings::string_list_t( ) ;
bool_t natus::gl::bindings::_init = false ;

NULL_STATIC_MEMBER( glCullFace ) ;
NULL_STATIC_MEMBER( glFrontFace ) ;
NULL_STATIC_MEMBER( glHint ) ;
NULL_STATIC_MEMBER( glLineWidth);
NULL_STATIC_MEMBER( glPointSize ) ;
NULL_STATIC_MEMBER( glPolygonMode ) ;
NULL_STATIC_MEMBER( glScissor ) ;
NULL_STATIC_MEMBER( glTexParameterf ) ;
NULL_STATIC_MEMBER( glTexParameterfv ) ;
NULL_STATIC_MEMBER( glTexParameteri ) ;
NULL_STATIC_MEMBER( glTexParameteriv ) ;
NULL_STATIC_MEMBER( glTexImage1D ) ;
NULL_STATIC_MEMBER( glTexImage2D ) ;
NULL_STATIC_MEMBER( glDrawBuffer ) ;
NULL_STATIC_MEMBER( glClear ) ;
NULL_STATIC_MEMBER( glClearColor ) ;
NULL_STATIC_MEMBER( glClearStencil ) ;
NULL_STATIC_MEMBER( glClearDepth ) ;
NULL_STATIC_MEMBER( glStencilMask ) ;
NULL_STATIC_MEMBER( glColorMask ) ;
NULL_STATIC_MEMBER( glDepthMask ) ;
NULL_STATIC_MEMBER( glDisable ) ;
NULL_STATIC_MEMBER( glEnable ) ;
NULL_STATIC_MEMBER( glFinish ) ;
NULL_STATIC_MEMBER( glFlush ) ;
NULL_STATIC_MEMBER( glBlendFunc ) ;
NULL_STATIC_MEMBER( glLogicOp ) ;
NULL_STATIC_MEMBER( glStencilFunc ) ;
NULL_STATIC_MEMBER( glStencilOp ) ;
NULL_STATIC_MEMBER( glDepthFunc ) ;
NULL_STATIC_MEMBER( glPixelStoref ) ;
NULL_STATIC_MEMBER( glPixelStorei ) ;
NULL_STATIC_MEMBER( glReadBuffer ) ;
NULL_STATIC_MEMBER( glReadPixels ) ;
NULL_STATIC_MEMBER( glGetBooleanv ) ;
NULL_STATIC_MEMBER( glGetDoublev ) ;
NULL_STATIC_MEMBER( glGetError ) ;
NULL_STATIC_MEMBER( glGetFloatv ) ;
NULL_STATIC_MEMBER( glGetIntegerv ) ;
NULL_STATIC_MEMBER( glGetString ) ;
NULL_STATIC_MEMBER( glGetTexImage ) ;
NULL_STATIC_MEMBER( glGetTexParameterfv ) ;
NULL_STATIC_MEMBER( glGetTexParameteriv ) ;
NULL_STATIC_MEMBER( glGetTexLevelParameterfv ) ;
NULL_STATIC_MEMBER( glGetTexLevelParameteriv ) ;
NULL_STATIC_MEMBER( glIsEnabled ) ;
NULL_STATIC_MEMBER( glDepthRange ) ;
NULL_STATIC_MEMBER( glViewport ) ;

// gl version 1.1
        
NULL_STATIC_MEMBER( glDrawArrays ) ;
NULL_STATIC_MEMBER( glDrawElements ) ;
NULL_STATIC_MEMBER( glGetPointerv ) ;
NULL_STATIC_MEMBER( glPolygonOffset ) ;
NULL_STATIC_MEMBER( glCopyTexImage1D ) ;
NULL_STATIC_MEMBER( glCopyTexImage2D ) ;
NULL_STATIC_MEMBER( glCopyTexSubImage1D ) ;
NULL_STATIC_MEMBER( glCopyTexSubImage2D ) ;
NULL_STATIC_MEMBER( glTexSubImage1D ) ;
NULL_STATIC_MEMBER( glTexSubImage2D ) ;
NULL_STATIC_MEMBER( glBindTexture ) ;
NULL_STATIC_MEMBER( glDeleteTextures ) ;
NULL_STATIC_MEMBER( glGenTextures ) ;
NULL_STATIC_MEMBER( glIsTexture ) ;


// gl version 1.2

NULL_STATIC_MEMBER( glDrawRangeElements ) ;
NULL_STATIC_MEMBER( glTexImage3D ) ;
NULL_STATIC_MEMBER( glTexSubImage3D ) ;
NULL_STATIC_MEMBER( glCopyTexSubImage3D ) ;

// gl version 1.3

NULL_STATIC_MEMBER( glActiveTexture ) ;
NULL_STATIC_MEMBER( glSampleCoverage ) ;
NULL_STATIC_MEMBER( glCompressedTexImage3D ) ;
NULL_STATIC_MEMBER( glCompressedTexImage2D ) ;
NULL_STATIC_MEMBER( glCompressedTexImage1D ) ;
NULL_STATIC_MEMBER( glCompressedTexSubImage3D ) ;
NULL_STATIC_MEMBER( glCompressedTexSubImage2D ) ;
NULL_STATIC_MEMBER( glCompressedTexSubImage1D ) ;
NULL_STATIC_MEMBER( glGetCompressedTexImage ) ;

// gl version 1.4

NULL_STATIC_MEMBER( glBlendFuncSeparate ) ;
NULL_STATIC_MEMBER( glMultiDrawArrays ) ;
NULL_STATIC_MEMBER( glMultiDrawElements ) ;
NULL_STATIC_MEMBER( glPointParameterf ) ;
NULL_STATIC_MEMBER( glPointParameterfv ) ;
NULL_STATIC_MEMBER( glPointParameteri ) ;
NULL_STATIC_MEMBER( glPointParameteriv ) ;
NULL_STATIC_MEMBER( glBlendColor ) ;
NULL_STATIC_MEMBER( glBlendEquation ) ;

// gl version 1.5

NULL_STATIC_MEMBER( glGenQueries)  ;
NULL_STATIC_MEMBER( glDeleteQueries ) ;
NULL_STATIC_MEMBER( glIsQuery ) ;
NULL_STATIC_MEMBER( glBeginQuery ) ;
NULL_STATIC_MEMBER( glEndQuery ) ;
NULL_STATIC_MEMBER( glGetQueryiv ) ;
NULL_STATIC_MEMBER( glGetQueryObjectiv ) ;
NULL_STATIC_MEMBER( glGetQueryObjectuiv ) ;
NULL_STATIC_MEMBER( glBindBuffer ) ;
NULL_STATIC_MEMBER( glDeleteBuffers ) ;
NULL_STATIC_MEMBER( glGenBuffers ) ;
NULL_STATIC_MEMBER( glIsBuffer ) ;
NULL_STATIC_MEMBER( glBufferData ) ;
NULL_STATIC_MEMBER( glBufferSubData ) ;
NULL_STATIC_MEMBER( glGetBufferSubData ) ;
NULL_STATIC_MEMBER( glMapBuffer ) ;
NULL_STATIC_MEMBER( glUnmapBuffer ) ;
NULL_STATIC_MEMBER( glGetBufferParameteriv ) ;
NULL_STATIC_MEMBER( glGetBufferPointerv ) ;

// gl version 2.0

NULL_STATIC_MEMBER( glBlendEquationSeparate)  ;
NULL_STATIC_MEMBER( glDrawBuffers ) ;
NULL_STATIC_MEMBER( glStencilOpSeparate ) ;
NULL_STATIC_MEMBER( glStencilFuncSeparate ) ;
NULL_STATIC_MEMBER( glStencilMaskSeparate ) ;
NULL_STATIC_MEMBER( glAttachShader ) ;
NULL_STATIC_MEMBER( glBindAttribLocation ) ;
NULL_STATIC_MEMBER( glCompileShader ) ;
NULL_STATIC_MEMBER( glCreateProgram ) ;
NULL_STATIC_MEMBER( glCreateShader ) ;
NULL_STATIC_MEMBER( glDeleteProgram ) ;
NULL_STATIC_MEMBER( glDeleteShader ) ;
NULL_STATIC_MEMBER( glDetachShader ) ;
NULL_STATIC_MEMBER( glDisableVertexAttribArray ) ;
NULL_STATIC_MEMBER( glEnableVertexAttribArray ) ;
NULL_STATIC_MEMBER( glGetActiveAttrib ) ;
NULL_STATIC_MEMBER( glGetActiveUniform ) ;
NULL_STATIC_MEMBER( glGetAttachedShaders ) ; 
NULL_STATIC_MEMBER( glGetAttribLocation ) ;
NULL_STATIC_MEMBER( glGetProgramiv ) ;
NULL_STATIC_MEMBER( glGetProgramInfoLog ) ;
NULL_STATIC_MEMBER( glGetShaderiv ) ;
NULL_STATIC_MEMBER( glGetShaderInfoLog ) ;
NULL_STATIC_MEMBER( glGetShaderSource ) ;
NULL_STATIC_MEMBER( glGetUniformLocation ) ;
NULL_STATIC_MEMBER( glGetUniformfv ) ;
NULL_STATIC_MEMBER( glGetUniformiv ) ;
NULL_STATIC_MEMBER( glGetVertexAttribdv ) ;
NULL_STATIC_MEMBER( glGetVertexAttribfv ) ;
NULL_STATIC_MEMBER( glGetVertexAttribiv ) ;
NULL_STATIC_MEMBER( glGetVertexAttribPointerv ) ;
NULL_STATIC_MEMBER( glIsProgram ) ;
NULL_STATIC_MEMBER( glIsShader ) ;
NULL_STATIC_MEMBER( glLinkProgram ) ;
NULL_STATIC_MEMBER( glShaderSource ) ;
NULL_STATIC_MEMBER( glUseProgram ) ;
NULL_STATIC_MEMBER( glUniform1f ) ;
NULL_STATIC_MEMBER( glUniform2f ) ;
NULL_STATIC_MEMBER( glUniform3f ) ;
NULL_STATIC_MEMBER( glUniform4f ) ;
NULL_STATIC_MEMBER( glUniform1i ) ;
NULL_STATIC_MEMBER( glUniform2i ) ;
NULL_STATIC_MEMBER( glUniform3i ) ;
NULL_STATIC_MEMBER( glUniform4i ) ;
NULL_STATIC_MEMBER( glUniform1fv ) ;
NULL_STATIC_MEMBER( glUniform2fv ) ;
NULL_STATIC_MEMBER( glUniform3fv ) ;
NULL_STATIC_MEMBER( glUniform4fv ) ;
NULL_STATIC_MEMBER( glUniform1iv ) ;
NULL_STATIC_MEMBER( glUniform2iv ) ;
NULL_STATIC_MEMBER( glUniform3iv ) ;
NULL_STATIC_MEMBER( glUniform4iv ) ;
NULL_STATIC_MEMBER( glUniformMatrix2fv ) ;
NULL_STATIC_MEMBER( glUniformMatrix3fv ) ;
NULL_STATIC_MEMBER( glUniformMatrix4fv ) ;
NULL_STATIC_MEMBER( glValidateProgram ) ;
NULL_STATIC_MEMBER( glVertexAttrib1d ) ;
NULL_STATIC_MEMBER( glVertexAttrib1dv ) ;
NULL_STATIC_MEMBER( glVertexAttrib1f ) ;
NULL_STATIC_MEMBER( glVertexAttrib1fv ) ;
NULL_STATIC_MEMBER( glVertexAttrib1s ) ;
NULL_STATIC_MEMBER( glVertexAttrib1sv ) ;
NULL_STATIC_MEMBER( glVertexAttrib2d ) ;
NULL_STATIC_MEMBER( glVertexAttrib2dv ) ;
NULL_STATIC_MEMBER( glVertexAttrib2f ) ;
NULL_STATIC_MEMBER( glVertexAttrib2fv ) ;
NULL_STATIC_MEMBER( glVertexAttrib2s ) ;
NULL_STATIC_MEMBER( glVertexAttrib2sv ) ;
NULL_STATIC_MEMBER( glVertexAttrib3d ) ;
NULL_STATIC_MEMBER( glVertexAttrib3dv ) ;
NULL_STATIC_MEMBER( glVertexAttrib3f ) ;
NULL_STATIC_MEMBER( glVertexAttrib3fv ) ;
NULL_STATIC_MEMBER( glVertexAttrib3s ) ;
NULL_STATIC_MEMBER( glVertexAttrib3sv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4Nbv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4Niv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4Nsv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4Nub ) ;
NULL_STATIC_MEMBER( glVertexAttrib4Nubv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4Nuiv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4Nusv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4bv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4d ) ;
NULL_STATIC_MEMBER( glVertexAttrib4dv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4f ) ;
NULL_STATIC_MEMBER( glVertexAttrib4fv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4iv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4s ) ;
NULL_STATIC_MEMBER( glVertexAttrib4sv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4ubv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4uiv ) ;
NULL_STATIC_MEMBER( glVertexAttrib4usv ) ;
NULL_STATIC_MEMBER( glVertexAttribPointer ) ;

// gl version 2.1

NULL_STATIC_MEMBER( glUniformMatrix2x3fv ) ;
NULL_STATIC_MEMBER( glUniformMatrix3x2fv ) ;
NULL_STATIC_MEMBER( glUniformMatrix2x4fv ) ;
NULL_STATIC_MEMBER( glUniformMatrix4x2fv ) ;
NULL_STATIC_MEMBER( glUniformMatrix3x4fv ) ;
NULL_STATIC_MEMBER( glUniformMatrix4x3fv ) ;

// gl version 3.0

NULL_STATIC_MEMBER( glColorMaski ) ;
NULL_STATIC_MEMBER( glGetBooleani_v ) ;
NULL_STATIC_MEMBER( glGetIntegeri_v ) ;
NULL_STATIC_MEMBER( glEnablei ) ;
NULL_STATIC_MEMBER( glDisablei ) ;
NULL_STATIC_MEMBER( glIsEnabledi ) ;
NULL_STATIC_MEMBER( glBeginTransformFeedback ) ;
NULL_STATIC_MEMBER( glEndTransformFeedback ) ; 
NULL_STATIC_MEMBER( glBindBufferRange ) ;
NULL_STATIC_MEMBER( glBindBufferBase ) ;
NULL_STATIC_MEMBER( glTransformFeedbackVaryings ) ;
NULL_STATIC_MEMBER( glGetTransformFeedbackVarying ) ;
NULL_STATIC_MEMBER( glClampColor ) ;
NULL_STATIC_MEMBER( glBeginConditionalRender ) ;
NULL_STATIC_MEMBER( glEndConditionalRender ) ;
NULL_STATIC_MEMBER( glVertexAttribIPointer ) ;
NULL_STATIC_MEMBER( glGetVertexAttribIiv ) ;
NULL_STATIC_MEMBER( glGetVertexAttribIuiv ) ;
NULL_STATIC_MEMBER( glVertexAttribI1i ) ;
NULL_STATIC_MEMBER( glVertexAttribI2i ) ;
NULL_STATIC_MEMBER( glVertexAttribI3i ) ;
NULL_STATIC_MEMBER( glVertexAttribI4i ) ;
NULL_STATIC_MEMBER( glVertexAttribI1ui ) ;
NULL_STATIC_MEMBER( glVertexAttribI2ui ) ;
NULL_STATIC_MEMBER( glVertexAttribI3ui ) ;
NULL_STATIC_MEMBER( glVertexAttribI4ui ) ;
NULL_STATIC_MEMBER( glVertexAttribI1iv ) ;
NULL_STATIC_MEMBER( glVertexAttribI2iv ) ;
NULL_STATIC_MEMBER( glVertexAttribI3iv ) ;
NULL_STATIC_MEMBER( glVertexAttribI4iv ) ;
NULL_STATIC_MEMBER( glVertexAttribI1uiv ) ;
NULL_STATIC_MEMBER( glVertexAttribI2uiv ) ;
NULL_STATIC_MEMBER( glVertexAttribI3uiv ) ;
NULL_STATIC_MEMBER( glVertexAttribI4uiv ) ;
NULL_STATIC_MEMBER( glVertexAttribI4bv ) ;
NULL_STATIC_MEMBER( glVertexAttribI4sv ) ;
NULL_STATIC_MEMBER( glVertexAttribI4ubv ) ;
NULL_STATIC_MEMBER( glVertexAttribI4usv ) ;
NULL_STATIC_MEMBER( glGetUniformuiv ) ;
NULL_STATIC_MEMBER( glBindFragDataLocation ) ;
NULL_STATIC_MEMBER( glGetFragDataLocation ) ;
NULL_STATIC_MEMBER( glUniform1ui ) ;
NULL_STATIC_MEMBER( glUniform2ui ) ;
NULL_STATIC_MEMBER( glUniform3ui ) ;
NULL_STATIC_MEMBER( glUniform4ui ) ;
NULL_STATIC_MEMBER( glUniform1uiv ) ;
NULL_STATIC_MEMBER( glUniform2uiv ) ;
NULL_STATIC_MEMBER( glUniform3uiv ) ;
NULL_STATIC_MEMBER( glUniform4uiv ) ;
NULL_STATIC_MEMBER( glTexParameterIiv ) ;
NULL_STATIC_MEMBER( glTexParameterIuiv ) ;
NULL_STATIC_MEMBER( glGetTexParameterIiv ) ;
NULL_STATIC_MEMBER( glGetTexParameterIuiv ) ;
NULL_STATIC_MEMBER( glClearBufferiv ) ;
NULL_STATIC_MEMBER( glClearBufferuiv ) ;
NULL_STATIC_MEMBER( glClearBufferfv ) ;
NULL_STATIC_MEMBER( glClearBufferfi ) ;
NULL_STATIC_MEMBER( glGetStringi ) ;
NULL_STATIC_MEMBER( glIsRenderbuffer ) ;
NULL_STATIC_MEMBER( glBindRenderbuffer ) ;
NULL_STATIC_MEMBER( glDeleteRenderbuffers ) ;
NULL_STATIC_MEMBER( glGenRenderbuffers ) ;
NULL_STATIC_MEMBER( glRenderbufferStorage ) ;
NULL_STATIC_MEMBER( glGetRenderbufferParameteriv ) ;
NULL_STATIC_MEMBER( glIsFramebuffer ) ;
NULL_STATIC_MEMBER( glBindFramebuffer ) ;
NULL_STATIC_MEMBER( glDeleteFramebuffers ) ;
NULL_STATIC_MEMBER( glGenFramebuffers ) ;
NULL_STATIC_MEMBER( glCheckFramebufferStatus ) ;
NULL_STATIC_MEMBER( glFramebufferTexture1D ) ;
NULL_STATIC_MEMBER( glFramebufferTexture2D ) ;
NULL_STATIC_MEMBER( glFramebufferTexture3D ) ;
NULL_STATIC_MEMBER( glFramebufferRenderbuffer ) ;
NULL_STATIC_MEMBER( glGetFramebufferAttachmentParameteriv ) ;
NULL_STATIC_MEMBER( glGenerateMipmap ) ;
NULL_STATIC_MEMBER( glBlitFramebuffer ) ;
NULL_STATIC_MEMBER( glRenderbufferStorageMultisample ) ;
NULL_STATIC_MEMBER( glFramebufferTextureLayer ) ;
NULL_STATIC_MEMBER( glMapBufferRange ) ;
NULL_STATIC_MEMBER( glFlushMappedBufferRange ) ;
NULL_STATIC_MEMBER( glBindVertexArray ) ;
NULL_STATIC_MEMBER( glDeleteVertexArrays ) ;
NULL_STATIC_MEMBER( glGenVertexArrays ) ;
NULL_STATIC_MEMBER( glIsVertexArray ) ;
 
// gl version 3.1

NULL_STATIC_MEMBER( glDrawArraysInstanced ) ;
NULL_STATIC_MEMBER( glDrawElementsInstanced ) ;
NULL_STATIC_MEMBER( glTexBuffer ) ;
NULL_STATIC_MEMBER( glPrimitiveRestartIndex ) ;
NULL_STATIC_MEMBER( glCopyBufferSubData ) ;
NULL_STATIC_MEMBER( glGetUniformIndices ) ;
NULL_STATIC_MEMBER( glGetActiveUniformsiv ) ;
NULL_STATIC_MEMBER( glGetActiveUniformName ) ;
NULL_STATIC_MEMBER( glGetUniformBlockIndex ) ;
NULL_STATIC_MEMBER( glGetActiveUniformBlockiv ) ;
NULL_STATIC_MEMBER( glGetActiveUniformBlockName ) ;
NULL_STATIC_MEMBER( glUniformBlockBinding ) ;

// gl version 3.2

NULL_STATIC_MEMBER( glDrawElementsBaseVertex ) ;
NULL_STATIC_MEMBER( glDrawRangeElementsBaseVertex ) ;
NULL_STATIC_MEMBER( glDrawElementsInstancedBaseVertex ) ;
NULL_STATIC_MEMBER( glMultiDrawElementsBaseVertex ) ;
NULL_STATIC_MEMBER( glProvokingVertex ) ;
NULL_STATIC_MEMBER( glFenceSync ) ;
NULL_STATIC_MEMBER( glIsSync ) ;
NULL_STATIC_MEMBER( glDeleteSync ) ;
NULL_STATIC_MEMBER( glClientWaitSync ) ;
NULL_STATIC_MEMBER( glWaitSync ) ;
NULL_STATIC_MEMBER( glGetInteger64v ) ;
NULL_STATIC_MEMBER( glGetSynciv ) ;
NULL_STATIC_MEMBER( glGetInteger64i_v ) ;
NULL_STATIC_MEMBER( glGetBufferParameteri64v ) ;
NULL_STATIC_MEMBER( glFramebufferTexture ) ;
NULL_STATIC_MEMBER( glTexImage2DMultisample ) ;
NULL_STATIC_MEMBER( glTexImage3DMultisample ) ;
NULL_STATIC_MEMBER( glGetMultisamplefv ) ;
NULL_STATIC_MEMBER( glSampleMaski ) ;

// gl version 3.3

NULL_STATIC_MEMBER( glBindFragDataLocationIndexed ) ;
NULL_STATIC_MEMBER( glGetFragDataIndex ) ;
NULL_STATIC_MEMBER( glGenSamplers ) ;
NULL_STATIC_MEMBER( glDeleteSamplers ) ;
NULL_STATIC_MEMBER( glIsSampler ) ;
NULL_STATIC_MEMBER( glBindSampler ) ;
NULL_STATIC_MEMBER( glSamplerParameteri ) ;
NULL_STATIC_MEMBER( glSamplerParameteriv ) ;
NULL_STATIC_MEMBER( glSamplerParameterf ) ;
NULL_STATIC_MEMBER( glSamplerParameterfv ) ;
NULL_STATIC_MEMBER( glSamplerParameterIiv ) ;
NULL_STATIC_MEMBER( glSamplerParameterIuiv ) ;
NULL_STATIC_MEMBER( glGetSamplerParameteriv ) ;
NULL_STATIC_MEMBER( glGetSamplerParameterIiv ) ;
NULL_STATIC_MEMBER( glGetSamplerParameterfv ) ;
NULL_STATIC_MEMBER( glGetSamplerParameterIuiv ) ;
NULL_STATIC_MEMBER( glQueryCounter ) ;
NULL_STATIC_MEMBER( glGetQueryObjecti64v ) ;
NULL_STATIC_MEMBER( glGetQueryObjectui64v ) ;
NULL_STATIC_MEMBER( glVertexAttribDivisor ) ;
NULL_STATIC_MEMBER( glVertexAttribP1ui ) ;
NULL_STATIC_MEMBER( glVertexAttribP1uiv ) ;
NULL_STATIC_MEMBER( glVertexAttribP2ui ) ;
NULL_STATIC_MEMBER( glVertexAttribP2uiv ) ;
NULL_STATIC_MEMBER( glVertexAttribP3ui ) ;
NULL_STATIC_MEMBER( glVertexAttribP3uiv ) ;
NULL_STATIC_MEMBER( glVertexAttribP4ui ) ;
NULL_STATIC_MEMBER( glVertexAttribP4uiv ) ;

// gl version 4.0

NULL_STATIC_MEMBER( glMinSampleShading ) ;
NULL_STATIC_MEMBER( glBlendEquationi ) ;
NULL_STATIC_MEMBER( glBlendEquationSeparatei ) ;
NULL_STATIC_MEMBER( glBlendFunci ) ;
NULL_STATIC_MEMBER( glBlendFuncSeparatei ) ;
NULL_STATIC_MEMBER( glDrawArraysIndirect ) ;
NULL_STATIC_MEMBER( glDrawElementsIndirect ) ;
NULL_STATIC_MEMBER( glUniform1d ) ;
NULL_STATIC_MEMBER( glUniform2d ) ;
NULL_STATIC_MEMBER( glUniform3d ) ;
NULL_STATIC_MEMBER( glUniform4d ) ;
NULL_STATIC_MEMBER( glUniform1dv ) ;
NULL_STATIC_MEMBER( glUniform2dv ) ;
NULL_STATIC_MEMBER( glUniform3dv ) ;
NULL_STATIC_MEMBER( glUniform4dv ) ;
NULL_STATIC_MEMBER( glUniformMatrix2dv ) ;
NULL_STATIC_MEMBER( glUniformMatrix3dv ) ;
NULL_STATIC_MEMBER( glUniformMatrix4dv ) ;
NULL_STATIC_MEMBER( glUniformMatrix2x3dv ) ;
NULL_STATIC_MEMBER( glUniformMatrix2x4dv ) ;
NULL_STATIC_MEMBER( glUniformMatrix3x2dv ) ;
NULL_STATIC_MEMBER( glUniformMatrix3x4dv ) ;
NULL_STATIC_MEMBER( glUniformMatrix4x2dv ) ;
NULL_STATIC_MEMBER( glUniformMatrix4x3dv ) ;
NULL_STATIC_MEMBER( glGetUniformdv ) ;
NULL_STATIC_MEMBER( glGetSubroutineUniformLocation ) ;
NULL_STATIC_MEMBER( glGetSubroutineIndex ) ;
NULL_STATIC_MEMBER( glGetActiveSubroutineUniformiv ) ;
NULL_STATIC_MEMBER( glGetActiveSubroutineUniformName ) ;
NULL_STATIC_MEMBER( glGetActiveSubroutineName ) ;
NULL_STATIC_MEMBER( glUniformSubroutinesuiv ) ;
NULL_STATIC_MEMBER( glGetUniformSubroutineuiv ) ;
NULL_STATIC_MEMBER( glGetProgramStageiv ) ;
NULL_STATIC_MEMBER( glPatchParameteri ) ;
NULL_STATIC_MEMBER( glPatchParameterfv ) ;
NULL_STATIC_MEMBER( glBindTransformFeedback ) ;
NULL_STATIC_MEMBER( glDeleteTransformFeedbacks ) ;
NULL_STATIC_MEMBER( glGenTransformFeedbacks ) ;
NULL_STATIC_MEMBER( glIsTransformFeedback ) ;
NULL_STATIC_MEMBER( glPauseTransformFeedback ) ;
NULL_STATIC_MEMBER( glResumeTransformFeedback ) ;
NULL_STATIC_MEMBER( glDrawTransformFeedback ) ;
NULL_STATIC_MEMBER( glDrawTransformFeedbackStream ) ;
NULL_STATIC_MEMBER( glBeginQueryIndexed ) ;
NULL_STATIC_MEMBER( glEndQueryIndexed ) ;
NULL_STATIC_MEMBER( glGetQueryIndexediv ) ;
 
// gl version 4.1
         
NULL_STATIC_MEMBER( glReleaseShaderCompiler ) ;
NULL_STATIC_MEMBER( glShaderBinary ) ;
NULL_STATIC_MEMBER( glGetShaderPrecisionFormat ) ;
NULL_STATIC_MEMBER( glDepthRangef ) ;
NULL_STATIC_MEMBER( glClearDepthf ) ;
NULL_STATIC_MEMBER( glGetProgramBinary ) ;
NULL_STATIC_MEMBER( glProgramBinary ) ;
NULL_STATIC_MEMBER( glProgramParameteri ) ;
NULL_STATIC_MEMBER( glUseProgramStages ) ;
NULL_STATIC_MEMBER( glActiveShaderProgram ) ;
NULL_STATIC_MEMBER( glCreateShaderProgramv ) ;
NULL_STATIC_MEMBER( glBindProgramPipeline ) ;
NULL_STATIC_MEMBER( glDeleteProgramPipelines ) ;
NULL_STATIC_MEMBER( glGenProgramPipelines ) ;
NULL_STATIC_MEMBER( glIsProgramPipeline ) ;
NULL_STATIC_MEMBER( glGetProgramPipelineiv ) ;
NULL_STATIC_MEMBER( glProgramUniform1i ) ;
NULL_STATIC_MEMBER( glProgramUniform1iv ) ;
NULL_STATIC_MEMBER( glProgramUniform1f ) ;
NULL_STATIC_MEMBER( glProgramUniform1fv ) ;
NULL_STATIC_MEMBER( glProgramUniform1d ) ;
NULL_STATIC_MEMBER( glProgramUniform1dv ) ;
NULL_STATIC_MEMBER( glProgramUniform1ui ) ;
NULL_STATIC_MEMBER( glProgramUniform1uiv ) ;
NULL_STATIC_MEMBER( glProgramUniform2i ) ;
NULL_STATIC_MEMBER( glProgramUniform2iv ) ;
NULL_STATIC_MEMBER( glProgramUniform2f ) ;
NULL_STATIC_MEMBER( glProgramUniform2fv ) ;
NULL_STATIC_MEMBER( glProgramUniform2d ) ;
NULL_STATIC_MEMBER( glProgramUniform2dv ) ;
NULL_STATIC_MEMBER( glProgramUniform2ui ) ;
NULL_STATIC_MEMBER( glProgramUniform2uiv ) ;
NULL_STATIC_MEMBER( glProgramUniform3i ) ;
NULL_STATIC_MEMBER( glProgramUniform3iv ) ;
NULL_STATIC_MEMBER( glProgramUniform3f ) ;
NULL_STATIC_MEMBER( glProgramUniform3fv ) ;
NULL_STATIC_MEMBER( glProgramUniform3d ) ;
NULL_STATIC_MEMBER( glProgramUniform3dv ) ;
NULL_STATIC_MEMBER( glProgramUniform3ui ) ;
NULL_STATIC_MEMBER( glProgramUniform3uiv ) ;
NULL_STATIC_MEMBER( glProgramUniform4i ) ;
NULL_STATIC_MEMBER( glProgramUniform4iv ) ;
NULL_STATIC_MEMBER( glProgramUniform4f ) ;
NULL_STATIC_MEMBER( glProgramUniform4fv ) ;
NULL_STATIC_MEMBER( glProgramUniform4d ) ;
NULL_STATIC_MEMBER( glProgramUniform4dv ) ;
NULL_STATIC_MEMBER( glProgramUniform4ui ) ;
NULL_STATIC_MEMBER( glProgramUniform4uiv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix2fv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix3fv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix4fv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix2dv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix3dv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix4dv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix2x3fv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix3x2fv ) ; 
NULL_STATIC_MEMBER( glProgramUniformMatrix2x4fv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix4x2fv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix3x4fv ) ; 
NULL_STATIC_MEMBER( glProgramUniformMatrix4x3fv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix2x3dv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix3x2dv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix2x4dv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix4x2dv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix3x4dv ) ;
NULL_STATIC_MEMBER( glProgramUniformMatrix4x3dv ) ;
NULL_STATIC_MEMBER( glValidateProgramPipeline ) ;
NULL_STATIC_MEMBER( glGetProgramPipelineInfoLog ) ; 
NULL_STATIC_MEMBER( glVertexAttribL1d  ) ;
NULL_STATIC_MEMBER( glVertexAttribL2d ) ;
NULL_STATIC_MEMBER( glVertexAttribL3d ) ;
NULL_STATIC_MEMBER( glVertexAttribL4d ) ;
NULL_STATIC_MEMBER( glVertexAttribL1dv ) ;
NULL_STATIC_MEMBER( glVertexAttribL2dv ) ;
NULL_STATIC_MEMBER( glVertexAttribL3dv ) ;
NULL_STATIC_MEMBER( glVertexAttribL4dv ) ;
NULL_STATIC_MEMBER( glVertexAttribLPointer ) ;
NULL_STATIC_MEMBER( glGetVertexAttribLdv ) ;
NULL_STATIC_MEMBER( glViewportArrayv ) ;
NULL_STATIC_MEMBER( glViewportIndexedf ) ;
NULL_STATIC_MEMBER( glViewportIndexedfv ) ;
NULL_STATIC_MEMBER( glScissorArrayv ) ;
NULL_STATIC_MEMBER( glScissorIndexed ) ;
NULL_STATIC_MEMBER( glScissorIndexedv ) ;
NULL_STATIC_MEMBER( glDepthRangeArrayv ) ;
NULL_STATIC_MEMBER( glDepthRangeIndexed ) ;
NULL_STATIC_MEMBER( glGetFloati_v ) ;
NULL_STATIC_MEMBER( glGetDoublei_v ) ;

/////////////////////////////////////////////////////////////////////////
// Function definitions
/////////////////////////////////////////////////////////////////////////

//**************************************************************
void_ptr_t natus::gl::bindings::load_gl_function( char_cptr_t name ) 
{
#if defined( NATUS_TARGET_OS_WIN )
    void *p = (void *)wglGetProcAddress(name);
    if (p == 0 ||
        (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1))
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        p = (void *)GetProcAddress(module, name);
    }
#elif defined( NATUS_TARGET_OS_LIN )
    void_ptr_t p = (void_ptr_t) glXGetProcAddress( (GLubyte const *) name ) ;
#else
#error "Requires implementation"
#endif
    return p;
}

//**************************************************************
bool_t natus::gl::bindings::is_supported( char const * name ) 
{
    auto const iter = ::std::find( _extensions.begin(), _extensions.end(), name ) ;
    return iter != _extensions.end() ;
}

//**************************************************************
natus::gl::result natus::gl::bindings::init( void_t ) 
{
    if( _init ) return natus::gl::result::ok ;

    if( !CHECK_AND_LOAD_COND( glGetIntegerv, "glGetIntegerv" ) )
    {
        return natus::gl::result::failed_load_function ;
    }

    if( !CHECK_AND_LOAD_COND( glGetStringi, "glGetStringi" ) )
    {

    }

    // load all supported extension strings
    {
        GLint numext = 0 ;
        this_t::glGetIntegerv( GL_NUM_EXTENSIONS, &numext  ) ;

        for( GLint i=0; i<numext; ++i )
        {
            GLubyte const * name = this_t::glGetStringi( GL_EXTENSIONS, i  ) ;
            _extensions.push_back( std::string((char const *)name) ) ;
        }
    }

    CHECK_AND_LOAD_BY_FN( glCullFace ) 
    CHECK_AND_LOAD_BY_FN( glFrontFace ) 
    CHECK_AND_LOAD_BY_FN( glHint ) 
    CHECK_AND_LOAD_BY_FN( glLineWidth) 
    CHECK_AND_LOAD_BY_FN( glPointSize ) 
    CHECK_AND_LOAD_BY_FN( glPolygonMode ) 
    CHECK_AND_LOAD_BY_FN( glScissor ) 
    CHECK_AND_LOAD_BY_FN( glTexParameterf ) 
    CHECK_AND_LOAD_BY_FN( glTexParameterfv ) 
    CHECK_AND_LOAD_BY_FN( glTexParameteri ) 
    CHECK_AND_LOAD_BY_FN( glTexParameteriv ) 
    CHECK_AND_LOAD_BY_FN( glTexImage1D ) 
    CHECK_AND_LOAD_BY_FN( glTexImage2D ) 
    CHECK_AND_LOAD_BY_FN( glDrawBuffer ) 
    CHECK_AND_LOAD_BY_FN( glClear ) 
    CHECK_AND_LOAD_BY_FN( glClearColor ) 
    CHECK_AND_LOAD_BY_FN( glClearStencil ) 
    CHECK_AND_LOAD_BY_FN( glClearDepth ) 
    CHECK_AND_LOAD_BY_FN( glStencilMask ) 
    CHECK_AND_LOAD_BY_FN( glColorMask ) 
    CHECK_AND_LOAD_BY_FN( glDepthMask ) 
    CHECK_AND_LOAD_BY_FN( glDisable ) 
    CHECK_AND_LOAD_BY_FN( glEnable ) 
    CHECK_AND_LOAD_BY_FN( glFinish ) 
    CHECK_AND_LOAD_BY_FN( glFlush ) 
    CHECK_AND_LOAD_BY_FN( glBlendFunc ) 
    CHECK_AND_LOAD_BY_FN( glLogicOp ) 
    CHECK_AND_LOAD_BY_FN( glStencilFunc ) 
    CHECK_AND_LOAD_BY_FN( glStencilOp ) 
    CHECK_AND_LOAD_BY_FN( glDepthFunc ) 
    CHECK_AND_LOAD_BY_FN( glPixelStoref ) 
    CHECK_AND_LOAD_BY_FN( glPixelStorei ) 
    CHECK_AND_LOAD_BY_FN( glReadBuffer ) 
    CHECK_AND_LOAD_BY_FN( glReadPixels ) 
    CHECK_AND_LOAD_BY_FN( glGetBooleanv ) 
    CHECK_AND_LOAD_BY_FN( glGetDoublev ) 
    CHECK_AND_LOAD_BY_FN( glGetError ) 
    CHECK_AND_LOAD_BY_FN( glGetFloatv ) 
    CHECK_AND_LOAD_BY_FN( glGetIntegerv ) 
    CHECK_AND_LOAD_BY_FN( glGetString ) 
    CHECK_AND_LOAD_BY_FN( glGetTexImage ) 
    CHECK_AND_LOAD_BY_FN( glGetTexParameterfv ) 
    CHECK_AND_LOAD_BY_FN( glGetTexParameteriv ) 
    CHECK_AND_LOAD_BY_FN( glGetTexLevelParameterfv ) 
    CHECK_AND_LOAD_BY_FN( glGetTexLevelParameteriv ) 
    CHECK_AND_LOAD_BY_FN( glIsEnabled ) 
    CHECK_AND_LOAD_BY_FN( glDepthRange ) 
    CHECK_AND_LOAD_BY_FN( glViewport ) 

    // gl version 1.1
        
    CHECK_AND_LOAD_BY_FN( glDrawArrays ) 
    CHECK_AND_LOAD_BY_FN( glDrawElements ) 
    CHECK_AND_LOAD_BY_FN( glGetPointerv ) 
    CHECK_AND_LOAD_BY_FN( glPolygonOffset ) 
    CHECK_AND_LOAD_BY_FN( glCopyTexImage1D ) 
    CHECK_AND_LOAD_BY_FN( glCopyTexImage2D ) 
    CHECK_AND_LOAD_BY_FN( glCopyTexSubImage1D ) 
    CHECK_AND_LOAD_BY_FN( glCopyTexSubImage2D ) 
    CHECK_AND_LOAD_BY_FN( glTexSubImage1D ) 
    CHECK_AND_LOAD_BY_FN( glTexSubImage2D ) 
    CHECK_AND_LOAD_BY_FN( glBindTexture ) 
    CHECK_AND_LOAD_BY_FN( glDeleteTextures ) 
    CHECK_AND_LOAD_BY_FN( glGenTextures ) 
    CHECK_AND_LOAD_BY_FN( glIsTexture ) 


    // gl version 1.2

    CHECK_AND_LOAD_BY_FN( glDrawRangeElements ) 
    CHECK_AND_LOAD_BY_FN( glTexImage3D ) 
    CHECK_AND_LOAD_BY_FN( glTexSubImage3D ) 
    CHECK_AND_LOAD_BY_FN( glCopyTexSubImage3D ) 

    // gl version 1.3

    CHECK_AND_LOAD_BY_FN( glActiveTexture ) 
    CHECK_AND_LOAD_BY_FN( glSampleCoverage ) 
    CHECK_AND_LOAD_BY_FN( glCompressedTexImage3D ) 
    CHECK_AND_LOAD_BY_FN( glCompressedTexImage2D ) 
    CHECK_AND_LOAD_BY_FN( glCompressedTexImage1D ) 
    CHECK_AND_LOAD_BY_FN( glCompressedTexSubImage3D ) 
    CHECK_AND_LOAD_BY_FN( glCompressedTexSubImage2D ) 
    CHECK_AND_LOAD_BY_FN( glCompressedTexSubImage1D ) 
    CHECK_AND_LOAD_BY_FN( glGetCompressedTexImage ) 

    // gl version 1.4

    CHECK_AND_LOAD_BY_FN( glBlendFuncSeparate ) 
    CHECK_AND_LOAD_BY_FN( glMultiDrawArrays ) 
    CHECK_AND_LOAD_BY_FN( glMultiDrawElements ) 
    CHECK_AND_LOAD_BY_FN( glPointParameterf ) 
    CHECK_AND_LOAD_BY_FN( glPointParameterfv ) 
    CHECK_AND_LOAD_BY_FN( glPointParameteri ) 
    CHECK_AND_LOAD_BY_FN( glPointParameteriv ) 
    CHECK_AND_LOAD_BY_FN( glBlendColor ) 
    CHECK_AND_LOAD_BY_FN( glBlendEquation ) 

    // gl version 1.5

    CHECK_AND_LOAD_BY_FN( glGenQueries)  
    CHECK_AND_LOAD_BY_FN( glDeleteQueries ) 
    CHECK_AND_LOAD_BY_FN( glIsQuery ) 
    CHECK_AND_LOAD_BY_FN( glBeginQuery ) 
    CHECK_AND_LOAD_BY_FN( glEndQuery ) 
    CHECK_AND_LOAD_BY_FN( glGetQueryiv ) 
    CHECK_AND_LOAD_BY_FN( glGetQueryObjectiv ) 
    CHECK_AND_LOAD_BY_FN( glGetQueryObjectuiv ) 
    CHECK_AND_LOAD_BY_FN( glBindBuffer ) 
    CHECK_AND_LOAD_BY_FN( glDeleteBuffers ) 
    CHECK_AND_LOAD_BY_FN( glGenBuffers ) 
    CHECK_AND_LOAD_BY_FN( glIsBuffer ) 
    CHECK_AND_LOAD_BY_FN( glBufferData ) 
    CHECK_AND_LOAD_BY_FN( glBufferSubData ) 
    CHECK_AND_LOAD_BY_FN( glGetBufferSubData ) 
    CHECK_AND_LOAD_BY_FN( glMapBuffer ) 
    CHECK_AND_LOAD_BY_FN( glUnmapBuffer ) 
    CHECK_AND_LOAD_BY_FN( glGetBufferParameteriv ) 
    CHECK_AND_LOAD_BY_FN( glGetBufferPointerv ) 

    // gl version 2.0

    CHECK_AND_LOAD_BY_FN( glBlendEquationSeparate)  
    CHECK_AND_LOAD_BY_FN( glDrawBuffers ) 
    CHECK_AND_LOAD_BY_FN( glStencilOpSeparate ) 
    CHECK_AND_LOAD_BY_FN( glStencilFuncSeparate ) 
    CHECK_AND_LOAD_BY_FN( glStencilMaskSeparate ) 
    CHECK_AND_LOAD_BY_FN( glAttachShader ) 
    CHECK_AND_LOAD_BY_FN( glBindAttribLocation ) 
    CHECK_AND_LOAD_BY_FN( glCompileShader ) 
    CHECK_AND_LOAD_BY_FN( glCreateProgram ) 
    CHECK_AND_LOAD_BY_FN( glCreateShader ) 
    CHECK_AND_LOAD_BY_FN( glDeleteProgram ) 
    CHECK_AND_LOAD_BY_FN( glDeleteShader ) 
    CHECK_AND_LOAD_BY_FN( glDetachShader ) 
    CHECK_AND_LOAD_BY_FN( glDisableVertexAttribArray ) 
    CHECK_AND_LOAD_BY_FN( glEnableVertexAttribArray ) 
    CHECK_AND_LOAD_BY_FN( glGetActiveAttrib ) 
    CHECK_AND_LOAD_BY_FN( glGetActiveUniform ) 
    CHECK_AND_LOAD_BY_FN( glGetAttachedShaders )  
    CHECK_AND_LOAD_BY_FN( glGetAttribLocation ) 
    CHECK_AND_LOAD_BY_FN( glGetProgramiv ) 
    CHECK_AND_LOAD_BY_FN( glGetProgramInfoLog ) 
    CHECK_AND_LOAD_BY_FN( glGetShaderiv ) 
    CHECK_AND_LOAD_BY_FN( glGetShaderInfoLog ) 
    CHECK_AND_LOAD_BY_FN( glGetShaderSource ) 
    CHECK_AND_LOAD_BY_FN( glGetUniformLocation ) 
    CHECK_AND_LOAD_BY_FN( glGetUniformfv ) 
    CHECK_AND_LOAD_BY_FN( glGetUniformiv ) 
    CHECK_AND_LOAD_BY_FN( glGetVertexAttribdv ) 
    CHECK_AND_LOAD_BY_FN( glGetVertexAttribfv ) 
    CHECK_AND_LOAD_BY_FN( glGetVertexAttribiv ) 
    CHECK_AND_LOAD_BY_FN( glGetVertexAttribPointerv ) 
    CHECK_AND_LOAD_BY_FN( glIsProgram ) 
    CHECK_AND_LOAD_BY_FN( glIsShader ) 
    CHECK_AND_LOAD_BY_FN( glLinkProgram ) 
    CHECK_AND_LOAD_BY_FN( glShaderSource ) 
    CHECK_AND_LOAD_BY_FN( glUseProgram ) 
    CHECK_AND_LOAD_BY_FN( glUniform1f ) 
    CHECK_AND_LOAD_BY_FN( glUniform2f ) 
    CHECK_AND_LOAD_BY_FN( glUniform3f ) 
    CHECK_AND_LOAD_BY_FN( glUniform4f ) 
    CHECK_AND_LOAD_BY_FN( glUniform1i ) 
    CHECK_AND_LOAD_BY_FN( glUniform2i ) 
    CHECK_AND_LOAD_BY_FN( glUniform3i ) 
    CHECK_AND_LOAD_BY_FN( glUniform4i ) 
    CHECK_AND_LOAD_BY_FN( glUniform1fv ) 
    CHECK_AND_LOAD_BY_FN( glUniform2fv ) 
    CHECK_AND_LOAD_BY_FN( glUniform3fv ) 
    CHECK_AND_LOAD_BY_FN( glUniform4fv ) 
    CHECK_AND_LOAD_BY_FN( glUniform1iv ) 
    CHECK_AND_LOAD_BY_FN( glUniform2iv ) 
    CHECK_AND_LOAD_BY_FN( glUniform3iv ) 
    CHECK_AND_LOAD_BY_FN( glUniform4iv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix2fv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix3fv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix4fv ) 
    CHECK_AND_LOAD_BY_FN( glValidateProgram ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib1d ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib1dv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib1f ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib1fv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib1s ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib1sv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib2d ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib2dv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib2f ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib2fv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib2s ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib2sv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib3d ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib3dv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib3f ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib3fv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib3s ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib3sv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4Nbv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4Niv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4Nsv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4Nub ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4Nubv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4Nuiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4Nusv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4bv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4d ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4dv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4f ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4fv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4iv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4s ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4sv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4ubv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4uiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttrib4usv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribPointer ) 

    // gl version 2.1

    CHECK_AND_LOAD_BY_FN( glUniformMatrix2x3fv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix3x2fv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix2x4fv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix4x2fv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix3x4fv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix4x3fv ) 

    // gl version 3.0

    CHECK_AND_LOAD_BY_FN( glColorMaski ) 
    CHECK_AND_LOAD_BY_FN( glGetBooleani_v ) 
    CHECK_AND_LOAD_BY_FN( glGetIntegeri_v ) 
    CHECK_AND_LOAD_BY_FN( glEnablei ) 
    CHECK_AND_LOAD_BY_FN( glDisablei ) 
    CHECK_AND_LOAD_BY_FN( glIsEnabledi ) 
    CHECK_AND_LOAD_BY_FN( glBeginTransformFeedback ) 
    CHECK_AND_LOAD_BY_FN( glEndTransformFeedback )  
    CHECK_AND_LOAD_BY_FN( glBindBufferRange ) 
    CHECK_AND_LOAD_BY_FN( glBindBufferBase ) 
    CHECK_AND_LOAD_BY_FN( glTransformFeedbackVaryings ) 
    CHECK_AND_LOAD_BY_FN( glGetTransformFeedbackVarying ) 
    CHECK_AND_LOAD_BY_FN( glClampColor ) 
    CHECK_AND_LOAD_BY_FN( glBeginConditionalRender ) 
    CHECK_AND_LOAD_BY_FN( glEndConditionalRender ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribIPointer ) 
    CHECK_AND_LOAD_BY_FN( glGetVertexAttribIiv ) 
    CHECK_AND_LOAD_BY_FN( glGetVertexAttribIuiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI1i ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI2i ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI3i ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI4i ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI1ui ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI2ui ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI3ui ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI4ui ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI1iv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI2iv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI3iv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI4iv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI1uiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI2uiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI3uiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI4uiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI4bv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI4sv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI4ubv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribI4usv ) 
    CHECK_AND_LOAD_BY_FN( glGetUniformuiv ) 
    CHECK_AND_LOAD_BY_FN( glBindFragDataLocation ) 
    CHECK_AND_LOAD_BY_FN( glGetFragDataLocation ) 
    CHECK_AND_LOAD_BY_FN( glUniform1ui ) 
    CHECK_AND_LOAD_BY_FN( glUniform2ui ) 
    CHECK_AND_LOAD_BY_FN( glUniform3ui ) 
    CHECK_AND_LOAD_BY_FN( glUniform4ui ) 
    CHECK_AND_LOAD_BY_FN( glUniform1uiv ) 
    CHECK_AND_LOAD_BY_FN( glUniform2uiv ) 
    CHECK_AND_LOAD_BY_FN( glUniform3uiv ) 
    CHECK_AND_LOAD_BY_FN( glUniform4uiv ) 
    CHECK_AND_LOAD_BY_FN( glTexParameterIiv ) 
    CHECK_AND_LOAD_BY_FN( glTexParameterIuiv ) 
    CHECK_AND_LOAD_BY_FN( glGetTexParameterIiv ) 
    CHECK_AND_LOAD_BY_FN( glGetTexParameterIuiv ) 
    CHECK_AND_LOAD_BY_FN( glClearBufferiv ) 
    CHECK_AND_LOAD_BY_FN( glClearBufferuiv ) 
    CHECK_AND_LOAD_BY_FN( glClearBufferfv ) 
    CHECK_AND_LOAD_BY_FN( glClearBufferfi ) 
    CHECK_AND_LOAD_BY_FN( glGetStringi ) 
    CHECK_AND_LOAD_BY_FN( glIsRenderbuffer ) 
    CHECK_AND_LOAD_BY_FN( glBindRenderbuffer ) 
    CHECK_AND_LOAD_BY_FN( glDeleteRenderbuffers ) 
    CHECK_AND_LOAD_BY_FN( glGenRenderbuffers ) 
    CHECK_AND_LOAD_BY_FN( glRenderbufferStorage ) 
    CHECK_AND_LOAD_BY_FN( glGetRenderbufferParameteriv ) 
    CHECK_AND_LOAD_BY_FN( glIsFramebuffer ) 
    CHECK_AND_LOAD_BY_FN( glBindFramebuffer ) 
    CHECK_AND_LOAD_BY_FN( glDeleteFramebuffers ) 
    CHECK_AND_LOAD_BY_FN( glGenFramebuffers ) 
    CHECK_AND_LOAD_BY_FN( glCheckFramebufferStatus ) 
    CHECK_AND_LOAD_BY_FN( glFramebufferTexture1D ) 
    CHECK_AND_LOAD_BY_FN( glFramebufferTexture2D ) 
    CHECK_AND_LOAD_BY_FN( glFramebufferTexture3D ) 
    CHECK_AND_LOAD_BY_FN( glFramebufferRenderbuffer ) 
    CHECK_AND_LOAD_BY_FN( glGetFramebufferAttachmentParameteriv ) 
    CHECK_AND_LOAD_BY_FN( glGenerateMipmap ) 
    CHECK_AND_LOAD_BY_FN( glBlitFramebuffer ) 
    CHECK_AND_LOAD_BY_FN( glRenderbufferStorageMultisample ) 
    CHECK_AND_LOAD_BY_FN( glFramebufferTextureLayer ) 
    CHECK_AND_LOAD_BY_FN( glMapBufferRange ) 
    CHECK_AND_LOAD_BY_FN( glFlushMappedBufferRange ) 
    CHECK_AND_LOAD_BY_FN( glBindVertexArray ) 
    CHECK_AND_LOAD_BY_FN( glDeleteVertexArrays ) 
    CHECK_AND_LOAD_BY_FN( glGenVertexArrays ) 
    CHECK_AND_LOAD_BY_FN( glIsVertexArray ) 
 
    // gl version 3.1

    CHECK_AND_LOAD_BY_FN( glDrawArraysInstanced ) 
    CHECK_AND_LOAD_BY_FN( glDrawElementsInstanced ) 
    CHECK_AND_LOAD_BY_FN( glTexBuffer ) 
    CHECK_AND_LOAD_BY_FN( glPrimitiveRestartIndex ) 
    CHECK_AND_LOAD_BY_FN( glCopyBufferSubData ) 
    CHECK_AND_LOAD_BY_FN( glGetUniformIndices ) 
    CHECK_AND_LOAD_BY_FN( glGetActiveUniformsiv ) 
    CHECK_AND_LOAD_BY_FN( glGetActiveUniformName ) 
    CHECK_AND_LOAD_BY_FN( glGetUniformBlockIndex ) 
    CHECK_AND_LOAD_BY_FN( glGetActiveUniformBlockiv ) 
    CHECK_AND_LOAD_BY_FN( glGetActiveUniformBlockName ) 
    CHECK_AND_LOAD_BY_FN( glUniformBlockBinding ) 

    // gl version 3.2

    CHECK_AND_LOAD_BY_FN( glDrawElementsBaseVertex ) 
    CHECK_AND_LOAD_BY_FN( glDrawRangeElementsBaseVertex ) 
    CHECK_AND_LOAD_BY_FN( glDrawElementsInstancedBaseVertex ) 
    CHECK_AND_LOAD_BY_FN( glMultiDrawElementsBaseVertex ) 
    CHECK_AND_LOAD_BY_FN( glProvokingVertex ) 
    CHECK_AND_LOAD_BY_FN( glFenceSync ) 
    CHECK_AND_LOAD_BY_FN( glIsSync ) 
    CHECK_AND_LOAD_BY_FN( glDeleteSync ) 
    CHECK_AND_LOAD_BY_FN( glClientWaitSync ) 
    CHECK_AND_LOAD_BY_FN( glWaitSync ) 
    CHECK_AND_LOAD_BY_FN( glGetInteger64v ) 
    CHECK_AND_LOAD_BY_FN( glGetSynciv ) 
    CHECK_AND_LOAD_BY_FN( glGetInteger64i_v ) 
    CHECK_AND_LOAD_BY_FN( glGetBufferParameteri64v ) 
    CHECK_AND_LOAD_BY_FN( glFramebufferTexture ) 
    CHECK_AND_LOAD_BY_FN( glTexImage2DMultisample ) 
    CHECK_AND_LOAD_BY_FN( glTexImage3DMultisample ) 
    CHECK_AND_LOAD_BY_FN( glGetMultisamplefv ) 
    CHECK_AND_LOAD_BY_FN( glSampleMaski ) 

    // gl version 3.3

    CHECK_AND_LOAD_BY_FN( glBindFragDataLocationIndexed ) 
    CHECK_AND_LOAD_BY_FN( glGetFragDataIndex ) 
    CHECK_AND_LOAD_BY_FN( glGenSamplers ) 
    CHECK_AND_LOAD_BY_FN( glDeleteSamplers ) 
    CHECK_AND_LOAD_BY_FN( glIsSampler ) 
    CHECK_AND_LOAD_BY_FN( glBindSampler ) 
    CHECK_AND_LOAD_BY_FN( glSamplerParameteri ) 
    CHECK_AND_LOAD_BY_FN( glSamplerParameteriv ) 
    CHECK_AND_LOAD_BY_FN( glSamplerParameterf ) 
    CHECK_AND_LOAD_BY_FN( glSamplerParameterfv ) 
    CHECK_AND_LOAD_BY_FN( glSamplerParameterIiv ) 
    CHECK_AND_LOAD_BY_FN( glSamplerParameterIuiv ) 
    CHECK_AND_LOAD_BY_FN( glGetSamplerParameteriv ) 
    CHECK_AND_LOAD_BY_FN( glGetSamplerParameterIiv ) 
    CHECK_AND_LOAD_BY_FN( glGetSamplerParameterfv ) 
    CHECK_AND_LOAD_BY_FN( glGetSamplerParameterIuiv ) 
    CHECK_AND_LOAD_BY_FN( glQueryCounter ) 
    CHECK_AND_LOAD_BY_FN( glGetQueryObjecti64v ) 
    CHECK_AND_LOAD_BY_FN( glGetQueryObjectui64v ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribDivisor ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribP1ui ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribP1uiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribP2ui ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribP2uiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribP3ui ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribP3uiv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribP4ui ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribP4uiv ) 

    // gl version 4.0

    CHECK_AND_LOAD_BY_FN( glMinSampleShading ) 
    CHECK_AND_LOAD_BY_FN( glBlendEquationi ) 
    CHECK_AND_LOAD_BY_FN( glBlendEquationSeparatei ) 
    CHECK_AND_LOAD_BY_FN( glBlendFunci ) 
    CHECK_AND_LOAD_BY_FN( glBlendFuncSeparatei ) 
    CHECK_AND_LOAD_BY_FN( glDrawArraysIndirect ) 
    CHECK_AND_LOAD_BY_FN( glDrawElementsIndirect ) 
    CHECK_AND_LOAD_BY_FN( glUniform1d ) 
    CHECK_AND_LOAD_BY_FN( glUniform2d ) 
    CHECK_AND_LOAD_BY_FN( glUniform3d ) 
    CHECK_AND_LOAD_BY_FN( glUniform4d ) 
    CHECK_AND_LOAD_BY_FN( glUniform1dv ) 
    CHECK_AND_LOAD_BY_FN( glUniform2dv ) 
    CHECK_AND_LOAD_BY_FN( glUniform3dv ) 
    CHECK_AND_LOAD_BY_FN( glUniform4dv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix2dv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix3dv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix4dv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix2x3dv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix2x4dv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix3x2dv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix3x4dv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix4x2dv ) 
    CHECK_AND_LOAD_BY_FN( glUniformMatrix4x3dv ) 
    CHECK_AND_LOAD_BY_FN( glGetUniformdv ) 
    CHECK_AND_LOAD_BY_FN( glGetSubroutineUniformLocation ) 
    CHECK_AND_LOAD_BY_FN( glGetSubroutineIndex ) 
    CHECK_AND_LOAD_BY_FN( glGetActiveSubroutineUniformiv ) 
    CHECK_AND_LOAD_BY_FN( glGetActiveSubroutineUniformName ) 
    CHECK_AND_LOAD_BY_FN( glGetActiveSubroutineName ) 
    CHECK_AND_LOAD_BY_FN( glUniformSubroutinesuiv ) 
    CHECK_AND_LOAD_BY_FN( glGetUniformSubroutineuiv ) 
    CHECK_AND_LOAD_BY_FN( glGetProgramStageiv ) 
    CHECK_AND_LOAD_BY_FN( glPatchParameteri ) 
    CHECK_AND_LOAD_BY_FN( glPatchParameterfv ) 
    CHECK_AND_LOAD_BY_FN( glBindTransformFeedback ) 
    CHECK_AND_LOAD_BY_FN( glDeleteTransformFeedbacks ) 
    CHECK_AND_LOAD_BY_FN( glGenTransformFeedbacks ) 
    CHECK_AND_LOAD_BY_FN( glIsTransformFeedback ) 
    CHECK_AND_LOAD_BY_FN( glPauseTransformFeedback ) 
    CHECK_AND_LOAD_BY_FN( glResumeTransformFeedback ) 
    CHECK_AND_LOAD_BY_FN( glDrawTransformFeedback ) 
    CHECK_AND_LOAD_BY_FN( glDrawTransformFeedbackStream ) 
    CHECK_AND_LOAD_BY_FN( glBeginQueryIndexed ) 
    CHECK_AND_LOAD_BY_FN( glEndQueryIndexed ) 
    CHECK_AND_LOAD_BY_FN( glGetQueryIndexediv ) 
 
    // gl version 4.1
         
    CHECK_AND_LOAD_BY_FN( glReleaseShaderCompiler ) 
    CHECK_AND_LOAD_BY_FN( glShaderBinary ) 
    CHECK_AND_LOAD_BY_FN( glGetShaderPrecisionFormat ) 
    CHECK_AND_LOAD_BY_FN( glDepthRangef ) 
    CHECK_AND_LOAD_BY_FN( glClearDepthf ) 
    CHECK_AND_LOAD_BY_FN( glGetProgramBinary ) 
    CHECK_AND_LOAD_BY_FN( glProgramBinary ) 
    CHECK_AND_LOAD_BY_FN( glProgramParameteri ) 
    CHECK_AND_LOAD_BY_FN( glUseProgramStages ) 
    CHECK_AND_LOAD_BY_FN( glActiveShaderProgram ) 
    CHECK_AND_LOAD_BY_FN( glCreateShaderProgramv ) 
    CHECK_AND_LOAD_BY_FN( glBindProgramPipeline ) 
    CHECK_AND_LOAD_BY_FN( glDeleteProgramPipelines ) 
    CHECK_AND_LOAD_BY_FN( glGenProgramPipelines ) 
    CHECK_AND_LOAD_BY_FN( glIsProgramPipeline ) 
    CHECK_AND_LOAD_BY_FN( glGetProgramPipelineiv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform1i ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform1iv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform1f ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform1fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform1d ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform1dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform1ui ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform1uiv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform2i ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform2iv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform2f ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform2fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform2d ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform2dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform2ui ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform2uiv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform3i ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform3iv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform3f ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform3fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform3d ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform3dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform3ui ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform3uiv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform4i ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform4iv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform4f ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform4fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform4d ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform4dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform4ui ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniform4uiv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix2fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix3fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix4fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix2dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix3dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix4dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix2x3fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix3x2fv )  
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix2x4fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix4x2fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix3x4fv )  
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix4x3fv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix2x3dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix3x2dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix2x4dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix4x2dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix3x4dv ) 
    CHECK_AND_LOAD_BY_FN( glProgramUniformMatrix4x3dv ) 
    CHECK_AND_LOAD_BY_FN( glValidateProgramPipeline ) 
    CHECK_AND_LOAD_BY_FN( glGetProgramPipelineInfoLog )  
    CHECK_AND_LOAD_BY_FN( glVertexAttribL1d  ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribL2d ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribL3d ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribL4d ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribL1dv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribL2dv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribL3dv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribL4dv ) 
    CHECK_AND_LOAD_BY_FN( glVertexAttribLPointer ) 
    CHECK_AND_LOAD_BY_FN( glGetVertexAttribLdv ) 
    CHECK_AND_LOAD_BY_FN( glViewportArrayv ) 
    CHECK_AND_LOAD_BY_FN( glViewportIndexedf ) 
    CHECK_AND_LOAD_BY_FN( glViewportIndexedfv ) 
    CHECK_AND_LOAD_BY_FN( glScissorArrayv ) 
    CHECK_AND_LOAD_BY_FN( glScissorIndexed ) 
    CHECK_AND_LOAD_BY_FN( glScissorIndexedv ) 
    CHECK_AND_LOAD_BY_FN( glDepthRangeArrayv ) 
    CHECK_AND_LOAD_BY_FN( glDepthRangeIndexed ) 
    CHECK_AND_LOAD_BY_FN( glGetFloati_v ) 
    CHECK_AND_LOAD_BY_FN( glGetDoublei_v ) 

    _init = true ;

    return natus::gl::result::ok ;
}

