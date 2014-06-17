#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "ring_manager.c"
#include "libraries/mac_manipulation_functions/mac_manipulation_functions.c"

char *device_mac_1 = "64:66:00:90:45:5e\0";

struct packet *send_data_failed(struct packet *packets, int items_to_send) {
	int i;
	char *body_dump;

	return NULL;
}

struct packet *send_data_success(struct packet *packets, int items_to_send) {
	int i;
	char *body_dump;
	struct packet *packets_copy = packets;

	syslog(LOG_DEBUG, "\nSending chunk of data:\n");
	syslog(LOG_DEBUG, "~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
	for(i=0;i<items_to_send;i++){
		char device_mac_str[] = { '\0', '\0', ':', '\0', '\0', ':', '\0', '\0', ':', '\0', '\0', ':', '\0', '\0', ':', '\0', '\0', '\0' };
		bytes_to_mac(device_mac_str, packets->mac);
		packets_copy = packets;
		packets = packets->next;
	}

	return packets_copy;
}

int fail_send_data_overflow() {
	int i, counter;
	ring_buffer_t *ring_buffer_metadata = ring_manager_init();
	ring_buffer_metadata->send_data_handler = &send_data_failed;
	packet_t node, *cursor;
	for(i=0;i<RING_TRIGGER_EVENT*5;i++){
		mac_to_bytes(device_mac_1, node.mac);
		node.channel = i;
		node.dbm = i;
		node.event_timestamp = 1234.5;

		ring_manager_insert_to_ring(ring_buffer_metadata, node);
	}

	counter = 0;
	cursor = ring_buffer_metadata->first_inserted_node;
	do {
		cursor = cursor->next;
		counter++;
	} while(cursor != ring_buffer_metadata->first_inserted_node);

	if(counter != ring_buffer_metadata->allocated_size)
		return 1;

	counter = 1;
	cursor = (ring_buffer_metadata->reader) ? ring_buffer_metadata->reader : ring_buffer_metadata->first_inserted_node;
	while(cursor != ring_buffer_metadata->writer){
		counter++;
		cursor = cursor->next;
	}

	if(counter != ring_buffer_metadata->size)
		return 2;

	if(ring_buffer_metadata->first_inserted_node == ring_buffer_metadata->reader)
		return 3;

	// ring_manager_print_remained_ring(ring_buffer_metadata);
	ring_manager_clean(ring_buffer_metadata);

	return 0;
}

int success_send_at_least_max_ring_size() {
	int i, counter;
	ring_buffer_t *ring_buffer_metadata = ring_manager_init();
	ring_buffer_metadata->send_data_handler = &send_data_success;
	packet_t node, *cursor;
	for(i=0;i<MAX_RING_SIZE+3;i++){
		mac_to_bytes(device_mac_1, node.mac);
		node.channel = i;
		node.dbm = i;
		node.event_timestamp = 1234.5;

		ring_manager_insert_to_ring(ring_buffer_metadata, node);
	}

	if(ring_buffer_metadata->allocated_size != MAX_RING_SIZE)
		return 4;

	if(MAX_RING_SIZE != ring_buffer_metadata->allocated_size)
		return 5;

	// ring_manager_print_remained_ring(ring_buffer_metadata);
	ring_manager_clean(ring_buffer_metadata);

	return 0;
}

int main(int argc, char *argv[]) {
	int err_code;

	err_code = fail_send_data_overflow();
	if ( err_code != 0) {
		printf("test failed: %d\n", err_code );
	}

	err_code = success_send_at_least_max_ring_size();
	if ( err_code != 0) {
		printf("test failed: %d\n", err_code );
	}

	if ( err_code == 0 )
		printf("Tests are ok!\n");

	return err_code;
}