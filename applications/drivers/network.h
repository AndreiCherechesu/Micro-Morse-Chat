#pragma once

#define DRIVER_NUM_NETWORK 0xa0001

#define COMMAND_GET 0
#define COMMAND_POST 1

char * network_get(const char *url, size_t len);
void network_post(const char *url, const char *payload);