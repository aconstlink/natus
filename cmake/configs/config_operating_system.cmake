
#
# determine the host and target operating system
#
# the values here can be initialized using a toolchain file

include( subdirectories )

set( NATUS_OS_CONFIGURED FALSE )


set( NATUS_HOST_OS "Unknown" )
set( NATUS_HOST_OS_WIN OFF )
set( NATUS_HOST_OS_LIN OFF )
set( NATUS_HOST_OS_MAC OFF )


set( NATUS_TARGET_OS "Unknown" )
set( NATUS_TARGET_OS_WIN OFF )
set( NATUS_TARGET_OS_LIN OFF )
set( NATUS_TARGET_OS_MAC OFF )


#######################################################################################
# Host os
#######################################################################################
message( STATUS "+++ Determine Host Operating System +++ " )

if( CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows" )

    set( NATUS_HOST_OS_WIN ON )
   
    message( ${CMAKE_SYSTEM_VERSION} )
    if( ${CMAKE_SYSTEM_VERSION} EQUAL 6.1 )
        set( NATUS_HOST_OS_WIN7 ON )
        
    # no windows 8. Only windows 8.1
    elseif( ${CMAKE_SYSTEM_VERSION} EQUAL 6.3 )
        set( NATUS_HOST_OS_WIN81 ON )
    elseif( ${CMAKE_SYSTEM_VERSION} GREATER 9 )
        set( NATUS_HOST_OS_WIN10 ON )
    endif()
    ## Determine Win7, Win8, Win8.1, Win10

elseif( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" )

    set( NATUS_HOST_OS_LIN ON )

elseif( CMAKE_HOST_SYSTEM_NAME STREQUAL "Apple" )

    set( NATUS_HOST_OS_MAC ON )

else()
    message( FATAL_ERROR "Unknown Host Operating System" )
endif()

#
# Print Info
#
if( NATUS_HOST_OS_WIN )
    message( "Host Os: Windows" )
elseif( NATUS_HOST_OS_LIN )
elseif( NATUS_HOST_OS_MAC )
endif()

#######################################################################################
# Target os
#######################################################################################
message( STATUS "+++ Determine Target Operating System +++ " )

if( CMAKE_SYSTEM_NAME STREQUAL "Windows" OR
    CMAKE_SYSTEM_NAME STREQUAL "WindowsStore" )

    set( NATUS_TARGET_OS_WIN_NAME_ "unknown" )
    
    if( NATUS_HOST_OS_WIN7 )
        set( NATUS_TARGET_OS_WIN_NAME_ "Windows 7" )
    elseif( NATUS_HOST_OS_WIN81 )
        set( NATUS_TARGET_OS_WIN_NAME_ "Windows 8.1" )
    elseif( NATUS_HOST_OS_WIN10 )
        set( NATUS_TARGET_OS_WIN_NAME_ "Windows 10" )
    endif() 
    
    set( NATUS_WINDOWS_NAMES "Windows 8.1" "Windows 10" )
    set( NATUS_TARGET_OS_WIN_NAME ${NATUS_TARGET_OS_WIN_NAME_} CACHE STRING "Select your windows target for this application/library." )
    
    set_property( CACHE NATUS_TARGET_OS_WIN_NAME PROPERTY STRINGS ${NATUS_WINDOWS_NAMES} )

    if( ${NATUS_TARGET_OS_WIN_NAME} STREQUAL "Windows 7" )
        set( NATUS_TARGET_OS_WIN7 ON )
    elseif( ${NATUS_TARGET_OS_WIN_NAME} STREQUAL "Windows 8" )
        set( NATUS_TARGET_OS_WIN8 ON )
        message( FATAL_ERROR "Windows 8 not supported at the moment. Please use Windows 8.1" )
    elseif( ${NATUS_TARGET_OS_WIN_NAME} STREQUAL "Windows 8.1" )
        set( NATUS_TARGET_OS_WIN81 ON )
    elseif( ${NATUS_TARGET_OS_WIN_NAME} STREQUAL "Windows 10" )
      set( NATUS_TARGET_OS_WIN10 ON )
    else()
        message( FATAL_ERROR "Unknown Windows target" )
    endif()

    set( NATUS_TARGET_OS_WIN ON )

    unset(NATUS_WINDOWS_NAMES)

elseif( CMAKE_SYSTEM_NAME STREQUAL "Linux" )

    set( NATUS_TARGET_OS_LIN ON )

elseif( CMAKE_SYSTEM_NAME STREQUAL "Apple" )

    set( NATUS_TARGET_OS_MAC ON )

else()
    message( FATAL_ERROR "Unknown Target Operating System" )
endif()

#
# Determine definitions
# These compiler definitions can be platform dependent.
#
if( NATUS_TARGET_OS_WIN )

    add_definitions( -DNATUS_TARGET_OS_WIN )
    add_definitions( -DNOMINMAX )

    if( NATUS_TARGET_OS_WIN7 )
        # _WIN32_WINNT_WIN7 = 0x0601
        add_definitions( -DWINVER=0x0601 )
        add_definitions( -D_WIN32_WINNT=0x0601 )
        add_definitions( -DNATUS_TARGET_OS_WIN7 )
    elseif( NATUS_TARGET_OS_WIN8 )
        # _WIN32_WINNT_WIN8 = 0x0602
        add_definitions( -DWINVER=0x0602 )
        add_definitions( -D_WIN32_WINNT=0x0602 )
        add_definitions( -DNATUS_TARGET_OS_WIN8 )
    elseif( NATUS_TARGET_OS_WIN81 )
        # _WIN32_WINNT_WINBLUE (8.1) = 0x0602 
        add_definitions( -DWINVER=0x0602 )
        add_definitions( -D_WIN32_WINNT=0x0602 )
        add_definitions( -DNATUS_TARGET_OS_WIN81 )
    elseif( NATUS_TARGET_OS_WIN10 )
      # _WIN32_WINNT_WIN10 (10) = 0x0A00 
        add_definitions( -DWINVER=0x0A00 )
        add_definitions( -D_WIN32_WINNT=0x0A00 )
        add_definitions( -DNATUS_TARGET_OS_WIN10 )
    endif()

elseif( NATUS_TARGET_OS_LIN )

    add_definitions( -DNATUS_TARGET_OS_LIN )

elseif( NATUS_TARGET_OS_MAC )
    message( FATAL_ERROR "Changes Required" )
endif()

#
# Find Platform SDKs
#
if( NATUS_TARGET_OS_WIN )

  include( config_platform_sdk )

endif()

#
# Print Info
#
if( NATUS_TARGET_OS_WIN )

    if( NATUS_TARGET_OS_WIN7 )
        message( STATUS "Windows Target: Windows 7")
    elseif( NATUS_TARGET_OS_WIN8 )
        message( STATUS "Windows Target: Windows 8")
    elseif( NATUS_TARGET_OS_WIN81 )
        message( STATUS "Windows Target: Windows 8.1")
    elseif( NATUS_TARGET_OS_WIN10 )
      message( STATUS "Windows Target: Windows 10" )
    endif()

elseif( NATUS_TARGET_OS_LIN )

    message( STATUS "Linux Target: Linux")

elseif( NATUS_TARGET_OS_MAC )

endif()

set( NATUS_OS_CONFIGURED TRUE )

