#ifndef __HONG_BAO_MANAGER_HPP__
#define __HONG_BAO_MANAGER_HPP__

#include "servercommon/hongbaodef.hpp"
#include "protocol/msghongbao.hpp"
#include "servercommon/crossdef.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"

#include <map>

enum HONGBAO_DATA_STATUS
{
	HONGBAO_DATA_STATUS_INVALID = 0,						// 初始状态
	HONGBAO_DATA_STATUS_LOADING,							// 数据加载中 
	HONGBAO_DATA_STATUS_LOAD_FINISH,						// 数据加载完成 
	HONGBAO_DATA_STATUS_MAX,
};

static const int HONGBAO_SAVE_INTERVAL = 30;				// 半分钟回写一次

class Role;

class HongBaoManager
{
	friend class RMIInitHongBaoBackObjectImpl;

public:
	static HongBaoManager & Instance();

	void Init(int _ret, int _hongbao_sys_type, HongBaoParam & _hongbao_param);

	void OnGameworldAccept();
	// 同步主服消息到跨服
	void OnSyncHongBaoInfoFromGame(crossgameprotocal::GameHiddenSyncHongBaoInfo * _hongbao_info);
	void OnSyncReqGiveRetFromGame(crossgameprotocal::GameHiddenSyncHongBaoReqGiveRet * _req_ret);
	void OnSyncReqGetRetFromGame(crossgameprotocal::GameHiddenSyncHongBaoReqGetRet * _req_ret);

	// 同步跨服消息到主服
	// 处理来至跨服的发红包
	void  OnHiddenGameSyncHongBaoReqGiveFromHidden(crossgameprotocal::HiddenGameSyncHongBaoReqGive * _give_msg);
	// 处理来至跨服的领红包
	void  OnHiddenGameSyncHongBaoReqGetFromHidden(crossgameprotocal::HiddenGameSyncHongBaoReqGet * _get_msg);
	// 处理来至跨服的操作失败消息 [在跨服找不到玩家,然后发回了主服的]
	void OnHiddenGameSyncHongBaoReqError(crossgameprotocal::HiddenGameSyncHongBaoReqError * _error);


	void OnServerStart();
	void OnServerStop();
	bool IsLoadFinish() const;
	void Update(unsigned long interval, time_t now_second);

	void AddChongZhi(Role * _role, long long _chong_zhi_gold);
	void AddGuildHongBaoTimesByZhanLingUpGrade(Role * _role);

	void OnClientReq(Role * _role, Protocol::CSHongBaoClientReq * _req);

private:
	HongBaoManager();
	~HongBaoManager();

	typedef std::map<int, HongBaoItem> HongBaoMap;									// < 红包ID , 红包信息 >
	typedef std::map<int, HongBaoItem>::iterator  HongBaoMapIt;
	typedef std::map<int, HongBaoItem>::reverse_iterator  HongBaoMapRIt;
	typedef std::map<UniqueServerID, HongBaoMap > HongBaoServerMap;
	typedef std::map<UniqueServerID, HongBaoMap >::iterator  HongBaoServerMapIt;

	typedef std::map<int, std::set< int > > HongBaoGuildMap;						// <家族ID, 红包ID>
	typedef std::map<int, std::set< int > >::iterator  HongBaoGuildMapIt;
	typedef std::map<UniqueServerID, HongBaoGuildMap > HongBaoGuildServerMap;
	typedef std::map<UniqueServerID, HongBaoGuildMap >::iterator  HongBaoGuildServerMapIt;


	int GetNewHongBaoID(int _sys_type);

	void InitUIDSet(HongBaoItem & _hongbao_param);
	void InitIDAutoIncrement();
	void InitDelayData();
	void InitCheckInvalid(int _sys_type, HongBaoItem & _hongbao_param);
	void AddInvalidInfo(unsigned int _times, int _sys_type, LL_UniqueHongBaoID _ll_unique_id);
	void RemoveInvalidInfo(unsigned int _times, int _sys_type, LL_UniqueHongBaoID _ll_unique_id);
	void CheckLoadFinish();
	void CheckInvalid(unsigned int _now_sec);

	void Save();
	void SaveHongBaoParam(int _sys_type);
	void ClearDirtyMark(int _sys_type);
	void SetDirtyMark(int _sys_type, LL_UniqueHongBaoID _ll_unique_hong_bao_id);
	void ClearDelecMergeMark(int _sys_type);
	void SetDelecMergeMark(int _sys_type, LL_UniqueHongBaoID _ll_unique_hong_bao_id);

	void AddHongBaoIDToGuildMap(int _guild_id, int _hongbao_id, const  UniqueServerID & u_sid);
	void RemoveHongBaoIDToGuildMap(int _guild_id, int _hongbao_id, UniqueServerID & u_sid);

	void AddHongBaoIndex(int _sys_type, const HongBaoItem & _hongbao_item, const UniqueServerID & u_sid);
	void AddHongBaoItem(int _sys_type, const HongBaoItem & _hongbao_item, const UniqueServerID & u_sid);

	// 从主服向隐藏服同步红包信息
	void SyncAllDataToHidden(int _sys_type);																	// 整表
	void SyncDataToHidden(int _sys_type, int _sync_status, const HongBaoItem & _hongbao_item, int _index = 0);	// 单个红包信息
	void SyncReqGiveRetToHidden(int _uid, int _ret, int _hongbao_id = 0, int _hongbao_sys_type = -1, int _hongbao_type = -1, char * _greeting = NULL);// 同步发红包操作结果到隐藏服
	void SyncReqGetRetToHidden(int _uid, int _ret, int _hongbao_id, int _hongbao_sys_type, int _val);		// 同步领红包操作结果到隐藏服

	// 在隐藏服处理来源于主服的同步信息
	void InitHongBaoInfoFromGame(crossgameprotocal::GameHiddenSyncHongBaoInfo * _hongbao_info);
	void UpdataHongBaoInfoFromGame(crossgameprotocal::GameHiddenSyncHongBaoInfo * _hongbao_info);

	// 获取可领取红包列表
	void SendHongBaoList(Role * _role, int _hongbao_sys_type);
	void SendHongBaoListByGuildHongBao(Role * _role);
	// 发送可领取红包列表变更
	void SendHongBaoListChange(int _hongbao_sys_type, const HongBaoItem & _hongbao_item, int _change_type);
	void SendHongBaoListChangeByGuildHongBao(const HongBaoItem & _hongbao_item,  const Protocol::SCHongBaoGetListChange & change);
	// 发送单个红包基础信息
	void SendHongBaoBaseInfo(Role * _role, int _hongbao_sys_type, int _hongbao_id);
	bool CanGetHongBaoBaseInfo(Role * _role, const HongBaoItem & _hongbao_item);
	// 发送单个红包基础信息变更
	void SendHongBaoBaseInfoChange(Role * _role, const HongBaoItem & _hongbao_item, int _change_type, int _change_index = 0);
	void SendHongBaoBaseInfoChangeByGuildHongBao(const HongBaoItem & _hongbao_item, const  Protocol::SCHongBaoBaseInfoChange & _change);

	// 首充获得红包
	void FirstChongZhi(Role * _role);
	// 由充值来获得可发送的公会红包次数
	void AddGuildHongBaoTimesByChongZhi(Role * _role, long long _chong_zhi_gold);
	int GetGuildHongBaoType(long long _chong_zhi_gold);

	void AddHongBaoTimesByChongZhi(Role * _role , long long _chong_zhi_gold);
	// 上面的调用下面
	void AddHongBaoTimesByChongZhi(Role * _role, long long _chong_zhi_gold , int _type);
	// 由累计充值来获得可发送的公会红包次数
	void AddHongBaoTimesByLeiJiChongZhi(Role * _role, long long _chong_zhi_gold);
	// 上面的调用下面
	void AddHongBaoTimesByLeiJiChongZhi(Role * _role, long long _chong_zhi_gold, long long _lei_chong_gold , int _type);
	// 是否是充值档位
	bool IsChongZhiType(int _type);
	// 是否是累充档位
	bool IsLeiJiChongZhiType(int _type);
	// _hong_bao_type 非红包sys_type
	void AddHongBaoTimes(Role * _role, int _hong_bao_type, const char * _reason, int _times = 1);


	// 领红包
	void GetHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_id);
	// 在主服领红包
	void GetHongBaoByGameWorld(Role * _role, int _hongbao_sys_type, int _hongbao_id);
	// 在隐藏服领红包
	void GetHongBaoByHidden(Role * _role, int _hongbao_sys_type, int _hongbao_id);
	// 处理来至隐藏服的领红包操作
	void GetHongBaoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req);
	// 能否领取红包
	bool CanGetHongBao(Role * _role, int _hongbao_sys_type, const HongBaoItem & _hongbao_item);
	// 能否领取红包 处理来至隐藏服的领红包操作
	bool CanGetHongBaoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req, const HongBaoItem & _hongbao_item);
	// 能否领取家族红包
	bool CanGetHongBaoByGuildHongBao(Role * _role, const HongBaoItem::InfoData & _info_data);
	//  能否领取家族红包 处理来至隐藏服的领红包操作
	bool CanGetHongBaoByGuildHongBaoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req, const HongBaoItem::InfoData & _info_data);
	// 更新红包基础信息
	void UpdataHongBaoBaseInfo(Role * _role, HongBaoItem::RewardInfo & _hongbao_reward_info_item);
	// 更新红包基础信息  处理来至隐藏服的领红包操作
	void UpdataHongBaoBaseInfoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req, HongBaoItem::RewardInfo & _hongbao_reward_info_item);
	// 检查红包是否已经领取完
	void CheckHongBaoGetInfo(HongBaoItem & _hongbao_item);

	// 发红包
	void GiveHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting);
	// 在主服发红包
	void GiveHongBaoByGameWorld(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting);
	// 在隐藏服发红包
	void GiveHongBaoByHidden(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting);
	// 处理来至隐藏服发红包的操作
	void  GiveHongBaoFromHidden(const GiveHongBaoUserInfo & _info);
	// 能否发红包
	bool CanGiveHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_type);
	// 能否发公会红包
	bool CanGiveHongBaoByGuildHongBao(Role * _role, int _hongbao_type);
	// 新建一个红包
	bool CreateNewHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting, HongBaoItem & _out_item);
	// 新建一个红包 处理来至隐藏服的发红包的操作
	bool CreateNewHongBaoFromHidden(const GiveHongBaoUserInfo & _info, HongBaoItem & _out_item);
	// 新建一个公会红包
	bool CreateNewHongBaoByGuildHongBao(int _guild_id, HongBaoItem & _out_item);
	// 计算新红包的数值
	void CalsNewHongBaoVal(HongBaoItem & _out_item, unsigned int _invalid_times, int _redbag_max, int _min_val, int _max_val, int _total, int _change_val);
	// 获取红包所对应的聊天频道
	int GetChatChannel(int _sys_type);
	// 发红包广播
	void OnHongBaoNotifyToChat(Role * _role, int _sys_type, int _type, int _hongbao_id, char * _greeting);

	int m_data_state;
	bool m_change_flag[HONGBAO_SYS_TYPE_MAX];
	bool m_load_flag[HONGBAO_SYS_TYPE_MAX];

	int m_hongbao_id_auto_increment[HONGBAO_SYS_TYPE_MAX];							// 红包ID自增

	std::set < LL_UniqueHongBaoID >  m_dirty_mark[HONGBAO_SYS_TYPE_MAX];
	std::set < LL_UniqueHongBaoID >  m_old_state[HONGBAO_SYS_TYPE_MAX];

	//各服务器红包信息
	std::map<UniqueServerID, HongBaoMap >m_hong_bao_arr[HONGBAO_SYS_TYPE_MAX];			// 总的红包信息表 ,加usid主要是为了跨服里获取

	//查询用表 , 创建或者删除家族红包时 , 应当同步修改此表 
	std::map<UniqueServerID, HongBaoGuildMap > m_hong_bao_guild_map;					// 加usid主要是为了跨服里获取

	//失效查询表
	std::map<unsigned int, std::set<LL_UniqueHongBaoID> > m_invaild_map[HONGBAO_SYS_TYPE_MAX];			// 失效的时间 , 红包unique_ID

	////////////////////////
	// 合服数据处理用的 , 处理完后就没用了
	std::vector<HongBaoItem> m_delay_data_vec;
	std::set < LL_UniqueHongBaoID > m_delete_merge[HONGBAO_SYS_TYPE_MAX];
	////////////////////////

};

#endif  // __HONG_BAO_MANAGER_HPP__