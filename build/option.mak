##########################################################################
# File:             option.mak
# Description:      System compile file and DO NOT modigy it at will.
# Version:          0.1 
# Author:           Maobin
# DATE              NAME                        DESCRIPTION
# 2013-01-10        Maobin                      Initial Version 0.1
##########################################################################
ifneq ("$(CFG)","")
include  $(PROJ_HOME)/build/$(CFG).mak
else
include  $(PROJ_HOME)/build/user.mak
endif

DIR_ARM := $(strip $(DIR_ARM))
DIR_TOOL       =  $(DIR_ARM)\Programs\3.1\569\win_32-pentium
DIR_ARMLIB     =  $(DIR_ARM)\Data\3.1\569\lib
DIR_ARMINC     =  $(DIR_ARM)\Data\3.1\569\include\windows

CC := $(DIR_TOOL)/armcc
ASM := $(DIR_TOOL)/armasm
AR := $(DIR_TOOL)/armar
LINK := $(DIR_TOOL)/armlink
FE := $(DIR_TOOL)/fromelf
MAKE := make
MAKE2 := make clean

BIN_ELF= $(BIN).elf

##800W 64M Flash##
ifneq ("$(findstring W64,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM8xxW64.txt
FLASH_TYPE = WINBOND
HARDWARE_PLATFORM = SIM800W
endif

##800W 128M Flash##
ifneq ("$(findstring W128,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM8xxW128.txt
FLASH_TYPE = MXIC
HARDWARE_PLATFORM = SIM800W
endif

##800V 128M Flash##
ifneq ("$(findstring V128,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM8xxV128.txt
FLASH_TYPE = MXIC
HARDWARE_PLATFORM = SIM800V
endif

##800V 64M Flash##
ifneq ("$(findstring V64,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM8xxV64.txt
FLASH_TYPE = WINBOND
HARDWARE_PLATFORM = SIM800V
endif

ifneq ("$(findstring H32,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM8xxH32.txt
  ifeq ($(strip $(FEA_APP_MULTI_APP_INDEX_APP_OPT)),-D__EAT_SECOND_APP__)
    SRC_SCAT_FILE = scat_SIM8xxH32_SECOND.txt
  endif
HARDWARE_PLATFORM = SIM800H
endif

ifneq ("$(findstring L16,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM8xxL16.txt
  ifeq ($(strip $(FEA_APP_MULTI_APP_INDEX_APP_OPT)),-D__EAT_SECOND_APP__)
    SRC_SCAT_FILE = scat_SIM8xxL16_SECOND.txt
  endif
HARDWARE_PLATFORM = SIM800H
endif

ifneq ("$(findstring 800M32,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM8xxH32.txt
HARDWARE_PLATFORM = SIM800
endif

ifneq ("$(findstring 808M32,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM808M32.txt
HARDWARE_PLATFORM = SIM808
endif

ifneq ("$(findstring 800C24,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM800C24.txt
HARDWARE_PLATFORM = SIM800C
endif

ifneq ("$(findstring 800C32,$(PROJ))","")
SRC_SCAT_FILE = scat_SIM800C32.txt
HARDWARE_PLATFORM = SIM800C
endif

#CORE提供的SYM文件名
#ifeq ($(strip $(HARDWARE_PLATFORM)),SIM800H)
    SRC_CORE_SYM_FILE = $(PROJ)_EAT_PCB01_gprs_MT6261_S00_limit.sym
#    SRC_CORE_SYM_FILE = $(PROJ)_EAT_PCB01_gprs_MT6260_S00.sym
    CORE_BIN_DIR = $(CORE_DIR)/$(PROJ)_EMBEDDEDAT
    CORE_BIN_CFG_FILE = $(PROJ)_EAT.cfg
#else
#    SRC_CORE_SYM_FILE = $(PROJ)_$(FLASH_TYPE)_EAT_PCB01_gprs_MT6252_S01.sym
#	CORE_BIN_DIR = $(CORE_DIR)/$(PROJ)_$(FLASH_TYPE)_EMBEDDEDAT
#endif

CORE_SYM_FILE= $(CORE_BIN_DIR)/$(SRC_CORE_SYM_FILE)

#CORE_SYM_FIEL = 
BIN_DIR = $(PROJ_HOME)/$(OUTPUT_BIN_DIR)
LIB_DIR = $(PROJ_HOME)/$(OUTPUT_LIB_DIR)
LOG_DIR = $(PROJ_HOME)/$(OUTPUT_LOG_DIR)

BIN_NAME = $(OUTPUT_BIN_DIR)/$(BIN)
ELF_NAME = $(OUTPUT_BIN_DIR)/$(BIN_ELF)

BUILD_DIR = $(PROJ_HOME)/build

LINK_FLAGS_TMP := --cpu ARM7EJ-S --map --info sizes,totals --symbols --feedback lnkfeedback.txt --xref --remove --pad 0x00 --datacompressor off --largeregions  
CFLAGS_TMP := --thumb --cpu ARM7EJ-S --littleend -O3 --remove_unneeded_entities --split_sections
ASMFLAGS_TMP := --16 --cpu ARM7EJ-S --apcs=/interwork --littleend -O3
#r replace specified files
#u update to more recent files
ARFLAGS_TMP := -ru

OUTPUT_LIST_FILE = $(BIN_NAME).lis
OUTPUT_SYM_FILE = $(BIN_NAME).sym

OUTPUT_FLAG = --output $(ELF_NAME)
SYM_FLAG = --symdefs $(OUTPUT_SYM_FILE)
LIST_FLAG = --list $(OUTPUT_LIST_FILE) 

LIB_PATH = --libpath $(DIR_ARMLIB)
INC_PATH = $(DIR_ARMINC)

# Scatter#
SCAT_FLAG = --scatter $(CORE_DIR)\$(SRC_SCAT_FILE) #$(OUTPUT_BIN_DIR)\$(SCATTER_FILE)

#head file dir
CORE_INC=$(PROJ_HOME)/$(CORE_DIR)/inc
SRC_INC = $(foreach n,$(SRC_OPEN_MODE_NAME) ,-I$(PROJ_HOME)/$(SRC_DIR)/$(n) )

INC_FLAG =-I$(INC_PATH)
INC_FLAG += -I$(CORE_INC)
INC_FLAG += $(SRC_INC)

#Core lib
CORE_LIB = $(CORE_DIR)\core.lib \
		   $(CORE_SYM_FILE)
	       
#system hardware platform
FEA_HARDWARE_PLATFORM_OPT = -D__SIMCOM_PROJ_$(HARDWARE_PLATFORM)__ \
                                               -D__USB_COM_PORT_ENABLE__
#compiler flags#
FEA_SYS_MODULE_OPT= $(FEA_HARDWARE_PLATFORM_OPT)

SYS_CFLAGS = $(FEA_APP_DEBUG_OPT)

SYS_CFLAGS += $(FEA_SYS_MODULE_OPT)
SYS_CFLAGS += $(FEA_APP_CFLAGS)

#CC flag#
CFLAGS = $(CFLAGS_TMP) $(SYS_CFLAGS) $(INC_FLAG)
#ASM flag#
ASMFLAGS = $(ASMFLAGS_TMP)
#AR flag#
ARFLAGS = $(ARFLAGS_TMP)
#LINK flag#
LINK_FLAGS = $(LINK_FLAGS_TMP) $(SCAT_FLAG) $(OUTPUT_FLAG) $(SYM_FLAG) $(LIST_FLAG) $(LIB_PATH) $(CORE_LIB)

