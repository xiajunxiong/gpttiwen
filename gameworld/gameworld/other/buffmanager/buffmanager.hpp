#ifndef __BUFF_MANAGER_HPP__
#define __BUFF_MANAGER_HPP__

#include "servercommon/buffdef.hpp"
#include "buffbase.hpp"
#include <list>
#include "other/attributelist.hpp"

typedef std::list<BuffBase*> BuffList;
class RoleModuleManager;
class BuffManager
{
public:
	BuffManager();
	~BuffManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const BuffListParam & param);
	void GetInitParam(BuffListParam * param);

	void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);
	void OnLogin();

	void OnFightEnd();//结束一次战斗
	void OnRoleLeaveScene(bool is_logout);
	void OnRoleEnterScene(int now_scene);

	void AddBuff(BuffBase* buff, bool is_effect = true);
	
	int ChangeMoveSpeedPer(int per);
	int GetMoveSpeedPer();
	int GetSpeedChangeRate();

	int AddExpUpPer(int per);
	int GetExpUpPer();
	void SendBuffList();
private:
	RoleModuleManager * m_role_module_mgr;
	void FreeBuff(BuffBase* buff_base);
	AttributeList m_attrs_add;									// 角色属性加值

	BuffList m_buff_list;
	int m_move_speed_per;
	int m_exp_up_per;

	time_t m_lase_update_time;
};



#endif // !__BUFF_MANAGER_HPP__
