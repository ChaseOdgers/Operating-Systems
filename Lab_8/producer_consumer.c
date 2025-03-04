#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

/*
 * Define constants for how big the shared queue should be and how
 * much total work the produceers and consumers should perform
 */
#define QUEUESIZE 5
#define WORK_MAX 30

/*
 * These constants specify how much CPU bound work the producer and
 * consumer do when processing an item. They also define how long each
 * blocks when producing an item. Work and blocking are implemented
 * int he do_work() routine that uses the msleep() routine to block
 * for at least the specified number of milliseconds.
 */
#define PRODUCER_CPU   25
#define PRODUCER_BLOCK 10
#define CONSUMER_CPU   25
#define CONSUMER_BLOCK 10

#define YOU_WILL_DETERMINE_FOR_PRODUCERS 100 //change 678 values to appropriate semaphore length
#define YOU_WILL_DETERMINE_FOR_CONSUMERS 100

/*****************************************************
 *   Shared Queue Related Structures and Routines    *
 *****************************************************/
typedef struct {
  // These should be protected by mutex
  int buf[QUEUESIZE];   /* Array for Queue contents, managed as circular queue */
  int head;             /* Index of the queue head */
  int tail;             /* Index of the queue tail, the next empty slot */

  // These two won't be used in the real solution
  int full;             /* Flag set when queue is full  */
  int empty;            /* Flag set when queue is empty */

  pthread_mutex_t *mutex;    /* Mutex protecting this Queue's data: buf, head and tail */
  sem_t  *slotsToPut;  		 /* Used by producers to await room to produce */
  sem_t  *slotsToGet; 		 /* Used by consumers to await something to consume */
} queue;

/*
 * Create the queue shared among all producers and consumers
 */
queue *queueInit (void)
{
  queue *q;

  /*
   * Allocate the structure that holds all queue information
   */
  q = (queue *)malloc (sizeof (queue));
  if (q == NULL)
	  return (NULL);
  /*
   * Initialize the state variables. See the definition of the Queue
   * structure for the definition of each.
   */
  q->empty = 1;
  q->full  = 0;

  q->head  = 0;
  q->tail  = 0;

  /*
   * Allocate and initialize the queue mutex
   */
  q->mutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mutex, NULL);

  /*
   * Allocate and initialize the slotsToPut and slotsToGet semaphores
   */
  q->slotsToPut = (sem_t *) malloc (sizeof (sem_t));
  sem_init (q->slotsToPut, 0, YOU_WILL_DETERMINE_FOR_PRODUCERS);

  q->slotsToGet = (sem_t *) malloc (sizeof (sem_t));
  sem_init (q->slotsToGet, 0, YOU_WILL_DETERMINE_FOR_CONSUMERS);

  return (q);
}

/*
 * Delete the shared queue, deallocating dynamically allocated memory
 */
void queueDelete (queue *q)
{
  /*
   * Destroy the mutex and deallocate its memory
   */
  pthread_mutex_destroy (q->mutex);
  free (q->mutex);

  /*
   * Destroy and deallocate the semaphores
   */
  sem_destroy(q->slotsToPut);
  free (q->slotsToPut);
  sem_destroy(q->slotsToGet);
  free (q->slotsToGet);

  /*
   * Deallocate the queue structure
   */
  free (q);
}

void queueAdd (queue *q, int in)
{

  /*
   * Put the input item into the free slot
   */
  q->buf[q->tail] = in;
  q->tail++;

  /*
   * Wrapping the value of tail around to zero if we reached the end of
   * the array. This implements the circularity of the queue inthe
   * array.
   */
  if (q->tail == QUEUESIZE)
    q->tail = 0;

  /*
   * If the tail pointer is equal to the head, then the enxt empty
   * slot in the queue is occupied and the queue is FULL
   */
  if (q->tail == q->head)
    q->full = 1;

  /*
   * Since we just added an element to the queue, it is certainly not
   * empty.
   */
  q->empty = 0;

  return;
}

void queueRemove (queue *q, int *out)
{
  /*
   * Copy the element at head into the output variable and increment
   * the head pointer to move to the next element.
   */
  *out = q->buf[q->head];
  q->head++;

  /*
   * Wrapping the index around to zero if it reached the size of the
   * array. This implements the circualrity of the queue int he array.
   */
  if (q->head == QUEUESIZE)
    q->head = 0;

  /*
   * If head catches up to tail as we delete an item, then the queue
   * is empty.
   */
  if (q->head == q->tail)
    q->empty = 1;

  /*
   * since we took an item out, the queue is certainly not full
   */
  q->full = 0;

  return;
}

/******************************************************
 *   Producer and Consumer Structures and Routines    *
 ******************************************************/
/*
 * Argument struct used to pass consumers and producers thier
 * arguments.
 *
 * q     - arg provides a pointer to the shared queue.
 *
 * count - arg is a pointer to a counter for this thread to track how
 *         much work it did.
 *
 * tid   - arg provides the ID number of the producer or consumer,
 *         whichis also its index into the array of thread structures.
 *
 */
typedef struct {
  queue *q;
  int   *count;
  int    tid;
} pcdata;

int memory_access_area[100000];


/*
 * Sleep for a specified number of milliseconds. We use this to
 * simulate I/O, since it will block the process. Different lengths fo
 * sleep simulate interaction with different devices.
 */
void msleep(unsigned int milli_seconds)
{
  struct timespec req = {0}; /* init struct contents to zero */
  time_t          seconds;

  /*
   * Converting number of milliseconds input to seconds and residual
   * milliseconds to handle the cse where input is more than one
   * thousand milliseconds.
   */
  seconds        = (milli_seconds/1000);
  milli_seconds  = milli_seconds - (seconds * 1000);

  /*
   * Fill in the time_spec's seconds and nanoseconds fields. Note we
   * multiply millisconds by 10^6 to convert to nanoseconds.
   */
  req.tv_sec  = seconds;
  req.tv_nsec = milli_seconds * 1000000L;

  /*
   * Sleep for the required period. The first parameter specifies how
   * long. In theory this thread can be awakened before the period is
   * over, perhaps by a signal. If so the timespec specified by the
   * second argument is filled in with how much time int he original
   * request is left. We use the same one. If this happens, we just
   * call nanosleep again to sleep for what remains of the origianl
   * request.
   */
  while(nanosleep(&req, &req)==-1) {
    printf("restless\n");
    continue;
  }

}

/*
 * Simulate doing work.
 */
void do_work(int cpu_iterations, int blocking_time)
{
  int i;
  int j;
  int local_var;

  local_var = 0;
  for (j = 0; j < cpu_iterations; j++ ) {
    for (i = 0; i < 1000; i++ ) {
      local_var = memory_access_area[i];
    }
  }

  if ( blocking_time > 0 ) {
    msleep(blocking_time);
  }
}

void *producer (void *parg)
{
  queue  *fifo;
  int     item;
  pcdata *mydata;
  int     my_tid;
  int    *total_inserted;

  mydata = (pcdata *) parg;

  fifo           = mydata->q;
  total_inserted = mydata->count; // this one is also shared!
  my_tid         = mydata->tid;

  /*
   * Continue producing until the total produced reaches the
   * configured maximum
   */
  while (1) {
    /*
     * Do work to produce an item. Then get a slot in the queue for
     * it. Finally, at the end of the loop, outside the critical
     * section, announce that we produced it.
     */
    pthread_mutex_lock(fifo->mutex);
    do_work(PRODUCER_CPU, PRODUCER_BLOCK);

    /*
     * If the queue is full, we have no place to put anything we
     * produce, so wait until it is not full. Use sem_wait on the
     * appropriate semaphore from the fifo queue, for the producer.
     */
     while(fifo->full && *total_inserted != WORK_MAX){
       printf ("prod %d:  FULL.\n", my_tid);
       sem_wait(fifo->slotsToPut);
     }

	/*
     * Check to see if the total produced by all producers has reached
     * the configured maximum, if so, we can quit. Before quitting, execute some
     * additional sem_post for the producers and consumers to free up ones
     * that are in the waiting queue for fifo->slotsToPut and fifo->slotsToGet respectively
     */
    if (*total_inserted >= WORK_MAX) {
      pthread_mutex_unlock(fifo->mutex);
      sem_post(fifo->slotsToGet);
      break;
    }

    /*
     * OK, so we produce an item. Increment the counter of total
     * widgets produced, and add the new widget ID, its number, to the
     * queue. We are accessing the shared queue fifo in this section.
     * So, we should ensure mutual exclusion.
     */

    item = (*total_inserted);
    queueAdd (fifo, item);
  	++(*total_inserted);

    pthread_mutex_unlock(fifo->mutex);
    /*
     * Announce the production outside the critical section
	 * Let the consumers know that there is item in the buffer
     */
    sem_post(fifo->slotsToPut);

    printf("prod %d:  %d.\n", my_tid, item);

  }

  printf("prod %d:  exited\n", my_tid);
  return (NULL);
}

void *consumer (void *carg)
{
  queue  *fifo;
  int     item;
  pcdata *mydata;
  int     my_tid;
  int    *total_consumed;

  mydata = (pcdata *) carg;

  fifo           = mydata->q;
  total_consumed = mydata->count; // this one is also shared!
  my_tid         = mydata->tid;

  /*
   * Continue producing until the total consumed by all consumers
   * reaches the configured maximum
   */
  while (1) {
    /*
     * If the queue is empty, we have no place to put anything we
     * produce, so wait until it is not empty. Use sem_wait on the
     * appropriate semaphore from the fifo queue, for the consumer.
     */
     pthread_mutex_lock(fifo->mutex);
     while(fifo->empty && *total_consumed != WORK_MAX){
       printf("con %d:  EMPTY. \n",my_tid);
       sem_wait(fifo->slotsToGet);
     }

	/*
     * If total consumption has reached the configured limit, we can
     * stop. Before stopping, execute some additional sem_post for the
     * producers and consumers to free up ones that are in the waiting
     * queue for fifo->slotsToPut and fifo->slotsToGet respectively
     */
    if (*total_consumed >= WORK_MAX) {
      pthread_mutex_unlock(fifo->mutex);
      sem_post(fifo->slotsToGet);
      break;
    }


    /*
     * Remove the next item from the queue. Increment the count of the
     * total consumed. Note that item is a local copy so this
     * thread can retain a memory of which item it consumed even if
     * others are busy consuming them. We are accessing the shared queue
     * fifo in this section. So, we should ensure mutual exclusion.
     */
    queueRemove (fifo, &item);
    (*total_consumed)++;
    pthread_mutex_unlock(fifo->mutex);
    sem_post(fifo->slotsToGet);
    /*
     * Do work outside the critical region to consume the item
     * obtained from the queue and then announce its consumption.
	 * Also, notify the producers that there is available space
	 * in the buffer
     */
    do_work(CONSUMER_CPU,CONSUMER_CPU);
    printf ("con %d:   %d.\n", my_tid, item);

  }

  printf("con %d:   exited\n", my_tid);
  return (NULL);
}

/***************************************************
 *   Main allocates structures, creates threads,   *
 *   waits to tear down.                           *
 ***************************************************/
int main (int argc, char *argv[])
{
  pthread_t *con;
  int        cons;
  int       *concount;

  queue     *fifo;
  int        i;

  pthread_t *pro;
  int       *procount;
  int        pros;

  pcdata    *thread_args;

  /*
   * Check the number of arguments and determine the number of
   * producers and consumers
   */
  if (argc != 3) {
    printf("Usage: producer_consumer number_of_producers number_of_consumers\n");
    exit(0);
  }

  pros = atoi(argv[1]);
  cons = atoi(argv[2]);

  /*
   * Create the shared queue
   */
  fifo = queueInit ();
  if (fifo ==  NULL) {
    fprintf (stderr, "main: Queue Init failed.\n");
    exit (1);
  }

  /*
   * Create a counter tracking how many items were produced, shared
   * among all producers, and one to track how many items were
   * consumed, shared among all consumers.
   */
  procount = (int *) malloc (sizeof (int));
  if (procount == NULL) {
    fprintf(stderr, "procount allocation failed\n");
    exit(1);
  }
  *procount=0;

  concount = (int *) malloc (sizeof (int));
  if (concount == NULL) {
    fprintf(stderr, "concount allocation failed\n");
    exit(1);
  }
  *concount=0;

  /*
   * Create arrays of thread structures, one for each producer and
   * consumer
   */
  pro = (pthread_t *) malloc (sizeof (pthread_t) * pros);
  if (pro == NULL) {
    fprintf(stderr, "pros\n");
    exit(1);
  }

  con = (pthread_t *) malloc (sizeof (pthread_t) * cons);
  if (con == NULL) {
    fprintf(stderr, "cons\n");
    exit(1);
  }

  /*
   * Create the specified number of producers
   */
  for (i=0; i<pros; i++){
    /*
     * Allocate memory for each producer's arguments
     */
    thread_args = (pcdata *)malloc (sizeof (pcdata));
    if (thread_args == NULL) {
      fprintf (stderr, "main: Thread_Args Init failed.\n");
      exit (1);
    }

    /*
     * Fill them in and then create the producer thread
     */
    thread_args->q     = fifo;
    thread_args->count = procount;
    thread_args->tid   = i;
    pthread_create (&pro[i], NULL, producer, thread_args);
  }

  /*
   * Create the specified number of consumers
   */
  for (i=0; i<cons; i++){
    /*
     * Allocate space for next consumer's args
     */
    thread_args = (pcdata *)malloc (sizeof (pcdata));
    if (thread_args == NULL) {
      fprintf (stderr, "main: Thread_Args Init failed.\n");
      exit (1);
    }

    /*
     * Fill them in and create the thread
     */
    thread_args->q     = fifo;
    thread_args->count = concount;
    thread_args->tid   = i;
    pthread_create (&con[i], NULL, consumer, thread_args);
  }

  /*
   * Wait for the create producer and consumer threads to finish
   * before this original thread will exit. We wait for all the
   * producers before waiting for the consumers, but that is an
   * unimportant detail.
   */
  for (i=0; i<pros; i++)
    pthread_join (pro[i], NULL);
  for (i=0; i<cons; i++)
    pthread_join (con[i], NULL);

  /*
   * Delete the shared fifo, now that we know there are no users of
   * it. Since we are about to exit we could skip this step, but we
   * put it here for neatness' sake.
   */
  queueDelete (fifo);

  return 0;
}
