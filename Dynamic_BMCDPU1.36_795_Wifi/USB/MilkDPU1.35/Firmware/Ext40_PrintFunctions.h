void Ext_Printer_Stop(void)
{
    Ext_Paper_Feed(2);
    Delay_Sec(1);
    USBHostShutdown();
}
void EXT_PRINT_UNDERSCORE(UINT8 cnt)
{
    UINT8 u_cnt;

    PBUF[0]=PBUF[1]=' ';
    for(u_cnt=2;u_cnt<(cnt+2);u_cnt++)
        PBUF[u_cnt]='-';
    PBUF[cnt+2]=0x00;
    
    Ext_Printer_SendString_Bold();
}

void EXT_Report_Header(UINT8 plength)
{   
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_POS_JUSTIFICATION_LEFT, USB_NULL, 0, 0);
    Ext_Printer_SendString_3636Bold();
    EXT_PRINT_UNDERSCORE(plength);
    Header_Footer_Print('E',0,4);//Print_Header_Ext();
}

void Ext_Print_Header(void)
{
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_POS_JUSTIFICATION_LEFT, USB_NULL, 0, 0);
    Ext_Printer_SendString_3636Bold();  
}

void Ext_Print_Date(UINT8 ptimeflag)
{
    UINT8 time_cnt;
    ZERO=0;

    Read_RTC();

    STRCPY(PBUF,"    /  /                           ");
    Copy_ShiftDT(PBUF+2);
  
    if(ptimeflag=='C')
    {
        Hex_DEC(0,CollectionRecord.WTIME[0] & 0x1F,PBUF+14,2);//sudha
        PBUF[16]=':';
        Hex_DEC(0,CollectionRecord.WTIME[1],PBUF+17,2);
        
        PBUF[19]=':';
        Hex_DEC(0,CollectionRecord.WTIME[2] & 0x7F,PBUF+20,2);

        time_cnt=25;
    }
    else
        time_cnt=13;

    Day_Shift(PBUF+time_cnt);

    PBUF[time_cnt+7]=' ';
  
    PBUF[32]=0x00;
    Ext_Printer_SendString();

    if(ptimeflag=='L' || ptimeflag=='C' || ptimeflag=='D' || ptimeflag=='d')
        EXT_PRINT_UNDERSCORE(32);
}

void Send_Print_Code_Name(UINT8 rtype)
{
    PBUF[0]=PBUF[1]=' ';
    Hex_DEC(0,Memberrec.C_CODE,PBUF+2,4);
    STRCPY(PBUF+6,"  COW  ");
    Buf_Type(Memberrec.MILK_TYPE,PBUF+8);
       
    STRNCPY(PBUF+13,Memberrec.NAME,20);
    
    if(rtype=='E')
    {
        PBUF[33]=0x00;
        Ext_Printer_SendString();
    }
    else
    {
        PBUF[33]='\r';
        PBUF[34]='\n';
        FSfwrite(PBUF,1,35,myFile);
    }
}
UINT8 Ext_Print_Heading(UINT8 pheaderflag)
{
    if (!Initialize_Printer())
        return;

    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_JOB_START, USB_NULL, 0, 0 );

    if(Ext_Print_L==40)
    {
        STRCPY(PBUF,"   MEMBERWISE REPORT");
        
        if(pheaderflag=='C')
            STRCPY(PBUF,"  MEMBER BILL REPORT");
       
        EXT_Report_Header(46);
    }
        
    else
    {
        STRCPY(PBUF,"            MEMBERWISE REPORT ");
        
        if(pheaderflag=='C')
            STRCPY(PBUF,"           MEMBER BILL REPORT ");
       
        EXT_Report_Header(80);

        EXT_PRINT_UNDERSCORE(80);
    }

    Ext_Print_From_To_Date();

    ZERO=0;
    
    Send_Print_Code_Name('E');

    if(Ext_Print_L==40)
    {
        STRCPY(PBUF,"  DATE  FAT  SNF QTY(L)  RATE INCENT. TOT_AMT");
        CLR_Enabled(PBUF+20);
      
        if(WEIGHT_KG=='Y')
            PBUF[21]='K';
    }      

    else
    {
        
        STRCPY(PBUF,"    DATE   SHIFT  MILK  FAT  SNF QTY(L)  RATE  INCENTIVE TOTAL_AMT M_MODE W_MODE");
        CLR_Enabled(PBUF+30);
        if(WEIGHT_KG=='Y')
            PBUF[37]='K';
     
    }

    Ext_Printer_SendString_Bold();
    if(Ext_Print_L==40)
        EXT_PRINT_UNDERSCORE(46);
    else
        EXT_PRINT_UNDERSCORE(80);
    
    return 1;
}

void Ext40_Print_Payment_Register_Start(void)
{
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_JOB_START, USB_NULL, 0, 0 );

    STRCPY(PBUF,"   PAYMENT REGISTER");
    EXT_Report_Header(42);

    Ext_Print_From_To_Date();
    
    STRCPY(PBUF,"CODE  QUANTITY  A.RATE  INCENTIVE   AMOUNT");
    USB_Print_SendString('E',1,42);

    EXT_PRINT_UNDERSCORE(42);
}

void Ext40_Print_Member_Shift_Record(void)
{
    Calculate_Amount();
    ZERO=0;
    
    PBUF[0]=PBUF[1]=' ';
    Copy_ShiftDT(PBUF+2);
    
    PBUF[4]=CollectionFlags.MILK_TYPE;

    Read_RTC();
    PBUF[5]=Shiftrec.DAY_SHIFT;
  
    PBUF[6]=' ';

    Calculate_Amount();
    ZERO=1;
    Hex_DEC(1,CollectionRecord.FAT,PBUF+7,4);
    PBUF[11]=' ';
    if(CollectionFlags.PAYMENT_STATUS==0)
        PBUF[11]='P';
    if(CLR_BASED=='Y')
        Hex_DEC(1,CollectionRecord.CLR1,PBUF+12,4);
    else
        Hex_DEC(1,CollectionRecord.SNF,PBUF+12,4);
    PBUF[16]=' ';
    if((CollectionRecord.FLAGS | 0xF7)!=0xFF)
        PBUF[16]='M';
    Hex_DEC(2,CollectionRecord.QTY,PBUF+17,6);
    PBUF[23]=' ';
    if((CollectionRecord.FLAGS | 0xFB)!=0xFF)
        PBUF[23]='M';
    
    Hex_DEC(2,CollectionRecord.RATE,PBUF+24,6);
    
    PBUF[30]=' ';
    Hex_DEC(2,CollectionRecord.INCENTIVE_AMT,PBUF+31,6);
    if(CollectionRecord.INCENTIVE_AMT>99999)
        Hex_DEC(2,CollectionRecord.INCENTIVE_AMT,PBUF+33,6);
    PBUF[37]=' ';
    Hex_DEC(2,Amount+CollectionRecord.INCENTIVE_AMT,PBUF+38,8);
    PBUF[46]=0x00;

    Ext_Printer_SendString();
}
