/*
 * test.cpp
 *
 *  Created on: 2018年11月23日
 *      Author: styxs
 */
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "Schedule.h"

int showPCBsList(VirtualPCB* pPCBList, int size){
	for(int i = 0; i < size; i++){
		printf("[Process %2d]arriveAt: %2u | workload: %2u | priority: %d\n",
				pPCBList[i].index, pPCBList[i].arriveT, pPCBList[i].remainWorkloadT, pPCBList[i].priority);
	}
}
int initPCBList(VirtualPCB* pPCBList, int size, int maxArriveTime = 20, int maxWorkload = 5, int maxPriority = 5){
	srand(time(NULL));
	for(int i = 0; i < size; i++){
		pPCBList[i].arriveT = rand() % maxArriveTime + 1;
		pPCBList[i].remainWorkloadT = rand() % maxWorkload + 1;
		pPCBList[i].priority = rand() % maxPriority + 1;
		pPCBList[i].remainBurstT = 0;
		pPCBList[i].waitT = 0;
		pPCBList[i].index = i+1;
	}
	showPCBsList(pPCBList, size);
}


int main(int argc, char* argv[]){
	const int SIZE = 20;
	initSemaphore(SIZE);
	int method = 0;
	if(argc == 2){
		if(strcmp(argv[1], "RR"))
			method = 1;
		else if(strcmp(argv[1], "SJF"))
			method = 2;
	}
	VirtualPCB list[SIZE];
	initPCBList(list, SIZE);
	pthread_t scheduler, ptt[SIZE];
	auto scheduleParam = scheduleParamter{list, 20, method};
	pthread_create(&scheduler, NULL, schedule, &scheduleParam);
	for(int i = 0; i < SIZE; i++){
		pthread_create(&ptt[i], NULL, processFunction, &list[i]);
	}
	pthread_join(scheduler, NULL);
}



