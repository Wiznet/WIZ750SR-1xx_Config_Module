
// @includes
#include "ssd1306.h"

// +---------------------------+
// |      Set MUX Ratio        |
// +---------------------------+
// |        0xA8, 0x3F         |
// +---------------------------+
//              |
// +---------------------------+
// |    Set Display Offset     |
// +---------------------------+
// |        0xD3, 0x00         |
// +---------------------------+
//              |
// +---------------------------+
// |  Set Display Start Line   |
// +---------------------------+
// |          0x40             |
// +---------------------------+
//              |
// +---------------------------+
// |     Set Segment Remap     |
// +---------------------------+
// |       0xA0 / 0xA1         |
// +---------------------------+
//              |
// +---------------------------+
// |   Set COM Output Scan     |
// |        Direction          |
// +---------------------------+
// |       0xC0 / 0xC8         |
// +---------------------------+
//              |
// +---------------------------+
// |   Set COM Pins hardware   |
// |       configuration       |
// +---------------------------+
// |        0xDA, 0x02         |
// +---------------------------+
//              |
// +---------------------------+
// |   Set Contrast Control    |
// +---------------------------+
// |        0x81, 0x7F         |
// +---------------------------+
//              |
// +---------------------------+
// | Disable Entire Display On |
// +---------------------------+
// |          0xA4             |
// +---------------------------+
//              |
// +---------------------------+
// |    Set Normal Display     |
// +---------------------------+
// |          0xA6             |
// +---------------------------+
//              |
// +---------------------------+
// |    Set Osc Frequency      |
// +---------------------------+
// |       0xD5, 0x80          |
// +---------------------------+
//              |
// +---------------------------+
// |    Enable charge pump     |
// |        regulator          |
// +---------------------------+
// |       0x8D, 0x14          |
// +---------------------------+
//              |
// +---------------------------+
// |        Display On         |
// +---------------------------+
// |          0xAF             |
// +---------------------------+

// @array Init command
const uint8_t INIT_SSD1306[] = {
  18,                                                             // number of initializers
  0, SSD1306_DISPLAY_OFF,                                         // 0xAE = Set Display OFF
  1, SSD1306_SET_MUX_RATIO, 63,                                   // 0xA8 - 64MUX for 128 x 64 version
                                                                  //      - 32MUX for 128 x 32 version
  1, SSD1306_MEMORY_ADDR_MODE, 0x00,                              // 0x20 = Set Memory Addressing Mode
                                                                  // 0x00 - Horizontal Addressing Mode
                                                                  // 0x01 - Vertical Addressing Mode
                                                                  // 0x02 - Page Addressing Mode (RESET)
  2, SSD1306_SET_COLUMN_ADDR, START_COLUMN_ADDR, END_COLUMN_ADDR, // 0x21 = Set Column Address, 0 - 127
  2, SSD1306_SET_PAGE_ADDR, START_PAGE_ADDR, END_PAGE_ADDR,       // 0x22 = Set Page Address, 0 - 7
  0, SSD1306_SET_START_LINE,                                      // 0x40
  1, SSD1306_DISPLAY_OFFSET, 0x00,                                // 0xD3
  0, SSD1306_SEG_REMAP_OP,                                        // 0xA0 / remap 0xA1
  0, SSD1306_COM_SCAN_DIR_OP,                                     // 0xC0 / remap 0xC8
  1, SSD1306_COM_PIN_CONF, 0x12,                                  // 0xDA, 0x12 - Disable COM Left/Right remap, Alternative COM pin configuration
                                                                  //       0x12 - for 128 x 64 version
                                                                  //       0x02 - for 128 x 32 version
  1, SSD1306_SET_CONTRAST, 0x7F,                                  // 0x81, 0x7F - reset value (max 0xFF)
  0, SSD1306_DIS_ENT_DISP_ON,                                     // 0xA4
  0, SSD1306_DIS_NORMAL,                                          // 0xA6
  1, SSD1306_SET_OSC_FREQ, 0x80,                                  // 0xD5, 0x80 => D=1; DCLK = Fosc / D <=> DCLK = Fosc
  1, SSD1306_SET_PRECHARGE, 0xc2,                                 // 0xD9, higher value less blinking
                                                                  // 0xC2, 1st phase = 2 DCLK,  2nd phase = 13 DCLK
  1, SSD1306_VCOM_DESELECT, 0x20,                                 // Set V COMH Deselect, reset value 0x22 = 0,77xUcc
  1, SSD1306_SET_CHAR_REG, 0x14,                                  // 0x8D, Enable charge pump during display on
  0, SSD1306_DISPLAY_ON                                           // 0xAF = Set Display ON
};

// @var array Chache memory Lcd 8 * 128 = 1024
static char cacheMemLcd[CACHE_SIZE_MEM];


/**
 * @desc    SSD1306 Init
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_Init(I2C_ConfigStruct* conf, uint8_t address)
{ 
  // 변수
  const uint8_t *commands = INIT_SSD1306;
  // 명령어 수
  unsigned short int no_of_commands = *commands++; // 첫 번째 바이트를 읽고 다음 주소로 이동
  // 인자
  uint8_t no_of_arguments;
  // 명령어
  uint8_t command;
  // 초기 상태
  uint8_t status = INIT_STATUS;

  // I2C: 초기화
  // -------------------------------------------------------------------------------------
  I2C_Init(conf);

  // I2C: 시작 및 SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW(conf, address);
  // 요청 - I2C 시작
  if (SSD1306_SUCCESS != status) {
   // error
    printf("address writing error\r\n");
    return status;
  }
  else{
    printf("address writing sucess\r\n");
  }

  // 명령어 루프
  while (no_of_commands) {

    // 인자 수
    no_of_arguments = *commands++; // 다음 바이트를 인자 수로 읽음

    // 명령어
    command = *commands++; // 명령어로 사용할 다음 바이트를 읽음

    // 명령어 전송
    // -------------------------------------------------------------------------------------
    status = SSD1306_Send_Command(conf, command);
    // 요청 - I2C 시작
    if (SSD1306_SUCCESS != status) {
      // 에러
      printf("1 SSD1306_Send_Command error\r\n");
      return status;
    }
    else{
      printf("1 SSD1306_Send_Command sucess\r\n");
    }

    // 인자 전송
    // -------------------------------------------------------------------------------------
    while (no_of_arguments--) {
      // 명령어 전송
      status = SSD1306_Send_Command(conf, *commands++); // 다음 바이트를 인자로 읽음
      // 요청 - I2C 시작
      if (SSD1306_SUCCESS != status) {
        // 에러
        printf("2 SSD1306_Send_Command error\r\n");
        return status;
      }
      else{
        printf("2 SSD1306_Send_Command sucess\r\n");
      }
    }
    // 감소
    no_of_commands--;
  }

  // I2C: 정지
  // -------------------------------------------------------------------------------------
  I2C_Stop(conf);

  // 성공
  return SSD1306_SUCCESS;
}


/**
 * @desc    SSD1306 Send Start and SLAW request
 *
 * @param   uint8_t
 *
 * @return  uint8_t
 */

uint8_t SSD1306_Send_StartAndSLAW (I2C_ConfigStruct* conf, uint8_t address)
{
  // init status
  uint8_t status = INIT_STATUS;

  // I2C: start
  // -------------------------------------------------------------------------------------
  I2C_Start(conf);

   status = I2C_WriteByte(conf, (address << 1));
  // request - send SLAW
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Send command
 *
 * @param   uint8_t command
 *
 * @return  uint8_t
 */

uint8_t SSD1306_Send_Command(I2C_ConfigStruct* conf, uint8_t command)
{

  // init status
  uint8_t status = INIT_STATUS;

  // send control byte
  // -------------------------------------------------------------------------------------   
  status = I2C_WriteByte(conf, SSD1306_COMMAND);
  // request - start I2C
  if (SSD1306_SUCCESS != status) {
    // error
    printf("Command mode error\r\n");
    return status;
  }
  else{
    printf("Command mode sucess\r\n");
  }

  // send command
  // -------------------------------------------------------------------------------------   
  status = I2C_WriteByte(conf, command);
  // request - start I2C
  if (SSD1306_SUCCESS != status) {
    // error
    printf("Command send error\r\n");
    return status;
  }
  else{
    printf("Command send sucess\r\n");
  }

 
  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Normal colors
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_NormalScreen (I2C_ConfigStruct* conf, uint8_t address)
{
  // init status
  uint8_t status = INIT_STATUS;

  // I2C: start & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (conf ,address);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command (conf ,SSD1306_DIS_NORMAL);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Inverse colors
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_InverseScreen (I2C_ConfigStruct* conf, uint8_t address)
{
  // init status
  uint8_t status = INIT_STATUS;

  // I2C: start & SLAW
  // -------------------------------------------------------------------------------------
   status = SSD1306_Send_StartAndSLAW (conf ,address);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------------------------------------------------------------------   
  status = SSD1306_Send_Command (conf ,SSD1306_DIS_NORMAL);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Update screen
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_UpdateScreen (I2C_ConfigStruct* conf, uint8_t address)
{
  // init status
  uint8_t status = INIT_STATUS;
  // init i
  uint16_t i = 0;

  // I2C: start & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (conf ,address);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------------------------------------------------------------------   
  status = I2C_WriteByte(conf, SSD1306_DATA_STREAM);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  //  send cache memory lcd
  // -------------------------------------------------------------------------------------
  while (i < CACHE_SIZE_MEM) {
    // send data
    status = I2C_WriteByte(conf, cacheMemLcd[i]);
    // request succesfull
    if (SSD1306_SUCCESS != status) {
      // error
      return status;
    }
    // increment
    i++;
  }

  // stop I2C
  I2C_Stop(conf);

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Clear screen
 *
 * @param   void
 *
 * @return  void
 */
void SSD1306_ClearScreen (void)
{
  // null cache memory lcd
  memset (cacheMemLcd, 0x00, CACHE_SIZE_MEM);
}

/**
 * @desc    SSD1306 Set position
 *
 * @param   uint8_t column -> 0 ... 127 
 * @param   uint8_t page -> 0 ... 7 or 3 
 *
 * @return  void
 */
 unsigned int _counter;
void SSD1306_SetPosition (uint8_t x, uint8_t y) 
{
  // calculate counter
  _counter = x + (y << 7);
}

/**
 * @desc    SSD1306 Update text poisition - this ensure that character will not be divided at the end of row, 
 *          the whole character will be depicted on the new row
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_UpdatePosition (void) 
{
  // y / 8
  uint8_t y = _counter >> 7;
  // y % 8
  uint8_t x = _counter - (y << 7);
  // x + character length + 1
  uint8_t x_new = x + CHARS_COLS_LENGTH + 1;

  // check position
  if (x_new > END_COLUMN_ADDR) {
    // if more than allowable number of pages
    if (y > END_PAGE_ADDR) {
      // return out of range
      return SSD1306_ERROR;
    // if x reach the end but page in range
    } else if (y < (END_PAGE_ADDR-1)) {
      // update
      _counter = ((++y) << 7);
    }
  }
 
  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Draw character
 *
 * @param   char character
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawChar(char character)
{
  // variables
  uint8_t i = 0;

  // update text position
  // this ensure that character will not be divided at the end of row, the whole character will be depicted on the new row
  if (SSD1306_UpdatePosition() == SSD1306_ERROR) {
    // error
    return SSD1306_ERROR;
  }

  // loop through 5 bits
  while (i < CHARS_COLS_LENGTH) {
    // read byte
    cacheMemLcd[_counter++] = FONTS[character - 32][i++];
  }

  // update position
  _counter++;

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Draw String
 *
 * @param   char * string
 *
 * @return  void
 */
void SSD1306_DrawString (const char *str)
{
  // init
  int i = 0;
  // loop through character of string
  while (str[i] != '\0') {
    // draw string
    SSD1306_DrawChar (str[i++]);
  }
}

/**
 * @desc    Draw pixel
 *
 * @param   uint8_t x -> 0 ... MAX_X
 * @param   uint8_t y -> 0 ... MAX_Y
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawPixel (uint8_t x, uint8_t y)
{
  uint8_t page = 0;
  uint8_t pixel = 0;

  // if out of range
  if ((x > MAX_X) || (y > MAX_Y)) {
    // out of range
    return SSD1306_ERROR;
  }
  // find page (y / 8)
  page = y >> 3;
  // which pixel (y % 8)
  pixel = 1 << (y - (page << 3));
  // update counter
  _counter = x + (page << 7);
  // save pixel
  cacheMemLcd[_counter++] |= pixel;

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    Draw line by Bresenham algoritm
 *  
 * @param   uint8_t x start position / 0 <= cols <= MAX_X-1
 * @param   uint8_t x end position   / 0 <= cols <= MAX_X-1
 * @param   uint8_t y start position / 0 <= rows <= MAX_Y-1 
 * @param   uint8_t y end position   / 0 <= rows <= MAX_Y-1
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawLine (uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
{
  // determinant
  int16_t D;
  // deltas
  int16_t delta_x, delta_y;
  // steps
  int16_t trace_x = 1, trace_y = 1;

  // delta x
  delta_x = x2 - x1;
  // delta y
  delta_y = y2 - y1;

  // check if x2 > x1
  if (delta_x < 0) {
    // negate delta x
    delta_x = -delta_x;
    // negate step x
    trace_x = -trace_x;
  }

  // check if y2 > y1
  if (delta_y < 0) {
    // negate detla y
    delta_y = -delta_y;
    // negate step y
    trace_y = -trace_y;
  }

  // Bresenham condition for m < 1 (dy < dx)
  if (delta_y < delta_x) {
    // calculate determinant
    D = (delta_y << 1) - delta_x;
    // draw first pixel
    if(SSD1306_DrawPixel (x1, y1) == SSD1306_ERROR){
      return SSD1306_ERROR;
    }
    // check if x1 equal x2
    while (x1 != x2) {
      // update x1
      x1 += trace_x;
      // check if determinant is positive
      if (D >= 0) {
        // update y1
        y1 += trace_y;
        // update determinant
        D -= 2*delta_x;    
      }
      // update deteminant
      D += 2*delta_y;
      // draw next pixel
      SSD1306_DrawPixel (x1, y1);
    }
  // for m > 1 (dy > dx)    
  } else {
    // calculate determinant
    D = delta_y - (delta_x << 1);
    // draw first pixel
    SSD1306_DrawPixel (x1, y1);
    // check if y2 equal y1
    while (y1 != y2) {
      // update y1
      y1 += trace_y;
      // check if determinant is positive
      if (D <= 0) {
        // update y1
        x1 += trace_x;
        // update determinant
        D += 2*delta_y;    
      }
      // update deteminant
      D -= 2*delta_x;
      // draw next pixel
      SSD1306_DrawPixel (x1, y1);
    }
  }
  // success return
  return SSD1306_SUCCESS;
}


void SSD1306_DrawBitmap(I2C_ConfigStruct* conf, uint8_t address, const uint8_t* bitmapData) {
  
	int i;
	// I2C: 시작 & SLAW
  if (SSD1306_Send_StartAndSLAW(conf, address) != SSD1306_SUCCESS) {
    // 에러 처리
    return;
  }

  // 데이터 스트림 전송
  if (SSD1306_Send_Command(conf, SSD1306_DATA_STREAM) != SSD1306_SUCCESS) {
    // 에러 처리
    return;
  }

  // 비트맵 데이터 전송
  for ( i = 0; i < sizeof(bitmapData); i++) {
    if (I2C_WriteByte(conf, bitmapData[i]) != SSD1306_SUCCESS) {
      // 에러 처리
      return;
    }
  }

  // I2C: 정지
  I2C_Stop(conf);
}

void screen_update_function(const char (*strings)[MAX_STRING_LENGTH],  int num_strings, int line_position, int flag_line_on_off) {
	int i;
  int line = 0;

    if(line_position == 0){
        line = 4;
    } 
    else if(line_position == 1){
        line = 21;
    } 
    else if(line_position == 2){
        line = 38;
    } 
        // clear screen
    SSD1306_ClearScreen ();
            // draw line
    if(flag_line_on_off == 1){
    SSD1306_DrawLine (0, MAX_X, line, line);  
    SSD1306_DrawLine (0, MAX_X, line + 14, line + 14);
    }

    for (i = 0; i < num_strings; i++) {
        SSD1306_SetPosition(2, (i*2)+1);
        SSD1306_DrawString(strings[i]);
    }
}

int line_position_select_function(int line_position){
    
		int ret;
    if(line_position == 0){
        ret = 4;
    } 
    else if(line_position == 1){
        ret = 21;
    } 
    else if(line_position == 2){
        ret = 38;
    } 
		return ret;
}


void copyStringsAll(const char (*source)[MAX_STRING_LENGTH], char (*destination)[MAX_STRING_LENGTH], int num_strings) {
    int i;
    for (i = 0; i < num_strings; i++) {
        strcpy(destination[i], source[i]);
    }
}


uint8_t copyStringsRange(const char (*source)[MAX_STRING_LENGTH], char (*destination)[MAX_STRING_LENGTH], int start, int num_strings) {
  int i;
  int j = 0;  
	for ( i = start; i < num_strings+1; i++) {
        strcpy(destination[j++], source[i]);
    }
    return j+1; // number of string return
}