/*
 * Schedule.cpp
 *
 *  Created on: 2018年11月22日
 *      Author: styxs
 */
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <limits.h>
#include <stdio.h>
#include "Schedule.h"

sem_t needSchedule, scheduleUp;
sem_t tmux_setAliveProcess;

void initSemaphore(){
	sem_init(&needSchedule, 0, 1);
	sem_init(&scheduleUp, 0, 0);
	sem_init(&tmux_setAliveProcess, 0, 1);
}
/* @return show how long the current burst is
 * Because in FCFS schedule, the burst time is the process's work time*/
Time getBurstTime(Time setMode = 0){
	static int time_unit = TIME_UNIT;
	if(setMode != 0){
		time_unit = setMode;
	}
	return time_unit;
}
/* return virtual time*/
Time getCurrentTime(Time setMode = 0){
	static Time currentT = 0;
	if(setMode != 0)
		currentT += setMode;
	return currentT;
}
int getAliveProcess(int setMode = -1){
	static int count;
	sem_wait(&tmux_setAliveProcess);
	if(setMode != -1)
		count = setMode;
	sem_post(&tmux_setAliveProcess);
	return count;
}


void* processFunction(void *pParameter){
	auto pPCB = (VirtualPCB*) pParameter;
	while(pPCB->remainWorkloadT > 0){
		sem_wait(&scheduleUp);
		if(pPCB->remainBurstT > 0){
			// it's this process's turn
			pPCB->remainWorkloadT -= pPCB->remainBurstT; // so burstTime must less than or equal to remainWorkloadT
			pPCB->remainBurstT = 0;
			printf("[Process %2d]execute at %u for %u second(s)\n",
					pPCB->index, getCurrentTime(), getBurstTime());
			getCurrentTime(getBurstTime());
			if(pPCB->remainWorkloadT <= 0){
				getAliveProcess(getAliveProcess()-1);
				printf("[Process %2d]Finished StillAlive:%2d\n", pPCB->index, getAliveProcess());
			}
		}
		else{
//			printf("[Process %2d]Wait\n", pPCB->index);
			pPCB->waitT += getBurstTime();
		}
		int i;
		sem_getvalue(&scheduleUp, &i);
		sleep(getBurstTime());
		if(i == 0){
			// all process know this burst's result (and run) we can start next burst
			sem_post(&needSchedule);
		}
	}

}
// these function will return the process's index which was chosen and burst time \
	if no one can be chosen, return -1
typedef struct {
	int index = -1;
	Time burstT;
}scheduleResult;
scheduleResult handleFCFS(VirtualPCB *pList, unsigned int size){
	scheduleResult result;
	// by this way, we have no need to find the first unfinished task
	Time firstArrive = UINT_MAX;
	for(unsigned int i = 0; i < size; i++){
		if(pList[i].arriveT <= getCurrentTime() &&pList[i].remainWorkloadT > 0 && pList[i].arriveT < firstArrive){
			firstArrive = pList[i].arriveT;
			result.index = i;
		}
	}
	// each process executes only once
	if(result.index != -1){
		result.burstT = pList[result.index].remainWorkloadT;
		// give the process burst time
		pList[result.index].remainBurstT = result.burstT;
	}
	return result;
}
scheduleResult handleRR(VirtualPCB *pList, unsigned int size){

}
scheduleResult handleSJF(VirtualPCB *pList, unsigned int size){

}

void* schedule(void *pParameter){
	auto pScheduleParameter = (scheduleParamter*) pParameter;
	getAliveProcess(pScheduleParameter->arraySize);
	bool isProcessing = true;
	while(isProcessing){
		sem_wait(&needSchedule); // scheduler wait to be waked up
		scheduleResult result;
		switch(pScheduleParameter->methodIndex){
		case 1:
			result = handleSJF(pScheduleParameter->pArray, pScheduleParameter->arraySize);
			break;
		case 2:
			result = handleRR(pScheduleParameter->pArray, pScheduleParameter->arraySize);
			break;
		default:
			result = handleFCFS(pScheduleParameter->pArray, pScheduleParameter->arraySize);
			break;
		}
		if(result.index >= 0){
			// make the process's serial number start with 1
			printf("[Scheduler]choose %d process for %u second(s)\n",
					pScheduleParameter->pArray[result.index].index, result.burstT);
			getBurstTime(result.burstT);
		}
		else{
			// think about the case that alive process haven't come
			if(getAliveProcess() != 0){
				getCurrentTime(TIME_UNIT);
				getBurstTime(TIME_UNIT);
			}
			else{
				printf("End\n");
				isProcessing = false;
				break;
			}
		}
		int showPCBsList(VirtualPCB* pPCBList, int size);
		//showPCBsList(pScheduleParameter->pArray, 20);
		for(int i = 0; i < getAliveProcess(); i++)
			sem_post(&scheduleUp);
	}
}
