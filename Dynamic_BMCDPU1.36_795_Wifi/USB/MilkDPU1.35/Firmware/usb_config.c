/*
********************************************************************************
                                                                                
Software License Agreement                                                      
                                                                                
Copyright � 2007-2008 Microchip Technology Inc.  All rights reserved.           
                                                                                
Microchip licenses to you the right to use, modify, copy and distribute Software
only when embedded on a Microchip microcontroller or digital signal controller  
that is integrated into your product or third party product (pursuant to the    
sublicense terms in the accompanying license agreement).                        
                                                                                
You should refer to the license agreement accompanying this Software for        
additional information regarding your rights and obligations.                   
                                                                                
SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,   
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF        
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.  
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER       
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR    
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES         
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR     
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF        
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES          
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.     
                                                                                
********************************************************************************
*/

// Created by the Microchip USBConfig Utility, Version 2.0.0.0, 11/18/2008, 8:08:56

#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "USB/usb.h"
#include "USB/usb_host_msd.h"
#include "USB/usb_host_msd_scsi.h"
#include "USB/usb_host_printer.h"
#include "USB/usb_host_printer_esc_pos.h"
// *****************************************************************************
// Media Interface Function Pointer Table for the Mass Storage client driver
// *****************************************************************************

USB_PRINTER_INTERFACE usbPrinterClientLanguages[] =
{
    {   // ESC/POS
        USBHostPrinterLanguageESCPOS,
        USBHostPrinterLanguageESCPOSIsSupported
    }
    ,
    {
        NULL,
        NULL
    }
};
/*
CLIENT_DRIVER_TABLE usbClientDrvTable[] =
{
    {
        USBHostPrinterInitialize,
        USBHostPrinterEventHandler,
        0
    }
};
*/

USB_PRINTER_SPECIFIC_INTERFACE usbPrinterSpecificLanguage[] =
{
    { 0x04B8ul, 0x0005ul, 0, {LANGUAGE_SUPPORT_FLAGS_ESCPOS} }   // Epson FX875
    ,
    { 0x04E8ul, 0x1101ul , 0, {LANGUAGE_SUPPORT_FLAGS_ESCPOS}} // sAMSUNG ViD
    ,
    { 0x03F0ul, 0x1101ul ,0, {LANGUAGE_SUPPORT_FLAGS_ESCPOS} } // HP
    ,
    { 0x04A9ul, 0x1101ul, 0, {LANGUAGE_SUPPORT_FLAGS_ESCPOS} } // canon
    ,
    { 0x0E75ul, 0x1101ul, 0, {LANGUAGE_SUPPORT_FLAGS_ESCPOS} }   // TVS RP45 40 Column
    ,
    { 0x0000, 0x0000, 0, {0} }
};


CLIENT_DRIVER_TABLE usbMediaInterfaceTable =
{
    
    USBHostMSDSCSIInitialize,
    USBHostMSDSCSIEventHandler,
    0
};

// *****************************************************************************
// Client Driver Function Pointer Table for the USB Embedded Host foundation
// *****************************************************************************

CLIENT_DRIVER_TABLE usbClientDrvTable[] =
{                                        
    
    
    {
        USBHostMSDInitialize,
        USBHostMSDEventHandler,
        0
    }
    ,
    {
        USBHostMSDSCSIInitialize,
    USBHostMSDSCSIEventHandler,
    0}
    ,
    
    {
        USBHostPrinterInitialize,
        USBHostPrinterEventHandler,
                0
    }
 

    
};

// *****************************************************************************
// USB Embedded Host Targeted Peripheral List (TPL)
// ********************** { INIT_VID_PID( 0x04B8ul, 0x0005ul ), 0, 0, {0} } // Epson Fx-875*******************************************************
USB_TPL usbTPL[]=
{
      { INIT_CL_SC_P( 8ul, 6ul, 0x50ul ), 0, 0, {TPL_CLASS_DRV} } // Most types of MSD flash drives
    ,
    { INIT_CL_SC_P( 8ul, 5ul, 0x50ul ), 0, 1, {TPL_CLASS_DRV} } // Some MSD flash drives use this instead
    ,
    { INIT_VID_PID( 0x04B8ul, 0x0005ul ), 0, 2, {0} } // Epson Fx-875 80 column
    ,
    { INIT_VID_PID( 0x04E8ul, 0x1101ul ), 1, 3, {0} } // sAMSUNG ViD
     ,
    { INIT_VID_PID( 0x03F0ul, 0x1101ul ), 0, 2, {0} } // HP
    ,
    { INIT_VID_PID( 0x04A9ul, 0x1101ul ), 1, 3, {0} } // canon
    ,
    { INIT_VID_PID( 0x0E75ul, 0x1101ul ), 0, 2, {0} } // TVS RP-45 40 column

    
    
    

    
    
    
    

};



