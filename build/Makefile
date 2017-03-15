##########################################################################
# File:             Makefile
# Description:      System compile file and DO NOT modigy it at will.
# Version:          0.1 
# Author:           Maobin
# DATE              NAME                        DESCRIPTION
# 2013-01-10        Maobin                      Initial Version 0.1
##########################################################################
# started#
export PROJ_HOME=$(shell pwd)

include $(PROJ_HOME)/build/option.mak

#需要编译的模块
TARGETS = $(SRC_OPEN_MODE_NAME)
TARGGETS_LIB := $(TARGETS)
ifeq ($(strip $(SRC_DIR)),demo)
#demo中的几个示例为单文件，不需要编译为.a文件
TARGGETS_LIB := $(filter-out timer audio flash gpio uart system i2c sim other pwm interrupt tcptest smsapi watchdog,$(TARGETS))
endif

LIB_OBJS:=$(foreach var,$(TARGGETS_LIB),$(OUTPUT_LIB_DIR)/$(var).a )

#必须有一个main.o，包含线程入口结构体值
LIB_MAIN_OBJ := $(OUTPUT_LIB_DIR)/main.o
LIB_OBJS += $(LIB_MAIN_OBJ)

CLEANS:=$(foreach var,$(TARGETS),$(var)_clean )

all:copy $(TARGETS) $(LIB_MAIN_OBJ) $(BIN)
clean:$(CLEANS) bin_clean

$(BIN):$(LIB_OBJS)
	$(LINK) $(LINK_FLAGS) $^ 
	$(FE) $(OS_FE_OPT) $(ELF_NAME) --bin --output=$(BIN_NAME)
	appgen $(BIN_NAME) 32
	cp -f $(BIN_NAME) $(CORE_BIN_DIR)\$(BIN)
	cp -f $(CORE_DIR)\$(CORE_BIN_CFG_FILE) $(CORE_BIN_DIR)

bin_clean:
	rm -fr $(BIN_NAME) $(ELF_NAME) $(OUTPUT_LIST_FILE) $(OUTPUT_SYM_FILE)
	rm -fr $(LIB_MAIN_OBJ)

copy:
	-mkdir $(subst /,\,$(BIN_DIR))
	-mkdir $(subst /,\,$(LIB_DIR))
	-mkdir $(subst /,\,$(LOG_DIR))
#	cp -f $(CORE_DIR)\$(SRC_SCAT_FILE) $(OUTPUT_BIN_DIR)\$(SCATTER_FILE)

#$(LIB_MAIN_OBJ):src\main.c
#	$(CC) -c $(CFLAGS) -o $@ $< 

$(TARGETS):
	cd $(SRC_DIR)/$@ && $(MAKE) PARAM=$@

$(CLEANS):
	cd $(SRC_DIR)/$(subst _clean,,$@) && $(MAKE2) PARAM=$(subst _clean,,$@)
