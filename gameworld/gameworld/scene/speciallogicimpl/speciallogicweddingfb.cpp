#include "speciallogicweddingfb.hpp"
#include "qingyuan/qingyuanconfig.hpp"
#include "qingyuan/weddingmanger.hpp"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "item/knapsack.h"

#include "protocol/msgqingyuan.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "obj/character/npc_monster.h"
#include "npc/npcmanager.h"
#include "global/qingyuan/qingyuanmanager.h"
#include "other/surface/surface.hpp"
#include "global/usercache/usercache.hpp"
#include "qingyuan/weddingmanger.hpp"

SpecialLogicWeddingFB::SpecialLogicWeddingFB(Scene * scene)
	: SpecialLogic(scene)
{
	m_is_finish = 0;

	m_wedding_id = 0;
	
	m_fb_begin_time = 0;
	m_fb_end_time = 0;

	m_link_seq = 0;
	m_link_begin_time = 0;
	m_link_end_time = 0;

	m_wedding_heat = 0;

	m_next_check_time = 0;
}

void SpecialLogicWeddingFB::Update(unsigned long interval, time_t now_second)
{
	if (0 != m_is_finish
		|| 0 == m_link_begin_time)
	{
		return;
	}

	if (0 == m_next_check_time)
	{//first

	}

	if (m_next_check_time < now_second)
	{
		m_next_check_time = now_second + 1;

		this->CheckLink(now_second);
	}
}

void SpecialLogicWeddingFB::OnRoleEnterScene(Role * role)
{
	if (NULL == role)
	{
		this->DelayKickOutRole(role);
		return;
	}

	if (this->CenEnterFB(role))
	{//无法领取
		this->DelayKickOutRole(role);
		return;
	}
	int role_uid = role->GetUID();

	//WeddingInfo * wedding_info = WeddingManger::Instance().GetWeddingInfo(m_wedding_id);
	//if (NULL == wedding_info)
	//{
	//	this->DelayKickOutRole(role);
	//	return;
	//}
	//
	//if (NULL != wedding_info->GetHost(role_uid))
	//{//主办者 换新服
	//	role->GetRoleModuleManager()->GetSurface()->WearSurface(SURFACE_TYPE_BODY,
	//		LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg().weddding_suit_id);
	//}

	if (NULL == this->GetRoleInfo(role_uid))
	{
		WeddingFB::RoleInfo * role_info = &m_join_role_info_map[role_uid];
		role_info->role_uid = role_uid;
		
		const WeddingFB::SubActXubQingMiAi & xu_qing_mi_ai_cfg 
			= LOGIC_CONFIG->GetQingYuanConfig()->GetSubActXubQingMiAi();
		if (!xu_qing_mi_ai_cfg.gift_pos.empty())
		{
			int _rand = RandomNum((int)xu_qing_mi_ai_cfg.gift_pos.size());
			role_info->xun_qing_mi_ai_posi = xu_qing_mi_ai_cfg.gift_pos[_rand];
		}
	}

	
	
	
	this->SendSceneInfo(role);
	this->SendRoleInfo(role);

	//小游戏通知
	this->SendXubQingMiAi(role);
	this->SendXinXinXiangYin(role);
	this->SendBaoWeiXiuQiu(role);
	this->SendZaoShengGuiZi(role);
}

void SpecialLogicWeddingFB::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (0 != m_is_finish)
	{
		return;
	}

	if (RESULT_TYPE_ATTACKER_WIN != ack->battle_result
		|| BATTLE_MODE_WEDDING_FB_BAO_WEI_XIU_QIU != ack->battle_mode)
	{
		return;
	}

	for (int index = 0
		; index < ack->attacker_list.role_count
		&& index < SIDE_MAX_ROLE_NUM
		; ++index)
	{
		Role * role = this->GetScene()->GetRoleByUID(ack->attacker_list.role_list[index].uid);
		if (NULL == role)
		{
			continue;
		}

		this->FetchBaoWeiXiuQiu(role, ack->business_data.param1);
	}
	
}

bool SpecialLogicWeddingFB::CanRoleStartFight(Role * role, CanRoleStartFightParam & fight_param)
{
	switch (fight_param.battle_mode)
	{
	case BATTLE_MODE_WEDDING_FB:
	case BATTLE_MODE_WEDDING_FB_BAO_WEI_XIU_QIU:
		{
			return true;
		}
	default:
		return false;
	}
	return false;
}

void SpecialLogicWeddingFB::SetSceneParam(int wedding_id, time_t begin_time)
{
	if (0 != m_wedding_id)
	{
		return;
	}
	m_wedding_id = wedding_id;

	m_fb_begin_time = begin_time - LOGIC_CONFIG->GetQingYuanConfig()->GetFbPrePareTime();
	m_fb_end_time = m_fb_begin_time + LOGIC_CONFIG->GetQingYuanConfig()->GetFbTotalTime();

	time_t now_time = EngineAdapter::Instance().Time();

	m_link_seq = 0;
	m_link_begin_time = m_fb_begin_time;
	m_link_end_time = m_fb_begin_time;

	this->CheckLink(now_time);
}

void SpecialLogicWeddingFB::OnFBCommonReq(Role * role, int common_type, int param)
{
	if (NULL == role)
	{
		return;
	}

	UNSTD_STATIC_CHECK(7 == WeddingFB::COMMON_OPER_TYPE_MAX)

	switch (common_type)
	{
	case WeddingFB::COMMON_OPER_TYPE_CANDIES:
		{
			this->FetchCandies(role);
		}
		break;
	case WeddingFB::COMMON_OPER_TYPE_RITE:
		{
			this->SetRiteParam(role, param);
		}
		break;
	case WeddingFB::COMMON_OPER_TYPE_MEAL:
		{
			this->Meal(role);
		}
		break;
	case WeddingFB::COMMON_OPER_TYPE_BLESS:
		{
			this->BuyBless(role, param);
		}
		break;
	case WeddingFB::COMMON_OPER_TYPE_HEAT_REWARD:
		{
			this->FetchHeatReward(role, param);
		}
		break;
	default:
		return;
	}
}

void SpecialLogicWeddingFB::OnFBSubActReq(Role * role, int common_type, int param)
{
	if (NULL == role)
	{
		return;
	}

	UNSTD_STATIC_CHECK(5 == WeddingFB::SUB_ACT_TYPE_MAX);
	switch (common_type)
	{
	case WeddingFB::SUB_ACT_TYPE_XUN_QING_MI_AI:
		{
			this->FetchXubQingMiAi(role);
		}
		break;
	case WeddingFB::SUB_ACT_TYPE_XIN_XIN_XIANG_YIN:
		{
			this->FetchXinXinXiangYin(role);
		}
		break;
	case WeddingFB::SUB_ACT_TYPE_BAO_WEI_XIU_QIU:
		{
			//this->FetchBaoWeiXiuQiu(role, param);
		}
		break;
	case WeddingFB::SUB_ACT_TYPE_ZAO_SHENG_GUI_ZI:
		{
			this->FetchZaoShengGuiZi(role, param);
		}
		break;
	default:
		return;
	}
}

void SpecialLogicWeddingFB::SendSceneInfo(Role * role)
{
	Protocol::SCWeddingFBSceneInfo info;
	info.m_is_finish = m_is_finish;
	info.m_wedding_id = m_wedding_id;

	info.m_fb_begin_time = (unsigned int)m_fb_begin_time;
	info.m_fb_end_time = (unsigned int)m_fb_end_time;

	info.m_link_seq = m_link_seq;

	info.m_link_begin_time = (unsigned int)m_link_begin_time;
	info.m_link_end_time = (unsigned int)m_link_end_time;
	info.m_wedding_heat = m_wedding_heat;

	info.xun_you_type = 0;

	WeddingInfo * wedding_info = WeddingManger::Instance().GetWeddingInfo(m_wedding_id);
	if (NULL != wedding_info)
	{
		info.xun_you_type = wedding_info->GetCommonInfo(false).xun_you_type;

		for (int i = 0; i < WeddingFB::HOST_NUM; i++)
		{
			info.host_uid[i] = wedding_info->GetHostUid(i);

			UserCacheNode *node = UserCacheManager::Instance().GetUserNode(info.host_uid[i]);
			if (NULL != node)
			{
				node->GetName(info.host_name[i]);
				info.host_avatar_type[i] = node->avatar_type;
			}
		}
	}

	if (role != NULL)
	{
		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
	else
	{
		this->GetScene()->SendToRole((const char *)&info, sizeof(info));
	}

}

void SpecialLogicWeddingFB::SendRoleInfo(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		return;
	}
	
	Protocol::SCWeddingFBRoleInfo info;
	info.meal__last_fetch_time = (unsigned int)role_info->meal__last_fetch_time;

	info.count = WeddingFB::COMMON_OPER_TYPE_MAX;
	memcpy(info.commom_param, role_info->commom_param, sizeof(info.commom_param));

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void SpecialLogicWeddingFB::SendFBSync(Role * role, int sync_type, int param)
{
	Protocol::SCWeddingFBSync info;
	info.sync_type = sync_type;
	info.param = param;

	if (role != NULL)
	{
		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
	else
	{
		this->GetScene()->SendToRole((const char *)&info, sizeof(info));
	}
}

void SpecialLogicWeddingFB::SendBarrage(Role *role, const char * chat, unsigned int chat_len)
{
	if (chat_len >= (unsigned int)REAL_TIME_COMMENT_LEN) return;

	if (NULL == role)
	{
		return;
	}

	if (NULL == chat)
	{
		return;
	}

	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		return;
	}

	int oper_cfg_param = LOGIC_CONFIG->GetQingYuanConfig()
		->GetFBCommonOperParam(m_link_seq, WeddingFB::COMMON_OPER_TYPE_BARRAGE);

	if (1 != oper_cfg_param)
	{//无法领取
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	{//热度
		const WeddingFB::BaseCfg & base_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg();
		if (base_cfg.barrage_heat_add_times > role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_BARRAGE])
		{
			this->AddHeat(base_cfg.barrage_heat_add_num);
			role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_BARRAGE]++;

			this->SendRoleInfo(role);
		}
	}

	Protocol::SCRealTimeComment info;
	info.comment_type = Protocol::CSRealTimeComment::COMMENT_TYPE_WEDDING_FB;
	info.sender_uid = role->GetUID();
	role->GetName(info.sender_name);
	info.msg_length = chat_len;
	memcpy(info.msg_content, chat, info.msg_length);

	int send_length = sizeof(info) - sizeof(info.msg_content) + info.msg_length;
	this->GetScene()->SendToRole((const char*)&info, send_length);
}

int SpecialLogicWeddingFB::CenEnterFB(Role * role)
{
	if (NULL == role)
	{
		return -1;
	}
	if (0 != m_is_finish)
	{//已完成
		return -2;
	}

	int role_uid = role->GetUID();

	WeddingInfo * wedding_info = WeddingManger::Instance().GetWeddingInfo(m_wedding_id);
	if (NULL == wedding_info)
	{
		return -3;
	}
	else
	{
		if (NULL != wedding_info->GetHost(role->GetUID()))
		{//主办者 换新服
			//role->GetRoleModuleManager()->GetSurface()->WearSurface(SURFACE_TYPE_BODY,
			//	LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg().weddding_suit_id);
		}
		else if (!wedding_info->BeInvite(role_uid))
		{//被邀请
			return -4;
		}
	}

	int oper_cfg_param = LOGIC_CONFIG->GetQingYuanConfig()
		->GetFBCommonOperParam(m_link_seq, WeddingFB::COMMON_OPER_TYPE_ENTER_SCENE);

	if (1 != oper_cfg_param)
	{//无法领取
		role->NoticeNum(errornum::EN_WEDDING_WAIT_LINK);
		return -5;
	}

	return 0;
}

void SpecialLogicWeddingFB::GMNextLink()
{
	m_link_end_time = EngineAdapter::Instance().Time();
}

void SpecialLogicWeddingFB::CheckLink(time_t now_second)
{
	if (0 != m_is_finish
		|| 0 == m_link_begin_time)
	{
		return;
	}
	if (m_link_end_time > now_second)
	{
		return;
	}
	const WeddingFB::LinkCfg * cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetFBLinkCfg(m_link_seq + 1);
	if (NULL == cfg)
	{
		this->Finish();
		return;
	}

	m_link_seq++;
	m_link_begin_time = m_link_end_time;
	m_link_end_time = m_link_begin_time + cfg->link_time;

	this->SendSceneInfo(NULL);
}

void SpecialLogicWeddingFB::Finish()
{
	m_is_finish = 1;

	this->SendSceneInfo(NULL);

	WeddingManger::Instance().OnWeddingFinish(m_wedding_id);

	this->DelayKickOutAllRole(CHANGE_SCENE_REASON_FB_FINISH);
}

void SpecialLogicWeddingFB::FetchCandies(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int oper_cfg_param = LOGIC_CONFIG->GetQingYuanConfig()
		->GetFBCommonOperParam(m_link_seq, WeddingFB::COMMON_OPER_TYPE_CANDIES);

	if (1 != oper_cfg_param)
	{//无法领取
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	if (0 != role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_CANDIES])
	{
		role->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
		return;
	}
	const WeddingFB::BaseCfg & base_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg();

	//记录
	role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_CANDIES]++;

	//发放
	if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut2(base_cfg.candies))
	{
		role->GetRoleModuleManager()->GetKnapsack()->Put(base_cfg.candies, PUT_REASON_WEDDING);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(&base_cfg.candies, 1, SNED_MAIL_TYPE_DEFAULT, true);
	}

	this->SendRoleInfo(role);

	//日志
	gamelog::g_log_qing_yuan.printf(LL_INFO, "SpecialLogicWeddingFB::FetchCandies "
		"| role_id[%d] role_info_param:[%d]",
		role_info->role_uid, role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_CANDIES]);
}

void SpecialLogicWeddingFB::SetRiteParam(Role * role, int param)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//记录
	role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_RITE] = param;

	this->SendRoleInfo(role);
}

void SpecialLogicWeddingFB::Meal(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int oper_cfg_param = LOGIC_CONFIG->GetQingYuanConfig()
		->GetFBCommonOperParam(m_link_seq, WeddingFB::COMMON_OPER_TYPE_MEAL);

	if (1 != oper_cfg_param)
	{//无法领取
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	
	const WeddingFB::BaseCfg & base_cfg = LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg();

	if (base_cfg.meal__max_num <= role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_MEAL])
	{
		role->NoticeNum(errornum::EN_WEDDING_MEAL_MAX);
		return;
	}

	time_t now_secend = EngineAdapter::Instance().Time();
	if (now_secend < base_cfg.meal__interval_time_s + role_info->meal__last_fetch_time)
	{
		role->NoticeNum(errornum::EN_WEDDING_MEAL_FAST);
		return;
	}

	//记录
	role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_MEAL]++;
	role_info->meal__last_fetch_time = now_secend - 1;

	//奖励
	long long reward_coin_bind = 0;
	std::vector<ItemConfigData> temp_reward_vec;
	int group_id_size = (int)base_cfg.meal__group_id_list.size();
	for (int i = 0; i < group_id_size; i++)
	{
		const WeddingFB::GroupItemCfg * reward_cfg
			= LOGIC_CONFIG->GetQingYuanConfig()->GetGroupRandCfg(base_cfg.meal__group_id_list[i]);
		if (NULL == reward_cfg)
		{
			continue;
		}

		reward_coin_bind += reward_cfg->coin_bind;
		if (0 != reward_cfg->reward_item.item_id)
		{
			temp_reward_vec.push_back(reward_cfg->reward_item);
		}
	}

	//发放
	if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(temp_reward_vec))
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutVec(temp_reward_vec, PUT_REASON_WEDDING);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(&base_cfg.candies, 1, SNED_MAIL_TYPE_DEFAULT, true);
	}

	role->GetRoleModuleManager()->GetMoney()->AddCoinBind(reward_coin_bind, __FUNCTION__);

	this->SendRoleInfo(role);

	//日志
	gamelog::g_log_qing_yuan.printf(LL_INFO, "SpecialLogicWeddingFB::Meal "
		"| role_id[%d] role_info_param:[%d]",
		role_info->role_uid, role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_MEAL]);
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF,
			sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_wedding_meal_notice,
			role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_MEAL], 
			base_cfg.meal__max_num);

		Protocol::SCSystemMsg info;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&info, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
		}
	}
}

void SpecialLogicWeddingFB::BuyBless(Role * role, int shop_item_index)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int oper_cfg_param = LOGIC_CONFIG->GetQingYuanConfig()
		->GetFBCommonOperParam(m_link_seq, WeddingFB::COMMON_OPER_TYPE_BLESS);

	if (1 != oper_cfg_param)
	{//无法领取
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	const WeddingFB::BlessShopItemCfg * shop_cfg 
		= LOGIC_CONFIG->GetQingYuanConfig()->GetFBBlessShopItemCfg(shop_item_index);
	if (NULL == shop_cfg)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//消耗
	if (!role->GetRoleModuleManager()->GetMoney()->UseMoney(shop_cfg->cost_type, shop_cfg->cost_num, __FUNCTION__, true))
	{
		role->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}

	//记录
	this->AddHeat(shop_cfg->add_heatt);

	//广播
	this->SendFBSync(NULL, Protocol::WEDDING_FB_SYNC_TYPE_BLESS_SHOP, shop_cfg->shop_item_index);

	if (true)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF,
			sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_wedding_fb_bless_notice,
			role->GetName(), shop_cfg->shop_item_index);

		Protocol::SCSystemMsg info;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&info, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE)))
		{
			this->GetScene()->SendToRole((const char *)&info, sizeof(info));
		}
	}

	gamelog::g_log_qing_yuan.printf(LL_INFO, "SpecialLogicWeddingFB::BuyBless "
		"| role_id[%d] shop+index:[%d] m_wedding_heat:[%d]",
		role_info->role_uid, shop_cfg->shop_item_index, m_wedding_heat);
}

void SpecialLogicWeddingFB::FetchHeatReward(Role * role, int reward_seq)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const WeddingFB::HeatRewardCfg * heat_reward_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GetFBHeatRewardCfg(reward_seq);
	if (NULL == heat_reward_cfg)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//可领
	if (heat_reward_cfg->need_hot > m_wedding_heat)
	{
		role->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}

	//已领
	if (IsSetBit(role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_HEAT_REWARD], reward_seq))
	{
		role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	//记录
	SetBit(role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_HEAT_REWARD], reward_seq);

	//发放
	if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(heat_reward_cfg->reward_vec))
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutVec(heat_reward_cfg->reward_vec, PUT_REASON_WEDDING);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(heat_reward_cfg->reward_vec, SNED_MAIL_TYPE_DEFAULT, true);
	}

	this->SendRoleInfo(role);

	gamelog::g_log_qing_yuan.printf(LL_INFO, "SpecialLogicWeddingFB::FetchHeatReward "
		"| role_id[%d] shop+index:[%d] fetch_flag:[%d]",
		role_info->role_uid, reward_seq, 
		role_info->commom_param[WeddingFB::COMMON_OPER_TYPE_HEAT_REWARD]);
}

void SpecialLogicWeddingFB::AddHeat(int add_num)
{
	if (0 >= add_num)
	{
		return;
	}

	m_wedding_heat += add_num;

	this->SendFBSync(NULL, Protocol::WEDDING_FB_SYNC_TYPE_HEAT, m_wedding_heat);
}

void SpecialLogicWeddingFB::FetchXubQingMiAi(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//配置
	const WeddingFB::SubActXubQingMiAi & reward_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GetSubActXubQingMiAi();

	//已领
	if (0 != role_info->xun_qing_mi_ai_fetch_flag)
	{
		role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}


	//记录
	role_info->xun_qing_mi_ai_fetch_flag = 1;
	this->SendXubQingMiAi(role);

	//奖励
	long long reward_coin_bind = 0;
	std::vector<ItemConfigData> temp_reward_vec;
	int group_id_size = (int)reward_cfg.item_list.size();
	for (int i = 0; i < group_id_size; i++)
	{
		const WeddingFB::GroupItemCfg * group_cfg
			= LOGIC_CONFIG->GetQingYuanConfig()->GetGroupRandCfg(reward_cfg.item_list[i]);
		if (NULL == group_cfg)
		{
			continue;
		}

		reward_coin_bind += group_cfg->coin_bind;
		if (0 != group_cfg->reward_item.item_id)
		{
			temp_reward_vec.push_back(group_cfg->reward_item);
		}
	}

	if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(temp_reward_vec))
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutVec(temp_reward_vec, PUT_REASON_WEDDING);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(temp_reward_vec, SNED_MAIL_TYPE_DEFAULT, true);
	}

	if (LOGIC_CONFIG->GetQingYuanConfig()->IsSubActFinish(WeddingFB::SUB_ACT_TYPE_XUN_QING_MI_AI, role_info->xun_qing_mi_ai_fetch_flag))
	{
		role->NoticeNum(errornum::EN_WEDDING_SUB_ACT_FINISH);
	}
	

	//日志
	gamelog::g_log_qing_yuan.printf(LL_INFO, "SpecialLogicWeddingFB::FetchXubQingMiAi "
		"| role_id[%d] fetch_flag:[%d]",
		role_info->role_uid,
		role_info->xun_qing_mi_ai_fetch_flag);
}

void SpecialLogicWeddingFB::SendXubQingMiAi(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		return;
	}

	Protocol::SCWeddingSubAct_XubQingMiAi info;

	info.poi_x = role_info->xun_qing_mi_ai_posi.x;
	info.poi_y = role_info->xun_qing_mi_ai_posi.y;
	

	info.fetch_flag = role_info->xun_qing_mi_ai_fetch_flag;

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void SpecialLogicWeddingFB::FetchXinXinXiangYin(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//配置
	const WeddingFB::SubActXinXinXiangYin & reward_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GetSubActXinXinXiangYin();

	//已领
	if (0 != role_info->xin_xin_xiang_yin_fetch_flag)
	{
		role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}


	//记录
	role_info->xin_xin_xiang_yin_fetch_flag = 1;
	this->SendXubQingMiAi(role);

	//奖励
	long long reward_coin_bind = 0;
	std::vector<ItemConfigData> temp_reward_vec;
	int group_id_size = (int)reward_cfg.item_list.size();
	for (int i = 0; i < group_id_size; i++)
	{
		const WeddingFB::GroupItemCfg * group_cfg
			= LOGIC_CONFIG->GetQingYuanConfig()->GetGroupRandCfg(reward_cfg.item_list[i]);
		if (NULL == group_cfg)
		{
			continue;
		}

		reward_coin_bind += group_cfg->coin_bind;
		if (0 != group_cfg->reward_item.item_id)
		{
			temp_reward_vec.push_back(group_cfg->reward_item);
		}
	}

	if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(temp_reward_vec))
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutVec(temp_reward_vec, PUT_REASON_WEDDING);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(temp_reward_vec, SNED_MAIL_TYPE_DEFAULT, true);
	}

	//奖励通知
	role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemListVec(temp_reward_vec, GET_REASON_WEDDING_SUB_ACT_XIN_XIN_XIANG_YIN, reward_coin_bind);

	if (LOGIC_CONFIG->GetQingYuanConfig()->IsSubActFinish(WeddingFB::SUB_ACT_TYPE_XIN_XIN_XIANG_YIN, role_info->xin_xin_xiang_yin_fetch_flag))
	{
		role->NoticeNum(errornum::EN_WEDDING_SUB_ACT_FINISH);
	}

	//日志
	gamelog::g_log_qing_yuan.printf(LL_INFO, "SpecialLogicWeddingFB::FetchXinXinXiangYin "
		"| role_id[%d] fetch_flag:[%d]",
		role_info->role_uid,
		role_info->xin_xin_xiang_yin_fetch_flag);
}

void SpecialLogicWeddingFB::SendXinXinXiangYin(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		return;
	}

	Protocol::SCWeddingSubAct_XinXinXiangYin info;
	info.fetch_flag = role_info->xin_xin_xiang_yin_fetch_flag;

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void SpecialLogicWeddingFB::FetchBaoWeiXiuQiu(Role * role, int npc_id)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	//配置
	const WeddingFB::SubActBaoWeiXiuQiu * reward_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GetSubActBaoWeiXiuQiu(npc_id);
	if (NULL == reward_cfg)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	int save_index = reward_cfg->reward_index;
	
	//已领
	if (IsSetBit(role_info->bao_wei_xiu_qiu_fetch_flag, save_index))
	{
		role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	//记录
	SetBit(role_info->bao_wei_xiu_qiu_fetch_flag, save_index);
	this->SendBaoWeiXiuQiu(role);

	//奖励
	if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(reward_cfg->item_list))
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutVec(reward_cfg->item_list, PUT_REASON_WEDDING);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(reward_cfg->item_list, SNED_MAIL_TYPE_DEFAULT, true);
	}

	if (LOGIC_CONFIG->GetQingYuanConfig()->IsSubActFinish(WeddingFB::SUB_ACT_TYPE_BAO_WEI_XIU_QIU, role_info->bao_wei_xiu_qiu_fetch_flag))
	{
		role->NoticeNum(errornum::EN_WEDDING_SUB_ACT_FINISH);
	}

	//日志
	gamelog::g_log_qing_yuan.printf(LL_INFO, "SpecialLogicWeddingFB::FetchBaoWeiXiuQiu "
		"| role_id[%d] save_index:[%d] fetch_flag:[%d]",
		role_info->role_uid, save_index,
		role_info->bao_wei_xiu_qiu_fetch_flag);

}

void SpecialLogicWeddingFB::SendBaoWeiXiuQiu(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		return;
	}

	const std::vector<int>& npc_id_vec
		= LOGIC_CONFIG->GetQingYuanConfig()->GetSubActBaoWeiXiuQiuNpcIDVec();

	//客户端要求转成(npc_id, 状态)下发
	Protocol::SCWeddingSubAct_BaoWeiXiuQing info;
	info.count = (int)npc_id_vec.size();
	for (int i = 0; i < info.count; i++)
	{
		info.info_list[i].npc_id = npc_id_vec[i];
		info.info_list[i].is_fetch 
			= IsSetBit(role_info->bao_wei_xiu_qiu_fetch_flag, i) ? 1 : 0;
	}

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void SpecialLogicWeddingFB::FetchZaoShengGuiZi(Role * role, int index)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	//配置
	const WeddingFB::SubActZaoShengGuiZi * reward_cfg
		= LOGIC_CONFIG->GetQingYuanConfig()->GetSubActZaoShengGuiZi(index);
	if (NULL == reward_cfg)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//已领
	if (IsSetBit(role_info->zao_sheng_gui_zi_fetch_flag, index))
	{
		role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	//记录
	SetBit(role_info->zao_sheng_gui_zi_fetch_flag, index);
	this->SendZaoShengGuiZi(role);

	//奖励
	if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(reward_cfg->item_list))
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutVec(reward_cfg->item_list, PUT_REASON_WEDDING);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(reward_cfg->item_list, SNED_MAIL_TYPE_DEFAULT, true);
	}
	role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemListVec(reward_cfg->item_list, GET_REASON_WEDDING_SUB_ACT_XIN_XIN_XIANG_YIN, 0);

	if (LOGIC_CONFIG->GetQingYuanConfig()->IsSubActFinish(WeddingFB::SUB_ACT_TYPE_ZAO_SHENG_GUI_ZI, role_info->zao_sheng_gui_zi_fetch_flag))
	{
		role->NoticeNum(errornum::EN_WEDDING_SUB_ACT_FINISH);
	}

	//日志
	gamelog::g_log_qing_yuan.printf(LL_INFO, "SpecialLogicWeddingFB::FetchZaoShengGuiZi "
		"| role_id[%d] save_index:[%d] fetch_flag:[%d]",
		role_info->role_uid, index,
		role_info->zao_sheng_gui_zi_fetch_flag);
}

void SpecialLogicWeddingFB::SendZaoShengGuiZi(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	WeddingFB::RoleInfo *role_info = this->GetRoleInfo(role->GetUID());
	if (NULL == role_info)
	{
		return;
	}

	Protocol::SCWeddingSubAct_ZaoShengGuiZi info;
	info.fetch_flag = role_info->zao_sheng_gui_zi_fetch_flag;

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

WeddingFB::RoleInfo * SpecialLogicWeddingFB::GetRoleInfo(int uid)
{
	std::map<int, WeddingFB::RoleInfo>::iterator role_info_it
		= m_join_role_info_map.find(uid);
	if (role_info_it == m_join_role_info_map.end())
	{
		return NULL;
	}

	return &role_info_it->second;
}
