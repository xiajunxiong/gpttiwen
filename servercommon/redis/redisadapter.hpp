#ifndef __REDIS_ADAPTER_HPP__
#define __REDIS_ADAPTER_HPP__

#include "hiredis/hiredis.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstring>

#ifdef _MSC_VER
#define strcasecmp stricmp
#define strncasecmp strnicmp
#define usleep(us)        Sleep((us)/1000)
#define pthread_self()    GetCurrentThreadId()
#endif

#define REDIS_REPLY_STRING 1
#define REDIS_REPLY_ARRAY 2
#define REDIS_REPLY_INTEGER 3
#define REDIS_REPLY_NIL 4
#define REDIS_REPLY_STATUS 5
#define REDIS_REPLY_ERROR 6

#define MAX_ERR_STR_LEN 128

typedef std::string             KEY;
typedef std::string				FIELD;
typedef std::string             VALUE;
typedef std::vector<KEY>        KEYS;
typedef KEYS                    FIELDS;
typedef std::vector<VALUE>      VALUES;
typedef std::vector<std::string>     VDATA;
typedef std::set<std::string>        SETDATA;
typedef std::pair<double, std::string> ScoreMember;

typedef redisContext RedisContext;
typedef redisReply RedisReply;

class RedisAdapter
{
public:
	RedisAdapter();
	~RedisAdapter();

	void Init(const std::string&ip, int port, const std::string& auth_str, int connect_timeout_ms);

	bool GET(const KEY& key, VALUE& value);
	bool SET(const KEY& key, const VALUE& value);
	bool GETSET(const KEY& key, const VALUE& new_value, VALUE& old_value);
	bool SETEX(const KEY& key, const VALUE& value, long long expire_seconds);
	bool PSETEX(const KEY& key, const VALUE& value, long long expire_milliseconds);

	bool MGET(const KEYS& keys, VDATA& vdata);
	bool MSET(const KEYS& keys, const VDATA& vdata);

	bool INCR(const KEY& key, long long& value);
	bool INCRBY(const KEY& key, long long incrby, long long& value);
	bool DECR(const KEY& key, long long& value);
	bool DECRBY(const KEY& key, long long decrby, long long& value);

	bool DEL(const KEY& key);
	bool DEL(const KEYS& keys);

	bool EXISTS(const KEY& key);
	bool EXPIRE(const KEY& key, long long expire_seconds);
	bool PEXPIRE(const KEY& key, long long expire_milliseconds);
	bool EXPIREAT(const KEY& key, long long expire_at_timestamp);
	bool PEXPIREAT(const KEY& key, long long expire_at_timestamp_ms);

	bool RANDOMKEY(KEY& key);

	bool HGET(const KEY& key, const FIELD& field, VALUE& value);
	bool HSET(const KEY& key, const FIELD& field, const VALUE& value, long long& ret_value);
	bool HMGET(const KEY& key, const FIELDS& fields, VDATA& vdata);
	bool HMSET(const KEY& key, const FIELDS& fields, const VDATA& vdata);
	bool HGETALL(const KEY& key, VDATA& vdata);
	bool HSETNX(const KEY& key, const FIELD& field, VALUE& value, long long& ret_value);
	bool HDEL(const KEY& key, const FIELD& field, long long& ret_value);
	bool HDEL(const KEY& key, const FIELDS& fields, long long& ret_value);

	bool LPUSH(const KEY& key, const VALUE& value, long long& list_length);
	bool LPUSH(const KEY& key, const VALUES& values, long long& list_length);
	bool LPOP(const KEY& key, VALUE& value);
	bool LLEN(const KEY& key, long long& list_length);
	bool LRANGE(const KEY& key, int start_index, int end_index, VDATA& values);
	bool LREM(const KEY& key, const VALUE& value, int count, long long& remove_num);
	bool LSET(const KEY& key, int index, const VALUE& value);
	bool LTRIM(const KEY& key, int start_index, int end_index);
	bool RPOP(const KEY& key, VALUE& value);
	bool RPUSH(const KEY& key, const VALUE& value, long long& list_length);
	bool RPUSH(const KEY& key, const VALUES& values, long long& list_length);

	bool BLPOP(const KEY& key, VDATA& vValues, unsigned int timeout_s);
	bool BRPOP(const KEY& key, VDATA& vValues, unsigned int timeout_s);

	bool SADD(const KEY& key, int value_num, const VALUE* values, long long& ret_value);
	bool SCARD(const KEY& key, long long& ret_value);
	bool SISMEMBER(const KEY& key, const VALUE& member, bool& ismember);
	bool SMEMBERS(const KEY& key, VDATA& vValues);
	bool SPOP(const KEY& key, VDATA& list);
	bool SRANDMEMBER(const KEY& key, int count, VALUES& values);
	bool SREM(const KEY& key, int value_num, const VALUE* values, long long& ret_value);

	bool ZADD(const KEY& key, int member_num, ScoreMember* member_list, long long& newly_add_num);
	bool ZADD(const KEY& key, bool update_on_exist, bool return_updated_member_num, int member_num, ScoreMember* member_list, long long& ret_value);
	bool ZCARD(const KEY& key, long long& ret_value);
	bool ZINCRBY(const KEY& key, double inc_score, const VALUE& member, std::string& member_new_score);
	bool ZPOPMAX(const KEY& key, int count, VDATA& vValues);
	bool ZPOPMIN(const KEY& key, int count, VDATA& vValues);
	bool ZRANGE(const KEY& key, int start_index, int stop_index, bool with_scores, VDATA& vValues);
	bool ZRANGEBYSCORE(const KEY& key, double min_score, double max_score, bool with_scores, int offset, int count, VDATA& vValues, bool min_score_open_interval = false, bool max_score_open_interval = false);
	bool ZRANK(const KEY& key, const VALUE& member, long long& ret_rank);
	bool ZREM(const KEY& key, int remove_num, const VALUE* member_list, long long& ret_value);
	bool ZREMRANGEBYRANK(const KEY& key, int start_index, int stop_index, long long& ret_value);
	bool ZREMRANGEBYSCORE(const KEY& key, double min_score, double max_score, long long& ret_value, bool min_score_open_interval = false, bool max_score_open_interval = false);
	bool ZSCORE(const KEY& key, const VALUE& member, VALUE& ret_score_str);

	inline const std::string GetLastErrorMsg() const { return m_last_error_msg; }
	inline const std::string GetLastErrorQuery() const { return m_last_error_query; }

	bool TryConnectOnServerStart();
private:
	RedisReply* ExecuteCmd(const char* format, va_list ap);

	bool CommandBool(const char* format, ...);
	bool CommandStatus(const char* format, ...);
	bool CommandInteger(long long& value, const char* format, ...);
	bool CommandString(std::string& str, const char* format, ...);
	bool CommandArray(VDATA& vdata, const char* format, ...);

	void FreeRedisReply(RedisReply* reply);

	int Connect();
	bool CheckReply(RedisReply* reply);

	void SetErrorInfo(RedisReply* reply, const char* query_cmd = NULL);
	void SetErrorString(const std::string& errmsg, const char* query_cmd = NULL);
private:
	std::string m_ip;
	int m_port;
	int m_connect_timeout_ms;
	std::string m_auth_str;
	RedisContext* m_ctx;
	std::string m_last_error_msg;
	std::string m_last_error_query;
};

#endif