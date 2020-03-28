


install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/GL" DESTINATION "include/natus/externals/khronos")
install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/GLES2" DESTINATION "include/natus/externals/khronos")
install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/GLES3" DESTINATION "include/natus/externals/khronos")
install( DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/KHR" DESTINATION "include/natus/externals/khronos")

add_library( gl_headers INTERFACE )
target_include_directories( gl_headers INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> )
target_include_directories( gl_headers INTERFACE $<INSTALL_INTERFACE:include/natus/externals/khronos> )
install_library( gl_headers ${PROJECT_NAME}-targets )

add_library( gles2_header INTERFACE )
target_include_directories( gles2_header INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> )
target_include_directories( gles2_header INTERFACE $<INSTALL_INTERFACE:include/natus/externals/khronos> )
install_library( gles2_header ${PROJECT_NAME}-targets )

add_library( gles3_header INTERFACE )
target_include_directories( gles3_header INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> )
target_include_directories( gles3_header INTERFACE $<INSTALL_INTERFACE:include/natus/externals/khronos> )
install_library( gles3_header ${PROJECT_NAME}-targets )