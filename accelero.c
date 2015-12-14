
//-----------------------------------------------------------------------------
// F31x_Blinky.c
//-----------------------------------------------------------------------------
// Copyright (C) 2007 Silicon Laboratories, Inc.
//
// AUTH: JS
// DATE: 03 JUL 02
//
// This program flashes the green LED on the C8051F31x target board about
// five times a second using the interrupt handler for Timer2.
//
// Target: C8051F31x
//
// Tool chain: KEIL Eval 'c'
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <c8051f310.h>                    // SFR declarations
#include <stdio.h>

#define SYSCLK       24500000 / 8

sbit CS = P1^3;
// SPI bus pins
sbit MOSI = P1^2;						  // 9bit SPI MOSI pin
sbit MISO = P1^1;						  // 9bit SPI MISO pin
sbit SCK = P1^0;


void PORT_Init (void);
void SYSCLK_Init (void);
void SPI_Read( unsigned char addr, unsigned char *buffer, unsigned char size );
void SPI_Write( unsigned char w );
void SPI_Init();
void SPI_write_cmd(unsigned char c);


void PORT_Init (void)
{
P0SKIP=0xCF;        //decale de maniere a avoir le spi0 sur port 1
XBR0=0x01;           //enable uart and spi
XBR1=0x40;            //crossbar enabled, on decale
P1MDOUT|=0x0D;

}


void SYSCLK_Init (void)
{
// disable watchdog timer
   PCA0MD &= ~0x40;                       // WDTE = 0 (clear watchdog timer
                                          // enable)
   OSCICN = 0xc3;                         // configure internal oscillator for
                                          // its lowest frequency
   RSTSRC = 0x04;                         // enable missing clock detector
}

void SPI_Init()
{
	SCK = 0;
	MOSI = 0;
	MISO = 1;
	CS=0;
}


#define WAIT { int wait=0; wait++; }

void SPI_write_cmd(unsigned char c)
{
	unsigned char b;

	EA=0;
	// 1st bit '0'
	MOSI = 0;
	WAIT
	SCK = 1; // tic
	WAIT
	SCK = 0; // toc
	// Ecriture des autres bits sur le bus
	for(b=0x80;b>0;b=b>>1)
	{
		WAIT
		MOSI = (c&b)?1:0;
		WAIT
		SCK = 1; // tic
		WAIT
		SCK = 0; // toc
	}
		WAIT
	MOSI=0;
	EA=1;
}



void SPI_Write( unsigned char w )
{
	unsigned char b;
	EA=0;

	// Ecriture des bits sur le bus
	for( b=0x80;b>0;b=b>>1)
	{
		MOSI = (w&b)?1:0;
		SCK = 1; // tic
		SCK = 0; // toc
	}
	MOSI = 0;
	EA=1;

}


void SPI_Read( unsigned char addr, unsigned char *buffer, unsigned char size )
{
	unsigned char b, i;

	EA=0;

	addr |= 0x80 | ( size > 1 ? 0x40 : 0x00 );

	// Ecriture des bits sur le bus
	for( b=0x80;b>0;b=b>>1)
	{
		WAIT
		MOSI = (addr&b)?1:0;
		WAIT
		SCK = 1; // tic
		WAIT
		SCK = 0; // toc
	}
		WAIT
	MOSI = 0;


	for ( i = 0 ; i < size ; i++ )
	{
		buffer[i] = 0x00;

		for ( b = 0x80 ; b >0 ; b >>= 1 )
		{
		WAIT
			SCK = 1; // tic
		WAIT
			SCK = 0; // toc
		WAIT
		WAIT
		WAIT
		WAIT
		WAIT
			if ( MISO ) buffer[i] |= b;
		}
	}


	EA=1;



}

void main(void){
char b;
		unsigned char buffer[20];
	   SYSCLK_Init ();
       PORT_Init ();                          // Initialize crossbar and GPIO

		SPI_Init();
	
		SPI_Read(0x00,buffer,1);
		b = 9;
		//SPI_write_cmd(0x31);          //envoie a l'adresse 31(base6) avec R/W=0 et multipleBytes=0
       // SPI_Write(0x03);		  //met le bit 6 a 0 sur ladresse 31, indique au gyro qu'on fonctionne en 4wire
		SPI_Read(0x2C,buffer,1);
		b = 9;
	//& range de +/- 16g et resolution à 10bits


	//	SPI_write_cmd(0x38);          //envoie a l'adresse 38(base6) avec R/W=0 et multipleBytes=0
     //   SPI_Write(0x20); //Mode bypass & interruption used INT2
     	SPI_Read(0x30,buffer,1);
    while(1){


    }

	

}











