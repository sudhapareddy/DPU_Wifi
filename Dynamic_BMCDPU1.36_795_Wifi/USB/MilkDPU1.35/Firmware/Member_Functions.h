
#ifdef NAME_PH_SEARCH1
UINT16 Get_Member_Name(UINT8 searchoption,UINT8 displine)
{
    UINT8 search_cnt,dis_pos;
    
    while(1)
    {
        Display_Boxes(displine,1);
        if(searchoption=='N')
        {
            STRCPY(LBUF,"Name ");
            search_cnt=15;
        }
        else
        {
            STRCPY(LBUF,"Phone No ");
            search_cnt=10;
        }
        Reset_Display_Values();
        
        DISPLAY_LAST_LINE=0xF0;
        dis_pos=20-search_cnt;
        DISPLAY_NO_SPACES=1;
        Invert_Disp_Msg(displine);
        DISPLAY_NO_SPACES=0;    
        
        STRCPY(SEARCH_BUF,"               ");
        
        GET_VALUE_STATUS=Get_String1(search_cnt,searchoption,NO_OF_CUSTOMERS,displine+dis_pos,(char *)&SEARCH_BUF);      
                
        Reset_Display_Values();
              
        if(GET_VALUE_STATUS=='L' || GET_VALUE_STATUS==0xFF)
            return 0xFFFF;
        
        else if(GET_VALUE_STATUS==0xF0)
            return 0xF0F0;

        else if(MODE_FLAG==1 && GET_VALUE_STATUS==0)
        {
            MODE_FLAG=0;
            return 0;
        }

        else if(GET_VALUE_STATUS>0 && GET_VALUE_STATUS!=0xFF && KBV!=KB_LEFT)
        { 
            Read_Member_CODE(RECORD_NO);
            
            return Memberrec.C_CODE;          
        }
    }
}
#endif

UINT16 Get_Bar_Member_Code(char *bararray)
{
    BYTE bar_code_array[10];
    UINT16 g_member_code=0,tempi;
    UINT32 g_bar_code;
    
    strncpy(bar_code_array,bararray,7);
    g_bar_code=atol(bararray);
            
    if(g_bar_code>=10000)            
    {
        for(tempi=1;tempi<=NO_OF_CUSTOMERS;tempi++)
        {
            Read_Member_Details(tempi);
            if((strncmp((char *)&Memberrec.BARCODE[0],"sct",3)==0) || (strncmp((char *)&Memberrec.BARCODE[0],"SCT",3)==0) && strncmp((char *)&Memberrec.BARCODE[3],(char *)&bar_code_array,7)==0)
            {
                g_member_code=Memberrec.C_CODE;
                break;
            }
        }
    }
    else
        g_member_code=g_bar_code;
    
    if(g_member_code>0 && g_member_code<10000)
        return g_member_code;
    else
        return 0xFFF0;
}

UINT16 Get_Member_Number_Small(UINT16 mcode,UINT8 displine)
{    
    BYTE tarray[10],c_search_option;
    UINT16 g_member_code=0;
    
    Set_Cursor(displine);
    Display_Char('_');
    
#ifdef NAME_PH_SEARCH1
    
    if(NAME_SEARCH!=0 && NAME_SEARCH!='N')
    {
        c_search_option='N';
        if(NAME_SEARCH==2)
            c_search_option='P';
               
        g_member_code=Get_Member_Name(c_search_option,LINE2);
        
        return g_member_code;
    }    
#endif

    while(1)
    {
        if(mcode>0)
        {
            Hex_DEC(0,mcode,tarray,4);
            tarray[4]='\0';
        }
        else
        {
            STRCPY((char *)&tarray,"       ");
            tarray[7]='\0'; 
        }
                    
        GET_VALUE_STATUS=Get_Numeric(7,0,displine,(char *)&tarray);
             
        if(GET_VALUE_STATUS==0xFF)
            return 0xFFFF;
        
        else if(GET_VALUE_STATUS==0xF0)
            return 0xF0F0;
        
        if(GET_VALUE_STATUS==0xF9)
            return 0xF9F9;

        else if(MODE_FLAG==1 && GET_VALUE_STATUS==0)
        {
            MODE_FLAG=0;
            return 0;
        }

        else if(GET_VALUE_STATUS>0 && GET_VALUE_STATUS!=0xFF)
        {
            g_member_code=Get_Bar_Member_Code(tarray);
            
            return g_member_code;          
        }
    }
}


UINT16 Get_Member_Number(UINT16 mcode,UINT8 displine)
{
    BYTE tarray[10];
    UINT16 g_member_code=0;

    while(1)
    {       
        STRCPY(LBUF," CODE|         ");
        LBUF[0]=0x83;
        LBUF[15]=0x85;
        Hex_DEC(0,mcode,tarray,4);
        tarray[4]='\0';
        if(mcode!=0)
        {
            Hex_DEC(0,mcode,LBUF+6,4);
        }
        else
        {
            STRCPY((char *)&tarray,"          ");
        }
      
        NOR_DISPLAY_LAST_LINE=0xC0;
        Disp_Msg_Big(0,displine);

        GET_VALUE_STATUS=Get_Numeric_Big(7,0,displine+6,(char *)&tarray);   //   211116 4       
        
        Reset_Display_Values();
        
        if(KBV!=KB_LEFT && GET_VALUE_STATUS==0xFF)
            return 0xFFFF;

        else if(MODE_FLAG==1 && GET_VALUE_STATUS==0)
        {
            MODE_FLAG=0;
            return 0;
        }

        else if(GET_VALUE_STATUS>0 && GET_VALUE_STATUS!=0xFF)
        {
            g_member_code=Get_Bar_Member_Code(tarray);
            
            return g_member_code;
            
        }
    }
}

UINT8 Member_Code_Entry(UINT8 hflag)
{
    UINT16 e_member_sno;

    if(hflag!=0)
        Member_Header(hflag);

    Member_Code=Get_Member_Number(0,LINE3);

    if(Member_Code==0xFFFF)
        return 0xFF;

    e_member_sno=Read_Member_SNO(Member_Code);

    if(e_member_sno!=0)
    {
        Read_Member_Details(e_member_sno);
        Tablerec.MILK_TYPE=Memberrec.MILK_TYPE;

        if(Memberrec.STATUS!='A' && hflag!='A')
        {
            STRCPY(LBUF,"In-Active Code");
            Disp_Msg_No_Spaces(0,LINE6+1);
            Wait_Key_Press('C');
            return 0;
        }
        else if(Memberrec.STATUS=='A' && hflag=='A')
        {
            STRCPY(LBUF,"Already Active");
            Disp_Msg_No_Spaces(0,LINE6+1);

            Delay_Sec(1);
            return 0;
        }
        else
           return 1;
    }
    else
    {
        STRCPY(LBUF," Invalid Code ");
        Disp_Msg_No_Spaces(0,LINE6+1);

        Delay_Sec(1);
        return 0;
    }
}

UINT8 Select_Milk_Type1(UINT8 stype)
{
    UINT8 select_count=1;
        
    if(stype!=2)
    {
        Up_Down_Select();
        
        Display_Selected_Boarder(LINE3,0x18);
        Display_Bar_Big_T(LINE3,10,0x89);
        
        if(stype==0)
            Display_Shift(LINE3);
        else
        {
            Copy_MonthRecDT(LBUF);
            Disp_Msg(LINE3);
        }
    }
    else
    {
        select_count=Memberrec.MILK_TYPE-0x42;
      
        Disp_Up_Down_Select();
    }
    
    STRCPY(LBUF," MILK TYPE|    ");
    LBUF[0]=0x83;
    LBUF[15]=0x85;
    NOR_DISPLAY_LAST_LINE=0xC0;
    Disp_Msg_Big(0,LINE5);    
    
    while(1)
    {
        DISPLAY_NO_SPACES=1;
        CURSOR_BLINK=0x00;
        DISPLAY_LAST_LINE=0x30;

        if(select_count==1)
            STRCPY(LBUF," COW");

        if(select_count==0)
            STRCPY(LBUF," BUF");
         
         Disp_Msg_Big(1,LINE5+11);
         Reset_Display_Values();

        KBV=Wait_Key_Press('C');

        switch(KBV)
        {
            case KB_CANCEL:

                return 0xFF;

            case KB_LEFT:

                return 'L';

            case KB_ENT:

                if(stype==2)
                    Memberrec.MILK_TYPE=0x42+select_count;
                

                return (0x42+select_count);              

        }
        select_count=Up_Down(select_count,2);
    }
}

UINT8 Select_Commission_Type(void)
{
    UINT8 s_c_count=0;

    Disp_Up_Down_Select();

    if(Memberrec.COMMISSION_TYPE=='N')
        s_c_count=0;
    else
        s_c_count=Memberrec.COMMISSION_TYPE-0x30;
     
    SEERAMReadString(Normal_Commission_Loc,Commission_Length,(char *)&Incentiverec);
    
    while(1)
    {
        if(s_c_count==0)
            STRCPY(LBUF," NOR ");
        else
        {
            STRCPY(LBUF,"SPL-1");
            LBUF[4]=s_c_count+0x30;
        }
        LBUF[5]=0x00;
        DISPLAY_LAST_LINE=0x98;
        Disp_Msg_No_Spaces(1,LINE4+10);
        Reset_Display_Values();
        
        STRCPY(LBUF,"Incentive/Ltr: 00.00");
        if(s_c_count==0)
            Hex_DEC(2,Incentiverec.NOR_COMM,LBUF+15,5);
        else
             Hex_DEC(2,Incentiverec.SPL_COMM[s_c_count-1],LBUF+15,5);        
        
        Disp_Msg(LINE6);

        KBV=Wait_Key_Press('C');//e

        switch(KBV)
        {
            case KB_LEFT:

                return 'L';

            case KB_CANCEL:

                return 0xFF;

            case KB_ENT:

                if(s_c_count==0)
                    Memberrec.COMMISSION_TYPE='N';
                else
                    Memberrec.COMMISSION_TYPE=s_c_count+0x30;

                return 1;
        }
        s_c_count=Up_Down(s_c_count,9);
    }
}

void Member_Header(UINT8 headertype)
{
    Display_Screen_Boarder();
        
    if(headertype=='A')
        STRCPY(LBUF,"  *ACTIVATION*");
    
    if(headertype=='B')
        STRCPY(LBUF,"*PRINT BARCODE*");
    
    if(headertype=='D')
        STRCPY(LBUF,"*DE-ACTIVATION*");
   
    if(headertype=='E')
        STRCPY(LBUF,"  *EDIT MEMBER*");
    
    if(headertype=='N')
        STRCPY(LBUF,"*ADD NEW MEMBER*");
   
    if(headertype=='1')
        STRCPY(LBUF," *SYNC MEMBERS*");
     
     if(headertype=='M')
     {
        Display_Box_Line4();
        STRCPY(LBUF,"  *MEMBER DETAILS*"); 
        Disp_Msg(LINE1);
        Back_Select();
     }
     else if(headertype=='m')
     {
         STRCPY(LBUF," *MEMBERS LIST*");
         Disp_Header_No_Boarder();
     }
     else
     {
        Disp_Header_Boarder();
     }
}
void Display_Code_Milk_Com(UINT8 htype,UINT8 level)
{
    UINT8 less;
    
    Member_Header(htype);
       
    Display_Boarder3();
    
    NOR_DISPLAY_LAST_LINE=0x60;

    if(level<4)
    {
        STRCPY(LBUF,"CODE | MILK|INCENTIV");     
        Invert_Disp_Msg(LINE3);   
    }

    STRCPY(LBUF,"               ");
    LBUF[0]=0x83;
    LBUF[5]=0x87;//0x84;
    LBUF[9]=0x87;//0x83
    LBUF[15]=0x85;
    ZERO=0;
    Hex_DEC(0,Member_Code,LBUF+1,4);

    if(level>1)
    {
        LBUF[6]=Memberrec.MILK_TYPE;
        if(Memberrec.MILK_TYPE=='C')
        {
            LBUF[7]='O';
            LBUF[8]='W';
        }
        else
        {
            LBUF[7]='U';
            LBUF[8]='F';
        }
    }
    if(level>2)
    {
        if(Memberrec.COMMISSION_TYPE=='N')
        {            
            LBUF[10]=Memberrec.COMMISSION_TYPE;
            LBUF[11]='O';
            LBUF[12]='R';
            LBUF[13]='.';   
        }
        else
        {
            LBUF[10]='S';
            LBUF[11]='P';
            LBUF[12]='L';
            LBUF[13]='-';   
            LBUF[14]=Memberrec.COMMISSION_TYPE;   
        }
             
    }
   
    if(level<4)
        Disp_Msg_Big(0,LINE4);
    else
        Disp_Msg_Big(0,LINE3);
    Reset_Display_Values();

    STRNCPY(LBUF,Memberrec.NAME,20);
    
    if(level==3)
        Disp_Msg(LINE7);
    
    if(level>=4)
    {       
        Display_Selected_Boarder(LINE5,0xC0);
        
        STRNCPY(LBUF,Memberrec.NAME,20);
        Disp_Msg(LINE5);
    }
    
    if(level==5)
    {
        STRCPY(LBUF,"BAR CODE: ");
        STRNCPY(LBUF+10,Memberrec.BARCODE,10);
        Disp_Msg(LINE6);
    } 
    
    if(level==4 || level==5)
    {  
        if(level==5)
        {
            DISPLAY_FIRST_LINE=0x01;
            STRCPY(LBUF," Ph.");

            STRNCPY(LBUF+5,Memberrec.CONTACT_NO,10);
        }
        else
        {
            STRCPY(LBUF," BAR");
            STRNCPY(LBUF+5,Memberrec.BARCODE,10);
        }
        LBUF[0]=0x83;
        LBUF[4]=0x83;   
        LBUF[15]=0x85;
       
        //
        NOR_DISPLAY_LAST_LINE=0xC0;
        Disp_Msg_Big(0,LINE7);
    }
   
    Reset_Display_Values();
}

void Add_Edit_Member(UINT8 hflag)
{
    UINT8 a_step=0,s_flag,a_temp_milk,b_match,clear_all_flag,prev_milk_Sel='C';
    UINT16 a_member_sno,tempi;
    UINT32 a_temp_b_code;
    
    while(1)
    {
        switch(a_step)
        {
            case 0:
                
                Member_Header(hflag);
                ZERO=0;
                
                if(hflag=='N') 
                {
                    if(NO_OF_CUSTOMERS>=1000)
                    {
                        STRCPY(LBUF,"MEMBERS CAPACITY");
                        Disp_Msg_Big(0,LINE4);
                        
                        STRCPY(LBUF,"  IS 0250 ONLY");
                        Hex_DEC(0,NO_OF_CUSTOMERS,LBUF+5,4);
                        Disp_Msg_Big(0,LINE6);
                        
                        Exit_Any_Key(1);
                        return;
                    }
                    STRCPY(LBUF,"  MEMBER S.NO:");
                    Hex_DEC(0,NO_OF_CUSTOMERS+1,LBUF+14,4);
                    Invert_Disp_Msg(LINE8);
                    
                    Member_Code=Get_Member_Number((MAX_CODE+1),LINE3);
                    
                    if(Member_Code==0xFFFF )
                        return;
                    
                    else if(Member_Code==0xFFF0)
                        a_step=0;
                    else
                    {
                        a_member_sno=Read_Member_SNO(Member_Code);
                        if(a_member_sno!=0 )
                        {
                            Read_Member_Details(a_member_sno);
                            
                            Display_Box_Boarder(LINE7);
                            STRCPY(LBUF,"Already Added ");
                            Disp_Msg_No_Spaces(0,LINE6+1);
                           
                            Wait_Key_Press('C');    
                        }
                        else
                            a_step=1;
                    }

                    if(a_step==1)
                    {
                        Memberrec.C_CODE=Member_Code;
                        STRCPY(Memberrec.NAME,"MEMBER              ");
                        Hex_DEC(0,Memberrec.C_CODE,Memberrec.NAME+7,4);
                        STRCPY(Memberrec.CONTACT_NO,"          ");
                        STRCPY(Memberrec.BARCODE,"SCT100    ");
                        Hex_DEC(0,Memberrec.C_CODE,Memberrec.BARCODE+6,4);
                        Memberrec.MILK_TYPE=prev_milk_Sel;//'C';
                        Memberrec.COMMISSION_TYPE='N';
                    }
                }
                else
                {
                    a_step=Member_Code_Entry('E');

                    if(a_step==0xFF)
                        return;

                    a_member_sno=Read_Member_SNO(Member_Code);
                }

                break;

            case 1:
                
                if(Select_Milk_Type1(2)==0xFF)
                    return;

                if(KBV==KB_LEFT)
                    a_step=0;
                else
                    a_step=2;
                break;

            case 2:

                Display_Code_Milk_Com(hflag,2);

                if(Select_Commission_Type()==0xFF)
                    return;

                if(KBV==KB_LEFT)
                    a_step=0;
                else
                    a_step=3;
                break;

            case 3:

                Display_Code_Milk_Com(hflag,3);
                a_temp_milk=Memberrec.MILK_TYPE;

                STRNCPY(Temp_Arr,Memberrec.NAME,20);
//                GET_VALUE_STATUS=Get_String(20,LINE7,(char *)&Temp_Arr);//subbu
                GET_VALUE_STATUS=Get_String_New(FONT_SMALL,20,NO_TERMINATOR,LINE7,(char *)&Temp_Arr);//subbu


                Memberrec.MILK_TYPE=a_temp_milk;
                if(GET_VALUE_STATUS==0xFF)
                    a_step=2;
                else
                {
                    STRNCPY(Memberrec.NAME,Temp_Arr,20);
                    a_step=4;
                }
                break;
            case 4:

                Display_Code_Milk_Com(hflag,4);
                a_temp_milk=Memberrec.MILK_TYPE;

                STRNCPY(Temp_Arr,Memberrec.BARCODE,10);
                GET_VALUE_STATUS=Get_String_Big(10,200,LINE7+5,(char *)&Temp_Arr);

                Memberrec.MILK_TYPE=a_temp_milk;
                if(GET_VALUE_STATUS==0xFF)
                    a_step=3;
                else
                {
                    a_temp_b_code=atol(Temp_Arr+3);
                    if((( strncmp((char *)&Temp_Arr[0],"sct",3)==0) || (strncmp((char *)&Temp_Arr[0],"SCT" ,3)==0)) && a_temp_b_code>=10000)
                    {                        
                        b_match=0;
                        
                        if(NO_OF_CUSTOMERS>0)
                        {
                            tempmemberrec=Memberrec;
                            for(tempi=1;tempi<=NO_OF_CUSTOMERS;tempi++)
                            {
                                Read_Member_Details(tempi);

                                if((hflag=='N' || (hflag=='E' && tempmemberrec.C_CODE!=Memberrec.C_CODE))&& strncmp((char *)&Temp_Arr[0],Memberrec.BARCODE,10)==0)
                                {
                                    STRCPY(LBUF," Duplicate Bar Code");
                                    Invert_Disp_Msg(LINE8);
                                    Delay_Sec(1);
                                    CLEAR_DATA_LINES_FROM(8,8);
                                    b_match=1;
                                }
                            }
                            Memberrec=tempmemberrec;
                        }
                        
                        if(b_match==0)
                        {
                            STRNCPY(Memberrec.BARCODE,Temp_Arr,10);
                            a_step=5;
                        }
                    }
                    else
                    {
                        STRCPY(LBUF,"  Invalid Bar Code");
                        Invert_Disp_Msg(LINE8);
                        Delay_Sec(1);
                        CLEAR_DATA_LINES_FROM(8,8);
                    }
                }
                break;

            case 5:

                Display_Code_Milk_Com(hflag,5);

                STRNCPY(Temp_Arr,Memberrec.CONTACT_NO,10);
                GET_VALUE_STATUS=Get_Numeric_Big(10,101,LINE7+5,(char *)&Temp_Arr);
                Reset_Display_Values();
                if(KBV==KB_LEFT)
                {
                    a_step=4;
                    break;
                }
                else if(GET_VALUE_STATUS==0xFF)
                    return;

                else if(GET_VALUE_STATUS!=0xFF)
                {
                    STRNCPY(Memberrec.CONTACT_NO,Temp_Arr,10);

                    if(hflag=='N')
                    {
                        NO_OF_CUSTOMERS++;
                        Memberrec.STATUS='A';
                        Memberrec.UPDATE_STATUS='N';
                    }
                    else
                    {
                        Memberrec.C_CODE=Member_Code;

                        if(Memberrec.UPDATE_STATUS!='N')
                            Memberrec.UPDATE_STATUS='E';
                    }
                    Display_Boarder3();
                    
                    if(NO_OF_CUSTOMERS==1 && hflag=='N')
                    {
                        Set_Delete_All_Members_Flag();
                    }
                    
                    if(GPRSrec.GPRS_ON=='Y' &&  ON_LINE=='Y' && GPRSrec.AUTO_TRANSFER=='Y')
                    {
                        MODEM_ERROR=Connect_GPRS();
                        if(MODEM_ERROR=='S')
                        {
                            SEERAMReadString(Clear_Memberlist_Flag_Loc,1,(char *)&clear_all_flag);
                            if(clear_all_flag==1)
                            {
                                MODEM_ERROR=Transfer_Member_Data(2);        //Clear All Members

                                if(MODEM_ERROR=='S')
                                {
                                    Reset_Delete_All_Members_Flag();
                                }
                            }
                            
                            SEERAMReadString(Clear_Memberlist_Flag_Loc,1,(char *)&clear_all_flag);
                            if(clear_all_flag==0xFF)
                            {                                
                                MODEM_ERROR=Transfer_Member_Data(0);
                                if(MODEM_ERROR!='S')
                                {
                                    Set_Member_Flag();
                                }   
                            }
                            else
                                Set_Member_Flag();
                            
                            if(MODEM_ERROR=='S')
                            {
                                Server_Messages();
                                Disconnect_Modem();
                            }           
                            else
                            {
                                 if(CONNECT_FLAG==1)
                                {
                                    Disconnect_Modem();
                                    CONNECT_FLAG=0;
                                }
                            }
                        }
                        else
                        {
                            Server_Messages();
                            Set_Member_Flag();
                            if(CONNECT_FLAG==1)
                            {
                                Disconnect_Modem();
                                CONNECT_FLAG=0;
                            }
                        }
                    }
                    else
                        Set_Member_Flag();

                    DISPLAY_NO_SPACES=1;
                    Please_Wait_Msg(LINE5);

                    if(hflag=='N')
                    {
                        Write_Member_Details_Flash(NO_OF_CUSTOMERS);
                        prev_milk_Sel=Memberrec.MILK_TYPE;

                    }
                    else if(hflag=='E')
                        Edit_Member_Details_Flash(a_member_sno,Memberrec.C_CODE);

                    STRCPY(LBUF,"DETAILS SAVED ");
                    Disp_Msg_No_Spaces(0,LINE5+1);

                    Buzzer(250);
                    Buzzer(250);
                    Reset_Display_Values();
                    a_step=0;
                    break;
                }
        }
    }
}

UINT8 Active_Inactive(UINT8 atype)
{
    UINT8 select_count=0;

    CURSOR_BLINK=0xC0;
    STRCPY(LBUF," DEACTIVATE|   ");
    if(atype=='A')
        STRCPY(LBUF,"  ACTIVATE |   ");
    LBUF[0]=0x83;
    //LBUF[11]=0x84;
    LBUF[15]=0x85;
   // DISPLAY_FIRST_LINE=0x01;
    Disp_Msg_Big(0,LINE7);
    Reset_Display_Values();

    while(1)
    {
        DISPLAY_LAST_LINE=0x30;

        if(select_count==0)
            STRCPY(LBUF,"YES");

        else if(select_count==1)
            STRCPY(LBUF," NO");

         Disp_Msg_No_Spaces(1,LINE7+12);
         Reset_Display_Values();

        KBV=Wait_Key_Press('C');//E

        switch(KBV)
        {
            case KB_CANCEL:

                return 0xFF;

            case KB_LEFT:

                return 'L';

            case KB_ENT:

                if(select_count==0)
                    return 'Y';

                else if(select_count==1)
                    return 'N';

        }
        select_count=Up_Down(select_count,2);
    }
}

void Deactivate_Member(UINT8 type)
{
    UINT8 confirm_count=0,d_step=0,h_flag,a_flag;
    UINT16 d_member_sno;

    while(1)
    {
        switch(d_step)
        {
            case 0:

                if(SMS_Flag=='A')
                    h_flag='A';

                else
                    h_flag='D';
                if(type=='B')
                    h_flag='B';
                d_step=Member_Code_Entry(h_flag);

                if(d_step==0xFF)
                    return;

                d_member_sno=Read_Member_SNO(Member_Code);
                    
                break;

            case 1:

                if(Memberrec.STATUS=='A' || (Memberrec.STATUS!='A' && SMS_Flag=='A'))
                {
                    Display_Code_Milk_Com(h_flag,4);
                    if(type=='D')
                    {
                        a_flag=Active_Inactive(SMS_Flag);

                        if(a_flag=='Y')
                            d_step=2;
                        else
                            d_step=0;
                    }
                    else
                    {
                        Read_Member_Details(d_member_sno);
                        Print_Barcode();
                        d_step=0;
                    }

                }
                break;

            case 2:

                Display_Code_Milk_Com(h_flag,4);

                if(SMS_Flag=='A')
                    Memberrec.STATUS='A';
                else
                    Memberrec.STATUS='D';

                Memberrec.C_CODE=Member_Code;
                if(Memberrec.UPDATE_STATUS!='N')
                    Memberrec.UPDATE_STATUS=h_flag;

                if(GPRSrec.GPRS_ON=='Y' &&  ON_LINE=='Y' && GPRSrec.AUTO_TRANSFER=='Y')
                {
                    MODEM_ERROR=Connect_GPRS();
                    if(MODEM_ERROR=='S')
                    {
                        MODEM_ERROR=Transfer_Member_Data(0);
                        if(MODEM_ERROR!='S')
                        {
                            Set_Member_Flag();

                        }
                        Server_Messages();
                        Disconnect_Modem();
                        
                    }
                    else
                    {
                        Set_Member_Flag();
                        Server_Messages();
                    }
                }
                else
                    Set_Member_Flag();

                Please_Wait_Msg(LINE5);

                Edit_Member_Details_Flash(d_member_sno,Memberrec.C_CODE);

                if(SMS_Flag=='A')
                    STRCPY(LBUF," ACTIVATED... ");
                else
                    STRCPY(LBUF," DE-ACTIVATED ");
                Disp_Msg_No_Spaces(0,LINE5+1);

                Delay_Sec(1);

                d_step=0;
                break;
        }
    }
}

void Delete_All_Members(void)
{
    UINT8 c_a_members_flag;
    
    Member_Header('m');
    
    STRCPY(LBUF,"Delete All Members?");

    if(Confirm_Yes()==1)
    {
        Display_Boarder3();
        
        STRCPY(LBUF,"DELETING MEMBERS");
        Disp_Msg_No_Spaces(0,LINE4+1);

        Please_Wait_Msg(LINE6);
           
        Erase_Member_Details();
        SEERAMReadString(Clear_Memberlist_Flag_Loc,1,(char *)&c_a_members_flag);
        if(c_a_members_flag!=1)
        {
            Set_Delete_All_Members_Flag();
        }
       
        Memberrec.UPDATE_STATUS='X';
        if(GPRSrec.GPRS_ON=='Y' &&  ON_LINE=='Y' && GPRSrec.AUTO_TRANSFER=='Y' && SIM_INSERTED==1)
        {
            MODEM_ERROR=Connect_GPRS();
            if(MODEM_ERROR=='S')
            {
                MODEM_ERROR=Transfer_Member_Data(2);
                if(MODEM_ERROR=='S')
                {
                    Reset_Delete_All_Members_Flag();
                }
                Server_Messages();
                Disconnect_Modem();
            }
            else
            {
                Server_Messages();
               
                if(CONNECT_FLAG==1)
                {
                    Disconnect_Modem();
                    CONNECT_FLAG=0;
                }
            }           
        }
        
        NO_OF_CUSTOMERS=0;
        MIN_CODE=0;
        MAX_CODE=0;

        STRCPY(LBUF," TOTAL MEMBERS");
        Disp_Msg_No_Spaces(0,LINE4+1);

        STRCPY(LBUF,"    DELETED");
        Disp_Msg_No_Spaces(0,LINE6+1);

        Delay_Sec(1);

        Clear_LCD();
    }
}

void Print_Barcode(void)
{
    UINT8 tempi;
    
    STRNCPY(PBUF,Memberrec.NAME,20);
    Printer_SendString(3,20);
    Printer_SendByte(0x1D);
    Printer_SendByte(0x6B);
    Printer_SendByte(69);
    Printer_SendByte(10);
    
    for(tempi=0;tempi<10;tempi++)
    {
        Printer_SendByte(Memberrec.BARCODE[tempi]);
    }
  
    Printer_SendByte(0x0D);    
    Paper_Feed(2);  
}

void Member_SubMenu(UINT8 membersubcnt)
{
    if(membersubcnt=='A')
        Add_Edit_Member('N');
    
    if(membersubcnt=='B')
        Add_Edit_Member('E');
    
    if(membersubcnt=='C')
    {
        SMS_Flag='A';
        Deactivate_Member('D');
        SMS_Flag='N';
    }
    
    if(membersubcnt=='D')
        Deactivate_Member('D');
     
    if(membersubcnt=='E')
        Deactivate_Member('B');
            
    if(membersubcnt=='H')
    {
        if(Password_Entry1('M')==1)
            Delete_All_Members();        
    }
    
    Clear_LCD();
}

UINT8 Select_Member_Fun(UINT8 smcnt)
{    
    UINT8 add_member_flag;
    
    MEMBER_TOTAL_FUNS=5;
    
    if(LOGIN_TYPE=='M' || LOGIN_TYPE=='D') 
    {
        MEMBER_TOTAL_FUNS=MEMBER_TOTAL_FUNS+1;
    }
    
    add_member_flag=1;
    if(DPUcontrolrec.MEMBER_LIST=='N')
    {
        add_member_flag=0;
        MEMBER_TOTAL_FUNS=MEMBER_TOTAL_FUNS-1;
    }
      
    if(add_member_flag==1 && smcnt==0)
    {
        STRCPY(LBUF," 1.ADD MEMBER");
        STRCPY(PBUF,"Code,Name,Milk,Phone ");

        return 'A';
    }
                
    if(smcnt==add_member_flag)
    {
        STRCPY(LBUF," 2.EDIT MEMBER");
        LBUF[1]=0x31+add_member_flag;
        STRCPY(PBUF,"Code,Name,Milk,Phone ");
        
        return 'B';
    }
                
    if(smcnt==add_member_flag+1)
    {
        STRCPY(LBUF," 3.ACTIVATE");
        LBUF[1]=0x31+smcnt;
        STRCPY(PBUF,"Member Activation... ");
        
        return 'C';
    }

    if(smcnt==add_member_flag+2)
    {
        STRCPY(LBUF," 4.DE-ACTIVATE");
        LBUF[1]=0x31+smcnt;
        STRCPY(PBUF,"Member Deactivation  ");
        
        return 'D';
    }
          
    if(smcnt==add_member_flag+3)
    {
        STRCPY(LBUF," 5.PRINT BARCODE");
        LBUF[1]=0x31+smcnt;
        STRCPY(PBUF,"Print Code and Name ");
       
        return 'E';
    }
                    
    if(smcnt==(MEMBER_TOTAL_FUNS-1))
    {             
        STRCPY(LBUF," 6.CLEAR LIST");
        LBUF[1]=smcnt+0x31;
        STRCPY(PBUF,"Clears Member List  ");
        
        return 'H';
    }  
}

void Member_Details(void)
{  
    UINT8 s_cnt=0,select_fun;
    
    while(1)
    {
        Member_Header('M');

        select_fun=Select_Member_Fun(s_cnt);
        Disp_Msg_Boarder();

        STRNCPY(LBUF,PBUF,20);
        Disp_Msg(LINE5);

        KBV=Wait_Key_Press('N');

        if(KBV==KB_CANCEL)
            return;
        
        if(KBV>=0x31 && KBV<=(0x30+MEMBER_TOTAL_FUNS))
        {
            s_cnt=KBV-0x31;
            select_fun=Select_Member_Fun(s_cnt);
            KBV=KB_ENT;
        }
        if(KBV==KB_ENT)
        {
            TIME_FLAG=0;
            Member_SubMenu(select_fun);
            TIME_FLAG=1;

            if(FN_KEY==1)
            {
                FN_KEY=0;
                return;
            }
        }        
                
        s_cnt=Up_Down(s_cnt,MEMBER_TOTAL_FUNS);
    }
}