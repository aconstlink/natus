
set( sources 
    "api.h"
    "result.h"
    "typedefs.h"

    "physical.hpp"
    "component.hpp"
    "system.h"
    "system.cpp"
    
    "imodule.h"
    
    "components/button.hpp"
    "components/key.hpp"
    "components/knob.hpp"
    "components/led.hpp"
    "components/light.hpp"
    "components/motor.hpp"
    "components/point.hpp"
    "components/scroll.hpp"
    "components/slider.hpp"
    "components/stick.hpp"
    "components/touch.hpp"
    )


set( win32_sources 
    "modules/win32/rawinput_module.h"
    "modules/win32/rawinput_module.cpp"
    "modules/win32/xinput_module.h"
    "modules/win32/xinput_module.cpp"
)

set( unwanted_compiler_options )
set( os_libs )

if( NATUS_TARGET_OS_WIN )
  list( APPEND sources ${win32_sources} )
  set( os_libs winmm.lib xinput
 )

  if( NATUS_COMPILER_MSC )
        # warning C4005 : '__useHeader' : macro redefinition 
        # Note: C4005 seem to be a redefinition bug in the window header files.
        set( unwanted_compiler_options "/wd4005" )
    endif()
endif()

natus_vs_src_dir( sources )

add_library( ${cur_lib_name} ${NATUS_LIBRARY_BUILD_TYPE} ${sources} )
add_library( ${cur_alias_name} ALIAS ${cur_lib_name} ) 
target_compile_options( ${cur_lib_name} PRIVATE ${unwanted_compiler_options} )

target_link_libraries( ${cur_lib_name}
  PRIVATE ${os_libs} 
  PUBLIC rtmidi
  PUBLIC natus::memory
  PUBLIC natus::log 
    )

###########################################################
# SECTION: Build Tree
###########################################################

##natus_export( ${cur_lib_name} )

###########################################################
# SECTION: Install
###########################################################

install_headers( "${sources}" "include/${PROJECT_NAME}/${cur_lib_name}" )
install_library( ${cur_lib_name} ${PROJECT_NAME}-targets )

