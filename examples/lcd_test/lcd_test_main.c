
#include <sdk/config.h>
#include <unistd.h>
#include <stdio.h>

#include <nuttx/arch.h>

#include <arch/board/board.h>
#include <arch/chip/pin.h>

#include <cxd56_pinconfig.h>

#include "rw1063.h"


#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int lcd_test_main(int argc, char *argv[])
#endif
{
  int i;

  // LCD Display's addr pins are connected to D08 and D09.
  // Address will be changed for this pins.
  // When both pins are zero. SPI Address is 0x78.
  board_gpio_write(PIN_PWM2, 0);
  board_gpio_write(PIN_SPI2_MISO, 0);
  board_gpio_config(PIN_PWM2,
                    PINCONF_MODE0,  /* GPIO      */
                    false,          /* input_en  */
                    true,           /* highdrive */
                    PINCONF_FLOAT   /* pull      */);
  board_gpio_config(PIN_SPI2_MISO,
                    PINCONF_MODE0,  /* GPIO      */
                    false,          /* input_en  */
                    true,           /* highdrive */
                    PINCONF_FLOAT   /* pull      */);

  lcd_init(0x78, true, false, false);

  lcd_setcurpos(0,0);
  lcd_printf("Hello LCD! This is top line strings.");
  lcd_setcurpos(0,1);
  lcd_printf("LOWER LINE STRING... UP TO 40 CHARACTORs");

  printf("Start shifting..\n");
  for(i=0; i<40; i++)
    {
      lcd_shift_display(false);
      usleep(300*1000);
    }

  return 0;
}
