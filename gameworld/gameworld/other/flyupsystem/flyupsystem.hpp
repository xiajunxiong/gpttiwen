#ifndef _FLY_UP_SYSTEM__HPP_
#define _FLY_UP_SYSTEM__HPP_

#include "servercommon/servercommon.h"
#include "servercommon/flyupsystemdef.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

class RoleModuleManager;
class FlyUpSystem
{
public:
	FlyUpSystem();
	~FlyUpSystem();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, const FlyUpSystemParam & param);
	void GetInitParam(FlyUpSystemParam * param);
	const AttributeList & ReCalcAttr(AttributeList & base_add, bool is_recalc);
	void OnDayChange();
	void OnRoleLogin();
	void OnRoleLogout();

	bool CanAcceptFeiShengTask(int task_id);

	int GetFlyUpSkillLevel();
	void RoleUpLevel();

	void RankFlyUpTask(int pre_task_id = 0);
	void StartShiLian();

	void OnTaskFinish(int task_type, int task_id);//�������
	void OnUpAdvance();//����ǿ��
	void OnFinishFly();//��ɱ��׷���
	void UplevelFlyUpSkill();
	void OnKeyUplevelFlyUpSkill();	//һ������
	void SaveAdvance(bool is_logout = false);
	int IsAddExpFlyUpFlag();

	int GetAttrPelletNum();//��ȡ�������Ե�ʹ������
	int GetFlyUpFlag();//�����׶�
	int GetTaskIdx();//����������
	void SendInfo(bool is_one_click_finish = false);
	void SendSigleSkillInfo();

	void GmFlyUp(int fly_up_level);

	bool IsCanOneClick();	// �Ƿ����һ�����
	void OneClickFinishTask(unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards);
private:
	RoleModuleManager *m_module_mgr;

	AttributeList m_attrs_add;
	FlyUpSystemParam m_param;
};
#endif
