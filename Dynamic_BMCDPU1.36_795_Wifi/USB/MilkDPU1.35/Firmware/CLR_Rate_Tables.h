
void SNF_FAT_Limits(UINT8 smilktype)
{
    UINT8 l_step=0;

    //ZERO=0;
    Read_SNF_Limits(smilktype);

    while(1)
    {
        switch(l_step)
        {
            case 0:

                Display_Screen_Boarder();

                STRCPY(LBUF,"*COW SNF LIMITS*");
                CLR_Enabled(LBUF+5);
               
                Buf_Type(smilktype,LBUF+1);
                Disp_Header_Boarder();

                Display_Limits(0,SNFLimitsrec.MIN_FAT,SNFLimitsrec.MAX_FAT,LINE4);
                Hex_DEC(1,SNFLimitsrec.MIN_FAT,Temp_Arr,4);
                Shift_Left(Temp_Arr,4);
                //Temp_Arr[4]='\0';

                GET_VALUE_STATUS=Get_Numeric_Big(4,1,LINE4+2,(char *)&Temp_Arr);
                Reset_Display_Values();

                if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    SNFLimitsrec.MIN_FAT=Float_Long(Temp_Arr);

                    if(SNFLimitsrec.MIN_FAT>=0)
                        l_step=1;
                }

                break;

            case 1:

                Display_Limits(0,SNFLimitsrec.MIN_FAT,SNFLimitsrec.MAX_FAT,LINE4);
                Hex_DEC(1,SNFLimitsrec.MAX_FAT,Temp_Arr,4);
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
                    SNFLimitsrec.MAX_FAT=Float_Long(Temp_Arr);

                    if(SNFLimitsrec.MAX_FAT>=SNFLimitsrec.MIN_FAT)
                    {
                        if((SNFLimitsrec.MAX_FAT-SNFLimitsrec.MIN_FAT)<=120)
                            l_step=2;
                    }
                }

                break;

            case 2:

                Display_Limits(0,SNFLimitsrec.MIN_FAT,SNFLimitsrec.MAX_FAT,LINE4);
                Display_Limits(1,SNFLimitsrec.MIN_SNF,SNFLimitsrec.MAX_SNF,LINE7);
                Hex_DEC(1,SNFLimitsrec.MIN_SNF,Temp_Arr,4);
                Shift_Left(Temp_Arr,4);
                //Temp_Arr[4]='\0';

                GET_VALUE_STATUS=Get_Numeric_Big(4,1,LINE7+2,(char *)&Temp_Arr);
                Reset_Display_Values();

                if(KBV==KB_LEFT)
                {
                    l_step=1;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    SNFLimitsrec.MIN_SNF=Float_Long(Temp_Arr);

                    if(SNFLimitsrec.MIN_SNF>=0)
                        l_step=3;
                }

                break;

            case 3:

                Display_Limits(1,SNFLimitsrec.MIN_SNF,SNFLimitsrec.MAX_SNF,LINE7);
                Hex_DEC(1,SNFLimitsrec.MAX_SNF,Temp_Arr,4);
                Shift_Left(Temp_Arr,4);
                //Temp_Arr[4]='\0';

                GET_VALUE_STATUS=Get_Numeric_Big(4,1,LINE7+10,(char *)&Temp_Arr);
                Reset_Display_Values();
                if(KBV==KB_LEFT)
                {
                    l_step=2;
                    break;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    SNFLimitsrec.MAX_SNF=Float_Long(Temp_Arr);

                    if(SNFLimitsrec.MAX_SNF>=SNFLimitsrec.MIN_SNF)
                    {
                        if((SNFLimitsrec.MAX_SNF-SNFLimitsrec.MIN_SNF)<=120)
                        {
                            SEERAMWriteString(EEPROM_ADDR,SNF_Limits_Length,(char *)&SNFLimitsrec);

                            STRCPY(LBUF,"   LIMITS UPDATED");
                            Disp_Msg(LINE6);
                            Delay_Sec(1);
                            return;
                        }
                    }
                }

                break;
        }
    }
}

void Update_SNF_Rates(UINT8 smilktype)
{
    UINT8 r_off_flag,ap_snf_formula;
    INT32 temp_rate,temp_fat,temp_snf;

    SEERAMReadString(AP_SNF_Formula_Loc,1,(char *)&ap_snf_formula);

    if(ap_snf_formula=='Y' && smilktype=='B')
    {
        temp_fat=MILK_FAT;
        temp_snf=MILK_SNF;

        if(temp_snf<LOW_SNF)
            temp_rate=((temp_fat*TS_Rate*2)/3000);
        else
        {
            if(temp_snf<=STD_SNF)
                temp_rate=((temp_fat*TS_Rate)/1000)-((STD_SNF-temp_snf)*DED_AMT);
            else
                temp_rate=((temp_fat*TS_Rate)/1000)+((temp_snf-STD_SNF)*INC_AMT);
            if(temp_rate<0)
                temp_rate=0;
        }

        CUM_AMOUNT=temp_rate;

    }
    else
    {
        CUM_AMOUNT=(MILK_FAT*TS_RATE_F)+(MILK_SNF*TS_RATE_S);

        CUM_AMOUNT=CUM_AMOUNT/100;

        CUM_AMOUNT=CUM_AMOUNT/10;
    }

    SEERAMReadString(Round_Off_Loc,1,(char *)&r_off_flag);

    if(r_off_flag=='Y')
    {
        Round_Amt=CUM_AMOUNT%10;

        if(Round_Amt>=5)
            CUM_AMOUNT=CUM_AMOUNT+(10-Round_Amt);
        else
            CUM_AMOUNT=CUM_AMOUNT-Round_Amt;
    }
   
    Tablerec.MILK_RATE=CUM_AMOUNT;
    Tablerec.MILK_TYPE=smilktype;
    Tablerec.MILK_FAT=MILK_FAT-SNFLimitsrec.MIN_FAT;
    Tablerec.MILK_SNF=MILK_SNF-SNFLimitsrec.MIN_SNF;
  
    Write_SNF_MilkTable();//4
}

void Update_Slabs(UINT8 slabmilktype,UINT16 updateslabs)
{
    EEPROM_ADDR=Get_Slabs_Table_Addr(slabmilktype);
    SEERAMWriteString(EEPROM_ADDR,((updateslabs+1)*Slab_Record_Length),(char *)&TEMP_BUF);
    Table_Updated_Msg();//1
}

UINT8 Enter_TS_Rate(UINT8 tstype,UINT8 displine)
{
    do
    {
        if(tstype=='F')
            STRCPY(LBUF,"TS Rate(10%):");  
        else
            STRCPY(LBUF,"TS Rate     :");  
        Disp_Msg(displine);
        
        DISPLAY_NO_SPACES=1;
        Hex_DEC(2,TS_Rate,Temp_Arr,7);
        Shift_Left(Temp_Arr,7);
        Temp_Arr[7]='\0';
        STRNCPY(LBUF,Temp_Arr,8);
        Invert_Disp_Msg(displine+13);
        
        DISPLAY_NO_SPACES=0;
        INVERT_DISPLAY=1;
        GET_VALUE_STATUS=Get_Numeric(7,2,displine+13,(char *)&Temp_Arr);
        INVERT_DISPLAY=0;

        if(KBV==KB_LEFT)
        {
            return 1;
        }
        if(GET_VALUE_STATUS==0xFF)
            return 0xFF;

        else if(GET_VALUE_STATUS!=0xFF)
        {
            TS_Rate=Float_Long(Temp_Arr);

            if(TS_Rate>0)
                return 0;
        }
    }while(1);
}


void SNF_Slabs(UINT8 smilktype)
{
    UINT8 Erase_Flag=0,e_slab,go_back;
    UINT16 slab_no=0,rate_change=0,temp_fat;
       
    Temp_SNF=0;
    Temp_FAT=0;

    Read_SNF_Limits(smilktype);
    TS_Rate=10000;
    if(SNFLimitsrec.MIN_FAT>=255 || SNFLimitsrec.MIN_SNF>=255 ||SNFLimitsrec.MAX_FAT>=255 ||SNFLimitsrec.MAX_SNF>=255)
    {
        Set_Limits_Msg();
     
        return;
    }

    SF=SNFLimitsrec.MIN_FAT+SNFLimitsrec.MIN_SNF;
    EF=SNFLimitsrec.MAX_FAT+SNFLimitsrec.MAX_SNF;    
    
    EEPROM_ADDR=Get_Rate_Table_Addr(smilktype);

    for(temp_fat=(SNFLimitsrec.MIN_FAT+SNFLimitsrec.MIN_SNF);temp_fat<=(SNFLimitsrec.MAX_FAT+SNFLimitsrec.MAX_SNF);)
    {
        SF=temp_fat;
        EF=(SNFLimitsrec.MAX_FAT+SNFLimitsrec.MAX_SNF);

        Display_Screen_Boarder();

        STRCPY(LBUF,"*COW SNF SLABS*");
        CLR_Enabled(LBUF+5);
        Buf_Type(smilktype,LBUF+1);
        Disp_Header_No_Boarder();

        STRCPY(LBUF,"START_SF  |   END_SF");
        //LBUF[10]=0x90;
        Invert_Disp_Msg(LINE4);
        
        do
        {
            if(EF>(SNFLimitsrec.MAX_FAT+SNFLimitsrec.MAX_SNF) || EF<temp_fat)
                EF=SNFLimitsrec.MAX_FAT+SNFLimitsrec.MAX_SNF;
            ZERO=1;
            STRCPY(LBUF,"  00.0          ");
            Hex_DEC(1,SF,LBUF+2,4);
            
            Hex_DEC(1,EF,LBUF+14,4);
            Shift_Left(LBUF+14,4);
            
            strncpy(Temp_Arr,LBUF+14,4);
            
            //Hex_DEC(1,EF,Temp_Arr,4);
           // Shift_Left(Temp_Arr,4);
            Disp_Msg(LINE5);
            
            GET_VALUE_STATUS=Get_Numeric(4,1,LINE5+14,(char *)&Temp_Arr);
            if(KBV==KB_LEFT)
                EF=0;

            if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                goto Check_Update1;

            else if(GET_VALUE_STATUS!=0xFF)
                EF=Float_Long(Temp_Arr);

        }while( EF>(SNFLimitsrec.MAX_FAT+SNFLimitsrec.MAX_SNF) ||  EF<temp_fat);

        go_back=0;
        go_back=Enter_TS_Rate('S',LINE7);
        TS_RATE_F=TS_Rate;
        TS_RATE_S=TS_Rate;
        if(go_back==0)
        {
            if(Update_Confirm()==0x01)//2
            {
                temp_fat=EF;

                STRNCPY((char *)&Slabrec.START_FAT,(char *)&SF,2);
                STRNCPY((char *)&Slabrec.START_SNF,(char *)&EF,2);

                Slabrec.FAT_TS_RATE=TS_RATE_F;
                Slabrec.SNF_TS_RATE=TS_RATE_S;
                slab_no++;

                STRNCPY((char *)&TEMP_BUF+(slab_no*Slab_Record_Length),(char *)&Slabrec,Slab_Record_Length);

                Please_Wait_Msg(LINE5);

                if(Erase_Flag==0)
                    Erase_SNF_Table(smilktype);
                Erase_Flag=1;

                for(MILK_FAT=SNFLimitsrec.MIN_FAT;MILK_FAT<=SNFLimitsrec.MAX_FAT;MILK_FAT++)
                {
                    for(MILK_SNF=SNFLimitsrec.MIN_SNF;MILK_SNF<=SNFLimitsrec.MAX_SNF;MILK_SNF++)
                    {
                        if((MILK_FAT+MILK_SNF)<=temp_fat && (MILK_FAT+MILK_SNF)>=SF)
                        {
                            Update_SNF_Rates(smilktype);
                        }
                    }
                }
                
                Temp_FAT=Temp_FAT+1;
                temp_fat=temp_fat+1;
            }  
        }
    }

    Check_Update1:
    if(Erase_Flag==1)
    {
        Display_Boarder3();
        
        e_slab=1;
        TEMP_BUF[0]=slab_no;
        TEMP_BUF[1]=e_slab;
        STRNCPY((char *)&TEMP_BUF+2,(char *)&rate_change,2);
       
        Update_Slabs(smilktype,slab_no);
    }
}

UINT8 Read_AP_Values(UINT8 ss,UINT8 es)
{
    UINT8 step=0;
    
    while(1)
    {
        switch(step)
        {
            case 0:

                CLEAR_DATA_LINES_FROM(3,8);
                STD_SNF=ss;
                LOW_SNF=ss;
                DED_AMT=20;
                STRCPY(LBUF,"STD. SNF:09.0");
                CLR_Enabled(LBUF+5);
                Hex_DEC(1,STD_SNF,LBUF+9,4);
                Disp_Msg(LINE3);
                
                Hex_DEC(1,STD_SNF,Temp_Arr,4);

                GET_VALUE_STATUS=Get_Numeric(4,1,LINE3+9,(char *)&Temp_Arr);

                if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                    return 0;
                else
                {
                    STD_SNF=Float_Long(Temp_Arr);

                    if(STD_SNF>=ss && STD_SNF<=es)
                        step=1;
                }
                break;

            case 1:

                STRCPY(LBUF,"LOW SNF :08.0");
                CLR_Enabled(LBUF+4);
                Hex_DEC(1,LOW_SNF,LBUF+9,4);
                Disp_Msg(LINE5);
                
                Hex_DEC(1,LOW_SNF,Temp_Arr,4);

                GET_VALUE_STATUS=Get_Numeric(4,1,LINE5+9,(char *)&Temp_Arr);

                if(KBV==KB_LEFT)
                    step=0;

                else if(GET_VALUE_STATUS==0xFF)
                    return 0;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    LOW_SNF=Float_Long(Temp_Arr);

                    if(LOW_SNF>=ss && LOW_SNF<=STD_SNF)
                        step=2;
                }
                break;

            case 2:

                STRCPY(LBUF,"DEC. AMT:00.10");
                Hex_DEC(2,DED_AMT,LBUF+9,5);
                Disp_Msg(LINE7);
                
                Hex_DEC(2,DED_AMT,Temp_Arr,5);
                Temp_Arr[5]='\0';

                GET_VALUE_STATUS=Get_Numeric(5,2,LINE7+9,(char *)&Temp_Arr);

                if(KBV==KB_LEFT)
                    step=1;

                else if(GET_VALUE_STATUS==0xFF)
                    return 0;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    DED_AMT=Float_Long(Temp_Arr);
                    INC_AMT=DED_AMT;
                   if( (DED_AMT*(STD_SNF - LOW_SNF)) <((TS_Rate*SNFLimitsrec.MIN_FAT)/1000))
                       step=3;
                   else
                       return 0;
                }
                break;
            case 3:

                STRCPY(LBUF,"INC. AMT:00.10");
                Hex_DEC(2,INC_AMT,LBUF+9,5);
                Disp_Msg(LINE8);
                
                Hex_DEC(2,INC_AMT,Temp_Arr,5);
                Temp_Arr[5]='\0';

                GET_VALUE_STATUS=Get_Numeric(5,2,LINE8+9,(char *)&Temp_Arr);

                if(KBV==KB_LEFT)
                    step=2;

                else if(GET_VALUE_STATUS==0xFF)
                    return 0;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    INC_AMT=Float_Long(Temp_Arr);

                   if( (INC_AMT*(STD_SNF - LOW_SNF)) <((TS_Rate*SNFLimitsrec.MIN_FAT)/1000))
                        return 1;
                   else
                       return 0;
                }
                break;
        }
    }
}

void SNF_Equation_Based(UINT8 milktype)
{
    UINT8 e_slab,ap_snf_formula,go_back;
    UINT8 Erase_Flag=0,fat_ts_flag,snf_limits_flag;
    UINT16 slab_no=0,rate_change=0;    
    UINT32 slab_update_flag; //no_of_records,   
    
    Temp_SNF=0;
    Temp_FAT=0;
    SEERAMReadString(AP_SNF_Formula_Loc,1,(char *)&ap_snf_formula);

    Read_SNF_Limits(milktype);

    SF=SNFLimitsrec.MIN_FAT;
    EF=SNFLimitsrec.MAX_FAT;
    SS=SNFLimitsrec.MIN_SNF;
    ES=SNFLimitsrec.MAX_SNF;

    if(SF>=255 || EF>=255 ||SS>=255 ||ES>=255)
    {
        Set_Limits_Msg();
     
        return;
    }
   
    EEPROM_ADDR=Get_Rate_Table_Addr(milktype);
    
    //no_of_records=(SNFLimitsrec.MAX_FAT+1-SNFLimitsrec.MIN_FAT)*(SNFLimitsrec.MAX_SNF+1-SNFLimitsrec.MIN_SNF);
    //FLASHReadString(EEPROM_ADDR,no_of_records*2,(char *)&Flash_Buffer);
   
    TS_Rate=10000;
    TS_RATE_F=10000;
    TS_RATE_S=10000;
    for(Temp_FAT=SNFLimitsrec.MIN_FAT;Temp_FAT<=SNFLimitsrec.MAX_FAT;)
    {
        SF=Temp_FAT;
        EF=SNFLimitsrec.MAX_FAT;
        SS=SNFLimitsrec.MIN_SNF;
        ES=SNFLimitsrec.MAX_SNF;        

        Display_Screen_Boarder();

        STRCPY(LBUF,">COW SNF FORMULA");
        Buf_Type(milktype,LBUF+1);
        CLR_Enabled(LBUF+5);
        Disp_Header_No_Boarder();

        do
        {
            if(EF>SNFLimitsrec.MAX_FAT || EF<Temp_FAT)
                EF=SNFLimitsrec.MAX_FAT;   
         
            Display_Start_End(0,SF,EF,LINE3);
            
            GET_VALUE_STATUS=Get_Numeric(4,1,LINE4+13,(char *)&Temp_Arr);
            
            if(KBV==KB_LEFT)
                EF=0;
            
            if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                goto Check_Update;
            
            else if(GET_VALUE_STATUS!=0xFF)
                EF=Float_Long(Temp_Arr);

        }while( EF>SNFLimitsrec.MAX_FAT ||  EF<Temp_FAT);
        
        fat_ts_flag=2;
        for(Temp_SNF=SNFLimitsrec.MIN_SNF;Temp_SNF<SNFLimitsrec.MAX_SNF;)
        {
            slab_update_flag=0;
            
            if(fat_ts_flag==2)
            {
                Display_Boarder3();
                Display_Start_End(0,SF,EF,LINE3);
                TS_Rate=TS_RATE_F;
                fat_ts_flag=Enter_TS_Rate('S',LINE5);

                if(fat_ts_flag==0xFF)
                    return;

                if(fat_ts_flag==1)
                {
                    if(Temp_SNF==SNFLimitsrec.MIN_SNF)
                        break; 
                    fat_ts_flag=2;
                }
            }
            if(fat_ts_flag==0)
            {
                TS_RATE_F=TS_Rate;    
				TS_RATE_S=TS_Rate;

                ES=SNFLimitsrec.MAX_SNF;
                snf_limits_flag=1;

                do
                {
                    if(ES>SNFLimitsrec.MAX_SNF || ES<Temp_SNF)
                        ES=SNFLimitsrec.MAX_SNF;

                    Display_Start_End(1,SS,ES,LINE6);

                    GET_VALUE_STATUS=Get_Numeric(4,1,LINE7+13,(char *)&Temp_Arr);

                    if(KBV==KB_LEFT)
                    {
                        snf_limits_flag=0;
                        fat_ts_flag=2;
                        break;//ES=0;
                    }

                    if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
                        goto Check_Update;

                    else if(GET_VALUE_STATUS!=0xFF)
                        ES=Float_Long(Temp_Arr);

                }while( ES>SNFLimitsrec.MAX_SNF || ES<Temp_SNF);

                if(snf_limits_flag==1)
                {
                    Temp_FAT=EF;
                    
                    Display_Start_End(1,SS,ES,LINE6);
                    go_back=0;
                    TS_Rate=TS_RATE_S;
                    go_back=Enter_TS_Rate('S',LINE8);

                    if(go_back==0xFF)
                        return;
                    if(go_back==1)
                        CLEAR_DATA_LINES_FROM(8,8);

                    if(go_back==0)
                    {
                        TS_RATE_S=TS_Rate;
						TS_RATE_F=TS_Rate;

                        if(ap_snf_formula=='Y' && milktype=='B')
                        {
                            if(Read_AP_Values(SS,ES)==0)
                                goto Check_Update;
                        }

                        if(Update_Confirm()==0x01)//3
                        {
                            Temp_SNF=ES;
                            Slabrec.START_FAT=SF;
                            Slabrec.END_FAT=EF;
                            Slabrec.START_SNF=SS;
                            Slabrec.END_SNF=ES;
                            Slabrec.FAT_TS_RATE=TS_RATE_F;
                            Slabrec.SNF_TS_RATE=TS_RATE_S;

                            slab_no++;
                            STRNCPY((char *)&TEMP_BUF+(slab_no*Slab_Record_Length),(char *)&Slabrec,Slab_Record_Length);

                           Please_Wait_Msg(LINE5);

                            if(Erase_Flag==0)
                                Erase_SNF_Table(milktype);

                            Erase_Flag=1;

                            for(MILK_FAT=SF;MILK_FAT<=EF;MILK_FAT++)
                            {
                                for(MILK_SNF=SS;MILK_SNF<=ES;MILK_SNF++)
                                    Update_SNF_Rates(milktype);
                            }
                            SS=ES+1;
                            Temp_SNF=SS;
                            fat_ts_flag=2;
                            slab_update_flag=1;
                        } 
                        else
                            fat_ts_flag=2;
                    }
                }
            }
        }
        if(slab_update_flag==1)
            Temp_FAT=Temp_FAT+1;
    }

    Check_Update:
    if(Erase_Flag==1)
    {
        Display_Boarder3();
        e_slab=0;
        TEMP_BUF[0]=slab_no;
        TEMP_BUF[1]=e_slab;
        STRNCPY((char *)&TEMP_BUF+2,(char *)&rate_change,2);
        
        Update_Slabs(milktype,slab_no);
       
    }
}


void Display_Table_Fat_Snf(UINT8 startflag,UINT8 fatcnt,UINT8 snfcnt,UINT16 ratecnt,UINT8 dline)
{
    ZERO=1;
  
    if(startflag==0)
    {
        Display_Selected_Boarder(LINE3,0x18);
        Display_Box_Top_Boarder(LINE6,0x18);   
        Display_Box_Boarder(LINE7);        
    }
    
    STRCPY(LBUF,"FAT:00.0  SNF:");
    CLR_Enabled(LBUF+10);
   
    Hex_DEC(1,fatcnt,(char *)&Temp_Arr[0],4);
    Shift_Left(Temp_Arr,4);
    if(fatcnt>0)    
        STRNCPY(LBUF+4,(char *)&Temp_Arr[0],4);
    
    Hex_DEC(1,snfcnt,(char *)&Temp_Arr[0],4);
    Shift_Left(Temp_Arr,4);
    if(snfcnt>0)    
    STRNCPY(LBUF+14,(char *)&Temp_Arr[0],4);        
    
    Disp_Msg(dline);
    
    if(ratecnt!='R')
    {
        STRCPY(LBUF,"RATE(Rs):");
        Hex_DEC(2,ratecnt,(char *)&Temp_Arr[0],6);
        Shift_Left(Temp_Arr,6);
        STRNCPY(LBUF+9,(char *)&Temp_Arr[0],6);
        
    }
    Disp_Msg(dline+40);
    
    Display_Bar(dline+8);
    Display_Bar_Middle_Plus(dline+20+8);
    Display_Bar(dline+40+8);
    Display_Bar_Middle_Up(dline+60+8);
   
}

void SNF_Table_Help(void)
{
    STRCPY(LBUF,"DEL-Select Fat&Snf");
    CLR_Enabled(LBUF+15);               
    Invert_Disp_Msg(LINE7);

    STRCPY(LBUF,"Fat:  |Snf:  |Edit:");
    CLR_Enabled(LBUF+7);
    LBUF[4]=0x80;LBUF[5]=0x81;
    //LBUF[6]=LBUF[13]=0x90;
    LBUF[11]=0x7E;LBUF[12]=0x7F;
    LBUF[19]=0x82;
    Invert_Disp_Msg(LINE8);
}

void Enter_SNF_Table(UINT8 milktype)
{
    UINT8 fat_cnt,snf_cnt,start_flag=0;
    UINT8 s_step=0;

    ZERO=1;

    Read_SNF_Limits(milktype);

    Import_Export_Header(milktype+32);
    
    fat_cnt=SNFLimitsrec.MIN_FAT;
    snf_cnt=SNFLimitsrec.MIN_SNF;

    Tablerec.MILK_TYPE=milktype;    

    while(1)
    {
        switch(s_step)
        {
            case 0:
               
                Tablerec.MILK_FAT=fat_cnt-SNFLimitsrec.MIN_FAT;
                Tablerec.MILK_SNF=snf_cnt-SNFLimitsrec.MIN_SNF;

                Milkrec.RATE=Read_SNF_MilkTable();//1

                Display_Table_Fat_Snf(start_flag,fat_cnt,snf_cnt,Milkrec.RATE,LINE3);      
                
                SNF_Table_Help();
                
                start_flag=1;
                KBV=Wait_Key_Press('C');//e

                switch(KBV)
                {                   
                    case KB_DEL:
                        
                        if(DPUcontrolrec.RATE_TABLES=='Y')
                        {
                            s_step=1;
                            start_flag=0;
                        }
                        break;
                        
                    case KB_CANCEL:

                        return;

                    case KB_DOWN:

                        fat_cnt++;

                        if(fat_cnt==SNFLimitsrec.MAX_FAT+1)
                            fat_cnt=SNFLimitsrec.MIN_FAT;

                        break;

                    case KB_UP:

                        if(fat_cnt==SNFLimitsrec.MIN_FAT)
                            fat_cnt=SNFLimitsrec.MAX_FAT+1;

                        fat_cnt--;

                        break;

                    case KB_RIGHT:

                        snf_cnt++;

                        if(snf_cnt==SNFLimitsrec.MAX_SNF+1)
                            snf_cnt=SNFLimitsrec.MIN_SNF;

                        break;

                    case KB_LEFT:

                        if(snf_cnt==SNFLimitsrec.MIN_SNF)
                            snf_cnt=SNFLimitsrec.MAX_SNF+1;

                        snf_cnt--;

                        break;

                    case KB_ENT:
                        if(DPUcontrolrec.RATE_TABLES=='Y')
                            s_step=3;
                        break;
                }

                break;
                
            case 1:
                
                Display_Table_Fat_Snf(start_flag,0,0,'R',LINE3);
                start_flag=1;
              
                GET_VALUE_STATUS=Get_Numeric(4,1,LINE3+4,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                {                    
                    s_step=0;
                    start_flag=0;
                }

                else if(GET_VALUE_STATUS==0xFF)
                    return;
                else if(GET_VALUE_STATUS!=0xFF)
                {
                    fat_cnt=Float_Long(Temp_Arr);
                    
                    if(fat_cnt>=SNFLimitsrec.MIN_FAT && fat_cnt<=SNFLimitsrec.MAX_FAT)
                    {                        
                        s_step=2;
                    }
                }
                break;

            case 2:
                
                Display_Table_Fat_Snf(start_flag,fat_cnt,0,'R',LINE3);
                                              
                GET_VALUE_STATUS=Get_Numeric(4,1,LINE3+14,(char *)&Temp_Arr);
                
                if(KBV==KB_LEFT)
                    s_step=1;

                else if(GET_VALUE_STATUS==0xFF)
                    return;
                else if(GET_VALUE_STATUS!=0xFF)
                {
                    snf_cnt=Float_Long(Temp_Arr);
                    
                    if(snf_cnt>=SNFLimitsrec.MIN_SNF && snf_cnt<=SNFLimitsrec.MAX_SNF)
                    {                        
                        s_step=3;
                        
                        Tablerec.MILK_FAT=fat_cnt-SNFLimitsrec.MIN_FAT;
                        Tablerec.MILK_SNF=snf_cnt-SNFLimitsrec.MIN_SNF;
                        
                        Milkrec.RATE=Read_SNF_MilkTable();

                        Display_Table_Fat_Snf(start_flag,fat_cnt,snf_cnt,Milkrec.RATE,LINE3);
                    }
                }
                break;


            case 3:

                STRCPY(LBUF,"Back: "); 
                LBUF[5]=0x7F;
                Disp_Msg(LINE7);
                
                Save_Exit_Msg();
                
                Hex_DEC(2,Milkrec.RATE,(char *)&Temp_Arr[0],6);
                Shift_Left(Temp_Arr,6);
                Temp_Arr[6]='\0';

                GET_VALUE_STATUS=Get_Numeric(6,2,LINE5+9,(char *)&Temp_Arr);

                if(KBV==KB_LEFT)
                {
                    s_step=0;
                    start_flag=0;
                    break;
                }
                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    Please_Wait_Msg_Small(LINE7);
                    Tablerec.MILK_RATE=Float_Long(Temp_Arr);

                    Tablerec.MILK_FAT=fat_cnt-SNFLimitsrec.MIN_FAT;
                    Tablerec.MILK_SNF=snf_cnt-SNFLimitsrec.MIN_SNF;

                    Write_SNF_MilkTable();//1
                  
                    snf_cnt++;

                    if(snf_cnt==SNFLimitsrec.MAX_SNF+1)
                    {
                        snf_cnt=SNFLimitsrec.MIN_SNF;

                        fat_cnt++;

                        if(fat_cnt==SNFLimitsrec.MAX_FAT+1)
                            fat_cnt=SNFLimitsrec.MIN_FAT;
                    }

                    s_step=0;
                }

                break;
        }
    }
}

UINT16 Display_Rate_Change_Msg(UINT8 ctype)
{
    UINT16 u_rate_change;
    
    Display_Boarder3();

    SEERAMReadString(EEPROM_ADDR,2,(char *)&u_rate_change);

    if(ctype=='I')
        STRCPY(LBUF,"RATE INCREASED:");
    else
        STRCPY(LBUF,"RATE DECREASED:");

    Hex_DEC(2,u_rate_change,LBUF+15,5);

    Disp_Msg(LINE7);
    
    return u_rate_change;
}

void Change_Rate_Table(UINT8 milktype,UINT8 fatsnf)
{
    UINT16 rate_change,s_rate_change;
    UINT8 change_type;

    while(1)
    {
        if(fatsnf=='S')
            Import_Export_Header(milktype+32);
        else
            Import_Export_Header(milktype);

        STRCPY(LBUF,"Rate Change By?");
        Disp_Msg(LINE3);

        change_type=Select_Yes_No(0,3);

        Display_Half_Box();

        switch(change_type)
        {
            case 0xFF:

                return;

            case 0:

                if(fatsnf=='F')
                {                   
                    EEPROM_ADDR=Buf_FAT_Rate_Inc_Loc+(milktype-0x42)*2;
                }
                if(fatsnf=='S')
                {
                    EEPROM_ADDR=Buf_SNF_Rate_Inc_Loc+(milktype-0x42)*2;
                }
                STRCPY(LBUF,"INCREASE:");
                change_type='I';
                break;

            case 1:

                if(fatsnf=='F')
                {
                    EEPROM_ADDR=Buf_FAT_Rate_Dec_Loc+(milktype-0x42)*2;
                   
                }
                if(fatsnf=='S')
                {
                    EEPROM_ADDR=Buf_SNF_Rate_Dec_Loc+(milktype-0x42)*2;
                  
                }
                STRCPY(LBUF,"DECREASE:");
                change_type='D';
                break;
        }
        
        Hex_DEC(2,0,Temp_Arr,5); 
        Disp_Msg_No_Spaces(0,LINE6+1);

        while(1)
        {
            GET_VALUE_STATUS=Get_Numeric_Big(5,2,LINE6+10,(char *)&Temp_Arr);
            
            if(KBV==KB_LEFT)
                break;

            if(GET_VALUE_STATUS==0xFF)
                return;

            else if(GET_VALUE_STATUS!=0xFF)
            {
                rate_change=Float_Long(Temp_Arr);

                if(rate_change>0)
                {
                    if(Update_Confirm()==0x01)//4
                    {
                        Please_Wait_Msg(LINE5);
                        SEERAMWriteString(EEPROM_ADDR,2,(char *)&rate_change);

                        if(fatsnf=='S')
                        {
                            if(milktype=='B')
                                SEERAMReadString(B_Rate_Change_Loc,2,(char *)&s_rate_change);
                            else
                                SEERAMReadString(C_Rate_Change_Loc,2,(char *)&s_rate_change);

                            if(change_type=='I')
                                s_rate_change=s_rate_change+rate_change;
                            else
                                s_rate_change=s_rate_change-rate_change;

                            if(milktype=='B')
                                SEERAMWriteString(B_Rate_Change_Loc,2,(char *)&s_rate_change);
                            else
                                SEERAMWriteString(C_Rate_Change_Loc,2,(char *)&s_rate_change);
                        }

                        if(fatsnf=='F')
                            Update_Fat_Table(milktype,change_type);//1

                        else if(fatsnf=='S')
                            Update_SNF_Table(milktype,change_type);//1

                        Display_Boarder3();
                        STRCPY(LBUF," RATE TABLE UPDATED");
                    }
                    else
                        STRCPY(LBUF," UPDATION CANCELLED");

                    Disp_Msg(LINE5);

                    Delay_Sec(1);
                    return;
                }
            }
        }
    }
}




void SNF_Functions_SubMenu(UINT8 submenucnt,UINT8 milktype)
{
    if(submenucnt==0)
        SNF_FAT_Limits(milktype);    
    
    if(submenucnt==1)
        FAT_SNF_Steps_Based_New(milktype);
       // SNF_Slabs(milktype);
    
    if(submenucnt==2)
        SNF_Equation_Based(milktype);
    
    if(submenucnt==3)
        Print_Slabs(milktype,'M');  
    
    if(EFFECT_RATE==1)
    {
        if(submenucnt==4)
        {
            TIME_RIGHT_ARROW='Y';
            SetDate_Time(milktype-0x40);
            TIME_RIGHT_ARROW='N';
        }
    }
    else
    {
        if(submenucnt==4)
            Enter_SNF_Table(milktype);
        
        if(submenucnt==5)
            Change_Rate_Table(milktype,'S');   
    }    
}