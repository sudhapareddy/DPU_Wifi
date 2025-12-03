void Help_Context(void)
{  
    STRCPY(LBUF,"Back:      |Next:   ");   
    LBUF[5]=0x7F;LBUF[17]=0x7E;
    Disp_Msg(LINE7);
    
    STRCPY(LBUF,"Save:ENTER |Exit:Esc"); 
    CURSOR_BLINK=0x80;
    Disp_Msg(LINE8);
    
    CURSOR_BLINK=0x00;
}

void Restart_System(UINT8 rflag)
{
    Display_Boarder3();

    if(rflag=='A')
        STRCPY(LBUF," ANALYZER CHANGED...");
    
    if(rflag=='D')
        STRCPY(LBUF,"  DATE CHANGED...");
    
    if(rflag=='M')
        STRCPY(LBUF,"   MEMORY CHANGED...");
    
    Disp_Msg(LINE4);

    STRCPY(LBUF," RESTART THE SYSTEM");
    Disp_Msg(LINE6);

    while(1)
       Buzzer(250);
}

void Read_Effective_Dates(UINT8 limittype,UINT8 effecttype)
{
    if(effecttype=='C')
        EEPROM_ADDR=C_BufFat_Effect_Date_Loc+(limittype*3); //current 
    else
        EEPROM_ADDR=New_BufFat_Effect_Date_Loc+(limittype*3);   //effective
    
    SEERAMReadString(EEPROM_ADDR,3,(char *)&Time[4]);

    if(Time[4]==0xFF || Time[5]==0xFF || Time[6]==0xFF)//1
    {
        Read_RTC();
        Time[4]=HEX_ASCII(Time[4]);
        Time[5]=HEX_ASCII(Time[5]);
        Time[6]=HEX_ASCII(Time[6]);
    }
}

void Display_Date_Time(UINT8 curline)
{
    LBUF[0]=0x83;
    LBUF[9]=0x83;
    LBUF[15]=0x85;
    
    LBUF[3]=LBUF[6]='/';
    LBUF[16]=0x00;
    
    
    if(curline==LINE5)
    {
        Hex_ASCII(Time[6],LBUF+1);
        Hex_ASCII(Time[5],LBUF+4);
        Hex_ASCII(Time[4],LBUF+7);
        Copy_Time(LBUF+10);
        LBUF[12]=':';
        NOR_DISPLAY_LAST_LINE=0x60;
        Disp_Msg_Big(0,curline);
    }
    else
    {
        Hex_DEC(0,Time[6],LBUF+1,2);
        Hex_DEC(0,Time[5],LBUF+4,2);
        Hex_DEC(0,Time[4],LBUF+7,2);
        LBUF[9]=' ';
        
        NOR_DISPLAY_LAST_LINE=0xC0;
        Disp_Msg_No_Spaces(0,curline);
    }    
}

void Date_Change(void)
{
    UINT8 d_temp_char;

    Read_RTC();
    SEERAMReadString(Date_Change_Loc,1,(char *)&d_temp_char);

    if(d_temp_char!=Time[4])
    {
        d_temp_char=0;
        SEERAMWriteString(Current_SIM_Loc,1,(char *)&d_temp_char);

        STRCPY(SMSrec.SIM_NO,"                    ");
        SMSrec.COUNTER=0;
        Write_SMS_Counter();
        
        d_temp_char=Time[4];
        SEERAMWriteString(Date_Change_Loc,1,(char *)&d_temp_char);
    }
}


void SetDate_Time(UINT8 limit)
{
    UINT8 sel_step,cur_line,max_days;    
    UINT8 temp_year,temp_month,temp_days,temp_hrs,temp_mins;
    
    //0-BUF FAT  1-COW FAT  2-BUF SNF  3-COW SNF

    if(limit==5)
        Admin_Header('S');
    else
    {
        if(limit<2)
            Import_Export_Header(0x42+limit);
        else
            Import_Export_Header(0x60+limit);
    }        
    ZERO=0;
    
    if(limit==5)
    {
        Help_Context();
        
        STRCPY(LBUF," YY/MM/DD HH:MM");
        LBUF[0]=0x83;
        LBUF[9]=0x83;
        LBUF[15]=0x85;
        
        NOR_DISPLAY_LAST_LINE=0x30;
        Disp_Msg_Big(0,LINE3);
        Reset_Display_Values();
        
        cur_line=LINE5;
    }
    else
    {
        Display_Boarder3();
        STRCPY(LBUF,"Current Effective");
        Disp_Msg(LINE3);

        Read_Effective_Dates(limit,'C');
        
        STRCPY(LBUF,"Date:");
        Hex_DEC(0,Time[6],LBUF+5,2);
        Hex_DEC(0,Time[5],LBUF+8,2);
        Hex_DEC(0,Time[4],LBUF+11,2);
        LBUF[7]=LBUF[10]='/';
        Disp_Msg(LINE4);
        
        Display_Box_Top_Boarder(LINE5,0x18);
        
        STRCPY(LBUF,"New Date(YY/MM/DD):");
        Disp_Msg(LINE6);
              
        Read_Effective_Dates(limit,'N');

        cur_line=LINE7;
        limit=3;
    }
    
    DISPCHAR_TYPE='N';
    sel_step=6;
    //DISPLAY_LAST_LINE=0x70;
    while(1)
    {
        Display_Date_Time(cur_line);   
        if(limit==5)
            Hex_ASCII(Time[sel_step],Temp_Arr);
        else
            Hex_DEC(0,Time[sel_step],Temp_Arr,2);
        Temp_Arr[2]=0x00;
        
       // INVERT_DISPLAY=0;
        
        switch(sel_step)
        {
            case 6:

                //DISPLAY_LAST_LINE=0xC0;
                
                GET_VALUE_STATUS=Get_Numeric_Big(2,0,cur_line+1,(char *)&Temp_Arr);

                if(KBV==KB_RIGHT)
                {
                    temp_year=atoi(Temp_Arr);
                    sel_step=5;
                }
                
                else if(GET_VALUE_STATUS==0xFF && KBV !=KB_LEFT)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    temp_year=atoi(Temp_Arr);

                    if(temp_year>0)
                    {
                        Time[6]=ASCII_BCD(Temp_Arr);  
                        if(limit==3)
                            Time[6]=HEX_ASCII(Time[6]);
                        sel_step=5;
                    }
                }
                break;

            case 5:
                
                GET_VALUE_STATUS=Get_Numeric_Big(2,0,cur_line+4,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                {
                    sel_step=6;
                }
                else if(KBV==KB_RIGHT)
                {
                    sel_step=4;
                    temp_month=atoi(Temp_Arr);
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else
                {
                    temp_month=atoi(Temp_Arr);

                    if(temp_month>=1 && temp_month<=12)
                    {
                        Time[5]=ASCII_BCD(Temp_Arr);  
                        if(limit==3)
                            Time[5]=HEX_ASCII(Time[5]);
                        sel_step=4;
                    }
                }
                break;
                
            case 4:

                max_days=Get_Month_Days(temp_month,temp_year);
                
                GET_VALUE_STATUS=Get_Numeric_Big(2,0,cur_line+7,(char *)&Temp_Arr);

                if(KBV==KB_LEFT)
                    sel_step=5;
                else if(KBV==KB_RIGHT)
                {
                    temp_days=atoi(Temp_Arr);
                    if(limit==3)
                        sel_step=6;
                    else
                        sel_step=2;
                }
                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    temp_days=atoi(Temp_Arr);

                    if(temp_days>0 && temp_days<=max_days)
                    {
                        Time[4]=ASCII_BCD(Temp_Arr); 
                        if(limit==3)
                        {
                            Time[4]=HEX_ASCII(Time[4]);
                            sel_step=0;
                        }
                        else
                            sel_step=2;   
                    }
                }
                break;
                
            case 2:
                
                GET_VALUE_STATUS=Get_Numeric_Big(2,0,cur_line+10,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                    sel_step=4;
                else if(KBV==KB_RIGHT)
                {
                    temp_hrs=atoi(Temp_Arr);
                    sel_step=1;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else
                {
                    temp_hrs=atoi(Temp_Arr);

                    if(temp_hrs>=00 && temp_hrs<=23)
                    {
                        Time[2]=ASCII_BCD(Temp_Arr);                   
                        sel_step=1;
                    }
                }
                break;

            case 1:
                         
                GET_VALUE_STATUS=Get_Numeric_Big(2,0,cur_line+13,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                    sel_step=2;
                
                else if(KBV==KB_RIGHT)
                {
                    temp_mins=atoi(Temp_Arr);
                    sel_step=6;
                }

                else if(GET_VALUE_STATUS==0xFF)
                {
                    return;
                }

                else
                {
                    temp_mins=atoi(Temp_Arr);

                    if(temp_mins>=0 && temp_mins<=59)
                    {
                        Time[1]=ASCII_BCD(Temp_Arr);                            
                        sel_step=0;
                    }
                }
                break;
                
            case 0:
                
                if(limit==5)
                {
                    Set_RTC();
                    
                    STRCPY(LBUF," DATE & TIME UPDATED");
                    Invert_Disp_Msg(LINE8);

                    Delay_Sec(1);

                    if(GPRSrec.GPRS_ON=='Y' && ON_LINE=='Y')
                        Date_Change();

                    Restart_System('D');        
                }
                else
                {
                    SEERAMWriteString(EEPROM_ADDR,3,(char *)&Time[4]);

                    STRCPY(LBUF,"Updated Successfully");
                    Invert_Disp_Msg(LINE5);
                    Delay_Sec(1);
                    Read_RTC();
                    return;
                }
   
        }
    }
}


void DPU_Control_Settings(void)
{
    DPUcontrolrec.MEMBER_LIST='Y';
    DPUcontrolrec.RATE_TABLES='Y';
    DPUcontrolrec.COLLECTION='Y';
    SEERAMWriteString(MemberList_Control_Loc,3,(char *)&DPUcontrolrec);
}

void Set_Default_Settings(void)
{
    Settingsrec.WEIGHT_MODE=1;
    Settingsrec.FAT_MODE=1;
    Settingsrec.ANALYZER='U';
    Settingsrec.COW_SNF='Y';
    Settingsrec.BUF_SNF='Y';
    Settingsrec.HIGH_FAT_SNF='Y';
    Settingsrec.LOW_FAT_SNF='Y';
    Settingsrec.MIXED_MILK='N';
    Settingsrec.SHIFT_CLOSE='N';
}

void Write_Offline_Settings(void)
{    
    Please_Wait_Msg(LINE5);
    
    Set_Default_Settings();

    SEERAMWriteString(Weight_Mode_Loc,9,(char *)&Settingsrec);    

    DPU_Control_Settings();

    MACHINE_LOCK='N';
    SEERAMWriteString(Machine_Lock_Loc,1,(char *)&MACHINE_LOCK);
}

void Device_ID(void)
{
    TEMP_BUF[0]=MACHINE_ID[3];
    TEMP_BUF[1]=MACHINE_ID[4];
    TEMP_BUF[2]=MACHINE_ID[5];
    TEMP_BUF[3]=MACHINE_ID[6];
    TEMP_BUF[4]=0x00;
    DEVICE_CODE=atoi(TEMP_BUF);
}

void Write_Default_Dealer_Settings(void)
{    
    Please_Wait_Msg(LINE5);
    
    SEERAMWriteString(Dairy_Name_Loc,32,(char *)&M_COMPANY);
    SEERAMWriteString(Dealer_Name_Loc,16,(char *)&M_DEALER);
    SEERAMWriteString(Brand_Name_Loc,16,(char *)&M_BRAND);    
    
    SEERAMWriteString(Mfg_Date_Loc,6,(char *)&M_MFG_DATE);

    SEERAMWriteString(Machine_Code_Loc,7,(char *)&M_DEVICE_NO);

    SEERAMWriteString(Batch_No_Loc,2,(char *)&M_BATCH_NO);
    
    SEERAMWriteString(Tare_Command_Loc,1,(char *)&M_TARE_COMMAND);
   
    
    SEERAMWriteString(Operator_Password_Loc,2,(char *)&M_USER_LOG_PASSWORD);
    SEERAMWriteString(Admin_Password_Loc,2,(char *)&M_LOG_PASSWORD); 
}

void Default_Settings(UINT8 resettype)
{
    UINT8 clr_flag,d_f_id,d_flash_status;
    UINT8 mfg_date[6];
    UINT16 d_temp_int,batch_no;    
    UINT32 templongno;

    FLASHReadString((Global_Variables_Loc+No_Of_Flashes_Loc),1,(char *)&NO_OF_FLASHES);    

    SEERAMReadString(Dealer_Selection_Loc,1,(char *)&DEALER_SELECTION);
   
    if(DEALER_SELECTION==0xFF)
        DEALER_SELECTION=2;
    
    
    SEERAMReadString(Machine_Code_Loc,7,(char *)&MACHINE_ID);
    
    if(MACHINE_ID[3]==0xFF && MACHINE_ID[4]==0xFF && MACHINE_ID[5]==0xFF && MACHINE_ID[6]==0xFF)
        STRNCPY(MACHINE_ID,(char *)&M_DEVICE_NO,7);        

    Device_ID();
    
    SEERAMReadString(Batch_No_Loc,2,(char *)&batch_no);
    
    if(batch_no==0xFFFF)
        batch_no=M_BATCH_NO;//117;

    FLASHReadString((Global_Variables_Loc+Default_Settings_Loc),1,(char *)&clr_flag);

    SEERAMReadString(Mfg_Date_Loc,6,(char *)&mfg_date);
    if(mfg_date[0]==0xFF)
        STRNCPY(mfg_date,(char *)&M_MFG_DATE,6);
    
    SEERAMReadString(On_Offline_Loc,1,(char *)&ON_LINE);
    FLASHReadString((Global_Variables_Loc+GPRS_ON_Loc),20,(char *)&GPRSrec);    
    FLASHReadString((Global_Variables_Loc+Domain_Name_Loc),20,(char *)&DOMAIN_SERVER_NAME);  
        
    FLASHReadString((Global_Variables_Loc+Weight_Mode_Loc),SETTINGS_RECORD_LENGTH,(char *)&Settingsrec);
    FLASHReadString((Global_Variables_Loc+Accuracy_Loc),WM_RECORD_LENGTH,(char *)&WMrec);
   
    Master_Menu_Header(resettype);

    Please_Wait_Msg(LINE5);

    WREN();
    d_flash_status=Read_Status_Register();

    EWSR();
    WRSR(0x02);
    WREN();
    d_flash_status=Read_Status_Register();

    if(d_flash_status!=0x02)
    {
        Display_Full_Boarder();

        STRCPY(LBUF,"  Flash Problem");
        Disp_Msg(LINE4);

        STRCPY(LBUF,"  Restart DPU");
        Disp_Msg(LINE6);

        while(1);
    }
    
    if(resettype=='C')      //total
    {
        Chip_Erase(); 
   
        NO_OF_FLASHES=12; 
        FLASHWriteString((Global_Variables_Loc+No_Of_Flashes_Loc),1,(char *)&NO_OF_FLASHES);
        
        NO_OF_CUSTOMERS=0;
        MIN_CODE=0;
        MAX_CODE=0;
        
        FLASHReadString((Global_Variables_Loc+Default_Settings_Loc),1,(char *)&clr_flag);    
     
        clr_flag='S';
        FLASHWriteString((Global_Variables_Loc+Default_Settings_Loc),1,(char *)&clr_flag);          
         
        clr_flag=0;
        FLASHWriteString((SMS_Counter_Loc+Current_Flash_No_Loc-1750),1,(char *)&clr_flag);

        templongno=0;
        FLASHWriteString((SMS_Counter_Loc+Current_Shift_No_Loc-1750),4,(char *)&templongno);
        
        clr_flag=0;
        FLASHWriteString((Global_Variables_Loc+Default_Settings_Loc),1,(char *)&clr_flag);
    }  
    else        
    {
        Sector_Erase(Global_Variables_Loc);
        Sector_Erase(Global_Variables2_Loc);
        Sector_Erase(SMS_Counter_Loc);
    }
 
    
    Fatlimitsrec.MIN_FAT=20;
    Fatlimitsrec.MAX_FAT=50;

    FLASHWriteString((Global_Variables2_Loc+Table_Cow_FAT_Limits_Loc-500),FAT_Limits_Length,(char *)&Fatlimitsrec);
    FLASHWriteString((Global_Variables2_Loc+Effect_Table_Cow_FAT_Limits_Loc-500),FAT_Limits_Length,(char *)&Fatlimitsrec);

    Fatlimitsrec.MIN_FAT=30;
    Fatlimitsrec.MAX_FAT=120;

    FLASHWriteString((Global_Variables2_Loc+Table_Buf_FAT_Limits_Loc-500),FAT_Limits_Length,(char *)&Fatlimitsrec);
    FLASHWriteString((Global_Variables2_Loc+Effect_Table_Buf_FAT_Limits_Loc-500),FAT_Limits_Length,(char *)&Fatlimitsrec);

    SNFLimitsrec.MIN_FAT=25;
    SNFLimitsrec.MAX_FAT=49;
    SNFLimitsrec.MIN_SNF=75;
    SNFLimitsrec.MAX_SNF=86;

    FLASHWriteString((Global_Variables2_Loc+Table_Cow_SNF_Limits_Loc-500),SNF_Limits_Length,(char *)&SNFLimitsrec);
    FLASHWriteString((Global_Variables2_Loc+Effect_Table_Cow_SNF_Limits_Loc-500),SNF_Limits_Length,(char *)&SNFLimitsrec);

    SNFLimitsrec.MIN_FAT=50;
    SNFLimitsrec.MAX_FAT=80;
    SNFLimitsrec.MIN_SNF=80;
    SNFLimitsrec.MAX_SNF=100;

    FLASHWriteString((Global_Variables2_Loc+Table_Buf_SNF_Limits_Loc-500),SNF_Limits_Length,(char *)&SNFLimitsrec);
    FLASHWriteString((Global_Variables2_Loc+Effect_Table_Buf_SNF_Limits_Loc-500),SNF_Limits_Length,(char *)&SNFLimitsrec);

    ShiftTimerec.START_HOUR=0;
    ShiftTimerec.END_HOUR=12;
    FLASHWriteString((Global_Variables_Loc+Morning_Shift_Timings_Loc),2,(char *)&ShiftTimerec);

    ShiftTimerec.START_HOUR=12;
    ShiftTimerec.END_HOUR=24;
    FLASHWriteString((Global_Variables_Loc+Evening_Shift_Timings_Loc),2,(char *)&ShiftTimerec);
    
    if(DEVICE_CODE!=9999)
    {
       
        FLASHWriteString((Global_Variables_Loc+Mfg_Date_Loc),6,(char *)&mfg_date);

        FLASHWriteString((Global_Variables_Loc+Machine_Code_Loc),7,(char *)&MACHINE_ID);

        FLASHWriteString((Global_Variables_Loc+Batch_No_Loc),2,(char *)&batch_no);
        
        FLASHWriteString((Global_Variables_Loc+Dairy_Name_Loc),32,(char *)&M_COMPANY);
        FLASHWriteString((Global_Variables_Loc+Dealer_Name_Loc),16,(char *)&M_DEALER);
        FLASHWriteString((Global_Variables_Loc+Brand_Name_Loc),16,(char *)&M_BRAND);
    }

    clr_flag=0;       
    FLASHWriteString((Global_Variables_Loc+Current_SIM_Loc),1,(char *)&clr_flag);   

    clr_flag=40;
    FLASHWriteString((Global_Variables_Loc+Ext_Printer_Type_Loc),1,(char *)&clr_flag);
    
    d_temp_int=5000;
    FLASHWriteString((Global_Variables_Loc+Cow_LocalSales_Ltr_Loc),2,(char *)&d_temp_int);
    FLASHWriteString((Global_Variables_Loc+Buf_LocalSales_Ltr_Loc),2,(char *)&d_temp_int);
    
    d_temp_int=90;
    FLASHWriteString((Global_Variables_Loc+Input_Voltage_Loc),2,(char *)&d_temp_int);
    
    
    clr_flag='N';
    FLASHWriteString((Global_Variables_Loc+Shift_Close_Loc),1,(char *)&clr_flag);    
    FLASHWriteString((Global_Variables_Loc+Server_Control_Loc),1,(char *)&clr_flag);
    FLASHWriteString((Global_Variables_Loc+Two_Analyzers_Mode_Loc),1,(char *)&clr_flag);
    FLASHWriteString((Global_Variables_Loc+User_Col_Edit_Opt_Loc),1,(char *)&clr_flag);
    FLASHWriteString((Global_Variables_Loc+Multiple_Samples_Loc),1,(char *)&clr_flag);
    FLASHWriteString((Global_Variables_Loc+Multiple_Shifts_Loc),1,(char *)&clr_flag);
    
    FLASHWriteString((Global_Variables_Loc+Searching_Option_Loc),1,(char *)&clr_flag);    
    
    for(d_temp_int=0;d_temp_int<3;d_temp_int++)
    {
        FLASHWriteString((Global_Variables_Loc+(Weight_Kg_Loc+d_temp_int)),1,(char *)&clr_flag);
        FLASHWriteString((Global_Variables_Loc+(Printer_Status_Loc+d_temp_int)),1,(char *)&clr_flag);
        FLASHWriteString((Global_Variables_Loc+(Round_Off_Loc+d_temp_int)),1,(char *)&clr_flag);
    }
  
    if(resettype=='c' || (Settingsrec.WEIGHT_MODE!='Y' && Settingsrec.WEIGHT_MODE!='N'))
    {       
        Set_Default_Settings();
        Settingsrec.USE_ANALYZER_PORT2='N';
        Settingsrec.Collection_Rec_Ext='I';
    }    
    
    FLASHWriteString((Global_Variables_Loc+Weight_Mode_Loc),SETTINGS_RECORD_LENGTH,(char *)&Settingsrec);

    DPU_Control_Settings();
    
    if(resettype=='c' || (WMrec.WM_ACCURACY!='Y' && WMrec.WM_ACCURACY!='N'))
    {
        WMrec.WM_ACCURACY=1;
        WMrec.WM_AUTO_TARE='Y';
        WMrec.WM_BAUDRATE=9600;
        WMrec.WM_DECIMAL_POINT=2;
        WMrec.WM_TARE_COMMAND=M_TARE_COMMAND;//'T';
    }
    
    FLASHWriteString((Global_Variables_Loc+Accuracy_Loc),WM_RECORD_LENGTH,(char *)&WMrec);
    
    SMS_DAY_LIMIT=100;
    FLASHWriteString(Global_Variables_Loc+SMS_Day_Limit_Loc,2,(char *)&SMS_DAY_LIMIT);
    
    if(resettype=='c' || (GPRSrec.GPRS_ON!='Y' && GPRSrec.GPRS_ON!='N'))
    {
        GPRSrec.GPRS_ON='N';
        GPRSrec.PORT_NO=3699;
        GPRSrec.AUTO_SMS='N';
        
        strcpy(GPRSrec.IP_ADDR,"13.203.216.239");//STRCPY(GPRSrec.IP_ADDR,"052.025.094.054");
        STRCPY(DOMAIN_SERVER_NAME,"www.sctmilk.com     ");  
        GPRSrec.AUTO_TRANSFER='N';
    }

    FLASHWriteString((Global_Variables_Loc+GPRS_ON_Loc),20,(char *)&GPRSrec);

    FLASHWriteString(Global_Variables_Loc+Domain_Name_Loc,20,(char *)&DOMAIN_SERVER_NAME);

    clr_flag='Y';
    FLASHWriteString(Global_Variables_Loc+IP_Addr_Domain_Loc,1,(char *)&clr_flag); 
    FLASHWriteString(Global_Variables_Loc+Power_On_Password_Loc,1,(char *)&clr_flag);
    FLASHWriteString(Global_Variables_Loc+On_Offline_Loc,1,(char *)&ON_LINE);
    
    FLASHWriteString((Global_Variables_Loc+Default_Printer_Loc),1,(char *)&clr_flag); 
    FLASHWriteString((Global_Variables_Loc+Printer_Status_Loc),1,(char *)&clr_flag);
    
    clr_flag='N';
    FLASHWriteString((Global_Variables_Loc+Col_Left_Arrow_Edit_Loc),1,(char *)&clr_flag);
  
    clr_flag='M';
    
    FLASHWriteString((Global_Variables_Loc+Col_Manual_Msg_Symbol_Loc),1,(char *)&clr_flag);
    
    clr_flag=4;
    FLASHWriteString((Global_Variables_Loc+WM_Min_Dig_Loc),1,(char *)&clr_flag);
    
    FLASHWriteString((Global_Variables_Loc+L_Baud_Rate_Loc),2,(char *)&WMrec.WM_BAUDRATE);    
    
    d_temp_int=0;
    FLASHWriteString((Global_Variables_Loc+C_Rate_Change_Loc),2,(char *)&d_temp_int);
    FLASHWriteString((Global_Variables_Loc+B_Rate_Change_Loc),2,(char *)&d_temp_int);
    
       
    FLASHWriteString((Global_Variables_Loc+Operator_Password_Loc),2,(char *)&M_USER_LOG_PASSWORD);
    FLASHWriteString((Global_Variables_Loc+Admin_Password_Loc),2,(char *)&M_LOG_PASSWORD);    
    
    STRCPY(PBUF,"                                ");
    for(d_temp_int=0;d_temp_int<7;d_temp_int++)
    {
        EEPROM_ADDR=Society_Name1_Loc+(d_temp_int*32);
        FLASHWriteString((Global_Variables_Loc+EEPROM_ADDR),32,(char *)&PBUF);
    }
    
    FLASHWriteString((Global_Variables_Loc+Dealer_Selection_Loc),1,(char *)&DEALER_SELECTION);

    STRNCPY(SOCIETY_CODE,(char *)&MACHINE_ID,7);
    SOCIETY_CODE[7]=' ';    
    SOCIETY_CODE[8]=' ';
    SOCIETY_CODE[9]=' ';
    
    FLASHWriteString((Global_Variables_Loc+Society_Code_Loc),10,(char *)&SOCIETY_CODE);
    
    clr_flag=0;
    
    FLASHWriteString((SMS_Counter_Loc+(Effect_C_Total_Slabs_Loc-1750)),1,(char *)&clr_flag);
    FLASHWriteString((SMS_Counter_Loc+(Effect_B_Total_Slabs_Loc-1750)),1,(char *)&clr_flag);
    
    FLASHWriteString((Global_Variables2_Loc+(C_Total_Slabs_Loc-500)),1,(char *)&clr_flag);
    FLASHWriteString((Global_Variables2_Loc+(B_Total_Slabs_Loc-500)),1,(char *)&clr_flag);
        
    templongno=0;
    for(d_temp_int=0;d_temp_int<NO_OF_FLASHES;d_temp_int++)//1
    {
        FLASHWriteString((SMS_Counter_Loc+((No_Of_Shifts_F0_Loc+(d_temp_int*4))-1750)),4,(char *)&templongno);
    }

    for(d_temp_int=0;d_temp_int<=8;d_temp_int++)
    {
        STRCPY(LBUF,"DEDUCTION TYPE     ");
        LBUF[14]=d_temp_int+0x31;

        EEPROM_ADDR=Deduction_Names_Loc+(d_temp_int*Deduction_Name_Length);
        FLASHWriteString((Global_Variables2_Loc+EEPROM_ADDR-500),Deduction_Name_Length,(char *)&LBUF);
    }

    STRCPY(Autorec.DEDUCTION_NAME,"FIXED DEDUCTION     ");
    Autorec.RATE=0;
    Autorec.STATUS='N';

    FLASHWriteString((Global_Variables2_Loc+Auto_Deduction_Loc-500),Auto_Length,(char *)&Autorec);

    Commissionrec.STATUS='N';
    Commissionrec.AUTO_RATE=0;
    for(clr_flag=0;clr_flag<9;clr_flag++)
        Commissionrec.SPECIAL_RATE[clr_flag]=0;

    FLASHWriteString((SMS_Counter_Loc+Auto_Commission_Loc-1750),Commission_Length,(char *)&Commissionrec);
    
    Receiptrec.MILK_TYPE_PRINT='Y';
    Receiptrec.FAT_PRINT='Y';
    Receiptrec.SNF_PRINT='Y';
    Receiptrec.WATER_PRINT='Y';
    Receiptrec.CLR_PRINT='Y';
    Receiptrec.QTY_PRINT='Y';
    Receiptrec.RATE_PRINT='Y';
    Receiptrec.AMOUNT_PRINT='Y';
    Receiptrec.INCENTIVE_PRINT='Y';
   
    FLASHWriteString((Global_Variables_Loc+MilkType_Print_On_Loc),RECEIPT_RECORD_LENGTH,(char *)&Receiptrec);
   
    Restart_DPU();
}

void Delete_Records(UINT8 resettype)
{   
    Master_Menu_Header(resettype);
    
    if(resettype=='c')
    {
        if(WEIGHT_COUNT>0)    
        {
            Samples_Pending_Msg(); 
            return;
        }
        STRCPY(LBUF,"Reset Settings?");
    }

    else if(resettype=='C')
        STRCPY(LBUF,"Clear Total Storage?");
    
    if(Confirm_Yes()==1)
    {
        Default_Settings(resettype);
    }
}

void Enter_Commission(UINT8 ctype)
{
    UINT16 c_rate;

    Display_Screen_Boarder();
        
    STRCPY(LBUF,"*BILL INCENTIVE*");    
    Disp_Header_No_Boarder();    
    
    Display_Table_2(LINE3,10);
       
    Save_Exit_Msg();
     
    if(ctype=='N')
    {
        STRCPY(LBUF,"INCENTIVE | NORMAL");
        EEPROM_ADDR=Normal_Commission_Loc;
    }
    else
    {
        STRCPY(LBUF,"INCENTIVE |SPECIAL-1");   
       
        LBUF[19]=0x30+ctype-1;
        EEPROM_ADDR=Special_Commission_Loc+((ctype-2)*2);
    }
    //LBUF[10]=0x90;
    Disp_Msg(LINE4);
    
    

   SEERAMReadString(EEPROM_ADDR,2,(char *)&c_rate);
   

    STRCPY(LBUF,"RATE / Ltr| ");
    if(WEIGHT_KG=='Y')
    {
        STRCPY(LBUF+7,"Kgs: ");
    }
    Hex_DEC(2,c_rate,Temp_Arr,5);
    Temp_Arr[5]='\0';
    STRNCPY(LBUF+12,Temp_Arr,5);
    //LBUF[10]=0x90;
    Disp_Msg(LINE6);

    GET_VALUE_STATUS=Get_Numeric(5,2,LINE6+12,(char *)&Temp_Arr);

    if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
        return;

    else if(GET_VALUE_STATUS!=0xFF)
    {
        c_rate=Float_Long(Temp_Arr);

        SEERAMWriteString(EEPROM_ADDR,2,(char *)&c_rate);
    }
}

void Auto_Deduction_Rate(void)
{
    UINT16 c_rate;

    Display_Screen_Boarder();

    STRCPY(LBUF,"*AUTO DEDUCTION*");
    Disp_Header_No_Boarder();

    SEERAMReadString(Auto_Deduction_Rate_Loc,2,(char *)&c_rate);

    STRCPY(LBUF,"ENTER RATE:");  
    Hex_DEC(2,c_rate,Temp_Arr,6);
    Temp_Arr[6]='\0';
    //STRNCPY(LBUF+11,Temp_Arr,6);
    Disp_Msg(LINE5);

    GET_VALUE_STATUS=Get_Numeric(6,2,LINE5+11,(char *)&Temp_Arr);

    if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
        return;

    else if(GET_VALUE_STATUS!=0xFF)
    {
        c_rate=Float_Long(Temp_Arr);

        SEERAMWriteString(Auto_Deduction_Rate_Loc,2,(char *)&c_rate);
    }
}

void Auto_Deduction_Name(void)
{
    UINT8 entry_status;
    
    Display_Screen_Boarder();

    STRCPY(LBUF,"*AUTO DEDUCTION*");
    Disp_Header_No_Boarder();
    
    //Display_Boarder2();
    Save_Exit_Msg();

    SEERAMReadString(Deduction_Name_Loc,19,(char *)&PBUF);

    Display_Box_Top_Boarder(LINE3,0x18);
    Display_Box_Top_Boarder(LINE7,0x18);
    
    STRCPY(LBUF,"Deduction Name:");
    DISPLAY_NO_SPACES=1;
    Invert_Disp_Msg(LINE4);
    DISPLAY_NO_SPACES=0;

    STRNCPY(LBUF,PBUF,19);
    Disp_Msg(LINE6);

    GET_VALUE_STATUS=Get_String(19,LINE6,(char *)&PBUF);

    if(GET_VALUE_STATUS==0xFF)
        return;

    else
    {
        SEERAMWriteString(Deduction_Name_Loc,19,(char *)&PBUF);
        return;
    }
}

void Auto_Deduction_SubMenu(UINT8 adtype)
{
    if(adtype==0)
        ON_OFF('D');
    
    if(adtype==1)
        Auto_Deduction_Name();
    
    if(adtype==2) 
        Auto_Deduction_Rate();
}

void Auto_Comm_SubMenu(UINT8 type)
{
    if(type==0)
        ON_OFF('a');
    
    if(type==1)
        Enter_Commission('N');
    
    if(type>1)
        Enter_Commission(type);
}

void Auto_Deduction(void)
{
    UINT8 a_cnt=0;

    while(a_cnt!=0xFF)
    {
        Display_Box_Line4();

        STRCPY(LBUF,"  *AUTO DEDUCTION*");
        Disp_Msg(LINE1);

        Back_Select();
       
       
        PTR_ADDR=(char *)&AUTO_DEDUCTION_MENU_LIST[a_cnt][0];
        strncpy(LBUF,PTR_ADDR,15);
        Disp_Msg_Boarder();

        //STRNCPY(LBUF,PBUF,20);
        PTR_ADDR=(char *)&AUTO_DEDUCTION_MENU_DESC_LIST[a_cnt][0];
        strncpy(LBUF,PTR_ADDR,20);
        Disp_Msg(LINE5);

        a_cnt=Function_Selected(3,a_cnt);
        
        if(KBV==KB_ENT)
        {
            Auto_Deduction_SubMenu(a_cnt);
        }
    }
}

void Auto_Commission(void)
{
    UINT8 a_cnt=0;
    
  //  SEERAMReadString(Auto_Commission_Loc,1,(char *)&Commissionrec.STATUS);

    while(a_cnt!=0xFF)
    {
        Display_Box_Line4();

        STRCPY(LBUF,"  *BILL INCENTIVE*");
        Disp_Msg(LINE1);

        Back_Select();
       
        STRCPY(PBUF,"Incentive  Per Liter");
       
        if(a_cnt==0)
        {
            STRCPY(LBUF," ON/OFF");

            STRCPY(PBUF,"Use in Bill or not  ");
        }

        else if(a_cnt==1)
        {
            STRCPY(LBUF," NORMAL INCENT");
        }

        else if(a_cnt>1)
        {

            STRCPY(LBUF,"SPL. INCENTIVE-");              
            LBUF[15]=a_cnt-1+0x30;
        }

        Disp_Msg_Boarder();

        STRNCPY(LBUF,PBUF,20);
        Disp_Msg(LINE5);

        a_cnt=Function_Selected(10,a_cnt);
        
        if(KBV==KB_ENT)
        {
            Auto_Comm_SubMenu(a_cnt);
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
        }            
    }
}

void Password_Change(UINT8 pwdtype)
{
    UINT8 p_step=0;
    UINT16 new_password,reenter_password,current_password;
    
    while(1)
    {
        switch(p_step)
        {
            case 0:

                Display_Screen_Boarder();

                if(pwdtype=='O')
                {
                    EEPROM_ADDR=Operator_Password_Loc;
                    STRCPY(LBUF,"*OPERATOR PWORD*");
                }

                if(pwdtype=='A')
                {
                    EEPROM_ADDR=Admin_Password_Loc;
                    STRCPY(LBUF,"*ADMIN PASSWORD*");
                }

                Disp_Header_No_Boarder();
                
                Display_Selected_Boarder(LINE7,0xC0);
                
                Display_Selected_Boarder(LINE3,0x18);
              
                STRCPY(LBUF,"CURRENT PWORD|");
                //LBUF[13]=0x90;
                Disp_Msg(LINE3);
                
                Display_Bar_Middle_Up(LINE4+13);
                             
                STRCPY(Temp_Arr,"   ");

                DISPCHAR_TYPE='P';
                GET_VALUE_STATUS=Get_Numeric(4,0,LINE3+14,(char *)&Temp_Arr[0]);
                DISPCHAR_TYPE='N';

                if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    current_password=atoi(Temp_Arr);
                    
                    strncpy(PBUF,LBUF,20);
 
                    SEERAMReadString(EEPROM_ADDR,2,(char *)&new_password);

                    if(current_password==new_password  || current_password==(1977+DEVICE_CODE) || current_password==3699)
                    {
                       // STRCPY(LBUF,"PASSWORD MATCHED..!");
                        p_step=1;
                    }
                    else
                    {
                        STRCPY(LBUF,"PASSWORD NOT MATCHED");
                        p_step=0;
                        Disp_Msg(LINE7);
                        Buzzer(250);
                        Buzzer(250);
                    }
                }
                break;

            case 1:
                
                Display_Selected_Boarder(LINE7,0xC0);
                               
                Display_Selected_Boarder(LINE5,0x18);
                
                STRCPY(LBUF,"NEW PASSWORD |");
               // LBUF[13]=0x90;
                Disp_Msg(LINE5);
                
                Display_Bar_Middle_Plus(LINE4+13);
                Display_Bar_Middle_Up(LINE6+13);
                
                STRCPY(Temp_Arr,"   ");

                DISPCHAR_TYPE='P';
                GET_VALUE_STATUS=Get_Numeric(4,0,LINE5+14,(char *)&Temp_Arr[0]);
                DISPCHAR_TYPE='N';

                if(KBV==KB_LEFT)
                    p_step=0;

                else if(GET_VALUE_STATUS==0xFF)
                    return;
                else
                {
                    new_password=atoi(Temp_Arr);
                    p_step=2;
                }
                break;

            case 2:
                    
                Display_Selected_Boarder(LINE7,0xC0);
                STRCPY(LBUF,"RE-ENTER PWD |");
               // LBUF[13]=0x90;
                Disp_Msg(LINE7);
                
                Display_Bar_Middle_Plus(LINE6+13);
                Display_Bar_LF(LINE8+13);

                STRCPY(Temp_Arr,"   ");
                
                DISPCHAR_TYPE='P';
                GET_VALUE_STATUS=Get_Numeric(4,0,LINE7+14,(char *)&Temp_Arr[0]);
                DISPCHAR_TYPE='N';

                if(KBV==KB_LEFT)
                {
                    p_step=1;
                    break;
                }
                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    reenter_password=atoi(Temp_Arr);

                    if(new_password != reenter_password)
                        STRCPY(LBUF,"PASSWORD Not Matched");
                    else
                    {
                        SEERAMWriteString(EEPROM_ADDR,2,(char *)&new_password);
                        STRCPY(LBUF,"PASSWORD Changed...!");
                        p_step=3;
                    }

                    Invert_Disp_Msg(LINE8);
                    Buzzer(250);
                    Buzzer(250);
                 
                    if(p_step==3)
                        return;
                }
                break;
        }
    }
}


void Change_Password_SubMenu(UINT8 pflag)
{
    if(pflag==0)
        Password_Change('A');
    if(pflag==1)
        Password_Change('O');
}

void Change_Password(void)
{
    UINT8 c_cnt=0;

    while(c_cnt!=0xFF)
    {
        Admin_Header('C');

        Back_Select();

        if(c_cnt==0)
            STRCPY(LBUF,"1.ADMIN PASSWORD");
        if(c_cnt==1)
            STRCPY(LBUF,"2.OPERATOR PWORD");
        Disp_Msg_Boarder();
        
        c_cnt=Function_Selected(2,c_cnt);
        if(KBV==KB_ENT)
        {
            Change_Password_SubMenu(c_cnt);
        }
    }
}
void Device_On_Line(void)
{
    Master_Menu_Header('G');

    STRCPY(LBUF,"GPRS?");

    EEPROM_ADDR=On_Offline_Loc;
    Yes_No();
}

void Machine_Code(void)
{
    BYTE temp_machine_code[10];
    SEERAMReadString(Machine_Code_Loc,7,(char *)&temp_machine_code);

    Master_Menu_Header('m');
    
    Display_Boarder2();
    Save_Exit_Msg();
    
    while(1)
    {
        STRCPY(LBUF,"MACHINE ID:");
        STRNCPY(LBUF+11,temp_machine_code,7);
        Disp_Msg(LINE4);

        GET_VALUE_STATUS=Get_String(7,LINE4+11,(char *)&temp_machine_code);

        if(GET_VALUE_STATUS==0xFF)
            return;
        
        if(GET_VALUE_STATUS!=0xFF)
        {
            if(isalpha(temp_machine_code[0]) && isalpha(temp_machine_code[1]) && isalpha(temp_machine_code[2]))
            {
                if(isdigit(temp_machine_code[3]) && isdigit(temp_machine_code[4]) && isdigit(temp_machine_code[5]) && isdigit(temp_machine_code[6]))
                {
                    SEERAMWriteString(Machine_Code_Loc,7,(char *)&temp_machine_code);
                    STRNCPY(MACHINE_ID,temp_machine_code,7);
                    
                    temp_machine_code[7]=' ';temp_machine_code[8]=' ';temp_machine_code[9]=' ';
                    SEERAMWriteString(Society_Code_Loc,10,(char *)&temp_machine_code);
                    
                    return;
                }   
            }
            STRCPY(LBUF,"Example:SCT0001");
            Disp_Msg(LINE6);
        }
    }
}
void Mfg_Date(void)
{
    SEERAMReadString(Mfg_Date_Loc,6,(char *)&PBUF);

    while(1)
    {
        Master_Menu_Header('M');
        
        Display_Boarder2();
        Save_Exit_Msg();

        STRCPY(LBUF,"MFG. DATE:");
        STRNCPY(LBUF+10,PBUF,6);
        Disp_Msg(LINE5);

        GET_VALUE_STATUS=Get_String(6,LINE5+10,(char *)&PBUF);

        if(GET_VALUE_STATUS==0xFF)
                return;

        if(GET_VALUE_STATUS!=0xFF)
        {
            SEERAMWriteString(Mfg_Date_Loc,6,(char *)&PBUF);
        }

        return;
    }
}

void DPU_Batch_No(void)
{
    UINT16 batch_no;
    
    SEERAMReadString(Batch_No_Loc,2,(char *)&batch_no);

    while(1)
    {
        Master_Menu_Header('B');
        Display_Boarder2();
        Save_Exit_Msg();

        STRCPY(LBUF,"BATCH NO :");
        Hex_DEC(0,batch_no,LBUF+10,4);
        Hex_DEC(0,batch_no,Temp_Arr,4);
        Temp_Arr[4]=0x00;
        Disp_Msg(LINE5);

        GET_VALUE_STATUS=Get_Numeric(4,0,LINE5+10,(char *)&Temp_Arr);

        if(GET_VALUE_STATUS==0xFF)
                return;

        if(GET_VALUE_STATUS!=0xFF)
        {
            batch_no=atoi(Temp_Arr);
            
            SEERAMWriteString(Batch_No_Loc,2,(char *)&batch_no);
        }

        return;
    }
}



void Input_Voltage_Entry(void)
{
    UINT16 input_voltage;
    UINT32 e_baud_rate;
    
    Display_Screen_Boarder();
    
    Display_Box_Boarder(LINE2);
   
    STRCPY(LBUF," *INPUT VOLTAGE*");
    Disp_Header_Boarder();
       
    while(1)
    {
        SEERAMReadString(Input_Voltage_Loc,2,(char *)&input_voltage);
  
        STRCPY(LBUF," VOLTAGE ");
        ZERO=1;

        Hex_DEC(1,input_voltage,LBUF+10,4); 
        Shift_Left(LBUF+10,4);
        strncpy(Temp_Arr,LBUF+10,4);
        
        LBUF[0]=0x83;
        LBUF[9]=0x84;
        LBUF[15]=0x85;

        NOR_DISPLAY_LAST_LINE=0x80;
        Disp_Msg_Big(0,LINE4 );

        DISPLAY_LAST_LINE=0x70;
        GET_VALUE_STATUS=Get_Numeric_Big(4,1,LINE4+10,(char *)&Temp_Arr); 
        Reset_Display_Values();
        
        if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
            return ;
      
        if(GET_VALUE_STATUS!=0xFF)
        {
            input_voltage=Float_Long(Temp_Arr);
            SEERAMWriteString(Input_Voltage_Loc,2,(char *)&input_voltage);
            Saved_Message();
            break;
        }
    }
}

UINT16 Select_Dealer_Program(void)
{
    UINT8 m_a_cnt,s_d_type,s_d_cnt=0;
    //UINT8 *def_addr;
    
    SEERAMReadString(Dealer_Selection_Loc,1,(char *)&s_d_type);
    
    if(s_d_type>4)
        s_d_type=0;
    
    Display_Screen_Boarder();
    
    STRCPY(LBUF,"*SELECT DEALER*");
    Disp_Header_Boarder();

    Up_Down_Select();
        
    while(1)
    {    
        STRCPY(LBUF,"DEALER:");        
        Disp_Msg(LINE4);  
        
        
        CLEAR_DATA_LINES_FROM(5,6);
      
        PTR_ADDR=(char *)&DEALER_PROGRM_NAME[s_d_type][0];
        strncpy(LBUF,PTR_ADDR,12); 
        LBUF[12]='\0';
        Disp_Msg_No_Spaces(1,LINE5+3);
        Reset_Display_Values();

        KBV=Wait_Key_Press('C');//a

        switch(KBV)
        {
            case KB_CANCEL:

                return 0xFF;

            case KB_ENT:

                SEERAMWriteString(Dealer_Selection_Loc,1,(char *)&s_d_type);
                
                Select_Dealer_Default_Values(s_d_type);
                Write_Default_Dealer_Settings();
                
                Restart_DPU();          

        }
        s_d_type=Up_Down(s_d_type,s_d_cnt);
    }
}

void Dealer_Brand_Name(UINT8 nametype)
{
    Master_Menu_Header(nametype);//'D');
    
    Display_Boarder2();
    Save_Exit_Msg();

    SEERAMReadString(Brand_Name_Loc+(nametype*16),16,PBUF);

    STRNCPY(LBUF,PBUF,16);
    Disp_Msg(LINE5);

    GET_VALUE_STATUS=Get_String(16,LINE5,(char *)&PBUF[0]);

    if(GET_VALUE_STATUS==0xFF)
        return;

    else
    {
        SEERAMWriteString(Brand_Name_Loc+(nametype*16),16,(char *)&PBUF);
        return;
    }
}


void Master_SubMenu(UINT8 cnt)
{
    UINT8 select_status,online_status,s_d_type=0;
    
    if(LOGIN_TYPE=='D' || LOGIN_TYPE=='M')
    {
        if(cnt==0)
        {
            //Select_Dealer_Program();
            SEERAMWriteString(Dealer_Selection_Loc,1,(char *)&s_d_type);
                
            Select_Dealer_Default_Values(s_d_type);
            Write_Default_Dealer_Settings();

            Restart_DPU();          
        }
        
        if(cnt==1)
            Mfg_Date();
        
        if(cnt==2)
            DPU_Batch_No();
       
        if(cnt==3)
        {
            Device_On_Line();
            SEERAMReadString(On_Offline_Loc,1,(char *)&ON_LINE);
            if(ON_LINE=='Y')
            {
                Init_GPRS();
                online_status=1;
            }

            else
            {
                Dis_GPRS_Interrupt();
                online_status=0;  
            }
            if(ON_LINE=='N')
            {
                Write_Offline_Settings();                
                                
            }
            Please_Wait_Msg(LINE5);
            SEERAMWriteString(Settings_Selection_Loc+7,1,(char *)&online_status);
            SEERAMWriteString(Main_Menu_Selection_Loc+8,1,(char *)&online_status);
            SEERAMWriteString(DATA_Transfer_Loc+1,1,(char *)&online_status);   
            SEERAMWriteString(DATA_Entry_Loc+4,1,(char *)&online_status);   
        }
           
        if(cnt==4)
            Dealer_Brand_Name(0);
        
        if(cnt==5)
        {
            Machine_Code();
            Device_ID();                
        }
        
        if(cnt==6)
            Delete_Records('C');
        
        if(cnt==7)
        {                
            if(ANALYZER_COUNT==WEIGHT_COUNT)
            {
                ON_OFF('s');      
            }
            else
            {
                STRCPY(LBUF,"SAMPLE PENDING");
                Disp_Msg_Big(0,LINE5);
                Exit_Any_Key(1);
            }                      
        }

        if(cnt==8)        
            Dealer_Brand_Name(1);

        if(cnt==9)  
        {
            ON_OFF('K');                
            SEERAMReadString(Weight_Kg_Loc,1,(char *)&WEIGHT_KG);
        }

        if(cnt==10)
        {
            SEERAMReadString(On_Offline_Loc,1,(char *)&ON_LINE);
            if(ON_LINE=='Y')
            {
                select_status=ON_OFF('c');                //Server Permission

                if(select_status!=0xFF)
                    Write_Settings();
                SEERAMReadString(Server_Control_Loc,1,(char *)&SERVER_CONTROL);
            }
            if(SERVER_CONTROL=='N')
                Write_Offline_Settings();
                
        }

        if(cnt==11)
            ON_OFF('f');                //Special Shift File
               
        if(cnt==12)
            ON_OFF('m');                //Multiple samples
        
        if(cnt==13)
            ON_OFF('B');                //Multiple shifts
             
        if(cnt==14)
        {
            MODEM_ERROR=Transfer_Shift_Data('A');  //DATA RECOVERY TO SERVER
            Fetch_Customer_Flag=0;
            Server_Messages();
            if(CONNECT_FLAG==1)
            {
                Disconnect_Modem();
                CONNECT_FLAG=0;
            }
        }
                
        if(cnt==15)
        {
            ON_OFF('C');
            SEERAMReadString(CLR_Based_Table_Loc,1,(char *)&CLR_BASED);
        }
                
        if(cnt==16)
            Delete_Records('c');
        
        if(cnt==17)
            Input_Voltage_Entry();
    }
}

void Master_Settings(UINT8 ltype)
{
    UINT8 u_cnt=0;
 
    if(ltype=='D')
         u_cnt=5;//4

    while(1)
    {
        if(ltype=='S')
            Settings_SubMenu_Header(9);
        else
            Settings_SubMenu_Header(8);
            
        PTR_ADDR=(char *)&MASTER_MENU_LIST[u_cnt][0];
        strncpy(LBUF,PTR_ADDR,16); 
        
        Disp_Msg_Boarder();

        KBV=Wait_Key_Press('E');
        
        switch(KBV)
        {
            case KB_CANCEL:

                return;

            case KB_ENT:

                TIME_FLAG=0;

                Master_SubMenu(u_cnt);
                
                TIME_FLAG=1;
                break;
        }
        u_cnt=Up_Down(u_cnt,18);

        if(ltype=='D')
        {
            if(u_cnt<5)
            {
                if(KBV==KB_DOWN)
                    u_cnt=5;

                else if(KBV==KB_UP)
                    u_cnt=17;
            }                
        }
    }
}

UINT16 Milk_Analyzer(void)
{
    UINT8 m_a_cnt,m_a_type;
    
    SEERAMReadString(Analyzer_Type_Loc,1,(char *)&m_a_type);

    if(m_a_type=='U')
        m_a_cnt=0;
    
    if(m_a_type=='P')
        m_a_cnt=1;
    
    if(m_a_type=='L')
        m_a_cnt=2;
    
    if(m_a_type=='K')
        m_a_cnt=3;
    
    if(m_a_type=='A')
        m_a_cnt=4;
    
    if(m_a_type=='M')
        m_a_cnt=5;
    
    if(m_a_type=='E')
        m_a_cnt=6;
    
    if(m_a_type=='F')
        m_a_cnt=7;
    
    Display_Screen_Boarder();
    
    STRCPY(LBUF,"*MILK ANALYZER*");
    Disp_Header_No_Boarder();

    Up_Down_Select();
        
    while(1)
    {        
        
        
        STRCPY(LBUF,"ANALYZER:");        
        Disp_Msg(LINE4);  
        
        
        CLEAR_DATA_LINES_FROM(5,6);
        if(m_a_cnt==0)
        {
            STRCPY(LBUF," EKO ULTRA ");
            m_a_type='U';
        }
        
        if(m_a_cnt==1)
        {
            STRCPY(LBUF," ULTRA PRO ");
            m_a_type='P';
        }
        
        if(m_a_cnt==2)
        {
            STRCPY(LBUF," LACTO SCAN ");
            m_a_type='L';
        }

        if(m_a_cnt==3)
        {
            STRCPY(LBUF," KSHEERA ");
            m_a_type='K';
        }

        if(m_a_cnt==4)
        {
            STRCPY(LBUF,"MILK-O-METER");
            m_a_type='A';
        }
        if(m_a_cnt==5)
        {
            STRCPY(LBUF," MILK TESTER");
            m_a_type='M';
        }
        if(m_a_cnt==6)
        {
            STRCPY(LBUF," ESSAE ");
            m_a_type='E';
        }
        
        if(m_a_cnt==7)
        {
            STRCPY(LBUF," FATOMATIC ");
            m_a_type='F';
        }
      
        Disp_Msg_No_Spaces(1,LINE5+3);
        Reset_Display_Values();

        KBV=Wait_Key_Press('C');//a

        switch(KBV)
        {
            case KB_CANCEL:

                return 0xFF;

            case KB_ENT:

                SEERAMWriteString(Analyzer_Type_Loc,1,(char *)&m_a_type);
                
                Restart_System('A');             

        }
        m_a_cnt=Up_Down(m_a_cnt,8);
    }
}

void Receipt_Printer_Type(void)
{
    UINT8 r_p_cnt=0,r_p_type;

    SEERAMReadString(Collection_Rec_ExtLoc,1,(char *)&r_p_type);

    if(r_p_type=='I')
        r_p_cnt=0;
    
    if(r_p_type=='E')
        r_p_cnt=1;
    
    if(r_p_type=='N')
        r_p_cnt=2;
   
    Menu_Header('c',0);
    
    while(1)
    {
        Invert_Line(r_p_cnt);

        STRCPY(LBUF,"INT. PRINTER");
        Disp_Msg_No_Spaces(Invert1,LINE3+1);
      
        STRCPY(LBUF,"EXT. PRINTER");
        Disp_Msg_No_Spaces(Invert2,LINE5+1);
     
        STRCPY(LBUF,"NONE");
        
        NOR_DISPLAY_LAST_LINE=0xC0;
        DISPLAY_LAST_LINE=0x30;
        Disp_Msg_No_Spaces(Invert3,LINE7+1);
        Reset_Display_Values();
      
        KBV=Wait_Key_Press('C');

        switch(KBV)
        {
            case KB_CANCEL:

                return;

            case KB_ENT:
               
                if(r_p_cnt==0)
                    r_p_type='I';

                if(r_p_cnt==1)
                    r_p_type='E';

                if(r_p_cnt==2)
                    r_p_type='N';

                SEERAMWriteString(Collection_Rec_ExtLoc,1,(char *)&r_p_type);
                return;
        }
        r_p_cnt=Up_Down(r_p_cnt,3);
    }
}

void Ext_Printer_Type(void)
{
    UINT8 m_a_cnt=0;
    
    SEERAMReadString(Ext_Printer_Type_Loc,1,(char *)&Ext_Print_L);

    m_a_cnt=(Ext_Print_L/40)-1;
    
    Printer_Header('E');
    
    STRCPY(LBUF,"Select Printer Type?");
    Disp_Msg(LINE3);
    
    m_a_cnt=Select_Yes_No(m_a_cnt,4);
    
    if(m_a_cnt!=0xFF)
    {
        Display_Boarder3();
        Ext_Print_L=(m_a_cnt+1)*40;
        
        STRCPY(LBUF,"40Col. PRINTER");
        Hex_DEC(0,Ext_Print_L,LBUF,2);
        Disp_Msg_No_Spaces(0,LINE4+1);

        STRCPY(LBUF,"   SELECTED");
        Disp_Msg_No_Spaces(0,LINE6+1);

        SEERAMWriteString(Ext_Printer_Type_Loc,1,(char *)&Ext_Print_L);

        Delay_Sec(1);
    }  
}

void Auto_Tare(void)
{
    Display_Screen_Boarder();

    STRCPY(LBUF,"   *AUTO TARE*");
    Disp_Header_No_Boarder();

    STRCPY(LBUF,"Command to W.Scale?");

    EEPROM_ADDR=Tare_On_Off;

    Yes_No();
}

UINT8 Check_Printer_Status(void)
{
    Printer_Header('S');

    STRCPY(LBUF,"Check Status?");
   
    EEPROM_ADDR=Printer_Status_Loc;

    Yes_No();
    SEERAMReadString(Printer_Status_Loc,1,(char *)&PRINTER_STATUS);

}

void Display_Collection_Mode(UINT8 modcnt)
{
    STRCPY(LBUF," FAT&SNF    WEIGHT");  
    CLR_Enabled(LBUF+5);  
    LBUF[10]=0x91;
    Invert_Disp_Msg(LINE4);
    
    if(modcnt==0)
        STRCPY(LBUF," MANUAL  MANUAL");   
    
    if(modcnt==1)
        STRCPY(LBUF," MANUAL   AUTO ");  
    
    if(modcnt==2)
        STRCPY(LBUF,"  AUTO   MANUAL"); 
    
    if(modcnt==3)
        STRCPY(LBUF,"  AUTO    AUTO "); 
    
    LBUF[0]=0x83;
    LBUF[8]=0x83;
    LBUF[15]=0x85;
    NOR_DISPLAY_LAST_LINE=0xC0;  
    Disp_Msg_Big(0,LINE5);
    Reset_Display_Values();
}

void Display_Collection_Modes(UINT8 mode)
{    
    STRCPY(LBUF+7,",FAT(A)-WM(A)");
    
    if(mode==3)
    {
        Settingsrec.FAT_MODE=1;
        Settingsrec.WEIGHT_MODE=1;
    }
    if(mode==2)
    {
        LBUF[18]='M';
        Settingsrec.FAT_MODE=1;
        Settingsrec.WEIGHT_MODE=0;
    }
    if(mode==1)
    {
        LBUF[12]='M';
        Settingsrec.FAT_MODE=0;
        Settingsrec.WEIGHT_MODE=1;
    }
    if(mode==0)
    {
        LBUF[12]='M';LBUF[18]='M';
        Settingsrec.FAT_MODE=0;
        Settingsrec.WEIGHT_MODE=0;
    }   
}

void Mode_Selection(void)
{
    UINT8 mod_cnt;

    Display_Screen_Boarder();
    
    SEERAMReadString(Weight_Mode_Loc,2,(char *)&Settingsrec.WEIGHT_MODE);
    
    if(WEIGHT_COUNT==ANALYZER_COUNT)
        DISPLAY_LAST_LINE=0x70;
    
    STRCPY(LBUF,">COLLECTION MODE");
    Disp_Header_No_Boarder();
        
    if(WEIGHT_COUNT==ANALYZER_COUNT)
    {
        mod_cnt=(2*Settingsrec.FAT_MODE)+Settingsrec.WEIGHT_MODE;

        if(mod_cnt>3)
            mod_cnt=0;

        Up_Down_Select();
   
        while(1)
        {
            Display_Collection_Modes(mod_cnt);
            Display_Collection_Mode(mod_cnt);

            KBV=Wait_Key_Press('C');//e

            switch(KBV)
            {
                case KB_CANCEL:

                    return;

                case KB_ENT:

                    SEERAMWriteString(Weight_Mode_Loc,2,(char *)&Settingsrec.WEIGHT_MODE);
                    return;
            }
            mod_cnt=Up_Down(mod_cnt,4);
        }
    }
    else
    {
        Samples_Pending_Msg();   
    }
}

UINT8 Select_Accuracy(void)
{
    UINT8 s_a_cnt=0;

    SEERAMReadString(Accuracy_Loc,1,(char *)&s_a_cnt);

    if(s_a_cnt>2)
        s_a_cnt=0;
    else
        s_a_cnt=s_a_cnt-1;
    
    Display_Screen_Boarder();

    STRCPY(LBUF,"   *ACCURACY*");
    Disp_Header_No_Boarder();
    
    STRCPY(LBUF,"Select Accuracy?");
    Disp_Msg(LINE3);
    
    s_a_cnt=Select_Yes_No(s_a_cnt,5);
    
    if(s_a_cnt!=0xFF)
    {
        s_a_cnt=s_a_cnt+1;

        SEERAMWriteString(Accuracy_Loc,1,(char *)&s_a_cnt);
    }
}

UINT8 ON_OFF(UINT8 tflag)
{
    UINT8 p_temp_char,temp_mode[5],ptr_array_index=0;

    Display_Screen_Boarder();
   
    if(tflag=='A')
    {
        EEPROM_ADDR=Amount_Bold_Loc;
        ptr_array_index=0;     
    }

    if(tflag=='a')
    {
        EEPROM_ADDR=Auto_Commission_On_Loc;
        ptr_array_index=1;
    }
    
    if(tflag=='B')
    {
        EEPROM_ADDR=Multiple_Shifts_Loc;
        ptr_array_index=2;
    }
    
    if(tflag=='C')
    {
        EEPROM_ADDR=CLR_Based_Table_Loc;
        ptr_array_index=3;
    }
    
    if(tflag=='c')
    {
        EEPROM_ADDR=Server_Control_Loc;
        ptr_array_index=4;
    }    
    
    if(tflag=='D')
    {
        EEPROM_ADDR=Auto_Deduction_On_Loc;
        ptr_array_index=5;
    }
    
    if(tflag=='E')
    {
        EEPROM_ADDR=User_Col_Edit_Opt_Loc;
        ptr_array_index=6;
    }
    
     if(tflag=='e')
    {
        EEPROM_ADDR=Col_Left_Arrow_Edit_Loc;
        ptr_array_index=7;
    }

    if(tflag=='F')
    {
       EEPROM_ADDR=AP_SNF_Formula_Loc;
       ptr_array_index=8;
    }
    if(tflag=='f')
    {
       EEPROM_ADDR=SPL_Shift_File_Loc;
       ptr_array_index=9;
    }

    if(tflag=='H')
    {
        EEPROM_ADDR=Header_Bold_Loc;
        ptr_array_index=10;
    }
    
    if(tflag=='K')
    {
        EEPROM_ADDR=Weight_Kg_Loc;
        ptr_array_index=11;
    }
   
    if(tflag=='M')
    {
        EEPROM_ADDR=Mixed_Milk_Loc;
        ptr_array_index=12;
    }
    
    if(tflag=='m')
    {
        EEPROM_ADDR=Multiple_Samples_Loc;
        ptr_array_index=13;
    }        
   
    if(tflag=='P')
    {
        EEPROM_ADDR=Default_Printer_Loc;
        ptr_array_index=14;
    }

    if(tflag=='R')
    {
        EEPROM_ADDR=Round_Off_Loc;
        ptr_array_index=15;
    }

    if(tflag=='S')
    {
        EEPROM_ADDR=Auto_SMS_Loc;
        ptr_array_index=16;
    }
    if(tflag=='s')
    {
        EEPROM_ADDR=Shift_Close_Loc;
        ptr_array_index=17;
    }

    if(tflag=='T')
    {
        EEPROM_ADDR=Auto_Transfer_Loc;
        ptr_array_index=18;
    }
        
    if(tflag=='V')
    {
        EEPROM_ADDR=Monitor_Voltage_Loc;
        ptr_array_index=19;
    }

     if(tflag=='2')
    {
        EEPROM_ADDR=Two_Analyzers_Mode_Loc;
        ptr_array_index=20;
    }
    if(tflag=='3')
    {
        EEPROM_ADDR=USE_ANALYZER_PORT2_Loc;
        ptr_array_index=21;
    }
    if(tflag=='p')
    {
        EEPROM_ADDR=Power_On_Password_Loc;
        ptr_array_index=22;
    }
      
    if(tflag>=10 && tflag<=18)
    {        
        EEPROM_ADDR=MilkType_Print_On_Loc+(tflag-10);
        ptr_array_index=23+(tflag-10);
    }
    if(tflag>=10 && tflag<=18)
        PTR_ADDR=(char *)&ON_OFF_SETTINGS_MENU_DESC_LIST[23][0];
    else
        PTR_ADDR=(char *)&ON_OFF_SETTINGS_MENU_DESC_LIST[ptr_array_index][0];
    
    strncpy(LBUF,PTR_ADDR,20);
    Disp_Msg(LINE3);
    
    
    PTR_ADDR=(char *)&ON_OFF_SETTINGS_MENU_LIST[ptr_array_index][0];
    strncpy(LBUF,PTR_ADDR,16);
    Disp_Header_No_Boarder();
           
    SEERAMReadString(EEPROM_ADDR,1,(char *)&p_temp_char);

    if(p_temp_char==0xFF)
        p_temp_char=0;    
    
    else if(p_temp_char=='N')
        p_temp_char=0;

    else if(p_temp_char=='Y')
        p_temp_char=1;
    else
        p_temp_char=1;
    
    p_temp_char=Select_Yes_No(p_temp_char,2);
    
    if(p_temp_char!=0xFF)
    {
        if(p_temp_char==0)
           p_temp_char='N';

       else 
           p_temp_char='Y';

       SEERAMWriteString(EEPROM_ADDR,1,(char *)&p_temp_char);

       if(tflag=='2')
       {
            if(p_temp_char=='Y')
            {
                SEERAMWriteString(Shift_Close_Loc,1,(char *)&p_temp_char);

                temp_mode[0]=1;
                temp_mode[1]=1;

                SEERAMWriteString(Weight_Mode_Loc,2,(char *)&temp_mode);
            }
       }
    }

    return 0x01;
}

void W_L_Baud_Rate(UINT8 btype)
{
    UINT16 wm_baud_rate;
    UINT32 e_baud_rate;
    
    Display_Screen_Boarder();
    
   // Display_Box_Boarder(LINE2);
    
    if(btype=='W')
    {
        STRCPY(LBUF,"*WEIGHING SCALE*");
        EEPROM_ADDR=WM_Baud_Rate_Loc;
    }
    else if(btype=='L')
    {
        
        STRCPY(LBUF,"*LACTO/M-TESTER*");        
        EEPROM_ADDR=L_Baud_Rate_Loc;

    }
    else
        return;
    Disp_Header_No_Boarder();
        
    STRCPY(LBUF,"Between 1200 - 57600");
    Invert_Disp_Msg(LINE8);
    
    /*Display_Box_Top_Boarder(LINE4,0x18);//1
    Display_Bar_Big_T(LINE4,9,0x8A);*/
    
    Disp_Top_Boarder_Big_T(LINE4,9,0x8A);
    
    while(1)
    {
        SEERAMReadString(EEPROM_ADDR,2,(char *)&wm_baud_rate);
  
        STRCPY(LBUF," BAUDRATE|");
        ZERO=1;

        Hex_DEC(0,wm_baud_rate,LBUF+10,5); 
        Shift_Left(LBUF+10,5);
        strncpy(Temp_Arr,LBUF+10,5);
       
        LBUF[0]=0x83;
        LBUF[15]=0x85;

        NOR_DISPLAY_LAST_LINE=0xC0;
        Disp_Msg_Big(0,LINE5 );

        
        GET_VALUE_STATUS=Get_Numeric_Big(5,0,LINE5+10,(char *)&Temp_Arr); 
        Reset_Display_Values();
        
        if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
            return ;
      
        if(GET_VALUE_STATUS!=0xFF)
        {
            e_baud_rate=atol(Temp_Arr);
            if(e_baud_rate>=1200 && e_baud_rate<=57600)
            {
                wm_baud_rate=e_baud_rate;
                SEERAMWriteString(EEPROM_ADDR,2,(char *)&wm_baud_rate);
                Saved_Message();
                break;
            }
        }
    }
}

void Display_Snf_Limits(void)
{
    STRCPY(LBUF,"FAT:     - ");
    Hex_DEC(1,SNFLimitsrec.MIN_FAT,LBUF+4,4);
    Hex_DEC(1,SNFLimitsrec.MAX_FAT,LBUF+11,4);
    Disp_Msg(LINE6); 
    
    STRCPY(LBUF,"SNF:     - ");
    CLR_Enabled(LBUF);
    Hex_DEC(1,SNFLimitsrec.MIN_SNF,LBUF+4,4);
    Hex_DEC(1,SNFLimitsrec.MAX_SNF,LBUF+11,4);
    Disp_Msg(LINE7); 
}

void Display_Fat_Snf_Limits(UINT8 dmilktype)
{
    Display_Full_Boarder();
    Enter_Exit_Msg();
    ZERO=1;
    Read_FAT_Limits(dmilktype);
    STRCPY(LBUF,"Cow FAT Limits:");
    Buf_Type(dmilktype,LBUF);
    Disp_Msg(LINE2);    
    
    STRCPY(LBUF,"     - ");
    Hex_DEC(1,Fatlimitsrec.MIN_FAT,LBUF,4);
    Hex_DEC(1,Fatlimitsrec.MAX_FAT,LBUF+7,4);
    Disp_Msg(LINE3);    
    
    Read_SNF_Limits(dmilktype);
        
    STRCPY(LBUF,"Cow SNF Limits:");
    Buf_Type(dmilktype,LBUF);
    Disp_Msg(LINE5);
        
    Display_Snf_Limits();
}

void Display_Analyzer(void)
{    
    if(Settingsrec.ANALYZER=='U')
        STRCPY(LBUF,"EKO ULTRA");
    
    if(Settingsrec.ANALYZER=='P')
        STRCPY(LBUF,"ULTRAPRO");
    
    if(Settingsrec.ANALYZER=='L')
        STRCPY(LBUF,"LACTO SCAN");
    
    if(Settingsrec.ANALYZER=='E')
        STRCPY(LBUF,"ESSAE  ");  
    
    if(Settingsrec.ANALYZER=='K')
        STRCPY(LBUF,"KSHEERA   ");
    
    if(Settingsrec.ANALYZER=='A')
        STRCPY(LBUF,"MILK-O-METER");
    
    if(Settingsrec.ANALYZER=='M')
        STRCPY(LBUF,"MILKO-TESTER");
    
    if(Settingsrec.ANALYZER=='F')
        STRCPY(LBUF,"FATOMATIC");
}


 void Current_Settings(void)
{
    UINT8 c_temp_char;

    while(1)
    {
        Display_Full_Boarder();
        Enter_Exit_Msg();
        
        Read_Settings();
        
        Display_Analyzer();

        c_temp_char=(2*Settingsrec.FAT_MODE)+Settingsrec.WEIGHT_MODE;
        Display_Collection_Modes(c_temp_char);
        Disp_Msg(LINE1);

        STRCPY(LBUF,"Use SNF:-COW: ,BUF:");
        CLR_Enabled(LBUF+4);
        LBUF[13]=Settingsrec.COW_SNF;
        LBUF[19]=Settingsrec.BUF_SNF;
        Disp_Msg(LINE3);

        STRCPY(LBUF,"CUT OFF-LOW:A,HIGH:");
        LBUF[12]=Settingsrec.LOW_FAT_SNF;
        LBUF[19]=Settingsrec.HIGH_FAT_SNF;
        Disp_Msg(LINE5);
        
        STRCPY(LBUF,"Mem.List: , Tables:");
        LBUF[9]=DPUcontrolrec.MEMBER_LIST;
        LBUF[19]=DPUcontrolrec.RATE_TABLES;
        Disp_Msg(LINE7);

        if(Wait_Key_Press('C')==KB_CANCEL)
            return;

        STRCPY(LBUF,"COL.MODE: ,  Mixed:");
        LBUF[9]=DPUcontrolrec.COLLECTION;
        LBUF[19]=Settingsrec.MIXED_MILK;
        Disp_Msg(LINE1);

        STRCPY(LBUF,"SMS: ,DATA: ,Shift:");
        LBUF[4]=GPRSrec.AUTO_SMS;
        LBUF[11]=GPRSrec.AUTO_TRANSFER;
        LBUF[19]=Settingsrec.SHIFT_CLOSE;
        Disp_Msg(LINE3);

        STRCPY(LBUF,"Lt-1200-100ML-Tare:");
        if(WEIGHT_KG=='Y')
        {
            LBUF[0]='K';LBUF[1]='g';
        }
        Hex_DEC(0,WMrec.WM_BAUDRATE,LBUF+3,4);
        
        if(WMrec.WM_ACCURACY==2)   
            LBUF[10]=' ';
      
        LBUF[19]=WMrec.WM_AUTO_TARE;
        Disp_Msg(LINE5);

        if(Commissionrec.STATUS=='N')
            STRCPY(LBUF,"Incentive : OFF");
        else
        {
            STRCPY(LBUF,"N:000.00 , S:0.00");
            ZERO=1;
            Hex_DEC(2,Commissionrec.AUTO_RATE,LBUF+2,6);
            Shift_Left(LBUF+2,6);
           // Hex_DEC(2,Commissionrec.SPECIAL_RATE,LBUF+13,6);
         //   Shift_Left(LBUF+13,6);
        }
        Disp_Msg(LINE7);

        if(Wait_Key_Press('C')==KB_CANCEL)
            return;
        
        Display_Fat_Snf_Limits('B');
        
        if(Wait_Key_Press('C')==KB_CANCEL)
            return ;
        
        Display_Fat_Snf_Limits('C');
        
        if(Wait_Key_Press('C')==KB_CANCEL)
            return;
    }
}


void Clear_Samples_Data(void)
{
    UINT8 tempi;
    UINT32 addr;
    
    for(tempi=0;tempi<5;tempi++)
    {
        addr=Sample_Records_Loc+(tempi*4096);
        
        Sector_Erase(addr);
    }
}

void Reset_Tra_Counters(void)
{
    if(WEIGHT_COUNT<1500 && WEIGHT_COUNT>0)
        Update_Total_Shift_Data();
    
    Clear_Samples_Data();
    
    Clear_Transaction_Counters();
}

void Contact_No_SubMenu(UINT8 headersubcnt)
{
    while(1)
    {
        Admin_Header('P');
        Display_Boarder2();
        
        Save_Exit_Msg();

        DISPLAY_NO_SPACES=1;
        STRCPY(LBUF,"CONTACT NO1:");
        LBUF[10]=0x31+headersubcnt;
        Invert_Disp_Msg(LINE3);
        DISPLAY_NO_SPACES=0;

        EEPROM_ADDR=Contact_No1_Loc+(headersubcnt*10);
        SEERAMReadString(EEPROM_ADDR,10,PBUF);
        STRNCPY(LBUF,PBUF,10);
        Disp_Msg(LINE5);

        GET_VALUE_STATUS=Get_Numeric(10,0,LINE5,(char *)&PBUF[0]);

        if(GET_VALUE_STATUS==0xFF || GET_VALUE_STATUS==0x60)
            break;

        if(GET_VALUE_STATUS!=0xFF && GET_VALUE_STATUS!=0x60)
        {
            SEERAMWriteString(EEPROM_ADDR,10,PBUF);
            break;
        }
    }
}

void Contact_Numbers(void)
{
    UINT8 header_count=0;

    while(header_count!=0xFF)
    {
        Admin_Header('c');

        STRCPY(LBUF,"Prev:         Next: ");
        LBUF[5]=0x80;LBUF[19]=0x81;
        Disp_Msg(LINE6);
        
        Back_Select();
       
        STRCPY(LBUF," 1.PHONE NUMBER1");
        LBUF[1]=0x31+header_count;
        LBUF[15]=0x31+header_count;
        Disp_Msg_Boarder();

        header_count=Function_Selected(3,header_count);
        if(KBV==KB_ENT)
        {
            Contact_No_SubMenu(header_count);
        }
    }
}

void Receive_Server_Settings(void)
{
    Server_Menu_Header('S');
    
    STRCPY(LBUF,"Receive Settings?");

    if(Confirm_Yes()==1)
    {
        Display_Boarder3();
        if(SIM_INSERTED==1 && Modem_Init_Flag==1  &&  SMS_STATE==0 && SERVER_STATE==0 &&  INIT_MODEM_STATE==0 )
        {
            Server_Communication(1);
            if(MODEM_ERROR=='S')
                Current_Settings();
        }
        else
        {
            STRCPY(LBUF," Try Later...");
            Disp_Msg_No_Spaces(0,LINE5+1);
            Delay_Sec(1);
        }
    }
}


void Use_SNF_Based(UINT8 milktype)
{
    Display_Screen_Boarder();

    EEPROM_ADDR=Use_Cow_SNF_Loc+('C'-milktype);
    
    STRCPY(LBUF,"*COW SNF BASED*");
    Buf_Type(milktype,LBUF+1);
    CLR_Enabled(LBUF+5);
    Disp_Header_No_Boarder();

    STRCPY(LBUF,"USE IN CALCULATIONS?");
    
    Yes_No();
}

void Cutoff_Limits(UINT8 ctype)
{
    Display_Screen_Boarder();

    if(ctype=='H')
    {
        STRCPY(LBUF,"*HIGH FAT & SNF*");

        EEPROM_ADDR=HighFatFlag_Loc;
    }

     if(ctype=='L')
     {
        STRCPY(LBUF,"*LOW FAT & SNF*");
        EEPROM_ADDR=LowFatFlag_Loc;
     }
    CLR_Enabled(LBUF+11);
    Disp_Header_No_Boarder();

    STRCPY(LBUF,"USE IN COLLECTION?");
    
    Yes_No();
}

void Society_Code(void)
{
    while(1)
    {
        Admin_Header('H');
        Display_Boarder2();
        
        Save_Exit_Msg();

        STRCPY(LBUF,"SOCIETY CODE:");
        DISPLAY_NO_SPACES=1;
        Invert_Disp_Msg(LINE4);
        DISPLAY_NO_SPACES=0;

        STRNCPY(LBUF,SOCIETY_CODE,10);
        Disp_Msg(LINE6);

        GET_VALUE_STATUS=Get_String(10,LINE6,(char *)&SOCIETY_CODE);

        if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                return;

        if(GET_VALUE_STATUS!=0xFF)
        {
            SEERAMWriteString(Society_Code_Loc,10,(char *)&SOCIETY_CODE);
            return;
        }
    }
}

void Deduction_Names(void)
{
    UINT16 name_count=0;

    Display_Screen_Boarder();

    STRCPY(LBUF,">DEDUCTION NAMES");
    Disp_Header_No_Boarder();
    while(1)
    {
        Up_Down_Select();
        
        STRCPY(LBUF,"NAME1:");
        LBUF[4]=0x31+name_count;
        DISPLAY_NO_SPACES=1;
        Invert_Disp_Msg(LINE3);
        DISPLAY_NO_SPACES=0;    

        EEPROM_ADDR=Deduction_Names_Loc+(name_count*Deduction_Name_Length);
        SEERAMReadString(EEPROM_ADDR,Deduction_Name_Length,(char *)&PBUF);

        STRNCPY(LBUF,PBUF,19);
        Disp_Msg(LINE5);

         KBV=Wait_Key_Press('E');

        switch(KBV)
        {
            case KB_CANCEL:

                return;

            case KB_ENT:

                Save_Exit_Msg();
                
                GET_VALUE_STATUS=Get_String(19,LINE5,(char *)&PBUF[0]);

                if(GET_VALUE_STATUS==0xFF)
                    return;

                else
                {
                    SEERAMWriteString(EEPROM_ADDR,Deduction_Name_Length,(char *)&PBUF);
                    name_count++;
                    if(name_count>=9)
                        name_count=0;
                        break;
                }
        }
        name_count=Up_Down(name_count,9);        
    }
}


void Milk_Dairy_Name(void)
{
    while(1)
    {
        Admin_Header('D');

        Display_Boarder2();    
        Save_Exit_Msg();

        SEERAMReadString(Dairy_Name_Loc,32,PBUF);

        STRNCPY(LBUF,PBUF,20);
        Disp_Msg(LINE4);

        STRNCPY(LBUF,PBUF+20,12);
        Disp_Msg(LINE5);

        GET_VALUE_STATUS=Get_String(32,LINE4,(char *)&PBUF[0]);        

        if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
            return;

        else if(GET_VALUE_STATUS!=0xFF)
        {
            SEERAMWriteString(Dairy_Name_Loc,32,(char *)&PBUF);
            return;
        }
    }
}

void Display_Input_Voltage(void)
{
    if(DISABLE_VOL_BUZ==0)
    {
        STRCPY(LBUF,"INPUT VOLTAGE:12.5V");
        Hex_DEC(1,VOLTAGE*10,LBUF+14,4);
        Disp_Msg(LINE6);        
        
        Buzzer(250);
    }
    //Buzzer(250);
}

void Read_Voltage(void)
{
    UINT16 length,m_input_voltage;
    
    AD1CON1bits.ADON = 1; // turn On A/D converter
    AD1CON1bits.SAMP = 1; // start sampling
    while(AD1CON1bits.DONE==0)
        ;
    AD1CON1bits.DONE=0;
    length = ADC1BUF0; // read the buffer with the result
    
    SEERAMReadString(Input_Voltage_Loc,2,(char *)&m_input_voltage);

    VOLTAGE=length*3.3;
    VOLTAGE=(VOLTAGE/1024)*5.7;
    
    MAX_INPUT_VOLTAGE=(float)(m_input_voltage)/10;
    
    if(VOLTAGE>1 && (VOLTAGE<6.5 || VOLTAGE >MAX_INPUT_VOLTAGE))//8
    {
        if(VOLTAGE>MAX_INPUT_VOLTAGE && VOLTAGE_HIGH_ACCEPT==0)//8
        {
            Display_Input_Voltage();

        }
        if(VOLTAGE<6.5)//6.5
        {
            STRCPY(LBUF,"SHIFT TO MAINS POWER");
            Disp_Msg(LINE6);

            STRCPY(LBUF,"   LOW BATTERY");
            Disp_Msg_Big(0,LINE4);
            Buzzer(250);
            Buzzer(250);                   
        }
    }

    AD1CON1bits.ADON = 0; // turn off A/D converter
    AD1CON1bits.SAMP = 0; // End sampling
}



void Header_SubMenu(UINT8 headersubcnt)
{
    while(1)
    {
        Admin_Header('H');
        Display_Boarder2();
        
        Save_Exit_Msg();

        STRCPY(LBUF,"ADDRESS LINE1:");
        LBUF[12]=0x31+headersubcnt;
        
        if(headersubcnt==3)// || headersubcnt==4)
        {
            STRCPY(LBUF,"FOOTER LINE1:");
           // LBUF[11]=0x31+(headersubcnt-3);
        }
        
        if(headersubcnt==4)
        {
            Society_Code();
            SEERAMReadString(Society_Code_Loc,10,(char *)&SOCIETY_CODE);
            return;
        }
        
        if(headersubcnt==5)
        {
            STRCPY(LBUF,"SOCIETY NAME:");
            EEPROM_ADDR=Society_Name1_Loc;
        }
        
        if(headersubcnt>=0 && headersubcnt<=3)
            EEPROM_ADDR=Society_Name2_Loc+(headersubcnt*32);
        
        DISPLAY_NO_SPACES=1;
        Invert_Disp_Msg(LINE3);
        DISPLAY_NO_SPACES=0;

        SEERAMReadString(EEPROM_ADDR,32,PBUF);
        STRNCPY(LBUF,PBUF,20);
        Disp_Msg(LINE5);

        STRCPY(LBUF,"                  ");
        STRNCPY(LBUF,PBUF+20,12);
        Disp_Msg(LINE6);

        GET_VALUE_STATUS=Get_String(32,LINE5,(char *)&PBUF[0]);

        if(KBV!=KB_LEFT && (GET_VALUE_STATUS==0xFF || GET_VALUE_STATUS==0x60))
            break;

        if(GET_VALUE_STATUS!=0xFF && GET_VALUE_STATUS!=0x60)
        {
            SEERAMWriteString(EEPROM_ADDR,32,PBUF);
            break;
        }
    }
}

void Header_Footer(void)
{
    UINT8 header_count=0;

    while(header_count!=0xFF)
    {
        Admin_Header('s');

        STRCPY(LBUF,"Prev:         Next: ");
        LBUF[5]=0x80;LBUF[19]=0x81;
        Disp_Msg(LINE6);
        
        Back_Select();        
       
        
        PTR_ADDR=(char *)&SOCIETY_DETAILS_MENU_LIST[header_count][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Boarder();
        
        header_count=Function_Selected(6,header_count);
                
        if(KBV==KB_ENT)
        {
            Header_SubMenu(header_count);            
        }
    }
}


void Admin_Settings_SubMenu(UINT8 adminmenucnt)
{
    if(adminmenucnt==0)
        Change_Password();

    if(adminmenucnt==1)
        Milk_Dairy_Name();

   if(adminmenucnt==2)
        Deduction_Names();

    if(adminmenucnt==3)
    {
        FONT_TYPE='B';
        TIME_RIGHT_ARROW='Y';
        SetDate_Time(5);
        TIME_RIGHT_ARROW='N';
        FONT_TYPE='S';
        CLEAR_BUF_FLAG=0;            
    }

    if(adminmenucnt==4)
        Header_Footer();  

    if(adminmenucnt==5)
        ON_OFF('F');    

    if(adminmenucnt==6)
    {
        ON_OFF('V');
        SEERAMReadString(Monitor_Voltage_Loc,1,(char *)&MONITOR_VOLTAGE);

        if(MONITOR_VOLTAGE=='Y')
        {
            Display_Boarder3();
            //DISABLE_VOL_BUZ=0;
            Read_Voltage();
            Display_Input_Voltage();
            Exit_Any_Key(1);
        }             
    }
    if(adminmenucnt==7)
    {            
        ON_OFF('K');                
        SEERAMReadString(Weight_Kg_Loc,1,(char *)&WEIGHT_KG);            
    }
    
     if(adminmenucnt==8)
        ON_OFF('p');
    
    if(adminmenucnt==9)
    {        
        Receive_Server_Settings();
    }

    if(adminmenucnt==10)
        Contact_Numbers();
}

void Admin_Settings(void)
{
    UINT8 u_cnt=0,total_funs=11;
    
    if(ON_LINE=='N')
        total_funs=9;

    while(u_cnt!=0xFF)
    {
        Settings_SubMenu_Header(6);
        
        PTR_ADDR=(char *)&ADMIN_SETTINGS_MENU_LIST[u_cnt][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Boarder();
        
        PTR_ADDR=(char *)&ADMIN_SETTINGS_MENU_DESC_LIST[u_cnt][0];
        strncpy(LBUF,PTR_ADDR,20);
        Disp_Msg(LINE5);

        u_cnt=Function_Selected(total_funs,u_cnt);//10
        
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            Admin_Settings_SubMenu(u_cnt);
            TIME_FLAG=1;
        }        
    }
}

void Display_From_To_Hours(UINT8 etype,UINT8 fhour,UINT8 ehour)
{
    /*Display_Box_Top_Boarder(LINE6,0x18);
    Display_Bar_Big_T(LINE6,5,0x8A);*/
    
    Disp_Top_Boarder_Big_T(LINE6,5,0x8A);
    
    NOR_DISPLAY_LAST_LINE=0xC0;
    STRCPY(LBUF," FROM|00-00 Hrs");
    LBUF[0]=0x83;
    //LBUF[5]=0x84;
    LBUF[15]=0x85;    
    Hex_DEC(0,fhour,LBUF+6,2);
    Hex_DEC(0,ehour,LBUF+9,2);
    Disp_Msg_Big(0,LINE7);
    ZERO=0;
    if(etype==0)
        Hex_DEC(0,fhour,Temp_Arr,2);
    else
        Hex_DEC(0,ehour,Temp_Arr,2);
    Temp_Arr[2]='\0';
}

void Morning_Shift_Timings(UINT8 dline)
{
    STRCPY(LBUF,"Morning:00 To 00 Hrs");
    Hex_DEC(0,M_ShiftTimerec.START_HOUR,LBUF+8,2);
    Hex_DEC(0,M_ShiftTimerec.END_HOUR,LBUF+14,2);
    Disp_Msg(dline);
}

void Evening_Shift_Timings(UINT8 dline)
{            
    STRCPY(LBUF,"Evening:00 To 00 Hrs");
    Hex_DEC(0,E_ShiftTimerec.START_HOUR,LBUF+8,2);
    Hex_DEC(0,E_ShiftTimerec.END_HOUR,LBUF+14,2);
    Disp_Msg(dline);
}



void Shift_Timings(UINT8 shifttype)
{
    UINT8  s_step=0,s_hour,e_hour;
    UINT32 temp_addr;
    
    if(shifttype=='M')
        temp_addr=Morning_Shift_Timings_Loc;        
    else
        temp_addr=Evening_Shift_Timings_Loc;
    
    SEERAMReadString(temp_addr,2,(char *)&ShiftTimerec);
    
    s_hour=ShiftTimerec.START_HOUR;
    e_hour=ShiftTimerec.END_HOUR;
    
    while(1)
    {
        switch(s_step)
        {
            case 0:

                Menu_Header('s',0);
                
                Display_Boarder3();
                ZERO=0;
                
                if(WEIGHT_COUNT>0 && s_hour!=0xFF && e_hour!=0xFF)
                {
                    
                    if(shifttype=='M')
                        Morning_Shift_Timings(LINE3);
                    else
                        Evening_Shift_Timings(LINE3);
                    
                    STRCPY(LBUF,"  SHIFT IS RUNNING");
                    Disp_Msg(LINE5);
                    
                    STRCPY(LBUF," YOU CAN NOT CHANGE");
                    Disp_Msg(LINE7);                    
                    
                    Exit_Any_Key(1);
                    return;
                }
                
                Display_Box_Boarder(LINE5);
                
                if(shifttype=='M')
                    STRCPY(LBUF,"Morning Shift:");
                else
                    STRCPY(LBUF,"Evening Shift:");
                Disp_Msg(LINE5);
             
                Display_From_To_Hours(0,s_hour,e_hour);

                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE7+6,(char *)&Temp_Arr);
                Reset_Display_Values();
                
                if(KBV==KB_LEFT || GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    s_hour=atoi(Temp_Arr);

                    if(shifttype=='M')
                    {
                        SEERAMReadString(Evening_Shift_Timings_Loc,2,(char *)&E_ShiftTimerec);
                        if(s_hour>=0 && s_hour<=E_ShiftTimerec.START_HOUR)
                        {
                            ShiftTimerec.START_HOUR=s_hour;
                            s_step=1;
                        }
                    }
                    else
                    {
                        SEERAMReadString(Morning_Shift_Timings_Loc,2,(char *)&M_ShiftTimerec);
                        if(s_hour>=M_ShiftTimerec.END_HOUR && s_hour<=24)
                        {
                            ShiftTimerec.START_HOUR=s_hour;
                            s_step=1;
                        }
                    }                   
                }

                break;

            case 1:
                
                Display_From_To_Hours(1,s_hour,e_hour);
                
                GET_VALUE_STATUS=Get_Numeric_Big(2,0,LINE7+9,(char *)&Temp_Arr);
                Reset_Display_Values();
                
                if(KBV==KB_LEFT)
                {
                    s_step=0;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else
                {
                    e_hour=atoi(Temp_Arr);
                    if(shifttype=='M')
                    {
                        if(e_hour>=s_hour  && e_hour<=E_ShiftTimerec.START_HOUR)
                        {                        
                            ShiftTimerec.END_HOUR=e_hour;
                            SEERAMWriteString(temp_addr,2,(char *)&ShiftTimerec);
                            return;
                        }
                    }
                    else
                    {
                         if(e_hour>=s_hour  && e_hour<=24)
                        {                        
                            ShiftTimerec.END_HOUR=e_hour;
                            SEERAMWriteString(temp_addr,2,(char *)&ShiftTimerec);
                            return;
                        }
                    }
                   
                }
                break;
        }
    }
}

void Liter_Rate(UINT8 milktype)
{
    UINT16 ltr_rate;
    
    while(1)
    { 
        Display_Screen_Boarder();
        if(milktype=='C')
        {
            EEPROM_ADDR=Cow_LocalSales_Ltr_Loc;
        }
        else
        {
            EEPROM_ADDR=Buf_LocalSales_Ltr_Loc;
        }    
        STRCPY(LBUF,"*COW LOCAL SALE*");
        Buf_Type(milktype,LBUF+1);
        Disp_Header_No_Boarder();
        
        SEERAMReadString(EEPROM_ADDR,2,(char *)&ltr_rate);
        
        if(ltr_rate==0xFFFF)
            ltr_rate=5000;

        STRCPY(LBUF," RATE/Lt:");
        LBUF[0]=0x83;
        
        if(WEIGHT_KG=='Y')
        {
            STRCPY(LBUF+6,"Kg:");
        }
        ZERO=1;
        Hex_DEC(2,ltr_rate,(char *)&LBUF+9,6);
        Shift_Left(LBUF+9,6);
        strncpy(Temp_Arr,LBUF+9,6);
        LBUF[15]=0x85;
        //Hex_DEC(2,ltr_rate,Temp_Arr,6);
        Disp_Msg_Big(0,LINE4);

        GET_VALUE_STATUS=Get_Numeric_Big(6,2,LINE4+9,(char *)&Temp_Arr);

        if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
            return;

        if(GET_VALUE_STATUS!=0xFF)
        {
            ltr_rate=Float_Long(Temp_Arr);

            SEERAMWriteString(EEPROM_ADDR,2,(char *)&ltr_rate);
            Saved_Message();

            break;
        }
    }
}



void Local_Sales_Rate(void)
{
    UINT8 table_count=0;

    while(1)
    {
        Display_Box_Line4();
        STRCPY(LBUF," *LOCAL SALES RATE*");
        Disp_Msg(LINE1);

        STRCPY(LBUF," COW MILK RATE");
        Buf_Type(('B'+table_count),LBUF+1);

        Disp_Msg_Boarder();

        STRCPY(LBUF,"Rate Per Liter...");
        Disp_Msg(LINE5);

        KBV=Wait_Key_Press('E');

        switch(KBV)
        {
            case KB_CANCEL:
                return;

            case KB_ENT:

                Liter_Rate('B'+table_count);
                break;


        }
        table_count=Up_Down(table_count,2);
    }
}


void Collection_Settings_SubMenu(UINT8 collectcnt)
{
    if(collectcnt==0)
    {
        TIME_FLAG=1;
        Auto_Deduction();               //Auto Deductions
        TIME_FLAG=0;
    }
    
    if(collectcnt==1)
        Receipt_Printer_Type();
   
    if(collectcnt==2)
    {
        if(ANALYZER_COUNT==WEIGHT_COUNT)
        {
            ON_OFF('2');    //Two Analyzer Mode
            SEERAMReadString(Two_Analyzers_Mode_Loc,1,(char *)&TWO_ANALYZERS);
        }
        else
        {           
            Display_Screen_Boarder();
            STRCPY(LBUF,"*DUAL ANALYZERS*");                    
            Disp_Header_Boarder();

            Samples_Pending_Msg(); 
        }
    }
    
    if(collectcnt==3)
        ON_OFF('3');    //Analyzer Port
   
    if(collectcnt==4)
        Local_Sales_Rate();
    
    if(collectcnt==5)
        ON_OFF('R');    //Round off Rate
    
    if(collectcnt==6)
        Shift_Timings('M');
    
    if(collectcnt==7)
        Shift_Timings('E');
    
    if(collectcnt==8)           //edit control
        ON_OFF('E');    
    
    if(collectcnt==9)           //left arrow edit during online col
        ON_OFF('e');
}

void Collection_Settings(void)
{
    UINT8 h_menu_cnt=0;

    while(h_menu_cnt!=0xFF)
    {
        Settings_SubMenu_Header(2);
       
        PTR_ADDR=(char *)&COL_SETTINGS2_MENU_LIST[h_menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Boarder();

        PTR_ADDR=(char *)&COL_SETTINGS2_MENU_DESC_LIST[h_menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,20);
        Disp_Msg(LINE5);

        h_menu_cnt=Function_Selected(10,h_menu_cnt);
        
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            Collection_Settings_SubMenu(h_menu_cnt);
            TIME_FLAG=1;
        }
    }
}

void Collection_Server_Settings_SubMenu(UINT8 collectcnt)
{
    if(collectcnt==0)
        Mode_Selection();               //Collection Mode

    if(collectcnt==1)
    {
        TIME_FLAG=1;
        Auto_Commission();
        TIME_FLAG=0;
    }
    
   if(collectcnt==2)
        Use_SNF_Based('B');

    if(collectcnt==3)
        Use_SNF_Based('C');

    if(collectcnt==4)
        Cutoff_Limits('H');

    if(collectcnt==5)
        Cutoff_Limits('L');

    if(collectcnt==6)
        ON_OFF('M');    //Mixed Milk
}

void Collection_Server_Settings(void)
{
    UINT8 h_menu_cnt=0,no_reports;

    while(h_menu_cnt!=0xFF)
    {
        //Display_Col_Header();
        Settings_SubMenu_Header(1);
       
        SEERAMReadString(Auto_Manual_On_Loc,1,(char *)&DPUcontrolrec.COLLECTION);
        
        if(SERVER_CONTROL=='Y' && DPUcontrolrec.COLLECTION=='N')
        {
            Display_Boarder3();
            STRCPY(LBUF,"SERVER CONTROL");    
            Disp_Msg_No_Spaces(0,LINE4+1);
            
            Exit_Any_Key(1);
            return;                    
        }
        
        if(SERVER_CONTROL=='Y' && DPUcontrolrec.COLLECTION=='Y')
            no_reports=1;
        else
            no_reports=7;

        PTR_ADDR=(char *)&COL_SETTINGS_MENU_LIST[h_menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,16);
        if(h_menu_cnt>1 && h_menu_cnt<6)
            CLR_Enabled(LBUF+7);
        Disp_Msg_Boarder();

        PTR_ADDR=(char *)&COL_SETTINGS_MENU_DESC_LIST[h_menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,20);
        Disp_Msg(LINE5);

        h_menu_cnt=Function_Selected(no_reports,h_menu_cnt);
        
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            Collection_Server_Settings_SubMenu(h_menu_cnt);
            TIME_FLAG=1;
        }
    }
}

void Analyzer_Settings_SubMenu(UINT8 analyzercnt)
{
    UINT8 online_kb_flag=0;
    
    if(analyzercnt==online_kb_flag)
    {
        W_L_Baud_Rate('L');  
        return;
    }
      
    if(analyzercnt==online_kb_flag+1)
    {
        Milk_Analyzer();
        return;
    }   
  
}

void Analyzer_Settings(void)
{
    UINT8 h_menu_cnt=0,a_limit=2,online_kb_flag=0;    
     
    if(SERVER_CONTROL=='Y' && DPUcontrolrec.COLLECTION=='N')
    {            
        a_limit=1; 
    }
   

    while(h_menu_cnt!=0xFF)
    {
        Settings_SubMenu_Header(3);
    
        if(h_menu_cnt==online_kb_flag)
        {
            STRCPY(LBUF," 2.BAUD RATE");
            LBUF[1]=0x31+h_menu_cnt;
            STRCPY(PBUF,"LACTO/ Milko/ Essae");
        }             
        
        if(h_menu_cnt==online_kb_flag+1)
        {
            STRCPY(LBUF," 3.ANALYZER");
            LBUF[1]=0x31+h_menu_cnt;
            STRCPY(PBUF,"Ultra/Pro/Lacto... ");

          //  menu_flag_sel='C';
        }
        

        Disp_Msg_Boarder();

        STRNCPY(LBUF,PBUF,19);
        Disp_Msg(LINE5);

        h_menu_cnt=Function_Selected(a_limit,h_menu_cnt);
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            Analyzer_Settings_SubMenu(h_menu_cnt);
            TIME_FLAG=1;
        }
    }
}

void WM_Digits(void)
{
    UINT8 wm_pos;
    
    Display_Screen_Boarder();   

   // Display_Box_Boarder(LINE2);

    STRCPY(LBUF,"*MINIMUM DIGITS*");
    Disp_Header_No_Boarder(); 

    STRCPY(LBUF,"  Between 02 to 09");
    Invert_Disp_Msg(LINE8);
    
    /*Display_Box_Top_Boarder(LINE4,0x18);
    Display_Bar_Big_T(LINE4,11,0x8A);*/
    Disp_Top_Boarder_Big_T(LINE4,11,0x8A);
    
    while(1)
    {
        SEERAMReadString(WM_Min_Dig_Loc,1,(char *)&wm_pos);

        if( wm_pos<2)
            wm_pos=2;
        else if(wm_pos>9)
            wm_pos=9;
        
        STRCPY(LBUF," MIN.DIGITS|   ");
        LBUF[0]=0x83;
        //LBUF[11]=0x84;
        LBUF[15]=0x85;
        LBUF[12]=Temp_Arr[0]=wm_pos+0x30;
        
        NOR_DISPLAY_LAST_LINE=0xC0;
        Disp_Msg_Big(0,LINE5);  
        
        GET_VALUE_STATUS=Get_Numeric_Big(1,0,LINE5+12,(char *)&Temp_Arr);   
        Reset_Display_Values();

        if(GET_VALUE_STATUS==0xFF)
            return;

        if(GET_VALUE_STATUS!=0xFF)
        {
            wm_pos=Temp_Arr[0]-0x30;

            if(wm_pos>=2 && wm_pos<=9)
            {
                SEERAMWriteString(WM_Min_Dig_Loc,1,(char *)&wm_pos);

                Saved_Message();     
                break;
            }
        }
    }    
}

/*void Tare_Command(void)
{
    UINT8 t_temp_char;

    Display_Screen_Boarder();
    
    STRCPY(LBUF," *TARE COMMAND*");
    Disp_Header_No_Boarder();
    
    //Display_Box_Boarder(LINE3);   
    Display_Box_Top_Boarder(LINE4,0x18);
    Display_Bar_Big_T(LINE4,12,0x8A);

    SEERAMReadString(Tare_Command_Loc,1,(char *)&t_temp_char);

    STRCPY(LBUF," TARE LETTER|   ");
    LBUF[0]=0x83;
    //LBUF[12]=0x84;
    LBUF[15]=0x85;
    LBUF[13]=t_temp_char;
    NOR_DISPLAY_LAST_LINE=0xC0;
    Disp_Msg_Big(0,LINE5);  

    GET_VALUE_STATUS=Get_String_Big(1,100,LINE5+13,(char *)&t_temp_char);
    Reset_Display_Values();
    
    if(GET_VALUE_STATUS==0xFF)
        return;

    else
    {
        SEERAMWriteString(Tare_Command_Loc,1,(char *)&t_temp_char);

        Saved_Message();      
    }
}*/

void Input_Command(UINT8 cmdtype)
{
    UINT8 t_temp_char;
    UINT16 cmdtype_addr_loc;

    Display_Screen_Boarder();
    
    if(cmdtype=='T')
    {
        cmdtype_addr_loc=Tare_Command_Loc;
        STRCPY(LBUF," *TARE COMMAND*");
        STRCPY(PBUF," TARE LETTER | ");
    }
    else if(cmdtype=='M')
    {
        cmdtype_addr_loc=Col_Manual_Msg_Symbol_Loc;
        STRCPY(LBUF," *MANUAL SYMBOL*");
        STRCPY(PBUF," ENTER SYMBOL| ");
    }
        
    Disp_Header_No_Boarder();
    
      
    /*Display_Box_Top_Boarder(LINE4,0x18);
    Display_Bar_Big_T(LINE4,13,0x8A);*/
    Disp_Top_Boarder_Big_T(LINE4,13,0x8A);

    SEERAMReadString(cmdtype_addr_loc,1,(char *)&t_temp_char);

    strncpy(LBUF,PBUF,15);
        
    LBUF[0]=0x83;
    LBUF[15]=0x85;
    LBUF[14]=t_temp_char;
    NOR_DISPLAY_LAST_LINE=0xC0;
    Disp_Msg_Big(0,LINE5);  

    GET_VALUE_STATUS=Get_String_Big(1,100,LINE5+14,(char *)&t_temp_char);
    Reset_Display_Values();
    
    if(GET_VALUE_STATUS==0xFF)
        return;

    else
    {
        SEERAMWriteString(cmdtype_addr_loc,1,(char *)&t_temp_char);

        Saved_Message();      
    }
}

void Read_WM_Dot_Pos(void)
{
    UINT8 wm_pos;
    
    Display_Screen_Boarder();  
    
    //Display_Box_Boarder(LINE2);
   
    
    STRCPY(LBUF,"*DECIMAL POINTS*");
    Disp_Header_No_Boarder();   
    
    STRCPY(LBUF,"  Between 00 to 03");
    Invert_Disp_Msg(LINE8);
    
    while(1)
    {
        SEERAMReadString(WM_Dot_Pos_Loc,1,(char *)&wm_pos);

        if(wm_pos>3)
            wm_pos=0;
        
       /* Display_Box_Top_Boarder(LINE4,0x18);
        Display_Bar_Big_T(LINE4,10,0x8A);*/
        
        Disp_Top_Boarder_Big_T(LINE4,10,0x8A);

        STRCPY(LBUF," AFTER DOT|    ");
        LBUF[0]=0x83;
        LBUF[15]=0x85;
        LBUF[11]=Temp_Arr[0]=wm_pos+0x30;
        
        NOR_DISPLAY_LAST_LINE=0xC0;
        Disp_Msg_Big(0,LINE5);  
       
        GET_VALUE_STATUS=Get_Numeric_Big(1,0,LINE5+11,(char *)&Temp_Arr);   
        Reset_Display_Values();

        if(GET_VALUE_STATUS==0xFF)
            return;

        if(GET_VALUE_STATUS!=0xFF)
        {
            wm_pos=Temp_Arr[0]-0x30;

            if(wm_pos>=0 && wm_pos<=3)
            {
                SEERAMWriteString(WM_Dot_Pos_Loc,1,(char *)&wm_pos);

                   Saved_Message();     
                   break;            }
        }
    }    
}

void WM_Settings_SubMenu(UINT8 wmcnt)
{
    if(wmcnt==0)
        Select_Accuracy();

    if(wmcnt==1)
       Auto_Tare();

    if(wmcnt==2)
        W_L_Baud_Rate('W');

    if(wmcnt==3)
        Read_WM_Dot_Pos();

    if(wmcnt==4)
        Input_Command('T');//Tare_Command();

    if(wmcnt==5)
       WM_Digits();
    
}

void WM_Settings(void)
{
    UINT8 h_menu_cnt=0,wm_fun_cnt=6;

    while(h_menu_cnt!=0xFF)
    {
        Settings_SubMenu_Header(5);      
    
        PTR_ADDR=(char *)&WM_SETTINGS_MENU_LIST[h_menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Boarder();
        
        PTR_ADDR=(char *)&WM_SETTINGS_MENU_DESC_LIST[h_menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,19);
        Disp_Msg(LINE5);

        h_menu_cnt=Function_Selected(wm_fun_cnt,h_menu_cnt);
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            WM_Settings_SubMenu(h_menu_cnt);
            TIME_FLAG=1;
        }
    }
}

void Print_Col_Field_Settings(void)
{
    UINT8 h_menu_cnt=0;
    
    while(h_menu_cnt!=0xFF)
    {
        Display_Box_Line4();

        STRCPY(LBUF,"  *RECEIPT FIELDS*");
        Disp_Msg(LINE1);

        Back_Select();
        
       
        PTR_ADDR=(char *)&RECEIPT_FIELDS_MENU_LIST[h_menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,12);
        Disp_Msg_Boarder();

        STRCPY(LBUF,"Print in Receipt-Y/N");
        Disp_Msg(LINE5);
   
        h_menu_cnt=Function_Selected(9,h_menu_cnt);
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            ON_OFF(10+h_menu_cnt);
            TIME_FLAG=1;            
        }
    }
}

/*void Col_Manual_Msg_Symbol(void)
{
    UINT8 manual_symbol_char;

    Display_Screen_Boarder();
    
    STRCPY(LBUF," *MANUAL SYMBOL*");
    Disp_Header_No_Boarder();
     
    Display_Box_Top_Boarder(LINE4,0x18);
    Display_Bar_Big_T(LINE4,13,0x8A);

    SEERAMReadString(Col_Manual_Msg_Symbol_Loc,1,(char *)&manual_symbol_char);

    STRCPY(LBUF," ENTER SYMBOL|  ");
    LBUF[0]=0x83;
    LBUF[15]=0x85;
    LBUF[14]=manual_symbol_char;
    NOR_DISPLAY_LAST_LINE=0xC0;
    Disp_Msg_Big(0,LINE5);  

    GET_VALUE_STATUS=Get_String_Big(1,100,LINE5+14,(char *)&manual_symbol_char);
    Reset_Display_Values();
    
    if(GET_VALUE_STATUS==0xFF)
        return;

    else
    {
        SEERAMWriteString(Col_Manual_Msg_Symbol_Loc,1,(char *)&manual_symbol_char);

        Saved_Message();      
    }
}*/

void Printer_Settings_SubMenu(UINT8 printercnt)
{
    if(printercnt==0)
        ON_OFF('A');

    if(printercnt==1)
        Ext_Printer_Type();

    if(printercnt==2)
        ON_OFF('H');

    if(printercnt==3)
        ON_OFF('P');

    if(printercnt==4)
        Check_Printer_Status();

    if(printercnt==5)
        Print_Col_Field_Settings();    
    
    if(printercnt==6)
        Input_Command('M');//Col_Manual_Msg_Symbol();
}

void Printer_Settings(void)
{
    UINT8 h_menu_cnt=0;

    while(h_menu_cnt!=0xFF)
    {        
        Settings_SubMenu_Header(4);
       
        PTR_ADDR=(char *)&PRINTER_SETTINGS_MENU_LIST[h_menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Boarder();

        PTR_ADDR=(char *)&PRINTER_SETTINGS_MENU_DESC_LIST[h_menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,20);
        Disp_Msg(LINE5);

        h_menu_cnt=Function_Selected(7,h_menu_cnt);
        
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            Printer_Settings_SubMenu(h_menu_cnt);
            TIME_FLAG=1;
        }
    }
}

void Restart_DPU(void)
{
    Display_Full_Boarder();
            
    STRCPY(LBUF,"  Restart DPU ");
    Disp_Msg_No_Spaces(0,LINE4+1);

    while(1)
        Buzzer(250);
}

void Settings_SubMenu_Header(UINT8 submenucnt)
{
    Display_Box_Line4();
    
    PTR_ADDR=(char *)&SETTINGS_SUBMENU_LIST[submenucnt][0];
    strncpy(LBUF,PTR_ADDR,20);
    Disp_Msg(LINE1);
    
    Back_Select();
}

void Settings_SubMenu(UINT8 settingscnt)
{
    if(settingscnt=='A')
        Current_Settings();
    
    if(settingscnt=='B')
        Collection_Server_Settings();
    
    if(settingscnt=='C')
        Collection_Settings();
   
    if(settingscnt=='D')
        Analyzer_Settings();
          
    if(settingscnt=='E')
        Printer_Settings();
   
    if(settingscnt=='F')
        WM_Settings();
    
    if(settingscnt=='G')
        Admin_Settings();
   
    if(settingscnt=='H')
    {
        BACKEND_FLAG='W';
        GPRS_Settings();
        BACKEND_FLAG='B';
    }
    
    if(settingscnt=='I')
    {
        Master_Settings('D');            
        Restart_DPU();
        return;
    }
    if(settingscnt=='J')
        Master_Settings('S');
}

void Select_Function_Menu(UINT8 sfuntype,UINT8 u_cnt)
{    
    UINT32 usr_tmp_addr;    
    
    TIME_FLAG=1;
    
    Display_Box_Line4();
    
    if(sfuntype=='A')
        STRCPY(LBUF," *USER ACCESS MENU*");
    if(sfuntype=='M')
        STRCPY(LBUF,"    *MAIN  MENU*");
    if(sfuntype=='D')
        STRCPY(LBUF,"    *DATA ENTRY*");
    if(sfuntype=='C')
        STRCPY(LBUF,"  *DATA  TRANSFER*");
    if(sfuntype=='S')
        STRCPY(LBUF,"     *SETTINGS*");
    if(sfuntype=='m')
         STRCPY(LBUF," *MEMORY FUNCTIONS*");
    if(sfuntype=='T')
        STRCPY(LBUF," *INTERNAL PRINTER*");
    if(sfuntype=='E')
        STRCPY(LBUF," *EXTERNAL PRINTER*");
    if(sfuntype=='V')
        STRCPY(LBUF,"   *VIEW REPORTS*");
    
    Disp_Msg(LINE1);
        
    Back_Select();
 
    User_Selected_Menu_List(sfuntype);

    usr_tmp_addr=u_cnt*16;
    strncpy(LBUF,MENU_SELECT_BUF+usr_tmp_addr,16); 
    Disp_Msg_Boarder();
    
    usr_tmp_addr=u_cnt*20;
    strncpy(LBUF,MENU_SELECT_DESC_BUF+usr_tmp_addr,20);
    Disp_Msg(LINE5);
        
}


void Settings(void)
{
    UINT8 set_count=0,select_fun;
    
    if(LOGIN_TYPE!='O')
    {
        if(ON_LINE=='Y')
            ADMIN_SETTINGS_MENU_ACCESS[7]=1;
        
        if(LOGIN_TYPE=='D')
            ADMIN_SETTINGS_MENU_ACCESS[8]=1;
        
        if(LOGIN_TYPE=='M')
            ADMIN_SETTINGS_MENU_ACCESS[9]=1;     
    }
      
    SETTINGS_MENU_FUN_CNT=User_Selected_Menu_List('S');
    
    
    
    while(1)
    {        
        Select_Function_Menu('S',set_count);
        
        if(set_count>0)
            Back_SubMenu();
        
        set_count=Function_Selected(SETTINGS_MENU_FUN_CNT,set_count);        
               
        if(KBV==KB_CANCEL)
            return;
        
        if(KBV==KB_ENT)
        {
            select_fun=SELECT_FUN_CODE[set_count];
            Settings_SubMenu(select_fun);
        }
    }
}

UINT8 Password_Entry1(UINT8 logintype)
{
    UINT8 p_step=0,login_password;
    UINT16 operator_password,admin_password;
    UINT32 m_pword,master_pword,dealer_pword;
    
    SEERAMReadString(Power_On_Password_Loc,1,(char *)&login_password);
    
    if(login_password=='N')
    {
        LOGIN_TYPE='A';
         return 1;
    }
       
    
    while(1)
    {
        switch(p_step)
        {
            case 0:

                Menu_Header('x',0);
              
                STRCPY(LBUF,"ESC to exit...");
                Disp_Msg_No_Spaces(0,LINE5+1);
                ZERO=1;
                Hex_DEC(0,0,Temp_Arr,8);

                DISPCHAR_TYPE='P';                
                GET_VALUE_STATUS=Get_Numeric_Big(8,0,LINE3+1,(char *)&Temp_Arr[0]);
                DISPCHAR_TYPE='N';

                if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                    return 0;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    m_pword=atol(Temp_Arr);
                    
                   
                    p_step=1;
                }

                break;

            case 1:
               
                SEERAMReadString(Operator_Password_Loc,2,(char *)&operator_password);
                
                if(operator_password>9999)
                    operator_password=M_USER_LOG_PASSWORD;
                
                SEERAMReadString(Admin_Password_Loc,2,(char *)&admin_password);
                
                if(admin_password>9999)
                    admin_password=M_LOG_PASSWORD;

                Buzzer(250);
                
                if(DEVICE_CODE<0 || DEVICE_CODE>9999)
                    DEVICE_CODE=1;
                
                master_pword=M_MASTER_PASSWORD+(DEVICE_CODE*M_MASTER_MULTIPLY_BY);
                dealer_pword=M_DEALER_PASSWORD+(DEVICE_CODE*M_DEALER_MULTIPLY_BY);
               
                if(m_pword==dealer_pword)
                {
                    LOGIN_TYPE='D';
                }

                else if(m_pword==master_pword)
                {
                    LOGIN_TYPE='M';
                }   

                else if(m_pword==admin_password)
                {
                    LOGIN_TYPE='A';
                }
                else if(m_pword==operator_password)
                {
                    LOGIN_TYPE='O';
                }
                else
                {
                    p_step=2; 
                    break;
                }
                
                if(logintype=='B' || logintype==LOGIN_TYPE)
                {
                    return 1;
                }
                else
                {
                    LOGIN_TYPE=logintype;
                    p_step=2; 
                    break;
                }
                
                break;
                
                

            case 2:

                Display_Boarder3();
              
                STRCPY(LBUF,"  Incorrect LOGIN");
                
                if(LOGIN_TYPE=='A')
                    STRCPY(LBUF,"   Incorrect ADMIN");
                Disp_Msg(LINE4);
                 
                STRCPY(LBUF,"    Password...!");
                Disp_Msg(LINE6);

                Buzzer(250);
                Buzzer(250);
                Buzzer(250);

                p_step=0;
                
                break;
        }
    }
}



