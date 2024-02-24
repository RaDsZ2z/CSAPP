#ifndef __CSAPP_H__
#define __CSAPP_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* Misc constants */
#define MAXLINE 8192 /* Max text line length */
#define MAXBUF 8192  /* Max I/O buffer size */
#define LISTENQ 1024 /* Second argument to listen() */

typedef struct sockaddr SA;

/* $begin rio_t */
#define RIO_BUFSIZE 8192
typedef struct
{
    int rio_fd;                /* Descriptor for this internal buf */
    int rio_cnt;               /* Unread bytes in internal buf */
    char *rio_bufptr;          /* Next unread byte in internal buf */
    char rio_buf[RIO_BUFSIZE]; /* Internal buffer */
} rio_t;
/* $end rio_t */

/* Reentrant protocol-independent client/server helpers */
// int open_clientfd(char *hostname, char *port);
int open_listenfd(char *port);

/* Wrappers for reentrant protocol-independent client/server helpers */
// int Open_clientfd(char *hostname, char *port);
int Open_listenfd(char *port);

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);

/* Our own error-handling functions */
void unix_error(const char *msg);
void posix_error(int code, const char *msg);
void dns_error(const char *msg);
void gai_error(int code, const char *msg);
void app_error(const char *msg);

void Getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host,
                 size_t hostlen, char *serv, size_t servlen, int flags);
void Close(int fd);

void rio_readinitb(rio_t *rp, int fd);
void Rio_readinitb(rio_t *rp, int fd);

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);

ssize_t rio_writen(int fd, void *usrbuf, size_t n);
void Rio_writen(int fd, void *usrbuf, size_t n);

pid_t Fork(void);

int Dup2(int fd1, int fd2);

void Execve(const char *filename, char *const argv[], char *const envp[]);

pid_t Wait(int *status);

int Open(const char *pathname, int flags, mode_t mode);

/* Memory mapping wrappers */
void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void *start, size_t length);
#endif
