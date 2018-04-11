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

#### 3.13 函数sync、fsync、fdatasync
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
#### 3.14 函数fcntl
*Page* 65
* 用于改变已经打开的文件的属性
```C
#include <fcntl.h>
int fcntl(int fd, int cmd, .../*int arg*/);
//返回值：若成功，则依赖于cmd;若出错，返回-1
```
