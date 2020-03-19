
#
# natus libraries
# Automatically generated via CMake's configure_file for the build tree.
#

####################################################################
# Import targets
####################################################################
include( "@NATUS_BINARY_DIR@/natus-targets.cmake" )

####################################################################
# Directories
####################################################################
set( NATUS_INCLUDE_BASE "@CMAKE_SOURCE_DIR@" )

set( NATUS_CONFIGS_DIR 
    "${NATUS_INCLUDE_BASE}/cmake/config" )

set( NATUS_MODULES_DIR 
    "${NATUS_INCLUDE_BASE}/cmake/modules"
    "${NATUS_INCLUDE_BASE}/cmake/macros"
    "${NATUS_INCLUDE_BASE}/cmake/functions")

# not perfect but currently required for dlls under windows
set( NATUS_BINDIR "@CMAKE_RUNTIME_OUTPUT_DIRECTORY@" )

set( NATUS_CXX_STANDARD @NATUS_CXX_STANDARD@ )

list( APPEND CMAKE_MODULE_PATH ${NATUS_MODULES_DIR} )
include( so_visual_studio_src_dir )
include( so_visual_studio_generate_bat_dll )
