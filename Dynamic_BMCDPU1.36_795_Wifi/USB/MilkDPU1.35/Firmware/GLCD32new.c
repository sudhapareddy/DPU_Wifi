#include "GLCD32n.h"
#include "GLCD32Font.c"

# define    NOP  asm("nop");

void Delay_Nops(UINT8 nops)
{
    UINT8 l_d_cnt;
    

    for(l_d_cnt=0;l_d_cnt<nops;l_d_cnt++);
 }    

void CS1_On_CS2_Off(void)
{
    LCDCS1 = 1;
    DelayMs(1);

    LCDCS2 = 0;
    DelayMs(1);
}

void CS1_Off_CS2_On(void)
{
    LCDCS1 = 0;
    DelayMs(1);

    LCDCS2 = 1;
    DelayMs(1);
}
void LCDreset(void)
{
    LCDRST = 0;
    DelayMs(1);
    LCDRST = 1;
    DelayMs(1);
    LCDRS = 0;
    
    CS1_On_CS2_Off();
   
    LCDwriteREG(yADDR);
    LCDwriteREG(xADDR);
    LCDwriteREG(lineNO);
    
    CS1_Off_CS2_On();

    LCDwriteREG(yADDR);
    LCDwriteREG(xADDR);
    LCDwriteREG(lineNO);

    LCDCS2 = 0;
    DelayMs(1);

    LCDdispON();
}

void LCD_Enable_On_Off(void)
{    
    LCDEN = 1;
    
    Delay_Nops(10);

    LCDEN = 0;

    Delay_Nops(10);
}

void LCDwriteREG(UINT8 regdata)
{
    LCDRS = 0;
    LCDPORT = LCDPORT & 0x00;
    LCDPORT = LCDPORT + regdata;
    
    LCD_Enable_On_Off();
}

void LCDwriteRAM(UINT8 regdata)
{
    if(INVERT_DISPLAY==1)
        regdata=~regdata;

    LCDRS = 1;
    LCDPORT = LCDPORT & 0x00;
    LCDPORT = LCDPORT + regdata;

    LCD_Enable_On_Off();   
}
 
void LCDdispON(void)
{
    CS1_On_CS2_Off();

    LCDwriteREG(dispON);
    
    CS1_Off_CS2_On();

    LCDwriteREG(dispON);

    LCDCS2 = 0;
    DelayMs(1);
}

void Write_LCD_Data(UINT8 ffline,UINT8 eeline)
{
    UINT8 l_cnt,p_cnt;
    
    Dev_Addr = xADDR+(ffline-1);

    for (l_cnt=ffline;l_cnt<=eeline;l_cnt++)
    {
        LCDwriteREG(Dev_Addr);
        LCDwriteREG(yADDR);

        for (p_cnt=0;p_cnt<64;p_cnt++)
            LCDwriteRAM(0x00);

        Dev_Addr++;
    }
}

void LCDfillPATT(UINT8 fline,UINT8 eline)
{
    UINT8 l_invert_status;

    l_invert_status=INVERT_DISPLAY;

    INVERT_DISPLAY=0;

    CS1_On_CS2_Off();
    
    Write_LCD_Data(fline,eline);
    
    CS1_Off_CS2_On();
    Write_LCD_Data(fline,eline);

    LCDCS2 = 0;
    DelayMs(1);
    INVERT_DISPLAY=l_invert_status;
}

void LCD_PAGE(UINT8 ftype,UINT8 plinepos)
{
    UINT8 pos_lineno;
    
    pos_lineno=(plinepos/20);
    grphlcdX=0xB8+pos_lineno;
    grphlcdY=plinepos-(pos_lineno*20);

    if(ftype=='S')
        grphlcdY=4+(grphlcdY*6);
    else if(ftype=='B')
        grphlcdY=grphlcdY*8;
    else if(ftype=='P')
        grphlcdY=grphlcdY*8+2;
    else
        grphlcdY=8+(grphlcdY*6);
}

void LSet_Cursor(UINT8 ypos)
{
    UINT8 l_d_cnt;

    grphlcdY=ypos;

    if (grphlcdY > 0x7F)	//checking Y address boundary
        grphlcdY = 0x7F;

    if (grphlcdX > 0xBF)		//checking X address boundary
        grphlcdX = 0xBF;

    if (grphlcdY < 0x40)
    {
        LCDCS1 = 1;
        Delay_Nops(10);

        LCDCS2 = 0;

        Delay_Nops(10);
       // CS1_On_CS2_Off();

        Dev_Addr = yADDR + grphlcdY;
    }
    else if (grphlcdY >= 0x40)
    {
        LCDCS1 = 0;
        Delay_Nops(10);

        LCDCS2 = 1;
        Delay_Nops(10);

        Dev_Addr = yADDR + grphlcdY - 0x40;
    }

    LCDwriteREG(Dev_Addr);
    Dev_Addr =  grphlcdX;
    LCDwriteREG(Dev_Addr);
}

void STRCPY(BYTE *Dest, BYTE  *Source)
{
    while (*Source)
        *Dest++ = *Source++;
    *Dest = '\0';
}

void STRNCPY(BYTE *Dest, BYTE  *Source,UINT16 snob)
{
    while (snob--)
        *Dest++ = *Source++;
}

void Display_Char(UINT8 dispchr)
{
    UINT8  d_half_2,d_y_index,d_font_ptr,d_temp;

    if(grphlcdY >= 0x40)
        d_half_2 = 1;
    else
        d_half_2 = 0;

    LSet_Cursor(grphlcdY);

     for(d_y_index = 0; d_y_index < 6; d_y_index++)
    {
        d_font_ptr=dispchr-0x20;
        d_temp=Font7x8[d_font_ptr][d_y_index] | CURSOR_BLINK;
        LCDwriteRAM(d_temp);

        grphlcdY++;

        if(grphlcdY >= 0x40 && d_half_2 == 0)//1
        {
            LSet_Cursor(grphlcdY);
            d_half_2 = 1;
        }
    }

    if(grphlcdY >= 0x40 && d_half_2 == 0)
    {
        LSet_Cursor(grphlcdY);
        d_half_2 = 1;
    }
}

void Display_Char_Big(UINT8 blinepos,UINT8 bpos,UINT8 bdispchar)
{
    UINT8 b_char_wdt,b_char_val;
    UINT16 b_y_index;
    UINT8  b_half_2,b_font_ptr;
    
    if(FONT_TYPE=='S')
    {
        Display_Char(bdispchar);
        return;
    }

    LCD_PAGE(FONT_TYPE,blinepos+bpos);

    if(grphlcdY >= 0x40)
        b_half_2 = 1;
    else
        b_half_2 = 0;

    b_char_wdt = 0x08;

    LSet_Cursor(grphlcdY);

    for (b_y_index = 1;b_y_index < (2*b_char_wdt);b_y_index++)
    {
        b_font_ptr=bdispchar-0x20;
        
        b_char_val=Terminal8x12[b_font_ptr][b_y_index];
        
        
        b_char_val=b_char_val | DISPLAY_FIRST_LINE;
        
        LCDwriteRAM(b_char_val);

        grphlcdY++;

        if(grphlcdY >= 0x40 && b_half_2 == 0)
        {
            LSet_Cursor(grphlcdY);
            b_half_2 = 1;
        }
        b_y_index++;
    }

    LCD_PAGE(FONT_TYPE,blinepos+20+bpos);
    LSet_Cursor(grphlcdY);
    
    if(grphlcdY >= 0x40)
        b_half_2 = 1;
    else
        b_half_2 = 0;
    
    for(b_y_index = 2;b_y_index<=(2*b_char_wdt);b_y_index++)
    {       
        b_font_ptr=bdispchar-0x20;
        b_char_val=Terminal8x12[b_font_ptr][b_y_index] | CURSOR_BLINK;
              
        if(FIRST2<2)
        {
            if(LAST2==0)
            {
                if(INVERT_DISPLAY==1)
                    LCDwriteRAM(b_char_val | 0x00);
                else
                    LCDwriteRAM(b_char_val | 0xFF);
                FIRST2++;
            }
        }
        else if((LAST2>=6  && LAST2<8)&& b_y_index>=14)
        {
            if(INVERT_DISPLAY==1)
                LCDwriteRAM(b_char_val | 0x00);
            else
                LCDwriteRAM(b_char_val | 0xFF);
            LAST2++;
        }        
        else
        {
            if(INVERT_DISPLAY==1)
                LCDwriteRAM(b_char_val | DISPLAY_LAST_LINE);
            else
                LCDwriteRAM(b_char_val | NOR_DISPLAY_LAST_LINE);
        }
        
        grphlcdY++;

        if(grphlcdY >= 0x40 && b_half_2 == 0)
        {
            LSet_Cursor(grphlcdY);
            b_half_2 = 1;
        }
        b_y_index++;
    }
}

void Set_Cursor(UINT8 clinepos)
{
    LCD_PAGE('S',clinepos);
    LSet_Cursor(grphlcdY);
}

void Disp_Underline(void)
{
       
      if(BOARDER==1)
        BOARDER=0;
   
    DISPLAY_LAST_LINE=0xB0;
    Disp_Msg_Big(1,LINE1);
    DISPLAY_LAST_LINE=0xF0;
    
}

void First_Last_Boarder(UINT8 bcnt)
{
    if(BOARDER==1)
    {
        if(bcnt==0)
        {
            FIRST2=0;
            LAST2=0;
        }
      
        if(bcnt==15)
        {
            FIRST2=2;
            LAST2=6;
        }
    }
}

void Display_Bar(UINT8 barline)
{
    Set_Cursor(barline);
    Display_Char(0x90);
}
/*
void Display_Bar_Big(UINT8 barline)
{
    //Set_Cursor(barline);
    Display_Char_Big(barline,0x87);
}*/

void Display_Bar_Data(UINT8 barline,UINT8 dispdata)
{
    Set_Cursor(barline);
    Display_Char(dispdata);
}

void Display_Bar_LF(UINT8 barline)
{
    Set_Cursor(barline);
    Display_Char(0x95);
}

void Display_Bar_Top_LF(UINT8 barline)
{
    Set_Cursor(barline);
    Display_Char(0x97);
}

void Display_Bar_Middle_Up(UINT8 barline)
{
    Set_Cursor(barline);
    Display_Char(0x99);
}

void Display_Bar_Middle_Down(UINT8 barline)
{
    Set_Cursor(barline);
    Display_Char(0x9A);
}

void Display_Bar_Middle_Plus(UINT8 barline)
{
    Set_Cursor(barline);
    Display_Char(0x98);
}

void Disp_Msg_Big(UINT8 dinvertstatus,UINT8 dlinepos)
{
    UINT8 b_cnt=0;

    FONT_TYPE='B';
    
    INVERT_DISPLAY=dinvertstatus;
  
    while((b_cnt<16) && (LBUF[b_cnt]!='\0'))
    {       
        First_Last_Boarder(b_cnt);
        Display_Char_Big(dlinepos,b_cnt,LBUF[b_cnt]);
        b_cnt++;
    }
  
    if(DISPLAY_NO_SPACES==1);
    else
    {
        while(b_cnt<16)
        {
            First_Last_Boarder(b_cnt);
            LBUF[b_cnt]=' ';

            Display_Char_Big(dlinepos,b_cnt,LBUF[b_cnt]);
            b_cnt++;
        }
    }
    INVERT_DISPLAY=0;

    if(CLEAR_BUF_FLAG!=1)
        STRCPY(LBUF,"                    ");
    FONT_TYPE='S';
}

void Disp_Msg_No_Spaces(UINT8 sinvert,UINT8 sline)
{
    DISPLAY_NO_SPACES=1;
    Disp_Msg_Big(sinvert,sline);
    DISPLAY_NO_SPACES=0;
}

void Display_Data(UINT8 dlinepos)
{
    UINT8 d_b_cnt=0;

    LCD_PAGE('S',dlinepos);

    while ((d_b_cnt<20) && (LBUF[d_b_cnt]!='\0'))
    {
        Display_Char(LBUF[d_b_cnt]);
        d_b_cnt++;
    }
    if(INVERT_DISPLAY==1 && DISPLAY_NO_SPACES==1);
    else
    {
        while(d_b_cnt<20)
        {
            LBUF[d_b_cnt]=' ';
            Display_Char(LBUF[d_b_cnt]);
            d_b_cnt++;
        }
    }

    if(CLEAR_BUF_FLAG!=1)
        STRCPY(LBUF,"                    ");
}

void Disp_Msg(UINT8 dlinepos)
{
    INVERT_DISPLAY=0;
    Display_Data(dlinepos);
}

void Invert_Disp_Msg(UINT8 ilinepos)
{
    INVERT_DISPLAY=1;
    Display_Data(ilinepos);
    INVERT_DISPLAY=0;

}

void Set_Cursor_rtc(UINT8 rlinepos)
{
    LCD_PAGE('R',rlinepos);
    LSet_Cursor(grphlcdY);
}

void LCD_Set_Cursor(UINT8 cfonttype,UINT8 clinepos)
{
    LCD_PAGE(cfonttype,clinepos);
    LSet_Cursor(grphlcdY);
}

void Clear_LCD(void)
{
    INVERT_DISPLAY=0;    
    LCDfillPATT(1,8);				//clears display RAM
}

void Reset_Display_Values(void)
{
    NOR_DISPLAY_LAST_LINE=0x00;
    DISPLAY_FIRST_LINE=0x00;
    DISPLAY_LAST_LINE=0xF0;
    CURSOR_BLINK=0x00;
    DISPLAY_NO_SPACES=0;
    INVERT_DISPLAY=0;
}

void Disp_Cursor_Blink(UINT8 ftype,UINT8 dline,UINT8 dpos,UINT8 blinkdata)
{
    LCD_Set_Cursor(ftype,dline+dpos);
    Display_Char_Big(dline,dpos,blinkdata);
}

void Init_LCD(void)
{
    TRISD=0x00;

    LCDRS_IO = 0;
    LCDBL_IO = 0;
    LCDEN_IO = 0;
    LCDRST_IO = 0;
    LCDCS2_IO = 0;
    LCDCS1_IO = 0;
    LCDRW_IO = 0;
    LCDRW = 0;
    LCDBL=0;
    BUZZER_IO=0;
    PWRLED_IO=0;
    PWRLED=0;
    BUZZER=0;
    PWRLED=1;
    CLEAR_BUF_FLAG=0;
    FIRST2=2;
    LAST2=0;
    BOARDER=0;
    
    Reset_Display_Values();
    
    LCDreset();
    
    Clear_LCD();

    BUZZER=1;
    LED_Blink=1;
}



void CLEAR_DATA_LINES_FROM(UINT8 sline,UINT8 eline)
{
    STRCPY(LBUF,"                    ");
    for(;sline<=eline;sline++)
        Disp_Msg((sline-1)*20);
}

void Display_Box(UINT8 bline)
{
    STRCPY(LBUF,"               ");
    LBUF[0]=0x83;
    LBUF[15]=0x85;
    Disp_Msg_Big(0,bline);
}

void Display_Boxes(UINT8 bline,UINT8 nob)
{
    while(nob--)
    {
        Display_Box(bline);
        bline=bline+40;
    }
}

void Display_Bar_Big_T(UINT8 barline,UINT8 barpos,UINT8 bardata)
{    
    
    FONT_TYPE='B';
    Display_Char_Big(barline,barpos,bardata);
    FONT_TYPE='S';
}

void Display_Box_Top_Boarder(UINT8 bline,UINT8 lpos)
{
    DISPLAY_FIRST_LINE=lpos;
    Display_Box(bline);
    DISPLAY_FIRST_LINE=0x00;
}


 void Disp_Top_Boarder_Big_T(UINT8 barline,UINT8 barpos,UINT8 bardata)
{    
    Display_Box_Top_Boarder(barline,0x18);
    Display_Bar_Big_T(barline,barpos,bardata);
}
    
void Display_Box_Boarder(UINT8 bline)
{
    NOR_DISPLAY_LAST_LINE=0xC0;
    Display_Box(bline);
    NOR_DISPLAY_LAST_LINE=0x00;
}

void Display_Selected_Boarder(UINT8 boxline,UINT8 linepos)
{
    NOR_DISPLAY_LAST_LINE=linepos;
    Display_Box(boxline);
    NOR_DISPLAY_LAST_LINE=0x00;
}


void Display_Box_Full_Boarder(UINT8 bline)
{
    DISPLAY_FIRST_LINE=0x01;
    Display_Box_Boarder(bline);
    DISPLAY_FIRST_LINE=0x00;    
}

void Display_Full_Boarder(void)
{    
    DISPLAY_FIRST_LINE=0x03;
    Display_Box(LINE1);
    DISPLAY_FIRST_LINE=0x00;
    
    Display_Boxes(LINE3,2);
    
    Display_Box_Boarder(LINE7);    
}

void Display_Boarder3(void)
{
    Reset_Display_Values();
    Display_Box(LINE3);
    Display_Box(LINE5);
    Display_Box_Boarder(LINE7);   
}

void Display_Half_Box(void)
{  
    Display_Box(LINE5);
    Display_Box_Boarder(LINE7);   
}

void Display_Boarder4(void)
{
    Display_Box_Top_Boarder(LINE2,0x02);
    Display_Boarder3();
}

void Display_Boarder2(void)
{
    Display_Box(LINE3);
    Display_Box(LINE5);
    Display_Box(LINE7);
    Display_Box_Boarder(LINE6);   
}

void Display_Box_Line4(void)
{
    NOR_DISPLAY_LAST_LINE=0x06;
    Display_Box(LINE1);
    NOR_DISPLAY_LAST_LINE=0x00;
    
    Display_Boarder3();
}

void Display_Table_2(UINT8 tableline,UINT8 tbarpos)
{   
    Display_Box_Top_Boarder(tableline,0x18);
    Display_Bar_Middle_Down(tableline+tbarpos);
    
    Display_Box_Top_Boarder(tableline+40,0x18);
    Display_Bar_Middle_Plus((tableline+40)+tbarpos);
       
    NOR_DISPLAY_LAST_LINE=0xC0;
    Display_Box_Top_Boarder(tableline+80,0x18);
    Display_Bar_Middle_Up((tableline+80)+tbarpos);
}