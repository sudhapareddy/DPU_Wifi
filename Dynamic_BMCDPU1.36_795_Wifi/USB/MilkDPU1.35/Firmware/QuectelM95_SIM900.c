#define GPRS_RESPONSE_READY()  (GPRS_FLAG == 1 || No_Response == 1)

UINT8 SIM_Check(void)
{
    UINT8 o_index, o_length;

    switch (SIM_CHECK_STATE)
    {
        case 0:
            strcpy(GPRSSEND_BUF, "ATE0\r\n");
            GPRS_SendString(GPRSSEND_BUF);
            OK_ERROR = 'O';
            Max_Timer_Count = 10;
            Expected_Response = 'O';
            MODEM_STATUS = 0;
            Read_SMS_Counter();
            SIM_CHECK_STATE = 1;
            break;

        case 1:
            if (!(GPRS_FLAG || No_Response)) break;

            if (No_Response)
            {
                GPRS_SendByte('+');
                GPRS_SendByte('+');
                GPRS_SendByte('+');
                Modem_Flags_Reset();
                MODEM_PRESENCE = 0;
                SIM_CHECK_STATE = 8;
            }
            else
            {
                MODEM_STATUS = 1;
                MODEM_PRESENCE = 1;

                strcpy(GPRSSEND_BUF, "AT+CREG?\r");
                GPRS_SendString(GPRSSEND_BUF);
                Expected_Response = 'c';
                GPRS_FLAG = 0;
                Max_Timer_Count = 30;
                SIM_CHECK_STATE = 2;
            }
            break;

        case 2:
            if (!(GPRS_FLAG || No_Response)) break;

            if (No_Response || OK_ERROR == 'E')
            {
                if (No_Response) Modem_Flags_Reset();
                SIM_CHECK_STATE = 0;
            }
            else
            {
                if (strncmp(GPRS_BUF + 2, "+CREG: 0,1", 9) == 0 &&
                    (GPRS_BUF[11] == '1' || GPRS_BUF[11] == '5'))
                {
                    SIM_INSERTED = 1;
                    ROAMING = (GPRS_BUF[11] == '5') ? 1 : 0;

                    STRCPY(GPRSSEND_BUF, "AT+QSPN?\r"); // operator name
                    GPRS_SendString(GPRSSEND_BUF);
                    Expected_Response = 'c';
                    SIM_CHECK_STATE = 3;
                }
                else if (GPRS_BUF[9] == '0' && GPRS_BUF[11] == '0')
                {
                    STRCPY(GPRSSEND_BUF, "AT+CFUN=0\r");
                    GPRS_SendString(GPRSSEND_BUF);
                    Expected_Response = 'c';
                    SIM_INSERTED = 0;
                    SIM_PRESENT = 0;
                    Modem_Init_Flag = 0;
                    SIM_CHECK_STATE = 6;
                }
                else
                {
                    Max_Timer_Count = 5;
                    Expected_Response = 'O';
                    SIM_CHECK_STATE = 0;

                    if (GPRS_BUF[9] == '0' && GPRS_BUF[11] == '2') SIM_INSERTED = 2;
                    else if (GPRS_BUF[9] == '0' && GPRS_BUF[11] == '3') SIM_INSERTED = 3;
                    else SIM_INSERTED = 0;
                }
            }
            break;

        case 3:
            if (!(GPRS_FLAG || No_Response)) break;

            if (No_Response || OK_ERROR == 'E')
            {
                if (No_Response) Modem_Flags_Reset();
                SIM_CHECK_STATE = 0;
            }
            else
            {
                o_index = 10;
                o_length = 0;

                while (GPRS_BUF[o_index] != '\"' && o_index <= 100)
                {
                    Operator[o_length++] = GPRS_BUF[o_index++];
                }

                Operator[o_length] = '\0';
                for (; o_length <= 11; o_length++) Operator[o_length] = ' ';

                STRCPY(GPRSSEND_BUF, "AT+QCCID\r"); // SIM number
                GPRS_SendString(GPRSSEND_BUF);
                Expected_Response = 'c';
                SIM_CHECK_STATE = 4;
            }
            break;

        case 4:
            if (!(GPRS_FLAG || No_Response)) break;

            if (No_Response || OK_ERROR == 'E')
            {
                if (No_Response) Modem_Flags_Reset();
                SIM_CHECK_STATE = 0;
            }
            else
            {
                STRNCPY(SIM_NO, GPRS_BUF + 2, 20);
                TimerCount = 0;

                if (strncmp(SIM_NO, SMSrec.SIM_NO, 20) != 0)
                {
                    SMSrec.COUNTER = 0;
                    STRNCPY(SMSrec.SIM_NO, SIM_NO, 20);
                    Write_SMS_Counter();
                }

                STRCPY(GPRSSEND_BUF, "AT+CSQ\r"); // signal strength
                GPRS_SendString(GPRSSEND_BUF);
                Expected_Response = 'c';
                SIM_CHECK_STATE = 5;
            }
            break;

        case 5:
            if (!(GPRS_FLAG || No_Response)) break;

            if (No_Response || OK_ERROR == 'E')
            {
                if (No_Response) Modem_Flags_Reset();
                SIM_CHECK_STATE = 0;
            }
            else
            {
                GPRS_BUF[10] = '\0';
                Signal_Strength = atoi(GPRS_BUF + 8);
                SIM_CHECK_STATE = 0;
            }

            Expected_Response = 'O';
            GPRS_FLAG = 0;
            break;

        case 6:
            if (!(GPRS_FLAG || No_Response)) break;

            STRCPY(GPRSSEND_BUF, "AT+CFUN=1\r");
            GPRS_SendString(GPRSSEND_BUF);
            SIM_INSERTED = 0;
            SIM_PRESENT = 0;
            Modem_Init_Flag = 0;
            Expected_Response = 'O';
            SIM_CHECK_STATE = 7;
            break;

        case 7:
            if (!(GPRS_FLAG || No_Response)) break;

            if (No_Response || OK_ERROR == 'E')
            {
                SIM_CHECK_STATE = 0;
            }
            else
            {
                SIM_CHECK_STATE = 8;
                GPRS_FLAG = 0;
                GPRS_BYTE_COUNT = 0;
                TimerCount = 0;
                Expected_Response = 'c';
            }
            break;

        case 8:
            if (!(GPRS_FLAG || No_Response)) break;

            SIM_CHECK_STATE = 0;
            break;

        default:
            SIM_CHECK_STATE = 0;
            break;
    }

    return 0;
}



void Send_SCT_Close(void)
{
//    STRCPY(SEND_BUF,"#SCTCLOSE:");
//    STRNCPY(SEND_BUF+10,MACHINE_ID,7);
//    SEND_BUF[17]='!';
//    SEND_BUF[18]='\r';
//    GPRS_SendString(SEND_BUF); 
    Delay_250Ms(2);
    GPRS_SendByte('+');
    GPRS_SendByte('+');
    GPRS_SendByte('+');
    Delay_Sec(1);
    STRCPY(GPRSSEND_BUF,"AT+QICLOSE\r");
    GPRS_SendString(GPRSSEND_BUF);
    
                Expected_Response='O';

    
    //Expected_Response='O';
    //GPRS_RecString_OK();     
}


void Display_Modem_STATUS(void)
{
    if((ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y') && (Big_RTC==1 && STANDBY==1))
    {        
            if(SIM_INSERTED==1)
                SIM_PRESENT=1;

            if(SIM_INSERTED==1 && Modem_Init_Flag==1)
            {            
                LBUF[0]=0x8B;
                LBUF[1]=0x8C;
                ZERO=1;

                Read_SMS_Counter();
                Hex_DEC(0,(SMS_DAY_LIMIT-SMSrec.COUNTER),LBUF+2,3);
                Shift_Left(LBUF+2,3);
                LBUF[5]=' ';
                STRNCPY(LBUF+6,Operator,10);

               // LBUF[17]=' ';
                Hex_DEC(0,Signal_Strength,LBUF+15,2);
                if(Signal_Strength>12)
                {
                    if(Signal_Strength>9 && Signal_Strength<=14)
                    {
                        LBUF[17]=0x87;
                        LBUF[18-ROAMING]=0x88;
                    }

                    else if(Signal_Strength>14 && Signal_Strength<=19)
                    {
                        LBUF[17]=0x87;
                        LBUF[18]=0x89;
                    }

                    else if(Signal_Strength>19 && Signal_Strength<=40)
                    {
                        LBUF[17]=0x87;
                        LBUF[18]=0x8A;
                    }
                    if(ROAMING==1)
                        LBUF[19]='R';
                    else
                        LBUF[19]=' ';
                }
                else if(Signal_Strength>4)
                    STRCPY(LBUF+6," LOW SIGNAL");
                else
                    STRCPY(LBUF+6," NO SIGNAL");
            }

            else  if(MODEM_PRESENCE==0 && MODEM_STATUS==0)
                STRCPY(LBUF,"   CHECKING MODEM   ");

            else if(MODEM_PRESENCE==1 && SIM_PRESENT==0)
                STRCPY(LBUF,"    CHECKING SIM    ");

            else if(SIM_INSERTED==2)
                STRCPY(LBUF,"Searching Network...");

            else if(SIM_INSERTED==3)
                STRCPY(LBUF,"   Invalid SIM...   ");

            else 
                STRCPY(LBUF,"Initializing Modem  ");
            Disp_Msg(LINE6);
    }
}

UINT8 GPRS_No_Response_Error(void)
{
    if(No_Response==1 || OK_ERROR=='E')
    {
        if(No_Response==1)
             Modem_Flags_Reset();

        INIT_MODEM_STATE=0;
        return 1;
    }
    return 0;
}

UINT8 Init_GPRS_Modem(void)
{
    UINT8 ip_domain;

    switch (INIT_MODEM_STATE)
    {
        case 0:
            STRCPY(GPRSSEND_BUF, "AT+QIFGCNT=0\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 1;
            Max_Timer_Count = 10;
            break;

        case 1:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) return 0;
            STRCPY(GPRSSEND_BUF, "AT+QIMUX=0\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 2;
            break;

        case 2:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) {
                INIT_MODEM_STATE = 15;
                STRCPY(GPRSSEND_BUF, "AT+QIDEACT\r");
                GPRS_SendString(GPRSSEND_BUF);
                Expected_Response = 'C';
                break;
            }
            STRCPY(GPRSSEND_BUF, "AT+QIMODE=1\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 3;
            break;

        case 3:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) return 0;
            STRCPY(GPRSSEND_BUF, "AT+QITCFG=3,2,1460,1\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 4;
            break;

        case 4:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) return 0;
            STRCPY(GPRSSEND_BUF, "AT+QIREGAPP\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 5;
            break;

        case 5:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) return 0;
            SEERAMReadString(IP_Addr_Domain_Loc, 1, (char *)&ip_domain);
            STRCPY(GPRSSEND_BUF, "AT+QIDNSIP=0\r");
            if (ip_domain == 'N') GPRSSEND_BUF[11] = '1';
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 6;
            break;

        case 6:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) return 0;
            STRCPY(GPRSSEND_BUF, "AT+CSCS=\"GSM\"\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 7;
            break;

        case 7:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) return 0;
            STRCPY(GPRSSEND_BUF, "AT+CMGF=1\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 8;
            break;

        case 8:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) return 0;
            STRCPY(GPRSSEND_BUF, "AT+CNMI=2,1,0,1,0\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 9;
            break;

        case 9:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) return 0;
            STRCPY(GPRSSEND_BUF, "AT+CSMP=17,167,0,241\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 10;
            break;

        case 10:
            if (!GPRS_RESPONSE_READY()) break;
            if (GPRS_No_Response_Error()) return 0;
            Modem_Init_Flag = 1;
            INIT_MODEM_STATE = 0;
            return 1;  // Initialization complete

        case 15:
            if (!GPRS_RESPONSE_READY()) break;
            STRCPY(GPRSSEND_BUF, "AT+QIACT\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'O';
            INIT_MODEM_STATE = 16;
            break;

        case 16:
            if (!GPRS_RESPONSE_READY()) break;
            Modem_Init_Flag = 0;
            INIT_MODEM_STATE = 0;
            return 0;

        case 17:
            if (!GPRS_RESPONSE_READY()) break;
            STRCPY(GPRSSEND_BUF, "ATE0\r");
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response = 'c';
            INIT_MODEM_STATE = 16;
            break;

        default:
            INIT_MODEM_STATE = 0; // reset unknown state
            break;
    }

    return 0;  // Still initializing
}

void Connect_GPRS_Modem(void)
{      
    if(SIM_INSERTED==1 && Signal_Strength>2 && Modem_Init_Flag==1)
    {
        if(SERVER_STATE==0)
        {
            GPRSDELAYCOUNT=0;
            //MODEM_ERROR=0;
                        
            SEND_BUF[0]=0x1B;
            GPRS_SendByte(SEND_BUF[0]);

            STRCPY(GPRSSEND_BUF,"AT+QISTAT\r");
            SERVER_STATE=1;
            GPRS_FLAG=0;
            GPRS_SendString(GPRSSEND_BUF);
            Expected_Response='c';
            
           GPRS_BYTE_COUNT=0;
           MODEM_ERROR=0;
        }
        else if(SERVER_STATE==1 && (GPRS_FLAG==1 || GPRSDELAYCOUNT>2))
        {
            if(GPRSDELAYCOUNT>2 && GPRS_FLAG==0)
            {
                SERVER_STATE=0;
                GPRSDELAYCOUNT=0;
                return;
            }
            
            if(GPRS_BUF[2]=='E' || GPRSOK_BUF[2]=='E')
            {
                SERVER_STATE=0;
                GPRSDELAYCOUNT=0;
                return;
            }

            if(GPRS_BUF[2]=='P' || GPRSOK_BUF[2]=='P')
            {
                STRCPY(GPRSSEND_BUF,"AT+QIACT\r");
                GPRS_SendString(GPRSSEND_BUF);
                Expected_Response='O';
                SERVER_STATE=2;
                GPRS_FLAG=0;
            }
            else
            {
                SERVER_STATE=2;
                GPRS_FLAG=1;
                OK_ERROR='O';
            }            
        }
        else if(SERVER_STATE==2 && (GPRS_FLAG==1 || GPRSDELAYCOUNT>5))
        {
            if( GPRS_FLAG==0 || OK_ERROR=='E')
            {
                SERVER_STATE=0;
                GPRSDELAYCOUNT=0;
                return;
            }
            Send_Domain_PortNo();
            
            SERVER_STATE=3;
           

        }
        else if(SERVER_STATE==3 && (GPRS_FLAG==1 || GPRSDELAYCOUNT>5))//40
        {
            if(GPRS_FLAG==0 || GPRS_BUF[2]=='E' )
            {
                SERVER_STATE=0;
                GPRSDELAYCOUNT=0;
              
                return;
            }
                        
            STRCPY(GPRSSEND_BUF,"\r\nCONNECT\r\n");
            if(strncmp(GPRSOK_BUF,GPRSSEND_BUF,11)==0)
            {
                Send_Start_Command();
            }
            else
            {
                SERVER_STATE=0;
                SIM_INSERTED=0;
                Modem_Init_Flag=0;
                Expected_Response='C';
                MODEM_ERROR=0x30;
            }
            GPRS_FLAG=0;
        }
    
        else if(SERVER_STATE==8 && (GPRS_FLAG==1 || GPRSDELAYCOUNT>5))//40
        {
            Send_Backend_Records();
        }
        else if(SERVER_STATE==9 && (GPRS_FLAG==1 || GPRSDELAYCOUNT>25))//40
        {
            Update_Backend_Records();         
           
        }
        else if(SERVER_STATE==10  && (GPRS_FLAG==1 || GPRSDELAYCOUNT>5))
        {
            Send_SCT_Close();
            
           // Expected_Response='O';
            SERVER_STATE=12;
            GPRS_FLAG=1;//0;
            GPRS_BYTE_COUNT=0;
        }
        else if(SERVER_STATE==12 && (GPRS_FLAG==1|| GPRSDELAYCOUNT>5))
        {
            SERVER_STATE=0;
            GPRS_FLAG=0;
            GPRS_BYTE_COUNT=0;
        }
    }
    else
	{
		SERVER_STATE=0;
        Modem_Init_Flag=0;
	}
}

UINT8 Check_Mobile_No(void)
{
    UINT8 m_index=0;

    while(m_index<10)
    {
        if(Memberrec.CONTACT_NO[m_index]>0x39 || Memberrec.CONTACT_NO[m_index]<0x30 || Memberrec.CONTACT_NO[0]==0x30)
        {
            MSG_SENT_ACK='I';
            return 1;
        }

        m_index++;
    }
    MSG_SENT_ACK='V';
    return 0;
}

UINT8 Msg_Details(void)
{
    UINT8 sms_count;
   
    Send_Dairy_Name_ID();
    
    Copy_MonthRecDT(SEND_BUF+32);    

    SEND_BUF[52]='\n';
   
    STRNCPY(SEND_BUF+53,Memberrec.NAME,14);
    SEND_BUF[67]='\n';
  
    STRCPY(SEND_BUF+68,"1111(C)\n");
    Hex_DEC(0,Memberrec.C_CODE,SEND_BUF+68,4);
    SEND_BUF[73]=Memberrec.MILK_TYPE;
    ZERO=1;
    sms_count=76;
     if(Cumrec.C_QTY>0)
    {
        STRCPY(SEND_BUF+sms_count,"Cow:00000.00 L\n");
        if(WEIGHT_KG=='Y')
            SEND_BUF[sms_count+13]='K';
        Hex_DEC(2,Cumrec.C_QTY,(SEND_BUF+sms_count+4),8);
        Shift_Left(SEND_BUF+sms_count+4,10);
        sms_count=sms_count+15;
    }
    if(Cumrec.B_QTY>0)
    {
        STRCPY(SEND_BUF+sms_count,"Buf:00000.00 L\n");
        if(WEIGHT_KG=='Y')
            SEND_BUF[sms_count+13]='K';
        Hex_DEC(2,Cumrec.B_QTY,(SEND_BUF+sms_count+4),8);
        Shift_Left(SEND_BUF+sms_count+4,10);
        sms_count=sms_count+15;
    }
    
    if(MemberBillrec.INCENT_AMT>0 || MemberBillrec.TOTAL_AMT>0)
    {
        STRCPY(SEND_BUF+sms_count,"Amt:000000.00\n");
        Hex_DEC(2,(Cumrec.C_AMOUNT+Cumrec.B_AMOUNT),SEND_BUF+sms_count+4,9);//-MemberBillrec.INCENT_AMT
        Shift_Left(SEND_BUF+sms_count+4,9);
        sms_count= sms_count+14;
    }    
    
    if(MemberBillrec.INCENT_AMT>0)
    {
        STRCPY(SEND_BUF+sms_count,"Inc:00000.00\n");
        Hex_DEC(2,MemberBillrec.INCENT_AMT,SEND_BUF+sms_count+4,8);
        Shift_Left(SEND_BUF+sms_count+4,8);
        sms_count= sms_count+13;
    }
    return sms_count;
}

void SMS_MemberBill_Data(void)
{
    UINT8 sms_m_count;
    
    sms_m_count=Msg_Details();
    
    if(MemberBillrec.DED_AMT>0)
    {
        STRCPY(SEND_BUF+sms_m_count,"Ded:00000.00\n");        
        Hex_DEC(2,MemberBillrec.DED_AMT,SEND_BUF+sms_m_count+4,8);
        Shift_Left(SEND_BUF+sms_m_count+4,8);
        sms_m_count=sms_m_count+13;
    }

    if(MemberBillrec.GRAND_TOTAL>=MemberBillrec.DED_AMT)
    {
       STRCPY(SEND_BUF+sms_m_count,"NET:");
            
        Hex_DEC(2,(MemberBillrec.GRAND_TOTAL-MemberBillrec.DED_AMT),SEND_BUF+sms_m_count+4,9);
        Shift_Left(SEND_BUF+sms_m_count+4,9);
        sms_m_count=sms_m_count+13;
    }
    else
    {
        STRCPY(SEND_BUF+sms_m_count,"DUE:");
        Hex_DEC(2,(MemberBillrec.DED_AMT-MemberBillrec.GRAND_TOTAL),SEND_BUF+sms_m_count+4,9);
        Shift_Left(SEND_BUF+sms_m_count+4,9);
        sms_m_count=sms_m_count+13;
    }
    SEND_BUF[sms_m_count]=0x00;
   
    ZERO=0;
}

void SMS_Memberwise_Data(void)
{
    UINT8 sms_m_count;
    
    sms_m_count=Msg_Details(); 
   
    if(MemberBillrec.PAID_AMT>0)
    {
        STRCPY(SEND_BUF+sms_m_count,"Paid:");
        Hex_DEC(2,MemberBillrec.PAID_AMT,SEND_BUF+sms_m_count+5,8);
        Shift_Left(SEND_BUF+sms_m_count+5,8);
        SEND_BUF[sms_m_count+13]='\n';
        sms_m_count=sms_m_count+14;
    }        

    if(MemberBillrec.GRAND_TOTAL>MemberBillrec.PAID_AMT)
    {
        if((MemberBillrec.GRAND_TOTAL-MemberBillrec.PAID_AMT)>0)
        {
            STRCPY(SEND_BUF+sms_m_count,"Bal:");

            Hex_DEC(2,(MemberBillrec.GRAND_TOTAL-MemberBillrec.PAID_AMT),SEND_BUF+sms_m_count+4,9);
            Shift_Left(SEND_BUF+sms_m_count+4,9);
            sms_m_count=sms_m_count+13;
        }
    }
    else
    {
        if((MemberBillrec.PAID_AMT-MemberBillrec.GRAND_TOTAL)>0)
        {
            STRCPY(SEND_BUF+sms_m_count,"Due:");
            Hex_DEC(2,(MemberBillrec.PAID_AMT-MemberBillrec.GRAND_TOTAL),SEND_BUF+sms_m_count+4,9);
            Shift_Left(SEND_BUF+sms_m_count+4,9);
            sms_m_count=sms_m_count+13;
        }
    }

    SEND_BUF[sms_m_count]=0x00;
   
    ZERO=0;
}

UINT8 SMS_Fat_Snf_Wtr_Clr(UINT8 stype,UINT16 sval,UINT8 scnt)
{
    STRCPY(SEND_BUF+scnt,"F:00.0\n");
    SEND_BUF[scnt]=stype;
    Hex_DEC(1,sval,SEND_BUF+scnt+2,4);  
    return (scnt=scnt+7);
}

void Send_Dairy_Name_ID(void)
{
    ZERO=0;
    SEERAMReadString(Dairy_Name_Loc,32,(char *)&SEND_BUF[0]);   
    SEND_BUF[20]='\n';
   
    STRNCPY((char *)&SEND_BUF[21],SOCIETY_CODE,10);  
    SEND_BUF[31]='\n';
}

void SMS_Collection_Data(void)
{
    UINT8 sms_c_count;
    UINT32 sms_incentive_amt;

    Send_Dairy_Name_ID();

    STRCPY(SEND_BUF+32,"01/01/15 10:10\n");
    Hex_DEC(0,Shiftrec.DAY,SEND_BUF+32,2);
    Hex_DEC(0,Shiftrec.MONTH,SEND_BUF+35,2);
    Hex_DEC(0,Shiftrec.YEAR,SEND_BUF+38,2);

    Hex_DEC(0,CollectionRecord.WTIME[0] & 0x1F,SEND_BUF+41,2);
    Hex_DEC(0,CollectionRecord.WTIME[1],SEND_BUF+44,2);

    STRNCPY(SEND_BUF+47,Memberrec.NAME,17);
    SEND_BUF[64]='\n';
    Hex_DEC(0,Memberrec.C_CODE,SEND_BUF+65,4);
    
    sms_c_count=69;  
    if(Receiptrec.MILK_TYPE_PRINT=='Y')
    {
        if((CollectionRecord.FLAGS & 0x01) ==1)
            STRCPY(SEND_BUF+sms_c_count,"(COW)");
        else
            STRCPY(SEND_BUF+sms_c_count,"(BUF)");
        
        sms_c_count=sms_c_count+5; 
        SEND_BUF[sms_c_count]='\n';
        sms_c_count=sms_c_count+1;
    }

    ZERO=1;
    if(Receiptrec.FAT_PRINT=='Y')
    {
        sms_c_count=SMS_Fat_Snf_Wtr_Clr('F',CollectionRecord.FAT,sms_c_count);
    }
    
    if((CLR_BASED=='N' && Receiptrec.SNF_PRINT=='Y') || (CLR_BASED=='Y' && Receiptrec.CLR_PRINT=='Y'))
    {
        if(CLR_BASED=='Y')
            sms_c_count=SMS_Fat_Snf_Wtr_Clr('C',CollectionRecord.SNF,sms_c_count);
        else
            sms_c_count=SMS_Fat_Snf_Wtr_Clr('S',CollectionRecord.SNF,sms_c_count);
    }
    
    if(Receiptrec.WATER_PRINT=='Y' && CollectionRecord.WATER1>0 && CollectionRecord.WATER1!=0xFFFF)
    {        
        sms_c_count=SMS_Fat_Snf_Wtr_Clr('W',CollectionRecord.WATER1,sms_c_count);
        
    }
    
    if((CLR_BASED=='N' && Receiptrec.CLR_PRINT=='Y') || (CLR_BASED=='Y' && Receiptrec.SNF_PRINT=='Y'))
    {
        if(CollectionRecord.CLR1>0 && CollectionRecord.CLR1!=0xFFFF)
        {         
            if(CLR_BASED=='N')
            {
                sms_c_count=SMS_Fat_Snf_Wtr_Clr('C',CollectionRecord.CLR1,sms_c_count);
            }
            else
            {
                sms_c_count=SMS_Fat_Snf_Wtr_Clr('S',CollectionRecord.CLR1,sms_c_count);
            }
        }
    }
    
    if(Receiptrec.RATE_PRINT=='Y')
    {
        STRCPY(SEND_BUF+sms_c_count,"R:000.00\n");
        Hex_DEC(2,CollectionRecord.RATE,SEND_BUF+sms_c_count+2,6);
        sms_c_count=sms_c_count+9;
    }
    if(Receiptrec.QTY_PRINT=='Y')
    {
        STRCPY(SEND_BUF+sms_c_count,"Q:000.00 Lt\n");
        if(WEIGHT_KG=='Y')
        {
            SEND_BUF[sms_c_count+9]='K';
            SEND_BUF[sms_c_count+10]='g';
        }
        Hex_DEC(2,CollectionRecord.QTY,SEND_BUF+sms_c_count+2,6);
        sms_c_count=sms_c_count+12;
    }

    if( CollectionRecord.FAT>0)
    {
        if(Receiptrec.AMOUNT_PRINT=='Y')
        {
            STRCPY(SEND_BUF+sms_c_count,"A:00000.00\n");
            Calculate_Amount();
            Hex_DEC(2,Amount,SEND_BUF+sms_c_count+2,8);
            Shift_Left(SEND_BUF+sms_c_count+2,8);        
            sms_c_count=sms_c_count+11;        
        }
        sms_incentive_amt=Read_Incentive_Comm(1,CollectionRecord);

        if(sms_incentive_amt>0 && Receiptrec.INCENTIVE_PRINT=='Y')
        {
            STRCPY(SEND_BUF+sms_c_count,"INC:");
            Hex_DEC(2,sms_incentive_amt,SEND_BUF+sms_c_count+4,6);
            Shift_Left(SEND_BUF+sms_c_count+4,6);
            SEND_BUF[sms_c_count+10]='\n';        
            sms_c_count=sms_c_count+11;
            if(Receiptrec.AMOUNT_PRINT=='Y')
            {
                STRCPY(SEND_BUF+sms_c_count,"NET:");
                Hex_DEC(2,(Amount+sms_incentive_amt),SEND_BUF+sms_c_count+4,8);
                Shift_Left(SEND_BUF+sms_c_count+6,8);
                SEND_BUF[sms_c_count+12]='\n';

                sms_c_count=sms_c_count+13;
            }
        }
    }
    SEND_BUF[sms_c_count]=0x00;  
}

UINT8 Summary_Milk_Data(UINT8 mtype,UINT8 sms_c_count)
{
    SEND_BUF[sms_c_count]='\n';
    sms_c_count=sms_c_count+1;
   
    ZERO=1;
    if(mtype=='C')
    {
        STRCPY(SEND_BUF+sms_c_count,"COW:");
        Hex_DEC(0,Cumrec.C_MEMBERS,SEND_BUF+sms_c_count+4,3);   
    }
    else
    {
        STRCPY(SEND_BUF+sms_c_count,"BUF:");
        Hex_DEC(0,Cumrec.B_MEMBERS,SEND_BUF+sms_c_count+4,3);
    }
    Shift_Left(SEND_BUF+sms_c_count+4,3);
    SEND_BUF[sms_c_count+7]='\n';
    sms_c_count=sms_c_count+8;

    if(mtype=='C')
    {
        sms_c_count=SMS_Fat_Snf_Wtr_Clr('F',Cumrec.C_FAT/10,sms_c_count);
        sms_c_count=SMS_Fat_Snf_Wtr_Clr('S',Cumrec.C_SNF/10,sms_c_count);
    }
    else
    {
        sms_c_count=SMS_Fat_Snf_Wtr_Clr('F',Cumrec.B_FAT/10,sms_c_count);
        sms_c_count=SMS_Fat_Snf_Wtr_Clr('S',Cumrec.B_SNF/10,sms_c_count);  
    }

    STRCPY(SEND_BUF+sms_c_count,"Q(L):");
    if(WEIGHT_KG=='Y')
        SEND_BUF[sms_c_count+2]='K';
    if(mtype=='C')
        Hex_DEC(2,Cumrec.C_QTY,SEND_BUF+sms_c_count+5,8);
    else
        Hex_DEC(2,Cumrec.B_QTY,SEND_BUF+sms_c_count+5,8);
    Shift_Left(SEND_BUF+sms_c_count+5,8);
    SEND_BUF[sms_c_count+13]='\n';
    sms_c_count=sms_c_count+14;

    STRCPY(SEND_BUF+sms_c_count,"R:");   
    if(mtype=='C')
        Hex_DEC(2,Cumrec.C_RATE,SEND_BUF+sms_c_count+2,6);
    else
        Hex_DEC(2,Cumrec.B_RATE,SEND_BUF+sms_c_count+2,6);
    Shift_Left(SEND_BUF+sms_c_count+2,6);
    SEND_BUF[sms_c_count+8]='\n';
    sms_c_count=sms_c_count+9;

    STRCPY(SEND_BUF+sms_c_count,"A:"); 
    if(mtype=='C')
        Hex_DEC(2,(Cumrec.C_AMOUNT+Cumrec.C_INC_AMT),SEND_BUF+sms_c_count+2,9);
    else
        Hex_DEC(2,(Cumrec.B_AMOUNT+Cumrec.B_INC_AMT),SEND_BUF+sms_c_count+2,9);
    Shift_Left(SEND_BUF+sms_c_count+2,9);
    SEND_BUF[sms_c_count+11]='\n';
    
    sms_c_count=sms_c_count+12;
    
    return sms_c_count;        
}

 void SMS_Summary_Data(void)
{
    UINT8 sms_c_count;
    ZERO=0;
    
    Send_Dairy_Name_ID();
   
    Copy_ShiftDT(SEND_BUF+32);
    SEND_BUF[40]=' ';
    SEND_BUF[41]=Shiftrec.DAY_SHIFT;
    SEND_BUF[42]='\n';
    
    sms_c_count=43;
    if(Cumrec.B_MEMBERS>0)
    {
        sms_c_count=Summary_Milk_Data('B',sms_c_count);
    }    
    if(Cumrec.C_MEMBERS>0)
    {
        sms_c_count=Summary_Milk_Data('C',sms_c_count);             
    }
    SEND_BUF[sms_c_count]=0x00;
}

 void Send_SMS_BackEnd(UINT8 sctype,UINT16 smembersno)
{
    UINT32 temp_s_addr;
    
    if(SIM_INSERTED==1 && Signal_Strength>2 && Modem_Init_Flag==1)
    {
        if(SMS_STATE==0)
        {
           strcpy(GPRSSEND_BUF,"ATE0\r");
           GPRS_SendString(GPRSSEND_BUF);
           Max_Timer_Count=5;
           SMS_STATE=1;
           Expected_Response='O';
      }
      else if(SMS_STATE==1 && (GPRS_FLAG==1 ||  No_Response==1))
      {
          if(OK_ERROR=='E' )
          {
              SMS_STATE=0;
              return;
          }
          else if(No_Response==1)
          {
              SIM_INSERTED=0;
              Modem_Init_Flag=0;
              SMS_STATE=0;
              return;

          }
            GPRS_OVERFLOW=0;
            GPRS_BYTE_COUNT=0;
            GPRSDELAYCOUNT=0;

            Read_SMS_Counter();

            if(sctype!='S')
            {
                if(sctype=='C')
                {
                    SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&GPRSShiftrec);                   
                    SEERAMReadString(Running_Shift_Incentive_Loc,20,(char *)&Incentiverec);
                }
                else
                    GPRSShiftrec=Shiftrec;

                Shiftrec=GPRSShiftrec;

                Read_Member_Details(smembersno);
                GPRSMemberrec=Memberrec;
            }
            else
            {
                temp_s_addr=Contact_No1_Loc+(smembersno*10);
                SEERAMReadString(temp_s_addr,10,Memberrec.CONTACT_NO);
            }
            
            if(Check_Mobile_No()==1 ||((CollectionRecord.QTY ==0xFFFF || CollectionRecord.FAT==0xFF) && (sctype=='C'||sctype=='c')))
            {
                if(sctype=='C')
                    SMS_COUNT=SMS_COUNT+1;

                SMS_STATE=0;
                if(MSG_SENT_ACK=='V')
                    MSG_SENT_ACK='N';
                return;
            }

            else
            {
                STRCPY(SEND_BUF,"AT+CMGS=\"9494943786\"\r");
                STRNCPY(SEND_BUF+9,Memberrec.CONTACT_NO,10);
                GPRS_FLAG=0;
                GPRS_OVERFLOW=0;
                GPRS_BYTE_COUNT=0;
                GPRS_SendString(SEND_BUF);
                Expected_Response='C';
                SMS_STATE=2;
            }
         
            GPRSDELAYCOUNT=0;
        }
        else if(SMS_STATE==2 && (GPRS_FLAG==1  || No_Response==1))
        {
            GPRS_FLAG=0;
            GPRS_OVERFLOW=0;
            if(GPRSDELAYCOUNT>2 && GPRS_FLAG==0)
            {
                SMS_STATE=0;
                GPRSDELAYCOUNT=0;
            }

            if(GPRS_BUF[1]=='>' || GPRS_BUF[2]=='>' || GPRS_BUF[3]=='>')
            {
                if(sctype=='S')
                    SMS_Summary_Data();
                else
                {
                    Memberrec=GPRSMemberrec;
                    Shiftrec=GPRSShiftrec;

                    if(sctype=='c' || sctype=='D' || sctype=='C')
                        SMS_Collection_Data();
                    else if(sctype=='m')
                        SMS_MemberBill_Data();
                    else
                        SMS_Memberwise_Data();
                }
                                
                GPRS_BYTE_COUNT=0;
                GPRS_SendString(SEND_BUF);
                Expected_Response='C';

                SEND_BUF[0]=0x1A;
                GPRS_SendByte(SEND_BUF[0]);
                SMS_STATE=3;
            }
            else
                SMS_STATE=0;

        }
        else if(SMS_STATE==3 && (GPRS_FLAG==1 || GPRSDELAYCOUNT>5))
        {
            GPRS_FLAG=0;
            GPRS_BYTE_COUNT=0;
            if(GPRSDELAYCOUNT>5 && GPRS_FLAG==0)
            {
                SMS_STATE=0;
                GPRSDELAYCOUNT=0;
            }
            if(GPRS_BUF[7]!='E' && GPRS_BUF[7]!='N' && OK_ERROR=='O')
            {
                Read_SMS_Counter();
                SMSrec.COUNTER=SMSrec.COUNTER+1;
                Write_SMS_Counter();
                
                if(sctype=='C' || sctype=='c' || sctype=='D')
                   Set_Collection_Flag('S');
               
                SMS_STATE=0;
                MSG_SENT_ACK='S';
                if(sctype=='C')
                    SMS_COUNT=SMS_COUNT+1;
            }
            else
            {
                SMS_STATE=0;
                MSG_SENT_ACK='E';
                if(sctype=='C')
                    SMS_COUNT=SMS_COUNT+1;
            }
        }                    
    }
}

void Modem_Restart(void)
{
    UINT8 m_temp_cnt;

    m_temp_cnt=Max_Timer_Count;

    if(ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y')
    {
        Display_Full_Boarder();
        
        STRCPY(LBUF,"MODEM RESTARTING...");
        Disp_Msg(LINE5);
       
        STRCPY(SEND_BUF,"AT+QPOWD=1\r");
        GPRS_SendString(SEND_BUF);

        GPRS_RecString_Cmd(REC_BUF);

        STRCPY(SEND_BUF,"AT+CFUN=0\r");
        GPRS_SendString(SEND_BUF);

        GPRS_RecString_Cmd(REC_BUF);
        SEND_BUF[8]='1';

        GPRS_SendString(SEND_BUF);

        GPRS_RecString_Cmd(REC_BUF);
        if(REC_BUF[15]=='1')
        {
            GPRS_RecString_Cmd(REC_BUF+30);
            GPRS_RecString_Cmd(REC_BUF+60);
        }      
    }
    Max_Timer_Count=m_temp_cnt;
}

void Msg_Delivery_Ack(void)
{
    if(MSG_SENT_ACK=='I')
        STRCPY(LBUF,"Invalid Phone No...");

    if(MSG_SENT_ACK=='N')
        STRCPY(LBUF,"No Collection Data");

    if(MSG_SENT_ACK=='E')
        STRCPY(LBUF,"Msg Sending Failed");

    if(MSG_SENT_ACK=='S')
        STRCPY(LBUF,"Msg Sending Success");
    
    Invert_Disp_Msg(LINE8);

}