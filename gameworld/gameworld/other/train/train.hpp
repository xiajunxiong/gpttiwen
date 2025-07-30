#ifndef __TRAIN_HPP__
#define __TRAIN_HPP__

#include "servercommon/traindef.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
struct RoleCommonSavePetTrainData2;
struct RoleCommonSavePartnerTrainData;

class Train
{
public:
	Train();
	~Train();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const TrainParam &param);
	void GetOtherInitParam(TrainParam *param);

	int GetTrainLevel(int type);
	int GetTrainSkillCapability();

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);
	const AttributeList & ReCalcAttrPer(AttributeList &base_add, const AttributeList & cpy_attr_list);

	void OnRoleLogin();
	void OnTrainUp(int type, int num);
	void OnTrainUpFast(int type, int num);

	bool OnPetTrainUp(int num);
	bool ConvertItem(int seq, bool is_auto_buy = false);
	void SetRedDotFlag(int train_type);

	int OnPetTrainUp2(int type,int num);
	int OnPartnerTrainUp(int type, int num);
	std::map<int, int> GetPetTrainUp2Map();
	std::map<int, int> GetPetTrainUp2MapByLevel(int role_level);
	RoleCommonSavePetTrainData2 * GetRoleCommonSavePetTrainData2(int type, bool set_dirty = true);
	RoleCommonSavePartnerTrainData * GetRoleCommonSavePartnerTrainData(int type, bool set_dirty = true);

	void SendAllInfo();
	void SendPromotedInfo();
	void SendRedDotFlagInfo();

	int GetRoleTrainSkillLevelNum(int skill_level);		//获取达到该技能等级的人物修炼技能数量
	int GetRolePetTrainSkillLevel();
	int GetMaxTrainLevel();				//获取此时的最高修炼技能等级

	void CheckUpLevelItem();
	void OnRoleChangeProf();		//转职

	//化身
	void SynHuaShenTrainData(TrainData * pet_train, TrainData * train_list, int count);		//同步本尊修炼技能到化身
private:
	RoleModuleManager *m_role_module_mgr;

	TrainParam m_param;
	AttributeList m_attrs_add;
	AttributeList m_attrs_add_per;
};

#endif // !__TRAIN_HPP__