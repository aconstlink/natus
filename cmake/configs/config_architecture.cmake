

#
# determine/option the host and target architecture
#
# the values here can be initialized using a toolchain file

set( NATUS_ARCH_CONFIGURED FALSE )

set( NATUS_HOST_ARCH "Unknown" )
set( NATUS_HOST_ARCH_X86 OFF )
set( NATUS_HOST_ARCH_BIT_32 OFF )
set( NATUS_HOST_ARCH_BIT_64 OFF )


set( NATUS_TARGET_ARCH "Unknown" )
set( NATUS_TARGET_ARCH_X86 ON ) # default
set( NATUS_TARGET_ARCH_ARM OFF )
set( NATUS_TARGET_ARCH_CBEA OFF )
set( NATUS_TARGET_ARCH_PPC OFF )
set( NATUS_TARGET_ARCH_MIPS OFF )
set( NATUS_TARGET_ARCH_BIT_32 OFF )
set( NATUS_TARGET_ARCH_BIT_64 ON ) #default

set( NATUS_ARCH_CONFIGURED TRUE )

