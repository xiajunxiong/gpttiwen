#ifndef __ROLE_CRYSTAL_FB_H__
#define __ROLE_CRYSTAL_FB_H__

#include "servercommon/rolecrystalfbdef.h"
#include "protocol/msgcrystalfb.h"
#include <map>

//使用体力类型
enum ENTER_ENERGY_CHANGE_TYPE
{
	//增加
	ENTER_ENERGY_CHANGE_TYPE_BUY = 0,					//购买体力
	ENTER_ENERGY_CHANGE_TYPE_USE_ITEM = 1,				//使用道具
	ENTER_ENERGY_CHANGE_TYPE_OTHER = 2,					//一些其他模块任务添加体力
	//减少
	ENTER_ENERGY_CHANGE_TYPE_ADVANCE_CRYSTAL = 3,		//水晶副本(普通)提前消耗
	ENTER_ENERGY_CHANGE_TYPE_CRYSTAL_PASS = 4,			//水晶副本(普通)实际通关
	ENTER_ENERGY_CHANGE_TYPE_CRYSTAL_FAST = 5,			//水晶副本(普通)快速扫荡
	ENTER_ENERGY_CHANGE_TYPE_MATERIAL = 6,				//玉虚境(通关+快速扫荡)
	ENTER_ENERGY_CHANGE_TYPE_PET_GOD = 7,				//神印之地
	ENTER_ENERGY_CHANGE_TYPE_PET_MONOPOLY = 8,			//灵兽岛
	ENTER_ENERGY_CHANGE_TYPE_ADVANCE_DREAM_NOTES = 9,	//梦渊笔录提前消耗
	ENTER_ENERGY_CHANGE_TYPE_DREAM_NOTES_PASS = 10,		//梦渊笔录实际通关
	ENTER_ENERGY_CHANGE_TYPE_DREAM_NOTES_FAST = 11,		//梦渊笔录快速扫荡
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

	int GetPuTongJingYingPlayTimes();			//获取普通+精英次数
	void AddPuTongJingYingPlayTimes(int times = 1);

	int GetJingYingPlayTimes();			//获取精英的次数
	void AddJingYingPlayTimes(int times = 1);

	bool HasUseTimes();
	int GetXiYouUseTimes();			//获取使用的次数
	void UseXiYouPlayTimes(int times = 1);
	int GetXiYouPlayTimes();			//获取稀有的总次数
	int GetXiYourRewardPlayTimes();		//获取当天获得稀有奖励次数
	void UseXiYouRewardPlayTimes(int times = 1);
	bool AddXiYouPlayTimesByNorexitem(int times = 1);

	int GetPuTongTimesByFbSeq(int fb_seq);	//获取普通类型对应关卡通关次数(每日重置)

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
