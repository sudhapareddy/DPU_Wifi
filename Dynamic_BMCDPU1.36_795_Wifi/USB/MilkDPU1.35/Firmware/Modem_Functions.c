void Send_End_Command(void)
{
    STRCPY(SEND_BUF,"#SCTEND:");
    STRNCPY(SEND_BUF+8,MACHINE_ID,7);
    SEND_BUF[15]='!';
    SEND_BUF[16]='\r';
    GPRS_SendString(SEND_BUF);
    GPRS_RecString_Data(REC_BUF);
}

void Modem_Flags_Reset(void)
{
    GPRS_OVERFLOW=0;
    No_Response=0;
    SIM_INSERTED=0;
    Modem_Init_Flag=0;
    SIM_CHECK_STATE=0;
    SMS_STATE=0;
    SERVER_STATE=0;
    INIT_MODEM_STATE=0;        
}


UINT8 Connect_GPRS(void)
{
    UINT8 gstatus;
    
    CONNECT_FLAG=0;
    
    Display_Boarder3();
    
    gstatus=Wait_Backend_To_Complete();    
    
    if(gstatus==0xFF)
        return;
    
    STRCPY(LBUF,"SUNServer Connecting");
    Disp_Msg(LINE5);
    
    if(SIM_INSERTED==1 && Modem_Init_Flag==1 )
    {
        SERVER_STATE=0;
        Only_Connect=1;
        do
        {
            Connect_Server_Backend();
        }while(SERVER_STATE!=0 && SERVER_STATE!=9);
        Only_Connect=0;
    }
    else
        return 0x02;

    if(SERVER_STATE==9 && CONNECT_FLAG==1)
    {
        SERVER_STATE=0;
        return MODEM_ERROR;
    }
        
    else
    {
        SERVER_STATE=0;
        Buzzer(250);
        Buzzer(250);        
        return MODEM_ERROR;
    }
}

void Add_Collection_Record(UINT8 recno,BYTE * recordbuf)
{
    UINT16 col_offset;

    col_offset=(recno*Upload_Record_Length);
    STRNCPY(recordbuf+24+col_offset,(char *)&CollectionRecord.CODE,2);
    recordbuf[26+col_offset]=CollectionFlags.MILK_TYPE;

    STRNCPY(recordbuf+27+col_offset,(char *)&CollectionRecord.FAT,2);
    STRNCPY(recordbuf+29+col_offset,(char *)&CollectionRecord.QTY,2);
    STRNCPY(recordbuf+31+col_offset,(char *)&CollectionRecord.RATE,2);

    recordbuf[33+col_offset]=Shiftrec.DAY_SHIFT;

    recordbuf[34+col_offset]=Shiftrec.DAY;

    recordbuf[35+col_offset]=Shiftrec.MONTH;

    recordbuf[36+col_offset]=Shiftrec.YEAR;

    recordbuf[37+col_offset]=CollectionRecord.WTIME[0] & 0x1F;  //excluding commission flags

    recordbuf[38+col_offset]=CollectionRecord.WTIME[1];    

    recordbuf[39+col_offset]=CollectionFlags.ANALYZER_MODE;

    recordbuf[40+col_offset]=CollectionFlags.WEIGHT_MODE;

    if(CollectionRecord.ATIME[0]==0 && CollectionRecord.ATIME[1]==0)
    {
        CollectionRecord.ATIME[0]=CollectionRecord.WTIME[0] & 0x1F;
        CollectionRecord.ATIME[1]=CollectionRecord.WTIME[1];
    }
       
    STRNCPY(recordbuf+41+col_offset,(char *)&CollectionRecord.CLR1,2);
    STRNCPY(recordbuf+43+col_offset,(char *)&CollectionRecord.WATER1,2);    
    recordbuf[45+col_offset]=CollectionRecord.ATIME[0];
    recordbuf[46+col_offset]=CollectionRecord.ATIME[1];
    
    if(CollectionRecord.INCENTIVE_AMT==0xFFFFFFFF)
        CollectionRecord.INCENTIVE_AMT=0;
    
    STRNCPY(recordbuf+47+col_offset,(char *)&CollectionRecord.INCENTIVE_AMT,4);
   
     recordbuf[51+col_offset]=CollectionRecord.WTIME[2] & 0x7F;  //Record Edit status
    if(CollectionRecord.WTIME[2]>=0x80)
    {
        recordbuf[52+col_offset]='E';
        
    }
    else
    {
        recordbuf[52+col_offset]='A';
    }
}

void GPRS_Send_End_Shift(char* gprsbuf)
{
    STRCPY(gprsbuf,"#SCTENDSHIFT:");
    STRNCPY(gprsbuf+13,MACHINE_ID,7);
    gprsbuf[20]='!';
    gprsbuf[21]='\r';
    GPRS_SendString(gprsbuf);    
}

void Send_Start_Command(void){
   
    SERVER_STATE=8;
    CONNECT_FLAG=1;

    STRCPY(GPRSSEND_BUF,"#SCTSTART:");
    STRNCPY(GPRSSEND_BUF+10,MACHINE_ID,7);
    GPRSSEND_BUF[17]='!';
    GPRSSEND_BUF[18]='\r';

    GPRS_SendString(GPRSSEND_BUF);

     Expected_Response='D';
}

void Send_Domain_PortNo(void)
{
    UINT8 ip_domain,port_sp,domain_sp,c_cnt=0,domain_ip[21],ip_addr_length;
    
    SEERAMReadString(GPRS_ON_Loc,20,(char *)&GPRSrec);

    if(GPRSrec.PORT_NO==0)
        GPRSrec.PORT_NO=3699;

    SEERAMReadString(IP_Addr_Domain_Loc,1,(char *)&ip_domain);
    
#ifdef WIFI
    
        STRCPY(GPRSSEND_BUF,"AT+CIPSTART=\"TCP\",\"");//052.025.205.160\",3699\r\n");
        domain_sp=19;

    #else
    
        STRCPY(GPRSSEND_BUF,"AT+QIOPEN=\"TCP\",\"");
        domain_sp=17;
    #endif
    
    if(ip_domain=='Y')
    {
        strncpy(domain_ip,GPRSrec.IP_ADDR,15);
        ip_addr_length=15;
    }
   
    else
    {                
        SEERAMReadString(Domain_Name_Loc,20,(char *)&DOMAIN_SERVER_NAME);
        strncpy(domain_ip,DOMAIN_SERVER_NAME,20);
        ip_addr_length=20;
    }

    c_cnt=0;
    while(c_cnt<ip_addr_length && domain_ip[c_cnt]!=' ')
    {
        GPRSSEND_BUF[domain_sp+c_cnt]=domain_ip[c_cnt];
        c_cnt++;
    }
    port_sp=domain_sp+c_cnt;   
   
    GPRSSEND_BUF[port_sp]='\"';
    port_sp=port_sp+1;

    GPRSSEND_BUF[port_sp]=',';
    port_sp=port_sp+1;

    #ifndef WIFI
    {
        GPRSSEND_BUF[port_sp]='\"';
        port_sp=port_sp+1;
    }
    #endif
    Hex_DEC(0,GPRSrec.PORT_NO,GPRSSEND_BUF+port_sp,4);
    port_sp=port_sp+4;

    #ifndef WIFI
    {
        GPRSSEND_BUF[port_sp]='\"';
        port_sp=port_sp+1;
    }
    #endif
    GPRSSEND_BUF[port_sp]='\r';
    port_sp=port_sp+1;
    #ifdef WIFI
    
        GPRSSEND_BUF[port_sp]='\n';
        port_sp=port_sp+1;
    #endif

    GPRSSEND_BUF[port_sp]='"';
    port_sp=port_sp+1;

   //GPRS_Send_HexData(GPRSSEND_BUF,port_sp);
    //STRCPY(GPRSSEND_BUF,"AT+CIPSTART=\"TCP\",\"www.sct-gkdz.onrender.com\",3699\r\n");
    GPRS_SendString(GPRSSEND_BUF);
    
    GPRS_FLAG=0;
    Expected_Response='c';
    CONNECT_FLAG=0;
    GPRSDELAYCOUNT=0;
}

void Send_Backend_Records(void)
{
    if(GPRS_FLAG==0)
    {
        SERVER_STATE=10;
    }
    else if(strncmp(GPRSSEND_BUF,GPRS_BUF,17)==0)//22
    {
        if(Only_Connect!=1)
        {
            GPRS_Send_HexData(BACKEND_RECORDS_BUF,(25+(BACKEND_RECORD_COUNT*Upload_Record_Length)));//17
            STRNCPY(GPRSSEND_BUF,BACKEND_RECORDS_BUF,25);
        }
        else
            MODEM_ERROR='S';
        SERVER_STATE=9;

        Expected_Response='D';
    }
    else
    {                
        SERVER_STATE=10;
        GPRSDELAYCOUNT=6;
        MODEM_ERROR=0x20;
    }

    GPRS_FLAG=0;
}

void Update_Backend_Records(void)
{
    if(GPRS_FLAG==0)//40
    {
        SERVER_STATE=10;
    }

    if(strncmp(GPRSSEND_BUF,GPRS_BUF,20)==0)
    {
        Original_Update_Upload_Flag(BACKEND_RECORD_COUNT);

        GPRS_Send_End_Shift(GPRSSEND_BUF);
        SERVER_STATE=10;

        Expected_Response='D';

        GPRSDELAYCOUNT=0;
        MODEM_ERROR='S';

    }
    else
    {
        SERVER_STATE=10;
        GPRSDELAYCOUNT=6;
        GPRS_FLAG=1;
        MODEM_ERROR=0x20;                
    }

    GPRS_FLAG=0;    
}


void Disconnect_Modem(void)
{
    STRCPY(LBUF,"CLOSING CONNECTION..");
    Disp_Msg(LINE7);
#ifndef NORMAL
    Send_SCT_Close();   
    GPRS_RecString_Cmd(REC_BUF2); 
#endif

    U3STAbits.OERR=0;
    STRCPY(LBUF,"  CONNECTION CLOSED");
    Disp_Msg(LINE7);
    
    CLEAR_DATA_LINES_FROM(4,7);
}


UINT8 Connect_Server_Backend(void)
{
#ifdef WIFI
    
        Connect_WIFI_Modem();
#elif defined(GPRS)
    
        Connect_GPRS_Modem();
#endif
}

#ifdef WIFI
    # include "WIFI.h"
#elif defined(GPRS)
    # include "QuectelM95_SIM900.c"

#endif




