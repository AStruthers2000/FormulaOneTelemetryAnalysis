/**
  User Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This is the user application source file which has application specific functions.

  Description:
    This is the user application source file which has functions necessary to execute the user application.

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

/*Include files*/
#include "mcc_generated_files/mcc.h"
#include "app.h"

/*User defined variables*/
uint16_t adones, adtenth, adhundredth;
uint16_t adcVoltValue;

/***************************User Functions*************************************/

void ConvertADCVoltage ( unsigned int adc_conv_data )
{
    adcVoltValue = (ADC_RESOLUTION*adc_conv_data);  //3.225 milli volts one step per division

    adones = adcVoltValue/10000;        // units place

    adtenth = adcVoltValue/1000;
    adtenth = adtenth%10;           // one tenth decimal place

    adhundredth = adcVoltValue/100;
    adhundredth = adhundredth%10;         // one hundredth decimal place
}

void Write2UART(char *buffer, unsigned int size)
{
    while(size)
    {
        UART2_Write(*buffer);
        while(!(UART2_StatusGet() & UART2_TX_COMPLETE))
        {

        }
        buffer++;
        size--;
    }
    while(!(UART2_StatusGet() & UART2_TX_COMPLETE))
    {

    }
}

int GetPacketSize(int header){
    switch(header){
        case 2:
            return 43;
        case 6:
            return 60;
        default:
            return 0;
    }
}

char bufferWrite[50];
int dataRecvCount = 0;
int headerPacket = 1;
int headerType = 0;
int neededPackets = 0;
int data[100];

void ReadUART(){
    
    uint8_t recv = UART2_Read();
    //while(recv != 10){
    //    bufferRead[index] = recv;
        //bufferRead++;
    //    index++;
    //}
    
    if(headerPacket){
        headerType = recv;
        headerPacket = 0;
        neededPackets = GetPacketSize(headerType);
        data = data[neededPackets];
    }else{
        data[dataRecvCount] = recv;
        dataRecvCount++;
        if(dataRecvCount == neededPackets){
            dataRecvCount = 0;
            headerPacket = 1;
        }
    }
    
    
    
    //printf("%d", recv);
    
    //sprintf(bufferWrite, "%d\n", recv);
    //Write2UART(bufferWrite, strlen(bufferWrite));   //Analog voltage value is transmitted over UART

    IO_RA2_Toggle();
}



