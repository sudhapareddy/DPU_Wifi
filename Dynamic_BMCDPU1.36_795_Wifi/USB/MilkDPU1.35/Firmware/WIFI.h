UINT8 WIFI_NAME_LENGTH,WIFI_INIT_ERR_CODE,DISP_HEADER_FLAG;
UINT8 WIFI_LIST_COUNT,AP_DETECTED,WIFI_MATCH,NO_AP_COUNT=0;
UINT8 SELECTED_WIFI[25],WIFI_LIST[25][20],DISP_WIFI_MENU_BUF[25];

typedef enum
{
    WIFI_CHECKING = 0,
    WIFI_INITIALIZING,
    WIFI_CONNECTING_AP,
    WIFI_SCANNING_AP_LIST,
    WIFI_CONNECTED,
    WIFI_NOT_IN_LIST,
    WIFI_WRONG_PASSWORD,
    WIFI_WRONG_NAME,
    WIFI_WRONG_NAME_PASSWORD,
    WIFI_NO_AP,
    WIFI_DISCONNECTED,
    WIFI_ERROR
} WIFI_STATE_T;

WIFI_STATE_T WIFI_STATE;


const char* WifiStateToString(WIFI_STATE_T st)
{
    switch(st)
    {
        case WIFI_CHECKING:
            return "Checking Wi-Fi Modem...";
        case WIFI_INITIALIZING:
            return "Initializing Wi-Fi...";
        case WIFI_CONNECTING_AP:
            return "Connecting to Wi-Fi...";
        case WIFI_SCANNING_AP_LIST:
            return "Searching Wi-Fi...";
        case WIFI_CONNECTED:
            return "Wi-Fi Connected";

        case WIFI_WRONG_PASSWORD:
        case WIFI_WRONG_NAME:
        case WIFI_WRONG_NAME_PASSWORD:
            return "Wrong Name/Password";

        case WIFI_NOT_IN_LIST:
        case WIFI_NO_AP:
            return "Wi-Fi Not Found";

        case WIFI_DISCONNECTED:
        case WIFI_ERROR:
        default:
            return "Check Connections...";
    }
}



void WIFI_Menu_Header(UINT8 hflag)
{
    Display_Screen_Boarder();
    
    if(hflag=='S')
        STRCPY(LBUF,"*WIFI SAVE LIST*");

    Disp_Header_No_Boarder();
}

void Display_Modem_STATUS(void)
{
    if((ON_LINE=='Y' && GPRSrec.GPRS_ON=='Y') && (Big_RTC==1 && STANDBY==1))
    {         
        if ((WIFI_STATE == WIFI_CONNECTED) && (Modem_Init_Flag == 1))
        {       
            //STRCPY(LBUF,"                ");
            
//            LBUF[0]=0x9D;
//            LBUF[1]=0x9E;
//            LBUF[2]='|';
            STRNCPY(LBUF,Operator,WIFI_NAME_LENGTH);

           
            LBUF[17]='|';
            if(Signal_Strength<50)
            {
                //Excellent
               
                
                LBUF[18]=0x87;
                LBUF[19]=0x8A;
                
            }
            else if(Signal_Strength>=50 && Signal_Strength<60)
            {                    
                //Good
                LBUF[18]=0x87;
                LBUF[19]=0x89;
            }
            else if(Signal_Strength>=60 && Signal_Strength<70)
            {
                //Fair
                LBUF[18]=0x87;
                LBUF[19]=0x88;
            }
            else if(Signal_Strength>=70)
            {             
                
                //Weak                
                //STRCPY(LBUF+15," Weak");
                LBUF[18]=0x87;
                LBUF[19]=0x88;
            }
            else
                STRCPY(LBUF+15,"NO SIGNAL");
            Disp_Msg(LINE6);
            return;
        }
//        else if(AP_DETECTED=='N' && Modem_Init_Flag==1)
//        {
//            
//            if(DISP_INTERVAL_CNT==0)
//            {
//                STRCPY(LBUF,"   Searching WIFI   ");
//                Disp_Msg(LINE6);
//                
//            }
//            else if(DISP_INTERVAL_CNT==5)
//            {
//                strncpy(LBUF,WIFIrec1.WIFI_NAME,16);
//                Disp_Msg(LINE6);                
//            }           
//            return;
//        }
//        else if(WIFI_INIT_ERR_CODE=='D')
//            STRCPY(LBUF,"   WIFI Detected    ");
//        else  if(WIFI_INIT_ERR_CODE==0)
//#ifdef WIFI
//            STRCPY(LBUF,"Checking WIFI MODEM");
//#elif defined(GPRS)
//            STRCPY(LBUF,"Checking GPRS MODEM");
//#endif
//       
//        else if(WIFI_INIT_ERR_CODE==1)
//            STRCPY(LBUF,"    Checking WIFI   ");
//        else if(WIFI_INIT_ERR_CODE==2)
//        {            
//            if(DISP_INTERVAL_CNT==0)
//            {
//                STRCPY(LBUF,"Connecting WIFI...  ");
//                Disp_Msg(LINE6);
//                
//            }
//            else if(DISP_INTERVAL_CNT==5)
//            {
//                strncpy(LBUF,WIFIrec1.WIFI_NAME,16);
//                Disp_Msg(LINE6);
//                
//            }
//            
//            return;
//        }
//        else if(WIFI_INIT_ERR_CODE==3  || WIFI_INIT_ERR_CODE==4)
//            STRCPY(LBUF,"Wrong Name/Password ");
//        else if(WIFI_INIT_ERR_CODE==5)
//            STRCPY(LBUF,"Fail To Connect WIFI");
//        else 
//            STRCPY(LBUF,"Initializing Modem  ");            
//        if(WIFI_MATCH=='N')
//            Disp_Msg(LINE6);
        
       

            if(DISP_INTERVAL_CNT<=5)
            {
                STRCPY(LBUF, WifiStateToString(WIFI_STATE));
                Disp_Msg(LINE6);
                
            }
            else if(DISP_INTERVAL_CNT>=5)
            {
                strncpy(LBUF,WIFIrec1.WIFI_NAME,16);
                Disp_Msg(LINE6);
                
            }
       
    }
}

UINT8 String_Separator(BYTE *buf,UINT8 separator,BYTE *copybuf)
{
    UINT8 length=0,b_index=0;
    
    while(buf[length]!=separator)
    {
        if(buf[length]!='"')
        {
            copybuf[b_index]=buf[length];
            b_index++;
        }
        length++;
    }
    length++;
    copybuf[b_index]=0x00;
    return length;
}

void Wifi_Name_Signal(UINT8 type)
{
    UINT8 s_index,t_index,wifi_strength;

    if(type=='I')
    {
        Modem_Init_Flag=1;
        SIM_INSERTED=1;
        INIT_MODEM_STATE=0;
    }
    else if(type=='C')
    {
        SIM_CHECK_STATE=0;
        SMS_STATE=0;
        MODEM_STATUS=1;
    }
   
    Max_Timer_Count=20;
    STRNCPY(Operator,GPRS_BUF+10,10);
    Signal_Strength=15;
    t_index=10;
    s_index=String_Separator(GPRS_BUF+t_index,',',Operator);  
    WIFI_NAME_LENGTH=s_index-2;
    if(s_index!=0)
    {
        t_index=t_index+s_index;
        s_index=String_Separator(GPRS_BUF+t_index,',',REC_BUF2);
        t_index=t_index+s_index;
        s_index=String_Separator(GPRS_BUF+t_index,',',REC_BUF2);   
        t_index=t_index+s_index;
        s_index=String_Separator(GPRS_BUF+t_index,'\r',REC_BUF2);

        if(REC_BUF2[0]=='-')
        {
            wifi_strength=atoi(REC_BUF2+1);
            Signal_Strength=wifi_strength;
        }
    }
}

void Send_WIFI_User_Pword(void)
{
    UINT8 c_cnt,wifi_buf[20],user_sp,pwd_sp;
    
    STRCPY(GPRSSEND_BUF,"AT+CWJAP=\"");
    c_cnt=0;
    user_sp=10;
    SEERAMReadString(Wifi_User_Name_Loc,16,(char *)&wifi_buf);
    while(wifi_buf[c_cnt]!='\0')
    {
        if(c_cnt>=16)
            break;
        GPRSSEND_BUF[user_sp+c_cnt]=wifi_buf[c_cnt];
        c_cnt++;
    }
    GPRSSEND_BUF[user_sp+c_cnt]='\"';
    c_cnt=c_cnt+1;                
    GPRSSEND_BUF[user_sp+c_cnt]=',';
    c_cnt=c_cnt+1;
    GPRSSEND_BUF[user_sp+c_cnt]='\"';
    c_cnt=c_cnt+1;

    pwd_sp=user_sp+c_cnt;
    c_cnt=0;
    SEERAMReadString(Wifi_User_Pword_Loc,16,(char *)&wifi_buf);
   
    while(wifi_buf[c_cnt]!='\0')
    {
         if(c_cnt>=16)
            break;
        GPRSSEND_BUF[pwd_sp+c_cnt]=wifi_buf[c_cnt];
        c_cnt++;
    }
    pwd_sp=pwd_sp+c_cnt;
    GPRSSEND_BUF[pwd_sp]='\"';
    pwd_sp=pwd_sp+1;
    GPRSSEND_BUF[pwd_sp]='\r';
    pwd_sp=pwd_sp+1;
    GPRSSEND_BUF[pwd_sp]='\n';
    pwd_sp=pwd_sp+1;
  
    GPRS_Send_HexData(GPRSSEND_BUF,pwd_sp);
    Expected_Response='c';
}


void Send_AT_Command(void)
{
    Max_Timer_Count=30;
    TimerCount=0;
    
    SEERAMReadString(Wifi_User_Name_Loc,32,(char *)&WIFIrec1);
    STRCPY(GPRSSEND_BUF,"AT\r\n");    
    GPRS_Send_HexData(GPRSSEND_BUF,4);
    Expected_Response='O';
} 
 
void Send_CWJAP_Query(void)
{
    STRCPY(GPRSSEND_BUF,"AT+CWJAP?\r\n");
    GPRS_Send_HexData(GPRSSEND_BUF,11);
    Expected_Response='c';
}

//void Init_Wifi_Modem1(void)
//{
//    switch (INIT_MODEM_STATE)
//    {
//        // ------------------------------------------------------------
//        // STATE 0 - Send AT and start modem checking
//        // ------------------------------------------------------------
//        case 0:
//            Send_AT_Command();             // AT\r\n
//            WIFI_STATE       = WIFI_CHECKING;
//            WIFI_INIT_ERR_CODE = 0;
//            INIT_MODEM_STATE = 1;
//        break;
//
//        // ------------------------------------------------------------
//        // STATE 1 - Set CIPMODE=1
//        // ------------------------------------------------------------
//        case 1:
//            if (GPRS_FLAG || No_Response)
//            {
//                if (No_Response_Error()) { WIFI_STATE = WIFI_ERROR; break; }
//
//                STRCPY(GPRSSEND_BUF,"AT+CIPMODE=1\r\n");
//                GPRS_Send_HexData(GPRSSEND_BUF,14);
//                Expected_Response='O';
//
//                WIFI_STATE       = WIFI_INITIALIZING;
//                INIT_MODEM_STATE = 2;
//            }
//        break;
//
//        // ------------------------------------------------------------
//        // STATE 2 - Set CIPMUX=0
//        // ------------------------------------------------------------
//        case 2:
//            if (GPRS_FLAG || No_Response)
//            {
//                if (No_Response_Error()) { WIFI_STATE = WIFI_ERROR; break; }
//
//                STRCPY(GPRSSEND_BUF,"AT+CIPMUX=0\r\n");
//                GPRS_Send_HexData(GPRSSEND_BUF,13);
//                Expected_Response='O';
//
//                INIT_MODEM_STATE = 3;
//            }
//        break;
//
//        // ------------------------------------------------------------
//        // STATE 3 - Set CWMODE=1
//        // ------------------------------------------------------------
//        case 3:
//            if (GPRS_FLAG || No_Response)
//            {
//                if (No_Response_Error()) { WIFI_STATE = WIFI_ERROR; break; }
//
//                STRCPY(GPRSSEND_BUF,"AT+CWMODE=1\r\n");
//                GPRS_Send_HexData(GPRSSEND_BUF,13);
//                Expected_Response='O';
//
//                INIT_MODEM_STATE = 4;
//            }
//        break;
//
//        // ------------------------------------------------------------
//        // STATE 4 - Send CWJAP "ssid","pwd" (first connection attempt)
//        // ------------------------------------------------------------
//        case 4:
//            if (GPRS_FLAG || No_Response)
//            {
//                if (No_Response_Error()) { WIFI_STATE = WIFI_ERROR; break; }
//
//                Send_WIFI_User_Pword();          // AT+CWJAP="ssid","pwd"
//                Expected_Response='O';
//
//                WIFI_STATE       = WIFI_CONNECTING_AP;
//                WIFI_INIT_ERR_CODE = 1;
//                INIT_MODEM_STATE = 5;         // always go to state 5 to handle CWJAP
//            }
//        break;
//
//        // ------------------------------------------------------------
//        // STATE 5 - CWJAP RESULT checking (only place handling errors)
//        // ------------------------------------------------------------
//        case 5:
//            if (GPRS_FLAG || No_Response)
//            {
//                if (No_Response_Error()) { WIFI_STATE = WIFI_ERROR; break; }
//
//                if (strncmp((char *)GPRS_BUF, "\r\n+CWJAP:", 9) == 0)
//                {
//                    if (GPRS_BUF[9] == '\"')      // +CWJAP:"SSID"
//                    {
//                        WIFI_STATE         = WIFI_CONNECTED;
//                        WIFI_INIT_ERR_CODE = 0;
//                        Modem_Init_Flag    = 1;
//                        INIT_MODEM_STATE   = 0;
//                    }
//                    else if (GPRS_BUF[9] >= '1' && GPRS_BUF[9] <= '5')
//                    {
//                        UINT8 code = GPRS_BUF[9] - '0';
//
//                        switch (code)
//                        {
//                            case 2:
//                                WIFI_STATE = WIFI_WRONG_PASSWORD;
//                                WIFI_INIT_ERR_CODE = 2;
//                            break;
//
//                            case 3:
//                                WIFI_STATE = WIFI_WRONG_NAME;
//                                WIFI_INIT_ERR_CODE = 3;
//                            break;
//
//                            case 1:
//                                WIFI_INIT_ERR_CODE = 4;    // timeout
//                                WIFI_STATE = WIFI_ERROR;
//                            break;
//
//                            default:
//                                WIFI_INIT_ERR_CODE = 5;    // generic failure
//                                WIFI_STATE         = WIFI_ERROR;
//                            break;
//                        }
//
//                        // After error -> scan list to retry selection
//                        STRCPY(GPRSSEND_BUF,"AT+CWLAP\r\n");
//                        GPRS_Send_HexData(GPRSSEND_BUF,10);
//                        Expected_Response='S';
//
//                        WIFI_STATE       = WIFI_SCANNING_AP_LIST;
//                        INIT_MODEM_STATE = 6;
//                    }
//                }
//                else if (strncmp((char *)GPRS_BUF,"\r\nNo AP\r\n",9) == 0)
//                {
//                    WIFI_STATE         = WIFI_NO_AP;
//                    WIFI_INIT_ERR_CODE = 6;
//                    INIT_MODEM_STATE   = 0;
//                }
//                else
//                {
//                    WIFI_STATE         = WIFI_ERROR;
//                    WIFI_INIT_ERR_CODE = 7;
//                    INIT_MODEM_STATE   = 0;
//                }
//            }
//        break;
//
//        // ------------------------------------------------------------
//        // STATE 6 - Process CWLAP result & retry CWJAP if valid
//        // ------------------------------------------------------------
//        case 6:
//            if (GPRS_FLAG || No_Response)
//            {
//                if (No_Response_Error()) { WIFI_STATE = WIFI_ERROR; break; }
//
//                if (OK_ERROR == 'O')
//                {
//                    WIFI_LIST_COUNT = Wifi_List();
//                    if (WIFI_LIST_COUNT > 0 && WIFI_LIST_COUNT != 0xFF)
//                    {
//                        if (Check_Selected_WIFI_In_List() == 1)
//                        {
//                            WIFI_STATE = WIFI_CONNECTING_AP;
//                            Send_WIFI_User_Pword();
//                            Expected_Response = 'O';
//                            INIT_MODEM_STATE  = 5;   // go back to CWJAP result handler
//                        }
//                        else
//                        {
//                            WIFI_STATE         = WIFI_NOT_IN_LIST;
//                            WIFI_INIT_ERR_CODE = 8;
//                            INIT_MODEM_STATE   = 0;
//                        }
//                    }
//                    else
//                    {
//                        WIFI_STATE         = WIFI_NO_AP;
//                        WIFI_INIT_ERR_CODE = 9;
//                        INIT_MODEM_STATE   = 0;
//                    }
//                }
//            }
//        break;
//    }
//}


void Initialize_WIFI(UINT8 itype)
{
    //if(strncmp(GPRS_BUF,"\r\n+CWJAP:\"",10)==0)
    if(strncmp(GPRS_BUF,"\r\n+CWJAP:",9)==0)
    {
        if(GPRS_BUF[9]=='\"')
        {
            AP_DETECTED='Y';
            WIFI_STATE=WIFI_CONNECTED;
            Wifi_Name_Signal(itype);

            if(itype=='C')
                SIM_CHECK_STATE=0;
            if(itype=='I')
                INIT_MODEM_STATE=0;
        }
        else if(GPRS_BUF[9]>=0x31 &&  GPRS_BUF[9]<=0x35)
        {
            WIFI_INIT_ERR_CODE=(GPRS_BUF[9]-0x30)+2;
            WIFI_STATE=WIFI_WRONG_NAME_PASSWORD;
            AP_DETECTED='E';
            if(itype=='I')
                INIT_MODEM_STATE=6;
        
            if(itype=='C')
                SIM_CHECK_STATE=3;
        }

    }
    if(strncmp(GPRS_BUF,"\r\nNo AP\r\n",9)==0 || strncmp(Operator,WIFIrec1.WIFI_NAME,WIFI_NAME_LENGTH)!=0)
    {
        if(strncmp(GPRS_BUF,"\r\nNo AP\r\n",9)==0)       
        {
            AP_DETECTED='N';  
            NO_AP_COUNT++;
            
            if(NO_AP_COUNT>10)
                WIFI_STATE=WIFI_NOT_IN_LIST;         
        }
      
        STRCPY(GPRSSEND_BUF,"AT+CWLAP\r\n");
        GPRS_Send_HexData(GPRSSEND_BUF,10);

        Expected_Response='S';
        Max_Timer_Count=100;       
        
        if(itype=='I')
            INIT_MODEM_STATE=6;

        if(itype=='C')
            SIM_CHECK_STATE=3;
        
    }
}

UINT8 No_Response_Error(void)
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

UINT8 Check_Selected_WIFI_In_List(void)
{
    UINT8 wifi_cnt=0;
    
    SEERAMReadString(Wifi_User_Name_Loc,32,(char *)&AUTO_WIFI_rec);
    for(wifi_cnt=0;wifi_cnt<WIFI_LIST_COUNT;wifi_cnt++)
    {
        PTR_ADDR=(char *)&WIFI_LIST[wifi_cnt][0];
        strncpy(DISP_WIFI_MENU_BUF,PTR_ADDR,16);
            
        if(strncmp(AUTO_WIFI_rec.WIFI_NAME,DISP_WIFI_MENU_BUF,16)==0)
            return 1;
    }
    
    return 0;
}

UINT8 Wifi_List(void)
{
    UINT16 wifi_index=0,temp_cnt=0;
    UINT8 row_cnt=0,wifi_name_flag;
    
    temp_cnt=0;
    wifi_name_flag=0;
    WIFI_LIST_COUNT=0;
    
    STRCPY(GPRS_BUF,GPRSOK_BUF);
    
    if(OK_ERROR!='O')
        return 0xFF;
    
    while(1)
    {
        if(wifi_index>5000)
            break;
        
        if(strncmp(GPRS_BUF+wifi_index,"\r\nOK\r\n",6)==0)  
        {
            wifi_index=0;
            break;
        }

        if(wifi_name_flag==0 && GPRS_BUF[wifi_index]==',' && GPRS_BUF[wifi_index+1]=='"')
        {                
            temp_cnt=0;
            wifi_name_flag=1;
            wifi_index=wifi_index+1;

        }
        else if(wifi_name_flag==1 && GPRS_BUF[wifi_index]!='"')
        {
            WIFI_LIST[row_cnt][temp_cnt]=GPRS_BUF[wifi_index];
            temp_cnt++;
        }
        else if(wifi_name_flag==1 && GPRS_BUF[wifi_index]=='"')
        {

//            while(temp_cnt<20)
//            {
//                WIFI_LIST[row_cnt][temp_cnt]=' ';
//                temp_cnt++;
//            }
            if(temp_cnt>=16)
                temp_cnt=16;
            WIFI_LIST[row_cnt][temp_cnt]='\0';

            wifi_name_flag=2;
            WIFI_LIST_COUNT=WIFI_LIST_COUNT+1;
            temp_cnt=0;
        }
        
        if(GPRS_BUF[wifi_index]==')' && GPRS_BUF[wifi_index+1]==0x0D && GPRS_BUF[wifi_index+2]==0x0A)
        {
            temp_cnt=0;
            wifi_name_flag=0;
            row_cnt=row_cnt+1;
            wifi_index=wifi_index+2;
        }


        wifi_index++;
        
        if(WIFI_LIST_COUNT>=25)
        {
            WIFI_LIST_COUNT=25;
            return WIFI_LIST_COUNT;
        }

    }
    
    return WIFI_LIST_COUNT;
}

void Init_Wifi_Modem(void)
{
    if(INIT_MODEM_STATE==0)
    {
        Send_AT_Command();
        INIT_MODEM_STATE=1; 
       // NO_AP_COUNT=0;
        WIFI_STATE = WIFI_CHECKING;
    }
    else if(INIT_MODEM_STATE==1 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response_Error()==1)
            return;
        {
            STRCPY(GPRSSEND_BUF,"AT+CIPMODE=1\r\n");
            GPRS_Send_HexData(GPRSSEND_BUF,14);
            Expected_Response='O';
            WIFI_STATE = WIFI_INITIALIZING;
            INIT_MODEM_STATE=2;    
        }
    }
    else if(INIT_MODEM_STATE==2 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response_Error()==1)
            return;
        {
            STRCPY(GPRSSEND_BUF,"AT+CIPMUX=0\r\n");
            GPRS_Send_HexData(GPRSSEND_BUF,13);
            Expected_Response='O';
            INIT_MODEM_STATE=3;    
        }
    
    }
    else if(INIT_MODEM_STATE==3 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response_Error()==1)
            return;
        {
            STRCPY(GPRSSEND_BUF,"AT+CWMODE=1\r\n");
            GPRS_Send_HexData(GPRSSEND_BUF,13);
            Expected_Response='O';
            INIT_MODEM_STATE=4;    
        }
    
    }
    else if(INIT_MODEM_STATE==4 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response_Error()==1)
           return;
        {          
            Send_CWJAP_Query();
            INIT_MODEM_STATE=5;
            WIFI_INIT_ERR_CODE=1;
        }    
    }
    else if(INIT_MODEM_STATE==5 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response_Error()==1)
           return;
        {             
            WIFI_INIT_ERR_CODE=2;
            Initialize_WIFI('I');
             
        }
    }
    else if(INIT_MODEM_STATE==6 && (GPRS_FLAG==1 || No_Response==1))   
    {
        if(No_Response_Error()==1)
           return;
        {      
            if(OK_ERROR=='O')
            {
                WIFI_MATCH=2;
               
                WIFI_LIST_COUNT=Wifi_List();
                if(WIFI_LIST_COUNT!=0xFF && WIFI_LIST_COUNT>0)
                {
                    WIFI_MATCH=Check_Selected_WIFI_In_List();//2
                }
                if(WIFI_MATCH==1)
                {
                    INIT_MODEM_STATE=7;
                    WIFI_MATCH='Y';
                     WIFI_STATE=WIFI_SCANNING_AP_LIST;
                //NO_AP_COUNT=0;
                    
                    Send_WIFI_User_Pword(); 
                    
                }
                else
                {
                   INIT_MODEM_STATE=0;    
                   WIFI_MATCH='N';
                }
            }
        }
    }
    else if(INIT_MODEM_STATE==7 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response_Error()==1)
           return;
        { 
            if(OK_ERROR=='O')
            {
                WIFI_INIT_ERR_CODE='D';
                AP_DETECTED='D';
                INIT_MODEM_STATE=0;
                Modem_Init_Flag=1;
               // NO_AP_COUNT=0;
            }
            else
            {
//               WIFI_INIT_ERR_CODE=3;
//                STRCPY(GPRSSEND_BUF,"AT+CWQAP\r\n");
//                GPRS_Send_HexData(GPRSSEND_BUF,10);
//                Expected_Response='O';
                INIT_MODEM_STATE=9;

            }
        }
    }
    else if(INIT_MODEM_STATE==9 )
    {
        //if(strncmp(GPRSOK_BUF,"\r\n+CWJAP:2\r\n",12)==0)  
        WIFI_INIT_ERR_CODE=3;
        WIFI_STATE=WIFI_WRONG_PASSWORD;
        
        Send_WIFI_User_Pword();

        INIT_MODEM_STATE=6;
    }
    else if(INIT_MODEM_STATE==8 && (GPRS_FLAG==1 || No_Response==1))
    {
        Modem_Flags_Reset();
    }
}
void Check_Wifi_Modem(void)
{
    switch (SIM_CHECK_STATE)
    {
        case 0:   // Step 1: Check modem alive
            Send_AT_Command();       // "AT\r\n"
            SIM_CHECK_STATE = 1;
            break;

        case 1:   // Handle AT response
            if (GPRS_FLAG || No_Response)
            {
                if (No_Response || OK_ERROR == 'E')
                {
                    Modem_Flags_Reset();
                    MODEM_STATUS    = 0;
                    MODEM_PRESENCE  = 0;
                    WIFI_STATE      = WIFI_DISCONNECTED;
                    SIM_CHECK_STATE = 0;
                }
                else
                {
                    MODEM_STATUS    = 1;
                    MODEM_PRESENCE  = 1;

                    // Now check Wi-Fi join status
                    Send_CWJAP_Query();  // AT+CWJAP?\r\n
                    SIM_CHECK_STATE = 2;
                }
            }
            break;

        case 2:   // Handle CWJAP? response
            if (GPRS_FLAG || No_Response)
            {
                if (No_Response || OK_ERROR == 'E')
                {
                    WIFI_STATE = WIFI_DISCONNECTED;
                }
                else
                {
                    if (strstr((char *)GPRS_BUF, "+CWJAP:\"") != NULL)
                    {
                        // Still joined to some AP
                        WIFI_STATE = WIFI_CONNECTED;
                    }
                    else if (strstr((char *)GPRS_BUF, "No AP") != NULL)
                    {
                        WIFI_STATE = WIFI_NO_AP;
                        Modem_Flags_Reset();
                    }
                    else
                    {
                        WIFI_STATE = WIFI_DISCONNECTED;
                        Modem_Flags_Reset();
                    }
                }

                SIM_CHECK_STATE = 0;    // done check
            }
            break;
    }
}



UINT8 Check_Wifi_Modem1(void)
{  
    if(SIM_CHECK_STATE==0)
    {
        Send_AT_Command();
        SIM_CHECK_STATE=1;    
       // WIFI_INIT_ERR_CODE=0;
         
    }
    else if(SIM_CHECK_STATE==1 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response==1 || OK_ERROR=='E')
        {
            if(No_Response==1)
                 Modem_Flags_Reset();
            SIM_CHECK_STATE=0;
            MODEM_STATUS=0;
            MODEM_PRESENCE=0;
        }
        else
        {
            Send_CWJAP_Query();
            SIM_CHECK_STATE=2;   
            //WIFI_INIT_ERR_CODE=1;
        }
    
    }
    else if(SIM_CHECK_STATE==2 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response==1 || OK_ERROR=='E')
        {
            if(No_Response==1)
                 Modem_Flags_Reset();
            SIM_CHECK_STATE=0;
        }
        else
        {
            if(OK_ERROR=='O')
                WIFI_INIT_ERR_CODE=2;
          
            Initialize_WIFI('C');
            
            
        }
    }
    else if(SIM_CHECK_STATE==3 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response==1 || OK_ERROR=='E')
        {
            if(No_Response==1)
                 Modem_Flags_Reset();
            SIM_CHECK_STATE=0;
        }
        else
        {        
            if(OK_ERROR=='O')
            {
                WIFI_MATCH=2;
                WIFI_LIST_COUNT=Wifi_List();
                if(WIFI_LIST_COUNT!=0xFF && WIFI_LIST_COUNT>0)
                {
                    WIFI_MATCH=Check_Selected_WIFI_In_List();//1
                }
                if(WIFI_MATCH==1)
                {
                    SIM_CHECK_STATE=4;    
                    WIFI_MATCH='Y';
                    
                    Send_WIFI_User_Pword(); 
                    
                }
                else
                {
                   SIM_CHECK_STATE=0; 
                   INIT_MODEM_STATE=0;
                   AP_DETECTED=0;                   
                   WIFI_MATCH='N';
                   
                }
                
            }
        }
    }
    else if(SIM_CHECK_STATE==4 && (GPRS_FLAG==1 || No_Response==1))
    {
        if(No_Response==1 || OK_ERROR=='E')
        {
            if(No_Response==1)
                 Modem_Flags_Reset();
            SIM_CHECK_STATE=0;
        }
        else
        {
            //Initialize_WIFI('C');//SIM_CHECK_STATE=0;
            if(OK_ERROR=='O')
            {
                WIFI_INIT_ERR_CODE='D';
                AP_DETECTED='D';
                SIM_CHECK_STATE=1;
            }
            else
            {
                SIM_CHECK_STATE=0;
            }
        }
    }
}

UINT8 Connect_WIFI_Modem(void)
{
    if(Signal_Strength>2 && Modem_Init_Flag==1)
    {
        if(SERVER_STATE==0)
        {            
            GPRSDELAYCOUNT=0;
            MODEM_ERROR=0;
          
            Send_Domain_PortNo();
            
            SERVER_STATE=1;
           
        }   
        else if(SERVER_STATE==1 && (GPRS_FLAG==1 || GPRSDELAYCOUNT>5))
        {
            if(GPRS_FLAG==0 || GPRS_BUF[2]=='E' )
            {
                SERVER_STATE=10;
            }
            else
            {
                STRCPY(GPRSSEND_BUF,"\r\nCONNECT\r\n");
                if(strncmp(GPRS_BUF,GPRSSEND_BUF,11)==0)
                {
                    STRCPY(GPRSSEND_BUF,"AT+CIPSEND\r\n");
                    GPRS_Send_HexData(GPRSSEND_BUF,12);
                  
                    Expected_Response='O';
                    SERVER_STATE=2;
                    CONNECT_FLAG=1;
                }
                else
                {
                    SERVER_STATE=10;
                    MODEM_ERROR=0x30;
                    CONNECT_FLAG=1;
                  
                }
                
            }
        }
        else if(SERVER_STATE==2 && (GPRS_FLAG==1 || GPRSDELAYCOUNT>5))//40
        {
            if(GPRS_FLAG==0 || GPRS_BUF[2]=='E' )
            {
                SERVER_STATE=0;
                GPRSDELAYCOUNT=0;
                Modem_Init_Flag=0;
                return;
            }            
            
            else
            {
                Send_Start_Command();
                SERVER_STATE=3;
                GPRS_FLAG=0;
                Expected_Response='D';
            }
        }
       
        else if(SERVER_STATE==3 && (GPRS_FLAG==1 || GPRSDELAYCOUNT>5))//40
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
            
            Expected_Response='C';
            SERVER_STATE=12;
            GPRS_FLAG=0;
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



UINT8 Show_WIFI_List(void)
{   
    STRCPY(GPRSSEND_BUF,"AT+CWQAP\r\n");
    GPRS_Send_HexData(GPRSSEND_BUF,10);
    GPRS_RecString_OK();
    
    Delay_Sec(2);
    OK_ERROR=0;
 
    STRCPY(GPRSSEND_BUF,"AT+CWLAP\r\n");
    GPRS_Send_HexData(GPRSSEND_BUF,10);
    
    GPRS_RecString_Command_Data(GPRS_BUF);
    
    if(OK_ERROR!='O')
        return 0xFF;    
    
    WIFI_LIST_COUNT=Wifi_List();
    
    STRCPY(GPRSSEND_BUF,"AT+CWJAP?\r\n");
    GPRS_Send_HexData(GPRSSEND_BUF,11);
    GPRS_RecString_OK();
    
    return WIFI_LIST_COUNT;
}

UINT8 Check_Stored_WIFI_List(char *selected_buf)
{
    UINT8 wifi_cnt=0;
    
    for(wifi_cnt=0;wifi_cnt<5;wifi_cnt++)
    {
        EEPROM_ADDR=WIFI_Details_Loc+(wifi_cnt*WIFI_Record_Length);
        SEERAMReadString(EEPROM_ADDR,32,(char *)&AUTO_WIFI_rec);
            
        if(strncmp(AUTO_WIFI_rec.WIFI_NAME,selected_buf,16)==0)
            return 1;
    }
    
    return 0;
}

void Wifi_Auto_Selection(void)
{
    UINT8 wifi_name[20],wifi_pwd[20],return_value;
    UINT8 prev_list_buf[150];
    
   
    Display_Boarder2();
    CLEAR_DATA_LINES_FROM(1,2);
    STRCPY(LBUF,"*AUTO Wi-Fi SCAN*");
    Disp_Msg_Big(1,LINE1);
    
    Save_Exit_Msg();
    
    DISPLAY_NO_SPACES=1;
    STRCPY(LBUF,"NAME:");
    Disp_Msg(LINE3);
    
    strncpy(LBUF,AUTO_WIFI_rec.WIFI_NAME,16);
    strncpy(wifi_name,AUTO_WIFI_rec.WIFI_NAME,16);
    Disp_Msg(LINE4);
    
    STRCPY(LBUF,"Password:");
    strcpy(wifi_pwd,"                ");
    Disp_Msg(LINE5);
    
    return_value=Get_String_New(FONT_SMALL,16,NULL_TERMINATOR,LINE6,(char *)&wifi_pwd);
    
    if(KBV==KB_CANCEL)                    
        return;
     else
     {
        if(return_value<16)
            wifi_pwd[return_value]='\0';
        else if(return_value>=16)
            wifi_pwd[16]='\0';
        
        strncpy(AUTO_WIFI_rec.WIFI_NAME,wifi_name,16);
        strncpy(AUTO_WIFI_rec.WIFI_PASSWORD,wifi_pwd,16);
        
        
        EEPROM_ADDR=WIFI_Details_Loc+WIFI_Record_Length;
        SEERAMReadString(EEPROM_ADDR,(4*WIFI_Record_Length),prev_list_buf);
        
        EEPROM_ADDR=WIFI_Details_Loc;
        SEERAMWriteString(EEPROM_ADDR,(4*WIFI_Record_Length),prev_list_buf);
                
        EEPROM_ADDR=WIFI_Details_Loc+(4*WIFI_Record_Length);
        SEERAMWriteString(EEPROM_ADDR,32,(char *)&AUTO_WIFI_rec);
        
        SEERAMWriteString(Wifi_User_Name_Loc,32,(char *)&AUTO_WIFI_rec);
                   
        Saved_Message();     
         return;
     }  
}

void Wifi_Details_SubMenu(UINT8 wifisubcnt)
{
    UINT8 return_value,wifi_step=0,wifi_name[20],wifi_pwd[20],selected_wifi[20];
       
    Display_Boarder2();
    STRCPY(LBUF," WIFI SAVED LIST -06");
    LBUF[19]=0x31+wifisubcnt;
    Disp_Msg(LINE1);
    
    Save_Exit_Msg();
    
    
    while(1)
    {
        switch(wifi_step)
        {
            case 0:

                DISPLAY_NO_SPACES=1;
                STRCPY(LBUF,"NAME:");
                Disp_Msg(LINE3);        

                EEPROM_ADDR=WIFI_Details_Loc+(wifisubcnt*WIFI_Record_Length);
                SEERAMReadString(EEPROM_ADDR,16,(char *)&wifi_name);
                SEERAMReadString((EEPROM_ADDR+16),16,(char *)&wifi_pwd);
                    
                strncpy(LBUF,wifi_name,16);
                Disp_Msg(LINE4);    
                         
                STRCPY(LBUF,"Password:");
                Disp_Msg(LINE5); 
                STRNCPY(LBUF,wifi_pwd,16);
                Disp_Msg(LINE6);   

                return_value=Get_String_New(FONT_SMALL,16,NULL_TERMINATOR,LINE4,(char *)&wifi_name);

                if(KBV==KB_CANCEL)                    
                    return;

               if(return_value>0)
               {
                   wifi_step=1;
                    if(return_value<16)
                        wifi_name[return_value]='\0';
                    else if(return_value>=16)
                        wifi_name[16]='\0';
                   STRNCPY((char *)&AUTO_WIFI_rec.WIFI_NAME,wifi_name,16);

                }
                break;

            case 1:
                
                STRCPY(LBUF,"Password:");
                Disp_Msg(LINE5);
                
                return_value=Get_String_New(FONT_SMALL,16,NULL_TERMINATOR,LINE6,(char *)&wifi_pwd);
                
                if(KBV==KB_CANCEL)                    
                    return;

               if(return_value>0)
               {
                   wifi_step=0;
                   if(return_value<16)
                       wifi_pwd[return_value]='\0';
                   else if(return_value>=16)
                       wifi_pwd[16]='\0';
                       
                   strncpy((char *)&AUTO_WIFI_rec.WIFI_PASSWORD,wifi_pwd,16);

                   EEPROM_ADDR=WIFI_Details_Loc+(wifisubcnt*WIFI_Record_Length);
                   SEERAMWriteString(EEPROM_ADDR,32,(char *)&AUTO_WIFI_rec);

                   SEERAMReadString(Wifi_User_Name_Loc,16,(char *)&selected_wifi);

                   if(strcmp(WIFIrec1.WIFI_NAME,selected_wifi)==0)
                   {
                       SEERAMWriteString(Wifi_User_Name_Loc,32,(char *)&AUTO_WIFI_rec);
                   }
                   Saved_Message();     
                    return;
                }
        }
    } 
}


void Select_WIFI_From_List(UINT8 total_wifi_cnt)
{
    UINT8 wifi_cnt,m_a_cnt=0,selected_wifi[20],match_status;
    
    while(1)
    {   
        STRCPY(LBUF,"WIFI NAME-"); 
        Hex_DEC(0,m_a_cnt,LBUF+10,2);
        Disp_Msg(LINE4);  
        
        CLEAR_DATA_LINES_FROM(5,6);
      
        PTR_ADDR=(char *)&WIFI_LIST[m_a_cnt][0];
       
        strncpy(SELECTED_WIFI,PTR_ADDR,16);
        strncpy(DISP_WIFI_MENU_BUF,PTR_ADDR,16);
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Big(1,LINE5);
        
       // Display_Label_Data(1,DISP_WIFI_MENU_BUF,14,LINE5);
        
        Reset_Display_Values();

        KBV=Wait_Key_Press('C');//a

        switch(KBV)
        {
            case KB_CANCEL:

                return;

            case KB_ENT:

                //SEERAMWriteString(Wifi_User_Name_Loc,16,(char *)&selected_wifi);
                match_status=Check_Stored_WIFI_List(SELECTED_WIFI);

                if(match_status==1)
                {
                    
                    SEERAMWriteString(Wifi_User_Name_Loc,32,(char *)&AUTO_WIFI_rec);
                    STRCPY(LBUF,"WIFI Selected...");
                    Invert_Disp_Msg(LINE8);
                    SEERAMReadString(Wifi_User_Name_Loc,32,(char *)&WIFIrec1);
                    Modem_Flags_Reset();
                    Delay_Sec(1);
                    return;
                }
                    
                else
                {
                    strncpy(AUTO_WIFI_rec.WIFI_NAME,SELECTED_WIFI,16);
                    Wifi_Auto_Selection();
//                    Wifi_Details_SubMenu('O');                    
                }
                        
                return;
        }
        m_a_cnt=Up_Down(m_a_cnt,total_wifi_cnt);
    }
}

void Select_Wifi_Header(UINT8 selflag)
{
    Display_Screen_Boarder();
    
    if(selflag=='A')
        STRCPY(LBUF,"   *AUTO SCAN*");
    else
        STRCPY(LBUF,"  *MANUAL LIST*");

    Disp_Header_Boarder();//Disp_Header_No_Boarder();
}


void Select_Wifi_Manual(void)
{   
    UINT8 wifi_cnt;
    
    Select_Wifi_Header('M');  
    
    Up_Down_Select();
    
    for(wifi_cnt=0;wifi_cnt<5;wifi_cnt++)
    {
        EEPROM_ADDR=WIFI_Details_Loc+(wifi_cnt*WIFI_Record_Length);
        SEERAMReadString(EEPROM_ADDR,32,(char *)&AUTO_WIFI_rec);
            
        PTR_ADDR=(char *)&WIFI_LIST[wifi_cnt][0];
        strncpy(PTR_ADDR,AUTO_WIFI_rec.WIFI_NAME,16);
    }
    Select_WIFI_From_List(5);
    
}


void Select_Wifi_Auto(void)
{    
    UINT8 wifi_cnt;
    
    Select_Wifi_Header('A');
    
    Please_Wait_Msg(LINE5);
    
    wifi_cnt=Show_WIFI_List();
    
    Up_Down_Select();
    
    if(wifi_cnt==0xFF)
    {
        STRCPY(LBUF,"Please Try Again....");
        Disp_Msg(LINE5);
        Delay_Sec(2);
        return;
    }
    
    if(WIFI_LIST_COUNT==0)
    {
        STRCPY(LBUF,"NO WIFI AVAILABLE...");
        Disp_Msg(LINE4);
        
        STRCPY(LBUF,"    SCAN AGAIN...   ");
        Disp_Msg(LINE6);
        Delay_Sec(2);
        return;
    }
    
    Select_WIFI_From_List(WIFI_LIST_COUNT);
    
}

void Wifi_Scan_SubMenu(UINT8 subpccnt)
{
    UINT8 temp;
    if(LOGIN_TYPE=='O')
        return;

    if(subpccnt==0)
        Select_Wifi_Auto();
    
    if(subpccnt==1)         
        Select_Wifi_Manual();
    
    SEERAMReadString(Wifi_User_Name_Loc,32,(char *)&WIFIrec1);
    
    temp=0;
}

void WIFI_Selection(void)
{
    UINT8 menu_cnt=0,select_fun,no_functions,menu_access[2];
     
    while(menu_cnt!=0xFF)
    {Settings_SubMenu_Header(10);
            
        PTR_ADDR=(char *)&WIFI_SELECT_MODE[menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,16); 
        
        Disp_Msg_Boarder();      
        menu_cnt=Function_Selected(2,menu_cnt);
        
        if(KBV==KB_ENT)
        {
            Wifi_Scan_SubMenu(menu_cnt);
            DISP_HEADER_FLAG=0;
        }
    }
    
}


void WIFI_Details_List(void)
{
    UINT8 menu_cnt=0,select_fun,no_functions=5;
      
    Display_Screen_Boarder();

    while(menu_cnt!=0xFF)
    {
        Display_Box_Line4();
        STRCPY(LBUF,"  *WIFI SAVED LIST*");
        Disp_Msg(LINE1);       
        
        Back_Select();
        
        PTR_ADDR=(char *)&WIFI_ENTRY_LIST[menu_cnt][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Boarder(); 
        
        menu_cnt=Function_Selected(5,menu_cnt);
                
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;

            Wifi_Details_SubMenu(menu_cnt);

            TIME_FLAG=1;            
        }        
        
    }   
}

void WIFI_SubMenu(UINT8 wificnt)
{
    if(wificnt=='A')
        WIFI_Selection();      
    
    if(wificnt=='B')
        WIFI_Details_List();
}

void Send_SCT_Close(void)
{    
    Delay_250Ms(1);//Delay_Sec(1);
    GPRS_SendByte('+');
    GPRS_SendByte('+');
    GPRS_SendByte('+');
    //Delay_Sec(1);
    Delay_250Ms(1);
    STRCPY(SEND_BUF,"AT+CIPCLOSE\r\n");
    GPRS_Send_HexData(SEND_BUF,13); 
    
    Expected_Response='O';

    //GPRS_RecString_Cmd(REC_BUF2); 
}
