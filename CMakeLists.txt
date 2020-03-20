cmake_minimum_required( VERSION 3.6 )
project(natus CXX)

if( POLICY CMP0079 )
  cmake_policy( SET CMP0079 NEW )
endif()

set( NATUS_MAJOR_VERSION 0 )
set( NATUSL_MINOR_VERSION 0 )
set( NATUS_PATCH_VERSION 0 )

################################################################
# SECTION: Configuring
################################################################

set( NATUS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( NATUS_BINARY_DIR ${PROJECT_BINARY_DIR} )
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
  "${CMAKE_CURRENT_LIST_DIR}/cmake"
  "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/"
  "${CMAKE_CURRENT_LIST_DIR}/cmake/configs"
  "${CMAKE_CURRENT_LIST_DIR}/cmake/macros/"
  #"${CMAKE_CURRENT_LIST_DIR}/cmake/functions/"
  )

set( CMAKE_INSTALL_PREFIX "${NATUS_BINARY_DIR}/install/natus-${NATUS_MAJOR_VERSION}.${NATUS_MINOR_VERSION}.${NATUS_PATCH_VERSION}" CACHE STRING "Install prefix" FORCE )

include( config_export_target )
include( config_operating_system )
include( config_compiler )
include( config_compiler_flags )
include( config_architecture )
include( config_instruction_set )
include( config_graphics )

include( config_build_environment )
include( config_build_library_type )
include( config_window_system )
include( config_build_configuration )

include( config_rt_link ) 
include( config_output_directory )

include( visual_studio_src_dir )
#include( install_headers )
#include( install_shaders )
#include( install_library )

natus_export_clear()

################################################################
# SECTION : Build in and compilable externals
################################################################

add_subdirectory( "externals/filesystem" )
#add_subdirectory( "externals/lua" )
#add_subdirectory( "externals/khronos" )
#add_subdirectory( "externals/rtmidi" )
#add_subdirectory( "externals/rapidxml" )
#add_subdirectory( "externals/stb" )
#add_subdirectory( "externals/ogg" )
#add_subdirectory( "externals/vorbis" )
#add_subdirectory( "externals/imgui" )

##add_subdirectory( "externals/libusb" )
#add_subdirectory( "externals/glm" )
#add_subdirectory( "externals/assimp" )

#zlib
# todo

################################################################
# SECTION: find externals
################################################################

# optionals
#find_package( OpenCtm )
#find_package( Fmod ) 

if( NATUS_TARGET_OS_WIN )
    #find_package( XInput REQUIRED )
    #find_package( XAudio2 REQUIRED )
 #   find_package( Wddk REQUIRED )
elseif( NATUS_OS_LINUX )
endif()

if( NATUS_WINDOW_SYSTEM_XLIB )
    #find_package( XLib )
endif()

################################################################
# SECTION: add sub directories
################################################################

set( subdirs_header_only 
    "core"
    "std"
    )

set( subdirs
    "memory"
    "log"
    "concurrent"
    "io"
    )


if( NATUS_TARGET_GRAPHICS_OPENGL )
    #list( APPEND SUBDIRS gli )
endif()

#
# header-only
#
foreach( _subdir ${subdirs_header_only} )
    
    set( cur_lib_name "${_subdir}" )
    set( cur_alias_name "${PROJECT_NAME}::${_subdir}" )
    add_subdirectory(${_subdir})

    target_compile_options( ${cur_lib_name} PRIVATE ${NATUS_COMPILER_OPTIONS_PRIVATE} )
    target_include_directories( ${cur_lib_name} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/.." )

    set_target_properties( ${cur_lib_name} PROPERTIES FOLDER "natus" )

    set_target_properties( ${cur_lib_name} PROPERTIES LINKER_LANGUAGE CXX )
    set_target_properties( ${cur_lib_name} PROPERTIES CXX_STANDARD ${NATUS_CXX_STANDARD} )
endforeach()

#
# full libs
#
add_library( complete INTERFACE )
add_library( ${PROJECT_NAME}::complete ALIAS complete )

foreach( _subdir ${subdirs} )

    set( cur_lib_name_short "${_subdir}" )
    set( cur_lib_name "${_subdir}" )
    string( TOUPPER ${cur_lib_name} cur_lib_name_upper)
    set( cur_dir_name "${_subdir}" )
    set( cur_dir_name_raw "${_subdir}" )
    set( cur_alias_name "${PROJECT_NAME}::${_subdir}" )
    set( cur_file_name "${PROJECT_NAME}_${_subdir}" )
    add_subdirectory( ${_subdir} )

    set_target_properties( ${cur_lib_name} PROPERTIES LINKER_LANGUAGE CXX )
    set_target_properties( ${cur_lib_name} PROPERTIES OUTPUT_NAME ${cur_file_name} )
    set_target_properties( ${cur_lib_name} PROPERTIES CXX_STANDARD ${NATUS_CXX_STANDARD} )

    if( NATUS_LIBRARY_BUILD_SHARED )
      target_compile_options( ${cur_lib_name} 
        PRIVATE -DNATUS_${cur_lib_name_upper}_EXPORT 
        INTERFACE -DNATUS_${cur_lib_name_upper}_IMPORT )    
    endif()

    target_compile_options( ${cur_lib_name} PRIVATE ${NATUS_COMPILER_OPTIONS_PRIVATE})
    target_include_directories( ${cur_lib_name} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/.." )

    set_target_properties( ${cur_lib_name} PROPERTIES FOLDER "natus" )
    
    target_link_libraries( complete INTERFACE ${cur_lib_name} )

endforeach()

export(TARGETS complete NAMESPACE natus:: APPEND FILE ${PROJECT_NAME}-targets.cmake)

################################################################
# SECTION : Extensions
################################################################

# project cars

#add_subdirectory( extensions )

################################################################
# SECTION : Export externals
################################################################

################################################################
# SECTION: in build tree fing_package generation
################################################################
configure_file( 
    "${NATUS_SOURCE_DIR}/cmake/natus-config.build.in.cmake"
    "${NATUS_BINARY_DIR}/natus-config.cmake"
    @ONLY )
