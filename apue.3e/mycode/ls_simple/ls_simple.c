#include "apue.h"
#include <dirent.h>

DIR *dp;
struct dirent *dirp;

int main(int argc, char **argv)
{
	if (argc == 1)
		argv[1] = "./";
	if (argc > 2)
		err_quit("usage: ls directory_name");
	
	if ((dp = opendir(argv[1])) == NULL)
		err_sys("can't open %s", argv[1]);
	while ((dirp = readdir(dp)) != NULL)
		printf("%s\t", dirp->d_name);
	puts("\n");

	closedir(dp);
	exit(0);
}
