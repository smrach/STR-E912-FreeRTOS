#ifndef __LCD_H
#define __LCD_H

/* Includes --------------------------------------------------------------------*/
#include "91x_lib.h"

/* Text color mode */
  typedef enum
  {
    BlackText=0,
    WhiteText=1
  } TextColorMode_TypeDef;
  
  typedef enum
  {
	  Command = 0,
	  Data = 1
  } DataOrCommand_TypeDef;

  typedef struct 
  {
	  u16 ForeColor;
	  u16 BackColor;
  } LcdTextColor_TypeDef;

  typedef struct 
  {
	  u8 RowPosition;
	  u8 ColumnPosition;
  } LcdTextPosition_TypeDef;

  typedef struct 
  {
	  LcdTextColor_TypeDef Colors;
	  LcdTextPosition_TypeDef Position;
  } LcdTextSettings_TypeDef;

#define BKLGHT_LCD_ON        1
#define BKLGHT_LCD_OFF       2

#define DISON     0xAF      // Display on
#define DISOFF    0xAE      // Display off
#define DISNOR    0xA6      // Normal display
#define DISINV    0xA7      // Inverse display
#define COMSCN    0xBB      // Common scan direction
#define DISCTL    0xCA      // Display control
//  #define DISCTL    0xBA      // Display control
#define SLPIN     0x95      // Sleep in
#define SLPOUT    0x94      // Sleep out
#define PASET     0x75      // Page address set
#define CASET     0x15      // Column address set
#define DATCTL    0xBC      // Data scan direction, etc.
#define RGBSET8   0xCE      // 256-color position set
#define RAMWR     0x5C      // Writing to memory
#define RAMRD     0x5D      // Reading from memory
#define PTLIN     0xA8      // Partial display in
#define PTLOUT    0xA9      // Partial display out
#define RMWIN     0xE0      // Read and modify write
#define RMWOUT    0xEE      // End
#define ASCSET    0xAA      // Area scroll set
#define SCSTART   0xAB      // Scroll start set
#define OSCON     0xD1      // Internal oscillation on
#define OSCOFF    0xD2      // Internal oscillation off
#define PWRCTR    0x20      // Power control
#define VOLCTR    0x81      // Electronic volume control
#define VOLUP     0xD6      // Increment electronic control by 1
#define VOLDOWN   0xD7      // Decrement electronic control by 1
#define TMPGRD    0x82      // Temperature gradient set
#define EPCTIN    0xCD      // Control EEPROM
#define EPCOUT    0xCC      // Cancel EEPROM control
#define EPMWR     0xFC      // Write into EEPROM
#define EPMRD     0xFD      // Read from EEPROM
#define EPSRRD1   0x7C      // Read register 1
#define EPSRRD2   0x7D      // Read register 2
#define NOP       0x25      // NOP instruction

#define LCD_COLOR_BLACK         0x000           ///< Black color
#define LCD_COLOR_YELLOW        0x1EF           ///< Yellow color
#define LCD_COLOR_GRAY          0xCCC           ///< Gray
#define LCD_COLOR_BLUE          0xF00           ///< Blue
#define LCD_COLOR_GREEN         0x0F0           ///< Green
#define LCD_COLOR_RED           0x00F           ///< Red
#define LCD_COLOR_WHITE         0xFFF           ///< White
#define LCD_DEFAULT_FG_COLOR    LCD_COLOR_BLACK   // black
#define LCD_DEFAULT_BG_COLOR    LCD_COLOR_WHITE   // white

// Warning: maybe some correction needed to GE12!!!
#define LCD_WIDTH               (130)     ///< in pixels
#define LCD_HEIGHT              (131)     ///< in pixels
#define LCD_COLORS              (4096)    ///< 12 bits
//#define LCD_MEM_SIZE          (25350)   // 130*130*12/8
#define LCD_MEM_SIZE            (25740)   ///< 130*132*12/8

/* Dummy only for porting not really used on lcd functions */
/* LCD Lines when LCD is managed as 2*17 characters */
#define Line1    0x0
#define Line2    0x2

/*----- High layer function -----*/
void LCD_Initialize(void);
static void LCD_Settings(void);
void LCD_DisplayChar(u8 ascii, LcdTextColor_TypeDef charColors);
void LCD_DisplayString(u8 *buf, LcdTextSettings_TypeDef textSettings);
void LCD_Printf (const char *fmt, ...);

static void LCD_WriteSpi(DataOrCommand_TypeDef writeType, unsigned int data);
static void LCD_WriteSpiCommand(unsigned int data);
static void LCD_WriteSpiData(unsigned int data);

static void _WriteChar(unsigned char);
static void _window(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
void LCD_WriteLogo(void);
#endif /*__LCD_H */
