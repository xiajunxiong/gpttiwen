#ifndef __SMART_MOUNTS_SYSTEM_HPP__
#define __SMART_MOUNTS_SYSTEM_HPP__

#include "servercommon/smartmountssystemdef.hpp"
#include "other/attributelist.hpp"
#include "item/itemgriddata.h"

class RoleModuleManager;
class NorexItem;
class SmartMountsSystem
{
public:
	SmartMountsSystem();
	~SmartMountsSystem();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const SmartMountsSystemParam & param);
	void GetInitParam(SmartMountsSystemParam * param);
	const SmartMountsSystemParam GetParam() { return m_param; }
	void InitEnd();
	void OnLogin();
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc, AttributeList &skill_attr_list);
	void ReCalcPetAttr(int index = -1);
	bool IsPetDomination(unsigned long long item_unique_id);
	void GetDominationBaseAttr(unsigned long long item_unique_id, AttributeList & base_add);
	void GetBaseSkillList(unsigned long long item_unique_id, std::vector<SkillPro> * base_skill_list);
	int GetSkillOrSpCap();			//获得技能或特效评分
	int GetCurRideSmartMounts() { return m_param.ride_index; }	
	int GetUnlockOrMaxJewelryLevel(bool is_unlock);
	char GetCurRideSmartMountsImage();
	void OnFinishFunGuide();														//完成指引

	void OnActiveSmartMounts(int index);											//解锁灵骑
	void OnLevelUpSmartMounts(int index);											//升级灵骑
	void OnBreakLevelSmartMounts(int index);										//灵骑突破
	void OnRefineNatureSmartMounts(int index);										//洗练灵性
	void OnSkillUpSmartMounts(int index, bool is_base_skill, int skill_index);		//升级技能(屏蔽)
	void OnRefineSkillSmartMounts(int index, int lock_flag, bool is_auto_buy);		//洗练技能(屏蔽)
	void OnSaveRefineSkillSmartMounts(int index);									//保存洗练(屏蔽)
	void OnCancelRefineSmartMounts(int index);										//取消洗练(屏蔽)
	void OnAddEnergySmartMounts(int index);											//补充精力
	void OnResetSkillSmartMounts(int index, bool is_base_skill, int skill_index);	//重置技能(屏蔽)
	void OnDominancePetSmartMounts(int index, int pet_index, int dominance_index);	//统御宠物
	void OnCancelDominancePet(int index, int dominance_index);						//取消统御
	bool OnCancelDominancePet(int pet_index);										//取消统御
	void OnRevealSmartMounts(int index);											//灵骑点化

	//灵骑技能改版
	void OnLearnSkillSmartMounts(int index, int skill_index, int column, int bag_index);	//学习技能
	void OnSkillUpSmartMounts(int index, int skill_index, int column, int bag_index);		//技能升级
	void OnSkillReplaceSmartMounts(int index, int skill_index, int column, int bag_index);	//置换技能
	void OnSkillItemComposeItem(int main_item_id, int stuff_item_id, int target_item_id);	//灵技合成	
	
	void OnWearOrnaments(int index);												//穿戴饰品
	bool OnTakeOffOrnaments(int index, bool is_notice = true);						//卸下饰品

	void OnSetImage(int index, bool is_advance_image);								//设置外观
	void OnActiveEquip(int index, int equip_index);									//激活装备
	void OnEquipLevelUp(int index, int equip_index);								//装备升级
	void OnJewelryStrength(int index);												//灵饰强化

	void OnActiveHoly(int index);													//激活圣物
	void OnHolyLevelUp(int index);													//圣物强化

	void OnRideSmartMounts(int index);												//骑乘灵骑
	void OnCancelRideSmartMounts();													//取消骑乘

	void OnConvertItem(int need_convert_item_id, int target_item_id);				//转换道具
	void OnActiveConvertItem(int active_item_bag_column, int active_item_bag_index);//激活灵骑道具转换

	void OnFinishBattle(int battle_mode, int pet_stat_num, long long * pet_unique_id_list);		//只有在战斗中出场的宠物,并且处于统御时才需要扣除精力值

	void GetUserSpecialEffectList(int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list);										//获取特效列表(人物)
	void GetBaseSpecialEffectList(unsigned long long item_unique_id, int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list);	//获取基础技能特效列表(宠物)

	void SendAllInfo();
	void SendSingleInfo(int index);
	void SendSingleAttr(int index);
	void SendDominanceInfo(int index);
	void SendShowInfo(int index, int notice_type, short param1, short param2, short param3 = 0, short param4 = 0);
	void SendRefineInfo(int index);
	void SendOtherInfo();
	void SendRelatedInfo(int index, int notice_type, int param1);

	void GmSetBaseSkill(int index, int skill_index, int skill_id);	//设置灵骑基础技能
	void GmSetEnergy(int index, int energy);

private:
	
	const NorexItem * GetSkillItem(int item_id) const;
	bool OnActiveOrLevelUpUnlockSkill(int index, bool is_active, bool is_notice = true);
	void GetSmartMountsBaseSkillIdList(int index, UInt16 * base_skill_id_list, int count);
	bool IsValid(int index) { return index >= 0 && index < MAX_SMART_MOUNTS_SYSTEM_NUM; }

	RoleModuleManager * m_module_mgr;

	int pet_attr_capability[MAX_SMART_MOUNTS_SYSTEM_NUM];								// 宠物基础属性评分
	AttrInfo attr_list[MAX_SMART_MOUNTS_SYSTEM_NUM][MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];	// 宠物属性(升级+突破) * 灵性值
	AttributeList m_attrs_add;										//多个灵骑增加的角色属性(包含技能属性)
	AttributeList m_skill_attr_list;								//多个灵骑技能增加的角色属性
	SmartMountsSystemParam m_param;

	std::map<unsigned long long, int> m_pet_dominance_map;			//key:pet_index, second:smart_mounts_index

};

#endif