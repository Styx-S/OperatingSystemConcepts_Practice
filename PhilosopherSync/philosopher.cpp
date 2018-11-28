/*
 * philosopher.cpp
 *
 *  Created on: 2018年11月28日
 *      Author: styxs
 */
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define EATING_TIME 3
#define PHILOSOPHER_NUMS 5
#define LEFT(location) ((location - 1 +PHILOSOPHER_NUMS) % PHILOSOPHER_NUMS)
#define RIGHT(location) ((location + 1) % PHILOSOPHER_NUMS)


sem_t chopsticks[PHILOSOPHER_NUMS];
sem_t limitEatingMeanwhile;

void declareChopsticks(int philosopher, int chop){
	printf("[Philosopher %d]I picked up the number %d chopsticks\n", philosopher+1, chop+1);
}

void thinking(int i){
	printf("[Philosopher %d]I am thinking\n", i+1);
}
void eating1(int i){
	sem_wait(&limitEatingMeanwhile);
	sem_wait(&chopsticks[LEFT(i)]);
	declareChopsticks(i, LEFT(i));
	sem_wait(&chopsticks[RIGHT(i)]);
	declareChopsticks(i, RIGHT(i));
	printf("[Philosopher %d]I am eating\n", i+1);
	sleep(EATING_TIME);
	sem_post(&chopsticks[LEFT(i)]);
	sem_post(&chopsticks[RIGHT(i)]);
	sem_post(&limitEatingMeanwhile);
}
void* philosopherFcuntion(void *p){
	int i = *(int*)p;
	while(true){
		thinking(i);
		if(rand() % 2){
			printf("[Philosopher %d]I am hungry\n", i+1);
			eating1(i);
		}
		else sleep(1);
	}
}


int main(){
	srand(time(NULL));
	for(int i = 0; i < PHILOSOPHER_NUMS; i++)
		sem_init(&chopsticks[i], 0, 1);
	sem_init(&limitEatingMeanwhile, 0, PHILOSOPHER_NUMS-1);
	pthread_t philosophers[PHILOSOPHER_NUMS];
	int indexes[] = {0,1,2,3,4};
	for(int i = 0; i < PHILOSOPHER_NUMS; i++){
		pthread_create(&philosophers[i], NULL, philosopherFcuntion, &indexes[i]);
	}
	pthread_join(philosophers[0], NULL);
}

