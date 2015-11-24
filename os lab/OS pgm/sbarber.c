/*
	The below shows sleeping barber solution as given in the wikipedia section. Some modification has been done to synchronise
	barber haircutting and customer getting haircut. Both mutexes and semaphores have been used in the code.
*/

#include<stdio.h>
#include<sys/sem.h>
#include<pthread.h>
#include<semaphore.h>

#define MAXCHAIRS 5

sem_t barber_ready;		// barber is ready or not
sem_t cust_ready;	// no of customers waiting in room
pthread_mutex_t access_seats = PTHREAD_MUTEX_INITIALIZER, dummy_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barber_busy = PTHREAD_COND_INITIALIZER;	// to make the barber wait for the current customer to go
int free_seats = MAXCHAIRS;

void barber()
{
	while(1)
	{
		sem_wait(&cust_ready);	// if any customer is ready then proceed, else sleep
		pthread_mutex_lock(&access_seats);	// acquire lock to change free_seats variable
			free_seats += 1;
			sem_post(&barber_ready);	// the barber makes himself available for cutting hair
		pthread_mutex_unlock(&access_seats);	// release lock
		
		printf("The barber is cutting\n");
		
		// make the barber wait till the customer does not signal that his cutting is finished
		pthread_mutex_lock(&dummy_mutex);
		pthread_cond_wait(&barber_busy , &dummy_mutex);
		pthread_mutex_unlock(&dummy_mutex);
	}
}

void customer(void* c)
{
	int cust_no = *((int*)c);
	while(1)
	{
		pthread_mutex_lock(&access_seats);	// acquire lock
		if(free_seats>0)
		{
			free_seats--;	// sit on a seat
			sem_post(&cust_ready);	// one more customer is ready (show in semaphore). This may wake up a sleeping barber
			pthread_mutex_unlock(&access_seats);
			sem_wait(&barber_ready);	// make the barber unavailable for others
			printf("Customer %d is having a hair cut\n", cust_no);
			sleep(rand()%4);
			printf("Customer %d finished haircut\n", cust_no);
			pthread_cond_signal(&barber_busy);	// inform barber that cutting is done
		}
		else
		{
			pthread_mutex_unlock(&access_seats);	// leave shop without cutting. Release lock before going
			printf("Customer %d leaving without haircut\n", cust_no);
		}
		sleep(4);	// gap between 2 continuous arrivals of the same customer
	}
}

int main()
{
	int i, id[MAXCHAIRS+2];
	
	sem_init(&barber_ready, 0, 0);	// initially barber is not ready
	sem_init(&cust_ready, 0, 0);	// initially no customers are waiting
	
	pthread_t brbr, cust[MAXCHAIRS+2];	// some extra customers are defined
	pthread_attr_t brbr_attr;
	
	pthread_attr_init(&brbr_attr);
	pthread_create(&brbr, &brbr_attr, (void*)&barber, 0);
	//pthread_create(&brbr, NULL, (void*)&barber, 0);
	
	for(i=0 ; i<MAXCHAIRS+2 ; ++i)
	{
		id[i] = i;
		pthread_create(&cust[i], NULL, (void*)customer, (void*)(&id[i]));
	}
	pthread_join(cust[0], NULL);
	return 0;
}
