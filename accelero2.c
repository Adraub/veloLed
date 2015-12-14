
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

sbit NSS = P1^3;

void PORT_Init (void);
void SPI0init (void);
void SYSCLK_Init (void);


void PORT_Init (void)
{
P0SKIP=0xCF;        //decale de maniere a avoir le spi0 sur port 1
XBR0=0x03;           //enable uart and spi
XBR1=0x40;            //crossbar enabled, on decale
P1MDOUT|=0x0C;

}

void SPI0init (void)
{
SPI0CN|=0x08;   //p181 , NSSMD=1X, 4 wire master mode
SPI0CFG|=0x70;  //mode de communication(178), demandé par l'oscillo p15 touche a CKPOL=1, CKPHA=1     MSTEN=1, opere en mode master
SPI0CN|=0x01;    //SPIEN=1 spi enabled
SPI0CKR=0x01;   //p182 divide clock by 4 in SPI transmission
}


void SYSCLK_Init (void)
{
   OSCICN = 0xc3;                         // configure internal oscillator for
                                          // its lowest frequency
   RSTSRC = 0x04;                         // enable missing clock detector
}





void main(void){
	   SYSCLK_Init ();
       PORT_Init ();                          // Initialize crossbar and GPIO
       SPI0init();    //initialise la transmission SPI

		SPIF=0;
		NSS=0;
       SPI0DAT=0x31;          //envoie a l'adresse 31(base6) avec R/W=0 et multipleBytes=0
        while(SPIF==0){}   //tant que pas de reponse
        NSS=1;
		SPIF=0;
		
		NSS=0;
    SPI0DAT=0x03;  //met le bit 6 a 0 sur ladresse 31, indique au gyro qu'on fonctionne en 3wire
	//& range de +/- 16g et resolution à 10bits
    while(SPIF==0){}   //tant que pas de reponse
		NSS=1;
	SPIF=0;
	NSS=0;
       SPI0DAT=0x38;          //envoie a l'adresse 38(base6) avec R/W=0 et multipleBytes=0
        while(SPIF==0){}   //tant que pas de reponse
        SPIF=0;
	NSS=1;
	NSS=0;
    SPI0DAT=0x20;  //Mode bypass & interruption used INT2
    while(SPIF==0){}   //tant que pas de reponse
    SPIF=0;
	NSS=1;
	NSS=0;
	SPI0DAT=0x80;
	while(SPIF==0){}   //tant que pas de reponse
    SPIF=0;
	NSS=1;




    while(1){


    }



}











