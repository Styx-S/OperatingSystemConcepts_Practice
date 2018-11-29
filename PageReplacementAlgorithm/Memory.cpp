/*
 * Memory.cpp
 *
 *  Created on: 2018年11月28日
 *      Author: styxs
 */
#include "Memory.h"
#include <cstring>
#include <limits.h>

#include <stdio.h>

Memory::Memory(int size){
	m_size = size;
	m_frameList = NULL;
	init();
}
Memory::~Memory(){
	delete[] m_frameList;
	m_frameList = NULL;
}
void Memory::init(){
	if(m_frameList)
		delete[] m_frameList;
	m_frameList = new Item[m_size];
	m_uPageFaultNums = 0;
	m_uRequestNums = 0;
	for(int i = 0; i < m_size; i++){
		m_frameList[i] = Item{-1};
	}
}
double Memory::reportPageFaultRate(){
	return ((double)m_uPageFaultNums) / m_uRequestNums;
}
void* Memory::requestPage(int logicPageIndex){
	m_uRequestNums++;
	bool canFind = false;
	for(int i = 0; i < m_size; i++){
		if(m_frameList[i].logicPageIndex == logicPageIndex ){
			canFind = true;
			break;
		}
	}
	if(!canFind){
		m_uPageFaultNums++;
		this->processPageFault(logicPageIndex);
	}
	// now we needn't return anything
	return NULL;
}

FIFOMemory::FIFOMemory(int size) : Memory(size){ // @suppress("Class members should be properly initialized")
	this->init();
}
FIFOMemory::~FIFOMemory(){
	// do-nothing
}
void FIFOMemory::init(){
	Memory::init();
	indexPoint2Oldest = INT_MAX;
}


LRUMemory::LRUMemory(int size) : Memory(size){
	leastRecentlyUsed = NULL;
	this->init();
}
LRUMemory::~LRUMemory(){
	delete[] leastRecentlyUsed;
	leastRecentlyUsed = NULL;
}
void LRUMemory::init(){
	Memory::init();
	count = 0;
	if(leastRecentlyUsed)
		delete leastRecentlyUsed;
	leastRecentlyUsed = new int[m_size];
	memset(leastRecentlyUsed, 0, m_size);
}


CLOCKMemory::CLOCKMemory(int size) : Memory(size){
	m_chances = NULL;
	this->init();
}
CLOCKMemory::~CLOCKMemory(){
	delete m_chances;
	m_chances = NULL;
}
void CLOCKMemory::init(){
	Memory::init();
	m_index = 0;
	if(m_chances)
		delete[] m_chances;
	m_chances = new bool[m_size];
	std::memset(m_chances, 0, m_size);
	m_index = INT_MAX;
}


void FIFOMemory::processPageFault(int logicPageIndex){
	++indexPoint2Oldest;
	if(indexPoint2Oldest < 0 || indexPoint2Oldest > m_size-1)
		indexPoint2Oldest = 0;
	m_frameList[indexPoint2Oldest] = Item{logicPageIndex};
//	printf("PageFault:%d (%d)\n", logicPageIndex, indexPoint2Oldest);
}
void* LRUMemory::requestPage(int logicPageIndex){
	m_uRequestNums++;
		bool canFind = false;
		int index;
		for(int i = 0; i < m_size; i++){
			if(m_frameList[i].logicPageIndex == logicPageIndex ){
				canFind = true;
				index = i;
				break;
			}
		}
		if(!canFind){
			m_uPageFaultNums++;
			this->processPageFault(logicPageIndex);
		}
		else{
			// LRU
			int k;
			// find location in the LRU
			for(int i = 0; i < m_size; i++){
				if(leastRecentlyUsed[i] == index){
					k = i;
					break;
				}
			}
			// set k at the first of LRU
			for(int i = k; i > 0; i--){
				leastRecentlyUsed[i] = leastRecentlyUsed[i-1];
			}
			leastRecentlyUsed[0] = index;

		}
		// now we needn't return anything
		return NULL;
}
void LRUMemory::processPageFault(int logicPageIndex){
	int j;
	if(count < m_size-1){
		count++;
		j = count;
		m_size++;
		for(int i = 0; i < m_size; i++){
			if(m_frameList[i].logicPageIndex == -1){
				m_frameList[i].logicPageIndex = logicPageIndex;
				logicPageIndex = i;
				break;
			}
		}
		printf("true\n");
	}
	else{
		printf("false0\n");
		j = m_size-1;
		m_frameList[leastRecentlyUsed[m_size-1]].logicPageIndex = -1;
		printf("false1\n");
	}
	for(int i = j; i > 0; i--){
		leastRecentlyUsed[i] = leastRecentlyUsed[i-1];
	}
	leastRecentlyUsed[0] = logicPageIndex;
	printf("true3\n");
}

void* CLOCKMemory::requestPage(int logicPageIndex){
	m_uRequestNums++;
	bool canFind = false;
	int index;
	for(int i = 0; i < m_size; i++){
		if(m_frameList[i].logicPageIndex == logicPageIndex ){
			canFind = true;
			index = i;
			break;
		}
	}
	if(!canFind){
		m_uPageFaultNums++;
		this->processPageFault(logicPageIndex);
	}
	else{
		// give it chance
		m_chances[index] = true;
	}
	// now we needn't return anything
	return NULL;
}
void CLOCKMemory::processPageFault(int logicPageIndex){
	m_index++;
	if(m_index < 0 || m_index > m_size-1)
		m_index = 0;
	while(m_chances[m_index]){
		m_chances[m_index] = false;
		m_index++;
		if(m_index < 0 || m_index > m_size-1)
			m_index = 0;
	}
	m_frameList[m_index] = Item{logicPageIndex};
	m_chances[m_index] = true;
}
