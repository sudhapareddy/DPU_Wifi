void No_USB_Found_Msg(void)
{
    STRCPY(LBUF," NO USB FOUND ");
    Disp_Msg_No_Spaces(0,LINE5+1);

    Delay_Sec(1);
}

void Empty_File_Msg(void)
{
    STRCPY(LBUF," EMPTY FILE...");
    Disp_Msg_No_Spaces(0,LINE5+1);

    Delay_Sec(1);
}

void Display_No_File_Msg(UINT8 dline)
{
    STRCPY(LBUF,"NO FILE FOUND.");
    Disp_Msg_No_Spaces(0,dline);
    
    Delay_Sec(2);
}

UINT8 File_Over_Write(void)
{
    CLEAR_DATA_LINES_FROM(4,7);
    
    STRCPY(LBUF,"DO YOU WANT TO");
    Disp_Msg_No_Spaces(0,LINE4+1);

    STRCPY(LBUF,"   OVERWRITE? ");
    Disp_Msg_No_Spaces(0,LINE6+1);
    
    do
    {
        KBV=Scan_Key('C');
 
        if(KBV==KB_CANCEL)
            return 0xFF;

        if(KBV==KB_ENT)
            return 0x01;

    }while(1);
}

void Write_USB_File(UINT8 index)
{
    SEND_BUF[index]='\r';
    SEND_BUF[index+1]='\n';
    FSfwrite(SEND_BUF,1,(index+2),myFile);
}

void Send_Header(void)
{    
    Header_Footer_Print('U',0,4);
    
    STRCPY(SEND_BUF,"DATE:  /  /        TIME:  :  :  ");

    Copy_Date(SEND_BUF+5);
    Copy_Time(SEND_BUF+24);
    Write_USB_File(32);
}

void Send_Shift_Record(char *tbuf)
{
    Calculate_Amount();    
    
    tbuf[0]=Shiftrec.DAY_SHIFT;
    tbuf[1]=',';
    tbuf[2]=CollectionFlags.MILK_TYPE;
    tbuf[3]=',';

    Hex_DEC(1,CollectionRecord.FAT,tbuf+4,4);//3
    tbuf[8]=',';

    Hex_DEC(1,CollectionRecord.SNF,tbuf+9,4);
    tbuf[13]=',';
   
    Hex_DEC(1,CollectionRecord.CLR1,tbuf+14,4);
    tbuf[18]=',';

    Hex_DEC(2,CollectionRecord.QTY,tbuf+19,6);
    tbuf[25]=',';

    Hex_DEC(2,CollectionRecord.RATE,tbuf+26,6);
    tbuf[32]=',';
    
    Hex_DEC(2,CollectionRecord.INCENTIVE_AMT,tbuf+33,7);
    tbuf[40]=',';
    
    Hex_DEC(2,Amount+CollectionRecord.INCENTIVE_AMT,tbuf+41,8);
    tbuf[49]=',';
    STRCPY(tbuf+50," Auto ");
    if(CollectionFlags.ANALYZER_MODE==0)
    {
        STRCPY(tbuf+50,"Manual");//
        
    }
    
    tbuf[56]=',';
    
    STRCPY(tbuf+57," Auto ");
    if(CollectionFlags.WEIGHT_MODE==0)
    {
        STRCPY(tbuf+57,"Manual");
    }
        
    tbuf[63]='\r';
    tbuf[64]='\n';
}

void Send_EIP_Data(void)
{
    UINT8 t_cnt=0;

    ZERO=0;
    
    Read_Collection_Flags();
    
    if(CollectionRecord.CODE>999)
        t_cnt=1;

    Hex_DEC(0,CollectionRecord.CODE,SEND_BUF,(3+t_cnt));
    SEND_BUF[3+t_cnt]=CollectionFlags.MILK_TYPE;
    Hex_DEC(0,CollectionRecord.FAT,SEND_BUF+4+t_cnt,3);
    Hex_DEC(0,CollectionRecord.SNF,SEND_BUF+7+t_cnt,3);
    Hex_DEC(0,CollectionRecord.WATER1,SEND_BUF+10+t_cnt,3);   
    Hex_DEC(0,CollectionRecord.QTY,SEND_BUF+12+t_cnt,5);
    
    Hex_DEC(0,(CollectionRecord.WTIME[1] & 0x1F),SEND_BUF+17+t_cnt,2);    
    Hex_DEC(0,CollectionRecord.WTIME[0],SEND_BUF+19+t_cnt,2);
    
    Hex_DEC(0,1,SEND_BUF+21+t_cnt,3);
    SEND_BUF[24+t_cnt]='-';
    Write_USB_File(25+t_cnt);
}

UINT8 Check_USB_Device(void)
{
    Display_Boarder3();

    STRCPY(LBUF,"INSERT DEVICE ");
    Disp_Msg_No_Spaces(0,LINE5+1);

    KBV=0xFF;

    while(1)
    {
        USBTasks();

        if(USBHostMSDSCSIMediaDetect())
        {
            deviceAttached = TRUE;

            if(FSInit())
            {
                STRCPY(LBUF," USB INSERTED ");
                Disp_Msg_No_Spaces(0,LINE5+1);

                Delay_250Ms(2);

                return 1;
            }
        }

        KBV=Scan_Key('A');

        if(KBV==KB_CANCEL)
            break;

    }  

    STRCPY(LBUF," CANCELLED... ");
    Disp_Msg_No_Spaces(0,LINE5+1);

    Delay_250Ms(2);

    return 0;
}

void File_Name_From_To(UINT8 filetype)
{
    ZERO=0;
    STRNCPY(PBUF,MACHINE_ID,7);
    PBUF[7]='_';
    if(filetype=='C')
    {
        STRCPY(PBUF+8,"Member_Bill_0001");
        Hex_DEC(0,Memberrec.C_CODE,PBUF+20,4);
    }

    if(filetype=='m')
        STRCPY(PBUF+8,"Payment_Register");

    if(filetype=='s')
        STRCPY(PBUF+8,"Summary__Report_");
    
    if(filetype=='S')
        STRCPY(PBUF+8,"Daywise_Summary_");

    if(filetype=='U')
        STRCPY(PBUF+8,"Total_Shift_Data");
    PBUF[24]='_';
    
    USB_FILE=1;
    Copy_MonthRecDT(PBUF+25);
    USB_FILE=0;
    STRNCPY(PBUF+45,".csv",4);
    PBUF[49]=0x00;
}

void Report_File_Name(UINT8 rfiletype)
{
    ZERO=0;

    if(rfiletype!='d')
    {
        Hex_DEC(0,Shiftrec.DAY,PBUF,2);
        Hex_DEC(0,Shiftrec.MONTH,PBUF+2,2);
        Hex_DEC(0,Shiftrec.YEAR,PBUF+4,2);
        PBUF[6]=Shiftrec.DAY_SHIFT;
        STRNCPY(PBUF+7,".EIP",4);
        PBUF[11]=0x00;

        return;
    }
  
    STRNCPY(PBUF,MACHINE_ID,7);
    STRCPY(PBUF+7,"_Detailed_Report_00.00.00_");
    USB_FILE=1;
    Copy_ShiftDT(PBUF+24);
    USB_FILE=0;
    Day_Shift(PBUF+33);
   
    STRNCPY(PBUF+40,".csv",4);
    PBUF[44]=0x00;
}

void Send_Average_Header(void)
{
    STRCPY(SEND_BUF,"DATE,SHIFT,MEMBERS,MILK,FAT,SNF,QUANTITY,RATE,AMOUNT,INCENTIVE,TOTAL AMT");
    CLR_Enabled(SEND_BUF+28);
    Write_USB_File(72);
}

UINT8 Insert_USB_FileName(UINT8 ifiletype)
{
    Report_File_Name(ifiletype);

    myFile = FSfopen(PBUF,"w");

    if(myFile == NULL)
    {
        Display_No_File_Msg(LINE5+1);
        return 0;
    }

    if(ifiletype=='d')
    {
        STRCPY(SEND_BUF,"                        DETAILED SHIFT REPORT");
        Write_USB_File(45);
    }
    else
        return 1;
   
    Send_Header();

    return 1;
}

UINT8 Insert_USB(void)
{
    if(USB_Check_Flag!='N')
    {
        if(Check_USB_Device()==0)
        {
            No_USB_Found_Msg();
            return 0;
        }
        else
            return 1;
    }
    return 1;
}

UINT8 Insert_USB_To(void)
{
    if(USB_Check_Flag=='Y' && Check_USB_Device()==0)
    {
        No_USB_Found_Msg();
        return 0;
    }
    else
        return 1;
}

UINT8 Insert_USB_To_FileName(UINT8 ifiletype)
{
    if(ifiletype!='D')
        File_Name_From_To(ifiletype);
    
    myFile = FSfopen(PBUF,"w");

    if(myFile == NULL)
    {
        Display_No_File_Msg(LINE5+1);

        return 0;
    }

    STRCPY(SEND_BUF,"                      ");
    if(ifiletype=='s')
        STRCPY(SEND_BUF+20,"SUMMARY SHIFT REPORT");
    
    if(ifiletype=='S')
        STRCPY(SEND_BUF+17,"DAYWISE SUMMARY REPORT");

    if(ifiletype=='m')
        STRCPY(SEND_BUF+21,"PAYMENT REGISTER   ");
   
    if(ifiletype=='C')
        STRCPY(SEND_BUF+22,"MEMBER BILL       ");

    if(ifiletype=='U')
        STRCPY(SEND_BUF+20,"TOTAL SHIFTWISE DATA ");
    Write_USB_File(40);
   
    Send_Header();
    
    if(ifiletype!='D')
    {
        STRCPY(SEND_BUF,"Bill From:");
        Copy_MonthRecDT(SEND_BUF+10);
        Write_USB_File(30);
    }
    
    if(ifiletype!='C')
    {
        STRCPY(SEND_BUF,"Members From:0001  To  ");

        Hex_DEC(0,Fromrec.START_MEMBER,SEND_BUF+13,4);
        Hex_DEC(0,Fromrec.END_MEMBER,SEND_BUF+23,4);
        Write_USB_File(27);
    }
    return 1;
}

void Write_Summary_Avg_Total(UINT8 reptype)
{
    Copy_ShiftDT(SEND_BUF);
    SEND_BUF[8]=',';

    if(reptype==0)
    {
        STRCPY(SEND_BUF+9,"EVENING");
        if(Shiftrec.DAY_SHIFT=='M')
            STRCPY(SEND_BUF+9,"MORNING");
    }
    else
    {
        STRCPY(SEND_BUF+9,"  BOTH ");
    }

    SEND_BUF[16]=',';

    Hex_DEC(0,USBsendrec.MEMBERS,SEND_BUF+17,4);
    SEND_BUF[21]=',';

    if(USBsendrec.MILK_TYPE=='C')
        STRCPY(SEND_BUF+22," COW");
    else if(USBsendrec.MILK_TYPE=='B')
    STRCPY(SEND_BUF+22," BUF");

    ZERO=1;
    SEND_BUF[26]=',';            

  
    Hex_DEC(2,USBsendrec.FAT,SEND_BUF+27,5);
    SEND_BUF[32]=',';
    if(CLR_BASED=='Y')
        Hex_DEC(2,USBsendrec.CLR1,SEND_BUF+33,5);
    else
        Hex_DEC(2,USBsendrec.SNF,SEND_BUF+33,5);
    SEND_BUF[38]=',';
    Hex_DEC(2,USBsendrec.QTY,SEND_BUF+39,8);
    SEND_BUF[47]=',';
    Hex_DEC(2,USBsendrec.RATE,SEND_BUF+48,6);
    SEND_BUF[54]=',';
    Hex_DEC(2,USBsendrec.AMOUNT,SEND_BUF+55,9);
    SEND_BUF[64]=',';
    Hex_DEC(2,USBsendrec.INC_AMT,SEND_BUF+65,9);
    SEND_BUF[74]=',';

    Hex_DEC(2,USBsendrec.TOTAL_AMOUNT,SEND_BUF+75,9);
    Write_USB_File(84);
}

void Send_Avg_Total(UINT8 reptype)
{
    ZERO=0;

    if((Cumrec.C_MEMBERS+Cumrec.B_MEMBERS)>0)
    {
        Calculate_Avg_Fat_Snf_Rate();
        if(Cumrec.C_MEMBERS>0)
        {        
            USBsendrec.MEMBERS=Cumrec.C_MEMBERS;
            USBsendrec.MILK_TYPE='C';
            USBsendrec.FAT=Cumrec.C_FAT;
            USBsendrec.SNF=Cumrec.C_SNF;
            USBsendrec.CLR1=Cumrec.C_CLR;
            USBsendrec.QTY=Cumrec.C_QTY;
            USBsendrec.RATE=Cumrec.C_RATE;
            USBsendrec.AMOUNT=Cumrec.C_AMOUNT;
            USBsendrec.INC_AMT=Cumrec.C_INC_AMT;
            USBsendrec.TOTAL_AMOUNT=Cumrec.C_AMOUNT+Cumrec.C_INC_AMT;
            
            Write_Summary_Avg_Total(reptype);
          
        }
        if(Cumrec.B_MEMBERS>0)
        {
            USBsendrec.MEMBERS=Cumrec.B_MEMBERS;
            USBsendrec.MILK_TYPE='B';
            USBsendrec.FAT=Cumrec.B_FAT;
            USBsendrec.SNF=Cumrec.B_SNF;
            USBsendrec.CLR1=Cumrec.B_CLR;
            USBsendrec.QTY=Cumrec.B_QTY;
            USBsendrec.RATE=Cumrec.B_RATE;
            USBsendrec.AMOUNT=Cumrec.B_AMOUNT;
            USBsendrec.INC_AMT=Cumrec.B_INC_AMT;
            USBsendrec.TOTAL_AMOUNT=Cumrec.B_AMOUNT+Cumrec.B_INC_AMT;
            
            Write_Summary_Avg_Total(reptype);
        }        
    }
}

void Send_Qty_Amount(UINT8 ucnt)
{
    ZERO=1;

    STRCPY(SEND_BUF,"TOTAL QUANTITY(L):");//1
    if(WEIGHT_KG=='Y')
        SEND_BUF[15]='K';
    Hex_DEC(2,TOTAL_QTY,SEND_BUF+18,10);
    Shift_Left(SEND_BUF+18,10);
    Write_USB_File(28);

    STRCPY(SEND_BUF,"TOTAL AMOUNT (Rs):");
    Hex_DEC(2,TOTAL_AMT,SEND_BUF+18,10);
    Shift_Left(SEND_BUF+18,10);
    Write_USB_File(28);
    
    ZERO=0;

    FSfclose(myFile);
}

void USB_Ext80_Payment_Register_Record(UINT8 ptype,UINT16 scode,UINT8 smilktype)
{
    UINT8 separator;
    INT32 u_net_amount;
    
    separator=',';
    if(ptype=='E')
        separator=' ';
  
    ZERO=0;
    
    Hex_DEC(0,scode,PBUF,4);
    
    ZERO=1;
    PBUF[4]=separator;                
  
    STRCPY(PBUF+5," COW");
    Buf_Type(smilktype+0x42,PBUF+6);
    PBUF[9]=separator;

    Hex_DEC(2,Balancerec.QTY,PBUF+10,9);
    PBUF[19]=separator;
 
    Hex_DEC(2,(Balancerec.CREDIT_AMT-Balancerec.INCENT_AMT),PBUF+20,9);
    PBUF[29]=separator;
    
    Hex_DEC(2,Balancerec.INCENT_AMT,PBUF+30,9);
    PBUF[39]=separator;
    
    Hex_DEC(2,Balancerec.CREDIT_AMT,PBUF+40,9);
    PBUF[49]=separator;
    
    Hex_DEC(2,Balancerec.PAID_AMT,PBUF+50,8);
    PBUF[58]=separator;
    
    Hex_DEC(2,Balancerec.DEBIT_AMT,PBUF+59,8);
    PBUF[67]=separator;
    PBUF[68]=' ';
    
    u_net_amount=Balancerec.CREDIT_AMT-(Balancerec.DEBIT_AMT+Balancerec.PAID_AMT);
    Hex_DEC(2,u_net_amount,PBUF+69,10);
    Shift_Left(PBUF+69,10);
     
    if(Balancerec.CREDIT_AMT>0 || Balancerec.DEBIT_AMT!=0)
    {
        USB_Print_SendString(ptype,0,79);
            
        Total_Summary(0x42+smilktype);
    }
}

UINT8 USB_Ext80_Payment_Register_Start(UINT8 ftype)
{
    if(ftype=='U')
    {
        if(Insert_USB_To_FileName('m')==0)
            return 0xFF;
      
        STRCPY(SEND_BUF,"CODE,MILK,TOTAL_QTY,AMOUNT,INCENTIVE,TOTAL_AMT,PAID_AMT, DED_AMT, NET_AMOUNT");
        Write_USB_File(76);
    }
    else
    {
        STRCPY(PBUF,"        PAYMENT REGISTER ");

        USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_POS_JUSTIFICATION_LEFT, USB_NULL, 0, 0 );
        Ext_Printer_SendString_3636Bold();
        
        EXT_PRINT_UNDERSCORE(80);
        Header_Footer_Print('E',0,4);

        EXT_PRINT_UNDERSCORE(80);
        Ext_Print_From_To_Date();
        STRCPY(PBUF,"  CODE MILK TOTAL_QTY   AMOUNT INCENTIVE TOTAL_AMT PAID_AMT DED_AMT   NET_AMOUNT");
        Ext_Printer_SendString_Bold();
        EXT_PRINT_UNDERSCORE(80);
    }
    
    return 1;
}

UINT8 USB_Memberbill_Start(void)
{
    if(Insert_USB_To_FileName('C')==0)
    {
        return 0xFF;
    }
   
    Send_Print_Code_Name('U');

    if(CollectionRecord.QTY!=0xFFFF )
    {
        STRCPY(SEND_BUF,"  DATE  , SHIFT ,MILK, FAT, SNF,QTY(L), RATE ,INCENTIVE, TOTAL_AMT,MILK_MODE,WT_MODE");
        CLR_Enabled(SEND_BUF+28);
        if(WEIGHT_KG=='Y')
            SEND_BUF[36]='K';
      
        Write_USB_File(84);//68
    }
    return 1;
}

void Display_Date(void)
{
    ZERO=0;
    Hex_DEC(0,Shiftrec.DAY,LBUF+0,2);
    LBUF[2]=LBUF[5]='/';
    Hex_DEC(0,Shiftrec.MONTH,LBUF+3,2);
    Hex_DEC(0,Shiftrec.YEAR,LBUF+6,2); 
    LBUF[8]=' ';
    LBUF[9]=Shiftrec.DAY_SHIFT;
    Disp_Msg(LINE7); 
}

void Send_EIP_Header(void)
{
    STRCPY(SEND_BUF,"000000000000000000001111-");
    Hex_DEC(0,DEVICE_CODE,SEND_BUF+20,4);
    Write_USB_File(25);    
}

void Send_EIP_Footer(UINT16 reccnt)
{
     STRCPY(SEND_BUF,"ENDENDENDENDENDEND123----");
    Hex_DEC(0,reccnt,SEND_BUF+18,3);
    Write_USB_File(25);
}

void Send_Shift_Report_Header(void)
{
    STRCPY(SEND_BUF,"  DATE  , TIME ,CODE,SHIFT,MILK, FAT, SNF, CLR,QUANTITY,  RATE,  INCENTIVE,  AMOUNT,MILK_MODE,WT_MODE");
    //CLR_Enabled(SEND_BUF+38);
//    if(CLR_BASED=='Y')
//    {
//        SEND_BUF[43]='S';SEND_BUF[44]='N';SEND_BUF[45]='F';
//    }
    Write_USB_File(101);
}

void Send_Shift_Report_Record(void)
{
    if(DATE_READ_FLAG==1 || SHIFT_READ_FLAG==1)
    {
        DATE_READ_FLAG=0;   
        SHIFT_READ_FLAG=0;
        Write_USB_File(0);
    }
    ZERO=0;
    Hex_DEC(0,Shiftrec.DAY,SEND_BUF,2);
    SEND_BUF[2]='/';
    Hex_DEC(0,Shiftrec.MONTH,SEND_BUF+3,2);
    SEND_BUF[5]='/';
    Hex_DEC(0,Shiftrec.YEAR,SEND_BUF+6,2);
    SEND_BUF[8]=',';
    Hex_DEC(0,CollectionRecord.WTIME[0] & 0x1F,SEND_BUF+9,2);
    SEND_BUF[11]=':';
    Hex_DEC(0,CollectionRecord.WTIME[1],SEND_BUF+12,2);
    SEND_BUF[14]=':';
    Hex_DEC(0,CollectionRecord.WTIME[2] & 0x7F,SEND_BUF+15,2);
    SEND_BUF[17]=',';
    

    Hex_DEC(0,CollectionRecord.CODE,SEND_BUF+18,4);
    SEND_BUF[22]=',';
    Send_Shift_Record(SEND_BUF+23);//64
    
    FSfwrite(SEND_BUF,1,87,myFile);
    Calc_Cum_Avg();//1
}

void Msg_Exporting_Data(void)
{
    Display_Boarder3();
     
    STRCPY(LBUF,"Exporting Shift Data");
    Disp_Msg(LINE5);
}

void Send_Total_Shift_Data(UINT8 tshifttype)
{
    UINT8  s_usb_flag;
    UINT16 e_rec_cnt=0;
    
    if(tshifttype=='U')
    {
        Import_Export_Header('T');

        if(Start_End_Date_Month()==0xFF)//U
            return;
       
        Import_Export_Header('T');

        Copy_MonthRecDT(LBUF);
        Disp_Msg(LINE3);
        
        if(Insert_USB_To()==0)
            return;
        Read_Member_CODE(1);
        Fromrec.START_MEMBER=Memberrec.C_CODE;
        Read_Member_CODE(NO_OF_CUSTOMERS);
        Fromrec.END_MEMBER=Memberrec.C_CODE;        
    }
  
    s_usb_flag=0;
   
    Msg_Exporting_Data();
    S_SNo=10000;
    E_SNo=10000;

    Shiftrec_SNo=S_SNo;
    SHIFT_READ_FLAG=0;
   
    Set_Initial_Dates();
    Init_Cum_Values();
    Fetch_Customer_Flag=1;    
       
    while(Fetch_Customer_Record())
    {
        Display_Date();
        
        if(CollectionRecord.QTY!=0xFFFF)
        {
            if(s_usb_flag==0)
            {
                if(tshifttype=='U' && Insert_USB_To_FileName(tshifttype)==0)
                    return;
                else if(tshifttype=='e' && Insert_USB_FileName(tshifttype)==0)
                    return;
                 
                if(tshifttype=='U')
                {
                    Send_Shift_Report_Header();
                }
                else
                {
                    Send_EIP_Header();
                }
                s_usb_flag=1;
            }

            if(tshifttype=='U')
            {
                Send_Shift_Report_Record();//1
            }
            else
            {
                if(SHIFT_READ_FLAG==1)
                {
                    SHIFT_READ_FLAG=0;
                    if(tshifttype=='e')
                    {
                        Send_EIP_Footer(e_rec_cnt);
                        FSfclose(myFile);                    

                        if(tshifttype=='e' && Insert_USB_FileName('e')==0)
                            return;
                        Send_EIP_Header();
                        e_rec_cnt=0;   
                    }
                }
                Send_EIP_Data();
                e_rec_cnt++;
            }
        }
        ZERO=0;              
    }
    if(s_usb_flag==1)
    {
        if(tshifttype=='e')
        {
            Send_EIP_Footer(e_rec_cnt);
        }
       
        FSfclose(myFile);
        
    }
    else
        no_data();
}

void Send_Detailed_Shift_Data(UINT8 tshifttype)
{
    UINT8  s_usb_flag;
    UINT16 e_rec_cnt=0,ccode;
    
    if(tshifttype=='d')
    {   
        if(Insert_USB_To()==0)
            return;
    }
   
    s_usb_flag=0;
    
    Msg_Exporting_Data();
    Init_Cum_Values();
    NO_COLLECTIONS=DynamicRead_BillDetails(10000);
    e_rec_cnt=0;
    Display_Date();
    
    for(ccode=Fromrec.START_MEMBER;ccode<=Fromrec.END_MEMBER;ccode++)
    {
        e_rec_cnt=0;
        while(e_rec_cnt<NO_COLLECTIONS)
        {
            CollectionRecord=CollectionRecords[e_rec_cnt];
            if(CollectionRecord.CODE==ccode || (TIMEWISE==1 && CollectionRecord.CODE>=Fromrec.START_MEMBER && CollectionRecord.CODE<=Fromrec.END_MEMBER ))
            {
                Read_Collection_Flags();

                if(CollectionRecord.QTY!=0xFFFF )
                {
                    if(s_usb_flag==0)
                    {
                        if(Insert_USB_FileName(tshifttype)==0)
                            return;

                        if(tshifttype=='d')
                        {
                            Send_Shift_Report_Header();
                        }
                        if(tshifttype=='E')
                            Send_EIP_Header();

                        s_usb_flag=1;
                    }
                    if(tshifttype=='d')
                        Send_Shift_Report_Record();

                    if(tshifttype=='E')
                        Send_EIP_Data();
                }
            }
            e_rec_cnt++;  
        }
        if(TIMEWISE==1)
                ccode=MAX_CODE+1;
    }
    if(s_usb_flag==1)
    {
        if(tshifttype=='d')
        {
            Send_Average_Header();
            Send_Avg_Total(0);
        }
        if(tshifttype=='E')
            Send_EIP_Footer(NO_COLLECTIONS);
        FSfclose(myFile);
    }
    else
        no_data();
}

UINT8 FatMin_Max(UINT8 fmilktype)
{
    FSFILE * f_myFile;
    UINT8 f_return_value,f_temp_count;

    FAT_MIN=0;
    FAT_MAX=0;

    Please_Wait_Msg(LINE5);

    if(fmilktype=='C')
        f_myFile = FSfopen("FAT_COW.csv","r");
   
    if(fmilktype=='B')
        f_myFile = FSfopen("FAT_BUF.csv","r");

    if(f_myFile == NULL)
     {
        Display_No_File_Msg(LINE5+1);
        return 0;
    }

     FAT_MIN=0;
     f_temp_count=0;

    while(1)
    {
        f_return_value=Read_Value(&myData[0], f_myFile);

        if(f_return_value==0)
            break;

        else if(f_temp_count==0)
        {
            if(FAT_MIN==0)
                FAT_MIN=atof(myData);
            else
                FAT_MAX=atof(myData);
        }

        if(f_temp_count==1)
        {
            f_temp_count=0;
        }
        else
            f_temp_count++;
    }

    FSfclose(f_myFile);

    if(FAT_MIN==0 && FAT_MAX==0)
    {
        Empty_File_Msg();

        return 0;
    }

    return 1;
}

UINT8 Read_Value(char *buf,FSFILE * file)
{
    UINT8 r_temp_char,r_i=0,r_num_bytes;

    while(1)
    {
        r_num_bytes=FSfread(&r_temp_char, 1, 1, file);

        if(r_num_bytes==0)
            return 0;
        if((r_temp_char>=0x30 && r_temp_char<=0x39 )|| r_temp_char=='.')
        {
            buf[r_i]=r_temp_char;
            r_i=r_i+1;
        }
        else if(r_temp_char=='\t' || r_temp_char==',')
        {
            buf[r_i]=0x00;
            return 0x01;
        }

        else if(r_temp_char=='\n')
        {
             buf[r_i]=0x00;
             return 0x02;
        }
    }
} 

UINT8 Read_Value1(char *buf,FSFILE * file)
{
    UINT8 r_temp_char,r_i=0,r_num_bytes;

    while(1)
    {
        r_num_bytes=FSfread(&r_temp_char, 1, 1, file);

        if(r_num_bytes==0)
            return 0;

        if(r_temp_char=='\t' || r_temp_char==',')
        {
            buf[r_i]=0x00;
            return r_i;
        }

        else if(r_temp_char=='\n')
        {
             buf[r_i]=0x00;
             return r_i;
        }
        else
        {
            buf[r_i]=r_temp_char;
            r_i=r_i+1;
        }
    }
}

UINT8 FAT_SNF_Min_Max(UINT8 fmilktype)
{
    FSFILE * myFile2;
    UINT16 f_read_limit;
    UINT8 f_temp_count;
    UINT8 f_return_value;


    FAT_MIN=0;
    FAT_MAX=0;
    SNF_MIN=0;
    SNF_MAX=0;

    if(fmilktype=='C')
    {
        STRCPY(SEND_BUF,"SNF_COW.csv");
    }

    if(fmilktype=='B')
    {
        STRCPY(SEND_BUF,"SNF_BUF.csv");
    }
    
    CLR_Enabled(SEND_BUF);
    myFile2 = FSfopen(SEND_BUF,"r");

    if(myFile2==NULL)
    {
        Display_No_File_Msg(LINE5+1);
        return 0;
    }
    f_temp_count=0;

    f_return_value=Read_Value1(&myData[0], myFile2);
    SNF_CHART_TYPE=myData[0];

    while(1)
    {
        f_return_value=Read_Value(&myData[0], myFile2);

        if(f_return_value==0)
            break;
        
        if((CLR_BASED=='N' && SNF_CHART_TYPE=='s') || (CLR_BASED=='Y' && SNF_CHART_TYPE=='c'))
        {
            if(FAT_MIN==0)
            {
                FAT_MIN=atof(myData);
                FAT_MAX=FAT_MIN;
            }
            else
                FAT_MAX=atof(myData);
        }
        else
        {
            if(SNF_MIN==0)
            {
                SNF_MIN=atof(myData);
                SNF_MAX=SNF_MIN;
            }
            else
                SNF_MAX=atof(myData);
        }
        
        if(f_return_value==2)
            break;

    }
    if((CLR_BASED=='N' && SNF_CHART_TYPE=='s') || (CLR_BASED=='Y' && SNF_CHART_TYPE=='c'))
        f_read_limit=((FAT_MAX*10)-(FAT_MIN*10))+1;
    else
        f_read_limit=((SNF_MAX*10)-(SNF_MIN*10))+1;

    f_temp_count=0;

    while(1)
    {
        f_return_value=Read_Value(&myData[0], myFile2);

        if(f_return_value==0)
            break;
        else if(f_temp_count==0)
        {
            if((CLR_BASED=='N' && SNF_CHART_TYPE=='s') || (CLR_BASED=='Y' && SNF_CHART_TYPE=='c'))
            {
                if(SNF_MIN==0)
                {
                    SNF_MIN=atof(myData);
                    SNF_MAX=SNF_MIN;
                }
                else
                    SNF_MAX=atof(myData);
            }
            else
            {
                if(FAT_MIN==0)
                {
                    FAT_MIN=atof(myData);
                    FAT_MAX=FAT_MIN;
                }
                else
                    FAT_MAX=atof(myData);
            }
        }
        
        if(f_temp_count==f_read_limit)
        {
            f_temp_count=0;
        }
        else
            f_temp_count++;
    }
    FSfclose(myFile2);

    if(FAT_MIN==0 || FAT_MAX==0 || SNF_MIN==0 || SNF_MAX==0)
    {
        Empty_File_Msg();
       
        return 0;
    }

    return 1;
}

UINT8 Export_Fat_Table(UINT8 fmilktype,UINT8 fstart)
{
    FSFILE * myFile_fat;
    UINT16 e_fat_cnt;
    BYTE fat_table_file_name[20];    

    Import_Export_Header(fmilktype);

    if(fstart==1 && Check_USB_Device()==0)
    {
        No_USB_Found_Msg();

        return 0xFF;
    }
    Display_Boarder3();

    STRCPY(LBUF,"Export Table:");
    Disp_Msg(LINE3);

    Please_Wait_Msg_Small(LINE5);

    if(fmilktype=='B')
        STRCPY(fat_table_file_name,"FAT_BUF.csv");
    else
        STRCPY(fat_table_file_name,"FAT_COW.csv");
    
    STRNCPY(LBUF,fat_table_file_name,11);
    Disp_Msg(LINE8);
    
    myFile_fat = FSfopen(fat_table_file_name,"r");

    if(myFile_fat != NULL)
    {
        FSfclose(myFile_fat);
        
        if(File_Over_Write()==0xFF)
            return;
    } 
    
    Display_Boarder3();

    Please_Wait_Msg_Small(LINE5);
   
    myFile_fat = FSfopen(fat_table_file_name,"w");
    
    if(myFile_fat == NULL)
    {
        Display_No_File_Msg(LINE5+1);
        return 0;
    }
    
    if(fmilktype=='C')
        EEPROM_ADDR=Table_Cow_FAT_Limits_Loc;
    else
        EEPROM_ADDR=Table_Buf_FAT_Limits_Loc;
    
    SEERAMReadString(EEPROM_ADDR,FAT_Limits_Length,(char *)&Fatlimitsrec);

    STRCPY(LBUF,"FAT:00.0 To 00.0");
    Hex_DEC(1,Fatlimitsrec.MIN_FAT,LBUF+4,4);
    Hex_DEC(1,Fatlimitsrec.MAX_FAT,LBUF+12,4);
    Disp_Msg(LINE3);

    ZERO=1;
    STRCPY(SEND_BUF,"FAT,RATE");
    SEND_BUF[8]='\r';
    SEND_BUF[9]='\n';
    FSfwrite(SEND_BUF,1,10,myFile_fat);
    if(Fatlimitsrec.MIN_FAT!=0xFFFF && Fatlimitsrec.MAX_FAT!=0xFFFF)
    {
        STRCPY(LBUF,"TRANSFERING DATA...");
        Disp_Msg(LINE5);

        for(e_fat_cnt=Fatlimitsrec.MIN_FAT;e_fat_cnt<=Fatlimitsrec.MAX_FAT;e_fat_cnt++)
        {
            Read_FAT_MilkTable(fmilktype,e_fat_cnt-Fatlimitsrec.MIN_FAT);

            Hex_DEC(1,e_fat_cnt,SEND_BUF,4);
            SEND_BUF[4]=',';
            Hex_DEC(2,Milkrec.RATE,SEND_BUF+5,6);
            SEND_BUF[11]='\r';
            SEND_BUF[12]='\n';

            FSfwrite(SEND_BUF,1,13,myFile_fat);
        }
    }

    FSfclose(myFile_fat);
}

UINT8 Export_SNF_Table(UINT8 snfmilktype,UINT8 estart)
{
    UINT16 e_fat_cnt,e_snf_cnt,e_b_cnt;
    FSFILE * myFile_snf;
    BYTE snf_table_file_name[20];

    Import_Export_Header(snfmilktype+32);

    if(estart==1 && Check_USB_Device()==0)
    {
        No_USB_Found_Msg();
        return;
    }

    STRCPY(LBUF,"Export Table:");
    Disp_Msg(LINE3);

    STRCPY(LBUF,"Please Wait...");
    Disp_Msg(LINE6);

    if(snfmilktype=='C')
        STRCPY(snf_table_file_name,"SNF_COW.csv");
    else
        STRCPY(snf_table_file_name,"SNF_BUF.csv");
    
    STRNCPY(LBUF,snf_table_file_name,11);
    Disp_Msg(LINE8);
    
    CLR_Enabled(snf_table_file_name);

    myFile_snf = FSfopen(snf_table_file_name,"r");

    if(myFile_snf != NULL)
    {
        FSfclose(myFile_snf);

        if(File_Over_Write()==0xFF)
            return;
    }

    Display_Boarder3();

    Please_Wait_Msg_Small(LINE5);

    
    myFile_snf = FSfopen(snf_table_file_name,"w");

    if(myFile_snf == NULL)
    {
        Display_No_File_Msg(LINE5+1);
        return 0;
    }
    
    STRCPY(LBUF,"Transferring Data...");
    Disp_Msg(LINE5);
    
    if(snfmilktype=='C')
        EEPROM_ADDR=Table_Cow_SNF_Limits_Loc;
    else
        EEPROM_ADDR=Table_Buf_SNF_Limits_Loc;

    SEERAMReadString(EEPROM_ADDR,SNF_Limits_Length,(char *)&SNFLimitsrec);

    ZERO=1;

    if(SNFLimitsrec.MIN_FAT!=0xFFFF && SNFLimitsrec.MAX_FAT!=0xFFFF && SNFLimitsrec.MIN_SNF!=0xFFFF && SNFLimitsrec.MAX_SNF!=0xFFFF)
    {
        SEND_BUF[0]='f';
        SEND_BUF[1] ='/';
        SEND_BUF[2] = CLR_BASED == 'Y' ? 'c' :'s';
        SEND_BUF[3]=',';

        e_b_cnt=4;
        for(e_snf_cnt=SNFLimitsrec.MIN_SNF;e_snf_cnt<=SNFLimitsrec.MAX_SNF;e_snf_cnt++)
        {
            Hex_DEC(1,e_snf_cnt,SEND_BUF+e_b_cnt,6);
            e_b_cnt=e_b_cnt+6;
           if(e_snf_cnt<SNFLimitsrec.MAX_SNF)
            {
                SEND_BUF[e_b_cnt]=',';
                e_b_cnt=e_b_cnt+1;
            }
        }

        SEND_BUF[e_b_cnt]='\r';
        e_b_cnt=e_b_cnt+1;
        SEND_BUF[e_b_cnt]='\n';
        e_b_cnt=e_b_cnt+1;

        FSfwrite(SEND_BUF,1,e_b_cnt,myFile_snf);
        ZERO=0;
        for(e_fat_cnt=SNFLimitsrec.MIN_FAT;e_fat_cnt<=SNFLimitsrec.MAX_FAT;e_fat_cnt++)
        {
            Hex_DEC(1,e_fat_cnt,SEND_BUF,4);
            SEND_BUF[4]=',';
            e_b_cnt=5;
            for(e_snf_cnt=SNFLimitsrec.MIN_SNF;e_snf_cnt<=SNFLimitsrec.MAX_SNF;e_snf_cnt++)
            {
                Tablerec.MILK_FAT=e_fat_cnt-SNFLimitsrec.MIN_FAT;
                Tablerec.MILK_SNF=e_snf_cnt-SNFLimitsrec.MIN_SNF;
                Tablerec.MILK_TYPE=snfmilktype;

                Read_SNF_MilkTable();

                Hex_DEC(2,Tablerec.MILK_RATE,SEND_BUF+e_b_cnt,6);
                e_b_cnt=e_b_cnt+6;

                if(e_snf_cnt<SNFLimitsrec.MAX_SNF)
                {
                    SEND_BUF[e_b_cnt]=',';
                    e_b_cnt=e_b_cnt+1;
                }
                STRCPY(LBUF,"F:00.0 S:00.0 000.00");
                Hex_DEC(1,e_fat_cnt,LBUF+2,4);
                Hex_DEC(1,e_snf_cnt,LBUF+9,4);
                Hex_DEC(2,Tablerec.MILK_RATE,LBUF+14,6);
                
                Disp_Msg(LINE7);
            }
            SEND_BUF[e_b_cnt]='\r';
            e_b_cnt=e_b_cnt+1;
            SEND_BUF[e_b_cnt]='\n';
            e_b_cnt=e_b_cnt+1;
            FSfwrite(SEND_BUF,1,e_b_cnt,myFile_snf);
        }
    }
    FSfclose(myFile_snf);
}

void Export_Members(void)
{    
    BYTE e_member_list[25];
    UINT16 e_member_sno;

    Import_Export_Header('M');

    if(NO_OF_CUSTOMERS==0)
    {
        STRCPY(LBUF,"MEMBERS LIST EMPTY");
        Disp_Msg(LINE4);

        Delay_Sec(1);

        return;
    }
    
    STRCPY(LBUF,"Export Members?");
    if(Confirm_Yes()==1)
    {
        if(Check_USB_Device()==0)
        {
            No_USB_Found_Msg();
            return;
        }

        Please_Wait_Msg(LINE5);

        STRNCPY(e_member_list,MACHINE_ID,7);
        STRCPY(e_member_list+7,"Members_List.csv");

        myFile = FSfopen(e_member_list,"r");

        if(myFile != NULL)
        {
            FSfclose(myFile);
            
            STRNCPY(LBUF,e_member_list,19);
            Disp_Msg(LINE8);
                 
            if(File_Over_Write()==0xFF)
                return;
        }

        Display_Boarder3();

        STRCPY(LBUF,"Transferring Data...");
        Disp_Msg(LINE5);

        myFile = FSfopen(e_member_list,"w");

        if(myFile == NULL)
        {
            Display_No_File_Msg(LINE5+1);
            return;
        }

        STRCPY(SEND_BUF,"CODE,MILK,INC,MEMBER NAME         ,CONTACT NO,STATUS,BAR CODE  ");
        Write_USB_File(63);        
        
        for(e_member_sno=1;e_member_sno<=NO_OF_CUSTOMERS;e_member_sno++)
        {
            Read_Member_Status(e_member_sno);
            
            if((Memberrec.STATUS=='A' || Memberrec.STATUS=='D'))
            {
                Read_Member_Details(e_member_sno);

                STRCPY(SEND_BUF,"    ,    ,   ,                    ,          ,      ,           ");

                Hex_DEC(0,Memberrec.C_CODE,SEND_BUF,4);
                SEND_BUF[5]=Memberrec.MILK_TYPE;
                SEND_BUF[10]=Memberrec.COMMISSION_TYPE;
                STRNCPY(SEND_BUF+14,Memberrec.NAME,20);
                STRNCPY(SEND_BUF+35,Memberrec.CONTACT_NO,10);
                SEND_BUF[46]=Memberrec.STATUS;
                STRNCPY(SEND_BUF+53,Memberrec.BARCODE,10);
                Write_USB_File(63);
            }
            STRCPY(LBUF,"Member Code:");
            Hex_DEC(0,Memberrec.C_CODE,LBUF+12,4);            
            Disp_Msg(LINE5);
        }
        FSfclose(myFile);
    }
}

UINT8 Download_FatTable(UINT8 fmilktype)
{    
    UINT8 d_return_value,d_temp_count;
    UINT32 d_offset_address;

    if(fmilktype=='C')
        myFile = FSfopen("FAT_COW.csv","r");

    if(fmilktype=='B')
        myFile = FSfopen("FAT_BUF.csv","r");

    if(myFile == NULL)
    {
        Display_No_File_Msg(LINE5+1);
        return 0;
    }

    numBytes=2;

    if(fmilktype=='C')
        d_offset_address=EFFECT_RATE*512;
    else
        d_offset_address=256+(EFFECT_RATE*512);
    
    FLASHReadString(Cow_FatTable_Loc,4096,(char *)&Flash_Buffer);
 
    STRCPY(LBUF,"RECEIVING DATA......");
    Disp_Msg(LINE5);

    Milkrec.FAT= Fatlimitsrec.MIN_FAT;

    d_return_value=Read_Value(&myData[0], myFile);

    if((myData[0]>0x39 || myData[0]<0x30) && myData[0]!='.')
    {
        d_return_value=Read_Value(&myData[0], myFile);
        d_return_value=Read_Value(&myData[0], myFile);
    }
    d_temp_count=1;

    while(1)
    {
        d_return_value=Read_Value(&myData[0], myFile);

        if(d_return_value==0)
            break;

        else if(d_temp_count==0)
        {
            Milkrec.FAT=Milkrec.FAT+1;
        }

        if(d_temp_count==1)
        {
            Milkrec.RATE=Float_Rate(myData);
            if((Milkrec.FAT-Fatlimitsrec.MIN_FAT)<=120)
            STRNCPY((char *)&Flash_Buffer[d_offset_address+((Milkrec.FAT-Fatlimitsrec.MIN_FAT)*Fat_Record_Length)],(char *)&Milkrec.RATE,2);
            d_temp_count=0;
        }
        else
            d_temp_count++;
    }

    Update_FAT_FLASH();
    FSfclose(myFile);
    return 1;
}

UINT8 Download_SNFTable(UINT8 smilktype)
{
    UINT8 d_return_value,d_temp_count;
    UINT16 d_fat_value,d_read_cnt;    
    FSFILE * myFile2;
        
    if(smilktype=='C') 
    {
        STRCPY(SEND_BUF,"SNF_COW.csv");      
    }

    if(smilktype=='B')
    {
        STRCPY(SEND_BUF,"SNF_BUF.csv");        
    }
    CLR_Enabled(SEND_BUF);
    myFile2 = FSfopen(SEND_BUF,"r");
    
    if(myFile2==NULL)
    {
        Display_No_File_Msg(LINE5+1);

        return 0;
    }

    d_fat_value=SNFLimitsrec.MIN_FAT;

    Erase_SNF_Table(smilktype);

    if((CLR_BASED=='N' && SNF_CHART_TYPE=='s') || (CLR_BASED=='Y' && SNF_CHART_TYPE=='c'))
    {
        d_read_cnt=(SNFLimitsrec.MAX_FAT-SNFLimitsrec.MIN_FAT)+1;
    }
    else
    {
        d_read_cnt=(SNFLimitsrec.MAX_SNF-SNFLimitsrec.MIN_SNF)+1;
    }
    while(1)
    {
        d_return_value=Read_Value(&myData[0],myFile2);

        if(d_return_value==2)
            break;
    }

    d_return_value=Read_Value(&myData[0],myFile2);
    d_temp_count=1;

    Tablerec.MILK_FAT=0;
    Tablerec.MILK_SNF=0;
    Tablerec.MILK_TYPE=smilktype;

    while(1)
    {
        d_return_value=Read_Value(&myData[0], myFile2);

        if(d_return_value==0)
            break;
        
        else if(d_temp_count==0)
        {
            if((CLR_BASED=='N' && SNF_CHART_TYPE=='s') || (CLR_BASED=='Y' && SNF_CHART_TYPE=='c'))
            {
                Tablerec.MILK_SNF=Tablerec.MILK_SNF+1;
                Tablerec.MILK_FAT=0;
                if(Tablerec.MILK_SNF==(SNFLimitsrec.MAX_SNF-SNFLimitsrec.MIN_SNF)+1)
                    break;              
            }
            else
            {
                Tablerec.MILK_FAT=Tablerec.MILK_FAT+1;
                Tablerec.MILK_SNF=0;
                if(Tablerec.MILK_FAT==(SNFLimitsrec.MAX_FAT-SNFLimitsrec.MIN_FAT)+1)
                    break;
            }
        }

        else if(d_temp_count!=0)
        {
            Tablerec.MILK_RATE=Float_Rate(myData);
            STRCPY(LBUF,"F:00.0 S:00.0 000.00");
            if(CLR_BASED=='Y')
                LBUF[7]='C';
            Hex_DEC(1,(Tablerec.MILK_FAT+SNFLimitsrec.MIN_FAT),LBUF+2,4);
            Hex_DEC(1,(Tablerec.MILK_SNF+SNFLimitsrec.MIN_SNF),LBUF+9,4);
            Hex_DEC(2,Tablerec.MILK_RATE,LBUF+14,6);
            Disp_Msg(LINE7);
            
            Write_SNF_MilkTable();
            
            if((CLR_BASED=='N' && SNF_CHART_TYPE=='s') || (CLR_BASED=='Y' && SNF_CHART_TYPE=='c'))
            {
                Tablerec.MILK_FAT=Tablerec.MILK_FAT+1;
            }
            else
            {
                Tablerec.MILK_SNF=Tablerec.MILK_SNF+1;
            }
        }

        if(d_temp_count==d_read_cnt)
        {
            d_temp_count=0;
        }
        else
            d_temp_count++;
    }

    FSfclose(myFile2);
    return 1;
}

UINT8 Check_USB(UINT8 ucheck)
{
    if(ucheck==1)
    {
        Display_Boarder3();
        STRCPY(LBUF,"Download Rate Chart?");

        if(Confirm_Yes()!=1)
            return 0xFF;
    }

    if(ucheck==1 &&Check_USB_Device()==0)
    {
        No_USB_Found_Msg();

        return 0xFF;
    }
    return 1;
}

UINT8 Import_FatTable(UINT8 fmilktype,UINT8 iusbcheck)
{
    Import_Export_Header(fmilktype);

    if(Check_USB(iusbcheck)==0xFF)
        return 0xFF;
   
    FileCheck=FatMin_Max(fmilktype);

    Display_Boarder3();

    if(FileCheck==1)
    {
        Fatlimitsrec.MIN_FAT=FAT_MIN*10;
        Fatlimitsrec.MAX_FAT=FAT_MAX*10;
        if(Fatlimitsrec.MAX_FAT>(Fatlimitsrec.MIN_FAT+120))
        {
                Fatlimitsrec.MAX_FAT=Fatlimitsrec.MIN_FAT+120;
        }

        if(EFFECT_RATE==1)
        {
            STRCPY(LBUF,"EFFECTIVE CHART:");
            Disp_Msg(LINE4);
        }
        else
        {
            EFFECT_RATE=0;
        }
        
        Write_FAT_Limits(fmilktype);
        Read_FAT_Limits(fmilktype);
       
        Download_FatTable(fmilktype);
        return 1;
    }
}

UINT8 Import_SNFTable(UINT8 smilktype,UINT8 iusbcheck)
{
    UINT8 total_slabs=0;
    
    Import_Export_Header(smilktype+32);

    if(Check_USB(iusbcheck)==0xFF)
        return 0xFF;
    
    FileCheck=FAT_SNF_Min_Max(smilktype);
    
    if(FileCheck==1)
    {
        SNFLimitsrec.MIN_FAT=FAT_MIN*10;
        SNFLimitsrec.MAX_FAT=FAT_MAX*10;

        SNFLimitsrec.MIN_SNF=SNF_MIN*10;
        SNFLimitsrec.MAX_SNF=SNF_MAX*10;
        
        if(SNFLimitsrec.MAX_FAT>(SNFLimitsrec.MIN_FAT+120))
        {
                SNFLimitsrec.MAX_FAT=SNFLimitsrec.MIN_FAT+120;
        }
        if(SNFLimitsrec.MAX_SNF>(SNFLimitsrec.MIN_SNF+120))
        {
                SNFLimitsrec.MAX_SNF=SNFLimitsrec.MIN_SNF+120;
        }

        Display_Boarder3();

        if(EFFECT_RATE==1)
        {
            STRCPY(LBUF,"EFFECTIVE CHART:");
            Disp_Msg(LINE4);
        }
        else
        {
            EFFECT_RATE=0;
          
        }
               
        Write_SNF_Limits(smilktype);
        Read_SNF_Limits(smilktype);

        Download_SNFTable(smilktype);        
      
        EEPROM_ADDR=Get_Slabs_Table_Addr(smilktype);
        SEERAMWriteString(EEPROM_ADDR,1,(char *)&total_slabs);
        return 1;
    }
}

void Download_Members(void)
{
    BYTE myData[100],d_member_list[25];
    UINT8 d_return_value,i_cnt;
    UINT16 d_total_members;
    

    Import_Export_Header('M');

    STRCPY(LBUF,"Download Members?");

    if(Confirm_Yes()==1)
    {
        Import_Export_Header('M');

        if(Check_USB_Device()==0)
        {
            No_USB_Found_Msg();
            
            return;
        }

        Please_Wait_Msg(LINE5);

        STRNCPY(d_member_list,MACHINE_ID,7);
        STRCPY(d_member_list+7,"Members_List.csv");

        myFile = FSfopen(d_member_list,"r");

        if(myFile==NULL)
        {
            Display_No_File_Msg(LINE5+1);
            return;
        }
        d_total_members=0;
       
        myFile = FSfopen(d_member_list,"r");

        for(i_cnt=0;i_cnt<7;i_cnt++)
            d_return_value=Read_Value1(&myData[0], myFile);

        while(1)
        {
            d_return_value=Read_Value1(&myData[0], myFile);
            
            if(d_return_value==0)
                break;
            
            Memberrec.C_CODE=atoi(myData);
            d_return_value=Read_Value1(&myData[0], myFile);
            Memberrec.MILK_TYPE=myData[0];

            d_return_value=Read_Value1(&myData[0], myFile);
            Memberrec.COMMISSION_TYPE=myData[0];

            d_return_value=Read_Value1(&myData[0], myFile);
            STRNCPY(Memberrec.NAME,myData,d_return_value);

            d_return_value=Read_Value1(&myData[0], myFile);
            STRNCPY(Memberrec.CONTACT_NO,myData,d_return_value);

            d_return_value=Read_Value1(&myData[0], myFile);
            Memberrec.STATUS=myData[0];
            
            d_return_value=Read_Value1(&myData[0], myFile);
            STRNCPY(Memberrec.BARCODE,myData,d_return_value);

            d_total_members=d_total_members+1;

            if(d_total_members==1)
            {
                Erase_Member_Details();
            }
            if(d_total_members>1000)
                break;

            STRCPY(LBUF,"MEMBERS... 0000");
            Hex_DEC(0,d_total_members,LBUF+11,4);
            Disp_Msg(LINE7);

            Write_Member_Details_Flash(d_total_members);

        }
        
        NO_OF_CUSTOMERS=d_total_members;
        FSfclose(myFile);
        
        STRCPY(LBUF," DOWNLOAD COMPLETED");
        Disp_Msg(LINE7);

        Delay_Sec(1);
    }
}


