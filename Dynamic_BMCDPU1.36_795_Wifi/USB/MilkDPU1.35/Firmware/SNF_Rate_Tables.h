
#define MAX_FAT_SLABS 10
#define MAX_SNF_SLABS 10

typedef struct {
    UINT16 START;
    UINT16 END;
    UINT16 INCREMENT;
} StepsSlab;

StepsSlab fatSlabs[MAX_FAT_SLABS];
StepsSlab snfSlabs[MAX_SNF_SLABS];
UINT8 fatSlabCount = 0;
UINT8 snfSlabCount = 0;
UINT32 BASE_RATE = 0;

void Display_Limits(UINT8 dlevel,UINT16 dval1,UINT16 dval2,UINT8 line)
{
    if(dlevel==0)
        Display_Box(line+20);

    NOR_DISPLAY_LAST_LINE=0xC0;    
    ZERO=1;
    if(dlevel==0)
    {
        Display_Box(line+60);

        STRCPY(LBUF," MIN FAT    MAX FAT");
    }
    if(dlevel==1)
    {
        STRCPY(LBUF," MIN SNF    MAX SNF");
        CLR_Enabled(LBUF+5);
        CLR_Enabled(LBUF+16);
    }
    LBUF[9]=0x8F;
    Invert_Disp_Msg(line-20);

    STRCPY(LBUF,"  00.0 |  00.0 ");
    LBUF[0]=0x83;
    LBUF[15]=0x85;
    Hex_DEC(1,dval1,LBUF+2,4);
    Shift_Left(LBUF+2,4);
    Hex_DEC(1,dval2,LBUF+10,4);
    Shift_Left(LBUF+10,4);
    
    DISPLAY_FIRST_LINE=0x01;
    Disp_Msg_Big(0,line);
    Temp_Arr[4]='\0';
}

void Erase_SNF_Table(UINT8 smilktype)
{
    UINT32 e_blockno;

    EEPROM_ADDR=Get_Rate_Table_Addr(smilktype);
   
    for(e_blockno=0;e_blockno<8;e_blockno++)
    {
        Sector_Erase(EEPROM_ADDR+(4096*e_blockno));
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


void Update_SNF_Table(UINT8 smilktype,UINT8 changetype)
{
    UINT16 fat_cnt,snf_cnt;
    UINT16 snf_r_length;
    UINT16 u_rate_change;
    UINT32 no_of_records;
    
    //ZERO=0;

    if(changetype=='I')
        EEPROM_ADDR=Buf_SNF_Rate_Inc_Loc+(smilktype-0x42)*2;
    
    if(changetype=='D')
        EEPROM_ADDR=Buf_SNF_Rate_Dec_Loc+(smilktype-0x42)*2;
    
    u_rate_change=Display_Rate_Change_Msg(changetype);

    Read_SNF_Limits(smilktype);

    if(smilktype=='C')
        EEPROM_ADDR=Cow_SNFTable_Loc;//1
    else
        EEPROM_ADDR=Buf_SNFTable_Loc;
    
//    no_of_records=((SNFLimitsrec.MAX_FAT+1)-SNFLimitsrec.MIN_FAT)*((SNFLimitsrec.MAX_SNF+1)-SNFLimitsrec.MIN_SNF);
//    snf_r_length=((SNFLimitsrec.MAX_SNF+1)-SNFLimitsrec.MIN_SNF)*2;
//    FLASHReadString(EEPROM_ADDR,(no_of_records*2),(char *)&Flash_Buffer);//
    
    no_of_records=((SNFLimitsrec.MAX_FAT+1)-SNFLimitsrec.MIN_FAT)*((SNFLimitsrec.MAX_SNF+1)-SNFLimitsrec.MIN_SNF);
    snf_r_length=(SNFLimitsrec.MAX_SNF+1)-SNFLimitsrec.MIN_SNF;
    FLASHReadString(EEPROM_ADDR,(no_of_records*2),(char *)&RATE_BUFFER);//    

    EFFECT_RATE=0;
    Erase_SNF_Table(smilktype);

    Display_Snf_Limits();

    for(fat_cnt=SNFLimitsrec.MIN_FAT;fat_cnt<=SNFLimitsrec.MAX_FAT;fat_cnt++)
    {
        for(snf_cnt=SNFLimitsrec.MIN_SNF;snf_cnt<=SNFLimitsrec.MAX_SNF;snf_cnt++)
        {  
            Tablerec.MILK_FAT=fat_cnt-SNFLimitsrec.MIN_FAT;
            Tablerec.MILK_SNF=snf_cnt-SNFLimitsrec.MIN_SNF;
            Tablerec.MILK_TYPE=smilktype;

            //STRNCPY((char *)&Milkrec.RATE,(char *)&Flash_Buffer[((Tablerec.MILK_FAT*snf_r_length)+(Tablerec.MILK_SNF*2))],2);//;Read_SNF_MilkTable();
            Milkrec.RATE = RATE_BUFFER[(Tablerec.MILK_FAT*snf_r_length)+Tablerec.MILK_SNF];
            
            if(changetype=='I')
                Milkrec.RATE=Milkrec.RATE+u_rate_change;

            if(changetype=='D')
            {
                if(Milkrec.RATE>=u_rate_change)
                    Milkrec.RATE=Milkrec.RATE-u_rate_change;
                else
                    Milkrec.RATE=0;
            }

            Tablerec.MILK_RATE=Milkrec.RATE;

            Write_SNF_MilkTable();//2
        }
    }
}

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
    if(SNFLimitsrec.MIN_FAT>=65535 || SNFLimitsrec.MIN_SNF>=65535 ||SNFLimitsrec.MAX_FAT>=65535 ||SNFLimitsrec.MAX_SNF>=65535)
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

    if(SF>=65535 || EF>=65535 ||SS>=65535 ||ES>=65535)
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
    UINT16 fat_cnt,snf_cnt;
    UINT8 s_step=0,start_flag=0;

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

void Update_Fat_Table(UINT8 milktype,UINT8 changetype)
{
    UINT8 f_cnt;    
    UINT16 u_rate_change;
    UINT32 u_offset_address;    
    
    if(changetype=='I')
        EEPROM_ADDR=Buf_FAT_Rate_Inc_Loc+(milktype-0x42)*2;
    
    if(changetype=='D')
        EEPROM_ADDR=Buf_FAT_Rate_Dec_Loc+(milktype-0x42)*2;

    u_rate_change=Display_Rate_Change_Msg(changetype);

    Read_FAT_Limits(milktype);

    f_cnt=Fatlimitsrec.MIN_FAT;
    if(milktype=='C')
        u_offset_address=0;
    else
        u_offset_address=256;

   // FLASHReadString(Cow_FatTable_Loc,1024,(char *)&Flash_Buffer);
     FLASHReadString(Cow_FatTable_Loc,4096,(char *)&Flash_Buffer);

    for(;f_cnt<=Fatlimitsrec.MAX_FAT;f_cnt++)
    {
        Read_FAT_MilkTable(milktype,(f_cnt-Fatlimitsrec.MIN_FAT));

        if(changetype=='I')
            Milkrec.RATE=Milkrec.RATE+u_rate_change;

        else if(changetype=='D')
        {
            if(Milkrec.RATE>=u_rate_change)
                Milkrec.RATE=Milkrec.RATE-u_rate_change;
            else
                Milkrec.RATE=0;
        }
      
        Display_Fat_Rate_Values(f_cnt,Milkrec.RATE);
      
        STRNCPY((char *)&Flash_Buffer[u_offset_address+((f_cnt-Fatlimitsrec.MIN_FAT)*Fat_Record_Length)],(char *)&Milkrec.RATE,2);
    }
    
    Update_FAT_FLASH();//1
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

void Print_Slabs_Record(UINT8 p_s_no,UINT16 minVal,UINT16 maxVal,UINT16 rateVal)
{
    if(rateVal>0)
    {
        STRCPY(PBUF,"                         ");
        Hex_DEC(0,p_s_no+1,PBUF,4);
        Hex_DEC(1,minVal,PBUF+9,4);
        Hex_DEC(1,maxVal,PBUF+16,4);
       // ZERO=1;
        Hex_DEC(2,rateVal,PBUF+25,7);
        Printer_SendString(0x03,32);
    }
}

void Print_Slabs(UINT8 pmilktype,UINT8 printtype)
{
    UINT8 p_total_slabs,p_e_slab,underscore_flag=0;
    INT16 p_s_no,p_s_rate_change;
    UINT32 rate_change_addr,total_slabs_addr,equ_slab_addr,print_slabs_addr;
    

    Print_LineNo=0;

    if(EFFECT_RATE==1)
    {
        if(pmilktype=='C')
        {
            total_slabs_addr=Effect_C_Total_Slabs_Loc;
            rate_change_addr=Effect_C_Rate_Change_Loc;            
            equ_slab_addr=Effect_C_Equ_Slab_Loc; 
            print_slabs_addr=Effect_C_Slabs_Loc;
        }
        else
        {
            total_slabs_addr=Effect_B_Total_Slabs_Loc;
            rate_change_addr=Effect_B_Rate_Change_Loc;
            equ_slab_addr=Effect_B_Equ_Slab_Loc; 
            print_slabs_addr=Effect_B_Slabs_Loc;
        }
    }
    else
    {
        if(pmilktype=='C')
        {
            total_slabs_addr=C_Total_Slabs_Loc;
            rate_change_addr=C_Rate_Change_Loc;            
            equ_slab_addr=C_Equ_Slab_Loc; 
            print_slabs_addr=C_Slabs_Loc;
        }
        else
        {
            total_slabs_addr=B_Total_Slabs_Loc;
            rate_change_addr=B_Rate_Change_Loc;
            equ_slab_addr=B_Equ_Slab_Loc;
            print_slabs_addr=B_Slabs_Loc;
        }       
    }

    SEERAMReadString(total_slabs_addr,1,(char *)&p_total_slabs);
    SEERAMReadString(rate_change_addr,2,(char *)&p_s_rate_change);
     
    if(printtype=='M' && (p_total_slabs==0 || p_total_slabs>30))
    {
        CLEAR_DATA_LINES_FROM(3,7);
        STRCPY(LBUF,"  No Slabs... ");
        Disp_Msg_No_Spaces(0,LINE4+1);
        Delay_Sec(1);
        return;
    }

    Start_Printer();

    SEERAMReadString(equ_slab_addr,1,(char *)&p_e_slab);
        
    if(p_e_slab==2 || printtype!='M')
    {
         STRCPY(PBUF,"*FAT & SNF STEPS(COW)*");
        Buf_Type(pmilktype,PBUF+17);
    }
    else
    {
        if(p_e_slab==0)    
        {
            STRCPY(PBUF,"*FAT & SNF FORMULA(COW)*");
            //CLR_Enabled(PBUF+7);
            Buf_Type(pmilktype,PBUF+19);
        }
        else if(p_e_slab==1)
        {
            STRCPY(PBUF,"*FAT + SNF SLABS(COW)*");
            Buf_Type(pmilktype,PBUF+17);
        }   
    }
    CLR_Enabled(PBUF+7);
    Printer_SendString(0x03,27);

    Font_Type(Normal);
    Justification(LEFT);
    PRINT_UNDERSCORE();

    Print_Report_Date_Time();
    Read_SNF_Limits(pmilktype);
    
    if(EFFECT_RATE==1)
        STRCPY(PBUF,"TABLE TYPE:EFFECTIVE TABLE");
    else
        STRCPY(PBUF,"TABLE TYPE:NORMAL TABLE   ");
        Printer_SendString(0x03,26);

    PRINT_UNDERSCORE();
    if(p_e_slab != 0)
    {
        STRCPY(PBUF,"S.NO    START    END        RATE");
        Printer_SendString(0x03,32);
    }
    else
    {
        STRCPY(PBUF,"M_FAT MX_FAT TS_RATE  M_SNF MX_SNF TS_RATE");
        CLR_Enabled(PBUF+24);
        CLR_Enabled(PBUF+31);
        Printer_SendString('S',42);       
    }
    
    PRINT_UNDERSCORE();    
    underscore_flag=0;
     ZERO=1;
     
     
    if(p_e_slab==2 || printtype != 'M')
    {
        if(printtype=='M')
        {
            EEPROM_ADDR=print_slabs_addr;
            SEERAMReadString(EEPROM_ADDR,(p_total_slabs*Slab_Record_Length),(char *)&Slabs);
        }
        else
            p_total_slabs=printtype;
        
        STRCPY(PBUF,"FAT STEPS:");
        Printer_SendString(0x02,10);
        for(p_s_no=0;p_s_no<p_total_slabs;p_s_no++)
            Print_Slabs_Record(p_s_no,Slabs[p_s_no].START_FAT,Slabs[p_s_no].END_FAT,Slabs[p_s_no].FAT_TS_RATE);
        PRINT_UNDERSCORE();
        STRCPY(PBUF,"SNF STEPS:");
        CLR_Enabled(PBUF);

        Printer_SendString(0x02,10);
        for(p_s_no=0;p_s_no<p_total_slabs;p_s_no++)
            Print_Slabs_Record(p_s_no,Slabs[p_s_no].START_SNF,Slabs[p_s_no].END_SNF,Slabs[p_s_no].SNF_TS_RATE);
    }
    else
    {  
        for(p_s_no=0;p_s_no<p_total_slabs;p_s_no++)
        {      

            EEPROM_ADDR=print_slabs_addr+(p_s_no*Slab_Record_Length);

            SEERAMReadString(EEPROM_ADDR,Slab_Record_Length,(char *)&Slabrec);
            //STRNCPY((char *)&p_s_value,(char *)&Slabrec,4);
            //STRNCPY((char *)&p_e_value,(char *)&Slabrec+2,4);
            STRCPY(PBUF,"                         ");
            if(p_e_slab==1)
            {
                Print_Slabs_Record(p_s_no,Slabrec.START_FAT,Slabrec.START_SNF,Slabrec.FAT_TS_RATE);
            }
            else
            {
                //ZERO=0;
                STRCPY(PBUF,"                                          ");

                Hex_DEC(1,Slabrec.START_FAT,PBUF+1,4);
                Hex_DEC(1,Slabrec.END_FAT,PBUF+8,4);
                //ZERO=1;
                Hex_DEC(2,Slabrec.FAT_TS_RATE,PBUF+13,7);

               // ZERO=0;
                Hex_DEC(1,Slabrec.START_SNF,PBUF+23,4);
                Hex_DEC(1,Slabrec.END_SNF,PBUF+30,4);
                //ZERO=1;
                Hex_DEC(2,Slabrec.SNF_TS_RATE,PBUF+35,7);
                Printer_SendString('S',42);

                if(Slabrec.END_SNF==SNFLimitsrec.MAX_SNF)
                {
                    PRINT_UNDERSCORE();  
                    underscore_flag=1;
                }
            }        
        }
    }
    if(underscore_flag==0)
        PRINT_UNDERSCORE();

    if(p_s_rate_change!=0)
    {
        if(p_s_rate_change>0)
        {
            STRCPY(PBUF,"Rate Increased By Rs.");
        }
        else
        {
            p_s_rate_change=(-1)*p_s_rate_change;
            STRCPY(PBUF,"Rate Decreased By Rs.");
        }
        Hex_DEC(2,p_s_rate_change,PBUF+20,5);
        Printer_SendString(0x02,25);
    }
    if(p_e_slab<2)
    {
        STRCPY(PBUF," *Manual Changes not reflected*");
        Printer_SendString(0x02,31);
    }

    Paper_Feed(3);
    
    Init_Regular_Font();
}


void Update_SNF_Rates_New(UINT8 smilktype,UINT16 milkFat,UINT16 milkSnf,UINT32 baseRate)
{
    UINT8 r_off_flag;
    
    
    CUM_AMOUNT = baseRate;

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
    Tablerec.MILK_FAT=(milkFat-SNFLimitsrec.MIN_FAT);
    Tablerec.MILK_SNF=(milkSnf-SNFLimitsrec.MIN_SNF);
  
    Write_SNF_MilkTable();//4
}

UINT16 Enter_Base_Rate(UINT32 baseRate,UINT8 displine)
{
    UINT16 base_Rate;
    
    do
    {
        STRCPY(LBUF," BaseRate:00.00");
        LBUF[0]=0x83;
        Hex_DEC(2,baseRate,LBUF+10,5);
        LBUF[15]=0x85;
        Disp_Msg_Big(0,displine);
        
        DISPLAY_NO_SPACES=1;
        Hex_DEC(2,baseRate,Temp_Arr,5);
        Shift_Left(Temp_Arr,5);
        Temp_Arr[5]='\0';
        STRNCPY(LBUF,Temp_Arr,6);
        Disp_Msg_Big(1,displine+10);
        
        DISPLAY_NO_SPACES=0;
        INVERT_DISPLAY=1;
        GET_VALUE_STATUS=Get_Numeric_Big(5,2,displine+10,(char *)&Temp_Arr);
        INVERT_DISPLAY=0;

        if(GET_VALUE_STATUS==0xFF)
            return 0xFF;

        else if(GET_VALUE_STATUS!=0xFF)
        {
            base_Rate=Float_Long(Temp_Arr);
            
            if(base_Rate > 0)
            return base_Rate;
        }
    }while(1);
}

UINT16 Enter_Rate_Increment(UINT8 displine)
{
    UINT16 inc_rate;
    
    do
    {
        STRCPY(LBUF,"Increment Rate:");
        Disp_Msg(displine);
        
        DISPLAY_NO_SPACES=1;
        Hex_DEC(2,0,Temp_Arr,5);
        Shift_Left(Temp_Arr,5);
        Temp_Arr[5]='\0';
        STRNCPY(LBUF,Temp_Arr,6);
        Invert_Disp_Msg(displine+15);
        
        DISPLAY_NO_SPACES=0;
        INVERT_DISPLAY=1;
        GET_VALUE_STATUS=Get_Numeric(5,2,displine+15,(char *)&Temp_Arr);
        INVERT_DISPLAY=0;

        if(KBV==KB_LEFT)
        {
            return 'L';
        }
        if(GET_VALUE_STATUS==0xFF)
            return 0xFF;

        else if(GET_VALUE_STATUS!=0xFF)
        {
            inc_rate=Float_Long(Temp_Arr);

            //if(inc_rate )
                return inc_rate;
        }
    }while(1);
}

void Generate_From_Slabs(UINT8 milktype) 
{
    UINT8 i, j;
    UINT16 fat, snf;
    UINT16 rate, fat_Add, snf_Add;
    UINT16 fat_base_add = 0;

    Erase_SNF_Table(milktype);

    for (i = 0; i < fatSlabCount; i++) 
    {
        UINT16 fat_Add_total = 0;
        UINT8 fat_inc_point = 1;

        for (fat = fatSlabs[i].START; fat <= fatSlabs[i].END; fat++) 
        {
            if (fat == SNFLimitsrec.MIN_FAT)
                fat_Add = 0;
            else
                fat_Add = fat_inc_point++ * fatSlabs[i].INCREMENT;

            UINT16 snf_base_add = 0;

            for (j = 0; j < snfSlabCount; j++) 
            {
                UINT8 snf_inc_point = 1;
                UINT16 snf_Add_total = 0;

                for (snf = snfSlabs[j].START; snf <= snfSlabs[j].END; snf++) 
                {
                    if (snf == SNFLimitsrec.MIN_SNF)
                        snf_Add = 0;
                    else
                        snf_Add = snf_inc_point++ * snfSlabs[j].INCREMENT;

                    rate = BASE_RATE + fat_base_add + fat_Add + snf_base_add + snf_Add;

                    Update_SNF_Rates_New(milktype, fat, snf, rate);

                    snf_Add_total = snf_Add; // remember last snf_Add
                }

                snf_base_add += snf_Add_total; // Add total of this slab after it finishes
            }
        }

        fat_base_add += fat_Add; // Accumulate fat slab after one group
    }
}


void Display_Header_Slabs(UINT8 milktype)
{
     Display_Screen_Boarder();

    STRCPY(LBUF," *COW SNF STEPS*");
    Buf_Type(milktype,LBUF+2);
    CLR_Enabled(LBUF+6);
    Disp_Header_No_Boarder();   
}

void Update_Slabs(UINT8 slabmilktype,UINT16 updateslabs)
{
    EEPROM_ADDR=Get_Slabs_Table_Addr(slabmilktype);
    SEERAMWriteString(EEPROM_ADDR,((updateslabs+1)*Slab_Record_Length),(char *)&TEMP_BUF);
    Table_Updated_Msg();//1
}

void Display_Start_End(UINT8 dtype,UINT16 start,UINT16 end,UINT8 displine)
{
    if(dtype==0)
        STRCPY(LBUF,"START_FAT |  END_FAT");
    else
    {
        STRCPY(LBUF,"START_SNF |  END_SNF");
        CLR_Enabled(LBUF+6);
        CLR_Enabled(LBUF+17);
    }
   // LBUF[10]=0x90;
    Invert_Disp_Msg(displine);   
    ZERO=1;
    
    STRCPY(LBUF,"   00.0      ");
    Hex_DEC(1,start,LBUF+3,4);
    Shift_Left(LBUF+3,4);
    
    Hex_DEC(1,end,LBUF+13,4);
    Shift_Left(LBUF+13,4);
    strncpy(Temp_Arr,LBUF+13,4);
    Temp_Arr[4]=0x00;
    
    Disp_Msg(displine+20);
}

void FAT_SNF_Steps_Based_New(UINT8 milktype)
{
    UINT8 e_slab=0,slab_Step=0;
    UINT8 Erase_Flag=0;
    UINT16 slab_no=0,rate_change=0;  
    UINT16 no_of_records,fat_inc_rate,snf_inc_rate;
    
    Temp_SNF=0;
    Temp_FAT=0;
    
    Read_SNF_Limits(milktype);

    SF=SNFLimitsrec.MIN_FAT;
    EF=SNFLimitsrec.MAX_FAT;
    SS=SNFLimitsrec.MIN_SNF;
    ES=SNFLimitsrec.MAX_SNF;

    if(SF>=65535 || EF>=65535 ||SS>=65535 ||ES>=65535)
    {
        Set_Limits_Msg();
     
        return;
    }
   
    EEPROM_ADDR=Get_Rate_Table_Addr(milktype);
    
    no_of_records=(SNFLimitsrec.MAX_FAT+1-SNFLimitsrec.MIN_FAT)*(SNFLimitsrec.MAX_SNF+1-SNFLimitsrec.MIN_SNF);
    FLASHReadString(EEPROM_ADDR,no_of_records*2,(char *)&Flash_Buffer);
    
    for(slab_no=0;slab_no<10;slab_no++)
    {
        Slabs[slab_no].START_FAT=0;
        Slabs[slab_no].END_FAT=0;
        Slabs[slab_no].START_SNF=0;
        Slabs[slab_no].END_SNF=0;
        Slabs[slab_no].FAT_TS_RATE=0;
        Slabs[slab_no].SNF_TS_RATE=0;
    }
    
    while(1)
    {       
        switch(slab_Step)
        {            
            case 0:
               
                Display_Header_Slabs(milktype);

                BASE_RATE = Enter_Base_Rate(0,LINE4);

                if(BASE_RATE == 0xFF)
                    return;
                
                slab_Step=1;
                SF=SNFLimitsrec.MIN_FAT;
                EF=0;//SNFLimitsrec.MAX_FAT;
                SS=SNFLimitsrec.MIN_SNF;
                ES=0;
                
                slab_no=0;
                fatSlabCount=0;
                snfSlabCount=0;
                
                break;
                
            case 1:
                
                Display_Header_Slabs(milktype);
                
                if(EF== SNFLimitsrec.MAX_FAT)
                {
                    slab_Step=3;
                    break;
                }
                
                do
                {
                    Display_Start_End(0,SF,0,LINE3);

                    GET_VALUE_STATUS=Get_Numeric(4,1,LINE4+13,(char *)&Temp_Arr);

                    if(KBV==KB_LEFT)
                    {
                        slab_Step=0;
                        break;
                    }
                    else if(GET_VALUE_STATUS == 0xFF)
                        return;

                    if(GET_VALUE_STATUS!=0xFF)
                        EF=Float_Long(Temp_Arr);
                    
                    if(EF >= SF && EF <= SNFLimitsrec.MAX_FAT)
                    {                        
                        slab_Step=2;
                        break;
                    }
                    else
                        EF=0;

                }while( EF>SNFLimitsrec.MAX_FAT ||  EF<Temp_FAT);
                
                
                break;
                
            case 2:                

                fat_inc_rate = Enter_Rate_Increment(LINE6);

                if(fat_inc_rate==0xFF || fat_inc_rate=='L')
                    slab_Step=1;
                else
                {
                    fatSlabs[fatSlabCount].START=SF;
                    fatSlabs[fatSlabCount].END=EF;  
                    fatSlabs[fatSlabCount].INCREMENT=fat_inc_rate;    
                        
                    Slabs[fatSlabCount].START_FAT=SF;
                    Slabs[fatSlabCount].END_FAT=EF; 
                    Slabs[fatSlabCount].FAT_TS_RATE=fat_inc_rate;
                    
                    fatSlabCount++;
                    
                    slab_Step=1;
                    SF=EF+1;
                    

                }
                break;

            case 3:                

                
                Display_Header_Slabs(milktype);
                
                if(ES== SNFLimitsrec.MAX_SNF)
                {
                    slab_Step=5;
                    break;
                }
                    
                do
                {
                    Display_Start_End(1,SS,0,LINE3);

                    GET_VALUE_STATUS=Get_Numeric(4,1,LINE4+13,(char *)&Temp_Arr);

                    if(KBV==KB_LEFT)
                    {
                        slab_Step=2;
                        break;
                    }
                    else if(GET_VALUE_STATUS==0xFF)
                        return;

                    else if(GET_VALUE_STATUS!=0xFF)
                    {
                        ES=Float_Long(Temp_Arr);

                        if(ES>= SS && ES <= SNFLimitsrec.MAX_SNF)
                        {
                            slab_Step=4;
                            
                            
                        }
                        else
                            ES=0;
                    }

                }while( ES>SNFLimitsrec.MAX_SNF || ES<Temp_SNF);

                break;
                    
            case 4:
                                 
                snf_inc_rate=Enter_Rate_Increment(LINE6);

                if(snf_inc_rate=='L' || snf_inc_rate==0xFF)
                    slab_Step=3;
                else
                {
                    snfSlabs[snfSlabCount].START=SS;
                    snfSlabs[snfSlabCount].END=ES;
                    Slabs[snfSlabCount].SNF_TS_RATE=snf_inc_rate;
                            
                    Slabs[snfSlabCount].START_SNF=SS;
                    Slabs[snfSlabCount].END_SNF=ES;
                    snfSlabs[snfSlabCount].INCREMENT=snf_inc_rate;
                    snfSlabCount++;
                    SS=ES+1;
                    slab_Step=3;
                    //slab_Step = (ES== SNFLimitsrec.MAX_SNF)? 5 : 3;

                }
                break;
                
            case 5:
                
                slab_no = fatSlabCount >= snfSlabCount ? fatSlabCount : snfSlabCount;
                Print_Slabs(milktype,slab_no);
    
                if(Update_Confirm()==0x01)//3    
                {
                    STRNCPY((char *)&TEMP_BUF+Slab_Record_Length,(char *)&Slabs,(slab_no*Slab_Record_Length));

                   Please_Wait_Msg(LINE5);                  
                   
                    Generate_From_Slabs(milktype);
                    slab_Step=6;
                     Erase_Flag=1;
                    
                } 
                else
                    slab_Step=0;
                
                break;
                
        case 6:            

            if(Erase_Flag==1)
            {
                Display_Boarder3();
                e_slab=2;
                TEMP_BUF[0]=slab_no;
                TEMP_BUF[1]=e_slab;
                STRNCPY((char *)&TEMP_BUF+2,(char *)&rate_change,2);

                Update_Slabs(milktype,slab_no);

            }
            return;
           
            
        }//switch
     }//while
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
       SNF_Slabs(milktype);
    
    if(submenucnt==4)
        Print_Slabs(milktype,'M');  
    
    if(EFFECT_RATE==1)
    {
        if(submenucnt==5)
        {
            TIME_RIGHT_ARROW='Y';
            SetDate_Time(milktype-0x40);
            TIME_RIGHT_ARROW='N';
        }
    }
    else
    {
        if(submenucnt==5)
            Enter_SNF_Table(milktype);
        
        if(submenucnt==6)
            Change_Rate_Table(milktype,'S');   
    }    
}

void SNF_Rates_Table(UINT8 milktype)
{
    UINT8 snf_count=0,limit_cnt,list_array_cnt;
    
    limit_cnt=7;
    if(EFFECT_RATE==1)
        limit_cnt=6;
    
    if(DPUcontrolrec.RATE_TABLES=='N')
    {
        snf_count=5;
    }
   
    while(snf_count!=0xFF)
    {
        Display_Box_Line4();
      
        STRCPY(LBUF,"   *COW SNF TABLE*");
        Buf_Type(milktype,LBUF+4);
        CLR_Enabled(LBUF+8);
        Disp_Msg(LINE1);

        Back_Select();
        
        Disp_Effect_Msg();
       
             
        list_array_cnt=snf_count;
        if(EFFECT_RATE==1)
        {
            if(snf_count==5)
                list_array_cnt=7;
        }
        PTR_ADDR=(char *)&SNF_RATES_MENU_LIST[list_array_cnt][0]; 
        
        strncpy(LBUF,PTR_ADDR,16);
        if(DPUcontrolrec.RATE_TABLES=='N')
        {
            LBUF[1]='1';
        }
        if(snf_count==1 || snf_count==3)
            CLR_Enabled(LBUF+7);
        Disp_Msg_Boarder();
        
        PTR_ADDR=(char *)&SNF_RATES_MENU_DESC_LIST[list_array_cnt][0]; 
        strncpy(LBUF,PTR_ADDR,19);
        Disp_Msg(LINE5);
        
        snf_count=Function_Selected(limit_cnt,snf_count);
        
        if(DPUcontrolrec.RATE_TABLES=='N')
        {
            snf_count=snf_count==0xFF ? 0xFF : 5;
        }
        
        if(KBV==KB_ENT)
        {          
            SNF_Functions_SubMenu(snf_count,milktype);
        }        
    }
}

void Snf_Rates(void)
{
    UINT8 table_count=0;

    while(table_count!=0xFF)
    {
        Rate_Tables_Header('S');
        
        Disp_Effect_Msg();  
        
        PTR_ADDR=(char *)&SNF_TABLE_NAMES_LIST[table_count][0];
        strncpy(LBUF,PTR_ADDR,16);    
        
        CLR_Enabled(LBUF+7);
        Disp_Msg_Boarder();
        
        STRCPY(LBUF,"Limits,Rates,Formula");
        Disp_Msg(LINE5);
             
        table_count=Function_Selected(2,table_count);
                
        if(KBV==KB_ENT)
        {
            SNF_Rates_Table(0x42+table_count);
        }
    }
}
