#ifndef __EXAMPLE_LCD_TEST_I2C_DEVIF_H__
#define __EXAMPLE_LCD_TEST_I2C_DEVIF_H__

/**
 * Initialize I2C device. This function will try to create device file
 * if it is not exist.
 *
 * @param[in] bus_no: I2C Bus number to initialize.
 * @param[in] freq:   I2C Bus frequency.
 * @return true if the initialization is succeeded.
 */
bool i2c_init(int bus_no, uint32_t freq);


/**
 * De-initialized the I2C device.
 * This function does not delete the device file.
 */
void i2c_deinit(void);

/**
 * Execute I2C transfer to send data to the I2C device.
 *
 * @param[in] trans_8bit_adr: 8bit Address of target I2C device.
 *                            I2C device address itself is 7bit.
 *                            So LSB is avoided.
 * @param[in] data: Data address to send.
 * @param[in] len: Length of the data.
 */
int i2c_send(unsigned char trans_8bit_adr, unsigned char *data, int len);

#endif  // __EXAMPLE_LCD_TEST_I2C_DEVIF_H__
