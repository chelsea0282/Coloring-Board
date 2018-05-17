#include <fsl_device_registers.h>                   // NXP::Device:Startup
#include "MK64F12_features.h"           // NXP::Device:Startup
#include "system_MK64F12.h"             // NXP::Device:Startup
#include "board_magnetometer.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "tft.h"


//***********Forward Declaration of Methods****************************************
void setup(void);
void PIT_begin(void);
void PIT0_IRQHandler(void);
void delay(void);
void PORTC_ITQHandler(void);
void setupGPIO(void);
//*********************************************************************************

//******************Global Variables***********************************************
//The mageleration state of the FRDM
MAGNETOMETER_STATE state;

//whether or not the drawing is paused
int drawingPause;
//global variables to hold all of the mageleration values
float mag_y;
float mag_x;
//hold x,y positions 
double pos_y;
double pos_x;
//**********************************************************************************

//***********************Constants**************************************************
int MAG_ZERO_X;
int MAG_ZERO_Y;
double MAX_Y = 240;
double MIN_Y = 0;
double MAX_X = 320;
double MIN_X = 0;
double WEIGHT = .00001;
int w = 10;
int h = 10;
int c =  0xF800;
//**********************************************************************************


int main(){
	//initializing everything  
	setup();
	PTE->PCOR |= (1<<26);
	//continuously updating the magnetometer values
	while(1){
	
		//getting new magnetometer state
		Magnetometer_GetState(&state);
		

		
		//updating magnetometer values with zeroing
		mag_x = state.x - MAG_ZERO_X;
		mag_y = state.y - MAG_ZERO_Y;
		
		//thresholding
		if(15 > mag_x && mag_x > -15){
			mag_x = 0;
		}
		if(15 > mag_y && mag_y > -15){
			mag_y = 0;
		}
		
		//disable interrupts to make updating variables atomic
		//so not only x or y is updated before drawing
		PIT->CHANNEL[0].TCTRL &= ~(2);
		
		//updating position values if they are in range and setting
		//them to the max values if not
		double tempx = pos_x + mag_x*WEIGHT; 
		if(tempx <= MAX_X || tempx >= MIN_X)
			pos_x = tempx;
		else if(tempx >= MAX_X)
			pos_x = MAX_X;
		else
			pos_x = MIN_X;
		
		double tempy = pos_y + mag_y*WEIGHT; 
		if(tempy <= MAX_Y || tempy >= MIN_Y)
			pos_y = tempy;
		else if(tempy >= MAX_Y)
			pos_y = MAX_Y;
		else
			pos_y = MIN_Y;
		
		//reenable interrupts 
		PIT->CHANNEL[0].TCTRL |= 2;
		
	}
}


//PIT timer setup
void PIT_begin(){
	NVIC_EnableIRQ(PIT0_IRQn); 
	NVIC_SetPriority(PIT0_IRQn, 5);
	SIM->SCGC6 |= (1<<23);
	PIT->MCR = 0x04;
	PIT->CHANNEL[0].LDVAL = 20970;
	PIT->CHANNEL[0].TCTRL |= 3;
}

//Interrupt handler for the pushbutton SW2 on the board
//If pushed this button should toggle the drawing functionality of the board
//indicated by whether or not the green led is on or red led is on
void PORTC_IRQHandler(){
	if(drawingPause){
 		drawingPause = 0;
		PTB->PSOR |= (1<<22);
		PTE->PCOR |= (1<<26);
	}
	else{
		drawingPause = 1;
		PTB->PCOR |= (1<<22);
		PTE->PSOR |= (1<<26);
	}
	PORTC->ISFR |= (1<<6);
}


//IRQ Handler to update the drawing page every millisecond
void PIT0_IRQHandler(){
	if(!drawingPause){
		//converting the positions to integer values
		int x = pos_x/1;
		int y = pos_y/1;
		//drawing on the screen with the position
		tft_fill_screen(x,y,w,h,c);
		
	}
	PIT->CHANNEL[0].TFLG |= 1;
}

//a helper function that just delays a small amount of time
void delay(){
	for(int i = 10000; i > 0; i--);
}

//a helper function that just sets all global
//values equal to 0 and sets up all other aspects
void setup(){
	//setup the TFT board
	tft_begin();
	//setup the GPIO pins
	setupGPIO();
	//initialize the magnetometer
	Magnetometer_Initialize();
	pos_x = 0;
	//setting the magelerometer offsets
	int tempx = 0;
	int tempy = 0;
	for(int i = 0; i <1000; i++){
		Magnetometer_GetState(&state);
		tempx += state.x;
		tempy += state.y;
	}
	MAG_ZERO_X = tempx/1000;
	MAG_ZERO_Y = tempy/1000;
	
	//setting all global variables to zero
	mag_y = 0;
	mag_x = 0;
	drawingPause = 0;
	//begin the PIT timer
	PIT_begin();
}



//setup the LED and any other desired pins as GPIO
void setupGPIO(){
		NVIC_EnableIRQ(PORTC_IRQn);
		NVIC_SetPriority(PORTC_IRQn, 3);
	
    //setting pins as GPIO
	  PORTB -> PCR[22] = (1<<8);		//PTB22 (red LED) is now GPIO
		PORTE -> PCR[26] = (1<<8);    //PTE26 (green LED) is now GPIO 
		PORTC -> PCR[6] = (1<<8);			//PTC6 (SW2) is now GPIO
		
		//further setup for the SW2 to act as a toggle switch
		//setting up the pull down resistor and toggle on falling edge
		PORTC->PCR[6] |= (1<<19);
		PORTC->PCR[6] |= (1<<17);
		PORTC->PCR[6] |= (1<<16);
		PORTC->PCR[6] |= 0x3;
	
    //setting up pins as outputs
		PTB -> PDDR |= (1<<22);
		PTE -> PDDR |= (1<<26);
		
		//setting up the SW2 as an input
		PTC -> PDDR &= ~(1<<6);
		
		//turning off the LEDs initially
		PTB->PTOR |= (1<<22);
		PTE->PTOR |= (1<<26); // green LED toggle
}
