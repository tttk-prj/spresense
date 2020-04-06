#include <stdio.h>
#include <stdlib.h>

#include "spresense_video_fairy.h"
#include "spresense_audio_fairy.h"
#include "spresense_gamepad_fairy.h"
#include "../../emulator/VirtualMachine.h"
#include "worker_util.h"

#include "spresense_port.h"

#include "rom.h"

#include "worker_common.h"

static struct cartridge_data gCdata;
static SpresenseVideoFairy videoFairy;
static SpresenseAudioFairy audioFairy;
static SpresenseGamepadFairy player1;
static VirtualMachine vm(videoFairy, audioFairy, &player1, NULL);

extern "C" int nes_main(int argc, FAR char *argv[])
{
  worker_exec();

  vm.setVideoScreenBuffer( (uint8_t (*)[Video::screenWidth]) videoFairy.initialize() );
  player1.initialize();

  gCdata.data = (uint8_t *)_rom;
  gCdata.size = _rom_len;

  vm.loadCartridge(&gCdata, "Game_rom");
  vm.sendHardReset();

  while(true) {
    vm.run();
  }

  return 0;
}

