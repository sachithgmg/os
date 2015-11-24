#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
using namespace std;

#define N 10
#define NP 5
#define NC 3

int rear = 0, front = 0;
int buffer[N];

sem_t emptyCount, fillCount;
sem_t mutex;

void * produce (void * arg){
		
	int id = *((int*)arg);
	int item = 0;
	while(true){

		sem_wait(&emptyCount);
		sem_wait(&mutex);

		buffer[rear] = item;
		rear = (rear + 1) % N;
		cout << "Produced " << item << " by producer " << id << endl;
		item++;
		
		sleep(2);

		sem_post(&mutex);
		sem_post(&fillCount);

	}
	pthread_exit(NULL);
}

void * consume (void * arg){
	
	int id = *((int*)arg);
	int item;

	while(true){

		sem_wait(&fillCount);
		sem_wait(&mutex);

		item = buffer[front];
		front = (front + 1) % N;
		cout << "Item " << item << " consumed by consumer " << id << "\n" ;
		sleep(2);
		
		sem_post(&mutex);
		sem_post(&emptyCount);

		
	}
	pthread_exit(NULL);
}

int main () {
	
	sem_init (&emptyCount,0,N);
	sem_init (&fillCount,0,0);	
	sem_init (&mutex,0,1);

	pthread_t producer[NP], consumer[NC];
	
	int temp[100];
	for(int i = 0;i < NP;i++){
		temp[i] = i + 1;
		pthread_create(&producer[i],NULL,produce,&temp[i]);
	}
	for(int i = 0;i < NC;i++){
		temp[i] = i + 1;
		pthread_create(&consumer[i],NULL,consume,&temp[i]);
	}

	for(int i = 0;i < NP;i++)
		pthread_join (producer[i],NULL);
	for(int i = 0;i < NC;i++)
		pthread_join (consumer[i],NULL);
	
	return 0;
}

