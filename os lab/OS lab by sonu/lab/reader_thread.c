#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<cstdlib>
pthread_mutex_t  writer_wait, db, counter;
int no_of_readers=0;

/*sub functions for reader and writer threads*/
void check_and_wait_if_busy(int id)
{
  if(pthread_mutex_trylock(&writer_wait)!=0){
  printf("Writer %d Waiting\n", id);
  pthread_mutex_lock(&writer_wait);
 }
}
void check_and_wait(int id)
{
 if(pthread_mutex_trylock(&writer_wait)!=0){
  printf("Reader %d Waiting\n", id);
  pthread_mutex_lock(&writer_wait);
 }
}
void read(int id)
{
 pthread_mutex_lock(&counter);
 no_of_readers++;
 pthread_mutex_unlock(&counter);
 if(no_of_readers==1)
  pthread_mutex_lock(&db);
 pthread_mutex_unlock(&writer_wait);
 printf("reader %d reading...\n", id);
 sleep(rand()%5);
 printf("reader %d finished reading\n", id);

 pthread_mutex_lock(&counter);
 no_of_readers--;
 pthread_mutex_unlock(&counter);
 if(no_of_readers==0)
  pthread_mutex_unlock(&db);

}

void write(int id)
{
 pthread_mutex_lock(&db);
 pthread_mutex_unlock(&writer_wait);
 printf("Writer %d writing...\n", id);
 sleep(rand()%4+2);
 printf("Writer %d finished writing\n", id);
 pthread_mutex_unlock(&db);
}

void *reader(void *arg)
{

 int id=*((int*)arg);
 printf("reader %d started\n", id);
 while(1)
 {
  sleep(rand()%4);
 check_and_wait(id);
 read(id);
}
}


void *writer(void* arg)
{
 int id=*((int*)arg);
 printf("writer %d started\n", id);
 while(1)
{
 sleep(rand()%5);
 check_and_wait_if_busy(id);
 write(id);
}
}


/**************************************************/
//MAIN
int main(int argc, char* argv[])
{
 pthread_t R[5],W[5];
 int ids[5];
 for(int i=0; i<5; i++)
 {
  ids[i]=i+1;
  pthread_create(&R[i], NULL, reader, (void*)&ids[i]);
  pthread_create(&W[i], NULL, writer, (void*)&ids[i]);
 }
 pthread_join(R[0], NULL);
 exit(0);
}

