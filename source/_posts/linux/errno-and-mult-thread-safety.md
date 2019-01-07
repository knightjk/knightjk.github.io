---
title: errno与多线程变量安全
archives : 技术探索
categories : unix
date: 2018-12-17
---

## errno作用
---
* errno的功能

先man一下：
```
The value in errno is significant only when the return value of the call indicated an error (i.e., -1 from most system calls; -1 or NULL from most library functions); a function that succeeds is allowed to change errno.  The value of errno is never set to zero by any system  call or library function.
For some system calls and library functions (e.g., getpriority(2)), -1 is a valid return on success.  In such cases, a successful return can be distinguished from an error return by setting errno to zero before the call, and then, if the call returns a status that indicates that an error may have occurred, checking to see if errno has a nonzero value.

errno is defined by the ISO C standard to be a modifiable lvalue of type int, and must not be explicitly declared; errno may be a macro. errno is thread-local; setting it in one thread does not affect its value in any other thread.
```
就是说errno:
1. 用于提示API的具体出错信息。linux的API返回出错都是非0值，但更具体的定位出错需要errno值来辅助定位。
2. errno一般不会被置0，然后只有API出错返回时errno才被设置，此时查询errno才有意义。
3. errno可能是一个宏，被定义为线程局部的,即errno能够符合线程安全。

* errno的值定义

errno的具体指定义在sysdeps/mach/hurd/bits/errno.h文件中，~~有些长，这里先不列出来~~可以通过gcc官网git找到对应的[定义](https://sourceware.org/git/?p=glibc.git;a=blob;f=sysdeps/mach/hurd/bits/errno.h;h=8f2fbfd80ffd3dc3c765a7635203d69b1329ead4;hb=0c1719e65b2a5a80331d4f635612799f853b0479)

## 全局变量在多线程的风险
---
已知 errno在使用时需要通过 *extern int errno*声明为全局量定义，那么在多线程情况下是怎样呢，已知全局量在运行内存中是存放在数据段中，而多线程只有占用各自的小部分栈段和CPU寄存器资源，其他资源是共享的，那么如果 *errno*定义为普通的全局量，那么在多线程中共用的全局量在线程各自改变时肯定会受影响，譬如，线程1和线程2同时操作出错，但由于errno是全局量，有可能线程1捕获到的出错返回errno的信息实际上是线程2的报错信息，这样定义errno是不科学的。伟大的gnu必然会考虑这种情况，所以把errno定义为线程安全性的。那么 errno如何实现线程安全，下面继续深入了解errno实现。

## errno深入探索实现机制
---
先查libc的源码，先用source insight查看extern int errno 关键字，可以找到如下的errno的可能的宏定义：
```S
#/*sysdeps/unix/sysv/linux/riscv/sysdep.S*/
#if IS_IN (libc)
# define errno __libc_errno
#endif
```
```c
/*errno.h(stdlib)*/
/* The error code set by various library functions.  */
extern int *__errno_location (void) __THROW __attribute_const__;
# define errno (*__errno_location ())
```
```c
/*errno.h(include)*/
static int rtld_errno;
#  define errno rtld_errno
```
```c
/*errno.c(csu)*/
__thread int errno;
```
那么，到底哪个是可能的errno的具体实现定义？这需要看程序中用到的errno的宏展开情况，编写以下测试代码：
```c
//errno_test.c
#include <errno.h>

extern int errno;


int main(int argc, char* argv[])
{
    return 1;
}

```
然后只进行宏展开，不编译：
```sh
gcc -E errno_test.c
```
生成的结果：
```sh
# 1 "errno_test.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 1 "<command-line>" 2
# 1 "errno_test.c"

# 1 "/usr/include/errno.h" 1 3 4
# 28 "/usr/include/errno.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 367 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 1 3 4
# 410 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/bits/wordsize.h" 1 3 4
# 411 "/usr/include/x86_64-linux-gnu/sys/cdefs.h" 2 3 4
# 368 "/usr/include/features.h" 2 3 4
# 391 "/usr/include/features.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 1 3 4
# 10 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 3 4
# 1 "/usr/include/x86_64-linux-gnu/gnu/stubs-64.h" 1 3 4
# 11 "/usr/include/x86_64-linux-gnu/gnu/stubs.h" 2 3 4
# 392 "/usr/include/features.h" 2 3 4
# 29 "/usr/include/errno.h" 2 3 4


# 1 "/usr/include/x86_64-linux-gnu/bits/errno.h" 1 3 4
# 24 "/usr/include/x86_64-linux-gnu/bits/errno.h" 3 4
# 1 "/usr/include/linux/errno.h" 1 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/errno.h" 1 3 4
# 1 "/usr/include/asm-generic/errno.h" 1 3 4


# 1 "/usr/include/asm-generic/errno-base.h" 1 3 4
# 5 "/usr/include/asm-generic/errno.h" 2 3 4
# 1 "/usr/include/x86_64-linux-gnu/asm/errno.h" 2 3 4
# 1 "/usr/include/linux/errno.h" 2 3 4
# 25 "/usr/include/x86_64-linux-gnu/bits/errno.h" 2 3 4
# 50 "/usr/include/x86_64-linux-gnu/bits/errno.h" 3 4

# 50 "/usr/include/x86_64-linux-gnu/bits/errno.h" 3 4
extern int *__errno_location (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 36 "/usr/include/errno.h" 2 3 4
# 58 "/usr/include/errno.h" 3 4

# 3 "errno_test.c" 2


# 5 "errno_test.c"
extern int
# 5 "errno_test.c" 3 4
          (*__errno_location ())
# 5 "errno_test.c"
               ;

int main(int argc, char* argv[])
{
    return 1;
}
```
也就是说，extern int errno 中的errno被展开成 (*__errno_location ())，再深入探索__errno_location的定义:
```c
/* The error code set by various library functions.  */
extern int *__errno_location (void) __THROW __attribute_const__;
# define errno (*__errno_location ())

```
往下追踪发现有两个定义，一个弱引用函数一个普通全局函数，先看弱引用函数定义：
```c
#if IS_IN (rtld)
/*sysdeps/mach/hurd/Errno-loc.c*/
/* rtld can not access TLS too early, thus rtld_errno.

   Instead of making __open/__close pass errno from TLS to rtld_errno, simply
   use a weak __errno_location using rtld_errno, which will be overriden by the
   libc definition.  */
static int rtld_errno;
int * weak_function
__errno_location (void)
{
  return &rtld_errno;
}
libc_hidden_weak (__errno_location)
#else
#include "../../../csu/errno-loc.c"
#endif
```
这里说明libc的 *__errno_location*函数被定义时会使用libc的而忽略弱引用的函数，弱引用先返回的是rtld_errno这个静态量的地址。<br>
再来看看普通的*__errno_location*函数定义：
```
/*Errno-loc.c(csu)*/
int *
__errno_location (void)
{
  return &errno;
}
```
也就是说，最后返回的还是errno，那么这个errno到底是什么量？先用排除法排除以上的宏定义，得到errno可能是如下定义：
```c
/*errno.c(csu)*/
__thread int errno;
```
这个errno在定义时加上了一个 *__thread*，这个__thread是什么？这个是线程局部存储的关键字，而且这个关键字需要编译器作为支撑的，[这里](https://gcc.gnu.org/onlinedocs/gcc-3.4.6/gcc/Thread_002dLocal.html)是gcc.gnu对__thread关键字的说明:
```
Thread-local storage (TLS) is a mechanism by which variables are allocated such that there is one instance of the variable per extant thread. The run-time model GCC uses to implement this originates in the IA-64 processor-specific ABI, but has since been migrated to other processors as well. It requires significant support from the linker (ld), dynamic linker (ld.so), and system libraries (libc.so and libpthread.so), so it is not available everywhere.

At the user level, the extension is visible with a new storage class keyword: __thread. For example:

     __thread int i;
     extern __thread struct state s;
     static __thread char *p;
The __thread specifier may be used alone, with the extern or static specifiers, but with no other storage class specifier. When used with extern or static, __thread must appear immediately after the other storage class specifier.

The __thread specifier may be applied to any global, file-scoped static, function-scoped static, or static data member of a class. It may not be applied to block-scoped automatic or non-static data member.

When the address-of operator is applied to a thread-local variable, it is evaluated at run-time and returns the address of the current thread's instance of that variable. An address so obtained may be used by any thread. When a thread terminates, any pointers to thread-local variables in that thread become invalid.

No static initialization may refer to the address of a thread-local variable.

In C++, if an initializer is present for a thread-local variable, it must be a constant-expression, as defined in 5.19.2 of the ANSI/ISO C++ standard.
```
也就是说，对于__thread关键字：
1. 使被修饰的变量在每个现存的线程有各自的一个变量实例
2. 一开始gcc用于IA-64处理器，后面其他处理器也有，但不能保证所有处理器和编译其可用
3. 可以单独使用，与外部或静态说明符一起使用，但不与其他存储类说明符一起使用。它可能不适用于块作用域的自动或非静态数据成员。
4. 当线程终止时，该线程中任何指向线程本地变量的指针都将无效。

更深入的线程本地存储可以看(~~鄙人还没有看过的~~)[这篇文章](https://akkadia.org/drepper/tls.pdf)。

## __thread的使用
---
从深入探索errno可以学到了__thread定义的变量是线程安全的，那么把__thread放在我们自己程序中来试验下是否真的是这样，首选，编写一段测试程序：<br>
头文件：
```c
//dynamic_link_variable.h
#ifndef __DYNAMIC_LINK_VARIABLE_H__
#define __DYNAMIC_LINK_VARIABLE_H__

#include <stdio.h>

typedef struct{
    int* data_point;
    size_t size;
}data_area_t;

int create_point(size_t size);  //用于创建data_area_t结构体节点
int modified_datas(int* data, size_t size); //用于更改数据
void echo_datas(pthread_t pthread); //用于打印数据
int delete_point(void); //用于删除节点
#endif
```
目标使用程序如下：
```c
//call_dynamic_link.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "dynamic_link_variable.h"
//gcc call_dynamic_link.c -m32 -g -o call_dynamic -pthread -ldynamic -L.  -Wl,-rpath,./

void * test_thread1(void *arg)
{
    int data[12];
    sleep(2);
    memset(data,0x22,sizeof(data));

    create_point(24);
    printf("after thread1 create datas:\n");
    echo_datas(pthread_self());
    modified_datas(data, sizeof(data)/sizeof(data[0]));
    printf("after thread1 modified datas:\n");
    echo_datas(pthread_self());
    return NULL;
}

void * test_thread2(void *arg)
{
    int data[12];
    sleep(1);
    memset(data,0x33,sizeof(data));

    create_point(24);
    printf("after thread2 create datas:\n");
    echo_datas(pthread_self());
    modified_datas(data, sizeof(data)/sizeof(data[0]));
    printf("after thread2 modified datas:\n");
    echo_datas(pthread_self());
    /*需要以下延迟来延长thread2的生存周期，否则如果thread2比thread1先退出，
    根据这个程序条件那么thread1将会利用thread2使用过的空间,也就是说，
    如果thread1和thread2有先后运行完成的关系，就会导致thread1和thread2开辟的同一段地址的空间
    这是free函数的机制，free并不真正把空间释放，之前的空间仍然有保留*/
    sleep(2);
    echo_datas(pthread_self());
    return NULL;
}


int main(int argc, char* argv[])
{
    pthread_t thread1;
    pthread_t thread2;

    int data[12];
    memset(data,0x44,sizeof(data));
    create_point(24);
    echo_datas(pthread_self());
    modified_datas(data, sizeof(data)/sizeof(data[0]));
    //create thread1
    printf("thread1 = 0x%x\n",(uint32_t)thread1);
    pthread_create(&thread1, NULL, test_thread1, NULL);
    //create thread2
    printf("thread2 = 0x%x\n",(uint32_t)thread2);
    pthread_create(&thread2, NULL, test_thread2, NULL);
    //thread join
    pthread_join(thread1, (void**)&data);
    //thread join
    pthread_join(thread2, (void**)&data);

    return 1;
}
```
就是在创建两个线程之前主线程先改变和打印设置的量，然后线程thread2先再次创建和改变打印设置的量，中间停顿2s，在此期间thread1创建并改变和打印设置的量，然后thread1结束，thread2停顿2s后再次打印，具体的打印都会把设置量的值和地址体现出来。<br>
接下来先再证实一下普通全局变量在多线程的使用情况，以下是变量的定义文件：
```c
//dynamic_link_variable.c
//test normal global data

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "dynamic_link_variable.h"

//gcc dynamic_link_variable.c -m32 -g -fPIC -shared -o libdynamic.so -pthread
data_area_t global_data = {.data_point = NULL, .size = 0};

int create_point(size_t size)
{
    if(global_data.data_point != NULL)
        return 1;

    global_data.data_point = (int *)malloc(sizeof(int)*size);
    if(global_data.data_point == NULL)
    {
        return -1;
    }

    global_data.size = size;

    return 0;
}

int modified_datas(int* data, size_t size)
{
    if(size > global_data.size)
        return -1;
    
    printf("modified data address is 0x%x\n",(uint32_t)&global_data);

    memcpy(global_data.data_point, data, size*(sizeof(int)));

    return 0;
}

void echo_datas(pthread_t pthread)
{
    int i = 0;
    //打印线程ID和变量的地址
    printf("pthread = 0x%x, echo data address is 0x%x, size is 0x%x\n",(uint32_t)pthread,(uint32_t)&global_data,global_data.size);
    for(i = 0;i < global_data.size; i++)
    {
        printf("0x%x ",global_data.data_point[i]);
    }
    printf("\n");
}

int delete_point(void)
{
    if(global_data.data_point != NULL)
        free(global_data.data_point);
    global_data.size = 0;
    global_data.data_point = NULL;
    return 0;
}
```
编译
```sh
gcc dynamic_link_variable.c -m32 -g -fPIC -shared -o libdynamic.so -pthread #生成libdynamic.so
gcc call_dynamic_link.c -m32 -g -o call_dynamic -pthread -ldynamic -L.  -Wl,-rpath,./
```
运行
```sh
seeing-zynq@seeing:thread_variable$ ./call_dynamic
pthread = 0xf7d51700, echo data address is 0xf7f43028, size is 0x18
0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
modified data address is 0xf7f43028
thread1 = 0x2f
thread2 = 0xf7d5fdc8
after thread2 create datas:
pthread = 0xf754fb40, echo data address is 0xf7f43028, size is 0x18
0x44444444 0x44444444 0x44444444 0x44444444 0x44444444 0x44444444 0x44444444 0x44444444 0x44444444 0x44444444 0x44444444 0x44444444 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
modified data address is 0xf7f43028
after thread2 modified datas:
pthread = 0xf754fb40, echo data address is 0xf7f43028, size is 0x18
0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
after thread1 create datas:
pthread = 0xf7d50b40, echo data address is 0xf7f43028, size is 0x18
0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
modified data address is 0xf7f43028
after thread1 modified datas:
pthread = 0xf7d50b40, echo data address is 0xf7f43028, size is 0x18
0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
pthread = 0xf754fb40, echo data address is 0xf7f43028, size is 0x18
0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
```
结果显示普通全局量 *global_data*的地址在多线程打印出来是一个地址（都为0xf7f43028），由此可见普通全局变量在多线程是不安全的，这个在上面有提到，初始化的全局变量在初始化的数据段中，然鹅多线程会共用这个初始化的数据段。<br>
那么只将全局量用 *__thread*修饰后会怎样，只更改的上述代码，将global_data在定义时用 *__thread*修饰：
```c
__thread data_area_t global_data = {.data_point = NULL, .size = 0};
```
然后编译
```c
gcc dynamic_link_variable.c -m32 -g -fPIC -shared -o libdynamic.so -pthread #生成libdynamic.so
```
运行
```sh
seeing-zynq@seeing:thread_variable$ ./call_dynamic
pthread = 0xf7d3a700, echo data address is 0xf7d3a6f8, size is 0x18
0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
modified data address is 0xf7d3a6f8
thread1 = 0x2f
thread2 = 0xf7d48dc8
after thread2 create datas:
pthread = 0xf7538b40, echo data address is 0xf7538b38, size is 0x18
0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
modified data address is 0xf7538b38
after thread2 modified datas:
pthread = 0xf7538b40, echo data address is 0xf7538b38, size is 0x18
0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
after thread1 create datas:
pthread = 0xf7d39b40, echo data address is 0xf7d39b38, size is 0x18
0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
modified data address is 0xf7d39b38
after thread1 modified datas:
pthread = 0xf7d39b40, echo data address is 0xf7d39b38, size is 0x18
0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
pthread = 0xf7538b40, echo data address is 0xf7538b38, size is 0x18
0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
```
结果显示，在不同线程中global_data的存放地址是不同的，主线程的global_data地址在0xf7d3a6f8，线程1的global_data地址在0xf7d39b38，线程2的global_data地址在0xf7538b38。从这里可以看出，__thread关键字为每个线程开辟的global_data的地址是不同的。那么此时的global_data还是我们所谓的全局量吗？它存放的位置在哪里，是仍然存放在初始化的数据段，还是放在堆、栈或者其他什么地方呢？可以通过readelf命令查看global_data的信息。<br>
命令：
```sh
readelf -a libdynamic.so
```
先看没有__thread修饰的情况，打印太多，只看需要看的部分
```
Symbol table '.symtab' contains 72 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
   .....
   59: 00002028     8 OBJECT  GLOBAL DEFAULT   24 global_data
   .....
```
可见在没有__thread修饰情况下，global_data为OBJECT类型，放置在全局。<br>
然后再看有__thread修饰的情况：
```
Symbol table '.symtab' contains 72 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
   .....
   61: 00000000     8 TLS     GLOBAL DEFAULT   17 global_data
   .....
```
也就是说在__thread修饰后，该变量仍然是全局性的，这说明__thread并不改变变量的存放位置，只是把类型声明为TLS类型。

## 另一种方法实现变量线程安全
---
其实就是以下4个函数，利用key值来锁定分配的内存的位置信息，然后达到各个线程通过访问pthread_getspecific函数的方式来间接对堆的各个不同空间进行访问。

```
int pthread_key_create(pthread_key_t *key, void (*destr_function) (void *));

int pthread_key_delete(pthread_key_t key);

int pthread_setspecific(pthread_key_t key, const void *pointer);

void * pthread_getspecific(pthread_key_t key);

```
man手册上有一个简单的使用栗子（在此之前，可能需要安装一下pthread的man手册，在ubuntu上用sudo apt-get install glibc-doc和sudo apt-get install manpages-posix manpages-posix-devl来安装关于pthread的man手册）：
```c
/* Key for the thread-specific buffer */
static pthread_key_t buffer_key;

/* Once-only initialisation of the key */
static pthread_once_t buffer_key_once = PTHREAD_ONCE_INIT;

/* Allocate the thread-specific buffer */
void buffer_alloc(void)
{
  pthread_once(&buffer_key_once, buffer_key_alloc);
  pthread_setspecific(buffer_key, malloc(100));
}

/* Return the thread-specific buffer */
char * get_buffer(void)
{
  return (char *) pthread_getspecific(buffer_key);
}

/* Allocate the key */
static void buffer_key_alloc()
{
  pthread_key_create(&buffer_key, buffer_destroy);
}

/* Free the thread-specific buffer */
static void buffer_destroy(void * buf)
{
  free(buf);
}
```
当然在这个过程中还需要利用pthread_once函数来锁定全局的key只用初始化一次。接下来编写上述生成libdynamic.so的另一个实现：
```c
//dynamic_link_variable2.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "dynamic_link_variable.h"


//gcc dynamic_link_variable2.c -m32 -g -fPIC -shared -o libdynamic.so -pthread

/* Key for the thread-specific buffer */
static pthread_key_t buffer_key;
/* Once-only initialisation of the key */
static pthread_once_t buffer_key_once = PTHREAD_ONCE_INIT;

/* Free the thread-specific buffer */
static void buffer_destroy(void * buf)
{
    data_area_t* data_area = (data_area_t*)buf;
    free(data_area->data_point);
    free(data_area);
}

/* Allocate the key */
static void buffer_key_alloc()
{
    pthread_key_create(&buffer_key, buffer_destroy);
    printf("--------------once process--------------\n");
}

/* Allocate the thread-specific buffer */
int buffer_alloc(size_t size)
{
    data_area_t* data_area;
    pthread_once(&buffer_key_once, buffer_key_alloc);   //only execute once

    data_area = (data_area_t*)malloc(sizeof(data_area_t));
    if(data_area == NULL)
        return -1;

    data_area->data_point = (int* )malloc(sizeof(int) * size);
    if(data_area->data_point == NULL)
    {
        free(data_area);
        return -2;
    }
    data_area->size = size;

    pthread_setspecific(buffer_key, data_area);

    return 0;
}

int create_point(size_t size)
{
    return buffer_alloc(size);
}

int modified_datas(int* data, size_t size)
{
    data_area_t* data_area = (data_area_t* )pthread_getspecific(buffer_key);
    
    if(data_area == NULL)
        return -1;
    if(size > data_area->size)
        return -2;

    printf("modified data address is 0x%x\n",(uint32_t)data_area);
    memcpy(data_area->data_point, data, size*(sizeof(int)));

    return 0;
}

void echo_datas(pthread_t pthread)
{
    int i = 0;
    data_area_t* data_area = (data_area_t* )pthread_getspecific(buffer_key);
    if(data_area == NULL)
        return ;

    printf("pthread = 0x%x, echo data address is 0x%x,size is 0x%x\n",(uint32_t)pthread, (uint32_t)data_area,data_area->size);
    for(i = 0;i < data_area->size; i++)
    {
        printf("0x%x ",data_area->data_point[i]);
    }
    printf("\n");
}

int delete_point(void)
{
    return 0;
}
```
编译：
```sh
gcc dynamic_link_variable2.c -m32 -g -fPIC -shared -o libdynamic.so -pthread
```
同样运行call_dynamic：
```sh
seeing-zynq@seeing:thread_variable$ ./call_dynamic
--------------once process--------------
pthread = 0xf7cfe700, echo data address is 0x81d1410,size is 0x18
0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
modified data address is 0x81d1410
thread1 = 0x2f
thread2 = 0xf7d0cdc8
after thread2 create datas:
pthread = 0xf74fcb40, echo data address is 0xf6b00470,size is 0x18
0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
modified data address is 0xf6b00470
after thread2 modified datas:
pthread = 0xf74fcb40, echo data address is 0xf6b00470,size is 0x18
0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
after thread1 create datas:
pthread = 0xf7cfdb40, echo data address is 0xf6900470,size is 0x18
0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
modified data address is 0xf6900470
after thread1 modified datas:
pthread = 0xf7cfdb40, echo data address is 0xf6900470,size is 0x18
0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x22222222 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
pthread = 0xf74fcb40, echo data address is 0xf6b00470,size is 0x18
0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x33333333 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
```
实现的效果与__thread的修饰符差不多，但是，通过pthread_setspecific系列函数是用于定义指针指向对于的内存空间的一个“别名”，然后在各个线程中各自指定使用。

## 疑问
---
1. __thread到底在gcc内部如何实现变量的线程安全?
2. pthread_setspecific系列函数如何实现在各个不同线程的键(key)-值对应机制？

## 参考
---
1. https://gcc.gnu.org/onlinedocs/gcc-3.4.6/gcc/Thread_002dLocal.html
2. https://blog.csdn.net/JS072110/article/details/44855565


*[未完持续...]*