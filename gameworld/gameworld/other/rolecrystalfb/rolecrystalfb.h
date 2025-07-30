#ifndef __ROLE_CRYSTAL_FB_H__
#define __ROLE_CRYSTAL_FB_H__

#include "servercommon/rolecrystalfbdef.h"
#include "protocol/msgcrystalfb.h"
#include <map>

//ʹ����������
enum ENTER_ENERGY_CHANGE_TYPE
{
	//����
	ENTER_ENERGY_CHANGE_TYPE_BUY = 0,					//��������
	ENTER_ENERGY_CHANGE_TYPE_USE_ITEM = 1,				//ʹ�õ���
	ENTER_ENERGY_CHANGE_TYPE_OTHER = 2,					//һЩ����ģ�������������
	//����
	ENTER_ENERGY_CHANGE_TYPE_ADVANCE_CRYSTAL = 3,		//ˮ������(��ͨ)��ǰ����
	ENTER_ENERGY_CHANGE_TYPE_CRYSTAL_PASS = 4,			//ˮ������(��ͨ)ʵ��ͨ��
	ENTER_ENERGY_CHANGE_TYPE_CRYSTAL_FAST = 5,			//ˮ������(��ͨ)����ɨ��
	ENTER_ENERGY_CHANGE_TYPE_MATERIAL = 6,				//���龳(ͨ��+����ɨ��)
	ENTER_ENERGY_CHANGE_TYPE_PET_GOD = 7,				//��ӡ֮��
	ENTER_ENERGY_CHANGE_TYPE_PET_MONOPOLY = 8,			//���޵�
	ENTER_ENERGY_CHANGE_TYPE_ADVANCE_DREAM_NOTES = 9,	//��Ԩ��¼��ǰ����
	ENTER_ENERGY_CHANGE_TYPE_DREAM_NOTES_PASS = 10,		//��Ԩ��¼ʵ��ͨ��
	ENTER_ENERGY_CHANGE_TYPE_DREAM_NOTES_FAST = 11,		//��Ԩ��¼����ɨ��
};


class RoleModuleManager;
class RoleCrystalFb
{
public:
	RoleCrystalFb();
	~RoleCrystalFb();

	void Init(RoleModuleManager * mgr, const RoleCrystalParam& param);
	void GetInitParam(RoleCrystalParam* param);

	void OnLogin(unsigned int login_timestamp, unsigned int last_save_timestamp);
	void Update(time_t now_second);
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnMonthChange();

	void OnBuyEnergy();
	void UseFastPassFb(int fb_seq,int times);
	int GetCrystalFbEnergy() { return m_param.role_enter_energy; }
	bool UseCrystalFbEnergy(int energy_value, const char* reason, int enter_energy_change_type);
	void UseCrystalFbEnergyItem(ItemID item_id, short num);
	bool AddCrystalFbEnergyByItem(int energy_value, const char* reason);
	void AddCrystalFbEnergy(int energy_value, const char* reason, int enter_energy_change_type, bool is_notice = true);
	void SendCrystalFbEnergy();

	void OnRoleChangeProfOrEneryChange(bool is_send = false);			

	int GetPuTongJingYingPlayTimes();			//��ȡ��ͨ+��Ӣ����
	void AddPuTongJingYingPlayTimes(int times = 1);

	int GetJingYingPlayTimes();			//��ȡ��Ӣ�Ĵ���
	void AddJingYingPlayTimes(int times = 1);

	bool HasUseTimes();
	int GetXiYouUseTimes();			//��ȡʹ�õĴ���
	void UseXiYouPlayTimes(int times = 1);
	int GetXiYouPlayTimes();			//��ȡϡ�е��ܴ���
	int GetXiYourRewardPlayTimes();		//��ȡ������ϡ�н�������
	void UseXiYouRewardPlayTimes(int times = 1);
	bool AddXiYouPlayTimesByNorexitem(int times = 1);

	int GetPuTongTimesByFbSeq(int fb_seq);	//��ȡ��ͨ���Ͷ�Ӧ�ؿ�ͨ�ش���(ÿ������)

	void SetFristPassCrystalFb(int fb_seq);
	bool HasFristPassCrystalFb(int fb_seq);

	void GetRewardPassCrystalFb(int fb_seq);

	void OnRoleEnterScene();
	void OnRoleEnterFBUseEnergy(int use_energy_num);
	void OnRoleEnterFBUseItem(const ItemConfigData& item);
	void OnRoleGetFBBossReward(int seq);
	void OnRoleBuyNormalFBTimes(int fb_seq);
	bool CanChallengeNormalFB(int seq);

	void OverFlowEnergyCal();

	void SendInfo();

	void* operator new(size_t c);
	void operator delete(void* m);
private:
	void ModifyCrystalFBEnergy(int energy_value, const char* reason, int enter_energy_change_type, bool is_notice = true);
	void GetNormalFBRecord(ARG_OUT int* record_num, ARG_OUT CrystalFBNormalRecord* record_list, int max_num);
	void GetNormalFBRecordPro(ARG_OUT int* record_num, ARG_OUT Protocol::SCCrystalFbInfo::CrystalFBNormalRecordPro* record_list, int max_num);
	void AddNormalFBRecord(int seq);
	
	RoleModuleManager * m_role_module_mgr;

	RoleCrystalParam m_param;
	std::map<int, int> m_index_by_monster;
};


#endif
