#ifndef __ASYNC_RELOAD_MONSTER_HPP__
#define __ASYNC_RELOAD_MONSTER_HPP__

#include "servercommon/asynctask/iasynctask.hpp"
#include "servercommon/asynctask/iasynctaskfactory.hpp"
#include <string>


class AsyncReloadMonsterFactory;
class MonsterPool;
class MonsterGroupPool;
class AsyncReloadMonster : public IAsyncTask
{
public:
	friend class AsyncReloadMonsterFactory;

	AsyncReloadMonster();
	virtual ~AsyncReloadMonster();

	virtual int DoTask();
	virtual int DoCallBack();
	virtual void Destroy();

	void SetConfigDir(const std::string& dir);
private:
	bool ReloadMonsterPool();
	bool ReloadMonsterGroupPool();
	void Release();

	std::string config_dir;
	MonsterPool* temp_monster_pool;
	MonsterGroupPool* temp_monster_group_pool;
};

class AsyncReloadMonsterFactory : public IAsyncTaskFactory
{
public:
	virtual IAsyncTask* Create();
};

#endif