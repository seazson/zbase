# define env
CC_PATH :=/opt/CodeSourcery/Sourcery_G++_Lite/bin/:/bin:/sbin:/usr/bin
KERNEL_PATH :=/home/zengyang/m82331/code/linux-3.11
CROSS_COMPLIE :=arm-none-linux-gnueabi-

CC := $(CROSS_COMPLIE)gcc
LD := $(CROSS_COMPLIE)ld
STRIP :=$(CROSS_COMPLIE)strip


export CC_PATH KERNEL_PATH
export CC LD STRIP