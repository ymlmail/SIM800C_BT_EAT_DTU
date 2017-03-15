##########################################################################
# File:             Mailefile 
# Version:          0.1 
# Author:           Maobin
# DATE              NAME                        DESCRIPTION
# 2011-04-10        Maobin                      Initial Version 0.1
##########################################################################
# started
include $(PROJ_HOME)/build/option.mak
OBJDIR=./
MODULE_NAME = $(PARAM)
LIB_RESULE=$(MODULE_NAME).a

#Two mothods#
#First mothod#
S1 := $(wildcard *.c)
SOURCE:=$(S1)

#Second mothod#
#SOURCE := 

#示例中只有一个源文件，所以把源文件编译为main.o
#MAIN_SRC := app_demo_tcpip.c  app_at_cmd_envelope.c
#MAIN_SRC := app_demo_tcpip.c  app_at_cmd_envelope.c
#MAIN_OBJ := main.o 

#头文件目录设置
LOCAL_INCLUDE := -I./

#编译，生成.a文件
include $(PROJ_HOME)/build/app_build.mak

