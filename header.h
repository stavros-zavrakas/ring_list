#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "libraries/utils/utils.c"
#include "libraries/ring_list/ring_list.c"
#include "libraries/mac_manipulation_functions/mac_manipulation_functions.c"

#define SENDER_AWAKE_EVERY_SECS 1

pthread_t sniffing_thread, sender_thread;
ring_buffer_t *ring_buffer_metadata;

struct packet *sending_data(struct packet *packets, int items_to_send);
static void *sniffing_data_loop();
static void *sending_data_loop();