#ifndef __ROLE_SCENE_STATUS_HPP__
#define __ROLE_SCENE_STATUS_HPP__

#include <map>
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/partnerdef.h"

struct BuffEffect
{
	BuffEffect() {}
	int buff_id;				// buff_id
	int buff_effect;			// buff效果
	int buff_param1;			// 效果参数1
	int buff_param2;			// 效果参数2
	int buff_param3;			// 效果参数3
	int buff_times;				// buff持续次数
	unsigned int buff_end_tamp; // buff结束时间戳
};

class Role;
class RoleSceneStatus
{
public:
	RoleSceneStatus();
	~RoleSceneStatus() {};

	void Init(Role* role);

	void OnRoleEnterScene();
	void OnRoleLeaveScene();

	void UpdateRole(int remain_hp, int remain_mp);
	void UpdatePet(int pet_id, int pet_index, int remain_hp, int remain_mp);
	void UpdatePartner(int partner_id, int remain_hp, int remain_mp);

	bool IsInAutoRecoverScene();

	void RecoverRole();
	void RecoverPet();
	void RecoverPartner();

	void SetSceneBuff(BuffEffect buff_param);

	const std::map<int,BuffEffect>* GetSceneBuffList() { return &m_scene_buff_list; }
private:
	Role* m_role;
	int pre_scene_type;//上一场景类型
	std::map<int,BuffEffect> m_scene_buff_list;//临时场景buff，更换场景会清除buff
};

#endif