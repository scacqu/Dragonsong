#include "redis_op.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

redisContext *redis_init(char const *ip, int port)
{
	redisContext *handle = NULL;
	if (ip == NULL)
	{
		printf("ip == NULL\n");
		return NULL;
	}

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    handle = redisConnectWithTimeout(ip, port, timeout);
    if (handle == NULL || handle->err) {
        if (handle) {
            printf("Connection error: %s\n", handle->errstr);
            redisFree(handle);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
		return NULL;
    }


	return handle;

}


void redis_free(redisContext *handle)
{
	if (handle != NULL)
            redisFree(handle);
}

int redis_keys_del(redisContext  *handle, char const *key)
{

    redisReply	*reply = NULL;
	int			ret = 0;

	if (handle == NULL || key == NULL )
	{
		printf("func redis_string_set err\n");
		return -1;
	}

    reply = redisCommand(handle,"DEL %s", key);
	if (reply->type != REDIS_REPLY_INTEGER)
	{
		ret = -1;
		goto end;
	}
	if (reply->integer > 0)
	{
		ret = 0;
	}
	else
	{
		ret = -1;
	}

end:
    freeReplyObject(reply);
	return ret;
}
//string

int redis_string_set(redisContext *handle, char const *key, char const *value)
{
    redisReply	*reply = NULL;
	int			ret = 0;

	if (handle == NULL || key == NULL || value == NULL)
	{
		printf("func redis_string_set err\n");
		return -1;
	}

    reply = redisCommand(handle,"SET %s %s", key, value);
	if ( reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"OK")==0 )
	{
		ret = 0;
	}
	else
	{
		ret = -1;
	}

    freeReplyObject(reply);

	return ret;
}


int redis_string_get(redisContext *handle, char const *key, char *value)
{
    redisReply	*reply = NULL;
	int			ret = 0;

	if (handle == NULL || key == NULL || value == NULL)
	{
		printf("func redis_string_set err\n");
		return -1;
	}

    reply = redisCommand(handle,"GET %s", key);
	if (reply->type == REDIS_REPLY_STRING)
	{
		// ok
		ret = 0;
		strcpy(value, reply->str);
	}
	else {
		// err	
		ret = -1;
	}


    freeReplyObject(reply);
	return ret;
}


// hash

int redis_hash_set_value(redisContext *handle, char const *key, char const *field, char const *value )
{
	int		ret = 0;
    redisReply	*reply = NULL;
	if (handle == NULL || key == NULL || field == NULL || value == NULL)
	{
		printf("redis_hash_set_value err\n");
		return -1;
	}
    reply = redisCommand(handle,"HSET %s %s %s", key, field, value);
	if (reply->type != REDIS_REPLY_INTEGER)
	{
		ret = -1;
	}
	else
	{
		ret = 0;
	}




    freeReplyObject(reply);
	return ret;

}

int redis_hash_get_value(redisContext *handle, char const *key, char const *field, char *value /*out*/)
{
	int		ret = 0;
    redisReply	*reply = NULL;
	if (handle == NULL || key == NULL || field == NULL || value == NULL)
	{
		printf("redis_hash_set_value err\n");
		return -1;
	}
    reply = redisCommand(handle,"HGET %s %s", key, field);

	if (reply->type != REDIS_REPLY_STRING)
	{
		ret = -1;
	}
	else
	{
		strcpy(value, reply->str);
		ret = 0;
	}




    freeReplyObject(reply);
	return ret;
}

int redis_hash_get_allfields(redisContext *handle, char const *key, char *out)
{
	int		ret = 0;


	return ret;
}

int redis_hash_get_allvalues(redisContext *handle, char const *key, char *out)
{
	int		ret = 0;


	return ret;
}

int redis_hash_del_file(redisContext *handle, char const *key, char const *field)
{
	int		ret = 0;


	return ret;
}

int redis_hash_del_files(redisContext *handle, char const *key, char const *field)
{
	return 0;
}

