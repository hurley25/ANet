#ifndef ANET_ANET_H
#define ANET_ANET_H

#include <sys/types.h>
#include <sys/socket.h>

#define ANET_OK         0
#define ANET_ERR        -1
#define ANET_ERR_LEN    256

/* Flags used with certain functions. */
#define ANET_NONE       0
#define ANET_IP_ONLY    (1<<0)

int anetTcpConnect(char *err, char *addr, int port);
int anetTcpNonBlockConnect(char *err, char *addr, int port);
int anetTcpNonBlockBindConnect(char *err, char *addr, int port, char *source_addr);
int anetTcpNonBlockBestEffortBindConnect(char *err, char *addr, int port, char *source_addr);

int anetUnixConnect(char *err, char *path);
int anetUnixNonBlockConnect(char *err, char *path);

int anetResolve(char *err, char *host, char *ipbuf, socklen_t ipbuf_len);
int anetResolveIP(char *err, char *host, char *ipbuf, socklen_t ipbuf_len);

int anetTcpServer(char *err, int port, char *bindaddr, int backlog);
int anetTcp6Server(char *err, int port, char *bindaddr, int backlog);

int anetUnixServer(char *err, char *path, mode_t perm, int backlog);

int anetTcpAccept(char *err, int serversock, char *ip, socklen_t ip_len, int *port);

int anetUnixAccept(char *err, int serversock);

int anetRead(int fd, char *buf, int count);
int anetWrite(int fd, char *buf, int count);

int anetNonBlock(char *err, int fd);
int anetBlock(char *err, int fd);

int anetEnableTcpNoDelay(char *err, int fd);
int anetDisableTcpNoDelay(char *err, int fd);

int anetTcpKeepAlive(char *err, int fd);
int anetKeepAlive(char *err, int fd, int interval);    /* interval for linux only */

int anetSetSendBuffer(char *err, int fd, int buffsize);
int anetSendTimeout(char *err, int fd, long long ms);

int anetFormatPeer(int fd, char *buf, size_t buf_len);
int anetFormatSock(int fd, char *buf, size_t buf_len);


#endif //ANET_ANET_H
