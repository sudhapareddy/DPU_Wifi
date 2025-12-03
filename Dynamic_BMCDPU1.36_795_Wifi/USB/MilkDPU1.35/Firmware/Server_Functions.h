UINT8 Transfer_Member_Data(UINT8 tratype)
{
    UINT8 t_temp_cnt;
    UINT32 s_no,e_no;

    Member_Header('1');
    
    if(tratype==0)
    {
        s_no=0;
        e_no=0;
    }
    else if(tratype==2)
    {
        s_no=100000;
        e_no=100000;
    }
    else
    {
        s_no=1;
        e_no=NO_OF_CUSTOMERS;
    }

    for(;s_no<=e_no;s_no++)
    {
       // if(s_no!=0)
        if(s_no!=0 && s_no!=100000)
            Read_Member_Details(s_no);

        if(Memberrec.UPDATE_STATUS=='N' || Memberrec.UPDATE_STATUS=='E' || Memberrec.UPDATE_STATUS=='A' || Memberrec.UPDATE_STATUS=='D' || Memberrec.UPDATE_STATUS=='X')
        {
            t_temp_cnt=0;
            if(s_no==100000)
                STRCPY(SEND_BUF,"#SCTDELETEALL:");
            else if(Memberrec.UPDATE_STATUS=='N')
            {
                t_temp_cnt=0;
                STRCPY(SEND_BUF,"#SCTADDMEMBER:");
            }
            else
            {
                t_temp_cnt=1;
                STRCPY(SEND_BUF,"#SCTEDITMEMBER:");
            }

            STRNCPY(SEND_BUF+14+t_temp_cnt,MACHINE_ID,7);
            ZERO=0;
            if(s_no!=100000)
            {
                Hex_DEC(0,Memberrec.C_CODE,SEND_BUF+21+t_temp_cnt,4);
                SEND_BUF[25+t_temp_cnt]=Memberrec.MILK_TYPE;
                SEND_BUF[26+t_temp_cnt]=Memberrec.COMMISSION_TYPE;
                STRNCPY(SEND_BUF+27+t_temp_cnt,Memberrec.NAME,20);
                STRNCPY(SEND_BUF+47+t_temp_cnt,Memberrec.CONTACT_NO,10);            
                SEND_BUF[57+t_temp_cnt]=Memberrec.STATUS;
               // STRNCPY(SEND_BUF+58+t_temp_cnt,Memberrec.BARCODE,10);
                SEND_BUF[58+t_temp_cnt]='!';
                SEND_BUF[59+t_temp_cnt]='\r';
                STRNCPY(LBUF,SEND_BUF+21,20);
            }
            else
            {
                SEND_BUF[21]='!';
                SEND_BUF[22]='\r';
                STRCPY(LBUF,"Deleting Members...");
            }
            GPRS_SendString(SEND_BUF);
            GPRS_RecString_Data(REC_BUF);            
            Disp_Msg(LINE4);

            //if(strncmp(SEND_BUF,REC_BUF,(25+t_temp_cnt))!=0)
            if(strncmp(SEND_BUF,REC_BUF,(21+t_temp_cnt))!=0)
            {
                if(Memberrec.UPDATE_STATUS=='N')
                    STRCPY(LBUF,"ERROR:ADD MEMBER");
                else
                    STRCPY(LBUF,"ERROR:EDIT MEMBER");
                Disp_Msg(LINE8);
                return 'E';

            }
            else
            {
                if(tratype==0)
                    Memberrec.UPDATE_STATUS=0xFF;
                else
                {
                    if(s_no!=100000)
                    {
                        Memberrec.UPDATE_STATUS=Memberrec.UPDATE_STATUS & 0xBF;
                        EEPROM_ADDR=Get_Member_Address(s_no);
                        FLASHWriteString(EEPROM_ADDR+Member_Transfer_Off,1,(char *)&Memberrec.UPDATE_STATUS);
                        //2607 Flash_No=0;
                    }
                }
            }
        }
    }
    return 'S';
}

UINT8 Send_Shift_Records(UINT16 nbytes)
{
    GPRSSEND_BUF[nbytes]='!';
    nbytes=nbytes+1;
    GPRSSEND_BUF[nbytes]='\r';
    nbytes=nbytes+1;
    GPRS_Send_HexData(GPRSSEND_BUF,nbytes);
    GPRS_RecString_Data(REC_BUF);

    if(strncmp(GPRSSEND_BUF,REC_BUF,20)!=0  || No_Response==1)
    {
        if(strncmp(REC_BUF,"#SCTRESEND:",11)==0)
            return 'R';
        else
            return 0x21;
    }
    
    else
        return 'S';
}

void Write_Settings(void)
{
    UINT8 enable[5];

    Please_Wait_Msg(LINE5);
    
    enable[0]=enable[1]=enable[2]='Y';
    enable[3]='N';
    SEERAMWriteString(MemberList_Control_Loc,4,(char *)&enable);
    
    SEERAMWriteString(GPRS_ON_Loc,1,(char *)&enable);
    
    //SEERAMWriteString(Machine_Lock_Loc,1,(char *)&enable);
   
}

UINT8 Transfer_Shift_Data(UINT8 ttype)
{    
    UINT8 t_rec_cnt,m_s_flag,clear_members_flag,tempsend_buf[50];
    UINT16 t_member_sno,t_buf_index;

    Menu_Header('T',0);

    if(ttype=='S' || ttype=='A')
    {
        if(Start_End_Date_Month()==0xFF)
            return 0xFF;
    
        CLEAR_DATA_LINES_FROM(3,8);
    }
  
    MODEM_ERROR=Connect_GPRS();
    if(MODEM_ERROR!='S')
    {
        return MODEM_ERROR;
    }
  

    STRCPY(LBUF,"SERVER CONNECTED");
    Disp_Msg_Line_Boarder(0,LINE4);
   
    STRCPY(LBUF,"TRANSFERING DATA....");
    Disp_Msg(LINE7);
    
    SEERAMReadString(MemberList_Control_Loc,3,(char *)&DPUcontrolrec);

    if(ttype=='S' || ttype=='D')
    {
        if(DPUcontrolrec.RATE_TABLES=='N')
        {
            SERVER_C_TYPE='S';
            MODEM_ERROR=Receive_Rate_Charts();
            if(MODEM_ERROR!='S')
            {
                return MODEM_ERROR;
            }
        }
        
        SEERAMReadString(Clear_Memberlist_Flag_Loc,1,(char *)&clear_members_flag);
        if(clear_members_flag==1 && DPUcontrolrec.MEMBER_LIST=='Y')
        {
            MODEM_ERROR=Transfer_Member_Data(2);

            if(MODEM_ERROR=='S')
            {
                Reset_Delete_All_Members_Flag();
            }
                
            else
                return MODEM_ERROR;
        }
        SEERAMReadString(Clear_Memberlist_Flag_Loc,1,(char *)&clear_members_flag);
        if(clear_members_flag==0xFF)
        {        
            SEERAMReadString(Members_Server_Flag,1,(char *)&m_s_flag);
            if(m_s_flag==1)
            {
                MODEM_ERROR=Transfer_Member_Data(1);

                if(MODEM_ERROR=='S')
                    Clear_Member_Flag();
                else
                    return MODEM_ERROR;
            }
        }
    }
    
    STRCPY(tempsend_buf,"#SCTSTARTSHIFT:");
    STRNCPY(tempsend_buf+15,MACHINE_ID,7);
    tempsend_buf[22]='!';
    tempsend_buf[23]='\r';
    GPRS_Send_HexData(tempsend_buf,23);
    GPRS_RecString_Data(REC_BUF);

    if(strncmp(REC_BUF,"#SCTNOTFOUND:",13)==0) //MACHINE NOT ADDED
    {
        return 0x20;
    }

    else if(strncmp(tempsend_buf,REC_BUF,22)!=0)        //ERROR:START SHIFT
    {     
        return 0x21;
    }
    
    Menu_Header('T',0);

    if(ttype=='S' || ttype=='A')
    {
        Copy_MonthRecDT(LBUF);
        Disp_Msg(LINE3);


        STRCPY(LBUF,"Transferring Data ");       
        Disp_Msg(LINE5);
        
        Fetch_Customer_Flag=1;
        DATE_READ_FLAG=1;
        Set_Initial_Dates();
    }

    else if(ttype=='C' || ttype=='D')
    {
        TSDate=Shiftrec.DAY;
        TSMonth=Shiftrec.MONTH;
        TSYear=Shiftrec.YEAR;
        
        TEDate=Shiftrec.DAY;        
        TEMonth=Shiftrec.MONTH;        
        TEYear=Shiftrec.YEAR;

        if(Shiftrec.DAY_SHIFT=='M')
            TShift=1;
        else
            TShift=2;

        Fetch_Customer_Flag=10;//1upto here
        NO_COLLECTIONS=0;      
    }
    
    S_SNo=10000;
    E_SNo=10000;
    Shiftrec_SNo=S_SNo;
    t_rec_cnt=0;
    
    while(Fetch_Customer_Record())
    {
        Read_Collection_Flags();
        
        if(CollectionRecord.QTY!=0xFFFF  && CollectionRecord.FAT!=0xFF && CollectionRecord.FAT>0 && (CollectionFlags.UPLOAD_STATUS==1 || ttype=='A') )//
        {
            if(t_rec_cnt==0)
            {
                STRCPY(GPRSSEND_BUF,"#SCTSYNCRECORDS:");
                STRNCPY(GPRSSEND_BUF+16,MACHINE_ID,7);
                t_buf_index=24;
                Sector_Erase(TempUpload_Loc);
            }
                        
            Add_Collection_Record(t_rec_cnt,GPRSSEND_BUF);
            TempUpdate_Upload_Flag(t_rec_cnt);
            t_rec_cnt=t_rec_cnt+1;
            GPRSSEND_BUF[23]=t_rec_cnt;
            t_buf_index=t_rec_cnt;
            t_buf_index=(t_buf_index*Upload_Record_Length)+24;       
            if(t_rec_cnt==35)//40)
            {
                MODEM_ERROR=Send_Shift_Records(t_buf_index);
                if(MODEM_ERROR=='R')
                    MODEM_ERROR=Send_Shift_Records(t_buf_index);
                if(MODEM_ERROR!='S')
                    return MODEM_ERROR;
                Original_Update_Upload_Flag(t_rec_cnt);
                t_rec_cnt=0;
            }     
        }

        Display_Date();
    }

    if(t_rec_cnt>0 && t_rec_cnt<35)//40
    {
        MODEM_ERROR=Send_Shift_Records(t_buf_index);
        if(MODEM_ERROR=='R')
            MODEM_ERROR=Send_Shift_Records(t_buf_index);
        if(MODEM_ERROR!='S')
            return MODEM_ERROR;
        Original_Update_Upload_Flag(t_rec_cnt);
    }
    
    STRCPY(tempsend_buf,"#SCTENDSHIFT:");
    STRNCPY(tempsend_buf+13,MACHINE_ID,7);
    tempsend_buf[20]='!';
    tempsend_buf[21]='\r';
    GPRS_Send_HexData(tempsend_buf,22);
    GPRS_RecString_Data(REC_BUF);

    if(strncmp(tempsend_buf,REC_BUF,20)!=0)
    {
        return 0x21;
    }
    return 'S';
}

UINT8 Receive_Fat_Table(UINT8 fmilktype)
{
    UINT8 r_t_cnt,temp_send_buf[50];
    UINT16 addr,r_rate_id,r_old_rate_id;
    UINT32 r_offset_address;

    //effective tables
    if(fmilktype=='C')
        r_offset_address=512;

    else
        r_offset_address=768;

    Import_Export_Header(fmilktype);

    FLASHReadString(Cow_FatTable_Loc,4096,(char *)&Flash_Buffer);

   
    ZERO=0;
    if(fmilktype=='C')
    {
        STRCPY(temp_send_buf,"#SCTFATCOWRECORD:");
        EEPROM_ADDR=Cow_Fat_Rate_Id_Loc;
    }
    else
    {
        STRCPY(temp_send_buf,"#SCTFATBUFRECORD:");
        EEPROM_ADDR=Buf_Fat_Rate_Id_Loc;
    }
    SEERAMReadString(EEPROM_ADDR,2,(char *)&r_old_rate_id);
    STRNCPY(temp_send_buf+17,MACHINE_ID,7);
    Hex_DEC(0,r_old_rate_id,temp_send_buf+24,4);
    temp_send_buf[28]='!';
    temp_send_buf[29]='\r';

    GPRS_Send_HexData(temp_send_buf,30);
    GPRS_RecString_Data(Flash_Buffer1); 
    
    if(strncmp(Flash_Buffer1,"#NO NEW RATE CHART AVAILABLE!",29)==0)
    {                           
        No_New_Chart_Msg();  
        Delay_250Ms(4);
        return 'S';
    }
    else if(strncmp(temp_send_buf,Flash_Buffer1,24)==0)
    {
        STRCPY(LBUF,"RECEIVING FAT DATA..");
        Disp_Msg(LINE7);
    
        STRNCPY(Temp_Arr,Flash_Buffer1+24,4);
        Temp_Arr[4]='\0';
        r_rate_id=atoi(Temp_Arr);

        if(r_rate_id!=r_old_rate_id)
        {
            STRNCPY(Temp_Arr,Flash_Buffer1+28,4);
            Temp_Arr[4]='\0';
            Fatlimitsrec.MIN_FAT=Float_Long(Temp_Arr);

            STRNCPY(Temp_Arr,Flash_Buffer1+32,4);
            Temp_Arr[4]='\0';
            Fatlimitsrec.MAX_FAT=Float_Long(Temp_Arr);

            if(Fatlimitsrec.MAX_FAT>(Fatlimitsrec.MIN_FAT+120))
                Fatlimitsrec.MAX_FAT=Fatlimitsrec.MIN_FAT+120;
            
            EFFECT_RATE=1;
            Write_FAT_Limits(fmilktype);
            Read_FAT_Limits(fmilktype);

            STRNCPY(Temp_Arr,Flash_Buffer1+36,2);
            Temp_Arr[2]='\0';
            Effectrec.DATE=atoi(Temp_Arr);

            STRNCPY(Temp_Arr,Flash_Buffer1+38,2);
            Temp_Arr[2]='\0';
            Effectrec.MONTH=atoi(Temp_Arr);

            STRNCPY(Temp_Arr,Flash_Buffer1+40,2);
            Temp_Arr[2]='\0';
            Effectrec.YEAR=atoi(Temp_Arr);

            EEPROM_ADDR=New_BufFat_Effect_Date_Loc+(fmilktype-0x42)*3;
            SEERAMWriteString(EEPROM_ADDR,3,(char *)&Effectrec);

            addr=42;
            r_t_cnt=Fatlimitsrec.MIN_FAT;

            RATE_BUF_INDEX=0;
            parseCSVString(Flash_Buffer1+42,1);
            STRNCPY((char *)&Flash_Buffer[r_offset_address],(char *)&RATE_BUFFER,(RATE_BUF_INDEX*2));
            
            Update_FAT_FLASH();//4
           
            if(fmilktype=='C')
            {
                STRCPY(SEND_BUF,"#SCTFATCOWEND:");
                EEPROM_ADDR=Cow_Fat_Rate_Id_Loc;
            }
            else
            {
                STRCPY(SEND_BUF,"#SCTFATBUFEND:");
                EEPROM_ADDR=Buf_Fat_Rate_Id_Loc;
            }

            STRNCPY(SEND_BUF+14,MACHINE_ID,7);
            Hex_DEC(0,r_rate_id,SEND_BUF+24,4);
            SEND_BUF[28]='!';
            SEND_BUF[29]='\r';
            GPRS_SendString(SEND_BUF);
            GPRS_RecString_Data(REC_BUF);
            if(No_Response==1)
                return 0x21;
                
            SEERAMWriteString(EEPROM_ADDR,2,(char *)&r_rate_id);
            return 'S';
        }
        else
        { 
            No_New_Chart_Msg();           
            return 'S';
        }
    }
    else
    {
        return 0x21;
    }   
}

UINT8 Receive_SNF_Table(UINT8 smilktype)
{
    UINT8 r_t_cnt,temp_send_buf[50];
    UINT16 r_addr,r_rate_id,r_old_rate_id;
    UINT16 r_present_fat;

    Import_Export_Header(smilktype+32);
    
    STRCPY(LBUF,"RECEIVING LIMITS...");
    Disp_Msg(LINE7);
    ZERO=0;

    if(smilktype=='C')
    {
        STRCPY(temp_send_buf,"#SCTSNFCOWLIMITS:\r");
        EEPROM_ADDR=Cow_Snf_Rate_Id_Loc;
    }
    else
    {
        STRCPY(temp_send_buf,"#SCTSNFBUFLIMITS:\r");
         EEPROM_ADDR=Buf_Snf_Rate_Id_Loc;
    }
    SEERAMReadString(EEPROM_ADDR,2,(char *)&r_old_rate_id);
    STRNCPY(temp_send_buf+17,MACHINE_ID,7);
    Hex_DEC(0,r_old_rate_id,temp_send_buf+24,4);
    temp_send_buf[28]='!';
    temp_send_buf[29]='\r';
    temp_send_buf[30]='\n';
    GPRS_Send_HexData(temp_send_buf,31);
    GPRS_RecString_Data(REC_BUF);
  
    if(strncmp(temp_send_buf,REC_BUF,24)==0 &&  No_Response==0)
    {
        STRNCPY(Temp_Arr,REC_BUF+24,4);
        Temp_Arr[4]='\0';
        r_rate_id=atoi(Temp_Arr);

        if(r_rate_id!=r_old_rate_id)
        {
            STRNCPY(Temp_Arr,REC_BUF+28,4);
            Temp_Arr[4]='\0';
            SNFLimitsrec.MIN_FAT=Float_Long(Temp_Arr);
            r_present_fat=SNFLimitsrec.MIN_FAT;

            STRNCPY(Temp_Arr,REC_BUF+32,4);
            Temp_Arr[4]='\0';
            SNFLimitsrec.MAX_FAT=Float_Long(Temp_Arr);
            
            if(SNFLimitsrec.MAX_FAT>(SNFLimitsrec.MIN_FAT+120))
            {
                SNFLimitsrec.MAX_FAT=SNFLimitsrec.MIN_FAT+120;
            }

            STRNCPY(Temp_Arr,REC_BUF+36,4);
            Temp_Arr[4]='\0';
            SNFLimitsrec.MIN_SNF=Float_Long(Temp_Arr);

            STRNCPY(Temp_Arr,REC_BUF+40,4);
            Temp_Arr[4]='\0';
            SNFLimitsrec.MAX_SNF=Float_Long(Temp_Arr);
            if(SNFLimitsrec.MAX_SNF>(SNFLimitsrec.MIN_SNF+120))
            {
                SNFLimitsrec.MAX_SNF=SNFLimitsrec.MIN_SNF+120;
            }
           
            EFFECT_RATE=1;
            Write_SNF_Limits(smilktype);
            Read_SNF_Limits(smilktype);
            
            STRNCPY(Temp_Arr,REC_BUF+44,2);
            Temp_Arr[2]='\0';
            Effectrec.DATE=atoi(Temp_Arr);

            STRNCPY(Temp_Arr,REC_BUF+46,2);
            Temp_Arr[2]='\0';
            Effectrec.MONTH=atoi(Temp_Arr);

            STRNCPY(Temp_Arr,REC_BUF+48,2);
            Temp_Arr[2]='\0';
            Effectrec.YEAR=atoi(Temp_Arr);
            
            EFFECT_RATE=1;
            Erase_SNF_Table(smilktype);
            EFFECT_RATE=0;
            Display_Boarder3();

            STRCPY(LBUF,"RECEIVING DATA...");//1
            Disp_Msg(LINE7);
        }
    }
    else
    {
        if(strncmp(REC_BUF,"#No Rate Tables exist!",22)==0)
        {
            STRCPY(LBUF,"No Rate chart...");
        
        }
        else
            STRCPY(LBUF,"Communication Error...");
        Disp_Msg(LINE4);
        Delay_250Ms(2);
        return 0x21;
    }

    RATE_BUF_INDEX=0;
    while(r_rate_id!=r_old_rate_id)
    {
        
        if(r_present_fat<=SNFLimitsrec.MAX_FAT)
        {
            if(smilktype=='C')
                STRCPY(temp_send_buf,"#SCTSNFCOWRECORD:\r");
            else
                STRCPY(temp_send_buf,"#SCTSNFBUFRECORD:\r");

            STRNCPY(temp_send_buf+17,MACHINE_ID,7);
            ZERO=0;
            Hex_DEC(1,r_present_fat,temp_send_buf+24,4);
            temp_send_buf[28]='!';
            temp_send_buf[29]='\r';
            temp_send_buf[30]='\n';
            GPRS_Send_HexData(temp_send_buf,31);
            GPRS_RecString_Data(Flash_Buffer1);
           
            if(strncmp(temp_send_buf,Flash_Buffer1,28)==0 &&  No_Response==0)
            {
                parseCSVString(Flash_Buffer1+28,0);
             
                r_present_fat=r_present_fat+1;
            }
            else
            {              
                return 0x21;
            }
        }
        else            
            break;
    }

    if(r_rate_id==r_old_rate_id)
    {
        No_New_Chart_Msg();
        Delay_250Ms(4);
        return 'S';
    }
    else
    {
        if(smilktype=='C')
            STRCPY(temp_send_buf,"#SCTSNFCOWEND:");
        else
            STRCPY(temp_send_buf,"#SCTSNFBUFEND:");

        STRNCPY(temp_send_buf+14,MACHINE_ID,7);
        Hex_DEC(0,r_rate_id,temp_send_buf+21,4);
        temp_send_buf[25]='!';
        temp_send_buf[26]='\r';
        GPRS_Send_HexData(temp_send_buf,27);
        GPRS_RecString_Data(Flash_Buffer1);
        
        if(strncmp(temp_send_buf,Flash_Buffer1,25)==0 &&  No_Response==0)
        {   
            EFFECT_RATE=1;
            Write_SNF_Limits(smilktype);
            Read_SNF_Limits(smilktype);

            Erase_SNF_Table(smilktype);

            Tablerec.MILK_FAT=0;
            Tablerec.MILK_SNF=0;
            Tablerec.MILK_TYPE=smilktype;
            Write_New_SNF_MilkTable(smilktype,(RATE_BUF_INDEX*2));

            EEPROM_ADDR=(smilktype=='B')?New_BufSnf_Effect_Date_Loc:New_CowSnf_Effect_Date_Loc;
            SEERAMWriteString(EEPROM_ADDR,3,(char *)&Effectrec);

            EFFECT_RATE=0;

            EEPROM_ADDR=(smilktype=='B')?Buf_Snf_Rate_Id_Loc:Cow_Snf_Rate_Id_Loc;
            SEERAMWriteString(EEPROM_ADDR,2,(char *)&r_rate_id);

            return 'S';
        }
        else
        {              
            return 0x21;
        }
    }
}

UINT8 Receive_Members1(void)
{
    UINT32 r_total_members,r_offset_addr,r_temp_member_cnt;
    UINT32 r_block;
    UINT8 temp_send_buf[50];

    Import_Export_Header('M');

    if(CONNECT_FLAG==1)
    {
        r_total_members=0;
        r_block=0;
        Display_Boarder3();

        STRCPY(LBUF,"RECEIVING MEMBER CNT");
        Disp_Msg(LINE7);

        STRCPY(SEND_BUF,"#SCTMEMBERCOUNT:\r");
        STRNCPY(SEND_BUF+16,MACHINE_ID,7);
        SEND_BUF[23]='!';
        SEND_BUF[24]='\r';

        GPRS_SendString(SEND_BUF);
        GPRS_RecString_Data(REC_BUF);

        if(strncmp(SEND_BUF,REC_BUF,23)!=0 )
        {
            Send_End_Command();
            return 0x21;
        }

        else
        {
            STRNCPY(Temp_Arr,REC_BUF+23,4);
            Temp_Arr[4]='\0';
            r_temp_member_cnt=atoi(Temp_Arr);

            STRCPY(LBUF," Member Count:");
            Hex_DEC(0,r_temp_member_cnt,LBUF+14,4);

            if(r_temp_member_cnt==0 ||  r_temp_member_cnt>1000)
            {
                Disp_Msg(LINE4);
                Delay_Sec(1);
                return 0x22;
            }
        }
        if(r_total_members==0 && r_block==0)
        {
            STRCPY(LBUF,"ERASING DATA...");
            Disp_Msg(LINE7);

            Erase_Member_Details();
            Reset_Delete_All_Members_Flag();
            Clear_Member_Flag();
        }

        NO_OF_CUSTOMERS=r_temp_member_cnt;
        STRCPY(LBUF,"RECEIVING DATA...");
        Disp_Msg(LINE7);
        MIN_CODE=0;
        MAX_CODE=0;

        while(1)
        {
            STRCPY(SEND_BUF,"#SCTMEMBER:\r");
            STRNCPY(SEND_BUF+11,MACHINE_ID,7);
            ZERO=0;
            Hex_DEC(0,r_block,SEND_BUF+18,4);
            SEND_BUF[22]='!';
            SEND_BUF[23]='\r';

            GPRS_SendString(SEND_BUF);
            GPRS_RecString_Data(Flash_Buffer);

            strncpy(temp_send_buf,SEND_BUF,18);
            STRCPY(temp_send_buf+18,"ENDMEMBERS!");


            if(strncmp(temp_send_buf,Flash_Buffer,29)==0)
            {
               Send_End_Command();
                return 'S';
            }
            else
            {
                if(strncmp(SEND_BUF,Flash_Buffer,22)==0 && No_Response==0)
                {
                    r_total_members=r_block*30;//30;

                    while(r_total_members<NO_OF_CUSTOMERS && r_total_members<(r_block+1)*30)//30)
                    {
                        STRCPY(LBUF,"RECORD NO:0000");
                        Hex_DEC(0,r_total_members+1,LBUF+10,4);
                        Disp_Msg(LINE4);

                        r_offset_addr=22+((r_total_members-(r_block*30))*37);

                        STRNCPY(Temp_Arr,Flash_Buffer+r_offset_addr,4);
                        Temp_Arr[4]='\0';
                        Memberrec.C_CODE=atoi(Temp_Arr);
                        
                        STRCPY(Memberrec.BARCODE,"SCT100    ");
                        Hex_DEC(0,Memberrec.C_CODE,Memberrec.BARCODE+6,4);

                        Memberrec.MILK_TYPE=Flash_Buffer[r_offset_addr+4];

                        Memberrec.COMMISSION_TYPE=Flash_Buffer[r_offset_addr+5];

                        STRNCPY(Memberrec.NAME,(Flash_Buffer+r_offset_addr+6),20);

                        STRNCPY(Memberrec.CONTACT_NO,(Flash_Buffer+r_offset_addr+26),10);

                        Memberrec.STATUS=Flash_Buffer[r_offset_addr+36];//'A';
                        
                        r_total_members=r_total_members+1;

                        Write_Member_Details_Flash(r_total_members);
                    }
                    if(r_total_members>=NO_OF_CUSTOMERS)
                    {
                        Send_End_Command();
                        return 'S';
                    }
                    else
                         r_block=r_block+1;
                }
                else
                {
                    Send_End_Command();
                    return 0x21;
                }
            }
        }
    }
}

UINT8 Receive_Rate_Charts(void)
{    
    BYTE c_temp_buf[10];
    UINT8 tempnob,tempsl,err_no,tmp_incent_cnt,tempsend_buf[50],clrBasedTable;
    UINT16 cow_fat_id,buf_fat_id,cow_snf_id,buf_snf_id;
    UINT16 rate_chart_ids[4];
   
    Server_Menu_Header('S');

    STRCPY(LBUF,"Waiting for Response");
    Disp_Msg(LINE5);

    ZERO=0;
   
    STRCPY(tempsend_buf,"#SCTSERVERVALUE2:\r");
    STRNCPY(tempsend_buf+17,MACHINE_ID,7);
    tempsend_buf[24]='!';
    tempsend_buf[25]='\r';

    //GPRS_SendString(SEND_BUF);
    GPRS_Send_HexData(tempsend_buf,26);
    GPRS_RecString_Data(Flash_Buffer1);
    
    if(strncmp(Flash_Buffer1+24,"NO SETTINGS FOUND",17)==0)
    {
        SERVER_READ_STATUS=11;
        STRCPY(LBUF,"No Settings found...");
        Disp_Msg(LINE5);
        Delay_250Ms(4);
        CLEAR_DATA_LINES_FROM(5,5);
        return 'S';
    }

    //if(strncmp(tempsend_buf,Flash_Buffer1,24)!=0 || Flash_Buffer1[93]!='!')//39+14//N:N111.11S222.22//55
    if(strncmp(tempsend_buf,Flash_Buffer1,24)!=0 || Flash_Buffer1[94]!='!')//39+14//N:N111.11S222.22//55

    {
        err_no=0x23;
        STRCPY(LBUF,"No Settings Received");
        Disp_Msg(LINE5);
        Delay_250Ms(2);
        CLEAR_DATA_LINES_FROM(5,5);
    }
    else
    {
        SERVER_READ_STATUS=11;
        SERVER_C_TYPE='S';
        if(SERVER_CONTROL!=Flash_Buffer1[24])
        {
            SERVER_CONTROL=Flash_Buffer1[24];

            SEERAMWriteString(Server_Control_Loc,1,(char *)&SERVER_CONTROL);
        }
        tempnob=7;
        tempsl=0;
        if(Flash_Buffer1[34]=='Y')
        {
            tempnob=5;
            tempsl=2;
        }
        if(SERVER_CONTROL=='N')
        {
            Write_Offline_Settings();
            return 'S';
        }
        else
        {
            if(SERVER_CONTROL=='Y')
            {
                SEERAMReadString(Weight_Mode_Loc,7,(char *)&c_temp_buf);

                c_temp_buf[0]=c_temp_buf[0]+0x30;
                c_temp_buf[1]=c_temp_buf[1]+0x30;

                if(strncmp(c_temp_buf+tempsl,Flash_Buffer1+25+tempsl,tempnob)!=0)
                {
                    Flash_Buffer1[25]=Flash_Buffer1[25]-0x30;
                    Flash_Buffer1[26]=Flash_Buffer1[26]-0x30;

                    SEERAMWriteString(Weight_Mode_Loc+tempsl,tempnob,(char *)&Flash_Buffer1[25+tempsl]);
                }

                SEERAMReadString(MemberList_Control_Loc,3,(char *)&c_temp_buf);
                if(strncmp(c_temp_buf,Flash_Buffer1+32,3)!=0)
                {
                    SEERAMWriteString(MemberList_Control_Loc,3,(char *)&Flash_Buffer1[32]);
                }
                SEERAMReadString(Auto_Transfer_Loc,1,(char *)&GPRSrec.AUTO_TRANSFER);

                if(GPRSrec.AUTO_TRANSFER!=Flash_Buffer1[35])
                {
                    GPRSrec.AUTO_TRANSFER=Flash_Buffer1[35];
                    SEERAMWriteString(Auto_Transfer_Loc,1,(char *)&GPRSrec.AUTO_TRANSFER);
                }

                SEERAMReadString(Shift_Close_Loc,1,(char *)&c_temp_buf);

                if(c_temp_buf[0]!=Flash_Buffer1[36])
                    SEERAMWriteString(Shift_Close_Loc,1,(char *)&Flash_Buffer1[36]);

                SEERAMReadString(Mixed_Milk_Loc,1,(char *)&c_temp_buf);

                if(c_temp_buf[0]!=Flash_Buffer1[37])
                    SEERAMWriteString(Mixed_Milk_Loc,1,(char *)&Flash_Buffer1[37]);

                SEERAMReadString(Machine_Lock_Loc,1,(char *)&MACHINE_LOCK);

                if(MACHINE_LOCK!=Flash_Buffer1[38])
                {
                    MACHINE_LOCK=Flash_Buffer1[38];
                    SEERAMWriteString(Machine_Lock_Loc,1,(char *)&MACHINE_LOCK);
                }
                
                SEERAMReadString(CLR_Based_Table_Loc,1,(char *)&clrBasedTable);
                
                if(clrBasedTable != Flash_Buffer1[93])
                {
                    clrBasedTable = Flash_Buffer1[93];
                    SEERAMWriteString(CLR_Based_Table_Loc,1,(char *)&clrBasedTable);
                    SEERAMReadString(CLR_Based_Table_Loc,1,(char *)&CLR_BASED);

                }

                Read_Settings();
                tempCommissionrec.STATUS=Flash_Buffer1[39];
                Flash_Buffer1[47]='\0';
                tempCommissionrec.AUTO_RATE=Float_Long(Flash_Buffer1+42);
               
                for(tmp_incent_cnt=0;tmp_incent_cnt<9;tmp_incent_cnt++)
                {
                    strncpy(Temp_Arr,Flash_Buffer1+48+(tmp_incent_cnt*5),5);
                    Temp_Arr[5]='\0';                   
                    tempCommissionrec.SPECIAL_RATE[tmp_incent_cnt]=Float_Long(Temp_Arr);
                }
                
                if(tempCommissionrec.STATUS!=Commissionrec.STATUS || tempCommissionrec.AUTO_RATE!=Commissionrec.AUTO_RATE || tempCommissionrec.SPECIAL_RATE!=Commissionrec.SPECIAL_RATE)
                {
                    SEERAMWriteString(Auto_Commission_Loc,Commission_Length,(char *)&tempCommissionrec);
                }                
            }
        }
        //#SCTSERVERVALUE2:SCT0001Y11UYYYYYYYYYYNY:000.50S001.00!
        //#SCTSERVERVALUE2:SCT0001Y11UYYYYYYYYYYNY:N000.50S001.00!
        //#SCTSERVERVALUE2:SCT0001Y11UYYYYYYYYYYNY:N00.50S01.0002.0003.0004.0005.0006.0007.0008.0009.00!
        //#SCTSERVERVALUE2:SCT0001Y11UYYYYYYYYYYNY:N00.50S01.0002.0003.0004.0005.0006.0007.0008.0009.00Y!
    }
    if(SERVER_C_TYPE=='B' && SERVER_READ_STATUS>0 && SERVER_READ_STATUS<5)
        return err_no;

    SERVER_C_TYPE='S';

    if(SERVER_CONTROL=='Y')
    {
        SEERAMReadString(MemberList_Control_Loc,3,(char *)&DPUcontrolrec);
        if(DPUcontrolrec.RATE_TABLES=='Y')
            return 'S';
        else
        {            
            Server_Menu_Header('R');

            STRCPY(tempsend_buf,"#SCTRATECHARTIDS:");
            STRNCPY(tempsend_buf+17,MACHINE_ID,7);
          
            EEPROM_ADDR=Cow_Fat_Rate_Id_Loc;
            SEERAMReadString(EEPROM_ADDR,8,(char *)&rate_chart_ids);
          /*  
            EEPROM_ADDR=Buf_Fat_Rate_Id_Loc;
            SEERAMReadString(EEPROM_ADDR,2,(char *)&buf_fat_id);

            EEPROM_ADDR=Cow_Fat_Rate_Id_Loc;
            SEERAMReadString(EEPROM_ADDR,2,(char *)&cow_fat_id);

            EEPROM_ADDR=Buf_Snf_Rate_Id_Loc;
            SEERAMReadString(EEPROM_ADDR,2,(char *)&buf_snf_id);

            EEPROM_ADDR=Cow_Snf_Rate_Id_Loc;
            SEERAMReadString(EEPROM_ADDR,2,(char *)&cow_snf_id);*/
            
            ZERO=0;
           
            Hex_DEC(0,rate_chart_ids[0],tempsend_buf+24,4);//cow fat
            Hex_DEC(0,rate_chart_ids[1],tempsend_buf+28,4);//buf fat
            Hex_DEC(0,rate_chart_ids[2],tempsend_buf+32,4);//cow snf
            Hex_DEC(0,rate_chart_ids[3],tempsend_buf+36,4);//buf snf
            
            tempsend_buf[40]='!';
            tempsend_buf[41]='\r';

            //GPRS_SendString(tempsend_buf);
            GPRS_Send_HexData(tempsend_buf,42);
            GPRS_RecString_Data(Flash_Buffer1);

            if(strncmp(tempsend_buf,Flash_Buffer1,24)!=0)
                return 0x21;

            if(strncmp(tempsend_buf,Flash_Buffer1,40)==0)
            {
                Server_Rate_Chart_Flag=1;
                No_New_Chart_Msg();
                Delay_250Ms(4);
                return 'S';
            }
            else
            {
                if(strncmp(tempsend_buf+36,Flash_Buffer1+36,4)==0)
                {
                    buf_snf_id=0;
                }
                else
                {
                    Flash_Buffer1[40]=0x00;
                    buf_snf_id=atoi(Flash_Buffer1+36);
                }

                if(strncmp(tempsend_buf+32,Flash_Buffer1+32,4)==0)
                {
                    cow_snf_id=0;
                }
                else
                {
                    Flash_Buffer1[36]=0x00;
                    cow_snf_id=atoi(Flash_Buffer1+32);
                }

                if(strncmp(tempsend_buf+28,Flash_Buffer1+28,4)==0)
                {
                    buf_fat_id=0;
                }
                else
                {
                    Flash_Buffer1[32]=0x00;
                    buf_fat_id=atoi(Flash_Buffer1+28);
                }

                if(strncmp(tempsend_buf+24,Flash_Buffer1+24,4)==0)
                {
                    cow_fat_id=0;
                }
                else
                {
                    Flash_Buffer1[28]=0x00;
                    cow_fat_id=atoi(Flash_Buffer1+24);
                }
            }
            if(cow_fat_id!=0)
            {
                MODEM_ERROR=Receive_Fat_Table('C');//1
                if(MODEM_ERROR!='S')
                {
                    return MODEM_ERROR;
                }
            }
            if(buf_fat_id!=0)
            {
                MODEM_ERROR=Receive_Fat_Table('B');//2
//                if(MODEM_ERROR!='S')
//                {
//                    return MODEM_ERROR;
//                }
            }
            if(cow_snf_id!=0)
            {
                MODEM_ERROR=Receive_SNF_Table('C');
               if( MODEM_ERROR!='S')
               {
                   return MODEM_ERROR;
               }
            }
            if(buf_snf_id!=0)
            {
                MODEM_ERROR=Receive_SNF_Table('B');
                if(MODEM_ERROR!='S')
                {
                    return MODEM_ERROR;
                }
            }

            return 'S';
        }
    }
}
