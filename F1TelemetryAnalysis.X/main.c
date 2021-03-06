/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  PIC24FJ1024GB610
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
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

/**
  Section: Included Files
*/
#include <string.h>
#include <stdio.h>
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "app.h"  
/*
    Main application
 */
int adcResult = 0;
//char bufferWrite[50];
int shouldWrite = 0;
    
void SendResult()
{
    //IO_RA1_Toggle();
    shouldWrite = 1;
    return;
}

int main(void)
{

    // initialize the device
    SYSTEM_Initialize();
    /*Clear screen and set cursor to the beginning of the current line*/
    /*
    sprintf(bufferWrite, "\033[2J");
    Write2UART(bufferWrite, strlen(bufferWrite));
    sprintf(bufferWrite, "\033[0;0H");
    Write2UART(bufferWrite, strlen(bufferWrite));
    sprintf(bufferWrite, "MCC: It's Easy!\r\nLab 1\r\n\r\n");
    Write2UART(bufferWrite, strlen(bufferWrite));
     */
    /*Start the timer with Period 1s */
    TMR1_Start();
    /*Select the ADC channel to which Potentiometer is connected*/

    while(1)
    {
        /*
        if(TMR1_GetElapsedThenClear() && shouldWrite)
        {
            IO_RA6_Toggle();

            sprintf(bufferWrite, "Hello from the MCC\n");
            Write2UART(bufferWrite, strlen(bufferWrite));   //Analog voltage value is transmitted over UART
            shouldWrite = 0;
        }*/
        ReadUART();
    }
    return -1;
}
/**
 End of File
*/

