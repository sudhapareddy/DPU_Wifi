
#include "prototypes.h"

void Printer_SendByte(UINT8 printbyte)
{
    while(!UARTTransmitterIsReady(UART_PRINTER));

    UARTSendDataByte(UART_PRINTER,printbyte);

    while(!UARTTransmissionHasCompleted(UART_PRINTER));
}

BYTE Printer_GetByte(void)
{
    BYTE p_return_byte=0;

    TimerCount=0;
    No_Response=0;
    Max_Timer_Count=2;

    while(!UARTReceivedDataIsAvailable(UART_PRINTER))
    {
        if(No_Response==1)
            return 0xFF;
    }
    p_return_byte = UARTGetDataByte(UART_PRINTER);

    return(p_return_byte);
}

void Printer_SendString(UINT8 pfont,UINT8 pnochars)
{
    UINT8 p_length,p_status;

    PRINTER_OVERFLOW=0;
    //SEERAMReadString(Printer_Status_Loc,1,(char *)&p_temp_char);
    p_length=pnochars;
    pnochars=0;
    
    while(PRINTER_STATUS=='Y')
    {
        Printer_SendByte(0x1B);

        Printer_SendByte(0x76);
        Printer_SendByte(0x76);
       
        p_status=Printer_GetByte();

        if((p_status & 0x04)==0x04 || (p_status & 0x08) ==0x08 || (p_status & 0x40) ==0x40)
        {

            CLEAR_DATA_LINES_FROM(5,7);
            if((p_status & 0x04)==0x04)
                STRCPY(LBUF,"Please Check Paper");

            if((p_status & 0x08) ==0x08)
                STRCPY(LBUF,"Printer Power Error.");

            if((p_status & 0x40) ==0x40)
                STRCPY(LBUF,"Printer Temp Over...");           

            Disp_Msg(LINE5);

            STRCPY(LBUF,"Esc-Off , Ent-Check");
            Disp_Msg(LINE7);

            Buzzer(250);
        }
         else
            break;


        KBV=Wait_Key_Press('E');

        if(KBV==KB_CANCEL)
        {
            PRINTER_STATUS='N';
            SEERAMWriteString(Printer_Status_Loc,1,(char *)&PRINTER_STATUS);
        }
        KBV = Scan_Key('E');
        
        if(KBV!=0xFF)
            break;
        Delay_Sec(2);
        CLEAR_DATA_LINES_FROM(5,7);
        Please_Wait_Msg_Small(LINE6);
    }

    
    if(pfont=='S')
    {
        Printer_SendByte(0x1B);
        Printer_SendByte(0x21);
        Printer_SendByte(0x01);
    }
   
    while(pnochars<p_length && PBUF[pnochars]!=0x00)
    {
        Printer_SendByte(PBUF[pnochars]);
        pnochars++;
    }

    Printer_SendByte(0x0D);

    
    Print_LineNo++;

    if(Print_LineNo>=25 && PRINTER_STATUS!='Y')
    {
        Delay_Sec(2);
        Print_LineNo=0;
    }
}

void Paper_Feed(UINT8 fcnt)
{
    while(fcnt--)
    {
        Printer_SendByte(0x1B);
        Printer_SendByte(0x4A);
        Printer_SendByte(30);
        
        Print_LineNo++;
        
        if(Print_LineNo>=25)
        {
            Delay_Sec(2);
            Print_LineNo=0;
        }
    }
}

void PRINT_UNDERSCORE(void)
{
    UINT8 p_u_cnt;

    Init_Regular_Font();
    for(p_u_cnt=0;p_u_cnt<32;p_u_cnt++)
            PBUF[p_u_cnt]='-';

    PBUF[32]='\0';
    Printer_SendString(0x03,32);
}

void Init_New_Printer(void)
{
#ifdef WIFI
    UARTConfigure(UART_PRINTER, UART_INVERT_RECEIVE_POLARITY | UART_INVERT_TRANSMIT_POLARITY | UART_ENABLE_PINS_TX_RX_ONLY);
#else
       UARTConfigure(UART_PRINTER,  UART_ENABLE_PINS_TX_RX_ONLY);
#endif 

    UARTSetFifoMode(UART_PRINTER, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART_PRINTER, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART_PRINTER, GetPeripheralClock(), 9600);
    UARTEnable(UART_PRINTER, UART_ENABLE_FLAGS(UART_PERIPHERAL |  UART_RX |UART_TX ));

    PRINTER_OVERFLOW=0;

    Printer_SendByte(0x1B);
    Printer_SendByte(0x40);		//Printer Initialization

    Printer_SendByte(0x1B);		//allign
    Printer_SendByte(0x52);
    Printer_SendByte(0x00);

    Printer_SendByte(0x1B);
    Printer_SendByte(0x40);

    Paper_Feed(1);
    SEERAMReadString(Printer_Status_Loc,1,(char *)&PRINTER_STATUS);
}
