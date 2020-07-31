
if( MSVC )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4244 /wd4324 /wd4310" )
endif()

set( LUA_BUILD_TYPE STATIC )
option( LUA_BUILD_AS_DLL "Build lua as dynamically linked library" OFF )


set( CMAKE_DEBUG_POSTFIX d )

file( GLOB INCLUDES
  "lua/*.h" )

file( GLOB SOURCES
  "lua/*.c" )

list( REMOVE_ITEM SOURCES "${CMAKE_CURRENT_LIST_DIR}/lua/onelua.c")

set( SOURCES 
    ${SOURCES} 
    ${INCLUDES}
)

if( LUA_BUILD_AS_DLL )    
    set( LUA_BUILD_TYPE SHARED )
endif()

add_library( lua ${LUA_BUILD_TYPE} ${SOURCES} )
set_target_properties( lua PROPERTIES LINKER_LANGUAGE C )

target_include_directories( lua
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  PUBLIC $<INSTALL_INTERFACE:"include/${PROJECT_NAME}/externals"> ) 

if( LUA_BUILD_AS_DLL )
    target_compile_options( lua PUBLIC -LUA_BUILD_AS_DLL -LUA_USE_LONGJMP) 
endif()

if( NATUS_TARGET_OS_WIN )
  target_compile_options( lua PUBLIC -LUA_USE_WINDOWS )
elseif( NATUS_TARGET_OS_LIN )
  target_compile_options( lua PUBLIC -LUA_USE_LINUX )
  target_link_libraries( lua PUBLIC m )
endif()
add_executable( onelua ${INCLUDES} "lua/onelua.c" )
target_link_libraries( onelua PRIVATE lua )
set_target_properties( onelua PROPERTIES LINKER_LANGUAGE C )

unset( LUA_BUILD_AS_DLL CACHE )
set_target_properties( lua PROPERTIES FOLDER "natus/externals/lua" )
set_target_properties( onelua PROPERTIES FOLDER "natus/externals/lua" )

###########################################################
# SECTION: Install
###########################################################

#install_headers( "${INCLUDES}" "include/${PROJECT_NAME}/externals/lua" )
#install_library( lua ${PROJECT_NAME}-targets )
