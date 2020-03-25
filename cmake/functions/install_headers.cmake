
function( install_headers FILE_LIST BASE_PATH )
    #message( "Will install to : " ${BASE_PATH} )
    #message( "Will install : " ${FILE_LIST} )
    
    foreach( FILE_NAME ${FILE_LIST} )

        get_filename_component( FNAME ${FILE_NAME} NAME )
        get_filename_component( FPATH ${FILE_NAME} DIRECTORY )
        
        # all .h, hxx and .hpp files
        # \\ escape sequence
        if( ${FNAME} MATCHES "\\.hx*p*$" )
            #message( "${FNAME} will install to : " "${BASE_PATH}/${FPATH}" )
            install( FILES ${FILE_NAME} DESTINATION "${BASE_PATH}/${FPATH}" )
        endif()
    endforeach() 

endfunction()
