#include <stdio.h>
#include <unistd.h>

#include <nuttx/arch.h>
// #include <common/up_arch.h>

#include <arch/board/board.h>
#include <arch/chip/pin.h>

#include <cxd56_pinconfig.h>

#define IGNORE_NACK

#define DELAY_PERIOD  (10)

#define SCL_LOW()   do{ board_gpio_write(scl_pin,  0); }while(0)
#define SCL_HIGH()  do{ board_gpio_write(scl_pin, -1); }while(0)
#define SDA_LOW()   do{ board_gpio_write(sda_pin,  0); }while(0)
#define SDA_HIGH()  do{ board_gpio_write(sda_pin, -1); }while(0)
#define WAIT_HALF() up_udelay(DELAY_PERIOD)

#ifdef DEBUG_SWI2C
#define DPRINT  printf
#else // DEBUG_SWI2C
#define DPRINT(...)
#endif

#define ACK   (0)
#define NACK  (1)

static void swI2C_sendStop(void);
static void swI2C_sendReStart(void);
static int  swI2C_readAck(void);
static void swI2C_sendByte(uint8_t b);
static void swI2C_sendStart(void);
static void swI2C_init(int scl, int sda);

static int scl_pin;
static int sda_pin;

static void swI2C_init(int scl, int sda)
{
  scl_pin = scl;
  sda_pin = sda;

  board_gpio_write(scl_pin, -1);  // Hi-Z
  board_gpio_write(sda_pin, -1);  // Hi-Z
  board_gpio_config(scl_pin,
                    PINCONF_MODE0,  /* GPIO      */
                    true,           /* input_en  */
                    true,           /* highdrive */
                    PINCONF_PULLUP  /* pull      */);
  board_gpio_config(sda_pin,
                    PINCONF_MODE0,  /* GPIO      */
                    true,           /* input_en  */
                    true,           /* highdrive */
                    PINCONF_PULLUP  /* pull      */);

  // Wait for one second until stable of LCD device.
  // up_udelay(100 * 1000);
}


static void swI2C_sendStart(void)
{
  DPRINT("== Start\n");
  SDA_LOW();
  WAIT_HALF();
  SCL_LOW();
  WAIT_HALF();
}

static void swI2C_sendByte(uint8_t b)
{
  uint8_t mask;
  DPRINT("Send: %02x\n", b);
  for(mask = 0x80; mask; mask >>=1)
  {
    if(mask & b)
    {
      SDA_HIGH();
    }
    else
    {
      SDA_LOW();
    }
    WAIT_HALF();
    SCL_HIGH();
    WAIT_HALF();
    SCL_LOW();  
  }
}

static int swI2C_readAck(void)
{
  int ret;
  SDA_HIGH();
  WAIT_HALF();
  SCL_HIGH();
  ret = board_gpio_read(sda_pin);
  DPRINT("ReadAck : %d\n", ret);
  WAIT_HALF();
  SCL_LOW();
  return ret;
}

static void swI2C_sendReStart(void)
{
  DPRINT("Restart\n");
  WAIT_HALF();
  WAIT_HALF();
}

static void swI2C_sendStop(void)
{
  DPRINT("Stop ====\n");
  SDA_LOW();
  WAIT_HALF();
  SCL_HIGH();
  WAIT_HALF();
  SDA_HIGH();
  WAIT_HALF();
}


int i2c_send(unsigned char addr, unsigned char *buf, int len)
{
  int i;
  int ret = 0;
#ifndef IGNORE_NACK
  int is_ack;
#endif

  DPRINT("=== writeI2C :%02x %d ====\n", addr, len);
  swI2C_sendStart();
  swI2C_sendByte(addr);
#ifdef IGNORE_NACK
  swI2C_readAck();
  if(1)
#else
  if(swI2C_readAck() == ACK)
#endif
  {
    for(i=0; i<len; i++)
    {
      DPRINT("i=%d, len=%d\n", i, len);
      swI2C_sendReStart();
      swI2C_sendByte(buf[i]);
#ifndef IGNORE_NACK
      is_ack =
#endif
        swI2C_readAck();
#ifndef IGNORE_NACK
      if(is_ack != ACK)
      {
        ret = -1;
        break;
      }
#endif
    }
  }
  else
  {
    ret = -1;
  }

  swI2C_sendStop();

  return ret;
}


bool i2c_init(int bus_no, uint32_t freq)
{
  // In SW I2C case, bus number is fixed as I2C0.
  swI2C_init(PIN_I2C0_BCK, PIN_I2C0_BDT);

  return true;
}


void i2c_deinit(void)
{
  board_gpio_write(scl_pin, -1);  // Hi-Z
  board_gpio_write(sda_pin, -1);  // Hi-Z
  board_gpio_config(scl_pin,
                    PINCONF_MODE0,  /* GPIO      */
                    false,          /* input_en  */
                    false,          /* highdrive */
                    PINCONF_FLOAT   /* pull      */);
  board_gpio_config(scl_pin,
                    PINCONF_MODE0,  /* GPIO      */
                    false,          /* input_en  */
                    false,          /* highdrive */
                    PINCONF_FLOAT   /* pull      */);
}


