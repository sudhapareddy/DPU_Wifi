#include "delay32.h"

# define	NOP  asm("nop");

# define   	DSDA    TRISAbits.TRISA3
# define   	DSCL    TRISAbits.TRISA2

# define   	SDA     PORTAbits.RA3
# define   	SCL     PORTAbits.RA2

# define 	RTCSEC	0
# define 	RTCMIN	1
# define 	RTCHRS	2
# define 	RTCDAY	4
# define 	RTCMON	5
# define 	RTCYRS	6
# define 	RTCCON	7




void InitRTC(UINT8 write_check)
{    
   UINT8 dat;

    DSDA=0;
    NOP;
    NOP;
    DSCL=0;
    NOP;
    NOP;
    SCL=0;
    NOP;
    NOP;
    SDA=0;
    NOP;
    NOP;

    if(write_check==1)
    {
        SEERAMWriteByte(RTCSEC,RTCCON,0x13,0xD0);
        dat=SEERAMReadByte(0,0,0xD0);
        dat=dat&0x7F;
        SEERAMWriteByte(RTCSEC,RTCSEC,dat,0xD0);
    }
}

void SEERAMStart(void)
{	 
    SCL=1;
    NOP;
    NOP;NOP;
    NOP;

    SDA=1;
    NOP;
    NOP;NOP;
    NOP;

    SDA=0;
    NOP;
    NOP;NOP;
    NOP;

    SCL=0;
    NOP;
    NOP;NOP;
    NOP;

}

void SEERAMStop(void)
{
    SCL=1;
    NOP;
    NOP;
NOP;
    NOP;
    SDA=0;
    NOP;
    NOP;NOP;
    NOP;

    SDA=1;
    NOP;
    NOP;NOP;
    NOP;

    SCL=0;
    NOP;
    NOP;NOP;
    NOP;

    SDA=0;
    NOP;
    NOP;NOP;
    NOP;

}
UINT8  Write1(UINT8 wcn)
{
    UINT8 ackn,TBuf;

    TBuf=wcn;
    wcn=0x08;

    while(wcn>0)
    {
        if ((TBuf & 0x80) == 0x80)	/* check if MSB is high */
            SDA = 1;
        else
            SDA = 0;			/* if not, set to low */

        TBuf = (TBuf << 1);		/* shift 1 place for next bit */
        NOP;
        NOP;
NOP;
        NOP;
        SCL=1;
        NOP;
        NOP;
        NOP;
        NOP;NOP;
        NOP;

        SCL=0;
        NOP;
        NOP;
        NOP;NOP;
        NOP;

        NOP;
        wcn--;
    }
NOP;
        NOP;
    SDA=1;
    NOP;
    NOP;NOP;
        NOP;

    DSDA=1;
    NOP;
    NOP;NOP;
        NOP;

    SCL=1;
    NOP;
    NOP;NOP;
        NOP;

    NOP;
    NOP;
    NOP;
    NOP;NOP;
        NOP;

    ackn= SDA;

    SCL=0;
    NOP;NOP;
        NOP;
    NOP;

    TBuf=0x00;
    NOP;
    NOP;NOP;
        NOP;

    DSDA=0;
    NOP;
    NOP;NOP;
        NOP;

    NOP;
    return ackn;
}

void Set_RTC(void)
{
    UINT8 ti;

    for(ti=0;ti<7;ti++)
        SEERAMWriteByte(0,ti,Time[ti],0xD0);
}

void Read_RTC(void)
{
    UINT8 ti;

    while(1)
    {
        for(ti=0;ti<7;ti++)
            Time[ti]=SEERAMReadByte(0,ti,0xD0);

        if(Time[0]>0x59 || Time[1]>0x59 || Time[2]>0x23 || Time[4]>0x31 || Time[5]>0x12 || Time[6]>0x99)
            InitRTC(1);
        else
            break;
    }
}

void SEERAMReadString(UINT32 adrr,UINT16 nob,char *Buf)
{
    char *i;
    UINT8 didw;
    
    while (nob > 0)
    {
        if(adrr<=65535)
            didw=0xA0;
        else
            didw=0xA2;	//0xA2 for 1024 && 0xA4 for 512

        i = (BYTE *)&adrr;

        *Buf++ = SEERAMReadByte(*(i+1),*(i),didw);

        adrr++;
        nob--;
    }
}

void SEERAMWriteString(UINT32 adr,UINT16 nob,char *Buf)
{
    char *i1;
    UINT8 didw;

    while (nob >0)
    {
        if(adr<=65535)
            didw=0xA0;
        else
            didw=0xA2;		//0xA2 for 1024 && 0xA4 for 512;

        i1 = (BYTE *)&adr;

        SEERAMWriteByte(*(i1+1),*(i1),*Buf++,didw);

        adr++;
        nob--;
    }
}
void ACK_Poll(UINT8 DIDW)
{
   UINT8 result;                    // Polling result

    result = 1;                     // Initialize result

    do
    {
        SEERAMStart();              // Generate Start condition
        result = Write1(DIDW);      // Output control byte

    } while (result == 1);

    SEERAMStop();                   // Generate Stop condition
} 

void SEERAMWriteByte(UINT8 adrh,UINT8 adrl,UINT8 data,UINT8 didw)
{
    SEERAMStart();
    Write(didw);

    if(didw!=0xD0)
        Write(adrh);

    Write(adrl);
    Write(data);

    SEERAMStop();

    ACK_Poll(didw);
	
}	
UINT8 SEERAMReadByte(UINT8 adrh,UINT8 adrl,UINT8 didw)
{
    UINT8 RAMBuf;

    SEERAMStart();
    Write(didw);

    if(didw !=0xD0)
        Write(adrh);

    Write(adrl);
    SEERAMStart();
    Write(didw+1);
    DSDA=1;
    RAMBuf=Read();

    SEERAMStop();

    return RAMBuf;
}		

UINT8 Read(void)
{
    UINT8 temp,TBuf,RAMBuf;

    TBuf=0x08;
    RAMBuf=0x00;

    while(TBuf>0)
    {
        SCL=1;
        NOP;
        NOP;
NOP;
    NOP;
        DSDA=1;
        NOP;
        NOP;NOP;
    NOP;

        delay();
        temp=SDA;
        NOP;
        NOP;NOP;
    NOP;

        RAMBuf = (RAMBuf << 1);                 /* shift 1 place to the left or shift in 0 */
        temp = SDA;                             /* save input */

        if (temp == 1)                          /* check to see if bit is high */
            RAMBuf = RAMBuf | 0x01;		/* if high, make bit high */

        NOP;
        NOP;NOP;
    NOP;

        SCL=0;
        NOP;
        NOP;NOP;
    NOP;

        delay();
        TBuf--;
    }
NOP;
    NOP;
    DSDA=0;
    NOP;
    NOP;NOP;
    NOP;

    return RAMBuf;
}

void Write(UINT8 wc)
{
    UINT8 TBuf;
    TBuf=wc;
    wc=0x08;

    while(wc>0)
    {
        if ((TBuf & 0x80) == 0x80)	/* check if MSB is high */
            SDA = 1;
        else
            SDA = 0;			/* if not, set to low */

        TBuf = (TBuf << 1);		/* shift 1 place for next bit */
        NOP;
        NOP;NOP;
    NOP;

        SCL=1;
        NOP;
        NOP;
        NOP;
        NOP;NOP;
    NOP;

        SCL=0;
        NOP;
        NOP;
        NOP;NOP;
    NOP;

        NOP;
        wc--;
    }
NOP;
    NOP;
    NOP;
    NOP;
    SDA=1;
    NOP;
    NOP;NOP;
    NOP;
NOP;
    NOP;
    DSDA=1;
    NOP;
    NOP;NOP;
    NOP;
    NOP;
    NOP;

    SCL=1;
    NOP;
    NOP;NOP;
    NOP;

    NOP;
    NOP;
    NOP;
    NOP;NOP;
    NOP;

    while(SDA);
NOP;
    NOP;
    SCL=0;
    NOP;
    NOP;

    TBuf=0x00;
    NOP;
    NOP

    DSDA=0;
    NOP;
    NOP;

    NOP;
}
