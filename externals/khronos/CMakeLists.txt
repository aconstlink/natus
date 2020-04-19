


install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/GL" DESTINATION "include/natus/externals/khronos/include")
install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/GLES2" DESTINATION "include/natus/externals/khronos/include")
install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/GLES3" DESTINATION "include/natus/externals/khronos/include")
install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/KHR" DESTINATION "include/natus/externals/khronos/include")

add_library( gl_headers INTERFACE )
target_include_directories( gl_headers INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> )
target_include_directories( gl_headers INTERFACE $<INSTALL_INTERFACE:include/natus/externals/khronos/include> )
install_library( gl_headers ${PROJECT_NAME}-targets )

add_library( gles2_header INTERFACE )
target_include_directories( gles2_header INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> )
target_include_directories( gles2_header INTERFACE $<INSTALL_INTERFACE:include/natus/externals/khronos/include> )
install_library( gles2_header ${PROJECT_NAME}-targets )

add_library( gles3_header INTERFACE )
target_include_directories( gles3_header INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include> )
target_include_directories( gles3_header INTERFACE $<INSTALL_INTERFACE:include/natus/externals/khronos/include> )
install_library( gles3_header ${PROJECT_NAME}-targets )


