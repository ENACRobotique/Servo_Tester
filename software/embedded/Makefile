##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

OPT_MODE=DEBUG
#OPT_MODE=OPT

# Compiler options here.
# -Wdouble-promotion -fno-omit-frame-pointer
GCCVERSIONGTEQ7 := $(shell expr `arm-none-eabi-gcc -dumpversion | cut -f1 -d.` \>= 7)
GCC_DIAG =  -Werror -Wno-error=unused-variable -Wno-error=format \
            -Wno-error=unused-function \
            -Wunused -Wpointer-arith \
            -Werror=sign-compare \
            -Wshadow -Wparentheses -fmax-errors=5 \
            -ftrack-macro-expansion=2 -Wstrict-overflow=2

ifeq "$(GCCVERSIONGTEQ7)" "1"
    GCC_DIAG += -Wvla-larger-than=128 -Wduplicated-branches -Wdangling-else \
                -Wformat-overflow=2 -Wformat-truncation=2
endif


ifeq ($(OPT_MODE),DEBUG)
  USE_OPT =  -O0  -ggdb3  -Wall -Wextra \
            -falign-functions=16 -fomit-frame-pointer \
            $(GCC_DIAG)
  USE_LTO = no
else
  USE_OPT =  -Ofast  -Wall -Wextra \
            -falign-functions=16 -fomit-frame-pointer \
             $(GCC_DIAG)
  USE_LTO = yes
endif



# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = -std=gnu11  -Wunsuffixed-float-constants 
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -std=gnu++1y -fno-rtti -fno-exceptions 
endif

# Enable this if you want the linker to remove unused code and data
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT = 
endif

# Enable this if you want link time optimizations (LTO)
ifeq ($(USE_LTO),)
  USE_LTO = no
endif

# If enabled, this option allows to compile the application in THUMB mode.
ifeq ($(USE_THUMB),)
  USE_THUMB = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = yes
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#
# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x400
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x400
endif

# Enables the use of FPU (no, softfp, hard).
ifeq ($(USE_FPU),)
  USE_FPU = no
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, sources and paths
#

# Define project name here
PROJECT = testeur

# Imported source files and paths
CHIBISTUDIO=/home/fabien/CHIBISTUDIO
MY_DIRNAME=$(CHIBISTUDIO)/ChibiOS_stable

CFG = ./cfg
CHIBIOS = ./chibios_svn_mirror
#STMSRC = $(CHIBISTUDIO)/COMMON/stm
VARIOUS = ./chibios_enac_various_common
USBD_LIB = $(VARIOUS)/Chibios-USB-Devices
BOARDGEN = $(VARIOUS)/TOOLS/boardGen.pl
BMPFLASH = $(VARIOUS)/TOOLS/bmpflash


# Licensing files.
include $(CHIBIOS)/os/license/license.mk
# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32l4xx.mk
# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32L4xx/platform_l432.mk
include $(CFG)/board.mk
include $(CHIBIOS)/os/hal/osal/rt-nil/osal.mk
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk
# Auto-build files in ./source recursively.
include $(CHIBIOS)/tools/mk/autobuild.mk
# Other files (optional).
# include $(CHIBIOS)/os/test/test.mk
# include $(CHIBIOS)/test/rt/rt_test.mk
#include $(CHIBIOS)/os/oslib/oslib.mk


# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32L432xC.ld

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC += $(ALLCSRC) \
       $(CHIBIOS)/os/various/syscalls.c \
       $(VARIOUS)/stdutil.c \
       $(VARIOUS)/printf.c \
       $(VARIOUS)/microrl/microrlShell.c \
       $(VARIOUS)/microrl/microrl.c \
       $(VARIOUS)/esc_dshot.c \
       $(VARIOUS)/hal_stm32_dma.c

# $(STARTUPSRC) \
#        $(KERNSRC) \
#        $(PORTSRC) \
#        $(OSALSRC) \
#        $(HALSRC) \
#        $(PLATFORMSRC) \


# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC += $(ALLCPPSRC)

# C sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACSRC =

# C++ sources to be compiled in ARM mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
ACPPSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCSRC =

# C sources to be compiled in THUMB mode regardless of the global setting.
# NOTE: Mixing ARM and THUMB mode enables the -mthumb-interwork compiler
#       option that results in lower performance and larger code size.
TCPPSRC =

# List ASM source files here
ASMSRC =
ASMXSRC = $(STARTUPASM) $(PORTASM) $(OSALASM)

INCDIR += $(CHIBIOS)/os/license \
         $(STARTUPINC) $(KERNINC) $(PORTINC) $(OSALINC) \
         $(HALINC) $(PLATFORMINC) $(BOARDINC) $(TESTINC) \
         $(CHIBIOS)/os/various $(VARIOUS)

INCDIR += $(ALLINC)

#
# Project, sources and paths
##############################################################################

##############################################################################
# Compiler settings
#

MCU  = cortex-m4

#TRGT = arm-elf-
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
# Enable loading with g++ only if you need C++ runtime support.
# NOTE: You can use C++ even without C++ support if you are careful. C++
#       runtime support makes code size explode.
LD   = $(TRGT)gcc
#LD   = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
AR   = $(TRGT)ar
OD   = $(TRGT)objdump
SZ   = $(TRGT)size
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary

# ARM-specific options here
AOPT =

# THUMB-specific options here
TOPT = -mthumb -DTHUMB

# Define C warning options here
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes

# Define C++ warning options here
CPPWARN = -Wall -Wextra -Wundef

#
# Compiler settings
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS = -DTRACE -DGCCOPT=$(USE_OPT)

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

#
# End of user defines
##############################################################################

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/rules.mk
$(OBJS): $(CFG)/board.h


$(CFG)/board.h: $(CFG)/board.cfg Makefile
	$(BOARDGEN) --no-pp-pin --no-pp-line --no-adcp-in	$<  $@

stflash: all
	@echo write $(BUILDDIR)/$(PROJECT).bin to flash memory
	st-flash write $(BUILDDIR)/$(PROJECT).bin 0x08000000
	@echo Done

flash: all
	@echo write $(BUILDDIR)/$(PROJECT).bin to flash memory
	$(BMPFLASH)  $(BUILDDIR)/$(PROJECT).elf
	@echo Done
