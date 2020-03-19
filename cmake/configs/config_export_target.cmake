

macro( natus_export name )

  #message( ${name} )
  export( TARGETS ${name} NAMESPACE natus:: APPEND FILE ${NATUS_BINARY_DIR}/natus-targets.cmake )

endmacro()

macro( natus_export_clear )

file( REMOVE ${NATUS_BINARY_DIR}/natus-targets.cmake )

endmacro()
