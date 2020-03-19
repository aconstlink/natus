
#
# run-time linkage check
#

##
## Run-Time Linkage
##

set( NATUS_RUNTIME_LINKAGE "Shared" CACHE STRING "How to link the C++ Runtime?" )
set_property(CACHE NATUS_RUNTIME_LINKAGE PROPERTY STRINGS "Static" "Shared")

if( ${NATUS_RUNTIME_LINKAGE} STREQUAL "Static" )
    set( NATUS_RUNTIME_STATIC TRUE )
    #add_definitions( -DNATUS_RUNTIME_STATIC )
elseif( ${NATUS_RUNTIME_LINKAGE} STREQUAL "Shared" )
    set( NATUS_RUNTIME_DYNAMIC TRUE )
    #add_definitions( -DNATUS_RUNTIME_DYNAMIC )
endif()

message( STATUS "Run-Time Linkage: " ${NATUS_RUNTIME_LINKAGE} )


