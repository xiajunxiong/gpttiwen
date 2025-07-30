#ifndef __WEDDING_MANGER_HPP__
#define __WEDDING_MANGER_HPP__

#include "servercommon/weddingdef.hpp"
#include <set>

class Role;
namespace crossgameprotocal 
{
	struct GameHiddenWeddingRoleReq;
	struct HiddenGameWeddingInfoSync;
};

class WeddingInfo
{
public:
	//uid储存位置 map<uid, save_index>
	typedef std::map<int, int> UidIndexMap;

	//申请信息
	struct ApplyInfo
	{
		ApplyInfo()
		{
			apply_time = 0;
			uid = 0;
		}

		bool operator<(const ApplyInfo &_other) const
		{
			if (apply_time != _other.apply_time)
			{
				return apply_time < _other.apply_time;
			}
			return uid < _other.uid;
		}

		unsigned int apply_time;//申请时间
		int uid;				//申请者uid
	};
	//申请set <信息>
	typedef std::set<ApplyInfo> ApplyTimeSet;
	//申请者uidmap <uid, 申请信息>
	typedef std::map<int, ApplyInfo> ApplyUidMap;
public:
	WeddingInfo();
	~WeddingInfo();

	//有效
	bool IsValid() const { return m_param.IsValid(); };
	void InitDBParam(WeddingFB::WeddingInfoDBParam &init_param);
	void GetDBParam(WeddingFB::WeddingInfoDBParam &db_param) const { db_param = m_param; };
	
	WeddingFB::HostInfo * GetHost(int uid);
	int GetHostUid(int index) const;

	int GetStatus() const { return m_param.is_finsih; };
	void SetStatus(int finish_suatus);

	int GetSaveIndex() const { return m_param.save_index; };
	WeddingID GetWeddingID() const { return m_param.wedding_id; };
	time_t GetBeginTime() const { return m_param.begin_time; };
	const WeddingFB::HostInfo * GetHostInfoList(int & out_count) const;
	const WeddingFB::InviteInfo &GetInviteList() const;
	const ApplyTimeSet &GetApplyTimeSet() const;

	WeddingFB::CommonInfo &GetCommonInfo(bool change);

	//添加邀请
	int GetInviteNum();
	int AddInvite(int invite_uid);
	bool BeInvite(int invite_uid);

	//添加申请  注 迭代器失效
	int AddApply(int apply_uid);
	//移除申请	注 迭代器失效
	int RemoveApply(int apply_uid);
	bool HasApply(int apply_uid);

	enum ErrorType
	{
		ERROR_TYPE_SUC = 0,	//成功
		ERROR_TYPE_IN_LIST,	//已在列表中
		ERROR_TYPE_NO_LIST,	//不在列表中
		ERROR_TYPE_FULL,	//列表已满

		ERROR_TYPE_MAX,
	};

	void Reset();
	void SetDataStatus(int status) { m_param.change_state = status;};
	int GetDataStatus() { return m_param.change_state; };

private:

	WeddingFB::WeddingInfoDBParam m_param;


	//邀请uid储存map
	UidIndexMap m_invite_save_map;


	//申请者		不存
	ApplyUidMap m_apply_uid_map;
	ApplyTimeSet m_apply_set;
};

class WeddingManger
{
	friend class RMIInitWeddingBackObjectImpl;
	int m_is_load_finish;		//数据加载完成

public:
	static WeddingManger & Instance();

	//加载完成数据
	bool IsLoadFinish() { return 0 != m_is_load_finish; };

	void OnServerStart();
	void OnServerStop();

	void LoadWeddingList(int _ret, WeddingInfoDBList & _param);
	void SaveWeddingList();

	void Update(unsigned long interval, time_t now_second);

	void OnRoleLogIn(Role * role);
	void OnRoleLogOut(Role * role);

	void OnGameServerConnect(std::set<int> server_id);


public:
	//玩家操作
	void OnReq(Role *role, int type, int param_1, int param_2);
	//场景弹幕
	void SendFBBarrage(Role *role, const char * chat, unsigned int chat_len);
	//跨服转接
	void SendGameHiddenRoleReq(int uid, int type, int param_1 = 0, int param_2 = 0);
	void GameHiddenRoleReq(crossgameprotocal::GameHiddenWeddingRoleReq *req);

	//通知主服
	void SyncLocal(int type, int p_1 = 0, int p_2 = 0, int p_3 = 0);

	//Get  注  RemoveWeddingInfo 后失效
	WeddingInfo * GetWeddingInfo(WeddingID wedding_id);
	WeddingInfo * GetWeddingInfoByUid(int uid);
	WeddingInfo * GetWeddingInfoByBegin(time_t begin_time);
	WeddingInfo * GetWeddingInfoByIndex(int save_index);


public:
	void OnWeddingFinish(WeddingID wedding_id);

	/////////////
	//巡游  婚宴结束后开启
	//预订
	void RegistXunYou(Role *role, int xun_you_type);

	//创建巡游
	int CreatXunYouCar(WeddingID wedding_id);

	//测试
	void GMCreatXunYouCar(int uid, int xun_you_type);
	WeddingID GMOpenWedding(Role *role);

private:
	//婚宴--预约
	bool WeddingRegist(Role *host_role, time_t regist_wedding_time);
	//婚宴--解除预约
	bool WeddingRegistOff(Role *host_role);

	//婚宴--邀请
	void WeddingInvite(Role *host_role, int invite_uid);
	//婚宴--申请
	void WeddingApply(Role *_role, int wedding_id);
	//婚宴--申请处理
	void WeddingApplyMark(Role *host_role, int mark_uid, bool mark_invite);

	//婚宴--购买邀请上限
	void WeddingBuyInviteSize(Role *_role);

	//通知
	//预约列表
	void SendRegistList(Role *role);
	//要请列表
	void SendInviteList(Role *role);
	void SendInviteAdd(WeddingID wedding_id, int add_uid, int is_total = 0);
	//申请列表
	void SendApplyList(Role *role);
	void SendApplyAdd(WeddingID wedding_id, int add_uid);
	//改变通知
	void SendWeddingChange(Role * role, int change_type, int param_1);

	//发送给订阅者
	void SendToSubRole(const void * data, unsigned int length);

	//基础信息(送给双方)
	void SendCommonInfo(WeddingID wedding_id);

	//婚宴--开启广播
	void SendWeddingStatusSync(int to_role_id, WeddingID wedding_id);
	//婚宴--展示信息(邀请函)
	void SendWeddingShow(int to_role_id, WeddingID wedding_id);

private:
	//////////////////
	//订阅
	//订阅--预约信息
	void PubSubRegist(Role *role);
	//移除订阅		注: 迭代器失效
	void PubSubRemove(int uid);
	//订阅--时间检测	注: 迭代器失效
	//	//客户端:不要失效时间 
	void PubSubCheckTime(time_t now_second);

	//时间-uid key
	struct TimeUidKey
	{
		TimeUidKey()
		{
			valid_time = 0;
			uid = 0;
		}

		bool operator < (const TimeUidKey &other_key) const
		{
			if (other_key.valid_time != valid_time)
			{
				return valid_time < other_key.valid_time;
			}
			return uid < other_key.uid;
		}

		time_t valid_time;
		int uid;
	};
	typedef std::map<TimeUidKey, GSNetID> TimeUidMap;
	typedef std::map<int, time_t> UidTimeMap;
	//预约订阅信息<时间-uid key, net_id>
	TimeUidMap m_regist_pub_sub_map;
	//预约订阅信息<uid, 失效时间>
	UidTimeMap m_regist_uid_map;

private:
	//////////////////
	//婚宴预约数据
	//注:迭代器失效
	int AddWeddingInfo(WeddingFB::WeddingInfoDBParam &wedding_db_param, bool is_load_data = false);
	void RemoveWeddingInfo(int wedding_id);

	WeddingInfo m_wedding_info_[WeddingFB::WEDDING_INFO_SIZE];
	bool m_wedding_info_old_flag_[WeddingFB::WEDDING_INFO_SIZE];

	time_t m_next_save_time;

	//婚礼储存map <婚礼id, 储存位置>
	typedef std::map<int, int>  WeddingIndexInfoMap;
	//婚礼时间multmap<婚礼开启时间, 储存位置>
	struct WeddingBeginKey
	{
		WeddingBeginKey()
		{
			begin_time = 0;
			wedding_id = 0;
		}

		bool operator < (const WeddingBeginKey &other_key) const
		{
			if (other_key.begin_time != begin_time)
			{
				return begin_time < other_key.begin_time;
			}
			return wedding_id < other_key.wedding_id;
		}

		time_t begin_time;
		int wedding_id;
	};
	typedef std::map<WeddingBeginKey, int> WeddingBeginKeyMap;
	//婚礼预约标记 <uid, 婚礼id>
	typedef std::map<int, WeddingID>  WeddingUidMap;

	WeddingIndexInfoMap m_wedding_id_map;	//婚礼储存map <婚礼id, 储存位置>
	WeddingBeginKeyMap m_wedding_time_map;	//婚礼时间multmap<婚礼开启时间, 储存位置>
	WeddingUidMap m_wedding_uid_map;		//婚礼预约标记 <uid, 婚礼id>
private:
	//////////////////
	//婚宴开启
	//婚宴状态检查
	void WeddingStatusCheck(time_t now_second);

	void OpenWedding(int wedding_id);


	//正在开启的婚宴列表
	time_t m_next_check_time;
	typedef std::set<WeddingBeginKey> WeddingBeginKeySet;
	WeddingBeginKeySet m_wedding_begin_set;

	//过期取消
	void WeddingVaildCheck(time_t now_second);
	void TimeOutWedding(WeddingID wedding_id);
	time_t m_wedding_vaild_check_time;

private:
	WeddingManger();
	~WeddingManger();
};

//本服信息缓存
class WeddingLocalManger
{
public:
	static WeddingLocalManger & Instance();

	void OnRoleLogIn(Role* role);
	void SendGameHiddenRoleReq(int uid, int type, int param_1 = 0, int param_2 = 0);


	void OnSycnWeddingInfo(crossgameprotocal::HiddenGameWeddingInfoSync *req);

	void AddOpenWedding(int wedding_id);
	void RemoveOpenWedding(int wedding_id);
private:
	//当前婚宴开启列表
	std::set<WeddingID> m_open_set;

private:
	WeddingLocalManger();
	~WeddingLocalManger();
};


#endif