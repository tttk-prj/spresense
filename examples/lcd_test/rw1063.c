#include <stdio.h>
#include <stdarg.h>

#include <nuttx/arch.h>

#include "i2c_if/i2c_devif.h"

/*** Control Data ***/
#define RW1063_CTRL_COMMAND  0x00
#define RW1063_CTRL_DATA     0x40
#define RW1063_CTRL_CONTINUE 0x80

/*** Instruction Clear Display ***/
#define RW1063_INST_CLEAR_DISP (0x01)

/*** Instruction Return to Home ***/
#define RW1063_INST_RETURN_HOME (0x02)

/*** Instruction Set Entry Mode ***/
#define RW1063_INST_SETENTMODE    (0x04)
#define RW1063_SETENTMODE_INC     (0x02)
#define RW1063_SETENTMODE_DEC     (0x00)
#define RW1063_SETENTMODE_SHIFT   (0x01)
#define RW1063_SETENTMODE_NOSHIFT (0x00)

/*** Instruction Display On or Off ***/
#define RW1063_INST_DISPLAY     (0x08)
#define RW1063_DISPLAY_DISPON   (0x04)
#define RW1063_DISPLAY_DISPOFF  (0x00)
#define RW1063_DISPLAY_CURON    (0x02)
#define RW1063_DISPLAY_CUROFF   (0x00)
#define RW1063_DISPLAY_BLINKON  (0x01)
#define RW1063_DISPLAY_BLINKOFF (0x00)

/*** Instruction Shift Cursor or Display ***/
#define RW1063_INST_SHIFT    (0x10)
#define RW1063_SHIFT_DISP    (0x08)
#define RW1063_SHIFT_CURS    (0x00)
#define RW1063_SHIFT_RIGHT   (0x04)
#define RW1063_SHIFT_LEFT    (0x00)

/*** Instruction Function Set ***/
#define RW1063_INST_SETFUNC   (0x20)
#define RW1063_SETFUNC_IF8BIT (0x10)
#define RW1063_SETFUNC_IF4BIT (0x00)
#define RW1063_SETFUNC_2LINE  (0x08)
#define RW1063_SETFUNC_1LINE  (0x00)
#define RW1063_SETFUNC_FSZ08  (0x04)
#define RW1063_SETFUNC_FSZ11  (0x00)

/*** Instruction Set DDRAM Address ***/
#define RW1063_INST_SETDDRAM_ADDR (0x80)
#define RW1063_HEIGHT_PITCH (0x40)
#define RW1063_MAX_WIDTH    (40)
#define RW1063_MAX_HEIGHT   (2)


static uint8_t lcd_devadr = -1;
static uint8_t rw1063_sendbuf[2];

static void lcd_control(uint8_t ctrl_cmd)
{
  rw1063_sendbuf[0] = RW1063_CTRL_COMMAND;
  rw1063_sendbuf[1] = ctrl_cmd;

  i2c_send(lcd_devadr, rw1063_sendbuf, 2);
  up_udelay(60);
}

static void lcd_data(uint8_t data)
{
  rw1063_sendbuf[0] = RW1063_CTRL_DATA;
  rw1063_sendbuf[1] = data;

  i2c_send(lcd_devadr, rw1063_sendbuf, 2);
  up_udelay(60);
}

void lcd_clear(void)
{
    lcd_control(RW1063_INST_CLEAR_DISP);
}

void lcd_gohome(void)
{
    lcd_control(RW1063_INST_RETURN_HOME);
}

void lcd_setcurpos(int pos_x, int pos_y)
{
  uint8_t cmd;
  if (  pos_x >= 0
     && pos_x < RW1063_MAX_WIDTH
     && pos_y >= 0
     && pos_y < RW1063_MAX_HEIGHT)
    {
      cmd = RW1063_INST_SETDDRAM_ADDR + (RW1063_HEIGHT_PITCH * pos_y) + pos_x;
      lcd_control(cmd);
    }
}

void lcd_shift_display(bool right_xleft)
{
  lcd_control(  RW1063_INST_SHIFT |
                RW1063_SHIFT_DISP |
                (right_xleft ? RW1063_SHIFT_RIGHT : RW1063_SHIFT_LEFT) );
}

void lcd_printf(const char *format, ...)
{
  int i;
  va_list ap;
  uint8_t pr_buf[RW1063_MAX_WIDTH+1];

  va_start(ap, format);
  vsnprintf((char *)pr_buf, RW1063_MAX_WIDTH+1, format, ap);
  va_end(ap);

  pr_buf[RW1063_MAX_WIDTH] = '\0';

  for (i=0; i<RW1063_MAX_WIDTH && pr_buf[i]; i++)
    {
      lcd_data(pr_buf[i]);
    }
}


void lcd_init(uint8_t dev_adr, bool clr, bool curs_on, bool blink_on)
{
  i2c_init(0 /*I2C0 of Spresense*/, 100000 /*100kHz*/);

  lcd_devadr = dev_adr;

  if (clr) lcd_clear();

  lcd_control(  RW1063_INST_SETFUNC   |
                RW1063_SETFUNC_IF8BIT |
                RW1063_SETFUNC_2LINE  |
                RW1063_SETFUNC_FSZ08  );

  lcd_control(  RW1063_INST_DISPLAY     |
                RW1063_DISPLAY_DISPON   |
                (curs_on ? RW1063_DISPLAY_CURON : RW1063_DISPLAY_CUROFF)   |
                (blink_on ? RW1063_DISPLAY_BLINKON : RW1063_DISPLAY_BLINKOFF) );

  lcd_control(  RW1063_INST_SETENTMODE    |
                RW1063_SETENTMODE_INC     |
                RW1063_SETENTMODE_NOSHIFT );

  lcd_gohome();
}


