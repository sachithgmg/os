#include<unistd.h>
# include <cstdio>
#include<cstdlib>
# include <pthread.h>
# define BufferSize 10

void *Producer(void *x);
void *Consumer(void *x);

int BufferIndex=0;
char *BUFFER;

pthread_cond_t Buffer_Not_Full=PTHREAD_COND_INITIALIZER;
pthread_cond_t Buffer_Not_Empty=PTHREAD_COND_INITIALIZER;
pthread_mutex_t mVar=PTHREAD_MUTEX_INITIALIZER;

int main()
{
    pthread_t ptid,ctid;

    BUFFER=(char *) malloc(sizeof(char) * BufferSize);

    pthread_create(&ptid,NULL,Producer,NULL);
    pthread_create(&ctid,NULL,Consumer,NULL);

    pthread_join(ptid,NULL);
    pthread_join(ctid,NULL);

    return 0;
}

void *Producer(void *x)
{
    for(;;)
    {
        pthread_mutex_lock(&mVar);
        if(BufferIndex==BufferSize)
        {
            pthread_cond_wait(&Buffer_Not_Full,&mVar);
        }
        BUFFER[BufferIndex++]='#';
        printf("Produce : %d \n",BufferIndex);
        pthread_mutex_unlock(&mVar);
        pthread_cond_signal(&Buffer_Not_Empty);
        sleep(rand()%5);
    }

}

void *Consumer(void *x)
{
    for(;;)
    {
        pthread_mutex_lock(&mVar);
        if(BufferIndex==-1)
        {
            pthread_cond_wait(&Buffer_Not_Empty,&mVar);
        }
        printf("Consume : %d \n",BufferIndex--);
        pthread_mutex_unlock(&mVar);
        pthread_cond_signal(&Buffer_Not_Full);
        sleep(rand()%5);
    }
}
