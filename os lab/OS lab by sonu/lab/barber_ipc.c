#include<pthread.h>
#include "mysem.c"
#define MAX_C 5
int sem_ID;         //----> creates a set of 5 semaphores. { mutex, cond_empty, counting semaphore, waiting semaphore, barber semaphore}
int customers_count=0;
int eflag=0,fflag=0;
void *barber(void *x)
{
 printf("barber started\n");
 while(1)
 {

  sem_change(sem_ID, 0, -1);

  if(customers_count==0)
  {
   printf("barber sleeping\n");
   eflag=1;
   sem_change(sem_ID, 0, 1);
   sem_change(sem_ID, 1, -1);
   sem_change(sem_ID, 0, -1);
  }

  customers_count--;

  sem_change(sem_ID, 0, 1);
  sem_change(sem_ID, 3, 1);
 // printf("tail:%d\n", tail);
 // pthread_mutex_lock(&B);

  sem_change(sem_ID, 4, -1);
 // pthread_mutex_unlock(&B);

  }
}
void *customer(void *arg)
{

  //printf("C\n");
 sem_change(sem_ID, 0, -1);
 if(customers_count==MAX_C)       // If all seats are occupied exit the thread
  {
   int *ptr=(int*)arg;
   *ptr=0;
   printf("No place for customer %u so leaving\n", pthread_self());
   sem_change(sem_ID, 0, 1);
  }
   else{
    customers_count++;

      if(customers_count==1 && eflag==1)
      {
      sem_change(sem_ID, 1, 1);
      eflag=0;
      }
      sem_change(sem_ID, 0, 1);
      printf("Customer %u got a place\n", pthread_self());
      sem_change(sem_ID, 3, -1);
      printf("Cutting for %u customer\n", pthread_self());
      sleep(rand()%5+4);
      sem_change(sem_ID, 4, 1);
      int *ptr=(int*)arg;
      *ptr=0;
   }
}
int main(int argc, char* argv[])
{
pthread_t  barber_thread;
int   live_threads[MAX_C+2];     // 0 = no thread is created with this index,

          // 1=there is a live thread with this index

pthread_t  customer_thread[MAX_C +2];


for(int i=0; i<MAX_C+2; i++)
 live_threads[i]=0;     // initially all are dead state


int array[]={1, 0, MAX_C, 0, 0};     // Initial values of different semaphores
sem_ID=sem_init_diff_val(5,array);

pthread_create(&barber_thread, NULL,barber, NULL);

sleep(2);
//Continuous thread generator....
while(1)
{
 for(int i=0; i<MAX_C+2; i++)
 {
  if(live_threads[i]==0)
  {
   live_threads[i]=1;
   pthread_create(&customer_thread[i], NULL,customer, (void*)&live_threads[i]);
   sleep(rand()%4);
  }

 }
}
exit(0);
}
