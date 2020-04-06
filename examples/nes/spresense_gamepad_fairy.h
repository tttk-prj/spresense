#ifndef __SPRESENSE_GAMEPAD_FAIRY_H__
#define __SPRESENSE_GAMEPAD_FAIRY_H__

#include "../../emulator/fairy/GamepadFairy.h"

# define BUTTON_SELECT PIN_I2S0_BCK
# define BUTTON_START  PIN_I2S0_LRCK
# define BUTTON_A      PIN_UART2_RTS
# define BUTTON_B      PIN_UART2_CTS

# define BUTTON_DOWN   PIN_EMMC_DATA2
# define BUTTON_LEFT   PIN_I2S0_DATA_IN
# define BUTTON_UP     PIN_I2S0_DATA_OUT
# define BUTTON_RIGHT  PIN_EMMC_DATA3

# define BUTTON_PORARITY_PUSH    0
# define GPIO_PULL_STATE  PIN_PULLUP

class SpresenseGamepadFairy : public GamepadFairy {
public:
  SpresenseGamepadFairy();
  ~SpresenseGamepadFairy();

  void onVBlank();
  void onUpdate();
  bool isPressed(uint8_t keyIdx);
  int initialize();
private:

	uint8_t get_button(int pin);

  uint8_t state;
};


#endif  // __SPRESENSE_GAMEPAD_FAIRY_H__
