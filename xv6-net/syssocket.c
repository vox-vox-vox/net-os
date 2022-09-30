// Copyright (c) 2012-2020 YAMAMOTO Masaya
// SPDX-License-Identifier: MIT

#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"

/**
 * @brief 创建一个socket文件，返回文件描述符
 * 
 * @return int 
 */
int
sys_socket(void)
{
  int fd, domain, type, protocol;
  struct file *f;

  if (argint(0, &domain) < 0 || argint(1, &type) < 0 || argint(2, &protocol) < 0)
    return -1;
  if ((f = socketalloc(domain, type, protocol)) == 0 || (fd = fdalloc(f)) < 0){
    if (f)
      fileclose(f);
    return -1;
  }
  return fd;
}
/**
 * @brief 和addr进行网络连接，传入的网络文件描述符所对应的socket结构体自动和一个匿名port进行连接
 * 
 * @return int 
 */
int
sys_connect(void)
{
  struct file *f;
  int addrlen;
  struct sockaddr *addr;

  if (argfd(0, 0, &f) < 0 || argint(2, &addrlen) < 0 || argptr(1, (void*)&addr, addrlen) < 0)
    return -1;
  if (f->type != FD_SOCKET)
    return -1;
  return socketconnect(f->socket, addr, addrlen);
}

/**
 * @brief socket函数返回的文件描述符，和特定port绑定
 * 
 * @return int 
 */
int
sys_bind(void)
{
  struct file *f;
  int addrlen;// addrlen 用于确定不同类型的socket长度，这里可以不用管。
  struct sockaddr *addr; // sockaddr 用于和多种不同类型socket适配，不需要知道里面是什么，可以理解成里面只有port

  if (argfd(0, 0, &f) < 0 || argint(2, &addrlen) < 0 || argptr(1, (void*)&addr, addrlen) < 0)
    return -1;
  if (f->type != FD_SOCKET)
    return -1;
  return socketbind(f->socket, addr, addrlen);
}

/**
 * @brief 将server端的socket结构体状态改为LISTEN
 * 
 * @return int 
 */
int
sys_listen(void)
{
  struct file *f;
  int backlog;

  if (argfd(0, 0, &f) < 0 || argint(1, &backlog) < 0)
    return -1;
  if (f->type != FD_SOCKET)
    return -1;
  return socketlisten(f->socket, backlog);
}

/**
 * @brief 
 * 
 * @return int 
 */
int
sys_accept(void)
{
  struct file *f, *af;
  int *addrlen, afd;
  struct sockaddr *addr = NULL;

  if (argfd(0, 0, &f) < 0 || argptr(2, (void*)&addrlen, sizeof(*addrlen)) < 0)
    return -1;
  if (addrlen && argptr(1, (void*)&addr, *addrlen) < 0)
    return -1;
  if (f->type != FD_SOCKET)
    return -1;
  if ((af = socketaccept(f->socket, addr, addrlen)) == 0 || (afd = fdalloc(af)) < 0){
    if (af)
      fileclose(af);
    return -1;
  }
  return afd;
}

int
sys_recv(void)
{
  struct file *f;
  int n;
  char *p;

  if (argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  if (f->type != FD_SOCKET)
    return -1;
  return socketread(f->socket, p, n);
}

int
sys_send(void)
{
  struct file *f;
  int n;
  char *p;

  if (argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  if (f->type != FD_SOCKET)
    return -1;
  return socketwrite(f->socket, p, n);
}

int
sys_recvfrom(void)
{
  struct file *f;
  int n;
  char *p;
  int *addrlen;
  struct sockaddr *addr = NULL;

  if (argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0 || argptr(4, (void*)&addrlen, sizeof(*addrlen)) < 0)
    return -1;
  if (addrlen && argptr(3, (void*)&addr, *addrlen) < 0)
    return -1;
  if (f->type != FD_SOCKET)
    return -1;
  return socketrecvfrom(f->socket, p, n, addr, addrlen);
}

int
sys_sendto(void)
{
  struct file *f;
  int n;
  char *p;
  int addrlen;
  struct sockaddr *addr;

  if (argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0 || argint(4, &addrlen) < 0 || argptr(3, (void*)&addr, addrlen) < 0)
    return -1;
  if (f->type != FD_SOCKET)
    return -1;
  return socketsendto(f->socket, p, n, addr, addrlen);
}
