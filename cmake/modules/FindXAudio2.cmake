
if( XAUDIO2_FOUND )
    return()
endif()

set( XAUDIO2_FOUND FALSE )

option( XAUDIO2_ENABLE "Enable DirectX XAudio2" ${XAUDIO_ENABLE_HINT} )

if( XAUDIO2_ENABLE )
    message( STATUS "[Enabled] : DirectX XAudio2" )
else()
    message( STATUS "[Disabled] : DirectX XAudio2" )
    return()
endif()

if( NATUS_TARGET_OS_WIN )

    set( XAUDIO2_DEFINITIONS "-DXAUDIO2_ENABLE" )
    
    # need to go through the directx sdk on windows 7
    if( NATUS_TARGET_OS_WIN7 )
        
        find_package( DirectXSdk )
        
        set( XAUDIO2_INCLUDEDIR ${DXSDK_INCLUDE_DIR} )
        set( XAUDIO2_LIBRARYDIR ${DXSDK_LIBRARY_DIR} )
        set( XAUDIO2_BINDIR ${DXSDK_BIN_DIR} )
        
        set( XAUDIO2_LIBRARIES winmm.lib ole32.lib )
        
        find_path( XAUDIO2_INCLUDE_DIRECTORY "xaudio2.h" HINTS ${XAUDIO2_INCLUDEDIR} )
        if( NOT XAUDIO2_INCLUDE_DIRECTORY )
            message( FATAL_ERROR "XAudio2.h not found in " ${XAUDIO2_INCLUDEDIR} ) 
        endif()
        
    elseif( NATUS_TARGET_OS_WIN8 )
    
    elseif( NATUS_TARGET_OS_WIN81 OR NATUS_TARGET_OS_WIN10 )
        
        find_package( Windows81Sdk )
        
        set( XAUDIO2_INCLUDEDIR ${WIN81_SDK_INCLUDE_DIR} )
        set( XAUDIO2_LIBRARYDIR ${WIN81_SDK_LIBRARY_DIR} )
        set( XAUDIO2_BINDIR ${DXSDK_BINDIR} )
        
        find_path( XAUDIO2_INCLUDE_DIRECTORY "xaudio2.h" HINTS ${XAUDIO2_INCLUDEDIR} )
        if( NOT XAUDIO2_INCLUDE_DIRECTORY )
            message( FATAL_ERROR "XAudio2.h not found in " ${XAUDIO2_INCLUDEDIR} ) 
        endif()
        
        find_library( XAUDIO2_LIB_GENERAL "xaudio2.lib" HINTS ${XAUDIO2_LIBRARYDIR} ) 
        if( NOT XAUDIO2_LIB_GENERAL )
            message( FATAL_ERROR "XAudio2.lib not found in " ${XAUDIO2_LIBRARYDIR} ) 
        endif()
        
        set( XAUDIO2_LIBRARIES winmm.lib ole32.lib ${XAUDIO2_LIB_GENERAL} )
        
    else()
      message( FATAL_ERROR "XAudio2 Path required" )
    endif()
    
endif()

set( XAUDIO2_FOUND TRUE )
