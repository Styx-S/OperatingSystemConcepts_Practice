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

sem_t needSchedule, *choose2run;
sem_t tmux_setAliveProcess;

void initSemaphore(int size){
	sem_init(&needSchedule, 0, 1);
	choose2run = new sem_t[size];
	for(int i = 0; i < size; i++)
		sem_init(&choose2run[i], 0, 0);
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
		// the index start with 1, so need minused 1
		sem_wait(&choose2run[pPCB->index-1]);
		// it's this process's turn
		pPCB->remainWorkloadT -= pPCB->remainBurstT; // so burstTime must less than or equal to remainWorkloadT
		pPCB->remainBurstT = 0;
		printf("[Process %2d]execute at %u for %u second(s)\n",
				pPCB->index, getCurrentTime(), getBurstTime());
		sleep(getBurstTime());
		getCurrentTime(getBurstTime());
		sem_post(&needSchedule);
	}
	// this process finished
	getAliveProcess(getAliveProcess()-1);

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
	scheduleResult result;
	// find the highest priority
	int high = -1, nums = 0;
	for(int i = 0; i < size; i++){
		if(pList[i].arriveT <= getCurrentTime() && pList[i].remainWorkloadT > 0){
			if(pList[i].priority > high){
				high = pList[i].priority;
				nums = 1;
			}
			else if(pList[i].priority == high)
				nums++;
		}
	}
	if(high == -1)
		return result;
	VirtualPCB* priorityList = new VirtualPCB[nums];
	// copy
	for(int i = 0, j = 0; i < size; i++){
		if(pList[i].arriveT <= getCurrentTime() && pList[i].priority == high && pList[i].remainWorkloadT > 0){
			priorityList[j++] = pList[i];
		}
	}
	result = handleFCFS(priorityList, nums);
	result.index = priorityList[result.index].index - 1;
	// write back the remainBurstT
	pList[result.index].remainBurstT = result.burstT;
	delete[] priorityList;
	return result;
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
			result = handleRR(pScheduleParameter->pArray, pScheduleParameter->arraySize);
			break;
		case 2:
			result = handleSJF(pScheduleParameter->pArray, pScheduleParameter->arraySize);
			break;
		default:
			result = handleFCFS(pScheduleParameter->pArray, pScheduleParameter->arraySize);
			break;
		}
		if(result.index >= 0){
			// make the process's serial number start with 1
			printf("[Scheduler]choose %d process for %u second(s) (has waited %u s)\n",
					pScheduleParameter->pArray[result.index].index, result.burstT, pScheduleParameter->pArray[result.index].waitT);
			getBurstTime(result.burstT);
		}
		else{
			// think about the case that alive process haven't come
			if(getAliveProcess() != 0){
				getCurrentTime(TIME_UNIT);
				getBurstTime(TIME_UNIT);
				sem_post(&needSchedule);
				continue;
			}
			else{
				printf("End\n");
				isProcessing = false;
				break;
			}
		}
		// set others' wait time
		for(int i = 0; i < pScheduleParameter->arraySize; i++){
			if(i != result.index){
				auto pCurrent = &pScheduleParameter->pArray[i];
				if(pCurrent->arriveT > getCurrentTime()+getBurstTime())
					continue;
				else if(pCurrent->arriveT < getCurrentTime()){
					pCurrent->waitT += getBurstTime();
				}
				else{
					pCurrent->waitT += getBurstTime() - (pCurrent->arriveT - getCurrentTime());
				}
			}
		}
		sem_post(&choose2run[result.index]);
	}
}
