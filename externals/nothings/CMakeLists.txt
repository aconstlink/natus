
# Used headers
set( INCLUDES 
  "stb/stb.h"
  "stb/stb_image.h"
  "stb/stb_image_write.h"
  "stb/stb_truetype.h"
  "stb/stb_vorbis.h" )

add_library( stb INTERFACE )
target_include_directories( stb INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/stb> )

if( NATUS_COMPILER_MSC )
  target_compile_options( stb INTERFACE /wd4267 /wd4996 )
elseif( NATUS_COMPILER_GNU )
  target_compile_options( stb INTERFACE -Wno-sign-compare -Wno-unused-but-set-variable )
endif()

export( TARGETS stb NAMESPACE natus:: APPEND FILE ${NATUS_BINARY_DIR}/natus-targets.cmake )
