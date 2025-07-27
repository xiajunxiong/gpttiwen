#include "redismanager.hpp"

//TODO 改为线程安全的单例创建
RedisManager& RedisManager::Instance()
{
	static RedisManager inst;
	return inst;
}

void RedisManager::Init(bool is_open, const std::string& key_prefix, const std::string&ip, int port, const std::string& auth_str, int connect_timeout_ms)
{
	if (NULL != m_redis_adapter)
	{
		delete m_redis_adapter;
		m_redis_adapter = NULL;
	}

	m_redis_adapter = new RedisAdapter();
	m_redis_adapter->Init(ip, port, auth_str, connect_timeout_ms);

	m_key_prefix = key_prefix;
	m_is_open = is_open;
}

bool RedisManager::GET(const std::string & key, std::string & value)
{
#ifdef _DEBUG
	assert(NULL != m_redis_adapter);
#endif

	return m_redis_adapter->GET(m_key_prefix + key, value);
}

bool RedisManager::SET(const std::string & key, const std::string & value)
{
#ifdef _DEBUG
	assert(NULL != m_redis_adapter);
#endif

	return m_redis_adapter->SET(m_key_prefix + key, value);
}

bool RedisManager::MGET(const KEYS & keys, VDATA & vdata)
{
	KEYS new_keys;
	new_keys.reserve(keys.size());
	for (size_t i = 0; i < keys.size(); ++i)
	{
		new_keys.push_back(m_key_prefix + keys[i]);
	}
	return m_redis_adapter->MGET(new_keys, vdata);
}

bool RedisManager::MSET(const KEYS & keys, const VDATA & vdata)
{
	KEYS new_keys;
	new_keys.reserve(keys.size());
	for (size_t i = 0; i < keys.size(); ++i)
	{
		new_keys.push_back(m_key_prefix + keys[i]);
	}
	return m_redis_adapter->MSET(new_keys, vdata);
}

bool RedisManager::TryConnectOnServerStart()
{
	if (!this->IsOpen()) return false;
	
	if (NULL == m_redis_adapter) return false;

	return m_redis_adapter->TryConnectOnServerStart();
}

RedisManager::RedisManager() : m_redis_adapter(NULL), m_is_open(false)
{
	
}

RedisManager::~RedisManager()
{
	if (NULL != m_redis_adapter)
	{
		delete m_redis_adapter;
		m_redis_adapter = NULL;
	}
}


