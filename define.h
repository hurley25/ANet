#ifndef ANET_DEFINE_H
#define ANET_DEFINE_H

#include <stdint.h>

#define zmalloc malloc
#define zfree(p) if (p) { free(p); }
#define zrealloc realloc

#define DEFAULT_BUFF_SIZE        1024

/*
 * 三次握手完成队列的最大长度
 *
 *   max(listen->backlog, net.core.somaxconn)
 *
 * 半连接队列最大长度
 *
 *   if kernel_version < 2.6.20
 *       net.ipv4.tcp_max_syn_backlog
 *   else
 *      roundup_pow_of_two(max(min(net.core.somaxconn, net.ipv4.tcp_max_syn_backlog),8)+1)
 */
#define DEFAULT_LISTEN_BACKLOG   10240
#define DEFAULT_MAX_CLIENT_COUNT 100000
#define DEFAULT_LISTEN_PORT      12345

#endif //ANET_DEFINE_H
