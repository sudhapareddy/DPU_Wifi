
#include "prototypes.h"

# define Normal     0x00
# define DoubleH    0x01
# define DoubleW    0x10
# define DoubleWH   0x11

# define LEFT       0x00
# define CENTER     0x01
# define RIGHT      0x02

void Init_Regular_Font(void)
{
    Printer_SendByte(0x1B);
    Printer_SendByte(0x21);
    Printer_SendByte(0x00);
}

void no_data(void)
{
    Display_Boarder3();
    STRCPY(LBUF,"  NO DATA...");
    Disp_Msg_No_Spaces(0,LINE5+1);
    Delay_Sec(1);
}

void Init_Variables(void)
{
    TOTAL_QTY=0;
    TOTAL_AMT=0;    
    TOTAL_PAID_AMT=0;
    TOTAL_DEDUCTION=0;
    TOTAL_AVG_RATE=0;
    TOTAL_MEMBERS=0;
    TOTAL_INCENTIVE=0;

    B_TOTAL_QTY=0;
    B_TOTAL_AMT=0;
    B_TOTAL_PAID_AMT=0;
    B_TOTAL_DEDUCTION=0;
    B_TOTAL_AVG_RATE=0;
    B_MEMBERS=0;
    B_TOTAL_INCENTIVE=0;
    
    TOTAL_QTY_COW=0;
    TOTAL_QTY_BUF=0;

    ZERO=0;
}

void USB_Print_SendString(UINT8 printertype,UINT8 printbold,UINT8 noc)
{
    if(printertype=='T')
    {        
        PBUF[noc]=0x00;        
        Printer_SendString(0x04,noc);
    }
    if(printertype=='E')
    {
        PBUF[noc+2]=0x00;
        Shift_Right1(PBUF,noc,2);
        if(printbold==1)
            Ext_Printer_SendString_Bold();
        else
            Ext_Printer_SendString(); 
    }
    if(printertype=='U')
    {
        PBUF[noc]='\r';
        PBUF[noc+1]='\n';
        Shift_Left(PBUF,noc);
        FSfwrite(PBUF,1,(noc+2),myFile);
    }
}

void Init_Cum_Values(void)
{
    Cumrec.C_QTY=0;
    Cumrec.C_AMOUNT=0;
    Cumrec.C_FAT=0;
    Cumrec.C_SNF=0;
    Cumrec.C_CLR=0;
    Cumrec.C_RATE=0;
    Cumrec.C_MEMBERS=0;
    Cumrec.C_INC_AMT=0;
    Cumrec.C_DEDUCTION=0;
    Cumrec.C_PAID_AMOUNT=0;

    Cumrec.B_QTY=0;
    Cumrec.B_RATE=0;
    Cumrec.B_AMOUNT=0;
    Cumrec.B_FAT=0;
    Cumrec.B_SNF=0;
    Cumrec.B_CLR=0;
    Cumrec.B_MEMBERS=0;
    Cumrec.B_INC_AMT=0;
    Cumrec.B_DEDUCTION=0;
    Cumrec.B_PAID_AMOUNT=0;

}

void Initialize_Thermal(void)
{
    Printer_SendByte(0x1B);
    Printer_SendByte(0x40);
}

void Font_Type(UINT8 font)
{
    Printer_SendByte(0x1D);
    Printer_SendByte(0x21);
    Printer_SendByte(font);
    
}

void Justification(UINT8 jtype)
{
    Printer_SendByte(0x1B);
    Printer_SendByte(0x61);
    Printer_SendByte(jtype);
}

void Line_Space(UINT8 spacelines)
{
    Printer_SendByte(0x1B);
    Printer_SendByte(0x33);
    Printer_SendByte(spacelines);
}

void Start_Printer(void)
{
    UINT8 p_temp_char;

    Initialize_Thermal();

    Line_Space(30);

    Init_Regular_Font();

    SEERAMReadString(Header_Bold_Loc,1,(char *)&p_temp_char);

    if(p_temp_char=='Y')
        Font_Type(DoubleH);
    else
        Font_Type(Normal);

    Justification(CENTER);    
}

void Common_Underscore(UINT8 ctype)
{
    if(ctype=='T')
        PRINT_UNDERSCORE();

    if(ctype=='E')
        EXT_PRINT_UNDERSCORE(32);
}

void Print_From_To_Date(void)
{
    Read_RTC();

    STRCPY(PBUF,"Bill From: ");
    Copy_MonthRecDT(PBUF+11);
    Printer_SendString(0x04,32);
}

void Print_Report_Date_Time(void)
{
    ZERO=0;

    STRCPY(PBUF,"DATE:  /  /        TIME:  :  :  ");
    Copy_Date(PBUF+5);   
    Copy_Time(PBUF+24);
    Printer_SendString(0x03,32);
}

void Print_C_Month(UINT8 cmonth,UINT8 cyear,UINT8 printertype)
{
    STRCPY(PBUF,"   ");
    Printer_SendString('S',3);
                
    Init_Regular_Font();
   
    ZERO=0;
    Hex_DEC(0,cmonth,PBUF,2);
    PBUF[2]='/';
    Hex_DEC(0,cyear,PBUF+3,2);
    PBUF[5]=':';
   
    USB_Print_SendString(printertype,1,6);
    ZERO=1;
}

void Print_Date(UINT8 ptimeflag)
{
    ZERO=0;
    if(ptimeflag!='s')
        Read_RTC();

    STRCPY(PBUF,"  /  /                   ");
    Copy_ShiftDT(PBUF);
   
    if(ptimeflag=='C' || ptimeflag=='D')
    {
        
        Hex_DEC(0,CollectionRecord.WTIME[0] & 0x1F,PBUF+11,2);//sudha
        PBUF[13]=':';
        Hex_DEC(0,CollectionRecord.WTIME[1],PBUF+14,2);
        PBUF[16]=':';
        
        Hex_DEC(0,CollectionRecord.WTIME[2] & 0x7F,PBUF+17,2);
        
    }

    Day_Shift(PBUF+25);

    Printer_SendString(0x03,32);
}

void Header_Footer_Print(UINT8 printertype,UINT8 pstartline,UINT8 pendline)
{
    UINT32 h_b_cnt;
    UINT8 index_cnt;
    
    if(printertype=='E')
    {
        Ext_Print_Report_Date_Time();
        index_cnt=2;
    }
    if(printertype=='T')
    {
        index_cnt=0;
        if(pstartline==0)
        {
            Font_Type(Normal);
            Justification(LEFT);

            PRINT_UNDERSCORE();

            Print_Report_Date_Time();
        }
    }
    if(pstartline==0)
    {        
        SEERAMReadString(Dairy_Name_Loc,32,PBUF);

        STRCPY(SEND_BUF,"                                ");

        if(strncmp(SEND_BUF,PBUF,32)!=0)
        {
            USB_Print_SendString(printertype,1,32);          
        }   

        if(strncmp(SEND_BUF,SOCIETY_CODE,10)!=0)
        {            
            STRCPY(PBUF,"SOCIETY CODE:");
            STRNCPY(PBUF+13,SOCIETY_CODE,10);
            USB_Print_SendString(printertype,0,23);
        }
    }

    for(h_b_cnt=pstartline;h_b_cnt<pendline;h_b_cnt++)
    {
        EEPROM_ADDR=Society_Name1_Loc+(h_b_cnt*32);        
        
        SEERAMReadString(EEPROM_ADDR,32,PBUF);

        if(strncmp(SEND_BUF,PBUF,32)!=0)
        {
            if(printertype=='E' && h_b_cnt==0)
                USB_Print_SendString(printertype,1,32);
            else
                USB_Print_SendString(printertype,0,32);           
        }
    }
    if(printertype=='T' && pstartline<4)
        PRINT_UNDERSCORE();
}

void Print_Report_Header(UINT8 preportflag)
{
    Start_Printer();

    switch(preportflag)
    {
        case 'B':

            STRCPY(PBUF,"*BONUS REPORT*");
            break;

        case 'C':

            STRCPY(PBUF,"*MEMBER BILL*");
            break;

        case 'c':

            STRCPY(PBUF,"*MEMBERWISE REPORT*");
            break;

        case 'E':

            STRCPY(PBUF,"*DEDUCTION REPORT*");
            break;

        case 'L':
            
            STRCPY(PBUF,"*LOCAL MILK SALE REPORT(COW)*");
            Buf_Type(LocalSalesrec.MILK_TYPE,PBUF+24);

                
            break;
        case 'S':

            STRCPY(PBUF,"*PAYMENT REGISTER*");
            break;
            
        case 'A':

            STRCPY(PBUF,"*SANGH SALE REPORT(COW)*");
            Buf_Type(Sanghrec.MILK_TYPE,PBUF+19);
          
            break;
        case 'R':

            STRCPY(PBUF,"*SANGH SALE RECEIPT(COW)*");
            Buf_Type(Sanghrec.MILK_TYPE,PBUF+20);

            break;
    }

    Printer_SendString(0x04,29);

    Header_Footer_Print('T',0,1);
}

void Print_Header(UINT8 pheaderflag)
{
    Start_Printer();
   
    switch(pheaderflag)
    {
         case 'A':

            STRCPY(PBUF,"*ABSENT MEMBER REPORT*");
            break;

        case 'C':

            STRCPY(PBUF,"*COLLECTION RECEIPT-000*");
            Hex_DEC(0,WEIGHT_COUNT,PBUF+20,3);            
            break;

       case 'c':

            STRCPY(PBUF,"*DETAILED SHIFT REPORT*");
            break;

        case 'D':

            STRCPY(PBUF,"*DUPLICATE RECEIPT-000*");
            ZERO=0;
            Hex_DEC(0,CollectionRecord.SHIFT_RECORD_NO,PBUF+19,3);
            ZERO=1;
            break;

        case 'd':

            STRCPY(PBUF,"*DEDUCTION RECEIPT*");
            break;

        case 'I':

            STRCPY(PBUF,"*INACTIVE MEMBERS LIST*");
            break;

        case 'L':

            STRCPY(PBUF,"*LOCAL SALE RECEIPT*");
            break;

        case 'T':

            STRCPY(PBUF,"*MEMBERS LIST*");
            break;
    }
    Printer_SendString(0x04,29);
 
    Header_Footer_Print('T',0,4);

    if(pheaderflag=='C' || pheaderflag=='D' || pheaderflag=='d')
    {
        ZERO=0;
        Hex_DEC(0,CollectionRecord.CODE,PBUF,4);
        PBUF[4]=':';
        STRNCPY(PBUF+5,Memberrec.NAME,20);
        Printer_SendString(0x03,25);
    }

    if(pheaderflag!='I' && pheaderflag!='l')
        Print_Date(pheaderflag);
}

void Print_Incentive(UINT8 rprinttype,UINT32 incamt)
{
    ZERO=1;
   
    STRCPY(PBUF,"INCENTIVE      (Rs):    ");
    Hex_DEC(2,incamt,PBUF+24,8);
    
    if(rprinttype=='U')
        PBUF[20]=',';
    USB_Print_SendString(rprinttype,0,32);  
}

void Print_Col_Grand_Total(UINT8 rprinttype,UINT32 comamt)
{
    UINT32 col_amount;
    
    ZERO=1;
    col_amount=Calculate_Col_Amount(CollectionRecord.QTY,CollectionRecord.RATE);
   
    STRCPY(PBUF,"TOTAL AMOUNT   (Rs):    ");
    Hex_DEC(2,(col_amount+comamt),PBUF+24,8);
    USB_Print_SendString(rprinttype,1,32);
}

void Print_Receipt_Data(UINT8 rprintertype)
{
    UINT8 p_b_flag,manual_symbol;
    //UINT16 water,clr;
    UINT32 amount,col_Incentive_Amt=0;
    
    if(CollectionRecord.RATE!=0)
        col_Incentive_Amt=Read_Incentive_Comm(1,CollectionRecord);
    else
        col_Incentive_Amt=0;
    
    SEERAMReadString(Col_Manual_Msg_Symbol_Loc,1,(char *)&manual_symbol);
    
    SEERAMReadString(MilkType_Print_On_Loc,RECEIPT_RECORD_LENGTH,(char *)&Receiptrec);
    
    if(manual_symbol==0xFF)
        manual_symbol='M';
    
    Read_Collection_Flags();
    ZERO=1;
    STRCPY(PBUF,"MILK TYPE          :         COW");
    Buf_Type(CollectionFlags.MILK_TYPE,PBUF+29);
    
    if(Receiptrec.MILK_TYPE_PRINT=='Y')
    {
        USB_Print_SendString(rprintertype,0,32);
    }
    
    if(CollectionRecord.FAT != 0xFF && CollectionRecord.FAT != 0)
    {
        STRCPY(PBUF,"FAT             (%):        ");
        if((CollectionRecord.FLAGS | 0xF7)!=0xFF)
        {

            if(manual_symbol!='M')
            {
                //STRCPY(PBUF,"FAT.            (%):        ");
                PBUF[3]=manual_symbol;
            }
            else
                STRCPY(PBUF,"FAT (Manual)    (%):        ");//1    
                
        }
        Hex_DEC(1,CollectionRecord.FAT,PBUF+28,4);
          
        if(Receiptrec.FAT_PRINT=='Y')
            USB_Print_SendString(rprintertype,0,32);       

        if(CollectionRecord.SNF>0)
        {
            STRCPY(PBUF,"SNF             (%):        ");
           // CLR_Enabled(PBUF);
                       
            Hex_DEC(1,CollectionRecord.SNF,PBUF+28,4);

            if((CLR_BASED=='N' && Receiptrec.SNF_PRINT=='Y') || (CLR_BASED=='Y' && Receiptrec.CLR_PRINT=='Y'))
                USB_Print_SendString(rprintertype,0,32);  
        }
        //if(CollectionRecord.CLR>0 && CollectionRecord.CLR!=0xFF)
        if(CollectionRecord.CLR1>0 && CollectionRecord.CLR1!=0xFFFF)
        {
            STRCPY(PBUF,"CLR             (%):        ");           
         
            Hex_DEC(1,CollectionRecord.CLR1,PBUF+28,4);
             
            if(Receiptrec.CLR_PRINT=='Y')
                USB_Print_SendString(rprintertype,0,32);  
        }
        
        if(CollectionRecord.WATER1>0 && CollectionRecord.WATER1!=0xFFFF)
        {
            STRCPY(PBUF,"WATER           (%):        ");
            Hex_DEC(1,CollectionRecord.WATER1,PBUF+28,4);
            
            if(Receiptrec.WATER_PRINT=='Y')
                USB_Print_SendString(rprintertype,0,32);  
        }


    }
    if(Receiptrec.QTY_PRINT=='Y')
    {
        STRCPY(PBUF,"QTY           (Ltr):      ");
     
        if((CollectionRecord.FLAGS | 0xFB)!=0xFF)
        {
      
            
            if(manual_symbol!='M')
            {
                //STRCPY(PBUF,"QTY.          (Ltr):      ");
                PBUF[3]=manual_symbol;
            }
            else
                STRCPY(PBUF,"QTY (Manual)  (Ltr):      ");      
        }
            
        if(WEIGHT_KG=='Y')
            STRNCPY(PBUF+15,"Kgs",3);

        Hex_DEC(2,CollectionRecord.QTY,PBUF+26,6);
        USB_Print_SendString(rprintertype,0,32); 
    }
    amount=0;
    if(CollectionRecord.FAT!=0xFF && CollectionRecord.FAT != 0)
    {
        if(Receiptrec.RATE_PRINT=='Y')
        {
            STRCPY(PBUF,"RATE           (Rs):      ");
            Hex_DEC(2,CollectionRecord.RATE,PBUF+26,6);
            USB_Print_SendString(rprintertype,0,32); 
        }               
        
        if(rprintertype=='T')
            PRINT_UNDERSCORE();
        else
            EXT_PRINT_UNDERSCORE(32);

        amount=Calculate_Col_Amount(CollectionRecord.QTY,CollectionRecord.RATE);
     
        STRCPY(PBUF,"AMOUNT         (Rs):    ");
        Hex_DEC(2,amount,PBUF+24,8);
        
    }
    if(CollectionRecord.FAT != 0xFF && CollectionRecord.FAT != 0 && Receiptrec.AMOUNT_PRINT=='Y')        
    {
        if(rprintertype=='T')
        {
            SEERAMReadString(Amount_Bold_Loc,1,(char *)&p_b_flag);
            if(p_b_flag=='Y')
                Font_Type(DoubleH);
            else
                Font_Type(Normal);
        }
        if(col_Incentive_Amt==0)
            USB_Print_SendString(rprintertype,1,32); 
        else
            USB_Print_SendString(rprintertype,0,32); 
    }
    
    
    ZERO=0;
    Font_Type(Normal);
    
    if(col_Incentive_Amt>0 && Receiptrec.INCENTIVE_PRINT=='Y')
    {
        Print_Incentive(rprintertype,col_Incentive_Amt);
        if(Receiptrec.AMOUNT_PRINT=='Y')
        {
            if(rprintertype=='T')
                PRINT_UNDERSCORE();   
            else
                EXT_PRINT_UNDERSCORE(32);

            Print_Col_Grand_Total(rprintertype,col_Incentive_Amt);  
        }
    }
    if((Receiptrec.AMOUNT_PRINT=='Y' && amount>0 ) || (Receiptrec.INCENTIVE_PRINT=='Y' && col_Incentive_Amt>0))
    {
        if(rprintertype=='T')
            PRINT_UNDERSCORE();
        else
            EXT_PRINT_UNDERSCORE(32);
    }
}

void Print_Receipt(UINT8 pcollecttype)
{
    Print_LineNo=0;
    Print_Header(pcollecttype);
    PRINT_UNDERSCORE();    

    Print_Receipt_Data('T');

    Header_Footer_Print('T',4,5);//6);

    Paper_Feed(2);
}

void Print_Deduction_Data(UINT8 dprintertype,UINT32 pdeductionamt)
{
    ZERO=1;

    STRCPY(PBUF,"DEDUCTION:  ");
    STRNCPY(PBUF+12,DED_BUF,20);
    USB_Print_SendString(dprintertype,0,32);

    if((Deductionrec.AMT-pdeductionamt)>0)
    {
        STRCPY(PBUF,"PREVIOUS  AMOUNT   :Rs.  ");
        Hex_DEC(2,Deductionrec.AMT-pdeductionamt,PBUF+25,7);
        USB_Print_SendString(dprintertype,0,32);
    }

    STRCPY(PBUF,"DEDUCTION AMOUNT   :Rs.   ");
    Hex_DEC(2,pdeductionamt,PBUF+25,7);    
    USB_Print_SendString(dprintertype,0,32);

    if((Deductionrec.AMT-pdeductionamt)>0)
    {
        if(dprintertype=='T')
            PRINT_UNDERSCORE();
        else
            EXT_PRINT_UNDERSCORE(38);

        STRCPY(PBUF,"TOTAL DEDUCTION AMT:Rs. ");
        Hex_DEC(2,Deductionrec.AMT,PBUF+24,8);
        USB_Print_SendString(dprintertype,1,32);
    }
    if(dprintertype=='T')
        PRINT_UNDERSCORE();
    else
        EXT_PRINT_UNDERSCORE(38);
}

void Print_Deduction_Receipt(UINT32 pdeductionamt)
{
    Print_LineNo=0;
    CollectionRecord.CODE=Memberrec.C_CODE;
    Print_Header('d');
    PRINT_UNDERSCORE();

    Print_Deduction_Data('T',pdeductionamt);
    Paper_Feed(2);
}

void Print_LocalSale_Receipt(UINT32 lsqty,UINT16 lsrate,UINT32 lsamount)
{
    Print_LineNo=0;
    
    Print_Header('L');

    ZERO=1;

    STRCPY(PBUF,"MILK  QUANTITY    RATE  AMOUNT(Rs)");
    Printer_SendString(0x03,32);

    PRINT_UNDERSCORE();
   
    STRCPY(PBUF,"COW   ");
    Buf_Type(LocalSalesrec.MILK_TYPE,PBUF);

    Hex_DEC(2,lsqty,PBUF+6,8);
    PBUF[14]=' ';
    Hex_DEC(2,lsrate,PBUF+15,7);
    PBUF[22]=' ';
    Hex_DEC(2,lsamount,PBUF+23,9);
    Printer_SendString(0x03,32);

    PRINT_UNDERSCORE();
    Paper_Feed(2);
}		

void Calculate_Avg_Fat_Snf_Rate(void)
{
    if(Cumrec.C_MEMBERS>0 && Cumrec.C_QTY>0)
    {
        Cumrec.C_FAT=(Cumrec.C_FAT*10)/Cumrec.C_QTY;
        Cumrec.C_SNF=Cumrec.C_SNF*10/Cumrec.C_QTY;
        Cumrec.C_CLR=Cumrec.C_CLR*10/Cumrec.C_QTY;
        Cumrec.C_RATE=Cumrec.C_RATE/Cumrec.C_QTY;//*10
    }
    if(Cumrec.B_MEMBERS>0 && Cumrec.B_QTY>0)
    {
        Cumrec.B_FAT=Cumrec.B_FAT*10/Cumrec.B_QTY;
        Cumrec.B_SNF=Cumrec.B_SNF*10/Cumrec.B_QTY;
        Cumrec.B_CLR=Cumrec.B_CLR*10/Cumrec.B_QTY;
        Cumrec.B_RATE=Cumrec.B_RATE/Cumrec.B_QTY;//*10
    }    
}

void Print_All_Totals(UINT8 ptype,UINT16 asamples,UINT32 aqty,UINT32 atotalamt,UINT32 incentamount,INT32 netamt)
{
    ZERO=1;
    if(asamples>0)
    {
        STRCPY(PBUF,"TOTAL SAMPLES     :         "); 
        Hex_DEC(0,(Cumrec.C_MEMBERS+Cumrec.B_MEMBERS),PBUF+28,4);
        if(ptype=='U')
        {
            PBUF[19]=',';    
            PBUF[20]=',';  
        }
        USB_Print_SendString(ptype,0,32);
    }
     
    STRCPY(PBUF,"TOTAL QUANTITY(Lt):   ");
    if(WEIGHT_KG=='Y')
    {
        PBUF[15]='K';
        PBUF[16]='g';
    }
    
    Hex_DEC(2,aqty,PBUF+22,10);
    if(ptype=='U')
    {
        PBUF[19]=',';    
        PBUF[20]=',';  
    }
    USB_Print_SendString(ptype,0,32);

    STRCPY(PBUF,"AMOUNT        (Rs):   ");
    Hex_DEC(2,atotalamt,PBUF+21,11);
    if(ptype=='U')
    {
        PBUF[19]=',';    
        PBUF[20]=',';  
    }
    USB_Print_SendString(ptype,0,32);

    if(incentamount>0)
    {
        STRCPY(PBUF,"INCENTIVE AMT (Rs):   ");
        Hex_DEC(2,incentamount,PBUF+22,10);
        if(ptype=='U')
        {
            PBUF[19]=',';    
            PBUF[20]=',';  
        }
        USB_Print_SendString(ptype,0,32);
       
        STRCPY(PBUF,"TOTAL AMOUNT  (Rs):   ");
        Hex_DEC(2,atotalamt+incentamount,PBUF+21,11);
        if(ptype=='U')
        {
            PBUF[19]=',';    
            PBUF[20]=',';  
        }
        USB_Print_SendString(ptype,0,32);
    }
    if(netamt!=0)
    {
        if(netamt>0)
        {
            STRCPY(PBUF,"NET AMOUNT    (Rs):   ");
            Hex_DEC(2,netamt,PBUF+21,11);

        }
        else
        {
            netamt=(-1)*netamt;

            STRCPY(PBUF,"DUE AMOUNT    (Rs):-   ");
            Hex_DEC(2,netamt,PBUF+23,9);
        }
        if(ptype=='U')
        {
            PBUF[19]=',';    
            PBUF[20]=',';  
        }
        USB_Print_SendString(ptype,0,32);
    }

    Common_Underscore(ptype);     
}

void Print_Average_Total(UINT8 ptype)//
{
    UINT8 netflag=0;
    UINT32 netamtc=0,netpaidc=0,netamtb=0,netpaidb=0;
    
    if(ptype=='M')
        ptype='T';
    
    else if(ptype=='m')
        ptype='E';
    ZERO=1;

    if(Cumrec.C_MEMBERS>0 || Cumrec.B_MEMBERS>0)
    {
        if(ptype=='U')
        {
            STRCPY(PBUF,"          ,      BUF,        COW");
        }
        else
        {
            STRCPY(PBUF,"                  BUF        COW");
        }
        USB_Print_SendString(ptype,0,32);
        Common_Underscore(ptype);
        
        STRCPY(PBUF,"Samples   :                     ");

        if(Cumrec.B_MEMBERS>0)
            Hex_DEC(0,Cumrec.B_MEMBERS,PBUF+18,3);
        if(ptype=='U')
        {
            PBUF[10]=',';
            PBUF[28]=',';
        }
            
        if(Cumrec.C_MEMBERS>0)
            Hex_DEC(0,Cumrec.C_MEMBERS,PBUF+29,3);
        USB_Print_SendString(ptype,0,32);

        Calculate_Avg_Fat_Snf_Rate();
        
        if(Cumrec.B_FAT>0 || Cumrec.C_FAT>0)
        {
            STRCPY(PBUF,"Avg. FAT  :                     ");

            if(Cumrec.B_MEMBERS>0)
                Hex_DEC(2,Cumrec.B_FAT,PBUF+16,5);
            if(ptype=='U')
            {
                PBUF[10]=',';
                PBUF[26]=',';
            }

            if(Cumrec.C_MEMBERS>0)
                Hex_DEC(2,Cumrec.C_FAT,PBUF+27,5);
            USB_Print_SendString(ptype,0,32);
        }
        if(CLR_BASED!='Y')
        {
            if(Cumrec.B_SNF>0 || Cumrec.C_SNF>0)
            {
                STRCPY(PBUF,"Avg. SNF  :                     ");
                CLR_Enabled(PBUF+5);

                if(Cumrec.B_MEMBERS>0)
                    Hex_DEC(2,Cumrec.B_SNF,PBUF+16,5);
                if(ptype=='U')
                {
                    PBUF[10]=',';
                    PBUF[26]=',';
                }

                if(Cumrec.C_MEMBERS>0)
                    Hex_DEC(2,Cumrec.C_SNF,PBUF+27,5);

                USB_Print_SendString(ptype,0,32);
            }
        }
        else
        {
            if(Cumrec.B_CLR>0 || Cumrec.C_CLR>0)
            {
                STRCPY(PBUF,"Avg. CLR  :                     ");
                CLR_Enabled(PBUF+5);

                if(Cumrec.B_MEMBERS>0)
                    Hex_DEC(2,Cumrec.B_CLR,PBUF+16,5);
                if(ptype=='U')
                {
                    PBUF[10]=',';
                    PBUF[26]=',';
                }

                if(Cumrec.C_MEMBERS>0)
                    Hex_DEC(2,Cumrec.C_CLR,PBUF+27,5);

                USB_Print_SendString(ptype,0,32);

            }
        }
             
        STRCPY(PBUF,"Total Qty :                     ");

        if(Cumrec.B_MEMBERS>0)
            Hex_DEC(2,Cumrec.B_QTY,PBUF+11,10);
        if(ptype=='U')
        {
            PBUF[10]=',';
            PBUF[21]=',';
        }
        if(Cumrec.C_MEMBERS>0)
            Hex_DEC(2,Cumrec.C_QTY,PBUF+22,10);
        USB_Print_SendString(ptype,0,32);        
       
        STRCPY(PBUF,"Avg. Rate :                     ");

        if(Cumrec.B_MEMBERS>0)
            Hex_DEC(2,Cumrec.B_RATE,PBUF+15,6);
        if(ptype=='U')
        {
            PBUF[10]=',';
            PBUF[25]=',';
        }
        if(Cumrec.C_MEMBERS>0)
            Hex_DEC(2,Cumrec.C_RATE,PBUF+26,6);
        USB_Print_SendString(ptype,0,32);
       
        STRCPY(PBUF,"Amount    :                     ");

        if(Cumrec.B_MEMBERS>0)
            Hex_DEC(2,Cumrec.B_AMOUNT,PBUF+11,10);
        if(ptype=='U')
        {
            PBUF[12]=',';
            PBUF[21]=',';
        }
        if(Cumrec.C_MEMBERS>0)
            Hex_DEC(2,Cumrec.C_AMOUNT,PBUF+22,10);
        USB_Print_SendString(ptype,0,32);
        if((Cumrec.C_INC_AMT+Cumrec.B_INC_AMT)>0)   
        {
            STRCPY(PBUF,"Incentive :                 ");
           
            if(Cumrec.B_INC_AMT>0)
            {
                Hex_DEC(2,Cumrec.B_INC_AMT,PBUF+11,10);
            }

             if(ptype=='U')
            {
                PBUF[10]=',';
                PBUF[21]=',';
            }
            if(Cumrec.C_INC_AMT>0)
            {
                Hex_DEC(2,Cumrec.C_INC_AMT,PBUF+22,10);
            }
            USB_Print_SendString(ptype,0,32);
        
            STRCPY(PBUF,"Total Amt :                 ");
            
            if(Cumrec.B_INC_AMT+Cumrec.B_AMOUNT>0)
            {
                Hex_DEC(2,Cumrec.B_INC_AMT+Cumrec.B_AMOUNT,PBUF+11,10);
                
            }

            if(ptype=='U')
            {
                PBUF[10]=',';
                PBUF[21]=',';
            }
            if(Cumrec.C_INC_AMT+Cumrec.C_AMOUNT>0){
                Hex_DEC(2,Cumrec.C_INC_AMT+Cumrec.C_AMOUNT,PBUF+22,10);
            }
            USB_Print_SendString(ptype,0,32);
        }
    }
    if(Cumrec.C_PAID_AMOUNT+Cumrec.B_PAID_AMOUNT>0)
    {
        netflag=1;
        STRCPY(PBUF,"Paid Amt  :                 ");
        Hex_DEC(2,Cumrec.B_PAID_AMOUNT,PBUF+11,10);

         if(ptype=='U')
        {
            PBUF[10]=',';
            PBUF[21]=',';
        }
     
         Hex_DEC(2,Cumrec.C_PAID_AMOUNT,PBUF+22,10);
        USB_Print_SendString(ptype,0,32);
    }
    if(Cumrec.C_DEDUCTION+Cumrec.B_DEDUCTION>0)
    {
        netflag=1;
        STRCPY(PBUF,"Deductions:                     ");
        Hex_DEC(2,Cumrec.B_DEDUCTION,PBUF+11,10);

         if(ptype=='U')
        {
            PBUF[10]=',';
            PBUF[21]=',';
        }
       
        Hex_DEC(2,Cumrec.C_DEDUCTION,PBUF+22,10);
        USB_Print_SendString(ptype,0,32);
    }
    if(netflag==1)
    {
        netamtb=Cumrec.B_AMOUNT+Cumrec.B_INC_AMT;
        netpaidb=Cumrec.B_DEDUCTION+Cumrec.B_PAID_AMOUNT;

        STRCPY(PBUF,"Net Amount:                 ");
        if(netamtb>=netpaidb)
        {
            Hex_DEC(2,netamtb-netpaidb,PBUF+11,10);
        }
        else
        {
            PBUF[11]='-';
            Hex_DEC(2,netpaidb-netamtb,PBUF+12,9);
        }
        if(ptype=='U')
        {
            PBUF[10]=',';
            PBUF[21]=',';
        }
        netamtc=Cumrec.C_AMOUNT+Cumrec.C_INC_AMT;
        netpaidc=Cumrec.C_DEDUCTION+Cumrec.C_PAID_AMOUNT;

        if(netamtc>=netpaidc)
        {
            Hex_DEC(2,netamtc-netpaidc,PBUF+22,10);
        }
        else
        {
            PBUF[22]='-';
            Hex_DEC(2,netpaidc-netamtc,PBUF+23,9);

        }

        USB_Print_SendString(ptype,0,32);

    }
    Common_Underscore(ptype);  
    if((Cumrec.C_MEMBERS+Cumrec.B_MEMBERS)>0)
    {
        Print_All_Totals(ptype,(Cumrec.C_MEMBERS+Cumrec.B_MEMBERS),(Cumrec.C_QTY+Cumrec.B_QTY),(Cumrec.C_AMOUNT+Cumrec.B_AMOUNT),(Cumrec.C_INC_AMT+Cumrec.B_INC_AMT),(netamtc+netamtb-netpaidc-netpaidb));
    }
}

void Calc_Cum_Avg(void)
{
    AVG_FAT_CALC=CollectionRecord.QTY;
    AVG_FAT_CALC=AVG_FAT_CALC*CollectionRecord.FAT;

    AVG_SNF_CALC=CollectionRecord.QTY;
    AVG_SNF_CALC=AVG_SNF_CALC*CollectionRecord.SNF;
    
    AVG_CLR_CALC=CollectionRecord.QTY;
    AVG_CLR_CALC=AVG_CLR_CALC*CollectionRecord.CLR1;

    AVG_RATE_CALC=CollectionRecord.RATE;
    AVG_RATE_CALC=AVG_RATE_CALC*CollectionRecord.QTY;

    if(CollectionFlags.MILK_TYPE=='C')
    {
        Cumrec.C_FAT=Cumrec.C_FAT+AVG_FAT_CALC;
        Cumrec.C_SNF=Cumrec.C_SNF+AVG_SNF_CALC;
        Cumrec.C_CLR=Cumrec.C_CLR+AVG_CLR_CALC;
        Cumrec.C_RATE=Cumrec.C_RATE+AVG_RATE_CALC;
        Cumrec.C_QTY=Cumrec.C_QTY+CollectionRecord.QTY;
        
        Cumrec.C_AMOUNT=Cumrec.C_AMOUNT+Amount;
        Cumrec.C_INC_AMT=Cumrec.C_INC_AMT+CollectionRecord.INCENTIVE_AMT;

        Cumrec.C_MEMBERS++;
    }
    if(CollectionFlags.MILK_TYPE=='B')
    {
        Cumrec.B_FAT=Cumrec.B_FAT+AVG_FAT_CALC;
        Cumrec.B_SNF=Cumrec.B_SNF+AVG_SNF_CALC;
        Cumrec.B_CLR=Cumrec.B_CLR+AVG_CLR_CALC;
        Cumrec.B_RATE=Cumrec.B_RATE+AVG_RATE_CALC;
        Cumrec.B_QTY=Cumrec.B_QTY+CollectionRecord.QTY;
        Cumrec.B_AMOUNT=Cumrec.B_AMOUNT+Amount;
        Cumrec.B_INC_AMT=Cumrec.B_INC_AMT+CollectionRecord.INCENTIVE_AMT;

        Cumrec.B_MEMBERS++;
    }
}

void Print_ShiftData_Record(UINT8 printtype)
{
    UINT8 font_type=0x03,font_length=32;
    ZERO=1;
    
    Hex_DEC(0,CollectionRecord.CODE,PBUF,4);
    
    if(CollectionRecord.FAT!=0xFF)
    {
        PBUF[4]=Memberrec.MILK_TYPE;
        Shift_Left(PBUF,5);
        
        Hex_DEC(1,CollectionRecord.FAT,PBUF+5,4);
        PBUF[9]=' ';
        if((CollectionRecord.FLAGS | 0xF7)!=0xFF)
        {
            PBUF[9]='M';
        }
        if(CLR_BASED == 'Y')
            Hex_DEC(1,CollectionRecord.CLR1,PBUF+10,4);
        else
            Hex_DEC(1,CollectionRecord.SNF,PBUF+10,4);
    }
    else
    {
        Shift_Left(PBUF,4);
        STRCPY(PBUF+4,"  Pending ");
        PENDING_SAMPLES_CNT=PENDING_SAMPLES_CNT+1;
    }
    PBUF[14]=' ';
    if((CollectionRecord.FLAGS | 0xFB)!=0xFF)
    {
        PBUF[14]='M';
    }
    
    if(CollectionRecord.QTY>9999)
        Hex_DEC(2,CollectionRecord.QTY,PBUF+15,6);
    else
        Hex_DEC(2,CollectionRecord.QTY,PBUF+15,5);

    PBUF[20]=' ';
    if(CollectionRecord.FAT!=0xFF)
    {
        Hex_DEC(2,CollectionRecord.RATE,PBUF+21,5);

        if(IncentiveReportFlag==1 || printtype=='E')
        {
            
            Hex_DEC(2,CollectionRecord.INCENTIVE_AMT,PBUF+26,7);
            PBUF[33]=' ';
            Hex_DEC(2,Amount+CollectionRecord.INCENTIVE_AMT,PBUF+34,8);
            font_type='S';
            font_length=42;
        }
        else
        {
            Hex_DEC(2,CollectionRecord.RATE,PBUF+21,4);
            if(CollectionRecord.RATE>999)
                Hex_DEC(2,CollectionRecord.RATE,PBUF+21,5);
        
            if(CollectionRecord.RATE>9999)
                Hex_DEC(2,CollectionRecord.RATE,PBUF+20,6);
            
            Hex_DEC(2,Amount+CollectionRecord.INCENTIVE_AMT,PBUF+25,7);
            if((Amount+CollectionRecord.INCENTIVE_AMT)>99999)
                Hex_DEC(2,Amount+CollectionRecord.INCENTIVE_AMT,PBUF+25,8);
           
        }
        
        Calc_Cum_Avg();
    }
    else
        STRCPY(PBUF+21," Pending    ");
    if(printtype=='E')
    {
        
        USB_Print_SendString('E',0,42);
    }
    else
        Printer_SendString(font_type,font_length);
}

void Print_Shift_Report_Start(void)
{
    UINT8 font_type=0x03,font_length=32;
    
    Print_Header('c');

    if(IncentiveReportFlag==1)
    {
        STRCPY(PBUF,"CODE  FAT  SNF  QTY(L) RATE INCENT. AMOUNT");
        font_type='S';
        font_length=42;
        if(WEIGHT_KG=='Y')
        {
           PBUF[20]='K';
        }
    }
    else
    {
        STRCPY(PBUF,"CODE  FAT  SNF QTY(L) RATE   AMT");
        if(WEIGHT_KG=='Y')
        {
           PBUF[19]='K';
        }
        
    }
    CLR_Enabled(PBUF+11);   
    
    Printer_SendString(font_type,font_length);

    PRINT_UNDERSCORE();
}
  
void Print_Shift_Report(UINT8 sprinttype)
{
    UINT8 print_start=0;
    UINT16 temp_rec_cnt,norec,ccode;

    Print_LineNo=0;
    Init_Variables();
    Init_Cum_Values();

    Disp_Checking_Data(0);
    PENDING_SAMPLES_CNT=0;

    temp_rec_cnt=0;
    norec=DynamicRead_BillDetails(10000);
   
    for(ccode=Fromrec.START_MEMBER;ccode<=Fromrec.END_MEMBER;ccode++)
    {
        temp_rec_cnt=0;
        while(temp_rec_cnt<norec)
        {
            CollectionRecord=CollectionRecords[temp_rec_cnt];
            if(CollectionRecord.CODE==ccode || (TIMEWISE==1 && CollectionRecord.CODE>=Fromrec.START_MEMBER && CollectionRecord.CODE<=Fromrec.END_MEMBER ))
            {
                Read_Collection_Flags();
                if(print_start==0)
                {
                    print_start=1;
                    if(sprinttype=='E')
                    {
                        if (!Initialize_Printer())
                            return;

                        Ext_Print_Shift_Report_Start();
                    }
                    else
                        Print_Shift_Report_Start();
                }

                Memberrec.MILK_TYPE=CollectionFlags.MILK_TYPE;
                Calculate_Amount();
                
                if(sprinttype=='E' || (sprinttype==0 || (sprinttype==1 && CollectionRecord.FAT==0xFF)))
                    Print_ShiftData_Record(sprinttype);
            }
            temp_rec_cnt++;
            
        }
        if(TIMEWISE==1)
                ccode=MAX_CODE+1;
    }
    
    if(print_start==1)
    {   
        if(sprinttype=='E')
            EXT_PRINT_UNDERSCORE(42);
        else
            PRINT_UNDERSCORE();

        if(sprinttype==0 || sprinttype=='E')
        {
            if(sprinttype=='E')
                Print_Average_Total('E');
            else
                Print_Average_Total('T');
        }

        if(PENDING_SAMPLES_CNT>0)
        {
            ZERO=0;
            STRCPY(PBUF,"PENDING SAMPLES:");
            Hex_DEC(0,PENDING_SAMPLES_CNT,PBUF+16,4);
            if(sprinttype=='E')
            {
                Ext_Printer_SendString();
                EXT_PRINT_UNDERSCORE(42);
            }
            else
            {
                Printer_SendString(0x03,20);
                PRINT_UNDERSCORE();
            }
        }
        if(sprinttype=='E')
            Ext_Printer_Stop();
        else
            Paper_Feed(2);        
    }
    else
        no_data();

    ZERO=0;
}

void PRINT_COMMON_UNDERSCORE(UINT8 ctype)
{
    if(ctype=='T')
        PRINT_UNDERSCORE();

    if(ctype==80)
        EXT_PRINT_UNDERSCORE(80);

    if(ctype==40 || ctype=='E')
        EXT_PRINT_UNDERSCORE(46);
}

void Print_Milk_Members_Total(UINT8 ptype,UINT8 reptype,UINT8 mtype,UINT16 members)
{
    if(reptype=='A')
    {
        STRCPY(PBUF,"ABSENT COW MEMBERS  :  0000/0000");
        Buf_Type(mtype,PBUF+7);
        Hex_DEC(0,members,PBUF+23,4);
    }
    else if(reptype=='I')
    {
        STRCPY(PBUF,"INACTIVE COW MEMBERS:  0000/0000");
        Buf_Type(mtype,PBUF+9);
        Hex_DEC(0,members,PBUF+23,4);
    }
    else
    {
        STRCPY(PBUF,"TOTAL COW MEMBERS   :       0000");
        Buf_Type(mtype,PBUF+6);
    }
    if(mtype=='C')
        Hex_DEC(0,Cumrec.C_MEMBERS,PBUF+28,4);
    else
        Hex_DEC(0,Cumrec.B_MEMBERS,PBUF+28,4);
    Shift_Left(PBUF+28,4);
        
    USB_Print_SendString(ptype,0,32);
}

void Print_Members_Total(UINT8 ptype,UINT8 rtype,UINT16 cmem,UINT16 bmem)
{
    PRINT_COMMON_UNDERSCORE(ptype);

    if(cmem>0)
    {
        Print_Milk_Members_Total(ptype,rtype,'C',cmem);
    }

    if(bmem>0)
    {
        Print_Milk_Members_Total(ptype,rtype,'B',bmem);

        PRINT_COMMON_UNDERSCORE(ptype);

        if(rtype=='A')
        {
            STRCPY(PBUF,"TOTAL ABSENT MEMBERS:  0000/0000");
            Hex_DEC(0,cmem+bmem,PBUF+23,4);
        }
        else if(rtype=='I')
        {
            STRCPY(PBUF,"INACTIVE MEMBERS TOT:  0000/0000");
            Hex_DEC(0,cmem+bmem,PBUF+23,4);
        }
        else
            STRCPY(PBUF,"TOTAL  MEMBERS      :           ");
        Hex_DEC(0,Cumrec.C_MEMBERS+Cumrec.B_MEMBERS,PBUF+28,4);      
        Shift_Left(PBUF+28,4);
        USB_Print_SendString(ptype,1,32);
    }

    PRINT_COMMON_UNDERSCORE(ptype);
    if(ptype=='T')    
        Paper_Feed(3);
    else
        Ext_Printer_Stop();
}

void Print_Member_List_Start(UINT8 listtype)
{
    Print_Header(listtype);

    STRCPY(PBUF,"MEMBERS FROM:0001  To  0001");//1
    Hex_DEC(0,Fromrec.START_MEMBER,PBUF+13,4);
    Hex_DEC(0,Fromrec.END_MEMBER,PBUF+23,4);
    Printer_SendString(0x03,27);

    if(listtype=='A')
        PRINT_UNDERSCORE();
    
    STRCPY(PBUF,"S.No CODE MILK STA  MEMBER NAME");

    if(listtype=='A' || listtype=='I')
        STRCPY(PBUF,"S.No CODE MILK  MEMBER NAME    ");

    Printer_SendString(0x03,31);
    PRINT_UNDERSCORE();
}

void Print_Member_List_Record(UINT8 listtype,UINT16 psno)
{
    Hex_DEC(0,psno,PBUF,4);
    Shift_Left(PBUF,4);
    PBUF[4]=' ';
    Hex_DEC(0,Memberrec.C_CODE,PBUF+5,4);
    Shift_Left(PBUF+5,4);
    PBUF[9]=' ';

    STRCPY(PBUF+10,"COW");   
    Buf_Type(Memberrec.MILK_TYPE,PBUF+10);
        
    PBUF[13]=' ';
    if(listtype=='T')
        PBUF[14]=Memberrec.STATUS;
    else
        PBUF[14]=' ';
    PBUF[15]=' ';
    STRNCPY(PBUF+16,Memberrec.NAME,16);

    Printer_SendString(0x03,32);
}

void Print_Member_List(UINT8 ptype,UINT8 listtype)
{
    UINT16 p_cow_mem,p_buf_mem,p_s_no,p_member_sno;
    UINT8 print_start=0;
    UINT16 noofrecords;
    UINT16 p_member_code,tempcount=0;
    UINT8 absentflag;

    Print_LineNo=0;
    p_member_code=1;
    p_s_no=1;
    Cumrec.C_MEMBERS=0;
    Cumrec.B_MEMBERS=0;
    p_cow_mem=0;
    p_buf_mem=0;

    Disp_Checking_Data(0);
    
    if(listtype=='A')
        noofrecords=DynamicRead_BillDetails(10000);
    
    for(p_member_code=Fromrec.START_MEMBER;p_member_code<=Fromrec.END_MEMBER;p_member_code++)
    {
        p_member_sno=Read_Member_SNO(p_member_code);
        Disp_Checking_Data(p_member_code);

        if(p_member_sno!=0) 
        {            
           Read_Member_Details(p_member_sno);
           absentflag=0;
           
            if(listtype=='A')
            {
                if(noofrecords>0)
                {
                    for(tempcount=0;tempcount<noofrecords;tempcount++)
                    {
                        CollectionRecord=CollectionRecords[tempcount];
                        if(CollectionRecord.CODE==p_member_code)
                        {
                            absentflag=1;
                            break;
                        }
                    }
                }           
            }

            if((Memberrec.STATUS=='D' && listtype=='I') || (Memberrec.STATUS=='A' && (listtype=='A' && absentflag==0)) || listtype=='T')
            {
                if(print_start==0)
                {
                    if(ptype=='T')
                        Print_Member_List_Start(listtype);
                    else
                    {
                        if(!Initialize_Printer())
                            return;
                        Ext_Print_Member_List_Start(listtype);
                    }
                    print_start=1;
                }
                ZERO=1;
                if(ptype=='T')
                    Print_Member_List_Record(listtype,p_s_no);
                else
                    Ext_Print_Member_List_Record(p_s_no);
                
                if(Memberrec.MILK_TYPE=='C')
                    p_cow_mem=p_cow_mem+1;
                
                else
                    p_buf_mem=p_buf_mem+1;
                
                p_s_no=p_s_no+1;
            }
            if(Memberrec.STATUS=='A' || listtype=='T' || listtype=='I')
            {
                if(Memberrec.MILK_TYPE=='C')
                    Cumrec.C_MEMBERS++;
                else
                    Cumrec.B_MEMBERS++;
            }
        }
    }

    if(print_start==1)
        Print_Members_Total(ptype,listtype,p_cow_mem,p_buf_mem);
    else
        no_data();
}

void Print_Grand_Total(UINT8 ptype)
{    
    if((TOTAL_QTY_COW+TOTAL_QTY_BUF)>0)
    {
        PRINT_COMMON_UNDERSCORE(ptype);
    }

    ZERO=1;
    if(TOTAL_QTY_COW>0)
    {
        STRCPY(PBUF,"MILK   QUANTITY(L)  TOTAL AMOUNT");
        if(WEIGHT_KG=='Y')
            PBUF[16]='K';
        USB_Print_SendString(ptype,1,32);
        if(ptype=='T')
        {
            PRINT_UNDERSCORE();
        }
        else
        {
            EXT_PRINT_UNDERSCORE(32);
        }
        STRCPY(PBUF,"COW                   ");
        
        Hex_DEC(2,TOTAL_QTY_COW,PBUF+8,10);
        Hex_DEC(2,TOTAL_AMT_COW,PBUF+22,10);       
        USB_Print_SendString(ptype,1,32);
    }
    if(TOTAL_QTY_BUF>0)
    {
        STRCPY(PBUF,"BUF                   ");
        
        Hex_DEC(2,TOTAL_QTY_BUF,PBUF+8,10);
        Hex_DEC(2,TOTAL_AMT_BUF,PBUF+22,10);       
        USB_Print_SendString(ptype,1,32);
    }
        
    if(TOTAL_QTY>0)
    {
        if(ptype=='T')
            PRINT_UNDERSCORE();
        else
            EXT_PRINT_UNDERSCORE(32);
        
        STRCPY(PBUF,"TOTAL:                  ");     
        Hex_DEC(2,TOTAL_QTY,PBUF+8,11);       
        Hex_DEC(2,TOTAL_AMT,PBUF+21,11);  
        USB_Print_SendString(ptype,1,32);
    }

    ZERO=0;
    if(ptype=='T')
    {
        PRINT_UNDERSCORE();
        Paper_Feed(2);
    }
    else
    {
        EXT_PRINT_UNDERSCORE(46);
        Ext_Printer_Stop();
    }
}


void Print_Local_Sale_Report(UINT8 ptype,UINT8 lmtype)
{
    UINT8 print_start=0;

    Print_LineNo=0;
    Init_Variables();
   
    Set_Initial_Dates();

    while(Fetch_LocalSales_Record('L',lmtype))
    {
        if(LocalSalesrec.QTY>0 && LocalSalesrec.MILK_TYPE==lmtype)
        {
            if(print_start==0)
            {
                if(ptype=='T')
                {
                    Print_Report_Header('L');

                    Print_From_To_Date();    //1
                }
                else
                {
                    if (!Initialize_Printer())
                    return;

                    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_JOB_START, USB_NULL, 0, 0 );

                    STRCPY(PBUF," COW LOCAL SALE REPORT");
                    Buf_Type(LocalSalesrec.MILK_TYPE,PBUF+1);
                 
                    EXT_Report_Header(32);
                    EXT_PRINT_UNDERSCORE(32);
                }
                STRCPY(PBUF,"DATE  SHIFT  QTY(L)   AMOUNT(Rs)");
                if(WEIGHT_KG=='Y')
                {
                    PBUF[17]='K';
                }
                USB_Print_SendString(ptype,1,32);
                PRINT_COMMON_UNDERSCORE(ptype);
                print_start=1;
            }
            ZERO=0;
            STRCPY(PBUF,"                         ");
            Copy_ShiftDT(PBUF);
                 
            PBUF[9]='(';
            PBUF[10]=Shiftrec.DAY_SHIFT;
            PBUF[11]=')';
            PBUF[12]=' ';
            ZERO=1;

            Hex_DEC(2,LocalSalesrec.QTY,PBUF+13,8);
            Shift_Left(PBUF+13,8);
            PBUF[21]=' ';
            Hex_DEC(2,LocalSalesrec.AMOUNT,PBUF+22,10);

            if(LocalSalesrec.QTY>0 && LocalSalesrec.QTY!=0xFFFFFFFF)
            {
                USB_Print_SendString(ptype,0,32);

                TOTAL_QTY=TOTAL_QTY+LocalSalesrec.QTY;
                TOTAL_AMT=TOTAL_AMT+LocalSalesrec.AMOUNT;
            }
        }
    }
    if(print_start==1)
    {
        Print_Grand_Total(ptype);//1        
        ZERO=0;
    }
    else
        no_data();
}

void Print_Sangh_Sale_Report(UINT8 sanghflag,UINT8 ptype,UINT8 smtype)
{
    UINT8 print_start=0;

    Print_LineNo=0;
    Init_Variables();
    
    if(sanghflag=='A')
        Set_Initial_Dates();
    while(1)
    {
        if(sanghflag=='A')
        {
            if(Fetch_LocalSales_Record('S',smtype)==0)
                break;
        }
        ZERO=0;

        if(Sanghrec.QTY!=0)
        {
            if(print_start==0)
            {
                if(ptype=='T')
                {
                    Print_Report_Header(sanghflag);
                    if(sanghflag=='A' )
                    Print_From_To_Date();
                }
                else
                {
                    if (!Initialize_Printer())
                        return;
                    
                    USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_JOB_START, USB_NULL, 0, 0 );

                    STRCPY(PBUF," COW SANGH SALE REPORT");
                    if(sanghflag=='R' )
                        STRCPY(PBUF+18,"CEIPT");
                    Buf_Type(Sanghrec.MILK_TYPE,PBUF+1);
                   
                    EXT_Report_Header(46);

                    if(sanghflag=='A' )
                        Ext_Print_From_To_Date();
                    else
                        EXT_PRINT_UNDERSCORE(46);
                }
                
                STRCPY(PBUF,"FAT  SNF  QUANTITY  RATE  AMOUNT");
                CLR_Enabled(PBUF+5);
                USB_Print_SendString(ptype,1,32);
             
                PRINT_COMMON_UNDERSCORE(ptype);             
                
                print_start=1;
            }

            Copy_ShiftDT(PBUF); 
            PBUF[8]=':';
            PBUF[9]=Shiftrec.DAY_SHIFT;

            USB_Print_SendString(ptype,0,10);

            ZERO=1;
            Hex_DEC(1,Sanghrec.FAT,PBUF,4);
            PBUF[4]=' ';
            Hex_DEC(1,Sanghrec.SNF,PBUF+5,4);
            PBUF[9]=' ';
            Hex_DEC(2,Sanghrec.QTY,PBUF+10,8);
            PBUF[18]=' ';
            Hex_DEC(2,Sanghrec.RATE,PBUF+18,6);

            Calculate_Sangh_Amt();

            Hex_DEC(2,sangh_amount,PBUF+24,8);

            if(sangh_amount>9999999)
                Hex_DEC(2,sangh_amount,PBUF+24,9);

            if(Sanghrec.QTY>0 && Sanghrec.QTY!=0xFFFFFFFF)
            {
                USB_Print_SendString(ptype,0,32);

                TOTAL_QTY=TOTAL_QTY+Sanghrec.QTY;
                TOTAL_AMT=TOTAL_AMT+sangh_amount;
            }
        }
        if(sanghflag!='A' )
            break;
    }
    if(print_start==1 )
    {
        if( sanghflag=='A' )
            Print_Grand_Total(ptype);
        else
        {
            PRINT_COMMON_UNDERSCORE(ptype);
            if(ptype=='T')
                Paper_Feed(2);
            else
                Ext_Printer_Stop();
        }
    }
        
    else
    {
        no_data();
    }
    ZERO=0;
}

void Total_Summary(UINT8 milktype)
{
    if(milktype=='C')
    {
        Cumrec.C_QTY=Cumrec.C_QTY+Balancerec.QTY;
        Cumrec.C_INC_AMT=Cumrec.C_INC_AMT+Balancerec.INCENT_AMT;
        Cumrec.C_AMOUNT=Cumrec.C_AMOUNT+Balancerec.CREDIT_AMT-Balancerec.INCENT_AMT;
        Cumrec.C_PAID_AMOUNT=Cumrec.C_PAID_AMOUNT+Balancerec.PAID_AMT;
        Cumrec.C_DEDUCTION=Cumrec.C_DEDUCTION+Balancerec.DEBIT_AMT;

        if(Balancerec.QTY>0)
        {
            AVG_RATE_CALC=Balancerec.AVG_RATE/Balancerec.QTY;
            AVG_RATE_CALC=AVG_RATE_CALC*Balancerec.QTY;
            Cumrec.C_RATE=Cumrec.C_RATE+AVG_RATE_CALC;
            
            AVG_FAT_CALC=Balancerec.AVG_FAT/Balancerec.QTY;
            AVG_FAT_CALC=AVG_FAT_CALC*Balancerec.QTY;
            Cumrec.C_FAT=Cumrec.C_FAT+AVG_FAT_CALC;
            
            AVG_SNF_CALC=Balancerec.AVG_SNF/Balancerec.QTY;
            AVG_SNF_CALC=AVG_SNF_CALC*Balancerec.QTY;
            Cumrec.C_SNF=Cumrec.C_SNF+AVG_SNF_CALC;
            
            AVG_CLR_CALC=Balancerec.AVG_CLR/Balancerec.QTY;
            AVG_CLR_CALC=AVG_CLR_CALC*Balancerec.QTY;
            Cumrec.C_CLR=Cumrec.C_CLR+AVG_CLR_CALC;
        }

        Cumrec.C_MEMBERS++;
    }

    if(milktype=='B')
    {
        Cumrec.B_QTY=Cumrec.B_QTY+Balancerec.QTY;
        Cumrec.B_INC_AMT=Cumrec.B_INC_AMT+Balancerec.INCENT_AMT;
        Cumrec.B_AMOUNT=Cumrec.B_AMOUNT+Balancerec.CREDIT_AMT-Balancerec.INCENT_AMT;
        Cumrec.B_PAID_AMOUNT=Cumrec.B_PAID_AMOUNT+Balancerec.PAID_AMT;
        Cumrec.B_DEDUCTION=Cumrec.B_DEDUCTION+Balancerec.DEBIT_AMT;

        if(Balancerec.QTY>0)
        {
            AVG_RATE_CALC=Balancerec.AVG_RATE/Balancerec.QTY;
            AVG_RATE_CALC=AVG_RATE_CALC*Balancerec.QTY;
            Cumrec.B_RATE=Cumrec.B_RATE+AVG_RATE_CALC;
            
            AVG_FAT_CALC=Balancerec.AVG_FAT/Balancerec.QTY;
            AVG_FAT_CALC=AVG_FAT_CALC*Balancerec.QTY;
            Cumrec.B_FAT=Cumrec.B_FAT+AVG_FAT_CALC;
            
            AVG_SNF_CALC=Balancerec.AVG_SNF/Balancerec.QTY;
            AVG_SNF_CALC=AVG_SNF_CALC*Balancerec.QTY;
            Cumrec.B_SNF=Cumrec.B_SNF+AVG_SNF_CALC;
            
            AVG_CLR_CALC=Balancerec.AVG_CLR/Balancerec.QTY;
            AVG_CLR_CALC=AVG_CLR_CALC*Balancerec.QTY;
            Cumrec.B_CLR=Cumrec.B_CLR+AVG_CLR_CALC;
        }

        Cumrec.B_MEMBERS++;
    }
}

void Print_Payment_Register_Start(void)
{
    Print_Report_Header('S');

    Print_From_To_Date();

    STRCPY(PBUF,"CODE  QUANTITY  A.RATE  INCENTIVE   AMOUNT");
    Printer_SendString('S',42);
    PRINT_UNDERSCORE();
}

void Print_Payment_Report_Record(UINT16 pcode,UINT8 pmilk,UINT8 printertype)
{
    ZERO=0;
    Hex_DEC(0,pcode,PBUF,4);

    PBUF[4]=0x42+pmilk;
    ZERO=1;

    PBUF[5]=' ';
   
    Hex_DEC(2,Balancerec.QTY,PBUF+6,9);
    if(Balancerec.QTY>99999999)
        Hex_DEC(2,Balancerec.QTY,PBUF+6,10);
    PBUF[15]=' ';
    if(Balancerec.QTY>0)
        Hex_DEC(2,(Balancerec.AVG_RATE/Balancerec.QTY),PBUF+16,6);
    else
        Hex_DEC(2,0,PBUF+16,6);
    PBUF[22]=' ';
    Hex_DEC(2,Balancerec.INCENT_AMT,PBUF+23,8);
    PBUF[31]=' ';
    Hex_DEC(2,Balancerec.CREDIT_AMT,PBUF+32,10);
    
    if(printertype=='E')
    {
        USB_Print_SendString('E',0,42);
     }
    else
        Printer_SendString('S',42);
}

void Print_Payment_Register_Record(UINT16 ptcode,UINT8 pmtype,UINT8 printertype)
{
    INT32 p_net_amount;
    
    Print_Payment_Report_Record(ptcode,pmtype,printertype);    
   
    
    Init_Regular_Font();
    
    STRCPY(PBUF,"P:00000.00 D:00000.00N:000000.00");
    Hex_DEC(2,Balancerec.PAID_AMT,PBUF+2,8);
    if(Balancerec.PAID_AMT>9999999)
    {
        Hex_DEC(2,Balancerec.PAID_AMT,PBUF+2,9);
        PBUF[10]=' ';
    }
    else
        Shift_Left(PBUF+2,8);

    Hex_DEC(2,Balancerec.DEBIT_AMT,PBUF+13,8);

    if(Balancerec.DEBIT_AMT>9999999)
    {
        Hex_DEC(2,Balancerec.DEBIT_AMT,PBUF+13,9);
        PBUF[21]='N';
    }
    else
        Shift_Left(PBUF+13,8);
    p_net_amount=(Balancerec.CREDIT_AMT-(Balancerec.DEBIT_AMT+Balancerec.PAID_AMT));
    
    Hex_DEC(2,p_net_amount,PBUF+23,9);

    if(p_net_amount>99999999)
    {
        Hex_DEC(2,p_net_amount,PBUF+23,10);
    }
    else
        Shift_Left(PBUF+23,9);
    

    if(Balancerec.PAID_AMT>0 || Balancerec.DEBIT_AMT!=0 || p_net_amount!=0)
    {      
        USB_Print_SendString(printertype,0,32);
    }
    if(Balancerec.QTY>0 || Balancerec.PAID_AMT>0 || Balancerec.DEBIT_AMT!=0 || p_net_amount!=0)
    {
        STRCPY(PBUF,"   ");
        USB_Print_SendString(printertype,0,3);
     
        Total_Summary(0x42+pmtype);
    }
}

void Print_Payment_Register_Report(UINT8 printtype)
{
    UINT8 print_start=0,no_members,records_present;
    UINT8 p_temp_code,tempi;
    UINT16 p_temp_count,p_member_code;

    Print_LineNo=0;
    Init_Cum_Values();
    Init_Variables();
    
    Disp_Checking_Data(0);
    
    PRESENT_MEMBER_NO=1;
    
    do
    {
        no_members=Get_No_Of_Customers(Fromrec.START_MEMBER,Fromrec.END_MEMBER);

        if(no_members>0)
            records_present=Read_New_Balance_Details(no_members);
        else
            break;
        
        if(records_present=='Y')
        {
           // for(p_temp_code=0;p_temp_code<no_members;p_temp_code++)
            for(p_temp_count=Fromrec.START_MEMBER;p_temp_count<=Fromrec.END_MEMBER;p_temp_count++)
            {
                for(p_temp_code=0;p_temp_code<no_members;p_temp_code++)
                {
                
                    p_member_code=ReportsMatchrec[p_temp_code].CUST_CODE;
                    if(p_temp_count==p_member_code)
                    {

                        Disp_Checking_Data(p_member_code);
                        DEDUCTION_FLAG=0;
                        for(tempi=0;tempi<2;tempi++)
                        {
                            if(tempi%2==0)
                            {
                                if(BalanceRecords[(p_temp_code)*2].QTY==0 && BalanceRecords[(p_temp_code)*2].DEBIT_AMT!=0)
                                {   
                                    BalanceRecords[(p_temp_code)*2+1].DEBIT_AMT=BalanceRecords[(p_temp_code)*2].DEBIT_AMT;
                                    BalanceRecords[(p_temp_code)*2].DEBIT_AMT=0;

                                }
                            }
                            Balancerec=BalanceRecords[(p_temp_code)*2+tempi];
                            if(DEDUCTION_FLAG==1)
                                Balancerec.DEBIT_AMT=0;

                            if(Balancerec.QTY>0 || Balancerec.PAID_AMT>0 || Balancerec.DEBIT_AMT!=0)
                            {
                                DEDUCTION_FLAG=1;
                                if(print_start==0)
                                {
                                    if(printtype=='T')
                                        Print_Payment_Register_Start();

                                    else if(printtype=='U')
                                    {
                                        if(USB_Ext80_Payment_Register_Start(printtype)==0xFF)
                                            return;
                                    }

                                    else
                                    {
                                        if (!Initialize_Printer())
                                            return;
                                        if(Ext_Print_L==40)
                                            Ext40_Print_Payment_Register_Start();
                                        else
                                            USB_Ext80_Payment_Register_Start(printtype);

                                        Please_Wait_Msg(LINE5);
                                    }

                                    print_start=1;

                                }
                                ZERO=0;
                                if(printtype=='U' || (Ext_Print_L==80 && printtype=='E'))
                                    USB_Ext80_Payment_Register_Record(printtype,p_member_code,tempi);

                                else if(printtype=='T' || (Ext_Print_L==40 && printtype=='E'))                
                                    Print_Payment_Register_Record(p_member_code,tempi,printtype);                        
                            }
                            if(Balancerec.QTY>0 || Balancerec.PAID_AMT>0 || Balancerec.DEBIT_AMT!=0)
                            {
                                if(printtype=='U')
                                    Write_USB_File(0);
                            }
                        }
                        break;
                    }
                }
            }
        }
        
    }while(no_members>=100);

    if(print_start==1)
    {
        if(printtype=='T' || (printtype=='E' && Ext_Print_L==40))
        {
            if(printtype=='T')
                PRINT_UNDERSCORE();
            else
                EXT_PRINT_UNDERSCORE(46);
            
            Print_Average_Total(printtype);
            
        }
        else if(printtype=='U' || (printtype=='E' &&Ext_Print_L==80))
        {
            if(printtype=='E')
                EXT_PRINT_UNDERSCORE(80);
            Print_Average_Total(printtype);
            ZERO=0;
            if(printtype=='U')
            {
                FSfclose(myFile);
            }
        }
              
        if(printtype=='T')
            Paper_Feed(2);
        
        if(printtype=='E')
            Ext_Printer_Stop();
    }
    else
        no_data();   
}
				
void Print_Heading(UINT8 preportflag)
{
    Print_Report_Header(preportflag);//1
    
    Print_From_To_Date(); 
    
    ZERO=0;
    Hex_DEC(0,Memberrec.C_CODE,PBUF,4);

    STRCPY(PBUF+4,"  COW    ");
    Buf_Type(Memberrec.MILK_TYPE,PBUF+6);

    STRNCPY(PBUF+13,Memberrec.NAME,19);
    Printer_SendString(0x03,32);
    
    PRINT_UNDERSCORE();

    STRCPY(PBUF,"DAY  FAT  SNF   QTY  RATE INCENTIVE AMOUNT");
    CLR_Enabled(PBUF+10);
    Printer_SendString('S',42);

    PRINT_UNDERSCORE();
}

void Print_Member_Shift_Record(void)
{
    Calculate_Amount();
    
    ZERO=0;
    Hex_DEC(0,Shiftrec.DAY,PBUF,2);
    PBUF[2]=Shiftrec.DAY_SHIFT;
    ZERO=1;
    PBUF[3]=CollectionFlags.MILK_TYPE;
    Hex_DEC(1,CollectionRecord.FAT,PBUF+4,4);
    PBUF[8]=' ';
    if(CollectionFlags.PAYMENT_STATUS==0)
        PBUF[8]='P';
    
    if(CLR_BASED == 'Y')
        Hex_DEC(1,CollectionRecord.CLR1,PBUF+9,4);
    else
        Hex_DEC(1,CollectionRecord.SNF,PBUF+9,4);
    PBUF[13]=' ';
   
    if((CollectionRecord.FLAGS | 0xF7)!=0xFF)
    {

        PBUF[13]='M';
    }

    Hex_DEC(2,CollectionRecord.QTY,PBUF+14,5);

    if(CollectionRecord.QTY>9999)
        Hex_DEC(2,CollectionRecord.QTY,PBUF+14,6);

    PBUF[19]=' ';    
     
    if((CollectionRecord.FLAGS | 0xFB)!=0xFF)
    {

            PBUF[19]='M';
    }
    PBUF[20]=' ';    
    
    Hex_DEC(2,CollectionRecord.RATE,PBUF+21,5);

    if(CollectionRecord.RATE>9999)
        Hex_DEC(2,CollectionRecord.RATE,PBUF+21,5);

    PBUF[26]=' ';   
    Hex_DEC(2,CollectionRecord.INCENTIVE_AMT,PBUF+27,6);
    
    if(CollectionRecord.INCENTIVE_AMT>99999)
        Hex_DEC(2,CollectionRecord.INCENTIVE_AMT,PBUF+27,7);
    
    PBUF[33]=' '; 
    Hex_DEC(2,Amount+CollectionRecord.INCENTIVE_AMT,PBUF+34,8);

    Printer_SendString('S',42);    
}

void Print_Auto_Deduction_Amt(UINT8 ptype)
{    
    if(MemberBillrec.DED_AMT!=0)
    {
        STRCPY(PBUF,"DEDUCTION AMOUNT   :    ");
        Hex_DEC(2,MemberBillrec.DED_AMT,PBUF+24,8);
        if(ptype=='U')
        {
            PBUF[20]=',';
            PBUF[21]=',';
        }
        USB_Print_SendString(ptype,0,32);
    }

    SEERAMReadString(Auto_Deduction_Loc,Auto_Length,(char *)&Autorec);

    if(Autorec.STATUS=='Y' && Autorec.RATE>0)
    {
        STRCPY(PBUF,"                          ");        
        STRNCPY(PBUF,Autorec.DEDUCTION_NAME,19);  
        PBUF[19]=':';
        Hex_DEC(2,Autorec.RATE,PBUF+26,6);
        if(ptype=='U')
        {
            PBUF[20]=',';
            PBUF[21]=',';
        }
        USB_Print_SendString(ptype,0,32);

        MemberBillrec.DED_AMT=MemberBillrec.DED_AMT+Autorec.RATE;
    }
    if(MemberBillrec.DED_AMT>0)
    {
        if(ptype=='T')
            PRINT_UNDERSCORE();
        if(ptype=='E')
        {
            if(Ext_Print_L==80)
                EXT_PRINT_UNDERSCORE(80);
            else
                EXT_PRINT_UNDERSCORE(46);
        }
    }
}

void Print_MemberBill_Report(UINT16 pmembersno,UINT8 ptype)
{
    UINT8 print_start=0,p_month=0,ccount,cshift;    
    UINT16 noofrecords,tempcount;
    INT32 p_temp_adr,m_net_amount;
      
    MemberBillrec.TOTAL_AMT=0;
    MemberBillrec.INCENT_AMT=0;
    MemberBillrec.DED_AMT=0;
    MemberBillrec.BILL_AMT=0;
    MemberBillrec.GRAND_TOTAL=0;
    MemberBillrec.PAID_AMT=0;
    ALREADY_PAID_AMOUNT=0;
    ALREADY_PAID_QTY=0;
   
    Init_Variables();
    Init_Cum_Values();

    Read_Member_Status(pmembersno);

    if(Memberrec.STATUS=='A')
    {
        ZERO=1;
       
        assign_dates();
        S_SNo=pmembersno;
        E_SNo=pmembersno;
        Shiftrec_SNo=pmembersno;
        
        
        Read_Member_Details(pmembersno);
        SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&RunningShift);
        
        ShiftDateRecord.YEAR=RunningShift.YEAR;
        ShiftDateRecord.MONTH=RunningShift.MONTH;
        ShiftDateRecord.DAY=RunningShift.DAY;
        ShiftDateRecord.DAY_SHIFT=RunningShift.DAY_SHIFT;
        assign_dates();
        cshift=check_date_inrange();
        
        for(ccount=0;ccount<cshift+1;ccount++)
        {
            if(ccount==0)
            {                
                noofrecords=DynamicRead_MemberBillDetails(pmembersno);
            }
            else
            {
                if(pmembersno!=10000)   
                    Read_Member_CODE(pmembersno);
                else
                    Memberrec.C_CODE=10000;
                noofrecords=Read_CurrentShiftDetails(Memberrec.C_CODE);
            }
                   
            for(tempcount=noofrecords;(tempcount>0);tempcount--)
            {
                CollectionRecord=CollectionRecords[tempcount-1];
                Read_Collection_Flags();
                if(ccount==0)
                        Shiftrec=ShiftRecords[tempcount-1];
                else
                        Shiftrec=RunningShift;
               
                CollectionRecord_NO=CollectionRecords_NOS[tempcount-1];

                if(CollectionRecord.WTIME[0] !=0xFF)
                {
                    if(CollectionRecord.QTY!=0xFFFF)
                    {
                        DEDUCTION_FLAG=0;
                        p_temp_adr=EEPROM_ADDR;

                        if(print_start==0)
                        {
                            if(ptype=='T')
                                Print_Heading('C');

                            if(ptype=='E')
                            {
                                if(Ext_Print_Heading('C')==0xFF)
                                    return;
                            }

                            if(ptype=='U')
                            {
                                if(USB_Memberbill_Start()==0xFF)
                                    return;
                                                            
                                Please_Wait_Msg_Small(LINE5);
                                
                            }
                            print_start=1;
                            Print_Feed_Status=1;
                        }

                        EEPROM_ADDR=p_temp_adr;

                        if(CollectionRecord.QTY==0 || CollectionRecord.QTY==0xFFFF)
                            CollectionRecord.RATE=0;

                        if(p_month!=Shiftrec.MONTH)
                        {
                            MONTH_READ_FLAG=1;
                            p_month=Shiftrec.MONTH;
                        }                   

                        if(MONTH_READ_FLAG==1 && ptype!='U' && (ptype=='T' || Ext_Print_L==40))
                        {
                            Print_C_Month(Shiftrec.MONTH,Shiftrec.YEAR,ptype);
                            MONTH_READ_FLAG=0;
                        }                   

                        MemberBillrec.INCENT_AMT=MemberBillrec.INCENT_AMT+CollectionRecord.INCENTIVE_AMT;
                        if(ptype=='T')
                            Print_Member_Shift_Record();
                        else if(ptype=='U' || (ptype=='E' && Ext_Print_L==80))
                            Ext80_USB_Member_Shift_Record(ptype);                       
                        else
                            Ext40_Print_Member_Shift_Record();

                        if(ptype!='U' && CollectionFlags.PAYMENT_STATUS==1 && DEALER_SELECTION!=3)
                            Set_Collection_Flag('P');           //payment flag setting
                        if(CollectionFlags.PAYMENT_STATUS==0)
                        {
                            ALREADY_PAID_AMOUNT = ALREADY_PAID_AMOUNT + Amount +CollectionRecord.INCENTIVE_AMT;
                            ALREADY_PAID_QTY= ALREADY_PAID_QTY + CollectionRecord.QTY;

                        }

                        TOTAL_QTY=TOTAL_QTY+CollectionRecord.QTY;
                        TOTAL_AMT=TOTAL_AMT+Amount;

                        Calc_Cum_Avg();
                    }
                }
            }
        }
        Init_Regular_Font();
        Read_Deduction_Amount(pmembersno);
        
        if(Deductionrec.AMT!=0xFFFFFFFF)
        {
            MemberBillrec.DED_AMT=Deductionrec.AMT;
        }
       
        ZERO=1;
        MemberBillrec.TOTAL_AMT=Cumrec.C_AMOUNT+Cumrec.B_AMOUNT;
        
        MemberBillrec.GRAND_TOTAL=MemberBillrec.TOTAL_AMT+MemberBillrec.INCENT_AMT;
        if(MemberBillrec.TOTAL_AMT>0 && print_start==1)
        {
            if(ptype=='T')
                PRINT_UNDERSCORE();
            else if(ptype=='E')
            {  
                if(Ext_Print_L==80)
                    EXT_PRINT_UNDERSCORE(80);
                else
                    EXT_PRINT_UNDERSCORE(46);
            }
            
            Print_Average_Total(ptype);
        }
     
        ZERO=1;
        if(ALREADY_PAID_AMOUNT>0)
        {
            STRCPY(PBUF,"Already Paid Amount:   ");
            if(ptype=='U')
            {
                PBUF[21]=',';
                PBUF[22]=',';
            }
            Hex_DEC(2,ALREADY_PAID_AMOUNT,PBUF+23,9);
            USB_Print_SendString(ptype,0,32);
        }
        if(print_start==1 && ALREADY_PAID_QTY!= TOTAL_QTY)
            Print_Auto_Deduction_Amt(ptype);
       
        CLEAR_DATA_LINES_FROM(4,6);
        
        MemberBillrec.GRAND_TOTAL=MemberBillrec.GRAND_TOTAL-ALREADY_PAID_AMOUNT;   
  
        m_net_amount=0;
        
        if(MemberBillrec.GRAND_TOTAL>=MemberBillrec.DED_AMT)
        {            
            STRCPY(LBUF,"NET AMT:");
            m_net_amount=MemberBillrec.GRAND_TOTAL-MemberBillrec.DED_AMT;  
            
        }
        else
        {           
            STRCPY(LBUF,"DUE AMT:");            
            m_net_amount=MemberBillrec.DED_AMT-MemberBillrec.GRAND_TOTAL;
        }
        
        Hex_DEC(2,m_net_amount,LBUF+8,9);
        Shift_Left(LBUF+8,9);
        Disp_Msg(LINE5);

        if(MemberBillrec.GRAND_TOTAL>=MemberBillrec.DED_AMT)
        {
            STRCPY(PBUF,"NET PAYMENT    (Rs):   ");            
        }
        else
        {
            STRCPY(PBUF,"DUE AMOUNT     (Rs):   ");  
        }
        Hex_DEC(2,m_net_amount,PBUF+23,9);
 
        if(print_start==1)
        {
            print_start=0;
            if(ptype=='U')
            {
                PBUF[21]=',';
                PBUF[22]=',';    
            }
            USB_Print_SendString(ptype,0,32);
            
            Deductionrec.AMT=MemberBillrec.DED_AMT-(MemberBillrec.GRAND_TOTAL);
            
            if(ptype!='U')
            {
                Deductionrec.AMT=0;
                CLEAR_DATA_LINES_FROM(7,7);                
            }
                    
            if(ptype=='T')
            {
                PRINT_UNDERSCORE();
                Paper_Feed(2);
            } 
            else if(ptype=='E')
            {  
                if(Ext_Print_L==80)
                    EXT_PRINT_UNDERSCORE(80);
                else
                    EXT_PRINT_UNDERSCORE(46);
                Ext_Printer_Stop();
                ZERO=0;
            }
            else if(ptype=='U')
            {         
                FSfclose(myFile);
            }
            
            if(ptype!='U')
            {
                Write_Deduction_Amount(pmembersno);
            }
#ifdef GPRS
            if(ptype!='U' && GPRSrec.GPRS_ON=='Y' &&  ON_LINE=='Y' && m_net_amount>0)
            {
                STRCPY(LBUF,"Sending SMS Pls Wait");
                Invert_Disp_Msg(LINE8);
                
                do
                {
                    Send_SMS_BackEnd('m',pmembersno);
                }while(SMS_STATE!=0);

                Msg_Delivery_Ack();
                CLEAR_DATA_LINES_FROM(LINE8,LINE8);
            }
#endif

        }
    }
    CLEAR_DATA_LINES_FROM(3,7);
}

void Print_Customerwise_Report(UINT8 smsoption,UINT16 pmembersno,UINT8 ptype)
{
    UINT8 print_start=0,p_month=0,cshift,ccount;
    UINT16 noofrecords,tempcount;
    
    Balancerec.QTY=0;
    Balancerec.CREDIT_AMT=0;
    Balancerec.DEBIT_AMT=0;
    Balancerec.PAID_AMT=0;
    Balancerec.PAID_QTY=0;
    Balancerec.AVG_RATE=0;

    ZERO=0;
       
    MemberBillrec.TOTAL_AMT=0;
    MemberBillrec.INCENT_AMT=0;
    MemberBillrec.DED_AMT=0;
    MemberBillrec.BILL_AMT=0;
    MemberBillrec.GRAND_TOTAL=0;
    MemberBillrec.PAID_AMT=0;
    
    Init_Variables();
    Init_Cum_Values();

    Read_Member_Status(pmembersno);

    if(Memberrec.STATUS=='A')
    {
        ZERO=1;
        assign_dates();
        
        MONTH_READ_FLAG=1;
        NO_COLLECTIONS=0;
        SEERAMReadString(Running_Shift_Details_Loc,4,(char *)&RunningShift);
        //ShiftDateRecord=RunningShift;
        ShiftDateRecord.YEAR=RunningShift.YEAR;
        ShiftDateRecord.MONTH=RunningShift.MONTH;
        ShiftDateRecord.DAY=RunningShift.DAY;
        ShiftDateRecord.DAY_SHIFT=RunningShift.DAY_SHIFT;
        assign_dates();
        cshift=check_date_inrange();
                
        for(ccount=0;ccount<cshift+1;ccount++)
        {
            if(ccount==0)
            {
                
                noofrecords=DynamicRead_MemberBillDetails(pmembersno);
            }
            else
            {
                if(pmembersno!=10000)   
                    Read_Member_CODE(pmembersno);
                else
                    Memberrec.C_CODE=10000;
                noofrecords=Read_CurrentShiftDetails(Memberrec.C_CODE);
            }
        
            for(tempcount=noofrecords;(tempcount>0);tempcount--)
            {
                CollectionRecord=CollectionRecords[tempcount-1];
                Read_Collection_Flags();
                if(ccount==0)
                    Shiftrec=ShiftRecords[tempcount-1];
                else
                    Shiftrec=RunningShift;

                if(CollectionRecord.QTY!=0xFFFF)
                {
                    if(print_start==0)
                    {
                        Read_Member_Details(pmembersno);
                        if(ptype=='T')
                            Print_Heading('c');
                        else
                            Ext_Print_Heading('c');
                        print_start=1;
                        Print_Feed_Status=1;                    
                    }

                    if(p_month!=Shiftrec.MONTH)
                    {
                        MONTH_READ_FLAG=1;
                        p_month=Shiftrec.MONTH;
                    }

                    if(MONTH_READ_FLAG==1 && ptype!='U' && (ptype=='T' || Ext_Print_L==40))
                    {                    
                        Print_C_Month(Shiftrec.MONTH,Shiftrec.YEAR,ptype);
                        MONTH_READ_FLAG=0;
                    }       

                    if(ptype=='T')
                        Print_Member_Shift_Record();

                    else if(Ext_Print_L==80)
                        Ext80_USB_Member_Shift_Record('E');
                    else
                        Ext40_Print_Member_Shift_Record();

                    if(CollectionFlags.PAYMENT_STATUS==0  && Amount!=0xFFFFFFFF)//paid
                        MemberBillrec.PAID_AMT=MemberBillrec.PAID_AMT+Amount+CollectionRecord.INCENTIVE_AMT;  

                    MemberBillrec.INCENT_AMT=MemberBillrec.INCENT_AMT+CollectionRecord.INCENTIVE_AMT;
                    Calc_Cum_Avg();
                }
            }
        }
        Init_Regular_Font();
        MemberBillrec.TOTAL_AMT=Cumrec.C_AMOUNT+Cumrec.B_AMOUNT;
        
        if(MemberBillrec.TOTAL_AMT>0 && print_start==1)
        {
            if(ptype=='T')
                PRINT_UNDERSCORE();
            else if(ptype=='E')
            {  
                if(Ext_Print_L==80)
                    EXT_PRINT_UNDERSCORE(80);
                else
                    EXT_PRINT_UNDERSCORE(46);
            }
            Print_Average_Total(ptype);

            Read_Deduction_Amount(pmembersno);
            if(Deductionrec.AMT!=0xFFFFFFFF)
                MemberBillrec.PAID_AMT= MemberBillrec.PAID_AMT+Deductionrec.AMT;

            if(MemberBillrec.PAID_AMT>0)
            {
                STRCPY(PBUF,"TOTAL PAID AMOUNT:    ");
                Hex_DEC(2,MemberBillrec.PAID_AMT,PBUF+22,10);
                USB_Print_SendString(ptype,1,32);
            }
            MemberBillrec.GRAND_TOTAL=MemberBillrec.TOTAL_AMT+MemberBillrec.INCENT_AMT;
            if(MemberBillrec.GRAND_TOTAL>=MemberBillrec.PAID_AMT)
            {
                STRCPY(PBUF,"BALANCE AMT  (Rs):    ");
                Hex_DEC(2,(MemberBillrec.GRAND_TOTAL-MemberBillrec.PAID_AMT),PBUF+22,10);
               
            }
            else
            {
                STRCPY(PBUF,"DUE AMOUNT   (Rs):    ");
                Hex_DEC(2,(MemberBillrec.PAID_AMT-MemberBillrec.GRAND_TOTAL),PBUF+22,10);               
            }
            USB_Print_SendString(ptype,1,32);
        }
        if(print_start==1)
        {
            if(ptype=='T')
            {
                PRINT_UNDERSCORE();
                Paper_Feed(2);
            }
            else
            {
                if(Ext_Print_L==80)
                    EXT_PRINT_UNDERSCORE(80);
                else
                    EXT_PRINT_UNDERSCORE(46);
                Ext_Printer_Stop();
            }

#ifdef GPRS

            if(GPRSrec.GPRS_ON=='Y' &&  ON_LINE=='Y' && smsoption==1)
            {
                STRCPY(LBUF,"Sending SMS Pls Wait");
                Invert_Disp_Msg(LINE8);
                
                do
                {
                    Send_SMS_BackEnd('M',pmembersno);
                }while(SMS_STATE!=0);

                Msg_Delivery_Ack();
                Delay_250Ms(2);
                CLEAR_DATA_LINES_FROM(6,6);
            }
#endif

        }
    }
}

void Init_Daywise_Report(void)
{
    Print_LineNo=0;
    Init_Variables();

    TOTAL_QTY=0;
    TOTAL_QTY_COW=0;
    TOTAL_QTY_BUF=0;
    TOTAL_AMT=0;
    TOTAL_AMT_COW=0;
    TOTAL_AMT_BUF=0;
}

void Print_Daywise_Report_Start(void)
{
    Start_Printer();
  
    STRCPY(PBUF,"*MEMBERWISE SUMMARY REPORT*");
    Printer_SendString(0x03,27);
   
    Header_Footer_Print('T',0,1);
    
    Print_From_To_Date();

    STRCPY(PBUF,"CODE  FAT% SNF%  QUANTITY A.RATE TOTAL_AMT");
    CLR_Enabled(PBUF+11);
    Printer_SendString('S',42);
    PRINT_UNDERSCORE();
}

void Print_Daywise_Report_Record(UINT16 pcode,UINT8 pmilk,UINT8 printertype)
{
    ZERO=1;
    Hex_DEC(0,pcode,PBUF,4);  
    PBUF[4]=0x42+pmilk;
    Shift_Left(PBUF,5);
  //  ZERO=1;

    PBUF[5]=' ';
    if(Balancerec.QTY>0)
        Hex_DEC(1,(Balancerec.AVG_FAT/Balancerec.QTY),PBUF+6,4);
    else
        Hex_DEC(1,0,PBUF+6,4);
    PBUF[10]=' ';
     if(Balancerec.QTY>0)
     {
        if(CLR_BASED=='Y')
            Hex_DEC(1,(Balancerec.AVG_CLR/Balancerec.QTY),PBUF+11,4);
        else
            Hex_DEC(1,(Balancerec.AVG_SNF/Balancerec.QTY),PBUF+11,4);
     }
    else
        Hex_DEC(1,0,PBUF+11,4);
    PBUF[15]=' ';
    Hex_DEC(2,Balancerec.QTY,PBUF+16,9);
    if(Balancerec.QTY>99999999)
        Hex_DEC(2,Balancerec.QTY,PBUF+16,10);
    PBUF[25]=' ';
    if(Balancerec.QTY>0)
        Hex_DEC(2,(Balancerec.AVG_RATE/Balancerec.QTY),PBUF+26,6);
    else
        Hex_DEC(2,0,PBUF+26,6);
    PBUF[32]=' ';
    //Hex_DEC(2,Balancerec.INCENT_AMT,PBUF+23,8);
    
    Hex_DEC(2,Balancerec.CREDIT_AMT,PBUF+33,9);
    
    if(Balancerec.CREDIT_AMT>99999999)
        Hex_DEC(2,Balancerec.CREDIT_AMT,PBUF+33,10);
    
    PBUF[42]=' ';

     if(printertype=='E')
     {
        USB_Print_SendString('E',0,42);
     }
    else
        Printer_SendString('S',42);
}

void Print_Daywise_Report(UINT8 ptype)
{
    UINT8 print_start=0,tempi,no_members;
    UINT16 p_temp_code,p_member_code,p_temp_count;
    UINT8 records_present,tempsno;

    Init_Cum_Values();
    Init_Daywise_Report();
    Disp_Checking_Data(0);
    
    PRESENT_MEMBER_NO=1;
    
    do
    {
        no_members=Get_No_Of_Customers(Fromrec.START_MEMBER,Fromrec.END_MEMBER);

        if(no_members>0)
            records_present=Read_New_Balance_Details(no_members);
        else
            break;
        
        if(records_present=='Y')
        {
            for(p_temp_count=Fromrec.START_MEMBER;p_temp_count<=Fromrec.END_MEMBER;p_temp_count++)
            {
                for(p_temp_code=0;p_temp_code<no_members;p_temp_code++)
                {
                
                    p_member_code=ReportsMatchrec[p_temp_code].CUST_CODE;
                   if(p_temp_count==p_member_code)
                    {
                        tempsno=ReportsMatchrec[p_temp_code].CODE_SNO;

                        Read_Member_Name(tempsno);

                        for(tempi=0;tempi<2;tempi++)
                        {
                            Balancerec=BalanceRecords[(p_temp_code)*2+tempi];
                            Balancerec.DEBIT_AMT=0;

                            if(Balancerec.QTY!=0)
                            {
                                if(print_start==0)
                                {
                                    if(ptype=='T')
                                        Print_Daywise_Report_Start();
                                    else
                                    {
                                        if (!Initialize_Printer())
                                            return;
                                        Ext_Print_Daywise_Report_Start();
                                    }

                                    print_start=1;
                                }

                                if(ptype=='T')
                                    Print_Daywise_Report_Record(p_member_code,tempi,ptype);//1
                                else
                                    Ext_Print_Daywise_Report_Record(p_member_code,tempi);

                                Total_Summary(0x42+tempi);                       
                            }
                        }
                    }
                }
            }
        }
    }while(no_members>=100);
         
    if(print_start==1)
    {
        Init_Regular_Font();
        
        Cumrec.B_PAID_AMOUNT=0;
        Cumrec.B_DEDUCTION=0;
        Cumrec.C_PAID_AMOUNT=0;
        Cumrec.C_DEDUCTION=0;
             
        if(ptype=='E')
        {
            EXT_PRINT_UNDERSCORE(46);
            Print_Average_Total('m');
            Ext_Printer_Stop();
        }
            
        else
        {
            PRINT_UNDERSCORE();
            Print_Average_Total('M');
            Paper_Feed(2);
        }
    }     
    else
        no_data();
}

void Print_Deduction_Report(UINT8 ptype)
{
    UINT8 print_start=0;
    UINT16 p_member_code,p_member_sno;

    Print_LineNo=0;
    Init_Variables();
    Disp_Checking_Data(0);
    for(p_member_code=Fromrec.START_MEMBER;p_member_code<=Fromrec.END_MEMBER;p_member_code++)
    {
        p_member_sno=Read_Member_SNO(p_member_code);
        Disp_Checking_Data(p_member_code);

        if(p_member_sno!=0)
        {  
            Read_Member_Status(p_member_sno);

            if(Memberrec.STATUS=='A')
            {
                Read_Deduction_Amount(p_member_sno);

                if(Deductionrec.AMT!=0xFFFFFFFF && Deductionrec.AMT!=0)
                {
                    if(print_start==0)
                    {
                        if(ptype=='T')
                        {
                            Print_Report_Header('E');
                        }
                        else
                        {
                            if (!Initialize_Printer())
                            return;
       
                            USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_JOB_START, USB_NULL, 0, 0 );

                            STRCPY(PBUF," DEDUCTION REPORT");
                            Ext_Print_Header();
                            EXT_PRINT_UNDERSCORE(32);
                            Header_Footer_Print('E',0,4);
                            EXT_PRINT_UNDERSCORE(32);
                        }

                        STRCPY(PBUF,"CODE  NAME                AMOUNT");
                        USB_Print_SendString(ptype,1,32);
                        if(ptype=='T')
                        {
                            PRINT_UNDERSCORE();
                        }
                        else
                        {
                            EXT_PRINT_UNDERSCORE(32);
                        }

                        print_start=1;
                    }

                    Read_Member_Details(p_member_sno);

                    ZERO=1;

                    Hex_DEC(0,Memberrec.C_CODE,PBUF,4);
                    PBUF[4]=Memberrec.MILK_TYPE;
                    Shift_Left(PBUF,5);
                    PBUF[5]=' ';
                    STRNCPY(PBUF+6,Memberrec.NAME,16);
                    PBUF[22]=' ';
                    Hex_DEC(2,Deductionrec.AMT,PBUF+23,9);
                    USB_Print_SendString(ptype,0,32);

                    TOTAL_DEDUCTION=TOTAL_DEDUCTION+Deductionrec.AMT;
                }
            }
        }
    }
    
    if(print_start==1)
    {
        if(ptype=='T')
            PRINT_UNDERSCORE();
        else
            EXT_PRINT_UNDERSCORE(32);
        
        STRCPY(PBUF,"DEDUCTION AMOUNT   :Rs.  ");
        Hex_DEC(2,TOTAL_DEDUCTION,PBUF+25,7);        
        USB_Print_SendString(ptype,0,32);
        
        if(ptype=='T')
        {
            PRINT_UNDERSCORE();
            Paper_Feed(2);
        }
        else
        {
            EXT_PRINT_UNDERSCORE(32);
            Ext_Printer_Stop();
        }
    }
    else
        no_data();
}

void Print_Bonus_Type(UINT8 ptype)
{
    ZERO=0;
    STRCPY(PBUF,"TYPE:PERCENTAGE      RATE:");

    if(Bonus_Type==0)
        STRCPY(PBUF,"TYPE:PER LITRE       RATE:");

    Hex_DEC(2,Bonus_Rate,PBUF+26,6);

    USB_Print_SendString(ptype,0,32);
}

void Printer_Bonus_Start(void)
{
    Print_Report_Header('B');//2
    Print_From_To_Date();
    
    Print_Bonus_Type('T');
    PRINT_UNDERSCORE();
    ZERO=1;

    STRCPY(PBUF,"CODE   QUANTITY   AMOUNT   BONUS");
    Printer_SendString(0x03,32);

    PRINT_UNDERSCORE( );               
}

void Print_Bonus_Record(UINT16 tempcode,UINT32 bonusamount,UINT8 milktype)
{
    ZERO=1;
    Hex_DEC(0,tempcode,PBUF,4);
    PBUF[4]=milktype;
    Shift_Left(PBUF,5);
    
    Hex_DEC(2,Balancerec.QTY,PBUF+5,10);

    PBUF[15]=' ';
    Hex_DEC(2,Balancerec.CREDIT_AMT,PBUF+15,9);
    if(Balancerec.CREDIT_AMT>99999999)
        Hex_DEC(2,Balancerec.CREDIT_AMT,PBUF+15,10);
    PBUF[24]=' ';

    Hex_DEC(2,bonusamount,PBUF+25,7);

    if(bonusamount>999999)
        Hex_DEC(2,bonusamount,PBUF+25,8);

    Printer_SendString(0x03,32);
}

void Print_Bonus_Totals(UINT32 totalqty,UINT32 totalamt,UINT32 totalbonus)
{
    ZERO=1;
    PRINT_UNDERSCORE();   
    
    Print_All_Totals('T',0,totalqty,totalamt,0,0);
    
    STRCPY(PBUF,"TOTAL BONUS  (Rs):    ");
    Hex_DEC(2,totalbonus,PBUF+22,10);
    Printer_SendString(0x03,32);
    
    PRINT_UNDERSCORE();

     Paper_Feed(2);
}

void Print_Bonus_Report(UINT8 printertype)
{
    UINT8 print_start=0,no_members,records_present;
    UINT16 p_temp_code,tempi,p_member_code,p_temp_count;
    UINT32 total_quantity=0,total_amount=0;
    UINT32 p_total_bonus=0,p_bonus_amount;

    Print_LineNo=0;
    Init_Variables();
    ZERO=1;

    Disp_Checking_Data(0);
    
    PRESENT_MEMBER_NO=1;
    
    do
    {
        no_members=Get_No_Of_Customers(Fromrec.START_MEMBER,Fromrec.END_MEMBER);

        if(no_members>0)
            records_present=Read_New_Balance_Details(no_members);
        else
            break;
        
        if(records_present=='Y')
        {
            for(p_temp_count=Fromrec.START_MEMBER;p_temp_count<=Fromrec.END_MEMBER;p_temp_count++)
            {  
                for(p_temp_code=0;p_temp_code<no_members;p_temp_code++)//reddy
                {
                    p_member_code=ReportsMatchrec[p_temp_code].CUST_CODE;
                    
                    if(p_temp_count==p_member_code)
                    {
                        p_bonus_amount=0;
                        for(tempi=0;tempi<2;tempi++)
                        {
                            Balancerec=BalanceRecords[(p_temp_code)*2+tempi];

                            if(Balancerec.CREDIT_AMT>0 || Balancerec.QTY>0)
                            {
                                if(print_start==0)
                                {
                                    if(printertype=='T')
                                        Printer_Bonus_Start();
                                    else
                                    {
                                        if (!Initialize_Printer())
                                            return;
                                        Ext_Printer_Bonus_Start();
                                    }

                                    print_start=1;
                                }
                                if(Bonus_Type==1)   //amount
                                {
                                    p_bonus_amount=Balancerec.CREDIT_AMT*Bonus_Rate;
                                    p_bonus_amount=p_bonus_amount/100;
                                }
                                else
                                    p_bonus_amount=Balancerec.QTY*Bonus_Rate;   //per liter

                                total_quantity=total_quantity+Balancerec.QTY;
                                p_bonus_amount=p_bonus_amount/100;

                                total_amount=total_amount+Balancerec.CREDIT_AMT;
                                p_total_bonus=p_total_bonus+p_bonus_amount;

                                if(printertype=='T')
                                    Print_Bonus_Record(p_member_code,p_bonus_amount,0x42+tempi);
                                else
                                    Ext_Print_Bonus_Record(p_member_code,p_bonus_amount,tempi+0x42);

                            }

                        }
                    }
                }
            }
        }
    }while(no_members>=100);

    if(print_start==1)
    {
        if(printertype=='T')
            Print_Bonus_Totals(total_quantity,total_amount,p_total_bonus);
        else
        {
            Ext_Print_Bonus_Totals(total_quantity,total_amount,p_total_bonus);            
        }
    }
    else
    {
        no_data();
    }
    ZERO=0;
}
