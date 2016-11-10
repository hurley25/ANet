#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "define.h"
#include "ae.h"

typedef struct aeApiState {
    int epfd;
    struct epoll_event *events;
} aeApiState;

static int aeApiCreate(aeEventLoop *eventLoop) {
    aeApiState *state = zmalloc(sizeof(aeApiState));

    if (!state) {
        goto err;
    }
    state->events = zmalloc(sizeof(struct epoll_event) * eventLoop->setsize);
    if (!state->events) {
       goto err;
    }
    state->epfd = epoll_create(1024);  /* 1024 is just a hint for the kernel */
    if (state->epfd == -1) {
        goto err;
    }
    eventLoop->apidata = state;

    return 0;

err:
    if (state) {
        zfree(state->events);
        zfree(state);
    }

    return -1;
}

static int aeApiResize(aeEventLoop *eventLoop, int setsize) {
    aeApiState *state = eventLoop->apidata;

    state->events = zrealloc(state->events, sizeof(struct epoll_event) * setsize);

    return 0;
}

static void aeApiFree(aeEventLoop *eventLoop) {
    aeApiState *state = eventLoop->apidata;

    close(state->epfd);
    zfree(state->events);
    zfree(state);
}

static int aeApiAddEvent(aeEventLoop *eventLoop, int fd, int mask) {
    aeApiState *state = eventLoop->apidata;
    struct epoll_event ee = {0};    /* avoid valgrind warning */

    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    int op = eventLoop->events[fd].mask == AE_NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    ee.events = 0;
    mask |= eventLoop->events[fd].mask; /* Merge old events */
    if (mask & AE_READABLE) {
        ee.events |= EPOLLIN;
    }
    if (mask & AE_WRITABLE) {
        ee.events |= EPOLLOUT;
    }

    ee.data.fd = fd;
    if (epoll_ctl(state->epfd, op, fd, &ee) == -1) {
        return -1;
    }

    return 0;
}

static void aeApiDelEvent(aeEventLoop *eventLoop, int fd, int delmask) {
    aeApiState *state = eventLoop->apidata;
    struct epoll_event ee = {0}; /* avoid valgrind warning */
    int mask = eventLoop->events[fd].mask & (~delmask);

    ee.events = 0;
    if (mask & AE_READABLE) {
        ee.events |= EPOLLIN;
    }

    if (mask & AE_WRITABLE) {
        ee.events |= EPOLLOUT;
    }

    ee.data.fd = fd;
    if (mask != AE_NONE) {
        epoll_ctl(state->epfd, EPOLL_CTL_MOD, fd, &ee);
    } else {
        /* Note, Kernel < 2.6.9 requires a non null event pointer even for EPOLL_CTL_DEL. */
        epoll_ctl(state->epfd, EPOLL_CTL_DEL, fd, &ee);
    }
}

static int aeApiPoll(aeEventLoop *eventLoop, struct timeval *tvp) {
    aeApiState *state = eventLoop->apidata;
    int retval, numevents = 0;

    retval = epoll_wait(state->epfd, state->events, eventLoop->setsize,
                        tvp ? (tvp->tv_sec * 1000 + tvp->tv_usec / 1000) : -1);
    if (retval > 0) {
        numevents = retval;
        for (int i = 0; i < numevents; i++) {
            int mask = 0;
            struct epoll_event *e = state->events + i;

            if (e->events & EPOLLIN) {
                mask |= AE_READABLE;
            }
            if (e->events & EPOLLOUT) {
                mask |= AE_WRITABLE;
            }
            if (e->events & EPOLLERR) {
                mask |= AE_WRITABLE;
            }
            if (e->events & EPOLLHUP) {
                mask |= AE_WRITABLE;
            }
            eventLoop->fired[i].fd = e->data.fd;
            eventLoop->fired[i].mask = mask;
        }
    }

    return numevents;
}

static char *aeApiName(void) {
    return "epoll";
}
