
#
# Configures the build configuration
#
set( NATUS_BUILD_CONFIG_CONFIGURED FALSE )

#
# Required other config modules
#
if( NOT NATUS_COMPILER_CONFIGURED )
    message( FATAL_ERROR "Config the build configuration requires the compiler to be configured." )
elseif( NOT NATUS_BUILD_ENV_CONFIGURED )
    message( FATAL_ERROR "Config the build configuration requires the build environment to be configured." )
endif()

message( STATUS "[config] : build configuration" )

# Do we have microsoft visual studio, the build config requires all flags
# to be set in the compiler option variable because visual studio allows 
# multiple build configurations.
if( NATUS_COMPILER_MSC )

    #set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /DNATUS_DEBUG" )
    #set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DNATUS_DEBUG" )

    #set( CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /DNATUS_RELEASE")	
    #set( CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /DNATUS_RELEASE")	

    #set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DNATUS_RELEASE")	

    #set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DNATUS_RELEASE")	
    #set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /DNATUS_RELEASE")	

    #set( CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /DNATUS_RELEASE")
    #set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /DNATUS_RELEASE")

# with a make system, we can only have one configuration at a time.
elseif( NATUS_COMPILER_GNU )

    #
    #
    #
    set( NATUS_BUILD_CONFIG_DEBUG OFF )
    set( NATUS_BUILD_CONFIG_RELEASE OFF ) 

    #
    # Section: Preset based on CMake
    #
    if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        set( NATUS_BUILD_CONFIG_NAME "Debug" )
    elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
        set( NATUS_BUILD_CONFIG_NAME "Release" )
    else()
        set( NATUS_BUILD_CONFIG_NAME "Debug" )
    endif()

    #
    # Section: User Option
    #
    set( NATUS_BUILD_CONFIG_NAMES "Debug" "Release" )
    set( NATUS_BUILD_CONFIG_NAME ${NATUS_BUILD_CONFIG_NAME} CACHE STRING "Select your build configuration." )
    set_property( CACHE NATUS_BUILD_CONFIG_NAME PROPERTY STRINGS ${NATUS_BUILD_CONFIG_NAMES} )

    #
    # Reset CMake variable
    #
    set( CMAKE_BUILD_TYPE ${NATUS_BUILD_CONFIG_NAME} CACHE STRING ${NATUS_BUILD_CONFIG_NAME} FORCE )

    #
    # Section: Definitions
    #
    if( NATUS_BUILD_CONFIG_NAME STREQUAL "Debug" )
        
        set( NATUS_BUILD_CONFIG_DEBUG ON )
        #add_definitions( -DNATUS_DEBUG )
        
    elseif( NATUS_BUILD_CONFIG_NAME STREQUAL "Release" )

        set( NATUS_BUILD_CONFIG_RELEASE ON )
        #add_definitions( -DNATUS_RELEASE )
        
    endif()

endif()

message( STATUS "[done] : Build Comfiguration" )

set( NATUS_BUILD_CONFIG_CONFIGURED TRUE )

