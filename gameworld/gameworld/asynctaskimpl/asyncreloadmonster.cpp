#include "asyncreloadmonster.hpp"
#include "gamelog.h"
#include "monster/monsterpool.hpp"
#include "monster/monster_group_pool.hpp"

AsyncReloadMonster::AsyncReloadMonster() : temp_monster_pool(NULL),
	temp_monster_group_pool(NULL)
{
	
}

AsyncReloadMonster::~AsyncReloadMonster()
{

}

int AsyncReloadMonster::DoTask()
{
	if (this->ReloadMonsterPool()
		&& this->ReloadMonsterGroupPool())
	{
		return 0;
	}
	else
	{
		this->Release();
		gamelog::g_log_workshop.printf(LL_MAINTANCE, "AsyncReloadMonster::DoTask Reload Failed!");
		return -1;
	}
}

int AsyncReloadMonster::DoCallBack()
{
	if (NULL != temp_monster_pool
		&& NULL != temp_monster_group_pool)
	{
		if (NULL != g_monster_pool)
		{
			delete g_monster_pool;
			g_monster_pool = NULL;
		}
		g_monster_pool = temp_monster_pool;

		if (NULL != g_monster_group_pool)
		{
			delete g_monster_group_pool;
			g_monster_group_pool = NULL;
		}
		g_monster_group_pool = temp_monster_group_pool;

		gamelog::g_log_world.printf(LL_MAINTANCE, "AsyncReloadMonster::DoCallBack Substitution SUCC SUCC SUCC!");
	}
	else
	{
		this->Release();
		gamelog::g_log_world.printf(LL_MAINTANCE, "AsyncReloadMonster::DoCallBack Substitution Abort Abort Abort!");
	}
	
	return 0;
}

void AsyncReloadMonster::Destroy()
{
	delete this;
}

void AsyncReloadMonster::SetConfigDir(const std::string& dir)
{
	assert(!dir.empty());

	config_dir = dir;
	if (!config_dir.empty() && config_dir[config_dir.size() - 1] != '/')
	{
		config_dir += '/';
	}
}

bool AsyncReloadMonster::ReloadMonsterPool()
{
	temp_monster_pool = new MonsterPool();
	std::string err;
	if (!temp_monster_pool->Init(config_dir, "battlemonstermanager.xml", &err))
	{
		delete temp_monster_pool;
		temp_monster_pool = NULL;
		gamelog::g_log_world.printf(LL_MAINTANCE, "AsyncReloadMonster::ReloadMonsterPool errormsg[%s]", err.c_str());
		return false;
	}

	return true;
}

bool AsyncReloadMonster::ReloadMonsterGroupPool()
{
	temp_monster_group_pool = new MonsterGroupPool();
	std::string err;
	if (!temp_monster_group_pool->Init(config_dir + "monster_group.xml", &err))
	{
		delete temp_monster_group_pool;
		temp_monster_group_pool = NULL;
		gamelog::g_log_world.printf(LL_MAINTANCE, "AsyncReloadMonster::ReloadMonsterGroupPool errormsg[%s]", err.c_str());
		return false;
	}

	return true;
}

void AsyncReloadMonster::Release()
{
	if (NULL != temp_monster_pool)
	{
		delete temp_monster_pool;
		temp_monster_pool = NULL;
	}

	if (NULL != temp_monster_group_pool)
	{
		delete temp_monster_group_pool;
		temp_monster_group_pool = NULL;
	}
}

IAsyncTask* AsyncReloadMonsterFactory::Create()
{
	return new AsyncReloadMonster();
}
