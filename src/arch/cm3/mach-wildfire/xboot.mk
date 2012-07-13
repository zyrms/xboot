#
# machine makefile.
#

ifeq ($(TARGET), cm3-wildfire)

CROSS		?= arm-none-eabi-

ASFLAGS		:= -g -ggdb -Wall
CFLAGS		:= -g -ggdb -Wall
CXXFLAGS	:= -g -ggdb -Wall
LDFLAGS		:= -T arch/$(ARCH)/$(MACH)/xboot.ld -nostartfiles
ARFLAGS		:= -rcs
OCFLAGS		:= -v -O binary
ODFLAGS		:=
MCFLAGS		:= -mcpu=cortex-m3 -mthumb

LIBDIRS		:=
LIBS 		:=

INCDIRS		+=	-I arch/$(ARCH)/$(MACH)/library/firmware/inc \
				-I arch/$(ARCH)/$(MACH)/library/cmsis \
				-I arch/$(ARCH)/$(MACH)/library/user
				
SRCDIRS		+=	arch/$(ARCH)/$(MACH)/library/firmware/src \
				arch/$(ARCH)/$(MACH)/library/cmsis \
				arch/$(ARCH)/$(MACH)/library/user

ASFLAGS		+= 	-D USE_STDPERIPH_DRIVER=1 -D STM32F10X_HD=1
CFLAGS		+= 	-D USE_STDPERIPH_DRIVER=1 -D STM32F10X_HD=1
CXXFLAGS	+=	-D USE_STDPERIPH_DRIVER=1 -D STM32F10X_HD=1


#
# add fixup rule
#
fixup:
	@echo make header information for irom booting
	@$(OC) -O ihex $(OUTDIR)/$(XBOOT).elf $(OUTDIR)/$(XBOOT).hex
	
endif