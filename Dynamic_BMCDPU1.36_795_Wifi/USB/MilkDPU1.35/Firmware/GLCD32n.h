# define	dispOFF		0x3e
# define 	dispON		0x3f

# define 	yADDR		0x40	//start address
# define 	xADDR		0xb8	//start address
# define 	lineNO		0xC0	//first diplay line

# define 	LCDnormdat	0x0800
# define 	NONE		0
# define 	HALF1		1
# define 	HALF2		2

# define	LINE1		0
# define	LINE2		20
# define 	LINE3		40
# define 	LINE4		60
# define 	LINE5		80
# define 	LINE6		100
# define 	LINE7		120
# define 	LINE8		140

# define 	LCDPORT		PORTD			//write date to this port with upper byte 0 (OUTPUT)

# define 	LCDATA0		PORTDbits.RD0	//)
# define 	LCDATA1		PORTDbits.RD1	//)
# define 	LCDATA2		PORTDbits.RD2	//)
# define 	LCDATA3		PORTDbits.RD3	//)--LCD DATA ports for REGISTER & RAM filling
# define 	LCDATA4		PORTDbits.RD4	//)
# define 	LCDATA5		PORTDbits.RD5	//)
# define 	LCDATA6		PORTDbits.RD6	//)
# define 	LCDATA7		PORTDbits.RD7	//)

# define 	LCDCS1		PORTGbits.RG12	//)
# define 	LCDCS2		PORTAbits.RA6	//)
# define 	LCDBL		PORTAbits.RA1	//)--Assertain these control lines as required to
# define 	LCDRST		PORTAbits.RA7	//)  pass in the information to LCD MODULE (OUTPUT)
# define 	LCDEN		PORTGbits.RG13	//)
//# define        LCDEN2          PORTGbits.RG14
# define 	LCDRS		PORTGbits.RG15	//)
# define 	LCDRW		PORTEbits.RE0	//)

# define 	LCDATA0_IO	TRISDbits.TRISD0
# define 	LCDATA1_IO	TRISDbits.TRISD1
# define 	LCDATA2_IO	TRISDbits.TRISD2
# define 	LCDATA3_IO	TRISDbits.TRISD3
# define 	LCDATA4_IO	TRISDbits.TRISD4
# define 	LCDATA5_IO	TRISDbits.TRISD5
# define 	LCDATA6_IO	TRISDbits.TRISD6
# define 	LCDATA7_IO	TRISDbits.TRISD7

# define 	LCDCS1_IO	TRISGbits.TRISG12
# define 	LCDCS2_IO	TRISAbits.TRISA6
# define 	LCDBL_IO	TRISAbits.TRISA1
# define 	LCDRST_IO	TRISAbits.TRISA7
# define 	LCDEN_IO	TRISGbits.TRISG13
//# define 	LCDEN2_IO	TRISGbits.TRISG14
# define 	LCDRS_IO	TRISGbits.TRISG15
# define 	LCDRW_IO	TRISEbits.TRISE0

# define 	LCDDATA 	PORTD

# define        PWRLED          PORTGbits.RG14
# define 	PWRLED_IO	TRISGbits.TRISG14



void LCDreset (void);
void LCDdispOFF(void);
void LCDdispON(void);
void LCDwriteREG(UINT8);
void LCDwriteRAM(UINT8);
void LSet_Cursor(UINT8);
void Set_Cursor(UINT8);				//sets start cursor position
void LCDfillPATT(UINT8,UINT8);		//fills complete display with given pattern
void Display_Char(UINT8);					//writes charecter on the LCD display
void Disp_Msg(UINT8);				//writes string to GLCD

