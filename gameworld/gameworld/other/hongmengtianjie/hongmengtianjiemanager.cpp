#include "hongmengtianjiemanager.hpp"
#include "other/hongmengtianjie/rolehongmengtianjie.hpp"

#include "global/commonsave/commonsave.hpp"
#include "global/commonsave/commonsavemanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/hongmengtianjie/hongmengtianjieconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "engineadapter.h"
#include "global/team/team.hpp"
#include "servercommon/maildef.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "world.h"
#include "servercommon/errornum.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msghongmengtianjie.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "global/usercache/usercache.hpp"
#include "other/zaohuaequip/zaohuaequipmanager.hpp"
#include "scene/speciallogicimpl/speciallogichongmengtianjie.hpp"

WorldHongMengTianJieManager & WorldHongMengTianJieManager::Instance()
{
	static WorldHongMengTianJieManager hptj_manger;
	return hptj_manger;
}

bool WorldHongMengTianJieManager::IsFirstPass(int fb_id)
{
	if (fb_id < 0 || fb_id >= HongMengTianJie::MAX_FB_INDEX_NUM)
		return false;

	CommonSaveWorldHongMengTianJie * fb_data = this->GetCommonSaveFBData();
	if (NULL == fb_data)
		return false;

	return fb_data->first_pass[fb_id].first_pass_time != 0;
}

int WorldHongMengTianJieManager::GetDifficultf(int fb_type, int fb_seq)
{
	const HongMengTianJieFbCfg * fb_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetFBCfg(fb_type, fb_seq);
	if (NULL == fb_cfg)
		return 0;

	bool is_last_layer = false;			// 是否是最后一层
	const HongMengTianJieFbCfg * next_fb_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetFBCfg(fb_type, fb_seq + 1);
	if (NULL == next_fb_cfg)
	{
		is_last_layer = true;
	}

	CommonSaveWorldHongMengTianJie * fb_data = this->GetCommonSaveFBData();
	if (NULL == fb_data)
		return 0;

	int difficultf = fb_cfg->first_difficult;			// 初始难度
	if (!is_last_layer)
	{
		// 下一层没有首通过，那么这一层的难度不下降
		if (fb_data->first_pass[next_fb_cfg->fb_id].first_pass_time == 0)
			return difficultf;

		unsigned int week_zero_time = GetWeekZeroTime(fb_data->first_pass[next_fb_cfg->fb_id].first_pass_time);
		int now_day = GetDayIndex(week_zero_time, EngineAdapter::Instance().Time());
		if (now_day < 0) now_day = 0;
		int count = now_day / 7;
		
		difficultf = difficultf - count * fb_cfg->down_percent;
		if (difficultf < fb_cfg->min_difficult) difficultf = fb_cfg->min_difficult;
	}
	else
	{
		// 最后一层看自己， 自己首通过后的两周才开始下降
		if (fb_data->first_pass[fb_cfg->fb_id].first_pass_time == 0)
			return difficultf;

		unsigned int week_zero_time = fb_data->first_pass[fb_cfg->fb_id].first_pass_time + EngineAdapter::Instance().NextWeekInterval(MONDAY, 0, 0, 0);
		int now_day = GetDayIndex(week_zero_time, EngineAdapter::Instance().Time());
		if (now_day < 0) now_day = 0;
		int count = now_day / 7;

		difficultf = difficultf - count * fb_cfg->down_percent;
		if (difficultf < fb_cfg->min_difficult) difficultf = fb_cfg->min_difficult;
	}

	return difficultf;
}

void WorldHongMengTianJieManager::SetFirstPassData(int fb_id, Team * team)
{
	if (fb_id < 0 || fb_id >= HongMengTianJie::MAX_FB_INDEX_NUM || NULL == team)
		return;

	CommonSaveWorldHongMengTianJie * fb_data = this->GetCommonSaveFBData();
	if (NULL == fb_data)
		return;

	if (fb_data->first_pass[fb_id].first_pass_time != 0)
		return;
	
	const HongMengTianJieRewardCfg * reward_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetRewardCfg(fb_id);
	if (NULL == reward_cfg)
		return;

	fb_data->first_pass[fb_id].first_pass_time = EngineAdapter::Instance().Time();

	static MailContentParam contentparam; contentparam.Reset();
	SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_hmtj_world_first_reward_subject);
	SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_hmtj_world_first_reward_content, fb_id);

	ItemDataWrapper item_info;
	int count = 0;
	for (int i = 0; i < (int)reward_cfg->world_reward.size() && i < MAX_ATTACHMENT_ITEM_NUM; i++)
	{
		const ItemBase * item_base = ITEMPOOL->GetItem(reward_cfg->world_reward[i].item_id);
		if (NULL == item_base) continue;

		item_info.item_id = reward_cfg->world_reward[i].item_id;
		item_info.is_bind = reward_cfg->world_reward[i].is_bind;
		item_info.num = reward_cfg->world_reward[i].num;
		contentparam.item_list[count++] = item_info;
	}

	int list_str_len = 0; static char list_str[20480] = { 0 };
	for (int i = 0; i < team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; i++)
	{
		Role *temp_role = team->GetMemberRoleByIndex(i);
		if (NULL == temp_role)continue;

		fb_data->first_pass[fb_id].pass_uid[i] = temp_role->GetUID();

		MailRoute::MailToUser(temp_role->GetUID(), MAIL_REASON_DEFAULT, contentparam);

		char item_str[64] = { 0 }; int itemlen = 0;

		if (list_str_len > 0)
		{
			itemlen = SNPRINTF(item_str, sizeof(item_str), ",%s",temp_role->GetName());
		}
		else
		{
			itemlen = SNPRINTF(item_str, sizeof(item_str), "%s",temp_role->GetName());
		}

		if (itemlen > 0)
		{
			STRNCPY(list_str + list_str_len, item_str, sizeof(list_str) - list_str_len);
			list_str_len += itemlen;
		}
	}

	this->SetCommonSaveDirtyMark();

	if (list_str_len > 0)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hmtj_world_first_notice, list_str, fb_id);

		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}
}

bool WorldHongMengTianJieManager::CanEnterFB(Role * role, int fb_type, int fb_seq)
{
	if (NULL == role)
		return false;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	const HongMengTianJieFbCfg * fb_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetFBCfg(fb_type, fb_seq);
	if (NULL == fb_cfg)
		return false;

	Team * team = role->GetMyTeam();
	if (!team || team->GetMemberCount() < 3)
	{
		return false;
	}

	int need_pass_fb_id = -1;
	if (fb_type == 2)
	{
		const HongMengTianJieFbCfg * need_fb_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetFBCfg(1, fb_seq);
		if (NULL == need_fb_cfg)
			return false;

		need_pass_fb_id = need_fb_cfg->fb_id;
	}

	for (int i = 0; i < team->GetMemberCount(); ++i)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member) continue;

		// 检查 等级
		if (member->GetLevel() < LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().level_limit)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hmtj_team_not_level_can_enter_notice, member->GetUID(), member->GetName());
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
			return false;
		}

		// 检查 副本次数
// 		if (!member->GetRoleModuleManager()->GetRoleHongMengTianJie()->IsBossTimes(true))
// 		{
// 			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hmtj_tean_not_times_can_enter_notice, member->GetUID(), member->GetName());
// 			if (length > 0)
// 			{
// 				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
// 			}
// 			return false;
// 		}

		// 检查 造化装备 装等限制
		int equip_level = member->GetRoleModuleManager()->GetZaoHuaEquipManager()->GetWearZaoHuaLevelCount();
		if (equip_level < fb_cfg->equipment_requirement)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hmtj_team_not_equip_can_enter_notice, member->GetUID(), member->GetName());
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
			return false;
		}

		// 检查 英雄副本 前置是否完成
		if (need_pass_fb_id >= 0 && !member->GetRoleModuleManager()->GetRoleHongMengTianJie()->IsFBPass(need_pass_fb_id))
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hmtj_team_not_pass_can_enter_notice, member->GetUID(), member->GetName());
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
			return false;
		}
	}

	return true;
}

void WorldHongMengTianJieManager::OnReq(const char * msg)
{
	if (NULL == msg) return;

	crossgameprotocal::GameHiddenHongMengTianJieReq * req = (crossgameprotocal::GameHiddenHongMengTianJieReq *)msg;

	switch (req->req_type)
	{
	case 0:
		{
			CommonSaveWorldHongMengTianJie * fb_data = this->GetCommonSaveFBData();
			if (NULL == fb_data)
				return;

			Protocol::SCHongMengTianJieWorldInfo info;
			for (int i = 0; i < HongMengTianJie::MAX_FB_INDEX_NUM; i++)
			{
				info.first_pass_time[i] = fb_data->first_pass[i].first_pass_time;
			}

			InternalComm::Instance().SendToGameUserThroughCross(req->role_uid, &info, sizeof(info));
		}
		break;
	case 1:
		{
			CommonSaveWorldHongMengTianJie * fb_data = this->GetCommonSaveFBData();
			if (NULL == fb_data)
				return;

			HongMengTianJie::WorldFBData &fb_param = fb_data->first_pass[req->param];

			Protocol::SCHongMengTianJiePassTeamInfo info;
			info.fb_id = req->param;
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
			{
				if (fb_param.pass_uid[i] == 0) continue;
				UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(fb_param.pass_uid[i]);
				if (NULL == user_node) continue;

				info.member_list[i].uid = fb_param.pass_uid[i];
				info.member_list[i].level = user_node->level;
				info.member_list[i].profession = user_node->profession;
				user_node->GetName(info.member_list[i].member_name);
				info.member_list[i].appearance = user_node->appearance;
			}

			InternalComm::Instance().SendToGameUserThroughCross(req->role_uid, &info, sizeof(info));
		}
		break;;
	}
}

void WorldHongMengTianJieManager::SendPassTime(Role * role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		CommonSaveWorldHongMengTianJie * fb_data = this->GetCommonSaveFBData();
		if (NULL == fb_data)
			return;

		Protocol::SCHongMengTianJieWorldInfo info;
		for (int i = 0; i < HongMengTianJie::MAX_FB_INDEX_NUM; i++)
		{
			info.first_pass_time[i] = fb_data->first_pass[i].first_pass_time;
		}

		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
	else
	{
		crossgameprotocal::GameHiddenHongMengTianJieReq info;
		info.role_uid = role->GetUID();
		info.req_type = 0;

		InternalComm::Instance().SendToHiddenThroughCross((char *)&info, sizeof(info));
	}
}

void WorldHongMengTianJieManager::SendPassTeam(Role * role, int fb_id)
{
	if (fb_id < 0 || fb_id >= HongMengTianJie::MAX_FB_INDEX_NUM)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		CommonSaveWorldHongMengTianJie * fb_data = this->GetCommonSaveFBData();
		if (NULL == fb_data)
			return;

		HongMengTianJie::WorldFBData &fb_param = fb_data->first_pass[fb_id];

		Protocol::SCHongMengTianJiePassTeamInfo info;
		info.fb_id = fb_id;
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
		{
			if (fb_param.pass_uid[i] == 0) continue;
			UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(fb_param.pass_uid[i]);
			if(NULL == user_node) continue;

			info.member_list[i].uid = fb_param.pass_uid[i];
			info.member_list[i].level = user_node->level;
			info.member_list[i].profession = user_node->profession;
			user_node->GetName(info.member_list[i].member_name);
			info.member_list[i].appearance = user_node->appearance;
		}

		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
	else
	{
		crossgameprotocal::GameHiddenHongMengTianJieReq info;
		info.role_uid = role->GetUID();
		info.req_type = 1;
		info.param = fb_id;

		InternalComm::Instance().SendToHiddenThroughCross((char *)&info, sizeof(info));
	}
}

void WorldHongMengTianJieManager::EnterFB(Role* role, int type, int fb_seq, int member_num, int* member_list)
{
	if (NULL == role) return;

	Team * team = role->GetMyTeam();
	if (!team)
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (!this->CanRoleEnterFB(role, type, fb_seq))
	{
		return;
	}

	const HongMengTianJieFbCfg * fb_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetFBCfg(type, fb_seq);
	if (NULL == fb_cfg) return;

	int target_scene_id = fb_cfg->scene_id;
	int target_scene_key = team->GetTeamSceneKey();
	Posi target_pos = World::GetInstWorld()->GetSceneManager()->GetTownPos(target_scene_id);

	SpecialLogicHongMengTianJie * special_logic =
		World::GetInstWorld()->GetSceneManager()->GetSpecialLogicHongMengTianJie(target_scene_id, target_scene_key);
	if (NULL == special_logic)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int difficultf = WorldHongMengTianJieManager::Instance().GetDifficultf(type, fb_seq);
	special_logic->InitScene(fb_cfg->fb_id, difficultf, fb_cfg->door_pos, type, fb_seq);
	special_logic->SetTeamMemberList(member_num, member_list);

	World::GetInstWorld()->GetSceneManager()->TeamGoTo(team, target_scene_id, target_scene_key, target_pos);
}

WorldHongMengTianJieManager::WorldHongMengTianJieManager()
{
}
WorldHongMengTianJieManager::~WorldHongMengTianJieManager()
{
}

CommonSave * WorldHongMengTianJieManager::GetCommonSave() const
{
	CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_HONGMENG_TIANJI);
	return cs;
}

CommonSaveWorldHongMengTianJie * WorldHongMengTianJieManager::GetCommonSaveFBData(bool is_auto_create)
{
	CommonSave * cs = this->GetCommonSave();
	if (cs == NULL)
	{
		return NULL;
	}

	CommonSaveWorldHongMengTianJie * save_data = cs->GetTypeData<CommonSaveWorldHongMengTianJie>(0, is_auto_create);
	return save_data;
}

void WorldHongMengTianJieManager::SetCommonSaveDirtyMark()
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	CommonSave * cs = this->GetCommonSave();
	if (cs == NULL)
		return;

	cs->SetDataChange(0);
}

bool WorldHongMengTianJieManager::CanRoleEnterFB(Role* role, int fb_type, int fb_seq)
{
	if (NULL == role) return false;

	const HongMengTianJieFbCfg * fb_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetFBCfg(fb_type, fb_seq);
	if (NULL == fb_cfg) return false;

	int need_pass_fb_id = -1;
	if (fb_type == 2)
	{
		const HongMengTianJieFbCfg * need_fb_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetFBCfg(1, fb_seq);
		if (NULL == need_fb_cfg)
			return false;

		need_pass_fb_id = need_fb_cfg->fb_id;
	}

	// 检查 等级
	if (role->GetLevel() < LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().level_limit)
	{
		return false;
	}

	// 检查 造化装备 装等限制
	int equip_level = role->GetRoleModuleManager()->GetZaoHuaEquipManager()->GetWearZaoHuaLevelCount();
	if (equip_level < fb_cfg->equipment_requirement)
	{
		return false;
	}

	// 检查 英雄副本 前置是否完成
	if (need_pass_fb_id >= 0 && !role->GetRoleModuleManager()->GetRoleHongMengTianJie()->IsFBPass(need_pass_fb_id))
	{
		return false;
	}

	return true;
}
