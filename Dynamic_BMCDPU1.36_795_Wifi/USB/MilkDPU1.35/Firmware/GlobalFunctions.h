  void Buzzer(UINT8 bdcount)
{
    BUZZER_IO=0;

    BUZZER=0;    
    DelayMs(bdcount);
    BUZZER=1;
}
  
  void parseCSVString(const char *input,UINT16 tempcnt) 
{    
    strncpy(GPRS_BUF, input, sizeof(GPRS_BUF) - 1);
    GPRS_BUF[sizeof(GPRS_BUF) - 1] = '\0';  // Null-terminate the buffer

    char *token = strtok(GPRS_BUF, ",");  // First token
    if (token == NULL) {
        return;
    }

    while (token != NULL) 
    {
        TEMP_FLOAT_VALUE = atof(token);
       // if(tempcnt>0)
        {
           RATE_BUFFER[RATE_BUF_INDEX]=(int)(TEMP_FLOAT_VALUE*100);
           //Flash_Buffer[RATE_BUF_INDEX*2]=(int)(TEMP_FLOAT_VALUE*100);
           RATE_BUF_INDEX++;
        }
        tempcnt++;
       
        token = strtok(NULL, ",");  // Get next token
    }
}

void Status_Bar(UINT8 statusline)
{
    UINT8 s_delay_cnt;

    CLEAR_BUF_FLAG=1;

    for(s_delay_cnt=0;s_delay_cnt<20;s_delay_cnt++)
    {
        LBUF[s_delay_cnt]=0x83;
        Buzzer(10);
        DelayMs(10);
        Disp_Msg(statusline);
    }

    CLEAR_BUF_FLAG=0;
}

void Day_Shift(BYTE *tbuf)
{
    STRCPY(tbuf,"EVENING");

    if(Shiftrec.DAY_SHIFT=='M')
        STRCPY(tbuf,"MORNING");
}

void Hex_ASCII(UINT8 hexvalue,BYTE * buf)
{	
    buf[1]= (hexvalue%16)|0x30;
    buf[0]=(hexvalue/16)|0x30;
}

void Copy_Date(BYTE *tbuf)
{
    Read_RTC();
    Hex_ASCII(Time[4],tbuf);
    Hex_ASCII(Time[5],tbuf+3);
    Hex_ASCII(Time[6],tbuf+6);
}

void Copy_Time(BYTE *tbuf)
{
    Hex_ASCII(Time[2],tbuf);
    Hex_ASCII(Time[1],tbuf+3);
    Hex_ASCII(Time[0],tbuf+6);
}

void Hex_DEC(UINT8 ddotpos,INT32 hvalue,BYTE * buf,UINT8 hnochars)
{
    UINT8 h_b_cnt,h_chr_cnt;
    
    NUM_SIGN='+';
    if(hvalue<0)
    {
        hvalue=(-1)*(hvalue);
        NUM_SIGN='-';
      
            buf[0]=' ';
   
        buf++;
        hnochars--;
    }
    h_chr_cnt=hnochars-1;
    while(hnochars>0)
    {
        if(ddotpos==1 && hnochars==h_chr_cnt)
            buf[hnochars-1]='.';

        else if(ddotpos==2 && hnochars==h_chr_cnt-1)
            buf[hnochars-1]='.';
        
        else if(ddotpos==3 && hnochars==h_chr_cnt-2)
            buf[hnochars-1]='.';
        
        else
        {
             buf[hnochars-1]= ((hvalue%10)|0x30);
             hvalue=hvalue/10;
        }
        hnochars--;
    }

    if(ZERO==1 || NUM_SIGN=='-')					//Eliminating Left Side Zero's
    {
        for(h_b_cnt=0;h_b_cnt<h_chr_cnt;h_b_cnt++)
        {
            if(buf[h_b_cnt]!=0x30 )
            {
                if(NUM_SIGN=='-')
                    buf[h_b_cnt-1]='-';
                break;
            }

            if(ddotpos==1 && h_b_cnt==h_chr_cnt-2)
                buf[h_b_cnt]=0x30;

            else if(ddotpos==2 && h_b_cnt==h_chr_cnt-3)
                buf[h_b_cnt]=0x30;

            else if(buf[h_b_cnt]==0x30)
                buf[h_b_cnt]=' ';
        }
    }
}

void Copy_ShiftDT(BYTE *tbuf)
{
    ZERO=0;
    Hex_DEC(0,Shiftrec.DAY,tbuf,2);
    tbuf[2]=tbuf[5]='/';
    
    if(USB_FILE==1)
    {
        tbuf[2]=tbuf[5]='.';
    }
  
    Hex_DEC(0,Shiftrec.MONTH,tbuf+3,2);
    Hex_DEC(0,Shiftrec.YEAR,tbuf+6,2);

}
void Copy_MonthRecDT(BYTE *tbuf)
{
    ZERO=0;
    STRCPY(tbuf,"dd/mm/yy TO dd/mm/yy");
    
    if(USB_FILE==1)
    {
        tbuf[2]=tbuf[5]=tbuf[14]=tbuf[17]='.';
    }
    
    Hex_DEC(0,Day_Monthrec.START_DATE,tbuf,2);
    Hex_DEC(0,Day_Monthrec.START_MONTH,tbuf+3,2);
    Hex_DEC(0,Day_Monthrec.START_YEAR,tbuf+6,2);

    Hex_DEC(0,Day_Monthrec.END_DATE,tbuf+12,2);
    Hex_DEC(0,Day_Monthrec.END_MONTH,tbuf+15,2);
    Hex_DEC(0,Day_Monthrec.END_YEAR,tbuf+18,2);
}

UINT32 Float_Long(BYTE *buf)
{
    UINT8 f_l_cnt,f_i_cnt;
    UINT32 f_long_val;

    f_l_cnt=0;
    f_i_cnt=0;

    while(buf[f_l_cnt]!='\0')
    {
        if(buf[f_l_cnt]!='.')
        {
            Int_Arr[f_i_cnt]=buf[f_l_cnt];
            f_i_cnt++;
        }
        f_l_cnt++;

        if(f_l_cnt>=20)
            f_l_cnt=0;
    }
    Int_Arr[f_i_cnt]='\0';

    f_long_val=atol(Int_Arr);

    return f_long_val;
}

UINT32 Float_Rate(BYTE *buf)
{
    UINT8 l_cnt,i_cnt,dot_pos=0,decimalpoints=0;
    UINT32 long_val;

    l_cnt=0;
    i_cnt=0;

    while(buf[l_cnt]!='\0')
    {
        if(buf[l_cnt]!='.')
        {

            Int_Arr[i_cnt]=buf[l_cnt];
            i_cnt++;
            if(dot_pos==1)
            {
                decimalpoints++;
            }
            if(decimalpoints==2)
                break;

        }
        else if(buf[l_cnt]=='.')
        {
            dot_pos=1;
        }


        l_cnt++;

        if(l_cnt>=20)
            l_cnt=0;
    }
    while(decimalpoints<2)
    {
        Int_Arr[i_cnt]='0';
        i_cnt++;
        decimalpoints++;
    }

    Int_Arr[i_cnt]='\0';

    long_val=atol(Int_Arr);

    return long_val;
}

void Shift_Left(BYTE * buf,UINT8 snok)
{
    UINT8 s_l_cnt=0,s_c_cnt=0;

    if(buf[0]=='-')
        s_l_cnt=1;
    
    for(;s_l_cnt<snok;)
    {
        if(buf[s_l_cnt]==' ')
            s_l_cnt++;
        else
            break;
    }

    if(s_l_cnt==0)
        return;
    
    if(buf[0]=='-')
        s_c_cnt=1;

    for(;s_l_cnt<snok;s_c_cnt++)
    {
        buf[s_c_cnt]=buf[s_l_cnt];
        buf[s_l_cnt]=' ';
        s_l_cnt++;
    }
}

void Shift_Right1(BYTE *buf,UINT8 length,UINT8 shiftchars)
{
    UINT8 s_r_cnt;
    
    for(s_r_cnt=length;s_r_cnt>0;s_r_cnt--)
        buf[s_r_cnt+shiftchars-1]=buf[s_r_cnt-1];
    
    for(s_r_cnt=0;s_r_cnt<shiftchars;s_r_cnt++)
        buf[s_r_cnt]=' ';    
}

void Shift_Right(BYTE * buf,UINT8 snok)
{
    UINT8 s_l_cnt,s_c_cnt=0;

    for(s_l_cnt=snok;s_l_cnt>0;s_l_cnt--)
    {
        if(buf[s_l_cnt-1]==' ')
            s_c_cnt++;
        else
            break;
    }

    if(s_c_cnt==0)
        return;
    
    Shift_Right1(buf,(snok-s_c_cnt),s_c_cnt);  
}

void Send_Tare_Command(void)
{
    if(CollectionFlag==0)
        Init_Weighing_Machine();
    
    WM_SendByte(WMrec.WM_TARE_COMMAND);  
    
    if(CollectionFlag==0)
        Disable_UART_Interrupts_WM();
}

#ifdef TOUCH_KEYPAD_ENABLE

UINT8 Scan_Touch_Key_Code(UINT16 stouchkey)
{
    UINT8 k_s_cnt=0;
    
    while(1)
    {
        if(Touch_Numeric[k_s_cnt][0]==stouchkey)
        {
            PRESSED_KEY=Touch_Numeric[k_s_cnt][1];
            
            BUZZER=0;
            DelayMs(5);
            BUZZER=1;        
            
            return PRESSED_KEY;
        }
        if(Touch_Numeric[k_s_cnt][0]==0xFFFF)
            return 0xFF;
        k_s_cnt++;
    }
}
#endif


UINT8 Get_Numeric(UINT8 gnochars,UINT8 gdp,UINT8 gline,BYTE *lbuf)
{
    UINT8 g_key_type,g_n_pos,g_dot_cnt,g_dot_flag,g_temp_count=0,g_dot_limit,rotate_flag=0;
    UINT32 g_data_value;
        
    g_dot_flag=0;
    g_dot_limit=0;
    
    if(gdp==101 || gdp==201 || gdp==200)
    {
        if(gdp!=200)
            DISPLAY_FIRST_LINE=0x01;
        NOR_DISPLAY_LAST_LINE=0xC0;
        
        if(gdp==101)
            gdp=0;
        else
            gdp=100;
    }
    
    if(gdp==100)
    {
        gdp=0;
        g_key_type='A';
    }
    else
    {
        g_key_type='N';
        if(gdp>0)
            g_dot_limit=gnochars-(gdp+1);
    }
    
    if(g_key_type=='N')
    {
        lbuf[gnochars]=0;
        g_data_value=Float_Long(lbuf);        
    }

    if(g_data_value<=0 && g_key_type=='N')
    {        
        for(g_n_pos=0;g_n_pos<gnochars;g_n_pos++)
            lbuf[g_n_pos]=' ';
    }

    LCD_Set_Cursor(FONT_TYPE,gline);
    g_n_pos=0;
    g_dot_cnt=0;
    DISP_CURSOR_FLAG=0;
    for(;;)
    {
        
        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);

        KBV=0xFF;
        
        if(g_n_pos<=gnochars)
        {
            CURSOR_FLAG=1;
            CURSOR_BLINK=0x80;

            if(FONT_TYPE=='B')
                CURSOR_BLINK=0x04;
            
            TEMP_CURSOR_BLINK=CURSOR_BLINK;
                
            if(g_n_pos==gnochars)
            {
                rotate_flag=1;
            }
            else
            {
                if(DISPCHAR_TYPE!='P')
                    Display_Char_Big(gline,g_n_pos,lbuf[g_n_pos]);
                else
                    Display_Char_Big(gline,g_n_pos,' ');
                
                LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
            }
            CURSOR_BLINK=0x00;
        }
        
        do
        {
            if(g_n_pos<=gnochars && DISP_CURSOR_FLAG==1)
            {
                DISP_CURSOR_FLAG=0;
                
                if(g_n_pos==gnochars)
                {
                    //LCD_Set_Cursor(FONT_TYPE,gline+0);
                    if(DISPCHAR_TYPE!='P')
                        Disp_Cursor_Blink(FONT_TYPE,gline,0,lbuf[0]);//Display_Char_Big(gline,0,lbuf[0]);
                    else
                        Disp_Cursor_Blink(FONT_TYPE,gline,0,'*');//Display_Char_Big(gline,0,'*');
                }
                else
                {
                    
                    Disp_Cursor_Blink(FONT_TYPE,gline,g_n_pos,lbuf[g_n_pos]);
                }
            }
            
            KBV=Scan_Key(g_key_type);						//Key sensing
            
            if(KBV!=0xFF)
            {
                CURSOR_FLAG=0;
                CURSOR_BLINK=0x00;
                DISP_CURSOR_FLAG=0;
            }
            
           
#ifdef NAME_PH_SEARCH1
            
            if(Change_Shift_Search()==1)
                return 0xF0;
            
#endif
         
           
        }while(KBV==0xFF);
       
        switch(KBV)
        {
            case KB_CANCEL:

                return 0xFF;
                
            case KB_UP:

                break;
                
            case KB_DOWN:

                break;

            case KB_LEFT:

               if(g_key_type=='N')
               {
                    return 0xFF;
               }
               else
               {
                   if(g_n_pos==0)
                       return 0xFF;

                   Display_Char_Big(gline,g_n_pos,lbuf[g_n_pos]);

                   if(g_n_pos==0)
                       g_n_pos=gnochars;
                   
                    g_n_pos=g_n_pos-1;

                   LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                   break;
               }

            case KB_RIGHT:

                if(g_key_type=='N' && TIME_RIGHT_ARROW=='Y')
                    return 0xFF;
                
               if(g_key_type=='A')
               {
                  Display_Char_Big(gline,g_n_pos,lbuf[g_n_pos]);

                   if(g_n_pos==gnochars-1)
                       g_n_pos=0;
                   else
                       g_n_pos=g_n_pos+1;

                   LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);

               }
               break;

            case KB_ENT:
                
                lbuf[gnochars] = 0x00;
                if(g_key_type=='N')
                {
                    if(FONT_TYPE=='S')
                        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                    
                    if(g_n_pos<gnochars)
                        Display_Char_Big(gline,g_n_pos,lbuf[g_n_pos]);
                    else
                    {
                        if(FONT_TYPE=='S')
                        {                            
                            Display_Char(' ');
                        }
                    }

                    g_data_value=Float_Long(lbuf);
                    
                    if(g_data_value<0)
                        break;
                }
                if(g_n_pos>0)
                {
                    while(g_dot_cnt<gdp)
                    {
                        lbuf[g_n_pos]=0x30;
                        g_dot_cnt++;
                        g_n_pos++;
                    }

                    if(g_n_pos==gnochars)
                        return gnochars;

                    else if(g_key_type=='N')
                    {
                        g_temp_count=1;
                        while(g_temp_count<=g_n_pos)
                        {
                            lbuf[gnochars-g_temp_count]=lbuf[g_n_pos-g_temp_count];
                            g_temp_count++;
                        }

                        g_temp_count=0;
                        while(g_temp_count<(gnochars-g_n_pos))
                        {
                            lbuf[g_temp_count]=0x30;
                            g_temp_count++;
                        }
                    }
                    return gnochars;
                }

                else if(g_n_pos == 0)
                {
                    lbuf[gnochars] = 0x00;
                    return gnochars;
                }
                break;

            case KB_BS:

                if(g_n_pos>0)
                {
                    if(FONT_TYPE=='S' || g_n_pos<gnochars)
                    {
                        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                        Display_Char_Big(gline,g_n_pos,0x20);
                    }

                    if(g_dot_flag==1)
                    {
                        if(g_dot_cnt>0)
                            g_dot_cnt--;

                        else if(g_dot_cnt==0)		//now if changed to else if
                            g_dot_flag=0;
                    }

                    g_n_pos--;

                    lbuf[g_n_pos] = ' ';

                    LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                    Display_Char_Big(gline,g_n_pos,0x20);
                }
                if(g_n_pos==0)
                    rotate_flag=0;
                break;
                
            case KB_TARE:
                
                Send_Tare_Command();
                break;

            default:

                if(rotate_flag==1)
                {
                    g_n_pos=0;
                }
                if(KBV==KB_DEL || (g_n_pos==0 && g_key_type=='N'))
                {
                    for(g_n_pos=0;g_n_pos<gnochars;g_n_pos++)
                    {
                        lbuf[g_n_pos]=' ';
                        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                        Display_Char_Big(gline,g_n_pos,0x20);
                    }
                    rotate_flag=0;
                    g_n_pos=0;
                    g_dot_flag=0;
                    g_dot_cnt=0;
                    if(KBV==KB_DEL)
                        break;
                
                }
                

                if(MODE_FLAG==1 && KBV==0xF1 && g_n_pos==0)
                    return 0;

                else if(g_n_pos < gnochars && (g_dot_cnt<gdp || gdp==0))
                {
                    if(g_key_type=='A')
                        lbuf[g_n_pos]=KEY_ALPHA(KBV);
                    else
                        lbuf[g_n_pos]= KEY_ASCII(gdp,KBV);	//		//Numeric Nos

                    if(lbuf[g_n_pos]!=0xFF)
                    {
                        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);

                        if(g_key_type=='N')
                        {
                            if(g_dot_flag==0 && (g_dot_limit>0 && g_n_pos==g_dot_limit))
                            {
                                if(lbuf[g_n_pos]!='.')
                                {
                                    lbuf[g_n_pos]=' ';
                                    break;
                                }
                            }
                            if(lbuf[g_n_pos]=='.')
                            {
                                if(g_dot_flag==0)
                                {
                                    g_dot_flag=1;
                                }
                                else
                                {
                                    lbuf[g_n_pos]=' ';
                                    break;
                                }
                            }
                            

                            if(g_dot_flag==1 && lbuf[g_n_pos]!='.' )
                                g_dot_cnt++;
                        }

                        if(DISPCHAR_TYPE!='P')
                            Display_Char_Big(gline,g_n_pos,lbuf[g_n_pos]);
                        else
                            Display_Char_Big(gline,g_n_pos,'*');

                        g_n_pos++;

                        if(g_n_pos==gnochars && g_key_type=='A')
                            g_n_pos=gnochars-1;

                        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                    }
                    else
                        lbuf[g_n_pos]=' ';

                    break;
                }
            break;
        }
    }
}

UINT8 Get_String(UINT8 snochars,UINT8 sline,BYTE *lbuf)
{
    UINT8 get_string_var;
    
    get_string_var=Get_Numeric(snochars,100,sline,lbuf);
    
    return get_string_var;
}

UINT8 Get_String_Big(UINT8 bnochars,UINT8 dp,UINT8 bline,BYTE *lbuf)
{
    UINT8 get_string_var;

    FONT_TYPE='B';
    get_string_var=Get_Numeric(bnochars,dp,bline,lbuf);
    FONT_TYPE='S';
    
    return get_string_var;
}

UINT8 Get_Numeric_Big(UINT8 nnochars,UINT8 dp,UINT8 nline,BYTE *lbuf)
{
    UINT8 get_status_var;

    FONT_TYPE='B';
    get_status_var=Get_Numeric(nnochars,dp,nline,lbuf);
    FONT_TYPE='S';

    return get_status_var;
}

UINT8 ASCII_BCD(BYTE *tbuf)			//Conversion from ASCII to BCD
{
    UINT8 a_temp_val;

    a_temp_val=((*tbuf<<4)&0xF0)| (*(tbuf+1))&0x0F;

    return a_temp_val;
}

UINT8 HEX_ASCII(UINT8 hexval)
{
    UINT8 h_ascii_value,h_arr[3];

    h_arr[1]=hexval%16;
    h_arr[0]=hexval/16;

    h_ascii_value=h_arr[0]*10+h_arr[1];

    return h_ascii_value;
}

void Disp_Checking_Data(UINT16 dcustcode)
{
    if(dcustcode==0)
    {
        Display_Boarder3();
        
        STRCPY(LBUF,"  Checking data...");
        Disp_Msg(LINE5);

        Please_Wait_Msg_Small(LINE7);
    }
    
    ZERO=1;
    
    if(dcustcode>0)
    {
        STRCPY(LBUF,"CUST CODE:1111");
        Hex_DEC(0,dcustcode,LBUF+10,4);
        Shift_Left(LBUF+10,4);
        Disp_Msg(LINE3);
    }    
}

//version1.06
 void String_Upper(BYTE *tbuf)
{
    UINT8 str_temp_cnt=0;
    
    while(tbuf[str_temp_cnt]!=0x00)
    {
        tbuf[str_temp_cnt]=toupper(tbuf[str_temp_cnt]);
        str_temp_cnt++;
    }    
}
 
 UINT8 Get_String_New(UINT8 fonttype,UINT8 gnochars,UINT8 nullterminator,UINT8 gline,BYTE *lbuf)
{
    UINT8 g_n_pos=0,rotate_flag=0;
    
    FONT_TYPE=fonttype;
    LCD_Set_Cursor(FONT_TYPE,gline);
    DISP_CURSOR_FLAG=0;
    for(;;)
    {        
        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);

        KBV=0xFF;
        
        if(g_n_pos<=gnochars)
        {
            CURSOR_FLAG=1;
            CURSOR_BLINK=0x80;

            if(FONT_TYPE=='B')
                CURSOR_BLINK=0x04;
            
            TEMP_CURSOR_BLINK=CURSOR_BLINK;
                
            if(g_n_pos==gnochars)
            {
                rotate_flag=1;
            }
            else
            {
                if(DISPCHAR_TYPE!='P')
                    Display_Char_Big(gline,g_n_pos,lbuf[g_n_pos]);
                else
                    Display_Char_Big(gline,g_n_pos,' ');
                
                LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
            }
            CURSOR_BLINK=0x00;
        }
        
        do
        {
            if(g_n_pos<=gnochars && DISP_CURSOR_FLAG==1)
            {
                DISP_CURSOR_FLAG=0;
                
                if(g_n_pos==gnochars)
                {
                    //LCD_Set_Cursor(FONT_TYPE,gline+0);
                    if(DISPCHAR_TYPE!='P')
                        Disp_Cursor_Blink(FONT_TYPE,gline,0,lbuf[0]);//Display_Char_Big(gline,0,lbuf[0]);
                    else
                        Disp_Cursor_Blink(FONT_TYPE,gline,0,'*');//Display_Char_Big(gline,0,'*');
                }
                else
                {
                    
                    Disp_Cursor_Blink(FONT_TYPE,gline,g_n_pos,lbuf[g_n_pos]);
                }
            }
            
            KBV=Scan_Key('A');						//Key sensing
            
            if(KBV!=0xFF)
            {
                CURSOR_FLAG=0;
                CURSOR_BLINK=0x00;
                DISP_CURSOR_FLAG=0;
            }
            
           
#ifdef NAME_PH_SEARCH1
            
            if(Change_Shift_Search()==1)
                return 0xF0;
            
#endif
         
           
        }while(KBV==0xFF);
       
        switch(KBV)
        {
            case KB_CANCEL:

                return 0xFF;
                
            case KB_UP:

                break;
                
            case KB_DOWN:

                break;

            case KB_LEFT:

                LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                 Display_Char_Big(gline,g_n_pos,lbuf[g_n_pos]);
                 
                 if(g_n_pos==0)
                     g_n_pos=gnochars;

                  g_n_pos=g_n_pos-1;

                 
                 LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                 break;

            case KB_RIGHT:
             
                LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                Display_Char_Big(gline,g_n_pos,lbuf[g_n_pos]);

                 if(g_n_pos==gnochars-1)
                     g_n_pos=0;
                 else
                     g_n_pos=g_n_pos+1;

                 LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);

               
               break;

            case KB_ENT:
                
                lbuf[gnochars] = 0x00;
               
                if(g_n_pos>0)
                {
                    if(nullterminator=='Y')
                        return g_n_pos;
                    
                    if(g_n_pos==gnochars)
                        return gnochars;

                    
                    
                }
                
                
                return gnochars;

               
                break;

            case KB_BS:

                if(g_n_pos>0)
                {
                    if(FONT_TYPE=='S' || g_n_pos<gnochars)
                    {
                        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                        Display_Char_Big(gline,g_n_pos,0x20);
                    }

                    g_n_pos--;

                    lbuf[g_n_pos] = ' ';

                    LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                    Display_Char_Big(gline,g_n_pos,0x20);
                }
                if(g_n_pos==0)
                    rotate_flag=0;
                break;
                
//            case KB_TARE:
//                
//                Send_Tare_Command();
//                break;

            default:

                if(rotate_flag==1)
                {
                    g_n_pos=0;
                }
                if(KBV==KB_DEL || (g_n_pos==0 && nullterminator=='Y'))
                {
                    for(g_n_pos=0;g_n_pos<gnochars;g_n_pos++)
                    {
                        lbuf[g_n_pos]=' ';
                        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                        Display_Char_Big(gline,g_n_pos,0x20);
                    }
                    rotate_flag=0;
                    g_n_pos=0;
                   
                    if(KBV==KB_DEL)
                        break;
                
                }
                

                if(MODE_FLAG==1 && KBV==0xF1 && g_n_pos==0)
                    return 0;

                else if(g_n_pos < gnochars)
                {
                    lbuf[g_n_pos]=KEY_ALPHA(KBV);
                  
                    if(lbuf[g_n_pos]!=0xFF)
                    {
                        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);

                        if(DISPCHAR_TYPE!='P')
                            Display_Char_Big(gline,g_n_pos,lbuf[g_n_pos]);
                        else
                            Display_Char_Big(gline,g_n_pos,'*');

                        g_n_pos++;

                        if(g_n_pos==gnochars)
                            g_n_pos=gnochars-1;

                        LCD_Set_Cursor(FONT_TYPE,gline+g_n_pos);
                    }
                    else
                        lbuf[g_n_pos]=' ';

                    break;
                }
            break;
        }
    }
}

 
#ifdef NAME_PH_SEARCH1

UINT8 Compare_Code(UINT8 sfield,UINT32 crecord,UINT8 clength)
{
    UINT8 cmp_status,field_buf[20];
    
    EEPROM_ADDR=Get_Member_Address(crecord);

    if(sfield=='P')
        FLASHReadString(EEPROM_ADDR+Member_Contact_No_Off,clength,(char *)&field_buf);
    else
        FLASHReadString(EEPROM_ADDR+Member_Name_Off,clength,(char *)&field_buf);

    String_Upper(SEARCH_BUF);
    String_Upper(field_buf);

    cmp_status=strncmp(SEARCH_BUF,field_buf,clength);

    if(clength!=0 && cmp_status==0)
    {
        Read_Member_Name(crecord);
        Read_Member_CODE(crecord);
        RECORD_NO=crecord;
        Hex_DEC(0,Memberrec.C_CODE,LBUF,4);
        LBUF[4]=':';
        strncpy(LBUF+5,Memberrec.NAME,15);
        CURSOR_BLINK=0x80;
        
        Invert_Disp_Msg(LINE4);
            
        CURSOR_BLINK=0x00;
        return 1;
    }
    
    return 0;			
}

UINT8 Search_User(UINT8 fieldval,UINT8 slength,UINT32 currentsearchrecord,UINT32 endsearchrecord)
{
    UINT8 c_status;
    
    if(currentsearchrecord<=endsearchrecord)
    {   
        for(;currentsearchrecord<=endsearchrecord;currentsearchrecord++)
        {
            c_status=Compare_Code(fieldval,currentsearchrecord,slength);
            if(c_status==1)
                return 'M';
        }
    }
    else
    {
        for(;currentsearchrecord>=endsearchrecord;currentsearchrecord--)
        {
            c_status=Compare_Code(fieldval,currentsearchrecord,slength);
            if(c_status==1)
                return 'M';
            
            
        }
    }

	return 'N';
}


UINT8 Change_Shift_Search(void)
{
    if(PS2_KEYBOARD==0 && KBV==0xA4)
        return 1;
    else
    {
        if(SHIFT_SEARCH==1)
        {
            SHIFT_SEARCH=0;
            return 1;
        }
    }
    
    return 0;
}


 #endif



