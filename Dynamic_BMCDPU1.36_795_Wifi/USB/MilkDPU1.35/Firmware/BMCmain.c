





//Boot Loader Code
 # define _SUPPRESS_PLIB_WARNING

#define   NAME_PH_SEAvRCH1

#define   WIFI

# define SMARTCHIP
#define BLOCK_1_COUNT 500
#define BLOCK_2_COUNT 1250
#define BLOCK_4_COUNT 1350

#define FONT_SMALL  'S'
#define FONT_BIG    'B'

#define NULL_TERMINATOR 'Y'
#define NO_TERMINATOR   'N'


# include <stdlib.h>
# include <string.h> 
# include <GenericTypeDefs.h> 
# include <ctype.h>
# include "Config.h"
# include "USB/usb.h"
# include "USB/usb_host_msd.h"
# include "USB/usb_host_msd_scsi.h"
# include "MDD File System/FSIO.h"
# include "variables.h"
# include "main.h"
# include "prototypes.h"
# include "ps2kb.h"
# include "glcd32new.c"
# include "Menu_List.h"
# include "Menu_Choice.h"
# include "ps2kb.c"
# include "Globalfunctions.h"
# include "messages.h"
# include "Common_Functions.h"
# include "uart_basic32.c"
# include "RTC_I2C.c"
# include "sst25vf016b.c"
# include "newprinter.c"
# include "epsonfx-875.c"
# include "MemoryFunctions.h"
# include "PrintFunctions.h"
# include "Ext40_PrintFunctions.h"
# include "Ext_PrintFunctions.h"
# include "USB_Functions.h"
# include "gprs.h"
# include "Modem_Functions.c"
//# include "QuectelM95_SIM900.c"
//# include "WIFI.h"
# include "Copy_New_RateCharts.h"
//# include "Fat_Snf_Steps.c"
# include "USB/usb_host_hid_parser.h" 
# include "Dealer_Selection.h"
# include "Display_Header_Messages.h"
# include "SNF_Rate_Tables.h"
# include "FAT_Rate_Tables.h"
# include "GPRS_File.h"
# include "Settings_File.h"
# include "Server_Functions.h"
# include "Member_Functions.h"



BOOL USB_ApplicationEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size)
{ 
    switch(event)
    {
        case EVENT_PRINTER_ATTACH:

            status.printerAttached      = 1;
            printerInfo                 = *(USB_PRINTER_DEVICE_ID *)data;
            return TRUE;

            break;

        case EVENT_PRINTER_UNSUPPORTED:

            return TRUE;
            break;

        case EVENT_PRINTER_TX_DONE:
//            UART2PrintString( "APP: OUT transfer complete\r\n" );
            return TRUE;
            break;

        case EVENT_PRINTER_RX_DONE:
//            UART2PrintString( "APP: IN transfer complete\r\n" );
            return TRUE;
            break;

        case EVENT_PRINTER_REQUEST_DONE:
//            UART2PrintString( "APP: Printer Status: " );
//            UART2PutHex( status.printer );
//            UART2PrintString( "\r\n" );
            return TRUE;
            break;

        // --------------------------------------------------------------------------
        // Host events


        case EVENT_VBUS_REQUEST_POWER:
            // The data pointer points to a byte that represents the amount of power
            // requested in mA, divided by two.  If the device wants too much power,
            // we reject it.
            return TRUE;

        case EVENT_VBUS_RELEASE_POWER:
            // Turn off Vbus power.
            // The PIC24F with the Explorer 16 cannot turn off Vbus through software.

            //This means that the device was removed
            deviceAttached = FALSE;
            return TRUE;
            break;

        case EVENT_HUB_ATTACH:
            return TRUE;
            break;

        case EVENT_UNSUPPORTED_DEVICE:
            return TRUE;
            break;

        case EVENT_CANNOT_ENUMERATE:
            //UART2PrintString( "\r\n***** USB Error - cannot enumerate device *****\r\n" );
            return TRUE;
            break;

        case EVENT_CLIENT_INIT_ERROR:
            //UART2PrintString( "\r\n***** USB Error - client driver initialization error *****\r\n" );
            return TRUE;
            break;

        case EVENT_OUT_OF_MEMORY:
            //UART2PrintString( "\r\n***** USB Error - out of heap memory *****\r\n" );
            return TRUE;
            break;

        case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
            //UART2PrintString( "\r\n***** USB Error - unspecified *****\r\n" );
            return TRUE;
            break;

        case EVENT_PRINTER_DETACH:

            status.printerAttached      = 0;
            status.pagePrinted          = 0;
            printerInfo.deviceAddress   = 0;
            return TRUE;
            break;

        default:
            break;
    }
    return FALSE;
}


void Send_Collection_Ext_Display(void)
{
    UINT8  ext_digit_count=0,ext_send_buf[50];
    UINT16 snf_clr;

    ZERO=1;
    STRCPY(ext_send_buf,"$9000                                      ?");

    Hex_DEC(0,CollectionRecord.CODE,ext_send_buf+5,4);
  
    Read_Collection_Flags();    
    
    ext_send_buf[9]=CollectionFlags.MILK_TYPE;
    
    if(CollectionFlags.MILK_TYPE=='B')
        ext_send_buf[9]='b';
   
    ext_digit_count=10;
    
    if(CollectionRecord.FAT>0)
    {
        Hex_DEC(1,CollectionRecord.FAT,ext_send_buf+ext_digit_count,4);
        ext_digit_count=ext_digit_count+4;        
    }
    else
        ext_digit_count=ext_digit_count+3;

    snf_clr=(CLR_BASED == 'Y') ? CollectionRecord.CLR1 : CollectionRecord.SNF;
    
    if(CollectionRecord.SNF>0)
    {
        Hex_DEC(1,CollectionRecord.SNF,ext_send_buf+ext_digit_count,4);
        ext_digit_count=ext_digit_count+4;        
    }
    else
        ext_digit_count=ext_digit_count+3;
  
     if(CollectionRecord.WATER1>0 && CollectionRecord.WATER1!=0xFFFF)
    {
        Hex_DEC(1,CollectionRecord.WATER1,ext_send_buf+ext_digit_count,4);  
        ext_digit_count=ext_digit_count+4;//3;        
    }
    else
        ext_digit_count=ext_digit_count+3;
    
    if(CollectionRecord.FAT>0)
    {        
        Hex_DEC(2,CollectionRecord.RATE,ext_send_buf+ext_digit_count,6);
        ext_digit_count=ext_digit_count+6;        
    }
    else
        ext_digit_count=ext_digit_count+5;
    
    
    if(CollectionRecord.QTY!=0xFFFF)
        Hex_DEC(2,CollectionRecord.QTY,ext_send_buf+ext_digit_count,7);
    ext_digit_count=ext_digit_count+7;

    Calculate_Amount();

    Hex_DEC(2,Amount,ext_send_buf+ext_digit_count,8);
    ext_digit_count=ext_digit_count+8;  

    ext_send_buf[43]='?';
    SP_SendString(ext_send_buf,44);

    ZERO=0;
}

void Display_From_To_Code(UINT8 etype,UINT16 fcode,UINT16 ecode)
{
    STRCPY(LBUF,"MEMBER CODE:");
    Disp_Msg(LINE5);
    
    Disp_Top_Boarder_Big_T(LINE6,5,0x8A);
                
    ZERO=0;
    STRCPY(LBUF," FROM|");
    LBUF[0]=0x83;
    LBUF[15]=0x85;    
    Hex_DEC(0,fcode,LBUF+6,4);    
    LBUF[10]='-';
    Hex_DEC(0,ecode,LBUF+11,4);
    NOR_DISPLAY_LAST_LINE=0xC0;
    Disp_Msg_Big(0,LINE7);
    
    if(etype==0)
        Hex_DEC(0,fcode,Temp_Arr,4);
    else
        Hex_DEC(0,ecode,Temp_Arr,4);
    Shift_Left(Temp_Arr,4);
    Temp_Arr[4]='\0';
}

UINT8 Start_End_Members(UINT8 daysflag)
{  
    UINT8  s_step=0,p_step;
    UINT16 s_member_code,end_code;
    
    while(1)
    {
        switch(s_step)
        {
            case 0:

                Display_Boarder3();
                            
                if(daysflag==1)                                 //From To Dates
                {
                    Copy_MonthRecDT(LBUF);
                    Disp_Msg(LINE3);
                }
             
                if(daysflag==3)                            //Selected Shift
                    Display_Shift(LINE3);
                            
                Display_From_To_Code(0,MIN_CODE,MAX_CODE);                
                
                GET_VALUE_STATUS=Get_Numeric_Big(4,0,LINE7+6,(char *)&Temp_Arr);
                                
                Reset_Display_Values();
                
                if(GET_VALUE_STATUS==0xF9)
                {
                    s_step=9;
                    p_step=0;
                    break;
                }
                
                if(KBV==KB_LEFT)
                    return 'L';

                if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                    return 0xFF;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    s_member_code=atoi(Temp_Arr);
                    
                    if(s_member_code>=MIN_CODE  && s_member_code<=MAX_CODE)
                    {
                        Fromrec.START_MEMBER=s_member_code;
                        s_step=1;
                    }
                }

                break;

            case 1:
               
                Display_From_To_Code(1,Fromrec.START_MEMBER,MAX_CODE);
                
                GET_VALUE_STATUS=Get_Numeric_Big(4,0,LINE7+11,(char *)&Temp_Arr);
                
                Reset_Display_Values();
                
                if(GET_VALUE_STATUS==0xF9)
                {
                    s_step=9;
                    p_step=1;
                    break;
                }
                
                if(KBV==KB_LEFT)
                {
                    s_step=0;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return 0xFF;

                else
                {
                    s_member_code=atoi(Temp_Arr);
                    
                    if(s_member_code>=Fromrec.START_MEMBER  && s_member_code<=MAX_CODE)
                    {                        
                        Fromrec.END_MEMBER=s_member_code;
                     
                        return 0x00;
                    }
                   
                }
                break;
                
            case 9:
                
                s_step=p_step;
                break;
        }
    }
}

UINT8 Check_Shift_Timings(void)
{
	UINT8 select_shift=0;

	Read_Date_Month_Year();

    if(HEX_ASCII(Time[2])>=M_ShiftTimerec.START_HOUR && HEX_ASCII(Time[2])<M_ShiftTimerec.END_HOUR)
	{
        Shiftrec.DAY_SHIFT='M';
		select_shift=1;
	}
    if(HEX_ASCII(Time[2])>=E_ShiftTimerec.START_HOUR && HEX_ASCII(Time[2])<E_ShiftTimerec.END_HOUR)
    {
		Shiftrec.DAY_SHIFT='E';
		select_shift=1;
	}
	return select_shift;
}

void Read_Shift(void)
{
    SEERAMReadString(Morning_Shift_Timings_Loc,2,(char *)&M_ShiftTimerec);
    SEERAMReadString(Evening_Shift_Timings_Loc,2,(char *)&E_ShiftTimerec);

    
    if(HEX_ASCII(Time[2])>=M_ShiftTimerec.START_HOUR && HEX_ASCII(Time[2])<M_ShiftTimerec.END_HOUR)
        Shiftrec.DAY_SHIFT='M';
    
    else if(HEX_ASCII(Time[2])>=E_ShiftTimerec.START_HOUR && HEX_ASCII(Time[2])<=E_ShiftTimerec.END_HOUR)
        Shiftrec.DAY_SHIFT='E';
    
    else if(HEX_ASCII(Time[2])<12)
        Shiftrec.DAY_SHIFT='M';
    
    else
        Shiftrec.DAY_SHIFT='E';
}

void Read_Date_Month_Year(void)
{    
    Read_RTC();

    Shiftrec.DAY=HEX_ASCII(Time[4]);
    Shiftrec.MONTH=HEX_ASCII(Time[5]);
    Shiftrec.YEAR=HEX_ASCII(Time[6]);
        
    Read_Shift();
}

UINT8 Select_Shift(void)
{
    UINT8 select_count=0;

    Read_RTC();

    Read_Shift();
    
    select_count=1;
    if(Shiftrec.DAY_SHIFT=='M')
        select_count=0;
    
    CURSOR_BLINK=0xC0;
    STRCPY(LBUF," SHIFT         ");
    LBUF[0]=0x83;
    LBUF[6]=0x83;
    LBUF[15]=0x85;

    Disp_Msg_Big(0,LINE7);
    
    while(1)
    {
        DISPLAY_NO_SPACES=1;
        CURSOR_BLINK=0x00;
        DISPLAY_LAST_LINE=0x30;

        if(select_count==0)
            STRCPY(LBUF,"MORNING");

        if(select_count==1)
            STRCPY(LBUF,"EVENING");
         
         Disp_Msg_Big(1,LINE7+8);
         Reset_Display_Values();

        KBV=Wait_Key_Press('C');

        switch(KBV)
        {
            case KB_CANCEL:

                return 0xFF;

            case KB_LEFT:

                return 'L';

            case KB_ENT:

                if(select_count==0)
                    return 'M';

                if(select_count==1)
                    return 'E';
        }
        select_count=Up_Down(select_count,2);
    }
}

void Display_Date_Values(UINT8 dval,UINT8 dtempdate)
{
    if(dval<4)
        Display_Box_Boarder(LINE7);
    else
    {
        CLEAR_DATA_LINES_FROM(5,6);
        NOR_DISPLAY_LAST_LINE=0x30;
    }
    ZERO=0;
    STRCPY(LBUF,"               ");
    LBUF[0]=0x83;
    LBUF[6]=0x83;
    if(dval<4)
        LBUF[10]=0x84;
    if(dval==4)
        LBUF[10]=0x87;
    LBUF[15]=0x85;
    
    Hex_DEC(0,Shiftrec.YEAR,LBUF+2,2);
    Hex_DEC(0,Shiftrec.YEAR,Temp_Arr,2);
    
    if(dval>1)
    {
        Hex_DEC(0,Shiftrec.MONTH,LBUF+7,2);
        Hex_DEC(0,Shiftrec.MONTH,Temp_Arr,2);
    }
    if(dval>2)
    {
        Hex_DEC(0,dtempdate,Temp_Arr,2);
        STRNCPY(LBUF+12,Temp_Arr,2);
    }
    Temp_Arr[2]='\0';
 
    if(dval<4)
        NOR_DISPLAY_LAST_LINE=0xC0;
   
    Disp_Msg_Big(0,LINE5);
   
    if(dval==4)
        NOR_DISPLAY_LAST_LINE=0x00;
}

UINT8 Select_Date_Month_Shift(UINT8 mflag)
{
    UINT8 sel_step=0,prev_step,exit_value;
    UINT8 temp_date,temp_year,temp_month,max_days;

    CURSOR_BLINK=0x30;
  
    STRCPY(LBUF," YEAR  MON|DATE");
    LBUF[0]=0x83;
    LBUF[6]=0x83;
    LBUF[15]=0x85;

    Disp_Msg_Big(0,LINE3);
    DISPLAY_FIRST_LINE=0x00;
    Read_Date_Month_Year();
    
    CURSOR_BLINK=0x00;
    TempShift=Shiftrec;
    
    while(1)
    {
        switch(sel_step)
        {
            case 0:

                Display_Date_Values(1,0);

                NOR_DISPLAY_LAST_LINE=0xC0;
                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE5+2,(char *)&Temp_Arr);

                if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                {
                    exit_value=0xFF;
                    sel_step=10;
                    break;
                }

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    temp_year=atoi(Temp_Arr);

                    if(temp_year>0)
                    {
                        prev_step=sel_step;
                        Shiftrec.YEAR=temp_year;
                        
                        if(temp_year!=TempShift.YEAR)
                            Shiftrec.MONTH=1;
                        else
                            Shiftrec.MONTH=TempShift.MONTH;

                        sel_step=1;
                    }
                }
                break;

             case 1:

                Display_Date_Values(2,0);

                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE5+7,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                {
                    sel_step=0;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                {
                    exit_value=0xFF;
                    sel_step=10;
                    break;
                }

                else
                {
                    temp_month=atoi(Temp_Arr);

                    if(temp_month>=1 && temp_month<=12)
                    {
                        if(mflag==1)
                            sel_step=2;
                        else
                        {
                            prev_step=sel_step;
                            sel_step=2;
                        }
                        
                        if(sel_step==2)
                            Shiftrec.MONTH=temp_month;
                    }
                }
                break;

            case 2:

                Read_RTC();

                if(Shiftrec.YEAR==HEX_ASCII(Time[6]) && Shiftrec.MONTH==HEX_ASCII(Time[5]))
                {
                    max_days=HEX_ASCII(Time[4]);
                    temp_date=max_days;
                }
                else
                {
                    max_days=Get_Month_Days(Shiftrec.MONTH,Shiftrec.YEAR);
                    temp_date=1;
                }

                Display_Date_Values(3,temp_date);

                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE5+12,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                {
                    sel_step=1;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                {
                    exit_value=0xFF;
                    sel_step=10;
                    break;
                }

                else
                {
                    Shiftrec.DAY=atoi(Temp_Arr);

                    if(Shiftrec.DAY>0 && Shiftrec.DAY<32)
                    {
                        if(Shiftrec.DAY<=max_days)
                        {
                            prev_step=sel_step;
                            temp_date=Shiftrec.DAY;
                            sel_step=3;
                            break;
                        }
                    }
                }
                break;

            case 3:
                
                Display_Date_Values(4,temp_date);
                
                Shiftrec.DAY_SHIFT=Select_Shift();
                
                if(KBV==KB_LEFT)
                {
                    sel_step=2;
                    break;
                }
                
                prev_step=2;
                
                if(Shiftrec.DAY_SHIFT==0xFF)
                    sel_step=0;

                else if(Read_Day_Flag('C')!=1 && mflag==2)
                    sel_step=9;
                else
                {
                    exit_value=0x00;
                    sel_step=10;
                    break;
                }
                break;

            case 9:

                NOR_DISPLAY_LAST_LINE=0xC0;
                STRCPY(LBUF," NO RECORDS...");
                Disp_Msg_No_Spaces(0,LINE7+1);
                Reset_Display_Values();

                Delay_Sec(1);

                sel_step=prev_step;

                break;

            case 10:

                Reset_Display_Values();
                return exit_value;
        }
    }
}

void Read_UltraPro_Inputs(BYTE * Analyzerbuf)
{    
    STRNCPY(Analyzer_Arr,(char *)Analyzerbuf+1,3);
    Analyzer_Arr[3]='\0';
    CollectionRecord.FAT=Float_Long(Analyzer_Arr);

    STRNCPY(Analyzer_Arr,(char *)Analyzerbuf+5,3);
    Analyzer_Arr[3]='\0';
    CollectionRecord.SNF=Float_Long(Analyzer_Arr);

    STRNCPY(Analyzer_Arr,(char *)Analyzerbuf+9,3);
    
    Analyzer_Arr[3]='\0';
    CollectionRecord.CLR1=Float_Long(Analyzer_Arr);

    STRNCPY(Analyzer_Arr,(char *)Analyzerbuf+13,3);
  
    Analyzer_Arr[3]='\0';
    CollectionRecord.WATER1=Float_Long(Analyzer_Arr);
}

void Read_Ultra_Inputs(BYTE *Analyzerbuf)
{
    if(Analyzerbuf[12]==0 || Analyzerbuf[12]==2 || Analyzerbuf[12]==3)
    {
        if((Analyzerbuf[0]==0x00 && Analyzerbuf[1]==0x00) && (Analyzerbuf[2]==0x00 && Analyzerbuf[3]==0x00) && (Analyzerbuf[4]==0x00 && Analyzerbuf[5]==0x00) && (Analyzerbuf[6]==0x00 && Analyzerbuf[7]==0x00))
        {
            JUNK_DATA=1;
            CollectionRecord.FAT=0;
            return;
        }
        else
            JUNK_DATA=0;
        
        Hex_ASCII(Analyzerbuf[0],Analyzer_Arr);
        Hex_ASCII(Analyzerbuf[1],Analyzer_Arr+2);
        Analyzer_Arr[3]='\0';

        CollectionRecord.FAT=Float_Long(Analyzer_Arr);

        Hex_ASCII(Analyzerbuf[2],Analyzer_Arr);
        Hex_ASCII(Analyzerbuf[3],Analyzer_Arr+2);
        Analyzer_Arr[3]='\0';
        CollectionRecord.SNF=Float_Long(Analyzer_Arr);

        Hex_ASCII(Analyzerbuf[4],Analyzer_Arr);
        Hex_ASCII(Analyzerbuf[5],Analyzer_Arr+2);
        Analyzer_Arr[3]='\0';
        CollectionRecord.CLR1=Float_Long(Analyzer_Arr);
        
        Hex_ASCII(Analyzerbuf[6],Analyzer_Arr);
        Hex_ASCII(Analyzerbuf[7],Analyzer_Arr+2);
       
        Analyzer_Arr[3]='\0';
        CollectionRecord.WATER1=Float_Long(Analyzer_Arr);
    }
    else
        CollectionRecord.FAT=0;
}

void Calculate_Milko_Tester_Inputs(BYTE * Analyzerbuf)
{
    Analyzerbuf[4]='\0';
    CollectionRecord.FAT=Float_Long(Analyzerbuf);

    CollectionRecord.SNF=0;

    CollectionRecord.CLR1=0;
    CollectionRecord.WATER1=0;
}

void Calculate_Lacto_Inputs(BYTE * Analyzerbuf,UINT8 anobytes)
{
    CollectionRecord.CLR1=0;
    CollectionRecord.WATER1=0;
    
    Analyzerbuf[5]='\0';
    CollectionRecord.FAT=Float_Long(Analyzerbuf+1);

    Analyzerbuf[11]='\0';
    CollectionRecord.SNF=Float_Long(Analyzerbuf+7);

    if(anobytes>17)
    {
        Analyzerbuf[17]='\0';
        CollectionRecord.CLR1=Float_Long(Analyzerbuf+13);      
    }
  
    if(anobytes>47)
    {
        Analyzerbuf[47]='\0';
       
        CollectionRecord.WATER1=Float_Long(Analyzerbuf+43);
    }
}

void Calculate_Essae_Inputs(BYTE * Analyzerbuf)
{
    Analyzerbuf[6]='\0';
    CollectionRecord.FAT=Float_Long(Analyzerbuf+2);

    Analyzerbuf[13]='\0';
    CollectionRecord.SNF=Float_Long(Analyzerbuf+9);

    CollectionRecord.CLR1=0;

    Analyzerbuf[20]='\0';
    CollectionRecord.WATER1=Float_Long(Analyzerbuf+16);    
       
    Analyzerbuf[0]=0x00;    
}

void Read_Analyzer_Col_Time(void)
{
    Read_RTC();
    
    CollectionRecord.ATIME[0]=HEX_ASCII(Time[2]);//1
    CollectionRecord.ATIME[1]=HEX_ASCII(Time[1]);
    CollectionRecord.ATIME[2]=HEX_ASCII(Time[0]);
}

void Calculate_Ksheera_Inputs(BYTE * Analyzerbuf)
{
    CollectionRecord.CLR1=0;
    CollectionRecord.WATER1=0;
    
    if(Analyzerbuf[12]=='F')
    {
        Analyzerbuf[17]='\0';
        CollectionRecord.FAT=Float_Long(Analyzerbuf+13);
    }

    if(Analyzerbuf[19]=='S')
    {
        Analyzerbuf[24]='\0';
        CollectionRecord.SNF=Float_Long(Analyzerbuf+20);
    }
    
    if(Analyzerbuf[26]=='C')
    {
        Analyzerbuf[31]='\0';
        CollectionRecord.CLR1=Float_Long(Analyzerbuf+27);
     
    }
   
    if(Analyzerbuf[54]=='W')
    {
        Analyzerbuf[59]='\0';
        CollectionRecord.WATER1=Float_Long(Analyzerbuf+55);
    }
}

void Calculate_Milkometer_Inputs(BYTE * Analyzerbuf)
{
    CollectionRecord.CLR1=0;
    CollectionRecord.WATER1=0;
    
    if(Analyzerbuf[52]=='F')
    {
        Analyzerbuf[71]='\0';
        CollectionRecord.FAT=Float_Long(Analyzerbuf+67);
    }

    if(Analyzerbuf[74]=='S')
    {
        Analyzerbuf[93]='\0';
        CollectionRecord.SNF=Float_Long(Analyzerbuf+89);
    }
    
    if(Analyzerbuf[96]=='D')
    {
        Analyzerbuf[115]='\0';
        CollectionRecord.CLR1=Float_Long(Analyzerbuf+111);     
    }
}

void Calculate_Analyzer_Data(BYTE * tbuf)
{
    UINT16 temp_snf,temp_clr;
    
    JUNK_DATA=0;
    
    if(Settingsrec.ANALYZER=='P')
        Read_UltraPro_Inputs(tbuf);

    if(Settingsrec.ANALYZER=='U')
        Read_Ultra_Inputs(tbuf);

    if(Settingsrec.ANALYZER=='M' || Settingsrec.ANALYZER=='F')
        Calculate_Milko_Tester_Inputs(tbuf);

    if(Settingsrec.ANALYZER=='K')
        Calculate_Ksheera_Inputs(tbuf);
    
    if(Settingsrec.ANALYZER=='A')
        Calculate_Milkometer_Inputs(tbuf);

    if(Settingsrec.ANALYZER=='L')
    {
        if(Settingsrec.USE_ANALYZER_PORT2=='Y')
            Calculate_Lacto_Inputs(tbuf,A_No_Bytes2);
        else
            Calculate_Lacto_Inputs(tbuf,A_No_Bytes);
    }
    
    if(Settingsrec.ANALYZER=='E')
        Calculate_Essae_Inputs(tbuf);
   
    Read_Analyzer_Col_Time();
        
//    temp_snf=CollectionRecord.SNF;
//    temp_clr=CollectionRecord.CLR1;
//    
//    if(CLR_BASED=='Y')
//    {
//        CollectionRecord.SNF=temp_clr;
//        CollectionRecord.CLR1=temp_snf;
//    }
}

UINT8 Read_Analyzer_Inputs(void)
{
    UINT16 a_baud_rate;

    LCD_Set_Cursor('S',LINE5);
    CURSOR_BLINK=0x80;
    Display_Char(' ');
    CURSOR_BLINK=0x00;
    
    Update_Analyzer_Flag=0;
    Update_Analyzer_Flag2=0;
    ANALYZER_FLAG2=0;
    ANALYZER_FLAG=0;

    if(Settingsrec.USE_ANALYZER_PORT2=='Y')
    {
        while(ANALYZER_FLAG2==0)
        {
            KBV=Scan_Key('E');

            if(KBV==KB_CANCEL)
                return 0xFF;
            
            if(KBV==KB_LEFT)
                return 'L';
        }
    }
    else
    {
        while(ANALYZER_FLAG==0)
        {
            KBV=Scan_Key('E');
           
            if(KBV==KB_CANCEL)
                return 0xFF;
            
            if(KBV==KB_LEFT)
                return 'L';
        }
    }
    CURSOR_BLINK=0x00;
    Display_Box_Boarder(LINE7);//CLEAR_DATA_LINES_FROM(7,8);
   
    if(Settingsrec.USE_ANALYZER_PORT2=='Y')
        Calculate_Analyzer_Data(REC_BUF2);
    else
        Calculate_Analyzer_Data(REC_BUF);
    Buzzer(250);
       
    return 1;
}

UINT8 Initialize_Analyzers(UINT8 anatype)
{
    UINT16 a_baud_rate,nobits;

    if(Settingsrec.ANALYZER=='P')
    {
        a_baud_rate=2400;
        nobits=8;        
    }

    else if(Settingsrec.ANALYZER=='L' || Settingsrec.ANALYZER=='M' || Settingsrec.ANALYZER=='F' || Settingsrec.ANALYZER=='E')
    {
        SEERAMReadString(L_Baud_Rate_Loc,2,(char *)&a_baud_rate);

        if(a_baud_rate==0)
            a_baud_rate=2400;

        A_No_Bytes=0;
        nobits=8;
    }
    else if(Settingsrec.ANALYZER=='K' || Settingsrec.ANALYZER=='A')
    {
        a_baud_rate=9600;
        nobits=8;
    }
    else
    {
        a_baud_rate=1200;
        nobits=9;
        
        if(Settingsrec.ANALYZER!='U')
        {
            Settingsrec.ANALYZER='U';
            SEERAMWriteString(Analyzer_Type_Loc,1,(char *)&Settingsrec.ANALYZER);
        }
    }
    
    if(anatype==1 || anatype=='B')
    {
        Init_Analyzer(a_baud_rate,nobits);
        
    }
    if(anatype==2 || anatype=='B')
        Init_Analyzer2(a_baud_rate,nobits);//1
   
    return 1;
}


 UINT8 Save_Transaction(UINT8 collectiontype)
{
    UINT8 tempfatmode;
    
    if(collectiontype=='C')
        Read_Date_Month_Year();

    if(collectiontype=='E')
    {
        Menu_Header('E',0);
        Display_Boarder3();
        STRCPY(LBUF," Collection Editing");
        Disp_Msg(LINE4);

        Please_Wait_Msg_Small(LINE6);

        Edit_BillDetails(Member_SNo);
    }
    else if(RunningShift.DAY_SHIFT==Shiftrec.DAY_SHIFT)
    {
        tempfatmode=Settingsrec.FAT_MODE;
        if(collectiontype=='E')
            Settingsrec.FAT_MODE='M';
        
        Write_Weight();
        
        CollectionRecord.ATIME[0]=CollectionRecord.WTIME[0];
        CollectionRecord.ATIME[1]=CollectionRecord.WTIME[1];
        CollectionRecord.ATIME[2]=CollectionRecord.WTIME[2];
        STRNCPY((char*)&AnalyzerRecord.FAT,(char*)&CollectionRecord.FAT,9);
        Write_Analyzer_Data();
        
        Settingsrec.FAT_MODE=tempfatmode;
        
        SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&TempShift);
        if(TempShift.DAY==Shiftrec.DAY && TempShift.MONTH==Shiftrec.MONTH && TempShift.YEAR==Shiftrec.YEAR && TempShift.DAY_SHIFT==Shiftrec.DAY_SHIFT )
        {
            //SEERAMReadString(Running_Shift_Incentive_Loc,4,(char *)&Incentiverec);  
            SEERAMReadString(Running_Shift_Incentive_Loc,20,(char *)&Incentiverec);  
            return 'S';
        }
        if(collectiontype=='C' || collectiontype=='D')
        {
            SEERAMWriteString(Running_Shift_Details_Loc,4,(char *)&Shiftrec);
            
            Write_Incentive_Data();
           
        }
        return 'S';
    }
    else
        return 0xFF;
}
 
 void Write_Incentive_Data(void)
 {  
     UINT8 temp_inc_cnt=0;
     
    SEERAMReadString(Auto_Commission_Loc,Commission_Length,(char *)&Commissionrec);
    if(Commissionrec.STATUS=='N')
    {
        Commissionrec.AUTO_RATE=0;
        for(temp_inc_cnt=0;temp_inc_cnt<9;temp_inc_cnt++)
            Commissionrec.SPECIAL_RATE[temp_inc_cnt]=0;
    }
    
    SEERAMWriteString(Running_Shift_Incentive_Loc,20,(char *)&Commissionrec.AUTO_RATE);
    SEERAMReadString(Running_Shift_Incentive_Loc,20,(char *)&Incentiverec);  
    
 }

UINT8 Save_Weight(UINT8 collectiontype)
{
    Read_Date_Month_Year();

    if(RunningShift.DAY_SHIFT==Shiftrec.DAY_SHIFT)
        Write_Weight();
    else
        return 0xFF;
    
    SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&TempShift);
    if(TempShift.DAY==Shiftrec.DAY && TempShift.MONTH==Shiftrec.MONTH && TempShift.YEAR==Shiftrec.YEAR && TempShift.DAY_SHIFT==Shiftrec.DAY_SHIFT )
    {
         SEERAMReadString(Running_Shift_Incentive_Loc,20,(char *)&Incentiverec);  
        return 'S';
    }
   
    if(collectiontype=='C' || collectiontype=='D')
    { 
        SEERAMWriteString(Running_Shift_Details_Loc,4,(char *)&Shiftrec);
        Write_Incentive_Data();
    }
}

void Display_Shift(UINT8 dline)
{
    STRCPY(LBUF,"  /  /       EVENING");
    Copy_ShiftDT(LBUF);

    if(Shiftrec.DAY_SHIFT=='M')
        STRCPY(LBUF+13,"MORNING");
    
    if(dline==LINE8)
    {
        LBUF[10]='|';
        Invert_Disp_Msg(LINE8);
    }
    else
        Disp_Msg(dline);
}

void Display_Shift_SampleNo(void)
{
    
    STRCPY(LBUF,"  /  /  (M) SNo:");
    Copy_ShiftDT(LBUF);
    LBUF[9]=Shiftrec.DAY_SHIFT;
    ZERO=0;
    Hex_DEC(0,CollectionRecord.SHIFT_RECORD_NO,LBUF+16,4);
   // Shift_Left(LBUF+16,4);
    ZERO=1;
    Disp_Msg(LINE1);
}

void Collection_Not_Found_Msg(void)
{
    STRCPY(LBUF,"COLLECTION NOT FOUND");
    Disp_Msg(LINE6);
}

void Collection_Receipt(UINT8 receipttype)
{
    UINT8 c_step=0;
    UINT16 c_member_sno,noofrecords=0,temprecordno=0;

    SEERAMReadString(MilkType_Print_On_Loc,RECEIPT_RECORD_LENGTH,(char *)&Receiptrec);
    
    while(1)
    {
        switch(c_step)
        {
            case 0:
                
                if(receipttype=='S')
                   Message_Header('C',1);
                else
                    Menu_Header('c',1);
                
                if(Select_Date_Month_Shift(2)==0xFF)
                    return;
                
                c_step=1;
                break;
                
            case 1:
                
                if(receipttype=='S')
                   Message_Header('C',1);
                else
                    Menu_Header('c',1);

               Display_Shift(LINE8);

                c_step=Member_Code_Entry(0);
               
                if(c_step==0xFF)
                    c_step=0;
                else if(c_step==1)
                {
                    c_member_sno=Read_Member_SNO(Member_Code);
                    c_step=2;
                }
                else
                    c_step=1;
                break;

            case 2:

                Please_Wait_Msg_Small(LINE6);
                noofrecords=DynamicRead_BillDetails(c_member_sno);
               
                if(noofrecords>0)
                {
                    temprecordno=noofrecords;
                    do
                    {
                        CollectionRecord=CollectionRecords[temprecordno-1];//1
                        CollectionRecord_NO.RECORDNO=CollectionRecords_NOS[temprecordno-1].RECORDNO;
                        CollectionRecord_NO.Type=CollectionRecords_NOS[temprecordno-1].Type;
                        Read_Collection_Flags();
                        Tablerec.MILK_TYPE=CollectionFlags.MILK_TYPE;
                        
                        if(noofrecords>1)
                        {
                            Display_Bill_Details(2,0);
                            KBV=Wait_Key_Press('E');
                            if(KBV==KB_UP)
                            {
                                if(temprecordno==noofrecords)
                                    temprecordno=1;
                                else
                                    temprecordno++;
                            }
                            if(KBV==KB_DOWN)
                            {
                                temprecordno--;
                                if(temprecordno==0)
                                    temprecordno=noofrecords;
                            }
                            if(KBV==KB_CANCEL)
                                return;
                        }
                        else
                            break;
                        
                        
                    }while(KBV!=KB_ENT && KBV!=KB_CANCEL );                           
                }
                if(CollectionRecord.FAT==0xFF || CollectionRecord.FAT==0 || noofrecords==0)
                {
                    Collection_Not_Found_Msg();//1

                    Wait_Key_Press('C');

                    c_step=1;
                }
                else
                    c_step=3;
               break;

            case 3:
               
                if(receipttype=='S')
                {
#ifdef GPRS
                   
                    STRCPY(LBUF,"Sending Message...");//1
                    Invert_Disp_Msg(LINE8);

                    do
                    {
                        Send_SMS_BackEnd('D',c_member_sno);
                     }while(SMS_STATE!=0);

                     Msg_Delivery_Ack();
                     Delay_250Ms(2);
#endif
                        
                }

                if(receipttype=='V')
                {
                    Clear_LCD();
                    Tablerec.MILK_TYPE=CollectionFlags.MILK_TYPE;
                    Display_Bill_Details(2,0);

                    KBV=Wait_Key_Press('C');

                    if(KBV==KB_CANCEL)
                        return;
                }
                else
                {
                    Please_Wait_Msg_Small(LINE6);
                    if(receipttype=='T')
                        Print_Receipt('D');

                    else if(receipttype=='E')
                        Ext_Print_Receipt('D');

                    Delay_Sec(1);
                } 
                
                c_step=1;

                break;
        }
    }
}

void Update_FAT_FLASH(void)
{
    Display_Boarder3();

    STRCPY(LBUF," Updating Rate Charts");
    Disp_Msg(LINE4);

    Please_Wait_Msg_Small(LINE6);
    
    Temp_Data_Saving(Cow_FatTable_Loc);

    STRCPY(LBUF,"  Updating Done....");
    Disp_Msg(LINE6);
}

void Calculate_Rate(UINT8 usesnf_status)
{
    UINT8 r_step,low_flag=0;
    UINT16 snf_clr;
    
    if(CLR_BASED=='Y')
        snf_clr = CollectionRecord.CLR1;
    else
        snf_clr = CollectionRecord.SNF;
    
    
    if(usesnf_status!='S')
    {
        LowFatFlag=Settingsrec.LOW_FAT_SNF;
        HighFatFlag=Settingsrec.HIGH_FAT_SNF;
    }
    else
    {
        LowFatFlag='Y';
        HighFatFlag='Y';
    }

    Fat_Reject_Flag=0;
    Snf_Reject_Flag=0;
   
    if(usesnf_status=='Y' || usesnf_status=='S')
        r_step=0;

    if(usesnf_status=='N')
        r_step=1;
    
    if(r_step==0)
    {
        if(Settingsrec.MIXED_MILK=='Y')
        {
            Read_SNF_Limits('C');
            CowSNFLimitsrec=SNFLimitsrec;
            
            Read_SNF_Limits('B');
            BufSNFLimitsrec=SNFLimitsrec;
            
            if(CollectionRecord.FAT<BufSNFLimitsrec.MIN_FAT || CollectionRecord.FAT<=CowSNFLimitsrec.MAX_FAT)
            {
                Tablerec.MILK_TYPE='C';
                SNFLimitsrec=CowSNFLimitsrec;
            }
            else 
            {
                Tablerec.MILK_TYPE='B';
                SNFLimitsrec=BufSNFLimitsrec;
            }
        }
        else
        {
            Read_SNF_Limits(Tablerec.MILK_TYPE);
        }
        if(CollectionRecord.FAT>=SNFLimitsrec.MIN_FAT && CollectionRecord.FAT<=SNFLimitsrec.MAX_FAT)
            Tablerec.MILK_FAT=CollectionRecord.FAT;
        else if(CollectionRecord.FAT>SNFLimitsrec.MAX_FAT)
        {
            if(HighFatFlag=='N')
            {
                Tablerec.MILK_FAT=CollectionRecord.FAT;
                Fat_Reject_Flag='L';
            }
            else
            {
                Tablerec.MILK_FAT=SNFLimitsrec.MAX_FAT;
                //high_flag=1;
            }
        }
        else
        {
            if(LowFatFlag=='N')
            {
                Tablerec.MILK_FAT=CollectionRecord.FAT;
                Fat_Reject_Flag='L';
            }
            else
            {
                Tablerec.MILK_FAT=SNFLimitsrec.MIN_FAT;
                low_flag=1;
            }
        }

        //if(CollectionRecord.SNF>=SNFLimitsrec.MIN_SNF && CollectionRecord.SNF<=SNFLimitsrec.MAX_SNF)
        if(snf_clr>=SNFLimitsrec.MIN_SNF && snf_clr<=SNFLimitsrec.MAX_SNF)
        {
            Tablerec.MILK_SNF=snf_clr;//CollectionRecord.SNF;
        }
        else if(snf_clr<SNFLimitsrec.MIN_SNF)
        {
            if(LowFatFlag=='N')
            {
                Tablerec.MILK_SNF=snf_clr;//CollectionRecord.SNF;
                Snf_Reject_Flag='L';
            }
            else
            {
                Tablerec.MILK_SNF=SNFLimitsrec.MIN_SNF;
                low_flag=1;
            }
        }
        else if(snf_clr>SNFLimitsrec.MAX_SNF)
        {
            if(HighFatFlag=='N')
            {
                Tablerec.MILK_SNF=snf_clr;//CollectionRecord.SNF;
                Snf_Reject_Flag='L';
            }
            else
            {
                Tablerec.MILK_SNF=SNFLimitsrec.MAX_SNF;
                //high_flag=1;
            }
        }
        
        if(low_flag==1 && Settingsrec.MIXED_MILK!='Y') // fat or snf less than lower limit
        {
            Tablerec.MILK_FAT=SNFLimitsrec.MIN_FAT;
            Tablerec.MILK_SNF=SNFLimitsrec.MIN_SNF;
        }
        
        Tablerec.MILK_FAT=Tablerec.MILK_FAT-SNFLimitsrec.MIN_FAT;
        Tablerec.MILK_SNF=Tablerec.MILK_SNF-SNFLimitsrec.MIN_SNF;
       
        if(Fat_Reject_Flag==0 && Snf_Reject_Flag==0)
            CollectionRecord.RATE=Read_SNF_MilkTable();
        else
            CollectionRecord.RATE=0;

    }
    else
    {
        if(Settingsrec.MIXED_MILK=='Y')
        {
            Read_FAT_Limits('C');
            CowFatLimitsrec=Fatlimitsrec;
            Read_FAT_Limits('B');
            BufFatLimitsrec=Fatlimitsrec;
            if(CollectionRecord.FAT<BufFatLimitsrec.MIN_FAT || CollectionRecord.FAT<=CowFatLimitsrec.MAX_FAT)
            {
                Tablerec.MILK_TYPE='C';
                Fatlimitsrec=CowFatLimitsrec;
            }
            else 
            {
                Tablerec.MILK_TYPE='B';
                Fatlimitsrec=BufFatLimitsrec;
            }
        }
        else
        {
        
            Read_FAT_Limits(Tablerec.MILK_TYPE);
        }
        if(CollectionRecord.FAT>=Fatlimitsrec.MIN_FAT && CollectionRecord.FAT<=Fatlimitsrec.MAX_FAT)
            Tablerec.MILK_FAT=CollectionRecord.FAT;
        else if(CollectionRecord.FAT>Fatlimitsrec.MAX_FAT)
        {
            if(HighFatFlag=='N')
            {
                Tablerec.MILK_FAT=CollectionRecord.FAT;
                Fat_Reject_Flag='L';
            }
            else
            {
                Tablerec.MILK_FAT=Fatlimitsrec.MAX_FAT;                
            }
        }
        else
        {
            if(LowFatFlag=='N')
            {
                Tablerec.MILK_FAT=CollectionRecord.FAT;
                    Fat_Reject_Flag='L';
            }
            else
            {
                Tablerec.MILK_FAT=Fatlimitsrec.MIN_FAT;
                low_flag=1;
            }
        }
        
        if(low_flag==1 && Settingsrec.MIXED_MILK!='Y')
            Tablerec.MILK_FAT=Fatlimitsrec.MIN_FAT;
        
        Tablerec.MILK_FAT=Tablerec.MILK_FAT-Fatlimitsrec.MIN_FAT;

        if(Fat_Reject_Flag==0)
            Read_FAT_MilkTable(Tablerec.MILK_TYPE,Tablerec.MILK_FAT);
        else
            Milkrec.RATE=0;

        CollectionRecord.RATE=Milkrec.RATE;        
    }
    
    if(CollectionRecord.RATE==0xFFFF)
        CollectionRecord.RATE=0;

    if(CollectionRecord.QTY==0xFFFF)
        CollectionRecord.QTY=0;
}

void Display_Code_Name(UINT8 line)
{
    ZERO=1;
    if(line==LINE3)
        Display_Boarder3();

    Hex_DEC(0,Memberrec.C_CODE,LBUF,4);
    LBUF[4]='(';   
    LBUF[5]=Tablerec.MILK_TYPE;
    LBUF[6]=')';
    LBUF[7]=':';

    STRNCPY(LBUF+8,Memberrec.NAME,12);
    Shift_Left(LBUF,20);
    if(line==LINE2)
    {
        Invert_Disp_Msg(LINE2);
    }
    else
        Disp_Msg(line);
}

void Display_Bill_Details(UINT8 dcollectiontype,UINT8 onlyweight)
{
    UINT8 empty_cursor=0;
    UINT16 snf_clr=0;
    ZERO=1;
    
    if(dcollectiontype=='C' &&  onlyweight=='C')
    {
        empty_cursor=1;
    }
    if(onlyweight=='C')
        onlyweight=0;
    
    TIME_FLAG=0;
    if(onlyweight==0 || onlyweight=='R')
    {
        if(dcollectiontype=='C')
        {
            Menu_Header('Z',0);            
        }
        else
        {
            Display_Full_Boarder();
            Display_Shift_SampleNo();
        }
 
        Display_Code_Name(LINE2);

        Display_Box_Top_Boarder(LINE3,0x18);
        Display_Box_Top_Boarder(LINE6,0x18);       
        
    }
    if(CollectionRecord.QTY>0 && onlyweight==1)
    {
        
        STRCPY(LBUF,"QUANTITY:      ");
        Hex_DEC(2,CollectionRecord.QTY,LBUF+9,6);
        Disp_Msg(LINE4);
        return;

    }  
   
    if(onlyweight==0 || onlyweight=='R')
    {
        if(onlyweight==0)
        {

            Display_Bar_Middle_Down(LINE3+5);
            Display_Bar_Middle_Down(LINE3+12);
            
            Display_Bar_Middle_Up(LINE6+5);
            Display_Bar_Middle_Up(LINE6+12);
        
            STRCPY(LBUF,"FAT  | SNF  |  WATER");  
            CLR_Enabled(LBUF+7);    
            Disp_Msg(LINE4);
            LBUF[5]=LBUF[12]=0x9B;
            
            if(CollectionRecord.FAT==0xFF)
                CollectionRecord.FAT=0;
            
            if(CollectionRecord.SNF==0xFF)
                CollectionRecord.SNF=0;
            
            if(CollectionRecord.CLR1==0xFFFF)
                CollectionRecord.CLR1=0;
            
            if(CollectionRecord.RATE==0xFFFF)
                CollectionRecord.RATE=0;
            
            if(CollectionRecord.FAT>0)
            {
                Hex_DEC(1,CollectionRecord.FAT,LBUF,4);
                Shift_Left(LBUF,4);
            }
            
            snf_clr = CLR_BASED=='Y' ? CollectionRecord.CLR1 : CollectionRecord.SNF;
            
            if(snf_clr>0)
            {
                Hex_DEC(1,snf_clr,LBUF+8,4);                
                Shift_Left(LBUF+8,4);
            }

            if(onlyweight==0)
            {
                if( CollectionRecord.WATER1 > 0 && CollectionRecord.WATER1 !=0xFFFF)
                    Hex_DEC(1,CollectionRecord.WATER1,LBUF+15,4);
            }       
            
            //Display_Box_Top_Boarder(LINE6,0x18);
        }
        else
        {
            Display_Bar_Middle_Down(LINE3+4);
            Display_Bar_Middle_Down(LINE3+9);
            Display_Bar_Middle_Down(LINE3+14);
            
            Display_Bar_Middle_Up(LINE6+4);
            Display_Bar_Middle_Up(LINE6+9);
            Display_Bar_Middle_Up(LINE6+14);
            
            snf_clr = CLR_BASED=='Y' ? CollectionRecord.CLR1 : CollectionRecord.SNF;
                        
            STRCPY(LBUF,"FAT |SNF |WTR |RATE ");
            CLR_Enabled(LBUF+5);    
            Disp_Msg(LINE4);
            
            LBUF[4]=LBUF[9]=LBUF[14]=0x9B;
                        
            if(CollectionRecord.FAT>0)
            {
                Hex_DEC(1,CollectionRecord.FAT,LBUF,4);
                Shift_Left(LBUF,4);
            }
            if(snf_clr>0)
            {
                Hex_DEC(1,snf_clr,LBUF+5,4);
                Shift_Left(LBUF+5,4);
            }
            
            if( CollectionRecord.WATER1 > 0 && CollectionRecord.WATER1 !=0xFF)
            {
                Hex_DEC(1,CollectionRecord.WATER1,LBUF+10,4);
                Shift_Left(LBUF+10,4);
            }
            
            Hex_DEC(2,CollectionRecord.RATE,LBUF+15,5);
            if(CollectionRecord.RATE>9999)
                Hex_DEC(2,CollectionRecord.RATE,LBUF+15,6);
        }
        
        Invert_Disp_Msg(LINE5);
    }
    
    Calculate_Amount();
    
    if(onlyweight==0)
    {
        if(CollectionRecord.FAT>0)
        {
            if(CollectionRecord.QTY>0)
            {
                if(dcollectiontype=='C' || dcollectiontype=='E')
                {
                    if(dcollectiontype=='C')
                    {
                        if(WEIGHT_COUNT==0)
                            SEERAMReadString(Normal_Commission_Loc,20,(char *)&Incentiverec);
                        else
                            SEERAMReadString(Running_Shift_Incentive_Loc,20,(char *)&Incentiverec);
                        CollectionRecord.INCENTIVE_AMT=Read_Incentive_Comm(0,CollectionRecord);
                    }
                    else
                        CollectionRecord.INCENTIVE_AMT=Read_Incentive_Comm(1,CollectionRecord);
                }
                if(CollectionRecord.INCENTIVE_AMT>0)
                {
                    CLEAR_DATA_LINES_FROM(6,6);
                    STRCPY(LBUF,"   Incentive:");

                    Hex_DEC(2,CollectionRecord.INCENTIVE_AMT,LBUF+13,7);
                    Disp_Msg(LINE6);
                }               
            }
            
            STRCPY(LBUF,"RATE |QTY(L)| AMOUNT");
            
            if(WEIGHT_KG=='Y')
            {
                LBUF[10]='K';
            }
            Disp_Msg(LINE7);

            ZERO=1;
            
            Hex_DEC(2,CollectionRecord.RATE,LBUF,5);
            Shift_Left(LBUF,5);
            
            if(CollectionRecord.RATE>9999)
                Hex_DEC(2,CollectionRecord.RATE,LBUF,6);

            ZERO=1;
          
            if(empty_cursor==0)
            {
                Hex_DEC(2,CollectionRecord.QTY,LBUF+6,6);
                Shift_Left(LBUF+6,6);
            }
          
           Hex_DEC(2,Amount,LBUF+13,7);

            if(Amount>999999)
                Hex_DEC(2,Amount,LBUF+13,8);           

            Invert_Disp_Msg(LINE8);
            Display_Bar_LF(LINE8+5);
            Display_Bar_LF(LINE8+12);
            
            Display_Bar_Middle_Plus(LINE6+5);
            Display_Bar_Middle_Plus(LINE6+12);
            
        }
        else
        {
            Display_Box_Boarder(LINE7);

        }
    }

    Send_Collection_Ext_Display();
}


UINT8 Close_Shift(UINT8 cstype)
{
    UINT8 shift_flag,tempbflag;
    
    SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&RunningShift);
    
    Display_Screen_Boarder();
   
    if(cstype==0)
    {
        STRCPY(LBUF,"  *SHIFT CLOSE*");
        Disp_Header_No_Boarder();  
        
        TempShift=Shiftrec;
        Shiftrec=RunningShift;
        Display_Shift(LINE4);
        Shiftrec=TempShift;
        
        Buzzer(250);
        Buzzer(250);
        
        STRCPY(LBUF,"Do U Want To Close?");
    }
    else if(cstype==1)
    {
        STRCPY(LBUF,"*TRANSFER SHIFT*");
        Disp_Header_No_Boarder(); 
        
        STRCPY(LBUF,"Data to Server?");
    }
    
    if(cstype==2 || Confirm_Yes()==1)
    {
        Display_Boarder3();
        
        if(RunningShift.YEAR==0xFF || RunningShift.MONTH==0xFF || RunningShift.DAY==0xFF || RunningShift.DAY_SHIFT==0xFF)
        {
            STRCPY(LBUF,"No Shift Pending...");
            Disp_Msg(LINE5);

            Buzzer(250);
            Buzzer(250);
            return 0;
        }
        TempShift=Shiftrec;
        Shiftrec=RunningShift;
        shift_flag=Read_Day_Flag('S');
        Shiftrec=TempShift;
        
        if(shift_flag==1 && cstype==0 && cstype!=2)
        {
            STRCPY(LBUF,"Already CLOSED");
            Disp_Msg_No_Spaces(0,LINE5+1);

            Buzzer(250);
            Buzzer(250);
            return 0;

        }
        SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&RunningShift);
        #ifndef anil
        if((RunningShift.YEAR!=Shiftrec.YEAR || RunningShift.MONTH!=Shiftrec.MONTH || RunningShift.DAY!=Shiftrec.DAY || RunningShift.DAY_SHIFT!=Shiftrec.DAY_SHIFT) || cstype==1)
        #endif
        {
            TempShift=Shiftrec;
            Shiftrec=RunningShift;
            if(cstype!=2 &&  ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y')
            {
                if(Modem_Init_Flag==1 && SIM_INSERTED==1 )
                {
                    tempbflag=BACKEND_FLAG;
                    MODEM_ERROR=Transfer_Shift_Data('D');
                    Fetch_Customer_Flag=0;
                    Server_Messages();
                    

                    if(MODEM_ERROR>=0x20)
                        Disconnect_Modem();
                    BACKEND_FLAG=tempbflag;
                }
                else if(Modem_Init_Flag==0 )
                {
                    STRCPY(LBUF,"MODEM NOT CONNECTED");
                    Disp_Msg(LINE5);
                }
                else if(SIM_INSERTED==0 )
                {
                    STRCPY(LBUF,"  SIM NOT INSERTED");
                    Disp_Msg(LINE5);
                }
            }
           
            if(shift_flag!=1 && (cstype==0 || cstype==2))
            {
                ZERO=0;
                Display_Boarder3();
                
                if(WEIGHT_COUNT==ANALYZER_COUNT)
                {
                    TempShift=Shiftrec;
                    Shiftrec=RunningShift;
                    Summary_Report('S','p','C');
                    Shiftrec=TempShift;
                    Reset_Tra_Counters();
                    Display_Shift(LINE3);
                    STRCPY(LBUF," SHIFT CLOSED ");
                    Disp_Msg_No_Spaces(0,LINE5+1);
                }
                else
                {   
                    Samples_Pending_Msg();
                    return 0xFF;
                }
                
            }
            Shiftrec=TempShift;
            Check_New_RateChart();
        }
        #ifndef anil
            else if(cstype==0)
            {
                Display_Shift(LINE3);
                STRCPY(LBUF," RUNNING SHIFT");
                Disp_Msg_No_Spaces(0,LINE5+1);
            }
        #endif
        
        if(cstype!=2)
        {
            Exit_Any_Key(1);
            return 1;
            
        }
    }
    else
        return 0xFF;
}

UINT8 Init_Collection(UINT8 collectiontype,UINT8 atype)
{
    UINT8 shift_flag,day_flag,close_status;

    Read_Settings();
    SEERAMReadString(Weight_Kg_Loc,1,(char *)&WEIGHT_KG);
    SEERAMReadString(Multiple_Samples_Loc,1,(char *)&MULTIPLE_SAMPLES); 
    SEERAMReadString(Multiple_Shifts_Loc,1,(char *)&MULTIPLE_SHIFTS); 
    
    SEERAMReadString(MilkType_Print_On_Loc,RECEIPT_RECORD_LENGTH,(char *)&Receiptrec);
    
    if(MULTIPLE_SAMPLES=='N')
    {                 
        MILK_SELECTION='N';
         
        
    }
    
    NAME_SEARCH=0;
    
#ifdef NAME_PH_SEARCH1
    SEERAMReadString(Searching_Option_Loc,1,(char *)&NAME_SEARCH);
    
    if(NAME_SEARCH!='N')
    {
    
        if(NAME_SEARCH<0 || NAME_SEARCH>2)
        {
            NAME_SEARCH=0;
            SEERAMWriteString(Searching_Option_Loc,1,(char *)&NAME_SEARCH);
        }
    }
#endif
    
    if(Settingsrec.FAT_MODE>1)
    {
        Collection_Header_Msg();
        
        STRCPY(LBUF,"PLEASE SELECT MODE");
        Disp_Msg(LINE4);

        Delay_Sec(1);

        return 0xFF;
    }
    
    ZERO=0;
    
    if(collectiontype=='C')
    {
		if(Check_Shift_Timings()==0)
		{
            Collection_Header_Msg();
            
			STRCPY(LBUF,"SHIFT TIMINGS ");
			Disp_Msg_No_Spaces(0,LINE3+1);

            Morning_Shift_Timings(LINE5);
            Evening_Shift_Timings(LINE6);

            Exit_Any_Key(1);
			return 0xFF;
		}
        
        Init_Weighing_Machine();
	}
    
    Collection_Header_Msg();
    Please_Wait_Msg(LINE5);
    
    shift_flag=Read_Day_Flag('S');
        
   
   // if(shift_flag==1 && collectiontype=='C')////version1.18
    if(shift_flag==1 && (collectiontype=='C'  || collectiontype=='S'))
    {
        if(collectiontype=='S')
            return 0xFF;
        Collection_Header_Msg();
    
        if(MULTIPLE_SHIFTS=='N')
        {
            Collection_Header_Msg();
            Display_Shift(LINE3);
            STRCPY(LBUF,"SHIFT CLOSED..");
            Disp_Msg_No_Spaces(0,LINE5+1);
            Long_Beep();
            return 0xFF;
        }
        
        
        else if(MULTIPLE_SHIFTS=='Y')
        {
        
            Display_Shift(LINE3);

            STRCPY(LBUF,"Shift Data Available");
            Disp_Msg(LINE5);

            STRCPY(LBUF,"U Want To Continue?");
            Disp_Msg(LINE6);

            Enter_Exit_Msg();
            while(1)
            {
                Buzzer(250);
                KBV=Wait_Key_Press('N');

                 if(KBV==KB_ENT)
                    break;
                if(KBV==KB_CANCEL)
                    return 0xFF;
            }        
        }
        else
        {
            MULTIPLE_SHIFTS='N';
            SEERAMWriteString(Multiple_Shifts_Loc,1,(char *)&MULTIPLE_SHIFTS); 
        
            return 0xFF;
        }
    }
     
    //version1.07
    
    if(collectiontype=='E')
        return 0x01;
    else if(collectiontype=='C' || collectiontype=='S')
    {
        SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&RunningShift);
        
        TempShift=Shiftrec;
        Shiftrec=RunningShift;
        day_flag=Read_Day_Flag('S');
        Shiftrec=TempShift;
        
        if(Shiftrec.YEAR==0xFF || Shiftrec.MONTH==0xFF || Shiftrec.DAY==0xFF || Shiftrec.DAY_SHIFT==0xFF)
        {
            STRCPY(LBUF,"RTC Error,Restart");
            Disp_Msg(LINE4);
            while(1);

        }
        if(RunningShift.YEAR==0xFF && RunningShift.MONTH==0xFF && RunningShift.DAY==0xFF && RunningShift.DAY_SHIFT==0xFF)
        {
            RunningShift=Shiftrec;
            Reset_Tra_Counters();
        }
        else if(RunningShift.YEAR==0xFF || RunningShift.MONTH==0xFF || RunningShift.DAY==0xFF || RunningShift.DAY_SHIFT==0xFF)
        {
            STRCPY(LBUF,"RTC Error,Restart");
            Disp_Msg(LINE4);
            while(1);
        }
        
       if((RunningShift.YEAR!=Shiftrec.YEAR || RunningShift.MONTH!=Shiftrec.MONTH || RunningShift.DAY!=Shiftrec.DAY || RunningShift.DAY_SHIFT!=Shiftrec.DAY_SHIFT && collectiontype=='C') || collectiontype=='S' )
        {
            if(day_flag!=1)
            {
                if(Settingsrec.SHIFT_CLOSE=='N' && WEIGHT_COUNT>0)
                { 
                    if(collectiontype=='S')
                    {
                        close_status=Close_Shift(2);
                        if(close_status==0xFF)
                            return close_status;
                    }
                    else
                    {
                        close_status=Close_Shift(0);
                    
                        if(close_status!=0x01)
                            return close_status;
                    }

                }
                else if(Settingsrec.SHIFT_CLOSE=='Y' ) 
                {
                    close_status=Close_Shift(2);
                    if(close_status==0xFF)
                        return close_status;
                }
            }
        }
    }
    
    ANALYZER_FLAG=0;
    WM_FLAG=0;
    Analyzer_Header_Display_Flag=0;
    
    Read_Date_Month_Year();
    RunningShift=Shiftrec;
    
    if(WEIGHT_COUNT>0)
    {
        //SEERAMReadString(Running_Shift_Incentive_Loc,4,(char *)&Incentiverec); 
        SEERAMReadString(Running_Shift_Incentive_Loc,20,(char *)&Incentiverec);
    }
    
    Display_Counters();
   
    if(atype==2)
    {
        ANALYZER_FLAG2=0;

        Update_Analyzer_Flag=0;
        Update_Analyzer_Flag2=0;

        GPRSDELAYCOUNT=0;
        SMS_STATE=0;
        
        Ena_GPRS_Interrupt();
    }
    if(WEIGHT_COUNT>999)//1
    {
        Collection_Header_Msg();
        
        Max_Samples_Msg();
       
        return 0xFF;
    }
    
    return 0x01;
}

void Init_Collection_Variables(void)
{
    CollectionRecord.FAT=0;
    CollectionRecord.SNF=0;
    CollectionRecord.QTY=0xFFFF;
    CollectionRecord.RATE=0;
    CollectionRecord.WATER1=0;//CollectionRecord.WATER=0;
    CollectionRecord.CLR1=0;//CollectionRecord.CLR=0;
    
    CollectionRecord.FLAGS=0xFF;
    
    CollectionFlags.MILK_TYPE=Memberrec.MILK_TYPE;
    if(Memberrec.MILK_TYPE=='B')
        CollectionRecord.FLAGS=0xFE;   
}

UINT8 Select_Col_Milk_Type(UINT8 displine8)
{
    UINT8 select_count=1;
    
    if(displine8==1)
        Disp_Up_Down_Select();
    
    if(Memberrec.MILK_TYPE=='B')
        select_count=0;
    
    while(1)
    {
        LBUF[15]='B'+select_count;
        if(select_count==0)
        {
            LBUF[16]='U';LBUF[17]='F';
        }

        if(select_count==1)
        {
            LBUF[16]='O';LBUF[17]='W';
        }

        INVERT_DISPLAY=1;

        LCD_Set_Cursor('S',LINE3+15);
        Display_Char(LBUF[15]);
        
        
        
        LCD_Set_Cursor('S',LINE3+16);
        Display_Char(LBUF[16]);
        
        LCD_Set_Cursor('S',LINE3+17);
        Display_Char(LBUF[17]);

        INVERT_DISPLAY=1;
        
        CURSOR_BLINK=0x80;
        CURSOR_FLAG=1;
        while(1)
        {
            if(DISP_CURSOR_FLAG==1)
            {
                DISP_CURSOR_FLAG=0;
                LBUF[15]='B'+select_count;
                Disp_Cursor_Blink('S',LINE3,15,LBUF[15]);
            }
            KBV=Scan_Key('C');//Wait_Key_Press('C');
            
            if(KBV!=0xFF)
            {
                CURSOR_FLAG=0;
                CURSOR_BLINK=0x00;
                DISP_CURSOR_FLAG=0;
                break;
            }
        }
        switch(KBV)
        {                    
            case KB_CANCEL:

                return 0xFF;

            case KB_LEFT:

                return 'L';

            case KB_ENT:

                return ('B'+select_count);

        }
        select_count=Up_Down(select_count,2);
    }
}

UINT8 Select_Col_Yes_No(UINT8 dpos)
{
    UINT8 select_count=1;
    
    while(1)
    {
        if(select_count==0)
        {
            LBUF[dpos]='N';LBUF[dpos+1]='O';LBUF[dpos+2]=' ';
        }

        if(select_count==1)
        {
            LBUF[dpos]='Y';LBUF[dpos+1]='E';LBUF[dpos+2]='S';
        }
        
        INVERT_DISPLAY=1;
        LCD_Set_Cursor('S',LINE4+dpos);
        Display_Char(LBUF[dpos]);
        
        LCD_Set_Cursor('S',LINE4+dpos+1);
        Display_Char(LBUF[dpos+1]);
        
        LCD_Set_Cursor('S',LINE4+dpos+2);
        Display_Char(LBUF[dpos+2]);
        
        INVERT_DISPLAY=1;

        KBV=Wait_Key_Press('C');

        switch(KBV)
        {
            case KB_CANCEL:

                return 0xFF;

            case KB_LEFT:

                return 'L';

            case KB_ENT:

                if(select_count==0)
                    return 'N';

                if(select_count==1)
                    return 'Y';

        }
        select_count=Up_Down(select_count,2);
    }
}

void Display_Col_Code_Milk(UINT8 coltype,UINT16 dcode,UINT8 dsamples)
{
    if(WEIGHT_COUNT==0)
        Display_Shift(LINE1); 
    else
        Display_Counters();
    
    CLEAR_DATA_LINES_FROM(2,4);
    if(coltype==2)
    {
        STRCPY(LBUF,"S.NO CODE M.T QTY-Lt");
        
        if(WEIGHT_KG=='Y')
        {
            LBUF[18]='K';
            LBUF[19]='g';
        }
    }
    else
    {
        STRCPY(LBUF,"S.NO   CODE    MILK");        
    }
  
    if(coltype==2)
        LBUF[4]=LBUF[9]=LBUF[13]=0x90;
    else
        LBUF[4]=LBUF[13]=0x90;
    Invert_Disp_Msg(LINE2);

    ZERO=1;
    if(coltype==2)
    {
        Hex_DEC(0,WEIGHT_COUNT+1,LBUF,4);
        Shift_Left(LBUF,4);
        
        if(dcode>0)
            Hex_DEC(0,CollectionRecord.CODE,LBUF+5,4);
        
        if(dsamples>0)
        {
            LBUF[10]=CollectionFlags.MILK_TYPE;

            if(CollectionFlags.MILK_TYPE=='C')
            {
                LBUF[11]='O';
                LBUF[12]='W';
            }
            else
            {
                LBUF[11]='U';
                LBUF[12]='F';
            }
            LBUF[13]=' ';
        }
        if(dcode>0 && dsamples)
            Hex_DEC(2,CollectionRecord.QTY,LBUF+14,6);
       
    }
    else
    {
        if(coltype!=2)
        {
            Hex_DEC(0,WEIGHT_COUNT+1,LBUF,4);
            Shift_Left(LBUF,4);
        }
        if(dcode>0 || dsamples==101)
        {
            Hex_DEC(0,CollectionRecord.CODE,LBUF+7,4);
            Shift_Left(LBUF+7,4);
        }
        
        if(dsamples<100 && dsamples>0)
        {
            Hex_DEC(0,CollectionRecord.CODE,LBUF+5,4);
            LBUF[9]='(';
            Hex_DEC(0,dsamples,LBUF+10,2);
            LBUF[12]=')';
            Shift_Left(LBUF+10,3);
        }
        if(dsamples>0)
        {
            LBUF[15]=CollectionFlags.MILK_TYPE;

            if(CollectionFlags.MILK_TYPE=='C')
            {
                LBUF[16]='O';
                LBUF[17]='W';
            }
            else
            {
                LBUF[16]='U';
                LBUF[17]='F';
            }
        }
    }
    Disp_Msg(LINE3);
}

void Display_Col_Qty(UINT8 qline,UINT16 addqty)
{
    UINT16 temp_qty;
    STRCPY(LBUF,"Qty(Lt):");
    //LBUF[7]=0x90;
    if(WEIGHT_KG=='Y')
    {
        LBUF[4]='K';LBUF[5]='g';
    }
    ZERO=1;
    if(CollectionRecord.QTY!=0xFFFF)
    {
        Hex_DEC(2,CollectionRecord.QTY,LBUF+8,6);
        Shift_Left(LBUF+8,6);
    }
    Disp_Msg(qline);
    temp_qty=CollectionRecord.QTY;
    CollectionRecord.QTY=CollectionRecord.QTY+addqty;
    Send_Collection_Ext_Display();
    CollectionRecord.QTY=temp_qty;
}

UINT8 Auto_Milk_Qty1(UINT8 ctype,UINT8 astep,UINT8 dline,UINT32 addqty)
{
    UINT32 milk_qty;

    Weight=0;
    milk_qty=0;
    
    Display_Col_Qty(dline,addqty);
        
    if(addqty>0)
    {
        STRCPY(LBUF,"Prev. Qty:");    
        ZERO=1;
        Hex_DEC(2,addqty,LBUF+10,7);
        Shift_Left(LBUF+10,7);
        ZERO=0;
        if(ctype==2)
            Disp_Msg(LINE5); 
        else
            Disp_Msg(LINE8);
    }
    
    do
    {
        if(WM_FLAG==1 && milk_qty!=Weight && Weight<65001)
        {
            Buzzer(50);
            milk_qty=Weight;

            CollectionRecord.QTY=milk_qty;

            Display_Col_Qty(dline,addqty);            

            INVERT_DISPLAY=1;
            
            Set_Cursor(dline+15);
            Display_Char('+');
            
            Set_Cursor(dline+16);
            Display_Char('/');

            Set_Cursor(dline+17);
            Display_Char('E');
            
            Set_Cursor(dline+18);
            Display_Char('N');

            Set_Cursor(dline+19);
            Display_Char('T');
            
            INVERT_DISPLAY=0;       
        }
        
        LCD_Set_Cursor('S',LINE8+6);
        KBV=Scan_Key('C');
        
        if(PS2_KEYBOARD==0 && KBV=='.')
        {
            KBV='+';
        }
        
        if(KBV==KB_LEFT)
        {
            if(LEFT_ARROW_EDIT=='N' || LOGIN_TYPE=='O')
                KBV=0xFF;
        }

    }while(KBV!='+' && KBV!=KB_CANCEL && KBV!=KB_ENT && KBV!=KB_LEFT);

    if(KBV==KB_CANCEL)
    {
        return 0xFF;
    }

    if(KBV==KB_LEFT)
    {
        CollectionRecord.QTY=0;
        return (astep-1);
    }

    if(milk_qty>0)
    {
        if((milk_qty+addqty)<65000 && (KBV==KB_ENT || KBV=='+'))
        {
            Buzzer(50);
            CollectionRecord.QTY=milk_qty;           
            WM_FLAG=0;
            CLEAR_BUF_FLAG=0;
            
            if(KBV=='+')
                return '+';
            else
                return (astep+1);
        }
        else
        {
            STRCPY(LBUF," CAPACITY:650 Lts");
            if(ctype==2)
            {
                Disp_Msg(LINE4);

                Delay_Sec(1);
                CLEAR_DATA_LINES_FROM(4,4);
            }
            else
            {
                Invert_Disp_Msg(LINE8);

                Delay_Sec(1);
                CLEAR_DATA_LINES_FROM(8,8);
            }
            return astep;
        }
    }   
    else
        return astep;
}

UINT8 Confirm_Add_Save_Weight(UINT8 cstep,UINT8 ctype,UINT32 addqty)
{
    UINT8 line;
    
    if( CollectionRecord.QTY==0xFFFF)
        return (cstep-1);

    INVERT_DISPLAY=1;
    
    if(ctype==2 || Settingsrec.WEIGHT_MODE==1)
    {
        line=LINE7;
        if(ctype==2)
            line=LINE4;
    
        Set_Cursor(line+15);
        Display_Char(' ');
        Set_Cursor(line+16);
        Display_Char('S');

        Set_Cursor(line+17);
        Display_Char('a');

        Set_Cursor(line+18);
        Display_Char('v');

        Set_Cursor(line+19);
        Display_Char('e');
    }

    while(1)
    {  
        KBV=Scan_Key('E');

        if(LEFT_ARROW_EDIT=='Y' && LOGIN_TYPE!='O')          //gopal reddy
        {
            if(KBV==KB_LEFT)
                return 'L';
        }
    
        if(KBV==KB_CANCEL )
            return 0xFF;
        
        if(KBV==KB_ENT)
            return 'S';
        
        if(KBV==0x20)//space bar
            return 'N';
    }
               
}

void Display_Manual_Mode_Msg(void)
{
    Display_Box_Top_Boarder(LINE6,0x18);
    Display_Box_Boarder(LINE7);
    STRCPY(LBUF,"MANUAL");  
    NOR_DISPLAY_LAST_LINE=0xC0;
    Disp_Msg_No_Spaces(0,LINE7+1);
    NOR_DISPLAY_LAST_LINE=0x00;
}

UINT8 Milk_Fat_Entry(UINT8 fstep,UINT8 line)
{
    UINT8 tcol_buf[10];
    UINT16 m_fat;
    
    Display_Manual_Mode_Msg();
    ZERO=1;
    Hex_DEC(1,CollectionRecord.FAT,tcol_buf,4);
    Shift_Left(tcol_buf,4);
    tcol_buf[4]=0x00;   

    INVERT_DISPLAY=1;
    
    GET_VALUE_STATUS=Get_Numeric(4,1,line,(char *)&tcol_buf);
    INVERT_DISPLAY=0;

    if(KBV==KB_LEFT)
        return (fstep-1);
    
    else if(GET_VALUE_STATUS==0xFF)
    {
        return 0xFF;
    }
    else if(GET_VALUE_STATUS!=0xFF)
    {
        m_fat=Float_Long(tcol_buf);

        if(m_fat>=240 || m_fat==0)
        {
             CollectionRecord.FAT=0;
             return fstep;            
        }
        else
        {
            CollectionRecord.FAT=m_fat;
            return (fstep+1);
        }
    }
}

UINT8 Milk_Qty_Entry(UINT8 qstep,UINT8 line)
{
    BYTE tcol_buf[10];
    UINT32 milk_qty;
    
    ZERO=1;
    
    Hex_DEC(2,CollectionRecord.QTY,tcol_buf,6);
    Shift_Left(tcol_buf,6);
    tcol_buf[6]=0x00;
    
    if(line==LINE8+6)
    {
        INVERT_DISPLAY=1;
    }
    
    GET_VALUE_STATUS=Get_Numeric(6,2,line,(char *)&tcol_buf);
    INVERT_DISPLAY=0;
    ZERO=0;
    if(KBV==KB_LEFT)
        return (qstep-1);

    else if(GET_VALUE_STATUS==0xFF)
    {
        return 0xFF;      
    }

    else if(GET_VALUE_STATUS!=0xFF)
    {
        milk_qty=Float_Long(tcol_buf);

        if(milk_qty>65000)
        {
            STRCPY(LBUF," CAPACITY:650   Lts");
            if(WEIGHT_KG=='Y')
            {
                LBUF[16]='K';LBUF[17]='g';LBUF[18]='s';
            }
            if(line==LINE8+6)
                Disp_Msg(LINE8);
            else
                Disp_Msg(LINE4);

            Delay_Sec(1);
            if(line==LINE8+6)
                CLEAR_DATA_LINES_FROM(8,8);
            else
                CLEAR_DATA_LINES_FROM(4,4);
            return qstep;
        }
        else
        {
            CollectionRecord.QTY=milk_qty;
            return (qstep+1);
        }
    }               
}

void Init_Milk_Analyzers(UINT8 mtype)
{
    if(Settingsrec.FAT_MODE==1 && mtype=='C')
    {
        if(Settingsrec.USE_ANALYZER_PORT2=='Y')
            Initialize_Analyzers(2);
        else
            Initialize_Analyzers(1);
    }
}

UINT8 User_Edit_Control(void)
{
    UINT8 user_edit_control,return_status=0;
    
    SEERAMReadString(User_Col_Edit_Opt_Loc,1,(char *)&user_edit_control);
                    
    if(user_edit_control=='N' && LOGIN_TYPE=='O')
    {
        STRCPY(LBUF," User Can not ");
        Disp_Msg_No_Spaces(0,LINE4+1);

        STRCPY(LBUF,"Modify Record ");
        Disp_Msg_No_Spaces(0,LINE6+1);
        Exit_Any_Key(1);   
        return_status=1;
    }
    return return_status;
}

void Print_Last_Transaction(void)
{
    if(ANALYZER_COUNT>0 && CollectionRecord.CODE==0xFFFF)
    {
        //FLASHReadString(Sample_Records_Loc +((ANALYZER_COUNT-1)*17),17,(char *)&CollectionRecord);
        FLASHReadString(Sample_Records_Loc +((ANALYZER_COUNT-1)*Current_Shift_Record_Length),Current_Shift_Record_Length,(char *)&CollectionRecord);
        Member_SNo=Read_Member_SNO(CollectionRecord.CODE);
        Read_Member_Details(Member_SNo);
        Read_Collection_Flags();
        CollectionRecord.SHIFT_RECORD_NO=WEIGHT_COUNT;
        Print_Receipt('D');//Print Last Transaction;

    }
}

#ifdef NAME_PH_SEARCH1

void Search_Mode_Change(void)
{
    if(MODE_FLAG==1)
    {
        if(NAME_SEARCH!='N')
        {
            SEERAMReadString(Searching_Option_Loc,1,(char *)&NAME_SEARCH);
            NAME_SEARCH++;
            if(NAME_SEARCH==3)
                NAME_SEARCH=0;

            SEERAMWriteString(Searching_Option_Loc,1,(char *)&NAME_SEARCH);
            Reset_Display_Values();
        }
    }
}
#endif

UINT8 Milk_Snf_Entry(UINT8 fstep,UINT16 snf_clr,UINT8 line)
{
    UINT8 tcol_buf[10];
   
    ZERO=1;
    Hex_DEC(1,snf_clr,tcol_buf,4);
    Shift_Left(tcol_buf,4);
    tcol_buf[4]=0x00;

    INVERT_DISPLAY=1;
    GET_VALUE_STATUS=Get_Numeric(4,1,line+8,(char *)&tcol_buf);
    INVERT_DISPLAY=0;
    
    if(KBV==KB_LEFT)
    {
        return (fstep-1);
    }
    else if(GET_VALUE_STATUS==0xFF)
    {
        return 0xFF;
    }

    else if(GET_VALUE_STATUS!=0xFF)
    {
        snf_clr=Float_Long(tcol_buf);   
        
        if(CLR_BASED=='Y')
        {
            CollectionRecord.CLR1 = snf_clr;
            return (fstep+1);
        }
        else
        {       
            if(snf_clr>250 || snf_clr==0)
            {            
                CollectionRecord.SNF=0;
                return fstep;  
            }
            else
            {
                CollectionRecord.SNF=snf_clr;
                return (fstep+1);
            } 
        }
    }
}


void Milk_Collection(UINT8 mcollectiontype)
{
    UINT8 use_snf,m_step,edit_col=0,auto_manual_On,analyzer_count_flag;
    UINT8 milk_type,analyzer_status;
    UINT8 tcol_buf[21];
    UINT8 tra_status,prev_mcollectiontype;
    
    UINT16 noofrecords,reccount,tempanalyzer_count;
    UINT16 snf_clr;
    
    UINT32 weeprom_addr;
    UINT32 add_qty;

    if(Init_Collection(mcollectiontype,1)==0xFF)
        return;
    
    if(NO_OF_CUSTOMERS==0)
    {
        STRCPY(LBUF,"NO MEMBER LIST");
        Disp_Msg_No_Spaces(0,LINE5+1);

        Exit_Any_Key(1);

        return;
    }
    
    m_step=0;
    
    
    Init_Milk_Analyzers(mcollectiontype);
    
    prev_mcollectiontype=mcollectiontype;
    
    SEERAMReadString(Col_Left_Arrow_Edit_Loc,1,(char *)&LEFT_ARROW_EDIT);
    
    while(1)
    {
        switch(m_step)
        {
            case 0:
                
                if(WEIGHT_COUNT>999)
                {
                    Max_Samples_Msg();
                    return;
                }
    
                MODE_FLAG=1;
                if(prev_mcollectiontype=='C')
                    mcollectiontype='C';

                if(mcollectiontype=='C')
                {
                    Display_Full_Boarder();
                    Menu_Header('Z',0);
                    
                    
                    Read_Date_Month_Year();
                    
                    if(RunningShift.YEAR!=Shiftrec.YEAR || RunningShift.MONTH!=Shiftrec.MONTH || RunningShift.DAY!=Shiftrec.DAY || RunningShift.DAY_SHIFT!=Shiftrec.DAY_SHIFT)
                        return;
                }
                else
                {
                    Menu_Header(mcollectiontype,0);
                    Shiftrec=EditShiftrec;
                }

                Member_Code=0;
                if(edit_col==0)
                    CollectionRecord.CODE=0;

                if(mcollectiontype=='E' && WEIGHT_COUNT>ANALYZER_COUNT)
                {
                    //FLASHReadString((Sample_Records_Loc+(ANALYZER_COUNT*17)),2,(char *)&CollectionRecord.CODE);
                    FLASHReadString((Sample_Records_Loc+(ANALYZER_COUNT*Current_Shift_Record_Length)),2,(char *)&CollectionRecord.CODE);
                    analyzer_count_flag=1;
                    Member_Code=CollectionRecord.CODE;                       
                }

                else if(mcollectiontype=='C' || (mcollectiontype=='E' && WEIGHT_COUNT==ANALYZER_COUNT))
                {
                    if(mcollectiontype=='E')
                    {
                        Menu_Header(mcollectiontype,1);                        
                        Display_Shift(LINE8);
                        
                        CollectionRecord.CODE=Get_Member_Number(Member_Code,LINE3);
                    }
                        
                    else
                    {
                        Display_Col_Code_Milk(1,0,0);
                        KBV=0xFF;
                      
                        CollectionRecord.CODE=Get_Member_Number_Small(Member_Code,LINE3+7);
                    }
                    
                    analyzer_count_flag=0;
                    if(KBV==KB_LEFT && CollectionFlag==1)
                    {
                        if(CollectionRecord.CODE==0xFFFF)
                        {
                            Print_Last_Transaction();
                        }
                        m_step=0;
                        break;
                    }
                }

                edit_col=0;
                
                if(CollectionRecord.CODE==0xFFF0)       //Invalid Member Code
                {
                    m_step=11;
                    break;
                }
                if(CollectionRecord.CODE==0xF9F9)       //Invalid RFID
                {
                    m_step=12;
                    break;
                }
                //Search Option
#ifdef NAME_PH_SEARCH1
                if(CollectionRecord.CODE==0xF0F0)
                {
                    Search_Mode_Change();
                    m_step=0;
                    break;
                }
#endif

                if(CollectionRecord.CODE==0xFFFF)
                {
                    return;                 
                }

                if(CollectionRecord.CODE==0)
                {
                    m_step=0;    
                    break;
                }

                Member_SNo=Read_Member_SNO(CollectionRecord.CODE);
                
                if(Member_SNo!=0)
                {
                    Read_Member_Details(Member_SNo);

                    if(Memberrec.STATUS!='A')
                    {
                        Member_InActive_Msg();
                        m_step=0;
                        break;
                    }

                    Init_Collection_Variables();
                    
                    //version1.07
                    if(mcollectiontype=='E')
                       noofrecords=DynamicRead_BillDetails(Member_SNo);
                    else
                        noofrecords=Read_CurrentShiftDetails(CollectionRecord.CODE);
                    
                    if(noofrecords>0)
                    {
                        CollectionRecord=CollectionRecords[noofrecords-1];
                        Read_Collection_Flags();
                    }             
                    
                    if(mcollectiontype!='C')
                    {
                        m_step='E';
                        break;
                    }
                    
                    else            //Normal Collection
                    { 
                        if(CollectionRecord.QTY==0xFFFF && CollectionRecord.FAT==0xFF)
                        {
                            Init_Collection_Variables();
                            Display_Col_Code_Milk(1,CollectionRecord.CODE,0);
                           
                            if((MILK_SELECTION=='Y' || MULTIPLE_SAMPLES=='Y') && Settingsrec.MIXED_MILK=='N')
                            {
                                Memberrec.MILK_TYPE=Select_Col_Milk_Type(1);
                                
                                if(Memberrec.MILK_TYPE==0xFF)
                                    return;
                                
                                if(Memberrec.MILK_TYPE=='L')
                                {
                                    m_step=0;
                                    break;
                                } 
                               
                            } 
                            if(Memberrec.MILK_TYPE!=0xFF && Memberrec.MILK_TYPE!='L')
                            {
                                CollectionFlags.MILK_TYPE=Memberrec.MILK_TYPE;
                                Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;
                            }          
                        }
                    }
                    if(CollectionRecord.QTY!=0xFFFF)//CollectionRecord.QTY>0
                    {
                        Tablerec.MILK_TYPE=(CollectionRecord.FLAGS & 0x01);
                        if( Tablerec.MILK_TYPE == 0x00)
                            Tablerec.MILK_TYPE='B';
                        else
                            Tablerec.MILK_TYPE='C';
                        
                        if(CollectionRecord.FAT==0xFF)
                        {
                            CollectionRecord.FAT=0;
                            CollectionRecord.SNF=0;
                            CollectionRecord.RATE=0;
                            CollectionRecord.WATER1=0;
                            CollectionRecord.CLR1=0;
                        }
                        else if(mcollectiontype=='C' && MULTIPLE_SAMPLES=='Y')//upto here
                        {
                            Display_Full_Boarder();
                            Display_Code_Name(LINE1);
                            
                            STRCPY(LBUF,"Continue Collection?");
                            Disp_Msg(LINE4);
                            
                            STRCPY(LBUF,"SAMPLES-00 Taken");
                            Hex_DEC(0,noofrecords,LBUF+8,2);
                            
                            if(Confirm_Yes()==1)                            
                            {
                                Clear_LCD();
                                Init_Collection_Variables();
                                Display_Col_Code_Milk(1,CollectionRecord.CODE,noofrecords);
                                
                                if(Settingsrec.MIXED_MILK=='N')
                                {
                                    Memberrec.MILK_TYPE=Select_Col_Milk_Type(1);

                                    if(Memberrec.MILK_TYPE==0xFF)
                                        return;

                                    if(Memberrec.MILK_TYPE=='L')
                                    {
                                        m_step=0;
                                        break;
                                    }
                                    
                                    if(Memberrec.MILK_TYPE!=0xFF && Memberrec.MILK_TYPE!='L')
                                    {
                                        Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;
                                        CollectionFlags.MILK_TYPE=Memberrec.MILK_TYPE;
                                        m_step=1;
                                    }
                                }
                                else
                                {
                                    Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;
									CollectionFlags.MILK_TYPE=Memberrec.MILK_TYPE;
                                    m_step=1;
                                }
                                                          
                                break;
                            }   
                        }
                       
                       //version1.07
                     
                        m_step='E';
                        prev_mcollectiontype=mcollectiontype;
                        mcollectiontype='E';
                        break;
                        
                    }
                    else
                    {
                        Init_Collection_Variables();

                        Read_Analyzer_Col_Time();

                        CollectionRecord.CLR1=0;
                        CollectionRecord.WATER1=0;
                        m_step=1;
                    }
                }
                else
                {
                    m_step=11;
                }

                break;

            case 1:

                MODE_FLAG=0;
                if(mcollectiontype=='C' && edit_col==0)
                {
                    Init_Collection_Variables();
                    SEERAMReadString(Fat_Mode_Loc,1,(char *)&Settingsrec.FAT_MODE);
                    SEERAMReadString(Weight_Mode_Loc,1,(char *)&Settingsrec.WEIGHT_MODE);
                }
                if(edit_col==1)
                    Settingsrec.FAT_MODE=0;
                
                Display_Full_Boarder();
                Display_Bill_Details(mcollectiontype,0);
                Hex_DEC(1,CollectionRecord.FAT,tcol_buf,4);
                
                if(Settingsrec.FAT_MODE==1 && mcollectiontype=='C')
                { 
                    FONT_TYPE='B';

                    NOR_DISPLAY_LAST_LINE=0xC0;

                    if(Settingsrec.USE_ANALYZER_PORT2=='Y')
                        Display_Char_Big(LINE7,1,'2');
                    else
                        Display_Char_Big(LINE7,1,'1');
                    Display_Char_Big(LINE7,2,':');
                    
                    Display_Analyzer();                    
                    Disp_Msg_No_Spaces(0,LINE7+3);
                    NOR_DISPLAY_LAST_LINE=0x00;
                    analyzer_status=Read_Analyzer_Inputs();
                    
                    if(analyzer_status==0xFF)
                        return;
                    else if(analyzer_status=='L')
                    {
                        m_step=0;
                        break;
                    }
                    else
                    {
                        Update_Analyzer_Flag=0;
                        Update_Analyzer_Flag2=0;
                        ANALYZER_FLAG2=0;
                        ANALYZER_FLAG=0;
                        m_step=2;
                        snf_clr = CLR_BASED == 'Y' ? CollectionRecord.CLR1 : CollectionRecord.SNF;
                    }

                    if(CollectionRecord.FAT==0 || ((Settingsrec.ANALYZER!='M' && Settingsrec.ANALYZER!='F') && snf_clr==0))
                    {                        
                        Hex_DEC(1,CollectionRecord.FAT,LBUF,4);
                        Shift_Left(LBUF,4);    
                        LBUF[5]=LBUF[13]=0x90;
                        Hex_DEC(1,snf_clr,LBUF+8,4);
                        Hex_DEC(1,CollectionRecord.WATER1,LBUF+15,4);

                        Invert_Disp_Msg(LINE5);

                        STRCPY(LBUF,"RepeatCollection");
                        Disp_Msg_No_Spaces(1,LINE7);
                        Wait_Key_Press('A');
                        
                        m_step=1;
                        break;
                    }                   
                }
                else
                {
                    m_step=Milk_Fat_Entry(m_step,LINE5);
                    
                    if(m_step==0xFF)
                        return;
                  
                }
                if(m_step==2 && CollectionRecord.FAT>0)
                {
                    if(Tablerec.MILK_TYPE=='C')
                        use_snf=Settingsrec.COW_SNF;

                    if(Tablerec.MILK_TYPE=='B')
                        use_snf=Settingsrec.BUF_SNF;
                    
                    Calculate_Rate(use_snf);
                                      
                    if(mcollectiontype=='C')
                    {
                        if(Settingsrec.FAT_MODE==1 && (Settingsrec.ANALYZER=='M' || Settingsrec.ANALYZER=='F'))
                        {
                            Calculate_Rate('N');
                            m_step=4;
                            add_qty=0;
                        }
                        else
                        {
                            m_step = (use_snf=='Y') ? 2 : 3;
                        }
                    }
                    else if(mcollectiontype=='E')
                    {
                        Settingsrec.WEIGHT_MODE=0;
                        m_step = (use_snf == 'N') ? 3 : 2;
                        
                    }
                }

                break;

            case 2:

                if(mcollectiontype=='C' && edit_col==0)
                {
                    CollectionRecord.RATE=0;
                    CollectionRecord.QTY=0;
                }

                Calculate_Rate(use_snf);
                       
                snf_clr = CLR_BASED == 'Y' ? CollectionRecord.CLR1 : CollectionRecord.SNF;
                if(snf_clr==0)
                    CollectionRecord.RATE=0; 
                if(Settingsrec.FAT_MODE==0 || mcollectiontype!='C' || edit_col==1)
                {
                    m_step=Milk_Snf_Entry(m_step,snf_clr,LINE5);
                    
                    if(KBV==KB_LEFT)                  
                        edit_col=1;
                    
                    if(m_step==0xFF)
                        return;
                }
                else
                    m_step=3;

                break;

            case 3:

                if(mcollectiontype=='C' && edit_col==0)
                {
                    CollectionRecord.RATE=0;
                    CollectionRecord.QTY=0;
                }

                Calculate_Rate(use_snf);

                m_step=4;
                add_qty=0;
              
                break;

            case 4:
                
                if(edit_col==1)
                    Settingsrec.WEIGHT_MODE=0;

                if(Settingsrec.WEIGHT_MODE==0)
                {
                    ZERO=1;
                    
                    if(edit_col==1)
                        Display_Bill_Details(mcollectiontype,0);
                        
                    else
                        Display_Bill_Details(mcollectiontype,'C');
                    m_step=Milk_Qty_Entry(m_step,LINE8+6);                  
                }
                else
                {  
                    Display_Box_Boarder(LINE7);//CLEAR_DATA_LINES_FROM(6,8);
                    Display_Bill_Details(mcollectiontype,'R');
                    m_step=Auto_Milk_Qty1(1,m_step,LINE7,add_qty);
                }
                if(m_step==0xFF)
                    return;
              
                if(m_step=='+')
                {
                    if(Settingsrec.WEIGHT_MODE==1 && WMrec.WM_AUTO_TARE=='Y')
                        WM_SendByte(WMrec.WM_TARE_COMMAND);
                    
                   add_qty=add_qty+CollectionRecord.QTY;
                   CollectionRecord.QTY=0;
                   m_step=4;
                   Display_Box_Boarder(LINE7);
                   break;
                }
                if(m_step==3)
                {
                    if(Settingsrec.WEIGHT_MODE==1)
                        CollectionRecord.QTY=add_qty;
                    Display_Bill_Details(mcollectiontype,0);
                    
                    edit_col=1;
                    m_step=1;
                    break;
                }

                break;

            case 5:

                if(Settingsrec.WEIGHT_MODE==0)
                {
                    Calculate_Amount();

                    Display_Bill_Details(mcollectiontype,0);

                    CURSOR_BLINK=0x80;
                    LCD_Set_Cursor('S',LINE8+19);

                    if(Amount>0)
                        Hex_DEC(2,Amount,LBUF+13,7);

                    if(Amount>999999)
                        Hex_DEC(2,Amount,LBUF+13,8);

                    Display_Char(LBUF[19]);

                    CURSOR_BLINK=0x00;

                    Send_Collection_Ext_Display();
                }
                
                m_step=Confirm_Add_Save_Weight(m_step,1,add_qty);

                if(m_step=='L')
                {
                   m_step=4; 
                   CollectionRecord.QTY=(CollectionRecord.QTY+add_qty);
                   add_qty=0;
                   edit_col=1;
                   Display_Box_Boarder(LINE7);//CLEAR_DATA_LINES_FROM(7,8);
                   break;
                }
                
                else if(m_step==0xFF)
                {     
                    return;                 
                }  
                
                if(m_step=='S' || m_step=='N')
                {
                    CollectionRecord.QTY=(CollectionRecord.QTY+add_qty);
                   // Display_Bill_Details(mcollectiontype,0);
                     Please_Wait_Msg(LINE5);
                                           
                    if((mcollectiontype=='C' && RunningShift.DAY_SHIFT==Shiftrec.DAY_SHIFT) || mcollectiontype=='E')
                        tra_status=Save_Transaction(mcollectiontype);
                    else
                        return;

                    if(tra_status==0xFF)
                        return;
                    
                   
                    
                    if(analyzer_count_flag==1)
                    {

                        ANALYZER_COUNT=ANALYZER_COUNT+1;
                        analyzer_count_flag=0;
                    }

                    if(mcollectiontype=='E')
                    {
                        Display_Boarder3();
                        STRCPY(LBUF,"  EDITED DATA ");//2
                        Disp_Msg_No_Spaces(0,LINE4+1);

                        STRCPY(LBUF,"  STORED..... ");
                        Disp_Msg_No_Spaces(0,LINE6+1);

                        mcollectiontype='D';
                    }
                    else
                    {
                        if(Settingsrec.WEIGHT_MODE==1 && WMrec.WM_AUTO_TARE=='Y')
                            WM_SendByte(WMrec.WM_TARE_COMMAND);
                    }

                   // Please_Wait_Msg(LINE5);
                    if(m_step=='S')
                    {
                        if(Settingsrec.Collection_Rec_Ext=='N')
                                ;
                        else if(Settingsrec.Collection_Rec_Ext=='E')
                            Ext_Print_Receipt(mcollectiontype);
                        else
                            Print_Receipt(mcollectiontype);
                    }
                    else
                    {
                        STRCPY(LBUF,"TRANSACTION SAVED ");
                        Invert_Disp_Msg(LINE8);
                    }
                    
                    Delay_Sec(1);

                    Clear_LCD();

                    m_step=0;
                    if(mcollectiontype=='D')
                    {
                        mcollectiontype='E';
                        if(prev_mcollectiontype=='C')
                            mcollectiontype='C';
                    }
                    edit_col=0;
                    break;
                }
                break;
                
            case 10:
                
                //Only if Mode_Selection option is enabled in dealer settings
                SEERAMReadString(Auto_Manual_On_Loc,1,(char *)&auto_manual_On);
                CollectionFlag=0;
                if(auto_manual_On=='Y' && WEIGHT_COUNT==ANALYZER_COUNT)
                {
                    SEERAMReadString(Weight_Mode_Loc,SETTINGS_RECORD_LENGTH,(char *)&Settingsrec);
                    
                    Mode_Selection();
                 
                    SEERAMReadString(Weight_Mode_Loc,SETTINGS_RECORD_LENGTH,(char *)&Settingsrec);
                    
                    Init_Milk_Analyzers(mcollectiontype);
                }
                if(mcollectiontype=='C')
                    CollectionFlag=1;
                Clear_LCD();
                m_step=0;
                break;
                
            case 11:
                
                STRCPY(LBUF,"INVALID MEMBER CODE");
                Disp_Msg(LINE6);

                Delay_Sec(1);                
                
                m_step=0;
                break;
                
            case 'E':                   //Edit
                
                if(noofrecords>0)
                {
                    reccount=noofrecords-1;
                    do
                    {
                        CollectionRecord=CollectionRecords[reccount];//2
                        Read_Collection_Flags();
                        CollectionRecord_NO.RECORDNO=CollectionRecords_NOS[reccount].RECORDNO;
                        CollectionRecord_NO.Type=CollectionRecords_NOS[reccount].Type;

                        Tablerec.MILK_TYPE=CollectionFlags.MILK_TYPE;
                        
                        Display_Bill_Details(mcollectiontype,0);

                        KBV=Wait_Key_Press('E');
                        
                       
                        if(KBV==KB_CANCEL || KBV==KB_ENT)
                            break;
                        
                        if(KBV==KB_UP)
                        {
                            if(reccount==0)
                                reccount=noofrecords-1;
                            else
                                reccount--;
                        }
                        if(KBV==KB_DOWN)
                        {
                            if(reccount==noofrecords-1)
                                reccount=0;
                            else
                                reccount++;
                        }

                    }while(1);
                }
                
                if(KBV==KB_CANCEL)
                {
                    m_step=0;                        
                    break;
                }
            
                if(noofrecords==0 || (CollectionRecord.QTY==0xFFFF && CollectionRecord.FAT==0xFF))
                {
                    Collection_Not_Found_Msg();
                 
                    Wait_Key_Press('C');
                    m_step=0;
                    break;
                }
                
                Read_Collection_Flags();

                if(CollectionRecord.QTY!=0xFFFF && CollectionRecord.FAT!=0xFF)
                {
                    if(CollectionFlags.PAYMENT_STATUS==0 || (CollectionFlags.UPLOAD_STATUS==0 && CollectionRecord_NO.Type!='L'))
                    {
                        if(CollectionFlags.PAYMENT_STATUS==0)
                            STRCPY(LBUF,"  PAYMENT TAKEN");

                        if(CollectionFlags.UPLOAD_STATUS==0)
                            STRCPY(LBUF," RECORD UPLOADED");

                        Disp_Msg_Big(1,LINE4);
                        
                        STRCPY(LBUF,"YOU CAN'T MODIFY");
                        Disp_Msg_Big(1,LINE6);

                        Exit_Any_Key(1);
                        m_step=0;                        
                        break;
                    }
                }
                             
                if(KBV==KB_CANCEL)
                {
                    if(WEIGHT_COUNT!=ANALYZER_COUNT)
                        return;
                    m_step=0;
                    break;
                }

                if(KBV==KB_ENT)
                {    
                    CLEAR_DATA_LINES_FROM(3,8);
                    
                    if(User_Edit_Control()==1)
                    {
                        m_step=0;
                        break;
                    }
                    
                    //EditRecord=CollectionRecord;

                    Display_Col_Code_Milk(1,CollectionRecord.CODE,0);

                    Memberrec.MILK_TYPE=CollectionFlags.MILK_TYPE;
                   
                    if((MILK_SELECTION=='Y' || MULTIPLE_SAMPLES=='Y') && Settingsrec.MIXED_MILK=='N')
                    {
                        Memberrec.MILK_TYPE=Select_Col_Milk_Type(1);
                        
                        if(Memberrec.MILK_TYPE==0xFF)
                            return;
                        
                        if(Memberrec.MILK_TYPE=='L')
                        {
                            m_step=0;
                            break;
                        }
                    }
                    CollectionFlags.MILK_TYPE=Memberrec.MILK_TYPE;
                    Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;
                    
                    if(CollectionRecord.QTY!=0xFFFF && CollectionRecord.FAT==0xFF)
                    {                       
                        CollectionRecord.FAT=0;
                        CollectionRecord.SNF=0;
                        CollectionRecord.RATE=0;
                        CollectionRecord.WATER1=0;
                        CollectionRecord.CLR1=0;                         
                    }
                    m_step=1;
                    Read_RTC();
                }
                break;
           // }
        }//switch
    }//while
}

void Display_Counters(void)
{
    if((WEIGHT_COUNT>0 && WEIGHT_COUNT <1001) && (CollectionFlag==1 || (Big_RTC==1 && STANDBY==1)))
    {
        STRCPY(LBUF,"W:   A:          ");
        ZERO=1;

        Hex_DEC(0,WEIGHT_COUNT,LBUF+2,3);
        Shift_Left(LBUF+2,3);

        Hex_DEC(0,ANALYZER_COUNT,LBUF+7,3);
        Shift_Left(LBUF+7,3);

        if(GPRSrec.GPRS_ON=='Y' && ON_LINE=='Y')
        {
            if(GPRSrec.AUTO_SMS=='Y')
            {
                LBUF[10]='S';
                LBUF[11]=':';
                Hex_DEC(0,SMS_COUNT,LBUF+12,3);
                Shift_Left(LBUF+12,3);
            }

            if(GPRSrec.AUTO_TRANSFER=='Y')
            {
                LBUF[15]='U';
                LBUF[16]=':';
                Hex_DEC(0,DU_COUNT,LBUF+17,3);
                Shift_Left(LBUF+17,3);
            }
        }

        //ZERO=0;080719

        if(CollectionFlag==1)
        {
            Disp_Msg(LINE1);            
        }

        else if(Big_RTC==1 && STANDBY==1)
            Disp_Msg(LINE3);
    }
}

void Milk_Collection_2Analyzers(UINT8 collectiontype2)
{
    UINT8 m2_step,tcol_buf[21];
    UINT16 noofrecords;
    UINT32 milk_qty,add_qty;

    if(Init_Collection(collectiontype2,2)==0xFF)
        return;
    
    Display_Full_Boarder();
    m2_step=0;

    while(1)
    {
        switch(m2_step)
        {
            case 0:
                
                SEERAMReadString(Weight_Mode_Loc,2,(char *)&Settingsrec.WEIGHT_MODE);
                Menu_Header('Z',0);
                
                if(WEIGHT_COUNT>999)
                {
                    Max_Samples_Msg();
                    return;
                }
    
                Read_Date_Month_Year();
                
                if(RunningShift.YEAR!=Shiftrec.YEAR || RunningShift.MONTH!=Shiftrec.MONTH || RunningShift.DAY!=Shiftrec.DAY || RunningShift.DAY_SHIFT!=Shiftrec.DAY_SHIFT)
                        return;

                CollectionRecord.CODE=0;
                Display_Col_Code_Milk(1,0,0);
                
                CollectionRecord.CODE=Get_Member_Number_Small(CollectionRecord.CODE,LINE3+5);
                
                if(CollectionRecord.CODE==0xFFF0)
                {
                    m2_step=11;
                    break;
                }
                
                if(CollectionRecord.CODE==0xF9F9)
                {
                    m2_step=12;
                    break;
                }
                
#ifdef NAME_PH_SEARCH1
                //Search option
                if(CollectionRecord.CODE==0xF0F0)       //
                {
                    Search_Mode_Change();
                    m2_step=0;
                    break;
                }//name search
#endif
               
                if(KBV==KB_LEFT && CollectionFlag==1)       //Duplicate Receipt
                {
                    if(CollectionRecord.CODE==0xFFFF)
                    {
                        Print_Last_Transaction();
                    }
                    m2_step=0;
                    break;
                }
                
                if(CollectionRecord.CODE==0xFFFF)       //Exit
                {  
                    return;
                }

                if(CollectionRecord.CODE==0)    //collection Mode change    
                {
                    m2_step=0;
                    break;
                }
                
                Member_SNo=Read_Member_SNO(CollectionRecord.CODE);

                if(Member_SNo!=0)
                {
                    Read_Member_Details(Member_SNo);

                    if(Memberrec.STATUS!='A')
                    {
                        Member_InActive_Msg();
                        m2_step=0;
                        break;
                    }

                    noofrecords=Read_CurrentShiftDetails(CollectionRecord.CODE);
                    if(noofrecords>0)
                    {
                        CollectionRecord=CollectionRecords[noofrecords-1];
                        
                        Read_Collection_Flags();
                    }
                   
                    if(CollectionRecord.QTY==0xFFFF && CollectionRecord.FAT==0xFF)
                    {
                        Display_Col_Code_Milk(1,CollectionRecord.CODE,noofrecords);
                        Init_Collection_Variables();
                        
                        //if(MULTIPLE_SAMPLES=='Y' && Settingsrec.MIXED_MILK=='N')
                        if((MILK_SELECTION=='Y' || MULTIPLE_SAMPLES=='Y') && Settingsrec.MIXED_MILK=='N')
                        {
                            Memberrec.MILK_TYPE=Select_Col_Milk_Type(0);
                            
                            CollectionFlags.MILK_TYPE=Memberrec.MILK_TYPE;
                            Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;
                            
                            if(Memberrec.MILK_TYPE==0xFF)
                                return;
                            if(Memberrec.MILK_TYPE=='L')
                            {
                                m2_step=0;
                                break;
                            }
                        }  
                        else
                        {
                            CollectionFlags.MILK_TYPE=Memberrec.MILK_TYPE;
                            Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;
                        }
                    }
                    
                    if(CollectionRecord.QTY!=0xFFFF  && MULTIPLE_SAMPLES=='Y')
                    {
                        CollectionRecord.RATE=0xFFFF;
                        Display_Col_Code_Milk(2,CollectionRecord.CODE,noofrecords);
                        STRCPY(LBUF,"  NEW SAMPLE:   ");
                        Disp_Msg(LINE4);
                        
                        if(Select_Col_Yes_No(14)=='Y')
                        {
                            Init_Collection_Variables();
                            
                            Display_Col_Code_Milk(1,CollectionRecord.CODE,noofrecords);
                            
                            if(Settingsrec.MIXED_MILK=='N')
                            {
                           
                                Memberrec.MILK_TYPE=Select_Col_Milk_Type(0);

                                CollectionFlags.MILK_TYPE=Memberrec.MILK_TYPE;
                                Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;

                                if(Memberrec.MILK_TYPE==0xFF)
                                    return;

                                if(Memberrec.MILK_TYPE=='L')
                                {
                                    m2_step=0;
                                    break;
                                }

                                else
                                {
                                    m2_step=1;  
                                    add_qty=0;
                                }
                            }
                            else
                            {
                                CollectionFlags.MILK_TYPE=Memberrec.MILK_TYPE;
                                Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;
                                m2_step=1;  
                                add_qty=0;
                            }

                            break;
                        }                   
                    }
                    else if(CollectionRecord.QTY!=0xFFFF )
                    {
                        CollectionRecord.RATE=0xFFFF;
                        Read_Collection_Flags();
                        Memberrec.MILK_TYPE=CollectionFlags.MILK_TYPE;
                        Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;
                            
                        Display_Bill_Details(collectiontype2,1);//0);                        
                        STRCPY(LBUF,"    Weight Taken");
                        Disp_Msg(LINE5); 
                        
                        KBV=Wait_Key_Press('C');
                        if(KBV==KB_CANCEL)
                            return;
                    }
                    
                    else
                    {
                        CLEAR_DATA_LINES_FROM(2,4);
                        
                        Init_Collection_Variables();

                        Read_RTC();

                        CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]);                        

                        CollectionRecord.WTIME[1]=HEX_ASCII(Time[1]);
                        CollectionRecord.WTIME[2]=HEX_ASCII(Time[0]);
                        if(CollectionFlags.MILK_TYPE=='B')
                            CollectionRecord.FLAGS=0xFE;
                       
                        m2_step=1;
                       
                    }
                }
                else
                {
                    m2_step=11;
                }
                
                if(m2_step==1)
                {
                    add_qty=0;
                    CollectionRecord.QTY=0;
                }

                break;
            
            case 1:                
                
                Display_Col_Code_Milk(1,CollectionRecord.CODE,101);
                
                if(Settingsrec.WEIGHT_MODE==0)
                {
                    ZERO=1;
                    
                    Display_Col_Qty(LINE4,0);
                    m2_step=Milk_Qty_Entry(m2_step,LINE4+8);
                    Display_Col_Qty(LINE4,0);
                 
                }
                else
                {
                    
                    m2_step=Auto_Milk_Qty1(2,m2_step,LINE4,add_qty);
                    
                    if(m2_step=='+')
                    {
                        if(Settingsrec.WEIGHT_MODE==1 && WMrec.WM_AUTO_TARE=='Y')
                            WM_SendByte(WMrec.WM_TARE_COMMAND);
                        
                        add_qty=add_qty+CollectionRecord.QTY;
                        CollectionRecord.QTY=0;
                        m2_step=1;//2;
                        CLEAR_DATA_LINES_FROM(5,5);
                        break;
                    }
                }
                if(m2_step==0xFF)
                    return;
                
                break;

            case 2:

                m2_step=Confirm_Add_Save_Weight(m2_step,2,add_qty);

                if(m2_step=='L')
                {
                    m2_step=1;
                    CollectionRecord.QTY=(CollectionRecord.QTY+add_qty);
                    add_qty=0;
                    Settingsrec.WEIGHT_MODE=0;
                    CLEAR_DATA_LINES_FROM(4,5);
                    break;
                }               
                else if(m2_step==0xFF)
                {   
                    return;                    
                   
                }    
                 else if(m2_step=='S')
                {
                    CollectionRecord.QTY=(CollectionRecord.QTY+add_qty);
                    if(Settingsrec.WEIGHT_MODE==1 && WMrec.WM_AUTO_TARE=='Y')
                        WM_SendByte(WMrec.WM_TARE_COMMAND);

                     if(RunningShift.DAY_SHIFT==Shiftrec.DAY_SHIFT)
                        Save_Weight(collectiontype2);
                     else
                         return;
                    if(Settingsrec.WEIGHT_MODE==1 && WMrec.WM_AUTO_TARE=='Y')
                        WM_SendByte(WMrec.WM_TARE_COMMAND);
                    
                    Display_Col_Qty(LINE4,0);
                    STRCPY(LBUF,"    Weight Saved");
                    Disp_Msg(LINE5);

                    Display_Counters();
                    Buzzer(250);

                    CLEAR_DATA_LINES_FROM(5,5);

                    m2_step=0;
                    add_qty=0;
                    break;
                }

                break;
                
            case 11:
                
                STRCPY(LBUF,"INVALID CODE..");//1
                Disp_Msg_No_Spaces(0,LINE3+1);

                Delay_Sec(1);
                
                m2_step=0;
                break;                
        }
    }
}

void Deduction(UINT8 ptype)
{
    UINT8 d_step=0;
    UINT16 deduction_count,d_member_sno;
    
    Read_Date_Month_Year();

    Clear_LCD();

    while(1)
    {
        switch(d_step)
        {
            case 0:

                ZERO=0;
                deduction_count=0;

                Menu_Header('G',1);

                d_step=Member_Code_Entry(0);                

                if(d_step==0xFF)
                    return;

                d_member_sno=Read_Member_SNO(Member_Code);

                break;

            case 1:

                Menu_Header('G',0);
                Display_Code_Name(LINE3);

                STRCPY(LBUF,"Select Deduction:");
                Disp_Msg(LINE5);
              
                EEPROM_ADDR=Deduction_Names_Loc+((deduction_count)*Deduction_Name_Length);
                SEERAMReadString(EEPROM_ADDR,Deduction_Name_Length,(char *)&LBUF);

                STRNCPY(DED_BUF,LBUF,20);
                Invert_Disp_Msg(LINE7);

                KBV=Wait_Key_Press('E');

                switch(KBV)
                {
                    case KB_CANCEL:
                        return;

                    case KB_LEFT:

                        d_step=0;
                        break;

                    case KB_ENT:

                        d_step=2;
                        break;
                 
                }
                deduction_count=Up_Down(deduction_count,9);
                break;

            case 2:

                Display_Code_Name(LINE3);
                
                Read_Deduction_Amount(d_member_sno);

                STRNCPY(LBUF,DED_BUF,20);
                Disp_Msg(LINE4);
                ZERO=1;
                STRCPY(LBUF,"PREV. AMT :");
                Hex_DEC(2,Deductionrec.AMT,LBUF+11,7);
                Shift_Left(LBUF+11,7);
                Disp_Msg(LINE5);

                Disp_Top_Boarder_Big_T(LINE6,7,0x8A);

                STRCPY(LBUF,"AMOUNT|       ");
                Hex_DEC(2,0,Temp_Arr,7);
                Shift_Left(Temp_Arr,7);
                NOR_DISPLAY_LAST_LINE=0xC0;
              //  LBUF[6]=0x84;
                Disp_Msg_No_Spaces(0,LINE7+1);
                
                GET_VALUE_STATUS=Get_Numeric_Big(7,2,LINE7+8,(char *)&Temp_Arr);
                NOR_DISPLAY_LAST_LINE=0x00;

                if(KBV==KB_LEFT)
                {
                    d_step=1;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                if(GET_VALUE_STATUS!=0xFF)
                {
                    Amount=Float_Long(Temp_Arr);

                    if(Amount>0)
                    {
                        Deductionrec.AMT=Deductionrec.AMT+Amount;

                        d_step=3;
                    }
                }

                break;

            case 3:
               
                Display_Code_Name(LINE3);

                STRNCPY(LBUF,DED_BUF,20);
                Disp_Msg(LINE4);

                STRCPY(LBUF,"PREV. AMT :");
                Hex_DEC(2,(Deductionrec.AMT-Amount),LBUF+11,7);
                Shift_Left(LBUF+11,7);
                Disp_Msg(LINE5);

                STRCPY(LBUF,"AMOUNT(Rs):");
                Hex_DEC(2,Amount,LBUF+11,7);
                Shift_Left(LBUF+11,7);
                Disp_Msg(LINE6);

                STRCPY(LBUF,"PRINT:ENT  ESC:Next");
                Invert_Disp_Msg(LINE8);

                KBV=Wait_Key_Press('C');

                if(KBV==KB_CANCEL)
                {
                    deduction_count=1;
                    d_step=0;
                }

                if(KBV==KB_ENT)
                {
                    Write_Deduction_Amount(d_member_sno);

                    if(ptype=='T')
                        Print_Deduction_Receipt(Amount);
                    else
                        Ext_Print_Deduction_Receipt(Amount);

                    deduction_count=1;
                    d_step=0;
                }

                break;
        }
    }
}

UINT8 Select_Bonus_Type(void)
{
    UINT8 bonus_count=0;

    STRCPY(LBUF,"Bonus Calculated as:");
    Disp_Msg(LINE3);
        
    bonus_count=Select_Yes_No(0,9);
    
    return bonus_count;
}

void Bonus_Report(UINT8 ptype)
{
    UINT8 b_step=0,m_status;

    while(1)
    {
        Menu_Header('B',0);       

        switch(b_step)
        {
            case 0:

                Bonus_Type=Select_Bonus_Type();

                if(Bonus_Type==0xFF)
                    return;

                b_step=1;

                break;

            case 1:

                STRCPY(LBUF,"BONUS TYPE:PERCENTAGE");
                

                if(Bonus_Type==0)
                {
                    STRCPY(LBUF,"BONUS TYPE:PER LITRE");
                }

                Disp_Msg(LINE3);

                STRCPY(LBUF,"BONUS:        ");
                Disp_Msg_No_Spaces(0,LINE5+1);

                Hex_DEC(2,0,Temp_Arr,6);
                Temp_Arr[6]='\0';

                GET_VALUE_STATUS=Get_Numeric_Big(6,2,LINE5+7,(char *)&Temp_Arr);

                if(KBV==KB_LEFT)
                {
                    b_step=0;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else
                {
                    Bonus_Rate=Float_Long(Temp_Arr);

                    if(Bonus_Rate>0)
                        b_step=2;
                }
                break;

            case 2:

                if(Start_End_Date_Month()==0xFF)//E
                    return;

                b_step=3;
                break;

            case 3:

                m_status=Start_End_Members(1);
                
                b_step=4;
                
                if(m_status==0xFF)
                    return;
                
                if(m_status=='L')
                    b_step=2;
              
                break;

            case 4:

                Print_Bonus_Report(ptype);
                return;
        }
    }
}

void Local_Sale(UINT8 cflag,UINT8 ptype)
{
    UINT8 l_step,status;    
    UINT16 ltr_rate;
    UINT32 local_qty,local_amount;

    l_step=0;

    while(1)
    {
        switch(l_step)
        {
            case 0:

                Memberrec.MILK_TYPE='C';
                Menu_Header('L',1);
                if(cflag=='C')               
                    Read_Date_Month_Year();
                else
                {
                    if(Select_Date_Month_Shift(1)==0xFF)
                        return;
                }
                l_step=1;
                break;
                
            case 1:
                
                Menu_Header('L',0);

                status=Select_Milk_Type1(0);
                if(status=='L')
                {
                    l_step=0;
                    break;
                }
                if(status==0xFF)
                    return;
                
                LocalSalesrec.MILK_TYPE=status;
                
                l_step=2;
                break;

            case 2:
                
                Read_Local_Sale();

                Menu_Header('L',0);

                Display_Boarder3();
                if(cflag=='E' && LocalSalesrec.QTY==0)
                {
                    
                    STRCPY(LBUF,"NO RECORDS AVAILABLE");
                    Disp_Msg(LINE4);

                    STRCPY(LBUF,"      TO EDIT!");
                    Disp_Msg(LINE6);

                    Wait_Key_Press('C');

                    l_step=1;
                    break;
                }
                LocalSalesrec.MILK_TYPE=status;
                
                if(LocalSalesrec.MILK_TYPE=='C')
                    EEPROM_ADDR=Cow_LocalSales_Ltr_Loc;
                    
                else
                    EEPROM_ADDR=Buf_LocalSales_Ltr_Loc;
                SEERAMReadString(EEPROM_ADDR,2,(char *)&ltr_rate);
                
                Display_Milk_Type( LocalSalesrec.MILK_TYPE,LINE3); 

                if(ltr_rate==0xFFFF)
                    ltr_rate=0;

                ZERO=1;
               
                STRCPY(LBUF,"Quantity(L):");
                if(WEIGHT_KG=='Y')
                {
                    LBUF[9]='K';
                }
                if(cflag=='E')
                {
                    Hex_DEC(2,LocalSalesrec.QTY,Temp_Arr,7);//
                    Shift_Left(Temp_Arr,7);
                    STRNCPY(LBUF+12,Temp_Arr,7);
                }
                else
                {
                    Hex_DEC(2,0,Temp_Arr,7);
                    Shift_Left(Temp_Arr,7);
                }
                Temp_Arr[7]=0x00;
                Disp_Msg(LINE5);

                GET_VALUE_STATUS=Get_Numeric(7,2,LINE5+12,(char *)&Temp_Arr);

                if(KBV==KB_LEFT)
                {
                    l_step=1;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    local_qty=Float_Long(Temp_Arr);

                    if(local_qty>0)
                    {
                        if(cflag=='C')
                            LocalSalesrec.QTY=LocalSalesrec.QTY+local_qty;
                        else
                            LocalSalesrec.QTY=local_qty;
                        l_step=3;
                        break;
                    }
                }
                break;

            case 3:
                
                local_amount=ltr_rate;
                local_amount=local_amount*local_qty;
                local_amount=local_amount/100;

                if(cflag=='C')
                    LocalSalesrec.AMOUNT=LocalSalesrec.AMOUNT+local_amount;
                else
                    LocalSalesrec.AMOUNT=local_amount;

                l_step=4;

                Buzzer(250);
                break;

            case 4:

                ZERO=1;
                STRCPY(LBUF,"Quantity(L):");
                if(WEIGHT_KG=='Y')
                {
                    LBUF[9]='K';
                }
               
                Hex_DEC(2,local_qty,LBUF+12,7);
                Shift_Left(LBUF+12,7);
                Disp_Msg(LINE4);

                STRCPY(LBUF,"Rate / Ltr :");
                if(WEIGHT_KG=='Y')
                    STRCPY(LBUF+7,"Kgs :");
                
                Hex_DEC(2,ltr_rate,LBUF+12,6);
                Shift_Left(LBUF+12,6);
                Disp_Msg(LINE5);

                STRCPY(LBUF,"Amount(Rs) :");
                Hex_DEC(2,local_amount,LBUF+12,8);
                Shift_Left(LBUF+12,8);
                Disp_Msg(LINE6);
                ZERO=0;

                STRCPY(LBUF,"Print:ENTER,Next:ESC");//1
                Invert_Disp_Msg(LINE8);

                KBV=Wait_Key_Press('C');

                if(KBV==KB_CANCEL)
                    l_step=0;

                if(KBV==KB_ENT)
                {
                    l_step=0;

                    Write_Local_Sale();

                    if(cflag!='C')
                        break;

                    if(ptype=='T')
                        Print_LocalSale_Receipt(local_qty,ltr_rate,local_amount);
                    else
                        Ext_Print_LocalSale_Receipt(local_qty,ltr_rate,local_amount);
                    
                }
                break;
        }
    }
}


void Data_Entry_SubMenu(UINT8 entrycnt)
{
    UINT8 int_ext_printer;

    SEERAMReadString(Default_Printer_Loc,1,(char *)&int_ext_printer);

    if(int_ext_printer=='Y')
        int_ext_printer='T';
    else
        int_ext_printer='E';
   
    if(entrycnt=='A')
    {
        MODE_FLAG=1;

        SEERAMReadString(Weight_Mode_Loc,SETTINGS_RECORD_LENGTH,(char *)&Settingsrec);
        CollectionFlag=1;

        if(Settingsrec.FAT_MODE==1 && TWO_ANALYZERS=='Y')
        {
            GPRS_FLAG=1;
            SERVER_STATE=0;
            Initialize_Analyzers('B');
            Milk_Collection_2Analyzers('C');

        }
        else if(WEIGHT_COUNT==ANALYZER_COUNT)
            Milk_Collection('C');
        else
        {
            CollectionFlag=1;
            Milk_Collection('E');
        }
        Disable_UART_Interrupts_WM();
        MODE_FLAG=0;
        CollectionFlag=0;
        SERVER_STATE=0;
        CLEAR_DATA_LINES_FROM(3,8);
    }

    if(entrycnt=='B')
        Deduction(int_ext_printer);

    if(entrycnt=='C')
        Local_Sale('C',int_ext_printer);

    if(entrycnt=='D')
        Sangh_Sale('C');

    if(entrycnt=='E')
    {
        Read_Date_Month_Year();
        Close_Shift(1);
    }    
}



void Data_Functions(void)
{
    UINT8 data_count=0,select_fun;

    SEERAMReadString(Weight_Mode_Loc,SETTINGS_RECORD_LENGTH,(char *)&Settingsrec);
    
    if(ON_LINE=='Y')
    {
        if(LOGIN_TYPE=='O')
            U_DATA_ENTRY_MENU_ACCESS[4]=1;
        else
            DATA_ENTRY_MENU_ACCESS[4]=1;
    }
    
    DATA_ENTRY_MENU_FUN_CNT=User_Selected_Menu_List('D');
    
    while(data_count!=0xFF)
    {                     
        Select_Function_Menu('D',data_count);
        
        data_count=Function_Selected(DATA_ENTRY_MENU_FUN_CNT,data_count);
      
        if(KBV==KB_ENT)
        {
            select_fun=SELECT_FUN_CODE[data_count];
            TIME_FLAG=0;
            Data_Entry_SubMenu(select_fun);
            TIME_FLAG=1;
        }                     
    }
}




void Rate_Tables_SubMenu(UINT8 tcnt)
{
    TIME_FLAG=0;

    if(tcnt==0)
       Fat_Rates();           

    if(tcnt==1)
        Snf_Rates();  
    
    TIME_FLAG=1;
}

void Rate_Tables(void)
{
    UINT8 table_count=0;

    Display_Screen_Boarder();
    STRCPY(LBUF,"  *RATE TABLES*");    
    Disp_Header_No_Boarder();
    
    if(DPUcontrolrec.RATE_TABLES!='N')
    {
        STRCPY(LBUF,"Select Table Type?");
        Disp_Msg(LINE3);

        EFFECT_RATE=Select_Yes_No(0,6);

        if(EFFECT_RATE==0xFF)
        {
            EFFECT_RATE=0;
            return;
        }
    }
    else
        EFFECT_RATE=0;
    
    while(table_count!=0xFF)
    {
        Rate_Tables_Header('R');
       
        TIME_FLAG=1;

        if(table_count==0)
            STRCPY(LBUF," 1.FAT RATES");
        if(table_count==1)
        {
            STRCPY(LBUF," 2.SNF RATES");
            CLR_Enabled(LBUF+3);
        }

        Disp_Msg_Boarder();

        STRCPY(LBUF,"Cow&Buf Rate Tables");        
        Disp_Msg(LINE5);
        
        table_count=Function_Selected(2,table_count);
               
        if(KBV==KB_ENT)
        {
            Rate_Tables_SubMenu(table_count);
        }
        
    }
}

void Display_RTC(UINT8 displine)
{
    if(SECONDS_COUNTER>=5)
    {
        SECONDS_COUNTER=0;
        Time[0]=RTCReadByte(0);
        
        if(Temp_RTC1!=Time[0])
        {
            Copy_Date(LBUF);
            Temp_RTC1=Time[0];
            
            LBUF[2]=LBUF[5]='/';

            LBUF[8]=' ';
            LBUF[9]=0x90;
            LBUF[10]=' ';
            LBUF[11]=' ';
            Copy_Time(LBUF+12);
            LBUF[14]=LBUF[17]=':';
            Invert_Disp_Msg(displine);
        }
    }
}



void Display_RTC_Big(void)
{        
    UINT8 temp_char=0,ext_send_buf[50];
    UINT16 length;
    
    ZERO=0;
  
    if(SECONDS_COUNTER>=5)
    {
        SECONDS_COUNTER=0;
        
        Time[0]=RTCReadByte(0);
      //  if(Temp_RTC1!=Time[0])
        {
            Read_RTC();
            STRNCPY(LBUF,(char *)&BRAND_NAME,16);
            Disp_Underline();

            CHECK_SIM_FLAG=1;
            Temp_RTC1=Time[0];
            
            if(MONITOR_VOLTAGE=='Y')
            {
                Read_Voltage();                
            }
           // else
            //    VOLTAGE=0;
            
            if(VOLTAGE>MAX_INPUT_VOLTAGE)
            {
                Display_Input_Voltage();
            }

            Hex_ASCII(Time[4],LBUF);
            Hex_ASCII(Time[5],LBUF+3);
            Hex_ASCII(Time[6],LBUF+6);
            LBUF[2]='-';
            LBUF[5]='-';

            
            Copy_Time(LBUF+9);

            LBUF[11]=LBUF[14]=':';
            LBUF[14]='\0';
            ZERO=0;
            STRCPY(ext_send_buf,"$6000     ScTLSEdPU");

            Hex_DEC(0,DEVICE_CODE,ext_send_buf+5,4);
            if(WEIGHT_COUNT>0)
            {
                STRCPY(ext_send_buf,"$6000              ");
                Hex_DEC(0,WEIGHT_COUNT,ext_send_buf+5,4);
                Hex_DEC(0,ANALYZER_COUNT,ext_send_buf+10,3);
                if(GPRSrec.GPRS_ON=='Y' && ON_LINE=='Y')
                {
                    if(SMS_COUNT>0 && GPRSrec.AUTO_SMS=='Y')
                        Hex_DEC(0,SMS_COUNT,ext_send_buf+13,3);
                    
                    if(DU_COUNT>0 && GPRSrec.AUTO_TRANSFER=='Y')
                        Hex_DEC(0,DU_COUNT,ext_send_buf+16,3);
                }
            }

            Hex_ASCII(Time[4],ext_send_buf+19);
            ext_send_buf[21]='-';
            Hex_ASCII(Time[5],ext_send_buf+22);
            ext_send_buf[24]=LBUF[9];
            ext_send_buf[25]=LBUF[10];
            ext_send_buf[26]='.';
            ext_send_buf[27]=LBUF[12];
            ext_send_buf[28]=LBUF[13];
            ext_send_buf[29]='.';
            Hex_ASCII(Time[0],ext_send_buf+30);

            STRCPY(ext_send_buf+32,"Col.0000    ");
            if(ANALYZER_COUNT>0)
                Hex_DEC(0,ANALYZER_COUNT,ext_send_buf+36,4);

            LBUF[8]=0x84;            
            
            Disp_Msg_Big(1,LINE7);         
            
            INVERT_DISPLAY=1;  
            Set_Cursor_rtc(LINE7+18);
            Hex_ASCII(Time[0],LBUF+18);
            Display_Char(LBUF[18]);

           
            Set_Cursor_rtc(LINE7+19);
            Display_Char(LBUF[19]);

            INVERT_DISPLAY=0;
                          
            ext_send_buf[43]='?';
            SP_SendString(ext_send_buf,44);

            if(MACHINE_LOCK!='Y')
            {
                if(PBUF[Shift_Index]=='\0')
                {
                    Shift_Index=0;
                    STRNCPY(LBUF,DISPLAY_BUF,16);
                    Display_Invert=~Display_Invert;
                }
                if(Display_Invert==0)
                {
                    EEPROM_ADDR=Dairy_Name_Loc;
                }
                else
                {
                    EEPROM_ADDR=Society_Name1_Loc;

                }
                length=32;
                SEERAMReadString(EEPROM_ADDR,length,(char *)&PBUF);
                PBUF[length]=' ';
                PBUF[length+1]=' ';
                PBUF[length+2]=0x00;

                if(Shift_Index>0)
                    STRNCPY(LBUF,DISPLAY_BUF,16);

                LBUF[15]=PBUF[Shift_Index];
                LBUF[16]=0x00;
                STRNCPY(DISPLAY_BUF,LBUF+1,15);
                NOR_DISPLAY_LAST_LINE=0x60;
                Disp_Msg_Big(0,LINE4);
                NOR_DISPLAY_LAST_LINE=0x00;

                Shift_Index++;
            }
            else
            {
                STRCPY(LBUF," MACHINE LOCKED");
                Disp_Msg_Big(1,LINE4);
            }
        }
    }
 }




void Display_Sangh_Fat_Snf(UINT8 dline)
{
    STRCPY(LBUF,"FAT:      SNF:");
    CLR_Enabled(LBUF+10);
    Hex_DEC(1,Sanghrec.FAT,LBUF+4,4);
    Shift_Left(LBUF+4,4);
    if(Sanghrec.SNF>0)
    {
        Hex_DEC(1,Sanghrec.SNF,LBUF+14,4);    
        Shift_Left(LBUF+14,4);
    }
    Disp_Msg(dline);
}

void Display_Sangh_Record(void)
{
    Display_Boarder3();

    Display_Milk_Type(Sanghrec.MILK_TYPE,LINE3);
    ZERO=1;
    Display_Sangh_Fat_Snf(LINE4);

    STRCPY(LBUF,"QTY(Ltr.):");
    if(WEIGHT_KG=='Y')
    {
        STRCPY(LBUF+4,"Kgs.):");
    }
    Hex_DEC(2,Sanghrec.QTY,LBUF+10,7);
    Shift_Left(LBUF+10,7);
    Disp_Msg(LINE5);

    STRCPY(LBUF,"RATE(Rs.):");
    Hex_DEC(2,Sanghrec.RATE,LBUF+10,6);
    Shift_Left(LBUF+10,6);
    Disp_Msg(LINE6);

}

void Sangh_Sale(UINT8 saletype)
{
    UINT8 e_step=0,s_milk_status,sangh_type;
    UINT16 m_fat,m_snf;
    
    while(1)
    {
        switch(e_step)
        {
            case 0:
                
                if(saletype=='C')
                   sangh_type=1;
                else
                    sangh_type=2;

                Menu_Header(sangh_type,1);

                if(Select_Date_Month_Shift(1)==0xFF)
                    return;

                Menu_Header(sangh_type,0);

                s_milk_status=Select_Milk_Type1(0);
                
                if(s_milk_status==0xFF)
                    return;
                
                if(s_milk_status=='L')
                {
                    e_step=0;
                    break;
                }

                Sanghrec.MILK_TYPE=s_milk_status;
                e_step=1;
                break;
                
            case 1:

                Read_Sangh_Sale();

                Menu_Header(sangh_type,0);

                if(saletype=='C')
                {
                    if(Sanghrec.QTY>0 && Sanghrec.RATE>0)
                    {
                        Buzzer(250);

                        Display_Sangh_Record();

                        STRCPY(LBUF,"ENTRY ALREADY DONE!");
                        Invert_Disp_Msg(LINE8);

                        Wait_Key_Press('C');//e

                        e_step=0;
                        break;
                    }
                }
                else if(saletype=='E')
                {
                    if(Sanghrec.QTY==0 || Sanghrec.RATE==0)
                    {
                        Menu_Header(sangh_type,0);

                        Display_Boarder3();
                        STRCPY(LBUF,"  NO RECORDS EXIST");
                        Disp_Msg(LINE3);

                        STRCPY(LBUF,"  ON GIVEN DATE...!");
                        Disp_Msg(LINE5);

                        Exit_Any_Key(1);

                        e_step=0;
                        break;
                    }
                }
                Sanghrec.MILK_TYPE=s_milk_status;
            
            e_step=2;
            break;

        case 2:

            ZERO=0;
            Menu_Header(sangh_type,0);
            Display_Boarder3();
            
            Display_Milk_Type(Sanghrec.MILK_TYPE,LINE3);

            STRCPY(LBUF,"QTY (Ltr):");
            if(WEIGHT_KG=='Y')
            {
                STRCPY(LBUF+5,"Kgs):");
            }
            ZERO=1;
            Hex_DEC(2,Sanghrec.QTY,Temp_Arr,7);//
            Shift_Left(Temp_Arr,7);
            if(Sanghrec.QTY>0)
                STRNCPY(LBUF+10,Temp_Arr,7);
            Temp_Arr[7]='\0';
            Disp_Msg(LINE5);

            GET_VALUE_STATUS=Get_Numeric(7,2,LINE5+10,(char *)&Temp_Arr);

            if(KBV==KB_LEFT)
            {
                e_step=0;
                break;
            }

            else if(GET_VALUE_STATUS==0xFF)
                return;

            else if(GET_VALUE_STATUS!=0xFF)
            {
                Sanghrec.QTY=Float_Long(Temp_Arr);
                if(Sanghrec.QTY>0)
                e_step=3;
            }
            break;

        case 3:

            if(Sanghrec.FAT==0xFF)
                Sanghrec.FAT=0;
            STRCPY(LBUF,"FAT: ");
            Hex_DEC(1,Sanghrec.FAT,Temp_Arr,4);
            Shift_Left(Temp_Arr,4);
            if(Sanghrec.FAT>0)
                STRNCPY(LBUF+4,Temp_Arr,4);
            Temp_Arr[4]='\0';
            Disp_Msg(LINE7);

            GET_VALUE_STATUS=Get_Numeric(4,1,LINE7+4,(char *)&Temp_Arr);

            if(KBV==KB_LEFT)
            {
                CLEAR_DATA_LINES_FROM(6,6);
                e_step=2;
                break;
            }

            else if(GET_VALUE_STATUS==0xFF)
                return;

            else if(GET_VALUE_STATUS!=0xFF)
            {
                m_fat=Float_Long(Temp_Arr);

                if(m_fat>0 && m_fat<=250)
                {
                    Sanghrec.FAT=m_fat;
                    e_step=4;
                }
            }
            break;

        case 4:

            if(Sanghrec.SNF==0xFF)
                Sanghrec.SNF=0;
          
            Display_Sangh_Fat_Snf(LINE7);
            
            Hex_DEC(1,Sanghrec.SNF,Temp_Arr,4);
            Shift_Left(Temp_Arr,4);
            Temp_Arr[4]='\0';

            GET_VALUE_STATUS=Get_Numeric(4,1,LINE7+14,(char *)&Temp_Arr);

            if(KBV==KB_LEFT)
            {

                CLEAR_DATA_LINES_FROM(6,6);
                e_step=3;
                break;

            }
            else if(GET_VALUE_STATUS==0xFF)
                return;

            else if(GET_VALUE_STATUS!=0xFF)
            {
                m_snf=Float_Long(Temp_Arr);
                if(m_snf>0 && m_snf<=250)
                {
                    Sanghrec.SNF=m_snf;
                    e_step=5;
                }
            }
            break;

        case 5:                 
           
            CollectionRecord.FAT=Sanghrec.FAT;
            CollectionRecord.SNF=Sanghrec.SNF;
            Tablerec.MILK_TYPE=Sanghrec.MILK_TYPE;

            Menu_Header(sangh_type,0);

            if(CollectionRecord.SNF==0)
                Calculate_Rate('N');
            else
                Calculate_Rate('S');

            Sanghrec.RATE=CollectionRecord.RATE;
            
            Please_Wait_Msg(LINE5);
            
            Write_Sangh_Sale();

            SEERAMReadString(Default_Printer_Loc,1,(char *)&sangh_type);
            if(sangh_type=='Y')
                sangh_type='T';
            else
                sangh_type='E';
            Print_Sangh_Sale_Report('R',sangh_type,0);
          
            Display_Sangh_Record();

            STRCPY(LBUF,"   RECORD UPDATED");
            Invert_Disp_Msg(LINE8);

            Delay_Sec(1);
            e_step=0;
            break;
        }
    }
}

void Edit_Shift_Report(void)
{
    Menu_Header('E',1);
    
    Read_Date_Month_Year();

    if(Select_Date_Month_Shift(2)==0xFF)
        return;
   
    CollectionFlag=0;
    EditShiftrec=Shiftrec;
    Milk_Collection('E');
}

UINT8 View_Payment_Register_Record(UINT16 vmembercode,UINT8 milktype)
{
    UINT16 vmembersno;
    INT32 v_net_amount;
    
    ZERO=0;
    
    if(Balancerec.PAID_AMT>0 || Balancerec.CREDIT_AMT>0 ||Balancerec.DEBIT_AMT!=0)
    {
        vmembersno=Read_Member_SNO(vmembercode);
        Read_Member_Details(vmembersno);
        
        Display_Full_Boarder();
        
        STRCPY(LBUF," *PAYMENT REGISTER*");       
        Invert_Disp_Msg(LINE1);
        CURSOR_BLINK=0x00;

        ZERO=0;
        
        Copy_MonthRecDT(LBUF);       
        Disp_Msg(LINE2);

        Hex_DEC(0,Memberrec.C_CODE,LBUF,4);
        LBUF[4]='(';
        LBUF[5]=milktype;
        LBUF[6]=')';
        LBUF[7]=0x90;//':';
        STRNCPY(LBUF+8,Memberrec.NAME,13);
        Disp_Msg(LINE3);

        ZERO=1;
        STRCPY(LBUF,"LITERS |  ");
        Hex_DEC(2,Balancerec.QTY,LBUF+10,10);
        Disp_Msg(LINE4);

        STRCPY(LBUF,"CREDITS|  ");
        Hex_DEC(2,Balancerec.CREDIT_AMT,LBUF+10,10);
        Disp_Msg(LINE5);

        STRCPY(LBUF,"DEBITS |   ");
        Hex_DEC(2,Balancerec.DEBIT_AMT,LBUF+10,10);
        Disp_Msg(LINE6);

        STRCPY(LBUF,"PAID   |   ");
        Hex_DEC(2,Balancerec.PAID_AMT,LBUF+10,10);
        Disp_Msg(LINE7);

        STRCPY(LBUF,"BALANCE|  ");
        LBUF[7]=0x9B;
        v_net_amount=(Balancerec.CREDIT_AMT-(Balancerec.DEBIT_AMT+Balancerec.PAID_AMT));
       
        Hex_DEC(2,v_net_amount,LBUF+9,11);
        Invert_Disp_Msg(LINE8);
        return 1;
    }
    else
        return 2;
}

void Memory_SubMenu(UINT8 memorysubcnt)
{
   if(memorysubcnt=='A')
        Edit_Shift_Report();
   
    if(memorysubcnt=='B')
        Member_Details();
    
   if(memorysubcnt=='C')
        Local_Sale('E','p');
   
    if(memorysubcnt=='D')
        Sangh_Sale('E');     
}

void Memory_Functions(void)
{
    UINT8 memory_count=0,select_fun;
    
    MEMORY_MENU_FUN_CNT=User_Selected_Menu_List('m');

    while(memory_count!=0xFF)
    {
        Select_Function_Menu('m',memory_count);   
        
        if(memory_count==1)
            Back_SubMenu();
     
        memory_count=Function_Selected(MEMORY_MENU_FUN_CNT,memory_count);
       
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            select_fun=SELECT_FUN_CODE[memory_count];
            Memory_SubMenu(select_fun);
            TIME_FLAG=1;
        }
    }
}

void Detailed_Shift_Report(UINT8 searchtype,UINT8 ptype)
{
    UINT8 print_type,m_status;
    
    Menu_Header('d',0);
    
    strcpy(LBUF,"Printing Sequence?");
    Disp_Msg(LINE3);
    
    TIMEWISE=Select_Yes_No(1,7);
    if(TIMEWISE==0xFF)
        return;
    
    Menu_Header('d',1);

    if(Select_Date_Month_Shift(2)==0xFF)
        return;
    
    Menu_Header('d',0);
    
    m_status=Start_End_Members(3);

    if(m_status==0xFF)
        return;
    IncentiveReportFlag=0;
    if(ptype=='T')
    {
        print_type=0;
        if(RunningShift.DAY==Shiftrec.DAY && RunningShift.MONTH==Shiftrec.MONTH && RunningShift.YEAR==Shiftrec.YEAR && RunningShift.DAY_SHIFT==Shiftrec.DAY_SHIFT && WEIGHT_COUNT >ANALYZER_COUNT)
        {
            Display_Boarder3(); 
            STRCPY(LBUF,"Print Only Pending?");
            
            print_type=Confirm_Yes();
            if(print_type==0xFF)
                return;
         }
         Print_Shift_Report(print_type);
    }

    else if(ptype=='U')
        Send_Detailed_Shift_Data('d');

    else if(ptype=='E')  
        Print_Shift_Report(ptype);  
}

UINT8 Selected_Shift_Members(UINT8 disphead)
{
    UINT8 a_step=0,m_status;
    
    while(1)
    {
        switch(a_step)
        {
            case 0:
                
                if(Select_Date_Month_Shift(2)==0xFF)
                    return 0xFF;
                 a_step=1;
                 
                 break;
                 
            case 1:
                
                if(disphead=='A')
                    Menu_Header('A',0);
                if(disphead=='G')
                    Message_Header('G',0);
                
                m_status=Start_End_Members(3);
                
                if(m_status==0xFF)
                    return 0xFF;
                
                else if(m_status=='L')
                    a_step=0;
                else
                    return 1;
                
                break;
        }       
    }
}

void Absent_Members(UINT8 ptype)
{
    Menu_Header('A',1);
    
    if(Selected_Shift_Members('A')!=1)
        return;
    
    Menu_Header('A',0);

    Print_Member_List(ptype,'A');  
}

void Display_Start_Date_Values(UINT8 val,UINT8 temp_year,UINT8 temp_month,UINT8 temp_date)
{
    NOR_DISPLAY_LAST_LINE=0x00;
    ZERO=0;
    CURSOR_BLINK=0x00;
    STRCPY(LBUF,"S_YEAR S_MON  S_DAY ");
    LBUF[6]=0x8E;
    LBUF[13]=0x8F;
    if(val==1)
        Invert_Disp_Msg(LINE3);
    else
    {
        LBUF[0]=LBUF[7]=LBUF[14]='E';
        Invert_Disp_Msg(LINE6);
    }

    STRCPY(LBUF,"     |    |    ");
    LBUF[0]=0x83;
    LBUF[15]=0x85;
    
    Hex_DEC(0,temp_year,LBUF+2,2);
    Hex_DEC(0,temp_year,Temp_Arr,2);
    if(temp_month>0)
    {
        Hex_DEC(0,temp_month,LBUF+7,2);
        Hex_DEC(0,temp_month,Temp_Arr,2);
    }
    if(temp_date>0)
    {
        Hex_DEC(0,temp_date,Temp_Arr,2);
        STRNCPY(LBUF+12,Temp_Arr,2);
    }
    Temp_Arr[2]='\0';
    CURSOR_BLINK=0xC0;
    NOR_DISPLAY_LAST_LINE=0xC0;
    DISPLAY_FIRST_LINE=0x01;
    if(val==1)
    {
        
        Disp_Msg_Big(0,LINE4);
        DISPLAY_FIRST_LINE=0x00;
        Display_Box_Boarder(LINE7);
    }
    else
    {
        DISPLAY_FIRST_LINE=0x01;
        Disp_Msg_Big(0,LINE7);
    }

    CURSOR_BLINK=0x00;
    DISPLAY_FIRST_LINE=0x01;
    NOR_DISPLAY_LAST_LINE=0xC0;
}

UINT8 Start_End_Date_Month(void)
{
    UINT8 e_step=0,p_step,exit_value;
    UINT8 temp_year,temp_month,max_days,temp_date;
   
    Read_Date_Month_Year();
    Day_Monthrec.START_YEAR=Shiftrec.YEAR;
    Day_Monthrec.END_YEAR=Shiftrec.YEAR;

    Day_Monthrec.START_MONTH=Shiftrec.MONTH;
    Day_Monthrec.END_MONTH=Shiftrec.MONTH;

    Day_Monthrec.START_DATE=1;
    Day_Monthrec.END_DATE=Shiftrec.DAY;

    while(1)
    {
        switch(e_step)
        {
            case 0:

                Display_Boarder3();

                Display_Start_Date_Values(1,Day_Monthrec.START_YEAR,0,0);

                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE4+2,(char *)&Temp_Arr);
                
                if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                {
                    exit_value=0xFF;
                    e_step=10;
                    break;
                }

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    temp_year=atoi(Temp_Arr);

                    if(temp_year>0 && temp_year<=Shiftrec.YEAR)
                    {
                        Day_Monthrec.START_YEAR=temp_year;
                        e_step=1;
                    }
                }
                break;

            case 1:

                if(Day_Monthrec.START_YEAR!=Shiftrec.YEAR)
                    Day_Monthrec.START_MONTH=1;

                Display_Start_Date_Values(1,Day_Monthrec.START_YEAR,Day_Monthrec.START_MONTH,0);

                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE4+7,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                {
                    e_step=0;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                {
                    exit_value=0xFF;
                    e_step=10;
                    break;
                }

                else
                {                   
                    temp_month=atoi(Temp_Arr);
                  
                    if(temp_month>=1)
                    {
                        if(Day_Monthrec.START_YEAR!=Shiftrec.YEAR && temp_month<=12)
                           e_step=2;
                        else if(Day_Monthrec.START_YEAR==Shiftrec.YEAR && temp_month<=Shiftrec.MONTH)
                            e_step=2;

                        if(e_step==2)
                            Day_Monthrec.START_MONTH=temp_month;
                    }
                }
                break;

            case 2:

                max_days=Get_Month_Days(Day_Monthrec.START_MONTH,Day_Monthrec.START_YEAR);
                
                temp_date=1;
                if(Day_Monthrec.START_YEAR==Shiftrec.YEAR && Day_Monthrec.START_MONTH==Shiftrec.MONTH)
                {
                    temp_date=Day_Monthrec.START_DATE;                   
                    max_days=Shiftrec.DAY;
                   
                }

                Display_Start_Date_Values(1,Day_Monthrec.START_YEAR,Day_Monthrec.START_MONTH,temp_date);

                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE4+12,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                {
                    e_step=1;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                {
                    exit_value=0xFF;
                    e_step=10;
                    break;
                }

                else
                {
                    Day_Monthrec.START_DATE=atoi(Temp_Arr);

                    if(Day_Monthrec.START_DATE<1 || Day_Monthrec.START_DATE>max_days)
                    {
                        Day_Monthrec.START_DATE=temp_date;
                        e_step=2;
                    }
                    else
                    {
                        e_step=3;
                    }
                }
                break;

             case 3:

                Display_Start_Date_Values(2,Day_Monthrec.END_YEAR,0,0);

                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE7+2,(char *)&Temp_Arr);
                NOR_DISPLAY_LAST_LINE=0x00;
                
                if(KBV==KB_LEFT)
                {
                    CLEAR_DATA_LINES_FROM(6,6);
                    e_step=2;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                {
                    exit_value=0xFF;
                    e_step=10;
                    break;
                }

                else
                { 
                    temp_year=atoi(Temp_Arr);
                  
                    if(temp_year>=Day_Monthrec.START_YEAR && temp_year<=Shiftrec.YEAR)
                    {
                        e_step=4;
                        if(Day_Monthrec.START_YEAR==temp_year)
                             Day_Monthrec.END_MONTH=Day_Monthrec.START_MONTH;
                         else
                             Day_Monthrec.END_MONTH=1;
                    }   
                    if(e_step==4)
                        Day_Monthrec.END_YEAR=temp_year;
                }
                break;

             case 4:

                Display_Start_Date_Values(2,Day_Monthrec.END_YEAR,Day_Monthrec.END_MONTH,0);

                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE7+7,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                {
                    e_step=3;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                {
                    exit_value=0xFF;
                    e_step=10;
                    break;
                }

                else
                {
                    temp_month=atoi(Temp_Arr);
                    
                    if(temp_month>=1 && temp_month<=12)
                    {                  
                        if(Day_Monthrec.END_YEAR>Day_Monthrec.START_YEAR  && (temp_month<1 || temp_month>12)) 
                        {
                            e_step=4;
                            break;
                        }

                        if(Day_Monthrec.END_YEAR==Shiftrec.YEAR)
                        {
                            if(Day_Monthrec.START_YEAR==Shiftrec.YEAR && (temp_month>=Day_Monthrec.START_MONTH && temp_month<=Shiftrec.MONTH))
                            {
                                e_step=5;
                            }      
                            else if(Day_Monthrec.START_YEAR!=Shiftrec.YEAR && temp_month<=Shiftrec.MONTH)
                            {
                                e_step=5;
                            }                                         
                        }
                        else
                        {                    
                            if(Day_Monthrec.END_YEAR>=Day_Monthrec.START_YEAR  && (temp_month>=1 && temp_month<=12))
                            {
                                e_step=5;
                            }

                            if(Day_Monthrec.START_YEAR==Day_Monthrec.END_YEAR && temp_month>=Day_Monthrec.START_MONTH)// || (Day_Monthrec.END_YEAR>Day_Monthrec.START_YEAR))
                            {
                                e_step=5;
                            }
                        }

                        if(e_step==5)
                            Day_Monthrec.END_MONTH=temp_month;
                    }
                }
                break;

            case 5:

                if(Day_Monthrec.END_MONTH==HEX_ASCII(Time[5]) && Day_Monthrec.END_YEAR==HEX_ASCII(Time[6]))
                    Day_Monthrec.END_DATE=HEX_ASCII(Time[4]);
                else
                    Day_Monthrec.END_DATE=Get_Month_Days(Day_Monthrec.END_MONTH,Day_Monthrec.END_YEAR);
                
                max_days=Day_Monthrec.END_DATE;

                Display_Start_Date_Values(2,Day_Monthrec.END_YEAR,Day_Monthrec.END_MONTH,Day_Monthrec.END_DATE);

                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE7+12,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                {
                    e_step=4;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                {
                    exit_value=0xFF;
                    e_step=10;
                    break;
                }
                else
                {
                    Day_Monthrec.END_DATE=atoi(Temp_Arr);

                    if(Day_Monthrec.END_DATE<1 || Day_Monthrec.END_DATE>max_days)
                    {
                        e_step=5;
                        break;
                    }
                    else
                    {
                        if(Day_Monthrec.START_MONTH==Day_Monthrec.END_MONTH && Day_Monthrec.START_YEAR==Day_Monthrec.END_YEAR)
                        {
                            if(Day_Monthrec.END_DATE>=Day_Monthrec.START_DATE)
                            {
                                exit_value=0x00;
                                e_step=10;
                                break;
                            }
                            else
                            {
                                e_step=5;
                                break;
                            }
                        }
                        exit_value=0x00;
                        e_step=10;
                        break;
                    }

                    break;
                }
                break;

            case 10:

                Reset_Display_Values();
                return exit_value;
        }
    }
}

void View_Payment_Register(void)    
{
    UINT8 view_flag=0,clear_flag=0,itemp,no_members,records_present;    
    UINT16 temp_sno,v_member_code,p_temp_code;
        
    PRESENT_MEMBER_NO=1;  
    
    do
    {
        Disp_Checking_Data(0);
       
        no_members=Get_No_Of_Customers(Fromrec.START_MEMBER,Fromrec.END_MEMBER);

        if(no_members>0)
            records_present=Read_New_Balance_Details(no_members);
        else
            break;
        p_temp_code=0;
        itemp=0;
        KBV=0xFF;
        view_flag=0;
        clear_flag=0;
        
        while(1)
        {
            if(itemp%2==0)
            {
                if(BalanceRecords[(p_temp_code)*2].QTY==0 && BalanceRecords[(p_temp_code)*2].DEBIT_AMT!=0)
                {   
                    BalanceRecords[(p_temp_code)*2+1].DEBIT_AMT=BalanceRecords[(p_temp_code)*2].DEBIT_AMT;
                    BalanceRecords[(p_temp_code)*2].DEBIT_AMT=0;

                }
            }
            Balancerec=BalanceRecords[(p_temp_code)*2+itemp];   
            v_member_code=ReportsMatchrec[p_temp_code].CUST_CODE;

            view_flag=View_Payment_Register_Record(v_member_code,0x42+itemp);   

            if(view_flag==2)
            {
                itemp++;

                if(itemp==2)
                {
                    if(p_temp_code==no_members)
                    {
                        if(clear_flag==0)
                        {
                            no_data();
                            break;
                        }
                        p_temp_code=0;
                    }
                    else
                        p_temp_code++;

                    itemp=0;
                }
                KBV=0xFF;
            }

            if(view_flag==1)
            {
                clear_flag=1;
                KBV=Wait_Key_Press('C');
            }
            if(KBV==KB_LEFT)
                break;

            if(KBV==KB_CANCEL)
                return;
            if(KBV==KB_UP)
            {
                if(itemp==0)
                {
                    if(p_temp_code==0)
                        p_temp_code=no_members;
                    else
                        p_temp_code--;
                    itemp=1;
                }
                else
                    itemp=0;

            }
            else if(KBV==KB_DOWN)
            {
                if(itemp==1)
                {
                    if(p_temp_code==no_members)
                        p_temp_code=0;
                    else
                        p_temp_code++;
                    itemp=0;
                }
                else
                    itemp=1;
            }
        }
    }while(no_members>=100);
}

void Feed_Printer(void)
{
    if(Print_Feed_Status==1)
    {        
        Paper_Feed(1);
        Delay_Sec(3);
    }
}

void Member_Bill(UINT8 ptype)
{
    UINT8 m_step=0,m_status;
    UINT16 m_member_sno;

   // if(Password_Entry('A')!=1)
   //     return;

    while(1)
    {
        Menu_Header('C',0);
       
        switch(m_step)
        {
            case 0:

                if(Start_End_Date_Month()==0xFF)
                    return;

                m_step=1;
                break;

            case 1:

                m_status=Start_End_Members(1);
                
                if(m_status==0xFF)
                    return;
                else if(m_status=='L')
                    m_step=0;
                else
                    m_step=2;


                if(ptype=='U')
                    m_step=3;
                break;

            case 2:

                m_step=3;
                break;

            case 3:

                Display_Boarder3();
                STRCPY(LBUF,"Member Bill Report?");
                
                if(Confirm_Yes()!=1)
                    return;

                Member_Code=Fromrec.START_MEMBER;

                Print_Feed_Status=0;

                Print_LineNo=0;
                Disp_Checking_Data(0);
                USB_Check_Flag='Y';
                
                for(;Member_Code<=Fromrec.END_MEMBER;Member_Code++)
                {
                    m_member_sno=Read_Member_SNO(Member_Code);
                    Read_Member_Status(m_member_sno);

                    if(Memberrec.STATUS=='A')                        
                    {
                        if(ptype=='U')
                        {
                            if(Insert_USB_To()==0)
                            {
                                USB_Check_Flag='Y';
                                return;
                            }
                        }
                      
                        USB_Check_Flag='N';
                        Disp_Checking_Data(Member_Code);
                        Print_MemberBill_Report(m_member_sno,ptype);
                    }
                }
                
                return;
        }
    }
}

void Memberwise_Report(UINT8 rtype,UINT8 ptype)
{
    UINT8 m_step=0,sms_option,m_status;
    UINT16 m_member_sno;

    while(1)
    {
        Menu_Header(rtype,0);

        switch(m_step)
        {
            case 0:

                if(Start_End_Date_Month()==0xFF)
                    return;

                m_step=1;
                break;

            case 1:

                m_status=Start_End_Members(1);
                
                if(m_status==0xFF)
                    return;
                else if(m_status=='L')
                    m_step=0;
                else
                    m_step=2;

                break;

            case 2:

                Member_Code=Fromrec.START_MEMBER;

                Print_Feed_Status=0;
                
                if(rtype=='W')
                {
                    if(ptype=='E')
                    {
                        if (!Initialize_Printer())
                            return;

                        Delay_Sec(1);
                        USBHostShutdown();
                    }
                    Print_LineNo=0;

#ifdef GPRS
                    sms_option=0;

                    Display_Boarder3();
                    if(GPRSrec.GPRS_ON=='Y' &&  ON_LINE=='Y')
                    {
                        STRCPY(LBUF,"Send Message Also?");

                        sms_option=Confirm_Yes();

                        if(sms_option==0xFF)
                            return;
                    }
#endif
                                       
                    Disp_Checking_Data(0);
               
                    for(;Member_Code<=Fromrec.END_MEMBER;Member_Code++)
                    {
                        m_member_sno=Read_Member_SNO(Member_Code);
                        Disp_Checking_Data(Member_Code);
                        
                        if(m_member_sno!=0)
                        {
                            Print_Customerwise_Report(sms_option,m_member_sno,ptype);
                            Delay_Sec(2);
                        }                        
                    }
                    
                    if(ptype=='T')
                        Feed_Printer();
                }
                else
                {
                    if(ptype=='V')
                        View_Payment_Register();
                    
                    else if(ptype=='U')
                    {
                        if(Insert_USB()==0)
                            return;
                    }
                    if(ptype!='V')
                        Print_Payment_Register_Report(ptype);
                }
               
                return;
        }
    }
}

void Daywise_Report(UINT8 ptype)
{
    UINT8 m_step=0,m_status;

    while(1)
    {
        switch(m_step)
        {                
            case 0:
                
                Menu_Header('M',0);
                m_status=Start_End_Date_Month();
                
                if(m_status==0xFF)
                    return;
                
                m_step=1;
                break;

            case 1:

                m_status=Start_End_Members(1);
                
                if(m_status==0xFF)
                    return;
                if(m_status=='L')
                {
                    m_step=0;
                    break;
                }
               
                Print_Daywise_Report(ptype);
                return;
        }
    }
}

void Deduction_Report(UINT8 ptype)
{
    UINT8 d_step=0;
   
    Menu_Header('D',0);
                
    if(Start_End_Members(0)!=0)
        return;
    
    CLEAR_DATA_LINES_FROM(5,8);
    Print_Deduction_Report(ptype);
}

void Print_LS_Report(UINT8 ptype,UINT8 reporttype)
{
    UINT8 l_milk_status,lstep=0;
    
    while(1)
    {
        switch(lstep)
        {
            case 0:
                
                 Menu_Header(reporttype,0);

                if(Start_End_Date_Month()==0xFF)
                    return;
                
                 lstep=1;
                 break;
                 
            case 1:
                
                l_milk_status=Select_Milk_Type1(1);
               
                if(l_milk_status==0xFF)
                    return;
                
                if(l_milk_status!='L')
                {
                    if(reporttype=='L')
                        Print_Local_Sale_Report(ptype,l_milk_status);
                    
                    if(reporttype=='S')
                        Print_Sangh_Sale_Report('A',ptype,l_milk_status);
                }
                lstep=0;
                break;
        }
    }
}

void Member_List(UINT8 rtype,UINT8 ptype)
{
    UINT8 m_step=0,m_status;
    
    Import_Export_Header(rtype);
    
    if(NO_OF_CUSTOMERS==0)
    {
        STRCPY(LBUF,"NO MEMBERS ADDED");
        Disp_Msg_Big(0,LINE4);

        Delay_Sec(1);

        return;
    }

    while(1)
    {
        switch(m_step)
        {
            case 0:
                
                if(Start_End_Members(0)!=0)
                    return; 
                
                m_step=1;
                break;
                
            case 1:
                
                Display_Boarder3();
                STRCPY(LBUF,"Print Member List?");
                if(Confirm_Yes()==1)
                    m_step=2;
                else
                    m_step=0;
                break;
                
            case 2:
                
                if(rtype=='M')
                    m_status='T';
                else
                    m_status='I';
                
                Print_Member_List(ptype,m_status);
              
                return;
        }
    }      
}

void View_Summary_Shift_Report(void)
{
    STRCPY(LBUF,"SAM|                 ");
    ZERO=1;
    
    Hex_DEC(0,Cumrec.B_MEMBERS,LBUF+8,4);
    Hex_DEC(0,Cumrec.C_MEMBERS,LBUF+16,4);
    Disp_Msg(LINE3);
    
    Calculate_Avg_Fat_Snf_Rate();

    STRCPY(LBUF,"FAT|            ");
    
    Hex_DEC(2,Cumrec.B_FAT,LBUF+7,5);
    Hex_DEC(2,Cumrec.C_FAT,LBUF+15,5);
    Disp_Msg(LINE4);

    STRCPY(LBUF,"SNF|            ");
    CLR_Enabled(LBUF);
   
    Hex_DEC(2,Cumrec.B_SNF,LBUF+7,5);
    Hex_DEC(2,Cumrec.C_SNF,LBUF+15,5);
    Disp_Msg(LINE5);

    STRCPY(LBUF,"QTY|        ");    
    Hex_DEC(2,Cumrec.B_QTY,LBUF+4,8);

    if(Cumrec.B_QTY>9999999)
        Hex_DEC(1,Cumrec.B_QTY,LBUF+4,8);
   
    Hex_DEC(2,Cumrec.C_QTY,LBUF+12,8);

    if(Cumrec.C_QTY>9999999)
        Hex_DEC(1,Cumrec.C_QTY,LBUF+12,8);

    Disp_Msg(LINE6);

    STRCPY(LBUF,"RAT|          ");    
    Hex_DEC(2,Cumrec.B_RATE,LBUF+6,6);
    Hex_DEC(2,Cumrec.C_RATE,LBUF+14,6);
    Disp_Msg(LINE7);

    STRCPY(LBUF,"AMT|        ");    
    Hex_DEC(2,Cumrec.B_AMOUNT+Cumrec.B_INC_AMT,LBUF+4,8);

    if(Cumrec.B_AMOUNT+Cumrec.B_INC_AMT>9999999)
        Hex_DEC(1,Cumrec.B_AMOUNT+Cumrec.B_INC_AMT,LBUF+4,8);    

    Hex_DEC(2,Cumrec.C_AMOUNT+Cumrec.C_INC_AMT,LBUF+12,8);

    if(Cumrec.C_AMOUNT+Cumrec.C_INC_AMT>9999999)
        
        Hex_DEC(1,Cumrec.C_AMOUNT+Cumrec.C_INC_AMT,LBUF+12,8);

    Disp_Msg(LINE8);
     ZERO=0;
}

UINT8 Display_View_Summary_Report(UINT8 reptype)
{    
    if(Cumrec.C_MEMBERS>0 || Cumrec.B_MEMBERS>0)
    {
        Display_Full_Boarder();
        if(reptype==0)
            Display_Shift(LINE1);     
        else
        {
            STRCPY(LBUF,"Date:");
            Copy_ShiftDT(LBUF+5);     
            LBUF[13]=0x00;
            Disp_Msg(LINE1);
        }
        STRCPY(LBUF,"*********BUF*****COW");                                
        Invert_Disp_Msg(LINE2);

        View_Summary_Shift_Report();
        
        KBV=Wait_Key_Press('C');

        if(KBV==KB_CANCEL)
            return 0xFF;        
    }
    return 1;
}

void Print_Daywise_Summary_Date(UINT8 printertype)
{
    STRCPY(PBUF,"DATE:");
    Copy_ShiftDT(PBUF+5);     
    PBUF[13]=0x00;      
    USB_Print_SendString(printertype,0,13);
}

void Print_Summary_Report(UINT8 reptype,UINT8 sflag)
{
    if(sflag==0)
    {
        Start_Printer();
        if(reptype==0)
            STRCPY(PBUF,"*SUMMARY SHIFT REPORT*");
        else
            STRCPY(PBUF,"*DAYWISE SUMMARY REPORT*");
        Printer_SendString(0x04,29);

        Header_Footer_Print('T',0,4);
    }
    if(sflag>0)
        PRINT_UNDERSCORE();
    
    if(reptype==0)
        Print_Date('s');
    else
    {
        Print_Daywise_Summary_Date('T');
    }
    Print_Average_Total('T');
    Paper_Feed(1);
}

void Ext_Print_Summary_Report(UINT8 reptype,UINT8 sflag)
{
    if(sflag==0)
    {
        CLEAR_DATA_LINES_FROM(6,7);
        if(!Initialize_Printer())
            return;

        Ext_Print_Summary_Head(reptype);
    }
    
    if(reptype==0)
        Ext_Print_Date('S');   
    else
        Print_Daywise_Summary_Date('E');
    Ext_Print_Average_Total(1);
}

void Send_Summary_Report(UINT8 reptype,UINT8 sflag)
{
    UINT8 rep_flag;
    if(sflag==0)
    {
        Read_Member_CODE(1);
        Fromrec.START_MEMBER=Memberrec.C_CODE;
        Read_Member_CODE(NO_OF_CUSTOMERS);
        Fromrec.END_MEMBER=Memberrec.C_CODE; 
        if(reptype==0)
            rep_flag='s';
        else
            rep_flag='S';
        if(Insert_USB_To_FileName(rep_flag)==0)
             return;

        Send_Average_Header();
    }

    Send_Avg_Total(reptype);
    USB_Check_Flag='N';       
}

void Summary_Report(UINT8 shifttype,UINT8 reporttype,UINT8 printtype)
{
    UINT16 s_member_sno,s_total_members,noofrecords;
    UINT8 p_shift,s_month,read_month,read_date,read_year,s_date,e_month,start_flag;
    UINT8 summary_report_type,shift_data_present=0;

    s_total_members=NO_OF_CUSTOMERS;
    
    Display_Screen_Boarder();

    if(reporttype=='U')
        STRCPY(LBUF," *SEND SUMMARY*");

    else if(reporttype=='p')
    {
        STRCPY(LBUF,"*SHIFT SUMMARY*");
        Disp_Header_Boarder();
    }

    else
        STRCPY(LBUF," *VIEW SUMMARY*");
    if(reporttype!='p')
        Disp_Header_No_Boarder();
    summary_report_type=0;
    if(shifttype=='A')
    {
        if(Start_End_Date_Month()==0xFF)
            return;
        
        Display_Boarder3();

        STRCPY(LBUF,"Select Report Type?");
        Disp_Msg(LINE3);

        summary_report_type=Select_Yes_No(0,8);

        if(summary_report_type==0xFF)
        {            
            return;
        }
    }
    else if(shifttype=='S')
    {
        if(printtype!='C')
        {
            if(Select_Date_Month_Shift(2)==0xFF)
                return;
        }
        Day_Monthrec.END_YEAR=Day_Monthrec.START_YEAR=Shiftrec.YEAR;
        Day_Monthrec.END_MONTH=Day_Monthrec.START_MONTH=Shiftrec.MONTH;
        Day_Monthrec.END_DATE=Day_Monthrec.START_DATE=Shiftrec.DAY;
    }
    Reset_Display_Values();

    if(reporttype=='U')
    {
       if(Insert_USB_To()==0)
        {
            USB_Check_Flag='Y';
             return;
        }
    }
    start_flag=0;
    Please_Wait_Msg(LINE6);
    
    for(read_year=Day_Monthrec.START_YEAR;read_year<=Day_Monthrec.END_YEAR;read_year++)
    {
        if(read_year==Day_Monthrec.END_YEAR)
        {
            e_month=Day_Monthrec.END_MONTH;
        }
        else
            e_month=12;
        
        if(read_year==Day_Monthrec.START_YEAR)
        {
            s_month=Day_Monthrec.START_MONTH;
        }
        else
            s_month=1;

        
        for(read_month=s_month;read_month<=e_month;read_month++)
        {
            s_date=1;
            read_date=Get_Month_Days(read_month,read_year);

            if(read_year==Day_Monthrec.START_YEAR && read_month==Day_Monthrec.START_MONTH)
                s_date=Day_Monthrec.START_DATE;

            if(read_year==Day_Monthrec.END_YEAR && read_month==Day_Monthrec.END_MONTH)
                read_date=Day_Monthrec.END_DATE;

            //Shiftrec.DAY=read_date;
            Shiftrec.MONTH=read_month;
            Shiftrec.YEAR=read_year;
            
            for(Shiftrec.DAY=s_date;Shiftrec.DAY<=read_date;Shiftrec.DAY++)
            {
                if(summary_report_type==1)
                {
                    Init_Cum_Values();
                    shift_data_present=0;
                }
                
                for(p_shift=0;p_shift<2;p_shift++)
                {
                    if(shifttype=='A')
                    {
                        if(p_shift==0)
                            Shiftrec.DAY_SHIFT='M';
                        else
                            Shiftrec.DAY_SHIFT='E';
                    }
                    if(summary_report_type!=1)
                    {
                        shift_data_present=0;                        
                        Init_Cum_Values();
                    }
                    if(Read_Day_Flag('C')==1)
                    {
                        shift_data_present=1;
                    }
                    
                    if(shift_data_present==1)
                    {    
                        s_member_sno=10000;

                        noofrecords=DynamicRead_BillDetails(s_member_sno);
                        while(noofrecords>0)
                        {
                            noofrecords=noofrecords-1;
                            CollectionRecord=CollectionRecords[noofrecords];
                            Read_Collection_Flags();
                            if(CollectionRecord.QTY!=0xFFFF && CollectionRecord.FAT!=0xFF)
                            {
                                Calculate_Amount();
                                
                                Calc_Cum_Avg();//2
                            }
                        }
                    }

                    if(summary_report_type==0 || (summary_report_type==1 && p_shift==1))
                    {
                        ZERO=0;
                       
                        if((Cumrec.C_MEMBERS+Cumrec.B_MEMBERS)>0)
                        {
                            if(reporttype=='v')
                            {
                                start_flag=Display_View_Summary_Report(summary_report_type);

                                if(start_flag==0xFF)
                                    return;                               
                            }
                            else if(printtype=='T')
                            {
                                Print_Summary_Report(summary_report_type,start_flag);
                            }
                            else if(printtype=='E')
                            {
                                Ext_Print_Summary_Report(summary_report_type,start_flag);
                            }
                            else if(reporttype=='U')
                            {
                                Send_Summary_Report(summary_report_type,start_flag);                                 
                            }
                            start_flag=start_flag+1; 
                            
                            if(shifttype=='S' && (printtype=='S' || GPRSrec.AUTO_SMS=='Y'))
                            {
                                Display_Shift(LINE4);
                                if(printtype=='S' || printtype=='C')
                                    Calculate_Avg_Fat_Snf_Rate();

                                SMS_Summary_Report();

                            } 
                            ZERO=0;
                        }
                    }   
                    if(shifttype=='S')  //single shift                      
                        break;                     
                }             
            }
        }
    }

    if(start_flag>=1)
    {
        if(shifttype=='A' || shifttype=='S')
        {
            if(printtype=='T')
            {
                Paper_Feed(1);
            }
                
            else if(printtype=='E')
            {
                Ext_Printer_Stop();
            }
            else if(reporttype=='U' && USB_Check_Flag!='Y')
                FSfclose(myFile);
        }
    }
    else
    {
        no_data();
    }
}

void Print_SubMenu(UINT8 printsubcnt,UINT8 ptype)
{       
    switch(printsubcnt)
    {
        case 'A':

            Absent_Members(ptype);
            return;

        case 'B':

            Collection_Receipt(ptype);
            return;
            
        case 'C':
        
            Deduction_Report(ptype);
            return;
            
        case 'D':

            TIME_FLAG=0;
            Detailed_Shift_Report('d',ptype);
            TIME_FLAG=1;
            return;
        
        case 'E':
            
            Summary_Report('A','p',ptype);
            return;

        case 'F':
            
            Member_List('M',ptype);
            return;
            
        case 'G':

            Member_List('I',ptype);     //Inactive Member List
            return;

        case 'H':

            Print_LS_Report(ptype,'L');
            return;

        case 'I':

            Print_LS_Report(ptype,'S');
            return;
            
        case 'J':

            Member_Bill(ptype);
            break;

        case 'K':

            Memberwise_Report('W',ptype);
            return;     

        case 'L':

            Memberwise_Report('w',ptype);
            return;
            
        case 'M':

            Daywise_Report(ptype);
            break;
            
       
        case 'N':

            Bonus_Report(ptype);
            return;
    }
}

void Print_Functions(UINT8 ptype)
{
    UINT8 print_cnt=0,select_fun;

    if(ptype=='E')
    {
        if (!Initialize_Printer())
            return;

        Delay_Sec(1);
        USBHostShutdown();
        SEERAMReadString(Ext_Printer_Type_Loc,1,(char *)&Ext_Print_L);
    }
    
    PRINT_MENU_FUN_CNT=User_Selected_Menu_List(ptype);
   
    while(print_cnt!=0xFF)
    {
        Select_Function_Menu(ptype,print_cnt);
        
        if(ptype=='E')
        {
            STRCPY(LBUF,"80 COL. PRINTER ");
            Hex_DEC(0,Ext_Print_L,LBUF,2);            
            Disp_Msg(LINE5);
            
        }

        print_cnt=Function_Selected(PRINT_MENU_FUN_CNT,print_cnt);
      
        if(KBV==KB_ENT)
        {          
            TIME_FLAG=0;
            select_fun=SELECT_FUN_CODE[print_cnt];
            Print_SubMenu(select_fun,ptype);
            TIME_FLAG=1;
        }          
    }
}

//DATA TRANSFER FUNCTIONS


void Tables_SubMenu(UINT8 subtablecnt,UINT8 headertype)
{
    UINT8 usb_status,spl_shift_file;
    
    USB_Check_Flag='Y';
    TIME_FLAG=0;
    SEERAMReadString(SPL_Shift_File_Loc,1,(char *)&spl_shift_file);
   
    if(subtablecnt==0)
    {
        usb_status=Export_Fat_Table('B',1);
        
        if(usb_status==0xFF)
            return;
        
        Export_SNF_Table('B',0);
        Export_Fat_Table('C',0);
        Export_SNF_Table('C',0);        
    }

    if(subtablecnt==1)
        Export_Fat_Table('B',1);    

    if(subtablecnt==2)
        Export_SNF_Table('B',1);
    
    if(subtablecnt==3)
        Export_Fat_Table('C',1);
    
    if(subtablecnt==4)
        Export_SNF_Table('C',1);
    
    if(subtablecnt==5)
    {
        Detailed_Shift_Report('d','U');
            
        if(spl_shift_file=='Y')
            Send_Detailed_Shift_Data('E');
    }

    if(subtablecnt==6)
        Member_Bill('U');
    
    if(subtablecnt==7)
        Export_Members();
    
    if(subtablecnt==8)
        Memberwise_Report('w','U');

    if(subtablecnt==9)
        Summary_Report('A','U',0);
    
    if(subtablecnt==10)
    {    
        Send_Total_Shift_Data('U');
        if(spl_shift_file=='Y')
            Send_Total_Shift_Data('e');
        Fetch_Customer_Flag=0;
    }
    TIME_FLAG=1;
}

void Export_Data(UINT8 headertype)
{
    UINT8 export_count=0;

    while(export_count!=0xFF)
    {
        Data_Transfer_Header('E');
        Back_Select();
        
        if(export_count<=4)
            Display_Table_Names(export_count);
        else
        {
          
            PTR_ADDR=(char *)&EXPORT_DATA_MENU_LIST[export_count-5][0];
            strncpy(LBUF,PTR_ADDR,16);
            Disp_Msg_Boarder();
        }

        
        export_count=Function_Selected(11,export_count);
        if(KBV==KB_ENT)
        {
            Tables_SubMenu(export_count,headertype);
            USBHostShutdown();
        }
    }
}

void Effect_Tables_SubMenu(UINT8 effectcnt)
{
    UINT8 usb_status;

    Display_Full_Boarder();

    switch(effectcnt)
    {
        case 0:

            usb_status=Import_FatTable('C',1);
            if(usb_status==0xFF)
                return;

            Import_FatTable('B',0);
            Import_SNFTable('C',0);
            Import_SNFTable('B',0);
            break;

        case 1:

            Import_FatTable('B',1);
            break;

        case 2:

            Import_SNFTable('B',1);
            break;

        case 3:

            Import_FatTable('C',1);
            break;

        case 4:

            Import_SNFTable('C',1);
            break;
            
        case 5:
            TIME_RIGHT_ARROW='Y';
            SetDate_Time('C'-0x42);
            break;
            
        case 6:
            TIME_RIGHT_ARROW='Y';
            SetDate_Time('B'-0x42);
            break;
            
        case 7:
            TIME_RIGHT_ARROW='Y';
            SetDate_Time('C'-0x40);
            break;
            
        case 8:
            TIME_RIGHT_ARROW='Y';
            SetDate_Time('B'-0x40);
            break;
    }
    TIME_RIGHT_ARROW='N';
}

void Display_Table_Names(UINT8 dtype)
{
        
    PTR_ADDR=(char *)&IMPORT_DATA_MENU_LIST[dtype][0];
    strncpy(LBUF,PTR_ADDR,16);
    if(dtype==2 || dtype==4)
        CLR_Enabled(LBUF+7);
    Disp_Msg_Boarder();
    
    if(dtype==5)
        Back_SubMenu();
    
    else if(dtype<=6)
        Back_Select();
}

void Effect_Rate_Chart(void)
{
    UINT8 effect_count=0;

    while(effect_count!=0xFF)
    {
        Data_Transfer_Header('e');

        if(effect_count<=4)
            Display_Table_Names(effect_count);
        else
        {
          
            PTR_ADDR=(char *)&EFFECT_DATES_MENU_LIST[effect_count-5][0];
            strncpy(LBUF,PTR_ADDR,16);
            Disp_Msg_Boarder();
            
            STRCPY(LBUF,"Effective Date From");
            Disp_Msg(LINE5);
        }

        effect_count=Function_Selected(9,effect_count);
        if(KBV==KB_ENT)
        {
            EFFECT_RATE=1;
            Effect_Tables_SubMenu(effect_count);
            EFFECT_RATE=0;
            USBHostShutdown();
        }
    }
}

void I_Tables_SubMenu(UINT8 subtablecnt)
{
    UINT8 usb_status;

    EFFECT_RATE=0;

    Display_Full_Boarder();

    switch(subtablecnt)
    {
        case 0:

            usb_status=Import_FatTable('B',1);
            if(usb_status==0xFF)
                return;
             
            Import_SNFTable('B',0);
            Import_FatTable('C',0);            
            Import_SNFTable('C',0);
            break;


        case 1:

            Import_FatTable('B',1);
            break;
            
        case 2:

            Import_SNFTable('B',1);
            break;
            
        case 3:

            Import_FatTable('C',1);
            break;

        case 4:

            Import_SNFTable('C',1);
            break;

        case 5:

            Effect_Rate_Chart();
            break;

        case 6:

            Download_Members();
            break;
    }
}

void Import_Data(UINT8 headertype)
{
    UINT8 import_count=0,start_count,no_functions;

    no_functions=7;
    
    if(DPUcontrolrec.MEMBER_LIST=='N')
        no_functions=6;
        
    if(DPUcontrolrec.RATE_TABLES=='Y')
    {
        import_count=0;
        start_count=0;
    }

    while(1)
    {
        Data_Transfer_Header('I');

        if(DPUcontrolrec.RATE_TABLES=='N')
        {
            import_count=6;
            start_count=6;
        }

        Display_Table_Names(import_count);

        KBV=Wait_Key_Press('N');

        if(KBV==KB_CANCEL)
            return;
        if(KBV>=(0x31+start_count) && KBV<=(0x30+no_functions))
        {
            import_count=KBV-0x31;
            KBV=KB_ENT;
        }
        if(KBV==KB_ENT)
        {
            I_Tables_SubMenu(import_count);
            USBHostShutdown();            
        }
        import_count=Up_Down(import_count,no_functions);

        if(DPUcontrolrec.MEMBER_LIST=='N' && import_count==6)
        {
            if(KBV==KB_UP)
              import_count=5;
            else
              import_count=0;
        }
    }
}



void Pendrive_SubMenu(UINT8 subcnt)
{
    USB_Check_Flag='Y';
    TIME_FLAG=0;
    if(subcnt==0)
        Export_Data('E');
    if(subcnt==1)
        Import_Data('I');
    
    TIME_FLAG=1;
}

void Pendrive_Transfer(void)
{
    UINT8 p_count=0;

    while(p_count!=0xFF)
    {
        Data_Transfer_Header('P');

        TIME_FLAG=1;
      
        PTR_ADDR=(char *)&PENDRIVE_MENU_LIST[p_count][0];
        strncpy(LBUF,PTR_ADDR,14);
        Disp_Msg_Boarder();

        PTR_ADDR=(char *)&PENDRIVE_MENU_DESC_LIST[p_count][0];
        strncpy(LBUF,PTR_ADDR,19);
        Disp_Msg(LINE5);

        p_count=Function_Selected(2,p_count);
        if(KBV==KB_ENT)
        {
            Pendrive_SubMenu(p_count);
            USBHostShutdown();
        }
        
        if(p_count!=0xFF)
        {
            if(DPUcontrolrec.RATE_TABLES=='N' && DPUcontrolrec.MEMBER_LIST =='N')
            {
                p_count=0;
            }
        }
    }
}

UINT8 Server_SubMenu(UINT8 ssmcount)
{
    switch(ssmcount)
    {       
        case 0:

            MODEM_ERROR=Transfer_Shift_Data('S');
            Fetch_Customer_Flag=0;
            return MODEM_ERROR;

        case 1:

            EFFECT_RATE=1;

            MODEM_ERROR=Receive_Rate_Charts();
            EFFECT_RATE=0;
            return MODEM_ERROR;
        
        case 2:
            
            MODEM_ERROR=Receive_Members1();
            return MODEM_ERROR;
    }
}

void Server_Messages(void)
{
    Display_Boarder3();

    if(MODEM_ERROR==0xFF)
        STRCPY(LBUF,"Transaction Canceled");
    
    else if(MODEM_ERROR==0x00)
        STRCPY(LBUF,"No Resp./Check Modem");
    
    else if(MODEM_ERROR==0x01)
        STRCPY(LBUF,"    Check Modem   ");
    
    else if(MODEM_ERROR==0x02)        
#ifdef WIFI
    STRCPY(LBUF,"      Check WIFI   ");
#elif defined(GPRS)
    STRCPY(LBUF,"      Check SIM   ");
#endif
    
    else if(MODEM_ERROR==0x30)
        STRCPY(LBUF," Please Try Later ");
    
    else if(MODEM_ERROR==0x08  || MODEM_ERROR==0x03)
        STRCPY(LBUF,"Modem Response Error");
    
    else if(MODEM_ERROR>=0x11 && MODEM_ERROR<=0x15)
        STRCPY(LBUF,"Connection Problem");
    
    else if(MODEM_ERROR==0x20)
        STRCPY(LBUF,"  Machine Not Added");
    
    else if(MODEM_ERROR==0x21)
        STRCPY(LBUF,"Communication Error");

    else if(MODEM_ERROR==0x22)
        STRCPY(LBUF,"High MemberCapacity");
    
    else if(MODEM_ERROR==0x23)
        STRCPY(LBUF,"No Settings Received");
    
    else if(MODEM_ERROR=='S')
        STRCPY(LBUF,"Transaction Success");
    
    else
        STRCPY(LBUF,"  Transaction Error");
    
    Disp_Msg(LINE5);
    Buzzer(250);
    Buzzer(250);
    Buzzer(250);
}

void Server_Communication(UINT8 sctype)
{
    UINT8 g_count,totalfuns=0x33;
    
    SEERAMReadString(MemberList_Control_Loc,3,(char *)&DPUcontrolrec);

    
    if(DPUcontrolrec.RATE_TABLES=='Y')
        totalfuns=totalfuns-1;
    
    if(DPUcontrolrec.MEMBER_LIST=='Y')
        totalfuns=totalfuns-1;

    g_count=sctype;

    while(1)
    {
        Clear_LCD();
        if(sctype==1)
        {
            Server_Menu_Header('S');
            SERVER_C_TYPE='B';
        }
        else
        {
            Display_Box_Line4();
            STRCPY(LBUF,"*GPRS COMMUNICATION*");
            Disp_Msg(LINE1);
            SERVER_C_TYPE='S';
            
            Back_Select();
        }
        TIME_FLAG=1;

        switch(g_count)
        {
            case 0:

                STRCPY(LBUF," 1.TRANSFER DATA");

                break;

            case 1:

                if(sctype==1)
                    break;
               
                STRCPY(LBUF,"2.RECEIVE CHARTS");
                break;

            case 2:

                STRCPY(LBUF,"3.RECEIVE MEMBERS");                
                if(DPUcontrolrec.RATE_TABLES=='Y' )
                    LBUF[0]='2';
                break;
        }
        Disp_Msg_Boarder();
        
        
        if(sctype==0)
            KBV=Wait_Key_Press('N');
        else
            KBV=KB_ENT;
        if(KBV==KB_CANCEL)
            return;
        if(KBV>=0x31 && KBV<=totalfuns)
        {
            g_count=KBV-0x31;
            if(DPUcontrolrec.RATE_TABLES=='Y' && g_count==1)
                g_count=2;
            KBV=KB_ENT;
        }
        if(KBV==KB_ENT)
        {
            CLEAR_DATA_LINES_FROM(3,5);
                            
            if(g_count==1 && sctype==0)
            {
                Server_Menu_Header('R');

            }
            if(g_count==2)
            {
                Member_Header('m');
            }

            if(g_count!=0)
            {
                MODEM_ERROR=Connect_GPRS();
                if(MODEM_ERROR!='S')
                {                    
                    Server_Messages();

                    if(CONNECT_FLAG==1)
                    {
                        Disconnect_Modem();
                        CONNECT_FLAG=0;
                    }
                    if(sctype==1)
                        return;
                    break;
                }
            }
            MODEM_ERROR=Server_SubMenu(g_count);
            Server_Messages();
            if(CONNECT_FLAG==1)
            {
                Disconnect_Modem();
                CONNECT_FLAG=0;

                if(g_count!=2)
                    Check_New_RateChart();
            }

            if(sctype==1)
            {
               return;
            }
        }

        g_count=Up_Down(g_count,3);
        
        if(DPUcontrolrec.MEMBER_LIST=='Y' && g_count==2)
        {
            if(KBV==KB_UP)
                g_count=1;
            else
                g_count=0;
        }
        
        if(DPUcontrolrec.RATE_TABLES=='Y' && g_count==1)
        {
            if(KBV==KB_UP)
                g_count=0;
            else
            {
                if(DPUcontrolrec.MEMBER_LIST=='Y')
                    g_count=0;
                else
                    g_count=2;
            }
        }
    }
}

void Data_Transfer_SubMenu(UINT8 subpccnt)
{
    if(LOGIN_TYPE=='O')
        return;

    if(subpccnt==0)
        Pendrive_Transfer();
    
    if(subpccnt==1)         
        Server_Communication(0);
}

void Data_Transfer(void)
{
    UINT8 t_count=0;
        
    if(ON_LINE=='Y')
    {
        if(LOGIN_TYPE=='O')
            U_DATA_TRANSFER_MENU_ACCESS[1]=1;
        else
            DATA_TRANSFER_MENU_ACCESS[1]=1;
    }
    
    DATA_TRANSFER_FUN_CNT=User_Selected_Menu_List('C');
      
    SEERAMReadString(MemberList_Control_Loc,3,(char *)&DPUcontrolrec);
    
    while(t_count!=0xFF)
    {               
        Select_Function_Menu('C',t_count);
        KBV=0xFF;
        
        t_count=Function_Selected(DATA_TRANSFER_FUN_CNT,t_count);
        
        if(KBV==KB_ENT)
        {           
            Data_Transfer_SubMenu(t_count);
            TIME_FLAG=1;
        }        
         
    }
}

void Group_SMS(void)
{
    UINT16 g_member_sno,noofrecords,pending_sms=0;

     Message_Header('G',1);
     
     if(Selected_Shift_Members('G')!=1)
         return;

    Message_Header('G',0);

    Disp_Checking_Data(0);
    
    noofrecords=DynamicRead_BillDetails(10000);
    pending_sms=0;
    while(noofrecords>0)
    {
        CollectionRecord=CollectionRecords[noofrecords-1];
        CollectionRecord_NO.RECORDNO=CollectionRecords_NOS[noofrecords-1].RECORDNO;
        CollectionRecord_NO.Type=CollectionRecords_NOS[noofrecords-1].Type;
        Read_Collection_Flags();
        if(CollectionFlags.SMS_STATUS==1 && CollectionRecord.CODE>=Fromrec.START_MEMBER && CollectionRecord.CODE<=Fromrec.END_MEMBER)
        {
            
            pending_sms=1;
            g_member_sno=Read_Member_SNO(CollectionRecord.CODE);
            Read_Member_Ph(g_member_sno);
            STRCPY(LBUF,"CODE:1234           ");
            Hex_DEC(0,CollectionRecord.CODE,LBUF+5,4);
            STRNCPY(LBUF+10,(char *)&Memberrec.CONTACT_NO,10);            
            Invert_Disp_Msg(LINE7);
            
#ifdef GPRS
            do
            {
                Send_SMS_BackEnd('c',g_member_sno);
             }while(SMS_STATE!=0);
#endif
        }
         noofrecords--;
    }
    if(pending_sms==0)
    {
        STRCPY(LBUF,"No Pending Messages");
        Invert_Disp_Msg(LINE8);
        Delay_Sec(1);
    }
}

void Balance_Command(void)
{
    UINT8 b_cnt,n_cnt,invalid_cmd=0;

    Display_Screen_Boarder();

    STRCPY(LBUF," *CHECK BALANCE*");
    Disp_Header_No_Boarder();

    if(SIM_INSERTED==0)
    {
        STRCPY(LBUF,"  INSERT SIM  ");
        Disp_Msg_No_Spaces(0,LINE5+1);
        Delay_Sec(1);
        return;
    }

    STRCPY(Temp_Arr,"               ");
    GET_VALUE_STATUS=Get_String(15,LINE4,(char *)&Temp_Arr);

    if(GET_VALUE_STATUS==0xFF)
        return;

    if(GET_VALUE_STATUS!=0xFF)
    {
        b_cnt=0;
        
        Please_Wait_Msg_Small(LINE6);
        
        Max_Timer_Count=10;
        STRCPY(SEND_BUF,"ATE0\r");
        GPRS_SendString(SEND_BUF);
        GPRS_RecString_Cmd(REC_BUF);
        if(No_Response==1)
            return ;
        Max_Timer_Count=10;
        STRCPY(SEND_BUF,"AT+CUSD=1,\"*222*2#\"\r");
        n_cnt=11;

        while(1)
        {
            SEND_BUF[n_cnt]=Temp_Arr[b_cnt];

            if(SEND_BUF[n_cnt]=='#' || b_cnt>15)
            {
                if(b_cnt>15)
                    invalid_cmd=1;
                break;
            }

            b_cnt++;
            n_cnt++;
        }
        if(invalid_cmd==0)
        {
            SEND_BUF[n_cnt+1]='\"';
            SEND_BUF[n_cnt+2]='\r';

            Max_Timer_Count=300;

            STRNCPY(LBUF,SEND_BUF,n_cnt+2);        
            GPRS_SendString(SEND_BUF);
            GPRS_RecString_OK();
            GPRS_RecString_OK();
            Max_Timer_Count=10;

            Display_Full_Boarder();

            for(b_cnt=0;b_cnt<8;b_cnt++)
            {
                STRNCPY(LBUF,(GPRSOK_BUF+11+(b_cnt*20)),20);
                Disp_Msg(LINE1+(b_cnt*20));
            }

            Wait_Key_Press('C');
        }
    }
}

void SMS_Summary_Report(void)
{    
    UINT8 pending_sms=0;
    UINT16 g_member_sno,noofrecords=0;

    while(noofrecords<3)
    {
        STRCPY(LBUF," Sending Message...");
        LBUF[19]=0x31+noofrecords;
        Invert_Disp_Msg(LINE8);
        pending_sms=1;
#ifdef GPRS
        do
        {
            Send_SMS_BackEnd('S',noofrecords);
         }while(SMS_STATE!=0);
#endif
         
         noofrecords++;
    }
}

void Message_Sub(UINT8 mcnt)
{
    switch(mcnt)
    {
        case 0:

            Collection_Receipt('S');
            return;

        case 1:

            Group_SMS();
            return;
            
        case 2:
            
            Summary_Report('S','p','S');
            return;
            
        case 3:

            Balance_Command();
            return;
    }
}

void Message(void)
{
    UINT8 m_cnt=0;
   
    SEERAMReadString(MilkType_Print_On_Loc,RECEIPT_RECORD_LENGTH,(char *)&Receiptrec);
    
    while(m_cnt!=0xFF)
    {
        Message_Header('M',1);

        if(SIM_INSERTED==0 || Modem_Init_Flag==0)
        {
            STRCPY(LBUF," Check Modem/SIM");
            LBUF[0]=0x83;
            Disp_Msg_Big(0,LINE4);
            
            Exit_Any_Key(1);
            return ;
        }

        Back_Select();
        STRCPY(PBUF,"Selected Shift  ");
      
        PTR_ADDR=(char *)&MESSAGE_MENU_LIST[m_cnt][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Boarder();

        PTR_ADDR=(char *)&MESSAGE_MENU_DESC_LIST[m_cnt][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg(LINE5);

        m_cnt=Function_Selected(4,m_cnt);
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            Message_Sub(m_cnt);
            TIME_FLAG=1;
        }
    }
}

void View_Shift_Data(UINT16 vmembersno,UINT8 viewtype)
{
    if(CollectionRecord.QTY!=0xFFFF)
    {
        if(viewtype==0 || (viewtype==1 && CollectionRecord.FAT==0xFF))
        {
            Read_Member_Details(vmembersno);            
            Display_Bill_Details('V',0);
        }
    }
}

void View_Shift_Report(void)
{  
    UINT8 view_type;
    UINT16 v_member_sno,v_total_members,view_flag=0,data_flag=0,temprecordno,noofrecords;

    Menu_Header('d',1);
    if(Select_Date_Month_Shift(2)==0xFF)
        return;

    Display_Boarder3();

    view_type=0;
    if(WEIGHT_COUNT>ANALYZER_COUNT)
    {
        STRCPY(LBUF,"View Only Pending?");

        view_type=Confirm_Yes();

        if(view_type==0xFF)
            return;
    }

    v_member_sno=1;
    v_total_members=NO_OF_CUSTOMERS;

    Please_Wait_Msg(LINE5);
    noofrecords=0;
   
    noofrecords=DynamicRead_BillDetails(10000);
    temprecordno=0;
    if(noofrecords==0)
    {
        STRCPY(LBUF,"NO SHIFT DATA...");
        Disp_Msg(LINE4);
        
        Exit_Any_Key(1);
        return;
    }
    
    while(1)
    {
        CollectionRecord=CollectionRecords[temprecordno];
        Read_Collection_Flags();
        
        if(view_type==0 || (view_type==1 && CollectionRecord.FAT==0xFF))
        {
            v_member_sno=Read_Member_SNO(CollectionRecord.CODE);
            Tablerec.MILK_TYPE=CollectionFlags.MILK_TYPE;
            
            View_Shift_Data(v_member_sno,view_type);
            KBV=Wait_Key_Press('E');
            
            view_flag=1;
        }
        
        if(view_flag==0)
        {
            temprecordno++;
            if(temprecordno==noofrecords)
                break;
            KBV=0xFF;
        }
        
        if(KBV==KB_CANCEL)
            return;
        if(KBV==KB_UP )
        {
            view_flag=0;
            if(temprecordno==0)
                temprecordno=noofrecords-1;
            else
                temprecordno--;
        }
        else if(KBV==KB_DOWN)
        {
            view_flag=0;
            if(temprecordno==noofrecords-1)
                temprecordno=0;
            else
                temprecordno++;
        }        
    }
}

void View_Reports_SubMenu(UINT8 vcnt)
{
    if(vcnt=='A')
    {        
        Collection_Receipt('V'); 
        TIME_FLAG=1;
    }

    if(vcnt=='B')
        View_Shift_Report();
            
    if(vcnt=='C')  
        Summary_Report('A','v',0);
    
    if(vcnt=='D')
        Memberwise_Report('w','V');
}

void View_Reports(void)
{
    UINT8 print_cnt=0,select_fun;
    
    VIEW_MENU_FUN_CNT=User_Selected_Menu_List('V');

    while(print_cnt!=0xFF)
    {            
        Select_Function_Menu('V',print_cnt);
        
        print_cnt=Function_Selected(VIEW_MENU_FUN_CNT,print_cnt);
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            select_fun=SELECT_FUN_CODE[print_cnt];
            View_Reports_SubMenu(select_fun);
            KBV=0xFF;
            TIME_FLAG=1;
        }
    }
}

UINT8 Wait_Backend_To_Complete(void)
{
    if(GPRSrec.GPRS_ON=='Y' && ON_LINE=='Y' && SIM_INSERTED==1 && Modem_Init_Flag==1)
    {
        Please_Wait_Msg_Small(LINE5);
        
        BACKEND_FLAG='B';
        while(SMS_STATE!=0 || SERVER_STATE!=0 || SIM_CHECK_STATE!=0 || INIT_MODEM_STATE!=0 )
        {
            KBV=Scan_Key('E');
            if(KBV==KB_CANCEL)
                return 0xFF;
        }
        
            return 1;
        BACKEND_FLAG='G';

    }
    CLEAR_DATA_LINES_FROM(5,5);

}


void Sub_Menu(UINT8 submaincnt)
{
    UINT8 bstatus;
    
    Read_Date_Month_Year();

    if(submaincnt=='A')
        Data_Functions();
    
    if(submaincnt==1 || submaincnt==2 || submaincnt==3 || submaincnt==4 || (submaincnt==5))
    {
        bstatus=Wait_Backend_To_Complete();
        if (bstatus==0xFF)
            return;
    }
    if(submaincnt=='B')
    {       
        Data_Transfer();
    }
      
    if(submaincnt=='C')
    {
        SERVER_STATE=0;
        
        Print_Functions('E');
    }
    if(submaincnt=='D')
    {  
        Print_Functions('T');
    }
    if(submaincnt=='E')
    {
        BACKEND_FLAG='M';
        Memory_Functions();
    }
    
    if(submaincnt=='F')
        View_Reports();
   
    if(submaincnt=='G')
        Settings();
    
    if(submaincnt=='H')
    {
        Rate_Tables();
        EFFECT_RATE=0;
    }
      
    if(submaincnt=='I')
    { 
        BACKEND_FLAG='G';
        Message();         
    }
    
    BACKEND_FLAG='B';
}

void Main_Menu(UINT8 main_count)
{   
    UINT8 select_fun;
        
    SP_SendString("$??",3);

    if(MENU_ACCESS!=1 && Password_Entry1('B')!=1)
        return;
    
      
    Big_RTC=0;
    MENU_ACCESS=1;
    BACKEND_FLAG='B';
           
    if(ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y')
    {
        ADMIN_MAIN_MENU_ACCESS[8]=0;
#ifdef GPRS
        ADMIN_MAIN_MENU_ACCESS[8]=1;
#endif
        
        
        SEERAMReadString(MemberList_Control_Loc,3,(char *)&DPUcontrolrec);
//        if(DPUcontrolrec.RATE_TABLES=='N')
//        {
//            ADMIN_MAIN_MENU_ACCESS[7]=0;
//        }         
//        else
//            ADMIN_MAIN_MENU_ACCESS[7]=1;
    }
    else
    {
        ADMIN_MAIN_MENU_ACCESS[8]=0;
    }
   
    MAIN_MENU_FUN_CNT=User_Selected_Menu_List('M');
      
    ALL_PERMISSIONS=0;
    while(1)
    {       
        Select_Function_Menu('M',main_count);
        
         Back_SubMenu();
        
        main_count=Function_Selected(MAIN_MENU_FUN_CNT,main_count);
        
        if(KBV==KB_CANCEL)
        {
            main_count=0;                
            return;
        }
      
        if(KBV==KB_ENT)
        {
            select_fun=SELECT_FUN_CODE[main_count];
            Sub_Menu(select_fun);
        }           
    }
 }


void Checking_Flash(void)
{
    UINT8 c_flash_status,c_f_id;
    UINT32 Flash_ID;

    STRCPY(LBUF,"CHECKING FLASH");
    Disp_Msg_No_Spaces(0,LINE6+1);

    InitFlash(1);
           
    WREN();
    c_flash_status=Read_Status_Register();

    EWSR();
    WRSR(0x02);
    WREN();
    c_flash_status=Read_Status_Register();
    
    Flash_ID=Jedec_ID_Read();
    
    if(c_flash_status!=0x02 || Flash_ID!=0x00BF2643)
    {
        STRCPY(LBUF,"FLASH Problem ");               
        Disp_Msg_No_Spaces(0,LINE4+1);

        STRCPY(LBUF,"  Restart DPU ");
        Disp_Msg_No_Spaces(0,LINE6+1);

        while(1)
            Buzzer(250);
    }
    else
        NO_OF_FLASHES=12;
    
    Sector_Erase(0);
    STRCPY(LBUF,"SMART");
    FLASHWriteString(0,5,(char *)&LBUF);  
    
    FLASHReadString(0,5,(char *)&Temp_Arr);
    
    if(strncmp(LBUF,Temp_Arr,5)!=0)
    {
        while(1);
    }
  
    STRCPY(LBUF,"  FLASH  O.K  ");
    Disp_Msg_No_Spaces(0,LINE6+1);
}


void Get_MIN_MAX_Code(void)
{
    UINT16 m_temp_char,g_member_sno;

    MIN_CODE=0;
    MAX_CODE=0;

    m_temp_char=NO_OF_CUSTOMERS;
    
    if(m_temp_char>1000)
        m_temp_char=1000;
    
    for(g_member_sno=1;g_member_sno<=m_temp_char;g_member_sno++)
    {
        Read_Member_Status(g_member_sno);
        if(Memberrec.STATUS=='A' || Memberrec.STATUS=='D')
        {
            Read_Member_CODE(g_member_sno);

            if(MIN_CODE>Memberrec.C_CODE || MIN_CODE==0)
                MIN_CODE=Memberrec.C_CODE;

            if(MAX_CODE<Memberrec.C_CODE)
                MAX_CODE=Memberrec.C_CODE;
        }
    }
}

void Clear_Transaction_Counters(void)
{    
    WEIGHT_COUNT=0;
    ANALYZER_COUNT=0;
    SMS_COUNT=0;
    UPLOAD_COUNT=0;
    DU_COUNT=0;
}

void Init_Transaction_Counters(void)
{
    UINT8 uflag=0,sflag=0;
    UINT16 noofrecords=0;
    UINT32 temprecordadr; 
   
    Clear_Transaction_Counters();
    
    do
    {
        Read_Current_Shift_Col_Record(noofrecords);
        
        Read_Collection_Flags();
        if(CollectionRecord.CODE!=0xFFFF)
        {
            if(CollectionRecord.QTY!=0xFFFF)
                WEIGHT_COUNT=WEIGHT_COUNT+1;
            
            if(CollectionRecord.FAT!=0xFF)
                ANALYZER_COUNT=ANALYZER_COUNT+1;
            
            if(CollectionFlags.UPLOAD_STATUS==0)
            {
                DU_COUNT=DU_COUNT+1;
            }
            
            if(CollectionFlags.UPLOAD_STATUS==0 && uflag==0)
                UPLOAD_COUNT=UPLOAD_COUNT+1;
            else
                uflag=1;
            
            if(CollectionFlags.SMS_STATUS==0 && sflag==0)
                SMS_COUNT=WEIGHT_COUNT;
            else
                sflag=1;
        }
        else
            break;
        
        noofrecords++;
        
    }while(1);    
}

void Read_Settings_Menu_Fun_Cnt(void)
{    
    SettingMenurec.DEALER=0;
    SettingMenurec.MASTER=0;
    
    if(LOGIN_TYPE=='D')
    {
        SettingMenurec.DEALER=1;
    }
    
    if(LOGIN_TYPE=='M')
    {
        SettingMenurec.MASTER=1;
    }
}

void Init_Global_Variables(void)
{
    SEERAMReadString(Server_Control_Loc,1,(char *)&SERVER_CONTROL);
    SEERAMReadString(Society_Code_Loc,10,(char *)&SOCIETY_CODE);
    SEERAMReadString(Machine_Code_Loc,7,(char *)&MACHINE_ID);
    
    Device_ID();

    SEERAMReadString(On_Offline_Loc,1,(char *)&ON_LINE);
    FLASHReadString((Global_Variables_Loc+GPRS_ON_Loc),20,(char *)&GPRSrec);
    SEERAMReadString(SMS_Day_Limit_Loc,2,(char *)&SMS_DAY_LIMIT);
    SEERAMReadString(Weight_Kg_Loc,1,(char *)&WEIGHT_KG);
    SEERAMReadString(Machine_Lock_Loc,1,(char *)&MACHINE_LOCK);
    
    if(SMS_DAY_LIMIT>100)
        SMS_DAY_LIMIT=100;

    MENU_ACCESS=0;
    STANDBY=1;
    TIME_FLAG=1;
    SIM_INSERTED=0;
    Modem_Init_Flag=0;
    Big_RTC=1;
    Shift_Index=0;
    Display_Invert=0;
    MODE_FLAG=0;
 
    MODEM_PRESENCE=0;
  
    FONT_TYPE='S';
    Fetch_Customer_Flag=0;
    
    Get_MIN_MAX_Code();

    Status_Bar(LINE8);

    New_Effective_Dates();
    Check_New_RateChart();
    EFFECT_RATE=0;
    WEIGHT_UNIT='K';
    LOGIN_TYPE='O';
    
    SEERAMReadString(Dealer_Selection_Loc,1,(char *)&DEALER_SELECTION);
    
    if(DEALER_SELECTION<0 || DEALER_SELECTION>4)
        DEALER_SELECTION=0;
    
    Select_Dealer_Default_Values(DEALER_SELECTION);

    SEERAMReadString(Two_Analyzers_Mode_Loc,1,(char *)&TWO_ANALYZERS);
    SEERAMReadString(CLR_Based_Table_Loc,1,(char *)&CLR_BASED);
    SEERAMReadString(Brand_Name_Loc,16,(char *)&BRAND_NAME);
    
    SECONDS_COUNTER=5;    
    JUNK_DATA=0;
    
    SEERAMReadString(Accuracy_Loc,WM_RECORD_LENGTH,(char *)&WMrec);     
   
    SEERAMReadString(MilkType_Print_On_Loc,RECEIPT_RECORD_LENGTH,(char *)&Receiptrec);
    SEERAMReadString(Ext_Printer_Type_Loc,1,(char *)&Ext_Print_L);
        
    SHIFT_SEARCH=0;
    TIME_RIGHT_ARROW='N';   
}

void Init_Ext_Display(void)
{
    UINT16 ext_disp_baudrate;
    UINT8 ext_send_buf[50];

    SEERAMReadString(Weight_Mode_Loc,SETTINGS_RECORD_LENGTH,(char *)&Settingsrec);

    if(Settingsrec.ANALYZER=='P')
        ext_disp_baudrate=2400;

    else if(Settingsrec.ANALYZER=='U')
        ext_disp_baudrate=1200;

    else if(Settingsrec.ANALYZER=='L' || Settingsrec.ANALYZER=='M' || Settingsrec.ANALYZER=='F')
    {
        SEERAMReadString(L_Baud_Rate_Loc,2,(char *)&ext_disp_baudrate);

        if(ext_disp_baudrate==0)
            ext_disp_baudrate=2400;
    }
    else if(Settingsrec.ANALYZER=='K' || Settingsrec.ANALYZER=='A' )
        ext_disp_baudrate=9600;

    ext_send_buf[9]='8';
    STRCPY(ext_send_buf,"$369924008     SctLSEdPU1111111111111111111?");

    Hex_DEC(0,DEVICE_CODE,ext_send_buf+10,4);
    Hex_DEC(0,ext_disp_baudrate,ext_send_buf+5,4);
    if(Settingsrec.ANALYZER=='U')
        ext_send_buf[9]='9';
    Hex_ASCII(Time[4],ext_send_buf+24);
    ext_send_buf[26]='-';
    Hex_ASCII(Time[5],ext_send_buf+27);
    SP_SendString(ext_send_buf,44);
}

void Checking_Global_Variables_Data(void)
{
    UINT8 c_temp_flash,c_block_no;
    UINT16 c_sample_cnt=0;
    UINT32 c_temp_adr;
    
    STRCPY(LBUF,"Please Wait.. ");
    Disp_Msg_No_Spaces(0,LINE6+1);
     
    FLASHReadString((Temp_Sector+3000),1,(char *)&c_block_no);//2500
    
    if(c_block_no==0x01)
    {
        c_sample_cnt=BLOCK_1_COUNT;
        EEPROM_ADDR=Global_Variables_Loc;
    }
    else if(c_block_no==0x02)
    {
        c_sample_cnt=BLOCK_2_COUNT;
        EEPROM_ADDR=Global_Variables2_Loc;
    }
    else if(c_block_no==0x04)
    {
        c_sample_cnt=BLOCK_4_COUNT;//1250;//1150;//100 1100
        EEPROM_ADDR=SMS_Counter_Loc;
    }        

    if(c_sample_cnt!=0x00 && c_block_no>=1 && c_block_no<=4)
    {
        FLASHReadString(Temp_Sector,c_sample_cnt,(char *)&Flash_Buffer);
        Sector_Erase(EEPROM_ADDR);

        FLASHWriteString(EEPROM_ADDR,c_sample_cnt,(char *)&Flash_Buffer);
        Sector_Erase(Temp_Sector);

        Checking_Flash();
    }
    
    FLASHReadString(Temp_Data_Sector_Flag+1,1,(char *)&c_temp_flash);//
    FLASHReadString(Temp_Data_Sector_Flag+2,4,(char *)&c_temp_adr);//

    if(c_temp_flash<=4 && c_temp_adr!=0xFFFFFFFF)
    {
        FLASHReadString(Temp_Data_Sector,4096,(char *)&Flash_Buffer);//
        
        Sector_Erase(c_temp_adr);
        FLASHWriteString(c_temp_adr,4096,(char *)&Flash_Buffer);

        Sector_Erase(Temp_Data_Sector_Flag);
    }
}

void Display_Field(BYTE* label, BYTE* value, UINT8 line) 
{
    STRCPY(LBUF, label);
    Disp_Msg(line);
    
    STRCPY(LBUF, value);
    LBUF[7] = 0;
    DISPLAY_NO_SPACES = 1;
    Invert_Disp_Msg(line + 12);
    DISPLAY_NO_SPACES = 0;
}


void Mfg_Details(UINT8 mtype)
{
    UINT16 batch_no;
    UINT8 value_Buf[10];
    
    Big_RTC=0;
    
    
    Display_Screen_Boarder();
    
    STRNCPY(LBUF,(char *)&BRAND_NAME,16);  
    Disp_Header_Boarder();
    Display_Field("DEVICE ID |", MACHINE_ID, LINE3);
#ifdef WIFI
    Display_Field("VERSION   |", "Wifi1.0", LINE4);
#elif defined(GPRS)
    Display_Field("VERSION   |", "GPRS1.0", LINE4);
#else
    Display_Field("VERSION   |", "BMC1.37", LINE4);
#endif
    SEERAMReadString(Mfg_Date_Loc,6,(char *)&value_Buf);
    value_Buf[6]=' ';
    Display_Field("MFG. DATE |", value_Buf, LINE5);
    Display_Field("CAPACITY  |", " 08 GB ", LINE6);

    SEERAMReadString(Batch_No_Loc,2,(char *)&batch_no); 
    value_Buf[0]=' ';
    Hex_DEC(0,batch_no,value_Buf+1,4);
    value_Buf[5]=' ';
    value_Buf[6]=' ';
    Display_Field("BATCH NO  |", value_Buf, LINE7);
    
    Display_Bar_LF(LINE8+10);
       
    if(mtype==0)
    {
        Delay_Sec(3);
        return;
    }

    TIME_FLAG=0;
    KBV=Wait_Key_Press('E');
    Clear_LCD();
    Big_RTC=1;
    TIME_FLAG=1;
}

void PrintLine(const char* text,char* feed) 
{
    UINT8 len;
    len = strlen(text); 
    STRNCPY(PBUF, text,len);
    Printer_SendString(0x03, len);
    if (strcmp(feed, "FEED") == 0)
        Paper_Feed(1);
    else
        PRINT_UNDERSCORE();
}

void Print_Help(void)
{
    UINT8 i;
    char* feed;
    
	Start_Printer();
    Font_Type(Normal);
    Justification(LEFT);
    
    const char* helpLines[8] = {
        "   SHORTCUT KEYS IN COLLECTION",
        "***Before Entering Code:",
       // "F1 - Change Collection Mode     ",
        "LEFT - Print Last Receipt",
        "SHIFT - Change Search Field",
        "       DEVICE ID:SUN0001",
        "LEFT - Edit Fat & Weight",
        "+ KEY - Add Quantity",
        "TAB - TARE Weight"
    };
    
    for(i=0;i<8;i++)
    {
        if(i==4)
            STRNCPY(PBUF+17,MACHINE_ID,7);
        feed = (i==0 || i==3  || i==4 || i==7) ? "UNDERSCORE" : "FEED";
        PrintLine(helpLines[i],feed);
    }
    
	Paper_Feed(3);
}


void main(void) 
 {
    UINT8 maincount=0;
    UINT16 osc_sample_cnt;

    DDPCONbits.JTAGEN=0;
    DDPCONbits.TROEN=0;
    DDPCONbits.TDOEN=0;

    SYS_FREQ=CLK_SYS;
    INTEnableSystemMultiVectoredInt();
    osc_sample_cnt = OSCCON;

    while(!(osc_sample_cnt & 0x00000020))
    {
        osc_sample_cnt = OSCCON;    // Wait for PLL lock to stabilize
    }

    Init_LCD(); 

    Init_Kbd();
 
    Display_Screen_Boarder();
      
    STRCPY(LBUF," Initializing ");
    Disp_Line_Boarder(1,LINE4);
    
    TRISGbits.TRISG14=0;
    TRISEbits.TRISE8=1;
   
    AD1PCFG = 0xFFFE; // bits 14 and 15 are 0, so AN14 and AN15 are AN inputs
    AD1CON3bits.ADCS = 2; // ADC clock period is Tad = 2 * (ADCS+1) * Tpb
    //AD1CON1bits.ADON = 0; // turn on A/D converter

    AD1CHSbits.CH0SA = 0; // connect pin AN14 to MUXA for sampling

    //AD1CON1bits.SAMP = 1; // start sampling
    AD1CON1bits.SSRC=0x07;
    AD1CON1bits.ASAM = 1; // Auto sampling

    Delay_Sec(1);
    BUZZER_IO=0;
    BUZZER=0;
   
    SCKD=0;
    SID=0;
    SOD=1;

    CED1=0;  
   
    deviceAttached = FALSE;
    USBInitialize(0);
    
    Read_Voltage();
    
    STRCPY(LBUF,"Checking RTC ");
    Disp_Msg_No_Spaces(0,LINE6+1);

    InitRTC();
    Read_RTC();
    
    STRCPY(LBUF,"RTC Module O.K");
    Disp_Msg_No_Spaces(0,LINE6+1);
  
    while(Time[6]==0 || Time[5]==0 || Time[4]==0 || Time[0]>0x59 || Time[1]>0x59 || Time[2]>0x23 || Time[4]>0x31 || Time[5]>0x12 || Time[6]>0x99)
    {
        STRCPY(LBUF,"Set Date/Time ");
        Disp_Msg_No_Spaces(0,LINE6+1);
        
        Wait_Key_Press('C');
        BUZZER=1;
        TIME_RIGHT_ARROW='Y';
        SetDate_Time(5);
        TIME_RIGHT_ARROW='N';
    }
   
    Checking_Flash();
    
    BUZZER=1;

    Checking_Global_Variables_Data();  
    
    //Select_Dealer();
      
    SEERAMReadString(Dealer_Name_Loc,16,LBUF);
    Disp_Line_Boarder(1,LINE1);

    for(NO_OF_CUSTOMERS=0;NO_OF_CUSTOMERS<1000;NO_OF_CUSTOMERS++)
    {
        Read_Member_CODE(NO_OF_CUSTOMERS+1);
        if(Memberrec.C_CODE==0xFFFF)
            break;
    }
    
    Init_Global_Variables();
  
    Display_Screen_Boarder();    
    STRNCPY(LBUF,(char *)&BRAND_NAME,16);    
    Disp_Msg_No_Spaces(1,LINE4);
    BOARDER=0;

    if(DEVICE_CODE>9999)
        DEVICE_CODE=1;
      
    Delay_Sec(2);

    Init_New_Printer();     

    SEERAMReadString(Monitor_Voltage_Loc,1,(char *)&MONITOR_VOLTAGE);
   
    Enable_Timer3();
    Init_Analyzer(9600,8);
    
    Init_Ext_Display();
    Mfg_Details(0);

    Initialize_Analyzers('B');
    SERVER_READ_STATUS=0;
  
    Init_Transaction_Counters();
    
 
    if(GPRSrec.GPRS_ON=='Y' && ON_LINE=='Y')
    {
        Init_GPRS();

        Date_Change();

        BACKEND_FLAG='S';
    }    
    
    
    while(1)
    {
        Clear_LCD();
        ANY_KEY=1;
        Big_RTC=1;       
        Shift_Index=0;
        
        do
        {
            PORTGbits.RG14=0;
           
            STRNCPY(LBUF,(char *)&BRAND_NAME,16);
            Disp_Underline();
           
            Display_Counters();

            KBV=Wait_Key_Press('N');

            if(SERVER_CONTROL=='Y' && MACHINE_LOCK=='Y')
                KBV=0xFF;
            
            if(KBV==KB_CANCEL)
                Mfg_Details(1);

        }while(KBV==0xFF);
        
        BACKEND_FLAG='B';
        PORTGbits.RG14=0;
        LED_Blink=0;
        ANY_KEY=0;        
        STANDBY=0;
        
        if(KBV>=0x31 && KBV<=0x39)
        {
            maincount=KBV-0x31;
            KBV=KB_ENT;
        }
        if(KBV==0x30)
        {
            Print_Help();
        }
        else
        {
            Main_Menu(maincount);
        }

        maincount=0;      
        
        STANDBY=1;
        BACKEND_FLAG='S';
        Big_RTC=1;
        LED_Blink=1;
        TIME_FLAG=1;
    }
}