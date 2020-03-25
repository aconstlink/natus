
set( GHC_FILESYSTEM_WITH_INSTALL ON )

add_subdirectory( "gulrak" )

#set_target_properties( ghc_filesystem PROPERTIES FOLDER "natus/externals" )

install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/gulrak/include/ghc" DESTINATION "include/natus/externals/gulrak/include")

install( TARGETS ghc_filesystem EXPORT ${PROJECT_NAME}-targets )

#export( TARGETS ghc_filesystem APPEND FILE ${PROJECT_NAME}-targets.cmake)