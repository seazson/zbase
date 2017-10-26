#######################################################################
#
#		edit by z
#
#######################################################################

#define config
CONFIG_PROC=ON
CONFIG_CMD_CACHE=ON
CONFIG_CMD_SOFTIRQ=OFF

# define module name
MODULE_NAME=ztools

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs := ztools_main.o ztools_fops.o

ifeq ($(CONFIG_PROC) ,ON)
EXTRA_CFLAGS += -DCONFIG_PROC
$(MODULE_NAME)-objs += ztools_proc.o
endif

ifeq ($(CONFIG_CMD_CACHE) ,ON)
EXTRA_CFLAGS += -DCONFIG_CMD_CACHE
$(MODULE_NAME)-objs += cmd_cache.o
endif

ifeq ($(CONFIG_CMD_SOFTIRQ) ,ON)
EXTRA_CFLAGS += -DCONFIG_CMD_SOFTIRQ
$(MODULE_NAME)-objs += cmd_softirq.o
endif

######################################################
#rules
######################################################
PWD :=$(shell pwd)
all:
	@echo "===============Making $(MODULE_NAME).ko=================="
	@echo "* info:"
	@echo "*   gcc    : $(CC) "
	@echo "*   kernel : $(KERNEL_PATH)"
	@echo "*   PATH   : $(CC_PATH)"
	@echo "*   PWD    : $(PWD)"
	@echo "*************************************************"
	@( PATH=$(CC_PATH);\
	$(MAKE) -C $(KERNEL_PATH) M=$(PWD) modules )	
	
clean:
	@echo "==============Clean up...========================"
	@( PATH=$(CC_PATH);\
	$(MAKE) -C $(KERNEL_PATH) M=$(PWD) clean)


