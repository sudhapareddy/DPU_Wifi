void Buf_Type(UINT8 milktype,char *tbuf)
{
    if(milktype=='B' || milktype=='b')
    {
        tbuf[0]='B';
        tbuf[1]='U';
        tbuf[2]='F';
    }
}

void FAT_Limits(UINT8 fmilktype)
{
    UINT8 l_step=0;

    while(1)
    {
        switch(l_step)
        {
            case 0:

                Read_FAT_Limits(fmilktype);

                Display_Screen_Boarder();

                STRCPY(LBUF,"*COW FAT LIMITS*");
                Buf_Type(fmilktype,LBUF+1);
                Disp_Header_Boarder();

                Display_Limits(0,Fatlimitsrec.MIN_FAT,Fatlimitsrec.MAX_FAT,LINE4);
                Hex_DEC(1,Fatlimitsrec.MIN_FAT,Temp_Arr,4);
                Shift_Left(Temp_Arr,4);
                Temp_Arr[4]='\0';

                GET_VALUE_STATUS=Get_Numeric_Big(4,1,LINE4+2,(char *)&Temp_Arr);
                Reset_Display_Values();

                if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    Fatlimitsrec.MIN_FAT=Float_Long(Temp_Arr);

                    if(Fatlimitsrec.MIN_FAT>=0)
                        l_step=1;
                }

                break;

            case 1:

                Display_Limits(0,Fatlimitsrec.MIN_FAT,Fatlimitsrec.MAX_FAT,LINE4);
                Hex_DEC(1,Fatlimitsrec.MAX_FAT,Temp_Arr,4);
                Shift_Left(Temp_Arr,4);
                //Temp_Arr[4]='\0';

                GET_VALUE_STATUS=Get_Numeric_Big(4,1,LINE4+10,(char *)&Temp_Arr);
                Reset_Display_Values();
                if(KBV==KB_LEFT)
                {
                    l_step=0;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    Fatlimitsrec.MAX_FAT=Float_Long(Temp_Arr);

                    if(Fatlimitsrec.MAX_FAT>=Fatlimitsrec.MIN_FAT)
                    {
                        if((Fatlimitsrec.MAX_FAT-Fatlimitsrec.MIN_FAT)<=120)//150
                        {
                            SEERAMWriteString(EEPROM_ADDR,FAT_Limits_Length,(char *)&Fatlimitsrec);
                            STRCPY(LBUF,"   LIMITS UPDATED");
                            Disp_Msg(LINE7);
                            Delay_Sec(1);
                            return;
                        }
                    }
                }

                break;
        }
    }
}

void FAT_Equation_Based(UINT8 fmilktype)
{
    UINT8 r_off_flag,go_back;
    UINT32 f_offset_address;

    Temp_SNF=0;
    Temp_FAT=0;

    Read_FAT_Limits(fmilktype);

    SF=Fatlimitsrec.MIN_FAT;
    EF=Fatlimitsrec.MAX_FAT;

    SEERAMReadString(Round_Off_Loc,1,(char *)&r_off_flag);
    
    if(EFFECT_RATE==1)
    {
        if(fmilktype=='C')
            f_offset_address=512;

        else
            f_offset_address=768;
    }
    else
    {
        if(fmilktype=='C')
           f_offset_address=0;
       else
           f_offset_address=256;
    }
    
    FLASHReadString(Cow_FatTable_Loc,4096,(char *)&Flash_Buffer);
    TS_Rate=10000;
    for(Temp_FAT=Fatlimitsrec.MIN_FAT;Temp_FAT<=Fatlimitsrec.MAX_FAT;)
    {
        SF=Temp_FAT;
        EF=Fatlimitsrec.MAX_FAT;

        Display_Screen_Boarder();

        STRCPY(LBUF,">COW FAT FORMULA");
        Buf_Type(fmilktype,LBUF+1);
        Disp_Header_No_Boarder();
      
        do
        {
            if(EF>Fatlimitsrec.MAX_FAT || EF<Temp_FAT)
                EF=Fatlimitsrec.MAX_FAT;
            
            Display_Start_End(0,SF,EF,LINE4);

            GET_VALUE_STATUS=Get_Numeric(4,1,LINE5+13,(char *)&Temp_Arr);

            if(KBV==KB_LEFT)
                EF=0;
            if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                return;

            else if(GET_VALUE_STATUS!=0xFF)
                EF=Float_Long(Temp_Arr);
            else
               EF=0; 

        }while(EF>Fatlimitsrec.MAX_FAT ||  EF<Temp_FAT);

        go_back=0;
        
        go_back=Enter_TS_Rate('F',LINE7);
        
        if(go_back==0xFF)
            return;
             
        if(go_back==0)
        {
            if(Update_Confirm()==0x01)//1
            {
                Temp_FAT=EF;
                
                STRCPY(LBUF,"TS Rate(10%):");
                Hex_DEC(2,TS_Rate,LBUF+13,7);
                Shift_Left(LBUF+13,7);
                Disp_Msg(LINE3);

                for(MILK_FAT=SF;MILK_FAT<=EF;MILK_FAT++)
                {
                    CUM_AMOUNT=MILK_FAT*TS_Rate;

                    CUM_AMOUNT=CUM_AMOUNT/100;

                    if(r_off_flag=='Y')
                    {
                        Round_Amt=CUM_AMOUNT%10;

                        if(Round_Amt<5)
                            CUM_AMOUNT=CUM_AMOUNT-Round_Amt;
                        else
                        {
                            Round_Amt=10-Round_Amt;
                            CUM_AMOUNT=CUM_AMOUNT+Round_Amt;
                        }
                    }

                    Milkrec.RATE=CUM_AMOUNT;

                    STRCPY(LBUF,"FAT:            ");
                    Hex_DEC(1,MILK_FAT,LBUF+4,4);
                    Disp_Msg(LINE5);

                    STRCPY(LBUF,"MILK RATE:");
                    Hex_DEC(2,Milkrec.RATE,LBUF+10,6);
                    Disp_Msg(LINE7);
                    STRNCPY((char *)&Flash_Buffer[f_offset_address+((MILK_FAT-Fatlimitsrec.MIN_FAT)*Fat_Record_Length)],(char *)&Milkrec.RATE,2);
                }
                Update_FAT_FLASH();//3
               
                Delay_Sec(1);
                Temp_FAT=Temp_FAT+1;
            }            
        }
    }
}

void Display_Table_Rate(UINT8 dline)
{
    STRCPY(LBUF,"RATE|Rs.");
    //LBUF[4]=0x84;
    Hex_DEC(2,Milkrec.RATE,LBUF+8,6);   
    LBUF[14]=0x00;
    Shift_Left(LBUF+8,6);
    NOR_DISPLAY_LAST_LINE=0x60;
    Disp_Msg_No_Spaces(0,dline+1);
    NOR_DISPLAY_LAST_LINE=0x00;
}


void Enter_Fat_Table(UINT8 milktype)
{
    UINT8 f_cnt,f_step=0;

    Import_Export_Header(milktype);
  
    Read_FAT_Limits(milktype);

    f_cnt=Fatlimitsrec.MIN_FAT;

    while(1)
    {
        switch(f_step)
        {
            case 0:
                
                Read_FAT_MilkTable(milktype,(f_cnt-Fatlimitsrec.MIN_FAT));
               
                Display_Fat_Rate_Values(f_cnt,Milkrec.RATE);
              
                STRCPY(LBUF,"EDIT|Fat:  |Rate:   ");
                //LBUF[4]=0x90;
                LBUF[9]=0x7F;  
               // LBUF[11]=0x90; 
                LBUF[17]=0x82;
                Invert_Disp_Msg(LINE7);
                
                STRCPY(LBUF,"View:  ,   |Esc:Exit");     
               
                LBUF[6]=0x80;
                LBUF[8]=0x81;    
               // LBUF[11]=0x90;  
                Invert_Disp_Msg(LINE8);
                
                KBV=Wait_Key_Press('C');//e

                switch(KBV)
                {
                    case KB_CANCEL:
                        return;

                    case KB_DOWN:

                        f_cnt++;

                        if(f_cnt==Fatlimitsrec.MAX_FAT+1)
                            f_cnt=Fatlimitsrec.MIN_FAT;

                        break;

                    case KB_UP:

                        if(f_cnt==Fatlimitsrec.MIN_FAT)
                            f_cnt=Fatlimitsrec.MAX_FAT+1;

                        f_cnt--;

                        break;
                        
                    case KB_LEFT:
                        
                        if(DPUcontrolrec.RATE_TABLES=='Y')
                            f_step=1;
                        break;

                    case KB_ENT:

                        if(DPUcontrolrec.RATE_TABLES=='Y')
                            f_step=2;
                        break;
                }

                break;
                
            case 1:
                
                Display_Fat_Rate_Values(0,0);
                Display_Box_Boarder(LINE7);
                
                Hex_DEC(1,0,Temp_Arr,4);
                Shift_Left(Temp_Arr,4);
                Temp_Arr[4]='\0';
                
                NOR_DISPLAY_LAST_LINE=0x60;              
                GET_VALUE_STATUS=Get_Numeric_Big(4,1,LINE5+1,(char *)&Temp_Arr);
                NOR_DISPLAY_LAST_LINE=0x00;
                if(KBV==KB_LEFT)
                    f_step=0;

                else if(GET_VALUE_STATUS==0xFF)
                    return;
                else if(GET_VALUE_STATUS!=0xFF)
                {
                    f_cnt=Float_Long(Temp_Arr);
                    
                    if(f_cnt>=Fatlimitsrec.MIN_FAT && f_cnt<=Fatlimitsrec.MAX_FAT)
                    {
                        Read_FAT_MilkTable(milktype,(f_cnt-Fatlimitsrec.MIN_FAT));
                        f_step=2;
                    }
                }
                break;

            case 2:

               // Display_Fat_Rate(f_cnt,1);
                Display_Fat_Rate_Values(f_cnt,Milkrec.RATE);
                CLEAR_DATA_LINES_FROM(7,7);
                Save_Exit_Msg();

                Hex_DEC(2,Milkrec.RATE,Temp_Arr,6);
                Shift_Left(Temp_Arr,6);
                Temp_Arr[6]='\0';
                NOR_DISPLAY_LAST_LINE=0x60;
                GET_VALUE_STATUS=Get_Numeric_Big(6,2,LINE5+8,(char *)&Temp_Arr);
                NOR_DISPLAY_LAST_LINE=0x00;
                if(KBV==KB_LEFT)
                    f_step=0;

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    Milkrec.RATE=Float_Long(Temp_Arr);

                    Please_Wait_Msg_Small(LINE7);
                    Write_FAT_MilkTable(milktype,f_cnt-Fatlimitsrec.MIN_FAT);

                    f_cnt++;

                    if(f_cnt==Fatlimitsrec.MAX_FAT+1)
                        f_cnt=Fatlimitsrec.MIN_FAT;

                    f_step=0;
                }

                break;
        }
    }
}




void FatRates_Submenu(UINT8 fcnt,UINT8 fmilktype)
{
    if(fcnt==0)
        FAT_Limits(fmilktype);
    
    if(fcnt==1)
        FAT_Equation_Based(fmilktype);
    
    if(EFFECT_RATE==1)
    {
        if(fcnt==2)
        {
            TIME_RIGHT_ARROW='Y';
            SetDate_Time(fmilktype-0x42);
            TIME_RIGHT_ARROW='N';
        }
    }
    else
    {
        if(fcnt==2)
            Enter_Fat_Table(fmilktype);
        
        if(fcnt==3)
            Change_Rate_Table(fmilktype,'F');
    }
}

void Fat_Rates_Table(UINT8 fmilktype)
{
    UINT8 rate_count=0,limit_cnt=4,list_array_cnt;
    
    if(EFFECT_RATE==1)
        limit_cnt=3;
    
    if(DPUcontrolrec.RATE_TABLES=='N')
    {
        rate_count=2;
    }

    while(rate_count!=0xFF)
    {
        Display_Box_Line4();
      
        STRCPY(LBUF,"   *COW FAT RATES*");
        Buf_Type(fmilktype,LBUF+4);
        Disp_Msg(LINE1);

        Back_Select();
        
        Disp_Effect_Msg();
               
     
        list_array_cnt=rate_count;
        if(EFFECT_RATE==1)
        {
            if(rate_count==2)
                list_array_cnt=4;
        }
        
        PTR_ADDR=(char *)&FAT_RATES_MENU_LIST[list_array_cnt][0];   
        strncpy(LBUF,PTR_ADDR,16);
        if(DPUcontrolrec.RATE_TABLES=='N')
        {
            LBUF[1]='1';
        }
        
        Disp_Msg_Boarder();
       
        PTR_ADDR=(char *)&FAT_RATES_MENU_DESC_LIST[list_array_cnt][0]; 
        strncpy(LBUF,PTR_ADDR,19);
        Disp_Msg(LINE5);

        rate_count=Function_Selected(limit_cnt,rate_count);        
         
        if(DPUcontrolrec.RATE_TABLES=='N')
        {
            rate_count=rate_count==0xFF ? 0xFF : 2;
        }
        
        if(KBV==KB_ENT)
        {
            FatRates_Submenu(rate_count,fmilktype);
        }
    }
}

void Fat_Rates(void)
{
    UINT8 table_count=0;

    while(table_count!=0xFF)
    {
        Rate_Tables_Header('F');
        
        Disp_Effect_Msg();
       
        PTR_ADDR=(char *)&FAT_TABLE_NAMES_LIST[table_count][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Boarder();
      
        STRCPY(LBUF,"Limits,Rates,Formula");
        Disp_Msg(LINE5);

        table_count=Function_Selected(2,table_count);
                        
        if(KBV==KB_ENT)
        { 
            Fat_Rates_Table(0x42+table_count);
        }
    }
}

