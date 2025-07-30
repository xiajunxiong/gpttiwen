#ifndef __ROLE_COMMON_SAVE_HPP__
#define __ROLE_COMMON_SAVE_HPP__

#include "servercommon/rolecommonsavedef.h"

struct RoleCommonSaveDataNode
{
	RoleCommonSaveDataNode()
	{
		is_dirty = false;
		is_new_create = false;
		is_invalid = false;
	}

	bool is_dirty;
	bool is_new_create;		//数据库是否有对应记录
	bool is_invalid;
	std::vector<char> data;
};

struct RoleCommonSaveKey
{
	RoleCommonSaveKey(int p1, int p2 = 0);
	bool operator<(const RoleCommonSaveKey & other)const;

	int param1;
	int param2;
};

//--------------------不存库--------------------------------
struct RetRoleCommonSaveData
{
	RetRoleCommonSaveData() :key(0)
	{
		data.Reset(0);
	}

	RoleCommonSaveKey key;
	RoleCommonSaveData data;
};

template<typename T>
struct RetRoleCommonSaveDataT
{
	RetRoleCommonSaveDataT() :key(0)
	{
		data.Reset();
	}

	RoleCommonSaveKey key;
	T data;
};

class RoleModuleManager;
class RoleCommonSave
{
public:
	RoleCommonSave(int save_type);
	virtual ~RoleCommonSave();
	
	template<typename T>
	static T ConvertTypeData(int save_type, const RoleCommonSaveData & role_data);
	static std::vector<char> ConvertVecData(int save_type, const RoleCommonSaveData & role_data);
	static RoleCommonSaveData ConvertRoleCommonSaveData(const std::vector<char> & vec_role_data);

	void InitRoleModuleManager(RoleModuleManager * mgr);
	void InitData(const RoleCommonSaveKey & key, const RoleCommonSaveData & role_data);
	void SetData(const RoleCommonSaveKey & key, const RoleCommonSaveData & data);

	template<typename T>
	T * GetTypeData(const RoleCommonSaveKey & key, bool is_auto_create = true);
	std::vector<char> * GetData(const RoleCommonSaveKey & key, bool is_auto_create = true);
	void SetDataChange(const RoleCommonSaveKey & key);
	void ClearAllData();
	void RemoveData(const RoleCommonSaveKey & key);

	int GetSaveType();
	void BeginDataForeach();
	bool GetNextData(RetRoleCommonSaveData ARG_OUT & data);
	bool GetLastData(RetRoleCommonSaveData ARG_OUT & data);
	bool GetRangeLastData(const RoleCommonSaveKey & key1, const RoleCommonSaveKey & key2, RetRoleCommonSaveData ARG_OUT & data);
	std::vector<RetRoleCommonSaveData> GetRangeData(const RoleCommonSaveKey & key1, const RoleCommonSaveKey & key2);
	int GetSaveDataNum()const;
	//----------------------
public:
	void GetChangeDataList(RoleCommonSaveDataParam & ARG_OUT data_list, bool is_clear = true);
	void ClearDirtyMark(const RoleCommonSaveDataParam::DBRoleCommonSaveDataItem & data_param_item);
private:
	RoleModuleManager * m_role_module_mgr;

	//--------------------------
	int m_save_type;
	bool m_is_data_change;
	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode> m_save_data;
	std::map<RoleCommonSaveKey, RoleCommonSaveDataNode>::const_iterator m_data_iter;
	//-------------------------------
};

template<typename T>
inline T RoleCommonSave::ConvertTypeData(int save_type, const RoleCommonSaveData & role_data)
{
#ifdef _MSC_VER
	static_assert(std::is_pod<T>::value, "T  is not pod type in" __FUNCTION__);
#endif
	unsigned int data_len = ::GetRoleCommonSaveDataLengthByType(save_type);
	assert(data_len == sizeof(T) && data_len <= sizeof(role_data));

	T ret;
	if (data_len == sizeof(T) && data_len <= sizeof(role_data))
	{
		memcpy(&ret, &role_data, data_len);
	}

	return ret;
}

template<typename T>
inline T * RoleCommonSave::GetTypeData(const RoleCommonSaveKey & key, bool is_auto_create)
{
#ifdef _MSC_VER
	static_assert(std::is_pod<T>::value, "T  is not pod type in" __FUNCTION__);
#endif

	std::vector<char> * data = this->GetData(key, is_auto_create);

#ifdef _DEBUG
	if (is_auto_create)
	{
		assert(data != NULL);
		if (::GetRoleCommonSaveDataLengthByType(m_save_type) > 0)
		{
			assert(data->size() == sizeof(T));
		}
	}
#endif

	if (data != NULL && data->size() >= sizeof(T))
	{
		std::vector<char> & data_ref = *data;
		return (T *)&data_ref[0];
	}

	return NULL;
}

#endif


