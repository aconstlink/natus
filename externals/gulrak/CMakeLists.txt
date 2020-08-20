
set( GHC_FILESYSTEM_WITH_INSTALL ON )


add_subdirectory( "filesystem" )

#install_library( ghc_filesystem ${PROJECT_NAME}-targets )

install( TARGETS ghc_filesystem 
        EXPORT ${PROJECT_NAME}-targets
        RUNTIME DESTINATION bin 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
    install( EXPORT ${PROJECT_NAME}-targets DESTINATION "cmake" )

