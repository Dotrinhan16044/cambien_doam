/*
 * lcd_i2c.h
 *
 *  Created on: Feb 22, 2023
 *      Author: Tuan
 */
#ifndef lcd_i2c_H_
#define lcd_i2c_H_

#include "main.h"

#include "stdio.h"
#include "stdint.h"
#include "string.h"

//commands
#define lcd_cleardisplay 	0x01
#define lcd_displaycontrol	0x08
#define lcd_functionset		0x28
#define lcd_entrymode 		0x04

//display
#define lcd_line1 			0x80
#define lcd_line2 			0xC0

// flags for display on/off control
#define lcd_display_on		0x04
#define lcd_display_off		0x00
#define lcd_cursor_on		0x02
#define lcd_cursor_off		0x00
#define lcd_blinkOn 		0x01
#define lcd_blinkOff 		0x00

//flags for backlight control
#define lcd_backlight_on 	0x08
#define lcd_backlight_off	0x00


#define En					0b00000100
#define Rw					0b00000010
#define Rs					0b00000001

typedef struct
{
	uint8_t LCD_Columns;
	uint8_t LCD_Rows;
	uint8_t SLAVE_ADDRESS;
	uint8_t LCD_Backlight_Value;
	uint8_t LCD_Display_Option;
	I2C_HandleTypeDef *hi2c;
}LCD_I2C_HandleTypeDef;

void lcd_i2c_init(LCD_I2C_HandleTypeDef *p_LCD, I2C_HandleTypeDef *p_hi2c, uint8_t p_col, uint8_t p_row, uint8_t p_Slave_Address);
void lcd_set_cursor(LCD_I2C_HandleTypeDef *p_LCD, uint8_t p_col, uint8_t p_row);
void lcd_send_cmd(LCD_I2C_HandleTypeDef *p_LCD, char p_cmd);	// send commands to the LCD
void lcd_send_data(LCD_I2C_HandleTypeDef *p_LCD, char p_data);	// send data to the LCD
void lcd_send_string (LCD_I2C_HandleTypeDef *p_LCD, char *str);	// send string to the LCD
void lcd_clear(LCD_I2C_HandleTypeDef *p_LCD);
//void delay_us(uint32_t us);    //delay micro second

#endif /* LCD_I2C_H_ */
