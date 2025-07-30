#ifndef __FRIEND_INTIMACY_HPP__
#define __FRIEND_INTIMACY_HPP__

#include "gamedef.h"
#include "servercommon/struct/friendintimacyparam.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/internalprotocal/battleprotocol.h"
#include <map>
#include <set>


enum FRIEND_INTIMACY_DATA_STATUS
{
	FRIEND_INTIMACY_DATA_STATUS_INVALID = 0,						// 初始状态
	FRIEND_INTIMACY_DATA_STATUS_LOADING,							// 数据加载中 
	FRIEND_INTIMACY_DATA_STATUS_LOAD_FINISH,						// 数据加载完成 
	FRIEND_INTIMACY_DATA_STATUS_MAX,
};

static const int FRIEND_INTIMACY_SAVE_INTERVAL = 60;				// 半分钟回写一次

class Role;

class FriendIntimacyManager
{
	friend class RMIInitFriendIntimacyBackObjectImpl;
public:
	typedef std::map<long long , FriendIntimacyInfo > FriendIntimacyMap;				// < friend_for_index , 玩家的好友亲密度信息 >
	typedef std::map<long long , FriendIntimacyInfo >::iterator	FriendIntimacyMapIt;	

public:
	static FriendIntimacyManager & Instance();
	void InitFriendIntimacyParam(int _ret, FriendIntimacyListParam & _param);
	void OnConnectHiddenSucc();
	void OnServerStart();
	void OnServerStop();
	bool IsLoadFinish() const;
	void Update(unsigned long interval, time_t now_second);
	void OnLogin(Role * _role);

	// 处理同步来的好友亲密度信息  消息可能来源于主服 也可能来源于 隐藏服
	void OnSyncFriendIntimacyInfo(crossgameprotocal::SyncFriendIntimacyInfo * _info);
	// 处理来至主服的  修改亲密度操作
	void OnAddIntimacyReqFromGameWorld(crossgameprotocal::GameHiddenAddIntimacyReq * _req);
	// 处理来至主服的  亲密度关系操作
	void OnIntimacyRelationReqFromGameWorld(crossgameprotocal::GameHiddenIntimacyRelationReq * _req);
	//  处理来至主服的  亲密度私聊时间戳设置
	void OnSetIntimacySingleChatTimesFromGameWorld(crossgameprotocal::GameHiddenIntimacySingleChatTimes * _times);
	//  处理来至主服的  亲密度私聊时间戳设置 [我也不想放在这 , 但是没地方放了]
	void OnGiveFlowerNoticeFromGameWorld(crossgameprotocal::GameHiddenGiveFlowerNotice * _notice);
	//  处理来至隐藏服的  亲密度私聊时间戳设置 [我也不想放在这 , 但是没地方放了]
	void OnGiveFlowerNoticeFromHidden(crossgameprotocal::HiddenGameGiveFlowerNotice * _notice);
	//  处理来至主服的  送花回吻 [我也不想放在这 , 但是没地方放了]
	void OnGiveFlowerBackKissFromGameWorld(crossgameprotocal::GameHiddenGiveFlowerBackKiss * _back_kiss);
	//  处理来至隐藏服的  送花回吻 [我也不想放在这 , 但是没地方放了]
	void OnGiveFlowerBackKissFromHidden(crossgameprotocal::HiddenGameGiveFlowerBackKiss * _back_kiss);



	// 获取与好友的亲密度
	int GetFriendIntimacy(int _role_uid, int _friend_uid);
	// 增加好友的亲密度
	void AddIntimacy(int _role_uid, int _friend_uid , int _add_val , const  char * _reason);
	//设置亲密度为0
	void ClearIntimacy(int _role_uid, int _friend_uid, const  char * _reason);
	// 获取最近一次的每日首次私聊的时间戳
	// to : 玩家对好友 
	unsigned int GetLastDayFristToSingleChatTimes(int _role_uid, int _friend_id);
	// from : 好友对玩家的
	unsigned int GetLastDayFristFromSingleChatTimes(int _role_uid, int _friend_id);
	// 设置最近一次的每日首次私聊的时间戳
	// to : 玩家对好友 
	void SetLastDayFristToSingleChatTimes(int _role_uid, int _friend_id , unsigned int _to_times ,const  char * _reason);
	// from : 好友对玩家的
	void SetLastDayFristFromSingleChatTimes(int _role_uid, int _friend_id, unsigned int _from_times, const  char * _reason);
	// 添加好友
	void OnAddFriend(int _role_id, int _friend_id);
	// 删除好友
	void OnRemoveFriend(int _role_id, int _friend_id);

	// 策划 :好友每天的第一次私聊需要加亲密度
	void OnSingleChat(int _role_id, int _friend_id, unsigned int now_times, const char * _reason);

	// 组队战斗也需要加亲密度
	void OnFinishFight(battlegameprotocol::BattleGameFightResultAck * _msg);

	// 送花的广播通知 [要说为啥送花的放在好感度里 , 因为没地方放了!!!]
	void GiveFlowerNotify(int _role_id, int _friend_id , int _item_id);
	// 送花回吻 
	void OnBackToKiss(Role * _role, int _to_role_id);

private:
	FriendIntimacyManager();
	~FriendIntimacyManager();

	void Save();
	void SaveParam();
	void ClearDirtyMark();
	void SetDirtyMark(long long _friend_for_index);
	void CheckLoadFinish();

	// 同步好友亲密度数据  原服  -> 隐藏服
	void SyncAllDataToHidden();
	// 同步好友亲密数据  隐藏服 -> 原服  
	void SyncFriendIntimacyDataToGameWorld(long long friend_for_index, int _sync_status);
	// 在隐藏服处理来源于主服的同步信息
	void InitFriendIntimacyInfoFromGameWorld(crossgameprotocal::SyncFriendIntimacyInfo * _info);
	void UpdataFriendIntimacyInfoFromHidden(crossgameprotocal::SyncFriendIntimacyInfo * _info);

	long long  AddFriendInfo(int _role_id, int _friend_id, const FriendIntimacyInfo & _item);
	long long  RemoveFriendInfo(int _role_id, int _friend_id);

	void AddIntimacyByHidden(int _role_uid, int _friend_uid, int _add_val, const  char * _reason);
	void AddIntimacyByGameWorld(int _role_uid, int _friend_uid, int _add_val,const  char * _reason);

	void OnAddFriendByHidden(int _role_id , int _friend_id);
	void OnRemoveFriendByHidden(int _role_id, int _friend_id);
	void OnIntimacyRelationReqToHidden(int _role_id, int _friend_id , bool _is_add);

	void SendList(Role * _role);
	void SendChangeInfo(Role * _role, int friend_id);

	bool IsCanAddInitimacyBattleType(int _battle_type);

	void SetLastDayFristToSingleChatTimesByHidden(int _role_uid, int _friend_id, unsigned int _to_times, const  char * _reason);
	void SetLastDayFristFromSingleChatTimesByHidden(int _role_uid, int _friend_id, unsigned int _to_times, const  char * _reason);
	void SetLastDayFristToSingleChatTimesByGameWorld(int _role_uid, int _friend_id, unsigned int _to_times, const  char * _reason);
	void SetLastDayFristFromSingleChatTimesByGameWorld(int _role_uid, int _friend_id, unsigned int _to_times, const  char * _reason);
	void SendLastDayFristSingleChatTimesToHidden(int _role_uid, int _friend_id, unsigned int _times, bool is_to);


	void GiveFlowerNotifyByHidden(int _role_id, int _friend_id, int _item_id, int _type);
	void GiveFlowerNotifyByGameWorld(int _role_id, int _friend_id, int _item_id, int _type);

	// 送花回吻 
	void OnBackToKissByHidden(int _role_id , GameName _role_name , int _to_role_id);
	void OnBackToKissByGameWorld(Role * _role, int _to_role_id);

	int m_data_state;
	bool m_load_flag;

	// 好友系统是否有数据发生过变更
	bool m_change_flag;
	// 有信息变动的玩家列表
	std::set< long long > m_dirty_mark;
	std::set< long long > m_old_state;

	// 所有玩家的信息列表
	// 原服为原服所有玩家
	// 隐藏服为所有连接服的所有玩家

	FriendIntimacyMap m_friend_intimacy_map;
};

#endif	// __FRIEND_INTIMACY_HPP__