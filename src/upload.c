#include "fcgi_config.h"

#include <stdlib.h>

#include <unistd.h>
#include "fcgi_stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "redis_op.h"

extern char **environ;
static int getfilename(char *src, char *out, char *uploadFileName)
{
	char	*p = NULL;	
	char	*ptemp = NULL;
	char	name[100] = {0};

	if (src == NULL || out == NULL)
		return -1;
	p = strstr(src, "filename=");
	if (p == NULL)
		return -1;

	p = p+10;

	ptemp = strchr(p, '\"');
	if (ptemp == NULL)
		return -1;

	strcpy(out, "/home/meine/develop/test/down/");
	//memcpy(name, p, ptemp-p);
	strncpy(name, p, ptemp-p);
	strncpy(uploadFileName, p, ptemp-p);
	strcat(out, name);
	
	return 0;
}

static char * memstr(char *src, char *substr, int len)
{
	int sublen = 0, i;

	if (src == NULL || substr == NULL)
	{
		return NULL;
	}

	sublen = strlen(substr);
	len = len - sublen+1;

	for (i=0; i<len; i++)
	{
		if (*src == *substr)
		{
			if (memcmp(src, substr, sublen) == 0)
			{
				// find
				return src;
			}
		}
		src++;
	}
	
	return NULL;

}

static int getuploadurl(char *filename, char *out)
{
	int ret = 0;

	char	*findstr = "http://192.168.171.139/";
	char	*findstr2 = "http://192.168.171.140/";
	char	*start = NULL, *end = NULL; 
	char	buf[2048] = {0};
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
				return -1;
			}
		}

		end = strchr(start, '\n');
		if (end == NULL)
		{
			printf("not find \n");
			return -1;
		}
		strncpy(out, start, end-start);

	}

	return 0;

}


static int savetoRedis(char const *filename, char const *fileurl)
{
	int		ret = 0;
	char			buf[1000] = {0};
	redisContext	*handle = NULL;

	handle = redis_init("127.0.0.1", 6379);
	if (handle == NULL)
	{
		printf("conn redis err\n");
		return -1;
	}

	ret = redis_hash_set_value(handle, "uploadfile", filename, fileurl );
	if (ret != 0)
	{
		printf("func redis_hash_set_value err\n");
		return -1;
	}
	else
	{
		//printf("redis_hash_set_value ok  hash  set ok\n");
	}

	redis_free(handle);

	return ret;
}
int main ()
{
	void	*len_buf = NULL;
	char	filename[100]={0};
	char	uploadname[100] = {0};
	char	*begin = NULL, *end = NULL, *temp = NULL;
	char	substr[1024] = {0};
	char	*p = NULL;
	int		i, filelen, ret = 0;
	int		ch, leftlen;

	char	fileurl[200] = {0};

	char **initialEnv = environ;
	int count = 0;

	while (FCGI_Accept() >= 0) {
		char *contentLength = getenv("CONTENT_LENGTH");
		int len;

		if (contentLength != NULL) 
		{
			len = strtol(contentLength, NULL, 10);
		}
		else 
		{
			len = 0;
		}

		if (len <= 0)
		{
			printf("No data from standard input.<p>\n");
		}
		else 
		{
			printf("Content-type: text/html\r\n"
					"\r\n"
					"<title>上传文件</title>"
					"<h1>upload</h1>\n"
					);

			len_buf = malloc(len);
			p = len_buf;

			if (len_buf == NULL)
			{
				printf("malloc err\n");
				break;
			}
			memset(len_buf, 0, len);

			for (i=0; i<len; i++)
			{
				if ( (ch = getchar()) < 0)
				{
                    printf("Error: Not enough bytes received on standard input<p>\n");
					continue;
				}
				p[i] = ch;
			}

			begin = p;

			temp = strstr(begin, "\r\n");
			if (temp == NULL)
			{
                printf("Error: strstr(begin, \r\n)<p>\n");
				goto end;
			}
			// 获取子串
			strncpy(substr, begin, temp-begin);

			temp+=2;
			// 解析上传的文件名
			ret = getfilename(temp, filename, uploadname);
			if (ret != 0)
			{
				printf("getfilename err\r\n");
				goto end;
			}

			temp = strstr(temp, "\r\n");
			if (temp == NULL)
			{
                printf("Error: strstr(temp, \r\n)\n");
				goto end;
			}
			temp+=2;

			temp = strstr(temp, "\r\n");
			if (temp == NULL)
			{
                printf("Error: strstr(temp, \r\n)<p>\n");
				goto end;
			}
			temp+=4;

			begin = temp;   //      文件开始指针


			leftlen = len - (begin-p);



			end = memstr(begin, substr, leftlen);
			if (end == NULL)
			{
                printf("Error: strstr(begin, substr)<p>\n");
				printf("%s\n", substr);
				for (i=0; i<len; i++)
				{
					putchar(p[i]);
				}
				goto end;
			}
			end-=3;
			filelen = end - begin + 1;

			// 文件保存
			umask(0);
			int fd = open(filename, O_RDWR | O_CREAT, 0664);
			if (fd < 0)
			{
                printf("Error: open(filename, O_RDWR | O_CREAT, 0664)<p>\n");
				goto end;
			}

			write(fd, begin, filelen);
			close(fd);
			printf("文件保存成功\n");
			printf("文件名为====>%s\n", filename);

			
			//将文件上传到fastdfs

			if ( getuploadurl(filename, fileurl) != 0 )
			{
				printf("getuploadurl() err\n");
				goto end;
			}

			printf("上传文件成功");
			printf("上传文件url====>%s\n", fileurl);



			//将文件名和返回的文件存储地址存入数据库中

			if (savetoRedis(filename, fileurl) != 0)
			{
				printf("savetoRedis() err\n");	
				goto end;
			}
			printf("文件名与url保存到redis成功\n");

end:
			memset(filename, 0, sizeof(filename));
			memset(fileurl, 0, sizeof(fileurl));
			memset(substr, 0, sizeof(substr));

			free(len_buf);
			len_buf = NULL;

		}
				

	}

	return 0;
}
