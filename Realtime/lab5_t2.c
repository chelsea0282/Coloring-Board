/*************************************************************************
 * Lab 5 "Easy test" used for grading
 * 
* At first there will be a 1 second delay in which nothing is happening
*	Then the blue led will be flashed one time by process 1. At this point
* realtime process 2 will take over and flash the red led 4 times in a row
*	then the first process will resume and flash the blue LED 2 more times.
* The green LED will NOT flash at the end as both processes meet their 
*respective deadlines.
 * 
 ************************************************************************/
 
#include "utils.h"
#include "3140_concur.h"
#include "realtime.h"

/*--------------------------*/
/* Parameters for test case */
/*--------------------------*/


 
/* Stack space for processes */
#define NRT_STACK 80
#define RT_STACK  80
 


/*--------------------------------------*/
/* Time structs for real-time processes */
/*--------------------------------------*/

/* Constants used for 'work' and 'deadline's */
realtime_t t_1msec = {0, 1};
realtime_t t_10sec = {10, 0};

/* Process start time */
realtime_t t_pRT1 = {1, 0};
realtime_t t_pRT2 = {2, 0};

/* Process deadline time */
realtime_t t_pRT1_dead = {7, 0};
realtime_t t_pRT2_dead = {3, 999};

 
/*------------------*/
/* Helper functions */
/*------------------*/
void shortDelay(){delay();}
void mediumDelay() {delay(); delay();}



/*----------------------------------------------------
 * Real-time process 2
 *   Blinks red LED 4 times. 
 *----------------------------------------------------*/
 
void pRT2(void) {
	int i;
	for (i=0; i<4;i++){
	LEDRed_On();
	shortDelay();
	LEDRed_Toggle();
	shortDelay();
	}
	
}

/*-------------------
 * Real-time process 1
* blinks the blue LED 3 times
 *-------------------*/

void pRT1(void) {
	int i;
	for (i=0; i<3;i++){
	LEDBlue_On();
	mediumDelay();
	LEDBlue_Toggle();
	mediumDelay();
	}
}


/*--------------------------------------------*/
/* Main function - start concurrent execution */
/*--------------------------------------------*/
int main(void) {	
	 
	LED_Initialize();

    /* Create processes */ 
    if (process_rt_create(pRT2, RT_STACK, &t_pRT2, &t_pRT2_dead) < 0) { return -1; }
    if (process_rt_create(pRT1, RT_STACK, &t_pRT1, &t_pRT1_dead) < 0) { return -1; } 
   
    /* Launch concurrent execution */
	process_start();

  LED_Off();
  while(process_deadline_miss>0) {
		LEDGreen_On();
		shortDelay();
		LED_Off();
		shortDelay();
		process_deadline_miss--;
	}
	
	return 0;
}
