/*************************************************************************
 * Lab 5 Test Case 1
 * 
 * This case will run the nonrealtime process (blinking the red LED 4 times)
 * to completion. Then there will be a pause where no process is running
 * as the realtime process's start time hasn't come yet, and then the realtime
 * processs will execute. At the end there will not be a green flash as the realtime
 *process will have met its deadline.
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
realtime_t t_pRT1 = {5, 0};

 
/*------------------*/
/* Helper functions */
/*------------------*/
void shortDelay(){delay();}
void mediumDelay() {delay(); delay();}



/*----------------------------------------------------
 * Non real-time process
 *   Blinks red led 4 times. 
 *   Should run uninhibited at first.
 *----------------------------------------------------*/
 
void pNRT(void) {
	int i;
	for (i=0; i<4;i++){
	LEDRed_On();
	shortDelay();
	LEDRed_Toggle();
	shortDelay();
	}
	
}

/*-------------------
 * Real-time process
		Blinks the blue LED 3 times
		should wait for 5 seconds to start executing
		which will be after the non realtime finishes
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
    if (process_create(pNRT, NRT_STACK) < 0) { return -1; }
    if (process_rt_create(pRT1, RT_STACK, &t_pRT1, &t_10sec) < 0) { return -1; } 
   
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
