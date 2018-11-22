/*
 * Schedule.h
 *
 *  Created on: 2018年11月22日
 *      Author: styxs
 */
#include <semaphore.h>

#ifndef SCHEDULE_H_
#define SCHEDULE_H_

typedef unsigned int Time;
typedef struct{
	int priority;
	Time arriveT; // when this process arrive
	Time waitT; // how long this process has waited （all）
	Time remainWorkloadT; // how long it still need to finish work
	Time remainBurstT; // how long it can work in current Burst
}VirtualPCB;

typedef struct{
	VirtualPCB* pArray;
	unsigned int arraySize;
	int methodIndex; // indicate to choose which method to schedule
}scheduleParamter;

extern sem_t needSchedule, scheduleUp; // all process wait scheduleUp to check their PCB to know who can run, \
	and when the one stop to run it set the needSchedule to wake up schedule Function \
	(So it should be define before calling schedule and processFunction)

/* choose a process to run by changing it's PCB*/
void schedule(void *pParameter);

/* do something if it was chosen to run*/
void processFunction(void *pParameter);

#endif /* SCHEDULE_H_ */
