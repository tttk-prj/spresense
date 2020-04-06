#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/boardctl.h>

#include <arch/board/board.h>
#include <arch/chip/pin.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <arch/chip/cxd56_adc.h>

#include "spresense_gamepad_fairy.h"

SpresenseGamepadFairy::SpresenseGamepadFairy()
{
}

int SpresenseGamepadFairy::initialize()
{
  board_gpio_write(BUTTON_A, -1);
  board_gpio_config(BUTTON_A, 0, true, true, GPIO_PULL_STATE);

  board_gpio_write(BUTTON_B, -1);
  board_gpio_config(BUTTON_B, 0, true, true, GPIO_PULL_STATE);

  board_gpio_write(BUTTON_SELECT, -1);
  board_gpio_config(BUTTON_SELECT, 0, true, true, GPIO_PULL_STATE);

  board_gpio_write(BUTTON_START, -1);
  board_gpio_config(BUTTON_START, 0, true, true, GPIO_PULL_STATE);

  board_gpio_write(BUTTON_UP, -1);
  board_gpio_config(BUTTON_UP, 0, true, true, GPIO_PULL_STATE);

  board_gpio_write(BUTTON_DOWN, -1);
  board_gpio_config(BUTTON_DOWN, 0, true, true, GPIO_PULL_STATE);

  board_gpio_write(BUTTON_LEFT, -1);
  board_gpio_config(BUTTON_LEFT, 0, true, true, GPIO_PULL_STATE);

  board_gpio_write(BUTTON_RIGHT, -1);
  board_gpio_config(BUTTON_RIGHT, 0, true, true, GPIO_PULL_STATE);

  return 0;
}


SpresenseGamepadFairy::~SpresenseGamepadFairy()
{
}


uint8_t SpresenseGamepadFairy::get_button(int pin)
{
  return board_gpio_read(pin) == BUTTON_PORARITY_PUSH ? 1 : 0;
}

void SpresenseGamepadFairy::onVBlank()
{
	int stick_pos;

	state = 0;

  state |= get_button(BUTTON_UP)    ? GamepadFairy::MASK_UP    : 0;
  state |= get_button(BUTTON_DOWN)  ? GamepadFairy::MASK_DOWN  : 0;
  state |= get_button(BUTTON_LEFT)  ? GamepadFairy::MASK_LEFT  : 0;
  state |= get_button(BUTTON_RIGHT) ? GamepadFairy::MASK_RIGHT : 0;

  state |= get_button(BUTTON_A)      << GamepadFairy::A;
  state |= get_button(BUTTON_B)      << GamepadFairy::B;
  state |= get_button(BUTTON_START)  << GamepadFairy::START;
  state |= get_button(BUTTON_SELECT) << GamepadFairy::SELECT;

#if 0
  printf("  %c%c%c%c%c%c%c%c\n",
        state & GamepadFairy::MASK_UP     ? 'U' : '.',
        state & GamepadFairy::MASK_DOWN   ? 'D' : '.',
        state & GamepadFairy::MASK_LEFT   ? 'L' : '.',
        state & GamepadFairy::MASK_RIGHT  ? 'R' : '.',
        state & GamepadFairy::MASK_A      ? 'A' : '.',
        state & GamepadFairy::MASK_B      ? 'B' : '.',
        state & GamepadFairy::MASK_START  ? 'S' : '.',
        state & GamepadFairy::MASK_SELECT ? 's' : '.'
      );
#endif
}

void SpresenseGamepadFairy::onUpdate()
{
}

bool SpresenseGamepadFairy::isPressed(uint8_t keyIdx)
{
  return ((state >> keyIdx) & 1) == 1;
}


