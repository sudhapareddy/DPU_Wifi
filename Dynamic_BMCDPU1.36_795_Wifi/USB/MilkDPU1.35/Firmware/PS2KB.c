#include <Scancodes.h>

void __ISR(_TIMER_1_VECTOR) _Timer1Handler(void)//, ipl5
{
   mT1ClearIntFlag();
   
    TimerCount++;
    RTC_Counter++;
    SECONDS_COUNTER++;
    MODEM_COUNTER++;
    
    DISP_INTERVAL_CNT++;
    
    if(DISP_INTERVAL_CNT>=10)
        DISP_INTERVAL_CNT=0;

    
    if(MODEM_COUNTER>25)
    {
        MODEM_STABLED=1;
    }

    if(TimerCount>=Max_Timer_Count)
    {
        No_Response=1;
        TimerCount=0;
    }
    
    if(RTC_Counter>=2)
    {
        RTC_Response=1;
        RTC_Counter=0;
    }
    if(SECONDS_COUNTER>=10)
        SECONDS_COUNTER=0;
 
    Bit_Count=11;
    if(LED_Blink==1)
        PWRLED =~PWRLED;
    
    else
        PWRLED=0;
    
    if(CURSOR_FLAG==1)
    {
        DISP_CURSOR_FLAG=1;
        if(CURSOR_BLINK==0x00)
            CURSOR_BLINK=TEMP_CURSOR_BLINK;
        else
            CURSOR_BLINK=0;
    }
 } 

void __ISR(_TIMER_3_VECTOR ) _Timer2Handler(void)//,ipl6
{
    mT3ClearIntFlag();
    if(A1DELAYCOUNT<10)
        A1DELAYCOUNT++;
    
    if(A2DELAYCOUNT<10)
        A2DELAYCOUNT++;
    
    if(GPRSDELAYCOUNT<50)
        GPRSDELAYCOUNT++;   

    if(Settingsrec.ANALYZER=='U')
    {
        if(BYTEGAPCOUNT>2)
           I_Index_Count=0;
       BYTEGAPCOUNT++;  

       if(BYTEGAPCOUNT2>2)
           I_Index_Count2=0;
       BYTEGAPCOUNT2++;  
    }
 }

void __ISR(_EXTERNAL_2_VECTOR) INT2Interrupt()//, ipl1
{
    mINT2ClearIntFlag();

    TimerCount=0;
    LCDBL=0;
    
    if(Bit_Count<11 && Bit_Count>2)
    {
        SCAN_REC_DATA=SCAN_REC_DATA>>1;

        if(PS2_DATA==1)
            SCAN_REC_DATA=SCAN_REC_DATA|0x80;
    }

    if(--Bit_Count==0)
    {
        Bit_Count=11;
        Prev_Data=Pres_Data;
        Pres_Data=SCAN_REC_DATA;

        if(Pres_Data==0xE0)
        {
            Ext_Code==1;
            Key_Int_Flag=0;
        }
        else if(Pres_Data==0xF0 && Prev_Data==0xE0)
        {
            Ext_Code=0;
            Key_Int_Flag=0;
        }
        else if(Prev_Data==0xE0 && Pres_Data!=0xF0)
        {
            Ext_Code=1;
            Key_Int_Flag=1;
        }
        else if(Prev_Data!=0xF0 && Pres_Data!=0XF0 && Pres_Data!=0xE0 && Prev_Data!=0xE0)
        {
            Ext_Code=0;
            Key_Int_Flag=1;
        }     
    }
}

void Init_Kbd(void)
{
    DATA_DIR=1;

    Bit_Count=11;
    SCAN_REC_DATA=0x00;
    PRESSED_KEY=0x00;

    Key_Press=0;
    Key_Int_Flag=0;
    Caps_Lock=0;

    ConfigINT0(EXT_INT_DISABLE);
    ConfigINT2(EXT_INT_PRI_1 | FALLING_EDGE_INT | EXT_INT_ENABLE);
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, 0xFFFF);
    
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_5);

    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_5);
    
    PS2_DATA=1;

    Ext_Code=0;
    Shift_Lock=0;    
}

void Enable_Timer3(void)
{
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_4 | T2_32BIT_MODE_ON,0xE100);
    OpenTimer3(T3_ON | T3_SOURCE_INT | T3_PS_1_8 ,0x05F5);
    ConfigIntTimer3(T3_INT_ON | T3_INT_PRIOR_6);
}

/*void Disable_Timer3(void)
{
    OpenTimer2(T2_OFF | T2_SOURCE_INT | T2_PS_1_4 | T2_32BIT_MODE_ON,0xE100);
    OpenTimer3(T3_OFF | T3_SOURCE_INT | T3_PS_1_8 ,0x05F5);
    ConfigIntTimer3(T3_INT_OFF | T3_INT_PRIOR_6);
}*/

UINT8 Decode(UINT8 dkeytype)
{
    Scan_Code=Pres_Data;

    if(Ext_Code==1)
        dkeytype='E';
    SHIFT_SEARCH=0;

    switch(Scan_Code)
    {
        case 0x12:

            Shift_Lock=1;
            
           /*version1.06*/
            if(MODE_FLAG==1 && Ext_Code==0)
            {
                SHIFT_SEARCH=1;
                Shift_Lock=0;
                
                CURSOR_FLAG=0;
                CURSOR_BLINK=0x00;
                DISP_CURSOR_FLAG=0;
            }
           
            Key_Press=1;
            PRESSED_KEY=0xFF;
            break;

        case 0x59:

            Shift_Lock=1;
            /*version1.06*/
            if(MODE_FLAG==1 && Ext_Code==0)
            {
                SHIFT_SEARCH=1;
                Shift_Lock=0;
                
                CURSOR_FLAG=0;
                CURSOR_BLINK=0x00;
                DISP_CURSOR_FLAG=0;               
            }
            Key_Press=1;
            PRESSED_KEY=0xFF;
            break;

        case 0x58:

            if(Caps_Lock==0)
                Caps_Lock=1;

            else if(Caps_Lock==1)
                Caps_Lock=0;

            Key_Press=1;
            PRESSED_KEY=0xFF;

            break;

        default:

            Key_Press=0;
            K_Index_Cnt=0;
            Unmatch=0;

            do
            {
                switch(dkeytype)
                {
                    case 'N':

                        if(Numeric[K_Index_Cnt][0]==Scan_Code && Ext_Code==0)
                        {
                            PRESSED_KEY=Numeric[K_Index_Cnt][1];
                            K_Index_Cnt=0;
                            Key_Press=1;
                            return PRESSED_KEY;
                        }
                        else if(Numeric[K_Index_Cnt][0]==0 && Ext_Code==0)
                        {
                            K_Index_Cnt=0;
                            Key_Press=0;
                            dkeytype='F';
                        }

                        break;

                    case 'A':

                        if(Unmatch==0 && Ext_Code==0)
                        {
                            switch(Shift_Lock)
                            {
                                case 0:

                                    if(Unshifted[K_Index_Cnt][0]==Scan_Code)
                                    {
                                        PRESSED_KEY=Unshifted[K_Index_Cnt][1];
                                        Key_Press=1;
                                        return PRESSED_KEY;
                                    }

                                    break;

                                case 1:

                                    if(Shifted[K_Index_Cnt][0]==Scan_Code)
                                    {
                                        Shift_Lock=0;
                                        PRESSED_KEY=Shifted[K_Index_Cnt][1];
                                        Key_Press=1;
                                        return PRESSED_KEY;
                                    }
                                    break;
                            }
                            if(Unshifted[K_Index_Cnt][0]==0 || Shifted[K_Index_Cnt][0]==0)
                            {
                                K_Index_Cnt=0;
                                PRESSED_KEY=0x00;
                                Key_Press=0;
                                Unmatch=1;
                            }
                        }
                        if(Unmatch==1 && Ext_Code==0)
                        {
                            if(Alpha[K_Index_Cnt][0]==Scan_Code)
                            {
                                PRESSED_KEY=Alpha[K_Index_Cnt][1];

                                if(Shift_Lock==1)
                                {
                                    if(Caps_Lock==1)
                                        PRESSED_KEY=PRESSED_KEY;

                                    else if(Caps_Lock==0)
                                        PRESSED_KEY=PRESSED_KEY-0x20;
                                }
                                else if(Caps_Lock==1)
                                    PRESSED_KEY=PRESSED_KEY-0x20;

                                Key_Press=1;
                                Shift_Lock=0;
                                return PRESSED_KEY;
                            }

                            if(Alpha[K_Index_Cnt][0]==0)
                            {
                                Shift_Lock=0;
                                PRESSED_KEY=0xFF;
                                Key_Press=1;
                                return 0xFF;
                            }
                        }
                        break;

                    case 'E':

                        if(Ext_Code==1 && External[K_Index_Cnt][0]==Scan_Code)
                        {
                            Ext_Code=0;
                            PRESSED_KEY=External[K_Index_Cnt][1];
                            Key_Press=1;
                            return PRESSED_KEY;
                        }
                        else if(External[K_Index_Cnt][0]==0)
                        {
                            K_Index_Cnt=0;
                            dkeytype='F';
                        }
                        break;

                    case 'C':

                        if( Esc_Enter[K_Index_Cnt][0]==Scan_Code)
                        {
                            Ext_Code=0;
                            PRESSED_KEY=Esc_Enter[K_Index_Cnt][1];
                            Key_Press=1;
                            return PRESSED_KEY;
                        }
                        else if(Esc_Enter[K_Index_Cnt][0]==0)
                        {
                            Ext_Code=0;
                            PRESSED_KEY=0xFF;
                            Key_Press=1;
                            return 0xFF;
                        }
                        break;

                    case 'F':

                        if(FunKeys[K_Index_Cnt-1][0]==Scan_Code)
                        {
                            Ext_Code=0;
                            PRESSED_KEY=FunKeys[K_Index_Cnt-1][1];
                            Key_Press=1;
                            return PRESSED_KEY;
                        }
                        else if(FunKeys[K_Index_Cnt-1][0]==0)
                        {
                            Ext_Code=0;
                            PRESSED_KEY=0x00;
                            Key_Press=0;
                            return 0xFF;
                        }
                        break;
                }

                K_Index_Cnt++;

            }while(1);
    }
}

void Update_AnalyzerData(UINT8 uanalyzerno)
{
    UINT8 usesnf,temp_disp_cursor,temp_col_cursor;
    BYTE temprecbuf[200];
    UINT16 u_member_sno;
    UINT32 u_eeprom_adr;
    UINT8 tempflag;
    
    if(uanalyzerno==1)
    {        
        STRNCPY(temprecbuf,REC_BUF,A_No_Bytes);
    }
    if(uanalyzerno==2)
    {
        STRNCPY(temprecbuf,REC_BUF2,A_No_Bytes2);
    }
    Calculate_Analyzer_Data(temprecbuf);
       
    if(CollectionRecord.FAT>0  && CollectionRecord.FAT<240 && JUNK_DATA==0)
    {
        temp_disp_cursor=CURSOR_FLAG;
        CURSOR_FLAG=0;
        temp_col_cursor=CURSOR_BLINK;
        CURSOR_BLINK=0x00;
        
        if(Analyzer_Header_Display_Flag==0)
        {            
            STRCPY(LBUF,"S.No CODE  FAT  SNF");
            CLR_Enabled(LBUF+16);
                      
            Invert_Disp_Msg(LINE6);
            Display_Bar_Top_LF(LINE6+4);
            Display_Bar_Top_LF(LINE6+10);
            Display_Bar_Top_LF(LINE6+15);

            CLEAR_BUF_FLAG=1;
            STRCPY(LBUF,"                    ");
            LBUF[4]=LBUF[10]=LBUF[15]=0x90;
            Disp_Msg(LINE7);
            Disp_Msg(LINE8);
            CLEAR_BUF_FLAG=0;
            Analyzer_Header_Display_Flag=1;
        }
        if(WEIGHT_COUNT>ANALYZER_COUNT)
        {            
            u_eeprom_adr=Sample_Records_Loc+(ANALYZER_COUNT*Current_Shift_Record_Length);//17
            FLASHReadString(u_eeprom_adr,8,(char *)&CollectionRecord);

            u_member_sno=Read_Member_SNO(CollectionRecord.CODE);
            Read_Member_Name(u_member_sno);
            Read_Member_Comm_Type(u_member_sno);
           
            Memberrec.MILK_TYPE='C';
            tempflag=CollectionRecord.FLAGS & 0x01;
            
            if(tempflag==0)
                Memberrec.MILK_TYPE='B';
            
             if(Memberrec.MILK_TYPE=='C')
                usesnf=Settingsrec.COW_SNF;

            if(Memberrec.MILK_TYPE=='B')
                usesnf=Settingsrec.BUF_SNF;
            
            if(Settingsrec.ANALYZER=='M' || Settingsrec.ANALYZER=='F')
                   usesnf='N';
            
            Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;
            
            Calculate_Rate(usesnf);
          
            Write_Analyzer_Data();
            
            Read_Collection_Flags();
            
            if(uanalyzerno==1)
                Set_Cursor(LINE8+0);

            else
                Set_Cursor(LINE7+0);
            ZERO=1;
            INVERT_DISPLAY=0;
            Display_Char(' ');
            LBUF[0]=' ';
            Hex_DEC(0,ANALYZER_COUNT,LBUF+1,3);
            Shift_Left(LBUF+1,3);
            Hex_DEC(0,CollectionRecord.CODE,LBUF+5,4);            
            LBUF[9]=CollectionFlags.MILK_TYPE;
            Shift_Left(LBUF+5,5);
            Hex_DEC(1,CollectionRecord.FAT,LBUF+11,4);
            Shift_Left(LBUF+11,4);
            Hex_DEC(1,CollectionRecord.SNF,LBUF+16,4); 
            Shift_Left(LBUF+16,4);
            
            if(Analyzer_Header_Display_Flag==1)
            {
                LBUF[4]=LBUF[10]=LBUF[15]=0x90;
            }
            
           

            if(uanalyzerno==1)
            {
                LBUF[0]=0x7E;
                Disp_Msg(LINE7);
            }

            if(uanalyzerno==2)
            {
                LBUF[0]=0x7E;
                Disp_Msg(LINE8);
            }
          
            Send_Collection_Ext_Display();
            if(Settingsrec.Collection_Rec_Ext=='N')
                ;

            else if(Settingsrec.Collection_Rec_Ext=='E')
                Ext_Print_Receipt('C');
            else
                Print_Receipt('C');
            Delay_Sec(1);
            Display_Counters();
        }
        
        
        CURSOR_BLINK=temp_col_cursor;
        CURSOR_FLAG=temp_disp_cursor;
    }

    INVERT_DISPLAY=0;
}

void Update_Analyzer_Inrerrupt_Data(UINT8 analyzerno)
{
    if(Settingsrec.FAT_MODE==1)
    {
        Read_RTC();

        TempTablerec=Tablerec;
        tempmemberrec=Memberrec;
        TempRecord=CollectionRecord;
        TempFlags=CollectionFlags;
        
        Update_AnalyzerData(analyzerno);

        Memberrec=tempmemberrec;
        CollectionRecord=TempRecord;
        CollectionFlags=TempFlags;
        Tablerec=TempTablerec;
    }
}

UINT8 Scan_Key(UINT8 scantype)
{
    UINT8 upload_cnt_change,sms_cnt_change,temp_col_cursor,temp_disp_cursor;
    UINT16 s_tempuploadcount,s_member_sno,temp_upload_count,prev_sms_cnt;    

    LED_Blink=0;
    
    if(MODEM_STABLED==1)
    {
        if(SIM_INSERTED==0)
            Modem_Init_Flag=0;

#ifndef NORMAL
        if(BACKEND_FLAG=='S' && SMS_STATE==0 && SERVER_STATE==0)
          Display_Modem_STATUS();
#endif
      
        if(RunningShift.DAY_SHIFT==Shiftrec.DAY_SHIFT)
        {

            if(CollectionFlag==1 && Update_Analyzer_Flag==1 && TWO_ANALYZERS=='Y')
            {
                Update_Analyzer_Inrerrupt_Data(1);
                Update_Analyzer_Flag=0;
            }
            if(CollectionFlag==1 && Update_Analyzer_Flag2==1 && TWO_ANALYZERS=='Y')
            {
                Update_Analyzer_Inrerrupt_Data(2);
                Update_Analyzer_Flag2 =0;
            }
        }
        else if(CollectionFlag==0 && TWO_ANALYZERS=='Y')
        {
            if(Update_Analyzer_Flag==1 && A1DELAYCOUNT<2)
            {
                A1DELAYCOUNT=0;
                Update_Analyzer_Flag=0;
            }
            if(Update_Analyzer_Flag2==1 && A2DELAYCOUNT<2)
            {
                A2DELAYCOUNT=0;
                Update_Analyzer_Flag2=0;
            }
        }

        if(ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y' )
        {
            if(CollectionFlag==0 && BACKEND_FLAG=='S' && SIM_INSERTED==1 && Modem_Init_Flag==1 && (SERVER_READ_STATUS>=0 && SERVER_READ_STATUS<5) &&  SMS_STATE==0 && SERVER_STATE==0 &&  INIT_MODEM_STATE==0 && SERVER_CONTROL=='Y')
            {
                SERVER_READ_STATUS=SERVER_READ_STATUS+1;
                
                Server_Communication(1);
                Clear_LCD();
                Display_Counters();
                BACKEND_FLAG='S';
                MODEM_STABLED=0;
                MODEM_COUNTER=0;
            }
#ifdef GPRS

            if(GPRSrec.AUTO_SMS=='Y' && Modem_Init_Flag==1 && ANALYZER_COUNT>SMS_COUNT && SMS_COUNT<1000  && (BACKEND_FLAG=='B' || BACKEND_FLAG=='S') && SERVER_STATE==0  )
            {
                TempRecord=CollectionRecord;//1
                TempFlags=CollectionFlags;
                tempmemberrec=Memberrec;                
                TEMPEEPROM_ADR=EEPROM_ADDR;
                TempShift=Shiftrec;
                TempCollectionRecord_NO=CollectionRecord_NO;                
                Read_Current_Shift_Col_Record(SMS_COUNT);
                
                Read_Collection_Flags();
                CollectionRecord_NO.Type='L';
                CollectionRecord_NO.RECORDNO=SMS_COUNT;//SMS_COUNT+1;                
                s_member_sno=Read_Member_SNO(CollectionRecord.CODE);
                prev_sms_cnt=SMS_COUNT;
                sms_cnt_change=0;                
                
                if(CollectionFlags.SMS_STATUS==0)
                    SMS_COUNT=SMS_COUNT+1;
                else
                {
                    Send_SMS_BackEnd('C',s_member_sno);

                }

                Memberrec=tempmemberrec;
                CollectionRecord=TempRecord;
                Shiftrec=TempShift;
                CollectionFlags=TempFlags;
                CollectionRecord_NO=TempCollectionRecord_NO;
                
                EEPROM_ADDR=TEMPEEPROM_ADR;
                
                if(SMS_COUNT>prev_sms_cnt)
                    sms_cnt_change=1;
                
                if(sms_cnt_change==1)
                {
                    sms_cnt_change=0;
                    temp_disp_cursor=CURSOR_FLAG;
                    CURSOR_FLAG=0;
                    temp_col_cursor=CURSOR_BLINK;
                    CURSOR_BLINK=0x00;

                    Display_Counters();

                    CURSOR_BLINK=temp_col_cursor;
                    CURSOR_FLAG=temp_disp_cursor;
                }              
            }
#endif
            else if((GPRSrec.AUTO_TRANSFER=='Y' && Modem_Init_Flag==1 && ANALYZER_COUNT> UPLOAD_COUNT && UPLOAD_COUNT<1000 && (BACKEND_FLAG=='S' || BACKEND_FLAG=='B') && SMS_STATE==0 && SIM_CHECK_STATE ==0 && INIT_MODEM_STATE==0) || SERVER_STATE!=0)
            {
                //if(MACHINE_ADDED=='Y')
                {
                    tempmemberrec=Memberrec;
                    TempRecord=CollectionRecord;
                  
                    TEMPEEPROM_ADR=EEPROM_ADDR;
                    TempShift=Shiftrec;
                    TEMPMEMBERSNO=Member_SNo;
                    TempCollectionRecord_NO=CollectionRecord_NO; 
                    temp_upload_count=UPLOAD_COUNT;
                    upload_cnt_change=0;
                    if(SERVER_STATE==10 &&  BACKEND_RECORD_COUNT!=0 && MODEM_ERROR=='S')
                    {
                        UPLOAD_COUNT=UPLOAD_COUNT+BACKEND_RECORD_COUNT+PREV_U_COUNT;
                        
                        DU_COUNT=DU_COUNT+BACKEND_RECORD_COUNT;
                        BACKEND_RECORD_COUNT=0;
                    }                    
                    
                    if(SERVER_STATE==0)
                    {
                        BACKEND_RECORD_COUNT=0;
                       
                        Sector_Erase(TempUpload_Loc);
                        STRCPY(BACKEND_RECORDS_BUF,"#SCTSYNCRECORDS:");
                        STRNCPY(BACKEND_RECORDS_BUF+16,MACHINE_ID,7);
                        BACKEND_RECORDS_BUF[23]=1;
                        s_tempuploadcount=UPLOAD_COUNT;
                        PREV_U_COUNT=0;
                        
                        while(ANALYZER_COUNT>s_tempuploadcount && BACKEND_RECORD_COUNT<=20)
                        {
                            Read_Current_Shift_Col_Record(s_tempuploadcount);

                            Read_Collection_Flags();
                            SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&Shiftrec);
                            Read_Member_SNO(CollectionRecord.CODE);
                            Read_Member_Details(s_member_sno);

                            if(CollectionFlags.UPLOAD_STATUS==0)
                                PREV_U_COUNT=PREV_U_COUNT+1;

                            else if(CollectionRecord.QTY!=0xFFFF && CollectionRecord.FAT!=0xFF)
                            {
                                Add_Collection_Record(BACKEND_RECORD_COUNT,BACKEND_RECORDS_BUF);
                                CollectionRecord_NO.Type='L';
                                CollectionRecord_NO.RECORDNO=s_tempuploadcount;//+1;
                                CollectionRecord_NO.FLASH_NO=0;

                                TempUpdate_Upload_Flag(BACKEND_RECORD_COUNT);//,s_member_sno);
                                BACKEND_RECORD_COUNT++;
                            }
                            else
                               PREV_U_COUNT=PREV_U_COUNT+1;

                            s_tempuploadcount++;
                        }
                        BACKEND_RECORDS_BUF[23]=BACKEND_RECORD_COUNT;
                        BACKEND_RECORDS_BUF[24+(BACKEND_RECORD_COUNT*Upload_Record_Length)]='!';//23//27
                    }
                    Read_Collection_Flags();
                    if(BACKEND_RECORD_COUNT!=0  || SERVER_STATE!=0)    
                    {
                        Connect_Server_Backend();
                    }
                    Memberrec=tempmemberrec;
                    CollectionRecord=TempRecord;
                    Shiftrec=TempShift;
                   
                    EEPROM_ADDR=TEMPEEPROM_ADR;
                    CollectionRecord_NO=TempCollectionRecord_NO; 
                    Member_SNo=TEMPMEMBERSNO;
                    
                    if(UPLOAD_COUNT>temp_upload_count)
                    {
                        upload_cnt_change=1;
                    }
                   
                    if(upload_cnt_change==1)
                    {
                        upload_cnt_change=0;
                   
                        temp_disp_cursor=CURSOR_FLAG;
                        CURSOR_FLAG=0;
                        temp_col_cursor=CURSOR_BLINK;
                        CURSOR_BLINK=0x00;

                        Display_Counters();

                        CURSOR_BLINK=temp_col_cursor;
                        CURSOR_FLAG=temp_disp_cursor;                       
                    }
                }
            }
#ifdef WIFI
//            else if((BACKEND_FLAG=='S' || BACKEND_FLAG=='B') && (Modem_Init_Flag!=1 || INIT_MODEM_STATE!=0))
//            {
//               Init_Wifi_Modem();                
//
//            }
//            else if((BACKEND_FLAG=='S' || BACKEND_FLAG=='B') && (Modem_Init_Flag==1 &&  SERVER_STATE==0 && INIT_MODEM_STATE==0 && BACKEND_FLAG=='S') || SIM_CHECK_STATE !=0)
//            {
//                  
//                Check_Wifi_Modem();
//            }
            // 1) Backend needs Wi-Fi but modem not ready OR init in progress → drive Init_Wifi_Modem
if ((BACKEND_FLAG == 'S' || BACKEND_FLAG == 'B') &&
    (Modem_Init_Flag != 1 || INIT_MODEM_STATE != 0))
{
    Init_Wifi_Modem();
}
// 2) Backend needs Wi-Fi AND modem is ready + idle, OR we are mid health-check → drive Check_Wifi_Modem
else if (((BACKEND_FLAG == 'S' || BACKEND_FLAG == 'B') &&
          (Modem_Init_Flag == 1 && SERVER_STATE == 0 && INIT_MODEM_STATE == 0)) ||
         (SIM_CHECK_STATE != 0))
{
    Check_Wifi_Modem();
}

#elif defined(GPRS)
            else if((SIM_INSERTED==1 && Modem_Init_Flag!=1 && SIM_CHECK_STATE ==0) || INIT_MODEM_STATE!=0)
            {                   
                Init_GPRS_Modem();

            }
            else if((CHECK_SIM_FLAG==1 &&  SMS_STATE==0 && SERVER_STATE==0 && INIT_MODEM_STATE==0 && BACKEND_FLAG=='S') || SIM_CHECK_STATE !=0 || SIM_INSERTED==0 )
            {                
                SIM_Check();

                if(SIM_CHECK_STATE==0)
                    CHECK_SIM_FLAG=0;
            }
#endif
        }
    }
    if(Key_Int_Flag==1)
    {
        if(MONITOR_VOLTAGE=='N')
          DISABLE_VOL_BUZ=1;
        if(Decode(scantype)==0xFF)
        {
            SCAN_REC_DATA=0x00;
            Key_Int_Flag=0;

            return 0xFF;
        }

        Key_Int_Flag=0;
    }

    if(Key_Press==1)
    {
        PS2_KEYBOARD=1;
        Key_Press=0;
        if(PRESSED_KEY==0xFD)
        {
            Send_Tare_Command();
            return 0xFF;
        }

        return PRESSED_KEY;
    }    
    
    else
        return 0xFF;
}

UINT8  KEY_ASCII(UINT8 keyflag,UINT8 keyvalue)
{
    if(keyvalue>=0x30 && keyvalue<=0x39)
        return keyvalue;

    else if(keyflag!=0 && keyvalue==0x2E)		//DOT(.)
        return 0x2E;

    else
        return 0xFF;
}	

UINT8 KEY_ALPHA(UINT8 alphavalue)
{
    if(alphavalue>=0x20 && alphavalue<=0x7E)
        return alphavalue;
    else
        return 0xFF;
}	

UINT8 Function_Keys(UINT8 fvalue)
{
    UINT8 int_ext_printer,gstatus;

    SEERAMReadString(Default_Printer_Loc,1,(char *)&int_ext_printer);
    
    SEERAMReadString(Accuracy_Loc,WM_RECORD_LENGTH,(char *)&WMrec);

    if(int_ext_printer=='Y')
        int_ext_printer='T';
    else
        int_ext_printer='E';

    if((fvalue >=0xF1 && fvalue<=0xFD)|| (fvalue>=0xA5 && fvalue<=0xA8))
    {
        //STRCPY(SEND_BUF,);
        SP_SendString("$??",3);
        Big_RTC=0;
    }
    
    switch(fvalue)
    {
        case 0xF1:
            
            MODE_FLAG=1;
            
            SEERAMReadString(Weight_Mode_Loc,SETTINGS_RECORD_LENGTH,(char *)&Settingsrec);
            CollectionFlag=1;

            if(Settingsrec.FAT_MODE==1 && TWO_ANALYZERS=='Y')
                Milk_Collection_2Analyzers('C');

            else if(WEIGHT_COUNT==ANALYZER_COUNT)
                Milk_Collection('C');

            else
            {
                CollectionFlag=0;
                Milk_Collection('E');
            }
            Disable_UART_Interrupts_WM();
            MODE_FLAG=0;
            CollectionFlag=0;
            BACKEND_FLAG='B';
            
            return 0xFF;

        case 0xF2:
            
            if(LOGIN_TYPE=='O' && Dataentryrec.DEDUCTION==0)
                return;
            //Deduction(int_ext_printer);
            BACKEND_FLAG='W';
            WIFI_Selection(); 
            BACKEND_FLAG='B';
            return 0xFF;

        case 0xF3:

            if(LOGIN_TYPE=='O' && Dataentryrec.LOCAL_SALE==0)
                return;
            Local_Sale('C',int_ext_printer);
            return 0xFF;

        case 0xF4:
            
            if(LOGIN_TYPE=='O')
            {
                if(int_ext_printer=='E' && ExtPrintFunrec.ABSENT_REPORT==0)
                    return;
                if(int_ext_printer=='T' &&  PrintFunrec.ABSENT_REPORT==0)
                return;
            } 
            Absent_Members(int_ext_printer);
            return 0xFF;

        case 0xF5:

            if(LOGIN_TYPE=='O')
            {
                if(int_ext_printer=='E' && ExtPrintFunrec.DETAILED_REPORT==0)
                    return;
                if(int_ext_printer=='T' &&  PrintFunrec.DETAILED_REPORT==0)
                return;
            }
            Detailed_Shift_Report('d',int_ext_printer);
            return 0xFF;

        case 0xF6:
            
            if(LOGIN_TYPE=='O')
            {
                if(int_ext_printer=='E' && ExtPrintFunrec.SUMMARY_REPORT==0)
                    return;
                if(int_ext_printer=='T' &&  PrintFunrec.SUMMARY_REPORT==0)
                return;
            }
            
            Summary_Report('S','p',int_ext_printer);
            return 0xFF;

        case 0xF7:
            
            if(LOGIN_TYPE=='O' && ViewFunrec.V_PAYMENT==0)
                return;   
            
            TIME_FLAG=0;
            Memberwise_Report('w','V');
            TIME_FLAG=1;
           
            return 0xFF;

        case 0xF8:

            if(LOGIN_TYPE=='O' )
            {
                if(int_ext_printer=='E' && ExtPrintFunrec.PAYMENT_REGISTER==0)
                    return;
                if(int_ext_printer=='T' &&  PrintFunrec.PAYMENT_REGISTER==0)
                return;
            }
            
            Memberwise_Report('w',int_ext_printer);
            return 0xFF;

        case 0xF9:
            
            if(LOGIN_TYPE=='O' )
            {
                if(int_ext_printer=='E' && ExtPrintFunrec.MEMBER_BILL_REPORT==0)
                    return;
                if(int_ext_printer=='T' &&  PrintFunrec.MEMBER_BILL_REPORT==0)
                return;
            }
            //if(LOGIN_TYPE!='O')
            {
                gstatus=Wait_Backend_To_Complete();
                if(gstatus==0xFF)
                    return;                
                
                Memberwise_Report('W',int_ext_printer);
               
                BACKEND_FLAG='B';
            }
            return 0xFF;

        case 0xFA:							//F10         
           
            if(GPRSrec.GPRS_ON=='Y' && ON_LINE=='Y')
            {
                 if(LOGIN_TYPE=='O' && Dataentryrec.TRANSFER==0)
                return;
           
               Close_Shift(1);
               Check_New_RateChart();
            }
            else
            {
                if(LOGIN_TYPE=='O' && Dataentryrec.SANGH_SALE==0)
                    return;
            
                Print_LS_Report(int_ext_printer,'S');
            }
            
            return 0xFF;

        case 0xFB:							//F11

            if(LOGIN_TYPE=='O' && MemoryFunrec.MEMBER_DETAILS==0)
                return;
            
            FN_KEY=1;
                        
            //if(Password_Entry(LOGIN_TYPE)==1)
                Member_Details();
            
            return 0xFF;

        case 0xFC:							//F12
            
            if(LOGIN_TYPE=='O' && SettingMenurec.ADMIN==0)
                return;
            
            //if(Password_Entry(LOGIN_TYPE)==1)
            {
                FONT_TYPE='B';
                TIME_RIGHT_ARROW='Y';
                SetDate_Time(5);
                TIME_RIGHT_ARROW='N';
                FONT_TYPE='S';
            }

            return 0xFF;

        case 0xA5:

            //if(Password_Entry(LOGIN_TYPE)==1)
                //Modem_Restart();
           
            return 0xFF;

        case 0xA8:

            Balance_Command();
            return 0xFF;
            
        case KB_TARE:
            
            Send_Tare_Command();
            return 0xFF;

        case 0x20:

            Paper_Feed(1);
            return 0xFF;
         
        default:

            return fvalue;
    }
}

UINT8 Wait_Key_Press(UINT8 wkeytype)
{
    UINT8 w_key_value=0xFF;
    UINT8 backendstatus;

    SCAN_REC_DATA=0x00;
    
    KBV=0xFF;
    do
    {
        w_key_value = Scan_Key(wkeytype);
      
        if(wkeytype=='E' || ANY_KEY==1 || wkeytype=='N')
        {
            if(TIME_FLAG==1)
            {
                LED_Blink=1;
                if(Big_RTC==1)
                {
                    Display_RTC_Big();
                }
                else if(Big_RTC==0)
                    Display_RTC(LINE8);              
            }            
        }

    }while (w_key_value == 0xFF);

    if((wkeytype=='E' || wkeytype=='N')&& ((PS2_KEYBOARD==1 && (w_key_value>=0xF1 && w_key_value<=0xFD)) || w_key_value==0x20 || (w_key_value>=0xA5 && w_key_value<=0xA8)))
    {
        if(MENU_ACCESS==1)
        {
            backendstatus=BACKEND_FLAG;
            BACKEND_FLAG='B';
            TIME_FLAG=0;
            Big_RTC=0;
            w_key_value=Function_Keys(w_key_value);
            if(backendstatus=='S')
                Big_RTC=1;
            else
                Big_RTC=0;
            BACKEND_FLAG=backendstatus;
            
            Clear_LCD();
            TIME_FLAG=1;
        }
    }

    return w_key_value;
}

