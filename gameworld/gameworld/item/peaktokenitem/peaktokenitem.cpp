#include "peaktokenitem.hpp"

#include "gamelog.h"

#include "obj/character/role.h"
#include "item/itemgriddata.h"
#include "other/rolemodulemanager.hpp"
#include "other/peaktoken/rolepeaktoken.hpp"
#include "other/peaktoken/peaktokenconfig.hpp"
#include "config/logicconfigmanager.hpp"

#include "obj/otherobj/peaktokenobj.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/errornum.h"

PeakTokenItem::PeakTokenItem()
	: ItemBase(I_TYPE_PEAK_TOKEN)
{
	m_reward_group_id = 0;
}

int PeakTokenItem::Use(Role * role, const ItemGridData & item_data, short & num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type, void * param) const
{
	if (this->GetItemId() != item_data.item_id)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return -1;
	}
	
	//无净值
	if (NULL == item_data.param)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return -2;
	}

	//静态场景
	Scene *scene = role->GetScene();
	if (NULL == scene || Scene::SCENE_TYPE_COMMON_SCENE != scene->GetSceneType())
	{
		role->NoticeNum(errornum::EN_KNAPSACK_USE_SCENE_LIMIT);
		return -3;
	}

	//本服使用
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_HIDDEN_REQ);
		return -4;
	}

	//批量使用限制
	if (1 != num)
	{
		role->NoticeNum(errornum::EN_ITEM_CANNOT_MULTI_USE);
		return -5;
	}

	//物品使用次数限制
	const PeakTokenConfig::OtherCfg & other_cfg = LOGIC_CONFIG->GetPeakTokenConfig()->GetOtherCfg();

	if (other_cfg.item_can_use_times <= item_data.param->peak_token_param.used_times)
	{
		role->NoticeNum(errornum::EN_ITEM_USE_COUNT_LIMIT);
		return -6;
	}

	//角色每日限用
	int today_use_times = role->GetRoleModuleManager()->GetRolePeakToken()->GetToDayUseTimes();
	if (0 != today_use_times)
	{
		role->NoticeNum(errornum::EN_TODAY_USE_COUNT_LIMIT);
		return -7;
	}



	//记录
	out_need_set_dirty = true;
	item_data.param->peak_token_param.used_times++;

	today_use_times++;
	role->GetRoleModuleManager()->GetRolePeakToken()->SetToDayUseTimes(today_use_times);
	role->GetRoleModuleManager()->GetRolePeakToken()->SendInfo();

	//创建
	int creat_ret = this->CreatAct(role);
	gamelog::g_log_role_activity.printf(LL_INFO, "PeakTokenItem::Use user[%d,%s] "
		"creat_ret:[%d] today_use_times:[%d] "
		"item:{[%d] param: [%s]}",
		role->GetUID(), role->GetName(),
		creat_ret, today_use_times,
		item_data.item_id, item_data.param->GetParamLog(this->GetItemType()));

	
	//该物品不会因使用被消耗
	return -777;
}

void PeakTokenItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	param_data->element_crystal_param.used_times = 0;
}

int PeakTokenItem::CreatAct(Role * role) const
{
	if (NULL == role)
	{
		return -1; 
	}
	Scene* scene = role->GetScene();
	if (NULL == scene)
	{
		return -2;
	}

	PeakTokenObj *creat_obj = new PeakTokenObj();
	creat_obj->Init(role, m_reward_group_id);
	creat_obj->SetPos(role->GetPos());
	scene->AddObj(creat_obj);

	return 0;
}
