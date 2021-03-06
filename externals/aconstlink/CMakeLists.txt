#set( RAPIDXML_INSTALL_HEADER_BASE "include/${PROJECT_NAME}/externals" )
#add_subdirectory( rapidxml )

add_library( rapidxml INTERFACE )
target_include_directories( rapidxml INTERFACE "rapidxml" )

#export( TARGETS rapidxml NAMESPACE natus:: APPEND FILE ${NATUS_BINARY_DIR}/snakeoil-targets.cmake )


#
# Khronos Header files
#
#
message( STATUS ">>> [external : Khronos Header] >>>" )


install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/khronos-header/GL" DESTINATION "khronos-header/natus/externals/khronos/include")
install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/khronos-header/GLES2" DESTINATION "khronos-header/natus/externals/khronos/include")
install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/khronos-header/GLES3" DESTINATION "khronos-header/natus/externals/khronos/include")
install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/khronos-header/KHR" DESTINATION "khronos-header/natus/externals/khronos/include")

add_library( gl_headers INTERFACE )
target_include_directories( gl_headers INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/khronos-header> )
target_include_directories( gl_headers INTERFACE $<INSTALL_INTERFACE:include/natus/externals/aconstlink/khronos-header> )
install_library( gl_headers ${PROJECT_NAME}-targets )

add_library( gles2_header INTERFACE )
target_include_directories( gles2_header INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/khronos-header> )
target_include_directories( gles2_header INTERFACE $<INSTALL_INTERFACE:include/natus/externals/aconstlink/khronos-header> )
install_library( gles2_header ${PROJECT_NAME}-targets )

add_library( gles3_header INTERFACE )
target_include_directories( gles3_header INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/khronos-header> )
target_include_directories( gles3_header INTERFACE $<INSTALL_INTERFACE:include/natus/externals/aconstlink/khronos-header> )
install_library( gles3_header ${PROJECT_NAME}-targets )

message( STATUS "<<< [external : Khronos Headers] <<<" )
