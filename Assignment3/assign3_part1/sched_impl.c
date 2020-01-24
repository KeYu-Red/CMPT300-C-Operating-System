#include "scheduler.h"
#include "sched_impl.h"
#include <stdio.h>
#include <stdlib.h>

/* Fill in your scheduler implementation code below: */

static void init_thread_info(thread_info_t *info, sched_queue_t *queue)
{
	/*...Code goes here...*/
	info->queue = queue;
	info->elem_queue = NULL;
	info->ava_cpu = malloc(sizeof(pthread_mutex_t));
	info->yield_cpu = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	*(info->ava_cpu) = lock;
	*(info->yield_cpu) = lock;
	//make cpu not avaliable first
	pthread_mutex_lock(info->ava_cpu);
	pthread_mutex_lock(info->yield_cpu);
}

static void destroy_thread_info(thread_info_t *info)
{
	/*...Code goes here...*/
	info->queue = NULL;
	info->elem_queue = NULL;
	//lock the yield_cpu
	pthread_mutex_unlock(info->yield_cpu);
//destory the mutex at the end of a thread
	pthread_mutex_destroy(info->ava_cpu);
	pthread_mutex_destroy(info->yield_cpu);
	//free the space 
	free(info->ava_cpu) ;
	free(info->yield_cpu) ;
	info->ava_cpu = NULL;
	info->yield_cpu = NULL;

}

	/* Block until the thread can enter the scheduler queue. */
static void  enter_sched_queue (thread_info_t *info)
{
	//malloc space for this thread in the list
	info->elem_queue = (list_elem_t*)malloc(sizeof(list_elem_t));
	list_elem_init(info->elem_queue, info);

	//block if there is no space for inserting the queue
	sem_wait(info->queue->if_get_in_queue); 
    // Block if another thread is accessing the queue
    pthread_mutex_lock(info->queue->mutex);
    list_insert_tail(info->queue->list_of_queue, info->elem_queue);
    sem_post(info->queue->queue_left_space);
    pthread_mutex_unlock(info->queue->mutex);
}
	/* Remove the thread from the scheduler queue. */
static void leave_sched_queue   (thread_info_t *info)
{
	sem_wait(info->queue->queue_left_space);
	pthread_mutex_lock(info->queue->mutex);
	list_remove_elem (info->queue->list_of_queue, info->elem_queue);
	free(info->elem_queue);
	sem_post(info->queue->if_get_in_queue);
	pthread_mutex_unlock(info->queue->mutex);
	/* While on the scheduler queue, block until thread is scheduled. */
}
static void wait_for_cpu   (thread_info_t *info)
{
	pthread_mutex_lock(info->ava_cpu);
	/* Voluntarily relinquish the CPU when this thread's timeslice is
	 * over (cooperative multithreading). */
}
static void release_cpu  (thread_info_t *info)
{
	pthread_mutex_unlock(info->yield_cpu);
}
/*...More functions go here...*/

static void init_sched_queue(sched_queue_t *queue, int queue_size)
{
	/*...Code goes here...*/
	queue->list_of_queue = (list_t*)malloc(sizeof(list_t));
	list_init(queue->list_of_queue);
	queue->if_get_in_queue = (sem_t*)malloc(sizeof(sem_t));
    queue->queue_left_space = (sem_t*)malloc(sizeof(sem_t));
    queue->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    sem_init(queue->if_get_in_queue,0, queue_size);
    sem_init(queue->queue_left_space,0,0);
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    *(queue->mutex) = lock;

}

static void destroy_sched_queue(sched_queue_t *queue)
{
	/*...Code goes here...*/
	list_elem_t * elem = list_get_head(queue->list_of_queue);
	while(elem != NULL){
		list_remove_elem (queue->list_of_queue, elem);
		destroy_thread_info((thread_info_t*) elem->datum);
		elem = list_get_head(queue->list_of_queue);
	}
	sem_destroy(queue->if_get_in_queue);
	sem_destroy(queue->queue_left_space);
}
	/* Allow a worker thread to execute. */
static void  wake_up_worker(thread_info_t *info)
{
	pthread_mutex_unlock(info->ava_cpu);
	/* Block until the current worker thread relinquishes the CPU. */
}
static void  wait_for_worker(sched_queue_t *queue)
{

	/* Select the next worker thread to execute.
	 * Returns NULL if the scheduler queue is empty. */
}
static void fifo_wait_for_worker(sched_queue_t *queue)
{
	//delete the head and put next as the head
  	list_elem_t* head = list_get_head(queue->list_of_queue);
  	if(!head)	return;
  	thread_info_t* worker = (thread_info_t*) head->datum;
  	pthread_mutex_lock(worker->yield_cpu);
}
static void rr_wait_for_worker(sched_queue_t *queue)
{
	// put the head into the tail and delete the head
  	list_elem_t *header;
  	thread_info_t *worker;
  	pthread_mutex_lock(queue->mutex);
    header = list_get_head(queue->list_of_queue);
    if(header != NULL) {
      worker = (thread_info_t*) header->datum;
      list_remove_elem(queue->list_of_queue, header);
      list_insert_tail(queue->list_of_queue, header);
    } else {
      pthread_mutex_unlock(queue->mutex);
      return ;
    }
    pthread_mutex_unlock(queue->mutex);
  	
 	pthread_mutex_lock(worker->yield_cpu);

}
static thread_info_t * next_worker(sched_queue_t *queue)
{
	/* Block until at least one worker thread is in the scheduler queue. */
	thread_info_t* next = NULL;
  
  /* Block while another thread is accessing the queue */
  	pthread_mutex_lock(queue->mutex); 

    list_elem_t* head = list_get_head(queue->list_of_queue);
    if(head) {
      next = (thread_info_t*) head->datum;
    }
    pthread_mutex_unlock(queue->mutex);

  	return next;
}
static void wait_for_queue(sched_queue_t *queue)
{
	sleep(1);
	sem_wait(queue->queue_left_space);
    /* As soon as we lock the consumption semaphore, unlock it again */
    sem_post(queue->queue_left_space);    
}


/* You need to statically initialize these structures: */

sched_impl_t sched_fifo = {
  { init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue,
    wait_for_cpu, release_cpu }, 
  { init_sched_queue, destroy_sched_queue, wake_up_worker, fifo_wait_for_worker,
    next_worker, wait_for_queue } },
sched_rr = {
  { init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue,
    wait_for_cpu, release_cpu },
  { init_sched_queue, destroy_sched_queue, wake_up_worker, rr_wait_for_worker,
    next_worker, wait_for_queue } };