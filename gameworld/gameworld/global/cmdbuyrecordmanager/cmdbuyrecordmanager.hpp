#ifndef __CMD_BUY_RECORD_MANAGER_HPP__
#define __CMD_BUY_RECORD_MANAGER_HPP__

#include <vector>

#include "servercommon/errornum.h"
#include "gamelog.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"

class CmdBuyRecordManager				
{
public:
	static CmdBuyRecordManager & Instance();

	template<typename T> static  T * GetCmdBuyRecordByUniqueId(Role * role,int save_type, long long unique_id);
	template<typename T> static int DelCmdBuyRecordByUniqueId(Role * role, int save_type, long long unique_id, LogAgent & log, const char * reason);//删除指定
	template<typename T> static T * GetNewCmdBuyRecord(Role * role, int save_type);
	template<typename T> static int DelRedundantCmdBuyRecord(Role * role, int save_type, LogAgent & log, const char * reason, int more_than_num = 5);//删除超过more_than_num记录(创建时间排序)越早创建会被删
	bool GetAnyCmdBuyRecordUniqueIdAndNeedGold(Role * role, int save_type,long long & uuid, int & need_gold);

private:
	CmdBuyRecordManager();
	~CmdBuyRecordManager();
};

#endif

template<typename T>
inline T * CmdBuyRecordManager::GetCmdBuyRecordByUniqueId(Role * role, int save_type, long long unique_id)
{
#ifdef _MSC_VER
	static_assert(std::is_pod<T>::value, "T  is not pod type in" __FUNCTION__);
#endif

	if (role == NULL)
	{
		return NULL;
	}

	RoleCommonSave * rcs = role->GetRoleModuleManager()->GetRoleCommonSaveManager()->GetRoleCommonSave(save_type);
	if (rcs == NULL)
	{
		return NULL;
	}

	RetRoleCommonSaveData get_data;
	rcs->BeginDataForeach();
	while (rcs->GetNextData(get_data))
	{
		T t_type = RoleCommonSave::ConvertTypeData<T>(save_type, get_data.data);
		if (t_type.unique_id == unique_id)
		{
			T * buy_item = rcs->GetTypeData<T>(get_data.key, false);
			return buy_item;
		}
	}

	return NULL;
}

template<typename T>
inline int CmdBuyRecordManager::DelCmdBuyRecordByUniqueId(Role * role, int save_type, long long unique_id, LogAgent & log, const char * reason)
{
#ifdef _MSC_VER
	static_assert(std::is_pod<T>::value, "T  is not pod type in" __FUNCTION__);
#endif

	if (role == NULL || reason == NULL)
	{
		return -__LINE__;
	}

	RoleCommonSave * rcs = role->GetRoleModuleManager()->GetRoleCommonSaveManager()->GetRoleCommonSave(save_type);
	if (rcs == NULL)
	{
		return -__LINE__;
	}

	RetRoleCommonSaveData get_data;
	rcs->BeginDataForeach();
	while (rcs->GetNextData(get_data))
	{
		T t_type = RoleCommonSave::ConvertTypeData<T>(save_type, get_data.data);
		if (t_type.unique_id == unique_id)
		{
			std::string log_str1 = ::GetString(t_type);
			log.printf(LL_INFO, "%s line:%d | save_type:%d reason:%s role[%d %s] %s", __FUNCTION__, __LINE__,save_type, reason, role->GetUID(), role->GetName(), log_str1.c_str());
			rcs->RemoveData(get_data.key);
			return 0;
		}
	}

	return -__LINE__;
}

template<typename T>
inline T * CmdBuyRecordManager::GetNewCmdBuyRecord(Role * role, int save_type)
{
#ifdef _MSC_VER
	static_assert(std::is_pod<T>::value, "T  is not pod type in" __FUNCTION__);
#endif

	if (role == NULL)
	{
		return NULL;
	}

	RoleCommonSave * rcs = role->GetRoleModuleManager()->GetRoleCommonSaveManager()->GetRoleCommonSave(save_type);
	if (rcs == NULL)
	{
		return NULL;
	}

	int next_key = 0;

	RetRoleCommonSaveData ret_data;
	if (rcs->GetLastData(ret_data))
	{
		next_key = ret_data.key.param1 + 1;

		T buy_item = RoleCommonSave::ConvertTypeData<T>(save_type, ret_data.data);
		time_t now = EngineAdapter::Instance().Time();
		if (now < (time_t)buy_item.create_time + CMD_BUY_ITEM_WAIT_TIME)
		{
			role->NoticeNum(errornum::EN_HAS_ORDER);
			return NULL;
		}
	}

	T * buy_item = rcs->GetTypeData<T>(next_key);
	rcs->SetDataChange(next_key);
	return buy_item;
}

template <class T>
bool CmdBuyRecordDelSort(const  T & a, const  T & b)
{
	if (a.data.create_time > b.data.create_time)
		return true;
	if (a.data.create_time < b.data.create_time)
		return false;

	if (a.data.unique_id > b.data.unique_id)
		return true;
	if (a.data.unique_id < b.data.unique_id)
		return false;

	return false;
}

template<typename T>
inline int CmdBuyRecordManager::DelRedundantCmdBuyRecord(Role * role, int save_type, LogAgent & log, const char * reason, int more_than_num)
{
#ifdef _MSC_VER
	static_assert(std::is_pod<T>::value, "T  is not pod type in" __FUNCTION__);
#endif
	if (role == NULL || reason == NULL)
	{
		return -__LINE__;
	}

	RoleCommonSave * rcs = role->GetRoleModuleManager()->GetRoleCommonSaveManager()->GetRoleCommonSave(save_type);
	if (rcs == NULL)
	{
		return -__LINE__;
	}



	std::vector<RetRoleCommonSaveDataT<T> > vec;

	RetRoleCommonSaveData get_data;
	rcs->BeginDataForeach();
	while (rcs->GetNextData(get_data))
	{
		RetRoleCommonSaveDataT<T> tmp;
		tmp.key = get_data.key;
		tmp.data = RoleCommonSave::ConvertTypeData<T>(save_type, get_data.data);
		vec.push_back(tmp);
	}

	//从N个开始删，所以越新创建排在前面防止删除
	std::sort(vec.begin(), vec.end(), CmdBuyRecordDelSort<RetRoleCommonSaveDataT<T> >);

	for (int i = more_than_num; i < (int)vec.size(); i++)				
	{
		const RetRoleCommonSaveDataT<T> & curr = vec[i];
		std::string log_str1 = ::GetString(curr.data);
		log.printf(LL_INFO, "%s line:%d | save_type:%d reason:%s role[%d %s] %s", __FUNCTION__, __LINE__, save_type, reason, role->GetUID(), role->GetName(), log_str1.c_str());
		rcs->RemoveData(curr.key);
	}

	return 0;
}
