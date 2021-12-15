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

int validate_head(q_t *queue)    //VALIDATE HEAD and queue
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

void enqueue(q_t *queue, void *client_socket);
{			
	Q_LOCK(queue);
	node_t *new_node = malloc(sizeof(node_t));	
	if (new_node == NULL) {
		DEBUG_PRINTF("Memory allocation for new_node failed! (add) \n");
		Q_UNLOCK(queue);
		return;
	}
	new_node->next = NULL;
	new_node->val = client_socket;
	
	if (queue->head == NULL) {			
		queue->head = new_node;    //set first node as head if linked list is empty
	}
	if (queue->tail != NULL) {   
		queue->tail->next = new_node;    //link new_node with last node if linked list is not empty
	}
	
	queue->tail = new_node;
	Q_UNLOCK(queue);
}

void dequeue(q_t *queue)
{
	if (validate_head(queue) == -1)
		return;
	
	Q_LOCK(queue);
	node_t *temp = queue->head;
	node_t *prev = NULL;
	while (temp != NULL) {
		if (queue->comparator(temp->val, client_socket) == 0) {
			if (temp == queue->head)
				queue->head = queue->head->next;
			if (temp == queue->tail)
				queue->tail = prev;
			if (prev != NULL)
				prev->next = temp->next;
			break;
		}
		prev = temp;
		temp = temp->next;
	}
	free(temp);
	temp = NULL;
	Q_UNLOCK(queue);
}

void flush_queue(q_t *queue)
{
	if (validate_head(queue) == -1)   //validate head list
		return;
	
	LL_LOCK(queue);
	node_t *temp = NULL;	
	while (queue->head != NULL) {
		temp = queue->head;
		queue->head = queue->head->next;
		free(temp);
	}
	queue->tail = NULL;
	LL_UNLOCK(queue);
}

void destroy_queue(q_t **queue)
{	
	flush_queue(*queue);
	if (pthread_mutex_destroy(&((*queue)->lock)) != 0)
		return;
	free(*queue);    //free list resources
	(*queue) = NULL;
}
