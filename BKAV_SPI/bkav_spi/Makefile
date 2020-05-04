obj-m += bkav_spi.o
bkav_spi-objs := bkav_spidev.o CamCmd.o CamIni.o control.o gpio.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD       := $(shell pwd)

ROOT := $(dir $(M))
BKAVINCLUDE := -I$(ROOT)../include -I$(ROOT)/include

GCCVERSION = $(shell gcc -dumpversion | sed -e 's/\.\([0-9][0-9]\)/\1/g' -e 's/\.\([0-9]\)/0\1/g' -e 's/^[0-9]\{3,4\}$$/&00/')

GCC49 := $(shell expr $(GCCVERSION) \>= 40900)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install: all
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
	depmod -a
	install -m 644 10-xcldma.rules /etc/udev/rules.d

clean:
	rm -rf *.o *.o.d *~ core .depend .*.cmd *.ko *.ko.unsigned *.mod.c .tmp_versions *.symvers .#* *.save *.bak Modules.* modules.order Module.markers *.bin

CFLAGS_bkav_spidev.o := -Wall -DDEBUG $(BKAVINCLUDE)

ifeq ($(GCC49),1)
	CFLAGS_bkav_spidev.o += -Wno-error=date-time
endif
#CFLAGS_CamCmd.o :=  $(XILINXINCLUDE)
CFLAGS_CamIni.o := $(BKAVINCLUDE)
CFLAGS_control.o := $(BKAVINCLUDE)
CFLAGS_CamCmd.o := $(BKAVINCLUDE)
CFLAGS_gpio.o := $(BKAVINCLUDE)

