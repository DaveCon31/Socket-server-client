#ifndef my_queue
#define my_queue

typedef struct node {
	void *val;
	struct node *next;
} node_t;    //node_t as node data type

typedef struct my_q q_t;    //opaque data type for link list

//initialize linked list with last_node & head = NULL
q_t *queue_create(void);

//FIFO
void enqueue(q_t *queue, void *client_socket);

//FIFO
void dequeue(q_t *queue);

//destroy queue
void destroy_queue(q_t **queue);

#endif
