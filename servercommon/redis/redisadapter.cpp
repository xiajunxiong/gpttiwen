#include "redisadapter.hpp"
#include "hiredis/hiredis.h"
#include <iostream>
#include <cassert>
#include "servercommon/servercommon.h"

#ifdef _MSC_VER
#include <winsock.h>
#endif

RedisAdapter::RedisAdapter() : m_port(0), m_connect_timeout_ms(0), m_ctx(NULL)
{
	
}

RedisAdapter::~RedisAdapter()
{
	if (NULL != m_ctx)
	{
		redisFree(m_ctx);
		m_ctx = NULL;
	}
}

void RedisAdapter::Init(const std::string&ip, int port, const std::string& auth_str, int connect_timeout_ms)
{
	m_ip = ip;
	m_port = port;
	m_auth_str = auth_str;
	m_connect_timeout_ms = connect_timeout_ms;
	m_ctx = NULL;
}

bool RedisAdapter::GET(const KEY& key, VALUE& value)
{
	return this->CommandString(value, "GET %s", key.c_str());
}

bool RedisAdapter::SET(const KEY& key, const VALUE& value)
{
	return this->CommandStatus("SET %s %s", key.c_str(), value.c_str());
}

bool RedisAdapter::GETSET(const KEY& key, const VALUE& new_value, VALUE& old_value)
{
	return this->CommandString(old_value, "GETSET %s %s", key.c_str(), new_value.c_str());
}

bool RedisAdapter::SETEX(const KEY& key, const VALUE& value, long long expire_seconds)
{
	// since redis 2.6.12, redis recommend "SET key value EX seconds" instead of "SETEX key seconds value"
	return this->CommandStatus("SET %s %s ex %lld", key.c_str(), value.c_str(), expire_seconds);
}

bool RedisAdapter::PSETEX(const KEY& key, const VALUE& value, long long expire_milliseconds)
{
	// since redis 2.6.12, redis recommend "SET key value PX milliseconds" instead of "PSETEX key milliseconds value"
	return this->CommandStatus("SET %s %s px %lld", key.c_str(), value.c_str(), expire_milliseconds);
}

bool RedisAdapter::MGET(const KEYS & keys, VDATA & vdata)
{
#ifdef _DEBUG
	assert(!keys.empty());
#endif

	std::string format = "MGET ";
	for (size_t i = 0; i < keys.size(); ++i)
	{
		format += keys[i] + " ";
	}
	return this->CommandArray(vdata, format.c_str());
}

bool RedisAdapter::MSET(const KEYS & keys, const VDATA & vdata)
{
#ifdef _DEBUG
	assert(keys.size() == vdata.size());
#endif

	std::string format = "MSET ";
	for (size_t i = 0; i < keys.size() && i < vdata.size(); ++i)
	{
		format += keys[i] + " " + vdata[i] + " ";
	}

	return this->CommandStatus(format.c_str());
}

bool RedisAdapter::INCR(const KEY& key, long long& value)
{
	return this->CommandInteger(value, "INCR %s", key.c_str());
}

bool RedisAdapter::INCRBY(const KEY& key, long long incrby, long long& value)
{
	return this->CommandInteger(value, "INCRBY %s %lld", key.c_str(), incrby);
}

bool RedisAdapter::DECR(const KEY& key, long long& value)
{
	return this->CommandInteger(value, "DECR %s", key.c_str());
}

bool RedisAdapter::DECRBY(const KEY& key, long long decrby, long long& value)
{
	return this->CommandInteger(value, "DECRBY %s %lld", key.c_str(), decrby);
}

bool RedisAdapter::DEL(const KEY& key)
{
	return this->CommandBool("DEL %s", key.c_str());
}

bool RedisAdapter::DEL(const KEYS& keys)
{
	std::string del_str = "DEL";
	for (size_t i = 0; i < keys.size(); ++i)
	{
		del_str += " " + keys[i];
	}

	return this->CommandBool(del_str.c_str());
}

bool RedisAdapter::EXISTS(const KEY& key)
{
	return this->CommandBool("EXISTS %s", key.c_str());
}

bool RedisAdapter::EXPIRE(const KEY& key, long long expire_seconds)
{
	return this->CommandBool("EXPIRE %s %lld", key.c_str(), expire_seconds);
}

bool RedisAdapter::PEXPIRE(const KEY& key, long long expire_milliseconds)
{
	return this->CommandBool("PEXPIRE %s %lld", key.c_str(), expire_milliseconds);
}

bool RedisAdapter::EXPIREAT(const KEY& key, long long expire_at_timestamp)
{
	return this->CommandBool("EXPIREAT %s %lld", key.c_str(), expire_at_timestamp);
}

bool RedisAdapter::PEXPIREAT(const KEY& key, long long expire_at_timestamp_millisecond)
{
	return this->CommandBool("PEXPIREAT %s %lld", key.c_str(), expire_at_timestamp_millisecond);
}

bool RedisAdapter::RANDOMKEY(KEY& key)
{
	return this->CommandString(key, "RANDOMKEY");
}

bool RedisAdapter::HGET(const KEY& key, const FIELD& field, VALUE& value)
{
	return this->CommandString(value, "HGET %s %s", key.c_str(), field.c_str());
}

bool RedisAdapter::HSET(const KEY& key, const FIELD& field, const VALUE& value, long long& ret_value)
{
	return this->CommandInteger(ret_value, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
}

bool RedisAdapter::HMGET(const KEY& key, const FIELDS& fields, VDATA& vdata)
{
	std::string format = "HMGET " + key;
	for (size_t i = 0; i < fields.size(); ++i)
	{
		format += " " + fields[i];
	}
	return this->CommandArray(vdata, format.c_str());
}

bool RedisAdapter::HMSET(const KEY& key, const FIELDS& fields, const VDATA& vdata)
{
	std::string format = "HMSET " + key;
	for (size_t i = 0; i < fields.size() && i < vdata.size(); ++i)
	{
		format += " " + fields[i] + " " + vdata[i];
	}
	return this->CommandStatus(format.c_str());
}

bool RedisAdapter::HGETALL(const KEY& key, VDATA& vdata)
{
	return this->CommandArray(vdata, "HGETALL %s", key.c_str());
}

bool RedisAdapter::HSETNX(const KEY& key, const FIELD& field, VALUE& value, long long& ret_value)
{
	return this->CommandInteger(ret_value, "HSETNX %s %s %s", key.c_str(), field.c_str(), value.c_str());
}

bool RedisAdapter::HDEL(const KEY& key, const FIELD& field, long long& ret_value)
{
	return this->CommandInteger(ret_value, "HDEL %s %s", key.c_str(), field.c_str());
}

bool RedisAdapter::HDEL(const KEY& key, const FIELDS& fields, long long& ret_value)
{
	std::string format = "HDEL " + key;
	for (size_t i = 0; i < fields.size(); ++i)
	{
		format += " " + fields[i];
	}
	return this->CommandInteger(ret_value, format.c_str());
}

bool RedisAdapter::LPUSH(const KEY& key, const VALUE& value, long long& list_length)
{
	return this->CommandInteger(list_length, "LPUSH %s %s", key.c_str(), value.c_str());
}

bool RedisAdapter::LPUSH(const KEY& key, const VALUES& values, long long& list_length)
{
	std::string format = "LPUSH " + key;
	for (size_t i = 0; i < values.size(); ++i)
	{
		format += " " + values[i];
	}
	return this->CommandInteger(list_length, format.c_str());
}

bool RedisAdapter::LPOP(const KEY& key, VALUE& value)
{
	return this->CommandString(value, "LPOP %s", key.c_str());
}

bool RedisAdapter::LLEN(const KEY& key, long long& list_length)
{
	return this->CommandInteger(list_length, "LLEN %s", key.c_str());
}

bool RedisAdapter::LRANGE(const KEY& key, int start_index, int end_index, VDATA& values)
{
	return this->CommandArray(values, "LRANGE %s %d %d", key.c_str(), start_index, end_index);
}

bool RedisAdapter::LREM(const KEY& key, const VALUE& value, int count, long long& remove_num)
{
	return this->CommandInteger(remove_num, "LREM %s %s %d");
}

bool RedisAdapter::LSET(const KEY& key, int index, const VALUE& value)
{
	return this->CommandBool("LSET %s %d %s", key.c_str(), index, value.c_str());
}

bool RedisAdapter::LTRIM(const KEY& key, int start_index, int end_index)
{
	return this->CommandBool("LTRIM %s %d %d", key.c_str(), start_index, end_index);
}

bool RedisAdapter::RPOP(const KEY& key, VALUE& value)
{
	return this->CommandString(value, "RPOP %s", key.c_str());
}

bool RedisAdapter::RPUSH(const KEY& key, const VALUE& value, long long& list_length)
{
	return this->CommandInteger(list_length, "RPUSH %s %s", key.c_str(), value.c_str());
}

bool RedisAdapter::RPUSH(const KEY& key, const VALUES& values, long long& list_length)
{
	std::string format = "RPUSH " + key;
	for (size_t i = 0; i < values.size(); ++i)
	{
		format += " " + values[i];
	}
	return this->CommandInteger(list_length, format.c_str());
}

bool RedisAdapter::BLPOP(const KEY& key, VDATA& vValues, unsigned int timeout_s)
{
	return this->CommandArray(vValues, "BLPOP %s %u", key.c_str(), timeout_s);
}

bool RedisAdapter::BRPOP(const KEY& key, VDATA& vValues, unsigned int timeout_s)
{
	return this->CommandArray(vValues, "BRPOP %s %u", key.c_str(), timeout_s);
}

bool RedisAdapter::SADD(const KEY& key, int value_num, const VALUE* values, long long& ret_value)
{
	std::string format = "SADD " + key;
	for (int i = 0; i < value_num; ++i)
	{
		format += " " + values[i];
	}
	return this->CommandInteger(ret_value, format.c_str());
}

bool RedisAdapter::SCARD(const KEY & key, long long & ret_value)
{
	return this->CommandInteger(ret_value, "SCARD %s", key.c_str());
}

bool RedisAdapter::SISMEMBER(const KEY & key, const VALUE & member, bool& is_member)
{
	long long ret_value = 0;
	if (!this->CommandInteger(ret_value, "SISMEMBER %s %s", key.c_str(), member.c_str()))
	{
		return false;
	}

	is_member = ret_value > 0;
	return true;
}

bool RedisAdapter::SMEMBERS(const KEY & key, VDATA & vValues)
{
	return this->CommandArray(vValues, "SMEMBERS %s", key.c_str());
}

bool RedisAdapter::SPOP(const KEY & key, VDATA & list)
{
	return this->CommandArray(list, "SPOP %s", key.c_str());
}

bool RedisAdapter::SRANDMEMBER(const KEY & key, int count, VALUES & values)
{
	return this->CommandArray(values, "SRANDMEMBER %s %d", key.c_str(), count);
}

bool RedisAdapter::SREM(const KEY & key, int value_num, const VALUE * values, long long & ret_value)
{
	std::string format = "SREM " + key;
	for (int i = 0; i < value_num; ++i)
	{
		format += " " + values[i];
	}
	return this->CommandInteger(ret_value, format.c_str());
}

bool RedisAdapter::ZADD(const KEY & key, int member_num, ScoreMember * member_list, long long & newly_add_num)
{
	std::string format = "ZADD " + key;
	for (int i = 0; i < member_num; ++i)
	{
		format += " " + STRING_SPRINTF("%.6f %s", member_list[i].first, member_list[i].second.c_str());
	}

	return this->CommandInteger(newly_add_num, format.c_str());
}

bool RedisAdapter::ZADD(const KEY & key, bool update_on_exist, bool return_updated_member_num, int member_num, ScoreMember * member_list, long long & ret_value)
{
	std::string UPDATE_ON_EXIST = update_on_exist ? "XX" : "NX";
	std::string format = "ZADD " + key + " " + UPDATE_ON_EXIST;
	if (return_updated_member_num)
	{
		format += " CH";
	}
	for (int i = 0; i < member_num; ++i)
	{
		format += " " + STRING_SPRINTF("%.6f %s", member_list[i].first, member_list[i].second.c_str());
	}

	return this->CommandInteger(ret_value, format.c_str());
}

bool RedisAdapter::ZCARD(const KEY & key, long long& ret_value)
{
	return this->CommandInteger(ret_value, "ZCARD %s", key.c_str());
}

bool RedisAdapter::ZINCRBY(const KEY & key, double inc_score, const VALUE & member, std::string & member_new_score)
{
	return this->CommandString(member_new_score, "ZINCRBY %s %f %s", key.c_str(), inc_score, member.c_str());
}

bool RedisAdapter::ZPOPMAX(const KEY & key, int count, VDATA& vValues)
{
	return this->CommandArray(vValues, "ZPOPMAX %s %d", key.c_str(), count);
}

bool RedisAdapter::ZPOPMIN(const KEY & key, int count, VDATA& vValues)
{
	return this->CommandArray(vValues, "ZPOPMIN %s %d", key.c_str(), count);
}

bool RedisAdapter::ZRANGE(const KEY & key, int start_index, int stop_index, bool with_scores, VDATA & vValues)
{
	if (with_scores)
	{
		return this->CommandArray(vValues, "ZRANGE %s %d %d WITHSCORES", key.c_str(), start_index, stop_index);
	}
	else
	{
		return this->CommandArray(vValues, "ZRANGE %s %d %d", key.c_str(), start_index, stop_index);
	}
}

bool RedisAdapter::ZRANGEBYSCORE(const KEY & key, double min_score, double max_score, bool with_scores, int offset, int count, VDATA & vValues, bool min_score_open_interval, bool max_score_open_interval)
{
	std::string format = "ZRANGEBYSCORE " + key;
	std::string min_score_str = min_score_open_interval ? STRING_SPRINTF(" (%f", min_score) : STRING_SPRINTF(" %f", min_score);
	std::string max_score_str = max_score_open_interval ? STRING_SPRINTF(" (%f", max_score) : STRING_SPRINTF(" %f", max_score);
	std::string with_scores_str = with_scores ? " WITHSCORES " : " ";
	std::string limit_offset_count = STRING_SPRINTF(" LIMIT %d %d", offset, count);
	format += min_score_str + max_score_str + with_scores_str + limit_offset_count;

	return this->CommandArray(vValues, format.c_str());
}

bool RedisAdapter::ZRANK(const KEY & key, const VALUE & member, long long & ret_rank)
{
	return this->CommandInteger(ret_rank, "ZRANK %s %s", key.c_str(), member.c_str());
}

bool RedisAdapter::ZREM(const KEY & key, int remove_num, const VALUE * member_list, long long & ret_value)
{
	std::string format = "ZREM " + key;
	for (int i = 0; i < remove_num; ++i)
	{
		format += " " + member_list[i];
	}
	return this->CommandInteger(ret_value, format.c_str());
}

bool RedisAdapter::ZREMRANGEBYRANK(const KEY & key, int start_index, int stop_index, long long & ret_value)
{
	return this->CommandInteger(ret_value, "ZREMRANGEBYRANK %s %d %d", key.c_str(), start_index, stop_index);
}

bool RedisAdapter::ZREMRANGEBYSCORE(const KEY & key, double min_score, double max_score, long long& ret_value, bool min_score_open_interval, bool max_score_open_interval)
{
	std::string format = "ZREMRANGEBYSCORE " + key;
	std::string min_score_str = min_score_open_interval ? STRING_SPRINTF(" (%f", min_score) : STRING_SPRINTF(" %f", min_score);
	std::string max_score_str = max_score_open_interval ? STRING_SPRINTF(" (%f", max_score) : STRING_SPRINTF(" %f", max_score);
	format += min_score_str + max_score_str;
	return this->CommandInteger(ret_value, format.c_str());
}

bool RedisAdapter::ZSCORE(const KEY & key, const VALUE & member, VALUE & ret_score_str)
{
	return this->CommandString(ret_score_str, "ZSCORE %s %s", key.c_str(), member.c_str());
}

std::string Truncate(const std::string& str, int width, bool show_ellipsis = true)
{
	if (str.length() > width)
	{
		if (show_ellipsis)
		{
			return str.substr(0, width) + "...";
		}
		else
		{
			return str.substr(0, width);
		}
	}
			
	return str;
}

bool RedisAdapter::TryConnectOnServerStart()
{
	int ret = this->Connect();
	if (REDIS_OK == ret)
	{
		return true;
	}
	else
	{
		return false;
	}
}

RedisReply* RedisAdapter::ExecuteCmd(const char* format, va_list ap)
{
	int ret = 0;
	if (m_ctx == NULL) {
		ret = Connect();
		if (ret < 0) {
			return NULL;
		}
	}

	redisReply *reply = (redisReply*)redisvCommand(m_ctx, format, ap);
	if (reply == NULL) 
	{
		if (m_ctx) 
		{
			printf("REDIS EXCUTE COMMAND FAILED£¨1£©, error[%d] errstr[%s]\n COMMAND[%s]\n", m_ctx->err, m_ctx->errstr, Truncate(format, 100, true).c_str());
			redisFree(m_ctx);
			m_ctx = NULL;
		}
		return NULL;
	}

	if (m_ctx->err != 0) 
	{
		printf("REDIS EXCUTE COMMAND FAILED£¨2£©, error[%d] errstr[%s]\n COMMAND[%s]\n", m_ctx->err, m_ctx->errstr, Truncate(format, 100, true).c_str());
		redisFree(m_ctx);
		m_ctx = NULL;

		if (reply) 
		{
			freeReplyObject(reply);
		}
		return NULL;
	}

	return reply;
}

bool RedisAdapter::CommandBool(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	RedisReply* reply = this->ExecuteCmd(format, ap);
	va_end(ap);

	if (NULL == reply)
	{
		this->SetErrorString("[CommandBool] QUERY REDIS REPLY NULL", format);
		return false;
	}

	if (!this->CheckReply(reply))
	{
		this->SetErrorInfo(reply, format);
		this->FreeRedisReply(reply);
		return false;
	}

	if (REDIS_REPLY_STATUS != reply->type && REDIS_REPLY_INTEGER != reply->type)
	{
		this->SetErrorString("[CommandBool] QUERY REDIS SUCCESS BUT TYPE NOT STATUS OR INTEGER", format);
		this->FreeRedisReply(reply);
		return false;
	}

	bool ret = false;
	if (REDIS_REPLY_STATUS == reply->type)
	{
		ret = (reply->str && reply->len > 0) || (reply->str && strcasecmp(reply->str, "OK") == 0);
	}
	else if (REDIS_REPLY_INTEGER == reply->type)
	{
		ret = reply->integer > 0;
	}
	
	if (!ret)
	{
		this->SetErrorString("[CommandBool] QUERY REDIS SUCCESS BUT RET IS FALSE", format);
		this->FreeRedisReply(reply);
		return false;
	}

	this->FreeRedisReply(reply);
	return true;
}

bool RedisAdapter::CommandStatus(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	RedisReply* reply = this->ExecuteCmd(format, ap);
	va_end(ap);

	if (NULL == reply)
	{
		this->SetErrorString("[CommandStatus] QUERY REDIS REPLY NULL", format);
		return false;
	}

	if (!this->CheckReply(reply))
	{
		this->SetErrorInfo(reply, format);
		this->FreeRedisReply(reply);
		return false;
	}

	if (REDIS_REPLY_STATUS != reply->type)
	{
		this->SetErrorString("[CommandStatus] QUERY REDIS SUCCESS BUT TYPE NOT STATUS", format);
		this->FreeRedisReply(reply);
		return false;
	}

	if (!reply->str || 0 == reply->len || strcasecmp(reply->str, "OK") != 0)
	{
		this->SetErrorString("[CommandStatus] QUERY REDIS SUCCESS BUT NOT RETURN \"OK\"", format);
		this->FreeRedisReply(reply);
		return false;
	}

	this->FreeRedisReply(reply);
	return true;
}

bool RedisAdapter::CommandInteger(long long& value, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	RedisReply* reply = this->ExecuteCmd(format, ap);
	va_end(ap);

	if (NULL == reply)
	{
		this->SetErrorString("[CommandInteger] QUERY REDIS REPLY NULL", format);
		return false;
	}

	if (!this->CheckReply(reply))
	{
		this->SetErrorInfo(reply, format);
		this->FreeRedisReply(reply);
		return false;
	}

	if (REDIS_REPLY_INTEGER != reply->type)
	{
		this->SetErrorString("[CommandInteger] QUERY REDIS SUCCESS BUT TYPE NOT INTERGER", format);
		this->FreeRedisReply(reply);
		return false;
	}

	value = reply->integer;

	this->FreeRedisReply(reply);
	return true;
}

bool RedisAdapter::CommandString(std::string& str, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	RedisReply* reply = this->ExecuteCmd(format, ap);
	va_end(ap);

	if (NULL == reply)
	{
		this->SetErrorString("QUERY REDIS REPLY NULL", format);
		return false;
	}

	if (!this->CheckReply(reply))
	{
		this->SetErrorInfo(reply, format);
		this->FreeRedisReply(reply);
		return false;
	}

	if (REDIS_REPLY_STRING != reply->type)
	{
		this->SetErrorString("QUERY REDIS SUCCESS BUT TYPE NOT STRING", format);
		this->FreeRedisReply(reply);
		return false;
	}

	str.assign(reply->str, reply->len);

	this->FreeRedisReply(reply);
	return true;
}

bool RedisAdapter::CommandArray(VDATA& vdata, const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	RedisReply* reply = this->ExecuteCmd(format, ap);
	va_end(ap);

	if (NULL == reply)
	{
		this->SetErrorString("QUERY REDIS REPLY NULL", format);
		return false;
	}

	if (!this->CheckReply(reply))
	{
		this->SetErrorInfo(reply, format);
		this->FreeRedisReply(reply);
		return false;
	}

	if (REDIS_REPLY_ARRAY != reply->type)
	{
		this->SetErrorString("QUERY REDIS SUCCESS BUT TYPE NOT ARRAY", format);
		this->FreeRedisReply(reply);
		return false;
	}

	vdata.clear();
	vdata.reserve(reply->elements);
	for (size_t i = 0; i < reply->elements; ++i)
	{
		vdata.push_back(std::string(reply->element[i]->str, reply->element[i]->len));
	}

	this->FreeRedisReply(reply);
	return true;
}

void RedisAdapter::FreeRedisReply(RedisReply* reply)
{
	freeReplyObject(reply);
}

int RedisAdapter::Connect()
{
	timeval tv;
	tv.tv_sec = m_connect_timeout_ms / 1000;
	tv.tv_usec = (m_connect_timeout_ms % 1000) * 1000;

	if (m_ctx != NULL) {
		redisFree(m_ctx);
		m_ctx = NULL;
	}

	m_ctx = redisConnectWithTimeout(m_ip.c_str(), m_port, tv);
	if (m_ctx == NULL || m_ctx->err != 0) {
		if (m_ctx != NULL) {
			printf("REDIS CONNECT FAILED£¨1£©, addr[%s:%d] error[%d] errstr[%s]\n", m_ip.c_str(), m_port, m_ctx->err, m_ctx->errstr);
			redisFree(m_ctx);
			m_ctx = NULL;
			return REDIS_ERR;
		}

		printf("REDIS CONNECT FAILED£¨2£©, addr[%s:%d]\n", m_ip.c_str(), m_port);
		return REDIS_ERR;
	}

	if (REDIS_ERR == redisSetTimeout(m_ctx, tv)) {
		printf("REDIS CONNECT FAILED£¨3£©, addr[%s:%d]\n", m_ip.c_str(), m_port);
		redisFree(m_ctx);
		m_ctx = NULL;
		return REDIS_ERR;
	}

	if (m_auth_str == "")
	{
		return REDIS_OK;
	}

	std::string auth = "AUTH " + m_auth_str;

	redisReply *reply = (redisReply*)redisCommand(m_ctx, auth.c_str());

	if (reply == NULL) {
		if (m_ctx) {
			redisFree(m_ctx);
			m_ctx = NULL;
		}
		return REDIS_ERR;
	}

	if (m_ctx->err != 0) {
		printf("REDIS CONNECT FAILED£¨4£©, addr[%s:%d] error[%d] errstr[%s]\n", m_ip.c_str(), m_port, m_ctx->err, m_ctx->errstr);
		redisFree(m_ctx);
		m_ctx = NULL;
		if (reply) {
			freeReplyObject(reply);
		}
		return REDIS_ERR;
	}

	if (REDIS_REPLY_ERROR == reply->type) {
		printf("REDIS CONNECT FAILED£¨5£©, addr[%s:%d]  reply_error_msg[%s]\n", m_ip.c_str(), m_port, reply->str);
		if (m_ctx) {
			redisFree(m_ctx);
			m_ctx = NULL;
		}
		freeReplyObject(reply);
		return REDIS_ERR;
	}

	freeReplyObject(reply);
	return REDIS_OK;
}

bool RedisAdapter::CheckReply(RedisReply* reply)
{
	if (NULL == reply) 
	{
		return false;
	}

	switch (reply->type) 
	{
	case REDIS_REPLY_STRING: 
		{
			return true;
		}
		break;

	case REDIS_REPLY_ARRAY: 
		{
			return true;
		}
		break;

	case REDIS_REPLY_INTEGER: 
		{
			return true;
		}
		break;

	case REDIS_REPLY_NIL: 
		{
			return false;
		}
		break;

	case REDIS_REPLY_STATUS: 
		{
			return true;
		}
		break;

	case REDIS_REPLY_ERROR: 
		{
			return false;
		}
		break;

	default: 
		{
			return false;
		}
		break;
	}

	return false;
}

void RedisAdapter::SetErrorInfo(RedisReply* reply, const char* query_cmd /* = NULL */)
{
	this->SetErrorString(std::string(reply->str, reply->len), query_cmd);
}

void RedisAdapter::SetErrorString(const std::string& errmsg, const char* query_cmd /* = NULL */)
{
	m_last_error_msg = errmsg;
	m_last_error_query = "";

	printf("REDIS QUERY ERROR! errmsg[%s]\n", m_last_error_msg.c_str());
	if (NULL != query_cmd)
	{
		m_last_error_query = Truncate(query_cmd, 100);
		printf("ERROR CMD[%s]\n", m_last_error_query.c_str());
	}
}

