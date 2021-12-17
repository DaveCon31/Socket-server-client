#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "my_queue.h"

#ifdef NDEBUG
#define DEBUG_PRINTF(...)
#else
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#endif

#define Q_LOCK(qk) pthread_mutex_lock(&qk->lock)
#define Q_UNLOCK(qk) pthread_mutex_unlock(&qk->lock)

typedef struct my_q {
	node_t *head;
	node_t *tail;
	pthread_mutex_t lock;
} q_t;

pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

int validate_queue(q_t *queue)    //VALIDATE HEAD and queue
{
	if (queue == NULL)
		return -1;
	if (queue->head == NULL)
		return -1;
	return 0;
}

q_t *queue_create(void)
{
	q_t *queue = malloc(sizeof(q_t));
	if (queue == NULL) {
		DEBUG_PRINTF("Memory allocation for list failed!\n");
	}
	
	queue->head = NULL;
	queue->tail = NULL;
	if (pthread_mutex_init(&queue->lock, NULL) != 0)
		return NULL;
	return queue;
}

void enqueue(q_t *queue, int *client_socket)
{			
	Q_LOCK(queue);
	node_t *new_node = malloc(sizeof(node_t));	
	if (new_node == NULL) {
		DEBUG_PRINTF("Memory allocation for new_node failed! (add) \n");
		pthread_cond_signal(&cond_var);
		Q_UNLOCK(queue);
		return;
	}
	new_node->val = client_socket;
	new_node->next = queue->head;
	queue->head = new_node;
	
	if (queue->head == NULL)    //tail will be the first added node in queue
		queue->tail = new_node;
	pthread_cond_signal(&cond_var);
	Q_UNLOCK(queue);
}

int* dequeue(q_t *queue)
{
	Q_LOCK(queue);
	if (validate_queue(queue) == -1) {
		pthread_cond_wait(&cond_var, &(queue->lock));
		Q_UNLOCK(queue);
		return NULL;
	}
	
	int *result = NULL;
	node_t *temp = queue->head;
	node_t *prev = NULL;
	while (temp->next != NULL) {
		prev = temp;
		temp = temp->next;
	}
	if (temp == queue->head)
		queue->head = NULL;
	else
		prev->next = NULL;
	
	result = temp->val;
	free(temp);
	temp = NULL;
	queue->tail = prev;
	Q_UNLOCK(queue);
	return result;
}

void flush_queue(q_t *queue)
{
	Q_LOCK(queue);
	if (validate_queue(queue) == -1) {
		Q_UNLOCK(queue);
		return;
	}
	
	node_t *temp = NULL;	
	while (queue->head != NULL) {
		temp = queue->head;
		queue->head = queue->head->next;
		free(temp);
	}
	queue->tail = NULL;
	Q_UNLOCK(queue);
}

void print_queue(q_t *queue)
{
	Q_LOCK(queue);
	if (validate_queue(queue) == -1) {
		Q_UNLOCK(queue);
		return;
	}
	
	node_t *temp = queue->head;
	while (temp != NULL) {
		printf("%d", *(int*)temp->val);
		temp = temp->next;
		if (temp != NULL)
			DEBUG_PRINTF(" ---> ");
	}
	DEBUG_PRINTF("\n");
}

void destroy_queue(q_t **queue)
{	
	flush_queue(*queue);
	if (pthread_mutex_destroy(&((*queue)->lock)) != 0)
		return;
	free(*queue);    //free list resources
	(*queue) = NULL;
}
