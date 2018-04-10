[Toc]
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
* 同一进程中的所有线程共享相同的当前工作目录，可用openat让多个不同的线程**同时在不同目录工作**
* 基于文件的两个函数调用，第二个依赖第一个结果时，在调用间隙文件可能被改变(TOCTTOU错误)
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
### 3.6函数 lseek
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

