#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
const int ITERATIONS=5;
int chopstick[5];
int philosphor(int n)
{
 int i,j,first,second;
 struct sembuf op;
 op.sem_num=0;
 op.sem_flg=0;
 printf("Philosopher %d was born ! \n",n);
 first=(n<4)?n:0;
 second=(n<4)?n+1:4;
 for(i=0;i<ITERATIONS;i++)
 {
  printf("Philosophor %d is thinking...\n",n);
  op.sem_op=-1;
  semop(chopstick[first],&op,1);
  op.sem_op=-1;
  semop(chopstick[second],&op,1);
  printf("Philosophor %d is eating....\n",n);
  op.sem_op=+1;
  semop(chopstick[first],&op,1);
  op.sem_op=+1;
  semop(chopstick[second],&op,1);
  sleep(5);
 }
 exit(n);
}
int main()
{
 int i,status;
 pid_t phil[5];
 for(i=0;i<5;i++) //1st loop
 {
  if((chopstick[i]=semget(IPC_PRIVATE,1,IPC_CREAT|0600))<0)
   return -1;
  if(semctl(chopstick[i],0,SETVAL,1)<0)
   return -1;
 }
 for(i=0;i<5;i++) //2nd loop
 {
  if((phil[i]=fork())==0)
   exit(philosphor(i));
 }
 for(i=0;i<5;i++) //3rd loop
 {
  waitpid(phil[i],&status,0);
  if(WEXITSTATUS(status)==i)
   printf("Philosophor %d went to heaven \n",i);
  else 
   printf("Philosophor %d went to hell \n",i);
 }
 for(i=0;i<5;i++) //4th loop
 {
  semctl(chopstick[i],0,IPC_RMID,0);
 }
 return 0;
}

