#ifndef __RW1063_H__
#define __RW1063_H__

/**
 * Initialize RW1063 based LCD Display.
 * This driver assumed 2 Lines and 5x8 charactor size.
 *
 * @param[in] dev_adr: I2C device address.
 * @param[in] clr: Clear display.
 * @param[in] curs_on: set true to enable cursor.
 * @param[in] blink_on: set true to enable blinking.
 */
void lcd_init(uint8_t dev_adr, bool clr, bool curs_on, bool blink_on);

/**
 * Clear Display.
 */
void lcd_clear(void);

/**
 * Set cursor to Home position (0,0).
 */
void lcd_gohome(void);

/**
 * Set cursor position (pos_x, pos_y).
 *
 * @param[in] pos_x: X axis position. (0:min, 39:max)
 * @param[in] pos_y: Y axis position. (0:min,  1:max)
 */
void lcd_setcurpos(int pos_x, int pos_y);

/**
 * Shift Display area to right or left.
 *
 * @param[in] right_xleft: shift right if set it true. shift left if set it false.
 */
void lcd_shift_display(bool right_xleft);

/**
 * same function of standard printf() on LCD display.
 */
void lcd_printf(const char *format, ...);

#endif  // __RW1063_H__

