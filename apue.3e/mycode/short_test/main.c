#include "apue.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
int main(int argc, char **argv)
{
	FILE *fp;
	int count;
	float da[10];
	float data[10] = {0.11, 2.12, 3.13, 4.44, 5.55, 3,77, 32.22, 2.554, 2.556, 3.22};
	if ((fp = fopen("array", "a+")) == NULL)
		err_sys("open error");
	if (fwrite(&data[2], sizeof(float), 3, fp) != 3)
		err_sys("fwrite error");
	if ((count = fread(&da, sizeof(float), 10, fp)) != 10)	
		err_sys("fread error");
	printf("%f", da[3]);
	fclose(fp);
	exit(0);
}
