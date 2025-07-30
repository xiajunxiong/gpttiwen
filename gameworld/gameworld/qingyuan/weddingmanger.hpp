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
	//uid����λ�� map<uid, save_index>
	typedef std::map<int, int> UidIndexMap;

	//������Ϣ
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

		unsigned int apply_time;//����ʱ��
		int uid;				//������uid
	};
	//����set <��Ϣ>
	typedef std::set<ApplyInfo> ApplyTimeSet;
	//������uidmap <uid, ������Ϣ>
	typedef std::map<int, ApplyInfo> ApplyUidMap;
public:
	WeddingInfo();
	~WeddingInfo();

	//��Ч
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

	//�������
	int GetInviteNum();
	int AddInvite(int invite_uid);
	bool BeInvite(int invite_uid);

	//�������  ע ������ʧЧ
	int AddApply(int apply_uid);
	//�Ƴ�����	ע ������ʧЧ
	int RemoveApply(int apply_uid);
	bool HasApply(int apply_uid);

	enum ErrorType
	{
		ERROR_TYPE_SUC = 0,	//�ɹ�
		ERROR_TYPE_IN_LIST,	//�����б���
		ERROR_TYPE_NO_LIST,	//�����б���
		ERROR_TYPE_FULL,	//�б�����

		ERROR_TYPE_MAX,
	};

	void Reset();
	void SetDataStatus(int status) { m_param.change_state = status;};
	int GetDataStatus() { return m_param.change_state; };

private:

	WeddingFB::WeddingInfoDBParam m_param;


	//����uid����map
	UidIndexMap m_invite_save_map;


	//������		����
	ApplyUidMap m_apply_uid_map;
	ApplyTimeSet m_apply_set;
};

class WeddingManger
{
	friend class RMIInitWeddingBackObjectImpl;
	int m_is_load_finish;		//���ݼ������

public:
	static WeddingManger & Instance();

	//�����������
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
	//��Ҳ���
	void OnReq(Role *role, int type, int param_1, int param_2);
	//������Ļ
	void SendFBBarrage(Role *role, const char * chat, unsigned int chat_len);
	//���ת��
	void SendGameHiddenRoleReq(int uid, int type, int param_1 = 0, int param_2 = 0);
	void GameHiddenRoleReq(crossgameprotocal::GameHiddenWeddingRoleReq *req);

	//֪ͨ����
	void SyncLocal(int type, int p_1 = 0, int p_2 = 0, int p_3 = 0);

	//Get  ע  RemoveWeddingInfo ��ʧЧ
	WeddingInfo * GetWeddingInfo(WeddingID wedding_id);
	WeddingInfo * GetWeddingInfoByUid(int uid);
	WeddingInfo * GetWeddingInfoByBegin(time_t begin_time);
	WeddingInfo * GetWeddingInfoByIndex(int save_index);


public:
	void OnWeddingFinish(WeddingID wedding_id);

	/////////////
	//Ѳ��  �����������
	//Ԥ��
	void RegistXunYou(Role *role, int xun_you_type);

	//����Ѳ��
	int CreatXunYouCar(WeddingID wedding_id);

	//����
	void GMCreatXunYouCar(int uid, int xun_you_type);
	WeddingID GMOpenWedding(Role *role);

private:
	//����--ԤԼ
	bool WeddingRegist(Role *host_role, time_t regist_wedding_time);
	//����--���ԤԼ
	bool WeddingRegistOff(Role *host_role);

	//����--����
	void WeddingInvite(Role *host_role, int invite_uid);
	//����--����
	void WeddingApply(Role *_role, int wedding_id);
	//����--���봦��
	void WeddingApplyMark(Role *host_role, int mark_uid, bool mark_invite);

	//����--������������
	void WeddingBuyInviteSize(Role *_role);

	//֪ͨ
	//ԤԼ�б�
	void SendRegistList(Role *role);
	//Ҫ���б�
	void SendInviteList(Role *role);
	void SendInviteAdd(WeddingID wedding_id, int add_uid, int is_total = 0);
	//�����б�
	void SendApplyList(Role *role);
	void SendApplyAdd(WeddingID wedding_id, int add_uid);
	//�ı�֪ͨ
	void SendWeddingChange(Role * role, int change_type, int param_1);

	//���͸�������
	void SendToSubRole(const void * data, unsigned int length);

	//������Ϣ(�͸�˫��)
	void SendCommonInfo(WeddingID wedding_id);

	//����--�����㲥
	void SendWeddingStatusSync(int to_role_id, WeddingID wedding_id);
	//����--չʾ��Ϣ(���뺯)
	void SendWeddingShow(int to_role_id, WeddingID wedding_id);

private:
	//////////////////
	//����
	//����--ԤԼ��Ϣ
	void PubSubRegist(Role *role);
	//�Ƴ�����		ע: ������ʧЧ
	void PubSubRemove(int uid);
	//����--ʱ����	ע: ������ʧЧ
	//	//�ͻ���:��ҪʧЧʱ�� 
	void PubSubCheckTime(time_t now_second);

	//ʱ��-uid key
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
	//ԤԼ������Ϣ<ʱ��-uid key, net_id>
	TimeUidMap m_regist_pub_sub_map;
	//ԤԼ������Ϣ<uid, ʧЧʱ��>
	UidTimeMap m_regist_uid_map;

private:
	//////////////////
	//����ԤԼ����
	//ע:������ʧЧ
	int AddWeddingInfo(WeddingFB::WeddingInfoDBParam &wedding_db_param, bool is_load_data = false);
	void RemoveWeddingInfo(int wedding_id);

	WeddingInfo m_wedding_info_[WeddingFB::WEDDING_INFO_SIZE];
	bool m_wedding_info_old_flag_[WeddingFB::WEDDING_INFO_SIZE];

	time_t m_next_save_time;

	//���񴢴�map <����id, ����λ��>
	typedef std::map<int, int>  WeddingIndexInfoMap;
	//����ʱ��multmap<������ʱ��, ����λ��>
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
	//����ԤԼ��� <uid, ����id>
	typedef std::map<int, WeddingID>  WeddingUidMap;

	WeddingIndexInfoMap m_wedding_id_map;	//���񴢴�map <����id, ����λ��>
	WeddingBeginKeyMap m_wedding_time_map;	//����ʱ��multmap<������ʱ��, ����λ��>
	WeddingUidMap m_wedding_uid_map;		//����ԤԼ��� <uid, ����id>
private:
	//////////////////
	//���翪��
	//����״̬���
	void WeddingStatusCheck(time_t now_second);

	void OpenWedding(int wedding_id);


	//���ڿ����Ļ����б�
	time_t m_next_check_time;
	typedef std::set<WeddingBeginKey> WeddingBeginKeySet;
	WeddingBeginKeySet m_wedding_begin_set;

	//����ȡ��
	void WeddingVaildCheck(time_t now_second);
	void TimeOutWedding(WeddingID wedding_id);
	time_t m_wedding_vaild_check_time;

private:
	WeddingManger();
	~WeddingManger();
};

//������Ϣ����
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
	//��ǰ���翪���б�
	std::set<WeddingID> m_open_set;

private:
	WeddingLocalManger();
	~WeddingLocalManger();
};


#endif