/******************************************************************************

Simple POS Demo For Epson Fx-875 Dot Matrix Printer

This is a simple POS printer demo to show how to send commands to a POS
printers.  When a printer attaches, a print sample is sent to the printer.
No other user interaction is required to generate the print-out.

This demo runs on an Explorer 16 (DM240001) with a PIC24FJ256GB110 (USB) PIM
(MA240014) and a USB PICtail Plus Daughter Board (AC164127).

In its initial state, this demo supports the printer:

    * Epson TM-T88IV - thermal receipt printer that uses the ESC/POS printer
        language.  This is a fast thermal printer that supports most of the
        features currently available in the ESC/POS printer language driver.

To modify this demo for a different printer, run the configuration tool
USBConfig.exe to generate the appropriate configuration files usb_config.c and
usb_config.h.

Refer to the Simple Full Sheet Demo for an example of sending commands to a
full sheet printer.

******************************************************************************/

/******************************************************************************

* FileName:         Simple POS Demo.c
* Dependencies:     USB Embedded Host library
* Processor:        PIC24F/PIC32
* Compiler:         C30 v3.10b/C32 v1.02
* Company:          Microchip Technology, Inc.

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "uart2.h"
#include "GraphicsConfig.h"
#include "Graphics/Primitive.h"
#include "usb_config.h"
#include "USB/usb.h"
#include "USB/usb_host_printer.h"
#include "USB/usb_host_printer_esc_pos.h"


// *****************************************************************************
// *****************************************************************************
// Constants
// *****************************************************************************
// *****************************************************************************

#define MAX_ALLOWED_CURRENT                 250


// *****************************************************************************
// *****************************************************************************
// Configuration Bits
// *****************************************************************************
// *****************************************************************************

#if defined __C30__ || defined __XC16__
    #if defined(__PIC24FJ256GB110__)
        _CONFIG2(FNOSC_PRIPLL & POSCMOD_HS & PLL_96MHZ_ON & PLLDIV_DIV2 & IESO_OFF) // Primary HS OSC with PLL, USBPLL /2
        _CONFIG1(JTAGEN_OFF & FWDTEN_OFF & ICS_PGx2)   // JTAG off, watchdog timer off
    #elif defined(__PIC24FJ64GB004__)
        _CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(POSCMOD_HS & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
        _CONFIG3(WPFP_WPFP0 & SOSCSEL_SOSC & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
        _CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)
    #elif defined(__PIC24FJ256GB106__)
        _CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & FWDTEN_OFF & ICS_PGx2) 
        _CONFIG2( PLL_96MHZ_ON & IESO_OFF & FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRIPLL & PLLDIV_DIV3 & IOL1WAY_ON)
    #elif defined(__PIC24FJ256DA210__) || defined(__PIC24FJ256GB210__)
        _CONFIG1(FWDTEN_OFF & ICS_PGx2 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
        _CONFIG2(POSCMOD_HS & IOL1WAY_ON & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
    #else
        #warning "No configuration bits set for this project.  USB needs specific configuration settings.  Please verify that the configuration bits are set correctly."
    #endif

#endif

// *****************************************************************************
// *****************************************************************************
// Data Structures
// *****************************************************************************
// *****************************************************************************

typedef struct
{
    BYTE    printer;
    BYTE    printerAttached     : 1;
    BYTE    pagePrinted         : 1;
    BYTE    overcurrentStateUSB : 1;
} PRINT_STATUS;


// *****************************************************************************
// *****************************************************************************
// Internal Function Prototypes
// *****************************************************************************
// *****************************************************************************

void    MonitorVBUS( void );
#if defined( __C30__ ) || defined __XC16__
    void PrintImagePOS( BYTE __prog__ *ptr, USB_PRINTER_IMAGE_INFO *imageInfo );
#elif defined( __PIC32MX__ )
    void PrintImagePOS( const BYTE *ptr, USB_PRINTER_IMAGE_INFO *imageInfo );
#endif


// *****************************************************************************
// *****************************************************************************
// Macros
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Global Variables
// *****************************************************************************
// *****************************************************************************

char                            buffer[50];
USB_PRINTER_IMAGE_INFO          imageInfo;
extern const IMAGE_FLASH        logoMCHP;
USB_PRINTER_GRAPHICS_PARAMETERS params;
USB_PRINTER_DEVICE_ID           printerInfo;
BYTE                            returnCode;
PRINT_STATUS                    status;


//******************************************************************************
//******************************************************************************
// USB Support Functions
//******************************************************************************
//******************************************************************************

/****************************************************************************
  Function:
    BOOL USB_ApplicationEventHandler( BYTE address, USB_EVENT event,
                void *data, DWORD size )

  Summary:
    This is the application event handler.  It is called when the stack has
    an event that needs to be handled by the application layer rather than
    by the client driver.

  Description:
    This is the application event handler.  It is called when the stack has
    an event that needs to be handled by the application layer rather than
    by the client driver.  If the application is able to handle the event, it
    returns TRUE.  Otherwise, it returns FALSE.

  Precondition:
    None

  Parameters:
    BYTE address    - Address of device where event occurred
    USB_EVENT event - Identifies the event that occured
    void *data      - Pointer to event-specific data
    DWORD size      - Size of the event-specific data

  Return Values:
    TRUE    - The event was handled
    FALSE   - The event was not handled

  Remarks:
    The application may also implement an event handling routine if it
    requires knowledge of events.  To do so, it must implement a routine that
    matches this function signature and define the USB_HOST_APP_EVENT_HANDLER
    macro as the name of that function.
  ***************************************************************************/

//******************************************************************************
//******************************************************************************
// Internal Functions
//******************************************************************************
//******************************************************************************

/****************************************************************************
  Function:
     void InitializeVbusMonitor( void )

  Description:
    This function initializes monitoring of the VBUS analog input.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/

void InitializeVbusMonitor( void )
{
    #if defined( __C30__) || defined __XC16__
        // Set up the A/D converter
        #if defined(__PIC24FJ256DA210__) 
            //The over current detection of the PIC24FJ256DA210 development board is
            //  not connected by default.
        #elif defined(__PIC24FJ256GB210__)
            ANSBbits.ANSB8 = 1;
        #else
            AD1PCFGL    = 0xFEFF;       // Disable digital input on AN8
        #endif
        AD1CSSL     = 0x0000;       // No scan
        AD1CHS      = 0x0008;       // Mux A uses AN8
        AD1CON3     = 0x1F05;       // 31 Tad auto-sample, Tad = 5*Tcy
        AD1CON2     = 0x0000;       // AVdd, AVss, MUX A only
        AD1CON1     = 0x80E4;       // Turn on, auto sampling, auto convert
    #endif

    return;
}



/****************************************************************************
  Function:
    void MonitorVBUS( void )

  Description:
    This routine monitors VBUS to check for overcurrent conditions.  The
    Explorer 16 with the PIC24FJ256GA110 PIM and USB PICtail Plus has an
    analog input dedicated to monitoring Vbus.  The voltage on Vbus is
    divided by two and fed into RB8/AN8.  The valid range for Vbus is
    4.4V - 5.25V.  If we are in an overcurrent condition, Vbus will be
    lower than 4.4V.  Full range on the A/D is 0x3FF for 3.3V.  So any
    value lower than the value calculated below on AN8 is overcurrent.
    Otherwise, the current level is fine.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    TRUE    - An overcurrent situation exists. USB shut down.
    FALSE   - Normal USB operation.

  Remarks:
    Since Vbus monitoring is application-specific, it is the application's
    responsibility.

    If we get an overcurrent, we must shut down immediately to avoid brownout
    or blackout.
  ***************************************************************************/

#define ADC_READING_VBUS            ADC1BUF0
#define OVERCURRENT_RESET_VOLTAGE   4800ul        // In millivolts
#define OVERCURRENT_TRIP_VOLTAGE    4750ul        // In millivolts
#define SYSTEM_VOLTAGE              3300ul        // In millivolts
#define OVERCURRENT_RESET_READING   (1023ul * (OVERCURRENT_RESET_VOLTAGE/2) / SYSTEM_VOLTAGE)
#define OVERCURRENT_TRIP_READING    (1023ul * (OVERCURRENT_TRIP_VOLTAGE/2) / SYSTEM_VOLTAGE)

void MonitorVBUS( void )
{
    #if defined( __PIC24FJ256GB110__)
        if (AD1CON1bits.DONE)
        {
            if (ADC_READING_VBUS < OVERCURRENT_TRIP_READING)
            {
                USBHostVbusEvent( EVENT_VBUS_OVERCURRENT, USB_ROOT_HUB, 0 );
                if (!status.overcurrentStateUSB)
                {
                    UART2PrintString( "\r\n***** USB Error - overcurrent detected *****\r\n" );
                    status.overcurrentStateUSB = TRUE;
                }
            }
            else if (ADC_READING_VBUS > OVERCURRENT_RESET_READING)
            {
                if (status.overcurrentStateUSB)
                {
                    USBHostVbusEvent( EVENT_VBUS_POWER_AVAILABLE, USB_ROOT_HUB, 0 );
                    UART2PrintString( "\r\n***** USB overcurrent condition removed *****\r\n" );
                }
                status.overcurrentStateUSB = FALSE;
            }
            AD1CON1     = 0x80E4;       // Start the next conversion.
        }
    #endif
}



/****************************************************************************
  Function:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/




//******************************************************************************
//******************************************************************************
//******************************************************************************
// Main
//******************************************************************************
//******************************************************************************
//******************************************************************************

BOOL Initialize_Printer(void)
{
    UINT8 i_temp_status;
    UINT32 i_temp_count;
    //setUSBDevice('P');

    MonitorVBUS();
    USBTasks();


    USBInitialize(0);
    InitializeVbusMonitor();
    
    i_temp_status=USBHostResetDevice(printerInfo.deviceAddress);

    if(i_temp_status==USB_SUCCESS)
        return TRUE;
    
    i_temp_count=0;

    Please_Wait_Msg_Small(LINE7);

    while(i_temp_count<90000)
    {
        MonitorVBUS();
        USBTasks();
        if (status.printerAttached)
        {          
            USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_FONT_NAME, USB_NULL, USB_PRINTER_FONT_POS_12x24, 0 ); // Font B
            USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_POS_JUSTIFICATION_LEFT, USB_NULL, 0, 0 );
            USBHostPrinterCommandWithReadyWait( &returnCode, printerInfo.deviceAddress, USB_PRINTER_FONT_MEDIUM, USB_NULL, 0, 0 );
           /* 
            // commented on 02/03/2022 
            * if(printerInfo.vid==0x0E75)// && printerInfo.pid==0x1101)
                Ext_Print_L   =40;
            else
                Ext_Print_L   =80;//210218 sudhakar171019*/
            
           
            
            return TRUE;
        }

        i_temp_count++;
    }

    CLEAR_DATA_LINES_FROM(5,6);
    strcpy(LBUF,"Check Ext. PRINTER ");
    Disp_Msg(LINE5);

    USBHostShutdown();  

    return FALSE;
}


/*******************************************************************************
Function:       void __attribute__((__interrupt__, auto_psv)) _XXX(void)

These are just here to catch any spurious interrupts that we see during
debugging.

*******************************************************************************/
#if defined( __C30__ ) || defined __XC16__

void __attribute__((interrupt, auto_psv)) _DefaultInterrupt(void)
{
  UART2PrintString( "!!! Default interrupt handler !!!\r\n" );
  while (1)
  {
      Nop();
      Nop();
      Nop();
  }
}

void __attribute__((interrupt, auto_psv)) _OscillatorFail(void)
{
  UART2PrintString( "!!! Oscillator Fail interrupt handler !!!\r\n" );
  while (1)
  {
      Nop();
      Nop();
      Nop();
  }
}
void __attribute__((interrupt, auto_psv)) _AddressError(void)
{
  UART2PrintString( "!!! Address Error interrupt handler !!!\r\n" );
  while (1)
  {
      Nop();
      Nop();
      Nop();
  }
}
void __attribute__((interrupt, auto_psv)) _StackError(void)
{
  UART2PrintString( "!!! Stack Error interrupt handler !!!\r\n" );
  while (1)
  {
      Nop();
      Nop();
      Nop();
  }
}
void __attribute__((interrupt, auto_psv)) _MathError(void)
{
  UART2PrintString( "!!! Math Error interrupt handler !!!\r\n" );
  while (1)
  {
      Nop();
      Nop();
      Nop();
  }
}

#endif



