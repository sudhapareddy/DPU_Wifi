void New_Effective_Dates(void)
{
    UINT8 update_flag=0,e_d_line;

    e_d_line=LINE4;

    for(update_flag=0;update_flag<4;update_flag++)
    {
        if(update_flag==0)
        {
            STRCPY(PBUF,"BUF FAT:");
        }
        if(update_flag==1)
        {
            STRCPY(PBUF,"COW FAT:");
        }
        
        if(update_flag==2)
        {
            STRCPY(PBUF,"BUF SNF:");
            CLR_Enabled(PBUF+4);
        }
        
        if(update_flag==3)
        {
            STRCPY(PBUF,"COW SNF:");
            CLR_Enabled(PBUF+4);
        }
       
        EEPROM_ADDR=New_BufFat_Effect_Date_Loc+(update_flag*3);
        SEERAMReadString(EEPROM_ADDR,3,(char *)&Effectrec);//1

        if(Effectrec.DATE!=0xFF && Effectrec.MONTH!=0xFF && Effectrec.YEAR!=0xFF)
        {
            if(e_d_line==LINE4)
            {
                Display_Screen_Boarder();

                STRCPY(LBUF,"EFFECTIVE DATES:");
                Disp_Header_Boarder();
            }
            
            STRNCPY(LBUF,PBUF,8);
            Hex_DEC(0,Effectrec.DATE,LBUF+8,2);
            Hex_DEC(0,Effectrec.MONTH,LBUF+11,2);
            Hex_DEC(0,Effectrec.YEAR,LBUF+14,2);
            LBUF[10]=LBUF[13]='/';
            Disp_Msg(e_d_line);

            e_d_line=e_d_line+20;
        }
    }
    if(e_d_line>LINE4)
        Delay_Sec(4);
}
void Display_Fat_Rate_Values(UINT8 fatval,UINT16 rateval)
{    
    ZERO=1;
    STRCPY(LBUF," FAT   RATE(Rs)");
    LBUF[0]=0x83;
    LBUF[6]=0x83;
    LBUF[15]=0x85;
    NOR_DISPLAY_LAST_LINE=0x60;
    Disp_Msg_Big(0,LINE3);    
    
    if(fatval>0)
    {
        Hex_DEC(1,fatval,LBUF+1,4);
        Shift_Left(LBUF+1,4);    
    }
    
    LBUF[0]=0x83;
    LBUF[6]=0x83;
    LBUF[15]=0x85;
    if(rateval>0)
    {
        Hex_DEC(2,rateval,LBUF+8,6);   
        Shift_Left(LBUF+8,6);
    }  
    
    Disp_Msg_Big(0,LINE5);
    NOR_DISPLAY_LAST_LINE=0x00;    
}

void Copy_New_Fat_Rates(UINT8 fmilktype)
{
    UINT8 f_cnt;
    UINT32 c_offset_address;
    
    if(fmilktype=='C')
    {
        c_offset_address=0;
    }
    else
    {
        c_offset_address=256;
    }
    
    STRCPY(LBUF,"COW FAT TABLE:");
    Buf_Type(fmilktype,LBUF);
    Disp_Msg(LINE7);

    EFFECT_RATE=1;
    Read_FAT_Limits(fmilktype);

    if((Fatlimitsrec.MAX_FAT-Fatlimitsrec.MIN_FAT)>120)
        Fatlimitsrec.MAX_FAT=Fatlimitsrec.MIN_FAT+120;
    
    EFFECT_RATE=0;
    Write_FAT_Limits(fmilktype);

    f_cnt=Fatlimitsrec.MIN_FAT;
    
    FLASHReadString(Cow_FatTable_Loc,4096,(char *)&Flash_Buffer);//1024

    for(;f_cnt<=Fatlimitsrec.MAX_FAT;f_cnt++)
    {
        
        if((Fatlimitsrec.MAX_FAT-Fatlimitsrec.MIN_FAT)>120)
            break;
            
        EFFECT_RATE=1;

        Read_FAT_MilkTable(fmilktype,(f_cnt-Fatlimitsrec.MIN_FAT));

        Display_Fat_Rate_Values(f_cnt,Milkrec.RATE);

        EFFECT_RATE=0;
        STRNCPY((char *)&Flash_Buffer[c_offset_address+((f_cnt-Fatlimitsrec.MIN_FAT)*Fat_Record_Length)],(char *)&Milkrec.RATE,2);
        
    }
    Update_FAT_FLASH();//2
}

void Display_Table_Fat_Snf(UINT8 startflag,UINT16 fatcnt,UINT16 snfcnt,UINT16 ratecnt,UINT8 dline)
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

void Copy_New_SNF_Rates(UINT8 smilktype)
{
    UINT16 fat_cnt,snf_cnt;
    UINT8 p_total_slabs,start_flag=0;
    UINT16 snf_record_length;
    UINT32 no_of_records;
    
    EFFECT_RATE=1;
    Read_SNF_Limits(smilktype);
    
    EFFECT_RATE=0;
    Write_SNF_Limits(smilktype);
    
    Tablerec.MILK_TYPE=smilktype;
    
    EFFECT_RATE=0;
    Erase_SNF_Table(smilktype);

    fat_cnt=SNFLimitsrec.MIN_FAT;
    snf_cnt=SNFLimitsrec.MIN_SNF;
    
    
    EFFECT_RATE=1;
    no_of_records=Read_New_SNF_MilkTable(smilktype);
    
    EFFECT_RATE=0;
    Write_New_SNF_MilkTable(smilktype,no_of_records);//3

    for(fat_cnt=SNFLimitsrec.MIN_FAT;fat_cnt<=SNFLimitsrec.MAX_FAT;fat_cnt++)
    {        
        if((SNFLimitsrec.MAX_FAT-SNFLimitsrec.MIN_FAT)>120)
            break;
        for(snf_cnt=SNFLimitsrec.MIN_SNF;snf_cnt<=SNFLimitsrec.MAX_SNF;snf_cnt++)
        {
            Tablerec.MILK_FAT=fat_cnt-SNFLimitsrec.MIN_FAT;
            Tablerec.MILK_SNF=snf_cnt-SNFLimitsrec.MIN_SNF;
            Tablerec.MILK_TYPE=smilktype;
            
            if((SNFLimitsrec.MAX_SNF-SNFLimitsrec.MIN_SNF)>120)
                break;

            //EFFECT_RATE=1;
            //Tablerec.MILK_RATE=Read_SNF_MilkTable();
            snf_record_length=(SNFLimitsrec.MAX_SNF+1)-SNFLimitsrec.MIN_SNF;
            Tablerec.MILK_RATE = RATE_BUFFER[(Tablerec.MILK_FAT*snf_record_length)+Tablerec.MILK_SNF];

            
            Display_Table_Fat_Snf(start_flag,fat_cnt,snf_cnt,Tablerec.MILK_RATE,LINE3);
            if(start_flag==0)
            {
                if(CLR_BASED=='Y')
                    STRCPY(LBUF,(smilktype=='B'?"   BUF CLR TABLE" : "   COW CLR Table"));
                else
                    STRCPY(LBUF,(smilktype=='B'?"   BUF SNF TABLE" : "   COW SNF Table"));
                Invert_Disp_Msg(LINE8);
                 start_flag=1;
            }
           
            //EFFECT_RATE=0;
            //Write_SNF_MilkTable();//3
        }
    }
    
    EFFECT_RATE=1;
    EEPROM_ADDR=Get_Slabs_Table_Addr(smilktype);
    SEERAMReadString(EEPROM_ADDR,1,(char *)&p_total_slabs);
    
    if(p_total_slabs>0)
    {
        SEERAMReadString(EEPROM_ADDR,((p_total_slabs+1)*Slab_Record_Length),(char *)&TEMP_BUF);
        
        EFFECT_RATE=0;
        EEPROM_ADDR=Get_Slabs_Table_Addr(smilktype);
        
        SEERAMWriteString(EEPROM_ADDR,((p_total_slabs+1)*Slab_Record_Length),(char *)&TEMP_BUF);                
    }
}


void Check_New_RateChart(void)
{
    UINT8 update_flag=0,chart_update=0,c_year,c_month,c_date;

    Read_RTC();

    c_date=HEX_ASCII(Time[4]);
    c_month=HEX_ASCII(Time[5]);
    c_year=HEX_ASCII(Time[6]);

    for(update_flag=0;update_flag<4;update_flag++)
    {        
        EEPROM_ADDR=New_BufFat_Effect_Date_Loc+(update_flag*3);
        SEERAMReadString(EEPROM_ADDR,3,(char *)&Effectrec);

        if(Effectrec.DATE!=0xFF && Effectrec.MONTH!=0xFF && Effectrec.YEAR!=0xFF)
        {
            if((Effectrec.DATE>0 && Effectrec.DATE<=31) && (Effectrec.MONTH>0 && Effectrec.MONTH<=12) && (Effectrec.YEAR>00 && Effectrec.YEAR<=99))
            {
                if((Effectrec.YEAR<c_year) || (Effectrec.YEAR==c_year && Effectrec.MONTH<c_month) || (Effectrec.YEAR==c_year && Effectrec.MONTH==c_month && Effectrec.DATE<=c_date))
                {
                    Display_Screen_Boarder();

                    STRCPY(LBUF,"*NEW RATE CHART*");
                    Disp_Header_No_Boarder();

                    if(update_flag==0)
                    {
                        Copy_New_Fat_Rates('B');
                    }

                    if(update_flag==1)
                    {
                        Copy_New_Fat_Rates('C');                 
                    }

                    if(update_flag==2)
                    {

                        Copy_New_SNF_Rates('B');

                    }

                    if(update_flag==3)
                    {
                        Copy_New_SNF_Rates('C');

                    }

                    EEPROM_ADDR=C_BufFat_Effect_Date_Loc+(update_flag*3);
                    SEERAMWriteString(EEPROM_ADDR,3,(char *)&Effectrec);


                    Effectrec.DATE=Effectrec.MONTH=Effectrec.YEAR=0xFF;
                    EEPROM_ADDR=New_BufFat_Effect_Date_Loc+(update_flag*3);
                    SEERAMWriteString(EEPROM_ADDR,3,(char *)&Effectrec);
                    chart_update=1;
                }
            }
        }
    }
    if(chart_update==1)
    {
        Display_Boarder3();
      
        Table_Updated_Msg();
        Clear_LCD();
    }
}