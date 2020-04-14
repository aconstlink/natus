#pragma once

#include "../result.h"
#include "../api.h"
#include "../typedefs.h"

#if defined( NATUS_TARGET_OS_WIN )
#include <windows.h>
#elif defined( NATUS_TARGET_OS_LIN )
#else
#endif

#include <GL/glcorearb.h>
#define GL_VERSION_1_1 0

#include <natus/std/list.hpp>

namespace natus
{
    namespace ogl
    {
        struct NATUS_OGL_API gl
        {
            natus_this_typedefs( gl ) ;

        public:

            typedef natus::std::list< natus::std::string > string_list_t ;
            static string_list_t _extensions ;

            static void_ptr_t load_gl_function( char_cptr_t name ) ;

            static bool_t _init ;

        public:

            static natus::ogl::result init( void_t ) ;

            static bool_t is_supported( char const* name ) ;

        public: // gl version 1.0

            static PFNGLCULLFACEPROC glCullFace ;
            static PFNGLFRONTFACEPROC glFrontFace ;
            static PFNGLHINTPROC glHint ;
            static PFNGLLINEWIDTHPROC glLineWidth;
            static PFNGLPOINTSIZEPROC glPointSize ;
            static PFNGLPOLYGONMODEPROC glPolygonMode ;
            static PFNGLSCISSORPROC glScissor ;
            static PFNGLTEXPARAMETERFPROC glTexParameterf ;
            static PFNGLTEXPARAMETERFVPROC glTexParameterfv ;
            static PFNGLTEXPARAMETERIPROC glTexParameteri ;
            static PFNGLTEXPARAMETERIVPROC glTexParameteriv ;
            static PFNGLTEXIMAGE1DPROC glTexImage1D ;
            static PFNGLTEXIMAGE2DPROC glTexImage2D ;
            static PFNGLDRAWBUFFERPROC glDrawBuffer ;
            static PFNGLCLEARPROC glClear ;
            static PFNGLCLEARCOLORPROC glClearColor ;
            static PFNGLCLEARSTENCILPROC glClearStencil ;
            static PFNGLCLEARDEPTHPROC glClearDepth ;
            static PFNGLSTENCILMASKPROC glStencilMask ;
            static PFNGLCOLORMASKPROC glColorMask ;
            static PFNGLDEPTHMASKPROC glDepthMask ;
            static PFNGLDISABLEPROC glDisable ;
            static PFNGLENABLEPROC glEnable ;
            static PFNGLFINISHPROC glFinish ;
            static PFNGLFLUSHPROC glFlush ;
            static PFNGLBLENDFUNCPROC glBlendFunc ;
            static PFNGLLOGICOPPROC glLogicOp ;
            static PFNGLSTENCILFUNCPROC glStencilFunc ;
            static PFNGLSTENCILOPPROC glStencilOp ;
            static PFNGLDEPTHFUNCPROC glDepthFunc ;
            static PFNGLPIXELSTOREFPROC glPixelStoref ;
            static PFNGLPIXELSTOREIPROC glPixelStorei ;
            static PFNGLREADBUFFERPROC glReadBuffer ;
            static PFNGLREADPIXELSPROC glReadPixels ;
            static PFNGLGETBOOLEANVPROC glGetBooleanv ;
            static PFNGLGETDOUBLEVPROC glGetDoublev ;
            static PFNGLGETERRORPROC glGetError ;
            static PFNGLGETFLOATVPROC glGetFloatv ;
            static PFNGLGETINTEGERVPROC glGetIntegerv ;
            static PFNGLGETSTRINGPROC glGetString ;
            static PFNGLGETTEXIMAGEPROC glGetTexImage ;
            static PFNGLGETTEXPARAMETERFVPROC glGetTexParameterfv ;
            static PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv ;
            static PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv ;
            static PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv ;
            static PFNGLISENABLEDPROC glIsEnabled ;
            static PFNGLDEPTHRANGEPROC glDepthRange ;
            static PFNGLVIEWPORTPROC glViewport ;

        public: // gl version 1.1

            static PFNGLDRAWARRAYSPROC glDrawArrays ;
            static PFNGLDRAWELEMENTSPROC glDrawElements ;
            static PFNGLGETPOINTERVPROC glGetPointerv ;
            static PFNGLPOLYGONOFFSETPROC glPolygonOffset ;
            static PFNGLCOPYTEXIMAGE1DPROC glCopyTexImage1D ;
            static PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D ;
            static PFNGLCOPYTEXSUBIMAGE1DPROC glCopyTexSubImage1D ;
            static PFNGLCOPYTEXSUBIMAGE2DPROC glCopyTexSubImage2D ;
            static PFNGLTEXSUBIMAGE1DPROC glTexSubImage1D ;
            static PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D ;
            static PFNGLBINDTEXTUREPROC glBindTexture ;
            static PFNGLDELETETEXTURESPROC glDeleteTextures ;
            static PFNGLGENTEXTURESPROC glGenTextures ;
            static PFNGLISTEXTUREPROC glIsTexture ;

        public: // gl version 1.2

            static PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements ;
            static PFNGLTEXIMAGE3DPROC glTexImage3D ;
            static PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D ;
            static PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D ;

        public: // gl version 1.3

            static PFNGLACTIVETEXTUREPROC glActiveTexture ;
            static PFNGLSAMPLECOVERAGEPROC glSampleCoverage ;
            static PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D ;
            static PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D ;
            static PFNGLCOMPRESSEDTEXIMAGE1DPROC glCompressedTexImage1D ;
            static PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D ;
            static PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D ;
            static PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D ;
            static PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage ;

        public: // gl version 1.4

            static PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate ;
            static PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays ;
            static PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements ;
            static PFNGLPOINTPARAMETERFPROC glPointParameterf ;
            static PFNGLPOINTPARAMETERFVPROC glPointParameterfv ;
            static PFNGLPOINTPARAMETERIPROC glPointParameteri ;
            static PFNGLPOINTPARAMETERIVPROC glPointParameteriv ;
            static PFNGLBLENDCOLORPROC glBlendColor ;
            static PFNGLBLENDEQUATIONPROC glBlendEquation ;

        public: // gl version 1.5

            static PFNGLGENQUERIESPROC glGenQueries  ;
            static PFNGLDELETEQUERIESPROC glDeleteQueries ;
            static PFNGLISQUERYPROC glIsQuery ;
            static PFNGLBEGINQUERYPROC glBeginQuery ;
            static PFNGLENDQUERYPROC glEndQuery ;
            static PFNGLGETQUERYIVPROC glGetQueryiv ;
            static PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv ;
            static PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv ;
            static PFNGLBINDBUFFERPROC glBindBuffer ;
            static PFNGLDELETEBUFFERSPROC glDeleteBuffers ;
            static PFNGLGENBUFFERSPROC glGenBuffers ;
            static PFNGLISBUFFERPROC glIsBuffer ;
            static PFNGLBUFFERDATAPROC glBufferData ;
            static PFNGLBUFFERSUBDATAPROC glBufferSubData ;
            static PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData ;
            static PFNGLMAPBUFFERPROC glMapBuffer ;
            static PFNGLUNMAPBUFFERPROC glUnmapBuffer ;
            static PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv ;
            static PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv ;

        public: // gl version 2.0

            static PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate  ;
            static PFNGLDRAWBUFFERSPROC glDrawBuffers ;
            static PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate ;
            static PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate ;
            static PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate ;
            static PFNGLATTACHSHADERPROC glAttachShader ;
            static PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation ;
            static PFNGLCOMPILESHADERPROC glCompileShader ;
            static PFNGLCREATEPROGRAMPROC glCreateProgram ;
            static PFNGLCREATESHADERPROC glCreateShader ;
            static PFNGLDELETEPROGRAMPROC glDeleteProgram ;
            static PFNGLDELETESHADERPROC glDeleteShader ;
            static PFNGLDETACHSHADERPROC glDetachShader ;
            static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray ;
            static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray ;
            static PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib ;
            static PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform ;
            static PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders ;
            static PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation ;
            static PFNGLGETPROGRAMIVPROC glGetProgramiv ;
            static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog ;
            static PFNGLGETSHADERIVPROC glGetShaderiv ;
            static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog ;
            static PFNGLGETSHADERSOURCEPROC glGetShaderSource ;
            static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation ;
            static PFNGLGETUNIFORMFVPROC glGetUniformfv ;
            static PFNGLGETUNIFORMIVPROC glGetUniformiv ;
            static PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv ;
            static PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv ;
            static PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv ;
            static PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv ;
            static PFNGLISPROGRAMPROC glIsProgram ;
            static PFNGLISSHADERPROC glIsShader ;
            static PFNGLLINKPROGRAMPROC glLinkProgram ;
            static PFNGLSHADERSOURCEPROC glShaderSource ;
            static PFNGLUSEPROGRAMPROC glUseProgram ;
            static PFNGLUNIFORM1FPROC glUniform1f ;
            static PFNGLUNIFORM2FPROC glUniform2f ;
            static PFNGLUNIFORM3FPROC glUniform3f ;
            static PFNGLUNIFORM4FPROC glUniform4f ;
            static PFNGLUNIFORM1IPROC glUniform1i ;
            static PFNGLUNIFORM2IPROC glUniform2i ;
            static PFNGLUNIFORM3IPROC glUniform3i ;
            static PFNGLUNIFORM4IPROC glUniform4i ;
            static PFNGLUNIFORM1FVPROC glUniform1fv ;
            static PFNGLUNIFORM2FVPROC glUniform2fv ;
            static PFNGLUNIFORM3FVPROC glUniform3fv ;
            static PFNGLUNIFORM4FVPROC glUniform4fv ;
            static PFNGLUNIFORM1IVPROC glUniform1iv ;
            static PFNGLUNIFORM2IVPROC glUniform2iv ;
            static PFNGLUNIFORM3IVPROC glUniform3iv ;
            static PFNGLUNIFORM4IVPROC glUniform4iv ;
            static PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv ;
            static PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv ;
            static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv ;
            static PFNGLVALIDATEPROGRAMPROC glValidateProgram ;
            static PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d ;
            static PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv ;
            static PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f ;
            static PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv ;
            static PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s ;
            static PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv ;
            static PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d ;
            static PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv ;
            static PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f ;
            static PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv ;
            static PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s ;
            static PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv ;
            static PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d ;
            static PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv ;
            static PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f ;
            static PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv ;
            static PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s ;
            static PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv ;
            static PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv ;
            static PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv ;
            static PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv ;
            static PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub ;
            static PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv ;
            static PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv ;
            static PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv ;
            static PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv ;
            static PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d ;
            static PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv ;
            static PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f ;
            static PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv ;
            static PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv ;
            static PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s ;
            static PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv ;
            static PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv ;
            static PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv ;
            static PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv ;
            static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer ;

        public: // gl version 2.1

            static PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv ;
            static PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv ;
            static PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv ;
            static PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv ;
            static PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv ;
            static PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv ;

        public: // gl version 3.0

            static PFNGLCOLORMASKIPROC glColorMaski ;
            static PFNGLGETBOOLEANI_VPROC glGetBooleani_v ;
            static PFNGLGETINTEGERI_VPROC glGetIntegeri_v ;
            static PFNGLENABLEIPROC glEnablei ;
            static PFNGLDISABLEIPROC glDisablei ;
            static PFNGLISENABLEDIPROC glIsEnabledi ;
            static PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback ;
            static PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback ;
            static PFNGLBINDBUFFERRANGEPROC glBindBufferRange ;
            static PFNGLBINDBUFFERBASEPROC glBindBufferBase ;
            static PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings ;
            static PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying ;
            static PFNGLCLAMPCOLORPROC glClampColor ;
            static PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender ;
            static PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender ;
            static PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer ;
            static PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv ;
            static PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv ;
            static PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i ;
            static PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i ;
            static PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i ;
            static PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i ;
            static PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui ;
            static PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui ;
            static PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui ;
            static PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui ;
            static PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv ;
            static PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv ;
            static PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv ;
            static PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv ;
            static PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv ;
            static PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv ;
            static PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv ;
            static PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv ;
            static PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv ;
            static PFNGLVERTEXATTRIBI4SVPROC  glVertexAttribI4sv ;
            static PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv ;
            static PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv ;
            static PFNGLGETUNIFORMUIVPROC glGetUniformuiv ;
            static PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation ;
            static PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation ;
            static PFNGLUNIFORM1UIPROC glUniform1ui ;
            static PFNGLUNIFORM2UIPROC glUniform2ui ;
            static PFNGLUNIFORM3UIPROC glUniform3ui ;
            static PFNGLUNIFORM4UIPROC glUniform4ui ;
            static PFNGLUNIFORM1UIVPROC glUniform1uiv ;
            static PFNGLUNIFORM2UIVPROC glUniform2uiv ;
            static PFNGLUNIFORM3UIVPROC glUniform3uiv ;
            static PFNGLUNIFORM4UIVPROC glUniform4uiv ;
            static PFNGLTEXPARAMETERIIVPROC glTexParameterIiv ;
            static PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv ;
            static PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv ;
            static PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv ;
            static PFNGLCLEARBUFFERIVPROC glClearBufferiv ;
            static PFNGLCLEARBUFFERUIVPROC glClearBufferuiv ;
            static PFNGLCLEARBUFFERFVPROC glClearBufferfv ;
            static PFNGLCLEARBUFFERFIPROC glClearBufferfi ;
            static PFNGLGETSTRINGIPROC glGetStringi ;
            static PFNGLISRENDERBUFFERPROC glIsRenderbuffer ;
            static PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer ;
            static PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers ;
            static PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers ;
            static PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage ;
            static PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv ;
            static PFNGLISFRAMEBUFFERPROC glIsFramebuffer ;
            static PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer ;
            static PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers ;
            static PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers ;
            static PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus ;
            static PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D ;
            static PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D ;
            static PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D ;
            static PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer ;
            static PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv ;
            static PFNGLGENERATEMIPMAPPROC glGenerateMipmap ;
            static PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer ;
            static PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample ;
            static PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer ;
            static PFNGLMAPBUFFERRANGEPROC glMapBufferRange ;
            static PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange ;
            static PFNGLBINDVERTEXARRAYPROC glBindVertexArray ;
            static PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays ;
            static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays ;
            static PFNGLISVERTEXARRAYPROC glIsVertexArray ;

        public: // gl version 3.1

            static PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced ;
            static PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced ;
            static PFNGLTEXBUFFERPROC glTexBuffer ;
            static PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex ;
            static PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData ;
            static PFNGLGETUNIFORMINDICESPROC glGetUniformIndices ;
            static PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv ;
            static PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName ;
            static PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex ;
            static PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv ;
            static PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName ;
            static PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding ;

        public: // gl version 3.2

            static PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex ;
            static PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex ;
            static PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex ;
            static PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex ;
            static PFNGLPROVOKINGVERTEXPROC glProvokingVertex ;
            static PFNGLFENCESYNCPROC glFenceSync ;
            static PFNGLISSYNCPROC glIsSync ;
            static PFNGLDELETESYNCPROC glDeleteSync ;
            static PFNGLCLIENTWAITSYNCPROC glClientWaitSync ;
            static PFNGLWAITSYNCPROC glWaitSync ;
            static PFNGLGETINTEGER64VPROC glGetInteger64v ;
            static PFNGLGETSYNCIVPROC glGetSynciv ;
            static PFNGLGETINTEGER64I_VPROC glGetInteger64i_v ;
            static PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v ;
            static PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture ;
            static PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample ;
            static PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample ;
            static PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv ;
            static PFNGLSAMPLEMASKIPROC glSampleMaski ;

        public: // gl version 3.3

            static PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed ;
            static PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex ;
            static PFNGLGENSAMPLERSPROC glGenSamplers ;
            static PFNGLDELETESAMPLERSPROC glDeleteSamplers ;
            static PFNGLISSAMPLERPROC glIsSampler ;
            static PFNGLBINDSAMPLERPROC glBindSampler ;
            static PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri ;
            static PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv ;
            static PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf ;
            static PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv ;
            static PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv ;
            static PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv ;
            static PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv ;
            static PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv ;
            static PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv ;
            static PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv ;
            static PFNGLQUERYCOUNTERPROC glQueryCounter ;
            static PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v ;
            static PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v ;
            static PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor ;
            static PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui ;
            static PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv ;
            static PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui ;
            static PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv ;
            static PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui ;
            static PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv ;
            static PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui ;
            static PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv ;

        public: // gl version 4.0

            static PFNGLMINSAMPLESHADINGPROC glMinSampleShading ;
            static PFNGLBLENDEQUATIONIPROC glBlendEquationi ;
            static PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei ;
            static PFNGLBLENDFUNCIPROC glBlendFunci ;
            static PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei ;
            static PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect ;
            static PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect ;
            static PFNGLUNIFORM1DPROC glUniform1d ;
            static PFNGLUNIFORM2DPROC glUniform2d ;
            static PFNGLUNIFORM3DPROC glUniform3d ;
            static PFNGLUNIFORM4DPROC glUniform4d ;
            static PFNGLUNIFORM1DVPROC glUniform1dv ;
            static PFNGLUNIFORM2DVPROC glUniform2dv ;
            static PFNGLUNIFORM3DVPROC glUniform3dv ;
            static PFNGLUNIFORM4DVPROC glUniform4dv ;
            static PFNGLUNIFORMMATRIX2DVPROC glUniformMatrix2dv ;
            static PFNGLUNIFORMMATRIX3DVPROC glUniformMatrix3dv ;
            static PFNGLUNIFORMMATRIX4DVPROC glUniformMatrix4dv ;
            static PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv ;
            static PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv ;
            static PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv ;
            static PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv ;
            static PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv ;
            static PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv ;
            static PFNGLGETUNIFORMDVPROC glGetUniformdv ;
            static PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glGetSubroutineUniformLocation ;
            static PFNGLGETSUBROUTINEINDEXPROC glGetSubroutineIndex ;
            static PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glGetActiveSubroutineUniformiv ;
            static PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName ;
            static PFNGLGETACTIVESUBROUTINENAMEPROC glGetActiveSubroutineName ;
            static PFNGLUNIFORMSUBROUTINESUIVPROC glUniformSubroutinesuiv ;
            static PFNGLGETUNIFORMSUBROUTINEUIVPROC glGetUniformSubroutineuiv ;
            static PFNGLGETPROGRAMSTAGEIVPROC glGetProgramStageiv ;
            static PFNGLPATCHPARAMETERIPROC glPatchParameteri ;
            static PFNGLPATCHPARAMETERFVPROC glPatchParameterfv ;
            static PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback ;
            static PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks ;
            static PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks ;
            static PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback ;
            static PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback ;
            static PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback ;
            static PFNGLDRAWTRANSFORMFEEDBACKPROC glDrawTransformFeedback ;
            static PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream ;
            static PFNGLBEGINQUERYINDEXEDPROC glBeginQueryIndexed ;
            static PFNGLENDQUERYINDEXEDPROC glEndQueryIndexed ;
            static PFNGLGETQUERYINDEXEDIVPROC glGetQueryIndexediv ;

        public: // gl version 4.1

            static PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler ;
            static PFNGLSHADERBINARYPROC glShaderBinary ;
            static PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat ;
            static PFNGLDEPTHRANGEFPROC glDepthRangef ;
            static PFNGLCLEARDEPTHFPROC glClearDepthf ;
            static PFNGLGETPROGRAMBINARYPROC glGetProgramBinary ;
            static PFNGLPROGRAMBINARYPROC glProgramBinary ;
            static PFNGLPROGRAMPARAMETERIPROC glProgramParameteri ;
            static PFNGLUSEPROGRAMSTAGESPROC  glUseProgramStages ;
            static PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram ;
            static PFNGLCREATESHADERPROGRAMVPROC  glCreateShaderProgramv ;
            static PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline ;
            static PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines ;
            static PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines ;
            static PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline ;
            static PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv ;
            static PFNGLPROGRAMUNIFORM1IPROC  glProgramUniform1i ;
            static PFNGLPROGRAMUNIFORM1IVPROC  glProgramUniform1iv ;
            static PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f ;
            static PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv ;
            static PFNGLPROGRAMUNIFORM1DPROC glProgramUniform1d ;
            static PFNGLPROGRAMUNIFORM1DVPROC glProgramUniform1dv ;
            static PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui ;
            static PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv ;
            static PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i ;
            static PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv ;
            static PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f ;
            static PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv ;
            static PFNGLPROGRAMUNIFORM2DPROC glProgramUniform2d ;
            static PFNGLPROGRAMUNIFORM2DVPROC glProgramUniform2dv ;
            static PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui ;
            static PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv ;
            static PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i ;
            static PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv ;
            static PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f ;
            static PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv ;
            static PFNGLPROGRAMUNIFORM3DPROC glProgramUniform3d ;
            static PFNGLPROGRAMUNIFORM3DVPROC glProgramUniform3dv ;
            static PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui ;
            static PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv ;
            static PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i ;
            static PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv ;
            static PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f ;
            static PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv ;
            static PFNGLPROGRAMUNIFORM4DPROC glProgramUniform4d ;
            static PFNGLPROGRAMUNIFORM4DVPROC glProgramUniform4dv ;
            static PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui ;
            static PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv ;
            static PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv ;
            static PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv ;
            static PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv ;
            static PFNGLPROGRAMUNIFORMMATRIX2DVPROC glProgramUniformMatrix2dv ;
            static PFNGLPROGRAMUNIFORMMATRIX3DVPROC glProgramUniformMatrix3dv ;
            static PFNGLPROGRAMUNIFORMMATRIX4DVPROC glProgramUniformMatrix4dv ;
            static PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv ;
            static PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv ;
            static PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv ;
            static PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv ;
            static PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv ;
            static PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv ;
            static PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv ;
            static PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv ;
            static PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv ;
            static PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv ;
            static PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv ;
            static PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv ;
            static PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline ;
            static PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog ;
            static PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d  ;
            static PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d ;
            static PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d ;
            static PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d ;
            static PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv ;
            static PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv ;
            static PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv ;
            static PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv ;
            static PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer ;
            static PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv ;
            static PFNGLVIEWPORTARRAYVPROC glViewportArrayv ;
            static PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf ;
            static PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv ;
            static PFNGLSCISSORARRAYVPROC glScissorArrayv ;
            static PFNGLSCISSORINDEXEDPROC glScissorIndexed ;
            static PFNGLSCISSORINDEXEDVPROC glScissorIndexedv ;
            static PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv ;
            static PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed ;
            static PFNGLGETFLOATI_VPROC glGetFloati_v ;
            static PFNGLGETDOUBLEI_VPROC glGetDoublei_v ;
        };
    }
}
