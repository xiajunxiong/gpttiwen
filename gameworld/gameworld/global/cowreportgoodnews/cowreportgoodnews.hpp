#ifndef  __COW_REPORT_GOOD_NEWS_HPP__
#define __COW_REPORT_GOOD_NEWS_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/commonsavedef.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/struct/cowreportgoodnewsparam.h"
#include "global/commonsave/commonsave.hpp"
#include "protocol/msgcowreportgoodnews.hpp"

#include <map>
#include <set>


enum COW_REPORT_GOOD_NEWS_DATA_STATUS
{
	COW_REPORT_GOOD_NEWS_DATA_STATUS_INVALID = 0,						// ��ʼ״̬
	COW_REPORT_GOOD_NEWS_DATA_STATUS_LOADING,							// ���ݼ����� 
	COW_REPORT_GOOD_NEWS_DATA_STATUS_LOAD_FINISH,						// ���ݼ������ 
	COW_REPORT_GOOD_NEWS_DATA_STATUS_MAX,
};

static const int COW_REPORT_GOOD_NEWS_SAVE_INTERVAL = 60;				// 1���ӻ�дһ��


class Role;

// �߻�˵�����ϵͳ���ǻ
class CowReportGoodNewsManager
{
	friend class RMIInitCowReportGoodNewsBuyListBackObjectImpl;
public:
	static CowReportGoodNewsManager & Instance();
	void InitParam(int _ret, CowReportGoodNewsBuyListParam & _param);
	void OnServerStart();
	void OnServerStop();
	bool IsLoadFinish() const;
	void Update(unsigned long interval, time_t now_second);
	void OnLogin(Role * _role);
	void OnCowReportGoodNewsOfflineEventByLogin(Role * _role, int _val);

	// �����������ط�����Ϣ   ��ҵĹ��������ǰ����֤
	void OnCowReportGoodNewsCanBuyFromHidden(crossgameprotocal::HiddenGameCowReportGoodNewsCheckCanBuy * _msg);
	// �����������ط�����Ϣ   ��ҵĹ�������
	void OnCowReportGoodNewsBuyItemFromHidden(crossgameprotocal::HiddenGameCowReportGoodNewsBuyItem * _msg);
	// �����������ط�����Ϣ
	void CowReportGoodNewsGetInfoFromHidden(crossgameprotocal::HiddenGameCowReportGoodNewsGetInfo * _get_info);

	// ����������������Ϣ  ��Ҷ�������ǰ����֤Ӧ��
	void OnCowReportGoodNewsCheckCanBuyAckFromGameWorld(crossgameprotocal::GameHiddenCowReportGoodNewsCheckCanBuyAck * _ack);
	// �����������ط�����Ϣ   ��ҵĹ�������Ӧ��
	void OnCowReportGoodNewsBuyItemAckFromGameWorld(crossgameprotocal::GameHiddenCowReportGoodNewsBuyItemAck * _ack);

	// ������ ���� ->���ط�  Ҳ���������ط� ->���� 
	void OnSyncCowReportGoodNewsBuyItemOfflineEvent(crossgameprotocal::SyncCowReportGoodNewsBuyItemOfflineEvent * _sync);

	// �ͻ��˲���
	void OnClientReq(Role * _role ,Protocol::CSCowReportGoodNewsClientReq * _req);

	// GM������ָ�� , �������� , Լ�����ؿ��(���������
	void GmCowReportGoodNewsResetData();

private:
	CowReportGoodNewsManager();
	virtual ~CowReportGoodNewsManager();

	void Save();
	void SaveParam();
	void ClearDirtyMark();
	void SetDirtyMark(int _role_id);
	void CheckLoadFinish();
	void ClearDataByFinish();

	void NoticeStatusChange();
	void NoticeStatusChangeByGameWorld();
	void NoticeStatusChangeToHidden();

	void GetInfoToGameWorld(int _role_id, int _type);

	//  ϵͳ��Ϣ
	void SendInfo(Role * _role);
	void SendInfoByGameWorld(Role * _role);
	void SendInfoToHidden(int _role_id);

	// ������Ϣ
	void SendRewardInfo(Role * _role);
	void SendRewardInfoByGameWorld(Role * _role);
	void SendRewardInfoToHidden(int _role_id);

	// ������Ϣ
	void SendBuyInfo(Role * _role);
	void SendBuyInfoByGameWorld(int _role_id);
	void GmResetSendBuyInfo();

	void SendCanBuyAckToHidden(int _role_id, int _ret);
	void SendBuyAckToHidden(int _role_id, int _ret);
	void SyncCowReportGoodNewsReturnGoldOfflineEvent(int _role_id, int _val);

	// ����״̬
	void CalcStatus(unsigned int now_second);
	// �Ƿ���չʾ��
	bool IsShowTime();
	// �Ƿ��ǹ�����
	bool IsBuyTimes();
	// �Ƿ��ǽ�����
	bool IsRewardTimes();
	// �Ƿ��Ѿ�����
	bool IsFinish();
	// �Ƿ�����
	bool IsBuy(int _role_id);

	//  ������Ʒ
	void OnBuyItem(Role * _role);
	void BuyItemByHidden(Role * _role);
	void BuyItemByGameWorld(Role * _role);
	void BuyItemToGameWorld(Role * _role);

	// ����
	void CalcReward();
	// ������
	void GiveReward();
	void GiveWinReward(std::set<int > & _set);
	void GiveLoseReward(const std::set<int > & _set);

private:
	int m_data_state;
	bool m_load_flag;
	// �Ƿ������ݷ��������
	bool m_change_flag;
	// ����Ϣ�䶯������б�
	std::set< int > m_dirty_mark;
	std::set< int > m_old_state;

	std::map<int , CowReportGoodNewsBuyInfo> m_buy_map;				// �����˵������Ϣ <uid , ������Ϣ>

	CommonSave * GetCommonSave() const;
	CommonSaveCowReportGoodNewsData * GetCommonSaveCowReportGoodNewsData( bool is_auto_create = true);
	void SetCommonSaveDirtyMark();
};

#endif	// __COW_REPORT_GOOD_NEWS_HPP__

