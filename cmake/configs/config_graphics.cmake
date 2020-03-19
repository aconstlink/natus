
#
# Can be used to configure the rendering system 
# for the target platform.
#

set( NATUS_TARGET_GRAPHICS_CONFIGURED FALSE )


set( NATUS_TARGET_GRAPHICS_NULL ON )
set( NATUS_TARGET_GRAPHICS_VULKAN OFF )

set( NATUS_TARGET_GRAPHICS_OPENGL OFF )
set( NATUS_TARGET_GRAPHICS_OPENGL_33 OFF )

set( NATUS_TARGET_GRAPHICS_OPENGLES OFF )
set( NATUS_TARGET_GRAPHICS_DIRECT3D OFF )

# by default, lets just choose OpenGL until there are more api implemented
set( NATUS_TARGET_GRAPHICS_API "OpenGL 3.3" CACHE STRING 
    "Select the graphics api to be compiled into the package." )

set_property(CACHE NATUS_TARGET_GRAPHICS_API PROPERTY STRINGS 
    #"null" "Vulkan" "OpenGL" "OpenGL ES" "Direct3D" )
    "null" "OpenGL 3.3" )

if( NATUS_TARGET_GRAPHICS_API STREQUAL "null" )
    message( "No Graphics API compiled" )
elseif( NATUS_TARGET_GRAPHICS_API STREQUAL "OpenGL 3.3" )
    find_package( OpenGL REQUIRED )
    set( NATUS_TARGET_GRAPHICS_OPENGL ON )
    set( NATUS_TARGET_GRAPHICS_OPENGL_33 ON )
    set( NATUS_TARGET_GRAPHICS_DEFINES 
      -DNATUS_TARGET_GRAPHICS_OPENGL
      -DNATUS_TARGET_GRAPHICS_OPENGL_33 )
elseif( NATUS_TARGET_GRAPHICS_API STREQUAL "OpenGL ES" )
    message( FATAL_ERROR "Graphcis API currently not supported" )
elseif( NATUS_TARGET_GRAPHICS_API STREQUAL "Vulkan" )
    message( FATAL_ERROR "Graphcis API currently not supported" )
elseif( NATUS_TARGET_GRAPHICS_API STREQUAL "Direct3D" )
    message( FATAL_ERROR "Graphcis API currently not supported" )
endif()

set( NATUS_TARGET_GRAPHICS_CONFIGURED TRUE )

