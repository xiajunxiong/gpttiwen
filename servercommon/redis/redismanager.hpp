#ifndef __REDIS_MANAGER_HPP__
#define __REDIS_MANAGER_HPP__

#include "redisadapter.hpp"
#include "servercommon/servercommon.h"

class RedisAdapter;
class RedisManager
{
public:
	static RedisManager& Instance();

	void Init(bool is_open, const std::string& key_prefix, const std::string&ip, int port, const std::string& auth_str, int connect_timeout_ms);

	bool GET(const std::string& key, std::string& value);
	bool SET(const std::string& key, const std::string& value);

	bool MGET(const KEYS& keys, VDATA& vdata);
	bool MSET(const KEYS& keys, const VDATA& vdata);

	inline bool IsOpen() const { return m_is_open; }
	bool TryConnectOnServerStart();
private:
	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(RedisManager);

private:
	RedisAdapter* m_redis_adapter;
	std::string m_key_prefix;
	bool m_is_open;
};

#endif