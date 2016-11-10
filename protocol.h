#ifndef ANET_PROTOCOL_H
#define ANET_PROTOCOL_H

#include <stdint.h>

#include "buffer.h"

// package magic number
#define MAGIC_NUMBER    0xBABABABA

// package type
#define QUERY_NAME      1
#define CHAT_MESSAGE    2

typedef struct {
    uint32_t magic;
    uint32_t type;
    uint32_t length;
} __attribute__((packed)) package_head_t;

typedef struct {
    package_head_t head;
    unsigned char data[0];
} __attribute__((packed)) package_t;

package_t *alloc_packet(uint32_t data_len);
void free_package(package_t *package);

int packet_decode(buffer_t *buffer, package_t **package);
int package_encode(buffer_t *buffer, package_t *package);

void do_package(package_t *req_package, package_t **resp_package);

#endif //ANET_PROTOCOL_H
