/*
 * test.cpp
 *
 *  Created on: 2018年11月29日
 *      Author: styxs
 */
#include "Memory.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <limits.h>

#define PATH "workload/workload%d"




double requestPages(Memory *pMemory, int workloadIndex){
	using namespace std;
	char pathbuffer[256];
	sprintf(pathbuffer, PATH, workloadIndex);
	ifstream ifile;
	ifile.open(pathbuffer);
	string spage;
	while(getline(ifile, spage)){
//		printf("request Page %s\n", spage.c_str());
		int p = std::stoi(spage);
		pMemory->requestPage(p);
	}
	return pMemory->reportPageFaultRate();

}

int main(){
	const int frameNums[] = {100, 500, 1000, 2000, 5000};
	const int workloads[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

	Memory *pMemory[3];
	for(size_t i = 0; i < sizeof(frameNums)/sizeof(int); i++){
		printf("%ld\n",i);
		int size = frameNums[i];
		pMemory[0] = new FIFOMemory(size);
		pMemory[1] = new LRUMemory(size);
		pMemory[2] = new CLOCKMemory(size);
		for(size_t j = 0; j < sizeof(workloads)/sizeof(int); j++){
			printf("%d\n",j);
			for(size_t ini = 0; ini < 3; ini++){

				int workloadindex = workloads[j];
				pMemory[ini]->init();
				if(ini == 1)continue;
				double rate = requestPages(pMemory[ini], workloadindex);
				printf("%ld-%ld:%lf\n", i,j,rate);
			}
		}
		for(int del = 0; del < 3; del++){
			printf("%d\n",del);
			fflush(stdin);
			delete pMemory[del];
		}
	}

}



