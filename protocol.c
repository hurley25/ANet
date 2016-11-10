#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "define.h"

package_t *alloc_packet(uint32_t data_len)
{
    package_t *package = zmalloc(sizeof(package_head_t) + data_len);

    if (package) {
        package->head.magic = MAGIC_NUMBER;
        package->head.length = data_len;
    }

    return package;
}

void free_package(package_t *package)
{
    zfree(package);
}

int packet_decode(buffer_t *buffer, package_t **package)
{
    size_t data_len = get_readable_size(buffer);
    if (data_len < sizeof(package_head_t)) {
        return -1;
    }
    package_head_t *package_head = (package_head_t *)(buffer->buff + buffer->read_idx);
    if (package_head->magic != MAGIC_NUMBER) {
        return -2;
    }

    size_t package_len = sizeof(package_head_t) + package_head->length;
    if (data_len < package_len) {
        return -1;
    }

    package_t *new_package = zmalloc(package_len);
    memcpy(new_package, buffer->buff + buffer->read_idx, package_len);
    buffer->read_idx += package_len;

    *package = new_package;

    return 0;
}

int package_encode(buffer_t *buffer, package_t *package)
{
    size_t avlid_size = get_writeable_size(buffer);
    size_t package_len = sizeof(package_head_t) + package->head.length;

    if (avlid_size < package_len) {
        return -1;
    }

    memcpy(buffer->buff + buffer->write_idx, package, package_len);
    buffer->write_idx += package_len;

    return 0;
}

void do_package(package_t *req_package, package_t **resp_package)
{
    static int n = 0;
    const char * const name = "hurley";
    switch (req_package->head.type) {
        case QUERY_NAME:
            *resp_package = alloc_packet((uint32_t)strlen(name) + 1);
            if (*resp_package) {
                memcpy((*resp_package)->data, name, strlen(name) + 1);
            } else {
                printf("alloc package error, OOM ?");
            }
            break;
        case CHAT_MESSAGE:
            // client send data with '\0'
            printf("chat message: No.%d %s\n", n++, req_package->data);
            break;
        default:
            printf("Unknown package, pcode: %d\n", req_package->head.type);
            break;
    }
}
