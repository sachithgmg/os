//3rd version of reader and writer problem
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
int no_wait,no_acc,count,*readers,sm;
int *buffer,shmid,n;
void sem_lock(int semid)
{ 
 struct sembuf sb;
 sb.sem_num=0;
 sb.sem_op=-1;
 sb.sem_flg=0;
 if((semop(semid,&sb,1))==-1)    
 {
  perror("\nFailed to acquire semaphore.");
  exit(0);
 }
}

void sem_unlock(int semid)
{
 struct sembuf sb;
 sb.sem_num=0;
 sb.sem_op=1;
 sb.sem_flg=0;
 if((semop(semid,&sb,1))==-1)    
 {
  perror("\nFailed to release semaphore.");
  exit(0);
 }
}
void writer(int id)
{
 printf("writer %d has come \n",id);
 while(1)
 {
  sem_lock(no_wait);
  sem_lock(no_acc);
  printf("Writer %d is writing\n",id);
  sleep(2);
  sem_unlock(no_wait);
  sem_unlock(no_acc);
  sleep(5);
 }
 exit(id);
}
void reader(int id)
{
 printf("Reader %d has come\n",id );
 while(1)
 {
  sem_lock(no_wait);
  sem_lock(count);
  (*readers)++;

  if((*readers)==1)
   sem_lock(no_acc);
  sem_unlock(count);
  sem_unlock(no_wait);
  printf("reader %d is reading\n",id);
  sem_lock(count);
   (*readers)--;
  if((*readers)==0)
   sem_unlock(no_acc);
  sem_unlock(count);
  sleep(1);
 }
 exit(id);
}
int main()
{
 n=10;
 int i,k;
 if((no_wait=semget(IPC_PRIVATE,1,0666|IPC_CREAT))==-1)
 {
  printf("error in creation semaphore\n");
  exit(0);
 }
 if((semctl(no_wait,0,SETVAL,1))==-1)
 {
  printf("error to set value\n");
 }
 if((no_acc=semget(IPC_PRIVATE,1,0666|IPC_CREAT))==-1)
 {
  printf("error in creation semaphore\n");
  exit(0);
 }
 if((semctl(no_acc,0,SETVAL,1))==-1)
 {
  printf("error to set value\n");
 }
 if((count=semget(IPC_PRIVATE,1,0666|IPC_CREAT))==-1)
 {
  printf("error in creation semaphore\n");
  exit(0);
 }
 if((semctl(count,0,SETVAL,1))==-1)
 {
  printf("error to set value\n");
 }
 if((sm=shmget(IPC_PRIVATE,sizeof(int),0666|IPC_CREAT))==-1)
 {
  printf("error in creation semaphore\n");
  exit(0);
 }
 readers=(int *)shmat(sm,(const void *)0,0);
 //printf("%d\n",*readers);
  
 for(i=0;i<5;i++)
 {
  k=fork();
  if(k==0)
   writer(i+1);
  k=fork();
  if(k==0)
   reader(i+1);
 }
 reader(6);
 exit(0);
}


