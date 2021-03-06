#include <C8051F310.h>               
#include "stdio.h"
#include "math.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK             24500000    // Internal oscillator frequency in Hz

#define SPI_CLOCK          5000000      // Maximum SPI clock

#define MAX_BUFFER_SIZE    6           // Maximum buffer Master SPI will receive

// Instruction Set for SPI
#define  SPI_WRITE         0x04        // Send a byte from the Master to the
                                       // Slave
#define  SPI_READ          0x08        // Send a byte from the Slave to the
                                       // Master
#define  SPI_READ_BUFFER   0x32        // Send a series of bytes from the Slave
                                       // to the Master
#define  ERROR_OCCURRED    0x40        // Indicator for the Slave to tell the
                                       // Master an error occurred

// Value used to sample accelerometer values
#define  ACCELERO_RESOLUTION    31.2   //In mg/LSB, resolution of accelerometer
#define  DATA_SAMPLE    		250     //Number of values sampled to evaluate rotation speed
#define  RADIUS    				0.047   //Distance between rotation center and acceleromter
#define  GRAVITY   				9.81    //Gravity m/s^2
#define  PI    					3.14     
#define  WHEELSECTORS    		40		//Number of LED instruction during one round


//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F30x
//-----------------------------------------------------------------------------

sfr16 TMR2RL   = 0xca;                    // Timer2 reload value
sfr16 TMR2     = 0xcc;                    // Timer2 counter

//-----------------------------------------------------------------------------
// Global Variables used in SPI communication
//-----------------------------------------------------------------------------

unsigned char SPI_Data = 0xA5;

unsigned char SPI_Data_Array[MAX_BUFFER_SIZE] = {0};


bit Error_Flag = 0;

unsigned char Command = 0x00;

//stores the different value of the scheme
unsigned char schema[WHEELSECTORS] = {0};

//used to store DATA_SAMPLE values summed of unsigned z-acceleration
unsigned long moyenne;

//iteration of accelerometer measures
int iter=0;

//position on the wheel (max WHEELSECTORS)
unsigned char counter=0;

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void Oscillator_Init (void);
void Port_Init (void);
void SPI0_Init (void);
void Accelerometer_Init (void);
void Init_Device (void);
void Timer2_Init (int);
void Timer2_ISR (void);
void UART0_Init (void);
void Delay(void);
void sampleAcceleration (void);

void SPI_Byte_Write (unsigned char);
void SPI_Byte_Read (unsigned char);
void SPI_Array_Read (unsigned char);

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------
void main (void)
{
  
	//number of iteration of char received
	char iter2=0;

  	Init_Device ();                     // Initializes hardware peripherals

	//obtain scheme from bluetooth
	while(iter2<WHEELSECTORS){
    	schema[iter2] = getchar ();
		iter2++;
	}

   while (1) {
   	//sample accelerometer measures
   	sampleAcceleration ();
	 Delay();
   }
   
}

/*Initialize system clock*/
void Oscillator_Init (void)
{
	OSCICN = 0xC3;                     // Set the internal oscillator to 24.5 MHz
	RSTSRC = 0x04;                     // Enable missing clock detector
}

/*Select I/O ports functions*/
void PORT_Init (void)
{
	 // set TxD (P0.4) as push-pull & RxD as open-drain for bluetooth serial communication
	P0MDOUT |= 0x10;  

	// Make SCK, MOSI, and NSS push-pull for SPIO communication                    
   P1MDOUT = 0x0D; 
         
   // Make color and select LED pins push pull             
   P2MDOUT = 0xFF; 
      
   //to be deleted                 
   P3MDOUT = 0x08;                     // Make the LED push-pull

	//enable uart (bluetooth) and spi (accelerometer)
	XBR0=0x03;           
	XBR1=0x40; 
	 
	//Translate SPI to port 1
	P0SKIP=0xCF;                  
}

/*Apply settings for the SPI accelerometer communication*/
void SPI0_Init()
{
   SPI0CFG   = 0x70;                   // Enable the SPI as a Master CKPHA = '1', CKPOL = '1'
   SPI0CN    = 0x09;                   // 4-wire Single Master, SPI enabled
   SPI0CKR   = (SYSCLK/(2*SPI_CLOCK))-1; // SPI clock frequency equation from the datasheet
   ESPI0 = 1;                          // Enable SPI interrupts

}


/*Send all the information to launch the accelerometer throught SPI*/
void Accelerometer_Init (){
	
	SPI_Byte_Read (0x00);
	// SPI_Data should now be set to 0xE5 if everything is fine

   //We set the accelerometer on 4-wire mode SPI, with a range of +/-16g
   //with a resolution of 10bits
   SPI_Data = 0x03; 
   //write in DATA_FORMAT register
   SPI_Byte_Write (0x31);
	
	//Accelerometer mode bypass & interruption INT2 set
	SPI_Data = 0x20;
	//write in FIFO_CTL register
   SPI_Byte_Write (0x38);

   //switch on normal power mode
	SPI_Data = 0x0A;
	//BW_RATE register
   SPI_Byte_Write (0x2C);
   
   //launch measurement
   SPI_Data = 0x08;
   // POWER_CTL register
   SPI_Byte_Write (0x2D);

	//Calibration correcting z axis constant measurement offset 
   SPI_Data = 0x07;
   //OFSZ register
   SPI_Byte_Write (0x20);

}

/* Init each parts of our system*/
void Init_Device (void)
{
   PCA0MD &= ~0x40;                    // Disable the Watchdog Timer
   Oscillator_Init ();
   Port_Init ();
   SPI0_Init ();
   Timer2_Init (0x7FFF);              // Init Timer2
   UART0_Init();
   EA = 1;            // Enable global interrupts
   NSSMD0 = 1;  //Disable communication on SPI bus
   Accelerometer_Init ();
}

/*SPI communication interruption from the example*/
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


/*Write a specified data on the register of the accelerometer*/
void SPI_Byte_Write (unsigned char addr)
{
   while (!NSSMD0);                    // Wait until the SPI is free
   NSSMD0 = 0;
   Command = SPI_WRITE;
   SPI0DAT = addr;
   while (!NSSMD0); 
	//wait until operation is finished
   // The rest of this command will be handled by the SPI ISR, which will
   // trigger when SPIF is set from sending the Command
}

/*Read a specified data on the register of the accelerometer*/
void SPI_Byte_Read (unsigned char addr)
{
   while (!NSSMD0);                    // Wait until the SPI is free, in case
                                       // it's already busy
   NSSMD0 = 0;
   Command = SPI_READ;
   SPI0DAT = 0x80 | addr;  //Set Read byte and no multiple bytes
   while (!NSSMD0); 
	//wait until operation is finished
   // The rest of this command will be handled by the SPI ISR, which will
   // trigger when SPIF is set from sending the Command
}


/*Read an array of data with size MAX_BUFFER_ARRAY and from the specified address*/
void SPI_Array_Read (unsigned char addr)
{
   while (!NSSMD0);                    // Wait until the SPI is free, in case
                                       // it's already busy
   NSSMD0 = 0;
   Command = SPI_READ_BUFFER;
   SPI0DAT = 0xC0 | addr; //Set Read byte and  multiple bytes
   while (!NSSMD0); 

   // The rest of this command will be handled by the SPI ISR, which will
   // trigger when SPIF is set from sending the Command
}

/*Init timer to launch interruption every n timer rise*/
void Timer2_Init (int n)
{
                                          // use SYSCLK/12 as timebase
   CKCON  &= ~0x10;                       // Timer2 clocked based on T2XCLK
   TMR2CN  = 0x04;                        // Stop Timer2 Clear TF2
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
   TH1     = 0x96;                        // d�bordement � 9600Hz
   TMOD   |= 0x20;                        // mode2 autoreload
   CKCON  &= 0xF4;                        // sysclk pour timer1
   TR1     = 1;                           // timer1 run
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
	P2=schema[counter];
	counter++;
	if(counter==WHEELSECTORS)
	{
			//scheme over, restart
		   counter=0;
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
// Delay for little while (used for accelerometer sampling)
//
//-----------------------------------------------------------------------------
void Delay (void)
{
   unsigned long count;

   for (count = 10000; count > 0; count--);
}


/*Read and apply values obtained from accelerometer*/
void sampleAcceleration (void)
{
	//contains y and z acceleration values
   long accel[2]={0,0};
   //value to be set on timer2 servoing
   unsigned int count;

	// Read the array of xyz of acceleration
  	 SPI_Array_Read (0x32);


	//The ADXL345 gives 10-bit acceleration values, but they are stored as bytes
	// (8-bits). To get the full value, two bytes must be combined for each axis.
  //The Y value 
 	 accel[0] = ((int)SPI_Data_Array[3]<<8)|(int)SPI_Data_Array[2];
  //The Z value
 	 accel[1] = ((int)SPI_Data_Array[5]<<8)|(int)SPI_Data_Array[4];
	//Convert those values in mg (mg=10^-3*gravity)
 	 accel[0]=(int)(accel[0]*ACCELERO_RESOLUTION);
 	 accel[1]=(int)(accel[1]*ACCELERO_RESOLUTION);
	
	//store value
	 moyenne+=(unsigned long)abs((int)accel[1]);
	 iter=iter+1;

	 if(iter==DATA_SAMPLE)
	 {
	 	//sampling is complete
		moyenne/=DATA_SAMPLE;
		//calculate timer2 reload value in correspondance with rotation speed
		count=(unsigned int)((SYSCLK/12)/((sqrt(((moyenne*GRAVITY)/RADIUS)/1000)*WHEELSECTORS)/(2*PI)));
		iter=0;
		Timer2_Init ((int)count);
		//display value on bluetooth
		//printf("y= %ld mg, ",accel[0]);
		//printf("z= %ld mg, ",accel[1]);
	  	//printf("norme= %u mg \n",(unsigned int)sqrt(accel[0]*accel[0]+accel[1]*accel[1]));
		//printf("moyZ= %lu rad/s\n",moyenne);
		printf("decaltimer= %u \n",count);
		moyenne=0;
	 }
}

