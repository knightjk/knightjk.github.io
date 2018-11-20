
---
title: 关于cache和buffer
---

cache<-磁盘，在磁盘读取时会有预读操作来预判提高磁盘读取效率
buffer->磁盘，在磁盘写入时会产生缓存空间(这是对SSD、磁盘写入放大的一种对应措施，因为写入时需要擦除，擦除的大小并不会是任意擦除而是按扇区或块擦除，因此在写入前会读出原有数据然后再擦除后写入包括原来读出的所有数据)，当磁盘写入速度慢时利用这部分空间存储临时数据。
    由于buffer的上述机制，使得在数据流输入到磁盘或者在终端(终端也有键盘的输入缓冲区)的过程有一些延迟，而导致buffer中的数据冲刷到目标上(磁盘或终端)的条件有以下几种：
    1.程序正常结束（如果程序崩溃时输出缓存不会更新）
    2.缓冲区满
    3.结束操作符，如‘\n’,endl等
    4.C++中使用unitbuf来设置流的内部状态
    5.清缓存，如fflush
    6.C++相关联流被读写时。
    buffer包括目录项缓存(描述文件系统路径名的目录项对象)，索引节点缓存(描述磁盘索引节点的索引节点对象)，页高速缓存(完整的数据页对象)。
释放缓存：
sync     #同步数据以防丢失
To free pagecache:
echo 1 > /proc/sys/vm/drop_caches
To free dentries and inodes:
echo 2 > /proc/sys/vm/drop_caches
To free pagecache, dentries and inodes:
echo 3 > /proc/sys/vm/drop_caches





