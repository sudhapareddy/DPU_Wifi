BYTE MENU_SELECT_BUF[500],MENU_SELECT_DESC_BUF[500],SELECT_FUN_CODE[50];
UINT8 MAIN_MENU_FUN_CNT,SETTINGS_MENU_FUN_CNT,PRINT_MENU_FUN_CNT,VIEW_MENU_FUN_CNT,ITEM_MENU_FUN_CNT;
UINT8 USER_MENU_FUN_CNT,DATA_ENTRY_MENU_FUN_CNT,ALL_PERMISSIONS;
UINT8 DATA_TRANSFER_FUN_CNT,MEMORY_MENU_FUN_CNT;
BYTE USER_MENU_SELECTION[20];

BYTE ADMIN_MAIN_MENU_ACCESS[10]={
    1,1,1,1,1,1,1,1,0,0,
};

BYTE DATA_ENTRY_MENU_ACCESS[5]={
    1,1,1,1,0,
};

BYTE DATA_TRANSFER_MENU_ACCESS[2]={
    1,0,
};

BYTE EXT_PRINTER_MENU_ACCESS[14]={
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

BYTE INT_PRINTER_MENU_ACCESS[14]={
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

BYTE MEMORY_MENU_ACCESS[4]={
    1,1,1,1,
};

BYTE VIEW_REPORTS_MENU_ACCESS[4]={
    1,1,1,1,
};

BYTE ADMIN_SETTINGS_MENU_ACCESS[10]={
    1,1,1,1,1,1,1,0,0,0,
};

BYTE RATE_TABLES_MENU_ACCESS[2]={
    1,1,
};

BYTE MESSAGE_MENU_ACCESS[4]={
    1,1,1,1,
};

BYTE USER_MAIN_MENU_ACCESS[10]={
    1,1,1,1,0,1,1,0,0,0,
};

BYTE U_DATA_ENTRY_MENU_ACCESS[5]={
    1,1,1,1,0,
};

BYTE U_DATA_TRANSFER_MENU_ACCESS[2]={
    1,0,
};

BYTE U_EXT_PRINTER_MENU_ACCESS[14]={
    1,1,1,1,1,1,1,1,1,0,0,0,0,0,
};

BYTE U_INT_PRINTER_MENU_ACCESS[14]={
    1,1,1,1,1,1,1,1,1,0,0,0,0,0,
};

BYTE U_MEMORY_MENU_ACCESS[4]={
    0,0,0,0,
};

BYTE U_VIEW_REPORTS_MENU_ACCESS[4]={
    1,1,1,0,
};

BYTE U_ADMIN_SETTINGS_MENU_ACCESS[10]={
    1,0,0,0,0,0,0,0,0,0,
};

BYTE U_RATE_TABLES_MENU_ACCESS[2]={
    0,0,
};

BYTE U_MESSAGE_MENU_ACCESS[4]={
    1,1,1,1,
};




# define	Main_Menu_Selection_Loc         2900
# define    Main_Menu_Record_Length         10

# define	DATA_Entry_Loc                  2910
# define    Data_Menu_Record_Length         5  

# define	DATA_Transfer_Loc               2916
# define    Transfer_Menu_Record_Length     2  

# define	Print_Fun_Selection_Loc         2918
# define    User_Print_Fun_Record_Length    14

# define	Ext_Print_Fun_Selection_Loc     2932
# define    E_User_Print_Fun_Record_Length  14

# define	Memory_Fun_Selection_Loc        2946
# define    User_Memory_Fun_Record_Length   4

# define	Settings_Selection_Loc          2950
# define    User_Settings_Record_Length     10

# define	View_Fun_Selection_Loc          2960
# define    User_View_Fun_Record_Length     4

# define    Message_Selection_Loc           2964
# define    User_Msg_Fun_Record_Length      4

struct adminmenu
{
    UINT8 A_DATA_ENTRY;
    UINT8 A_DATA_TRANSFER;
    UINT8 A_EXT_PRINTER;
    UINT8 A_INT_PRINTER;
    UINT8 A_MEM_FUN;
    UINT8 A_VIEW_REPORTS;
    UINT8 A_AETTINGS;
    UINT8 A_RATE_TABLES;
    UINT8 A_MESSAGE;
    UINT8 A_USER_ACCESS;
    
}__attribute__ ((__packed__)) Adminmenurec;

struct Mainfunmenu
{    
    UINT8 DATA_ENTRY_FUN;
    UINT8 DATA_TRANSFER_FUN;
    UINT8 EXT_PRINT_FUNS;
    UINT8 INT_PRINT_FUNS;
    UINT8 MEM_FUNS;
    UINT8 VIEW_REPORTS;
    UINT8 SETTINGS;
    UINT8 RATE_TABLES;
    UINT8 MESSAGE;
    UINT8 PERMISSIONS;
  
}__attribute__ ((__packed__)) MainMenurec;

struct Dataentrymenu
{
    UINT8 COLLECTION;
    UINT8 DEDUCTION;
    UINT8 LOCAL_SALE;
    UINT8 SANGH_SALE;
    UINT8 TRANSFER;
    //UINT8 SHIFT_CLOSE;
    
}__attribute__ ((__packed__)) Dataentryrec;


struct transferfun
{
    UINT8 PENDRIVE;
    UINT8 SERVER;
}__attribute__ ((__packed__)) Datatransferrec;

struct Printfunctions
{    
    UINT8 ABSENT_REPORT;
    UINT8 COL_RECEIPT;
    UINT8 DEDUCTION_REPORT;
    UINT8 DETAILED_REPORT;
    UINT8 SUMMARY_REPORT;
    UINT8 MEMBER_LIST;
    UINT8 INACTIVE_LIST;
    UINT8 LOCAL_SALE_REPORT;
    UINT8 SANGH_SALE_REPORT;
    UINT8 MEMBER_BILL_REPORT;
    UINT8 MEMBERWISE_REPORT;
    UINT8 PAYMENT_REGISTER;
    UINT8 DAYWISE_REPORT;
    UINT8 BONUS_REPORT;
  
}__attribute__ ((__packed__)) ExtPrintFunrec,PrintFunrec;


struct Memorymenu
{
    UINT8 EDIT_SHIFT;
    UINT8 MEMBER_DETAILS;
    UINT8 EDIT_LOCALSALE;
    UINT8 EDIT_SANGHSALE;
    
}__attribute__ ((__packed__)) MemoryFunrec;

struct Viewmenu
{
    UINT8 V_COL;
    UINT8 V_DETAILED;
    UINT8 V_SUMMARY;
    UINT8 V_PAYMENT;
}__attribute__ ((__packed__)) ViewFunrec;

struct Settingmenu
{    
    UINT8 VIEW_SETTINGS;
    UINT8 COL_SETTINGS1;
    UINT8 COL_SETTINGS2;
    UINT8 MILK_ANALYZER;
    UINT8 PRINTER;
    UINT8 WM_SCALE;
    UINT8 ADMIN;
    UINT8 GPRS;
    UINT8 DEALER;
    UINT8 MASTER;
  
}__attribute__ ((__packed__)) SettingMenurec;

struct smsmenu
{
    UINT8 COL_SMS;
    UINT8 GROUP_SMS;
    UINT8 SUMMARY_SMS;
    UINT8 VIEW_BAL;
}__attribute__ ((__packed__)) SMSFunrec;

void Permissions_Menu(UINT8 pmtype)
{
     Display_Box_Line4();
       
    if(pmtype=='M')
        STRCPY(LBUF,"*MAIN MENU - ACCESS*");
     if(pmtype=='D')
        STRCPY(LBUF,"*DATA ENTRY-ACCESS* ");
     if(pmtype=='C')
        STRCPY(LBUF,"DATA TRANSFER-ACCESS");
    if(pmtype=='T' || pmtype=='E')
        STRCPY(LBUF,"PRINT FUN'S - ACCESS");
    if(pmtype=='S')
        STRCPY(LBUF,"SETTINGS MENU ACCESS");
    if(pmtype=='V')
        STRCPY(LBUF," VIEW REPORTS-ACCESS");
    if(pmtype=='m')
        STRCPY(LBUF,"MEMORY FUN'S-ACCESS"); 
    if(pmtype=='s')
        STRCPY(LBUF,"MESSAGE MENU ACCESS");
      
    Disp_Msg(LINE1);
    
    Disp_Up_Down_Select();
}

UINT8 Menu_Selection_Addr(UINT8 menutype)
{
    if(menutype=='M')   
    {
        if(LOGIN_TYPE=='O' || ALL_PERMISSIONS==1)
        {  
            STRNCPY(USER_MENU_SELECTION,USER_MAIN_MENU_ACCESS,10);
            
        }
        else
        {
            STRNCPY(USER_MENU_SELECTION,ADMIN_MAIN_MENU_ACCESS,10);
        }
        return Main_Menu_Record_Length;
    }    
    
    if(menutype=='D')
    {
        if(LOGIN_TYPE=='O' || ALL_PERMISSIONS==1)
        {
            STRNCPY(USER_MENU_SELECTION,U_DATA_ENTRY_MENU_ACCESS,5);
        }
         else
        {
            STRNCPY(USER_MENU_SELECTION,DATA_ENTRY_MENU_ACCESS,5);
        }
        return Data_Menu_Record_Length;
    }
    
    if(menutype=='C')
    {    
        if(LOGIN_TYPE=='O' || ALL_PERMISSIONS==1)
        { 
            STRNCPY(USER_MENU_SELECTION,U_DATA_TRANSFER_MENU_ACCESS,5);
        }
         else
        {
            STRNCPY(USER_MENU_SELECTION,DATA_TRANSFER_MENU_ACCESS,5);
        }
        return Transfer_Menu_Record_Length;
    } 
    
    if(menutype=='E')
    {       
        if(LOGIN_TYPE=='O' || ALL_PERMISSIONS==1)
        { 
            STRNCPY(USER_MENU_SELECTION,U_EXT_PRINTER_MENU_ACCESS,14);
        }
         else
        {
            STRNCPY(USER_MENU_SELECTION,EXT_PRINTER_MENU_ACCESS,14);
        }
        return E_User_Print_Fun_Record_Length;
    }
    
    if(menutype=='T')
    {  
        if(LOGIN_TYPE=='O' || ALL_PERMISSIONS==1)
        {
            STRNCPY(USER_MENU_SELECTION,U_INT_PRINTER_MENU_ACCESS,14);
        }
         else
        {
            STRNCPY(USER_MENU_SELECTION,INT_PRINTER_MENU_ACCESS,14);
        }
        return User_Print_Fun_Record_Length;
    }
    
    if(menutype=='m')    //memory
    {        
        if(LOGIN_TYPE=='O' || ALL_PERMISSIONS==1)
        {
            STRNCPY(USER_MENU_SELECTION,U_MEMORY_MENU_ACCESS,4);
        }
         else
        {
            STRNCPY(USER_MENU_SELECTION,MEMORY_MENU_ACCESS,4);
        }
        
        return User_Memory_Fun_Record_Length;
    }   
    
    if(menutype=='V')
    {
        if(LOGIN_TYPE=='O' || ALL_PERMISSIONS==1)
        {
            STRNCPY(USER_MENU_SELECTION,U_VIEW_REPORTS_MENU_ACCESS,4);
        }
         else
        {
            STRNCPY(USER_MENU_SELECTION,VIEW_REPORTS_MENU_ACCESS,4);
        }
        return User_View_Fun_Record_Length;
    }     
   
    if(menutype=='S')
    {
        if(LOGIN_TYPE=='O' || ALL_PERMISSIONS==1)
             STRNCPY(USER_MENU_SELECTION,U_ADMIN_SETTINGS_MENU_ACCESS,10);
        else
        {
            STRNCPY(USER_MENU_SELECTION,ADMIN_SETTINGS_MENU_ACCESS,10);
        }
        return User_Settings_Record_Length;
    }
    
    if(menutype=='R')
    {
        
    }
}

void Menu_List_Addr(UINT8 menutype,UINT8 menuarraycnt)
{
    if(menutype=='M')   
        PTR_ADDR=(char *)&MAIN_MENU_MENU_LIST[menuarraycnt][0];
    
    if(menutype=='D')
        PTR_ADDR=(char *)&DATA_ENTRY_MENU_LIST[menuarraycnt][0];
    
    if(menutype=='C')
        PTR_ADDR=(char *)&DATA_TRANSFER_MENU_LIST[menuarraycnt][0];
    
    if(menutype=='E')
        PTR_ADDR=(char *)&PRINTER_MENU_LIST[menuarraycnt][0];
    
    if(menutype=='T')
        PTR_ADDR=(char *)&PRINTER_MENU_LIST[menuarraycnt][0];
    
    if(menutype=='m')  
        PTR_ADDR=(char *)&MEMORY_FUN_MENU_LIST[menuarraycnt][0];
    
     if(menutype=='V')
        PTR_ADDR=(char *)&VIEW_REPORTS_MENU_LIST[menuarraycnt][0];
    
    if(menutype=='S')
        PTR_ADDR=(char *)&SETTINGS_MENU_LIST[menuarraycnt][0];
    
    if(menutype=='s')
        PTR_ADDR=(char *)&MESSAGE_MENU_LIST[menuarraycnt][0];
   
}

void Menu_List_Desc_Addr(UINT8 menutype,UINT8 menuarraycnt)
{
    if(menutype=='M')   
        PTR_ADDR=(char *)&MAIN_MENU_MENU_DESC_LIST[menuarraycnt][0];
    
    if(menutype=='D')
        PTR_ADDR=(char *)&DATA_ENTRY_MENU_DESC_LIST[menuarraycnt][0];    
        
    if(menutype=='S')
        PTR_ADDR=(char *)&SETTINGS_MENU_DESC_LIST[menuarraycnt][0];
    
    if(menutype=='s')
        PTR_ADDR=(char *)&MESSAGE_MENU_DESC_LIST[menuarraycnt][0];
   
}


UINT8 User_Selected_Menu_List(UINT8 funtype)
{
     UINT8 usr_cnt=0,select_cnt=0,usr_total_funs;
     UINT32 usr_tmp_addr;
  
    usr_total_funs=Menu_Selection_Addr(funtype);
          
    while(usr_cnt<usr_total_funs)
    {
        if(USER_MENU_SELECTION[usr_cnt]!=0)
        {             
            Menu_List_Addr(funtype,usr_cnt);

            usr_tmp_addr=select_cnt*16;

            ZERO=1;
           
            Hex_DEC(0,(select_cnt+1),MENU_SELECT_BUF+usr_tmp_addr,2);
            MENU_SELECT_BUF[usr_tmp_addr+2]='.';
            strncpy(MENU_SELECT_BUF+usr_tmp_addr+3,PTR_ADDR,14);              
            
            Shift_Left(MENU_SELECT_BUF+usr_tmp_addr,17);
            
            if(MENU_SELECT_BUF[usr_tmp_addr+15]==' ')
                Shift_Right1(MENU_SELECT_BUF+usr_tmp_addr,16,1);
            
            usr_tmp_addr=select_cnt*20;  

            if(funtype=='M' || funtype=='D' || funtype=='S' || funtype=='s')
            {
                Menu_List_Desc_Addr(funtype,usr_cnt);                     

                strncpy(MENU_SELECT_DESC_BUF+usr_tmp_addr,PTR_ADDR,20);     
            }
            else
            {
                STRCPY(PBUF,"                    ");
                strncpy(MENU_SELECT_DESC_BUF+usr_tmp_addr,PBUF,20);   
            }           

           SELECT_FUN_CODE[select_cnt]='A'+usr_cnt;

           select_cnt++;            
        }
        usr_cnt++;
    }
     
     return select_cnt;   
 } 
