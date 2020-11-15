# Makefile

# Add your debugging flag to CFLAGS

ifeq ($(DEBUG), y)
	DEBFLAGS = -O -g -DSCULL_DEBUG
else
	DEBFLAGS = -O2
endif

ifneq($(KERNELRELEASE),)
# call from kernel build system
scull-objs := main.o pipe.o access.o

obj-m := scull.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif


