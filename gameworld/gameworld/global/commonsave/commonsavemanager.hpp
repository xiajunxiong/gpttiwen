#ifndef __COMMON_SAVE_MANAGER_HPP__
#define __COMMON_SAVE_MANAGER_HPP__

#include "global/commonsave/commonsave.hpp"
#include <functional>

enum COMMON_SAVE_DATA_STATUS
{
	COMMON_SAVE_DATA_STATUS_INVALID = 0,						// 初始状态
	COMMON_SAVE_DATA_STATUS_LOADING,							// 数据加载中 
	COMMON_SAVE_DATA_STATUS_LOAD_FINISH,						// 数据加载完成 
	COMMON_SAVE_DATA_STATUS_MAX,
};

class ICommonSaveLoadSuccCallback
{
public:
	virtual ~ICommonSaveLoadSuccCallback() {}
	virtual void Callback() = 0;
};

class CommonSaveManager
{
	friend class RMIInitCommonDataBackObjectImpl;	
public:
	static CommonSaveManager & Instance();

	void OnServerStart();
	void OnServerStop();
	void Update(unsigned long interval, time_t now_second);
	bool IsLoadFinish() const;
	CommonSave * GetCommonSave(int save_type);

	void Gm(int op, int save_type, int p1, int p2, int p3);
	void RegisterLoadSuccCallback(ICommonSaveLoadSuccCallback* callback);
private:
	CommonSaveManager();
	~CommonSaveManager();
private:
	void InitCommoneData(const CommonSaveDataParam::DBCommonSaveDataItem & data_item);
	bool LoadCommonData(long long id_fram);
	void LoadCommonDataSucc();
	bool SaveCommonData();
	void Add(CommonSave * common_save);

	int m_data_state;
	CommonSave * m_common_save_list[COMMON_SAVE_DATA_TYPE_MAX];

	std::vector<ICommonSaveLoadSuccCallback*> m_load_succ_callback;
};

#endif

