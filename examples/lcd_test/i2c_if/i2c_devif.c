/****************************************************************************
 * system/i2c/i2c_devif.c
 *
 *   Copyright (C) 2016 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
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
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
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

#include <sdk/config.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>

#include <nuttx/i2c/i2c_master.h>

#include <cxd56_i2cdev.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Device naming */

#define DEVNAME_FMT    "/dev/i2c%d"
#define DEVNAME_FMTLEN (8 + 3 + 1)

/****************************************************************************
 * Private Data
 ****************************************************************************/

static char g_devname[DEVNAME_FMTLEN];

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: i2cdev_path
 ****************************************************************************/

static FAR char *i2cdev_path(int bus)
{
  snprintf(g_devname, DEVNAME_FMTLEN, DEVNAME_FMT, bus);
  return g_devname;
}

/****************************************************************************
 * Name: i2cdev_exists
 ****************************************************************************/

static bool i2cdev_exists(int bus)
{
  struct stat buf;
  FAR char *devpath;
  int ret;

  /* Get the device path */

  devpath = i2cdev_path(bus);

  /* Check if something exists at that path */

  ret = stat(devpath, &buf);
  if (ret >= 0)
    {
      /* Return TRUE only if it is a character driver */

      if (S_ISCHR(buf.st_mode))
        {
          return true;
        }
    }

  return false;
}

/****************************************************************************
 * Name: i2cdev_open
 ****************************************************************************/

static int i2cdev_open(int bus)
{
  FAR char *devpath;

  /* Get the device path */

  devpath = i2cdev_path(bus);

  /* Open the file for read-only access (we need only IOCTLs) */

  return open(devpath, O_RDONLY);
}

/****************************************************************************
 * Name: i2cdev_transfer
 ****************************************************************************/

static int i2cdev_transfer(int fd, FAR struct i2c_msg_s *msgv, int msgc)
{
  struct i2c_transfer_s xfer;

  /* Set up the IOCTL argument */

  xfer.msgv = msgv;
  xfer.msgc = msgc;

  /* Perform the IOCTL */

  return ioctl(fd, I2CIOC_TRANSFER, (unsigned long)((uintptr_t)&xfer));
}

/**************** Grobal Functions ********************************************/

static int i2c_device_fd = -1;
static uint32_t bus_freq = 0;

bool i2c_init(int bus_no, uint32_t freq)
{
  if (i2cdev_exists(bus_no) == false)
    {
          board_i2cdev_initialize(bus_no);
    }

  if (i2c_device_fd < 0)
    {
      i2c_device_fd = i2cdev_open(bus_no);
      bus_freq = freq;
    }

  return true;
}

void i2c_deinit(void)
{
  if (i2c_device_fd > 0)
    {
      close(i2c_device_fd);
      i2c_device_fd = -1;
      bus_freq = 0;
    }
}


int i2c_send(unsigned char trans_8bit_adr, unsigned char *data, int len)
{
  struct i2c_msg_s i2c_cmd;

  i2c_cmd.frequency = bus_freq;
  i2c_cmd.addr      = trans_8bit_adr >> 1;  // Make it as 7bit.
  i2c_cmd.flags     = 0;                    // Write Mode.
  i2c_cmd.buffer    = data;
  i2c_cmd.length    = len;
  if (i2cdev_transfer(i2c_device_fd, &i2c_cmd, 1) == 0)
    {
      return len;
    }
  else
    {
      return -1;
    }
}


