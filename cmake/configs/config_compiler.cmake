

#
# determine the compiler
# for cross compilation, the target platform is required to be known
# 
# the values here can be initialized using a toolchain file

set( NATUS_COMPILER_CONFIGURED FALSE )

#just set the cxx version to 11 for now
#it need to be used in the target property CXX_STANDARD
set( NATUS_CXX_STANDARD 11 )

# Microsoft compiler
set( NATUS_COMPILER_MSC OFF )
set( NATUS_COMPILER_MSC_14 OFF ) # vs 2015
set( NATUS_COMPILER_MSC_15 OFF ) # vs 2017
set( NATUS_COMPILER_MSC_16 OFF ) # vs 2019
set( NATUS_CXX_STANDARD 17 )

# Gnu compiler (GCC, GCC-C++)
set( NATUS_COMPILER_GNU OFF )
set( NATUS_COMPILER_CLANG OFF )

if( MSVC_IDE OR MSVC )

    set( NATUS_COMPILER_MSC ON )
    
    if( MSVC_VERSION EQUAL 1900 )
      set( NATUS_COMPILER_MSC_14 on )
      set( NATUS_CXX_STANDARD 11 )
    elseif( MSVC_VERSION GREATER 1909 AND MSVC_VERSION LESS 1920 )
      set( NATUS_COMPILER_MSC_15 on )
      set( NATUS_CXX_STANDARD 14 )
    elseif( MSVC_VERSION GREATER 1919 AND MSVC_VERSION LESS 1930 )
      set( NATUS_COMPILER_MSC_16 on )
      set( NATUS_CXX_STANDARD 17 )
    else()
      message( FATAL "MSVC Compiler not yet supported" )
    endif()

    #add_definitions( -DNATUS_COMPILER_MSC )
    #set( NATUS_COMPILER_DEFINITION_PUBLIC NATUS_COMPILER_MSC )

elseif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

    set( NATUS_COMPILER_GNU ON )
    #add_definitions( -DNATUS_COMPILER_GNU )
else()

    message( FATAL_ERROR "Unsupported compiler")

endif()

#
# Print Info
#
if( NATUS_COMPILER_MSC )

    message( STATUS "[compiler] : Microsoft Compiler Suite" )

elseif( NATUS_COMPILER_GNU )

    message( STATUS "[compiler] : GNU Compiler Suite" )

endif()

set( CMAKE_CXX_STANDARD ${NATUS_CXX_STANDARD} )
set( NATUS_COMPILER_CONFIGURED TRUE )

