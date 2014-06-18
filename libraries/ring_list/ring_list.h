#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <semaphore.h>

#define RING_TRIGGER_EVENT 					10
#define MAX_RING_SIZE 						500
#define MAX_TIME_BETWEEN_POSTS_MILLISECS 	180000

struct packet {
	char mac[6];
	char channel; 
	signed char dbm;
	double event_timestamp;
	struct packet *next;
};

typedef struct packet packet_t;

struct ring_buffer {
	sem_t *sem;
	int allocated_size;
	int size;
	struct packet *writer;
	struct packet *reader;
	struct packet *first_inserted_node;
	struct packet *(*send_data_handler)(struct packet *packets_p, int ring_size);
	uint64_t last_server_call_millisecs;
};

typedef struct ring_buffer ring_buffer_t;

ring_buffer_t *ring_manager_init();
packet_t *ring_manager_create_node(struct packet node_p);
void ring_manager_insert_to_ring(struct ring_buffer *ring_buffer_metadata, struct packet node_p);
void ring_manager_send_data(struct ring_buffer *ring_buffer_metadata);
void ring_manager_print_full_ring(struct ring_buffer *ring_buffer_metadata);
void ring_manager_print_remained_ring(struct ring_buffer *ring_buffer_metadata);
void ring_manager_clean(struct ring_buffer *ring_buffer_metadata);