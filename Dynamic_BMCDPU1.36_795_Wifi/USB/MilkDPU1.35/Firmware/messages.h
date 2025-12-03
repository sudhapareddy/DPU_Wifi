void Set_Limits_Msg(void)
{
    Display_Full_Boarder();
    STRCPY(LBUF,"  Set Limits  ");
    Disp_Msg_No_Spaces(0,LINE4+1);
    Delay_Sec(1);
}

UINT8 Update_Confirm(void)
{
    UINT8 confirm_status;

    Display_Boarder3();

    STRCPY(LBUF,"Update Rate Table?");
    confirm_status=Confirm_Yes();
    
    Display_Boarder3();

    return confirm_status;
}

void Disp_Header_Boarder(void)
{
    DISPLAY_LAST_LINE=0xC0;//0x38
    Disp_Msg_Big(1,LINE1);
    DISPLAY_LAST_LINE=0xF0;
    BOARDER=0;
}

void Disp_Header_No_Boarder(void)
{
    DISPLAY_LAST_LINE=0xF0;
    Disp_Msg_Big(1,LINE1);
    BOARDER=0;
}

void Disp_Line_Boarder(UINT8 invert,UINT8 displine)
{
    BOARDER=1;
    Disp_Msg_Big(invert,displine);
    BOARDER=0;
}

void Display_Screen_Boarder(void)
{
    Display_Full_Boarder();
    BOARDER=1;
}

void Disp_Effect_Msg(void)
{
    if(EFFECT_RATE==1)
    {
        STRCPY(LBUF,"  EFFECTIVE CHARTS");
        Invert_Disp_Msg(LINE8);
    }
}

void Collection_Header_Msg(void)
{
    Display_Screen_Boarder();
    
    STRCPY(LBUF,"  *COLLECTION*");
    Disp_Header_No_Boarder();
}

void Please_Wait_Msg(UINT8 mline)
{
    Display_Boarder3();
    STRCPY(LBUF,"PLEASE WAIT...");    
    Disp_Msg_No_Spaces(0,mline+1);
}

void Please_Wait_Msg_Small(UINT8 mline)
{
    STRCPY(LBUF,"   Please Wait...");
    Disp_Msg(mline);
}

void Max_Samples_Msg(void)
{
    STRCPY(LBUF,"Max Samples Reached");
    Disp_Msg(LINE5);
    Exit_Any_Key(1);
}

void Member_InActive_Msg(void)
{
    STRCPY(LBUF,"MEMBER NOT ACTIVE");
    Disp_Msg(LINE5);
    Delay_Sec(1);
}



void Table_Updated_Msg(void)
{
    STRCPY(LBUF," TABLE UPDATED");  
    Disp_Msg_No_Spaces(0,LINE5+1);
    Delay_Sec(1);
}

void Samples_Pending_Msg(void)
{
    Display_Shift(LINE3);
    
    STRCPY(LBUF," Samples Pending...");
    Disp_Msg(LINE5);
    
    Exit_Any_Key(1);
}

void No_New_Chart_Msg(void)
{
    STRCPY(LBUF,"NO NEW RATE CHARTS..");
    Disp_Msg(LINE7);
}

void Saved_Message(void)
{
    STRCPY(LBUF," Saved Successfully");           
    Invert_Disp_Msg(LINE8);
    
    Delay_Sec(1);
}

void Disp_Msg_Boarder(void)
{ 
    BOARDER=1;
    Disp_Msg_Big(1,LINE3);
    BOARDER=0;
}

void Disp_Msg_Line_Boarder(UINT8 itype,UINT8 bline)
{
    BOARDER=1;
    Disp_Msg_Big(itype,bline);
    BOARDER=0;
}

void Exit_Any_Key(UINT8 etype)
{
    STRCPY(LBUF,"Exit - Press Any Key");
    Invert_Disp_Msg(LINE8);
    
    if(etype==1)
    {
        Buzzer(250);
        Buzzer(250);

        Wait_Key_Press('A');
    }
}

void Display_Milk_Type(UINT8 dmilktype,UINT8 dmline)
{
    STRCPY(LBUF,"Milk Type:  COW");
    Buf_Type(dmilktype,LBUF+12);
    
    Disp_Msg(dmline);
}

void Disp_Up_Down_Select(void)
{
    STRCPY(LBUF,"Select: |Up: |Down: ");
    LBUF[12]=0x80;LBUF[19]=0x81;
    //LBUF[8]=LBUF[13]=0x90;
    LBUF[7]=0x82;
    Invert_Disp_Msg(LINE8);
}
void Up_Down_Select(void)
{
    Display_Boarder2();
    
    Disp_Up_Down_Select();
}
void Save_Exit_Msg(void)
{
    STRCPY(LBUF,"ENTER:Save|Esc:Exit");
    //LBUF[10]=0x90;
    Invert_Disp_Msg(LINE8);
}

void Enter_Exit_Msg(void)
{
    STRCPY(LBUF,"ENTER:Next|Esc:Exit");
   // LBUF[10]=0x90;
    Invert_Disp_Msg(LINE8);
}
