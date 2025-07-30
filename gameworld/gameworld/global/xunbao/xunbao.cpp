#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "protocol/msgother.h"
#include "engineadapter.h"
#include "world.h"
#include "other/pet/pet.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/string/gameworldstr.h"
#include "config/sharedconfig/sharedconfig.h"
#include "gamelog.h"
#include "other/funcguide/funcguideconfig.h"
#include "other/funcguide/funcguide.hpp"

#include "global/xunbao/xunbaoconfig.hpp"
#include "xunbao.hpp"
#include "other/pet/petconfig.hpp"
#include "other/partner/partner.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "item/expense/norexitem.hpp"

#include "servercommon/rolecommonsavedef.h"

XunBao & XunBao::Instance()
{
	static XunBao xb;
	return xb;
}

void XunBao::OnUserLogin(Role * role)
{
	this->SendOpenInfo(role);
}

int XunBao::Draw(Role * role, int draw_type, const int draw_times, int num, XunBaoComsumeInfo * comsume_info, bool use_free_times)
{
	if (role == NULL)
	{
		return -__LINE__;
	}

	time_t now = EngineAdapter::Instance().Time();
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(role->GetUniqueServerID());
	int cur_real_open_day = GetDayIndex(open_server_timestamp, now) + 1;

	XunBaoDrawCfg * draw_cfg = LOGIC_CONFIG->GetXunBaoConfig()->GetXunBaoDrawCfg(draw_type);

	if (draw_cfg == NULL || !draw_cfg->is_open || (draw_cfg->open_day > 0 && draw_cfg->open_day > cur_real_open_day))
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return -__LINE__;
	}

	switch (draw_cfg->open_type)
	{
		case 1:
			{
				if (draw_cfg->start_time != 0 || draw_cfg->end_time != 0)
				{
					if (now < draw_cfg->start_time || now > draw_cfg->end_time)
					{
						role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
						return -__LINE__;
					}
				}
			}
			break;
		case 2:		//
			{
				const tm * now_tm = EngineAdapter::Instance().LocalTime();
				bool week_day_open = ((1 << now_tm->tm_wday) & draw_cfg->week_open_flag);

				int now_hour_min = now_tm->tm_hour * 100 + now_tm->tm_min;
				if (!week_day_open || (draw_cfg->week_day_open_begin_time == draw_cfg->week_day_open_end_time && draw_cfg->week_day_open_end_time == 0 && (now_hour_min  < draw_cfg->week_day_open_begin_time || now_hour_min > draw_cfg->week_day_open_end_time)))
				{
					role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
					return -__LINE__;
				}
			}
			break;
		default:
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return -__LINE__;
			}
			break;
	}

	RoleCommonSaveXunBao * xun_bao_info = role->GetRoleModuleManager()->GetXunBaoInfo(draw_type);
	RoleCommonSaveXunBao * times_info = role->GetRoleModuleManager()->GetXunBaoInfo(draw_cfg->free_times_num);
	if (times_info == NULL || xun_bao_info == NULL)return -__LINE__;
	
	int consume_item_id = 0;
	int comsume_sum = 0;
	bool is_bind = false;
	std::vector<short> col_list;			//ConsumeItemByIndexList 调用需要
	std::vector<short> index_list;			//ConsumeItemByIndexList 调用需要
	std::vector<short> consume_num_list;	//ConsumeItemByIndexList 调用需要	

	for (int i = 0; !use_free_times && i < num; ++i)
	{
		ItemDataWrapper item_info;
		if (!role->GetRoleModuleManager()->GetKnapsack()->GetItemGridWrapData(comsume_info[i].col, comsume_info[i].idx, &item_info))
		{
			return -__LINE__;
		}

		if (consume_item_id == 0) consume_item_id = item_info.item_id;
		if (consume_item_id != item_info.item_id) return -__LINE__;

		is_bind = is_bind || item_info.is_bind;
		col_list.push_back(comsume_info[i].col);
		index_list.push_back(comsume_info[i].idx);
		consume_num_list.push_back(comsume_info[i].num);

		comsume_sum += comsume_info[i].num;
	}

	bool can_use_free_times = false;
	switch (draw_times)
	{
		case  1:
			{
				if (use_free_times && now >= times_info->free_time && draw_cfg->free_draw_interval != 0)
				{
					can_use_free_times = true;
					break;
				}
				if (consume_item_id == draw_cfg->once_item_id && comsume_sum == 1) break;

				return -__LINE__;
			}
			break;
		case 5:
			{
				if (consume_item_id == draw_cfg->once_item_id && comsume_sum == 5) break;
				if (consume_item_id == draw_cfg->five_item_id && comsume_sum == 1) break;

				return -__LINE__;
			}
			break;
		default:
			return -__LINE__;
	}

	if (!can_use_free_times && !role->GetRoleModuleManager()->GetKnapsack()->HasItem(consume_item_id, comsume_sum))
	{
		return -__LINE__;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckEmptyGridNoLessThanInTemporary(draw_times))
	{
		role->NoticeNum(errornum::EN_TEMPORARY_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}

	if (draw_cfg->is_has_pet_reward && !role->GetRoleModuleManager()->GetPet()->HaveSpace(draw_times, false))
	{
		role->NoticeNum(errornum::EN_PET_COUNT_LIMIT2);
		return -__LINE__;
	}
	
	std::vector<ItemConfigData> reward;
	std::vector<bool> is_broadcast_vec;

	int tmp_draw_times = draw_times;
	if (draw_times == 5 && draw_cfg->five_draw_floor_item_rate_count > 0 && !draw_cfg->five_draw_floor_cfg.empty()) //每次5连必定获得保底奖励（4随机+1保底）；
	{
		tmp_draw_times -= 1;

		ItemConfigData temp;
		bool is_b = false;
		if (!draw_cfg->GetFiveDrawFloorItem(temp))
		{
			return -__LINE__;
		}

		//temp.is_bind = is_bind; //如果是绑定的寻宝钥匙，所获的的物品也是绑定的
		is_broadcast_vec.push_back(is_b);
		reward.push_back(temp);
	}

	const CommonDataParam & common_data = role->GetRoleModuleManager()->GetCommonData();
	for (int i = 0; i < tmp_draw_times; ++i)
	{
		ItemConfigData temp;
		bool is_b = false;
		int xi_you_type = 0;
		if (!draw_cfg->Draw(role->GetLevel(), temp, is_b, xi_you_type, common_data.xun_bao_bless_pet_id_list[XUN_BAO_REWARD_TYPE_SILVER], common_data.xun_bao_bless_pet_id_list[XUN_BAO_REWARD_TYPE_GOLD]))
		{
			return -__LINE__;
		}

		//temp.is_bind = is_bind; //如果是绑定的寻宝钥匙，所获的的物品也是绑定的

		is_broadcast_vec.push_back(is_b);
		reward.push_back(temp);
	}

	if (reward.empty())
	{
		return -__LINE__;
	}

	int cumulative_times = role->GetRoleModuleManager()->GetXunBaoDrawCumulativeTimes(draw_cfg->draw_times_reward_group_id);
	int old_cumulative_times = cumulative_times;// 用来记录log
	cumulative_times += draw_times;

	//前5次随机一次发放首抽奖励
	bool can_effect_other_bao_di= true;  //其他保底是否起作用
	bool get_manual_pet_xun_bao_reward = false;
	bool get_fist_reward = false;
	bool get_second_reward = false;
	if (can_effect_other_bao_di)
	{
		bool is_can_first_draw = true;
		//仅限制该池子
		if (0 == role->GetRoleModuleManager()->GetCommonData().is_first_manual_pet_xun_bao_flag && draw_type == FIRST_MANUAL_COMMON_PET_XUN_BAO_DRAW_TYPE)
		{
			can_effect_other_bao_di  = false;
			
			//单抽则不进入下面的逻辑
			ItemConfigData manual_reward(LOGIC_CONFIG->GetXunBaoConfig()->GetOtherCfg().xunbao_pet_id_3, true, 1);
			if ((int)reward.size() <= 1)
			{
				is_can_first_draw = false;	
				reward.back() = manual_reward;		
			}
			else
			{
				int index = (int)reward.size() - 2;
				reward[index] = manual_reward;
			}

			get_manual_pet_xun_bao_reward = true;
		}

		if (is_can_first_draw && role->GetRoleModuleManager()->IsXunBaoFirstDraw(draw_type))
		{
			can_effect_other_bao_di = false;

			bool tmp_b = RandomNum(10) < 3;
			int first_draw_times = role->GetRoleModuleManager()->GetXunBaoFirstDrawTimes(draw_type);
			if (first_draw_times + draw_times >= XUN_BAO_FIRST_DRAW_TIMES)
			{
				tmp_b = true;
			}

			if (tmp_b)
			{
				ItemConfigData temp;
				if (!draw_cfg->GetFirstDrawFloorItem(temp))
				{
					return -__LINE__;
				}

				reward.back() = temp;
				is_broadcast_vec.back() = false;
				get_fist_reward = true;
			}
		}
	}

	if (can_effect_other_bao_di && !role->GetRoleModuleManager()->IsXunBaoFirstDraw(draw_type) && draw_cfg->second_draw_bao_di_times > 0)		//第一次（首5抽保底）保底完成才能出发，第二次保底（首20抽保底）
	{
		if (role->GetRoleModuleManager()->IsXunBaoSecondDraw(draw_type))
		{
			can_effect_other_bao_di = false;

			int reward_group_id = draw_cfg->second_draw_bao_di_reward_group_id;

			if (draw_cfg->is_reset_floors && LOGIC_CONFIG->GetXunBaoConfig()->IsHitDrawCumulativeReward(reward_group_id, reward))
			{
				get_second_reward = true;
				cumulative_times = 0;
			}


			const ItemConfigData * cumulative_reward_cfg = LOGIC_CONFIG->GetXunBaoConfig()->GetDrawCumulativeReward(reward_group_id, cumulative_times, draw_cfg->second_draw_bao_di_times);
			if (cumulative_reward_cfg != NULL)
			{
				get_second_reward = true;

				reward.back() = *cumulative_reward_cfg;
				is_broadcast_vec.back() = true;
				cumulative_times = 0;
			}			
		}
	}

	if (can_effect_other_bao_di && draw_cfg->can_cumulative_bao_di)
	{
		int reward_group_id = draw_cfg->draw_times_reward_group_id;

		if (draw_cfg->is_reset_floors && LOGIC_CONFIG->GetXunBaoConfig()->IsHitDrawCumulativeReward(reward_group_id, reward))
		{
			cumulative_times = 0;
		}

		const ItemConfigData * cumulative_reward_cfg = LOGIC_CONFIG->GetXunBaoConfig()->GetDrawCumulativeReward(reward_group_id, cumulative_times, draw_cfg->cumulative_bao_di_times);
		if (cumulative_reward_cfg != NULL)
		{
			reward.back() = *cumulative_reward_cfg;
			is_broadcast_vec.back() = true;
			cumulative_times = 0;
		}
	}

	int xi_you_bao_di_add_times = xun_bao_info->xi_you_floor_curr_times;
	if (can_effect_other_bao_di && draw_cfg->xi_you_bao_di_times > 0)
	{
		int xi_you_bao_di_add_times_old = xi_you_bao_di_add_times;
		for (int i = 0; i < (int)reward.size(); ++i)
		{
			const PetCfg * cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(reward[i].item_id);
			if (NULL != cfg)
			{
				if (cfg->quality == PET_QUALITY_GOLD)
				{
					xi_you_bao_di_add_times += 1;
				}

				if (cfg->quality == PET_QUALITY_RARE)
				{
					xi_you_bao_di_add_times = 0;
					xi_you_bao_di_add_times_old = 0;
					break;
				}
			}
		}

		if (xi_you_bao_di_add_times_old >= (int)draw_cfg->xi_you_bao_di_times)
		{
			const ItemConfigData * xi_you_baoi_di_reward_cfg = LOGIC_CONFIG->GetXunBaoConfig()->GetXiYouBaoReward(draw_type);
			if (xi_you_baoi_di_reward_cfg != NULL)
			{
				reward.back() = *xi_you_baoi_di_reward_cfg;
				is_broadcast_vec.back() = true;
				cumulative_times = 0;			//次数保底重置
				xi_you_bao_di_add_times = 0;		//稀有保底也重置
			}
		}
	}

	if (reward.empty() || !role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList((short)reward.size(), &reward[0]))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}

	if (!can_use_free_times)
	{
		if (col_list.empty()) return -__LINE__;
		if (!(col_list.size() == index_list.size() && col_list.size() == consume_num_list.size())) return -__LINE__;

		if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItemByIndexList((int)col_list.size(), &col_list[0], &index_list[0], &consume_num_list[0], __FUNCTION__))
		{
			return -__LINE__;
		}
	}
	else
	{
		times_info->free_time = (unsigned int)now + draw_cfg->free_draw_interval;
	}

	if (get_manual_pet_xun_bao_reward)
	{
		role->GetRoleModuleManager()->GetCommonData().is_first_manual_pet_xun_bao_flag = 1;
	}

	if (get_fist_reward)
	{
		role->GetRoleModuleManager()->SetXunBaoFirstDraw(draw_type);
	}

	if (get_second_reward)
	{
		role->GetRoleModuleManager()->SetXunBaoSecondDraw(draw_type);
	}

	xun_bao_info->xi_you_floor_curr_times = xi_you_bao_di_add_times;

	role->GetRoleModuleManager()->AddXunBaoFirstDrawTimes(draw_type, draw_times);
	role->GetRoleModuleManager()->SetXunBaoDrawCumulativeTimes(draw_cfg->draw_times_reward_group_id, cumulative_times);
	role->GetRoleModuleManager()->SendXunBaoInfo();

	EventHandler::Instance().OnTueasureHunt(role, draw_times, draw_cfg->xun_bao_treasure_type);
	this->SendDrawResult(role, reward);

	std::string log_str = STRING_SPRINTF("draw_type:%d  is_use_free_times:%d draw_times:%d  fisrt_draw_times:%d  is_fisrt:%d is_second:%d  old_cumulative_times:%d  cumulative_times:%d  rewards:{",
		draw_type, (int)can_use_free_times, draw_times, role->GetRoleModuleManager()->GetXunBaoFirstDrawTimes(draw_type), (int)get_fist_reward, (int)get_second_reward, old_cumulative_times, cumulative_times);
	for (int i = 0; i < (int)is_broadcast_vec.size() && i < (int)reward.size(); ++i)
	{
		log_str += STRING_SPRINTF("[item_id:%d num:%d is_bind:%d is_broadcast:%d] ", reward[i].item_id, reward[i].num, (int)reward[i].is_bind, (int)is_broadcast_vec[i]);

		if (!is_broadcast_vec[i])
		{
			continue;
		}

		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_xun_bao_broadcast, role->GetUID(), role->GetName(), reward[i].item_id);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	log_str += "}";
	gamelog::g_log_xun_bao.printf(LL_INFO, "XunBao::Draw role_id:%d,role_name:%s %s", role->GetUID(), role->GetName(), log_str.c_str());

	return 0;
}

void XunBao::SendOpenInfo(Role * role)
{
	std::vector<const XunBaoDrawCfg *> open_cfg = LOGIC_CONFIG->GetXunBaoConfig()->GetOpenDrawCfg();

	Protocol::SCXunBaoOpenInfo info;
	info.count = 0;

	for (int i = 0; i < (int)open_cfg.size() && i < ARRAY_LENGTH(info.open_info); ++i)
	{
		info.open_info[i].draw_type = (short)open_cfg[i]->draw_type;
		info.open_info[i].is_open = open_cfg[i]->is_open;
		info.open_info[i].open_day = open_cfg[i]->open_day;
		info.open_info[i].open_type = open_cfg[i]->open_type;
		info.open_info[i].start_time = (unsigned int)open_cfg[i]->start_time;
		info.open_info[i].end_time = (unsigned int)open_cfg[i]->end_time;
		info.open_info[i].week_open_flag = (unsigned int)open_cfg[i]->week_open_flag;
		info.open_info[i].day_begin_time = (unsigned short)open_cfg[i]->week_day_open_begin_time;
		info.open_info[i].day_end_time = (unsigned short)open_cfg[i]->week_day_open_end_time;

		info.count += 1;
	}

	unsigned int send_len = sizeof(info) - (ARRAY_LENGTH(info.open_info) - info.count) * sizeof(info.open_info[0]);

	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, send_len);
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&info, send_len);
	}

}

void XunBao::DrawGuidePet(Role * role, int column, int index)
{
	if (role == NULL)
	{
		return;
	}

	if (role->GetRoleModuleManager()->GetFuncGuide()->IsDrawGuidePet())
	{
		return;
	}

	ItemDataWrapper item_info;
	if (!role->GetRoleModuleManager()->GetKnapsack()->GetItemGridWrapData(column, index, &item_info))
	{
		return;
	}

	int consume_item_id = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg().draw_guide_pet_comsume_item;
	if (item_info.item_id != consume_item_id)
	{
		return;
	}


	if (!role->GetRoleModuleManager()->GetPet()->HaveSpace(1, false))
	{
		role->NoticeNum(errornum::EN_PET_COUNT_LIMIT2);
		return;
	}

	const ItemConfigData * item = LOGIC_CONFIG->GetFuncGuideConfig()->GetGuidePet();
	if (NULL == item)
	{
		return;
	}

	const ItemBase * item_ib = ITEMPOOL->GetItem(item->item_id);
	if (item_ib == NULL || item_ib->GetItemType() != I_TYPE_SPECIAL_PET)
	{
		return;
	}

	ItemDataWrapper temp;
	if (!role->GetRoleModuleManager()->GetPet()->FillSpecialPetData(item_ib->GetItemId(), temp))
	{
		return;
	}

	NetValueItemParam param;
	param.SetData(temp.param_data);
	int pet_index = role->GetRoleModuleManager()->GetPet()->AddPet(&param, __FUNCTION__, temp.is_bind, false, temp.item_unique_id);
	if (pet_index < 0)
	{
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItemByIndex(column, index, 1, __FUNCTION__))
	{
		return;
	}

	Protocol::SCXunBaoRet ret;
	ret.item_list[0].type = 1;
	ret.item_list[0].id = pet_index;
	ret.item_list[0].num = 1;
	ret.count = 1;
	int send_len = sizeof(ret) - (ARRAY_LENGTH(ret.item_list) - ret.count) * sizeof(ret.item_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &ret, send_len);

	role->GetRoleModuleManager()->GetFuncGuide()->OnDrawGuidePet();
}

void XunBao::DrawGuidePet2(Role * role, int column, int index)
{
	if (role == NULL)
	{
		return;
	}

	if (role->GetRoleModuleManager()->GetFuncGuide()->IsDrawGuidePet2())
	{
		return;
	}

	ItemDataWrapper item_info;
	if (!role->GetRoleModuleManager()->GetKnapsack()->GetItemGridWrapData(column, index, &item_info))
	{
		return;
	}
	const FuncGuideOtherCfg & other_cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg();
	int consume_item_id = other_cfg.draw_guide_pet_comsume_item;
	if (item_info.item_id != consume_item_id)
	{
		return;
	}


	if (!role->GetRoleModuleManager()->GetPet()->HaveSpace(1, false))
	{
		role->NoticeNum(errornum::EN_PET_COUNT_LIMIT2);
		return;
	}


	const ItemBase * item_ib = ITEMPOOL->GetItem(other_cfg.draw_guide_pet_2_reward_special_item_id);
	if (item_ib == NULL || item_ib->GetItemType() != I_TYPE_SPECIAL_PET)
	{
		return;
	}

	ItemDataWrapper temp;
	if (!role->GetRoleModuleManager()->GetPet()->FillSpecialPetData(item_ib->GetItemId(), temp, other_cfg.draw_guide_pet_2_pet_level))
	{
		return;
	}

	NetValueItemParam param;
	param.SetData(temp.param_data);

	int pet_index = role->GetRoleModuleManager()->GetPet()->AddPet(&param, __FUNCTION__, temp.is_bind, false, temp.item_unique_id);
	if (pet_index < 0)
	{
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItemByIndex(column, index, 1, __FUNCTION__))
	{
		return;
	}

	Protocol::SCXunBaoRet ret;
	ret.item_list[0].type = 1;
	ret.item_list[0].id = pet_index;
	ret.item_list[0].num = 1;
	ret.count = 1;
	int send_len = sizeof(ret) - (ARRAY_LENGTH(ret.item_list) - ret.count) * sizeof(ret.item_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &ret, send_len);

	role->GetRoleModuleManager()->GetFuncGuide()->OnDrawGuidePet2();
}

void XunBao::DrawGuidePartner(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	if (role->GetRoleModuleManager()->GetFuncGuide()->IsDrawGuidePartner())
	{
		return;
	}

	const FuncGuideOtherCfg & other_cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg();

	const std::vector<ItemConfigData> & rewards = other_cfg.draw_guide_partner_reward;


	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(rewards))
	{
		return;
	}


	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(other_cfg.draw_guide_partner_comsume_item_id, other_cfg.draw_guide_partner_comsume_item_num, __FUNCTION__))
	{
		return;
	}

	Protocol::SCXunBaoRet ret;
	ret.count = 0;
	for (int i = 0; i < (int)rewards.size(); ++i)
	{

		const ItemConfigData & reward = rewards[i];
		const ItemBase * item_ib = ITEMPOOL->GetItem(reward.item_id);
		if (!item_ib)
		{
			return;
		}

		bool active_ret = false;
		//检查主动消耗
		if (item_ib->GetItemType() == I_TYPE_EXPENSE)
		{
			NorexItem * nor_item = (NorexItem *)item_ib;
			if (nor_item->GetUseType() == NorexItem::I_NOREX_ITEM_PARTNER_LICENCE)
			{
				int partner_id = -1;

				active_ret = role->GetRoleModuleManager()->GetPartner()->ActiveAppointedPartner(reward.item_id, UNLOCK_TYPE_ITEM_NOREX, &partner_id, false);

				if (active_ret)
				{

					ret.item_list[0].type = 2;
					ret.item_list[0].id = partner_id;
					ret.item_list[0].num = 1;
					ret.count += 1;
				}
			}
		}

		if (!active_ret)
		{
			role->GetRoleModuleManager()->GetKnapsack()->Put(reward, PUT_REASON_XUN_BAO_GUIDE_PARTNER);
			ret.item_list[ret.count].type = 0;
			ret.item_list[ret.count].id = reward.item_id;
			ret.item_list[ret.count].num = reward.num;
			ret.count += 1;

		}
	}

	int send_len = sizeof(ret) - (ARRAY_LENGTH(ret.item_list) - ret.count) * sizeof(ret.item_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &ret, send_len);

	role->GetRoleModuleManager()->GetFuncGuide()->OnDrawGuidePartner();

}

void XunBao::DrawGuidePartner2(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	if (role->GetRoleModuleManager()->GetFuncGuide()->IsDrawGuidePartner2())
	{
		return;
	}

	const FuncGuideOtherCfg & other_cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg();

	const std::vector<ItemConfigData> & rewards = other_cfg.draw_guide_partner_reward_2;

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(rewards))
	{
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(other_cfg.draw_guide_partner_comsume_item_id_2, other_cfg.draw_guide_partner_comsume_item_num_2, __FUNCTION__))
	{
		return;
	}

	Protocol::SCXunBaoRet ret;
	ret.count = 0;
	for (int i = 0; i < (int)rewards.size(); ++i)
	{

		const ItemConfigData & reward = rewards[i];
		const ItemBase * item_ib = ITEMPOOL->GetItem(reward.item_id);
		if (!item_ib)
		{
			return;
		}

		bool active_ret = false;
		//检查主动消耗
		if (item_ib->GetItemType() == I_TYPE_EXPENSE)
		{
			NorexItem * nor_item = (NorexItem *)item_ib;
			if (nor_item->GetUseType() == NorexItem::I_NOREX_ITEM_PARTNER_LICENCE)
			{
				int partner_id = -1;

				active_ret = role->GetRoleModuleManager()->GetPartner()->ActiveAppointedPartner(reward.item_id, UNLOCK_TYPE_ITEM_NOREX, &partner_id, false);

				if (active_ret)
				{

					ret.item_list[ret.count].type = 2;
					ret.item_list[ret.count].id = partner_id;
					ret.item_list[ret.count].num = 1;
					ret.count += 1;
				}
			}
		}

		if (!active_ret)
		{
			role->GetRoleModuleManager()->GetKnapsack()->Put(reward, PUT_REASON_XUN_BAO_GUIDE_PARTNER);
			ret.item_list[ret.count].type = 0;
			ret.item_list[ret.count].id = reward.item_id;
			ret.item_list[ret.count].num = reward.num;
			ret.count += 1;

		}
	}

	int send_len = sizeof(ret) - (ARRAY_LENGTH(ret.item_list) - ret.count) * sizeof(ret.item_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &ret, send_len);

	role->GetRoleModuleManager()->GetFuncGuide()->OnDrawGuidePartner2();
}

int XunBao::ExchangeItem(Role * role, int draw_type, int get_seq, int comsume_seq1, int comsume_seq2,
			 const std::set<Protocol::XunBaoExchangeComsumeItem> & comsume1, const std::set<Protocol::XunBaoExchangeComsumeItem> & comsume2)
{
	XunBaoDrawCfg * draw_cfg = LOGIC_CONFIG->GetXunBaoConfig()->GetXunBaoDrawCfg(draw_type);
	if (draw_cfg == NULL)
	{
		return -__LINE__;
	}

	bool is_bind = false;
	std::set<int> comsume_pet_index_set;
	{
		const XunBaoExchangeComsumeCfg * comsume_cfg1 = LOGIC_CONFIG->GetXunBaoConfig()->GetExchangeComsumeCfg(draw_cfg->exchange_material_id1, comsume_seq1);
		if (comsume_cfg1 == NULL)
		{
			return -__LINE__;
		}

		if ((int)comsume1.size() != comsume_cfg1->num)
		{
			return -__LINE__;
		}

		for (std::set<Protocol::XunBaoExchangeComsumeItem>::const_iterator iter = comsume1.begin(); iter != comsume1.end(); ++iter)
		{
			int index = iter->index;
			if (iter->type == 0)
			{
				const ItemGridData * pet_grid = role->GetRoleModuleManager()->GetPet()->GetPetGridData(index);
				if (pet_grid == NULL)
				{
					return -__LINE__;
				}

				is_bind = is_bind || pet_grid->is_bind;

				const PetParam & pet = pet_grid->param->pet_param;
				if (pet.str_level < comsume_cfg1->strengthen_level)
				{
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetPet()->IsLock(iter->index))
				{
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetSmartMountsSystem()->IsPetDomination(pet_grid->item_unique_id))
				{
					role->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetPet()->IsPetBenedictionVice(iter->index))
				{
					role->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetPet()->IsInPlatfformBattle(pet_grid->item_unique_id, true))
				{
					return -__LINE__;
				}

				if (0 != pet.pet_equipment_id)
				{
					role->NoticeNum(errornum::EN_PET_HSA_EQUIPMENT_ERR);
					return -__LINE__;
				}

				comsume_pet_index_set.insert(index);
			}
			else
			{
				const ItemGridData * pet_grid = role->GetRoleModuleManager()->GetPet()->GetPetGridDataFromStorage(index);
				if (pet_grid == NULL)
				{
					return -__LINE__;
				}
				const PetParam & pet = pet_grid->param->pet_param;
				if (pet.str_level < comsume_cfg1->strengthen_level)
				{
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetPet()->IsInPlatfformBattle(pet_grid->item_unique_id, true))
				{
					return -__LINE__;
				}

				comsume_pet_index_set.insert(index + ItemNamespace::PET_STORAGE_INDEX_BEGIN);
			}
		}
	}
	{
		const XunBaoExchangeComsumeCfg * comsume_cfg2 = LOGIC_CONFIG->GetXunBaoConfig()->GetExchangeComsumeCfg(draw_cfg->exchange_material_id2, comsume_seq2);
		if (comsume_cfg2 == NULL)
		{
			return -__LINE__;
		}

		if ((int)comsume2.size() != comsume_cfg2->num)
		{
			return -__LINE__;
		}

		for (std::set<Protocol::XunBaoExchangeComsumeItem>::const_iterator iter = comsume2.begin(); iter != comsume2.end(); ++iter)
		{
			int index = iter->index;
			if (iter->type == 0)
			{
				const ItemGridData * pet_grid = role->GetRoleModuleManager()->GetPet()->GetPetGridData(iter->index);
				if (pet_grid == NULL)
				{
					return -__LINE__;
				}
				const PetParam & pet = pet_grid->param->pet_param;
				if (pet.str_level < comsume_cfg2->strengthen_level)
				{
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetPet()->IsLock(iter->index))
				{
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetSmartMountsSystem()->IsPetDomination(pet_grid->item_unique_id))
				{
					role->NoticeNum(errornum::EN_SMART_MOUNTS_SYSTEM_PET_DOMINATION);
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetPet()->IsPetBenedictionVice(iter->index))
				{
					role->NoticeNum(errornum::EN_PET_NOT_OP_BY_BENEDICTION);
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetPet()->IsInPlatfformBattle(pet_grid->item_unique_id, true))
				{
					return -__LINE__;
				}

				if (0 != pet.pet_equipment_id)
				{
					role->NoticeNum(errornum::EN_PET_HSA_EQUIPMENT_ERR);
					return -__LINE__;
				}

				comsume_pet_index_set.insert(index);
			}
			else
			{
				const ItemGridData * pet_grid = role->GetRoleModuleManager()->GetPet()->GetPetGridDataFromStorage(iter->index);
				if (pet_grid == NULL)
				{
					return -__LINE__;
				}

				const PetParam & pet = pet_grid->param->pet_param;
				if (pet.str_level < comsume_cfg2->strengthen_level)
				{
					return -__LINE__;
				}

				if (role->GetRoleModuleManager()->GetPet()->IsInPlatfformBattle(pet_grid->item_unique_id, true))
				{
					return -__LINE__;
				}

				comsume_pet_index_set.insert(index + ItemNamespace::PET_STORAGE_INDEX_BEGIN);
			}
		}
	}

	const XunBaoExchangeGetCfg * get_cfg = LOGIC_CONFIG->GetXunBaoConfig()->GetExchangeGetCfg(draw_cfg->exchange_get_id, get_seq);
	if (get_cfg == NULL)
	{
		return -__LINE__;
	}

	if (!role->GetRoleModuleManager()->GetPet()->HaveSpace(get_cfg->num, false))
	{
		role->NoticeNum(errornum::EN_PET_COUNT_LIMIT2);
		return -__LINE__;
	}

	if (role->GetRoleModuleManager()->GetPet()->ConversionExpByList(comsume_pet_index_set, 1, __FUNCTION__) != 0)
	{
		return -__LINE__;
	}

	role->GetRoleModuleManager()->GetPet()->DeletePetByList(comsume_pet_index_set, __FUNCTION__);
	for (int i = 0; i < get_cfg->num; ++i)
	{
		role->GetRoleModuleManager()->GetPet()->AcquirePet(get_cfg->pet_id, __FUNCTION__, get_cfg->strengthen_level, false, GET_PET_REASON_TYPE_XUN_BAO_EXCHANGE, is_bind);
	}

	gamelog::g_log_xun_bao.printf(LL_INFO, "%s role_id:%d,role_name:%s", __FUNCTION__, role->GetUID(), role->GetName());

	return 0;

}

void XunBao::SendDrawResult(Role * role, const std::vector<ItemConfigData> & reward)
{
	if (!role)
	{
		return;
	}

	Protocol::SCXunBaoRet ret;
	ret.count = 0;
	for (int i = 0; i < (int)reward.size(); ++i)
	{
		const ItemConfigData & curr = reward[i];
		const ItemBase * item_ib = ITEMPOOL->GetItem(curr.item_id);
		if (NULL != item_ib)
		{
			switch (item_ib->GetBagsBelong())
			{
				case B_BELONG_PET_BAG:
					{
						if (I_TYPE_PET == item_ib->GetItemType())
						{
							int pet_idnex = role->GetRoleModuleManager()->GetPet()->AcquirePet(curr.item_id, __FUNCTION__, 1, false, GET_PET_REASON_TYPE_INVALID, curr.is_bind, true);
							if (pet_idnex >= 0 && ret.count < ARRAY_LENGTH(ret.item_list))
							{
								ret.item_list[ret.count].type = 1;
								ret.item_list[ret.count].id = pet_idnex;
								ret.item_list[ret.count].num = 1;
								ret.count++;
							}
						}
						else if (I_TYPE_SPECIAL_PET == item_ib->GetItemType())
						{
							int pet_idnex = role->GetRoleModuleManager()->GetPet()->AcquireSpecialPet(item_ib, __FUNCTION__, curr.is_bind, true);
							if (pet_idnex >= 0 && ret.count < ARRAY_LENGTH(ret.item_list))
							{
								ret.item_list[ret.count].type = 1;
								ret.item_list[ret.count].id = pet_idnex;
								ret.item_list[ret.count].num = 1;
								ret.count++;
							}
						}
					}
					break;
				default:
					{
						bool is_get = false;
						if (item_ib->GetItemType() == I_TYPE_EXPENSE)
						{
							NorexItem * nor_item = (NorexItem *)item_ib;
							if (nor_item->GetUseType() == NorexItem::I_NOREX_ITEM_PARTNER_LICENCE)
							{
								int partner_id = -1;

								bool active_ret = role->GetRoleModuleManager()->GetPartner()->ActiveAppointedPartner(curr.item_id, UNLOCK_TYPE_ITEM_NOREX, &partner_id, false);

								if (active_ret)
								{
									is_get = true;
									if (partner_id >= 0 && ret.count < ARRAY_LENGTH(ret.item_list))
									{
										ret.item_list[ret.count].type = 2;
										ret.item_list[ret.count].id = partner_id;
										ret.item_list[ret.count].num = 1;
										ret.count++;
									}
								}
							}
						}

						if (!is_get)
						{
							role->GetRoleModuleManager()->GetKnapsack()->Put(curr, PUT_REASON_XUN_BAO);
							if (ret.count < ARRAY_LENGTH(ret.item_list))
							{
								ret.item_list[ret.count].type = 0;
								ret.item_list[ret.count].id = curr.item_id;
								ret.item_list[ret.count].num = curr.num;
								ret.count++;
							}
						}
					}
					break;
			}
		}
	}
	EngineAdapter::Instance().NetSend(role->GetNetId(), &ret, sizeof(ret));
}

XunBao::XunBao()
{
}

XunBao::~XunBao()
{
}