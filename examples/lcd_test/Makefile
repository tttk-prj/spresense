
-include $(TOPDIR)/Make.defs
-include $(SDKDIR)/Make.defs

CONFIG_EXAMPLES_LCD_TEST_PRIORITY ?= SCHED_PRIORITY_DEFAULT
CONFIG_EXAMPLES_LCD_TEST_STACKSIZE ?= 2048

APPNAME = lcd_test
PRIORITY = $(CONFIG_EXAMPLES_LCD_TEST_PRIORITY)
STACKSIZE = $(CONFIG_EXAMPLES_LCD_TEST_STACKSIZE)

CFLAGS += -I$(TOPDIR)/arch/arm/src/
CFLAGS += -I$(TOPDIR)/arch/arm/src/cxd56xx/
CFLAGS += -I$(SDKDIR)/bsp/src/

ASRCS =
CSRCS = rw1063.c

ifeq ($(CONFIG_EXAMPLES_LCD_TEST_USE_HW_IIC),y)
CFLAGS += -I$(SDKDIR)/bsp/board/common/include
CSRCS += i2c_if/i2c_devif.c
else
CSRCS += i2c_if/i2c_devif_sw.c
endif

MAINSRC = lcd_test_main.c

CONFIG_EXAMPLES_LCD_TEST_PROGNAME ?= lcd_test$(EXEEXT)
PROGNAME = $(CONFIG_EXAMPLES_LCD_TEST_PROGNAME)

include $(APPDIR)/Application.mk
