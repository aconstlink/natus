
set( GHC_FILESYSTEM_WITH_INSTALL ON )

add_subdirectory( "filesystem" )

#set_target_properties( ghc_filesystem PROPERTIES FOLDER "natus/externals" )

install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/filesystem/include/ghc" DESTINATION "include/natus/externals/filesystem/include")

#install( TARGETS ghc_filesystem EXPORT ${PROJECT_NAME}-targets )

#export( TARGETS ghc_filesystem APPEND FILE ${PROJECT_NAME}-targets.cmake)

add_library( ghc_filesystem_natus INTERFACE )
target_include_directories( ghc_filesystem_natus INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/filesystem/include> )
target_include_directories( ghc_filesystem_natus INTERFACE $<INSTALL_INTERFACE:include/natus/externals/filesystem/include> )
install_library( ghc_filesystem_natus ${PROJECT_NAME}-targets )
