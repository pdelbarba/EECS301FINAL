#include "count_binary.h"

/* A "loop counter" variable. */
static alt_u8 count;
/* A variable to hold the value of the button pio edge capture register. */
volatile int edge_capture;


/* Button pio functions */

/*
  Some simple functions to:
  1.  Define an interrupt handler function.
  2.  Register this handler in the system.
*/

/*******************************************************************
 * static void handle_button_interrupts( void* context, alt_u32 id)*
 *                                                                 *  
 * Handle interrupts from the buttons.                             *
 * This interrupt event is triggered by a button/switch press.     *
 * This handler sets *context to the value read from the button    *
 * edge capture register.  The button edge capture register        *
 * is then cleared and normal program execution resumes.           *
 * The value stored in *context is used to control program flow    *
 * in the rest of this program's routines.                         *
 *                                                                 *
 * Provision is made here for systems that might have either the   *
 * legacy or enhanced interrupt API active, or for the Nios II IDE *
 * which does not support enhanced interrupts. For systems created *
 * using the Nios II softawre build tools, the enhanced API is     *
 * recommended for new designs.                                    *
 ******************************************************************/
#ifdef BUTTON_PIO_BASE

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
static void handle_button_interrupts(void* context)
#else
static void handle_button_interrupts(void* context, alt_u32 id)
#endif
{
    /* Cast context to edge_capture's type. It is important that this be 
     * declared volatile to avoid unwanted compiler optimization.
     */
    volatile int* edge_capture_ptr = (volatile int*) context;
    /* Store the value in the Button's edge capture register in *context. */
    *edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE);
    /* Reset the Button's edge capture register. */
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0);
    
    /* 
     * Read the PIO to delay ISR exit. This is done to prevent a spurious
     * interrupt in systems with high processor -> pio latency and fast
     * interrupts.
     */
    IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE); 
}

/* Initialize the button_pio. */

static void init_button_pio()
{
    /* Recast the edge_capture pointer to match the alt_irq_register() function
     * prototype. */
    void* edge_capture_ptr = (void*) &edge_capture;
    /* Enable all 4 button interrupts. */
    IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTON_PIO_BASE, 0xf);
    /* Reset the edge capture register. */
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_PIO_BASE, 0x0);
    /* Register the interrupt handler. */
#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
    alt_ic_isr_register(BUTTON_PIO_IRQ_INTERRUPT_CONTROLLER_ID, BUTTON_PIO_IRQ, 
      handle_button_interrupts, edge_capture_ptr, 0x0);
#else
    alt_irq_register( BUTTON_PIO_IRQ, edge_capture_ptr, 
      handle_button_interrupts);
#endif
}
#endif /* BUTTON_PIO_BASE */

#ifdef SEVEN_SEG_PIO_BASE
static void sevenseg_set_hex(int hex)
{
    static alt_u8 segments[16] = {
        0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84, /* 0-9 */
        0x88, 0xE0, 0xF2, 0xC2, 0xB0, 0xB8 };                       /* a-f */

    unsigned int data = segments[hex & 15] | (segments[(hex >> 4) & 15] << 8);

    IOWR_ALTERA_AVALON_PIO_DATA(SEVEN_SEG_PIO_BASE, data);
}
#endif

static void lcd_init( FILE *lcd )
{
    /* If the LCD Display exists, write a simple message on the first line. */
    LCD_PRINTF(lcd, "%c%s Counting will be displayed below...", ESC,
               ESC_TOP_LEFT);
}

static void initial_message()
{
    printf("\n\n**************************\n");
    printf("* Hello from Nios II!    *\n");
    printf("* Counting from 00 to ff *\n");
    printf("**************************\n");
}

static void count_led()
{
#ifdef LED_PIO_BASE
    IOWR_ALTERA_AVALON_PIO_DATA(
        LED_PIO_BASE,
        count
        );
#endif
}

static void count_sevenseg()
{
#ifdef SEVEN_SEG_PIO_BASE
    sevenseg_set_hex(count);
#endif
}

static void count_lcd( void* arg )
{
    FILE *lcd = (FILE*) arg;
    LCD_PRINTF(lcd, "%c%s 0x%x\n", ESC, ESC_COL2_INDENT5, count);
}

/* count_all merely combines all three peripherals counting */

static void count_all( void* arg )
{
    count_led();
    count_sevenseg();
    count_lcd( arg );
    printf("%02x,  ", count);
}
  

static void handle_button_press(alt_u8 type, FILE *lcd)
{
    /* Button press actions while counting. */
    if (type == 'c')
    {
        switch (edge_capture) 
        {
            /* Button 1:  Output counting to LED only. */
        case 0x1:
            count_led();
            break;
            /* Button 2:  Output counting to SEVEN SEG only. */
        case 0x2:
            count_sevenseg();
            break;
            /* Button 3:  Output counting to D only. */
        case 0x4:
            count_lcd( lcd );
            break;
            /* Button 4:  Output counting to LED, SEVEN_SEG, and D. */ 
        case 0x8:
            count_all( lcd );
            break;
            /* If value ends up being something different (shouldn't) do
               same as 8. */
        default:
            count_all( lcd );
            break;
        }
    }
    /* If 'type' is anything else, assume we're "waiting"...*/
    else
    {
        switch (edge_capture)
        {
        case 0x1:
            printf( "Button 1\n");
            edge_capture = 0;
            break;
        case 0x2:
            printf( "Button 2\n");
            edge_capture = 0;
            break;
        case 0x4:
            printf( "Button 3\n");
            edge_capture = 0;
            break;
        case 0x8:
            printf( "Button 4\n");
            edge_capture = 0;
            break;
        default:
            printf( "Button press UNKNOWN!!\n");
            break;
        }
    }
}
    
int main(void)
{ 
    int i;
    int wait_time;
    FILE * lcd;

    count = 0;

    /* Initialize the LCD, if there is one.
     */
    lcd = LCD_OPEN();
    if(lcd != NULL) {lcd_init( lcd );}
    
    /* Initialize the button pio. */

#ifdef BUTTON_PIO_BASE
    init_button_pio();
#endif

/* Initial message to output. */

    initial_message();

/* Continue 0-ff counting loop. */

    while( 1 ) 
    {
        usleep(100000);
        if (edge_capture != 0)
        {
            /* Handle button presses while counting... */
            handle_button_press('c', lcd);
        }
        /* If no button presses, try to output counting to all. */
        else
        {
            count_all( lcd );
        }
        /*
         * If done counting, wait about 7 seconds...
         * detect button presses while waiting.
         */
        if( count == 0xff )
        {
            LCD_PRINTF(lcd, "%c%s %c%s %c%s Waiting...\n", ESC, ESC_TOP_LEFT,
                       ESC, ESC_CLEAR, ESC, ESC_COL1_INDENT5);
            printf("\nWaiting...");
            edge_capture = 0; /* Reset to 0 during wait/pause period. */

            /* Clear the 2nd. line of the LCD screen. */
            LCD_PRINTF(lcd, "%c%s, %c%s", ESC, ESC_COL2_INDENT5, ESC,
                       ESC_CLEAR);
            wait_time = 0;
            for (i = 0; i<70; ++i)
            {
                printf(".");
                wait_time = i/10;
                LCD_PRINTF(lcd, "%c%s %ds\n", ESC, ESC_COL2_INDENT5,
                    wait_time+1);

                if (edge_capture != 0) 
                {
                    printf( "\nYou pushed:  " );
                    handle_button_press('w', lcd);
                }
                usleep(100000); /* Sleep for 0.1s. */
            }
            /*  Output the "loop start" messages before looping, again.
             */
            initial_message();
            lcd_init( lcd );
        }
        count++;
    }
    LCD_CLOSE(lcd);
    return 0;
}
