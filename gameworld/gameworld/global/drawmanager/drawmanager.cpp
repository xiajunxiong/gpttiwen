#include "drawmanager.hpp"

#include "engineadapter.h"
#include "world.h"
#include "gamelog.h"

#include "obj/character/role.h"
#include "obj/character/robot.h"

#include "servercommon/errornum.h"
#include "scene/scenemanager.hpp"
#include "protocol/msgdraw.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "global/usercache/usercache.hpp"
#include "global/wildbossmanager/wildbossconfig.hpp"
#include "other/robot/robotmanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/hundredghost/hundredghostconfig.hpp"
#include "config/otherconfig/knapsack_config.hpp"

#include <set>

DrawManager::DrawManager()
{
	m_next_check_timeout = 0;
	m_draw_group_id_auto = 100;
}

DrawManager::~DrawManager()
{
}

DrawManager & DrawManager::Instance()
{
	static DrawManager dm;
	return dm;
}

void DrawManager::OnServerStart()
{
}

void DrawManager::OnServerStop()
{
	this->CheckOutTime(0, true);
}

void DrawManager::OnRoleLogOut(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	//角色离线直接发放随机奖励(邮件)
	DrawUIDMap::iterator it = m_draw_uid_map.find(role->GetUID());
	if (m_draw_uid_map.end() == it)
	{
		return;
	}

	DrawGroupMap::iterator group_it = m_draw_group.find(it->second);
	if (m_draw_group.end() == group_it)
	{
		return;
	}

	this->GiveRandDraw(role->GetUID(), group_it->second);
}

void DrawManager::Update(unsigned long interval, time_t now_second)
{
	if ((unsigned int)now_second > m_next_check_timeout)
	{
		this->CheckOutTime((unsigned int)now_second);
		m_next_check_timeout = (unsigned int)now_second + 3;
	}
}

void DrawManager::OnReq(Role *role, int type, int param_1, int param_2)
{
	if (NULL == role)
	{
		return;
	}

	switch (type)
	{
	case Protocol::DRAW_REQ_TYPE_ROLE_DRAW:
		{
			this->OnRoleDraw(role, param_1, param_2);
		}
		break;
	default:
		break;
	}
}

void DrawManager::AddDrawGroup(DrawGroupInfo & draw_group_info, int draw_group_type)
{
	if (0 != draw_group_info.draw_group_id)
	{//已分配id  id应该自动分配
		return;
	}

	if (0 >= draw_group_info.draw_count
		|| DRAW_GROUP_MAX_ITEM_NUM < draw_group_info.draw_count)
	{
		return;
	}

	if (DRAW_GROUP_TYPE_DEF > draw_group_type
		|| DRAW_GROUP_TYPE_MAX <= draw_group_type)
	{
		return;
	}

	draw_group_info.draw_group_type = draw_group_type;

	unsigned int now_time = (unsigned int)(EngineAdapter::Instance().Time());

	//设置超时 为0 或 过大 时设置默认值
	if (0 == draw_group_info.out_time
		|| draw_group_info.out_time > now_time + 600)
	{
		draw_group_info.out_time = now_time + DRAW_GROUP_DEF_OUT_TIME;
	}

	//分配奖励组id
	this->GetNewDrawGroupId(draw_group_info.draw_group_id);

	//存入
	m_draw_group[draw_group_info.draw_group_id] = draw_group_info;
	m_draw_group_out_time.insert(DrawGroupOutTimeMap::value_type(draw_group_info.out_time, draw_group_info.draw_group_id));
	
	//翻牌状态记录
	for (int i = 0; i < draw_group_info.draw_count && i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		int &temp_uid = draw_group_info.draw_info_list[i].role_uid;
		if (-1 == draw_group_info.draw_info_list[i].draw_index
			&& 0 != temp_uid)
		{
			m_draw_uid_map[temp_uid] = draw_group_info.draw_group_id;
		}
	}
}

const DrawGroupInfo * DrawManager::GetDrawGroup(int draw_group_id) const
{
	DrawGroupMap::const_iterator it = m_draw_group.find(draw_group_id);
	if (m_draw_group.end() == it)
	{
		return NULL;
	}
	return &it->second;
}

void DrawManager::OnRoleDraw(Role * role, int draw_group_id, int draw_index)
{
	if (NULL == role)
	{
		return;
	}

	DrawGroupMap::iterator it = m_draw_group.find(draw_group_id);
	if (m_draw_group.end() == it)
	{
		return;
	}
	DrawGroupInfo &draw_group_info = it->second;

	if (0 > draw_index
		|| DRAW_GROUP_MAX_ITEM_NUM <= draw_index
		|| draw_group_info.draw_count <= draw_index)
	{
		return;
	}

	//获得角色的信息
	int uid_index = -1;

	//uid暂存 操作者本身
	std::set<int> temp_uid;
	for (int i = 0; i < draw_group_info.draw_count && i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		//检测 抽取位置是否被占用  draw_index
		if (draw_index == draw_group_info.draw_info_list[i].draw_index)
		{
			role->NoticeNum(errornum::EN_DRAW_MANAGER_HAS_BEEN_SELECT);
			return;
		}

		//找到角色
		if (role->GetUID() == draw_group_info.draw_info_list[i].role_uid)
		{//支持同角色领取多次
			if (-1 != draw_group_info.draw_info_list[i].draw_index)
			{
				continue;
			}
			if (-1 == uid_index)
			{
				uid_index = i;
			}
		}
		else
		{
			temp_uid.insert(draw_group_info.draw_info_list[i].role_uid);
		}
	}

	if (0 > uid_index
		|| DRAW_GROUP_MAX_ITEM_NUM <= uid_index)
	{
		return;
	}

	DrawGroupInfo::DrawItem &role_draw_info = draw_group_info.draw_info_list[uid_index];
	//奖励已获得
	if (-1 != role_draw_info.draw_index)
	{
		return;
	}

	//发放
	this->SendGroupReward(role_draw_info, draw_group_info.draw_group_type, role);
	role_draw_info.draw_index = draw_index;

	//移出
	m_draw_uid_map.erase(role->GetUID());

	//广播队伍玩家
	Protocol::SCRoleDrawInfo info;
	info.m_draw_group_id = draw_group_id;
	info.uid = role->GetUID();
	info.draw_index = draw_index;

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));


	for (std::set<int>::iterator temp_uid_it = temp_uid.begin(); temp_uid_it != temp_uid.end(); temp_uid_it++)
	{
		Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*temp_uid_it));
		if (tmp_role == NULL)
		{
			continue;
		}

		EngineAdapter::Instance().NetSend(tmp_role->GetNetId(), (const char*)&info, sizeof(info));
	}
	
	//检测该翻牌中是否还所有玩家都翻了，只剩机器人未翻
	if (this->IsAllUserFetch(draw_group_info))
	{
		this->GiveAllDraw(draw_group_info);
	}
}

void DrawManager::GiveAllDraw(DrawGroupInfo & draw_group_info)
{
	//
	int no_draw_count = 0;
	//未被领取的奖励index
	int no_draw_uid[DRAW_GROUP_MAX_ITEM_NUM] = { 0 };
	//map<uid,Role*>暂存
	std::map<int, Role *> temp_uid_map;

	//未被使用的draw_index
	std::set<int> no_used_draw_index;
	for (int i = 0; i < draw_group_info.draw_count && i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		no_used_draw_index.insert(i);
	}

	for (int i = 0; i < draw_group_info.draw_count && i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		//未被领取的奖励index
		if (-1 == draw_group_info.draw_info_list[i].draw_index)
		{
			no_draw_uid[no_draw_count] = i;
			no_draw_count++;
		}
		else
		{
			no_used_draw_index.erase(draw_group_info.draw_info_list[i].draw_index);
		}

		//直接移除翻牌状态 防止锁死
		m_draw_uid_map.erase(draw_group_info.draw_info_list[i].role_uid);

		//uid暂存
		{
			Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(draw_group_info.draw_info_list[i].role_uid));
			if (tmp_role == NULL)
			{
				continue;
			}
			temp_uid_map[draw_group_info.draw_info_list[i].role_uid] = tmp_role;
		}
	}

	//未被占用的位置
	int no_use_draw_index_count = 0;
	int auto_draw_index_list[DRAW_GROUP_MAX_ITEM_NUM] = { -1 };
	for (std::set<int>::iterator it = no_used_draw_index.begin(); it != no_used_draw_index.end() && no_use_draw_index_count < no_draw_count; it++)
	{
		auto_draw_index_list[no_use_draw_index_count] = *it;
		no_use_draw_index_count++;
	}
	//洗牌
	std::random_shuffle(auto_draw_index_list, auto_draw_index_list + no_use_draw_index_count);

	//发放
	for (int i = 0; i < draw_group_info.draw_count && i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		DrawGroupInfo::DrawItem &role_draw_info = draw_group_info.draw_info_list[i];

		if (-1 != role_draw_info.draw_index)
		{
			continue;
		}

		if (0 >= no_use_draw_index_count)
		{
			break;
		}
		no_use_draw_index_count--;
		int draw_index = auto_draw_index_list[no_use_draw_index_count];

		{//发放
			Role *tmp_role = NULL;
			std::map<int, Role *>::iterator tmp_role_it = temp_uid_map.find(role_draw_info.role_uid);
			if (tmp_role_it != temp_uid_map.end())
			{
				tmp_role = tmp_role_it->second;
			}

			this->SendGroupReward(role_draw_info, draw_group_info.draw_group_type, tmp_role);
			role_draw_info.draw_index = draw_index;
		}

		//广播队伍玩家
		Protocol::SCRoleDrawInfo info;
		info.m_draw_group_id = draw_group_info.draw_group_id;
		info.uid = role_draw_info.role_uid;
		info.draw_index = draw_index;

		for (std::map<int, Role *>::iterator tmp_role_it = temp_uid_map.begin();
			tmp_role_it != temp_uid_map.end();
			tmp_role_it++)
		{
			if (NULL != tmp_role_it->second)
			{
				EngineAdapter::Instance().NetSend(tmp_role_it->second->GetNetId(), (const char*)&info, sizeof(info));
			}
		}
	}
}

void DrawManager::GiveRandDraw(int uid, DrawGroupInfo & draw_group_info, Role *role/* = NULL*/)
{
	int no_draw_count = 0;
	//未被领取的奖励index
	int no_draw_uid[DRAW_GROUP_MAX_ITEM_NUM] = { 0 };
	//map<uid,Role*>暂存
	std::map<int, Role *> temp_uid_map;

	//未被使用的draw_index
	std::set<int> no_used_draw_index;
	for (int i = 0; i < draw_group_info.draw_count && i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		no_used_draw_index.insert(i);
	}

	//获得角色的信息
	int uid_index = -1;

	for (int i = 0; i < draw_group_info.draw_count && i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		//未被领取的奖励index
		if (-1 == draw_group_info.draw_info_list[i].draw_index)
		{
			no_draw_uid[no_draw_count] = i;
			no_draw_count++;
		}
		else
		{
			no_used_draw_index.erase(draw_group_info.draw_info_list[i].draw_index);
		}

		//找到角色
		if (uid == draw_group_info.draw_info_list[i].role_uid)
		{
			uid_index = i;
		}
		else	
		{//uid暂存
			Role * tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(draw_group_info.draw_info_list[i].role_uid));
			if (tmp_role == NULL)
			{
				continue;
			}
			temp_uid_map[draw_group_info.draw_info_list[i].role_uid] = tmp_role;
		}
	}

	//移除翻牌状态 防止锁死
	m_draw_uid_map.erase(uid);

	//随机翻牌位置
	int rand_draw_index = -1;
	int rand_num = RandomNum(no_used_draw_index.size());
	for (std::set<int>::iterator it = no_used_draw_index.begin(); it != no_used_draw_index.end(); it++)
	{
		if (0 >= rand_num)
		{
			rand_draw_index = *it;
			break;
		}
		rand_num--;
	}

	if (0 > uid_index
		|| DRAW_GROUP_MAX_ITEM_NUM <= uid_index)
	{
		return;
	}

	if (0 > rand_draw_index
		|| DRAW_GROUP_MAX_ITEM_NUM <= rand_draw_index
		|| draw_group_info.draw_count <= rand_draw_index)
	{
		return;
	}

	DrawGroupInfo::DrawItem &role_draw_info = draw_group_info.draw_info_list[uid_index];
	//奖励已获得
	if (-1 != role_draw_info.draw_index)
	{
		return;
	}

	//发放
	this->SendGroupReward(role_draw_info, draw_group_info.draw_group_type, role);
	role_draw_info.draw_index = rand_draw_index;

	//广播队伍玩家
	Protocol::SCRoleDrawInfo info;
	info.m_draw_group_id = draw_group_info.draw_group_id;
	info.uid = uid;
	info.draw_index = rand_draw_index;

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
	}

	for (std::map<int, Role *>::iterator tmp_role_it = temp_uid_map.begin();
		tmp_role_it != temp_uid_map.end();
		tmp_role_it++)
	{
		if (NULL != tmp_role_it->second)
		{
			EngineAdapter::Instance().NetSend(tmp_role_it->second->GetNetId(), (const char*)&info, sizeof(info));
		}
	}
}

bool DrawManager::IsDrawing(int uid)
{
	DrawUIDMap::iterator it = m_draw_uid_map.find(uid);
	if (m_draw_uid_map.end() == it)
	{
		return false;
	}

	return true;
}

void DrawManager::PutRewardItem(DrawGroupInfo::DrawItem & reward_info, int num, const ItemConfigData * item_cfg_list, int prof)
{
	if (NULL == item_cfg_list)
	{
		return;
	}

	for (int i = 0; i < num; i++)
	{
		ItemConfigData  item_info = item_cfg_list[i];
		const ItemBase *itembase = ITEMPOOL->GetItem(item_info.item_id);
		if (NULL == itembase)
		{
			continue;
		}
		if (I_TYPE_CONVERT_ITEM == itembase->GetItemType())
		{
			if (0 == prof)
			{
				prof = rand() % PROFESSION_TYPE_NUM;
				if(0 == prof) prof += 1;
			}
			int new_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(item_info.item_id, prof);
			if (new_item_id != 0)
			{
				item_info.item_id = new_item_id;

				itembase = ITEMPOOL->GetItem(item_info.item_id);
				if (NULL == itembase)
				{
					continue;
				}
			}
		}

		bool is_net_value_item = (itembase->GetItemType() >= I_TYPE_NET_VALUE_ITEM_BEGIN);
		if (is_net_value_item)
		{
			for (int index = 0; index < item_info.num; index++)
			{
				ItemDataWrapper tmp_item_wrapper(item_info.item_id, 1, item_info.is_bind);
				NetValueItemParam tmp_item_param;
				if (!itembase->HasRandomNetValue(&tmp_item_param))
				{
					itembase->RandomNetValueOnPutItem(NULL, &tmp_item_param, PUT_REASON_DRAW_MANAGER);
					itembase->SetRandomNetValueFlag(&tmp_item_param);

					tmp_item_param.GetData(tmp_item_wrapper.param_data);

					tmp_item_wrapper.has_param = 1;
					memcpy(tmp_item_wrapper.param_data, tmp_item_param.data, sizeof(tmp_item_wrapper.param_data));
				}
				reward_info.item_list.push_back(tmp_item_wrapper);
			}
		}
		else
		{
			ItemDataWrapper tmp_item_wrapper(item_info.item_id, item_info.num, item_info.is_bind);
			reward_info.item_list.push_back(tmp_item_wrapper);
		}
	}
}

void DrawManager::ShuffDrawGroupByUid(DrawGroupInfo & deaw_group_info)
{
	int shuff_count = 0;
	int tmp_uid_list[DRAW_GROUP_MAX_ITEM_NUM] = { 0 };
	for (int i = 0; i < DRAW_GROUP_MAX_ITEM_NUM && i < deaw_group_info.draw_count; i++)
	{
		tmp_uid_list[i] = deaw_group_info.draw_info_list[i].role_uid;
		shuff_count++;
	}
	//洗牌
	std::random_shuffle(deaw_group_info.draw_info_list, deaw_group_info.draw_info_list + shuff_count);

	//赋上之前的uid
	for (int i = 0; i < DRAW_GROUP_MAX_ITEM_NUM && i < shuff_count; i++)
	{
		deaw_group_info.draw_info_list[i].role_uid = tmp_uid_list[i];
	}
}

void DrawManager::SendGroupReward(DrawGroupInfo::DrawItem reward_info, int group_type, Role * role)
{
	if (-1 != reward_info.draw_index)
	{
		return;
	}

	int &role_uid = reward_info.role_uid;

	bool item_put_succ = false;
	bool money_put_succ = false;
	if (NULL != role)
	{//先尝试放入背包
		item_put_succ = true;
		if (!reward_info.item_list.empty())
		{
			if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList((short)reward_info.item_list.size(), &reward_info.item_list[0]))
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutList((short)reward_info.item_list.size(), &reward_info.item_list[0], PUT_REASON_DRAW_MANAGER);
			}
			else
			{
				item_put_succ = false;
			}
		}


		//货币
		money_put_succ = true;
		DrawGroupRewardMoneyMap::iterator it = reward_info.money_type.begin();
		for (; it != reward_info.money_type.end(); it++)
		{
			if (0 >= it->second)
			{
				continue;
			}

			UNSTD_STATIC_CHECK(1 == DRAW_GROUP_VIRTUAL_TYPE_MAX);
			switch (it->first)
			{
			case DRAW_GROUP_VIRTUAL_TYPE_LUCK_COIN:
				{
					role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(it->second, MONEY_TYPE_LUCK_COIN, __FUNCTION__);
				}
				break;
			default:
				break;
			}
		}
	}
	
	//确定角色存在
	UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(role_uid);
	GameName role_name;
	if (NULL != role)
	{	
		role->GetName(role_name);
	}
	else if (NULL != user_cache_node)
	{
		user_cache_node->GetName(role_name);
	}
	else
	{
		return;
	}	

	//公告
	bool is_break = false;
	for (int i = 0; i < (int)reward_info.item_list.size(); ++i)
	{
		switch (group_type)
		{
			case DRAW_GROUP_TYPE_FIELD_BOSS:
			{
				if (LOGIC_CONFIG->GetWildBossConfig()->IsItemNotice(reward_info.item_list[i].item_id))
				{
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_wildboss_poker_reward_notice, role_name, reward_info.item_list[i].item_id);
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
							0, 0, 0, 0);
					}
				}
			}
			break;
			case DRAW_GROUP_TYPE_CRYSTAL:
			{
				if (LOGIC_CONFIG->GetRoleCrystalFbConfig()->IsItemNotice(reward_info.item_list[i].item_id))
				{
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_crystal_fb_poker_reward_notice, role_name, reward_info.item_list[i].item_id);
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
							0, 0, 0, 0);
					}
				}
			}
			break;
			case DRAW_GROUP_TYPE_HUNDRED_GHOST:
			{
				if (LOGIC_CONFIG->GetHundredGhostConfig()->IsItemNotice(reward_info.item_list[i].item_id))
				{
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hundred_ghost_poker_reward_notice, role_name, reward_info.item_list[i].item_id);
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
							0, 0, 0, 0);
					}
				}
			}
			break;
			default:
			is_break = true;
			break;
		}
		if (is_break)
		{
			break;
		}

	}

	//MailSubject subject;
	//MailContentTxt contenttxt;
	char * mail_subject = NULL;
	char * mail_content = NULL;

	UNSTD_STATIC_CHECK(9 == DRAW_GROUP_TYPE_MAX)
	switch (group_type)
	{
	case DRAW_GROUP_TYPE_MI_YI:
		{
			mail_subject = gamestring::g_miyi_poker_title;
			mail_content = gamestring::g_miyi_poker_reward_content;		
		}
		break;
	case DRAW_GROUP_TYPE_FIELD_BOSS:
		{
			mail_subject = gamestring::g_wildboss_poker_title;
			mail_content = gamestring::g_wildboss_poker_reward_content;
		}
		break;
	case DRAW_GROUP_TYPE_SHAN_HAI:
		{
			mail_subject = gamestring::g_shanhaiboss_poker_title;
			mail_content = gamestring::g_shanhaiboss_poker_reward_content;
		}
		break;
	case DRAW_GROUP_TYPE_CRYSTAL:
	case DRAW_GROUP_TYPE_CRYSTAL_FAST_PASS:
		{		
			mail_subject = gamestring::g_crystal_fb_poker_reward_subject;
			mail_content = gamestring::g_crystal_fb_poker_reward_content;
		}
		break;
	case DRAW_GROUP_TYPE_HUNDRED_GHOST:
		{
			mail_subject = gamestring::g_hundred_ghost_poker_title;
			mail_content = gamestring::g_hundred_ghost_poker_reward_content;
		}
		break;

	case DRAW_GROUP_TYPE_PET_GOD_FB:
	case DRAW_GROUP_TYPE_PET_GOD_FB_AUTO:
	case DRAW_GROUP_TYPE_DEF:
	default:
		{
			mail_subject = gamestring::g_draw_reward_subject;
			mail_content = gamestring::g_draw_reward_content;	
		}
	}

	if (!item_put_succ|| !money_put_succ)
	{//放失败 发邮件
		static MailContentParam contentparam;
		contentparam.Reset();

		int len_1 = SNPRINTF(contentparam.subject, 
			sizeof(contentparam.subject), 
			mail_subject);
		int len_2 = SNPRINTF(contentparam.contenttxt, 
			sizeof(contentparam.contenttxt), 
			mail_content);

		int count = 0;
		bool has_virtual = false;
		if (!money_put_succ)
		{
			DrawGroupRewardMoneyMap::iterator it = reward_info.money_type.begin();
			for (; it != reward_info.money_type.end(); it++)
			{
				if (0 >= it->second)
				{
					continue;
				}

				UNSTD_STATIC_CHECK(1 == DRAW_GROUP_VIRTUAL_TYPE_MAX);
				switch (it->first)
				{
				case DRAW_GROUP_VIRTUAL_TYPE_LUCK_COIN:
					{
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_LUCK_COIN] = it->second;
						has_virtual = true;
					}
					break;
				default:
					break;
				}
			}
		}

		for (int i = 0; i < (int)reward_info.item_list.size(); i++)
		{
		
			if (count >= MAX_ATTACHMENT_ITEM_NUM)
			{
				if (len_1 > 0 && len_2 > 0)
				{
					MailRoute::MailToUser(role_uid, MAIL_REASON_DEFAULT, contentparam);
				}

				contentparam.Reset();

				len_1 = SNPRINTF(contentparam.subject, 
					sizeof(contentparam.subject), 
					mail_subject);
				len_2 = SNPRINTF(contentparam.contenttxt, 
					sizeof(contentparam.contenttxt), 
					mail_content);

				count = 0;
				has_virtual = false;
			}

			contentparam.item_list[count] = reward_info.item_list[i];
			count++;
		}

		if (has_virtual || 0 < count)
		{
			if (len_1 > 0 && len_2 > 0)
			{
				MailRoute::MailToUser(role_uid, MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
}

void DrawManager::GetRoleOrRobotName(int role_uid, GameName & name)
{
	//机器人
	Robot *robot = RobotManager::Instance().GetRobot(role_uid);
	if (NULL != robot)
	{
		robot->GetName(name);
	}

	//角色离线
	UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(role_uid);
	if (NULL != user_cache_node)
	{
		user_cache_node->GetName(name);
	}
}

void DrawManager::GetItemInfoType(ItemDrawInfo & info_type, const ItemDataWrapper & item_Data)
{
	info_type.item_info = item_Data;
	info_type.param_len = 0;
	const ItemBase * item = ITEMPOOL->GetItem(item_Data.item_id);
	if (NULL == item)
	{
		return;
	}
	info_type.param_len = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
}

void DrawManager::CheckOutTime(unsigned int now_second, bool clear_all)
{
	while (!m_draw_group_out_time.empty())
	{
		DrawGroupOutTimeMap::iterator timeout_it = m_draw_group_out_time.begin();
		if (timeout_it->first > now_second && !clear_all)
		{
			break;
		}

		DrawGroupMap::iterator draw_group_it = m_draw_group.find(timeout_it->second);
		if (draw_group_it == m_draw_group.end())
		{
			//迭代器失效
			m_draw_group_out_time.erase(timeout_it);
			continue;
		}

		//发放
		this->GiveAllDraw(draw_group_it->second);

		//删除 迭代器失效
		m_draw_group.erase(draw_group_it);
		m_draw_group_out_time.erase(timeout_it);
	}
}

void DrawManager::GetNewDrawGroupId(int & new_draw_group_id)
{
	m_draw_group_id_auto++;
	new_draw_group_id = m_draw_group_id_auto;
}

bool DrawManager::IsAllUserFetch(DrawGroupInfo & draw_group_info)
{	
	for (int i = 0; i < draw_group_info.draw_count && i < DRAW_GROUP_MAX_ITEM_NUM; i++)
	{
		DrawGroupInfo::DrawItem &role_draw_info = draw_group_info.draw_info_list[i];
		if (0 == role_draw_info.role_uid)
		{
			continue;
		}
		UserCacheNode * user_cache = UserCacheManager::Instance().GetUserNode(role_draw_info.role_uid);
		if (NULL != user_cache && -1 == role_draw_info.draw_index)
		{
			return false;
		}
	}

	return true;
}
