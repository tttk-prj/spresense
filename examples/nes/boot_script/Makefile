-include $(TOPDIR)/.config
-include $(SDKDIR)/.config

MKROMIMG = $(SDKDIR)/tools/mkromfsimg.sh

all: nsh_romfsimg.h

context: all

nsh_romfsimg.h: rcS.template
	$(MKROMIMG) $(SDKDIR)

clean:
	rm -f nsh_romfsimg.h

