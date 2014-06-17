#include "mac_manipulation_functions.h"

void dec_to_hex(char num, char hex[], int from_limit) {
	int nimble;

	nimble = (num >> 4) & 0xF;
	hex[from_limit] = arrayHex[nimble];

	nimble = num & 0xF;
	hex[from_limit+1] = arrayHex[nimble];
}

int hex_to_dec(char *hex) {
	int i, j, num, len, counter = 0;
	int sum = 0;
	
	len=strlen(hex);
	for(i=len-1;i>=0;i--) {
		if(hex[i] >= '0' && hex[i] <= '9') {
			num=hex[i]-'0';
		} else {
			if(hex[i]>='A' && hex[i]<='F')
				num=hex[i]-55;
			else if(hex[i]>='a' && hex[i]<='f')
				num=hex[i]-87;
		}
		sum=sum+pow(16,counter)*num;
		counter++;
	}
	return sum;
}

void bytes_to_mac(char *device_mac_str, char mac_bytes[]) {
	int i, from_limit;

	for(i=0;i<6;i++){
		char tmp[3];
		from_limit = (i*2)+i;
		dec_to_hex(mac_bytes[i], device_mac_str, from_limit);
	}
}

void mac_to_bytes(const char *device_mac, char mac_bytes[]){
	char to_limit, i;
	int mac_byte;
	char *chunk;
	for(i=0;i<6;i++){
		to_limit = (i*2)+i;
		chunk = strndup(device_mac+to_limit, 2);
		mac_byte = hex_to_dec(chunk);
		mac_bytes[i] = mac_byte;
		free(chunk);
	}
}

uint64_t mac_to_uint64(const char *device_mac) {
	const char compressed_device_mac[] = { 
		device_mac[0], device_mac[1], 
		device_mac[3], device_mac[4], 
		device_mac[6], device_mac[7],
		device_mac[9], device_mac[10], 
		device_mac[12], device_mac[13], 
		device_mac[15], device_mac[16],
		'\0'
	};
	return strtoull((const char *)&compressed_device_mac, NULL, 16);
}