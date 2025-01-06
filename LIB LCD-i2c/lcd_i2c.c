/*
 * lcd_i2c.c
 *
 *  Created on: Feb 22, 2023
 *      Author: Tuan
 */

#include "lcd_i2c.h"


void lcd_i2c_init(LCD_I2C_HandleTypeDef *p_LCD, I2C_HandleTypeDef *p_hi2c, uint8_t p_col, uint8_t p_row, uint8_t p_Slave_Address)
{
	p_LCD->SLAVE_ADDRESS = p_Slave_Address;
	p_LCD->LCD_Backlight_Value = lcd_backlight_on;
	p_LCD->LCD_Columns = p_col;
	p_LCD->LCD_Rows = p_row;
	p_LCD->hi2c = p_hi2c;
	p_LCD->LCD_Display_Option = lcd_display_on | lcd_cursor_off | lcd_blinkOff;


	//4 bit initialization
	HAL_Delay(40); 	//waiting for > 40ms
	lcd_send_cmd(p_LCD, 0x30);
	HAL_Delay(5);  // waiting for >4.1ms
	lcd_send_cmd(p_LCD, 0x30);
	HAL_Delay(1);  // waiting for >100us
	lcd_send_cmd(p_LCD, 0x30);
	HAL_Delay(10);
	lcd_send_cmd(p_LCD, 0x20);	//4 bit mode
	HAL_Delay(10);

	//display initialization
	lcd_send_cmd(p_LCD, 0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd(p_LCD, 0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off		HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd(p_LCD, 0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd(p_LCD, 0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd(p_LCD, lcd_displaycontrol | p_LCD -> LCD_Display_Option); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	HAL_Delay(1);
}

void lcd_send_cmd (LCD_I2C_HandleTypeDef *p_lcd, char cmd)
{
	char data_h, data_l;
	uint8_t data_t[4];
	data_h = (cmd & 0xf0);
	data_l = ((cmd<<4) & 0xf0);

	data_t[0] = data_h | 0x0C;	//en=1, rs=0
	data_t[1] = data_h | 0x08;	//en=0, rs=0
	data_t[2] = data_l | 0x0C;	//en=1, rs=0
	data_t[3] = data_l | 0x08;	//en=0, rs=1

	HAL_I2C_Master_Transmit(p_lcd->hi2c, p_lcd->SLAVE_ADDRESS, data_t, 4, 100);
}

void lcd_send_data (LCD_I2C_HandleTypeDef *p_LCD, char data)
{
	char data_h, data_l;
	uint8_t data_t[4];
	data_h = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);

	data_t[0] = data_h | 0x0D; //en=1, rs=0
	data_t[1] = data_h | 0x09; //en=0, rs=0
	data_t[2] = data_l | 0x0D;
	data_t[3] = data_l | 0x09;

	HAL_I2C_Master_Transmit(p_LCD -> hi2c, p_LCD->SLAVE_ADDRESS, data_t, 4, 100);
}


void lcd_set_cursor(LCD_I2C_HandleTypeDef *p_LCD, uint8_t p_col, uint8_t p_row)
{
	uint8_t t_row_Offets[] = {0x00, 0x40, 0x14, 0x54};
	if(p_row > p_LCD->LCD_Rows) p_row = p_LCD->LCD_Rows - 1;
	lcd_send_cmd(p_LCD, 0x80 | (p_col + t_row_Offets[p_row]));
}

void lcd_send_string (LCD_I2C_HandleTypeDef *p_LCD, char *str)
{
	while (*str) lcd_send_data(p_LCD, *str++);
}

void lcd_clear (LCD_I2C_HandleTypeDef *p_LCD)
{
	lcd_send_cmd(p_LCD, lcd_cleardisplay);
	HAL_Delay(5);
	lcd_set_cursor(p_LCD, 0, 0);
}
