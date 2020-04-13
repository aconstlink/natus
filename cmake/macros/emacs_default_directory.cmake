
macro( natus_emacs_default_directory NATUS_SOURCE_DIR )

configure_file( 
  ${NATUS_SOURCE_DIR}/cmake/configure/emacs.build.make.in.el 
  ${PROJECT_BINARY_DIR}/emacs.build.make.el )


endmacro()
