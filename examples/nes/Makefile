############################################################################
# accel/Makefile
#
#   Copyright 2018 Sony Semiconductor Solutions Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name of Sony Semiconductor Solutions Corporation nor
#    the names of its contributors may be used to endorse or promote
#    products derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
############################################################################

-include $(TOPDIR)/.config
-include $(TOPDIR)/Make.defs
-include $(SDKDIR)/Make.defs
include $(APPDIR)/Make.defs

# NES Emulator built-in application info

CONFIG_EXAMPLES_NES_PRIORITY ?= SCHED_PRIORITY_DEFAULT
CONFIG_EXAMPLES_NES_STACKSIZE ?= 2048

APPNAME = nes
PRIORITY = $(CONFIG_EXAMPLES_NES_PRIORITY)
STACKSIZE = $(CONFIG_EXAMPLES_NES_STACKSIZE)


# Commands definition

GIT_CLONE = git clone
EMULATOR_SRC_GIT = https://github.com/tttk-prj/Cycloa.git
CYCLOA_DIR  = Cycloa
EMULATOR_CORE_DIR  = Cycloa/src/emulator

NES_ROM  = Cycloa/roms/alter_ego/Alter_Ego.nes
GAME_ROM = rom.h

NSH_ROMFSIMG = boot_script/nsh_romfsimg.h

# NES Emulator Example

MAINSRC  =  nes_main.cxx
CSRCS    =  worker_util.c
CXXSRCS +=  spresense_audio_fairy.cxx                  				  \
            spresense_gamepad_fairy.cxx               				  \
            spresense_video_fairy.cxx          				          \
            $(EMULATOR_CORE_DIR)/fairy/FairyConstantValues.cxx  \
            $(EMULATOR_CORE_DIR)/VirtualMachine.cxx             \
            $(EMULATOR_CORE_DIR)/mapper/board/VRC4.cxx          \
            $(EMULATOR_CORE_DIR)/mapper/Mapper3.cxx             \
            $(EMULATOR_CORE_DIR)/mapper/Mapper0.cxx             \
            $(EMULATOR_CORE_DIR)/mapper/Mapper2.cxx             \
            $(EMULATOR_CORE_DIR)/mapper/Mapper4.cxx             \
            $(EMULATOR_CORE_DIR)/mapper/Mapper23.cxx            \
            $(EMULATOR_CORE_DIR)/mapper/Mapper1.cxx             \
            $(EMULATOR_CORE_DIR)/mapper/Mapper21.cxx            \
            $(EMULATOR_CORE_DIR)/mapper/Mapper25.cxx            \
            $(EMULATOR_CORE_DIR)/Audio.cxx                      \
            $(EMULATOR_CORE_DIR)/Video.cxx                      \
            $(EMULATOR_CORE_DIR)/Cartridge.cxx                  \
            $(EMULATOR_CORE_DIR)/file/NesFile.cxx               \
            $(EMULATOR_CORE_DIR)/Processor.cxx

# CXXFLAGS += -DPERFORMANCE_CHECK -I$(EMULATOR_CORE_DIR) -I$(EMULATOR_COR_DIR)/fairy -I$(EMULATOR_CORE_DIR)/mapper -I$(EMULATOR_CORE_DIR)/file
CXXFLAGS += -I$(EMULATOR_CORE_DIR) -I$(EMULATOR_COR_DIR)/fairy -I$(EMULATOR_CORE_DIR)/mapper -I$(EMULATOR_CORE_DIR)/file
CFLAGS += -I$(EMULATOR_CORE_DIR) -I$(EMULATOR_COR_DIR)/fairy -I$(EMULATOR_CORE_DIR)/mapper -I$(EMULATOR_CORE_DIR)/file

cycloa_clone:
	if [ ! -d $(CYCLOA_DIR) ] ; then \
		$(GIT_CLONE) $(EMULATOR_SRC_GIT); \
	fi
	ls -l $(CYCLOA_DIR)

$(GAME_ROM): $(NES_ROM)
	cp $(NES_ROM) _rom
	xxd -i _rom > $(GAME_ROM)
	rm _rom

clean_gamerom:
	@rm -f _rom $(GAME_ROM)

clean: clean_Cycloa clean_worker clean_gamerom

clean_Cycloa:
	@rm -f `find Cycloa -name "*.o"`

$(NSH_ROMFSIMG):
	@$(MAKE) -C boot_script TOPDIR="$(TOPDIR)" SDKDIR="$(SDKDIR)" APPDIR="$(APPDIR)" CROSSDEV=$(CROSSDEV)

context: build_worker cycloa_clone $(GAME_ROM) $(NSH_ROMFSIMG)
	echo $(EMULATOR_SRC_GIT)

-include $(APPDIR)/Application.mk

# ifeq ($(CONFIG_FS_ROMFS),y)
# .depend: video_worker/romfs.h
# video_worker/romfs.h:
#	@echo >$@
#endif

build_worker:
	@$(MAKE) -C video_worker TOPDIR="$(TOPDIR)" SDKDIR="$(SDKDIR)" APPDIR="$(APPDIR)" CROSSDEV=$(CROSSDEV)

# $(OBJS): build_worker

# clean:: clean_worker

clean_worker:
	@$(MAKE) -C video_worker TOPDIR="$(TOPDIR)" SDKDIR="$(SDKDIR)" APPDIR="$(APPDIR)" CROSSDEV=$(CROSSDEV) clean

-include Make.dep
.PHONY: preconfig
preconfig:

