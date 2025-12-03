void Invert_Line(UINT8 invertcnt1)
{
    Invert1=0;
    Invert2=0;
    Invert3=0;
    if(invertcnt1>=2)
        Invert3=(invertcnt1+1)/3;
    else
    {
        Invert1=(invertcnt1+1)/1;
        Invert2=(invertcnt1+1)/2;
    }  
}

UINT8 Up_Down(UINT8 udcount,UINT8 maxcount)
{
    switch(KBV)
    {
        case KB_DOWN:

            udcount=udcount+1;

            if(udcount>=maxcount)
                udcount=0;

            return udcount;

        case KB_UP:

            if(udcount<=0)
                udcount=maxcount;

            udcount=udcount-1;

            return udcount;

        default:

            return udcount;
    }    
}

UINT8 Function_Selected(UINT8 maxfuns,UINT8 presentcount)
{
    KBV=0xFF;
    
    KBV=Wait_Key_Press('N');

    if(KBV==KB_CANCEL)
        return 0xFF;

    else if(KBV>=0x31 && (KBV<=(0x30+maxfuns) && KBV<=0x39))
    {
        presentcount=KBV-0x31;
        KBV=KB_ENT;
    }
    else
        presentcount=Up_Down(presentcount,maxfuns);
    
    return presentcount;
}

UINT8 Select_Yes_No(UINT8 scnt,UINT8 stype)
{
    Display_Box_Top_Boarder(LINE4,0x06);
    Display_Box_Boarder(LINE7);
    
    while(1)
    {
        Invert_Line(scnt);

        if(stype==0)
            STRCPY(LBUF,"OFFLINE");
        if(stype==1)      
            STRCPY(LBUF,"NO ");
        if(stype==2)
            STRCPY(LBUF,"OFF");
        if(stype==3)
            STRCPY(LBUF,"INCREASE");
        if(stype==4)
            STRCPY(LBUF,"40 Column");
        if(stype==5)
            STRCPY(LBUF,"100 ML");
        if(stype==6)
            STRCPY(LBUF,"NORMAL TABLES");
        if(stype==7)
            STRCPY(LBUF,"CODE WISE");        
        if(stype==8)
            STRCPY(LBUF,"SHIFT WISE");
        if(stype==9)
            STRCPY(LBUF,"PER LITRE");
        
        DISPLAY_NO_SPACES=1;
        Disp_Msg_Big(Invert1,LINE5+1);

        if(stype==0)
            STRCPY(LBUF,"ONLINE");
        if(stype==1)
            STRCPY(LBUF,"YES");
        if(stype==2)
            STRCPY(LBUF,"ON ");
        if(stype==3)
            STRCPY(LBUF,"DECREASE");
        if(stype==4)
            STRCPY(LBUF,"80 Column");
        if(stype==5)
            STRCPY(LBUF," 10 ML");
        if(stype==6)
            STRCPY(LBUF,"EFFECT TABLES");
        if(stype==7)
            STRCPY(LBUF,"TIME WISE");
        if(stype==8)
            STRCPY(LBUF,"DATE WISE");
        if(stype==9)
            STRCPY(LBUF,"PERCENTAGE(%)");

        DISPLAY_LAST_LINE=0x30;
        NOR_DISPLAY_LAST_LINE=0xC0;
        Disp_Msg_Big(Invert2,LINE7+1);
        Reset_Display_Values();

        KBV=Wait_Key_Press('C');

        switch(KBV)
        {
            case KB_CANCEL:

                return 0xFF;

            case KB_ENT:

                return scnt;
        }
       
        scnt=Up_Down(scnt,2);
    }
}

void Yes_No(void)
{
    UINT8 y_cnt,y_val,stype=1;

    TIME_FLAG=0;
    y_cnt=1;
    
    SEERAMReadString(EEPROM_ADDR,1,(char *)&y_val);
    
    if(y_val==0xFF)
        y_cnt=0;

    if(y_val=='N')
        y_cnt=0;

    if(y_val=='Y')
        y_cnt=1;
    
    STRNCPY(PBUF,LBUF,20);
    Display_Boarder3();
    
    STRNCPY(LBUF,PBUF,20);
    Disp_Msg(LINE3);
    
    if(EEPROM_ADDR==On_Offline_Loc)
        stype=0;
    y_val=Select_Yes_No(y_cnt,stype);
    
    if(y_val!=0xFF)
    {
        if(y_val==0)
            y_val='N';
        
        if(y_val==1)
            y_val='Y';
        
        SEERAMWriteString(EEPROM_ADDR,1,(char *)&y_val);        
    }
    TIME_FLAG=1;   
}

UINT8 Confirm_Yes(void)
{
    UINT8 confirm_count=1;

    TIME_FLAG=0;
    
    if(strncmp(LBUF,"Delete All Members?",19)==0 || strncmp(LBUF,"Send Message Also?",18)==0)
        confirm_count=0;
    
    Disp_Msg(LINE3);    
        
    confirm_count=Select_Yes_No(confirm_count,1);
    
    CLEAR_DATA_LINES_FROM(3,7);
    
    if(confirm_count==0xFF)
        Display_Boarder3();
    
    return confirm_count;
}

void Long_Beep(void)
{
    KBV=0xFF;
    Exit_Any_Key(0);
    do
    {
        Buzzer(250);
        KBV=Scan_Key('N');

    }while(KBV==0xFF);
}

