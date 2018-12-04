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
#include <cstdio>

#define PATH "workload/workload%d"
#define PATH_RESULT "result/result.csv"



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
void writeResult(double* rateList, int row, int column, int kind){
	const int INTERVAL_FRAME = kind * column;
	FILE* fp = fopen(PATH_RESULT, "w");
	if(fp == NULL)
		return;
	for(int i = 0; i < kind; i++){
		for(int j = 0; j < row; j++){
			for(int k = 0; k < column; k++){
				fprintf(fp, "%lf", rateList[i + j * INTERVAL_FRAME + k * kind]);
				if(k != column-1)
					fprintf(fp, ",");
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
}

int main(){
	const int frameNums[] = {100, 500, 1000, 2000, 5000};
	const int workloads[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
	const int FRAMENUMS = sizeof(frameNums)/sizeof(int);
	const int WORKLOADNUMS = sizeof(workloads)/sizeof(int);
	Memory *pMemory[3];
	double result[FRAMENUMS * WORKLOADNUMS * 3];
	for(size_t i = 0; i < sizeof(frameNums)/sizeof(int); i++){
		printf("-------------\n");
		printf("FrameSize:%d\n",frameNums[i]);
		int size = frameNums[i];
		pMemory[0] = new FIFOMemory(size);
		pMemory[1] = new LRUMemory(size);
		pMemory[2] = new CLOCKMemory(size);
		for(size_t j = 0; j < sizeof(workloads)/sizeof(int); j++){
			printf("Workload:%d\n",workloads[j]);
			for(size_t ini = 0; ini < 3; ini++){

				int workloadindex = workloads[j];
				pMemory[ini]->init();
				//if(ini != 1)continue;
				double rate = requestPages(pMemory[ini], workloadindex);
				printf("%1.d:%lf\n", (int)ini+1,rate);
				result[ini + i * 3 * WORKLOADNUMS + j * 3] = rate;
			}
		}
		for(int del = 0; del < 3; del++){
			delete pMemory[del];
		}
	}
	writeResult(result, FRAMENUMS, WORKLOADNUMS, 3);
}



