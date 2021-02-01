#set( RAPIDXML_INSTALL_HEADER_BASE "include/${PROJECT_NAME}/externals" )
#add_subdirectory( rapidxml )

add_library( rapidxml INTERFACE )
target_include_directories( rapidxml INTERFACE "rapidxml" )

#export( TARGETS rapidxml NAMESPACE natus:: APPEND FILE ${NATUS_BINARY_DIR}/snakeoil-targets.cmake )
