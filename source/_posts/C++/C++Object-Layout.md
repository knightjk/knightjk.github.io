---
title: C++对象在空间中的布局
---

## C++空类空间
---
* C中的struct空结构体
```c
struct SEmpty{};             //size of SEmpty is 0;
struct SEmpty sEmpty;         //size of sEmpty is 0;
```

* C++中的class与struct空类
```C++
class CEmpty{};              //size of CEmpty is 1
struct SEmpty{};             //size of SEmpty is 1
```
>    这是因为C++标准中不允许对象的大小为0，所以用一个字节来区分空类，使得C++类在实例化时得以在内存中配置唯一的地址，但子类继承空类并不会把该字节长度加入子类中：

```C++
class CDeriveE1:public CEmpty { };                  //size of CDeriveE1 is 1
class CDeriveE2:public CEmpty {public: int a; };    //size of CDeriveE2 is 4 (32 bit machine -m32)
class CDeriveE3:public CEmpty {public: int fun(){} ; };    //size of CDeriveE3 is 1 (32 bit machine -m32),成员函数不占空间
```
>    CDeriveE2中有成员变量，并且该成员变量可以在类实例化时可以分配内存唯一地址，这样，那个一字节的空间会被有些编译器(现在用的g++)替换成该成员变量的空间.


## C++数据类空间
---
* 单个数据类
```C++
class CBase0                
{
public:
    int baseData0;
}; //size of CBase0 is 4  ，baseData0占4字节     
```
* 继承数据类
```C++
class CDrive0B1: public CBase0 { }; //size of CDrive0B1 is 4
class CDrive0B2: public CBase0 { }; //size of CDrive0B2 is 4
```
>   很显然，这就是数据继承的意义，子类数据继承父类数据而不用开辟新的空间而导致浪费，那么以下这种情况会怎样。
* 菱形多继承
```C++
class CDriveD0: public CDrive0B1, public CDrive0B2 {};  //size of CDrive0B2 is 8
```
>   子类的数据空间被扩大一倍，那么子类到底拥有的是哪里的数据，是父类CDrive0B1和CDrive0B2的数据，还是祖先类CBase0与其中一个父类的数据？通过以下实验观察下：
```C++
CDriveD0 cdrived0;

// cdrived0.baseData0 = 1;             //编译出错，非静态变量baseData0引起歧义
// cdrived0.Base0::baseData0 = 1;      //编译出错，非静态变量baseData0引起歧义
cdrived0.CDrive0B1::baseData0 = 1;
cdrived0.CDrive0B2::baseData0 = 2;
```
> 通过gdb断点打印cdrived0信息如下：
```sh
(gdb) print cdrived0
$1 = {<CDrive0B1> = {<CBase0> = {baseData0 = 1}, <No data fields>}, <CDrive0B2> = {<CBase0> = {baseData0 = 2}, <No data fields>}, <No data fields>}
(gdb)
```
> 通过该现象说明，对于菱形继承的数据区，子类并没有单独保留祖先类的数据，但父类都继承自祖先类数据，在子类中会对多个父类的数据都保留一份。而如果子类只保留祖先类的数据，两个父类是相对不同的类却在子类中没能被继承数据，这样相当于两个不同的父类共用一套数据，那么在构造该子类进行初始化时，到底该由哪个父类的构造函数进行该数据的初始化就成为了矛盾；如果子类把父类和祖先类的数据都保留下来，那么，对于父类来说，祖先类的数据与父类不同，这也不符合继承规则，这种情况与一个子类非菱形继承三个父类的情况的区分难以辨识（~~没错这就是强行为编译器辩解~~）。唯一能够保持语义的是父类双亲各自保留数据，祖先类数据由双亲各自表现。


## 虚函数表
---

### 有无virtual关键字的区别

* 类的大小
<p style="text-indent:2em">
    继续研究类中有虚继承和类中有虚函数的情况：
</p>

```C++
//基类：
class CBase1
{
public: 

    void func1(void)
    {
        cout<<"CBase1::func1"<<endl;
    };

    void base1func1(void)
    {
        cout<<"CBase1::base1func1"<<endl;
    };
    
    virtual void vfunc1(void)
    {
        cout<<"CBase1::vfunc1"<<endl;
    };

    virtual void vb1func1(void)
    {
        cout<<"CBase1::vb1func1"<<endl;
    }
};  //size of CBase1 is 4
```

1. 没有virtual的子类单继承时

```C++
class CDeriveE1:public CEmpty { };          //size of CDeriveE1 is 1;
```

2. 有virtual的子类单继承时
```C++
class CVDeriveE1:public virtual CEmpty { };     //size of CVDeriveE1 is 4 (-m32)
class CDrive: public CBase1 { };                //size of CDrive is 4 (-m32)
```
>   这里分为两种情况，一种是虚继承，即在定义时指定父类派生到子类的方式声明父类为子类的虚类，如CVDeriveE1虚继承CEmpty那样。另一种为普通继承，但继承的父类里面有虚函数，如CDrive继承CBase1那样，这两种方式都会使得子类在原来没有虚表指针的情况下多一个虚表指针，区别在于，一方面，从子类的大小上说，虚继承只是在子类中指定有这么一个指向虚表的指针，如果子类原来就有虚表指针，那么虚继承不会再给子类多分配一个虚表指针的空间，如下定义所示：
```C++
class CVDrive: public virtual CBase1 { };                //size of CVDrive is 4 (-m32)
```
>   这种情况放在虚继承多个父类也一样，只要其中一个父类有虚函数，，或者所有父类都采用次虚继承方式（即使所有父类都没有虚函数），虚继承方式不会再产生新叠加的虚表指针。<br>
>   另一方面，从虚继承本身的意义上说，虚继承时的虚函数表指针用于归总父类中相同的虚函数，这种情况常用于菱形继承，但菱形继承也不会对虚继承多分配一个虚表指针空间，这个后面在进行说明（没有虚函数的菱形继承，有虚函数的菱形继承，没有虚函数有虚继承的菱形继承，有虚函数有虚继承的菱形继承的情况）。

3. 有继承多个virtual的基类时

```C++
class CBase2
{
public: 
    void func2(void)
    {
        cout<<"CBase2::func2"<<endl;
    };

    void base1func2(void)
    {
        cout<<"CBase2::base1func2"<<endl;
    };
    
    virtual void vfunc2(void)
    {
        cout<<"CBase2::vfunc2"<<endl;
    };
};

class CDrive1B1: public  CBase1
{
public: 
    void func1(void)
    {
        cout<<"CDrive1B1::func1"<<endl;
    };

    void cdriveb1func1(void)
    {
        cout<<"CDrive1B1::cdriveb1func1"<<endl;
    };

    virtual void vfunc1(void)
    {
        cout<<"CDrive1B1::vfunc1"<<endl;
    };

    virtual void v1func1(void)
    {
        cout<<"CDrive1B2::v1func1"<<endl;
    } ;
};  //size of CDrive1B1 is 4

class CDrive1B2: public  CBase1 
{
public: 
    void func1(void)
    {
        cout<<"CDrive1B2::func1"<<endl;
    };

    void cdriveb2func1(void)
    {
        cout<<"CDrive1B2::cdriveb2func1"<<endl;
    };

    virtual void vfunc1(void)
    {
        cout<<"CDrive1B2::vfunc1"<<endl;
    } ;

    virtual void v2func1(void)
    {
        cout<<"CDrive1B2::v2func1"<<endl;
    } ;
};         //size of CDrive1B2 is 4

class CDriveD1: public CDrive1B1,public CDrive1B2
{
public:
    void func1(void)
    {
        cout<<"CDriveD1::func1"<<endl;
    };

    void cdrivedfunc1(void)
    {
        cout<<"CDriveD1::cdrivedfunc1"<<endl;
    };

    virtual void vfunc1(void)
    {
        cout<<"CDriveD1::vfunc1"<<endl;
    };

    virtual void v1func1(void)
    {
        cout<<"CDriveD1::v1func1"<<endl;
    } ;

    virtual void v2func1(void)
    {
        cout<<"CDriveD1::v2func1"<<endl;
    } ;

    virtual void vd1func1(void)
    {
        cout<<"CDriveD1::vd1func1"<<endl;
    } ;
};      //size of CDriveD1 is 8

class CDriveD1E: public CDrive1B1, public virtual CDeriveE1 
{
public:
    virtual void vfunc1(void)
    {
        cout<<"CDriveD1::vfunc1"<<endl;
    };
};        //size of CDriveD1E is 4，这里说明虚继承不会在已有虚指针上再多加一个


```
>   CDriveD1属于菱形继承，分别从CDrive1B1和CDrive1B2继承了虚表指针，通过以下程序和gdb打印可以看出:

```C++
CDrive1B1 cdrive1b1;
CDrive1B2 cdrive1b2;
CDriveD1 cdrived ;
```
```sh
(gdb) p cdriveb1
$2 = {
    <CBase1> = {
        _vptr.CBase1 = 0x8049550 <vtable for CDrive1B1+8>
        }, 
        <No data fields>
    }
(gdb) p cdriveb2
$3 = {
    <CBase1> = {
        _vptr.CBase1 = 0x804953c <vtable for CDrive1B2+8>
        }, 
        <No data fields>
    }
(gdb) p cdrived
$4 = {
    <CDrive1B1> = {
        <CBase1> = {
            _vptr.CBase1 = 0x80494f8 <vtable for CDriveD1+8>
            }, 
            <No data fields>
        }, 
    <CDrive1B2> = {
        <CBase1> = {
            _vptr.CBase1 = 0x8049514 <vtable for CDriveD1+36>
            }, 
            <No data fields>
        }, 
    <No data fields>
    }

```
>   在cdrived存在两个虚函数表指针，其中分别是从父类CDrive1B1和CDrive1B2获取，这说明对于多继承情况下，子类对象会对每个父类多分配出对应虚函数表指针的空间，所以cdrived的空间大小为两个虚表指针所占空间之和；而父类中的虚函数表指针又是从祖父类中获取。


### 深入虚函数和虚继承

* 虚函数的作用

>   以下摘要自维基百科：
```
在面向对象程序设计领域，C++、Object Pascal 等语言中有虚函数（英语：virtual function）或虚方法（英语：virtual method）的概念。这种函数或方法可以被子类继承和覆盖，通常使用动态调度实现。这一概念是面向对象程序设计中（运行时）多态的重要组成部分。简言之，虚函数可以给出目标函数的定义，但该目标的具体指向在编译期可能无法确定。
在面向对象程序设计中，派生类继承自基类。使用指针或引用访问派生类对象时，指针或引用本身所指向的类型可以是基类而不是派生类。

```
>   意思很明显，也就是说虚函数能够提供在子类重写父类方法后，父类指针或引用能够在运行期动态选择运行子类对象的函数。继续上面的类定义，给出对应示例：

1. 单继承无虚函数与有虚函数：
```C++
CDrive1B1 cdrive1b1;
CBase1* pbase1;

pbase1 = &cdrive1b1;
pbase1->func1();    //输出"CBase1::func1"
pbase1->vfunc1();   //输出"CDrive1B1::vfunc1"
```
>   gdb显示pbase1和cdrive1b1的内容：<br>
```sh
(gdb) p *pbase1
$2 = {_vptr.CBase1 = 0x8049550 <vtable for CDrive1B1+8>}
(gdb) p cdrive1b1
$3 = {<CBase1> = {_vptr.CBase1 = 0x8049550 <vtable for CDrive1B1+8>}, <No data fields>}
```
>   gdb显示的pbase1和cdrive1b1内容只是虚表指针，还需要虚表指针内更深层次的信息,在此之前，先确定虚表指针在对象中存储的位置，由于对象cdrive1b1及其父类没有数据成员，为了充分验证，新追加数据类如下(~~要补图呀，补CDrive1B3的关系图！~~)：<br>
```C++
class CDrive1B3: public  CBase1 
{
public: 
    void func1(void)
    {
        cout<<"CDrive1B3::func1"<<endl;
    };

    void cdriveb2func1(void)
    {
        cout<<"CDrive1B3::cdriveb2func1"<<endl;
    };

    virtual void vfunc1(void)
    {
        cout<<"CDrive1B3::vfunc1"<<endl;
    } ;

    virtual void v2func1(void)
    {
        cout<<"CDrive1B3::v2func1"<<endl;
    } ;

    int cdriveData;
};         //size of CDrive1B3 is 8

```
>   运行程序
```C++
// CDrive1B1 cdrive1b1;
CDrive1B3 cdrive1b3;
CBase1* pbase1;

// pbase1 = &cdrive1b1;
pbase1 = &cdrive1b3;
pbase1->func1();    //输出"CBase1::func1"
pbase1->vfunc1();   //输出"CDrive1B3::vfunc1"
```
>   gdb 打印cdrive1b3对象的内容和虚函数表指针<br>
```sh
(gdb) p cdrive1b3
$1 = {<CBase1> = {_vptr.CBase1 = 0x80496b8 <vtable for CDrive1B3+8>}, cdriveData = 1}
(gdb) p *pbase1
$2 = {_vptr.CBase1 = 0x80496b8 <vtable for CDrive1B3+8>}
```
>   继续打印cdrive1b3存储的信息<br>
```sh
(gdb) p/x (&((int)cdrive1b3))
$3 = 0xffffce0c
(gdb) p/x (&((int)cdrive1b3))+1
$4 = 0xffffce10
(gdb) p/x *((&((int)cdrive1b3))+0)
$5 = 0x80496b8
(gdb) p/x *((&((int)cdrive1b3))+1)
$6 = 0x1
```
>   由此可知，cdrive1b3存放的第一个数据是虚表指针（值为0x8049604），第二个数据是数据成员cdriveData的值（值为0x01），也就是说，虚表指正存在对象的前面的位置中，至少目前g++是这样处理虚表指针的位置。那么，虚表指针0x80496b8指向的内容存放的是什么？继续打印虚表指针指向的内容：
```sh
(gdb) p (long*)*(long*)0x80496b8
$7 = (long *) 0x8048ec6 <CDrive1B3::vfunc1()>
(gdb) p (long*)*((long*)0x80496b8 + 1)
$8 = (long *) 0x8048d38 <CBase1::vb1func1()>
(gdb) p (long*)*((long*)0x80496b8 + 2)
$9 = (long *) 0x8048ef6 <CDrive1B3::v2func1()>
(gdb) p (long*)*((long*)0x80496b8 + 3)
$10 = (long *) 0x0
```
>   继续换一种方式确定对象中虚函数的存储位置：
```sh
(gdb) p cdrive1b3.vfunc1
$19 = {void (CDrive1B3 * const)} 0x8048ec6 <CDrive1B3::vfunc1()>
(gdb) p cdrive1b3.vb1func1
$20 = {void (CBase1 * const)} 0x8048d38 <CBase1::vb1func1()>
(gdb) p cdrive1b3.v2func1
$21 = {void (CDrive1B3 * const)} 0x8048ef6 <CDrive1B3::v2func1()>
```
>   cdrive1b3虚表指针存放的是该类中所有虚函数的指针，如图所示（~~要补图呀~~）。从结果可以看出，如果子类重写父类虚函数，那么子类对象调用的是子类的虚函数，同样，即使父类指针指向子类对象，父类指针也仍然会用子类重写的虚函数。

2. 多继承中的虚函数
>   接下来观察继承多个有虚函数的父类的情况，先定义一个简单多继承非多次继承的子类，分析之：
```C++
class CDriveB1B2: public CBase1,public CBase2
{
};//size of CDriveB1B2 is 8
```
>   该多继承类结构如下图所示~~补图呀~~<br>

>   构造该对象，并用gdb打印该对象的内容：
```C++
CDriveB1B2 cdriveb1b2;      
```
```sh
(gdb) p cdriveb1b2
$1 = {<CBase1> = {_vptr.CBase1 = 0x804953c <vtable for CDriveB1B2+8>}, <CBase2> = {
    _vptr.CBase2 = 0x804954c <vtable for CDriveB1B2+24>}, <No data fields>}
(gdb) p (long*)*((long*)0x804953c + 1)
$2 = (long *) 0x8048d38 <CBase1::vb1func1()>
(gdb) p (long*)*((long*)0x804953c + 0)
$3 = (long *) 0x8048d08 <CBase1::vfunc1()>
(gdb) p (long*)*((long*)0x804953c + 1)
$4 = (long *) 0x8048d38 <CBase1::vb1func1()>
(gdb) p (long*)*((long*)0x804953c + 2)
$5 = (long *) 0xfffffffc
(gdb) p (long*)*((long*)0x804954c + 0)
$6 = (long *) 0x8048d68 <CBase2::vfunc2()>
(gdb) p (long*)*((long*)0x804954c + 1)
$7 = (long *) 0x0
```
>   由此可以看出，cdriveb1b2有两个虚表指针，占据cdriveb1b2的存储空间，这两个虚表指针分别继承自两个父类CBase1和CBase2的虚函数表，由此可知，CDriveB1B2的存储关系如下~~补图呀~~

>   再来分析多父类有多次继承的子类，先定义一个
```C++
class CDriveD1S: public CDrive1B1, public CBase2
{

};  //size of CDriveD1S is 8
```
```C++
CDriveD1S cdrive1s;     //
```
>   CDriveD1S类的继承关系如下图所示~~补图~~<br>

>   那么实际上CDriveD1S的存储空间及其虚表内容是怎样，继续用gdb的方式与上面同样的方法将cdrive1s的内容打印出来：
```sh
#对象存储的内容
(gdb) p cdrive1s
$1 = {
    <CDrive1B1> = {
        <CBase1> = {
            _vptr.CBase1 = 0x80494bc <vtable for CDriveD1S+8>
            },
            <No data fields>
            }, 
        <CBase2> = {
            _vptr.CBase2 = 0x80494d0 <vtable for CDriveD1S+28>
            }, 
            <No data fields>
    }

#第一个虚表指针的内容
(gdb) p (long*)*((long*)0x80494bc + 0)
$5 = (long *) 0x8048df8 <CDrive1B1::vfunc1()>
(gdb) p (long*)*((long*)0x80494bc + 1)
$6 = (long *) 0x8048d38 <CBase1::vb1func1()>
(gdb) p (long*)*((long*)0x80494bc + 2)
$7 = (long *) 0x8048e28 <CDrive1B1::v1func1()>
(gdb) p (long*)*((long*)0x80494bc + 3)
$8 = (long *) 0xfffffffc

#第二个虚表指针的内容
(gdb) p (long*)*((long*)0x80494d0 + 0)
$9 = (long *) 0x8048d68 <CBase2::vfunc2()>
(gdb) p (long*)*((long*)0x80494d0 + 1)
$10 = (long *) 0x0

```
>   虽然cdrive1s打印的虚函数内容多一些，但虚表结构是与cdriveb1b2一致。

3. 菱形继承中的虚函数
>   继续研究CDriveD1类的继承关系，如图所示~~给图呀~~：<br>

>   同样用gdb打印虚表信息：

```sh
# cdrived中的结构
(gdb) p cdrived
$12 = {
    <CDrive1B1> = {
        <CBase1> = {
            _vptr.CBase1 = 0x80494f8 <vtable for CDriveD1+8>
            }, 
            <No data fields>
        }, 
    <CDrive1B2> = {
        <CBase1> = {
            vptr.CBase1 = 0x8049514 <vtable for CDriveD1+36>
            }, 
            <No data fields>
        }, 
    <No data fields>
    }
# cdrived第一张虚表（从CDrive1B1）
(gdb) p (long*)*((long*)0x80494f8 + 0)
$14 = (long *) 0x8048f18 <CDriveD1::vfunc1()>
(gdb) p (long*)*((long*)0x80494f8 + 1)
$15 = (long *) 0x8048d38 <CBase1::vb1func1()>
(gdb) p (long*)*((long*)0x80494f8 + 2)
$16 = (long *) 0x8048f4e <CDriveD1::v1func1()>
(gdb) p (long*)*((long*)0x80494f8 + 3)
$17 = (long *) 0x8048f7e <CDriveD1::v2func1()>
(gdb) p (long*)*((long*)0x80494f8 + 4)
$18 = (long *) 0x8048fb4 <CDriveD1::vd1func1()>
(gdb) p (long*)*((long*)0x80494f8 + 5)
$19 = (long *) 0xfffffffc

# cdrived第二张虚表（从CDrive1B2）
(gdb) p (long*)*((long*)0x8049514 + 0)
$20 = (long *) 0x8048f47 <non-virtual thunk to CDriveD1::vfunc1()>
(gdb) p (long*)*((long*)0x8049514 + 1)
$21 = (long *) 0x8048d38 <CBase1::vb1func1()>
(gdb) p (long*)*((long*)0x8049514 + 2)
$22 = (long *) 0x8048fad <non-virtual thunk to CDriveD1::v2func1()>
(gdb) p (long*)*((long*)0x8049514 + 3)
$23 = (long *) 0x0
```
>   菱形继承的虚函数表与普通多继承的数据一致，但是，仔细观察可以发现，这两个分别从CDrive1B1和CDrive1B2继承而来的虚表有重叠部分 —— 祖先类的虚函数CBase1::vb1func1()被重复放在这两个父类的虚函数表中了。那么如果子类CDriveD1要调用祖先类的vb1func1会怎样？
```C++
    cdrived.vb1func1();         //编译出错，提示函数‘vb1func1’有歧义
    cdrived.CBase1::vb1func1(); //编译出错，提示‘CBase1’对‘CDriveD1’有歧义
```
>   此时编译会出错，出错的原因是这两个父类在各自的虚表中都保留一份CBase1::vb1func1()的函数指针，虽然内容一样，但实际运行时程序无法选择到底用哪一边的虚函数指针的偏置来索引对应的函数（是应该用CDrive1B1的虚表指针索引呢还是应该用CDrive1B2的虚表指针索引），这就引发歧义；即使用作用域限定也不行，~~这个不懂~~这个等了解作用域运算的作用后再细研究。那有什么方法能够使得子类CDriveD1能够调用祖先类CBase1的vb1func1函数呢？下面的虚继承可以完成这项艰巨的任务。

* 虚继承的作用

>   紧接上面的问题，为了做对比，保留原有的函数结构，只在父类用虚继承的方式继承祖父类CBase1，新的类定义如下~~添图~~：

```C++
class CVDrive1B1: public virtual CBase1
{
public: 
    void func1(void)
    {
        cout<<"CVDrive1B1::func1"<<endl;
    };

    void cdriveb1func1(void)
    {
        cout<<"CVDrive1B1::cdriveb1func1"<<endl;
    };

    virtual void vfunc1(void)
    {
        cout<<"CVDrive1B1::vfunc1"<<endl;
    };

    virtual void v1func1(void)
    {
        cout<<"CVDrive1B1::v1func1"<<endl;
    } ;
};  //size of CVDrive1B1 is 4

class CVDrive1B2: public virtual CBase1 
{
public: 
    void func1(void)
    {
        cout<<"CVDrive1B2::func1"<<endl;
    };

    void cdriveb2func1(void)
    {
        cout<<"CVDrive1B2::cdriveb2func1"<<endl;
    };

    virtual void vfunc1(void)
    {
        cout<<"CVDrive1B2::vfunc1"<<endl;
    } ;

    virtual void v2func1(void)
    {
        cout<<"CVDrive1B2::v2func1"<<endl;
    } ;
};         //size of CVDrive1B2 is 4

class CVDriveD1: public CVDrive1B1,public CVDrive1B2
{
public:
    void func1(void)
    {
        cout<<"CVDriveD1::func1"<<endl;
    };

    void cdrivedfunc1(void)
    {
        cout<<"CVDriveD1::cdrivedfunc1"<<endl;
    };

    virtual void vfunc1(void)
    {
        cout<<"CVDriveD1::vfunc1"<<endl;
    };

    virtual void v1func1(void)
    {
        cout<<"CVDriveD1::v1func1"<<endl;
    } ;

    virtual void v2func1(void)
    {
        cout<<"CVDriveD1::v2func1"<<endl;
    } ;

    virtual void vd1func1(void)
    {
        cout<<"CVDriveD1::vd1func1"<<endl;
    } ;
};      //size of CVDriveD1 is 8
```
>   接下来，尝试CVDriveD1对象能不能调用祖父类CBase1中的函数vb1func1()
```C++
    CVDriveD1 cvdrived;
    cvdrived.vb1func1();            //输出"CBase1::vb1func1"
    cvdrived.CBase1::vb1func1();    //输出"CBase1::vb1func1"
    cvdrived.base1func1();          //输出"CBase1::vbase1func1"
```
>   由此可以看出，虚继承可以使得菱形继承中的子类能够访问祖先类的函数（包括虚函数和非虚函数，非虚函数后面为何能够调用现在还不知道如何解释）。为什么虚继承能够有如此功能，还是老方法，用gdb打印虚表的关系：
```sh
(gdb) p cvdrived
$1 = {
    <CVDrive1B1> = {
        <CBase1> = {
            _vptr.CBase1 = 0x8049804 <vtable for CVDriveD1+20>
        }, 
        <No data fields>}, 
    <CVDrive1B2> = {
        <No data fields>
        }, 
        <No data fields>
    }
(gdb) p sizeof(cvdrived)
$2 = 8
```
>   这个很奇怪，cvdrived的大小为8，而打印出的虚表指针只有一个，而且只有其中第一个父类CVDrive1B1有虚表指针，第二个父类CVDrive1B2没有，cvdrived也没有其他数据成员。那么cvdrived的另外一个空间存放的是什么数据？继续调试：

```sh
(gdb) p/x (&((int)cvdrived))
$3 = 0xffffce24
(gdb) p/x (&((int)cvdrived))+1
$4 = 0xffffce28
(gdb) p/x *((&((int)cvdrived))+0)
$5 = 0x8049804
(gdb) p/x *((&((int)cvdrived))+1)
$6 = 0x804982c
```
>   cvdrived的第二个存储数据值0x804982c与已知的虚函数表指针值0x8049804相差不大，由此猜测0x804982c也是指针，接下来，按常规调试打印这指针所指向的内容：
```sh
#打印虚表（0x8049804）
(gdb) p (long*)*((long*)0x8049804 + 0)
$10 = (long *) 0x804912c <CVDriveD1::vfunc1()>
(gdb) p (long*)*((long*)0x8049804 + 1)
$11 = (long *) 0x8048d74 <CBase1::vb1func1()>
(gdb) p (long*)*((long*)0x8049804 + 2)
$12 = (long *) 0x8049172 <CVDriveD1::v1func1()>
(gdb) p (long*)*((long*)0x8049804 + 3)
$13 = (long *) 0x80491a2 <CVDriveD1::v2func1()>
(gdb) p (long*)*((long*)0x8049804 + 4)
$14 = (long *) 0x80491d8 <CVDriveD1::vd1func1()>
(gdb) p (long*)*((long*)0x8049804 + 5)
$15 = (long *) 0xfffffffc
#第二个指针（0x804982c）
(gdb) p (long*)*((long*)0x804982c + 0)
$19 = (long *) 0x804916a <non-virtual thunk to CVDriveD1::vfunc1()>
(gdb) p (long*)*((long*)0x804982c + 1)
$20 = (long *) 0x0
```
>   看起来虚继承后的虚函数表的内容被”整理“过，该虚表中只存在被子类重写和没被子类重写的虚函数，并没有像非虚继承那样重复保存两份相同的虚函数指针在不同的虚表上，仔细观察虽然有两个CVDriveD1::vfunc1()函数，但其函数地址不同（分别是0x804912c和0x804916a），而且已经指明其中一个为非虚函数（<non-virtual thunk to CVDriveD1::vfunc1()>）那么CVDriveD1存储的第二个指针的意义是什么，跪求大神们赐教？

>   由此，可以得出CVDriveD1对象的虚函数表结构图~~图图图！~~



## 多态作为参数的限制
<p style="text-indent:2em">
    在非指针的情况下，子类对父类变量的数据复制往往会发生截断，但类中的函数并不占空间，那么在子类对父类变量的复制过程中父类如何选择使用函数。
</p>

---
### 1. 非指针变量的父子类之间的复制不能多态

<p style="text-indent:2em">
    比如有如下定义：
</p>

```c++
class Book
{
public:
    virtual ~Book(){cout<<"this is book destruction."<<endl;};
    virtual void reading(){cout<<"reading a book"<<endl;}
protected:
    uint32_t pageNum;
    char* bookName;
};

class AlgorithmBook:public Book
{
public:
    virtual ~AlgorithmBook(){cout<<"this is algorithm book destruction"<<endl;};
    virtual void reading(){cout<<"reading an algorithm book"<<endl;}
protected:
    uint32_t tuiFunCnt;
};
```
<p style="text-indent:2em">
    尝试运行下面程序：
</p>

```C++
AlgorithmBook algoBook;
Book book = algoBook;       //父类局部
Book* pbook = &algoBook;    //父类指针
algoBook.reading();         //使用AlgorithmBook::reading
book.reading();             //使用Book::reading
pbook->reading();           //使用AlgorithmBook::reading
```
<p style="text-indent:2em">
    可见父类的非指针变量(book)并没有调用到子类的方法(reading)，为什么会这样，考虑下以下两种情形
</p>

* 假设父类非指针变量可以调用子类多态化的函数
<p style="text-indent:2em">
    也就是book只占有Book所存储的空间的情况下能够调用子类的reading，这样在现有的函数功能看起来没问题（只是一个普通打印信息），但如果子类的reading如果定义为如下的函数:
</p>

```C++
void AlgorithmBook::reading()
{
    cout<<"reading an algorithm book"<<endl;
    cout<<"lesson count is "<<this->nTuiFunCnt<<endl;
}
```

<p style="text-indent:2em">
    这样就会导致book的确需要增加子类AlgorithmBook的nTuiFunCnt成员存放空间，这样显然不成立book只占用Book的空间大小的情况，另外无论如何重载复制构造函数或赋值运算符，基类(Book)都没有子类的那个成员，所以这种既不想浪费空间又要调用子类的矛盾成立。
</p>

* 假设父类非指针变量能够全盘复制子类中的所有成员
<p style="text-indent:2em">
    既然book在这种情况下需要子类AlgorithmBook，那么全盘复制不就行了。但是，这实际导致了空间浪费，如果其他子类（比如有一个子类LiteratureBook的对象）也要复制到非指针变量中，那么，在定义基类非指针变量的时候就要分配所有的子类空间，当子类繁多时，基类以此扩大空间的定义，空间占用相当大；而实际上，在单纯地想用基类运作时用不到到这些不需要的资源的可能性很多。
</p>

<p style="text-indent:2em">
    当然像这种父类非指针变量给子类非指针变量赋值或初始化复制也是不可以的，还有子类指针指向父类地址的行为也是会不被允许：
</p>

```C++
Book book;
AlgorithmBook algoBook1 = book; //编译出错，提示不适配的转换
AlgorithmBook algoBook2;
AlgorithmBook* palgoBook;

algoBook2 = book;               //编译出错，提示不适配的转换
palgoBook = &book;              //编译出错，提示指针类型不适配
palgoBook = dynamic_cast<AlgorithmBook*>(&book);    //编译警告，dynamic_cast的转换将不会成功。
```
<p style="text-indent:2em">
    编译器这样处理的原因是，当父类对象要对子类对象赋值或复制时，对于那些子类有而父类没有的子类成员的处理是未知的（保持原样不就行了？这个坑待填）。而子类指针指向父类时，对于后续继续对该子类指针的使用很有可能使用到父类没有的成员，这样导致的结果也是未知的有危险的。
</p>

### 2. 父子类作为函数也不能多态
<p style="text-indent:2em">
    尝试将变量作为普通变量，引用，指针传入函数：
</p>

```C++
int test(Book book, Book& qbook, Book* pbook)
{
    book.reading();
    qbook.reading();
    pbook->reading();
    return 1;
}
```
<p style="text-indent:2em">
    运行程序：
</p>

```C++
AlgorithmBook algoBook;
test(algoBook,algoBook,&algoBook);
```
<p style="text-indent:2em">
    这时候，test函数实际的运行情况是这样：
</p>

```C++
test((Book )algoBook, (AlgorithmBook& )algoBook, (AlgorithmBook* )&algoBook)
{
    ((Book )algoBook).reading();    //使用Book::reading
    algoBook.reading();             //使用AlgorithmBook::reading
    algoBook->reading();            //使用AlgorithmBook::reading
    return 1;
}
```
<p style="text-indent:2em">
    这种函数普通参数不能多态的规则也是遵循上述一样的规则 —— 空间最小化，否则，在多重继承和多重函数调用势必没有足够的资源（堆栈）能够支持其运行。
</p>



## 参考
1. *《深度探索C++对象模型》*
2. *https://blog.csdn.net/SuLiJuan66/article/details/48897867*
3. *http://blog.51cto.com/haoel/124595*