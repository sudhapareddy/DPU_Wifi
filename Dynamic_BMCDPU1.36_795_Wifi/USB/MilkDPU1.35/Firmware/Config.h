/* 
 * File:   Config.h
 * Author: sys
 *
 * Created on September 17, 2014, 10:15 AM
 */

 #pragma config UPLLEN   = ON            // USB PLL Enabled
 #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF//20v
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1 


/* #pragma config UPLLEN   = ON            // USB PLL Enabled
 #pragma config UPLLIDIV = DIV_4         // USB PLL Input Divider

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_1, FPLLODIV = DIV_4, FWDTEN = OFF//20v
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1*/


#define CLK_SYS (80000000L)//8

# define        BUZZER_IO       TRISEbits.TRISE7
# define        BUZZER          PORTEbits.RE7

//# define        PWRLED_IO       TRISGbits.TRISG14
//# define        PWRLED          PORTGbits.RG14
UINT32 SYS_FREQ;
                             // State machine state of the attached device.
void DelayMs(UINT8 delay)
{
    UINT16 delaycount;

    while(delay--)
    {
        delaycount=1000;
        while(delaycount--);
    }
}

void Delay_250Ms(UINT8 delay)
{    
    while(delay--)
    {
        DelayMs(250);
    }
}

void Delay_Sec(UINT8 delay)
{
    while(delay--)
        Delay_250Ms(4);
}
