#ifndef __CLOUD_ARENA_DATA_LOADER_HPP__
#define __CLOUD_ARENA_DATA_LOADER_HPP__

class CloudArenaDataLoader
{
public:
	CloudArenaDataLoader();
	~CloudArenaDataLoader();

	inline int GetDataStatus() const { return m_data_status; }
	inline int GetUserDataStatus() const { return m_user_data_status; }
	void SetDataStatus(int status);
	void SetUserDataStatus(int status);

	bool IsAllLoadFinish() const;

	bool LoadData();
	bool LoadUserData(long long offset);
private:
	void OnLoadStatusChange();

	int m_data_status;
	int m_user_data_status;
};


#endif