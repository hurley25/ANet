#include "anet.h"
#include "define.h"
#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    char err[ANET_ERR_LEN];
    int conn_fd = anetTcpConnect(err, "127.0.0.1", DEFAULT_LISTEN_PORT);
    if (conn_fd == ANET_ERR) {
        printf("connect error: %s\n", err);
        return -1;
    }

    package_t *query_package = alloc_packet(0);
    query_package->head.type = QUERY_NAME;
    if (anetWrite(conn_fd, (char *)query_package, sizeof(package_head_t)) < 0) {
        printf("send error, exit.");
    }

    char buff[1024];
    if (anetRead(conn_fd, buff, sizeof(package_head_t)) < 0) {
        printf("read error, exit.");
        return -1;
    }
    package_t *resp_package = (package_t *)buff;
    if (resp_package->head.magic != MAGIC_NUMBER) {
        printf("read error package, exit");
        return -1;
    }
    if (anetRead(conn_fd, buff + sizeof(package_head_t), resp_package->head.length) < 0) {
        printf("read error, exit.");
        return -1;
    }

    const char * const send_info = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char send_buff[1024];
    snprintf(send_buff, sizeof(send_buff), "Hey, %s. I say: %s.", resp_package->data, send_info);

    package_t *package = alloc_packet((uint32_t)strlen(send_buff) + 1);
    package->head.type = CHAT_MESSAGE;
    // copy with '\0'
    memcpy(package->data, send_buff, strlen(send_buff) + 1);

    int package_len = sizeof(package_head_t) + package->head.length;

    printf("send package size: %d\n", package_len);
    for (int i = 0; i < 20; ++i) {
        if (anetWrite(conn_fd, (char *)package, package_len) < 0) {
            printf("send error, exit.");
        }
    }

    printf("slow send ...\n");
    for (int i = 0; i < 20; ++i) {
        if (anetWrite(conn_fd, (char *)package, package_len) < 0) {
            printf("send error, exit.");
        }
        usleep(10000);
    }

    printf("more slow send ...\n");
    for (int n = 0; n < 20; ++n) {
        for (int i = 0; i < package_len; ++i) {
            if (anetWrite(conn_fd, (char *) package + i, 1) < 0) {
                printf("send error, exit.");
            }
            usleep(10000);
        }
    }

    zfree(package);
    close(conn_fd);

    return 0;
}
