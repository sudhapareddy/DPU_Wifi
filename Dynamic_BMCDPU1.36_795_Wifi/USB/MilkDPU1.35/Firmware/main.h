
//# define SREEJA
//# define AP_Format

//# define Mixed_Milk
//Global Memory divided into two blocks Global_varaiables1(500 bytes) and
//Global_varaiables2(1500 bytes rate tables data)

# define        Upload_Record_Length        29      //27 bytes
# define        Current_Shift_Record_Length  19

# define        SETTINGS_RECORD_LENGTH      11
# define        RECEIPT_RECORD_LENGTH       9

# define        Power_On_Password_Loc       4

# define        Batch_No_Loc                5           //int

# define        SMS_Day_Limit_Loc           7           //int

# define        Members_Server_Flag         9           //char

# define        Cow_Fat_Rate_Id_Loc         10          //int

# define        Buf_Fat_Rate_Id_Loc         12          //int

# define        Cow_Snf_Rate_Id_Loc         14          //int

# define        Buf_Snf_Rate_Id_Loc         16          //int

# define        C_BufFat_Effect_Date_Loc    18          //char

# define        C_BufFat_Effect_Month_Loc   19          //char

# define        C_BufFat_Effect_Year_Loc    20          //char


# define        C_CowFat_Effect_Date_Loc    21          //char

# define        C_CowFat_Effect_Month_Loc   22          //char

# define        C_CowFat_Effect_Year_Loc    23          //char


# define        C_BufSnf_Effect_Date_Loc    24          //char

# define        C_BufSnf_Effect_Month_Loc   25          //char

# define        C_BufSnf_Effect_Year_Loc    26          //char


# define        C_CowSnf_Effect_Date_Loc    27          //char

# define        C_CowSnf_Effect_Month_Loc   28          //char

# define        C_CowSnf_Effect_Year_Loc    29          //char


# define        New_BufFat_Effect_Date_Loc  30          //char

# define        New_BufFat_Effect_Month_Loc 31          //char

# define        New_BufFat_Effect_Year_Loc  32          //char


# define        New_CowFat_Effect_Date_Loc  33          //char

# define        New_CowFat_Effect_Month_Loc 34          //char

# define        New_CowFat_Effect_Year_Loc  35          //char


# define        New_BufSnf_Effect_Date_Loc  36          //char

# define        New_BufSnf_Effect_Month_Loc 37          //char

# define        New_BufSnf_Effect_Year_Loc  38          //char


# define        New_CowSnf_Effect_Date_Loc  39          //char

# define        New_CowSnf_Effect_Month_Loc 40          //char

# define        New_CowSnf_Effect_Year_Loc  41          //char


# define        Machine_Code_Loc            42          //7 char

# define        Default_Settings_Loc        49          //char

# define        Mfg_Date_Loc                50          //6 char

# define        Cow_LocalSales_Ltr_Loc      56          //int

# define        Buf_LocalSales_Ltr_Loc      58          //int

# define        Clear_Memberlist_Flag_Loc   62          //char

//# define        IP_Addr_Domain_Loc          63          //char

//Printer Settings

# define        Default_Printer_Loc         64         //char

# define        Printer_Status_Loc          65         //char

# define        Header_Bold_Loc             66         //char

# define        Amount_Bold_Loc             67         //char

# define        On_Offline_Loc              69         //char

# define        Morning_Shift_Timings_Loc   71          //2 chars

# define        Evening_Shift_Timings_Loc   73          //2 chars

# define        Brand_Name_Loc              75          //16 chars

# define        Dealer_Name_Loc             91          //16 chars

# define        MilkType_Print_On_Loc       107         //char

# define        Fat_Print_On_Loc            108         //char

# define        Snf_Print_On_Loc            109         //char

# define        Water_Print_On_Loc          110         //char

# define        Clr_Print_On_Loc            111         //char

# define        Qty_Print_On_Loc            112         //char

# define        Rate_Print_On_Loc           113         //char

# define        Amount_Print_On_Loc         114         //char

# define        INCENTIVE_Print_On_Loc      115         //char

# define        User_Col_Edit_Opt_Loc       116         //char

# define        Multiple_Shifts_Loc         117         //char

# define        Col_Left_Arrow_Edit_Loc     118         //char

# define        Society_Code_Loc            122         //10 char

# define        Operator_Password_Loc       132         //int

# define        Admin_Password_Loc          134         //int

//wm settings

# define    Accuracy_Loc                    136         //char

# define    Tare_On_Off                     137         //char

# define    WM_Baud_Rate_Loc                138         //int

# define    WM_Dot_Pos_Loc                  140        //char

# define    Tare_Command_Loc                141         //char


//Collection Settings

# define	Weight_Mode_Loc                 142         //char

# define	Fat_Mode_Loc                    143        //char

# define	Analyzer_Type_Loc               144         //char

# define	Use_Cow_SNF_Loc                 145        //char
    
# define	Use_Buf_SNF_Loc                 146         //char

# define 	HighFatFlag_Loc                 147         //char

# define	LowFatFlag_Loc                  148         //char

# define    Mixed_Milk_Loc                  149         //char

# define    Shift_Close_Loc                 150         //char

# define	USE_ANALYZER_PORT2_Loc          151         //char

# define    Collection_Rec_ExtLoc           152         //char

# define    Two_Analyzers_Mode_Loc          153         //1 bytes


# define	Buf_FAT_Rate_Inc_Loc        154         //int

# define	Cow_FAT_Rate_Inc_Loc        156         //int

# define	Buf_SNF_Rate_Inc_Loc        158         //int

# define	Cow_SNF_Rate_Inc_Loc        160         //int

# define	Buf_FAT_Rate_Dec_Loc        162         //int

# define	Cow_FAT_Rate_Dec_Loc        164         //int

# define	Buf_SNF_Rate_Dec_Loc        166         //int

# define	Cow_SNF_Rate_Dec_Loc        168         //int

# define    No_Of_Flashes_Loc           170         //char   int Members_Capacity_Loc

# define    Monitor_Voltage_Loc         171         //char

# define    L_Baud_Rate_Loc             172         //int

# define    Ext_Printer_Type_Loc        174         //char

# define    Date_Change_Loc             175         //char

# define    Current_SIM_Loc             176        //char

# define    Round_Off_Loc               177         //char

# define    CLR_Based_Table_Loc         178         //char

# define    AP_SNF_Formula_Loc          179         //char


//Society Details

# define	Society_Name1_Loc           180         //32 chars

# define	Society_Name2_Loc           212         //32 chars

# define	Society_Name3_Loc           244         //32 chars

# define	Society_Name4_Loc           276         //32 chars

# define	Footer_Name1_Loc            308         //32 chars

//# define	Footer_Name2_Loc            340         //32 chars
# define    Wifi_User_Name_Loc          340         //16 chars

# define    Wifi_User_Pword_Loc         356         //16 chars

# define    Contact_No1_Loc             372         //10 chars

# define    Contact_No2_Loc             382         //10 chars

# define    Contact_No3_Loc             392         //10 chars

# define    Dairy_Name_Loc              404         //32 chars

# define    Col_Manual_Msg_Symbol_Loc   438         //char

# define    WM_Min_Dig_Loc              439         //1 chars 

//GPRS Settings
# define    GPRS_ON_Loc                 440         //char

# define    Port_No_Loc                 441         //int

# define    Auto_SMS_Loc                443         //char

# define    Auto_Transfer_Loc           444         //char

# define    IP_Address_Loc              445         //15 chars

# define    Domain_Name_Loc             460         //20 chars

# define    IP_Addr_Domain_Loc          480          //char

# define    Server_Control_Loc          481         //char

# define    MemberList_Control_Loc      482         //1 byte

# define    RateTables_On_Loc           483         //char

# define    Auto_Manual_On_Loc          484         //char

# define    Machine_Lock_Loc            485         //char

# define    Running_Shift_Details_Loc   486         //4 bytes

# define    Weight_Kg_Loc               490         //1 byte

# define    Multiple_Samples_Loc        491         //1 byte

# define    SPL_Shift_File_Loc          492         //char

# define    Dealer_Selection_Loc        493         //char

# define    WIFI_List_Count_Loc         494         //char


//# define    Running_Shift_Incentive_Loc 493         //4 bytes

# define    Searching_Option_Loc        497         //char

# define    Input_Voltage_Loc           498         //int



			/* FAT & SNF Limits */
# define	SNF_Limits_Length        sizeof(SNFLimitsrec)
			/*Table - Cow Limits*/

# define	Table_Cow_SNF_Limits_Loc    500


# define	Table_C_Min_Fat_Off         0		//int

# define	Table_C_Max_Fat_Off         2		//int

# define	Table_C_Min_SNF_Off         4		//int

# define	Table_C_Max_SNF_Off         6		//int


			/*Table - Buf Limits*/

# define	Table_Buf_SNF_Limits_Loc    508


# define	Table_B_Min_Fat_Off         0		//int

# define	Table_B_Max_Fat_Off         2		//int

# define	Table_B_Min_SNF_Off         4		//int

# define	Table_B_Max_SNF_Off         6		//int

			/*Auto Deduction*/
# define	Auto_Deduction_Loc          516
# define	Auto_Length                 23

# define	Auto_Deduction_On_Loc       516		//char

# define	Deduction_Name_Loc          517		//20 chars

# define	Auto_Deduction_Rate_Loc     537		//int

			/*Auto Commission*/
/*
# define	Auto_Commission_Loc         539
# define	Commission_Length           5

# define	Auto_Commission_On_Loc      539		//char

# define	Normal_Commission_Loc       540		//int

# define	Special_Commission_Loc      542		//int*/


//Table based
# define	FAT_Limits_Length           sizeof(Fatlimitsrec)
			/* COW FAT Limits	*/


# define	Table_Cow_FAT_Limits_Loc    544


# define	Table_C_Min_Fat_Off         0		//int

# define	Table_C_Max_Fat_Off         2		//int


                /* BUF FAT Limits	*/


# define	Table_Buf_FAT_Limits_Loc    548


# define	Table_B_Min_Fat_Off         0		//int

# define	Table_B_Max_Fat_Off         2		//int

	/* Effective SNF Table FAT & SNF Limits	*/

/*Table - Cow Limits*/

# define	Effect_Table_Cow_SNF_Limits_Loc    552

# define	Effect_Table_C_Min_Fat_Off         0		//int

# define	Effect_Table_C_Max_Fat_Off         2		//int

# define	Effect_Table_C_Min_SNF_Off         4		//int

# define	Effect_Table_C_Max_SNF_Off         6		//int


/*Table - Buf Limits*/

# define	Effect_Table_Buf_SNF_Limits_Loc    560

# define	Effect_Table_B_Min_Fat_Off         0		//int

# define	Effect_Table_B_Max_Fat_Off         2		//int

# define	Effect_Table_B_Min_SNF_Off         4		//int

# define	Effect_Table_B_Max_SNF_Off         6		//int

		/* COW FAT Limits	*/


# define	Effect_Table_Cow_FAT_Limits_Loc    568

# define	Effect_Table_C_Min_Fat_Off         0		//int

# define	Effect_Table_C_Max_Fat_Off         2		//int


                /* BUF FAT Limits	*/


# define	Effect_Table_Buf_FAT_Limits_Loc    572

# define	Effect_Table_B_Min_Fat_Off         0		//int

# define	Effect_Table_B_Max_Fat_Off         2		//int


/*Deduction Names*/

//Total:9*19=171 bytes

# define	Deduction_Names_Loc     576
# define	Deduction_Name_Length	19

//Cow Slabs
//Total Slabs:30x16=480 bytes

# define    C_Total_Slabs_Loc       747         //char
# define    C_Equ_Slab_Loc          748       //char
# define    C_Rate_Change_Loc       749        //int

# define	C_Slabs_Loc             763
# define	Slab_Record_Length      16


//Buf Slabs
//Total Slabs:30x16=480 bytes

# define    B_Total_Slabs_Loc       1231         //char
# define    B_Equ_Slab_Loc          1232        //char
# define    B_Rate_Change_Loc       1233         //int

# define	B_Slabs_Loc             1247

	

//Total eeprom data 1715

//SMS Counter
//Records:25*21=525
# define    SMS_DATA_RTC_Loc        15
# define    SMS_START1_Loc          1750         //char
# define	SMS_Record_Length       21

//# define	Mobile_No1_Off           0          //20 chars
//# define	SMS_Count_Off           20

# define    Current_Flash_No_Loc    1780        //char
# define    Current_Shift_No_Loc    1781        //long

# define    No_Of_Shifts_F0_Loc     1797        //long 12*4=48 bytes


//Effective Cow Slabs
//Total Slabs:30x16=480 bytes

# define    Effect_C_Total_Slabs_Loc  1851         //char
# define    Effect_C_Equ_Slab_Loc     1852       //char
# define    Effect_C_Rate_Change_Loc  1853        //int

# define	Effect_C_Slabs_Loc        1867 
# define	Effect_C_Slab_Record_Length 16


//Effect Buf Slabs
//Total Slabs:30x16=480 bytes

# define    Effect_B_Total_Slabs_Loc       2351         //char
# define    Effect_B_Equ_Slab_Loc          2352        //char
# define    Effect_B_Rate_Change_Loc       2353         //int

# define	Effect_B_Slabs_Loc             2367


	/*Auto Commission*/


# define	Auto_Commission_On_Loc      2850		//char

# define	Auto_Commission_Loc         2850
# define	Commission_Length           21//5

# define	Normal_Commission_Loc       2851		//int

# define	Special_Commission_Loc      2853		//int

# define    Running_Shift_Incentive_Loc 2875         //10*2=20 bytes

# define    WIFI_Details_Loc            2900        //5*32=160 bytes
# define    WIFI_Record_Length          32



# define    Global_Variables_Loc    4096    //1687552

# define    Temp_Sector             8192

# define    Temp_Data_Sector_Flag   12288

 # define   Temp_Data_Sector        16384

# define    Global_Variables2_Loc   20480

# define    SMS_Counter_Loc         24576


# define    TempUpload_Loc          28672
# define    Sample_Records_Loc      32768   //(5*4096)



//Each Deduction:4 bytes
//Total Members :1000*4=4000 bytes(4K)


# define	Deduction_Details_Loc   53248
# define	Deduction_Record_Length	4

# define	Deduction_Amount_Off	0	

//Member Details total data 49,152(12x4K)

# define	Member_Details_Loc      57344 
# define	Member_Record_Length	46

# define    Member_Code_Off         0               //int

# define	Member_Name_Off         2		//20 chars including null

# define	Milk_Type_Off           22		//char

# define	Commission_Type_Off     23		//char

# define	Member_Status_Off       24		//char

# define	Member_Contact_No_Off	25		//10 char

# define    Member_Barcode_Off      35      //10 characters

# define    Member_Transfer_Off     45      //char


// member details end location 106496+4096=110592




//End Address       1556480


			/*SNF Table - COW details*/

//FAT DIFFERENCE MAXIMUM    12.8
//SNF DIFFERENCE MAXIMUM    12.8
//128*128*2=32768 bytes

# define    Cow_SNFTable_Loc	110592
//Cow snf table end location    1589248


			/*SNF Table - BUF details*/

//FAT DIFFERENCE MAXIMUM    12.8
//SNF DIFFERENCE MAXIMUM    12.8
//128*128*2=32768 bytes

# define    Buf_SNFTable_Loc	143360
//BUF snf table end location    1622016


/*Effective SNF Table - COW details*/
//FAT DIFFERENCE MAXIMUM    12.8
//SNF DIFFERENCE MAXIMUM    12.8
//128*128*2=32768 bytes

# define    Effect_Cow_SNFTable_Loc	176128
//Effective Cow snf table end location    1654784


/*Effective SNF Table - BUF details*/
//FAT DIFFERENCE MAXIMUM    12.8
//SNF DIFFERENCE MAXIMUM    12.8
//128*128*2=32768 bytes

# define    Effect_Buf_SNFTable_Loc	208896
//Effective BUF snf table end location    1687552


		/*FAT Rate Table  */

//FAT Range:0.0 to 15.0
//Total:128x2=256 Bytes Cow ,BUF normal and effective

# define	Cow_FatTable_Loc        241664//106496
# define	Buf_FatTable_Loc        241920//106752
# define	Effect_Cow_FatTable_Loc	242176//107008
# define	Effect_Buf_FatTable_Loc	242432//107264


# define	Fat_Record_Length	2



			/*Local Sales*/

//Morning Shift:11 bytes
//Total Days:31
//Total:31*11=248 bytes
//12 Months:12*31*11=2976 bytes x4=12000(3x4k=12288)

# define	LocalSales_Loc              242688//241664+1024//1757184 //1511424 //
# define	LocalSales_Record_Length    11
# define	LocalSales_Month_Length     1364 //248

# define	L_Quantity_Off		0		//long

# define	L_Amount_Off		4		//long

//Local sales end address  253952

//Sangh sales data 1000x12=12000(3x4K)
# define	SanghSales_Loc              253952//1769472 //1523712//
# define	Sangh_Record_Length         11
# define	SanghSales_Month_Length     1364 //310

# define	S_Quantity_Off              0		//long

# define	S_Fat_Off                   4		//char

# define	S_SNF_Off                   5		//char

# define	S_Amount_Off                6		//int

//Sangh sales ending location 266240


// In each block for shifts 65536 bytes allotted i.e., Max Shifts in a block 4681
// each block consists of 160 4k blocks
//    16 4k blocks for shift data 
//    144 4k blocks for Collection data 49152 records max


# define    SHIFT_DATE_S_LOC    266240
# define    SHIFT_DATE_LEN      30
//# define    SHIFT_DATE_LEN      14    //dd,mm,yy,shift,S.RNO(4bytes),E.RNO(2bytes) Nor.comm(2bytes),spl.comm(2bytes)
//# define    SHIFT_DATE_R_LEN    10  //dd,mm,yy,shift,S.RNO(4bytes),E.RNO(2bytes)




//Flash Total Capacity: 7FFFFF (83,88,607 Bytes)
//Total 12 blocks
//Each Block:160 * 4K Sectors :655360

//Shift Data:16x4k sectors for shift dates
//SHIFT_DATE_LEN 14bytes

//144X4k sectors for collection data

//49000 Records for each block
//4500 Shift Dates for each block
//800 records for shift - 1 year data

// Total 64 4k blocks for general data
# define    STARTING_RECORD_LOC 331776        


	
# define     FLASH_MAX_SHIFTS    4500 //exact value 4681
# define     FLASH0_MAX_RECORDS  42000  //exact 42130

//# define     FLASH0_MAX_RECORDS  49000  //exact 49152

# define    MILK_RECORD_LENGTH  14
			


struct ShiftDateStructure
{
    UINT8 DAY_SHIFT;
    UINT8 MONTH;
    UINT8 DAY;
    UINT8 YEAR;
    UINT32 SRNO;
    UINT16 NOREC;
    //UINT16 COMM_AMT[10];
    UINT16 NOR_COMM;
    UINT16 SPL_COMM[9];
    
    
}__attribute__ ((__packed__)) ShiftDateRecord;
struct ratesheet
{
    UINT16  FAT;
    UINT16  RATE;

}__attribute__ ((__packed__)) Milkrec,Milkrec1;

struct Memberrecord
{
    UINT16 C_CODE; 
    BYTE  NAME[20];
    UINT8 MILK_TYPE;
    UINT8 COMMISSION_TYPE;
    UINT8 STATUS;
    BYTE  CONTACT_NO[10];
    BYTE  BARCODE[10];
    UINT8 UPDATE_STATUS;    

}__attribute__ ((__packed__)) Memberrec,tempmemberrec,GPRSMemberrec;

struct LocalSales
{   
    UINT32 QTY;
    UINT32 AMOUNT;
    UINT8  MILK_TYPE;
    UINT8  MONTH;
    UINT8 YEAR;
                                                                                                                                    
}__attribute__ ((__packed__)) LocalSalesrec,LocalSalesrec1;

struct SanghSales
{    
    UINT32 QTY;
    UINT8 FAT;
    UINT8 SNF;
    UINT16 RATE;
    UINT8 MILK_TYPE;
    UINT8 MONTH;
    UINT8 YEAR;

}__attribute__ ((__packed__)) Sanghrec,Sanghrec1;

struct PrintReceipt
{    
    UINT8 MILK_TYPE_PRINT;
    UINT8 FAT_PRINT;
    UINT8 SNF_PRINT;
    UINT8 WATER_PRINT;
    UINT8 CLR_PRINT;
    UINT8 QTY_PRINT;
    UINT8 RATE_PRINT;
    UINT8 AMOUNT_PRINT;
    UINT8 INCENTIVE_PRINT;
  
}__attribute__ ((__packed__)) Receiptrec;


struct shift
{
    UINT8 DAY_SHIFT;
    UINT8 MONTH;
    UINT8 DAY;
    UINT8 YEAR;
}__attribute__ ((__packed__)) Shiftrec,RunningShift,TempShift,GPRSShiftrec,EditShiftrec,ShiftRecords[1000];

struct FlagsStructure
{
    UINT8 MILK_TYPE;
    UINT8 SHIFT1;
    UINT8 WEIGHT_MODE;
    UINT8 ANALYZER_MODE;
    UINT8 SMS_STATUS;
    UINT8 UPLOAD_STATUS;
    UINT8 PAYMENT_STATUS;
    UINT8 INCENTIVE_COMM;
    UINT8 RESERVED1;
    UINT8 RESERVED2;
        
}__attribute__ ((__packed__)) CollectionFlags,TempFlags;

struct	CollectionStructure
{
    UINT16 CODE;
    UINT16 QTY;
    UINT8 WTIME[3];
    UINT8 FLAGS;  //bit0:Milk_type,bit1:shift,bit2:Weight_Mode,bit3:Analyzer_Mode,bit4:SMS_Status,bit5:Upload_Status,bit6:Payment_Status,bit7:Incentive Type    
    UINT8 FAT;
    UINT8 SNF;   
    UINT16 RATE;   
    UINT16 CLR1;

    UINT8 ATIME[3]; 
    UINT16 WATER1;
    UINT32 INCENTIVE_AMT;
    UINT32 SHIFT_RECORD_NO;    
    
}__attribute__ ((__packed__)) CollectionRecord,AnalyzerRecord,TempRecord,EditRecord,CollectionRecords[1000];

struct CollectionRecordLoc
{
    UINT8 Type;
    UINT32 RECORDNO;
    UINT8 FLASH_NO;
    
}__attribute__ ((__packed__)) CollectionRecord_NO,TempCollectionRecord_NO,CollectionRecords_NOS[1000];

struct matchrec
{
    UINT16 CODE_SNO;
    UINT16 MEMBER_CODE;
}__attribute__ ((__packed__)) Matchrec;

struct ReportsMatch
{
    UINT16 CODE_SNO;
    UINT16 CUST_CODE;
    
}__attribute__ ((__packed__)) ReportsMatchrec[105];

struct AnalyzerTra
{
    UINT16 CLR;
    UINT16 WATER;
    UINT8 Date[2];
}__attribute__ ((__packed__)) AnalyzerTrarec,AnalyzerTrarec1;

struct Deduction
{
    UINT8 TYPE;
   // UINT32 AMT;
    INT32 AMT;

}__attribute__ ((__packed__)) Deductionrec,Deductionrec1;

struct autoDeduction
{
    UINT8 STATUS;
    BYTE DEDUCTION_NAME[20];
    UINT16 RATE;

}__attribute__ ((__packed__)) Autorec;

struct Commission
{
    UINT8 STATUS;
    UINT16 AUTO_RATE;
    UINT16 SPECIAL_RATE[9];

}__attribute__ ((__packed__)) Commissionrec,tempCommissionrec;

struct Incentive
{
    UINT16 NOR_COMM;
    UINT16 SPL_COMM[9];

}__attribute__ ((__packed__)) Incentiverec;

struct Shifttimings
{
    UINT8 START_HOUR;
    UINT8 END_HOUR;
}__attribute__ ((__packed__)) ShiftTimerec,M_ShiftTimerec,E_ShiftTimerec;

struct limitsrecord
{
    UINT16 MIN_FAT;
    UINT16 MAX_FAT;
    UINT16 MIN_SNF;
    UINT16 MAX_SNF;
    

}__attribute__ ((__packed__)) SNFLimitsrec,CowSNFLimitsrec,BufSNFLimitsrec;

struct Fatlimitsrecord
{
    UINT8 MIN_FAT;
    UINT8 MAX_FAT;

}__attribute__ ((__packed__)) Fatlimitsrec,CowFatLimitsrec,BufFatLimitsrec;

struct balrec
{
    UINT32 QTY;
    UINT32 CREDIT_AMT;
    INT32  DEBIT_AMT;
    UINT32 PAID_AMT;
    UINT32 AVG_RATE;
    UINT32 PAID_QTY;
    UINT32 INCENT_AMT;
    UINT32 AVG_FAT;
    UINT32 AVG_SNF;   
    UINT32 AVG_CLR; 

}__attribute__ ((__packed__)) Balancerec,BalanceRecords[210];

struct FATTables
{
    UINT8 MILK_TYPE;
    UINT16  MILK_FAT;
    UINT16  MILK_SNF;
    UINT16  MILK_RATE;

}__attribute__ ((__packed__)) Tablerec,TempTablerec;

struct cummulative
{
    UINT16 B_MEMBERS;
    UINT32 B_FAT;
    UINT32 B_SNF;
    UINT32 B_CLR;
    UINT32 B_QTY;
    UINT64 B_RATE;
    UINT32 B_AMOUNT;
    UINT32 B_INC_AMT;
    UINT32 B_PAID_AMOUNT;
    UINT32 B_DEDUCTION;

    UINT16 C_MEMBERS;
    UINT32 C_FAT;
    UINT32 C_SNF;
    UINT32 C_CLR;
    UINT32 C_QTY;
    UINT64 C_RATE;
    UINT32 C_AMOUNT;
    UINT32 C_INC_AMT;
    UINT32 C_PAID_AMOUNT;
    UINT32 C_DEDUCTION;


}__attribute__ ((__packed__)) Cumrec;

struct common
{
    UINT8  MILK_TYPE;
    UINT16 MEMBERS;
    UINT32 FAT;
    UINT32 SNF;
    UINT32 CLR1;
    UINT32 QTY;
    UINT64 RATE;
    UINT32 AMOUNT;
    UINT32 INC_AMT;
    UINT32 PAID_AMOUNT;
    UINT32 DEDUCTION;
    UINT32 TOTAL_AMOUNT;
}__attribute__ ((__packed__)) USBsendrec;

struct Dateregister
{
    UINT8 START_DATE;
    UINT8 START_MONTH;
    UINT8 START_YEAR;
    UINT8 END_DATE;
    UINT8 END_MONTH;
    UINT8 END_YEAR;

}__attribute__ ((__packed__)) Day_Monthrec;

struct memberbill
{
    UINT32 TOTAL_AMT;
    UINT32 INCENT_AMT;
    UINT32 BILL_AMT;
    UINT32 PAID_AMT;
    INT32 DED_AMT;
    UINT32 GRAND_TOTAL;
    
}__attribute__ ((__packed__)) MemberBillrec;

struct fromrec
{
    UINT32 START_MEMBER;
    UINT32 END_MEMBER;
    
}__attribute__ ((__packed__)) Fromrec;

struct effect
{
    UINT8 DATE;
    UINT8 MONTH;
    UINT8 YEAR;

}__attribute__ ((__packed__)) Effectrec;

struct smsrec
{
    BYTE SIM_NO[20];
    UINT8 COUNTER;

}__attribute__ ((__packed__)) SMSrec;

struct wifirec
{
    BYTE WIFI_NAME[16];
    BYTE WIFI_PASSWORD[16];

}__attribute__ ((__packed__)) WIFIrec1,AUTO_WIFI_rec;
struct slabs
{
    UINT16 START_FAT;
    UINT16 END_FAT;
    UINT16 START_SNF;
    UINT16 END_SNF;
    UINT32 FAT_TS_RATE;
    UINT32 SNF_TS_RATE;

}__attribute__ ((__packed__)) Slabrec,Slabs[10];

struct weigh
{
    UINT8  WM_ACCURACY;
    UINT8  WM_AUTO_TARE;
    UINT16 WM_BAUDRATE;
    UINT8  WM_DECIMAL_POINT;
    UINT8  WM_TARE_COMMAND;

}__attribute__ ((__packed__)) WMrec;

# define WM_RECORD_LENGTH 6

struct gprs
{
    UINT8  GPRS_ON;
    UINT16 PORT_NO;
    UINT8  AUTO_SMS;
    UINT8  AUTO_TRANSFER;
    BYTE   IP_ADDR[15];   
    
}__attribute__ ((__packed__)) GPRSrec;

struct domain
{
    BYTE   IP_ADDR[15];
    BYTE   DOMAIN_NAME[20];
    UINT8  IP_DOMAIN_SELECTION;
    
}__attribute__ ((__packed__)) Domainrec;


struct dpu
{
    UINT8 MEMBER_LIST;
    UINT8 RATE_TABLES;
    UINT8 COLLECTION;

}__attribute__ ((__packed__)) DPUcontrolrec;

struct col
{
    UINT8 WEIGHT_MODE;
    UINT8 FAT_MODE;
    UINT8 ANALYZER;
    UINT8 COW_SNF;
    UINT8 BUF_SNF;
    UINT8 HIGH_FAT_SNF;
    UINT8 LOW_FAT_SNF;
    UINT8 MIXED_MILK;
    UINT8 SHIFT_CLOSE;
    UINT8 USE_ANALYZER_PORT2;
    UINT8 Collection_Rec_Ext;

}__attribute__ ((__packed__)) Settingsrec;














