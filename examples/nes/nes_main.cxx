#include <stdio.h>
#include <stdlib.h>

#include "spresense_video_fairy.h"
#include "spresense_audio_fairy.h"
#include "spresense_gamepad_fairy.h"
#include "../../emulator/VirtualMachine.h"
#include "worker_util.h"

#include "spresense_port.h"
#include "worker_common.h"

// #define EMBEDDED_ROM_IMG

#ifdef EMBEDDED_ROM_IMG
#include "rom.h"
static struct cartridge_data gCdata;
#else
#include <sys/stat.h>
#endif

static SpresenseVideoFairy videoFairy;
static SpresenseAudioFairy audioFairy;
static SpresenseGamepadFairy player1;
static VirtualMachine vm(videoFairy, audioFairy, &player1, NULL);

static int is_exist(const char *fname)
{
  int ret;
  struct stat ss;
  ret = stat(fname, &ss);
  return ret ? 0 /*no file*/ : 1 /*exist*/;
}

extern "C" int nes_main(int argc, FAR char *argv[])
{
#ifndef EMBEDDED_ROM_IMG
  if (argc != 2)
  {
    printf("Usage: %s <romimg file>\n", argv[0]);
    return -1;
  }
  if (!is_exist(argv[1]))
  {
    printf("No such a file: %s\n", argv[1]);
    return -1;
  }
#endif

  worker_exec();

  vm.setVideoScreenBuffer( (uint8_t (*)[Video::screenWidth]) videoFairy.initialize() );
  player1.initialize();

#ifdef EMBEDDED_ROM_IMG
  gCdata.data = (uint8_t *)_rom;
  gCdata.size = _rom_len;

  vm.loadCartridge(&gCdata, "Game_rom");
#else
  vm.loadCartridge(argv[1]);
#endif

  vm.sendHardReset();
  while(true) {
    vm.run();
  }

  return 0;
}

