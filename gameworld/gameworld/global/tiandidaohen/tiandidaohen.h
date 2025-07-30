#ifndef _TIAN_DI_DAO_HEN_HPP_
#define _TIAN_DI_DAO_HEN_HPP_

#include "servercommon/servercommon.h"

class Role;
class TianDiDaoHen
{
public:
	static TianDiDaoHen & Instance();

	void OnUserLogin(Role * role);
	void OnUserLoginOriginGame(int role_id);
	void Update(unsigned long interval, time_t now_second);
	void OnGameServerConnect(std::set<int> server_id);

	bool IsOpen()const;
	void SendTianDiDaoHenOpenInfo(Role * role = NULL, int role_id = 0);
	void SetInfo(int state,long long next_state_time);
	void SyncInfoToGame(int server_id = 0);

	void GmNextState();
private:
	TianDiDaoHen();
	virtual ~TianDiDaoHen();
private:
	bool m_is_set_info_from_hidden;
};



#endif

