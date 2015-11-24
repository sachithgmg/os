/*
	reader_writer problem giving preference neither to the reader nor to the writer.
*/

#include <stdio.h>
#include <sys/sem.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

sem_t order_mutex;		// to enforce the ordering of the incoming processess (threads)
sem_t access_mutex;		// to access the resource
sem_t rc_mutex;			// to resrict access to read_count variable
int read_count = 0;

void writer(void *ww)
{
	while(1)
	{
	int w_num = *((int*)ww);
	sem_wait(&order_mutex);
		sem_wait(&access_mutex);
	sem_post(&order_mutex);
	
	printf("Writer %d is writing\n", w_num);
	sleep(2);
	printf("Writer %d finished writing\n", w_num);
	
		sem_post(&access_mutex);
	}
}

void reader(void *rr)
{
	while(1)
	{
	int r_num = *((int*)rr);
	sem_wait(&order_mutex);
		sem_wait(&rc_mutex);
			read_count++;
			if(read_count == 1)
				sem_wait(&access_mutex);
	sem_post(&order_mutex);
		sem_post(&rc_mutex);
	
	printf("Reader %d is reading\n", r_num);
	sleep(2);
	printf("Reader %d finished reading\n", r_num);
	
	sem_wait(&rc_mutex);
		read_count--;
		if(read_count == 0)
			sem_post(&access_mutex);
	sem_post(&rc_mutex);
	}
}	
int main(int argc, char **argv[])
{
	pthread_t R[5], W[5];   // declare 5 readers and 5 writers (thread id)
	sem_init(&rc_mutex, 0, 1);				// pritect read_count value
	sem_init(&order_mutex, 0, 1);			// max number of readers
	sem_init(&access_mutex, 0, 1);			// max number of writers

	int ids[5], i;
	for(i=0 ; i<5 ; ++i)
	{
		ids[i] = i+1;
		pthread_create(&R[i], NULL, (void*)&reader, (void*)&ids[i]);
	}
	for(i=0 ; i<5 ; ++i)
	{
		ids[i] = i+1;
		pthread_create(&W[i], NULL, (void*)&writer, (void*)&ids[i]);
	}
	for(i=0 ; i<5 ; ++i)
	{
		pthread_join(R[i], NULL);
	}
	for(i=0 ; i<5 ; ++i)
	{
		pthread_join(W[i], NULL);
	}
	return 0;
}
