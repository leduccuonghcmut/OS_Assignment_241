#include "queue.h"
#include "sched.h"
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
static struct queue_t ready_queue;
static struct queue_t run_queue;
static pthread_mutex_t queue_lock;

#ifdef MLQ_SCHED
static struct queue_t mlq_ready_queue[MAX_PRIO];
#endif

int queue_empty(void) {
#ifdef MLQ_SCHED
	unsigned long prio;
	for (prio = 0; prio < MAX_PRIO; prio++)
		if(!empty(&mlq_ready_queue[prio])) 
			return -1;
#endif
	return (empty(&ready_queue) && empty(&run_queue));
}

void init_scheduler(void) {
#ifdef MLQ_SCHED
	int i ;

	for (i = 0; i < MAX_PRIO; i ++){
		mlq_ready_queue[i].size = 0;
	}
#endif
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}

#ifdef MLQ_SCHED
/* 
 *  Stateful design for routine calling
 *  based on the priority and our MLQ policy
 *  We implement stateful here using transition technique
 *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
 */
 static int slot[MAX_PRIO] = {0};
//  void set_slot_for_queue()
// {
//     pthread_mutex_lock(&queue_lock);
// 	for (int i = 0; i < MAX_PRIO; i++)
// 		slot[i] = MAX_PRIO - i;
//     pthread_mutex_unlock(&queue_lock);
// }
// int check_new_process(int i)
// {
// 	for (int j = 0; j < i; j++)
// 	{
// 		if (empty(&mlq_ready_queue[j]) || slot[j] <= 0)
// 			continue;
// 		return j - 1;
// 	}
// 	return -2;
// }
void set_slot_for_queue()
{
#ifdef SYNC
    pthread_mutex_lock(&queue_lock);
#endif
	for (int i = 0; i < MAX_PRIO; i++)
		slot[i] = MAX_PRIO - i;
#ifdef SYNC
    pthread_mutex_unlock(&queue_lock);
#endif
}
int check_new_process(int i)
{
	for (int j = 0; j < i; j++)
	{
		if (empty(&mlq_ready_queue[j]) || slot[j] <= 0)
			continue;
		return j - 1;
	}
	return -2;
}
struct pcb_t * get_mlq_proc(void) {
	// struct pcb_t * proc = NULL;
	// /*TODO: get a process from PRIORITY [ready_queue].
	//  * Remember to use lock to protect the queue.
	//  */
	
	
	// static int i= 0;
	// int check = 0;
	// for (;; i = (i + 1)%MAX_PRIO) {
	// 	if (slot[i] == 0) {
	// 		slot[i] = MAX_PRIO - i;
	// 		check = 0;
	// 	continue;
	// 	}
	// 	if (empty(&mlq_ready_queue[i])) {
	// 		check++;
	// 		if (check == MAX_PRIO) break;
	// 		continue;
	// 	}
	// 	check = 0;
	// 	pthread_mutex_lock(&queue_lock);
	// 	slot[i]--;
	// 	proc = dequeue(&mlq_ready_queue[i]);
	// 	pthread_mutex_unlock(&queue_lock);
	// 	break;
	// }

	// return proc;	
		struct pcb_t *proc = NULL;
	/*TODO: get a process from PRIORITY [ready_queue].
	 * Remember to use lock to protect the queue.
	 */
	int check = 0;
	int flag = 0;
	for (int i = 0; i < MAX_PRIO; ++i)
	{
		check = check_new_process(i);
		if (check != -2)
		{
			i = check;
			continue;
		}

		if (empty(&mlq_ready_queue[i]))
		{
			if (flag == 1 && i == MAX_PRIO - 1)
			{
				flag = 0;
				set_slot_for_queue();
				i = -1;
			}
			continue;
		}
		if (slot[i] <= 0)
		{
			flag = 1;
			if (i == MAX_PRIO - 1)
			{
				flag = 0;
				set_slot_for_queue();
				i = -1;
			}
			continue;
		}
 #ifdef SYNC
        pthread_mutex_lock(&queue_lock);
 #endif
		proc = dequeue(&mlq_ready_queue[i]);
		slot[i]--;
 #ifdef SYNC
		pthread_mutex_unlock(&queue_lock);
 #endif
		break;
	}

	return proc;
}

void put_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);	
}

struct pcb_t * get_proc(void) {
	return get_mlq_proc();
}

void put_proc(struct pcb_t * proc) {
	return put_mlq_proc(proc);
}

void add_proc(struct pcb_t * proc) {
	return add_mlq_proc(proc);
}
#else
struct pcb_t * get_proc(void) {
	struct pcb_t * proc = NULL;
	/*TODO: get a process from [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */
	pthread_mutex_lock(&queue_lock);
	dequeue(&run_queue[0]);
	pthread_mutex_unlock(&queue_lock);
	return proc;
}

void put_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&run_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&ready_queue, proc);
	pthread_mutex_unlock(&queue_lock);	
}
#endif

