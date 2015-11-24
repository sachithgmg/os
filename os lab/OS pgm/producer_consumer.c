#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include<semaphore.h>

#define MAX_BUF 5

int fillCount = 0, emptyCount = MAX_BUF;
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

sem_t buffer_sem;

void produce(void* i);
void consume(void* i);

// The below two functions use conditional mutexes
void produce_good(void* i);
void consume_good(void* i);

// The below two use semaphores
void produce_sem(void* i);
void consume_sem(void* i);

int main(int argc, char **argv[])
{
	pthread_t P,C; //one producer and one consumer
	int pid, cid;
	pid = 1; 
	cid = 1;
	
	//pthread_create(&P, NULL, (void*)&produce, (void*)(&pid));   //address of P should be passed
	//pthread_create(&C, NULL, (void*)&consume, (void*)(&cid));
	
	 pthread_create(&P, NULL, (void*)&produce_good, (void*)(&pid));
	 pthread_create(&C, NULL, (void*)&consume_good, (void*)(&cid));
	
	sem_init(&buffer_sem, 0, 1);
	//pthread_create(&P, NULL, (void*)&produce_sem, (void*)(&pid));
	//pthread_create(&C, NULL, (void*)&consume_sem, (void*)(&cid));
	
	pthread_join(P, NULL);  //main program will wait for the thread to terminate
	pthread_join(C, NULL);
	return 0;
}

void produce(void* i)
{
	int p = *((int*)(i));
	printf("Producer %d created\n", p);
	while(1)
	{
		pthread_mutex_lock(&buffer_mutex);
		if(emptyCount != 0)
		{
			printf("Producer %d is producing\n", p);
			sleep(rand()%5);
			fillCount++;
			emptyCount--;
		}
		pthread_mutex_unlock(&buffer_mutex);
	}
}

void produce_sem(void* i)
{
	int p = *((int*)(i));
	printf("Producer %d created\n", p);
	while(1)
	{
		// pthread_mutex_lock(&buffer_mutex);
		sem_wait(&buffer_sem);
		if(emptyCount != 0)
		{
			printf("Producer %d is producing\n", p);
			sleep(rand()%5);
			fillCount++;
			emptyCount--;
		}
		// pthread_mutex_unlock(&buffer_mutex);
		sem_post(&buffer_sem);
	}
}

void consume_sem(void* i)
{
	int c = *((int*)i);
	printf("Consumer %d created\n", c);
	while(1)
	{
		// pthread_mutex_lock(&buffer_mutex);
		sem_wait(&buffer_sem);
		if(fillCount != 0)
		{
			printf("Consumer %d is consuming\n", c);
			sleep(rand()%4);
			fillCount--;
			emptyCount++;
		}
		// pthread_mutex_unlock(&buffer_mutex);
		sem_post(&buffer_sem);
	}
}

void consume(void* i)
{
	int c = *((int*)i);
	printf("Consumer %d created\n", c);
	while(1)
	{
		pthread_mutex_lock(&buffer_mutex);
		if(fillCount != 0)
		{
			printf("Consumer %d is consuming\n", c);
			sleep(rand()%4);
			fillCount--;
			emptyCount++;
		}
		pthread_mutex_unlock(&buffer_mutex);
	}
}

void produce_good(void* i)
{
	int p = *((int*)(i));
	printf("Producer %d created\n", p);
	while(1)
	{
		printf("Producer %d is producing\n", p);
		sleep(rand()%2);
		
		pthread_mutex_lock(&buffer_mutex);
		if(emptyCount == 0) //no space to produce
		{
			pthread_cond_wait(&buffer_not_full ,&buffer_mutex);
		}
				
		//printf("Producer %d is producing\n", p);
		//sleep(rand()%5);
		fillCount++;
		emptyCount--;
		pthread_mutex_unlock(&buffer_mutex);
		pthread_cond_signal(&buffer_not_empty);
	}
}

void consume_good(void* i)
{
	int c = *((int*)i);
	printf("Consumer %d created\n", c);
	while(1)
	{
		printf("Consumer %d is consuming\n", c);
		sleep(rand()%1);
		
		pthread_mutex_lock(&buffer_mutex);
		if(fillCount == 0) //no item to consume
		{
			pthread_cond_wait(&buffer_not_empty ,&buffer_mutex);
		}
		
		//printf("Consumer %d is consuming\n", c);
		//sleep(rand()%4);
		fillCount--;
		emptyCount++;
		pthread_mutex_unlock(&buffer_mutex);
		pthread_cond_signal(&buffer_not_full);
	}
}
