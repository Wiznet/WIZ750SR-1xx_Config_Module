#include <stdio.h>
#include "common.h"
#include "W7500x_board.h"
#include "W7500x_it.h"
#include "W7500x_i2c.h"
#include "ssd1306.h"
#include "configData.h"
#include "uartHandler.h"
#include "common.h"
#include "dhcp.h"

#define MAX_STRING_LENGTH 22
#define PAGE_FIRST 0
#define PAGE_SECOND 1

#define LINE_FIRST 0
#define LINE_SECOND 1
#define LINE_THIRD 2

#define SSD1306_PRINT_CH_0 0
#define SSD1306_PRINT_CH_1 1
#define SSD1306_PRINT_CH_2 2

#define IS_WAITING_FOR_CONFIG 0
#define CONFIG_DATA_SELECTION 1
#define CONFIG_IN_PROGRESS 2

#define DRAW_LINE_ON 1
#define DRAW_LINE_OFF 0


void Ctrl_Decrease_Line(int8_t end_line, int8_t end_page);
void Ctrl_Increase_Line(int8_t end_line, int8_t end_page);
void Config_Module_Btn_Process(void);
void SSD1306_PRINT_DATA_PROCESS(void);
uint8_t set_flag_on_data_change(const uint32_t* prev_array, const uint32_t* current_array, uint16_t array_size);