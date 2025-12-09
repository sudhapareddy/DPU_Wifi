
void Set_IP_Addr(void)
{
    GPRS_Menu_Header('I');
    Display_Boarder2();
    Save_Exit_Msg();

    STRCPY(LBUF,"SET IP ADDRESS:");
    DISPLAY_NO_SPACES=1;
    Invert_Disp_Msg(LINE4);
    DISPLAY_NO_SPACES=0;

    STRNCPY(LBUF,GPRSrec.IP_ADDR,15);
    Disp_Msg(LINE6);

    while(1)
    {
        GET_VALUE_STATUS=Get_String(15,LINE6,(char *)&GPRSrec.IP_ADDR[0]);

        switch(KBV)
        {
            case KB_CANCEL:
                return;

            case KB_ENT:                

                SEERAMWriteString(IP_Address_Loc,15,(char *)&GPRSrec.IP_ADDR);

                Saved_Message();
                return;

        }
    }
}

void Domain_Name(void)
{
    GPRS_Menu_Header('D');
    
    Display_Boarder2();
    Save_Exit_Msg();
    
    STRCPY(LBUF,"Enter Name:");
    DISPLAY_NO_SPACES=1;
    Invert_Disp_Msg(LINE3);
    DISPLAY_NO_SPACES=0;

    SEERAMReadString(Domain_Name_Loc,20,(char *)&DOMAIN_SERVER_NAME);
    STRNCPY(LBUF,DOMAIN_SERVER_NAME,20);
    Disp_Msg(LINE5);

    while(1)
    {
        GET_VALUE_STATUS=Get_String(20,LINE5,(char *)&DOMAIN_SERVER_NAME);

        switch(KBV)
        {
            case KB_CANCEL:
                return;

            case KB_ENT:

                SEERAMWriteString(Domain_Name_Loc,20,(char *)&DOMAIN_SERVER_NAME);

                Saved_Message();
                return;
        }
    }
}

void PortNo_SMSLimit(UINT8 ptype)
{
    UINT16 temp_no;

    while(1)
    {
        GPRS_Menu_Header(ptype);
        
        Display_Table_2(LINE3,15);

        if(ptype=='P')
        {
            STRCPY(LBUF,"CURRENT PORT NO|");
            EEPROM_ADDR=Port_No_Loc;
        }
        if(ptype=='S')
        {
            STRCPY(LBUF,"CURRENT LIMIT  |");
            EEPROM_ADDR=SMS_Day_Limit_Loc;
        }
        SEERAMReadString(EEPROM_ADDR,2,(char *)&temp_no);
        Hex_DEC(0,temp_no,(char *)&LBUF+16,4);        
        Disp_Msg(LINE4);

        if(ptype=='P')
            STRCPY(LBUF,"NEW PORT NUMBER|");
        if(ptype=='S')
            STRCPY(LBUF,"NEW DAY LIMIT  |");        
        Disp_Msg(LINE6);

        Temp_Arr[0]=' ';
        Temp_Arr[1]=0x00;

        GET_VALUE_STATUS=Get_Numeric(4,0,LINE6+16,(char *)&Temp_Arr);

        if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
            return;

        if(GET_VALUE_STATUS!=0xFF)
        {
            temp_no=atoi(Temp_Arr);

            if(temp_no>0)
            {
                SEERAMWriteString(EEPROM_ADDR,2,(char *)&temp_no);

                if(ptype=='P')
                    GPRSrec.PORT_NO=temp_no;
                else
                    SMS_DAY_LIMIT=temp_no;

                Saved_Message();
                break;
            }
        }
    }
}

void GPRS_On_Off(void)
{
    GPRS_Menu_Header('G');

      #ifdef WIFI
            STRCPY(LBUF,"ACTIVATE WIFI?");
#elif defined(GPRS)
            STRCPY(LBUF,"ACTIVATE GPRS?");
#endif  
    
    
    EEPROM_ADDR=GPRS_ON_Loc;

    Yes_No();
}

void Domain_On_Off(void)
{
    GPRS_Menu_Header('d');

    STRCPY(LBUF,"Connect with IP ADDR");

    EEPROM_ADDR=IP_Addr_Domain_Loc;

    Yes_No();
}

void GPRS_SubMenu(UINT8 gprscnt)
{
    UINT8 gstatus;
    SEERAMReadString(GPRS_ON_Loc,20,(char *)&GPRSrec);
    
    Clear_LCD();

    gstatus=Wait_Backend_To_Complete();
    if(gstatus==0xFF)
        return;
#ifdef GPRS
    if(gprscnt==0)
        ON_OFF('S');
    if(gprscnt==1)
        PortNo_SMSLimit('S');   
#endif
#ifdef WIFI
    if(gprscnt==0)
        WIFI_Selection();      
    
    if(gprscnt==1)
        WIFI_Details_List(); 
#endif

    if(gprscnt==2)
        Domain_Name();

    if(gprscnt==3)
        Set_IP_Addr();

    if(gprscnt==4)
        Domain_On_Off();

    if(gprscnt==5)
        PortNo_SMSLimit('P');
    
    if(gprscnt==6)
        ON_OFF('T');
    
    if(gprscnt==7)
    {
        GPRS_On_Off();  
        SIM_CHECK_STATE=0;
        INIT_MODEM_STATE=0;
        SERVER_STATE=0;
        SMS_STATE=0;
        SEERAMReadString(GPRS_ON_Loc,20,(char *)&GPRSrec);
        if(GPRSrec.GPRS_ON=='Y')
        {
            Init_GPRS();
        }
        else
        {
            Dis_GPRS_Interrupt();            
        }            
    }

    SEERAMReadString(GPRS_ON_Loc,20,(char *)&GPRSrec);
    BACKEND_FLAG='B';
}

void GPRS_Settings(void)
{
    UINT8 gprs_cnt=0,gprs_cnt_limit=8,menu_list_cnt=0;//8;
    
    //WIFI_Settings();
    
    if(SERVER_CONTROL=='Y')
        gprs_cnt_limit=6;//6   
    
    while(gprs_cnt!=0xFF)
    {
        Settings_SubMenu_Header(7);

        menu_list_cnt=0;

        PTR_ADDR=(char *)&GPRS_SETTINGS_MENU_LIST[gprs_cnt+menu_list_cnt][0];
        strncpy(LBUF,PTR_ADDR,16);
        Disp_Msg_Boarder();        
       
        PTR_ADDR=(char *)&GPRS_SETTINGS_MENU_DESC_LIST[gprs_cnt+menu_list_cnt][0];
        strncpy(LBUF,PTR_ADDR,20);
        Disp_Msg(LINE5);

        gprs_cnt=Function_Selected(gprs_cnt_limit,gprs_cnt);
        
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;

            GPRS_SubMenu(gprs_cnt);

            TIME_FLAG=1;            
        }        
    }
}