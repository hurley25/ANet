#ifndef ANET_SERVER_H
#define ANET_SERVER_H

#include "define.h"
#include "anet.h"
#include "ae.h"
#include "buffer.h"

typedef struct {
    aeEventLoop *loop;
    int listen_fd;
    int port;
    int backlog;
    int max_client_count;
    char err_info[ANET_ERR_LEN];
} server_t;

typedef struct {
    aeEventLoop *loop;
    int fd;
    buffer_t *read_buffer;
    buffer_t *write_buffer;
} client_t;

void init_server(server_t *server);
void wait_server(server_t *server);

#endif //ANET_SERVER_H
