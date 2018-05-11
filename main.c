#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "board.h"

//position type to hold x,y,z positions 
typedef struct{
	float y;
	float x;
	float z;
} position_t;

//***********Forward Declaration of Methods****************************************
void setup(void);
void PIT_begin(void);
void PIT0_IRQHandler(void);
void delay(void);
position_t * positionInit(void);
//*********************************************************************************

//******************Global Variables***********************************************
//The acceleration state of the FRDM
ACCELEROMETER_STATE state;
//global variables to hold all of the acceleration values
int acc_y;
int acc_x;
int acc_z;
//global variables to hold all of the velocity values
volatile float vel_y;
volatile float vel_x;
volatile float vel_z;
//global variable to hold the current position of the board
position_t* currentPosition;
//**********************************************************************************

//***********************Constants**************************************************
int ACCEL_ZERO_X;
int ACCEL_ZERO_Y;
int ACCEL_ZERO_Z;
float DELAY_TIME = .01;
//**********************************************************************************


int main(){
	//initializing everything 
	hardware_init();
	Accelerometer_Initialize();
	setup();
	currentPosition = positionInit();
	PIT_begin();
	
	//continuously updating the position, speed and acceleration values
	while(1){
		delay();
		//getting new acceleration state
		Accelerometer_GetState(&state);
		
		//updating acceleration values
		//using a thresholding around the average value
		acc_x = state.x - ACCEL_ZERO_X;
		acc_y = state.y - ACCEL_ZERO_Y;
		acc_z = state.z - ACCEL_ZERO_Z;
		if(20 > acc_x && acc_x > -20){
			acc_x = 0;
		}
		if(20 > acc_y && acc_y > -20){
			acc_y = 0;
		}
		if(20 > acc_z && acc_z > -20){
			acc_z = 0;
		}
		
		
		//updating velocity values
		vel_x = vel_x + acc_x*DELAY_TIME;
		vel_y = vel_y + acc_y*DELAY_TIME;
		vel_z = vel_z + acc_z*DELAY_TIME;
		
		//updating position values
		currentPosition->x = currentPosition->x + vel_x*DELAY_TIME;
		currentPosition->y = currentPosition->y + vel_y*DELAY_TIME;
		currentPosition->z = currentPosition->z + vel_z*DELAY_TIME;
		
	}
}


//TODO 
//PIT timer setup
void PIT_begin(){
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK;
	PIT->MCR &= ~(1<<1);
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK;
	PIT->CHANNEL[0].TCTRL = 3;
	NVIC_EnableIRQ(PIT0_IRQn);
}


//TODO
//IRQ Handler to update the drawing page every millisecond
//Currently implemented to show the current pos, speed, and accel
void PIT0_IRQHandler(){
	debug_printf("cat");
	debug_printf("Xpos: %5d Xvel: %5d Xacc: %5d\r\n", currentPosition->x, vel_x, acc_x);
	debug_printf("Xacc: %5d Yacc: %5d Zacc: %5d\r\n", state.x, state.y, state.z);
	PIT->CHANNEL[0].TFLG |= 1;
	//debug_printf("Ypos: %5d Yvel: %5d Yacc: %5d\r\n", currentPosition->y, vel_y, acc_y);
	//debug_printf("Zpos: %5d Zvel: %5d Zacc: %5d\r\n", currentPosition->z, vel_z, acc_z);
	
}

//a helper function that just delays a small amount of time
void delay(){
	for(int i = 50000; i > 0; i--){}
}
//a helper function that just sets all global
//values equal to 0 for speed, acceleration, and position
//and also zeros accelerometer
void setup(){
	
	//setting the accelerometer offsets
	int tempx = 0;
	int tempy = 0;
	int tempz = 0;
	for(int i = 0; i <1000; i++){
		Accelerometer_GetState(&state);
		tempx += state.x;
		tempy += state.y;
		tempz += state.z;
	}
	ACCEL_ZERO_X = tempx/1000;
	ACCEL_ZERO_Y = tempy/1000;
	ACCEL_ZERO_Z = tempz/1000;
	
	//setting all global variables to zero
	acc_y = 0;
	acc_x = 0;
	acc_z = 0;
	vel_y = 0;
	vel_x = 0;
	vel_z = 0; 
}

position_t * positionInit(){
	position_t temp;
	temp.x = 0.0;
	temp.y = 0.0;
	temp.z = 0.0;
	return &temp;
}
