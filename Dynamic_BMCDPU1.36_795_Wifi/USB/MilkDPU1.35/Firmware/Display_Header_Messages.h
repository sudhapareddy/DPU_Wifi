void GPRS_Menu_Header(UINT8 hflag)
{
    Display_Screen_Boarder();
    
    if(hflag=='D')
        STRCPY(LBUF,"  *DOMAIN NAME*");

    if(hflag=='d')
        STRCPY(LBUF,"*IP ADDR/DOMAIN*");

    if(hflag=='G')
        STRCPY(LBUF," *GPRS SETTINGS*");

    if(hflag=='I')
        STRCPY(LBUF,"  *IP ADDRESS*");

    if(hflag=='P')
        STRCPY(LBUF,"  *PORT NUMBER*");

    if(hflag=='S')
        STRCPY(LBUF,"*SMS DAY LIMIT*");    
    
    if(hflag=='W')
        STRCPY(LBUF,"*SMS DAY LIMIT*");

    Disp_Header_No_Boarder();
}



void Server_Menu_Header(UINT8 stype)
{
    Display_Screen_Boarder();
    
    STRCPY(LBUF,">SERVER SETTINGS");
    
    if(stype=='R')
        STRCPY(LBUF,"*RECEIVE CHARTS*");
    
    if(stype=='W')
        STRCPY(LBUF,"*W.SCALE-SERVER*");
    
    if(stype=='A')
        STRCPY(LBUF,">ANALYZER SERVER");
    
    Disp_Header_No_Boarder();
}

void Menu_Header(UINT8 headertype,UINT8 noboarder)
{
    if(headertype!='Z')
    {
        Display_Screen_Boarder();
    }
    else
    {
        CLEAR_DATA_LINES_FROM(1,5);
        if(WEIGHT_COUNT==0)
            Display_Shift(LINE1);  
        else
            Display_Counters();
        
        return;
    }

    if(headertype=='A')
        STRCPY(LBUF,"*ABSENT MEMBERS*");   
    
    if(headertype=='B')
       STRCPY(LBUF," *BONUS REPORT*");
    
    if(headertype=='C')
        STRCPY(LBUF,"  *MEMBER BILL*");
    
    if(headertype=='c')
        STRCPY(LBUF," *COL. RECEIPT*");
        
    if(headertype=='D')
        STRCPY(LBUF,"DEDUCTION REPORT");
    
    if(headertype=='d')
        STRCPY(LBUF,"*DETAILED SHIFT*");
    
    if(headertype=='E')
        STRCPY(LBUF,"  *EDIT SHIFT*");
    
    if(headertype=='G')
        STRCPY(LBUF,"   *DEDUCTION*");
    
    if(headertype=='L')
        STRCPY(LBUF,">LOCAL MILK SALE");
    
    if(headertype=='M')
        STRCPY(LBUF,"*DAYWISE REPORT*");
    
    if(headertype=='S')
        STRCPY(LBUF,"SANGHSALE REPORT");
    
    if(headertype==1)
        STRCPY(LBUF,"  *SANGH SALE*");
    
    if(headertype==2)
        STRCPY(LBUF,"*EDIT SANGHSALE*");
    
    if(headertype=='s')
        STRCPY(LBUF," *SHIFT TIMINGS*");
    
    if(headertype=='T')
        STRCPY(LBUF,"  *SHIFT DATA*");
        
    if(headertype=='W')
        STRCPY(LBUF,"  *MEMBERWISE*");
    
    if(headertype=='w')
        STRCPY(LBUF,"PAYMENT REGISTER");
    
    if(headertype=='x')
    {
        STRCPY(LBUF,"*Enter Password*");

        if(LOGIN_TYPE=='D')
            STRCPY(LBUF,"ENTER DEALER PWD");
        
        if(LOGIN_TYPE=='M')
            STRCPY(LBUF,"ENTER MASTER PWD");

        if(LOGIN_TYPE=='A')
            STRCPY(LBUF,">ENTER ADMIN PWD");      
    }
    if(noboarder==0)
        Disp_Header_No_Boarder();
    else
        Disp_Header_Boarder();//
        
}

void Import_Export_Header(UINT8 headertyp)
{
    Display_Screen_Boarder();

    if(headertyp=='B' || headertyp=='C')    
    {
        STRCPY(LBUF,">*COW FAT TABLE*");
        Buf_Type(headertyp,LBUF+2);
    }
     
    if(headertyp=='b' || headertyp=='c')
    {
        STRCPY(LBUF,">*COW SNF TABLE*");
        Buf_Type(headertyp,LBUF+2);
        CLR_Enabled(LBUF+6);
    }
     
    if(headertyp=='I')
        STRCPY(LBUF,"INACTIVE MEMBERS");
     
   if(headertyp=='M')
        STRCPY(LBUF,">*MEMBERS LIST*");
     
   if(headertyp=='T')
        STRCPY(LBUF,"*SHIFTWISE DATA*");
    
    Disp_Header_No_Boarder();
}

void Admin_Header(UINT8 aheadertype)
{
    if(aheadertype=='s' || aheadertype=='C' || aheadertype=='c')
    {
        Display_Box_Line4();
    }
    else
        Display_Screen_Boarder();
    
    if(aheadertype=='C')
        STRCPY(LBUF,"  *CHANGE PASSWORD*");
    
    if(aheadertype=='c')
        STRCPY(LBUF,"  *CONTACT NUMBERS*");
    
    if(aheadertype=='P')
        STRCPY(LBUF," *PHONE NUMBERS*");
    
   if(aheadertype=='D')
        STRCPY(LBUF,"  *DAIRY NAME*");
    
   if(aheadertype=='E')
        STRCPY(LBUF,"*EFFECTIVE DATE*");
    
    if(aheadertype=='H')
        STRCPY(LBUF,">SOCIETY DETAILS");
    
    if(aheadertype=='S')
    {
        STRCPY(LBUF,"*SET DATE&TIME*");
        Disp_Header_Boarder();
        return;
    }
    
    if(aheadertype=='s')
        STRCPY(LBUF," *SOCIETY DETAILS*");
         
    if(aheadertype=='c' || aheadertype=='s' || aheadertype=='C')
        Disp_Msg(LINE1);
    else
        Disp_Header_No_Boarder();
}

void Master_Menu_Header(UINT8 sheadertype)
{
    Display_Screen_Boarder();
    
    if(sheadertype=='B')
        STRCPY(LBUF," *DPU BATCH NO*");
    
    if(sheadertype=='C')
        STRCPY(LBUF,"DEFAULT SETTINGS");
    
    if(sheadertype=='c')
        STRCPY(LBUF,"*RESET SETTINGS*");    
        
    if(sheadertype=='G')
        STRCPY(LBUF,"   *GPRS MODE*");
    
    if(sheadertype=='M')
        STRCPY(LBUF,"  *MFG. DATE*");
    
    if(sheadertype=='m')
        STRCPY(LBUF," *MACHINE CODE*");
    
     if(sheadertype==0)
        STRCPY(LBUF,"  *BRAND NAME*");
    
    if(sheadertype==1)
        STRCPY(LBUF," *DEALER NAME*");
   
    Disp_Header_No_Boarder();
}

void Message_Header(UINT8 mheadertype,UINT8 gdispboarder)
{
    if(mheadertype=='M')
    {
        Display_Box_Line4();
        STRCPY(LBUF,"      *MESSAGE*");
        Disp_Msg(LINE1);
        return;
    }
    
    Display_Screen_Boarder();
    
    if(mheadertype=='C')
    {
        STRCPY(LBUF,"*COLLECTION SMS*");
        
    }
    
    if(mheadertype=='G')
    {
        STRCPY(LBUF,"   *GROUP SMS*");
        
    } 
    
    if(gdispboarder==1)
        Disp_Header_Boarder();
    else
        Disp_Header_No_Boarder();
    
}

void Back_Select(void)
{
    DISPLAY_FIRST_LINE=0x18;
    Display_Box(LINE6);
    DISPLAY_FIRST_LINE=0x00;
    
    STRCPY(LBUF,"Back:Esc    Select: ");
    LBUF[19]=0x82;
    Disp_Msg(LINE7);
}

void Data_Transfer_Header(UINT8 dheadertype)
{
    Display_Box_Line4();
    
    if(dheadertype=='E')
        STRCPY(LBUF,"    *EXPORT DATA*");

    if(dheadertype=='e')
        STRCPY(LBUF,"EFFECTIVE RATE CHART");

    if(dheadertype=='G')
        STRCPY(LBUF,"   *GPRS TRANSFER*");

    if(dheadertype=='I')
        STRCPY(LBUF,"    *IMPORT DATA*");

    if(dheadertype=='P')
        STRCPY(LBUF," *PENDRIVE TRANSFER*");

    Disp_Msg(LINE1);
    Back_SubMenu();
}

void Printer_Header(UINT8 pheadertype)
{
    Display_Screen_Boarder();

    if(pheadertype=='E')
        STRCPY(LBUF," *EXT. PRINTER*");
  
    if(pheadertype=='S')
        STRCPY(LBUF,"*PRINTER STATUS*");
  
    Disp_Header_No_Boarder();
}

void Back_SubMenu(void)
{
    DISPLAY_FIRST_LINE=0x18;
    Display_Box(LINE6);
    DISPLAY_FIRST_LINE=0x00;
    
    STRCPY(LBUF,"Back:Esc   SubMenu: ");
    LBUF[19]=0x82;
    Disp_Msg(LINE7);
}

void Rate_Tables_Header(UINT8 rtheadertype)
{
    Display_Box_Line4();
  
    if(rtheadertype=='R')
    {
        STRCPY(LBUF,"    *RATE TABLES*");
        
        if(EFFECT_RATE==1)
            STRCPY(LBUF," *EFFECTIVE TABLES*");
    }
    
    if(rtheadertype=='F')
        STRCPY(LBUF,"  *FAT RATE TABLE*");
    
    if(rtheadertype=='S')
    {
        STRCPY(LBUF,"  *SNF RATE TABLE*");
        CLR_Enabled(LBUF+3);
    }
       
    Disp_Msg(LINE1);

    Back_SubMenu();
}
