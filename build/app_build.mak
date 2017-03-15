##########################################################################
# File:             app_build.mak 
# Description:      System compile file and DO NOT modigy it at will.
# Version:          0.1 
# Author:           Maobin
# DATE              NAME                        DESCRIPTION
# 2013-01-10        Maobin                      Initial Version 0.1
# 2013-04-09        Maobin                      add assembly file compile
#########################################################################

#filter out main.c
ifneq ($(filter %main.c ,$(SOURCE)),)
MAIN_SRC := $(filter %main.c ,$(SOURCE))
MAIN_OBJ := main.o
SOURCE := $(filter-out %main.c,$(SOURCE))
endif

CSRC := $(filter %.c ,$(SOURCE))
ASMSRC := $(filter %.s ,$(SOURCE))
COBJECT:=$(patsubst %.c,%.o,$(CSRC))
ASMOBJECT:= $(patsubst %.s,%.o,$(ASMSRC))

OBJECT := $(COBJECT) $(ASMOBJECT)

ifeq ("$(firstword $(OBJECT))","")
LIB_RESULE :=
endif

all:$(LIB_RESULE) $(MAIN_OBJ)

$(LIB_RESULE):$(OBJECT)
	echo $(COBJECT) 
	$(AR) $(ARFLAGS) $(LIB_DIR)/$@ $(addprefix $(LIB_DIR)/,$(notdir $(OBJECT)))

$(COBJECT):%.o:%.c
	$(CC) -c $(CFLAGS) $(LOCAL_INCLUDE) -o $(LIB_DIR)/$(notdir $@) $< 1>&2 2>$(LOG_DIR)/output.log
	
$(ASMOBJECT):%.o:%.s
	$(ASM) -c $(ASMFLAGS) $(LOCAL_INCLUDE) -o $(LIB_DIR)/$(notdir $@) $< 1>&2 2>$(LOG_DIR)/output.log

$(MAIN_OBJ):$(MAIN_SRC)
	$(CC) -c $(CFLAGS) $(LOCAL_INCLUDE) -o $(LIB_DIR)/$@ $< 1>&2 2>$(LOG_DIR)/output.log

.PHONY:
   clean:
	rm -fr $(addprefix $(LIB_DIR)/,$(notdir $(OBJECT))) $(LIB_DIR)/$(LIB_RESULE) $(LIB_DIR)/$(MAIN_OBJ)
