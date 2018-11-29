/*
 * frame.h
 *
 *  Created on: 2018年11月28日
 *      Author: styxs
 */

#ifndef MEMORY_H_
#define MEMORY_H_


typedef struct{
	int logicPageIndex = -1;
}Item;
class Memory{
public:
	Memory(int size);
	virtual ~Memory();
	virtual void* requestPage(int logicPageIndex);
	virtual void processPageFault(int logicPageIndex) = 0;
	virtual void init();
	double reportPageFaultRate();
protected:
	Item* m_frameList;
	int m_size;
	int m_uPageFaultNums;
	int m_uRequestNums;
};


class FIFOMemory : public Memory{
public:
	FIFOMemory(int size);
	virtual ~FIFOMemory();
	virtual void* requestPage(int logicPageIndex){return Memory::requestPage(logicPageIndex);}
	virtual void processPageFault(int logicPageIndex);
	void init();
protected:
	// implement by queue or just a array with a index(point to the oldest item)
	int indexPoint2Oldest;
};

class LRUMemory : public Memory{
public:
	LRUMemory(int size);
	virtual ~LRUMemory();
	virtual void* requestPage(int logicPageIndex);
	virtual void processPageFault(int logicPageIndex);
	void init();
protected:
	int *leastRecentlyUsed;
	int count;
};

class CLOCKMemory : public Memory{
public:
	CLOCKMemory(int size);
	virtual ~CLOCKMemory();
	virtual void* requestPage(int logicPageIndex);
	virtual void processPageFault(int logicPageIndex);
	void init();
protected:
	bool* m_chances;
	int m_index;
};



#endif /* MEMORY_H_ */
