#include "redis_op.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void)
{
	int				ret = 0;
	char			buf[1000] = {0};
	redisContext	*handle = NULL;

	handle = redis_init("127.0.0.1", 6379);
	if (handle == NULL)
	{
		printf("conn redis err\n");
		return -1;
	}
	

	ret = redis_hash_set_value(handle, "redisapi", "test", "hehehe" );
	if (ret != 0)
	{
		printf("func redis_hash_set_value err\n");
		return -1;
	}
	else
	{
		printf("redis_hash_set_value ok  hash  set ok\n");
	}

	ret = redis_hash_get_value(handle, "redisapi", "test", buf);
	if (ret != 0)
	{
		printf("func redis_hash_get_value err\n");
		return -1;
	}
	else
	{
		printf("get value ok\n");
		printf("test=====>%s\n", buf);
	}


	redis_free(handle);

	return 0;
}

int main1234(void)
{

	int				ret = 0;
	char			buf[1000] = {0};
	redisContext	*handle = NULL;


	handle = redis_init("127.0.0.1", 6379);
	if (handle == NULL)
	{
		printf("conn redis err\n");
		return -1;
	}

	ret = redis_string_set(handle, "foo", "bar");
	if (ret != 0)
	{
		printf("func redis_string_set() err\n");
	}
	printf("set ok\n");

	ret = redis_string_get(handle, "foo", buf);
	if (ret != 0)
	{
		printf("func redis_string_set() err\n");
	}
	printf("get ok..\n");
	printf("key foo ----> %s\n", buf);

	ret = redis_keys_del(handle, "foo");
	if (ret != 0)
	{
		printf("del err\n");
	}
	else
		printf("del ok\n");


	redis_free(handle);



}
