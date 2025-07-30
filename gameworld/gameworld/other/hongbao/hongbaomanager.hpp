#ifndef __HONG_BAO_MANAGER_HPP__
#define __HONG_BAO_MANAGER_HPP__

#include "servercommon/hongbaodef.hpp"
#include "protocol/msghongbao.hpp"
#include "servercommon/crossdef.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"

#include <map>

enum HONGBAO_DATA_STATUS
{
	HONGBAO_DATA_STATUS_INVALID = 0,						// ��ʼ״̬
	HONGBAO_DATA_STATUS_LOADING,							// ���ݼ����� 
	HONGBAO_DATA_STATUS_LOAD_FINISH,						// ���ݼ������ 
	HONGBAO_DATA_STATUS_MAX,
};

static const int HONGBAO_SAVE_INTERVAL = 30;				// ����ӻ�дһ��

class Role;

class HongBaoManager
{
	friend class RMIInitHongBaoBackObjectImpl;

public:
	static HongBaoManager & Instance();

	void Init(int _ret, int _hongbao_sys_type, HongBaoParam & _hongbao_param);

	void OnGameworldAccept();
	// ͬ��������Ϣ�����
	void OnSyncHongBaoInfoFromGame(crossgameprotocal::GameHiddenSyncHongBaoInfo * _hongbao_info);
	void OnSyncReqGiveRetFromGame(crossgameprotocal::GameHiddenSyncHongBaoReqGiveRet * _req_ret);
	void OnSyncReqGetRetFromGame(crossgameprotocal::GameHiddenSyncHongBaoReqGetRet * _req_ret);

	// ͬ�������Ϣ������
	// ������������ķ����
	void  OnHiddenGameSyncHongBaoReqGiveFromHidden(crossgameprotocal::HiddenGameSyncHongBaoReqGive * _give_msg);
	// �����������������
	void  OnHiddenGameSyncHongBaoReqGetFromHidden(crossgameprotocal::HiddenGameSyncHongBaoReqGet * _get_msg);
	// ������������Ĳ���ʧ����Ϣ [�ڿ���Ҳ������,Ȼ�󷢻���������]
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

	typedef std::map<int, HongBaoItem> HongBaoMap;									// < ���ID , �����Ϣ >
	typedef std::map<int, HongBaoItem>::iterator  HongBaoMapIt;
	typedef std::map<int, HongBaoItem>::reverse_iterator  HongBaoMapRIt;
	typedef std::map<UniqueServerID, HongBaoMap > HongBaoServerMap;
	typedef std::map<UniqueServerID, HongBaoMap >::iterator  HongBaoServerMapIt;

	typedef std::map<int, std::set< int > > HongBaoGuildMap;						// <����ID, ���ID>
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

	// �����������ط�ͬ�������Ϣ
	void SyncAllDataToHidden(int _sys_type);																	// ����
	void SyncDataToHidden(int _sys_type, int _sync_status, const HongBaoItem & _hongbao_item, int _index = 0);	// ���������Ϣ
	void SyncReqGiveRetToHidden(int _uid, int _ret, int _hongbao_id = 0, int _hongbao_sys_type = -1, int _hongbao_type = -1, char * _greeting = NULL);// ͬ�������������������ط�
	void SyncReqGetRetToHidden(int _uid, int _ret, int _hongbao_id, int _hongbao_sys_type, int _val);		// ͬ������������������ط�

	// �����ط�������Դ��������ͬ����Ϣ
	void InitHongBaoInfoFromGame(crossgameprotocal::GameHiddenSyncHongBaoInfo * _hongbao_info);
	void UpdataHongBaoInfoFromGame(crossgameprotocal::GameHiddenSyncHongBaoInfo * _hongbao_info);

	// ��ȡ����ȡ����б�
	void SendHongBaoList(Role * _role, int _hongbao_sys_type);
	void SendHongBaoListByGuildHongBao(Role * _role);
	// ���Ϳ���ȡ����б���
	void SendHongBaoListChange(int _hongbao_sys_type, const HongBaoItem & _hongbao_item, int _change_type);
	void SendHongBaoListChangeByGuildHongBao(const HongBaoItem & _hongbao_item,  const Protocol::SCHongBaoGetListChange & change);
	// ���͵������������Ϣ
	void SendHongBaoBaseInfo(Role * _role, int _hongbao_sys_type, int _hongbao_id);
	bool CanGetHongBaoBaseInfo(Role * _role, const HongBaoItem & _hongbao_item);
	// ���͵������������Ϣ���
	void SendHongBaoBaseInfoChange(Role * _role, const HongBaoItem & _hongbao_item, int _change_type, int _change_index = 0);
	void SendHongBaoBaseInfoChangeByGuildHongBao(const HongBaoItem & _hongbao_item, const  Protocol::SCHongBaoBaseInfoChange & _change);

	// �׳��ú��
	void FirstChongZhi(Role * _role);
	// �ɳ�ֵ����ÿɷ��͵Ĺ���������
	void AddGuildHongBaoTimesByChongZhi(Role * _role, long long _chong_zhi_gold);
	int GetGuildHongBaoType(long long _chong_zhi_gold);

	void AddHongBaoTimesByChongZhi(Role * _role , long long _chong_zhi_gold);
	// ����ĵ�������
	void AddHongBaoTimesByChongZhi(Role * _role, long long _chong_zhi_gold , int _type);
	// ���ۼƳ�ֵ����ÿɷ��͵Ĺ���������
	void AddHongBaoTimesByLeiJiChongZhi(Role * _role, long long _chong_zhi_gold);
	// ����ĵ�������
	void AddHongBaoTimesByLeiJiChongZhi(Role * _role, long long _chong_zhi_gold, long long _lei_chong_gold , int _type);
	// �Ƿ��ǳ�ֵ��λ
	bool IsChongZhiType(int _type);
	// �Ƿ����۳䵵λ
	bool IsLeiJiChongZhiType(int _type);
	// _hong_bao_type �Ǻ��sys_type
	void AddHongBaoTimes(Role * _role, int _hong_bao_type, const char * _reason, int _times = 1);


	// ����
	void GetHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_id);
	// ����������
	void GetHongBaoByGameWorld(Role * _role, int _hongbao_sys_type, int _hongbao_id);
	// �����ط�����
	void GetHongBaoByHidden(Role * _role, int _hongbao_sys_type, int _hongbao_id);
	// �����������ط�����������
	void GetHongBaoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req);
	// �ܷ���ȡ���
	bool CanGetHongBao(Role * _role, int _hongbao_sys_type, const HongBaoItem & _hongbao_item);
	// �ܷ���ȡ��� �����������ط�����������
	bool CanGetHongBaoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req, const HongBaoItem & _hongbao_item);
	// �ܷ���ȡ������
	bool CanGetHongBaoByGuildHongBao(Role * _role, const HongBaoItem::InfoData & _info_data);
	//  �ܷ���ȡ������ �����������ط�����������
	bool CanGetHongBaoByGuildHongBaoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req, const HongBaoItem::InfoData & _info_data);
	// ���º��������Ϣ
	void UpdataHongBaoBaseInfo(Role * _role, HongBaoItem::RewardInfo & _hongbao_reward_info_item);
	// ���º��������Ϣ  �����������ط�����������
	void UpdataHongBaoBaseInfoFromHidden(const crossgameprotocal::HiddenGameSyncHongBaoReqGet * _req, HongBaoItem::RewardInfo & _hongbao_reward_info_item);
	// ������Ƿ��Ѿ���ȡ��
	void CheckHongBaoGetInfo(HongBaoItem & _hongbao_item);

	// �����
	void GiveHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting);
	// �����������
	void GiveHongBaoByGameWorld(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting);
	// �����ط������
	void GiveHongBaoByHidden(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting);
	// �����������ط�������Ĳ���
	void  GiveHongBaoFromHidden(const GiveHongBaoUserInfo & _info);
	// �ܷ񷢺��
	bool CanGiveHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_type);
	// �ܷ񷢹�����
	bool CanGiveHongBaoByGuildHongBao(Role * _role, int _hongbao_type);
	// �½�һ�����
	bool CreateNewHongBao(Role * _role, int _hongbao_sys_type, int _hongbao_type, char * _greeting, HongBaoItem & _out_item);
	// �½�һ����� �����������ط��ķ�����Ĳ���
	bool CreateNewHongBaoFromHidden(const GiveHongBaoUserInfo & _info, HongBaoItem & _out_item);
	// �½�һ��������
	bool CreateNewHongBaoByGuildHongBao(int _guild_id, HongBaoItem & _out_item);
	// �����º������ֵ
	void CalsNewHongBaoVal(HongBaoItem & _out_item, unsigned int _invalid_times, int _redbag_max, int _min_val, int _max_val, int _total, int _change_val);
	// ��ȡ�������Ӧ������Ƶ��
	int GetChatChannel(int _sys_type);
	// ������㲥
	void OnHongBaoNotifyToChat(Role * _role, int _sys_type, int _type, int _hongbao_id, char * _greeting);

	int m_data_state;
	bool m_change_flag[HONGBAO_SYS_TYPE_MAX];
	bool m_load_flag[HONGBAO_SYS_TYPE_MAX];

	int m_hongbao_id_auto_increment[HONGBAO_SYS_TYPE_MAX];							// ���ID����

	std::set < LL_UniqueHongBaoID >  m_dirty_mark[HONGBAO_SYS_TYPE_MAX];
	std::set < LL_UniqueHongBaoID >  m_old_state[HONGBAO_SYS_TYPE_MAX];

	//�������������Ϣ
	std::map<UniqueServerID, HongBaoMap >m_hong_bao_arr[HONGBAO_SYS_TYPE_MAX];			// �ܵĺ����Ϣ�� ,��usid��Ҫ��Ϊ�˿�����ȡ

	//��ѯ�ñ� , ��������ɾ��������ʱ , Ӧ��ͬ���޸Ĵ˱� 
	std::map<UniqueServerID, HongBaoGuildMap > m_hong_bao_guild_map;					// ��usid��Ҫ��Ϊ�˿�����ȡ

	//ʧЧ��ѯ��
	std::map<unsigned int, std::set<LL_UniqueHongBaoID> > m_invaild_map[HONGBAO_SYS_TYPE_MAX];			// ʧЧ��ʱ�� , ���unique_ID

	////////////////////////
	// �Ϸ����ݴ����õ� , ��������û����
	std::vector<HongBaoItem> m_delay_data_vec;
	std::set < LL_UniqueHongBaoID > m_delete_merge[HONGBAO_SYS_TYPE_MAX];
	////////////////////////

};

#endif  // __HONG_BAO_MANAGER_HPP__