
const UINT8 DEALER_PROGRM_NAME[][12]={ 
                                        " GOPAL REDDY",
                                    };

const UINT8 MASTER_MENU_LIST[][16]={
                
                                    " DEALER PROGRAM ",
                                    " MFG. DATE      ",
                                    " DPU BATCH NO   ",
                                    " ONLINE MODE    ",
                                    " BRAND NAME     ",
                                    " DEVICE CODE    ",
                                    "DEFAULT SETTINGS",
                                    "AUTO SHIFT CLOSE",
                                    " DEALER NAME    ",
                                    " QUANTITY IN Kg ",
                                    " SERVER CONTROL ",
                                    " SPL. SHIFT FILE",     
                                    "MULTIPLE SAMPLES",
                                    " MULTIPLE SHIFTS",
                                    " DATA RECOVERY  ",
                                    " CLR BASED TABLE",
                                    " RESET SETTINGS ",
                                    " INPUT VOLTAGE  ",
};

const UINT8 MAIN_MENU_MENU_LIST[][16]={
                                    "DATA ENTRY      ",
                                    "DATA TRANSFER   ",
                                    "EXT PRINTER     ",
                                    "INT PRINTER     ",
                                    "MEMORY FUN'S    ",
                                    "REPORTS VIEW    ",
                                    "SETTINGS        ",
                                    "RATE TABLES     ",
                                    "MESSAGE         ",
                                    "PERMISSIONS     ",
                                    
};

const UINT8 MAIN_MENU_MENU_DESC_LIST[][20]={
                                    "Collect,Sale,Deduct ",
                                    "Pendrive            ",
                                    "Ext.Printer Reports ",
                                    "Int.Printer Reports ",
                                    "Add/Edit/Del Record ",
                                    "Reports Viewing...  ",
                                    "Admin & Hardware... ",
                                    "FAT&SNF Rate Tables ",
                                    "Single / Group SMS  ",
                                    "UserAccess Function ",
};                   

const UINT8 PRINTER_MENU_LIST[][16]={
                                    "ABSENT MEMBERS  ",
                                    "COL. RECEIPT    ",
                                    "DEDUCTION       ",
                                    "DETAILED SHIFT  ",
                                    "SUMMARY REPORT  ",
                                    "MEMBER LIST     ",
                                    "INACTIVE LIST   ",
                                    "LOCAL SALE      ",
                                    "SANGH SALE      ",
                                    "MEMBER BILLS    ",
                                    "MEMBERWISE      ",
                                    "PAYMENT REG.    ",
                                    "DAYWISE REPORT  ",
                                    "BONUS REPORT    ",
};

const UINT8 DATA_ENTRY_MENU_LIST[][16]={
                                    "COLLECTION      ",
                                    "DEDUCTION       ",
                                    "LOCAL SALE      ",
                                    "SANGH SALE      ",   
                                    "TRANSFER        ",
};
      
const UINT8 DATA_ENTRY_MENU_DESC_LIST[][20]={  
                                        "Milk Collection...  ",
                                        "Deduction Entries   ",
                                        "Entry of Local Sale ",
                                        "Entry of Sangh Sale ",
                                        "Current Shift Data  ",
};     

const UINT8 DATA_TRANSFER_MENU_LIST[][16]={
                                    "PENDRIVE        ",
                                    "SERVER          ",
};

const UINT8 MEMORY_FUN_MENU_LIST[][16]={
                                        "EDIT SHIFTDATA  ",
                                        "MEMBER DETAILS  ",
                                        "EDIT LOCALSALE  ",
                                        "EDIT SANGHSALE  ",
};

const UINT8 VIEW_REPORTS_MENU_LIST[][16]={
                                        "COLLECTION      ",
                                        "DETAILED SHIFT  ",
                                        "SUMMARY REPORT  ",
                                        "PAYMENT REG.    ",
};

const UINT8 FAT_RATES_MENU_LIST[][16]={    
                                        " 1.FAT LIMITS   ",
                                        " 2.FAT EQUATION ",
                                        " 3.VIEW & EDIT  ",
                                        " 4.RATE CHANGE  ",
                                        "3.EFFECTIVE DATE",
};

const UINT8 FAT_RATES_MENU_DESC_LIST[][20]={
                                        "Minimum & Maximum   ",
                                        "Equation based Rate ",
                                        "Fat Rates View&Edit ",
                                        "Increase / Decrease ",
                                        "New Effect Date...  ",
};

const UINT8 SNF_RATES_MENU_LIST[][16]={
                                        " 1.LIMITS       ",
                                        " 2.FAT+SNF STEPS",
                                        " 3.FORMULA      ",
                                        " 4.FAT+SNF SLABS",
                                        " 5.PRINT SLABS  ",
                                        " 6.VIEW & EDIT  ",
                                        " 7.RATE CHANGE  ",
                                        "6.EFFECTIVE DATE",
};
           
const UINT8 SNF_RATES_MENU_DESC_LIST[][20]={
                                        "Minimum & Maximum   ",
                                        "Different Steps...  ",
                                        "With TS Formula...  ",
                                        "Different Slabs...  ",
                                        "Print Entered Slabs ",
                                        "Rates View & Edit   ",
                                        "Increase / Decrease ",
                                        "New Effect Date...  ",
};

const UINT8 IMPORT_DATA_MENU_LIST[][16]={    
                                        " 1.ALL TABLES   ",
                                        " 2.BUF FAT TABLE",
                                        " 3.BUF SNF TABLE",
                                        " 4.COW FAT TABLE",
                                        " 5.COW SNF TABLE",
                                        "6.EFFECTIVE LIST",
                                        " 7.MEMBERS LIST ",    
};

const UINT8 EXPORT_DATA_MENU_LIST[][16]={
                                        "6.DETAILED SHIFT",
                                        " 7.MEMBER BILL  ", 
                                        " 8.MEMBERS LIST ",
                                        " 9.PAYMENT REG. ",
                                        "10.SUMMARY SHIFT",
                                        "11.SHIFTWISE DAT",
};

const UINT8 EFFECT_DATES_MENU_LIST[][16]={    
                                        " 6.COW FAT DATE ",
                                        " 7.BUF FAT DATE ",
                                        " 8.COW SNF DATE ",
                                        " 9.BUF SNF DATE ",
};

const UINT8 SETTINGS_MENU_LIST[][16]={
                                    "VIEW SETTINGS   ",
                                    "COL-SETTINGS1   ",
                                    "COL-SETTINGS2   ",                
                                    "MILK ANALYZER   ",
                                    "PRINTER         ",
                                    "WEIGHING SCALE  ",                
                                    "ADMIN SETTINGS  ",
#ifdef WIFI
                                    "WI-FI SETTINGS  ",
#elif defined(GPRS)
                                    "GPRS SETTINGS   ",
#endif                                 
                                    "DEALER LOGIN    ",
                                    "SUNIMPEX LOGIN ",
};
                        
const UINT8 SETTINGS_MENU_DESC_LIST[][20]={                       
                                    "Selected Settings...",
                                    "Auto/Manual ,Cut Off",
                                    "Timings,DualAnalyzer",
                                    "Selection,Baud Rate ",
                                    "On/Off,Ext.Printer..",
                                    "Accuracy,B.Rate,Tare",
                                    "Header,Password,Date",
                                    "IP Addr,Port No,Act ",
                                    "For Service Engineer",
                                    "For Company purpose ",
                
};

const UINT8 COL_SETTINGS_MENU_LIST[][16]={
                                        " 1.COLLECT MODE ",
                                        "2.BILL INCENTIVE",
                                        " 3.BUF SNF Based",
                                        " 4.COW SNF Based",
                                        "5.HIGH SNF & FAT",
                                        " 6.LOW SNF & FAT",
                                        " 7.MIXED MILK   ",
};



const UINT8 COL_SETTINGS_MENU_DESC_LIST[][20]={
                                            "Fat&Wt. Auto/Manual ",
                                            "Eff. From Next Shift",
                                            "Use in Collection...",
                                            "Use in Collection...",
                                            "Accept/Reject Limits",
                                            "Accept/Reject Limits",
                                            "During Collection...",   
};

const UINT8 COL_SETTINGS2_MENU_LIST[][16]={
                                        "1.AUTO DEDUCTION",
                                        " 2.COL. RECEIPT ",
                                        "3.DUAL ANALYZERS",
                                        "4.ANALYZER PORT2",
                                        "5.LOCALSALE RATE",
                                        "6.ROUND OFF RATE",
                                        " 7.MORNING SHIFT",
                                        " 8.EVENING SHIFT",
                                        " 9.EDIT CONTROL ",
                                        "10.<- ARROW EDIT",
};
 
const UINT8 COL_SETTINGS2_MENU_DESC_LIST[][20]={                  
                                        "Rate & Name Settings",
                                        "Int/Ext/No Printer  ",
                                        "Select Analyzer Mode",
                                        "In Normal Collection",
                                        "Rate Per Liter...   ",
                                        "Rate round off...   ",
                                        "Shift Start Timing..",
                                        "Shift Start Timing..",
                                        "Col. Editing By User",
                                        "During Online Col.  ",
};


const UINT8 PRINTER_SETTINGS_MENU_LIST[][16]={
                                        " 1.AMOUNT BOLD  ",
                                        " 2.EXT. PRINTER ",
                                        " 3.HEADER BOLD  ",
                                        " 4.DEFAULT PRINT",
                                        "5.PRINTER STATUS",
                                        "6.RECEIPT FIELDS",
                                        "7.MANUAL SYMBOL ",
                                        
};
                
const UINT8 PRINTER_SETTINGS_MENU_DESC_LIST[][20]={
                                        "Prints Amount Bold  ",
                                        "40Column / 80Column ",
                                        "Prints Header Bold  ",               
                                        "Internal Printer    ",
                                        "Check Paper Status  ",
                                        "Fields Print Yes/No ",
                                        "Symbol name printing",
};


 const UINT8 RECEIPT_FIELDS_MENU_LIST[][12]={
                                        " 1.MILK     ",
                                        " 2.FAT      ",
                                        " 3.SNF      ",
                                        " 4.WATER    ",
                                        " 5.CLR      ",
                                        " 6.QUANTITY ",
                                        " 7.RATE     ",
                                        " 8.AMOUNT   ",
                                        " 9.INCENTIVE",  
};

const UINT8 WM_SETTINGS_MENU_LIST[][16]={
                                        " 1.ACCURACY     ",
                                        " 2.AUTO TARE    ",
                                        " 3.BAUD RATE    ",
                                        "4.DECIMAL POINTS",
                                        " 5.TARE COMMAND ",
                                        " 6.MIN.  DIGITS ",
                                        " 7.WM SERVER    ",
};
                
const UINT8 WM_SETTINGS_MENU_DESC_LIST[][20]={
                                        "100 ML or 10 ML     ",               
                                        "Tare to WM On / Off ",
                                        "Select Baud Rate... ",
                                        "No Dot From W.Scale ",
                                        "Tare Letter...      ",
                                        "Number of Digits    ",
                                        "Control from Server ",
};

const UINT8 ADMIN_SETTINGS_MENU_LIST[][16]={
                                        " 1.CHANGE PWORD ",
                                        " 2.DAIRY NAME   ",
                                        "3.DEDUCTION NAME",
                                        " 4.SET DATE&TIME",
                                        "5.SOCIETY DETAIL",
                                        " 6.AP FORMULA   ",
                                        " 7.CHECK VOLTAGE",
                                        " 8.WEIGHT IN Kg ",
                                        " 9.POWER ON PWD ",
                                        "10.SERVER DATA  ",
                                        "11.PHONE NUMBERS",
};
                
const UINT8 ADMIN_SETTINGS_MENU_DESC_LIST[][20]={ 
                                        "Admin & Operator... ",
                                        "MilkDairy Name Entry",
                                        "User Defined Names..",
                                        "Edit Date & Time... ",
                                        "SocietyDetails Entry",
                                        "For Buf SNF Table...",
                                        "Monitor I/P Voltage ",
                                        "Milk Quantity in Kgs",
                                        "Power on Password...",
                                        "To Receive Settings.",
                                        "For ShiftSummary Msg",
};

const UINT8 ON_OFF_SETTINGS_MENU_LIST[][16]={
                                        " *AMOUNT BOLD*  ",
                                        "*BILL INCENTIVE*",
                                        ">MULTIPLE SHIFTS",
                                        "*CLR RATE TABLE*",
                                        "*SERVER CONTROL*",
                                        "*AUTO DEDUCTION*",
                                        "COL EDIT CONTROL",                                        
                                        ">ACCESS <- ARROW",
                                        "*AP SNF FORMULA*",
                                        "*SPL SHIFT FILE*",
                                        "  *HEADER BOLD* ",
                                        "*WEIGHT IN Kgs* ",
                                        "  *MIXED MILK*  ",
                                        "MULTIPLE SAMPLES",
                                        ">DEFAULT PRINTER",
                                        "*ROUND OFF RATE*",
                                        "*AUTO SMS MODE* ",
                                        "AUTO SHIFT CLOSE",
                                        " *AUTO TRANSFER*",
                                        ">MONITOR VOLTAGE",
                                        "*DUAL ANALYZERS*",
                                        "  *SECOND PORT* ",
                                        "*POWER ON PWORD*",
                                        ">MILK TYPE FIELD",
                                        "   *FAT FIELD*  ",
                                        "   *SNF FIELD*  ",
                                        " *WATER FIELD*  ", 
                                        "   *CLR FIELD*  ",
                                        "*QUANTITY FIELD*",
                                        "  *RATE FIELD*  ",
                                        " *AMOUNT FIELD* ",
                                        ">INCENTIVE FIELD",
};
 
   
const UINT8 ON_OFF_SETTINGS_MENU_DESC_LIST[][20]={ 
                                        "Bold in Receipt?    ",
                                        "Incentive for Bill? ",
                                        "More than 1 Shift  ?",
                                        "Use CLR Based Rates?",
                                        "Receive Settings?   ",
                                        "In Member Bill?     ",
                                        "Editing for User?   ",                                       
                                        "Edit in Auto Col.?  ",
                                        "For AP Customers?   ",
                                        "Special EIP File?   ",
                                        "Bold in Receipt?    ",
                                        "Weight Mode Kgs?    ",
                                        "Cow&Buf Combination?",
                                        "More than 1 Sample ?",
                                        "Thermal Printer?    ",
                                        "Paisa Rounded to 0? ",
                                        "Backend SMS Sending?",
                                        "Close Automatically?",
                                        "Backend Transfer?   ",
                                        "Check Input Voltage?",
                                        "Data from 2Analyzers",
                                        "Alternative Port?   ",
                                        "Power On Password?  ",
                                        "Print in Receipt?   ", 
};

const UINT8 GPRS_SETTINGS_MENU_LIST[][16]={
#ifdef GPRS
                                        " 1.AUTO SMS    ",
                                        "2.SMS DAY LIMIT",
#endif
#ifdef WIFI
                                        "1.WIFI SCAN LIST",
                                        " 2.SAVED LIST   ",
#endif
                                        
                                        " 3.DOMAIN NAME ",
                                        " 4.IP ADDRESS  ",
                                        "5.IP ADDR/DOMAIN",
                                        " 6.PORT NUMBER  ",
                                        " 7.AUTO TRANSFER",
#ifdef WIFI
                                        " 8.ACTIVATE WIFI",
#elif defined(GPRS)
                                        " 8.ACTIVATE GPRS",
#endif
                                        
};
                    
 const UINT8 GPRS_SETTINGS_MENU_DESC_LIST[][20]={ 
#ifdef GPRS
                                        "Backend SMS Sending ",
                                        "Sending SMS Per Day ",
#endif
#ifdef WIFI
                                        "Wifi list Selection ",
                                        "Select Wifi frm list",
#endif
                                        "Domain Name Entry...",
                                        "Server IP Addr Entry",
                                        "Connect With IP/Addr",
                                        "Connecting Port No  ",
                                        "Backend Data Sending",
                                        "Online Activation...",
 };         

 const UINT8 SNF_TABLE_NAMES_LIST[][16]={
                                    " 1.BUF SNF TABLE",
                                    " 2.COW SNF TABLE",
 };
 
 const UINT8 FAT_TABLE_NAMES_LIST[][16]={
                                    " 1.BUF FAT TABLE",
                                    " 2.COW FAT TABLE",
 };
 
 const UINT8 SOCIETY_DETAILS_MENU_LIST[][16]={
                                    " 1.ADDRESS LINE1",
                                    " 2.ADDRESS LINE2",
                                    " 3.ADDRESS LINE3",
                                    " 4.FOOTER LINE1 ",
                                    " 5.SOCIETY CODE ",
                                    " 6.SOCIETY NAME ",
 };
 
 const UINT8 AUTO_DEDUCTION_MENU_LIST[][15]={    
                                        " ON/OFF        ",
                                        " DEDUCTION NAME",
                                        " DEDUCTION RATE",
 };
                
const UINT8 AUTO_DEDUCTION_MENU_DESC_LIST[][20]={
                                        "Use in Bill or not  ",
                                        "Print Name in Bill  ",
                                        "Deduction Amt / Bill",
};
                
const UINT8 PENDRIVE_MENU_LIST[][14]={
                                    " 1.EXPORT DATA",
                                    " 2.IMPORT DATA",
};

const UINT8 PENDRIVE_MENU_DESC_LIST[][20]={
                                        "Reports, Member List",
                                        "Tables, Member List ",
};

const UINT8 MESSAGE_MENU_LIST[][16]={
                                    "1.COLLECTION SMS",
                                    " 2.GROUP SMS    ",
                                    " 3.SHIFT SUMMARY",
                                    " 4.VIEW BALANCE ",
};
    
const UINT8 MESSAGE_MENU_DESC_LIST[][20]={    
                                    "Selected Member     ",
                                    "Selected Shift      ",
                                    "Selected Shift      ",
                                    "Check Balance...    ",
};

const UINT8 USER_INTERFACE_MENU_LIST[][16]={
                                    " 1.MAIN MENU    ",
                                    " 2.SUB MENU     ",
 };

const UINT8 SETTINGS_SUBMENU_LIST[][20]={
                                    "                    ",
                                    "COLLECTION SETTING-1",
                                    "COLLECTION SETTING-2",
                                    "*ANALYZER SETTINGS* ",
                                    " *PRINTER SETTINGS* ",
                                    "    *WM SETTINGS*   ",
                                    "  *ADMIN SETTINGS*  ",
#ifdef WIFI
                                    "   *WI-FI SETTINGS* ",
                                    #endif      
#ifdef GPRS
                                    "   *GPRS SETTINGS*  ",
#endif                                    
                                    "  *DEALER SETTINGS* ",
                                    "*SUN IMPEX SETTINGS*",
                                    "  *WIFI SELECTION*  ",
};
     
const UINT8 WIFI_SELECT_MODE[][16]={
                                    " 1.AUTO SCAN    ",
                                    " 2.MANUAL MODE  ",
 };

const UINT8 WIFI_DETAILS_MENU_LIST[][16]={
                                    " 1.SCAN MODE    ",
                                    " 2.LIST ENTRY   ",
 };

const UINT8 WIFI_ENTRY_LIST[][16]={
                                    " 1.WIFI USER1   ",
                                    " 2.WIFI USER2   ",
                                    " 3.WIFI USER3   ",
                                    " 4.WIFI USER4   ",
                                    " 5.WIFI USER5   ",
 };
 
      