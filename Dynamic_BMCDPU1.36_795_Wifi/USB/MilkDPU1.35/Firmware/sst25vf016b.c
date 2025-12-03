/*Software Driver

SST25VF016B 16 Mbit(2M x 8) Serial Flash Memory

February 19th, 2009, Rev. 1.1

ABOUT THE SOFTWARE
This application note provides software driver examples for SST25VF016B,
Serial Flash. Extensive comments are include/*d in each routine to describe
the function of each routine.  The interface coding uses polling method
rather than the SPI protocol to interface with these serial devices.  The
functions are differentiated below in terms of the communication protocols
(uses Mode 0) and specific device operation instructions. This code has been
designed to compile using the Keil compiler.


ABOUT THE SST25VF016B

Companion product datasheets for the SST25VF016B should be reviewed in
conjunction with this application note for a complete understanding
of the device.


Device Communication Protocol(pinout related) functions:

Functions                    		Function
------------------------------------------------------------------
init					Initializes clock to set up mode 0.
Send_Byte				Sends one byte using SI pin to send and
					shift out 1-bit per clock rising edge
Get_Byte				Receives one byte using SO pin to receive and shift
					in 1-bit per clock falling edge
Poll_SO					Used in the polling for RY/BY# of SO during AAI programming
CE_High					Sets Chip Enable pin of the serial flash to high
CE_Low					Clears Chip Enable of the serial flash to low
Hold_Low				Clears Hold pin to make serial flash hold
Unhold					Unholds the serial flash
WP_Low					Clears WP pin to make serial flash write protected
UnWP					Disables write protection pin

Note:  The pin names of the SST25VF016B are used in this application note. The associated test code
will not compile unless these pinouts (SCK, SI, SO, SO, CE, WP, Hold) are pre-defined on your
software which should reflect your hardware interfaced.


Device Operation Instruction functions:

Functions                    		Function
------------------------------------------------------------------
Read_Status_Register			Reads the status register of the serial flash
EWSR					Enables the Write Status Register
WRSR					Performs a write to the status register
WREN					Write enables the serial flash
WRDI					Write disables the serial flash
EBSY					Enable SO to output RY/BY# status during AAI programming
DBSY					Disable SO to output RY/BY# status during AAI programming
Read_ID					Reads the manufacturer ID and device ID
Jedec_ID_Read				Reads the Jedec ID
Read					Reads one byte from the serial flash and returns byte(max of 25 MHz CLK frequency)
Read_Cont				Reads multiple bytes(max of 25 MHz CLK frequency)
HighSpeed_Read				Reads one byte from the serial flash and returns byte(max of 50 MHz CLK frequency)
HighSpeed_Read_Cont			Reads multiple bytes(max of 50 MHz CLK frequency)
Byte_Program				Program one byte to the serial flash
Auto_Add_IncA				Initial Auto Address Increment process
Auto_Add_IncB				Successive Auto_Address_Increment process after AAI initiation
Auto_Add_IncA_EBSY			Initial Auto Address Increment process with EBSY
Auto_Add_IncB_EBSY			Successive Auto_Address_Increment process after AAI initiation with EBSY
Chip_Erase				Erases entire serial flash
Sector_Erase				Erases one sector (4 KB) of the serial flash
Block_Erase_32K				Erases 32 KByte block memory of the serial flash
Block_Erase_64K				Erases 64 KByte block memory of the serial flash
Wait_Busy				Polls status register until busy bit is low
Wait_Busy_AAI				Polls status register until busy bit is low for AAI programming
WREN_Check				Checks to see if WEL is set
WREN_AAI_Check				Checks to see if WEL and AAI mode is set




"C" LANGUAGE DRIVERS
*/
/********************************************************************/
/* Copyright Silicon Storage Technology, Inc. (SST), 1994-2009	    */
/* Example "C" language Driver of SST25VF016B Serial Flash	    */
/* Conrado Canio, Silicon Storage Technology, Inc.                  */
/*                                                                  */
/* Revision 1.0, August 1st, 2005			  	    */
/* Revision 1.1, February 19th, 2009				    */
/*      - Modified Wait_Busy() and Wait_Busy_AAI() procedures	    */
/*	 to Work for both protected and unprotected Flash	    */
/*                                                                  */
/*								    */
/********************************************************************/
#include <p32xxxx.h>


//#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
//#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1
//#define SYS_FREQ (80000000L)

#define SCK     PORTDbits.RD15
#define SCKD    TRISDbits.TRISD15

#define SO	PORTFbits.RF2
#define SOD	TRISFbits.TRISF2

#define SI	PORTFbits.RF8
#define SID	TRISFbits.TRISF8

//#define CE2	PORTCbits.RC2       //U5
//#define CED2	TRISCbits.TRISC2

//#define CE4	PORTDbits.RD14      //(U8)
//#define CED4	TRISDbits.TRISD14

//#define CE4	PORTBbits.RB11      //(U8)
//#define CED4	TRISBbits.TRISB11

#define CE1	PORTCbits.RC1       // U6
#define CED1	TRISCbits.TRISC1

//#define CE3	PORTCbits.RC3       //U7
//#define CED3	TRISCbits.TRISC3


//UINT32 devid;

void init();
void Send_Byte(BYTE out);
BYTE Get_Byte();
void Poll_SO();
void CE_High(void);
void CE_Low(void);
void Hold_Low();
void Unhold();
void WP_Low();
void UnWP();
BYTE Read_Status_Register();
void EWSR(void);
void WRSR(BYTE);
void WREN(void);
void WRDI(void);
void EBSY(void);
void DBSY(void);
BYTE Read_ID(BYTE ID_addr);
UINT32 Jedec_ID_Read(void);
BYTE Reads(UINT32 Dst);
void Read_Cont(UINT32 Dst, UINT32 no_bytes);
//BYTE HighSpeed_Read(UINT32 Dst);
void HighSpeed_Read_Cont(UINT32 Dst, UINT32 no_bytes);
void Byte_Program(UINT32 Dst, BYTE byte);
void Auto_Add_IncA(UINT32 Dst, BYTE byte1, BYTE byte2);
void Auto_Add_IncB(BYTE byte1, BYTE byte2);
void Auto_Add_IncA_EBSY(UINT32 Dst, BYTE byte1, BYTE byte2);
void Auto_Add_IncB_EBSY(BYTE byte1, BYTE byte2);
void Chip_Erase(void);

void Block_Erase_32K(UINT32 Dst);
void Block_Erase_64K(UINT32 Dst);
void Wait_Busy(void);
void Wait_Busy_AAI(void);
void WREN_Check(void);
void WREN_AAI_Check(void);

void Verify(BYTE byte, BYTE cor_byte);
//BYTE readbuf[128];
//BYTE  upper_128[128];
/* global array to store read data */
					/* to upper RAM area from 80H - FFH */

/************************************************************************/
/* PROCEDURE: init							*/
/*									*/
/* This procedure initializes the SCK to low. Must be called prior to 	*/
/* setting up mode 0.							*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		SCK							*/
/************************************************************************/

/************************************************************************/
/* PROCEDURE: Send_Byte							*/
/*									*/
/* This procedure outputs a byte shifting out 1-bit per clock rising	*/
/* edge on the the SI pin(LSB 1st).					*/
/*									*/
/* Input:								*/
/*		out							*/
/*									*/
/* Output:								*/
/*		SI							*/
/************************************************************************/
void Send_Byte(BYTE sout)
{
    UINT8 bit_cnt;// = 0;

    for (bit_cnt = 0; bit_cnt < 8; bit_cnt++)
    {
        if ((sout & 0x80) == 0x80)	/* check if MSB is high */
            SI = 1;
        else 
            SI = 0;			/* if not, set to low */
        
        SCK = 1;			/* toggle clock high */
        sout = (sout << 1);		/* shift 1 place for next bit */

        SCK = 0;			/* toggle clock low */
    }
}

/************************************************************************/
/* PROCEDURE: Get_Byte							*/
/*									*/
/* This procedure inputs a byte shifting in 1-bit per clock falling	*/
/* edge on the SO pin(LSB 1st).						*/
/*									*/
/* Input:								*/
/*		SO							*/
/*									*/
/* Output:								*/
/*		None							*/
/************************************ ************************************/
BYTE Get_Byte(void)
{
    BYTE bit_cnt = 0, in = 0, temp_bit = 0;

    for (bit_cnt = 0; bit_cnt < 8; bit_cnt++)
    {
        in = (in << 1);		/* shift 1 place to the left or shift in 0 */
        temp_bit = SO;		/* save input */
        SCK = 1;		/* toggle clock high */
        if (temp_bit == 1)			/* check to see if bit is high */
            in = in | 0x01;		/* if high, make bit high */

        SCK = 0;		/* toggle clock low */

    }
    return in;
}

void CE_High(void)
{
        CE1 = 1;

}

/************************************************************************/
/* PROCEDURE: CE_Low							*/
/*									*/
/* This procedure drives the CE of the device to low.  			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		CE							*/
/*									*/
/************************************************************************/
void CE_Low(void)
{
        CE1 = 0;

}



/************************************************************************/
/* PROCEDURE: Hold()							*/
/*									*/
/* This procedure clears the Hold pin to low.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		Hold							*/
/************************************************************************/
/*void Hold_Low(void)
{
//	Hold = 0;			//* clear Hold pin 
}*/

/************************************************************************/
/* PROCEDURE: Unhold()							*/
/*									*/
/* This procedure sets the Hold pin to high.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		Hold							*/
/************************************************************************/
/*void Unhold(void)
{
//	Hold = 1;			// set Hold pin 
}*/

/************************************************************************/
/* PROCEDURE: WP()							*/
/*									*/
/* This procedure clears the WP pin to low.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		WP							*/
/************************************************************************/
/*void WP_Low(void)
{
//	WP = 0;				// clear WP pin 
}*/

/************************************************************************/
/* PROCEDURE: UnWP()							*/
/*									*/
/* This procedure sets the WP pin to high.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		WP							*/
/************************************************************************/
/*void UnWP(void)
{
//	WP = 1;				// set WP pin 
}*/

/************************************************************************/
/* PROCEDURE: Read_Status_Register					*/
/*									*/
/* This procedure read the status register and returns the byte.	*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		byte							*/
/************************************************************************/
BYTE Read_Status_Register(void)
{
    BYTE r_status_byte = 0;

    CE_Low();			/* enable device */
    Send_Byte(0x05);		/* send RDSR command */
    r_status_byte = Get_Byte();		/* receive byte */
    CE_High();			/* disable device */

    return r_status_byte;
}

void Global_Unlock(void)
{
    WREN();
    WREN_Check();
    CE_Low();				// enable device
    Send_Byte(0x98);			//send Chip Erase command (60h or C7h) /
    CE_High();				//disable device /
    Wait_Busy();
}
/************************************************************************/
/* PROCEDURE: EWSR							*/
/*									*/
/* This procedure Enables Write Status Register.  			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void EWSR(void)
{
    CE_Low();			/* enable device */
    Send_Byte(0x50);		/* enable writing to the status register */
    CE_High();			/* disable device */
}

/************************************************************************/
/* PROCEDURE: WRSR							*/
/*									*/
/* This procedure writes a byte to the Status Register.			*/
/*									*/
/* Input:								*/
/*		byte							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void WRSR(BYTE statusbyte)
{
    CE_Low();			/* enable device */
    Send_Byte(0x01);		/* select write to status register */
    Send_Byte(statusbyte);		/* data that will change the status of BPx
                                       or BPL (only bits 2,3,4,5,7 can be written) */
    CE_High();			/* disable the device */
}

/************************************************************************/
/* PROCEDURE: WREN							*/
/*									*/
/* This procedure enables the Write Enable Latch.  It can also be used 	*/
/* to Enables Write Status Register.					*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void WREN(void)
{
    CE_Low();			/* enable device */
    Send_Byte(0x06);		/* send WREN command */
    CE_High();			/* disable device */
}

/************************************************************************/
/* PROCEDURE: Jedec_ID_Read						*/
/*									*/
/* This procedure Reads the manufacturer's ID (BFh), memory type (25h)  */
/* and device ID (41h).  It will use 9Fh as the JEDEC ID command.    	*/
/* Please see the product datasheet for details.  			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		IDs_Read:ID1(Manufacture's ID = BFh, Memory Type (25h), */
/*		 and Device ID (80h)					*/
/*									*/
/************************************************************************/
UINT32 Jedec_ID_Read(void)
{
    UINT32 j_temp_id;

    j_temp_id = 0;

    CE_Low();			 /* enable device */
    Send_Byte(0x9F);		 /* send JEDEC ID command (9Fh) */
    j_temp_id = (j_temp_id | Get_Byte()) << 8; /* receive byte */
    j_temp_id = (j_temp_id | Get_Byte()) << 8;
    j_temp_id = (j_temp_id | Get_Byte()); 	 /* temp value = 0xBF2541 */
    CE_High();			 /* disable device */

    return j_temp_id;
}

/************************************************************************/
/* PROCEDURE:	HighSpeed_Read						*/
/*									*/
/* This procedure reads one address of the device.  It will return the 	*/
/* byte read in variable byte.						*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst:	Destination Address 000000H - 1FFFFFH		*/
/*      								*/
/*									*/
/* Returns:								*/
/*		byte							*/
/*									*/
/************************************************************************/
/*BYTE HighSpeed_Read1(UINT32 dst)
{
    BYTE read_byte = 0;

    CE_Low();			// enable device 
    Send_Byte(0x0B); 		// read command 
    Send_Byte(((dst & 0xFFFFFF) >> 16));	// send 3 address bytes 
    Send_Byte(((dst & 0xFFFF) >> 8));
    Send_Byte(dst & 0xFF);
    Send_Byte(0xFF);		//dummy byte
    read_byte = Get_Byte();
    CE_High();			// disable device 

    return read_byte;			// return one byte read 
 }
*/

/************************************************************************/
/* PROCEDURE:	HighSpeed_Read_Cont					*/
/*									*/
/* This procedure reads multiple addresses of the device and stores	*/
/* data into 128 byte buffer. Maximum byte that can be read is 128 bytes*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 1FFFFFH	*/
/*      	no_bytes	Number of bytes to read	(max = 128)	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/

/************************************************************************/
/* PROCEDURE:	Byte_Program						*/
/*									*/
/* This procedure programs one address of the device.			*/
/* Assumption:  Address being programmed is already erased and is NOT	*/
/*		block protected.					*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 1FFFFFH	*/
/*		byte:		byte to be programmed			*/
/*      								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Byte_Program(UINT32 dst, BYTE writebyte)
{
    CE_Low();				/* enable device */
    Send_Byte(0x02); 			/* send Byte Program command */
    Send_Byte(((dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
    Send_Byte(((dst & 0xFFFF) >> 8));
    Send_Byte(dst & 0xFF);
    Send_Byte(writebyte);			/* send byte to be programmed */
    CE_High();				/* disable device */

}

/************************************************************************/
/* PROCEDURE: Chip_Erase						*/
/*									*/
/* This procedure erases the entire Chip.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/

void Chip_Erase(void)
{
    WREN();
    WREN_Check();
    CE_Low();				// enable device
    Send_Byte(0xC7);			//send Chip Erase command (60h or C7h) /
    CE_High();				//disable device /
    Wait_Busy();

}

/************************************************************************/
/* PROCEDURE: Sector_Erase						*/
/*									*/
/* This procedure Sector Erases the Chip.				*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 1FFFFFH	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Sector_Erase(UINT32 dst)
{
    WREN();
    WREN_Check();
    
    CE_Low();				/* enable device */
    Send_Byte(0x20);			/* send Sector Erase command */
    Send_Byte(((dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
    Send_Byte(((dst & 0xFFFF) >> 8));
    Send_Byte(dst & 0xFF);
    CE_High();				/* disable device */
    Wait_Busy();
 
}

/************************************************************************/
/* PROCEDURE: Wait_Busy							*/
/*									*/
/* This procedure waits until device is no longer busy (can be used by	*/
/* Byte-Program, Sector-Erase, Block-Erase, Chip-Erase).		*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Wait_Busy(void)
{
    while ((Read_Status_Register() & 0x03) == 0x03)	/* waste time until not busy */
        Read_Status_Register();
    }


/************************************************************************/
/* PROCEDURE: WREN_Check						*/
/*									*/
/* This procedure checks to see if WEL bit set before program/erase.	*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void WREN_Check(void)
{
    UINT8 w_check_byte;

    w_check_byte = Read_Status_Register();	/* read the status register */

    if (w_check_byte != 0x02)		/* verify that WEL bit is set */
    {
        while(1)
        {
            WREN();
            w_check_byte = Read_Status_Register();

            if(w_check_byte == 0x02)
               return;
        }
    }
}

void FLASHWriteString(UINT32 waddr,UINT32 wnobytes,BYTE *wbuf)
{
    EWSR();
    WRSR(0x02);

    while(wnobytes>0)
    {
        WREN();
        WREN_Check();
        Byte_Program(waddr,*wbuf);
        waddr=waddr+1;
        Wait_Busy();
        wbuf++;
        wnobytes--;
    }
}

void FLASHReadString(UINT32 raddr,UINT32 rnobytes,BYTE *wbuf)
{
    UINT32 r_byte_cnt;

    EWSR();
    WRSR(0x02);

    CE_Low();

    Send_Byte(0x0B);
    Send_Byte(((raddr & 0xFFFFFF) >> 16));
    Send_Byte(((raddr & 0xFFFF) >> 8));
    Send_Byte(raddr & 0xFF);
    Send_Byte(0xFF);

    for (r_byte_cnt = 0; r_byte_cnt < rnobytes; r_byte_cnt++)
    {
        *wbuf = Get_Byte();
       
        wbuf++;
    }

    CE_High();
}

void InitFlash(UINT8 writecheck)
{
    SPI3CON=0;
    CED1=0;
    CE1=1;
    SCKD=0;
    SOD=0;

    if(writecheck==1)
    {
        EWSR();
        WRSR(0x02);
        WREN();
        WREN_Check();

        SCK = 0;	/* set clock to low initial state */
    }
    Global_Unlock();
}

void SEERAMReadString(UINT32 radrr,UINT16 rnob,char *rbuf)
{
    if(radrr<500)
        FLASHReadString((Global_Variables_Loc+radrr),rnob,rbuf);

    else if(radrr>=500 && radrr<1750)
        FLASHReadString((Global_Variables2_Loc+radrr-500),rnob,rbuf);

    else if(radrr>=1750 && radrr<3100)//3000)//2900 //2850)
        FLASHReadString((SMS_Counter_Loc+radrr-1750),rnob,rbuf);
 }

void SEERAMWriteString(UINT32 wadr,UINT16 wnob,BYTE *wbuf)
{
    if(wadr<500)
    {
        FLASHReadString(Global_Variables_Loc,500,(char *)&Global_Buffer);

        STRNCPY((char *)&Global_Buffer[wadr],wbuf,wnob);

        Sector_Erase(Temp_Sector);

        FLASHWriteString(Temp_Sector,500,(char *)&Global_Buffer);//
        Global_Buffer[3000]=0x01;
        FLASHWriteString(Temp_Sector+3000,1,(char *)&Global_Buffer[3000]);//2500
        Sector_Erase(Global_Variables_Loc);
        //FLASHWriteString(Global  _Variables_Loc,50,(char *)&Global_Buffer);
        FLASHWriteString(Global_Variables_Loc,500,(char *)&Global_Buffer);
        Global_Buffer[0]=0;
        FLASHWriteString(Temp_Sector+3000,1,(char *)&Global_Buffer);//2500
    }
    else if(wadr>=500 && wadr<1750)
    {
        FLASHReadString(Global_Variables2_Loc,1250,(char *)&Global_Buffer);

        STRNCPY((char *)&Global_Buffer[wadr-500],wbuf,wnob);

        Sector_Erase(Temp_Sector);

        FLASHWriteString(Temp_Sector,1250,(char *)&Global_Buffer);//
        Global_Buffer[3000]=0x02;
        FLASHWriteString(Temp_Sector+3000,1,(char *)&Global_Buffer[3000]);//2500
        Sector_Erase(Global_Variables2_Loc);
        FLASHWriteString(Global_Variables2_Loc,1250,(char *)&Global_Buffer);
        Global_Buffer[0]=0;
        FLASHWriteString(Temp_Sector+3000,1,(char *)&Global_Buffer);//2500
    }
    else if(wadr>=1750 && wadr<3100)//3000)//2900//2850)
    {
        FLASHReadString(SMS_Counter_Loc,1350,(char *)&Global_Buffer);//1250//1150 //21//1100

        STRNCPY((char *)&Global_Buffer[wadr-1750],wbuf,wnob);

        Sector_Erase(Temp_Sector);

        FLASHWriteString(Temp_Sector,1350,(char *)&Global_Buffer);//1150//22,1150
        Global_Buffer[3000]=0x04;
        FLASHWriteString(Temp_Sector+3000,1,(char *)&Global_Buffer[3000]);//2500
        Sector_Erase(SMS_Counter_Loc);
        
        FLASHWriteString(SMS_Counter_Loc,1350,(char *)&Global_Buffer);//1150//100//1100
        Global_Buffer[0]=0;
        FLASHWriteString(Temp_Sector+3000,1,(char *)&Global_Buffer);//
    }
  
    Sector_Erase(Temp_Sector);
}
