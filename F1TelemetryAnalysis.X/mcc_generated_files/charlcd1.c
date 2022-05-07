/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include <stdint.h>
#include <stdbool.h>

#include "mcc.h"
#include "charlcd1.h"
#include "pin_manager.h"

#define FCY (_XTAL_FREQ/2)

#include <libpic30.h>

/* Private Definitions ***********************************************/
#define FAST_INSTRUCTION_TIME_US    50
#define SLOW_INSTRUCTION_TIME_US    1600
#define STARTUP_TIME_MS             30
#define SIGNAL_TIMING_US            1

#define MAX_COLUMN                  16

enum COMMAND
{
    COMMAND_CLEAR_SCREEN        = 0x01,
    COMMAND_RETURN_HOME         = 0x02,
    COMMAND_ENTER_DATA_MODE     = 0x06,
    COMMAND_CURSOR_OFF          = 0x0C,
    COMMAND_CURSOR_ON           = 0x0F,
    COMMAND_MOVE_CURSOR_LEFT    = 0x10,
    COMMAND_MOVE_CURSOR_RIGHT   = 0x14,
    COMMAND_SET_MODE_4_BIT      = 0x28,
    COMMAND_SET_MODE_8_BIT      = 0x38,
    COMMAND_ROW_0_HOME          = 0x80,
    COMMAND_ROW_1_HOME          = 0xC0,
    COMMAND_START_UP_1          = 0x33,   
    COMMAND_START_UP_2          = 0x32 
};
   
/* Private Functions *************************************************/
static void CarriageReturn( void );
static void ShiftCursorLeft( void );
static void ShiftCursorRight( void );
static void ShiftCursorUp( void );
static void ShiftCursorDown( void );
static void SendData( char );
static void SendCommand( unsigned int );
static void DataNibbleWrite(uint8_t value);

/* Private variables ************************************************/
static uint8_t row;
static uint8_t column;

/*********************************************************************
 * Function: bool CHARLCD1_Initialize(void);
 *
 * Overview: Initializes the LCD screen.  Can take several hundred
 *           milliseconds.
 *
 * PreCondition: none
 *
 * Input: None
 *
 * Output: true if initialized, false otherwise
 *
 ********************************************************************/
bool CHARLCD1_Initialize( void )
{          
    DataNibbleWrite(0);
    
    // RW pin is not selected in pin manager
    CharLCD1_RS_SetLow(); 
    CharLCD1_E_SetLow();     
    CharLCD1_E_SetHigh();
    __delay_ms(STARTUP_TIME_MS);
    
    SendCommand( COMMAND_SET_MODE_4_BIT );
    SendCommand( COMMAND_CURSOR_OFF );
    SendCommand( COMMAND_ENTER_DATA_MODE );

    CHARLCD1_ClearScreen();

    return true;
}


/* This function enables printf using the LCD module */
int __attribute__((__section__(".libc.write"))) write(int handle, void *buffer, unsigned int len) {
    CHARLCD1_PutString(buffer, len);
    return(len);
}

/*********************************************************************
 * Function: void CHARLCD1_PutString(char* inputString, uint16_t length);
 *
 * Overview: Puts a string on the LCD screen.  Unsupported characters will be
 *           discarded.  May block or throw away characters is LCD is not ready
 *           or buffer space is not available.  Will terminate when either a
 *           null terminator character (0x00) is reached or the length number
 *           of characters is printed, which ever comes first.
 *
 * PreCondition: already initialized via CHARLCD1_Initialize()
 *
 * Input: char* - string to print
 *        uint16_t - length of string to print
 *
 * Output: None
 *
 ********************************************************************/
void CHARLCD1_PutString( char* inputString , uint16_t length )
{
    while(length--)
    {
        switch(*inputString)
        {
            case 0x00:
                return;

            default:
                CHARLCD1_PutChar( *inputString++ );
                break;
        }
    }
}
/*********************************************************************
 * Function: void CHARLCD1_PutChar(char);
 *
 * Overview: Puts a character on the LCD screen.  Unsupported characters will be
 *           discarded.  May block or throw away characters is LCD is not ready
 *           or buffer space is not available.
 *
 * PreCondition: already initialized via CHARLCD1_Initialize()
 *
 * Input: char - character to print
 *
 * Output: None
 *
 ********************************************************************/
void CHARLCD1_PutChar( char inputCharacter )
{
    static char lastCharacter = 0;
        
    switch(inputCharacter)
    {
        case '\r':
            if(lastCharacter != '\n')
            {
                CarriageReturn();
            }
            break;

        case '\n': 
            if(lastCharacter != '\r')
            {
                CarriageReturn();
            }
            
            if(row == 0)
            {
                ShiftCursorDown();
            }
            else
            {
                ShiftCursorUp();
            }
            break;

        case '\b':
            ShiftCursorLeft();
            CHARLCD1_PutChar( ' ' );
            ShiftCursorLeft();
            break;
            
        case '\f':
            CHARLCD1_ClearScreen();
            break;

        default:
            if(column == MAX_COLUMN)
            {
                column = 0;
                if(row == 0)
                {
                    SendCommand( COMMAND_ROW_1_HOME );
                    row = 1;
                }
                else
                {
                    SendCommand( COMMAND_ROW_0_HOME );
                    row = 0;
                }
            }
            
            SendData( inputCharacter );
            column++;
            break;
    }
    
    lastCharacter = inputCharacter;
}
/*********************************************************************
 * Function: void CHARLCD1_ClearScreen(void);
 *
 * Overview: Clears the screen, if possible.
 *
 * PreCondition: already initialized via CHARLCD1_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
void CHARLCD1_ClearScreen( void )
{
    SendCommand( COMMAND_CLEAR_SCREEN );
    SendCommand( COMMAND_RETURN_HOME );

    row = 0;
    column = 0;
}

/*********************************************************************
 * Function: void CHARLCD1_CursorEnable(bool enable)
 *
 * Overview: Enables/disables the cursor
 *
 * PreCondition: None
 *
 * Input: bool - specifies if the cursor should be on or off
 *
 * Output: None
 *
 ********************************************************************/
void CHARLCD1_CursorEnable( bool enable )
{
    if(enable == true)
    {
        SendCommand( COMMAND_CURSOR_ON );
    }
    else
    {
        SendCommand( COMMAND_CURSOR_OFF );
    }
}

/*******************************************************************/
/*******************************************************************/
/* Private Functions ***********************************************/
/*******************************************************************/
/*******************************************************************/
/*********************************************************************
 * Function: static void CarriageReturn(void)
 *
 * Overview: Handles a carriage return
 *
 * PreCondition: already initialized via CHARLCD1_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void CarriageReturn( void )
{
    if(row == 0)
    {
        SendCommand( COMMAND_ROW_0_HOME );
    }
    else
    {
        SendCommand( COMMAND_ROW_1_HOME );
    }
    column = 0;
}
/*********************************************************************
 * Function: static void ShiftCursorLeft(void)
 *
 * Overview: Shifts cursor left one spot (wrapping if required)
 *
 * PreCondition: already initialized via CHARLCD1_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void ShiftCursorLeft( void )
{
    uint8_t i;

    if(column == 0)
    {
        if(row == 0)
        {
            SendCommand( COMMAND_ROW_1_HOME );
            row = 1;
        }
        else
        {
            SendCommand( COMMAND_ROW_0_HOME );
            row = 0;
        }

        //Now shift to the end of the row
        for(i = 0; i <( MAX_COLUMN - 1 ); i++)
        {
            ShiftCursorRight();
        }
    }
    else
    {
        column--;
        SendCommand( COMMAND_MOVE_CURSOR_LEFT );
    }
}
/*********************************************************************
 * Function: static void ShiftCursorRight(void)
 *
 * Overview: Shifts cursor right one spot (wrapping if required)
 *
 * PreCondition: already initialized via CHARLCD1_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void ShiftCursorRight( void )
{
    SendCommand( COMMAND_MOVE_CURSOR_RIGHT );
    column++;

    if(column == MAX_COLUMN)
    {
        column = 0;
        if(row == 0)
        {
            SendCommand( COMMAND_ROW_1_HOME );
            row = 1;
        }
        else
        {
            SendCommand( COMMAND_ROW_0_HOME );
            row = 0;
        }
    }
}
/*********************************************************************
 * Function: static void ShiftCursorUp(void)
 *
 * Overview: Shifts cursor up one spot (wrapping if required)
 *
 * PreCondition: already initialized via CHARLCD1_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void ShiftCursorUp( void )
{
    uint8_t i;

    for(i = 0; i < MAX_COLUMN; i++)
    {
        ShiftCursorLeft();
    }
}
/*********************************************************************
 * Function: static void ShiftCursorDown(void)
 *
 * Overview: Shifts cursor down one spot (wrapping if required)
 *
 * PreCondition: already initialized via CHARLCD1_Initialize()
 *
 * Input: None
 *
 * Output: None
 *
 ********************************************************************/
static void ShiftCursorDown( void )
{
    uint8_t i;

    for(i = 0; i < MAX_COLUMN; i++)
    {
        ShiftCursorRight();
    }
}

/*********************************************************************
 * Function: static void SendData(char data)
 *
 * Overview: Sends data to LCD
 *
 * PreCondition: None
 *
 * Input: char - contains the data to be sent to the LCD
 *
 * Output: None
 *
 ********************************************************************/
static void SendData( char data )
{
  
    // RW pin is not selected in pin manager 
    CharLCD1_RS_SetHigh();
    
    DataNibbleWrite(data>>4);
    __delay_us(SIGNAL_TIMING_US);
    CharLCD1_E_SetHigh();
    __delay_us(SIGNAL_TIMING_US);
    
    CharLCD1_E_SetLow();
    
    DataNibbleWrite(data);
    __delay_us(SIGNAL_TIMING_US);
    CharLCD1_E_SetHigh();
    __delay_us(SIGNAL_TIMING_US);
    CharLCD1_E_SetLow();
    
    // RS pin is not selected in pin manager
    __delay_us(FAST_INSTRUCTION_TIME_US);
}

/*********************************************************************
 * Function: static void SendCommand(char data)
 *
 * Overview: Sends command to LCD
 *
 * PreCondition: None
 *
 * Input: char - contains the command to be sent to the LCD
 *        unsigned int - has the specific delay for the command
 *
 * Output: None
 *
 ********************************************************************/
static void SendCommand( enum COMMAND command )
{       
    // RW pin is not selected in pin manager 
    CharLCD1_RS_SetLow();
    
    DataNibbleWrite(command >> 4);
    CharLCD1_E_SetHigh();
    __delay_us(SIGNAL_TIMING_US);
    CharLCD1_E_SetLow();
    
    CharLCD1_RS_SetLow();
    DataNibbleWrite(command);
    CharLCD1_E_SetHigh();
    __delay_us(SIGNAL_TIMING_US);
    CharLCD1_E_SetLow();

    switch(command)
    {
        case COMMAND_MOVE_CURSOR_LEFT:
        case COMMAND_MOVE_CURSOR_RIGHT:
        case COMMAND_SET_MODE_8_BIT:
		case COMMAND_SET_MODE_4_BIT:
        case COMMAND_CURSOR_OFF:
            __delay_us(FAST_INSTRUCTION_TIME_US);
            break;
            
        case COMMAND_ENTER_DATA_MODE:
        case COMMAND_CLEAR_SCREEN:
        case COMMAND_RETURN_HOME:
        case COMMAND_CURSOR_ON:
        case COMMAND_ROW_0_HOME:
        case COMMAND_ROW_1_HOME:
        default:
            __delay_us(SLOW_INSTRUCTION_TIME_US);
            break;
    }
    
}

static void DataNibbleWrite(uint8_t value)
{   
    if(value & 0x01)
    {
        CharLCD1_DB4_SetHigh();
    }
    else
    {
        CharLCD1_DB4_SetLow();
    }
    
    
    if(value & 0x02)
    {
        CharLCD1_DB5_SetHigh();
    }
    else
    {
        CharLCD1_DB5_SetLow();
    }
    
  
    if(value & 0x04)
    {
        CharLCD1_DB6_SetHigh();
    }
    else
    {   
        CharLCD1_DB6_SetLow();
    }
    

    if(value & 0x08)
    {
        CharLCD1_DB7_SetHigh();
    }
    else
    {
        CharLCD1_DB7_SetLow();
        
    }
}

