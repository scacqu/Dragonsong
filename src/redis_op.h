#ifndef _REDIS_OP_H
#define _REDIS_OP_H

#include <hiredis.h>



redisContext *redis_init(char const *ip, int port);

int redis_keys_del(redisContext  *handle, char const *key);

void redis_free(redisContext *handle);

//string
int redis_string_set(redisContext*handle, char const *key, char const *value);

int redis_string_get(redisContext  *handle, char const *key, char *value);

int redis_string_getlen(redisContext *handle, char const *key, int *len);

// hash 
int redis_hash_set_value(redisContext *handle, char const *key, char const *field, char const *value );

int redis_hash_get_value(redisContext *handle, char const *key, char const *field, char *value /*out*/);

int redis_hash_get_allfields(redisContext *handle, char const *key, char *out);

int redis_hash_get_allvalues(redisContext *handle, char const *key, char *out);

int redis_hash_del_file(redisContext *handle, char const *key, char const *field);

int redis_hash_del_files(redisContext *handle, char const *key, char const *field);







#endif
