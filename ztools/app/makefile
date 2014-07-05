#######################################################################
#
#		edit by z
#
#######################################################################

CONFIG_MTD=ON
CONFIG_IIC=ON

DIR_SRCS=src
#SRCS=$(shell cd $(DIR_SRCS);ls *.c)
#SRCS_PATH=$(addprefix $(DIR_SRCS)/,$(SRCS))
PWD :=$(shell pwd)

#src file list
ifeq ($(CONFIG_MTD) ,ON)
	src-files += $(PWD)/$(DIR_SRCS)/mtd.c
endif
ifeq ($(CONFIG_IIC) ,ON)
	src-files += $(PWD)/$(DIR_SRCS)/iic.c
endif
src-files +=main.c common.c

CINCLUDE = -I./ 

#必须指明指令集，不然可能编译出不支持的指令
CFLAG += -mcpu=arm920t 
CFLAG += -T armelf_linux_eabi.lds

TARGET=ztools

all:
	$(CC) $(src-files) $(CFLAG) $(CINCLUDE) -o $(TARGET) 
	chmod +x $(TARGET)

clean:
	rm -rf *.o
	rm -f $(TARGET)
