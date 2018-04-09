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
>caddr_t 内存地址（ 1 2 . 9节）
clock_t 时钟滴答计数器（进程时间）
comp_t 压缩的时钟滴答
dev_t 设备号（主和次）
fdse_t 文件描述符集
fpos_t 文件位置
gid_t 数值组ID
ino_t i节点编号
mode_t 文件类型，文件创建方式
nlink_t 目录项的连接计数
off_t 文件长度和位移量（带符号的）（lseek）
pid_t 进程I D和进程组I D（带符号的）
ptrdiff_t 两个指针相减的结果（带符号的）
rlim_t 资源限制
sigatomic_t 能原子地存取的数据类型
sigset_t 信号集
size_t 对象（例如字符串）长度（不带符号的）
ssize_t 返回字节计数的函数（带符号的）（read, write）
time_t 日历时间的秒计数器
uid_t 数值用户ID
> wchar_t 能表示所有不同的字符码
