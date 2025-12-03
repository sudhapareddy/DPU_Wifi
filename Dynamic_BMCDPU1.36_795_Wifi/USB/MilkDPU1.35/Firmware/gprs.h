void GPRS_Send_Interrupt_Data(UINT16 sendtype,BYTE* intbuf)
{
     UINT16 tempcount=0,gnobytes;
     
     gnobytes=sendtype;
     
    if(ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y' )
    {
        GPRS_OVERFLOW=0;
        GPRSDELAYCOUNT=0;
        GPRS_BYTE_COUNT=0;
        GPRS_FLAG=0;
        TimerCount=0;
        No_Response=0;
        
        if(Modem_Init_Flag==0xFF)
            Modem_Init_Flag=0;
        
        while(1)
        {
            while(!UARTTransmitterIsReady(UART_GPRS) && No_Response==0);

            UARTSendDataByte(UART_GPRS, *intbuf);
            if(gnobytes==0)
            {
                #ifdef WIFI
                
                    if(*intbuf==0x0A || *intbuf==0x00 ||tempcount==0x2000)
                    break;
               
                #else 
               
                    if(*intbuf==0x0D || *intbuf==0x00 ||tempcount==0x2000)
                    break;
               
                #endif
                tempcount++;
            }
            else if(sendtype>0)
            {
                gnobytes=gnobytes-1;
                if(gnobytes==0x00)
                   break;
            }

            intbuf++;
            
        }
        while(!UARTTransmissionHasCompleted(UART_GPRS) && No_Response==0);
    }
}
void GPRS_SendString(BYTE* gprsbuf)
{
    GPRS_Send_Interrupt_Data(0,gprsbuf);
   
}

void GPRS_Send_HexData(BYTE* gprsbuf,UINT16 gnobytes)
{
    GPRS_Send_Interrupt_Data(gnobytes,gprsbuf);
   
}

void GPRS_Interrupt_Receive_Data(UINT8 comtype,char *intbuf)
{
    if(ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y' )
    {
        Expected_Response=comtype;
        No_Response=0;

        GPRS_FLAG=0;
        TimerCount=0;
       while(GPRS_FLAG==0 && No_Response==0)
           ;
       if(No_Response==1)
       {
           No_Response=1;
           MODEM_STATUS=0;
       }
       else
       {
           if(comtype=='O')
               STRCPY(REC_BUF,GPRSOK_BUF);
           else if(comtype=='C')
               STRCPY(intbuf,GPRS_BUF);
           MODEM_STATUS=1;
       }
    }
}
void GPRS_RecString_OK(void)
{
    GPRS_Interrupt_Receive_Data('O',REC_BUF);
   
}

void GPRS_RecString_Command_Data(char* gprsbuf)
{
    if(ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y' )
    {
        Expected_Response='S';
        Max_Timer_Count=200;
        No_Response=0;
        TimerCount=0;

        while(GPRS_FLAG==0 && No_Response==0)
            ;
        if(No_Response==0 && GPRS_FLAG==1)
        {
            STRCPY(gprsbuf,GPRSOK_BUF);

        }
    }    
}

void GPRS_RecString_Data(char* gprsbuf)
{
    if(ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y' )
    {
        Expected_Response='D';
        Max_Timer_Count=100;
        No_Response=0;
        TimerCount=0;

        while(GPRS_FLAG==0 && No_Response==0)
            ;
        if(No_Response==0 && GPRS_FLAG==1)
        {
            STRCPY(gprsbuf,GPRS_BUF);

        }
    }    
}

UINT8 GPRS_RecString_Cmd(char* gprsbuf)
{
    GPRS_Interrupt_Receive_Data('C',gprsbuf);   
    
}

void GPRS_SendByte(BYTE gprsbyte)
{
    if(ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y' )
    {
        while(!UARTTransmitterIsReady(UART_GPRS));
        UARTSendDataByte(UART_GPRS,gprsbyte);

        while(!UARTTransmissionHasCompleted(UART_GPRS));
    }
}

void Ena_GPRS_Interrupt(void)
{
   INTEnable(INT_SOURCE_UART_RX(UART_GPRS), INT_ENABLED);
   INTSetVectorPriority(INT_VECTOR_UART(UART_GPRS), INT_PRIORITY_LEVEL_1);
   INTSetVectorSubPriority(INT_VECTOR_UART(UART_GPRS), INT_SUB_PRIORITY_LEVEL_1);
}

void Init_GPRS(void)
{
    GPRS_OVERFLOW=0;
    UARTConfigure(UART_GPRS, UART_ENABLE_PINS_TX_RX_ONLY);
   // UARTConfigure(UART_GPRS,UART_INVERT_RECEIVE_POLARITY | UART_INVERT_TRANSMIT_POLARITY |  UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART_GPRS, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_GPRS, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    
    UARTSetDataRate(UART_GPRS, GetPeripheralClock(), 115200);
    UARTEnable(UART_GPRS, UART_ENABLE_FLAGS(UART_PERIPHERAL |  UART_TX |  UART_RX));
    
    Ena_GPRS_Interrupt();
}

void Dis_GPRS_Interrupt(void)
{
    INTEnable(INT_SOURCE_UART_RX(UART_GPRS), INT_DISABLED);
}

void __ISR(GPRS_VECTOR) GPRSHandler(void)//_UART_2_VECTOR//, ipl6
{
    if(INTGetFlag(INT_SOURCE_UART_RX(UART_GPRS)))
    {
         GPRSDATABYTE=UARTGetDataByte(UART_GPRS);
         if(Expected_Response=='S' && GPRSDATABYTE!=0)
         {
             
             if(GPRSDATABYTE==0x0D && GPRS_BYTE_COUNT<=2)
             {
                 GPRSOK_BUF[0]=GPRSDATABYTE;
                 GPRS_BYTE_COUNT=1;
             }
             else if(GPRSDATABYTE==0x0A && GPRS_BYTE_COUNT==1)
             {
                 GPRSOK_BUF[1]=GPRSDATABYTE;
                 GPRS_BYTE_COUNT=2;
             }
             else if(GPRSOK_BUF[0]==0x0D && GPRSOK_BUF[1]==0x0A)
             {             

                GPRSOK_BUF[GPRS_BYTE_COUNT]=GPRSDATABYTE;
                if(GPRS_BYTE_COUNT>=2  && GPRSOK_BUF[GPRS_BYTE_COUNT]==0x0A && GPRSOK_BUF[GPRS_BYTE_COUNT-1]==0x0D)//&& GPRSOK_BUF[GPRS_BYTE_COUNT-5]==0x0D && GPRSOK_BUF[GPRS_BYTE_COUNT-4]==0x0A
                {
                    if(GPRSOK_BUF[GPRS_BYTE_COUNT-3]=='O' && GPRSOK_BUF[GPRS_BYTE_COUNT-2]=='K')
                    {
                        OK_ERROR='O';
                        GPRS_FLAG=1;
                        GPRSOK_BUF[GPRS_BYTE_COUNT+1]=0;
                        GPRS_BYTE_COUNT=0;
                    }                 
                    

                }
                GPRS_BYTE_COUNT++;
             }
             
         }

         if(Expected_Response=='C' || Expected_Response=='c' )
         {
              if(GPRSDATABYTE==0x0D && GPRS_BYTE_COUNT<=2)
             {
                 GPRS_BUF[0]=GPRSDATABYTE;
                 GPRS_BYTE_COUNT=1;
             }
             else if(GPRSDATABYTE==0x0A && GPRS_BYTE_COUNT==1)
             {
                 GPRS_BUF[1]=GPRSDATABYTE;
                 GPRS_BYTE_COUNT=2;
             }
             else if(GPRS_BUF[0]==0x0D && GPRS_BUF[1]==0x0A)
             {
                if(GPRS_BYTE_COUNT >=2 && (GPRSDATABYTE==0x0A || GPRSDATABYTE=='>'))
                {
                    GPRS_BUF[GPRS_BYTE_COUNT]=GPRSDATABYTE;
                    GPRS_BUF[GPRS_BYTE_COUNT+1]=0;
                    
                    if(Expected_Response=='C')
                        GPRS_FLAG=1;
                    
                    else if(GPRS_BUF[3]=='C' && GPRS_BUF[7]=='E')
                    {
                        GPRS_FLAG=1;
                        OK_ERROR='E';
                        SIM_INSERTED=0;
                    }
                        
                    GPRS_BYTE_COUNT=0;
                    Expected_Response='O';

                }
                else
                {
                     GPRS_BUF[GPRS_BYTE_COUNT]=GPRSDATABYTE;
                     GPRS_BYTE_COUNT++;
                }
             }
         }
         else if(Expected_Response=='O')
         {
             if(GPRSDATABYTE==0x0D && GPRS_BYTE_COUNT<=2)
             {
                 GPRSOK_BUF[0]=GPRSDATABYTE;
                 GPRS_BYTE_COUNT=1;
             }
             else if(GPRSDATABYTE==0x0A && GPRS_BYTE_COUNT==1)
             {
                 GPRSOK_BUF[1]=GPRSDATABYTE;
                 GPRS_BYTE_COUNT=2;
             }
             else if(GPRSOK_BUF[0]==0x0D && GPRSOK_BUF[1]==0x0A)
             {

                GPRSOK_BUF[GPRS_BYTE_COUNT]=GPRSDATABYTE;
                if(GPRS_BYTE_COUNT>=4  && GPRSOK_BUF[GPRS_BYTE_COUNT]==0x0A && GPRSOK_BUF[GPRS_BYTE_COUNT-1]==0x0D)//&& GPRSOK_BUF[GPRS_BYTE_COUNT-5]==0x0D && GPRSOK_BUF[GPRS_BYTE_COUNT-4]==0x0A
                {
                    if(GPRSOK_BUF[GPRS_BYTE_COUNT-3]=='O' && GPRSOK_BUF[GPRS_BYTE_COUNT-2]=='K')
                    {
                        OK_ERROR='O';
                    }
                    else if(GPRSOK_BUF[GPRS_BYTE_COUNT-5]=='F' && GPRSOK_BUF[GPRS_BYTE_COUNT-4]=='A' && GPRSOK_BUF[GPRS_BYTE_COUNT-3]=='I' && GPRSOK_BUF[GPRS_BYTE_COUNT-2]=='L')
                    {
                        OK_ERROR='F';
                    }
                    else
                    {
                        OK_ERROR='E';
                    }

                    GPRS_FLAG=1;
                    GPRSOK_BUF[GPRS_BYTE_COUNT+1]=0;
                    GPRS_BYTE_COUNT=0;

                }
                else if(GPRS_BYTE_COUNT>190)
                    GPRS_BYTE_COUNT=0;
                else
                    GPRS_BYTE_COUNT++;
             }

         }
         else if(Expected_Response=='D')
         {
            if(GPRSDATABYTE=='#')
            {
                GPRS_BUF[0]=GPRSDATABYTE;
                GPRS_BYTE_COUNT=1;
            }
            else if(GPRS_BYTE_COUNT >=2 && GPRS_BUF[0]=='#' &&  GPRSDATABYTE=='!')
            {

                GPRS_BUF[GPRS_BYTE_COUNT]=GPRSDATABYTE;
                GPRS_BUF[GPRS_BYTE_COUNT+1]=0;
                GPRS_FLAG=1;
                GPRS_BYTE_COUNT=0;
                
            }
            else
            {
                GPRS_BUF[GPRS_BYTE_COUNT]=GPRSDATABYTE;

                GPRS_BYTE_COUNT++;
            }
         }
         if(GPRS_BYTE_COUNT>1900)
         {
             GPRS_BYTE_COUNT=0;
             GPRS_FLAG=1;
         }


        INTClearFlag(INT_SOURCE_UART_RX(UART_GPRS));
    }

    // We don't care about TX interrupt
    if (INTGetFlag(INT_SOURCE_UART_TX(UART_GPRS)))
    {
        INTClearFlag(INT_SOURCE_UART_TX(UART_GPRS));
    }

}
