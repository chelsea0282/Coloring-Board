#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "board.h"

//***********Forward Declaration of Methods****************************************
void setup();
void PIT_begin();
void PIT0_IRQHandler();
void delay();
//*********************************************************************************

//******************Global Variables***********************************************
//The acceleration state of the FRDM
ACCELEROMETER_STATE state;
//global variables to hold all of the acceleration values
int acc_y;
int acc_x;
int acc_z;
//global variables to hold all of the velocity values
int vel_y;
int vel_x;
int vel_z;
//global variables to hold all of the position values
int pos_y;
int pos_x;
int pos_z;
//**********************************************************************************

//***********************Constants**************************************************
int ACCEL_ZERO_X = 15;
int ACCEL_ZERO_Y = 420;
int ACCEL_ZERO_Z = 1008;
int DELAY_TIME = 10000/DEFAULT_SYSTEM_CLOCK;
//**********************************************************************************


int main(){
	//initializing everything 
	hardware_init();
	Accelerometer_Initialize();
	setup();
	PIT_begin();
	
	//continuously updating the position, speed and acceleration values
	while(1){
		delay();
		//getting new acceleration state
		Accelerometer_GetState(&state);
		
		//updating acceleration values
		acc_x = state.x - ACCEL_ZERO_X;
		acc_y = state.y - ACCEL_ZERO_Y;
		acc_x = state.z - ACCEL_ZERO_Z;
		
		//updating velocity values
		vel_x = vel_x + acc_x*DELAY_TIME;
		vel_y = vel_y + acc_y*DELAY_TIME;
		vel_z = vel_z + acc_z*DELAY_TIME;
		
		//updating position values
		pos_x = pos_x + vel_x*DELAY_TIME;
		pos_y = pos_y + vel_y*DELAY_TIME;
		pos_z = pos_z + vel_z*DELAY_TIME;
		
	}
}


//TODO 
//PIT timer setup
void PIT_begin(){
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK;
	PIT->MCR &= ~(1<<1);
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK/2;
	PIT->CHANNEL[0].TCTRL = 3;
	NVIC_EnableIRQ(PIT0_IRQn);
}


//TODO
//IRQ Handler to update the drawing page every millisecond
//Currently implemented to show the current pos, speed, and accel
void PIT0_IRQHandler(){
	debug_printf("Xpos: %5d Xvel: %5d Xacc: %5d\r\n", pos_x, vel_x, acc_x);
	debug_printf("Ypos: %5d Yvel: %5d Yacc: %5d\r\n", pos_y, vel_y, acc_y);
	debug_printf("Zpos: %5d Zvel: %5d Zacc: %5d\r\n", pos_z, vel_z, acc_z);
	
}

//a helper function that just delays a small amount of time
void delay(){
	for(int i = 10000; i > 0; i--){}
}
//a helper function that just sets all global
//values equal to 0 for speed, acceleration, and position
void setup(){
	acc_y = 0;
	acc_x = 0;
	acc_z = 0;
	vel_y = 0;
	vel_x = 0;
	vel_z = 0;
	pos_y = 0;
	pos_x = 0;
	pos_z = 0;
}
