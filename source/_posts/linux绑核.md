
---
title: linux绑核
---

## 应用函数
sched_getaffinity()//获取目标进程所用CPU信息
sched_setaffinity()//设置目标进程所用CPU信息
CPU_COUNT()//获取当前进程可用CPU核心个数
CPU_SET()//设置当前进程对应的CPU核心
CPU_ISSET()//判断指定的CPU核是否是对应的正在使用的CPU核
get_nprocs_conf()//获取CPU核心数
get_nprocs()//获取当前进程可用CPU核心数
sysconf(_SC_NPROCESSORS_CONF)//程序已配置的核心数(类似于get_nprocs_conf)
sysconf(_SC_NPROCESSORS_ONLN)//程序当前可用核心数(类似于get_nprocs？)
pthread_setaffinity_np()//线程绑定CPU
pthread_getaffinity_np()//获取线程所用CPU信息
//据说，线程和进程的CPU绑定是分开的。也就是进程在绑定一个CPU的同时线程可用绑定另外一个CPU，得试下。

syscall(SYS_gettid)
gettid()与pthread_self()不一样，pthread_self()获取的线程id是针对进程内部的线程id，此线程id可以在不同进程复用，符合POSIX thread  ID，gettid()获取内核线程的id，在GLibc库中是不提供的指定函数，通过调用syscall(SYS_gettid)来获取该线程id。

top -p pid
输入f,把P选项调出来，再输入q
然后输入H
在top中输入H和获取进程中所有线程状态
