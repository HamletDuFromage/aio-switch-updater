#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
BUILD		:=	build
SOURCES		:=	source lib/zipper/source
RESOURCES	:=	resources
DATA		:=	data
INCLUDES	:=	include lib/zipper/include /lib/borealis/library/include/borealis/extern/nlohmann
APP_TITLE	:=	All-in-One Switch Updater
APP_AUTHOR	:=	HamletDuFromage
APP_VERSION :=  2.18.1
TARGET		:=	$(notdir $(CURDIR))

ROMFS				:=	resources
BOREALIS_PATH		:=	lib/borealis
BOREALIS_RESOURCES	:=	romfs:/
#APP_RESOURCES		:=	romfs:/

#---------------------------------------------------------------------------------
# version control constants
#---------------------------------------------------------------------------------
#TARGET_VERSION	:= $(shell git describe --dirty --always --tags)
#APP_VERSION		:= $(TARGET_VERSION)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections \
			$(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__ \
			-DBOREALIS_RESOURCES="\"$(BOREALIS_RESOURCES)\"" \
			-DAPP_VERSION="\"$(APP_VERSION)\"" \
			-DAPP_TITLE="\"$(APP_TITLE)\"" -DAPP_TITLE_LOWER="\"$(TARGET)\""


CXXFLAGS	:= $(CFLAGS) -std=gnu++20 -fexceptions -Wno-reorder

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lm -lcurl -lnx -lz -lmbedtls -lmbedx509 -lmbedcrypto -lstdc++fs

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)

include $(TOPDIR)/$(BOREALIS_PATH)/library/borealis.mk

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(CONFIG_JSON)),)
	jsons := $(wildcard *.json)
	ifneq (,$(findstring $(TARGET).json,$(jsons)))
		export APP_JSON := $(TOPDIR)/$(TARGET).json
	else
		ifneq (,$(findstring config.json,$(jsons)))
			export APP_JSON := $(TOPDIR)/config.json
		endif
	endif
else
	export APP_JSON := $(TOPDIR)/$(CONFIG_JSON)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) $(ROMFS) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(ROMFS):
	@[ -d $@ ] || mkdir -p $@
	@echo Merging ROMFS...
	@cp -ruf $(CURDIR)/$(BOREALIS_PATH)/resources/i18n/. $(CURDIR)/$(ROMFS)/i18n/
	@cp -ruf $(CURDIR)/$(ROMFS)/i18n/zh-CN/. $(CURDIR)/$(ROMFS)/i18n/zh-Hans/
	@cp -ruf $(CURDIR)/$(ROMFS)/i18n/zh-TW/. $(CURDIR)/$(ROMFS)/i18n/zh-Hant/
	@rm -rf $(CURDIR)/$(ROMFS)/i18n/*/installer.json $(CURDIR)/$(ROMFS)/i18n/*/main.json $(CURDIR)/$(ROMFS)/i18n/*/popup.json $(CURDIR)/$(ROMFS)/i18n/*/custom_layout.json
	@$(MAKE) -C $(CURDIR)/aiosu-rcm -f $(CURDIR)/aiosu-rcm/Makefile
	@cp $(CURDIR)/aiosu-rcm/output/aio_rcm.bin $(CURDIR)/$(ROMFS)/aio_rcm.bin
# @$(MAKE) -C $(CURDIR)/aiosu-forwarder -f $(CURDIR)/aiosu-forwarder/Makefile
	@cp $(CURDIR)/aiosu-forwarder/aiosu-forwarder.nro $(CURDIR)/$(ROMFS)/aiosu-forwarder.nro

$(BUILD): $(ROMFS)
	@[ -d $@ ] || mkdir -p $@
	@MSYS2_ARG_CONV_EXCL="-D;$(MSYS2_ARG_CONV_EXCL)" $(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
ifeq ($(strip $(APP_JSON)),)
	@rm -fr $(BUILD) $(notdir $(CURDIR))*.nro $(notdir $(CURDIR))*.nacp $(notdir $(CURDIR))*.elf
# @rm -fr $(CURDIR)/aiosu-forwarder/build $(CURDIR)/aiosu-forwarder/*.nro $(CURDIR)/aiosu-forwarder/*.nacp $(CURDIR)/aiosu-forwarder/*.elf
else
	@rm -fr $(BUILD) $(TARGET).nsp $(TARGET).nso $(TARGET).npdm $(TARGET).elf
endif


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
ifeq ($(strip $(APP_JSON)),)

all	:	$(OUTPUT).nro

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

else

all	:	$(OUTPUT).nsp

$(OUTPUT).nsp	:	$(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).nso	:	$(OUTPUT).elf

endif

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
