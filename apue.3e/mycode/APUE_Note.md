# UNIX环境系统编程
## 第一章 UNIX基础知识
### 1.4文件和目录
*Page 5*
* **异常退出分类**
	1. err_quit(): 用户层面异常
	2. err_sys(): 系统函数调用失败

### 1.5输入和输出
*Page* 7
**不带缓冲的**I/O: open, read, write, lseek, close都使用文件描述符

### 1.6程序和进程
*Page* 11
一个进程内的所有线程共享同一地址空间、文件描述符、栈、以及与进程相关的属性。因为它们能访问同一存储区，所以各线程**在访问共享数据时**需要采取同步措施以避免不一致性。
* **线程ID只在所属进程内有意义**

### 1.7出错处理
*Page* 11
* **errno规则**
	1. 如果没有出错， 其值不会被例程清除
	2. 任何函数不会将其设置为0

```C
#include <string.h>
char *strerror(int errnum);
// 返回出错消息字符串
```
```C
#include <stdio.h>
void perror(const char *msg);
//直接输出msg：出错消息
```
* **标准UNIX惯例**
	直接将程序名argv[0]传给perror

## 第二章 UNIX标准及实现
### 2.8基本系统数据类型
*Page* 47

## 第三章 文件I/O
[Linux 中直接 I/O 机制的介绍](https://www.ibm.com/developerworks/cn/linux/l-cn-directio/)
### 3.3  函数 open和 openat
*Page* 50
```C
#include <fcntl.h>
int open(const char *path, int oflag,.../*mode_t mode*/);
int openat(int dirfd, const char *path, int oflag,.../*mode_t mode*/);
/*
两函数返回值：若成功返回文件描述符;若出错， 返回-1
openat要求事先获取目录描述符，再传入， 如：使用opendir将返回的指针用dirfd转成目录描述符或直接用open打开目录
*/
```
* openat**优点**：
	* 同一进程中的所有线程共享相同的当前工作目录，可用openat让多个不同的线程**同时在不同目录工作**
	* 防止基于文件的两个函数调用，第二个依赖第一个结果时，在调用间隙文件可能被改变(TOCTTOU错误)
* **oflag**参数中和**读写同步相关的**：
	**1. O_SYNC**: 
    *每次write等待物理I/O操作完成*
	**2. O_DSYNC**: 
    *每次write等待物理I/O操作完成，但如果写操作不影响读取刚写入的数据，则不需等待文件属性被更新*
	**3. O_RSYNC**:
    *使每一个以文件描述符作为参数进行的read操作等待，直至所有对文件同一部分挂起的写操作都完成*
### 3.4函数 creat
*Page* 52
create函数** 只能创建只写文件 **
```C
#include <fcntl.h>
int creat(const chat *path, mode_t mode);
//返回值：若成功，返回为只写打开的文件描述符；若出错，返回-1
//可用open代替
```
### 3.5函数 close
*Page* 53
1. 关闭文件会自动释放进程加在文件上的所有记录锁
2. 进程终止时内核自动关闭打开的所有文件
### 3.6 函数 lseek
*Page* 53
1. 通常读写操作都从当前文件偏移量开始，并偏移增加读写的字节数
2. 除非指定**O_APPEND**选项，否则打开文件时偏移量都为0
```C
#include <unistd.h>
off_t lseek(int fd, off_t offset, int whence);
//返回值：若成功，返回新的文件偏移量；若出错，返回为-1
```
* 若whence是SEEK_SET，则偏移量是距文件开始处offset字节
* 若whence是SEEK_CUR，则偏移量是当前值增加offset字节（可正可负）	
> 可用SEEK_CUR**确定当前偏移量**
* 若whence是SEEK_END，则偏移量是文件长度加offset字节（可正可负）

### 3.7 函数read
*Page* 57
```C
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t nbytes);
//返回值：读到的字节数，若已到文件尾，返回0若出错,返回-1
```
* 返回字节数少于所要求的情况：
	* 网络的缓冲机制可能造成
	* 从管道或FIFO读
	* 从某些面向记录的设备（如磁带）读，一次最多返回一个记录
	* 当信号造成中断，已经读了部分数据
	* 从终端设备一次最多一行（可变）
	* 读普通文件提前到达文尾
	
### 3.8 函数 write
```C
#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t nbytes);
//返回值： 成功，返回已写的字节数;出错，返回-1
```
*  一般返回值和参数nbytes相同， 否则出错，如磁盘满或超出文件长度限制

### 3.9 I/O的效率
*Page* 59
带缓冲的I/O利用了预读技术，效率更高
一般缓冲区设置为磁盘块长度， 由st_blksize表示
### 3.10 文件共享
*Page* 60~61
不同进程有**相同或不同的进程表项和文件表项**，即不同的进程中的不同文件描述符对应不同的文件表项(不同的偏移量和状态标志)，但是可能指向**同一个V节点表项**(linux中与文件系统无关的通用i节点结构)
[书中图3-8]
### 3.11 文件读写的原子操作
*Page* 62
#### 1. 追加文件
使用O_APPEND标志的写操作将*定位到尾部*和*写*封装成原子操作，避免了其他进程在该进程尾部后改变
#### 2. 函数pread和pwrite
```C
#include <unistd.h>
ssize_t pread(int fd, void *buf, ssize_t nbytes, off_t offset);
//返回值：读到的字节数，若到文尾，返回0;若出错，返回-1
ssize_t pwrite(int fd, const *buf, size_t nbytes, off_t offset);
//返回值：若成功，返回已写的字节数；若出错，返回-1
```
* 调用pread时，法中断定位和读操作
* 不更新当前文件偏移量
#### 3. 创建一个文件
使用**O_CREAT**和**O_EXCL**创建文件是原子操作，如果存在则open失败
当文件存在，errno == EEXIST
#### 3.12 函数dup和dup2
*Page* 63
![c](https://raw.githubusercontent.com/tyhser/APUE_Note/master/%E6%96%87%E4%BB%B6%E6%8F%8F%E8%BF%B0%E7%AC%A6%2C%E6%89%93%E5%BC%80%E7%9A%84%E6%96%87%E4%BB%B6%E5%8F%A5%E6%9F%84%E4%BB%A5%E5%8F%8Ai-node%E4%B9%8B%E9%97%B4%E7%9A%84%E5%85%B3%E7%B3%BB.png)
* 由于进程级文件描述符表的存在，不同的进程中会出现相同的文件描述符，它们可能指向同一个文件，也可能指向不同的文件
* 两个不同的文件描述符，若指向同一个打开文件句柄(系统维护)，将共享同一文件偏移量。因此，如果通过其中一个文件描述符来修改文件偏移量（由调用read()、write()或lseek()所致），那么从另一个描述符中也会观察到变化，无论这两个文件描述符是否属于不同进程，还是同一个进程，情况都是如此。
* 要获取和修改打开的文件标志（例如：O_APPEND、O_NONBLOCK和O_ASYNC），可执行fcntl()的F_GETFL和F_SETFL操作，其对作用域的约束与上一条颇为类似。
* 文件描述符标志（即，close-on-exec）为进程和文件描述符所私有。对这一标志的修改将不会影响同一进程或不同进程中的其他文件描述符
-------------------------------------
```C
#include <unistd.h>
int dup(int fd);
int dup2(int fd, int fd2);
//两函数的返回值：若成功，返回新的文件描述符若出错，返回-1
//用于复制现有描述符
```
**复制后的文件描述符指向同一个文件表项，所以共享文件状态标志和偏移量**

### 3.13 函数sync、fsync、fdatasync
*Page* 65
* 保证实际文件内容和缓冲区内容一致的函数
```C
#include <unistd.h>
int fsync(int fd);		//更新数据部分和文件属性
int fdatasync(int fd);	//只影响数据部分，不更新文件属性
//以上两函数等待磁盘操作结束才返回
//返回值：若成功，返回0;若出错，返回-1
void sync(void);//只将所有修改过的块缓冲区排入写队列，然后返回，不等待磁盘操作
```
### 3.14 函数fcntl
*Page* 65
* 用于改变已经打开的文件的属性
```C
#include <fcntl.h>
int fcntl(int fd, int cmd, .../*int arg*/);
//返回值：若成功，则依赖于cmd;若出错，返回-1
```
* fcntl函数有5种功能：
	* 复制一个已有的描述符(cmd = F_DUPFD 或F_DUPFD_CLOEXEC)
	* 获取/设置文件描述符标志(cmd = F_GETFD 或 F_SETFD)
	* 获取/设置文件状态标志(cmd = F_GETFL 或 F_SETFL)
	* 获取/设置异步I/O所有权(cmd = F_GETOWN 或 F_SETOWN)
	* 获取/设置记录锁(cmd = F_GETLK、F_SETLK、F_SETLKW)
 要取得文件访问方式位<值为0,1,2>(不包括O_EXEC)返回值必须要**和O_ACCMODE<0x0003>位与操作后**再用switch比较出结果，因为需要屏蔽其他位
### 3.15 函数ioctl
*Page* 70
 ```C
 #include <unistd.h>	/*System V*/
 #include <sys/ioctl.h>	/*BSD and Linux*/
 int ioctl(int fd, int request, ...);
 //返回值：若出错，返回-1;若成功，返回其他值
 ```
### 3.16 /dev/fd
把文件描述符映射成底层物理文件的符号链接，打开相当于复制文件描述符
## 第四章 文件和目录
### 4.2 函数stat、fstat、fstatat、lstat
* 返回文件信息
```C
#include <sys/stat.h>
int stat(const char *restrict pathname, struct *restrict buf);
int fstat(int fd, struct stat *buf);
int lstat(const char *restrict pathname, struct stat *restrict buf);
int fstatat(int dirfd, const char *restrict pathname, struct stat *restrict buf, int flag);
//所有四个函数的返回值：若成功；返回0；若出错，返回-1
```
* 对fstatat：
	- 1: 如果pathname是相对路径，则是相对于dirfd的路径，不是相对于当前进程工作空间。
	- 2: 如果pathname是相对路径，且dirfd为AT_FDCWD，则是相对于当前进程工作空间。
	- 3: 如果pathname是绝对路径，则dirfd会被忽略。
第四个参数flags可以取以下值:
AT_EMPTY_PATH: pathname可以为空，此时获得的是dirfd所指向文件的信息。
AT_NO_AUTOMOUNT:和挂载点有关，允许手机挂载点的属性。
AT_SYMLINK_NOFOLLOW:如果path指向一个符号链接，则返回该链接信息。默认情况下，fstatat返回的就是链接信息。
### 4.3 文件类型
* Page* 76
1. 使用S_ISREG()、S_ISDIR()系列宏**传入stat结构中的st_mode**判断类型
2. 使用S_TYPEISMQ()、S_TYPEISSEM()系列宏**传入stat结构指针**判断是否是消息队列信号量等文件

### 4.4 设置用户ID和组ID
*Page* 78
* 通常，有效用户ID等于实际用户ID，有效组ID等于实际组ID
* st_mode中可用常量S_ISUID和S_ISGID测试
### 4.5 文件访问权限
1. 目录的可执行权限意味着目录可访问
2. 在open函数中指定O_TRUNC标志必须对文件具有写权限
3. 在目录中创建文件必须对该目录有写权限和可执行权限
4. 删除现有文件必须对该文件的目录有写权限和可执行权限
5. 如果用7个exec函数中的任何一个执行某文件， 都必须对该文件有可执行权限
### 4.6 新文件的目录的所有权

### 4.7 函数access和faccessat
*Page* 81
按照实际用户ID和实际组ID进行**访问权限测试**
```C
#include <unistd.h>
int access(const char *pathname, int mode);
int faccessat(int fd, const char *pathname, int mode, int flag);
//两个函数的返回值：若成功， 返回0;若出错，返回-1
```
### 4.8 函数umask
*Page* 83
```C
#include <sys/stat.h>
mode_t umask(mode_t cmask);
//返回值：之前文件模式创建屏蔽字
```
### 4.12 文件长度
*Page* 89
stat结构成员st_size表示
### 4.13 文件截断
```C
#include <unistd.h>
int truncate(const *pathname, off_t length);
int ftrincate(int fd, off_t length);
```
对打开的或现有的文件截断， 如果现有文件短与目标值，增加空洞
### 4.14 文件系统
![磁盘与文件系统](https://www.ibm.com/developerworks/cn/linux/l-cn-vfs/4.jpg)
创建硬链接就是增加指向该i节点的目录项，增加i节点链接计数等工作
### 4.15 创建和解除文件链接的函数
*Page* 93
### 4.17 符号链接
符号链接存在跟随符号链接和不跟随的问题
### 4.19 文件时间
*Page* 100
修改时间：st_mtim
状态更改时间：st_ctim
## 第五章 标准I/O库
### 5.2 流和FILE对象
**fwide设置流的定向**
```C
#include <stdio.h>
#include <wchar.h>
int fwide(FILE *fp, int mode);
//返回值：若流是宽定向的，返回正值;若流是字节定向的，返回负值;若流是未定向的，返回0
```
* 若mode参数值为负，fwide将试图使指定的流是字节定向的
* 若mode参数值为正，fwide将试图使指定的流是宽定向的
* 若mode参数值为正，fwide将不试图设置流的定向，但返回标识该流定向的值
**fwide并不改变已定向流的定向，且fwide无出错返回，可在调用前清除errno值再检查errno的值**
### 5.3 标准输入、标准输出、标准错误
<stdio.h>定义了预定义文件指针
### 5.4 缓冲
```C
#include <stdio.h>
void setbuf(FILE *restrict fp, char *restrict buf);
void setvbuf(FILE *restrict fp, char *restrict buf, int mode, size_t size);
//返回值：若成功，返回0;若出错，返回非0
```
* 设置全缓冲、行缓冲、不带缓冲
* 用fflush冲洗流
### 5.5 打开流
```C
#include <stdio.h>
FILE *fopen(const char *restrict pathname, const *restrict type);
FILE freopen(const char *restrict pathname, const *restrict type, FILE *restrict fp);
FILE *fdopen(int fd, const char *type);
//三个函数返回值：若成功，返回文件指针;若出错，返回NULL
```
* fdopen使标准i/o流和文件描述符相结合
** 如果中间没有fflush、fseek、fsetpos、rewind，则在输出后面不能直接跟随输入**
** 如果中间没有fdeek、fsetpos、rewind，或者输入操作没有到达文尾，则在输入操作之后不能直接跟随输出**
### 5.6 读和写流
*Page* 121
每个流在FILE对象维护了两个标志：
* 出错标志;
* 文件结束标志
调用ferrror或feof验证是否出错或结束
ungetc将字符压入流中（写回标准I/O缓冲区中）
### 5.9 二进制I/O
```C
#include <stdio.h>
size_t fread(void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);
size_t fwrite(const void *restrict ptr, size_t size, size_t nobj, FILE *restrict fp);
//两函数的返回值：读或写的对象数
```
