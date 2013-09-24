#ifndef __WEB_SOCK_W32_H__
#define __WEB_SOCK_W32_H__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x500
#endif

#include <WinSock2.h>

// Windows uses _DEBUG and NDEBUG
#ifdef _DEBUG
#undef DEBUG
#define DEBUG 1
#endif

#if !defined(POLLERR)
/* Event flag definitions for WSAPoll(). */
#define POLLRDNORM  0x0100
#define POLLRDBAND  0x0200
#define POLLIN      (POLLRDNORM | POLLRDBAND)
#define POLLPRI     0x0400

#define POLLWRNORM  0x0010
#define POLLOUT     (POLLWRNORM)
#define POLLWRBAND  0x0020

#define POLLERR     0x0001
#define POLLHUP     0x0002
#define POLLNVAL    0x0004

typedef struct pollfd {
    SOCKET  fd;
    SHORT   events;
    SHORT   revents;

} WSAPOLLFD, *PWSAPOLLFD, FAR *LPWSAPOLLFD;

#endif /* !defined(POLLERR) */

#pragma warning(disable : 4996)

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)

#define MSG_NOSIGNAL 0
#define SHUT_RDWR SD_BOTH

#define SOL_TCP IPPROTO_TCP

#define random rand
#define usleep _sleep

typedef INT (WSAAPI *PFNWSAPOLL)(LPWSAPOLLFD fdarray, ULONG nfds, INT timeout);
extern PFNWSAPOLL poll;

extern INT WSAAPI emulated_poll(LPWSAPOLLFD fdarray, ULONG nfds, INT timeout);

/* override configure because we are not using Makefiles */

#define LWS_NO_FORK
#define DATADIR "."

#endif
