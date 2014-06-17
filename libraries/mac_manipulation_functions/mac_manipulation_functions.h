#ifndef MACMANIPULATION_H
#define MACMANIPULATION_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <inttypes.h>

static char arrayHex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void dec_to_hex(char num, char hex[], int from_limit);
int hex_to_dec(char *hex);
void bytes_to_mac(char *device_mac_str, char mac_bytes[]);
void mac_to_bytes(const char *device_mac, char mac_bytes[]);
uint64_t mac_to_uint64(const char *device_mac);

#endif /* MACMANIPULATION_H */