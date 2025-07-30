#ifndef __COMMON_SAVE_HPP__
#define __COMMON_SAVE_HPP__

#include "servercommon/commonsavedef.h"

struct CommonSaveDataNode
{
	CommonSaveDataNode()
	{
		is_dirty = false;
		is_new_create = false;
		is_invalid = false;
		memset(&data, 0, sizeof(data));
	}

	bool is_dirty;
	bool is_new_create;		//数据库是否有对应记录
	bool is_invalid;
	std::vector<char> data;
};

struct CommonSaveKey
{
	CommonSaveKey(int p1, int p2 = 0, int p3 = 0);
	CommonSaveKey(int p1, long long p2_3);
	CommonSaveKey(long long  p1_2);
	bool operator<(const CommonSaveKey & other)const;
	 
	int param1;
	int param2;
	int param3;
};

//--------------------不存库--------------------------------
struct RetCommonSaveData
{
	RetCommonSaveData() :key(0)
	{
		data.Reset(0);
	}

	CommonSaveKey key;
	CommonSaveData data;
};

class CommonSave
{
public:
	CommonSave(int save_type);
	virtual ~CommonSave();
	
	static std::vector<char> ConvertVecData(int save_type, const CommonSaveData & role_data);
	static CommonSaveData ConvertCommonSaveData(const std::vector<char> & vec_role_data);

	virtual void Update(unsigned long interval, time_t now_second); 
	void InitData(const CommonSaveKey & key, const CommonSaveData & role_data);
	void SetData(const CommonSaveKey & key, const CommonSaveData & data);
	void Save(bool is_stop_server = false);	

	template<typename T>
	T * GetTypeData(const CommonSaveKey & key, bool is_auto_create = true);
	std::vector<char> * GetData(const CommonSaveKey & key, bool is_auto_create = true);

	void SetDataChange(const CommonSaveKey & key);
	void ClearAllData();
	void RemoveData(const CommonSaveKey & key);

	int GetSaveType();
	void BeginDataForeach();
	bool GetNextData(RetCommonSaveData ARG_OUT & data);
	bool GetLastData(RetCommonSaveData ARG_OUT & data);
	bool GetRangeLastData(const CommonSaveKey & key1, const CommonSaveKey & key2, RetCommonSaveData ARG_OUT & data);
	std::vector<RetCommonSaveData> GetRangeData(const CommonSaveKey & key1, const CommonSaveKey & key2);
	std::vector<RetCommonSaveData> GetAllData();
private:
	void GetChangeDataList(CommonSaveDataParam & ARG_OUT data_list, bool is_clear = true);
	void ClearChangeFlag(const CommonSaveDataParam & role_data_param);

private:	
	//--------------------------
	int m_save_type;
	time_t m_next_data_save_time;
	bool m_is_data_change;
	std::map<CommonSaveKey, CommonSaveDataNode> m_save_data;
	std::map<CommonSaveKey, CommonSaveDataNode>::const_iterator m_data_iter;
	//-------------------------------
};

template<typename T>
inline T * CommonSave::GetTypeData(const CommonSaveKey & key, bool is_auto_create)
{
	std::vector<char> * data = this->GetData(key, is_auto_create);

#ifdef _DEBUG
	if (is_auto_create)
	{
		assert(data != NULL);
		if (::GetCommonSaveDataLengthByType(m_save_type) > 0)
		{
			assert(data->size() == sizeof(T));
		}
	}
#endif

	if (data != NULL && !data->empty() && data->size() >= sizeof(T))
	{
		std::vector<char> & data_ref = *data;
		return (T *)&data_ref[0];
	}

	return NULL;
}

#endif


