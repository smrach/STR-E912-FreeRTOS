
#include "91x_lib.h"
#include "lcd.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bmp.h"
#include "bits.h"
#include "fonts.h"


#define SSP0_FREQ   100000 // (100KHz)

GPIO_InitTypeDef GPIO_InitStructure;
SSP_InitTypeDef	SSP_InitStructure;

void sDelay (unsigned long a) { while (--a!=0); }

unsigned int i,j;
unsigned int _currCol, _currRow;

void LCD_Initialize(void) {

	// GPIO
	// Pin for backlight (P6.4)
	// Init GPIO Structure
	/*
	GPIO_InitStructure.GPIO_Direction = GPIO_PinOutput;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Type = GPIO_Type_PushPull ;
	GPIO_InitStructure.GPIO_Alternate=GPIO_OutputAlt1;
	GPIO_Init (GPIO6, &GPIO_InitStructure);
	GPIO_WriteBit(GPIO6, GPIO_Pin_4, Bit_SET);
	*/
	
	// Pin for LCD_CS (P0.1)
	GPIO_InitStructure.GPIO_Direction = GPIO_PinOutput;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Type = GPIO_Type_PushPull ;
	GPIO_InitStructure.GPIO_Alternate=GPIO_OutputAlt1;
	GPIO_Init (GPIO0, &GPIO_InitStructure);
	GPIO_WriteBit(GPIO0, GPIO_Pin_1, Bit_RESET);

	// SSP1
	// Enable the __SSP1 Clock
	SCU_APBPeriphClockConfig(__SSP1 ,ENABLE);
	//
	// Set desired frequency
	//SSP1->PR = (SCU_GetPCLKFreqValue()*1000)/SSP0_FREQ;
	//SSP1->PR = 250;
	//
	// Gonfigure SSP1_CLK (P3.04), SSP1_MOSI (P3.06)
	GPIO_InitStructure.GPIO_Direction = GPIO_PinOutput;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4| GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Type = GPIO_Type_PushPull ;
	GPIO_InitStructure.GPIO_IPInputConnected = GPIO_IPInputConnected_Enable;
	GPIO_InitStructure.GPIO_Alternate = GPIO_OutputAlt2;
	GPIO_Init (GPIO3, &GPIO_InitStructure);

    // SSP1 configuration
    SSP_DeInit(SSP1);
    SSP_InitStructure.SSP_FrameFormat = SSP_FrameFormat_Motorola;
    SSP_InitStructure.SSP_Mode = SSP_Mode_Master;
    SSP_InitStructure.SSP_CPOL = SSP_CPOL_Low;
    SSP_InitStructure.SSP_CPHA = SSP_CPHA_1Edge;
    SSP_InitStructure.SSP_DataSize = SSP_DataSize_9b;
    SSP_InitStructure.SSP_ClockRate = 0;
    SSP_InitStructure.SSP_ClockPrescaler = 250;
    SSP_Init(SSP1, &SSP_InitStructure);
    //
    // Enable SSP1
    SSP_Cmd(SSP1, ENABLE);

  _currCol = 0;
  _currRow = 0;
  
  LCD_Settings();
}
static void LCD_WriteSpi(DataOrCommand_TypeDef writeType, unsigned int data){
   data =  writeType ? (data|0x0100) : (data&~0x0100);
   portENTER_CRITICAL()
    {
    while(SSP_GetFlagStatus(SSP1, SSP_FLAG_TxFifoNotFull)==RESET);
    SSP_SendData(SSP1, data);
    }
    portEXIT_CRITICAL();
}

static void LCD_WriteSpiCommand(unsigned int data){
	 data = (data & ~0x0100);

	while(SSP_GetFlagStatus(SSP1, SSP_FLAG_TxFifoNotFull)==RESET);

	SSP_SendData(SSP1, data);
}

static void LCD_WriteSpiData(unsigned int data){
	data = (data | 0x0100);

	while(SSP_GetFlagStatus(SSP1, SSP_FLAG_TxFifoNotFull)==RESET);

	SSP_SendData(SSP1, data);
}
/*
void Backlight(unsigned char state) {
  if(state == BKLGHT_LCD_ON)
    GPIO_Write(GPIO6,BIT4);     // Set P6.4 to HIGH
  else
    GPIO_Write(GPIO6,BIT4);     // Set PB6.4 to LOW
}*/

static void LCD_Settings(void) {
  LCD_WriteSpiCommand(DISCTL);	// Display control
  LCD_WriteSpiData(0x00); 		// default
  LCD_WriteSpiData(0x20); 		// (32 + 1) * 4 = 132 lines (of which 130 are visible)
  LCD_WriteSpiData(0x0a); 		// default
  LCD_WriteSpiCommand(COMSCN);	// COM scan
  LCD_WriteSpiData(0x01);  		// Scan 1-80
  LCD_WriteSpiCommand(OSCON);	// Internal oscilator ON	
  LCD_WriteSpiCommand(SLPOUT);	// Sleep out
  LCD_WriteSpiCommand(VOLCTR); 	// Voltage control
  LCD_WriteSpiData(0x23); 		// middle value of V1
  LCD_WriteSpiData(0x03); 		// middle value of resistance value
  LCD_WriteSpiCommand(TMPGRD);	// Temperature gradient
  LCD_WriteSpiData(0x00); 		// default
  LCD_WriteSpiCommand(PWRCTR);  // Power control
  LCD_WriteSpiData(0x0f);   	// referance voltage regulator on, circuit voltage follower on, BOOST ON
  LCD_WriteSpiCommand(DISNOR);	// Normal display
  LCD_WriteSpiCommand(DISINV);	// Inverse display
  LCD_WriteSpiCommand(PTLOUT);  // Partial area off
  LCD_WriteSpiCommand(DATCTL);	// Data control
  LCD_WriteSpiData(0x00); 		// all inversions off, column direction
  LCD_WriteSpiData(0x03); 		// RGB sequence
  LCD_WriteSpiData(0x02); 		// Grayscale -> 16
  LCD_WriteSpiCommand(PASET);	// Page Address set
  LCD_WriteSpiData(0);
  LCD_WriteSpiData(131);
  LCD_WriteSpiCommand(CASET);	// Page Column set
  LCD_WriteSpiData(0);
  LCD_WriteSpiData(131);
  sDelay(10000);// wait ~100ms
}
void _window(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	unsigned int x1 = x + 2;
	unsigned int y1 = y + 0;
	unsigned int x2 = x1 + width - 1;
	unsigned int y2 = y1 + height - 1;

    LCD_WriteSpiCommand(CASET); // column
    LCD_WriteSpiData(x1);
    LCD_WriteSpiData(x2);
    LCD_WriteSpiCommand(PASET); // row
    LCD_WriteSpiData(y1);
    LCD_WriteSpiData(y2);
    LCD_WriteSpiCommand(RAMWR); // start write to ram
}
void LCD_DisplayChar(u8 ascii, LcdTextColor_TypeDef charColors){
    unsigned int i, j;

    u8 Byte_1 = 0;
    u8 Byte_2 = 0;
    u8 Byte_3 = 0;
    u8 Pixel = 0;

    _window(_currCol, _currRow, FONT_WIDTH, FONT_HEIGHT);

    for (j = 0; j < FONT_HEIGHT; j++)
    {
        Pixel = font_digits[j + (FONT_HEIGHT * ascii)];
        for (i = 0; i < FONT_WIDTH; i++)
        {
            if (i % 2)
            {
                if (Pixel & 0x80)      // Pixel ON
                {
                    Byte_2 &= 0xF0;
                    Byte_2 |= charColors.ForeColor >> 8;
                    Byte_3 = charColors.ForeColor & 0x00FF;
                }
                else                  // Pixel OFF
                {
                    Byte_2 &= 0xF0;
                    Byte_2 |= charColors.BackColor >> 8;
                    Byte_3 = charColors.BackColor & 0x00FF;
                }
                LCD_WriteSpiData(Byte_1);
                LCD_WriteSpiData(Byte_2);
                LCD_WriteSpiData(Byte_3);
            }
            else
            {
                if (Pixel & 0x80)      // Pixel ON
                {
                    Byte_1 = charColors.ForeColor >> 4;
                    Byte_2 = charColors.ForeColor << 4;
                }
                else                // Pixel OFF
                {
                    Byte_1 = charColors.BackColor >> 4;
                    Byte_2 = 0;
                    Byte_2 |= charColors.BackColor << 4;
                }
            }
            Pixel <<= 1;
        }
    }
    _currCol += FONT_WIDTH;
    if (_currCol + FONT_WIDTH > LCD_WIDTH) {
    	_currCol = 0;
    	_currRow += FONT_HEIGHT;
    	if (_currRow + FONT_HEIGHT > LCD_HEIGHT) {
    	   	_currRow = 0;
    	}
    }
}
/* Send the string character by character on lCD */
void LCD_DisplayString(u8 *buf, LcdTextSettings_TypeDef textSettings){
  //if (textSettings.Position.RowPosition)
    _currRow = textSettings.Position.RowPosition;
  //if (textSettings.Position.ColumnPosition)
    _currCol = textSettings.Position.ColumnPosition;
  while (*buf!=0)
  {
	  vTaskDelay( 1 );
  	/* Display one character on LCD */
    LCD_DisplayChar(*buf, textSettings.Colors);
    /* Point on the next character */
    buf++;
  }
}
 /* 
  / Display with 4096 colors 3 bytes => 2 pixels
  / Byte1 -> RRRRGGGG
  / Byte2 -> BBBBRRRR
  / Byte3 -> GGGGBBBB
  */
void LCD_WriteLogo(void) {
	LCD_WriteSpiCommand(DISOFF);    // Display OFF
	LCD_WriteSpiCommand(RAMWR); 	// WRITE MEMORY
	for(j=0; j<sizeof(bmp); j++) {
		LCD_WriteSpiData(bmp[j]);	//Send Bitmap Data
	}
	//vTaskDelay(10); 				// wait ~100ms 	
	sDelay(10000);
	LCD_WriteSpiCommand(DISON);	    // Display On
}
