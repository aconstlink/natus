
add_subdirectory( "gulrak" )

#set_target_properties( ghc_filesystem PROPERTIES FOLDER "natus/externals" )

install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/gulrak/include/ghc" DESTINATION "include/natus/externals/gulrak/include")
