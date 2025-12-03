
#include <plib.h>

#define GetSystemClock()           (SYS_FREQ)
//#define GetPeripheralClock()       (SYS_FREQ/1)
//#define GetInstructionClock()      (SYS_FREQ)
#define I2C_CLOCK_FREQ             500//500

// EEPROM Constants
#define EEPROM_I2C_BUS              I2C2
#define EEPROM_ADDRESS              0x68        // 0b1010000 Serial EEPROM address


BOOL RTCStartTransfer( BOOL restart )
{
    I2C_STATUS  status;

    // Send the Start (or Restart) signal
    if(restart)
    {
        I2CRepeatStart(EEPROM_I2C_BUS);
    }
    else
    {
        // Wait for the bus to be idle, then start the transfer
        while( !I2CBusIsIdle(EEPROM_I2C_BUS) );

        if(I2CStart(EEPROM_I2C_BUS) != I2C_SUCCESS)
        {
           // DBPRINTF("Error: Bus collision during transfer Start\n");
            return FALSE;
        }
    }

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(EEPROM_I2C_BUS);

    } while ( !(status & I2C_START) );

    return TRUE;
}


/*******************************************************************************
  Function:
    BOOL RTCTransmitOneByte( UINT8 data )

  Summary:
    This transmits one byte to the EEPROM.

  Description:
    This transmits one byte to the EEPROM, and reports errors for any bus
    collisions.

  Precondition:
    The transfer must have been previously started.

  Parameters:
    data    - Data byte to transmit

  Returns:
    TRUE    - Data was sent successfully
    FALSE   - A bus collision occured

  Example:
    <code>
    RTCTransmitOneByte(0xAA);
    </code>

  Remarks:
    This is a blocking routine that waits for the transmission to complete.
  *****************************************************************************/

BOOL RTCTransmitOneByte( UINT8 data )
{
    // Wait for the transmitter to be ready
    while(!I2CTransmitterIsReady(EEPROM_I2C_BUS));

    // Transmit the byte
    if(I2CSendByte(EEPROM_I2C_BUS, data) == I2C_MASTER_BUS_COLLISION)
    {
       // DBPRINTF("Error: I2C Master Bus Collision\n");
        return FALSE;
    }

    // Wait for the transmission to finish
     while(!I2CTransmissionHasCompleted(EEPROM_I2C_BUS));

    return TRUE;
}


/*******************************************************************************
  Function:
    void StopTransfer( void )

  Summary:
    Stops a transfer to/from the EEPROM.

  Description:
    This routine Stops a transfer to/from the EEPROM, waiting (in a 
    blocking loop) until the Stop condition has completed.

  Precondition:
    The I2C module must have been initialized & a transfer started.

  Parameters:
    None.
    
  Returns:
    None.
    
  Example:
    <code>
    RTCStopTransfer();
    </code>

  Remarks:
    This is a blocking routine that waits for the Stop signal to complete.
  *****************************************************************************/

void RTCStopTransfer( void )
{
    I2C_STATUS  status;

    // Send the Stop signal
    I2CStop(EEPROM_I2C_BUS);

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(EEPROM_I2C_BUS);

    } while ( !(status & I2C_STOP) );
}


// ****************************************************************************
// ****************************************************************************
// Application Main Entry Point
// ****************************************************************************
// ****************************************************************************
 UINT8               i2cData[10];
    I2C_7_BIT_ADDRESS   RTCSlaveAddress;
    int                 RTCIndex;
    int                 RTCDataSz;
    UINT32              RTCactualClock;
    BOOL                RTCAcknowledged;
    BOOL                ASuccess = TRUE;
    UINT8               i2cbyte;
    
    
 void RTCWriteByte(UINT8 radrl,UINT8 rdata)
{
//    if(rdata==)
    I2C_FORMAT_7_BIT_ADDRESS(RTCSlaveAddress, EEPROM_ADDRESS, I2C_WRITE);
    i2cData[0] = RTCSlaveAddress.byte;
    i2cData[1] = radrl;              // EEPROM location to program (high address byte)
    i2cData[2] = rdata;              // EEPROM location to program (low address byte)
   // i2cData[3] = 0xAA;              // Data to write
    RTCDataSz = 3;

    // Start the transfer to write data to the EEPROM
    if( !RTCStartTransfer(FALSE) )
    {
        while(1);
    }

    // Transmit all data
    RTCIndex = 0;
    while( ASuccess && (RTCIndex < RTCDataSz) )
    {
        // Transmit a byte
        if (RTCTransmitOneByte(i2cData[RTCIndex]))
        {
            // Advance to the next byte
            RTCIndex++;

            // Verify that the byte was acknowledged
            if(!I2CByteWasAcknowledged(EEPROM_I2C_BUS))
            {
               // DBPRINTF("Error: Sent byte was not acknowledged\n");
                ASuccess = FALSE;
            }
        }
        else
        {
            ASuccess = FALSE;
        }
    }

    // End the transfer (hang here if an error occured)
    RTCStopTransfer();
    if(!ASuccess)
    {
        while(1);
    }


    // Wait for EEPROM to complete write process, by polling the ack status.
    RTCAcknowledged = FALSE;
    do
    {
        // Start the transfer to address the EEPROM
        if( !RTCStartTransfer(FALSE) )
        {
            while(1);
        }
        
        // Transmit just the EEPROM's address
        if (RTCTransmitOneByte(RTCSlaveAddress.byte))
        {
            // Check to see if the byte was acknowledged
            RTCAcknowledged = I2CByteWasAcknowledged(EEPROM_I2C_BUS);
        }
        else
        {
            ASuccess = FALSE;
        }

        // End the transfer (stop here if an error occured)
        RTCStopTransfer();
        if(!ASuccess)
        {
            while(1);
        }

    } while (RTCAcknowledged != TRUE);

}

UINT8 RTCReadByte(UINT8 radrl)
{
    I2C_FORMAT_7_BIT_ADDRESS(RTCSlaveAddress, EEPROM_ADDRESS, I2C_WRITE);
    i2cData[0] = RTCSlaveAddress.byte;
    i2cData[1] = radrl;              // EEPROM location to read (high address byte)
   // i2cData[2] = 0x00;              // EEPROM location to read (low address byte)
    RTCDataSz = 2;
    
    // Start the transfer to read the EEPROM.
    if( !RTCStartTransfer(FALSE) )
    {
        while(1);
    }
    
    // Address the EEPROM.
    RTCIndex = 0;
    while( ASuccess & (RTCIndex < RTCDataSz) )
    {
        // Transmit a byte
        if (RTCTransmitOneByte(i2cData[RTCIndex]))
        {
            // Advance to the next byte
            RTCIndex++;
        }
        else
        {
            ASuccess = FALSE;
        }

        // Verify that the byte was acknowledged
        if(!I2CByteWasAcknowledged(EEPROM_I2C_BUS))
        {
           // DBPRINTF("Error: Sent byte was not acknowledged\n");
            ASuccess = FALSE;
        }
    }

    // Restart and send the EEPROM's internal address to switch to a read transfer
    if(ASuccess)
    {
        // Send a Repeated Started condition
        if( !RTCStartTransfer(TRUE) )
        {
            while(1);
        }

        // Transmit the address with the READ bit set
        I2C_FORMAT_7_BIT_ADDRESS(RTCSlaveAddress, EEPROM_ADDRESS, I2C_READ);
        if (RTCTransmitOneByte(RTCSlaveAddress.byte))
        {
            // Verify that the byte was acknowledged
            if(!I2CByteWasAcknowledged(EEPROM_I2C_BUS))
            {
               // DBPRINTF("Error: Sent byte was not acknowledged\n");
                ASuccess = FALSE;
            }
        }
        else
        {
            ASuccess = FALSE;
        }
    }

    // Read the data from the desired address
    if(ASuccess)
    {
        if(I2CReceiverEnable(EEPROM_I2C_BUS, TRUE) == I2C_RECEIVE_OVERFLOW)
        {
           // DBPRINTF("Error: I2C Receive Overflow\n");
            ASuccess = FALSE;
        }
        else
        {
            while(!I2CReceivedDataIsAvailable(EEPROM_I2C_BUS));
            i2cbyte = I2CGetByte(EEPROM_I2C_BUS);
        }

    }

    // End the transfer (stop here if an error occured)
    RTCStopTransfer();
    if(!ASuccess)
    {
        return 0xFF;
    }
    else
        return i2cbyte;

}

void InitRTC(void)
{
    UINT8 rtc_data;
    
    RTCactualClock = I2CSetFrequency(EEPROM_I2C_BUS, GetPeripheralClock(), I2C_CLOCK_FREQ);
    I2CEnable(EEPROM_I2C_BUS, TRUE);
    I2C_FORMAT_7_BIT_ADDRESS(RTCSlaveAddress, EEPROM_ADDRESS, I2C_WRITE);
    
    RTCWriteByte(7,0x13);
    
    rtc_data=RTCReadByte(0);
   // rtc_data=rtc_data&0x7F;
    
    if(rtc_data>0x59)
    {
        rtc_data=0;
        RTCWriteByte(0,rtc_data);
    }   
}

void Set_RTC(void)
{
    UINT8 s_t_index;

    for(s_t_index=0;s_t_index<7;s_t_index++)
        RTCWriteByte(s_t_index,Time[s_t_index]);
}


void Read_RTC(void)
{
     UINT8 r_t_index;
     
    for(r_t_index=0;r_t_index<7;r_t_index++)
    {
        Time[r_t_index]=RTCReadByte(r_t_index);
        
    }
   if(Time[0]>0x59)
    {
        Time[0]=0;
         RTCWriteByte(0,Time[0]);
     ;
    }
}

void RTCWriteString(UINT8 loc,UINT8 nob,char *buf)
{
    UINT8 s_t_index=0;
    for(s_t_index=0;s_t_index<nob;s_t_index++)
    {
        RTCWriteByte(loc+s_t_index,*buf);
        buf++;
    }
    
}

void RTCReadString(UINT8 loc,UINT8 nob,char *buf)
{
   UINT8 s_t_index=0;
    
    for(s_t_index=0;s_t_index<nob;s_t_index++)
    {
         *buf=RTCReadByte(loc+s_t_index);
        buf++;
    }
}




