# used when gnu make is the default make (rather than say, BSD make)
ifeq ($(firstword $(sort 4.1.99 $(MAKE_VERSION))),4.1.99)
include makefile
else
	@echo "Need GNU make of at least 4.2"
endif
