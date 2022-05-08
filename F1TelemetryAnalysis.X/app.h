/**
  User Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This is the user application header file which has application specific functions.

  Description:
    This is the user application header file which has functions necessary to execute the user application.

*/

/*
Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*/

#ifndef _APP_H
#define _APP_H

#ifdef	__cplusplus
extern "C" {
#endif

#define VDD (33000)                                    // Supply Voltage of 3.3 V is represented as 33000*10^-4 V
                                                       // to get the required ADC_RESOLUTION
#define ADC_STEPS (1024)                               // 10 bit ADC has 1024 Levels
#define ADC_RESOLUTION (VDD/ADC_STEPS)                 // Calculate the smallest resolution of the ADC

/*User defined variables*/
extern uint16_t adones, adtenth, adhundredth;
extern uint16_t adc_volt_value;

/*
Function:
void ConvertADCVoltage ( unsigned int adc_conv_data )

Description:
Converts digital voltage to analog voltage
*/

void ConvertADCVoltage (unsigned int adc_conv_data);   //function to calculate the analog value

/*
Function:
void Write2UART(char *buffer, unsigned int size);

Description:
Writes the buffer to UART
*/

void Write2UART(char *buffer, unsigned int size);

#ifdef	__cplusplus
}
#endif

#endif //ifndef _APP_H

