#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "ring_list.c"
#include "libraries/mac_manipulation_functions/mac_manipulation_functions.c"

#define SENDER_AWAKE_EVERY_SECS 1

pthread_t sniffing_thread, sender_thread;
ring_buffer_t *ring_buffer_metadata;

struct packet *sending_data(struct packet *packets, int items_to_send) {
	int i;
	char *body_dump;
	struct packet *packets_copy = packets;

	for(i=0;i<items_to_send;i++){
		char device_mac_str[] = { '\0', '\0', ':', '\0', '\0', ':', '\0', '\0', ':', '\0', '\0', ':', '\0', '\0', ':', '\0', '\0', '\0' };
		bytes_to_mac(device_mac_str, packets->mac);
		packets_copy = packets;
		packets = packets->next;
	}

	return packets_copy;
}

static void *sniffing_data_loop() {
	int i, counter;
	packet_t node, *cursor;
    char device_mac_add[18];

	while (1) {
        int rnd = (int)rand()%5;
        sleep(rnd);
        sprintf(device_mac_add, "64:b9:e8:99:c0:%02u", rnd);

		mac_to_bytes(device_mac_add, node.mac);
		node.channel = rnd;
		node.dbm = rnd - 50;
		node.event_timestamp = getTimeMillisecs();
		printf("%lf - Inserting: mac: %s [dbm: %d] - [channel: %d]\n", node.event_timestamp, device_mac_add, node.dbm, node.channel);
		ring_manager_insert_to_ring(ring_buffer_metadata, node);
	}
}

static void *sending_data_loop() {
	while (1) {
		ring_manager_send_data(ring_buffer_metadata);
		sleep(SENDER_AWAKE_EVERY_SECS);
	}	
}

int sniffer_init() {
}

int main(int argc, char *argv[]) {
	ring_buffer_metadata = ring_manager_init();
	ring_buffer_metadata->send_data_handler = &sending_data;

	// Thread to get data.
	if(pthread_create(&sniffing_thread, NULL, sniffing_data_loop, NULL)) {
		syslog(LOG_ERR, "Error creating sender thread");
		return -1;
	}

	// Thread to send data.
	if(pthread_create(&sender_thread, NULL, sending_data_loop, NULL)) {
		syslog(LOG_ERR, "Error creating sender thread");
		return -1;
	}
	pthread_join(sender_thread, NULL);
	pthread_join(sniffing_thread, NULL);

	return 0;
}