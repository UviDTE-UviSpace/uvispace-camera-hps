obj-m += uvispace_camera_driver.o

SRC := $(shell pwd)

CROSS_COMPILE := $(SDKROOT)/sysroots/x86_64-angstromsdk-linux/usr/bin/arm-angstrom-linux-gnueabi/arm-angstrom-linux-gnueabi-
KDIR := $(SDKROOT)/sysroots/armv7at2hf-neon-angstrom-linux-gnueabi/usr/src/kernel/
ARCH := arm

all: check-env
	make -C $(KDIR) M=$(SRC) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) modules

clean: check-env
	make -C $(KDIR) M=$(SRC) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) clean

check-env:
ifndef SDKROOT
	$(error SDKROOT environment variable is undefined)
endif
