/****************************************************************************
 * asmp/asmp_main.c
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

#include <nuttx/config.h>
#include <nuttx/compiler.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <unistd.h>

#include <nuttx/drivers/ramdisk.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <debug.h>
#include <errno.h>

#include <asmp/asmp.h>
#include <asmp/mptask.h>
#include <asmp/mpshm.h>
#include <asmp/mpmq.h>
#include <asmp/mpmutex.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* MP object keys. Must be synchronized with worker. */

#include "render_request.h"
#include "worker_common.h"

/* Check configuration.  This is not all of the configuration settings that
 * are required -- only the more obvious.
 */

#if CONFIG_NFILE_DESCRIPTORS < 1
#  error "You must provide file descriptors via CONFIG_NFILE_DESCRIPTORS in your configuration file"
#endif

#define message(format, ...)    printf(format, ##__VA_ARGS__)
#define err(format, ...)        fprintf(stderr, format, ##__VA_ARGS__)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static mpmq_t mq;

#ifdef USE_SHARED_MEM
static mpshm_t shm_inst[4];
static void *shm_buff[4];
static void *subcore_shm_buff[4];
#endif

/****************************************************************************
 * Symbols from Auto-Generated Code
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void * attach_shm(int key, mptask_t *tsk, mpshm_t *shm, int sz)
{
  int ret;
  void *buf = NULL;

  /* Initialize MP shared memory and bind it to MP task */

  ret = mpshm_init(shm, key, sz);
  if (ret < 0)
    {
      err("mpshm_init() failure. %d\n", ret);
      return buf;
    }
  ret = mptask_bindobj(tsk, shm);
  if (ret < 0)
    {
      err("mptask_binobj(shm) failure. %d\n", ret);
      return buf;
    }

  /* Map shared memory to virtual space */

  buf = mpshm_attach(shm, 0);
  if (!buf)
    {
      err("mpshm_attach() failure.\n");
      return buf;
    }
  message("attached at %08x\n", (uintptr_t)buf);
  memset(buf, 0, sz);

  return buf;
}

static void receive_subcore_shmaddr(int idx)
{
  uint32_t msg;
  mpmq_receive(&mq, &msg);
  subcore_shm_buff[idx] = (void *)msg;;
}


int send_render_request(struct RenderRequestContainer *req);

static int run_worker(const char *filename)
{
  mpmutex_t mutex;
  mptask_t mptask;
  int ret;

  /* Initialize MP task */

  ret = mptask_init(&mptask, filename);
  if (ret != 0)
    {
      err("mptask_init() failure. %d\n", ret);
      return ret;
    }

  ret = mptask_assign(&mptask);
  if (ret != 0)
    {
      err("mptask_assign() failure. %d\n", ret);
      return ret;
    }

  /* Initialize MP mutex and bind it to MP task */

  ret = mpmutex_init(&mutex, KEY_MUTEX);
  if (ret < 0)
    {
      err("mpmutex_init() failure. %d\n", ret);
      return ret;
    }
  ret = mptask_bindobj(&mptask, &mutex);
  if (ret < 0)
    {
      err("mptask_bindobj(mutex) failure. %d\n", ret);
      return ret;
    }

  /* Initialize MP message queue with assigned CPU ID, and bind it to MP task */

  ret = mpmq_init(&mq, KEY_MQ, mptask_getcpuid(&mptask));
  if (ret < 0)
    {
      err("mpmq_init() failure. %d\n", ret);
      return ret;
    }
  ret = mptask_bindobj(&mptask, &mq);
  if (ret < 0)
    {
      err("mptask_bindobj(mq) failure. %d\n", ret);
      return ret;
    }

#ifdef USE_SHARED_MEM
  shm_buff[0] = attach_shm(KEY_SHM1, &mptask, &shm_inst[0], SHM_TILE_SIZE);
  shm_buff[1] = attach_shm(KEY_SHM2, &mptask, &shm_inst[1], SHM_TILE_SIZE);
  shm_buff[2] = attach_shm(KEY_SHM3, &mptask, &shm_inst[2], SHM_TILE_SIZE);
  shm_buff[3] = attach_shm(KEY_SHM4, &mptask, &shm_inst[3], SHM_TILE_SIZE);
#endif

  /* Run worker */

  ret = mptask_exec(&mptask);
  if (ret < 0)
    {
      err("mptask_exec() failure. %d\n", ret);
      return ret;
    }

  printf("worker exec is done\n");
  usleep(100);
  printf("Send dummy msg to the worker\n");
  send_render_request(NULL);
  usleep(100);

  printf("Wait for subfore_address\n");
#ifdef USE_SHARED_MEM
  receive_subcore_shmaddr(0);
  printf("1 is comming.\n");
  receive_subcore_shmaddr(1);
  printf("2 is comming.\n");
  receive_subcore_shmaddr(2);
  printf("3 is comming.\n");
  receive_subcore_shmaddr(3);
  printf("Get sub-core attched address : %08x, %08x, %08x, %08x\n",
      subcore_shm_buff[0], subcore_shm_buff[1],
      subcore_shm_buff[2], subcore_shm_buff[3] );
#endif

  return 0;
}

void set_shm_ptr(struct RenderRequestContainer *req)
{
  req->lineBuf     = (uint8_t *)shm_buff[(req->lineNo * 2)];
  req->lineBuf_sub = (uint8_t *)subcore_shm_buff[(req->lineNo*2)];
  req->vbuf        = (uint16_t *)shm_buff[(req->lineNo*2) + 1];
  req->vbuf_sub    = (uint16_t *)subcore_shm_buff[(req->lineNo*2) + 1];
}

int send_render_request(struct RenderRequestContainer *req)
{
  return mpmq_send(&mq, MSG_ID_SAYHELLO, (uint32_t)req);
}

struct RenderRequestContainer *recv_render_request(void)
{
  uint32_t msg;
  mpmq_receive(&mq, &msg);
  return (struct RenderRequestContainer *)msg;
}


uintptr_t vbuf_v2p(struct RenderRequestContainer *req)
{
  return mpshm_virt2phys( &shm_inst[(req->lineNo*2) + 1], req->vbuf );
}

void *get_shm_buffer(int idx)
{
  return shm_buff[idx];
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: asmp_main
 ****************************************************************************/

int worker_exec(void)
{
  (void) run_worker("/mnt/spif/renderer");

  return 0;
}
