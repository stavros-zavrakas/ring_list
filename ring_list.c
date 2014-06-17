#include "ring_list.h"
#include <pthread.h>

ring_buffer_t *ring_manager_init() {
	ring_buffer_t *ring_buffer_metadata = (ring_buffer_t*)malloc(sizeof(ring_buffer_t));
	ring_buffer_metadata->writer = NULL;
	ring_buffer_metadata->reader = NULL;
	ring_buffer_metadata->first_inserted_node = NULL;
	ring_buffer_metadata->size = 0;
	ring_buffer_metadata->allocated_size = 0;
	ring_buffer_metadata->last_server_call_millisecs = 0;
	ring_buffer_metadata->sem = (sem_t *)malloc(sizeof(sem_t));
	sem_init(ring_buffer_metadata->sem, 0, 1); 
	return ring_buffer_metadata;
}

packet_t *ring_manager_create_node(struct packet node_p) {
	int *mac_tmp; 
	packet_t *node = (struct packet*)malloc(sizeof(struct packet));
	if(node) {
		memcpy(node->mac, node_p.mac, (size_t)6);
		node->channel = node_p.channel;
		node->dbm = node_p.dbm;
		node->event_timestamp = node_p.event_timestamp;
	}
	return node;
}

void ring_manager_insert_to_ring(struct ring_buffer *ring_buffer_metadata, struct packet node_p) {
	
	sem_wait(ring_buffer_metadata->sem);

	if(ring_buffer_metadata->allocated_size == 0) {
		packet_t *node = ring_manager_create_node(node_p);
		ring_buffer_metadata->writer = node;
		ring_buffer_metadata->first_inserted_node = node;
		ring_buffer_metadata->writer->next = ring_buffer_metadata->first_inserted_node;
		ring_buffer_metadata->size = 1;
		ring_buffer_metadata->allocated_size = 1;
	} else {
		if(ring_buffer_metadata->allocated_size < MAX_RING_SIZE) {
			packet_t *node = ring_manager_create_node(node_p);
			ring_buffer_metadata->writer->next = node;
			ring_buffer_metadata->writer = node;
			ring_buffer_metadata->writer->next = ring_buffer_metadata->first_inserted_node;
			ring_buffer_metadata->allocated_size++;
			ring_buffer_metadata->size++;
		} else {
			if(ring_buffer_metadata->writer->next != ring_buffer_metadata->reader 
				&& (ring_buffer_metadata->reader != NULL && ring_buffer_metadata->writer->next != ring_buffer_metadata->first_inserted_node)){

				ring_buffer_metadata->writer = ring_buffer_metadata->writer->next;

				memcpy(ring_buffer_metadata->writer->mac, node_p.mac, (size_t)6);
				ring_buffer_metadata->writer->channel = node_p.channel;
				ring_buffer_metadata->writer->dbm = node_p.dbm;
				ring_buffer_metadata->writer->event_timestamp = node_p.event_timestamp;
				ring_buffer_metadata->size++;

			} else 
				printf("Overflowing\n");
		}
	}

	sem_post(ring_buffer_metadata->sem);
}

void ring_manager_send_data(struct ring_buffer *ring_buffer_metadata) {
	
	if (
		ring_buffer_metadata->size >= RING_TRIGGER_EVENT ||
	    (ring_buffer_metadata->size > 0 && (getTimeMillisecs() - ring_buffer_metadata->last_server_call_millisecs > MAX_TIME_BETWEEN_POSTS_MILLISECS) )
	   ) {
		
		int ring_size_to_send = _MIN(ring_buffer_metadata->size, RING_TRIGGER_EVENT);

		sem_wait(ring_buffer_metadata->sem);

		packet_t *node_to_revert = ring_buffer_metadata->reader;

		if(ring_buffer_metadata->reader== NULL)
			ring_buffer_metadata->reader = ring_buffer_metadata->first_inserted_node;
		else
			ring_buffer_metadata->reader = ring_buffer_metadata->reader->next;
		
		sem_post(ring_buffer_metadata->sem);

		packet_t *packets = ring_buffer_metadata->send_data_handler(ring_buffer_metadata->reader, ring_size_to_send);
		
		sem_wait(ring_buffer_metadata->sem);

		if(packets == NULL)
			ring_buffer_metadata->reader = node_to_revert;
		else {
			ring_buffer_metadata->reader = packets;
			ring_buffer_metadata->size = ring_buffer_metadata->size - ring_size_to_send;
		}

		ring_buffer_metadata->last_server_call_millisecs = getTimeMillisecs();

		sem_post(ring_buffer_metadata->sem);

	}
	
}

void ring_manager_print_full_ring(struct ring_buffer *ring_buffer_metadata) {
	int i;
	packet_t *cursor = ring_buffer_metadata->first_inserted_node;
	for(i=0;i<ring_buffer_metadata->allocated_size;i++){
		printf("%d\n", cursor->dbm);
		cursor = cursor->next;
	}
}

void ring_manager_print_remained_ring(struct ring_buffer *ring_buffer_metadata) {
	packet_t *cursor = (ring_buffer_metadata->reader) ? ring_buffer_metadata->reader : ring_buffer_metadata->first_inserted_node;
	do {
		printf("%d\n", cursor->dbm);
		cursor = cursor->next;
	} while (cursor != ring_buffer_metadata->writer->next);
}

void ring_manager_clean(struct ring_buffer *ring_buffer_metadata) {
	int i;
	packet_t *to_free, *cursor = ring_buffer_metadata->first_inserted_node;

	for(i=0;i<ring_buffer_metadata->allocated_size;i++){
		to_free = cursor;
		cursor = cursor->next;
		free(to_free);
	}

	free(ring_buffer_metadata->sem);
	free(ring_buffer_metadata);
}
