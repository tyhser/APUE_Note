#include "apue.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#define ONE_MB (1024 * 1024)
#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
int fd1, fd2, fd3, nr;
void fun_open_test(void)
{
	int fd;
	if ((fd = open("a.txt", O_CREAT|O_EXCL, 0777)) < 0)
	{
		err_sys("open error");
		if (errno == EEXIST)
			printf("then ....");
	}
	else
		printf("created a.txt\n");
}
void fork_fd_test(void)
{
	char buff[20];
	pid_t pid;
	fd1 = open("data.in", O_RDWR);
	pid = fork();
	if (pid == 0)
	{
		nr = read(fd1, buff, 10);
		buff[nr] = '\0';
		printf("pid:%d content: %s\n", getpid(), buff);
		exit(0);
	}

	nr = read(fd1, buff,10);
	buff[nr] = '\0';
	printf("pid:%d content: %s\n", getpid(), buff);
}
void stat_test(char *filename)
{
	struct stat buf;
	if (stat(filename, &buf) < 0)
		err_quit("stat");;

}
void umask_test(void)
{
	umask(0);
	if (creat("foo", RWRWRW) < 0)
		err_sys("creat error for foo");
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (creat("bar", RWRWRW) < 0)
		err_sys("creat error for bar");
}

void link_test(void)
{
	if (open("tem", O_RDWR) < 0)
		err_sys("open error");
	if (unlink("tem") < 0)
		err_sys("unlink error");
	printf("file unlinked\n");
	sleep(15);
	printf("done\n");
	exit(0);
}
int main(int argc, char **argv)
{
	int val;
	long num_procs;  
   	long page_size;  
	long num_pages;  
	long free_pages;  
    	long long mem;  
    	long long free_mem;  
    	num_procs = sysconf (_SC_NPROCESSORS_CONF);  
    	printf ("CPU 个数为: %ld 个\n", num_procs);  
    	page_size = sysconf (_SC_PAGESIZE);  
    	printf ("系统页面的大小为: %ld K\n", page_size / 1024 );  
    	num_pages = sysconf (_SC_PHYS_PAGES);  
    	printf ("系统中物理页数个数: %ld 个\n", num_pages);  
    	free_pages = sysconf (_SC_AVPHYS_PAGES);  
    	printf ("系统中可用的页面个数为: %ld 个\n", free_pages);  
    	mem = (long long) ((long long)num_pages * (long long)page_size);  
    	mem /= ONE_MB;  
    	free_mem = (long long)free_pages * (long long)page_size;  
    	free_mem /= ONE_MB;  
    	printf ("总共有 %lld MB 的物理内存, 空闲的物理内存有: %lld MB\n", mem, free_mem);  
	link_test();
	exit(0);
}
