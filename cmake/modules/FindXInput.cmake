
set( XINPUT_FOUND False )

if( NATUS_TARGET_OS_WIN7 )
    message( STATUS "[XInput] : Hint Win7 SDK" )

    if( NATUS_TARGET_ARCH_BIT_64 )
        set( INT_ARCH "/x64" ) 
    endif()

    set( SEARCH_XINPUT_INCLUDE_PATHS 
        "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Include"
        "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A/Include"
        $ENV{XINPUT_INCLUDEDIR_UM}
    )

    set( XINPUT_LIBRARIES_SEARCH_PATHS 
        "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib${INT_ARCH}"
        "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A/Lib${INT_ARCH}"
        $ENV{XINPUT_LIBRARYDIR_UM}
    )
    
elseif( NATUS_TARGET_OS_WIN81 OR NATUS_TARGET_OS_WIN10 )
    message( STATUS "[XInput] : Hint Win8 SDK" )
        
    if( NATUS_TARGET_ARCH_BIT_64 )
        set( INT_ARCH "/x64" ) 
    endif()

    set( SEARCH_XINPUT_INCLUDE_PATHS 
        "C:/Program Files (x86)/Windows Kits/8.1/Include/um"
        "c:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/um"
        $ENV{XINPUT_INCLUDEDIR_UM}
    )

    set( XINPUT_LIBRARIES_SEARCH_PATHS 
        "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um${INT_ARCH}"
        "c:/Program Files (x86)/Windows Kits/10/Lib/10.0.18362.0/um${INT_ARCH}"
        $ENV{XINPUT_LIBRARYDIR_UM}
    )
endif()

set( SEARCH_XINPUT_INCLUDE_PATHS
    ${SEARCH_XINPUT_INCLUDE_PATHS}	
)

find_path( XINPUT_INCLUDE_DIRECTORY 
    NAMES "Xinput.h"
    HINTS ${SEARCH_XINPUT_INCLUDE_PATHS} 
    )

if( NOT XINPUT_INCLUDE_DIRECTORY )
    message( FATAL_ERROR "xinput include directory required" ) 
endif()

set( XINPUT_INCLUDE_DIRS 
    ${XINPUT_INCLUDE_DIRECTORY}
)

message( STATUS "xinput: " ${XINPUT_INCLUDE_DIRECTORY})

#windows 7
# look under C:\Program Files (x86)\Microsoft SDKs\Windows
#include_directories( ${XINPUT_INCLUDE_DIRECTORY} )
#include_directories( ${XINPUT_INCLUDE_DIRECTORY}/../shared )
#include_directories( ${XINPUT_INCLUDE_DIRECTORY}/../winrt )

#windows 8
#look under C:\Program Files (x86)\Windows Kits
#include_directories( ${XINPUT_INCLUDE_DIRECTORY} )

set( XINPUT_LIBRARIES_SEARCH_PATHS 
    ${XINPUT_LIBRARIES_SEARCH_PATHS}
    )

find_library( XINPUT_LIBRARY_GENERAL 
    NAMES "xinput" 
    HINTS ${XINPUT_LIBRARIES_SEARCH_PATHS} 
    )

if( NOT XINPUT_LIBRARY_GENERAL )
    message( FATAL_ERROR "xinput Library required" )
endif()

set( XINPUT_LIBRARIES 
    general ${XINPUT_LIBRARY_GENERAL}
)

add_library( xinput INTERFACE )
target_include_directories( xinput INTERFACE ${XINPUT_INCLUDE_DIRS} )
target_link_libraries( xinput INTERFACE ${XINPUT_LIBRARY_GENERAL} )

unset( XINPUT_INCLUDE_DIRECTORY CACHE )
unset( XINPUT_LIBRARY_GENERAL CACHE )

set( XINPUT_FOUND True )

