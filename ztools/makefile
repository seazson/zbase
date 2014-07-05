#######################################################################
#
#		edit by z
#
#######################################################################

include config.mk

PWD :=$(shell pwd)

.PHONY: all clean app ko

all:
	@(cd $(PWD)/ko  && $(MAKE)  )|| exit 1
	@(cd $(PWD)/app  && $(MAKE)  )|| exit 1

clean:
	@(cd $(PWD)/ko  && $(MAKE) clean )|| exit 1
	@(cd $(PWD)/app && $(MAKE) clean )|| exit 1

app:
	@(cd $(PWD)/app  && $(MAKE)  )|| exit 1

ko:	
	@(cd $(PWD)/ko  && $(MAKE)  )|| exit 1
	
help:
	@echo "make app"
	@echo "make ko"
	@echo "make all"
	@echo "make clean"