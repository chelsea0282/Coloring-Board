#include "3140_concur.h"
#include "utils.h"
#include <stdlib.h>
#include <fsl_device_registers.h>
#include "realtime.h"
#include "utils.h" // ADDED BY HOJUNG

int process_deadline_miss;
int process_deadline_met;
volatile realtime_t current_time;

/**
 * This structure holds the process structure information
 */

typedef struct process_state {
	unsigned int* original_sp;
	unsigned int* sp;
	// Stack size
	int n;
	//other info for realtime processes
	realtime_t* start;
	realtime_t* deadline;
	struct process_state * next;
} process_t;

//class of process_t with all variables


//New functions and other things for lab 5

//******************************************************************

//the queue for ready realtime processes
process_t* realtime_queue_ready = NULL;

//queue for not ready real time processes 
process_t* realtime_queue_not_ready = NULL;


//helper function for deadline is of type realtime_t so it can't just be compared -- so here's the function
//returns 1 if d1 < d2 and 0 otherwise
int compareRealtime(realtime_t* d1, realtime_t* d2) {
	//true if d1<d2
	if(d1->sec > d2->sec){
		return 0;
	}

	else if (d1->sec < d2->sec) {
		return 1;
	}

	else if(d1->msec > d2->msec){
		return 0;
	}

	else if( d1->msec < d2->msec){
		return 1;
	}
	
	return 0;
}

//compares a realtime object to the current time
//if current time >= d1 return 1 otherwise 0

int currentTimeIsGreater(realtime_t* d1) {
	if(current_time.sec > d1->sec){
		return 1;
	}
	else if(current_time.sec == d1->sec && current_time.msec >= d1->msec){
		return 1;
	}
	return 0;
}



//need to add a dequeue function for the rt ready queue as well
process_t* dequeue_rt(){
	if(realtime_queue_ready == NULL) {
		return NULL;
	}

	process_t* result = realtime_queue_ready;
	realtime_queue_ready = realtime_queue_ready->next;
	result->next = NULL;

	return result;

}


//this append function will add a process to the correct spot in 
//a queue based on its deadline (earliest deadline first for ready RT)
//and earliest start time first for not ready
void append_rt(process_t* process){
	if(process == NULL) return;

	//case where process is not ready and not ready queue is empty
	if(!currentTimeIsGreater(process->start)){
		if(realtime_queue_not_ready == NULL) {
			realtime_queue_not_ready = process;
			realtime_queue_not_ready->next = NULL;
			return;
		}

		process_t* nrtmp = realtime_queue_not_ready;
		if(compareRealtime(process->start, nrtmp->start)){
			realtime_queue_not_ready = process;
			process->next = nrtmp;
			return;
		}

		while(nrtmp->next != NULL && compareRealtime(nrtmp->next->start, process->start)) {
			nrtmp = nrtmp->next;
		}
		if(nrtmp->next == NULL) {
			nrtmp->next = process;
			//just to make sure there is nothing else in there
			process->next = NULL;
		}

		else{
			process->next = nrtmp->next;
			nrtmp->next = process;
		}
	}

	//case where the process is ready
	else{
		if(realtime_queue_ready == NULL) {
			process->next = NULL;
			realtime_queue_ready = process;
			realtime_queue_ready->next = NULL;
			return;
		}
		process_t* tmp = realtime_queue_ready;

		//adding if the added process is the smallest
		if(compareRealtime(process->deadline, tmp->deadline)){
			realtime_queue_ready = process;
			process->next = tmp;
			return;
		}
		//adding if the added process is not the smallest deadline
		while(tmp->next != NULL && compareRealtime(tmp->next->deadline, process->deadline)) {
			tmp = tmp->next;
		}

		if(tmp->next == NULL) {
			tmp->next = process;
			//just to make sure there is nothing else in there
			process->next = NULL;
		}

		else{
			process->next = tmp->next;
			tmp->next = process;
		}
		
		
	}
}



//PIT interupt handler for the current time variable
void PIT1_IRQHandler(){

	//current_time is a struct and counting up millisec XD
	current_time.msec++;
	if(current_time.msec == 1000){
		current_time.sec++;
		current_time.msec = 0;
	}
	PIT->CHANNEL[1].TFLG = 1;
}






//a helper function to update the realtime queue with any realtime processes
//that have become ready
void startCheck(){
	if(realtime_queue_not_ready == NULL){
		return;
	}
	process_t * tmp = realtime_queue_not_ready;
	while(currentTimeIsGreater(tmp->start)){
		realtime_queue_not_ready = realtime_queue_not_ready->next;
		append_rt(tmp);
		if(tmp->next == NULL) return;
		tmp = tmp-> next;
	}
	return;
}

/* Create a new realtime process out of the function f with the given parameters.

 * Returns -1 if unable to malloc a new process_t, 0 otherwise.

 */

int process_rt_create(void (*f)(void), int n, realtime_t* start, realtime_t* deadline){
	//allocate space in the stack for the new process
	process_t* new_process = malloc(sizeof(process_t));
	if(new_process == NULL) return -1;
	//init stack with all variables and stuff and returns sp
	new_process->original_sp = process_stack_init(f, n);
	new_process->n = n;
	if(new_process->original_sp == NULL) {
		return -1;
	}
	new_process->sp = new_process->original_sp;

	new_process->start = start;
	new_process->deadline = deadline;
	unsigned int tme = 1000*(new_process->start->sec + new_process->deadline->sec) + new_process->start->msec + new_process->deadline->msec;
	new_process->deadline->sec = tme/1000;
	new_process->deadline->msec = tme%1000;

	//here is one nuance, instead of process queue we must add to realtime
	//queue and order based on deadline
	append_rt(new_process);
	return 0;
}

//******************************************************************
//end of newly created things
//		Created Processes from previous Labs (needs to be tweaked to remove
//some stuff that is there for locks)

//******************************************************************

//creating the type process_t
typedef struct process_state process_t;



//setting up global variable invariants
process_t* current_process = NULL;
process_t* process_queue = NULL;



// Appends process to the end of the queue if process is not NULL.
// Returns 1 if the append occurred. Otherwise 0.
int append(process_t* process) {
	if(process == NULL) return 0;
	if(process_queue == NULL) {
		process_queue = process;
		return 1;
	}

	process_t*  tmp = process_queue; 
	while(tmp->next != NULL){
		tmp = tmp->next;
	}
	tmp->next = process;
	process->next = NULL;
	return 1;
}
// Prepends the process to the beginning of the queue if process
// is not NULL. Returns 1 on success, 0 otherwise
int enqueue(process_t* process) {
	if(process == NULL) return 0;
	process->next = process_queue;
	process_queue = process;
	return 1;
}



// Returns and removes the head of the queue. If there are no
// elements in the queue, returns NULL
process_t* dequeue() {

	if(process_queue == NULL) {
		return process_queue;
	}
	process_t* result = process_queue;
	process_queue = process_queue->next;
	result->next = NULL;
	return result;

}



int process_create (void (*f)(void), int n) {
	process_t* new_process = malloc(1 * sizeof(process_t));
	if(new_process == NULL) return -1;
	new_process->original_sp = process_stack_init(f, n);
	new_process->n = n;
	if(new_process->original_sp == NULL) return -1;

	new_process->sp = new_process->original_sp;
	new_process->deadline = NULL;
	new_process->start = NULL;

	if(process_queue == NULL){
		process_queue = new_process;
		process_queue->next = NULL;
	} else {
		append(new_process);
	}
	return 0;
}



void process_start(void) {
	process_deadline_miss = 0;
	process_deadline_met = 0;
	current_time.sec = 0;
	current_time.msec = 0;
	
	// Enable PIT Clock
	SIM->SCGC6 = SIM_SCGC6_PIT_MASK;
	// Enable PIT timer
	PIT->MCR &= ~(1 << 1);
	// Set countdown
	PIT->CHANNEL[0].LDVAL = 0x1a6666;
	// 0.001s counter --> IRQ handler --> increment time
	PIT->CHANNEL[1].LDVAL = DEFAULT_SYSTEM_CLOCK / 1000 ;
	// Enable interrupts
	PIT->CHANNEL[0].TCTRL = 3;
	PIT->CHANNEL[1].TCTRL = 3;
	
	
	NVIC_SetPriority(PIT1_IRQn, 1);
	NVIC_SetPriority(SVCall_IRQn, 2);
	NVIC_SetPriority(PIT0_IRQn, 3);
	//timer for context switching
	NVIC_EnableIRQ(PIT0_IRQn);
	//timer for realtime
	NVIC_EnableIRQ(PIT1_IRQn);

	process_begin();
}



// current process is either null or it points to a process which
// is not contained in the queue. The head of the queue is the next
// process to execute.
unsigned int * process_select(unsigned int * cursp) {
	
	// Disable interrupts and reset interrupt flag
	PIT->CHANNEL[0].TCTRL &= ~(2);
	
	//checks if the start time is before the current time
  startCheck();

  //beginning process select with an empty current_process (very start)
  if(current_process == NULL){
	}
	
  //checking if the current process has terminated
  else if(cursp == NULL){
  		//real time process
      if(current_process->deadline != NULL){
        if(currentTimeIsGreater(current_process->deadline)){
          process_deadline_miss ++;
        }
        else{
          process_deadline_met ++;	
			}
    }
		process_stack_free(current_process->original_sp, current_process->n);
		current_process = NULL;
		}
 
	
  else{
  	current_process->sp = cursp;
  	if(current_process->deadline != NULL){
    	append_rt(current_process);
			current_process = NULL;
    }
    else{
    	append(current_process);
			current_process = NULL;
    }
  }
  
  
		//if there are realtime processes ready
		if(realtime_queue_ready!= NULL){
			current_process = dequeue_rt();
		}
    else{
    	current_process = dequeue();
    }

	//need to ensure that the scheduler wont terminate if
	//a process is still waiting on the not ready queue
	if(current_process == NULL) {
		if(realtime_queue_not_ready != NULL){
			while(realtime_queue_ready == NULL){
				startCheck();
			}
			current_process = dequeue_rt();
		}
		else
			return NULL;
	}
  
	//enabling interrupts and sending the sp
	PIT->CHANNEL[0].TCTRL |= 2;
	return current_process->sp;

}
//******************************************************************************

//end of previously created functions

