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
<table>
  <tr>
    <th>参数</th>
    <th>详细解释</th>
  </tr>
  <tr>
    <th> caddr_t </th>
    <th> 内存地址 </th>
  </tr>
  <tr>
    <th>clock_t </th>
    <th> 时钟滴答计数器（进程时间） </th>
  <tr>
    <th>comp_t </th>
    <th> 压缩的时钟滴答 </th>
  </tr>
  <tr>
    <th>dev_t</th>
    <th>设备号(主和次)</th>
    </tr>
  <tr>
    <th>fpos_t</th>
    <th>文件位置</th>
  </tr>
  <tr>
    <th>gid_t</th>
    <th>数值组ID</th>
  </tr>
  <tr>
    <th>ino_t</th>
    <th>i节点编号</th>
  </tr>
  <tr>
    <th>mode_t</th>
    <th> 文件类型，文件创建方式
</th>
  </tr>
  <tr>
    <th>nlink_t</th>
    <th>目录项的连接计数</th>
  </tr>
  <tr>
    <th>off_t</th>
    <th> 文件长度和位移量(带符号的)(lseek)
</th>
  </tr>
  <tr>
    <th>pid_t</th>
    <th> 进程ID和进程组ID(带符号的)
</th>
  </tr>
  <tr>
    <th>ptrdiff_t</th>
    <th> 两个指针相减的结果(带符号的)
</th>
  </tr>
  <tr>
    <th>rlim_t</th>
    <th>资源限制</th>
  </tr>
  <tr>
    <th>sigatomic_t</th>
    <th>能原子地存取的数据类型</th>
  </tr>
  <tr>
    <th>sigset_t</th>
    <th>信号集</th>
  </tr>
  <tr>
    <th>size_t</th>
    <th> 对象(例如字符串)长度(不带符号的)
</th>
  </tr>
  <tr>
    <th>ssize_t</th>
    <th>返回字节计数的函数(带符号的)(read, write)</th>
  </tr>
  <tr>
    <th>time_t</th>
    <th>日历时间的秒计数器</th>
  </tr>
  <tr>
    <th>uid_t</th>
    <th>数值用户ID</th>
  </tr>
  <tr>
    <th>wchar_t</th>
    <th>能表示所有不同的字符码</th>
  </tr>
</table>

## 第三章 文件I/O
### 3.3  函数 open和 openat
*Page* 50
