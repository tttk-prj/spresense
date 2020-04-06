/****************************************************************************
 * asmp/worker/hello/hello.c
 *
 *   Copyright 2018 Sony Semiconductor Solutions Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Sony Semiconductor Solutions Corporation nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <errno.h>

#include <asmp/types.h>
#include <asmp/mpshm.h>
#include <asmp/mpmutex.h>
#include <asmp/mpmq.h>

#include "../../render_request.h"

#include "asmp.h"

/* MP object keys. Must be synchronized with supervisor. */

#include "../../worker_common.h"

#define ASSERT(cond) if (!(cond)) wk_abort()

#if 0
const uint16_t nesPaletteRGB565[64] =
    {
        ARGB2RGB565(0x787878), ARGB2RGB565(0x2000B0), ARGB2RGB565(0x2800B8), ARGB2RGB565(0x6010A0),
        ARGB2RGB565(0x982078), ARGB2RGB565(0xB01030), ARGB2RGB565(0xA03000), ARGB2RGB565(0x784000),
        ARGB2RGB565(0x485800), ARGB2RGB565(0x386800), ARGB2RGB565(0x386C00), ARGB2RGB565(0x306040),
        ARGB2RGB565(0x305080), ARGB2RGB565(0x000000), ARGB2RGB565(0x000000), ARGB2RGB565(0x000000),
        ARGB2RGB565(0xB0B0B0), ARGB2RGB565(0x4060F8), ARGB2RGB565(0x4040FF), ARGB2RGB565(0x9040F0),
        ARGB2RGB565(0xD840C0), ARGB2RGB565(0xD84060), ARGB2RGB565(0xE05000), ARGB2RGB565(0xC07000),
        ARGB2RGB565(0x888800), ARGB2RGB565(0x50A000), ARGB2RGB565(0x48A810), ARGB2RGB565(0x48A068),
        ARGB2RGB565(0x4090C0), ARGB2RGB565(0x000000), ARGB2RGB565(0x000000), ARGB2RGB565(0x000000),
        ARGB2RGB565(0xFFFFFF), ARGB2RGB565(0x60A0FF), ARGB2RGB565(0x5080FF), ARGB2RGB565(0xA070FF),
        ARGB2RGB565(0xF060FF), ARGB2RGB565(0xFF60B0), ARGB2RGB565(0xFF7830), ARGB2RGB565(0xFFA000),
        ARGB2RGB565(0xE8D020), ARGB2RGB565(0x98E800), ARGB2RGB565(0x70F040), ARGB2RGB565(0x70E090),
        ARGB2RGB565(0x60D0E0), ARGB2RGB565(0x787878), ARGB2RGB565(0x000000), ARGB2RGB565(0x000000),
        ARGB2RGB565(0xFFFFFF), ARGB2RGB565(0x90D0FF), ARGB2RGB565(0xA0B8FF), ARGB2RGB565(0xC0B0FF),
        ARGB2RGB565(0xE0B0FF), ARGB2RGB565(0xFFB8E8), ARGB2RGB565(0xFFC8B8), ARGB2RGB565(0xFFD8A0),
        ARGB2RGB565(0xFFF090), ARGB2RGB565(0xC8F080), ARGB2RGB565(0xA0F0A0), ARGB2RGB565(0xA0FFC8),
        ARGB2RGB565(0xA0FFF0), ARGB2RGB565(0xA0A0A0), ARGB2RGB565(0x000000), ARGB2RGB565(0x000000)
    };
#else
const uint16_t nesPaletteRGB565[64] = {
    RGB565(0x75,0x75,0x75), RGB565(0x27,0x1B,0x8F), RGB565(0x00,0x00,0xAB), RGB565(0x47,0x00,0x9F),
    RGB565(0x8F,0x00,0x77), RGB565(0xAB,0x00,0x13), RGB565(0xA7,0x00,0x00), RGB565(0x7F,0x0B,0x00),
    RGB565(0x43,0x2F,0x00), RGB565(0x00,0x47,0x00), RGB565(0x00,0x51,0x00), RGB565(0x00,0x3F,0x17),
    RGB565(0x1B,0x3F,0x5F), RGB565(0x00,0x00,0x00), RGB565(0x05,0x05,0x05), RGB565(0x05,0x05,0x05),

    RGB565(0xBC,0xBC,0xBC), RGB565(0x00,0x73,0xEF), RGB565(0x23,0x3B,0xEF), RGB565(0x83,0x00,0xF3),
    RGB565(0xBF,0x00,0xBF), RGB565(0xE7,0x00,0x5B), RGB565(0xDB,0x2B,0x00), RGB565(0xCB,0x4F,0x0F),
    RGB565(0x8B,0x73,0x00), RGB565(0x00,0x97,0x00), RGB565(0x00,0xAB,0x00), RGB565(0x00,0x93,0x3B),
    RGB565(0x00,0x83,0x8B), RGB565(0x11,0x11,0x11), RGB565(0x09,0x09,0x09), RGB565(0x09,0x09,0x09),

    RGB565(0xFF,0xFF,0xFF), RGB565(0x3F,0xBF,0xFF), RGB565(0x5F,0x97,0xFF), RGB565(0xA7,0x8B,0xFD),
    RGB565(0xF7,0x7B,0xFF), RGB565(0xFF,0x77,0xB7), RGB565(0xFF,0x77,0x63), RGB565(0xFF,0x9B,0x3B),
    RGB565(0xF3,0xBF,0x3F), RGB565(0x83,0xD3,0x13), RGB565(0x4F,0xDF,0x4B), RGB565(0x58,0xF8,0x98),
    RGB565(0x00,0xEB,0xDB), RGB565(0x66,0x66,0x66), RGB565(0x0D,0x0D,0x0D), RGB565(0x0D,0x0D,0x0D),

    RGB565(0xFF,0xFF,0xFF), RGB565(0xAB,0xE7,0xFF), RGB565(0xC7,0xD7,0xFF), RGB565(0xD7,0xCB,0xFF),
    RGB565(0xFF,0xC7,0xFF), RGB565(0xFF,0xC7,0xDB), RGB565(0xFF,0xBF,0xB3), RGB565(0xFF,0xDB,0xAB),
    RGB565(0xFF,0xE7,0xA3), RGB565(0xE3,0xFF,0xA3), RGB565(0xAB,0xF3,0xBF), RGB565(0xB3,0xFF,0xCF),
    RGB565(0x9F,0xFF,0xF3), RGB565(0xDD,0xDD,0xDD), RGB565(0x11,0x11,0x11), RGB565(0x11,0x11,0x11)
};
#endif


static void do_frame_rendering(struct RenderRequestContainer *req)
{
	int pix_pos;
	uint8_t *frame = req->lineBuf_sub;
	uint8_t mask = req->paletteMask;
	uint16_t *vbuf = req->vbuf_sub;

  for (pix_pos = 0; pix_pos < 240 /*screenHeight*/ * 256 /*screenWidth*/; pix_pos++) {
  	*vbuf++ = nesPaletteRGB565[frame[pix_pos] & mask];
  }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

static mpshm_t shm_inst[4];
static void *shm_buff[4];

static void *attach_shm(mpshm_t *shm, int key, int sz)
{
  int ret;
  void *buf = NULL;

  /* Initialize MP shared memory */

  ret = mpshm_init(shm, key, sz);
  ASSERT(ret == 0);

  /* Map shared memory to virtual space */

  buf = (char *)mpshm_attach(shm, 0);
  ASSERT(buf);

  return buf;
}


int main(void)
{
  mpmutex_t mutex;

  mpmq_t mq;
  uint32_t msgdata;
  int ret;

  /* Initialize MP Mutex */

  ret = mpmutex_init(&mutex, KEY_MUTEX);
  ASSERT(ret == 0);

  /* Initialize MP message queue,
   * On the worker side, 3rd argument is ignored.
   */

  ret = mpmq_init(&mq, KEY_MQ, 0);
  ASSERT(ret == 0);

#ifdef USE_SHARED_MEM
  shm_buff[0] = attach_shm(&shm_inst[0], KEY_SHM1, SHM_TILE_SIZE);
  shm_buff[1] = attach_shm(&shm_inst[1], KEY_SHM2, SHM_TILE_SIZE);
  shm_buff[2] = attach_shm(&shm_inst[2], KEY_SHM3, SHM_TILE_SIZE);
  shm_buff[3] = attach_shm(&shm_inst[3], KEY_SHM4, SHM_TILE_SIZE);

  ret = mpmq_receive(&mq, &msgdata);
  ASSERT(ret == MSG_ID_SAYHELLO);

  // Send Shared Mem attached address in sub-core
  ret = mpmq_send(&mq, MSG_ID_SAYHELLO, (uint32_t)shm_buff[0]);
  ret = mpmq_send(&mq, MSG_ID_SAYHELLO, (uint32_t)shm_buff[1]);
  ret = mpmq_send(&mq, MSG_ID_SAYHELLO, (uint32_t)shm_buff[2]);
  ret = mpmq_send(&mq, MSG_ID_SAYHELLO, (uint32_t)shm_buff[3]);
#endif

  /* Rendering cycle */

  while(1)
    {
      ret = mpmq_receive(&mq, &msgdata);
      ASSERT(ret == MSG_ID_SAYHELLO);

      /* Copy hello message to shared memory */
      struct RenderRequestContainer *req = (struct RenderRequestContainer *)msgdata;

      // do_line_rendering(req);
      do_frame_rendering(req);

      ret = mpmq_send(&mq, MSG_ID_SAYHELLO, msgdata);
      ASSERT(ret == 0);
    }

  return 0;
}

