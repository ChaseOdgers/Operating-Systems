source code:
-----------------------------
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#define BUFFER_SIZE 5
#define RAND 100
#define TRUE 1
pthread_mutex_t mutex;
sem_t full, empty;
int buffer[BUFFER_SIZE],count;
pthread_t tid;
pthread_attr_t attr;
void *producer(void *param);
void *consumer(void *param);
void initialize();
int enqueue(int);
int Dequeue(int *);
int main(int argc, char *argv[])
{
int i;
if(argc != 4)
{
fprintf(stderr, "USAGE:pass Three Arguements:");
}
int Sleep_Time = atoi(argv[1]);
int Producer_Num = atoi(argv[2]);
int Consumer_num = atoi(argv[3]);
initialize();
for(i=0;i<Producer_Num;i++)
{
pthread_create(&tid,&attr,producer,NULL);
}
for(i=0;i<Consumer_num;i++)
{
pthread_create(&tid,&attr,consumer,NULL);
}
sleep(Sleep_Time);
printf("***** Exit *******\n");
exit(0);
}
void initialize()
{
pthread_mutex_init(&mutex, NULL);
sem_init(&full, 0, 0);
sem_init(&empty, 0, BUFFER_SIZE);
pthread_attr_init(&attr);
count = 0;
}
void *producer(void *param)
{
int item;
while(TRUE)
{
int random_number = rand()/RAND;
sleep(random_number);
item = rand();
sem_wait(&empty);
pthread_mutex_lock(&mutex);
if(enqueue(item))
{
fprintf(stderr, " Producer report error condition\n");
}
else
{
printf("producer produced %d\n", item);
}
pthread_mutex_unlock(&mutex);
sem_post(&full);
}
}


void *consumer(void *param)
{
int item;
while(TRUE)
{
int random_number = rand()/RAND;
sleep(random_number);
sem_wait(&full);
pthread_mutex_lock(&mutex);
if(Dequeue(&item))
{
fprintf(stderr, "Consumer report error condition\n");
}
else
{
printf("consumer consumed %d\n", item);
}
pthread_mutex_unlock(&mutex);
sem_post(&empty);
}
}
int enqueue(int item)
{
if(count<BUFFER_SIZE)
{
buffer[count] = item;
count++;
return 0;
}
else
{
return -1;
}
}
int Dequeue(int *item)
{
if(counter>0)
{
*item = buffer[(count-1)];
count--;
return 0;
}
else
{
return -1;
}
}
