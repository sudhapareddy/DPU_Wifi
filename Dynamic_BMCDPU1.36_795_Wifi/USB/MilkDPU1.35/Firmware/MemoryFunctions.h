void Set_Member_Flag(void)                  //Members Upload status to server
{
    UINT8 s_flag;
    
    SEERAMReadString(Members_Server_Flag,1,(char *)&s_flag);
    if(s_flag!=1)
    {
        s_flag=1;
        SEERAMWriteString(Members_Server_Flag,1,(char *)&s_flag);
    }
}
 
 void Clear_Member_Flag(void)
{
    UINT8 s_flag;
    
    SEERAMReadString(Members_Server_Flag,1,(char *)&s_flag);
    if(s_flag!=0xFF)
    {
        s_flag=0xFF;
        SEERAMWriteString(Members_Server_Flag,1,(char *)&s_flag);
    }
}

void Set_Delete_All_Members_Flag(void)                  //Members Upload status to server
{
    UINT8 c_a_flag;
    
    SEERAMReadString(Clear_Memberlist_Flag_Loc,1,(char *)&c_a_flag);
    if(c_a_flag!=1)
    {
        c_a_flag=1;
        SEERAMWriteString(Clear_Memberlist_Flag_Loc,1,(char *)&c_a_flag);
    }
}

void Reset_Delete_All_Members_Flag(void)
{
    UINT8 s_flag;
    
    SEERAMReadString(Clear_Memberlist_Flag_Loc,1,(char *)&s_flag);
    if(s_flag!=0xFF)
    {
        s_flag=0xFF;
        SEERAMWriteString(Clear_Memberlist_Flag_Loc,1,(char *)&s_flag);
    }
}
  
void Read_Settings(void)
{
    SEERAMReadString(Weight_Mode_Loc,11,(char *)&Settingsrec);
    SEERAMReadString(MemberList_Control_Loc,3,(char *)&DPUcontrolrec);

    SEERAMReadString(Accuracy_Loc,6,(char *)&WMrec);
    SEERAMReadString(GPRS_ON_Loc,20,(char *)&GPRSrec);
    SEERAMReadString(Auto_Commission_Loc,Commission_Length,(char *)&Commissionrec);
}

void assign_dates(void)
{
    TSDate=Day_Monthrec.START_DATE;
    TSMonth=Day_Monthrec.START_MONTH;
    TSYear=Day_Monthrec.START_YEAR;
    
    TEYear=Day_Monthrec.END_YEAR;    
    TEMonth=Day_Monthrec.END_MONTH;
    TEDate=Day_Monthrec.END_DATE;
    
    TShift=2;
}

void Edit_Flash_Data(UINT32 taddr,UINT32 noofbytes,char *ebuf)
{
    UINT32 e_temp_addr1,e_temp_addr2;
    INT32 moffsetcount;
    
    e_temp_addr1=(taddr/4096)*4096;
    e_temp_addr2=taddr-e_temp_addr1;
    
    moffsetcount=e_temp_addr2+noofbytes-4096;
    
    if(moffsetcount<0)
        moffsetcount=0;
    
    FLASHReadString(e_temp_addr1,4096,(char *)&Flash_Buffer);
    STRNCPY((char *)&Flash_Buffer+e_temp_addr2,ebuf,noofbytes-moffsetcount);

    Temp_Data_Saving(e_temp_addr1);   
        
    if(moffsetcount>0)
    {
        FLASHReadString(e_temp_addr1+4096,4096,(char *)&Flash_Buffer);
        STRNCPY((char *)&Flash_Buffer,ebuf+noofbytes-moffsetcount,moffsetcount);
        Temp_Data_Saving(e_temp_addr1+4096);
    }    
}

void Temp_Data_Saving(UINT32 taddr)
{
    Sector_Erase(Temp_Data_Sector);
    FLASHWriteString(Temp_Data_Sector,4096,(char *)&Flash_Buffer);

    Sector_Erase(Temp_Data_Sector_Flag);
    STRNCPY((char *)&LBUF+2,(char *)&taddr,4);

    LBUF[1]=0;  //flash no
    FLASHWriteString(Temp_Data_Sector_Flag,6,(char *)&LBUF);
    Sector_Erase(taddr);
    FLASHWriteString(taddr,4096,(char *)&Flash_Buffer);

    Sector_Erase(Temp_Data_Sector_Flag);
}

void Erase_Member_Details(void)
{
    UINT32 tempcount;

    for(tempcount=0;tempcount<12;tempcount++)
    {
        EEPROM_ADDR=Member_Details_Loc+(tempcount*4096);
        Sector_Erase(EEPROM_ADDR);
    }
    MIN_CODE=0;
    MAX_CODE=1;
}

void Payment_Register_Total(void)
{
    TOTAL_QTY=TOTAL_QTY+(Balancerec.QTY-Balancerec.PAID_QTY);
    TOTAL_AMT=TOTAL_AMT+(Balancerec.CREDIT_AMT-Balancerec.PAID_AMT);
    TOTAL_DEDUCTION=TOTAL_DEDUCTION+Balancerec.DEBIT_AMT;
}

UINT8 Get_Month_Days(UINT8 gtempmonth,UINT8 gtempyear)
{
    if(gtempmonth==4 || gtempmonth==6 || gtempmonth==9 || gtempmonth==11)
    {
        return 30;
    }
    else if(gtempmonth==2)
    {
        if(gtempyear%4==0)
            return 29;
        else
            return 28;
    }
    else
        return 31;
}

void Set_Initial_Dates(void)
{
    MONTH_READ_FLAG=0;

    TEDate=Day_Monthrec.END_DATE;
    TEMonth=Day_Monthrec.END_MONTH;    
    TEYear=Day_Monthrec.END_YEAR;
    
    TSYear=Day_Monthrec.START_YEAR;    

    if(TEYear==Day_Monthrec.START_YEAR)
        TSMonth=Day_Monthrec.START_MONTH;
    else
        TSMonth=1;   
    
    if(TEYear==Day_Monthrec.START_YEAR && TEMonth==Day_Monthrec.START_MONTH)
        TSDate=Day_Monthrec.START_DATE;
    else
        TSDate=1;
    
    TShift=2;
    NO_COLLECTIONS=0;    
}

UINT8 check_date_inrange(void)
{
    UINT8 inrange=0;
    
    if(ShiftDateRecord.YEAR<TEYear && ShiftDateRecord.YEAR>TSYear)
        inrange=1;
    
    else if(ShiftDateRecord.YEAR==TSYear && ShiftDateRecord.YEAR==TEYear )
    {
        if(ShiftDateRecord.MONTH>TSMonth && ShiftDateRecord.MONTH<TEMonth)
            inrange=1;
        
        else if(ShiftDateRecord.MONTH==TSMonth && ShiftDateRecord.MONTH==TEMonth)
        {
            if(ShiftDateRecord.DAY >=TSDate && ShiftDateRecord.DAY <=TEDate)
                inrange=1;                
        }
        else if(ShiftDateRecord.MONTH==TSMonth && ShiftDateRecord.DAY >=TSDate)
            inrange=1;
        
        else if(ShiftDateRecord.MONTH==TEMonth && ShiftDateRecord.DAY <=TEDate)
            inrange=1;
    }
    else if(ShiftDateRecord.YEAR==TSYear &&  ShiftDateRecord.YEAR<TEYear)
    {
        if(ShiftDateRecord.MONTH>TSMonth)
            inrange=1;
        
        else if(ShiftDateRecord.MONTH==TSMonth && ShiftDateRecord.DAY>=TSDate)
            inrange=1;
    }
    else if(ShiftDateRecord.YEAR==TEYear &&  ShiftDateRecord.YEAR>TSYear)
    {
        if(ShiftDateRecord.MONTH<TEMonth)
            inrange=1;
        else if(ShiftDateRecord.MONTH==TEMonth && ShiftDateRecord.DAY<=TEDate)
            inrange=1;
    }
    
    if(inrange==1)
       TEST_SHIFT_CNT++;
    
    return inrange;
}

void Read_SMS_Counter(void)
{
    RTCReadString(SMS_DATA_RTC_Loc,SMS_Record_Length,(char *)&SMSrec);
}

void Write_SMS_Counter(void)
{
    RTCWriteString(SMS_DATA_RTC_Loc,SMS_Record_Length,(char *)&SMSrec);
}

void Read_ShiftDate_Record(UINT32 shiftcount)
{
    UINT32 flashadr1;
    
    flashadr1=SHIFT_DATE_S_LOC+(shiftcount*SHIFT_DATE_LEN);   
    flashadr1=flashadr1+(1024*640*Flash_No);
    
    FLASHReadString(flashadr1,SHIFT_DATE_LEN,(char *)&ShiftDateRecord);
}

void Read_ShiftDate(UINT32 shiftcount)
{
    UINT32 flashadr1;
    
    flashadr1=SHIFT_DATE_S_LOC+(shiftcount*SHIFT_DATE_LEN);
    flashadr1=flashadr1+(1024*640*Flash_No);
    
    FLASHReadString(flashadr1,4,(char *)&ShiftDateRecord);
}

UINT8 Read_Day_Flag(UINT8 dctype)
{
    UINT8 flashcount;
    UINT32 noofshifts,shiftcounter;

    SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&RunningShift);
   
    if(RunningShift.DAY==Shiftrec.DAY && RunningShift.YEAR==Shiftrec.YEAR && RunningShift.MONTH==Shiftrec.MONTH && RunningShift.DAY_SHIFT==Shiftrec.DAY_SHIFT && WEIGHT_COUNT>0 )
    {        
        if(WEIGHT_COUNT>0 && dctype!='S')  
            return 1;
        else
            return 0;
    }
    
    for(flashcount=0;flashcount<NO_OF_FLASHES;flashcount++)//1
    {
        SEERAMReadString(No_Of_Shifts_F0_Loc+(flashcount*4),4,(char *)&noofshifts);
        Flash_No=flashcount;
        if(noofshifts==0xFFFFFFFF)
        {
            noofshifts=0;
            SEERAMWriteString(No_Of_Shifts_F0_Loc+(flashcount*4),4,(char *)&noofshifts);
            
        } 
        for(shiftcounter=0;shiftcounter<noofshifts;shiftcounter++)
        {
            Read_ShiftDate(shiftcounter);
            
            if(ShiftDateRecord.DAY==Shiftrec.DAY && ShiftDateRecord.MONTH==Shiftrec.MONTH && ShiftDateRecord.YEAR==Shiftrec.YEAR && ShiftDateRecord.DAY_SHIFT==Shiftrec.DAY_SHIFT)
            {
                Read_ShiftDate_Record(shiftcounter);
                return 1;

            }
        }
    }
    return 0;
}

UINT32 Calculate_Col_Amount(UINT16 colqty,UINT16 colrate)
{
    UINT32 col_amount=0;
    if(colqty!=0xFFFF && colrate!=0xFFFF)
    {
        col_amount=colrate;
        col_amount=col_amount*colqty;
        col_amount=col_amount/100;
    }
    
    return col_amount;
}

void Calculate_Amount(void)
{
    Amount=Calculate_Col_Amount(CollectionRecord.QTY,CollectionRecord.RATE);
}

void Calculate_Sangh_Amt(void)
{
    sangh_amount=Calculate_Col_Amount(Sanghrec.QTY,Sanghrec.RATE);
}

void Read_Local_Sale_Addr(void)
{    
    EEPROM_ADDR=LocalSales_Loc+(Shiftrec.MONTH-1)*LocalSales_Month_Length;
    EEPROM_ADDR=EEPROM_ADDR+((Shiftrec.DAY-1)*LocalSales_Record_Length*4);
    
    if(Shiftrec.DAY_SHIFT=='E')
        EEPROM_ADDR=EEPROM_ADDR+(LocalSales_Record_Length*2);
    
    if(LocalSalesrec.MILK_TYPE=='B')
        EEPROM_ADDR=EEPROM_ADDR+LocalSales_Record_Length;
}

void Read_Local_Sale(void)
{
    Read_Local_Sale_Addr();
    
    FLASHReadString(EEPROM_ADDR,LocalSales_Record_Length,(char *)&LocalSalesrec);
    if(LocalSalesrec.MONTH!=Shiftrec.MONTH || LocalSalesrec.YEAR !=Shiftrec.YEAR || LocalSalesrec.QTY==0xFFFFFFFF || LocalSalesrec.AMOUNT==0xFFFFFFFF)
    {
        LocalSalesrec.QTY=0;
        LocalSalesrec.AMOUNT=0;
    }    
}

void Write_Local_Sale(void)
{
    Read_Local_Sale_Addr();
    
    FLASHReadString(EEPROM_ADDR,LocalSales_Record_Length,(char *)&LocalSalesrec1);
    LocalSalesrec.YEAR=Shiftrec.YEAR;
    LocalSalesrec.MONTH=Shiftrec.MONTH;
    
    if(LocalSalesrec1.MONTH==0xFF && LocalSalesrec1.YEAR==0xFF)
    {        
        FLASHWriteString(EEPROM_ADDR,LocalSales_Record_Length,(char *)&LocalSalesrec);         
    }
    else
    {
        Edit_Flash_Data(EEPROM_ADDR,LocalSales_Record_Length,(char *)&LocalSalesrec);
    }
}

void Read_Sangh_Sale_Addr(void)
{
    EEPROM_ADDR=SanghSales_Loc+(Shiftrec.MONTH-1)*SanghSales_Month_Length;
    EEPROM_ADDR=EEPROM_ADDR+((Shiftrec.DAY-1)*Sangh_Record_Length*4);
    
    if(Shiftrec.DAY_SHIFT=='E')
        EEPROM_ADDR=EEPROM_ADDR+(Sangh_Record_Length*2);
    
    if(Sanghrec.MILK_TYPE=='B')
        EEPROM_ADDR=EEPROM_ADDR+Sangh_Record_Length;    
}

void Read_Sangh_Sale(void)
{
    Read_Sangh_Sale_Addr();

    FLASHReadString(EEPROM_ADDR,Sangh_Record_Length,(char *)&Sanghrec);
    
    if(Sanghrec.MONTH!=Shiftrec.MONTH || Sanghrec.YEAR !=Shiftrec.YEAR || Sanghrec.QTY==0xFFFFFFFF)
    {
        Sanghrec.QTY=0;
        Sanghrec.RATE=0;
        Sanghrec.FAT=0;
        Sanghrec.SNF=0;
    }   
}

void Write_Sangh_Sale(void)
{    
    Read_Sangh_Sale_Addr();
    FLASHReadString(EEPROM_ADDR,Sangh_Record_Length,(char *)&Sanghrec1);
    
    Sanghrec.YEAR=Shiftrec.YEAR;
    Sanghrec.MONTH=Shiftrec.MONTH;
    
    if(Sanghrec1.MONTH==0xFF && Sanghrec1.YEAR==0xFF)
    {        
        FLASHWriteString(EEPROM_ADDR,Sangh_Record_Length,(char *)&Sanghrec);
    }
    else
    {
        Edit_Flash_Data(EEPROM_ADDR,Sangh_Record_Length,(char *)&Sanghrec);    
    }     
}

UINT32 Get_Slabs_Table_Addr(UINT8 slabmilktype)
{
    if(EFFECT_RATE==1)
    {
        if(slabmilktype=='C')
            return Effect_C_Total_Slabs_Loc;
        else
            return Effect_B_Total_Slabs_Loc;
    }
    else
    {
        if(slabmilktype=='C')
            return C_Total_Slabs_Loc;
        else
            return B_Total_Slabs_Loc;
    }
}

UINT32 Get_SNF_Table_Limits_Addr(UINT8 snfmtype)
{
    if(EFFECT_RATE==1)
    {
        if(snfmtype=='C')
            return Effect_Table_Cow_SNF_Limits_Loc;
        else
            return Effect_Table_Buf_SNF_Limits_Loc;
    }
    else
    {
        if(snfmtype=='C')
            return Table_Cow_SNF_Limits_Loc;
        else
            return Table_Buf_SNF_Limits_Loc;
    }
}

void Read_SNF_Limits(UINT8 rmilktype)
{      
    EEPROM_ADDR=Get_SNF_Table_Limits_Addr(rmilktype);
    SEERAMReadString(EEPROM_ADDR,SNF_Limits_Length,(char *)&SNFLimitsrec);	
}

void Write_SNF_Limits(UINT8 wsnfmilktype)
{
    EEPROM_ADDR=Get_SNF_Table_Limits_Addr(wsnfmilktype);
    SEERAMWriteString(EEPROM_ADDR,SNF_Limits_Length,(BYTE *)&SNFLimitsrec);
}

UINT32 Get_FAT_Table_Limits_Addr(UINT8 fatmilktype)
{
    if(EFFECT_RATE==1)
    {
        if(fatmilktype=='C')
            return Effect_Table_Cow_FAT_Limits_Loc;
        else
            return Effect_Table_Buf_FAT_Limits_Loc;
    }
    else
    {
        if(fatmilktype=='C')
            return Table_Cow_FAT_Limits_Loc;
        else
            return Table_Buf_FAT_Limits_Loc;
    }
}

void Read_FAT_Limits(UINT8 rfmilktype)
{   
    EEPROM_ADDR=Get_FAT_Table_Limits_Addr(rfmilktype);   

    SEERAMReadString(EEPROM_ADDR,FAT_Limits_Length,(char *)&Fatlimitsrec);    
}

void Write_FAT_Limits(UINT8 wfmilktype)
{
    EEPROM_ADDR=Get_FAT_Table_Limits_Addr(wfmilktype);   

    SEERAMWriteString(EEPROM_ADDR,FAT_Limits_Length,(char *)&Fatlimitsrec);    
}

UINT32 Get_Rate_Table_Addr(UINT8 rtmilktype)
{
    UINT32 temp_table_addr;
    
    if(EFFECT_RATE==1)
    {
        if(rtmilktype=='C')
            temp_table_addr=Effect_Cow_SNFTable_Loc;
        else
            temp_table_addr=Effect_Buf_SNFTable_Loc;
    }
    else
    {
        if(rtmilktype=='C')
            temp_table_addr=Cow_SNFTable_Loc;
        else
            temp_table_addr=Buf_SNFTable_Loc;
    }
    return temp_table_addr;
}

void Read_Table_Addr(void)
{
    UINT32 s_r_l;

     Read_SNF_Limits(Tablerec.MILK_TYPE);

     s_r_l=(SNFLimitsrec.MAX_SNF-SNFLimitsrec.MIN_SNF+1)*2;

     EEPROM_ADDR=Get_Rate_Table_Addr(Tablerec.MILK_TYPE);


    EEPROM_ADDR=EEPROM_ADDR+((Tablerec.MILK_FAT*s_r_l)+(Tablerec.MILK_SNF*2));
}

UINT16 Read_SNF_MilkTable(void)
{
    Read_Table_Addr();

    FLASHReadString(EEPROM_ADDR,2,(char *)&Tablerec.MILK_RATE);

    if(Tablerec.MILK_RATE==0xFFFF)
        Tablerec.MILK_RATE=0;

    return Tablerec.MILK_RATE;
}

void Write_SNF_MilkTable(void)
{
    UINT16 milk_Rate;
    Read_Table_Addr();

    FLASHReadString(EEPROM_ADDR,2,(char *)&milk_Rate);//Tablerec1.MILK_RATE);

    //if(Tablerec1.MILK_RATE!=0xFFFF)
    if(milk_Rate!=0xFFFF)
    {
        Edit_Flash_Data(EEPROM_ADDR,2,(char *)&Tablerec.MILK_RATE);
       
    }
    else
        FLASHWriteString(EEPROM_ADDR,2,(char *)&Tablerec.MILK_RATE); 
}

UINT32 Read_New_SNF_MilkTable(UINT8 milktype)
{    
    UINT32 no_of_records;
    
    Read_SNF_Limits(milktype);

    //snf_record_length=(SNFLimitsrec.MAX_SNF-SNFLimitsrec.MIN_SNF+1)*2;
    no_of_records=((SNFLimitsrec.MAX_FAT+1)-SNFLimitsrec.MIN_FAT)*((SNFLimitsrec.MAX_SNF+1)-SNFLimitsrec.MIN_SNF);
    no_of_records = no_of_records * 2;

    EEPROM_ADDR=Get_Rate_Table_Addr(milktype);
    
    FLASHReadString(EEPROM_ADDR,no_of_records,(char *)&RATE_BUFFER);
    
    return no_of_records;
}

void Write_New_SNF_MilkTable(UINT8 milktype,UINT16 totalrecords)
{    
    //Read_Table_Addr();
    EEPROM_ADDR=Get_Rate_Table_Addr(milktype);

    FLASHWriteString(EEPROM_ADDR,totalrecords,(char *)&RATE_BUFFER); 
    
    FLASHReadString(EEPROM_ADDR,totalrecords,(char *)&RATE_BUFFER);
    milktype='B';
}
UINT32 Get_Fat_Table_Addr(UINT8 fatmilktype)
{
    if(EFFECT_RATE==1)
    {
        if(fatmilktype=='B')
            return Effect_Buf_FatTable_Loc;

        else
            return Effect_Cow_FatTable_Loc;
    }
    else
    {
        if(fatmilktype=='B')
            return Buf_FatTable_Loc;

        else
            return Cow_FatTable_Loc;
    }
}

void Read_FAT_MilkTable(UINT8 rmilktype,UINT16 rfatlocation)
{
    EEPROM_ADDR=Get_Fat_Table_Addr(rmilktype);

    EEPROM_ADDR=EEPROM_ADDR+(rfatlocation*Fat_Record_Length);
    FLASHReadString(EEPROM_ADDR,Fat_Record_Length,(char *)&Milkrec.RATE);

    if(Milkrec.RATE==0xFFFF)
        Milkrec.RATE=0;
}

void Write_FAT_MilkTable(UINT8 wmilktype,UINT16 wfatlocation)
{
    EEPROM_ADDR=Get_Fat_Table_Addr(wmilktype);

    EEPROM_ADDR=EEPROM_ADDR+(wfatlocation*Fat_Record_Length);
    FLASHReadString(EEPROM_ADDR,2,(char *)&Milkrec1.RATE);

    if(Milkrec1.RATE!=0xFFFF)
    {
        Edit_Flash_Data(EEPROM_ADDR,2,(char *)&Milkrec.RATE);
    }
    else
        FLASHWriteString(EEPROM_ADDR,Fat_Record_Length,(char *)&Milkrec.RATE);
}

void Read_Member_Details(UINT16 rmembersno)
{
    EEPROM_ADDR=Get_Member_Address(rmembersno);
    
    FLASHReadString(EEPROM_ADDR,Member_Record_Length,(char *)&Memberrec);   
}

UINT16 Read_Member_SNO(UINT16 rmembercode)
{   
    UINT16 tempi;
    
     for(tempi=1;tempi<=NO_OF_CUSTOMERS;tempi++)
    {
        Read_Member_CODE(tempi);
        
        if(rmembercode==Memberrec.C_CODE)
        {        
            return tempi;
        }               
    }    

    return 0;  
}

void Read_Member_Status(UINT16 rmembersno)
{
    EEPROM_ADDR=Get_Member_Address(rmembersno);
    FLASHReadString(EEPROM_ADDR+Member_Status_Off,1,(char *)&Memberrec.STATUS);
}

void Read_Member_MilkType(UINT16 rmembersno)
{
    EEPROM_ADDR=Get_Member_Address(rmembersno);
    FLASHReadString(EEPROM_ADDR+Milk_Type_Off,1,(char *)&Memberrec.MILK_TYPE);
}

void Read_Member_Ph(UINT16 rmembersno)
{
    EEPROM_ADDR=Get_Member_Address(rmembersno);
    FLASHReadString(EEPROM_ADDR+Member_Contact_No_Off,10,(char *)&Memberrec.CONTACT_NO);
}

void Read_Collection_Flags(void)
{
    UINT8 tempflag;    
    /*
                             CollectionRecord.FLAGS
        *************************************************************************************************************
        Bit7        -   Bit6     -   Bit5        -   Bit4    -   Bit3      -    Bit2    -   Bit1        -   Bit0
        Incentive   -   Payment  -   Upload      -   Sms     -   Fat mode  -    Weight  -   Shift       -   Milktype
        *************************************************************************************************************
        1:Normal                                                 1:Auto         1:Auto      1:Morning       1:Cow 
        0:Special       0:Paid       0:Uploaded      0:Sent      0:Manual       0:Manual    0:Evening       0:Buf
    */
    
    CollectionFlags.MILK_TYPE='C';
    tempflag=CollectionRecord.FLAGS & 0x01;
    if(tempflag==0)
        CollectionFlags.MILK_TYPE='B';
    
    CollectionFlags.SHIFT1='M';
    tempflag=CollectionRecord.FLAGS & 0x02;
    if(tempflag==0)
        CollectionFlags.SHIFT1='E';

    CollectionFlags.WEIGHT_MODE=1;
    tempflag=CollectionRecord.FLAGS & 0x04;
    if(tempflag==0)
        CollectionFlags.WEIGHT_MODE=0;

    CollectionFlags.ANALYZER_MODE=1;
    tempflag=CollectionRecord.FLAGS & 0x08;
    if(tempflag==0)
        CollectionFlags.ANALYZER_MODE=0;

    CollectionFlags.SMS_STATUS=1;
    tempflag=CollectionRecord.FLAGS & 0x10;
    if(tempflag==0)
        CollectionFlags.SMS_STATUS=0;
    
    CollectionFlags.UPLOAD_STATUS=1;
    tempflag=CollectionRecord.FLAGS & 0x20;
    if(tempflag==0)
        CollectionFlags.UPLOAD_STATUS=0;
    
    CollectionFlags.PAYMENT_STATUS=1;
    tempflag=CollectionRecord.FLAGS & 0x40;
    if(tempflag==0)
        CollectionFlags.PAYMENT_STATUS=0;
    
    CollectionFlags.INCENTIVE_COMM='N';
    
    tempflag=CollectionRecord.FLAGS & 0x80;
    if(tempflag==0)
        CollectionFlags.INCENTIVE_COMM='S';
}

void Read_Member_Name(UINT16 rmembersno)
{
    EEPROM_ADDR=Get_Member_Address(rmembersno);
    FLASHReadString(EEPROM_ADDR+Member_Name_Off,20,(char *)&Memberrec.NAME);
}

void Read_Member_Comm_Type(UINT16 rmembersno)
{
    EEPROM_ADDR=Get_Member_Address(rmembersno);
    FLASHReadString(EEPROM_ADDR+Commission_Type_Off,1,(char *)&Memberrec.COMMISSION_TYPE);
}

void Write_Member_Details_Flash(UINT16 wmembersno)
{    
    if(MIN_CODE>Memberrec.C_CODE || MIN_CODE==0)
        MIN_CODE=Memberrec.C_CODE;

    if(MAX_CODE<Memberrec.C_CODE)
        MAX_CODE=Memberrec.C_CODE;

    EEPROM_ADDR=Get_Member_Address(wmembersno);
    FLASHWriteString(EEPROM_ADDR,Member_Record_Length,(char *)&Memberrec);
}

void Edit_Member_Details_Flash(UINT16 emembersno,UINT16 enewcode)
{
    EEPROM_ADDR=Get_Member_Address(emembersno);
    
    if(EEPROM_ADDR>0)
    {
        Edit_Flash_Data(EEPROM_ADDR,Member_Record_Length,(char *)&Memberrec);
    }
}

void Read_Deduction_Amount(UINT32 membersno)
{
    EEPROM_ADDR=Deduction_Details_Loc+(membersno-1)*4;
    FLASHReadString(EEPROM_ADDR,4,(char *)&Deductionrec.AMT);

    if(Deductionrec.AMT==0xFFFFFFFF)
        Deductionrec.AMT=0;
}

void Write_Deduction_Amount(UINT32 wmembersno)
{
    EEPROM_ADDR=Deduction_Details_Loc+(wmembersno-1)*4;
    FLASHReadString(EEPROM_ADDR,4,(char *)&Deductionrec1.AMT);

    if(Deductionrec1.AMT!=0xFFFFFFFF)
    {
        Edit_Flash_Data(EEPROM_ADDR,4,(char *)&Deductionrec.AMT);       
    }
    else
        FLASHWriteString(EEPROM_ADDR,4,(char *)&Deductionrec.AMT);
}

UINT8 Get_No_Of_Customers(UINT16 scode,UINT16 ecode)
{
    UINT16 codecount;
    UINT8 reccount=0;
    
    for(codecount=PRESENT_MEMBER_NO;codecount<=NO_OF_CUSTOMERS;codecount++)
    {
        Read_Member_CODE(codecount);
                
        if(Memberrec.C_CODE==scode && Memberrec.C_CODE==ecode)
        {
            ReportsMatchrec[0].CODE_SNO=codecount;
            ReportsMatchrec[0].CUST_CODE=Memberrec.C_CODE;
            reccount=1;    
            break;
        }
        if(Memberrec.C_CODE>=scode && Memberrec.C_CODE<=ecode)
        {            
            ReportsMatchrec[reccount].CODE_SNO=codecount;
            ReportsMatchrec[reccount].CUST_CODE=Memberrec.C_CODE;
            reccount++;
            
        }  
        if(reccount==100)
            break;
    }
    PRESENT_MEMBER_NO=codecount+1;
    for(codecount=0;codecount<(reccount+1)*2;codecount++)
    {
        BalanceRecords[codecount].QTY=0;
        BalanceRecords[codecount].CREDIT_AMT=0;
        BalanceRecords[codecount].DEBIT_AMT=0;
        BalanceRecords[codecount].PAID_AMT=0;
        BalanceRecords[codecount].PAID_QTY=0;
        BalanceRecords[codecount].AVG_RATE=0;
        BalanceRecords[codecount].AVG_FAT=0;
        BalanceRecords[codecount].AVG_SNF=0;
        BalanceRecords[codecount].AVG_CLR=0;
        BalanceRecords[codecount].INCENT_AMT=0;
    }
    
    return reccount;   
}

UINT8 Read_New_Balance_Details(UINT8 norecs)
{
    UINT8 codecount,offset,array_count,inrange,records_present;    
    UINT8 tempflashno,flashcount;//,outflag,inflag;
    UINT16 tempsno,start_code,end_code,b_member_code;
    UINT32 col_amount;
    UINT32 noofshifts,flashadr1,currentrecordno,shiftcounter;
    UINT8 cshift,noofrecords;//,count
    UINT16 minbcode=0,maxbcode=1;    
  
    TEST_SHIFT_CNT=0;
    for(codecount=0,array_count=0;codecount<=norecs;codecount++)
    {    
        tempsno=ReportsMatchrec[codecount].CODE_SNO;
        if((ReportsMatchrec[codecount].CUST_CODE<minbcode || minbcode==0) && ReportsMatchrec[codecount].CUST_CODE!=0)
            minbcode=ReportsMatchrec[codecount].CUST_CODE;
        if(ReportsMatchrec[codecount].CUST_CODE>maxbcode )
            maxbcode=ReportsMatchrec[codecount].CUST_CODE;
        Read_Deduction_Amount(tempsno);
        
        if(Deductionrec.AMT!=0xFFFFFFFF)
              BalanceRecords[array_count].DEBIT_AMT=Deductionrec.AMT;
        
        array_count=array_count+2;
    }
      
    SEERAMReadString(Current_Flash_No_Loc,1,(char *)&CURRENT_FLASH_NO); 
    if(CURRENT_FLASH_NO>11)
        CURRENT_FLASH_NO=11;
    tempflashno=CURRENT_FLASH_NO;

    records_present='N';
  
    start_code=minbcode;
    end_code=maxbcode;
    SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&RunningShift);
        
    ShiftDateRecord.YEAR=RunningShift.YEAR;
    ShiftDateRecord.MONTH=RunningShift.MONTH;
    ShiftDateRecord.DAY=RunningShift.DAY;
    ShiftDateRecord.DAY_SHIFT=RunningShift.DAY_SHIFT;
    assign_dates();
    cshift=check_date_inrange();
    if(cshift==1)
    {
        noofrecords=Read_CurrentShiftDetails(10000); 

        while(noofrecords>0)
        {
            CollectionRecord=CollectionRecords[noofrecords-1];
            if(CollectionRecord.CODE>=start_code && CollectionRecord.CODE<=end_code && CollectionRecord.QTY!=0xFFFF && CollectionRecord.RATE!=0xFFFF)
            {

                Read_Collection_Flags();

                col_amount=Calculate_Col_Amount(CollectionRecord.QTY,CollectionRecord.RATE);
                CollectionRecord.INCENTIVE_AMT=Read_Incentive_Comm(1,CollectionRecord);
              
                offset=0;
                if(CollectionFlags.MILK_TYPE=='C')
                    offset=1;

                for(array_count=0;array_count<(norecs*2);array_count=array_count+2)                        
                {                            
                    b_member_code=ReportsMatchrec[array_count/2].CUST_CODE;

                    if(b_member_code==CollectionRecord.CODE)
                    {
                        if(CollectionRecord.QTY!=0xFFFF)
                            BalanceRecords[(array_count)+offset].QTY=BalanceRecords[(array_count)+offset].QTY+CollectionRecord.QTY;

                        BalanceRecords[(array_count)+offset].CREDIT_AMT=BalanceRecords[(array_count)+offset].CREDIT_AMT+col_amount+CollectionRecord.INCENTIVE_AMT;
                        BalanceRecords[(array_count)+offset].INCENT_AMT=BalanceRecords[(array_count)+offset].INCENT_AMT+CollectionRecord.INCENTIVE_AMT;
                        if(CollectionRecord.RATE!=0xFFFF)
                        {
                            AVG_RATE_CALC=CollectionRecord.RATE;
                            AVG_RATE_CALC=AVG_RATE_CALC*CollectionRecord.QTY;

                            BalanceRecords[(array_count)+offset].AVG_RATE=BalanceRecords[(array_count)+offset].AVG_RATE+AVG_RATE_CALC;
                            
                            AVG_FAT_CALC=CollectionRecord.QTY;
                            AVG_FAT_CALC=AVG_FAT_CALC*CollectionRecord.FAT;
                            BalanceRecords[(array_count)+offset].AVG_FAT=BalanceRecords[(array_count)+offset].AVG_FAT+AVG_FAT_CALC;

                            AVG_SNF_CALC=CollectionRecord.QTY;
                            AVG_SNF_CALC=AVG_SNF_CALC*CollectionRecord.SNF;
                            BalanceRecords[(array_count)+offset].AVG_SNF=BalanceRecords[(array_count)+offset].AVG_SNF+AVG_SNF_CALC;
                        
                            AVG_CLR_CALC=CollectionRecord.QTY;
                            AVG_CLR_CALC=AVG_CLR_CALC*CollectionRecord.CLR1;
                            BalanceRecords[(array_count)+offset].AVG_CLR=BalanceRecords[(array_count)+offset].AVG_CLR+AVG_CLR_CALC;
                        
                        }
                        if(CollectionFlags.PAYMENT_STATUS==0  && col_amount!=0xFFFFFFFF)//=='P'
                        {
                            BalanceRecords[(array_count)+offset].PAID_AMT=BalanceRecords[(array_count)+offset].PAID_AMT+col_amount+CollectionRecord.INCENTIVE_AMT;
                            BalanceRecords[(array_count)+offset].PAID_QTY=BalanceRecords[(array_count)+offset].PAID_QTY+CollectionRecord.QTY;
                        }
                        records_present='Y';

                        break;
                    }
                }
            }
            noofrecords--;
        }
    }
    
    for(flashcount=0;flashcount<NO_OF_FLASHES;flashcount++)
    {
        SEERAMReadString(No_Of_Shifts_F0_Loc+(tempflashno*4),4,(char *)&noofshifts);
        
        Flash_No=tempflashno;
        CollectionRecord.CODE=0xFFFF;
        
        for(shiftcounter=noofshifts;shiftcounter>0;shiftcounter--)
        {
            Read_ShiftDate(shiftcounter-1);
            
            assign_dates();
            inrange=check_date_inrange();
            
            if(inrange==1)
            {
                Read_ShiftDate_Record(shiftcounter-1);
                //STRNCPY((char *)&Incentiverec,(char *)&ShiftDateRecord.NOR_COMM,4);
                STRNCPY((char *)&Incentiverec,(char *)&ShiftDateRecord.NOR_COMM,20);
                for(currentrecordno=ShiftDateRecord.SRNO;currentrecordno<(ShiftDateRecord.SRNO+ShiftDateRecord.NOREC);currentrecordno++)
                {
                    Flash_No=tempflashno;
                    flashadr1=STARTING_RECORD_LOC+(currentrecordno*MILK_RECORD_LENGTH);                   
                    flashadr1=flashadr1+(1024*640*Flash_No);
                    FLASHReadString(flashadr1,2,(char *)&TempRecord.CODE);                      
                    
                    if(TempRecord.CODE>=start_code && TempRecord.CODE<=end_code)
                    {
                        FLASHReadString(flashadr1,MILK_RECORD_LENGTH,(char *)&TempRecord); 
                        if(TempRecord.QTY!=0xFFFF && TempRecord.RATE!=0xFFFF)
                        {
                        
                            CollectionRecord=TempRecord;   
                            Read_Collection_Flags();

                            col_amount=Calculate_Col_Amount(CollectionRecord.QTY,CollectionRecord.RATE);
                            CollectionRecord.INCENTIVE_AMT=Read_Incentive_Comm(1,CollectionRecord);
                            
                            offset=0;
                            if(CollectionFlags.MILK_TYPE=='C')
                                offset=1;

                            for(array_count=0;array_count<(norecs*2);array_count=array_count+2)                        
                            {                            
                                b_member_code=ReportsMatchrec[array_count/2].CUST_CODE;

                                if(b_member_code==CollectionRecord.CODE)
                                {
                                    BalanceRecords[(array_count)+offset].QTY=BalanceRecords[(array_count)+offset].QTY+CollectionRecord.QTY;
                                    BalanceRecords[(array_count)+offset].CREDIT_AMT=BalanceRecords[(array_count)+offset].CREDIT_AMT+col_amount+CollectionRecord.INCENTIVE_AMT;
                                    BalanceRecords[(array_count)+offset].INCENT_AMT=BalanceRecords[(array_count)+offset].INCENT_AMT+CollectionRecord.INCENTIVE_AMT;
                                    AVG_RATE_CALC=CollectionRecord.RATE;
                                    AVG_RATE_CALC=AVG_RATE_CALC*CollectionRecord.QTY;

                                    BalanceRecords[(array_count)+offset].AVG_RATE=BalanceRecords[(array_count)+offset].AVG_RATE+AVG_RATE_CALC;

                                    AVG_FAT_CALC=CollectionRecord.QTY;
                                    AVG_FAT_CALC=AVG_FAT_CALC*CollectionRecord.FAT;
                                    BalanceRecords[(array_count)+offset].AVG_FAT=BalanceRecords[(array_count)+offset].AVG_FAT+AVG_FAT_CALC;

                                    AVG_SNF_CALC=CollectionRecord.QTY;
                                    AVG_SNF_CALC=AVG_SNF_CALC*CollectionRecord.SNF;
                                    BalanceRecords[(array_count)+offset].AVG_SNF=BalanceRecords[(array_count)+offset].AVG_SNF+AVG_SNF_CALC;

                                    AVG_CLR_CALC=CollectionRecord.QTY;
                                    AVG_CLR_CALC=AVG_CLR_CALC*CollectionRecord.CLR1;
                                    BalanceRecords[(array_count)+offset].AVG_CLR=BalanceRecords[(array_count)+offset].AVG_CLR+AVG_CLR_CALC;

                                    if(CollectionFlags.PAYMENT_STATUS==0  && Amount!=0xFFFFFFFF)//=='P'
                                    {
                                        BalanceRecords[(array_count)+offset].PAID_AMT=BalanceRecords[(array_count)+offset].PAID_AMT+col_amount+CollectionRecord.INCENTIVE_AMT;
                                        BalanceRecords[(array_count)+offset].PAID_QTY=BalanceRecords[(array_count)+offset].PAID_QTY+CollectionRecord.QTY;
                                    }
                                    records_present='Y';

                                    break;
                                }
                            }
                        }
                    }
                }
            }        
        }
        
         if(tempflashno==0)
            tempflashno=NO_OF_FLASHES-1;
        else
            tempflashno--;
    }
    return records_present;
}

void Set_Collection_Flag(UINT8 flagtype)
{
    UINT32 flashadr1;
    
    if(CollectionRecord_NO.Type=='L')
    {
        //flashadr1=Sample_Records_Loc+(CollectionRecord_NO.RECORDNO*17);Current_Shift_Record_Length
        
        flashadr1=Sample_Records_Loc+(CollectionRecord_NO.RECORDNO*Current_Shift_Record_Length);//17
    }
    else
    {
        flashadr1=STARTING_RECORD_LOC+(CollectionRecord_NO.RECORDNO*MILK_RECORD_LENGTH);         
        flashadr1=flashadr1+(1024*640*CollectionRecord_NO.FLASH_NO);
    }
    FLASHReadString(flashadr1+7,1,(char *)&TempRecord.FLAGS);
    if(flagtype=='S')                                       //sms
        TempRecord.FLAGS=TempRecord.FLAGS & 0xEF;
    
    else if(flagtype=='P')                                  //payment
        TempRecord.FLAGS=TempRecord.FLAGS & 0xBF;
    
    else if(flagtype=='U')                                  //upload
        TempRecord.FLAGS=TempRecord.FLAGS & 0xDF;
    
    FLASHWriteString(flashadr1+7,1,(char *)&TempRecord.FLAGS);
}

void Edit_BillDetails(UINT32 emembersno)
{
    UINT8 noofbytes;//=17;
    
    Read_Collection_Flags();
    if(CollectionFlags.UPLOAD_STATUS==0)
    {
        CollectionRecord.WTIME[2]=0x80 | CollectionRecord.WTIME[2];       
        CollectionRecord.FLAGS=CollectionRecord.FLAGS | 0x30;
        DU_COUNT=DU_COUNT-1;
        SMS_COUNT=0;
    }
    
    CollectionRecord.FLAGS=CollectionRecord.FLAGS & 0xF3;
    CollectionRecord.FLAGS=CollectionRecord.FLAGS | 0x01;
    
    if(Tablerec.MILK_TYPE=='B')
        CollectionRecord.FLAGS=CollectionRecord.FLAGS & 0xFE;
        
    if(CollectionRecord_NO.Type=='L')
    {
        EEPROM_ADDR=Sample_Records_Loc+(CollectionRecord_NO.RECORDNO*Current_Shift_Record_Length);//17);
        noofbytes=Current_Shift_Record_Length;
    }
    else
    {
        EEPROM_ADDR=STARTING_RECORD_LOC+(CollectionRecord_NO.RECORDNO*MILK_RECORD_LENGTH);
        noofbytes=MILK_RECORD_LENGTH; 
    }  
 
    Edit_Flash_Data(EEPROM_ADDR,noofbytes,(char *)&CollectionRecord);
    UPLOAD_COUNT=CollectionRecord.SHIFT_RECORD_NO-1;
}

UINT32 Read_Incentive_Comm( UINT8 coltype,struct CollectionStructure coll_rec)
{
    UINT16 i_commission_rate=0;
    UINT32 incentive_amt=0;
    UINT8 temp_spl_inc_flag;
    struct CollectionStructure tempcoll;
    
    tempcoll=CollectionRecord;
    //SEERAMReadString(Auto_Commission_Loc,1,(char *)&Commissionrec.STATUS);
            
   if(coll_rec.RATE>0 && coll_rec.RATE!=0xFFFF)
    {    
        if(coltype==1)
        {
            CollectionRecord.FLAGS=coll_rec.FLAGS;
            Read_Collection_Flags();

            if(CollectionFlags.INCENTIVE_COMM=='N')
                i_commission_rate= Incentiverec.NOR_COMM;
            else 
            {
                temp_spl_inc_flag=coll_rec.WTIME[0] & 0xE0;
                if(temp_spl_inc_flag == 0x00)
                    i_commission_rate= Incentiverec.SPL_COMM[0];
                if(temp_spl_inc_flag == 0x20)
                    i_commission_rate= Incentiverec.SPL_COMM[1];
                if(temp_spl_inc_flag == 0x40)
                    i_commission_rate= Incentiverec.SPL_COMM[2]; 
                if(temp_spl_inc_flag == 0x60)
                    i_commission_rate= Incentiverec.SPL_COMM[3];
                if(temp_spl_inc_flag == 0x80)
                    i_commission_rate= Incentiverec.SPL_COMM[4];
                if(temp_spl_inc_flag == 0xA0)
                    i_commission_rate= Incentiverec.SPL_COMM[5];
                if(temp_spl_inc_flag == 0xC0)
                    i_commission_rate= Incentiverec.SPL_COMM[6];
                if(temp_spl_inc_flag == 0xE0)
                    i_commission_rate= Incentiverec.SPL_COMM[7];
            }
        }
        else //during collection
        {
            SEERAMReadString(Auto_Commission_Loc,1,(char *)&Commissionrec.STATUS);
            if(Commissionrec.STATUS=='N')
                return 0;
            if(Memberrec.COMMISSION_TYPE=='N')
                i_commission_rate= Incentiverec.NOR_COMM;
            else
            {               
                i_commission_rate= Incentiverec.SPL_COMM[Memberrec.COMMISSION_TYPE-0x31];
            }
        }
        
        if(i_commission_rate>0 && i_commission_rate!=0xFFFF)
        {
            incentive_amt=coll_rec.QTY*i_commission_rate;

            incentive_amt=incentive_amt/100;
        }
    }
    else
    {
        incentive_amt=0;
    } 
    CollectionRecord=tempcoll;
    return incentive_amt;
}

UINT16 Read_CurrentShiftDetails(UINT16 membercode)
{
    UINT32 w_eeprom_addr;
    UINT16 tempcounter;
    UINT16 noofrecords=0;
    UINT8 ticnt=0;
    
    CollectionRecord.QTY=0xFFFF;
    CollectionRecord.FAT=0xFF;
    CollectionRecord.SNF=0xFF;
    
    CollectionRecord.WATER1=0x0000;
    CollectionRecord.CLR1=0xFFFF;
    
    tempcounter=0;

   // SEERAMReadString(Running_Shift_Incentive_Loc,4,(char *)&Incentiverec); 
    SEERAMReadString(Running_Shift_Incentive_Loc,20,(char *)&Incentiverec); 
    
    if(Incentiverec.NOR_COMM!=0)// || Incentiverec.SPL_COMM!=0)
        IncentiveReportFlag=1;
    else
    {
        for(ticnt=0;ticnt<9;ticnt++)
        {
            if(Incentiverec.SPL_COMM[ticnt]!=0)
            {
                IncentiveReportFlag=1;
                break;
            }
        }
    }
    
    while(tempcounter<WEIGHT_COUNT)
    {
       // w_eeprom_addr=Sample_Records_Loc+(tempcounter*17);
        w_eeprom_addr=Sample_Records_Loc+(tempcounter*Current_Shift_Record_Length);        
        
        FLASHReadString(w_eeprom_addr,2,(char *)&TempRecord.CODE);
        
        if(TempRecord.CODE==membercode || membercode==10000)
        {
            FLASHReadString(w_eeprom_addr,Current_Shift_Record_Length,(char *)&TempRecord); //17
            TempRecord.INCENTIVE_AMT=Read_Incentive_Comm(1,TempRecord);
           
            TempRecord.SHIFT_RECORD_NO=tempcounter+1;
            
            CollectionRecords[noofrecords]=TempRecord;
            CollectionRecords_NOS[noofrecords].RECORDNO=tempcounter;
            CollectionRecords_NOS[noofrecords].Type='L';
            
            noofrecords++;
        }
        tempcounter++;
    }
    return noofrecords;    
}

void Write_Weight(void)
{
    UINT32 w_eeprom_addr;
      
    CollectionRecord.FLAGS=0xFF;
    
    if(TWO_ANALYZERS=='N' || (TWO_ANALYZERS=='Y' && Settingsrec.MIXED_MILK=='N') )
    {
        if(Tablerec.MILK_TYPE=='B')
            CollectionRecord.FLAGS=0xFE;
    }
    
    if(Shiftrec.DAY_SHIFT=='E')
        CollectionRecord.FLAGS=CollectionRecord.FLAGS & 0xFD;
    
    if(Settingsrec.WEIGHT_MODE==0)
        CollectionRecord.FLAGS=CollectionRecord.FLAGS & 0xFB;
    
    if(Memberrec.COMMISSION_TYPE!='N')
        CollectionRecord.FLAGS=CollectionRecord.FLAGS & 0x7F;
    
    Read_RTC();
    CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]);//HEX_ASCII(Time[2]);
    CollectionRecord.WTIME[1]=HEX_ASCII(Time[1]);
    CollectionRecord.WTIME[2]=HEX_ASCII(Time[0]);    
    
    if(Memberrec.COMMISSION_TYPE!='N')
    {
        CollectionRecord.FLAGS=CollectionRecord.FLAGS & 0x7F;
        
         Read_RTC();
        if(Memberrec.COMMISSION_TYPE=='1')
            CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]);
        if(Memberrec.COMMISSION_TYPE=='2')
            CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]) | 0x20;
        if(Memberrec.COMMISSION_TYPE=='3')
            CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]) | 0x40;
        if(Memberrec.COMMISSION_TYPE=='4')
            CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]) | 0x60;
        if(Memberrec.COMMISSION_TYPE=='5')
            CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]) | 0x80;
        if(Memberrec.COMMISSION_TYPE=='6')
            CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]) | 0xA0;
        if(Memberrec.COMMISSION_TYPE=='7')
            CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]) | 0xC0;
        if(Memberrec.COMMISSION_TYPE=='8')
            CollectionRecord.WTIME[0]=HEX_ASCII(Time[2]) | 0xE0;
    }
        
        

    //w_eeprom_addr=Sample_Records_Loc +(WEIGHT_COUNT*17);
    
    w_eeprom_addr=Sample_Records_Loc +(WEIGHT_COUNT*Current_Shift_Record_Length);
    FLASHWriteString(w_eeprom_addr,8,(char *)&CollectionRecord);
    WEIGHT_COUNT=WEIGHT_COUNT+1;
}

void Write_Analyzer_Data(void)
{       
    AnalyzerRecord.FLAGS=AnalyzerRecord.FLAGS & 0xFD;
    
    //AEEPROM_ADDR=Sample_Records_Loc+(ANALYZER_COUNT*17);
    
    AEEPROM_ADDR=Sample_Records_Loc+(ANALYZER_COUNT*Current_Shift_Record_Length);
    FLASHReadString(AEEPROM_ADDR+7,1,(char *)&CollectionRecord.FLAGS);
    
    if(Settingsrec.FAT_MODE==0)
        CollectionRecord.FLAGS=CollectionRecord.FLAGS & 0xF7;
    
    if(TWO_ANALYZERS=='Y'  && Settingsrec.MIXED_MILK=='Y')
    {
        //CollectionRecord.FLAGS=CollectionRecord.FLAGS | 0x01;

        if(Tablerec.MILK_TYPE=='B')
            CollectionRecord.FLAGS=CollectionRecord.FLAGS & 0xFE;
    }
    
    FLASHWriteString(AEEPROM_ADDR+7,12,(char *)&CollectionRecord.FLAGS);//10
    FLASHReadString(AEEPROM_ADDR+7,12,(char *)&CollectionRecord.FLAGS);//10
    
    ANALYZER_COUNT=ANALYZER_COUNT+1;
}

void TempUpdate_Upload_Flag(UINT16 trecno)
{   
    if(CollectionFlags.UPLOAD_STATUS==1 && CollectionRecord.QTY!=0xFFFF)
    {     
        FLASHWriteString((TempUpload_Loc+(trecno*6)),6,(char *)&CollectionRecord_NO);   
    }
}

void Original_Update_Upload_Flag(UINT16 orecno)
{
    UINT8 o_temprec=0;

    while(o_temprec<orecno)
    {
        FLASHReadString((TempUpload_Loc+(o_temprec*6)),6,(char *)&CollectionRecord_NO);//5
        
        Set_Collection_Flag('U');       
     
        o_temprec++;
    }
}

UINT16 DynamicRead_MemberBillDetails(UINT32 rmembersno)
{
    UINT16 shiftcounter,r_member_code;
    UINT32 flashadr1;
    UINT32 noofrecords,currentrecordno;
    UINT8 flashcount=0,datematch=0;
    UINT32 noofshifts;
    UINT8 tempflashno;
    UINT8 inrange;//outflag,inflag=0;
    UINT8 tempincentcnt;
    
    noofrecords=0;
    if(ONLYONESHIFT==1)
    {
        SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&RunningShift);
        if(RunningShift.DAY==Shiftrec.DAY && RunningShift.YEAR==Shiftrec.YEAR && RunningShift.MONTH==Shiftrec.MONTH && RunningShift.DAY_SHIFT==Shiftrec.DAY_SHIFT && WEIGHT_COUNT>0)
        {
            if(rmembersno!=10000)   
                Read_Member_CODE(rmembersno);
            else
                Memberrec.C_CODE=10000;

            noofrecords=Read_CurrentShiftDetails(Memberrec.C_CODE);

            //090118return noofrecords;
        }
    }
    SEERAMReadString(Current_Flash_No_Loc,1,(char *)&CURRENT_FLASH_NO);
    
    if(CURRENT_FLASH_NO>11)
        CURRENT_FLASH_NO=11;
    tempflashno=CURRENT_FLASH_NO;
    
    
    Read_Member_CODE(rmembersno);
    r_member_code=Memberrec.C_CODE;
    
    for(flashcount=0;flashcount<NO_OF_FLASHES;flashcount++)//
    {
        SEERAMReadString(No_Of_Shifts_F0_Loc+(tempflashno*4),4,(char *)&noofshifts);
        
        Flash_No=tempflashno;
        
        CollectionRecord.CODE=0xFFFF;       
        
        for(shiftcounter=noofshifts;shiftcounter>0;shiftcounter--)
        {            
            Read_ShiftDate(shiftcounter-1);
            if(ONLYONESHIFT==1)
            {
                if(ShiftDateRecord.DAY==Shiftrec.DAY && ShiftDateRecord.MONTH==Shiftrec.MONTH && ShiftDateRecord.YEAR==Shiftrec.YEAR && ShiftDateRecord.DAY_SHIFT==Shiftrec.DAY_SHIFT)
                    inrange=1;
                else
                    inrange=0;
            }
            else
            {
                assign_dates();
                inrange=check_date_inrange();
            }
          
            if(inrange==1)
            {
                datematch=1;
                Read_ShiftDate_Record(shiftcounter-1);
                //STRNCPY((char *)&Incentiverec,(char *)&ShiftDateRecord.NOR_COMM,4);//1
                
                STRNCPY((char *)&Incentiverec,(char *)&ShiftDateRecord.NOR_COMM,20);//1
                
               /* if(Incentiverec.NOR_COMM!=0 ||Incentiverec.SPL_COMM!=0)
                    IncentiveReportFlag=1;*/
                if(Incentiverec.NOR_COMM!=0)
                    IncentiveReportFlag=1;
                else
                {
                    for(tempincentcnt=0;tempincentcnt<9;tempincentcnt++)
                    {
                        if(Incentiverec.SPL_COMM[tempincentcnt]!=0)
                        {
                            IncentiveReportFlag=1;
                            break;
                        }
                    }
                }
                
                for(currentrecordno=ShiftDateRecord.SRNO;currentrecordno<(ShiftDateRecord.SRNO+ShiftDateRecord.NOREC);currentrecordno++)
                {
                    flashadr1=STARTING_RECORD_LOC+(currentrecordno*MILK_RECORD_LENGTH);                   
                    flashadr1=flashadr1+(1024*640*Flash_No);
                    FLASHReadString(flashadr1,2,(char *)&TempRecord.CODE);  
                   
                    if(TempRecord.CODE==r_member_code || rmembersno==10000)
                    {                        
                        FLASHReadString(flashadr1,MILK_RECORD_LENGTH,(char *)&TempRecord);  
                        if(TempRecord.FAT!=0xFF && TempRecord.SNF!=0xFF && TempRecord.CLR1!=0xFFFF && TempRecord.RATE!=0xFFFF)
                        {                        
                            TempRecord.INCENTIVE_AMT=Read_Incentive_Comm(1,TempRecord);
                           // TempRecord.CLR=0;
                            //TempRecord.WATER=0;
                           // TempRecord.CLR1=0;
                            TempRecord.WATER1=0;
                            TempRecord.SHIFT_RECORD_NO=(currentrecordno-ShiftDateRecord.SRNO)+1;
                            TempRecord.ATIME[0]=TempRecord.WTIME[0];
                            TempRecord.ATIME[1]=TempRecord.WTIME[1];
                            TempRecord.ATIME[2]=TempRecord.WTIME[2];

                            CollectionRecords[noofrecords]=TempRecord;

                            CollectionRecords_NOS[noofrecords].RECORDNO=currentrecordno;
                            CollectionRecords_NOS[noofrecords].Type='G';
                            CollectionRecords_NOS[noofrecords].FLASH_NO=tempflashno;

                            ShiftRecords[noofrecords].DAY=ShiftDateRecord.DAY;
                            ShiftRecords[noofrecords].DAY_SHIFT=ShiftDateRecord.DAY_SHIFT;
                            ShiftRecords[noofrecords].MONTH=ShiftDateRecord.MONTH;
                            ShiftRecords[noofrecords].YEAR=ShiftDateRecord.YEAR;

                            noofrecords++;

                            if(noofrecords==999)
                                return noofrecords;
                        }
                    }
                }
            }
           /* else if(ShiftDateRecord.YEAR>TEYear )//|| (ShiftDateRecord.YEAR==TEYear && ShiftDateRecord.MONTH > TEMonth) || (ShiftDateRecord.YEAR==TEYear && ShiftDateRecord.MONTH == TEMonth && ShiftDateRecord.DAY >TEDate ))
            {
                outflag=1;
            }   
            else if(ShiftDateRecord.YEAR<TSYear)// || (ShiftDateRecord.YEAR==TSYear && ShiftDateRecord.MONTH < TSMonth) || (ShiftDateRecord.YEAR==TSYear && ShiftDateRecord.MONTH == TSMonth && ShiftDateRecord.DAY <TSDate ))
            {
                inflag=1;
            } 
            if(inflag==1 && outflag==1)// && noofrecords>599)
                return noofrecords;*/
            
        }
        
        if(tempflashno==0)
            tempflashno=NO_OF_FLASHES-1;
        else
            tempflashno--;
    }
    return noofrecords;    
}

UINT16 DynamicRead_BillDetails(UINT32 rmembersno)
{
    UINT16 noofrecords;
    
    ONLYONESHIFT=1;
    noofrecords=DynamicRead_MemberBillDetails(rmembersno);
    ONLYONESHIFT=0;
    
    return noofrecords;
}

void Erase_ShiftData(UINT8 flashno)
{     
     UINT32 tempno,tempadr=0;
    
    for(tempno=0;tempno<160;tempno++)
    {
        tempadr=SHIFT_DATE_S_LOC+(1024*640*flashno)+(tempno*4096);
        Sector_Erase(tempadr);
    }  
}

void Update_Total_Shift_Data(void)
{
    UINT16 noofrecords,currentrecordno;
    UINT32 shiftadr,recordadr,maxrecords;    
    
    Display_Boarder3();
    STRCPY(LBUF," Updating Shift Data");
    Disp_Msg(LINE4);
    
    Please_Wait_Msg_Small(LINE6);
    
    if(RunningShift.YEAR==0xFF && RunningShift.MONTH==0xFF && RunningShift.DAY==0xFF && RunningShift.DAY_SHIFT==0xFF)
        return;
    
    SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&RunningShift);
    SEERAMReadString(Current_Flash_No_Loc,1,(char *)&CURRENT_FLASH_NO);
    
    if(CURRENT_FLASH_NO>11)
        CURRENT_FLASH_NO=11;
    
    SEERAMReadString(Current_Shift_No_Loc,4,(char *)&CURRENT_SHIFT_NO);
    maxrecords=FLASH0_MAX_RECORDS;
    
    //Each FlashBlock is 160 4k blocks
    shiftadr=SHIFT_DATE_S_LOC+(4096*160*CURRENT_FLASH_NO); 
    if(CURRENT_SHIFT_NO==0)
    {
        ShiftDateRecord.SRNO=0;
        ShiftDateRecord.NOREC=0;
        //shiftadr=SHIFT_DATE_S_LOC;
    }
    else
    {
        //Getting Previous shiftrecord address
        shiftadr=shiftadr+((CURRENT_SHIFT_NO-1)*SHIFT_DATE_LEN);
        
        //Reading previous shift data srno and norec details
        FLASHReadString(shiftadr,SHIFT_DATE_LEN,(char *)&ShiftDateRecord);
        //shiftadr=SHIFT_DATE_S_LOC+(CURRENT_SHIFT_NO*SHIFT_DATE_LEN);
        //current shift address
        shiftadr=shiftadr+SHIFT_DATE_LEN;
    }
    
    if((ShiftDateRecord.SRNO+ShiftDateRecord.NOREC+WEIGHT_COUNT)>maxrecords  || CURRENT_SHIFT_NO==FLASH_MAX_SHIFTS)
    {
        SEERAMWriteString(No_Of_Shifts_F0_Loc+(CURRENT_FLASH_NO*4),4,(char *)&CURRENT_SHIFT_NO);
        CURRENT_FLASH_NO++;
        
        if(CURRENT_FLASH_NO==NO_OF_FLASHES)//should be three here
            CURRENT_FLASH_NO=0;
       
        SEERAMWriteString(Current_Flash_No_Loc,1,(char *)&CURRENT_FLASH_NO);
        
        CURRENT_SHIFT_NO=0;
        SEERAMWriteString(Current_Shift_No_Loc,4,(char *)&CURRENT_SHIFT_NO);
        Erase_ShiftData(CURRENT_FLASH_NO);
        
        CURRENT_SHIFT_NO=0;
        SEERAMWriteString(No_Of_Shifts_F0_Loc+(CURRENT_FLASH_NO*4),4,(char *)&CURRENT_SHIFT_NO);
     
        ShiftDateRecord.SRNO=0;
        ShiftDateRecord.NOREC=0;
        shiftadr=SHIFT_DATE_S_LOC+(4096*160*CURRENT_FLASH_NO);
    } 
    
    if(((ShiftDateRecord.SRNO+ShiftDateRecord.NOREC+WEIGHT_COUNT)<=maxrecords) && CURRENT_SHIFT_NO<FLASH_MAX_SHIFTS)//sudhakar
    {
        ShiftDateRecord.SRNO=ShiftDateRecord.SRNO+ShiftDateRecord.NOREC;
        ShiftDateRecord.NOREC=WEIGHT_COUNT;
        
        for(noofrecords=0;noofrecords<WEIGHT_COUNT;noofrecords++)
        {            
            Read_Current_Shift_Col_Record(noofrecords);
            
            currentrecordno=ShiftDateRecord.SRNO+noofrecords;
            recordadr=STARTING_RECORD_LOC+(4096*160*CURRENT_FLASH_NO);
            recordadr=recordadr+(currentrecordno*MILK_RECORD_LENGTH); 
                        
            FLASHWriteString(recordadr,MILK_RECORD_LENGTH,(char *)&CollectionRecord);  
        }
                
        STRNCPY((char *)&ShiftDateRecord,(char *)&RunningShift,4);
        //SEERAMReadString(Running_Shift_Incentive_Loc,4,(char *)&ShiftDateRecord.NOR_COMM);
        SEERAMReadString(Running_Shift_Incentive_Loc,20,(char *)&ShiftDateRecord.NOR_COMM);
        
        FLASHWriteString(shiftadr,SHIFT_DATE_LEN,(char *)&ShiftDateRecord);
        FLASHReadString(shiftadr,SHIFT_DATE_LEN,(char *)&ShiftDateRecord);
        CURRENT_SHIFT_NO++;
       
        SEERAMWriteString(Current_Shift_No_Loc,4,(char *)&CURRENT_SHIFT_NO);
        SEERAMWriteString(No_Of_Shifts_F0_Loc+(CURRENT_FLASH_NO*4),4,(char *)&CURRENT_SHIFT_NO);
    }
    
    Display_Boarder3();
}

UINT8 Fetch_Customer_Record(void)
{
    while(TEYear>=TSYear)
    {
        Shiftrec.YEAR=TEYear;

        while(TEMonth>=TSMonth)
        {
            Shiftrec.MONTH=TEMonth;
          
            while(TEDate>=TSDate)
            {
                Shiftrec.DAY=TEDate;

                while(TShift>0)
                {
                    if(TShift==1)
                        Shiftrec.DAY_SHIFT='M';
                    else
                        Shiftrec.DAY_SHIFT='E';
                    if(Read_Day_Flag('C')==1)
                    {
                        if(Fetch_Customer_Flag!=0)
                        {
                            while(S_SNo<=E_SNo)
                            {
                                if(NO_COLLECTIONS==0)
                                {
                                    CollectionRecord.QTY=0xFFFF;
                                    NO_COLLECTIONS=DynamicRead_BillDetails(S_SNo);
                                }
                                if(NO_COLLECTIONS>0)
                                {
                                    CollectionRecord=CollectionRecords[NO_COLLECTIONS-1];
                                    CollectionRecord_NO.RECORDNO=CollectionRecords_NOS[NO_COLLECTIONS-1].RECORDNO;
                                    CollectionRecord_NO.Type=CollectionRecords_NOS[NO_COLLECTIONS-1].Type;
                                    CollectionRecord_NO.FLASH_NO=CollectionRecords_NOS[NO_COLLECTIONS-1].FLASH_NO;
                                    Read_Collection_Flags();
                                }

                                if(NO_COLLECTIONS==0)    
                                {
                                    CollectionRecord.QTY=0xFFFF;
                                    S_SNo++;
                                }

                                if( CollectionRecord.QTY!=0xFFFF && CollectionRecord.FAT!=0xFF)//CollectionRecord.QTY>0 &&
                                {
                                    NO_COLLECTIONS=NO_COLLECTIONS-1;
                                    if(NO_COLLECTIONS==0)
                                        S_SNo++;
                                    return 1;
                                }
                                else if(NO_COLLECTIONS>0)
                                    NO_COLLECTIONS=NO_COLLECTIONS-1;                                  
                            }
                            S_SNo=Shiftrec_SNo;
                        }
                        else
                        {
                            if(NO_COLLECTIONS==0)
                            {
                                CollectionRecord.QTY=0xFFFF;
                                NO_COLLECTIONS=DynamicRead_BillDetails(S_SNo);
                            }
                            if(NO_COLLECTIONS>0)
                            {
                                CollectionRecord=CollectionRecords[NO_COLLECTIONS-1];
                                CollectionRecord_NO.RECORDNO=CollectionRecords_NOS[NO_COLLECTIONS-1].RECORDNO;
                                CollectionRecord_NO.Type=CollectionRecords_NOS[NO_COLLECTIONS-1].Type;
                                CollectionRecord_NO.FLASH_NO=CollectionRecords_NOS[NO_COLLECTIONS-1].FLASH_NO;

                                Read_Collection_Flags();

                            }

                            if(NO_COLLECTIONS==0)    
                            {
                                CollectionRecord.QTY=0xFFFF;
                            }

                            if( CollectionRecord.QTY!=0xFFFF && CollectionRecord.FAT!=0xFF)//CollectionRecord.QTY>0 &&
                            {
                                NO_COLLECTIONS=NO_COLLECTIONS-1;
                                if(NO_COLLECTIONS==0)    
                                    TShift--;

                                return 1;
                            }
                            else if(NO_COLLECTIONS>0)
                                NO_COLLECTIONS=NO_COLLECTIONS-1;
                        }
                    }
                    SHIFT_READ_FLAG=1;
                    if(Fetch_Customer_Flag==10)
                    {
                        Fetch_Customer_Flag=0;
                        return 0;
                    }
                    if(NO_COLLECTIONS==0)     
                        TShift--;
                }
                TShift=2;
                DATE_READ_FLAG=1;
                TEDate--;
            }

            MONTH_READ_FLAG=1;
            TEMonth--;
            TSDate=1;
            if(TEYear==Day_Monthrec.START_YEAR && TEMonth==Day_Monthrec.START_MONTH)
                TSDate=Day_Monthrec.START_DATE;
           
            TEDate=Get_Month_Days(TEMonth,TEYear);//31;
        }

        TEYear--;
        
        if(TEYear==Day_Monthrec.END_YEAR)
            TEMonth=Day_Monthrec.END_MONTH;
        else
            TEMonth=12;
        
        if(TEYear==Day_Monthrec.START_YEAR)
            TSMonth=Day_Monthrec.START_MONTH;
        else
            TSMonth=1;
        
        TEDate=Get_Month_Days(TEMonth,TEYear);
        
        if(TEYear==Day_Monthrec.START_YEAR && TEMonth==Day_Monthrec.START_MONTH)
            TSDate=Day_Monthrec.START_DATE;
        else
            TSDate=1;      
    }
    return 0;
}

UINT8 Fetch_LocalSales_Record(UINT8 salestype,UINT8 tempmilktype)
{    
    while(TEYear>=TSYear)
    {
        Shiftrec.YEAR=TEYear;
        
        while(TEMonth>=TSMonth)
        {
            Shiftrec.MONTH=TEMonth;
            
            while(TEDate>=TSDate)
            {
                Shiftrec.DAY=TEDate;

                while(TShift>0)
                {
                    if(TShift==1)
                        Shiftrec.DAY_SHIFT='M';
                    else
                        Shiftrec.DAY_SHIFT='E';
                    
                    if(salestype=='L')
                    {
                        LocalSalesrec.MILK_TYPE=tempmilktype;
                        Read_Local_Sale();
                    }
                        
                    else
                    {
                        Sanghrec.MILK_TYPE=tempmilktype;
                        Read_Sangh_Sale();
                        LocalSalesrec.MONTH=Sanghrec.MONTH;
                        LocalSalesrec.YEAR=Sanghrec.YEAR;
                    }

                    if(Shiftrec.MONTH==LocalSalesrec.MONTH && Shiftrec.YEAR==LocalSalesrec.YEAR)
                    {
                        TShift--;
                        return 1;
                    }
                    
                    TShift--;

                 }
                TShift=2;
                 DATE_READ_FLAG=1;
                 TEDate--;
            }
            MONTH_READ_FLAG=1;           
            TEMonth--;
            TSDate=1;
           
            if(TEYear==Day_Monthrec.START_YEAR && TEMonth==Day_Monthrec.START_MONTH)
                TSDate=Day_Monthrec.START_DATE;
           
            TEDate=Get_Month_Days(TEMonth,TEYear);
        }
       
        TEYear--;
        
        if(TEYear==Day_Monthrec.END_YEAR)
            TEMonth=Day_Monthrec.END_MONTH;
        else
            TEMonth=12;
        
        if(TEYear==Day_Monthrec.START_YEAR)
            TSMonth=Day_Monthrec.START_MONTH;
        else
            TSMonth=1;
        
        TEDate=Get_Month_Days(TEMonth,TEYear);
        
        if(TEYear==Day_Monthrec.START_YEAR && TEMonth==Day_Monthrec.START_MONTH)
            TSDate=Day_Monthrec.START_DATE;
        else
            TSDate=1;   
       
    }
    return 0;
}

UINT32 Get_Member_Address(UINT16 gmembersno)
{
    UINT32 g_temp_long;

    if(gmembersno<1000)
    {
        g_temp_long=gmembersno;
        g_temp_long=Member_Details_Loc+(g_temp_long*Member_Record_Length);

    }
    else
        g_temp_long=0;
    
    return g_temp_long;
}

void Read_Member_CODE(UINT16 rmembersno)
{
    EEPROM_ADDR=Get_Member_Address(rmembersno);
    FLASHReadString(EEPROM_ADDR+Member_Code_Off,2,(char *)&Memberrec.C_CODE);
}

void Read_Current_Shift_Col_Record(UINT16 recordno)
{
    UINT32 s_eeprom_adr;
    
    //s_eeprom_adr=Sample_Records_Loc+(recordno*17);
    s_eeprom_adr=Sample_Records_Loc+(recordno*Current_Shift_Record_Length);
    FLASHReadString(s_eeprom_adr,Current_Shift_Record_Length,(char *)&CollectionRecord);//17
    
    SEERAMReadString(Running_Shift_Incentive_Loc,20,(char *)&Incentiverec);//added26-05-2020  
    CollectionRecord.INCENTIVE_AMT=Read_Incentive_Comm(1,CollectionRecord);
    
}