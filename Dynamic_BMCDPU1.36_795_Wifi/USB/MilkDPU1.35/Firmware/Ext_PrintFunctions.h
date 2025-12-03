void CLR_Enabled(char *tbuf)
{
    if(CLR_BASED=='Y')
    {
        tbuf[0]='C';tbuf[1]='L';tbuf[2]='R';
    }
}
void Set_Printer_Type(void)
{
   UINT8 print_count=0,p_t_count=46;

   if(Ext_Print_L==80)        
   {
       p_t_count=80;
    }
   while(PBUF[print_count]!=0 && print_count<p_t_count)
   {
        print_count++;
   }
    
    if(Ext_Print_L!=40)
    {
        PBUF[print_count]='\n';
        PBUF[print_count+1]=0;

    }
    else
        PBUF[print_count]=0;
}

void Ext_Printer_SendString(void)
{
    Set_Printer_Type();
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_FONT_NAME, USB_NULL, USB_PRINTER_FONT_POS_18x72, 0 );
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_FONT_MEDIUM, USB_NULL, 0, 0 );
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_POS_TEXT_LINE, USB_DATA_POINTER_RAM(PBUF), 1, USB_PRINTER_TRANSFER_COPY_DATA );
}

void Ext_Printer_SendString_Bold(void)
{
    Set_Printer_Type();
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_FONT_NAME, USB_NULL, USB_PRINTER_FONT_POS_18x72, 0 );
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_FONT_BOLD, USB_NULL, 0, 0 );
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_POS_TEXT_LINE, USB_DATA_POINTER_RAM(PBUF), 1, USB_PRINTER_TRANSFER_COPY_DATA );
}

void Ext_Printer_SendString_3636Bold(void)
{
    Set_Printer_Type();
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_FONT_NAME, USB_NULL, USB_PRINTER_FONT_POS_36x72, 0 );
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_FONT_BOLD, USB_NULL, 0, 0 );
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_POS_TEXT_LINE, USB_DATA_POINTER_RAM(PBUF), 1, USB_PRINTER_TRANSFER_COPY_DATA );
}


void Ext_Paper_Feed(UINT8 fcnt)
{
    while(fcnt--)
    {
        STRCPY(PBUF,"                                ");
        PBUF[33]=0x00;
        Ext_Printer_SendString();
    }
}

void Ext_Print_Code_Name(UINT16 pcode)
{
    ZERO=0;
    PBUF[0]=PBUF[1]=PBUF[6]=' ';
    Hex_DEC(0,pcode,PBUF+2,4);
    PBUF[7]=':';
    STRNCPY(PBUF+8,Memberrec.NAME,20);
    PBUF[28]='\0';
    Ext_Printer_SendString();
}

void Ext_Print_Deduction_Receipt(UINT32 pdeductionamt)
{
    if (!Initialize_Printer())
        return;

    STRCPY(PBUF," DEDUCTION RECEIPT");
    CollectionRecord.CODE=Memberrec.C_CODE;
    EXT_Report_Header(38);

    EXT_PRINT_UNDERSCORE(36);

    Ext_Print_Code_Name(Memberrec.C_CODE);

    Ext_Print_Date('d');

    Print_Deduction_Data('E',pdeductionamt);

    Ext_Printer_Stop();
}

void Ext_Print_LocalSale_Receipt(UINT32 lsqty,UINT16 lsrate,UINT32 lsamount)
{    
    if (!Initialize_Printer())
        return;

    STRCPY(PBUF," LOCAL SALE RECEIPT");
    EXT_Report_Header(38);

    EXT_PRINT_UNDERSCORE(36);

    Ext_Print_Date('L');

    ZERO=1;

    STRCPY(PBUF,"  MILK    QTY(L)   RATE   AMOUNT(Rs)");
    if(WEIGHT_KG=='Y')
        PBUF[14]='K';
    
    Ext_Printer_SendString_Bold();

    EXT_PRINT_UNDERSCORE(36);

    STRCPY(PBUF,"                                    ");
   
    STRCPY(PBUF+2,"COW");
    Buf_Type(LocalSalesrec.MILK_TYPE,PBUF+2);        
   
    PBUF[5]=' ';
    Hex_DEC(2,lsqty,PBUF+9,7);
    Hex_DEC(2,lsrate,PBUF+17,6);
    Hex_DEC(2,lsamount,PBUF+28,8);
    Ext_Printer_SendString();

    EXT_PRINT_UNDERSCORE(36);

    Ext_Printer_Stop();
}

void Ext_Print_Receipt(UINT8 pctype)
{
    
    if (!Initialize_Printer())
        return;

    EXT_PRINT_UNDERSCORE(36);

    STRCPY(PBUF," DUPLICATE RECEIPT");
    if(pctype=='C')
        STRCPY(PBUF," COLLECTION RECEIPT");
    
    EXT_Report_Header(36);

    Ext_Print_Date('C');
    
    Ext_Print_Code_Name(CollectionRecord.CODE);
    
    Print_Receipt_Data('E');

    Ext_Printer_Stop();
}

void Ext_Print_Summary_Head(UINT8 reptype)
{
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_JOB_START, USB_NULL, 0, 0 );
    
    if(reptype==0)
        STRCPY(PBUF,"   SUMMARY SHIFT REPORT");
    else
        STRCPY(PBUF,"  DAYWISE SUMMARY REPORT");

    EXT_Report_Header(46);
    Ext_Print_From_To_Date();

    STRCPY(PBUF,"  MEMBER   FAT  SNF TOTAL_QTY  RATE  TOTAL_AMT");
    CLR_Enabled(PBUF+15);
  
    Ext_Printer_SendString_Bold();

    EXT_PRINT_UNDERSCORE(46);
}

void Ext_Print_Members_From_To(void)
{
    STRCPY(PBUF,"  MEMBERS FROM:0001  To  0001");
    Hex_DEC(0,Fromrec.START_MEMBER,PBUF+15,4);
    Hex_DEC(0,Fromrec.END_MEMBER,PBUF+25,4);
    Ext_Printer_SendString();
}

void Ext_Print_Member_List_Start(UINT8 mlisttype)
{
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_JOB_START, USB_NULL, 0, 0);
    
    if(mlisttype=='T')
        STRCPY(PBUF,"     MEMBERS LIST");
    
    else if(mlisttype=='I')
        STRCPY(PBUF,"  INACTIVE MEMBERS LIST");
    
    else
        STRCPY(PBUF,"  ABSENT MEMBERS LIST");
    
    EXT_Report_Header(46);
    
    if(mlisttype=='A')
         Ext_Print_Date('A');
    
    EXT_PRINT_UNDERSCORE(46);

    Ext_Print_Members_From_To();

    STRCPY(PBUF,"  S.NO  CODE MILK STATUS  MEMBER_NAME         ");
    Ext_Printer_SendString_Bold();
    EXT_PRINT_UNDERSCORE(46);
}

void Ext_Print_Member_List_Record(UINT16 msno)
{
    PBUF[0]=PBUF[1]=' ';
    Hex_DEC(0,msno,PBUF+2,4);
    PBUF[6]=PBUF[7]=' '; 
    Hex_DEC(0,Memberrec.C_CODE,PBUF+8,4);//1

    STRCPY(PBUF+12," COW ");
    Buf_Type(Memberrec.MILK_TYPE,PBUF+13);

    STRCPY(PBUF+17,"DEACTIVE");
    if(Memberrec.STATUS=='A')
        STRCPY(PBUF+17," ACTIVE ");
   
    PBUF[25]=' ';
    STRNCPY(PBUF+26,Memberrec.NAME,20);
    PBUF[46]=0x00;
    Ext_Printer_SendString();
}

void Ext_Print_Average_Total(UINT8 pheader)
{
    ZERO=1;

    if(pheader==0)
    {
        STRCPY(PBUF,"  MEMBERS  FAT  SNF   TOT_QTY   RATE TOTAL_AMT");
        CLR_Enabled(PBUF+16);
       
        Ext_Printer_SendString();

        EXT_PRINT_UNDERSCORE(46);
    }
    ZERO=0;
    Calculate_Avg_Fat_Snf_Rate();
    
    if(Cumrec.C_MEMBERS>0)
    {    
        PBUF[2]='C';      
        PBUF[3]=':';
        Hex_DEC(0,Cumrec.C_MEMBERS,PBUF+4,3);
        if(Cumrec.C_MEMBERS>999)
            Hex_DEC(0,Cumrec.C_MEMBERS,PBUF+4,4);
        ZERO=1;

        PBUF[7]=' ';
        Hex_DEC(2,Cumrec.C_FAT,PBUF+8,5);
        
        PBUF[13]=' ';
        if(CLR_BASED=='Y')
            Hex_DEC(2,Cumrec.C_CLR,PBUF+14,5);
        else
            Hex_DEC(2,Cumrec.C_SNF,PBUF+14,5);
        
        PBUF[19]=' ';
        Hex_DEC(2,Cumrec.C_QTY,PBUF+20,9);
        PBUF[29]=' ';
        Hex_DEC(2,Cumrec.C_RATE,PBUF+30,6);
        
        PBUF[36]=' ';
        Hex_DEC(2,Cumrec.C_AMOUNT+Cumrec.C_INC_AMT,PBUF+37,9);
        
        PBUF[46]=0x00;

        Ext_Printer_SendString_Bold();
    }

    ZERO=0;
    if(Cumrec.B_MEMBERS>0)
    {
        PBUF[2]='B';       
        PBUF[3]=':';
        Hex_DEC(0,Cumrec.B_MEMBERS,PBUF+4,3);
        if(Cumrec.C_MEMBERS>999)
            Hex_DEC(0,Cumrec.B_MEMBERS,PBUF+4,4);
        ZERO=1;

        PBUF[7]=' ';
        Hex_DEC(2,Cumrec.B_FAT,PBUF+8,5);
        
        PBUF[13]=' ';
        if(CLR_BASED=='Y')
            Hex_DEC(2,Cumrec.B_CLR,PBUF+14,5);
        else
            Hex_DEC(2,Cumrec.B_SNF,PBUF+14,5);
        
        PBUF[19]=' ';
        Hex_DEC(2,Cumrec.B_QTY,PBUF+20,9);
        
        PBUF[29]=' ';
        Hex_DEC(2,Cumrec.B_RATE,PBUF+30,6);
        
        PBUF[36]=' ';
        Hex_DEC(2,Cumrec.B_AMOUNT+Cumrec.B_INC_AMT,PBUF+37,9);
        
        PBUF[46]=0x00;
        Ext_Printer_SendString_Bold();
    }
    if((Cumrec.C_MEMBERS +Cumrec.B_MEMBERS)>0)
        EXT_PRINT_UNDERSCORE(46);
}

void Ext_Print_Shift_Report_Start(void)
{
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_JOB_START, USB_NULL, 0, 0 );

    EXT_PRINT_UNDERSCORE(42);

    STRCPY(PBUF," DETAILED SHIFT REPORT");

    EXT_Report_Header(42);

    Ext_Print_Date('S');

    EXT_PRINT_UNDERSCORE(42);
 
    STRCPY(PBUF,"  CODE  FAT  SNF QTY(L) RATE  INCENT. AMOUNT");
    CLR_Enabled(PBUF+13);
    
    if(WEIGHT_KG=='Y')
       PBUF[22]='K';
    Ext_Printer_SendString_Bold();

    EXT_PRINT_UNDERSCORE(42);
}

void Ext_Print_From_To_Date(void)
{
    if(Ext_Print_L==40)
        EXT_PRINT_UNDERSCORE(46);

    STRCPY(PBUF,"  Bill From ");
    Copy_MonthRecDT(PBUF+12);

    Ext_Printer_SendString_Bold();
   
   EXT_PRINT_UNDERSCORE(46);
}

void Ext_Print_Daywise_Report_Start(void)
{
    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_JOB_START, USB_NULL, 0, 0 );

    STRCPY(PBUF," MEMBER SUMMARY REPORT");
    
    EXT_Report_Header(46);

    Ext_Print_From_To_Date();
    Ext_Print_Members_From_To();

    EXT_PRINT_UNDERSCORE(46);

    STRCPY(PBUF,"  CODE  FAT% SNF%   QUANTITY A.RATE  TOTAL_AMT");
    CLR_Enabled(PBUF+13);
    Ext_Printer_SendString_Bold();

    EXT_PRINT_UNDERSCORE(46);
    
}

void Ext_Print_Daywise_Report_Record(UINT16 pcode,UINT8 pmilk)
{
    ZERO=0;
    Hex_DEC(0,pcode,PBUF,4);
    ZERO=1;
    PBUF[4]=0x42+pmilk;
    PBUF[5]=' ';

    if(Balancerec.QTY>0)
    {
        Hex_DEC(1,(Balancerec.AVG_FAT/Balancerec.QTY),PBUF+6,4);
    }
    else
        Hex_DEC(1,0,PBUF+6,4);
    
    PBUF[10]=' ';
    
    if(Balancerec.QTY>0)
    {
        if(CLR_BASED=='Y')
            Hex_DEC(1,(Balancerec.AVG_CLR/Balancerec.QTY),PBUF+11,4);
        else
            Hex_DEC(1,(Balancerec.AVG_SNF/Balancerec.QTY),PBUF+11,4);

    }
    else
        Hex_DEC(1,0,PBUF+11,4);
    PBUF[15]=' ';
    
    Hex_DEC(2,Balancerec.QTY,PBUF+16,10);
    PBUF[26]=' ';
    
    if(Balancerec.QTY>0)
    {
        Hex_DEC(2,(Balancerec.AVG_RATE/Balancerec.QTY),PBUF+27,6);
    }
    else
        Hex_DEC(2,0,PBUF+16,6);
    
    PBUF[33]=' ';
    
    Hex_DEC(2,Balancerec.CREDIT_AMT,PBUF+34,10);
    USB_Print_SendString('E',0,44);
}

void Ext_Print_Report_Date_Time(void)
{
    ZERO=0;

    STRCPY(PBUF,"  DATE:  /  /      TIME:  :  :  ");
    Copy_Date(PBUF+7);
    Copy_Time(PBUF+24);
    if(Ext_Print_L==40)
        PBUF[32]=0x00;
    Ext_Printer_SendString();
}

void Ext_Printer_Bonus_Start(void)
{
    STRCPY(PBUF,"     BONUS REPORT");
    EXT_Report_Header(46);

    Ext_Print_From_To_Date();

    Print_Bonus_Type('E');
    ZERO=1;

    Ext_Print_Members_From_To();

    EXT_PRINT_UNDERSCORE(46);

    STRCPY(PBUF,"  CODE  MILK    QUANTITY     AMOUNT  BONUS_AMT");
    Ext_Printer_SendString_Bold();
    EXT_PRINT_UNDERSCORE(46);
                    
}
void Ext_Print_Bonus_Record(UINT16 tempcode,UINT32 bonusamount,UINT8 milktype)
{
    STRCPY(PBUF,"                                      ");

    Hex_DEC(0,tempcode,PBUF+2,4);
    PBUF[6]=' ';
    PBUF[7]=' ';
    PBUF[8]=' ';
    
    PBUF[9]='C';PBUF[10]='O';PBUF[11]='W';
    Buf_Type(milktype,PBUF+9);

    Hex_DEC(2,Balancerec.QTY,PBUF+14,10);
    Hex_DEC(2,Balancerec.CREDIT_AMT,PBUF+25,10);
    Hex_DEC(2,bonusamount,PBUF+36,10);
    PBUF[46]=0x00;

    Ext_Printer_SendString();
}
void Ext_Print_Bonus_Totals(UINT32 totalqty,UINT32 totalamt,UINT32 totalbonus)
{
    EXT_PRINT_UNDERSCORE(46);

    STRCPY(PBUF,"  TOTAL :                            ");
    Hex_DEC(2,totalqty,PBUF+14,10);
    Hex_DEC(2,totalamt,PBUF+25,10);
    Hex_DEC(2,totalbonus,PBUF+36,10);
    PBUF[46]=0x00;

    Ext_Printer_SendString_Bold();

    EXT_PRINT_UNDERSCORE(46);
    Ext_Printer_Stop();
}

void Ext80_USB_Member_Shift_Record(UINT8 rectype)
{
    UINT8 separator;
    
    separator=',';
    if(rectype=='E')
        separator=' ';
     
    ZERO=0;
    Copy_ShiftDT(PBUF);
    
    PBUF[2]='/';
    PBUF[5]='/';
    PBUF[8]=separator;
    Day_Shift(PBUF+9);
   
    PBUF[16]=separator;
   
    STRCPY(PBUF+17,"COW");
    Buf_Type(CollectionFlags.MILK_TYPE,PBUF+17);
    
    PBUF[20]=separator;
   
    Calculate_Amount();
    ZERO=1;
    Hex_DEC(1,CollectionRecord.FAT,PBUF+21,4);
    if(rectype=='E')
    {
        PBUF[25]=' ';
        if(CollectionFlags.PAYMENT_STATUS==0)
            PBUF[25]='P';
    }
    else
        PBUF[25]=separator;
    if(CLR_BASED=='Y')
        Hex_DEC(1,CollectionRecord.CLR1,PBUF+26,4);
    else
        Hex_DEC(1,CollectionRecord.SNF,PBUF+26,4);
    
    PBUF[30]=separator;

    Hex_DEC(2,CollectionRecord.QTY,PBUF+31,6);
   
    PBUF[37]=separator;

    Hex_DEC(2,CollectionRecord.RATE,PBUF+38,6);
    PBUF[44]=separator;
    
    Hex_DEC(2,CollectionRecord.INCENTIVE_AMT,PBUF+45,9);
    PBUF[54]=separator;

    Hex_DEC(2,Amount+CollectionRecord.INCENTIVE_AMT,PBUF+55,9);
    PBUF[64]=separator;
    STRCPY(PBUF+65," Auto ");
   
    if(CollectionFlags.ANALYZER_MODE==0)
    {
        STRCPY(PBUF+65,"Manual");
    }
    
    PBUF[71]=separator;
    
    STRCPY(PBUF+72," Auto ");
    
    if(CollectionFlags.WEIGHT_MODE==0)
    {
        STRCPY(PBUF+72,"Manual");
    }
   
    USB_Print_SendString(rectype,0,78);
   
}



