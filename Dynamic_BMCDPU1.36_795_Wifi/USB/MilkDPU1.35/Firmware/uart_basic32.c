/*********************************************************************
 *
 *      PIC32MX UART Library Interface Example
 *
 *********************************************************************
 * FileName:        uart_basic.c
 * Dependencies:    plib.h
 *
 * Processor:       PIC32MX
 *
 * Complier:        MPLAB C32
 *                  MPLAB IDE
 * Company:         Microchip Technology Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ?Company?) for its PIC32MX Microcontroller is intended
 * and supplied to you, the Company?s customer, for use solely and
 * exclusively on Microchip Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ?AS IS? CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *********************************************************************
 * The purpose of this example code is to demonstrate the PIC32MX
 * peripheral library macros and functions supporting the UART
 * module and its various features.
 *
 * Platform: Explorer-16 with PIC32MX PIM
 *
 * Features demonstrated:
 *    - UART perepheral library usage
 *
 * Description:
 *      This program uses the UART library to communicate through
 *      the serial port on the Explorer-16. With an RS232 cable
 *      attatched to the Explorer-16 and a pc, the program will
 *      output text and recieve input using a terminal program.
 *
 * Notes:
 *    - PIC32MX 2xx PIMS are unconnected to the Explorer-16 DB9
 *      connector. It must be soldered to the test points on top of
 *      the PIM for proper functionality. The README file contains
 *      a list of the connections that need to be made.
 ********************************************************************/

	/*  BAUD RATE:1200 - 8BIT - EVEN PARITY for ULTRA ANALYZER 		*/
	/*  BAUD RATE:2400 - 7BIT - EVEN PARITY for ULTRA PRO ANALYZER 	*/


#include <plib.h>
#include<string.h>

#define	GetPeripheralClock()		(SYS_FREQ/(1 << OSCCONbits.PBDIV))
#define	GetInstructionClock()		(SYS_FREQ)

#define UART_ANALYZER       UART2
#define UART_ANALYZER2      UART4
#define UART_WM             UART5
#define UART_PRINTER        UART6   //6
#define UART_GPRS           UART3   //3

#define ANALYZER_OVERFLOW   U2STAbits.OERR
#define ANALYZER2_OVERFLOW  U4STAbits.OERR
#define WM_OVERFLOW         U5STAbits.OERR
#define GPRS_OVERFLOW       U3STAbits.OERR//3
#define PRINTER_OVERFLOW    U6STAbits.OERR//6
#define Analyzer_VECTOR     _UART_2_VECTOR  //32   //
#define Analyzer2_VECTOR    _UART_4_VECTOR  //49  //
#define WM_VECTOR           _UART_5_VECTOR  //51  //
#define GPRS_VECTOR         _UART_3_VECTOR  //51  //


void Init_UART_Interrupts_WM(void)
{
   INTEnable(INT_SOURCE_UART_RX(UART_WM), INT_ENABLED);
   INTSetVectorPriority(INT_VECTOR_UART(UART_WM), INT_PRIORITY_LEVEL_4);
   INTSetVectorSubPriority(INT_VECTOR_UART(UART_WM), INT_SUB_PRIORITY_LEVEL_0);
   INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
}

void Init_UART_Interrupts_A(void)
{
   INTEnable(INT_SOURCE_UART_RX(UART_ANALYZER), INT_ENABLED);
   INTSetVectorPriority(INT_VECTOR_UART(UART_ANALYZER), INT_PRIORITY_LEVEL_2);
   INTSetVectorSubPriority(INT_VECTOR_UART(UART_ANALYZER), INT_SUB_PRIORITY_LEVEL_0);

   INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
}

void Init_UART_Interrupts_A2(void)
{
   INTEnable(INT_SOURCE_UART_RX(UART_ANALYZER2), INT_ENABLED);
   INTSetVectorPriority(INT_VECTOR_UART(UART_ANALYZER2), INT_PRIORITY_LEVEL_3);
   INTSetVectorSubPriority(INT_VECTOR_UART(UART_ANALYZER2), INT_SUB_PRIORITY_LEVEL_0);

   INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
}

void Init_Analyzer(UINT32 abaud, BYTE anoofbits)
{
    UARTConfigure(UART_ANALYZER, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART_ANALYZER,  UART_INTERRUPT_ON_RX_NOT_EMPTY);
    
    if(anoofbits==9)
        UARTSetLineControl(UART_ANALYZER, UART_DATA_SIZE_8_BITS | UART_PARITY_EVEN | UART_STOP_BITS_1);
    else
        UARTSetLineControl(UART_ANALYZER, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
  
    UARTSetDataRate(UART_ANALYZER, GetPeripheralClock(), abaud);
    UARTEnable(UART_ANALYZER, UART_ENABLE_FLAGS(UART_PERIPHERAL |  UART_TX |  UART_RX));

    ANALYZER_OVERFLOW=0;
    Init_UART_Interrupts_A();
    I_Index_Count=0;
    BYTEGAPCOUNT=0;
    DPS_FLAG=0;
    if(Settingsrec.ANALYZER=='F')
    {
        A1DELAYCOUNT=2;
        READ_FAT_VALUE1=0;
    }
    if(abaud==1200)
        A1DELAYCOUNT=2;
}

void Init_Analyzer2(UINT32 a2baud, BYTE a2noofbits)
{
    UARTConfigure(UART_ANALYZER2,  UART_INVERT_RECEIVE_POLARITY | UART_ENABLE_PINS_TX_RX_ONLY);//
    UARTSetFifoMode(UART_ANALYZER2, UART_INTERRUPT_ON_RX_NOT_EMPTY);

    if(a2noofbits==9)
        UARTSetLineControl(UART_ANALYZER2, UART_DATA_SIZE_8_BITS | UART_PARITY_EVEN | UART_STOP_BITS_1);
    else
        UARTSetLineControl(UART_ANALYZER2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);

    UARTSetDataRate(UART_ANALYZER2, GetPeripheralClock(), a2baud);
    UARTEnable(UART_ANALYZER2, UART_ENABLE_FLAGS(UART_PERIPHERAL |  UART_RX));

    U4STAbits.UTXEN=0;
    
    ANALYZER2_OVERFLOW=0;    
    Init_UART_Interrupts_A2();
    I_Index_Count2=0;
    BYTEGAPCOUNT2=0;
    DPS_FLAG2=0;
    
    if(Settingsrec.ANALYZER=='F')
    {
        A2DELAYCOUNT=2;
        READ_FAT_VALUE2=0;
    }
    
    if(a2baud==1200)
        A2DELAYCOUNT=2;
}

void Set_Update_Analyzer_Flags(UINT8 sanaport,UINT8 sdatalen)
{
    if(sanaport==1)
    {
        ANALYZER_FLAG=1;            
        Update_Analyzer_Flag=1;
        A1DELAYCOUNT=0;
        A_No_Bytes=sdatalen;
    }
    else if(sanaport==2)
    {
        ANALYZER_FLAG2=1;            
        Update_Analyzer_Flag2=1;
        A2DELAYCOUNT=0;
        A_No_Bytes2=sdatalen;
    }
}

void Reset_Analyzer_Flags(UINT8 ranaport)
{
    if(ranaport==1)
    {
        ANALYZER_FLAG=0;
        DPS_FLAG=0;
    }
    
    else if(ranaport==2)
    {
        ANALYZER_FLAG2=0;
        DPS_FLAG2=0;
    }
}

UINT8 ULTRA_PRO_Analyzer_Data(UINT8 anaport,UINT8 anadatabyte,char* anadatabuf,UINT8 anaindexcnt)
{
    anadatabyte=anadatabyte & 0x7F;
    
    if(anadatabyte=='(')
    {
        ANALYZER_FLAG=0;
        anaindexcnt=1;
        anadatabuf[0]=anadatabyte;

    }
    else if(anadatabyte!=0xFF)
    {
        //if(I_Index_Count>35)
        if(anaindexcnt>35)
        {
            anaindexcnt=0;
            Reset_Analyzer_Flags(anaport);
            /*if(anaport==1)
                ANALYZER_FLAG=0;
            else if(anaport==2)
                ANALYZER_FLAG2=0;*/
        }

        //anadatabuf[I_Index_Count]=anadatabyte;
        anadatabuf[anaindexcnt]=anadatabyte;

        if(anadatabuf[0]=='(' && anadatabyte==')' && anaindexcnt==30)//I_Index_Count==30)
        {
            Set_Update_Analyzer_Flags(anaport,32);
            /*if(anaport==1)
            {
                ANALYZER_FLAG=1;            
                Update_Analyzer_Flag =1;
                A1DELAYCOUNT=0;
                A_No_Bytes=32;
            }
            else if(anaport==2)
            {
                ANALYZER_FLAG2=1;            
                Update_Analyzer_Flag2 =1;
                A2DELAYCOUNT=0;
                A_No_Bytes2=32;
            }*/
           
           anaindexcnt=0;           
        }

        anaindexcnt++;//I_Index_Count++;
    }
    return anaindexcnt;
}

UINT8 ULTRA_Analyzer_Data(UINT8 anaport,UINT8 anadatabyte,char* anadatabuf,UINT8 anaindexcnt)
{
    anadatabuf[anaindexcnt]=anadatabyte;

    if(anaindexcnt==24)
    {
        /*if(anaport==1)
        {
            ANALYZER_FLAG=1;        
            Update_Analyzer_Flag =1;
            A1DELAYCOUNT=0;
            A_No_Bytes=24;
        }
        if(anaport==2)
        {
            ANALYZER_FLAG2=1;        
            Update_Analyzer_Flag2 =1;
            A2DELAYCOUNT=0;
            A_No_Bytes2=24;
        }*/
        Set_Update_Analyzer_Flags(anaport,24);
       
       anaindexcnt=0;
    }
    else if(anaindexcnt>24)
    {
        anaindexcnt=0;
        Reset_Analyzer_Flags(anaport);
        /*if(anaport==1)
            ANALYZER_FLAG=0;
        if(anaport==2)
            ANALYZER_FLAG2=0;
       // A1DELAYCOUNT=0;*/

    }
    else
    {
        anaindexcnt++;
    }
    
    return anaindexcnt;
}

UINT8 LACTO_ESSAE_Analyzer_Data(UINT8 anaport,UINT8 anadatabyte,char* anadatabuf,UINT8 anaindexcnt)
{

    if(anadatabyte=='F' || anadatabyte=='#')
    {
        //ANALYZER_FLAG=0;
        Reset_Analyzer_Flags(anaport);
        
        anaindexcnt=1;
        anadatabuf[0]=anadatabyte;

    }
    else if(anadatabyte!=0xFF && (anadatabuf[0]=='F' || anadatabuf[0]=='#'))
    {
        anadatabuf[anaindexcnt]=anadatabyte;

        if(anaindexcnt >10 && anadatabuf[anaindexcnt-1]==0x0D && anadatabyte==0x0A)
        {
           /* ANALYZER_FLAG=1;
            A_No_Bytes=I_Index_Count;            
            Update_Analyzer_Flag =1;
            A1DELAYCOUNT=0;*/
            Set_Update_Analyzer_Flags(anaport,anaindexcnt);
            anaindexcnt=0;
        }
        else
            anaindexcnt++;
    }
    return anaindexcnt;
}

UINT8 KSHEERA_Analyzer_Data(UINT8 anaport,UINT8 anadatabyte,char* anadatabuf,UINT8 anaindexcnt)
{
    if(anadatabyte=='$')
    {
        //ANALYZER_FLAG=0;
        Reset_Analyzer_Flags(anaport);        
        
        anadatabuf[0]=anadatabyte;
        anaindexcnt=1;

    }
    else if(anadatabyte!=0xFF && anadatabuf[0]=='$')
    {
        anadatabuf[anaindexcnt]=anadatabyte;

        if(anadatabuf[0]=='$' && anadatabuf[anaindexcnt-1]==0x0D && anadatabyte==0x0A)
        {
            /*ANALYZER_FLAG=1;
            A_No_Bytes=anaindexcnt;           
            Update_Analyzer_Flag =1;
            A1DELAYCOUNT=0;*/
            Set_Update_Analyzer_Flags(anaport,anaindexcnt);
            
            anaindexcnt=0;
        }
        else if(anadatabuf[0]=='$' && anaindexcnt>65)
        {
            /*ANALYZER_FLAG=1;
            A_No_Bytes=anaindexcnt;            
            Update_Analyzer_Flag =1;
            A1DELAYCOUNT=0;*/
            Set_Update_Analyzer_Flags(anaport,anaindexcnt);
           
           anaindexcnt=0;
        }
        else if(anaindexcnt>76)
        {
            anaindexcnt=0;
        }
        anaindexcnt++;
    }
    else
        anaindexcnt=0;
    
    return anaindexcnt;
}

UINT8 MILK_O_METER_Analyzer_Data(UINT8 anaport,UINT8 anadatabyte,char* anadatabuf,UINT8 anaindexcnt)
{
    if(anadatabyte==0x0A && anaindexcnt==0)
    {
        //ANALYZER_FLAG=0;
        Reset_Analyzer_Flags(anaport);
        
        anadatabuf[0]=anadatabyte;
        anaindexcnt=1;

    }
    else if(anadatabyte==0x0D && anaindexcnt==1 && anadatabuf[0]==0x0A)
    {
        anadatabuf[1]=anadatabyte;
        anaindexcnt=2;
    }
    else if( anadatabyte !=0xFF && anaindexcnt<147 && anaindexcnt>=2)
    {
        anadatabuf[anaindexcnt]=anadatabyte;
        anaindexcnt++;
    }
    else
    {
        anaindexcnt=0;
    }
    if(anaindexcnt==147)
    {
        /*ANALYZER_FLAG=1;
        A_No_Bytes=anaindexcnt;        
        Update_Analyzer_Flag =1;
        A1DELAYCOUNT=0;*/
        Set_Update_Analyzer_Flags(anaport,anaindexcnt);
        
        anaindexcnt=0;
    }
    
    return anaindexcnt;
}

UINT8 MILK_TESTER_FATOMATIC_Analyzer_Data(UINT8 anaport,UINT8 anadatabyte,char* anadatabuf,UINT8 anaindexcnt)
{
    UINT8 read_fat_value;

    if (anaindexcnt==30)
        anaindexcnt = 0;
    
    if (anadatabyte == '?' || anadatabyte=='%')
    {
        anaindexcnt = 0;   
        read_fat_value=0;
    }

    else if((anadatabyte >= '0' && anadatabyte <= '9') || anadatabyte == '.' || anadatabyte == 0x0D)
    {
        anadatabuf[anaindexcnt] = anadatabyte;
        anaindexcnt++;
    }
    else if (anadatabyte == 0x0A)
    {
        if(anadatabuf[anaindexcnt-1]==0x0D)
        {
            anadatabuf[anaindexcnt-1] = 0x00;                        

            if (anadatabuf[anaindexcnt-4] == '.')
                anadatabuf[anaindexcnt-2] = 0x00;

            if(Settingsrec.ANALYZER=='F')
            {                        
                if(anadatabuf[0]=='0' && anadatabuf[1]=='0' && anadatabuf[2]=='0' && anadatabuf[3]=='0')
                {
                    Reset_Analyzer_Flags(anaport);
                    anaindexcnt = 0;
                    //ANALYZER_FLAG2 = 0;
                    //READ_FAT_VALUE2=1;
                    read_fat_value=1;
                }
                else if(read_fat_value==1)//(READ_FAT_VALUE2==1) 
                {                                
                    anadatabuf[4]=0x00;
                    anadatabuf[3]=anadatabuf[2];
                    anadatabuf[2]='.';

                    Set_Update_Analyzer_Flags(anaport,anaindexcnt);
                    READ_FAT_VALUE2=0;
                    anaindexcnt = 0;
                }
            }
            else
            {
                Set_Update_Analyzer_Flags(anaport,anaindexcnt);
                
                anaindexcnt = 0;
            }                                 
        }
    }
    return anaindexcnt;
}

void __ISR(Analyzer_VECTOR) IntAnalyzerHandler(void)//_UART_2_VECTOR//, ipl2
{
    if(INTGetFlag(INT_SOURCE_UART_RX(UART_ANALYZER)))
    {
        DATABYTE=UARTGetDataByte(UART_ANALYZER);

        if(A1DELAYCOUNT>=2)
        {
            if(Settingsrec.ANALYZER=='P' && CollectionFlag==1 )//Analyzer_Type
            {                
                I_Index_Count=ULTRA_PRO_Analyzer_Data(1,DATABYTE,REC_BUF,I_Index_Count);
            }

            else if(Settingsrec.ANALYZER=='U' && CollectionFlag==1)
            {
                if(BYTEGAPCOUNT<2 || I_Index_Count==0)
                {
                    BYTEGAPCOUNT=0;
                    I_Index_Count=ULTRA_Analyzer_Data(1,DATABYTE,REC_BUF,I_Index_Count);                  
                }
                else
                {
                    
                    I_Index_Count=0;
                }
                    
            }
           
            else if((Settingsrec.ANALYZER=='L' || Settingsrec.ANALYZER=='E') && CollectionFlag==1)
            {
                 I_Index_Count=LACTO_ESSAE_Analyzer_Data(1,DATABYTE,REC_BUF,I_Index_Count);              
            }    
           
            else if(Settingsrec.ANALYZER=='K' && CollectionFlag==1)
            {
                I_Index_Count=KSHEERA_Analyzer_Data(1,DATABYTE,REC_BUF,I_Index_Count);
               
            }

            else if(Settingsrec.ANALYZER=='A' && CollectionFlag==1) //Milk-O-Meter
            {
                I_Index_Count=MILK_O_METER_Analyzer_Data(1,DATABYTE,REC_BUF,I_Index_Count);                
            }

            else if((Settingsrec.ANALYZER=='M' || Settingsrec.ANALYZER=='F') && CollectionFlag==1)
            {
                I_Index_Count=MILK_TESTER_FATOMATIC_Analyzer_Data(1,DATABYTE,REC_BUF,I_Index_Count);              
            }
        } 
        else if(DATABYTE!=0xFF && DATABYTE!=0x00)
        {
            A1DELAYCOUNT=0;
        }

        INTClearFlag(INT_SOURCE_UART_RX(UART_ANALYZER));
    }
    
    // We don't care about TX interrupt
    if (INTGetFlag(INT_SOURCE_UART_TX(UART_ANALYZER)))
    {
        INTClearFlag(INT_SOURCE_UART_TX(UART_ANALYZER));
    }
    
}

void __ISR(Analyzer2_VECTOR) IntAnalyzer2Handler(void)//_UART_4_VECTOR//, ipl3
{
    if(INTGetFlag(INT_SOURCE_UART_RX(UART_ANALYZER2)))
    {
        DATABYTE2=UARTGetDataByte(UART_ANALYZER2) ;

        if(A2DELAYCOUNT>=2)
        { 
            if(Settingsrec.ANALYZER=='P' && CollectionFlag==1)
            {
                I_Index_Count2=ULTRA_PRO_Analyzer_Data(2,DATABYTE2,REC_BUF2,I_Index_Count2);

            }

            else if(Settingsrec.ANALYZER=='U' && CollectionFlag==1)
            {
                if(BYTEGAPCOUNT2<2 || I_Index_Count2==0)
                {
                    
                    BYTEGAPCOUNT2=0;
                    I_Index_Count2=ULTRA_Analyzer_Data(2,DATABYTE2,REC_BUF2,I_Index_Count2);                   
                }
                else
                {
                    
                    I_Index_Count2=0;
                }
            }
            
            else if((Settingsrec.ANALYZER=='L' || Settingsrec.ANALYZER=='E') && CollectionFlag==1)
            {
                I_Index_Count2=LACTO_ESSAE_Analyzer_Data(2,DATABYTE2,REC_BUF2,I_Index_Count2);
             
            }

            else if(Settingsrec.ANALYZER=='K' && CollectionFlag==1)
            {
                I_Index_Count2=KSHEERA_Analyzer_Data(2,DATABYTE2,REC_BUF2,I_Index_Count2);                
            }

            else if(Settingsrec.ANALYZER=='A' && CollectionFlag==1)
            {
                I_Index_Count2=MILK_O_METER_Analyzer_Data(2,DATABYTE2,REC_BUF2,I_Index_Count2);
                
            }

            else if((Settingsrec.ANALYZER=='M' || Settingsrec.ANALYZER=='F') && CollectionFlag==1)
            {
                I_Index_Count2=MILK_TESTER_FATOMATIC_Analyzer_Data(2,DATABYTE2,REC_BUF2,I_Index_Count2);
             
            }
        }
        else if(DATABYTE2!=0xFF && DATABYTE2!=0x00)
            A2DELAYCOUNT=0;

        
        INTClearFlag(INT_SOURCE_UART_RX(UART_ANALYZER2));

    }
    
    // We don't care about TX interrupt
    if (INTGetFlag(INT_SOURCE_UART_TX(UART_ANALYZER2)))
    {
        INTClearFlag(INT_SOURCE_UART_TX(UART_ANALYZER2));
    }
    
}

void __ISR(WM_VECTOR) IntWMHandler(void)//_UART_5_VECTOR//, ipl4
{
	// Is this an RX interrupt?

    if(INTGetFlag(INT_SOURCE_UART_RX(UART_WM)))
    {
        if(WMCount>20)
            WMCount=0;

        WM_DATABYTE=UARTGetDataByte(UART_WM);

        if((WM_DATABYTE >=0x30 && WM_DATABYTE <=0x39) || WM_DATABYTE=='.' )
        {
            WMBuf[WMCount]=WM_DATABYTE;
            if(WM_DATABYTE=='.')
                 WM_Dot_Pos=WMCount;
            WMCount++;

        }
        
        else if(WM_DATABYTE=='-')
        {
            Wt_Sign=1;
            WMCount=0;
            WM_FLAG=1;
            WEIGHT_UNIT=0;
            Weight=0; 
            
        }
        else if(WM_DATABYTE =='L' || WM_DATABYTE =='l')
            WEIGHT_UNIT='L';
        
        else if(WM_DATABYTE =='K' || WM_DATABYTE =='k' || WM_DATABYTE =='c' || WM_DATABYTE =='C')
            WEIGHT_UNIT='K';
        
        else if(WMCount>=WM_MIN_DIG && Wt_Sign==0 && (((WEIGHT_UNIT!='L' || WEIGHT_UNIT=='B') && WEIGHT_KG=='Y')||((WEIGHT_UNIT=='L' || WEIGHT_UNIT=='B') && WEIGHT_KG!='Y')))//RxChar==0x0D &&
        {
            if(WM_Dot_Pos>0)
            {
                if(WMrec.WM_ACCURACY==1 || WMCount<=(WM_Dot_Pos+2))
                    WMBuf[WM_Dot_Pos+2]='0';
                WMBuf[WM_Dot_Pos+3]='\0';
            }
            else
            {
                if(WMrec.WM_DECIMAL_POINT==3)
                    WMCount--;
                else if(WMrec.WM_DECIMAL_POINT==1)
                {
                    WMBuf[WMCount]='0';
                    WMCount++;
                }
                else if(WMrec.WM_DECIMAL_POINT==0)
                {
                    WMBuf[WMCount]='0';
                    WMCount++;
                    WMBuf[WMCount]='0';
                    WMCount++;
                }
                if(WMrec.WM_ACCURACY==1)
                    WMBuf[WMCount-1]='0';
                WMBuf[WMCount]='\0';
            }

            TEMP_LONG=Float_Long(WMBuf);
            if(TEMP_LONG<0xFFFF)
                Weight=TEMP_LONG;
            else
                Weight=0xFFFF;

            WM_FLAG=1;
            WMCount=0;
            Wt_Sign=0;
            WM_Dot_Pos=0;
            WEIGHT_UNIT='B';

        }
        else if(WM_DATABYTE==' ' || WM_DATABYTE=='+' || WM_DATABYTE=='t')// || WM_DATABYTE=='$')
        {
        }
        
        else if(WMCount>2)
        {
            Wt_Sign=0;
            WMCount=0;          
            WM_Dot_Pos=0;
            WEIGHT_UNIT='B';
        }
        INTClearFlag(INT_SOURCE_UART_RX(UART_WM));
    }

    // We don't care about TX interrupt
    if (INTGetFlag(INT_SOURCE_UART_TX(UART_WM)))
    {
        INTClearFlag(INT_SOURCE_UART_TX(UART_WM));
    }
}

void SP_SendString(char *sbuf,UINT8 slen)
{
    UINT8 sp_index=0;

    ANALYZER_OVERFLOW=0;

    while(sp_index<slen) 
    {
         while(!UARTTransmitterIsReady(UART_ANALYZER));

        UARTSendDataByte(UART_ANALYZER, sbuf[sp_index]);

        sp_index++;
    }
    while(!UARTTransmissionHasCompleted(UART_ANALYZER));
}

void Disable_UART_Interrupts_WM(void)
{
   INTEnable(INT_SOURCE_UART_RX(UART_WM), INT_DISABLED);
}

void Init_Weighing_Machine(void)
{
    if(WMrec.WM_BAUDRATE==0)
        WMrec.WM_BAUDRATE=2400;

    UARTConfigure(UART_WM, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART_WM, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_WM, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1 );
    UARTSetDataRate(UART_WM, GetPeripheralClock(), WMrec.WM_BAUDRATE);
    UARTEnable(UART_WM, UART_ENABLE_FLAGS(UART_PERIPHERAL |  UART_TX |  UART_RX ));
    
    Init_UART_Interrupts_WM();
    WM_OVERFLOW=0;
    WM_Dot_Pos=0;
    SEERAMReadString(WM_Min_Dig_Loc,1,(char *)&WM_MIN_DIG);
    if(WM_MIN_DIG<2 || WM_MIN_DIG>4 )
        WM_MIN_DIG=4;

}

void WM_SendByte(UINT8 wmsendbyte)
{
    WM_OVERFLOW=0;
    while(!UARTTransmitterIsReady(UART_WM))
            ;
    UARTSendDataByte(UART_WM,wmsendbyte);

    while(!UARTTransmissionHasCompleted(UART_WM))
        ;
}
