#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "system.h"
#include <stdio.h>
#include <unistd.h>

#ifndef LCD_DISPLAY_NAME
/* Some hardware is not present because of system or because of simulation */
#   define LCD_CLOSE(x) /* Do Nothing */
#   define LCD_OPEN() NULL
#   define LCD_PRINTF(lcd, args...) /* Do Nothing */

#else
/* With hardware devices present, use these definitions */
#   define LCD_CLOSE(x) fclose((x))
#   define LCD_OPEN() fopen("/dev/lcd_display", "w")
#   define LCD_PRINTF fprintf

#endif

/* Cursor movement on the LCD */
/* Clear */
#define ESC 27
/* Position cursor at row 1, column 1 of LCD. */
#define ESC_CLEAR "K"
/* Position cursor at row1, column 5 of LCD. */
#define ESC_COL1_INDENT5 "[1;5H"
/* Position cursor at row2, column 5 of LCD. */
#define ESC_COL2_INDENT5 "[2;5H"
/* Integer ASCII value of the ESC character. */
#define ESC_TOP_LEFT "[1;0H"
