#ifndef  __UTA_CHAMPION_RECORD_MANAGER_HPP__
#define  __UTA_CHAMPION_RECORD_MANAGER_HPP__

#include "servercommon/userprotocal/msgutachampionrecord.h"
#include "servercommon/internalprotocal/bigcrosshgameprotocal.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

class Role;

class UTAChampionRecordManager
{
public:
	static UTAChampionRecordManager & Instance();
		
	void OnGetUTAChampionRecord(Role * _role, Protocol::CSGetUTAChampionRecord * _get_utacr);
	void SendUTAChampionRecordToRole(const char * _data, int _length);

	void GameHiddenGetUTAChampionRecord(crossgameprotocal::GameHiddenGetUTAChampionRecord * _get_utacr);

	// 将玩家自身或队伍数据添加到手册记录中
	void GmAddUTAChampionRecord(Role * _role , int _index);

	void OnRequestUpdateUserData(int role_id);
private:
	UTAChampionRecordManager();
	~UTAChampionRecordManager();


	void OnGetUTAChampionRecordByHidden(int _uid, int _begin_index, int _end_index);
	void OnGetUTAChampionRecordByGameWorld(int _uid, int _begin_index, int _end_index);

	void BuildUTAChampionRecordUserInfo(Role * _role, UTAChampionRecordUserInfo & _out_user_info);

};

#endif	// __UTA_CHAMPION_RECORD_MANAGER_HPP__
