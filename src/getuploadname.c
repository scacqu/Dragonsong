#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{

	char	*filename = "./getuploadname.c";

	char	*findstr = "example file url: http://192.168.171.139/";
	char	*findstr2 = "example file url: http://192.168.171.140/";
	int		findlen;
	char	*start = NULL, *end = NULL; 
	char	buf[2048] = {0};
	char	uploadurl[200] = {0};
	ssize_t	flen;
	pid_t	pid;

	int		fds[2];

	pipe(fds);
	pid = fork();
	
	if (pid < 0)
	{
		perror("fork()");
		return -1;
	}
	else if (pid == 0)
	{
		// child
		// 将标准输出从定向到fds[1]	
		close(fds[0]);
		if (dup2(fds[1], STDOUT_FILENO) < 0 )
		{
			perror("dup2");	
			exit(1);
		}
		dup2(fds[1], STDERR_FILENO);
		execlp("fdfs_test", "fdfs_test", "/etc/fdfs/client.conf", "upload", filename, 0);

	}
	else
	{ // parent
		close(fds[1]);
		if (wait(NULL) != pid )
			perror("waitpid()");

		flen = read(fds[0], buf, sizeof(buf));
		start = strstr(buf, findstr);
		if (start == NULL)
		{
			start = strstr(buf, findstr2);
			if (start == NULL)
			{
				printf("not find\n");
				exit(1);
			}
		}
		findlen = strlen(findstr);
		start += findlen;

		end = strchr(start, '\n');
		if (end == NULL)
		{
			printf("not find \n");
			exit(0);
		}
		strncpy(uploadurl, start, end-start);

		printf("获取上传文件url成功\n");
		printf("uploadurl = %s\n", uploadurl);
	}

	return 0;
}
