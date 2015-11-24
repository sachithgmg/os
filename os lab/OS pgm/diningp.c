/*
	The below code is a solution to the problem of 'Dining Philosopher's Problem'. The approach used below is the one given by
	sir in class. Improvements are always welcome to the below code
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define PHIL_COUNT 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
typedef enum {HUNGRY, THINKING, EATING} phil_state;		// from 0 to 2
phil_state state[PHIL_COUNT+1];		// index from 1
sem_t spoon[PHIL_COUNT+1];		// index from 1
sem_t phil_sem[PHIL_COUNT+1];	// index from 1

void* philosopher(void* p);
void pick_forks(int i);
void put_forks(int i);
void thinking(int i);
void test(int i);
void eat(int i);

int main()
{
	int i, j, id[PHIL_COUNT];
	pthread_t phil[PHIL_COUNT];
	
	pthread_mutex_init(&mutex, NULL);	// initialise with NULL attribute

	
	for(i=0 ; i<PHIL_COUNT ; ++i)
	{
		state[i+1] = THINKING;	// gives an integer value of 1 to the state
		sem_init(&spoon[i+1], 0, 1);
		sem_init(&phil_sem[i+1], 0, 0);
		// printf("state = %d\n", state[i]);
	}
	
	for(i=0 ; i<PHIL_COUNT ; ++i)
	{
		id[i] = i+1;	// need to send different variables to each thread
		pthread_create(&phil[i], NULL, philosopher, (void*)&id[i]);
	}
	
	for(i=0 ; i<PHIL_COUNT ; ++i)
	{
		pthread_join(phil[i], NULL);
	}
	
	return 0;
}

void* philosopher(void *p)
{
	int ph = *((int*)p);
	printf("philosopher %d started\n", ph);
	while(1)
	{
		thinking(ph);
		pick_forks(ph);
		eat(ph);
		put_forks(ph);
	}
}

void pick_forks(int i)
{
	pthread_mutex_lock(&mutex);
		state[i] = HUNGRY;
		test(i);
	pthread_mutex_unlock(&mutex);
	sem_wait(&phil_sem[i]);
}

void put_forks(int i)
{
	pthread_mutex_lock(&mutex);
		state[i] = THINKING;
		//sem_post(&spoon[(((i+1)%PHIL_COUNT == 0)?PHIL_COUNT:(i+1)%PHIL_COUNT)]);
		//sem_post(&spoon[(((i-1) == 0)?PHIL_COUNT:i-1)]);
		test(((i+1)%PHIL_COUNT == 0)?PHIL_COUNT:(i+1)%PHIL_COUNT);
		test(((i-1) == 0)?PHIL_COUNT:i-1);
	pthread_mutex_unlock(&mutex);
}

void thinking(int i)
{
	printf("philosopher %d is thinking\n", i);
	sleep(rand()%5);
}

void test(int i)
{
	if((state[i] == HUNGRY) && state[(((i+1)%PHIL_COUNT == 0)?PHIL_COUNT:(i+1)%PHIL_COUNT)] != EATING 
						&& state[(((i-1) == 0)?PHIL_COUNT:i-1)] != EATING)
	{
		//sem_wait(&spoon[(((i+1)%PHIL_COUNT == 0)?PHIL_COUNT:(i+1)%PHIL_COUNT)]);
		//sem_wait(&spoon[(((i-1) == 0)?PHIL_COUNT:i-1)]);	
		state[i] = EATING;
		sem_post(&phil_sem[i]);
	}
}

void eat(int i)
{
	printf("Philosopher %d is eating\n", i);
	sleep(rand()%5);
	printf("Philosopher %d finished eating\n", i);
}
