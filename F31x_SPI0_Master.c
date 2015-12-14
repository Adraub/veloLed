//-----------------------------------------------------------------------------
// F31x_SPI0_Master.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program configures a C8051F31x as a 4-wire SPI Single Master.
//
// The SPI clock in this example is limited to 500 kHz when used with the
// SPI0_Slave code example.  During a SPI_Read, the slave needs some time to
// interpret the command and write the appropriate data to the SPI0DAT
// register, and the slave no longer has enough time to complete the
// SPI_READ_BUFFER command with a clock greater than 500 kHz.  For faster SPI
// clocks, a dummy byte between the command and the first byte of Read data
// will be required.
//
// This example is intended to be used with the SPI0_Slave example.
//
// Pinout:
//
// P0.0 - SPI SCK    (digital output, push-pull)
// P0.1 - SPI MISO   (digital input, open-drain)
// P0.2 - SPI MOSI   (digital output, push-pull)
// P0.3 - SPI NSS    (digital output, push-pull)
//
// P3.3 - LED        (digital output, push-pull)
//
// all other port pins unused.
//
//
// How To Test:
//
// 1) Download the code to a F310-TB that is connected as above to
//    another device running the SPI0_Slave code.
// 2) Verify the LED pins of jumper J3 are populated.
// 3) Verify J9 and J10 are populated.
// 4) Run the code.
// 5) If the communication passes, the LEDs on both the Master and Slave
//    boards will blink slowly. If it fails, the LEDs will be OFF.
//
//
// Target:         C8051F31x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -12 DEC 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F310.h>                 // SFR declarations
#include "stdio.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             24500000    // Internal oscillator frequency in Hz

#define SPI_CLOCK          500000      // Maximum SPI clock
                                       // The SPI clock is a maximum of 500 kHz
                                       // when this example is used with
                                       // the SPI0_Slave code example.

#define MAX_BUFFER_SIZE    6           // Maximum buffer Master will send

// Instruction Set
#define  SLAVE_LED_ON      0x01        // Turn the Slave LED on
#define  SLAVE_LED_OFF     0x02        // Turn the Slave LED off
#define  SPI_WRITE         0x04        // Send a byte from the Master to the
                                       // Slave
#define  SPI_READ          0x08        // Send a byte from the Slave to the
                                       // Master
#define  SPI_WRITE_BUFFER  0x10        // Send a series of bytes from the
                                       // Master to the Slave
#define  SPI_READ_BUFFER   0x32        // Send a series of bytes from the Slave
                                       // to the Master
#define  ERROR_OCCURRED    0x40        // Indicator for the Slave to tell the
                                       // Master an error occurred

#define  ACCELERO_RESOLUTION    31.2

sbit LED = P3^3;                       // LED='1' means ON
sbit SW2 = P0^7;                          // SW2='0' means switch pressed
//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F30x
//-----------------------------------------------------------------------------

sfr16 TMR2RL   = 0xca;                    // Timer2 reload value
sfr16 TMR2     = 0xcc;                    // Timer2 counter

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

unsigned char SPI_Data = 0xA5;

unsigned char SPI_Data_Array[MAX_BUFFER_SIZE] = {0};

bit Error_Flag = 0;

unsigned char Command = 0x00;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void PCA0_Init (void);
void Oscillator_Init (void);
void Port_Init (void);
void SPI0_Init (void);
void Init_Device (void);
void Timer2_Init (int counts);
void Timer2_ISR (void);
void UART0_Init (void);
void Delay(void);

void SPI_Byte_Write (unsigned char addr);
void SPI_Byte_Read (unsigned char addr);
void SPI_Array_Read (unsigned char addr);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
   int accel[3]={0,0,0};

   Init_Device ();                     // Initializes hardware peripherals
	Timer2_Init (0xFFF0);                  // Init Timer2
	UART0_Init();
   EA = 1;                             // Enable global interrupts

   LED = 0;

   // TEST BEGIN --------------------------------------------------------------

   

   NSSMD0 = 1;

   SPI_Byte_Read (0x00);

                      // Wait until the Read transfer has


   SPI_Data = 0x03; //met le bit 6 a 0 sur ladresse 31, indique au gyro qu'on fonctionne en 4wire
   //& range de +/- 16g et resolution � 10bits
   // Write a value
   SPI_Byte_Write (0x31);
                // Wait until the Write transfer has
                                      // finished

	SPI_Data = 0x20;//Mode bypass & interruption used INT2
   // Write a value
   SPI_Byte_Write (0x38);
                // Wait until the Write transfer has
                                      // finished
	SPI_Data = 0x0A;//bit d4=0 normal power
   // Write a value
   SPI_Byte_Write (0x2C);
                // Wait until the Write transfer has
                                      // finished
		SPI_Data = 0x08;//measurement mode on
   // Write a value
   SPI_Byte_Write (0x2D);


   

   while (1) {                            // spin forever
			// Read the array back from the slave
  	 SPI_Array_Read (0x32);

   // Read the same value back

	//The ADXL345 gives 10-bit acceleration values, but they are stored as bytes (8-bits). To get the full value, two bytes must be combined for each axis.
  //The X value is stored in values[0] and values[1].
 	 accel[0] = ((int)SPI_Data_Array[1]<<8)|(int)SPI_Data_Array[0];
  //The Y value is stored in values[2] and values[3].
 	 accel[1] = ((int)SPI_Data_Array[3]<<8)|(int)SPI_Data_Array[2];
  //The Z value is stored in values[4] and values[5].
 	 accel[2] = ((int)SPI_Data_Array[5]<<8)|(int)SPI_Data_Array[4];
 	 accel[0]=(int)(accel[0]*ACCELERO_RESOLUTION);
 	 accel[1]=(int)(accel[1]*ACCELERO_RESOLUTION);
 	 accel[2]=(int)(accel[2]*ACCELERO_RESOLUTION);
     printf("x= %d mg, ",accel[0]);
	 printf("y= %d mg, ",accel[1]);
	 printf("z= %d mg \n",accel[2]);
	 Delay();
   }
   
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PCA0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function disables the watchdog timer.
//
//-----------------------------------------------------------------------------
void PCA0_Init (void)
{
   PCA0MD &= ~0x40;                    // Disable the Watchdog Timer
}

//-----------------------------------------------------------------------------
// Oscillator_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use the internal oscillator
// at 24.5 MHz.
//
//-----------------------------------------------------------------------------
void Oscillator_Init (void)
{
   OSCICN = 0xc3;                     // Set the internal oscillator to
                                       // 24.5 MHz
	RSTSRC = 0x04;                         // enable missing clock detector
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// P0.0  -  SCK  (SPI0), Push-Pull,  Digital
// P0.1  -  MISO (SPI0), Open-Drain, Digital
// P0.2  -  MOSI (SPI0), Push-Pull,  Digital
// P0.3  -  NSS  (SPI0), Push-Pull,  Digital
//
// P3.3  -  LED,         Push-Pull,  Digital (LED D2 on Target Board)
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
	P0MDOUT |= 0x10;                       // TxD (P0.4) as push-pull
   P1MDOUT = 0x0D;                     // Make SCK, MOSI, and NSS push-pull
   P3MDOUT = 0x08;                     // Make the LED push-pull


   
	XBR0=0x03;           //enable uart and spi
	XBR1=0x40;  
	P0SKIP=0xCF;        //decale de maniere a avoir le spi0 sur port 1          
}

//-----------------------------------------------------------------------------
// SPI0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configures SPI0 to use 4-wire Single Master mode. The SPI timing is
// configured for Mode 0,0 (data centered on first edge of clock phase and
// SCK line low in idle state).
//
//-----------------------------------------------------------------------------
void SPI0_Init()
{
   SPI0CFG   = 0x70;                   // Enable the SPI as a Master
                                       // CKPHA = '1', CKPOL = '1'
   SPI0CN    = 0x09;                   // 4-wire Single Master, SPI enabled

   // SPI clock frequency equation from the datasheet
   SPI0CKR   = (SYSCLK/(2*SPI_CLOCK))-1;

   ESPI0 = 1;                          // Enable SPI interrupts

}

//-----------------------------------------------------------------------------
// Init_Device
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Calls all device initialization functions.
//
//-----------------------------------------------------------------------------
void Init_Device (void)
{
   PCA0_Init ();                       // Disable the Watchdog Timer first
   Oscillator_Init ();
   Port_Init ();
   SPI0_Init ();
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SPI_ISR
//-----------------------------------------------------------------------------
//
// Handles all error checks and single-byte writes.
//
// Note: SPI_WRITE_ARRAY is not handled by this ISR in order to take
// advantage of double-buffering (checking the TXBMT flag) using polling.
//
//
// Typical Write:
//
//              | 1st sent | 2nd sent | 3rd sent |   ...    | last sent |
//              ---------------------------------------------------------
//  Master NSSv | Command  |   Data1  |   Data2  |   ...    |   DataN   |  NSS^
//  Slave       |   N/A    |    N/A   |    N/A   |   ...    |    N/A    |
//
// Typical Read:
//
//              | 1st sent | 2nd sent | 3rd sent |   ...    | last sent |
//              ---------------------------------------------------------
//  Master NSSv | Command  |   dummy  |   dummy  |   ...    |   dummy   |  NSS^
//  Slave       |   N/A    |   Data1  |   Data2  |   ...    |   DataN   |
//-----------------------------------------------------------------------------
void SPI_ISR (void) interrupt 6
{
   static unsigned char array_index = 0;
   static char state = 0;

   if (WCOL == 1)
   {
      // Write collision occurred
      WCOL = 0;                        // Clear the write collision flag

      Error_Flag = 1;
   }
   else
   {
      if (SPI0DAT == ERROR_OCCURRED)
      {
         // This example recognizes when an error occurs, but does not include
         // any error handling.  The transfer can be aborted or rescheduled,
         // if desired.
         Error_Flag = 1;
      }

      // When the Master enters the ISR, the SPIF flag should be set from
      // sending the Command byte.  This ISR handles the remaining steps of the
      // SPI transfer process.
      // <state> == 0: writing or reading 1 byte of data
      // <state> == 1: for READ commands (first time, only a dummy byte is
      //               sent but the second time, the data must be read from
      //               SPI0DAT)
      // <state> == 2: NSS = 1 to end the transfer, final byte read
      //
      // Note: SPI_WRITE_BUFFER is not handled here because it's done in
      // polled mode
      if (state == 0)
      {
         switch (Command)
         {
            case SLAVE_LED_ON:
            case SLAVE_LED_OFF:
               NSSMD0 = 1;             // Release the slave (not expecting
                                       // data back)

               break;

            case SPI_WRITE:
               SPI0DAT = SPI_Data;

               state = 2;              // Advance to the final state (only
                                       // writing one byte)

               break;

            case SPI_READ:
               SPI0DAT = 0xFF;         // Send a dummy byte so the Slave can
                                       // send the data

               state = 2;              // Advance to the final state (only
                                       // reading one byte)

               break;

            case SPI_READ_BUFFER:
               SPI0DAT = 0xFF;         // Send a dummy byte so the Slave can
                                       // start sending the data

               state = 1;              // Advance to the next state where the
                                       // data can be received
                                       // The data from the slave is not
                                       // available until after the second
                                       // transfer is completed.
                                       // The dummy byte allows the slave to
                                       // send data, since the Master controls
                                       // SCK.

               break;

            default:
               state = 2;              // Any errors in the Command parsing
                                       // should go to state 2 where NSSMD0
                                       // is de-asserted
         }
      }
      else if (state == 1)             // This state is for READ_ARRAY
      {                                // commands where the data must be read
                                       // after the first dummy byte is sent
         switch (Command)
         {
            case SPI_READ_BUFFER:
               SPI_Data_Array[array_index] = SPI0DAT;
               SPI0DAT = 0xFF;

               array_index++;

               if (array_index == (MAX_BUFFER_SIZE-1))
               {
                  state = 2;
               }

               break;
            default:
               state = 2;              // Any errors in the Command parsing
                                       // should go to state 2 where NSSMD0
                                       // is de-asserted
         }
      }
      else if (state == 2)
      {
         switch (Command)
         {
            case SPI_READ:
               SPI_Data = SPI0DAT;     // Read the data from the slave

               break;

            case SPI_READ_BUFFER:
               SPI_Data_Array[array_index] = SPI0DAT; // Read the last data
                                                      // without sending a
                                                      // dummy byte
				array_index=0;

               break;
         }

         NSSMD0 = 1;                   // De-select the Slave

         state = 0;                    // Reset the state
      }

      SPIF = 0;                        // Clear the SPIF flag
   }
}



//-----------------------------------------------------------------------------
// SPI_Byte_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Note: SPI_Data must contain the data to be sent before calling this
// function.
//
// Writes a single byte to the SPI Slave.  The slave does not respond to this
// command, so the command consists of:
//
// Command = SPI_WRITE
// Length = 1 byte of command, 1 byte of data
//
//-----------------------------------------------------------------------------
void SPI_Byte_Write (unsigned char addr)
{
   while (!NSSMD0);                    // Wait until the SPI is free, in case
                                       // it's already busy

   NSSMD0 = 0;

   Command = SPI_WRITE;

   SPI0DAT = addr;

   while (!NSSMD0); 

   // The rest of this command will be handled by the SPI ISR, which will
   // trigger when SPIF is set from sending the Command
}

//-----------------------------------------------------------------------------
// SPI_Byte_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Note: SPI_Data will contain the data received after calling this function.
//
// Reads a single byte from the SPI Slave.  The command consists of:
//
// Command = SPI_READ
// Length = 1 byte of command, 1 byte of data
//
//-----------------------------------------------------------------------------
void SPI_Byte_Read (unsigned char addr)
{
   while (!NSSMD0);                    // Wait until the SPI is free, in case
                                       // it's already busy

   NSSMD0 = 0;

   Command = SPI_READ;

   SPI0DAT = 0x80 | addr;

   while (!NSSMD0); 

   // The rest of this command will be handled by the SPI ISR, which will
   // trigger when SPIF is set from sending the Command
}


//-----------------------------------------------------------------------------
// SPI_Array_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Note: SPI_Data_Array will contain the data received after calling this
// function.
//
// Reads a single byte from the SPI Slave.  The command consists of:
//
// Command = SPI_READ_BUFFER
// Length = 1 byte of command, MAX_BUFFER_SIZE bytes of data
//
//-----------------------------------------------------------------------------
void SPI_Array_Read (unsigned char addr)
{
   while (!NSSMD0);                    // Wait until the SPI is free, in case
                                       // it's already busy

   NSSMD0 = 0;

   Command = SPI_READ_BUFFER;

   SPI0DAT = 0xC0 | addr;

   while (!NSSMD0); 

   // The rest of this command will be handled by the SPI ISR, which will
   // trigger when SPIF is set from sending the Command
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Configure Timer2 to 16-bit auto-reload and generate an interrupt at
// interval specified by <counts> using SYSCLK/48 as its time base.
//
void Timer2_Init (int n)
{
                                          // use SYSCLK/12 as timebase
   CKCON  &= ~0x10;                       // Timer2 clocked based on T2XCLK;

   TMR2CN  = 0x04;                        // Stop Timer2; Clear TF2; Voir datasheet p199
   TMR2RL  = -n;                          // Init reload values

   ET2 = 1;

}


//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//


void UART0_Init()
{
   //Timer 1 init
   TH1     = 0x96;                        // d�bordement � 57600Hz
	
   TMOD   |= 0x20;                        // mode2 autoreload
   CKCON  &= 0xF4;                        // sysclk pour timer1
   TR1     = 1;                           // timer1 run


   //UART Init
   REN0    = 1;                           // reception autoris�e
   SBUF0   = '\n';                        // pour commencer nouvelle  
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
// This routine changes the state of the LED whenever Timer2 overflows.
//
void Timer2_ISR (void) interrupt 5
{
   if (~SW2) {
      LED = 1;
   }
   else {
      LED = ~LED;
   }

   TF2H = 0;                              // clear Timer2 interrupt flag
}

//-----------------------------------------------------------------------------
// Delay
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Delay for little while (used for blinking the LEDs)
//
//-----------------------------------------------------------------------------
void Delay (void)
{
   unsigned long count;

   for (count = 200000; count > 0; count--);
}

