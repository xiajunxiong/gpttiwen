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
	int GetSkillOrSpCap();			//��ü��ܻ���Ч����
	int GetCurRideSmartMounts() { return m_param.ride_index; }	
	int GetUnlockOrMaxJewelryLevel(bool is_unlock);
	char GetCurRideSmartMountsImage();
	void OnFinishFunGuide();														//���ָ��

	void OnActiveSmartMounts(int index);											//��������
	void OnLevelUpSmartMounts(int index);											//��������
	void OnBreakLevelSmartMounts(int index);										//����ͻ��
	void OnRefineNatureSmartMounts(int index);										//ϴ������
	void OnSkillUpSmartMounts(int index, bool is_base_skill, int skill_index);		//��������(����)
	void OnRefineSkillSmartMounts(int index, int lock_flag, bool is_auto_buy);		//ϴ������(����)
	void OnSaveRefineSkillSmartMounts(int index);									//����ϴ��(����)
	void OnCancelRefineSmartMounts(int index);										//ȡ��ϴ��(����)
	void OnAddEnergySmartMounts(int index);											//���侫��
	void OnResetSkillSmartMounts(int index, bool is_base_skill, int skill_index);	//���ü���(����)
	void OnDominancePetSmartMounts(int index, int pet_index, int dominance_index);	//ͳ������
	void OnCancelDominancePet(int index, int dominance_index);						//ȡ��ͳ��
	bool OnCancelDominancePet(int pet_index);										//ȡ��ͳ��
	void OnRevealSmartMounts(int index);											//����㻯

	//���＼�ܸİ�
	void OnLearnSkillSmartMounts(int index, int skill_index, int column, int bag_index);	//ѧϰ����
	void OnSkillUpSmartMounts(int index, int skill_index, int column, int bag_index);		//��������
	void OnSkillReplaceSmartMounts(int index, int skill_index, int column, int bag_index);	//�û�����
	void OnSkillItemComposeItem(int main_item_id, int stuff_item_id, int target_item_id);	//�鼼�ϳ�	
	
	void OnWearOrnaments(int index);												//������Ʒ
	bool OnTakeOffOrnaments(int index, bool is_notice = true);						//ж����Ʒ

	void OnSetImage(int index, bool is_advance_image);								//�������
	void OnActiveEquip(int index, int equip_index);									//����װ��
	void OnEquipLevelUp(int index, int equip_index);								//װ������
	void OnJewelryStrength(int index);												//����ǿ��

	void OnActiveHoly(int index);													//����ʥ��
	void OnHolyLevelUp(int index);													//ʥ��ǿ��

	void OnRideSmartMounts(int index);												//�������
	void OnCancelRideSmartMounts();													//ȡ�����

	void OnConvertItem(int need_convert_item_id, int target_item_id);				//ת������
	void OnActiveConvertItem(int active_item_bag_column, int active_item_bag_index);//�����������ת��

	void OnFinishBattle(int battle_mode, int pet_stat_num, long long * pet_unique_id_list);		//ֻ����ս���г����ĳ���,���Ҵ���ͳ��ʱ����Ҫ�۳�����ֵ

	void GetUserSpecialEffectList(int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list);										//��ȡ��Ч�б�(����)
	void GetBaseSpecialEffectList(unsigned long long item_unique_id, int max_other_sp_num, short* out_other_sp_num, int* out_other_sp_list);	//��ȡ����������Ч�б�(����)

	void SendAllInfo();
	void SendSingleInfo(int index);
	void SendSingleAttr(int index);
	void SendDominanceInfo(int index);
	void SendShowInfo(int index, int notice_type, short param1, short param2, short param3 = 0, short param4 = 0);
	void SendRefineInfo(int index);
	void SendOtherInfo();
	void SendRelatedInfo(int index, int notice_type, int param1);

	void GmSetBaseSkill(int index, int skill_index, int skill_id);	//���������������
	void GmSetEnergy(int index, int energy);

private:
	
	const NorexItem * GetSkillItem(int item_id) const;
	bool OnActiveOrLevelUpUnlockSkill(int index, bool is_active, bool is_notice = true);
	void GetSmartMountsBaseSkillIdList(int index, UInt16 * base_skill_id_list, int count);
	bool IsValid(int index) { return index >= 0 && index < MAX_SMART_MOUNTS_SYSTEM_NUM; }

	RoleModuleManager * m_module_mgr;

	int pet_attr_capability[MAX_SMART_MOUNTS_SYSTEM_NUM];								// ���������������
	AttrInfo attr_list[MAX_SMART_MOUNTS_SYSTEM_NUM][MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];	// ��������(����+ͻ��) * ����ֵ
	AttributeList m_attrs_add;										//����������ӵĽ�ɫ����(������������)
	AttributeList m_skill_attr_list;								//������＼�����ӵĽ�ɫ����
	SmartMountsSystemParam m_param;

	std::map<unsigned long long, int> m_pet_dominance_map;			//key:pet_index, second:smart_mounts_index

};

#endif