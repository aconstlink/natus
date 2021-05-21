
set( sources 
    "api.h"
    "typedefs.h"
    "result.h"
    "enums.h"
    "buffer.hpp"

    "global.h"
    "global.cpp"

    "async.h"
    "async.cpp"
    "id.hpp"
    "object.hpp"

    "object/capture_object.h"
    "object/buffer_object.h"
    "object/synth_object.h"

    "backend/audio_capture_helper.h"
    "backend/audio_capture_helper.cpp"
    "backend/backend.h"
    "backend/backend.cpp"
    "backend/types.h"
    "backend/oal/oal.h"
    "backend/oal/oal.cpp"
    "backend/null/null_audio_capture_helper.h"
    )

if( NATUS_TARGET_OS_WIN )
  list( APPEND sources 
    "backend/wasapi/wasapi_capture_helper.h"
    "backend/wasapi/wasapi_capture_helper.cpp"
    "backend/xaudio/xaudio2.h"
    "backend/xaudio/xaudio2.cpp"
)
endif()
natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} ) 

target_link_libraries( ${cur_lib_name}
  PRIVATE OpenAL
  PUBLIC natus::math
  PUBLIC natus::memory
  PUBLIC natus::log
    )

if( XAUDIO2_ENABLE )
  target_link_libraries( ${cur_lib_name}
    PRIVATE xaudio2 )
endif()

if( NATUS_COMPILER_GNU )
  target_compile_options( ${cur_lib_name} PUBLIC -Wno-pessimizing-move )
endif()

if( NATUS_TARGET_ARCH_BIG_ENDIAN )
  target_compile_options( ${cur_lib_name} PRIVATE -DNATUS_BIG_ENDIAN )
endif()

###########################################################
# SECTION: Build Tree
###########################################################

##natus_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name}" )
install_library( ${cur_lib_name} ${PROJECT_NAME}-targets )

