#include "mysterioustrial.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/uniqueidgenerator.hpp"

#include "obj/character/role.h"

#include "gameworld/protocol/msgother.h"
#include "gameworld/gamelog.h"

#include "other/funopen/funopen.hpp"
#include "other/workshop/gemstoneconfig.hpp"
#include "other/pet/pet.hpp"
#include "other/capability/capability.hpp"
#include "other/partner/partner.hpp"
#include "other/route/mailroute.hpp"
#include "other/couragechallenge/couragechallenge.h"

#include "item/knapsack.h"
#include "equipment/equipment.h"
#include "equipment/equipmentmanager.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/activityconfig/huanjiezhanchang/activityhuanjiezhanchangconfig.hpp"

#include "other/pet/petconfig.hpp"
#include "equipment/jewelry.h"
#include "other/miyi/rolemiyi.hpp"
#include "other/miyi/miyiconfig.hpp"
#include "other/fabao/fabao.hpp"
#include "other/train/train.hpp"
#include "other/prestige/prestige.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "other/collection/collection.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/petmonopoly/petmonopoly.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "other/smartmountssystem/smartmountssystem.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"

#include "global/colosseum/colosseummanager.hpp"
#include "config/activityconfig/activitybigdipper/activitybigdipperconfig.hpp"

MysteriousTrial::MysteriousTrial()
{
}

MysteriousTrial::~MysteriousTrial()
{
}

void MysteriousTrial::Init(RoleModuleManager * module_mgr, const MysteriousTrialParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;

	if (m_param.change_reset_flag == 0)
	{
		m_param.Reset();
		m_param.change_reset_flag = 1;
	}

	if (m_param.transfer == 0)
	{
		for (int i = 0; i < RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
		{
			for (int j = 0; j < RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM && j < ARRAY_ITEM_COUNT(m_param.mysterious_trial_info[i]); j++)
			{
				m_param.new_mysterious_trial_info[i][j] = m_param.mysterious_trial_info[i][j];
			}
		}
		m_param.transfer = 1;
	}
}

void MysteriousTrial::GetInitParam(MysteriousTrialParam * param)
{
	*param = m_param;
}

void MysteriousTrial::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{

}

void MysteriousTrial::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_MYSTERIOUS_TRIAL) || this->IsCheckTypeInvaild()) return;

	//该阶段配置存在
	const std::map<int, MysteriousTrialConfig>* cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialCfg(m_param.type);
	if (cfg == NULL)
	{
		return;
	}

	if (m_param.open_day + 1 > RA_MYSTERIOUS_TRIAL_OPEN_DAY_MAX_NUM)
	{
		m_param.type += 1;
		m_param.ActivityExpiredReset();
		this->OnOpenTrial();
		this->SendInfo();
		return;
	}

	//防止跟登陆解锁冲突,导致开启天数多出一天
	if (m_param.active_open_day_id == EngineAdapter::Instance().DayID())
	{
		return;
	}

	m_param.open_day += 1;
	cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialCfg(m_param.type);
	if (cfg == NULL) return;

	for (std::map<int, MysteriousTrialConfig>::const_iterator it = cfg->begin(); it != cfg->end(); ++it)
	{
		int day_index = it->first - 1;	//配置已经检测,不需要再进行判断
		for (int i = 0; i < RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM; i++)
		{
			if (2 == it->second.node_cfg[i].parameter1 && 0 == m_param.new_mysterious_trial_info[day_index][i].flag)
			{
				m_param.new_mysterious_trial_info[day_index][i].schedule = 0;
				if (day_index < m_param.open_day)
				{
					this->SendSingleInfo(day_index, i);
				}
			}
		}
	}

	this->OnInfoChange(MSC_CON_LOGIN_DAY, 1);
	this->SendInfo();
}

void MysteriousTrial::OnRoleLogin()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_MYSTERIOUS_TRIAL) || this->IsCheckTypeInvaild()) return;

	this->OnOpenTrial();
	this->SendInfo();
}

void MysteriousTrial::OnOpenTrial()
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_MYSTERIOUS_TRIAL) || this->IsCheckTypeInvaild()) return;

	//该阶段配置存在
	const std::map<int, MysteriousTrialConfig>* cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialCfg(m_param.type);
	if (cfg == NULL)
	{
		return;
	}

	if (m_param.open_day == 0)
	{
		m_param.open_day++;
		m_param.active_open_day_id = EngineAdapter::Instance().DayID();
		//新加条件时需要考虑一下是否要在这触发
		UNSTD_STATIC_CHECK(MSC_CON_MAX == 62);
		this->OnInfoChange(MSC_CON_LOGIN_DAY, 1);
		this->OnInfoChange(MSC_CON_ROLE_LEVEL, m_module_mgr->GetRole()->GetLevel());
		this->OnInfoChange(MSC_CON_PROF_PRO, m_module_mgr->GetRole()->GetAdvanceTimes());
		this->OnInfoChange(MSC_CON_PET_INTEN, 1);
		this->OnInfoChange(MSC_CON_COUR_CHAG, 1);
		this->OnInfoChange(MSC_CON_PARTNER_NUM, 1);
		this->OnInfoChange(MSC_CON_PET_CAPABIL, 1);
		this->OnInfoChange(MSC_CON_GEMS_MOSAIC, 1);
		this->OnInfoChange(MSC_CON_PARTNER_PRO, 1);
		this->OnInfoChange(MSC_CON_JOIN_FAMILY, 1);
		this->OnInfoChange(MSC_CON_USER_CAPABIL, 1);
		this->OnInfoChange(MSC_CON_COURAGE_CHALLENGE, 1);
		this->OnInfoChange(MSC_CON_CATCH_PET, 1);
		this->OnInfoChange(MSC_CON_COLOSSEUM, 1);
		this->OnInfoChange(MSC_CON_ROLE_SKILL_LEVEL, 1);
		this->OnInfoChange(MSC_CON_GET_MEDAL, 1);
		this->OnInfoChange(MSC_CON_PARTNER_FETTERS, 1);
		this->OnInfoChange(MSC_CON_MI_YI, 1);
		this->OnInfoChange(MSC_CON_SPECIAL_PET_INTEN, 1, 0, -1);
		this->OnInfoChange(MSC_CON_PRESTIG_LEVEL, 1);
		this->OnInfoChange(MSC_CON_PET_SKILL_NUM, 1);
		this->OnInfoChange(MSC_CON_GET_FA_BAO, 1);
		this->OnInfoChange(MSC_CON_UNLOCK_SMART_MOUNTS, 1);
		this->OnInfoChange(MSC_CON_ACT_COLLECT_QUAILTY, 1, -1);
		this->OnInfoChange(MSC_CON_DREAM_NOTES_CHALLENGE, 1);
		this->OnInfoChange(MSC_CON_LUN_HUI_WANG_CHUAN, 1);
		this->OnInfoChange(MSC_CON_CLOUD_ARENA, 1);
		this->OnInfoChange(MSC_CON_BRAVE_GROUD_PASS, 1);
		this->OnInfoChange(MSC_CON_HUAN_JIE_ZHAN_CHANG, 1);
		this->OnInfoChange(MSC_CON_PARTNER_EQUIPMENT, 1);
		this->OnInfoChange(MSC_CON_PARTNER_QUALITY, 1);
		this->OnInfoChange(MSC_CON_EQUIP_FULL_SET_STR, 1);
		this->OnInfoChange(MSC_CON_PET_MONOPOLY, 1);
		this->OnInfoChange(NSC_CON_REALM_BREAK_FIGHT, 1);
		this->OnInfoChange(MSC_CON_PASS_MI_YI_BOSS, 1);
		this->SendInfo();
	}
}


void MysteriousTrial::OnGetMysteriousTrialReward(int day, int seq)
{
	if (this->IsCheckTypeInvaild()) return;

	int day_index = day - 1;
	const MysteriousTrialCfg* cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialCfg(m_param.type, day, seq);
	if (cfg == NULL)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (m_param.new_mysterious_trial_info[day_index][seq].flag != 1)
	{
		m_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}
	const MysteriousTrialRewardCfg * reward_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialRewardCfg(cfg->reward_id, (unsigned int)m_module_mgr->GetRole()->GetRoleCreateTime());
	if (NULL == reward_cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!reward_cfg->reward_list.empty())
	{
		if (!m_module_mgr->GetKnapsack()->CheckForPutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0]))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		}
		if (!m_module_mgr->GetKnapsack()->PutList((short)reward_cfg->reward_list.size(), &reward_cfg->reward_list[0], PUT_REASON_RA_MYSTERIOUS_TRIAL))
		{
			m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	m_param.new_mysterious_trial_info[day_index][seq].flag = 2;
	m_param.new_mysterious_trial_info[day_index][seq].schedule = 0;
	m_param.task++;
	this->SendFinallyInfo();
	this->SendSingleInfo(day_index, seq);
}

void MysteriousTrial::OnGetMysteriousTrialFinallyReward(int seq)
{
	if (this->IsCheckTypeInvaild()) return;

	const MysteriousTrialFinallyCfg *cfg_reward = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialFinallyReward(m_param.type, seq);
	if (cfg_reward == NULL)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (m_param.activity_flag.IsBitSet(seq) || m_param.task < cfg_reward->request_score)
	{
		m_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}
	if (!m_module_mgr->GetKnapsack()->CheckForPut(cfg_reward->item.item_id, cfg_reward->item.num))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (!m_module_mgr->GetKnapsack()->Put(cfg_reward->item, PUT_REASON_RA_MYSTERIOUS_TRIAL))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	m_param.activity_flag.SetBit(seq);
	this->SendFinallyInfo();
}

bool MysteriousTrial::OnBuyEveryDayDiscountReward(int day, int chong_zhi_gold, int type)
{
	if (this->IsCheckTypeInvaild()) return false;

	if (type == m_param.type && m_param.activity_flag.IsBitSet(RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM + day))
	{
		//m_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
		return false;
	}

	const MysteriousTraialBuyCfg* cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialBuyCfg(type, day);
	if (NULL == cfg)
	{
		return false;
	}
	if (chong_zhi_gold != cfg->price)
	{
		return false;
	}
	bool is_true = true;
	if (m_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		if (m_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_RA_MYSTERIOUS_TRIAL))
		{
			is_true = false;
		}
	}
	if (is_true)
	{
		static MailContentParam contentparam; contentparam.Reset();
		ItemDataWrapper item_info;
		item_info.item_id = cfg->item.item_id;
		item_info.is_bind = cfg->item.is_bind;
		item_info.num = cfg->item.num;
		contentparam.item_list[0] = item_info;

		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
		MailRoute::MailToUser(m_module_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);

		//m_module_mgr->NoticeNum(noticenum::NT_KNAPSACKFULL_SEND_MAIL);

	}
	if (type == m_param.type)
	{
		m_param.activity_flag.SetBit(RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM + day);
		gamelog::g_log_rand_activity.printf(LL_INFO, "mail, role[%d,%s] item_id[%d] num[%d] day[%d] type[%d] reason[%s]", m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
			cfg->item.item_id, cfg->item.num, day, type, __FUNCTION__);
	}
	this->SendFinallyInfo();
	return true;
}

void MysteriousTrial::OnBuyItemCfgCheck(int type, int day)
{
	if (m_param.type != type || this->IsCheckTypeInvaild()) return;

	if (m_param.activity_flag.IsBitSet(RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM + day))
	{
		return;
	}

	const MysteriousTraialBuyCfg* cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialBuyCfg(type, day);
	if (NULL == cfg)
	{
		return;
	}
	if (cfg->price <= 0)
	{
		return;
	}
	RoleCommonSave * rcs = m_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_MYSTERIOUS_TRIAL);
	if (rcs == NULL)
	{
		return;
	}
	RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0);
	if (buy_item == NULL)
	{
		return;
	}
	time_t now = EngineAdapter::Instance().Time();
	if (now < (time_t)buy_item->create_time + CMD_BUY_ITEM_WAIT_TIME)
	{
		m_module_mgr->NoticeNum(errornum::EN_HAS_ORDER);

		return;
	}
	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return;
	}
	if (buy_item->create_time != 0)
	{
		std::string log_str1 = ::GetString(*buy_item);
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), log_str1.c_str());
	}

	buy_item->cfg_vesion = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg().mysterious_cfg_ver;
	buy_item->need_gold = cfg->price;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = 0;
	buy_item->unique_id = unique_key;
	buy_item->param1 = type;
	buy_item->param2 = day;
	buy_item->rewards_item[0].is_bind = cfg->item.is_bind;
	buy_item->rewards_item[0].item_id = cfg->item.item_id;
	buy_item->rewards_item[0].num = cfg->item.num;
	buy_item->rewards_num = 1;
	rcs->SetDataChange(0);
	m_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TIME_TYPE_MYSTERIOUS_TRIAL_NEW;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = unique_key;

	m_module_mgr->NetSend(&info, sizeof(info));
	return;
}

int MysteriousTrial::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSave * rcs = m_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_MYSTERIOUS_TRIAL);
	if (rcs == NULL)
	{
		return -__LINE__;
	}

	const RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0, false);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s line:%d | role[%d %s] chongzhi_gold[%d] unique_id[%lld] %s", __FUNCTION__, __LINE__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(),
		chong_zhi_gold, unique_id, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}
	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return  -__LINE__;
	}

	if (m_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_RA_MYSTERIOUS_TRIAL);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	if (buy_item->param1 == m_param.type)
	{
		m_param.activity_flag.SetBit(RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM + buy_item->param2);
		this->SendFinallyInfo();

		gamelog::g_log_rand_activity.printf(LL_INFO, "mail, role[%d,%s] item_id[%d] num[%d] day[%d] type[%d] reason[%s]", m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
			rewards[0].item_id, rewards[0].num, buy_item->param2, buy_item->param1, __FUNCTION__);
	}
	rcs->RemoveData(0);
	m_module_mgr->GetRole()->Save();
	return 0;
}

int MysteriousTrial::OnBuyGiftItem(int day, ARG_OUT int * price)
{
	if (this->IsCheckTypeInvaild()) return 1;

	if (m_param.activity_flag.IsBitSet(RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM + day))
	{
		return 2;
	}
	const MysteriousTraialBuyCfg* cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialBuyCfg(m_param.type, day);
	if (NULL == cfg)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = cfg->price;
		return 0;
	}
	bool is_true = true;
	if (m_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		if (m_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_RA_MYSTERIOUS_TRIAL))
		{
			is_true = false;
		}
	}
	if (is_true)
	{
		static MailContentParam contentparam; contentparam.Reset();
		ItemDataWrapper item_info;
		item_info.item_id = cfg->item.item_id;
		item_info.is_bind = cfg->item.is_bind;
		item_info.num = cfg->item.num;
		contentparam.item_list[0] = item_info;

		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
		MailRoute::MailToUser(m_module_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);

		//m_module_mgr->NoticeNum(noticenum::NT_KNAPSACKFULL_SEND_MAIL);

		gamelog::g_log_rand_activity.printf(LL_INFO, "mail, role[%d,%s] item_id[%d] num[%d] day[%d] reason[%s]", m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
			item_info.item_id, item_info.num, day, __FUNCTION__);
	}

	m_param.activity_flag.SetBit(RA_MYSTERIOUS_TRAIL_CMD_BUY_START_NUM + day);
	this->SendFinallyInfo();

	return 0;
}

void MysteriousTrial::SendInfo()
{
	int day = m_param.open_day;

	Protocol::SCMysteriousTrialInfo info;
	info.task_num = m_param.task;
	info.open_day = m_param.open_day;

	if (info.open_day > RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM)
		day = RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM;
	info.goal_num = day;
	info.activity_flag = m_param.activity_flag;
	info.type = m_param.type;
	info.reserve_sh = 0;

	memcpy(info.ra_mysterious_trial, m_param.new_mysterious_trial_info, sizeof(info.ra_mysterious_trial));
	int length = sizeof(info.ra_mysterious_trial) - (sizeof(info.ra_mysterious_trial) / RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM) * info.goal_num;
	m_module_mgr->NetSend(&info, sizeof(info) - length);
}

void MysteriousTrial::GmAddMysteriousTrial(int day, int seq, int add_num)
{
	const MysteriousTrialCfg * cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialCfg(m_param.type, day, seq);
	if (NULL == cfg) return;

	if (add_num > 0 && day > 0 && day <= RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM && seq >= 0 && seq < RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM &&
		m_param.new_mysterious_trial_info[day - 1][seq].schedule < cfg->parameter2 && m_param.new_mysterious_trial_info[day - 1][seq].flag == 0)
	{
		m_param.new_mysterious_trial_info[day - 1][seq].schedule += add_num;
	}

	if (m_param.new_mysterious_trial_info[day - 1][seq].flag == 0 && m_param.new_mysterious_trial_info[day - 1][seq].schedule >= cfg->parameter2)
	{
		m_param.new_mysterious_trial_info[day - 1][seq].flag = 1;
	}

	if (day - 1 < m_param.open_day)
	{
		this->SendSingleInfo(day - 1, seq);
	}
}

void MysteriousTrial::GmSetDayId()
{
	m_param.active_open_day_id = 0;
}

void MysteriousTrial::SendSingleInfo(int day_index, int seq)
{
	Protocol::SCMysteriousTrialSingleInfo info;
	info.start_time = day_index + 1;
	info.seq = seq;
	info.state = m_param.new_mysterious_trial_info[day_index][seq].flag;
	info.progress = m_param.new_mysterious_trial_info[day_index][seq].schedule;
	info.target_num = m_param.task;
	info.reserve_sh = m_param.new_mysterious_trial_info[day_index][seq].reserve_sh;
	m_module_mgr->NetSend(&info, sizeof(info));
}

void MysteriousTrial::SendFinallyInfo()
{
	Protocol::SCMysterialFinallyRewardInfo info;
	info.activity_flag = m_param.activity_flag;
	m_module_mgr->NetSend(&info, sizeof(info));
}

void MysteriousTrial::OnInfoChange(int type, int num, int param, int param2)
{
	if (this->IsCheckTypeInvaild()) return;

	if (type < MSC_CON_LOGIN_DAY || type >= MSC_CON_MAX) return;

	//不够等级依然记录数据变化
	//if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_MYSTERIOUS_TRIAL)) return;

	int add_num = num;
	const std::map<int, MysteriousTrialConfig>* cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMysteriousTrialCfg(m_param.type);
	if (cfg == NULL) return;

	bool is_gems = false;
	for (std::map<int, MysteriousTrialConfig>::const_iterator it = cfg->begin(); it != cfg->end(); ++it)
	{
		int day_index = it->first - 1;	//配置已经检测,不需要再进行判断
		for (int i = 0; i < RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM; i++)
		{
			if (type == it->second.node_cfg[i].activity_type && 0 == m_param.new_mysterious_trial_info[day_index][i].flag)
			{
				UNSTD_STATIC_CHECK(MSC_CON_MAX == 62);
				switch (type)
				{
				case MSC_CON_LOGIN_DAY:
				case MSC_CON_ROLE_LEVEL:
				case MSC_CON_PROF_PRO:
				case MSC_CON_KILL_BOSS:
				case MSC_CON_COMP_TREA:
				case MSC_CON_COMPLETE_FB:
				case MSC_CON_ONLINE_TIME:
				case MSC_CON_TEMP_COMP_FB:
				case MSC_CON_TRIAL_BOSS:
				case MSC_CON_TRA_MAR_PUR:
				case MSC_CON_KILL_YW_BOSS:
				case MSC_CON_ACT_COLLECT:
				case MSC_CON_SHOP_CONSUME:
				case MSC_CON_JOIN_XIYOUFB:
				case MSC_CON_FAST_PASS_DREAM_NOTES:
				case MSC_CON_PAR_LIMIT_ACTIVITY:
				case MSC_CON_PAR_CLOUD_ARENA:
					{
						if (add_num < 0) return;
					}
					break;
				case MSC_CON_PET_USE_BOOK:
					{
						if(add_num < 0) return;

						if (it->second.node_cfg[i].parameter3 != 0 && param < it->second.node_cfg[i].parameter3)
						{
							continue;
						}
						if (it->second.node_cfg[i].parameter4 != 0 && param2 < it->second.node_cfg[i].parameter4)
						{
							continue;
						}
					}	
					break;
				case MSC_CON_SPECIAL_PET_INTEN:
					{
						if (-1 != param2)
						{
							if (param < 0 || add_num < 0)return;

							if (it->second.node_cfg[i].parameter2 != add_num || it->second.node_cfg[i].parameter3 > param)
							{
								continue;
							}
						}
						add_num = m_module_mgr->GetPet()->GetPetNumByQualityAndStr(it->second.node_cfg[i].parameter2, it->second.node_cfg[i].parameter3) >= 1 ? it->second.node_cfg[i].parameter2 : 0;

					}
					break;
				case MSC_CON_ROLE_SKILL_LEVEL:
					{
						int max_role_skill_level = m_module_mgr->GetRoleSkill()->GetMaxSkillLevel();
						int max_train_skill_Level = m_module_mgr->GetTrain()->GetMaxTrainLevel();
						add_num = max_role_skill_level >= max_train_skill_Level ? max_role_skill_level : max_train_skill_Level;
					}
					break;
				case MSC_CON_PET_SKILL_NUM:
					{
						add_num = m_module_mgr->GetPet()->GetHasMaxPassiveSkillNum();
					}
					break;
				case MSC_CON_PRESTIG_LEVEL:
					{
						add_num = m_module_mgr->GetPrestige()->GetMaxVillageLevel();
					}
					break;
				case MSC_CON_MI_YI:
					{
						add_num = m_module_mgr->GetMiYi()->CheckLayerFinish(it->second.node_cfg[i].parameter3) ? 1 : 0;
					}
					break;
				case MSC_CON_PARTNER_FETTERS:
					{
						add_num = m_module_mgr->GetPartner()->GetActiveJiBanNum();
					}
					break;
				case MSC_CON_GET_MEDAL:
					{
						add_num = m_module_mgr->GetMedal()->GetCurWearNumByColor(it->second.node_cfg[i].parameter3);
					}
					break;
				case MSC_CON_CATCH_PET:
					{
						add_num = m_module_mgr->GetPet()->GetPetNumByQualityAndStr(it->second.node_cfg[i].parameter2) >= 1 ? it->second.node_cfg[i].parameter2 : 0;
					}
					break;
				case MSC_CON_COLOSSEUM:
					{
						if (ColosseumManager::Instance().GetRoleRank(m_module_mgr->GetUid()) > it->second.node_cfg[i].parameter2)return;
						add_num = it->second.node_cfg[i].parameter2;
					}
					break;
				case MSC_CON_EQPM_FORG:
					{
						if (param == 0) return;
						const ItemBase * item = ITEMPOOL->GetItem(param);
						if (NULL == item || (item->GetItemType() != I_TYPE_EQUIPMENT && item->GetItemType() != I_TYPE_JEWELRY)) return;
						if (item->GetItemType() == I_TYPE_EQUIPMENT)
						{
							Equipment* item_eq = (Equipment *)item;
							if (it->second.node_cfg[i].parameter3 != 0 && item_eq->GetEquipColor() < it->second.node_cfg[i].parameter3)
							{
								continue;
							}
							if (it->second.node_cfg[i].parameter4 != 0 && item_eq->GetLimitLevel() < it->second.node_cfg[i].parameter4)
							{
								continue;
							}
						}
						else if (item->GetItemType() == I_TYPE_JEWELRY)
						{
							Jewelry* item_je = (Jewelry *)item;
							if (it->second.node_cfg[i].parameter3 != 0 && item_je->GetEquipColor() < it->second.node_cfg[i].parameter3)
							{
								continue;
							}
							if (it->second.node_cfg[i].parameter4 != 0 && item_je->GetLimitLevel() < it->second.node_cfg[i].parameter4)
							{
								continue;
							}
						}
					}
					break;
				case MSC_CON_PET_INTEN:
					{
						add_num = m_module_mgr->GetPet()->GetPetIntensifyMax(it->second.node_cfg[i].parameter3);
					}
					break;
				case MSC_CON_COUR_CHAG:
					{
						add_num = m_module_mgr->GetCourageChallenge()->GetLayerScoreNum(it->second.node_cfg[i].parameter3);
					}
					break;
				case MSC_CON_PARTNER_NUM:
					{
						add_num = m_module_mgr->GetPartner()->GetPartnerCurCount();
					}
					break;
				case MSC_CON_PET_CAPABIL:
					{
						add_num = m_module_mgr->GetPet()->GetPetCapabilityMax();
					}
					break;
				case MSC_CON_GEMS_MOSAIC:
					{
						add_num = m_module_mgr->GetEquipmentManager()->GetEquipGemNumByGrade(it->second.node_cfg[i].parameter3);
					}
					break;
				case MSC_CON_PARTNER_PRO:
					{
						add_num = m_module_mgr->GetPartner()->GetMaxPartnerNumByGrade(it->second.node_cfg[i].parameter3);
					}
					break;
				case MSC_CON_JOIN_FAMILY:
					{
						if (add_num < 0) return;
						if (NULL == m_module_mgr->GetRole()->GetGuild())
						{
							add_num = 0;
						}
					}
					break;
				case MSC_CON_USER_CAPABIL:
					{
						add_num = m_module_mgr->GetCapability()->GetRoleCapability();
						if (add_num <= 0) return;
					}
					break;
				case MSC_CON_COURAGE_CHALLENGE:
					{
						add_num = m_module_mgr->GetCourageChallenge()->GetReachStarScoreCount();
					}
					break;
				case MSC_CON_JOIN_XIYOUFB_LEVEL:
					{
						add_num = param;
					}
					break;
				case MSC_CON_GET_FA_BAO:
					{
						add_num = m_module_mgr->GetFaBao()->GetFaBaoNumByColorAndLevel(it->second.node_cfg[i].parameter4, it->second.node_cfg[i].parameter3);
					}
					break;
				case MSC_CON_UNLOCK_SMART_MOUNTS:
					{
						add_num = m_module_mgr->GetSmartMountsSystem()->GetUnlockOrMaxJewelryLevel((0 == it->second.node_cfg[i].parameter3) ? true : false);
					}
					break;
				case MSC_CON_ACT_COLLECT_QUAILTY:
					{
						if (-1 != param && 0 != it->second.node_cfg[i].parameter3 && it->second.node_cfg[i].parameter3 != param)
						{
							continue;
						}
						add_num = m_module_mgr->GetCollection()->GetActiveNumByColor(it->second.node_cfg[i].parameter3);
					}
					break;
				case MSC_CON_DREAM_NOTES_CHALLENGE:
					{
						add_num = m_module_mgr->GetDreamNotesChallenge()->GetPassLevel();
					}
					break;
				case MSC_CON_LUN_HUI_WANG_CHUAN:
					{
						add_num = (m_module_mgr->GetLunHuiWangChuan()->GetPassIDX() <= 0) ? 0 : m_module_mgr->GetLunHuiWangChuan()->GetPassIDX() - 1;
					}
					break;
				case MSC_CON_CLOUD_ARENA:
					{
						add_num = m_module_mgr->GetCommonData().cloud_arena_season_highest_dan_id;
						if (add_num < 0) return;
					}
					break;
				case MSC_CON_KILL_SHAN_HAI_BOSS:
					{
						if (add_num <= 0) return;

						if (it->second.node_cfg[i].parameter3 != 0 && param != it->second.node_cfg[i].parameter3)
						{
							continue;
						}
					}
					break;
				case MSC_CON_BRAVE_GROUD_PASS:
					{
						add_num = m_module_mgr->GetCommonData().brave_ground_max_reached_level;
						if (add_num < 0) return;
					}
					break;
				case MSC_CON_HUAN_JIE_ZHAN_CHANG:
					{
						int rank_score = m_module_mgr->GetHuaShenManager()->GetRankScore();
						if (rank_score <= 0) return;

						const ActivityHuanJieZhanChangRankCfg * rank_cfg = LOGIC_CONFIG->GetActivityHuanJieZhanChangConfig()->GetActivityHuanJieZhanChangRankCfg(rank_score);
						if (NULL == rank_cfg) return;

						add_num = rank_cfg->rank;
						if (add_num < 0) return;
					}
					break;
				case MSC_CON_PARTNER_EQUIPMENT:
					{
						add_num = m_module_mgr->GetPartner()->GetWearSetEquipNum();
					}
					break;
				case MSC_CON_PARTNER_QUALITY:
					{
						add_num = m_module_mgr->GetPartner()->GetPartnerNumByQualityNum(it->second.node_cfg[i].parameter3);
					}
					break;
				case MSC_CON_NTHER_WORLD_PURGATORY:
					{
						if (add_num <= 0) return;

						if (it->second.node_cfg[i].parameter3 != -1 && param != it->second.node_cfg[i].parameter3)
						{
							continue;
						}
					}
					break;
				case MSC_CON_NTHER_WORLD_PURGATORY_ALL:
					{
						if (add_num <= 0) return;
						if (it->second.node_cfg[i].parameter3 == 1)
						{
							const BigDipperCfg * big_dipper_cfg = LOGIC_CONFIG->GetActivityBigDipperConfig()->GetBaseCfg(param);
							if (NULL == big_dipper_cfg)continue;
							if (big_dipper_cfg->type != BigDipperCfg::MONSTER_TYPE_0)continue;
						}
						else if (it->second.node_cfg[i].parameter3 == 2)
						{
							const BigDipperCfg * big_dipper_cfg = LOGIC_CONFIG->GetActivityBigDipperConfig()->GetBaseCfg(param);
							if (NULL == big_dipper_cfg)continue;
							if (big_dipper_cfg->type != BigDipperCfg::MONSTER_TYPE_1)continue;
						}
					}
					break;
				case MSC_CON_SPECIAL_PET_ABANDON:
					{
						if (0 >= add_num || 0 >= param)return;

						if (it->second.node_cfg[i].parameter3 != 0 && it->second.node_cfg[i].parameter3 != param)
						{
							continue;
						}
					}
					break;
				case MSC_CON_PET_REFINED:
				case MSC_CON_PARTNER_BREAK:
				case MSC_CON_PET_INTEN_NUMBER:
				case MSC_CON_MEDAL_UP_GRADE:
				case MSC_CON_PET_LIAN_YAO:
					{
						if(0 >= add_num) return;

						if (it->second.node_cfg[i].parameter3 != 0 && param < it->second.node_cfg[i].parameter3)
						{
							continue;
						}
					}
					break;
				case MSC_CON_EQUIP_FULL_SET_STR:
					{
						add_num = m_module_mgr->GetEquipmentManager()->GetMinEquipStr();
					}
					break;
				case MSC_CON_PET_MONOPOLY:
					{
						add_num = m_module_mgr->GetPetMonopoly()->GetCurLevel();
					}
					break;
				case NSC_CON_REALM_BREAK_FIGHT:
					{
						add_num = m_module_mgr->GetRealmBreak()->GetCurFightSeq();
					}
					break;
				case MSC_CON_PASS_MI_YI_BOSS:
					{
						add_num = m_module_mgr->GetMiYi()->IsFirstKill(it->second.node_cfg[i].parameter3, it->second.node_cfg[i].parameter4 - 1) ? 0 : 1;
					}
					break;
				}

				if (it->second.node_cfg[i].parameter1 == 1 || (it->second.node_cfg[i].parameter1 == 2 && m_param.open_day > day_index))
				{
					if (is_gems)
					{
						if (m_param.new_mysterious_trial_info[day_index][i].schedule > 0)
						{
							m_param.new_mysterious_trial_info[day_index][i].schedule -= add_num;
						}
						is_gems = false;
					}
					else
					{
						if (m_param.new_mysterious_trial_info[day_index][i].schedule + add_num >= MAX_INT)
						{
							m_param.new_mysterious_trial_info[day_index][i].schedule = MAX_INT;
						}
						else
						{
							m_param.new_mysterious_trial_info[day_index][i].schedule += add_num;
						}
					}
				}
				else if (it->second.node_cfg[i].parameter1 == 0 || it->second.node_cfg[i].parameter1 == 3)
				{
					m_param.new_mysterious_trial_info[day_index][i].schedule = add_num;
				}
				else
				{
					return;
				}
				if (it->second.node_cfg[i].parameter1 == 3)//直接等于条件需要完全满足
				{
					if (m_param.new_mysterious_trial_info[day_index][i].schedule == it->second.node_cfg[i].parameter2)
					{
						m_param.new_mysterious_trial_info[day_index][i].flag = 1;
					}
				}
				else
				{
					if (m_param.new_mysterious_trial_info[day_index][i].schedule >= it->second.node_cfg[i].parameter2)
					{
						m_param.new_mysterious_trial_info[day_index][i].flag = 1;
					}
				}

				if (it->second.node_cfg[i].parameter1 != 2 || m_param.open_day > i)
				{
					this->SendSingleInfo(day_index, i);
				}
			}
			add_num = num;
		}
	}
	/*for (int i = 0; i < RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM; i++)
	{
		for (int j = 0; j < RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM; j++)
		{
			if (type == (*cfg)[i].node_cfg[j].activity_type && m_param.new_mysterious_trial_info[i][j].flag == 0)
			{
				switch (type)
				{
					case MSC_CON_LOGIN_DAY:
					case MSC_CON_ROLE_LEVEL:
					case MSC_CON_PROF_PRO:
					case MSC_CON_KILL_BOSS:
					case MSC_CON_COMP_TREA:
					case MSC_CON_COMPLETE_FB:
					case MSC_CON_ONLINE_TIME:
					case MSC_CON_TEMP_COMP_FB:
					case MSC_CON_PET_USE_BOOK:
					case MSC_CON_TRIAL_BOSS:
					case MSC_CON_TRA_MAR_PUR:
					case MSC_CON_KILL_YW_BOSS:
					case MSC_CON_ACT_COLLECT:
					case MSC_CON_SHOP_CONSUME:
					case MSC_CON_JOIN_XIYOUFB:
					{
						if (add_num < 0) return;
					}
					break;
					case MSC_CON_SPECIAL_PET_INTEN:
						{
							if (param < 0 || add_num < 0)return;

							if ((*cfg)[i].node_cfg[j].parameter2 != add_num || (*cfg)[i].node_cfg[j].parameter3 != param)
							{
								add_num = 0;
							}
						}
						break;
					case MSC_CON_ROLE_SKILL_LEVEL:
						{
							if (add_num < 0)return;

							if ((*cfg)[i].node_cfg[j].parameter2 > add_num)
							{
								add_num = 0;
							}
						}
						break;
					case MSC_CON_PET_SKILL_NUM:
						{
							if (add_num < 0)return;
							add_num = param;
							if ((*cfg)[i].node_cfg[j].parameter2 > add_num)
							{
								add_num = 0;
							}
						}
						break;
					case MSC_CON_PRESTIG_LEVEL:
						{
							if (add_num < 0)return;
							add_num = param;
							if((*cfg)[i].node_cfg[j].parameter2 > add_num)
							{
								add_num = 0;
							}
						}
						break;
					case MSC_CON_MI_YI:
						{
							if (add_num < 0)return;
							add_num = (*cfg)[i].node_cfg[j].parameter2;
							const MiYiLevelCfg * miyi_cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySeq((*cfg)[i].node_cfg[j].parameter2 - 1);
							if (NULL != miyi_cfg)
							{
								for (int miyi_monster_idx = 0; miyi_monster_idx < MI_YI_MAX_MONSTER_NUM && miyi_monster_idx < (int)miyi_cfg->monsters.size(); miyi_monster_idx++)
								{
									if (!m_module_mgr->GetMiYi()->IsWeekKilled(miyi_cfg->seq, miyi_monster_idx))
									{
										add_num = 0;
									}
								}
							}
							else
							{
								add_num = 0;
							}
						}
						break;
					case MSC_CON_PARTNER_FETTERS:
						{
							if (add_num < 0)return;
							if (m_param.new_mysterious_trial_info[i][j].schedule >= add_num)return;
						}
						break;
					case MSC_CON_GET_MEDAL:
						{
							if (param < 0 || add_num < 0)return;

							if ((*cfg)[i].node_cfg[j].parameter2 > add_num || (*cfg)[i].node_cfg[j].parameter3 > param)
							{
								add_num = 0;
							}
						}
						break;
					case MSC_CON_CATCH_PET:
						{
							if (param < 0)return;
							const PetCfg* pet_config = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(param);
							if (pet_config == NULL)return;
							add_num = pet_config->quality;
							if ((*cfg)[i].node_cfg[j].parameter2 != pet_config->quality)
							{
								add_num = 0;
							}
						}
						break;
					case MSC_CON_COLOSSEUM:
					{
							if (add_num < 0)return;
							if (add_num > (*cfg)[i].node_cfg[j].parameter2)return;
							add_num = (*cfg)[i].node_cfg[j].parameter2;
					}
					break;
					case MSC_CON_EQPM_FORG:
					{
						if (param == 0) return;
						const ItemBase * item = ITEMPOOL->GetItem(param);
						if (NULL == item || (item->GetItemType() != I_TYPE_EQUIPMENT && item->GetItemType() != I_TYPE_JEWELRY)) return;
						if (item->GetItemType() == I_TYPE_EQUIPMENT)
						{
							Equipment* item_eq = (Equipment *)item;
							add_num = item->GetLimitLevel();
							if (item_eq->GetEquipColor() != (*cfg)[i].node_cfg[j].parameter3 || add_num != (*cfg)[i].node_cfg[j].parameter2)
							{
								add_num = 0;
							}
						}
						else if(item->GetItemType() == I_TYPE_JEWELRY)
						{
							Jewelry* item_je = (Jewelry *)item;
							add_num = item_je->GetLimitLevel();
							if (item_je->GetEquipColor() != (*cfg)[i].node_cfg[j].parameter3 || add_num != (*cfg)[i].node_cfg[j].parameter2)
							{
								add_num = 0;
							}
						}
					}
					break;
					case MSC_CON_PET_INTEN:
					{
						add_num = m_module_mgr->GetPet()->GetPetIntensifyMax();
					}
					break;
					case MSC_CON_COUR_CHAG:
					{
						add_num = m_module_mgr->GetCourageChallenge()->GetLayerScoreNum((*cfg)[i].node_cfg[j].parameter3);
					}
					break;
					case MSC_CON_PARTNER_NUM:
					{
						add_num = m_module_mgr->GetPartner()->GetPartnerCurCount();
					}
					break;
					case MSC_CON_PET_CAPABIL:
					{
						add_num = m_module_mgr->GetPet()->GetPetCapabilityMax();
					}
					break;
					case MSC_CON_GEMS_MOSAIC:
					{
						if (param <= 0) return;

						const GemstoneBasicInfoCfg * mate_gem = LOGIC_CONFIG->GetGemstoneConfig()->GetGemstoneBasicInfoCfg(param);
						if (mate_gem == NULL) return;
						switch (add_num)
						{
							case 0:                            // 镶嵌宝石
							{
								if (mate_gem->level != (*cfg)[i].node_cfg[j].parameter3)
								{
									add_num = 0;
								}
								else
								{
									add_num = 1;
								}
							}
							break;
							case 1:                           // 合成宝石/升级宝石
							{
								if (mate_gem->level == (*cfg)[i].node_cfg[j].parameter3 && mate_gem->level - 1 < (*cfg)[i].node_cfg[j].parameter3)
								{
									add_num = 1;
								}
								else
								{
									add_num = 0;
								}
							}
							break;
							case 2:                          // 拆卸宝石
							{
								if (mate_gem->level != (*cfg)[i].node_cfg[j].parameter3)
								{
									add_num = 0;
								}
								else
								{
									add_num = 1;
									is_gems = true;
								}
							}
							break;
							default:
							break;
						}
					}
					break;
					case MSC_CON_PARTNER_PRO:
					{
						if (add_num < 0) return;
						if (add_num < m_param.new_mysterious_trial_info[i][j].schedule)
						{
							add_num = m_param.new_mysterious_trial_info[i][j].schedule;
						}
					}
					break;
					case MSC_CON_JOIN_FAMILY:
					{
						if (add_num < 0) return;
						if (NULL == m_module_mgr->GetRole()->GetGuild())
						{
							add_num = 0;
						}
					}
					break;
					case MSC_CON_USER_CAPABIL:
					{
						add_num = m_module_mgr->GetCapability()->GetRoleCapability();
						if (add_num <= 0) return;
					}
					break;
					case MSC_CON_JOIN_XIYOUFB_LEVEL:
						{
							add_num = param;
						}
						break;
				}

				if ((*cfg)[i].node_cfg[j].parameter1 == 1 || ((*cfg)[i].node_cfg[j].parameter1 == 2 && m_param.open_day > i))
				{
					if (is_gems)
					{
						if (m_param.new_mysterious_trial_info[i][j].schedule > 0)
						{
							m_param.new_mysterious_trial_info[i][j].schedule -= add_num;
						}
						is_gems = false;
					}
					else
					{
						if (m_param.new_mysterious_trial_info[i][j].schedule + add_num >= MAX_INT)
						{
							m_param.new_mysterious_trial_info[i][j].schedule = MAX_INT;
						}
						else
						{
							m_param.new_mysterious_trial_info[i][j].schedule += add_num;
						}
					}
				}
				else if ((*cfg)[i].node_cfg[j].parameter1 == 0 || (*cfg)[i].node_cfg[j].parameter1 == 3)
				{
					m_param.new_mysterious_trial_info[i][j].schedule = add_num;
				}
				else
				{
					return;
				}
				if ((*cfg)[i].node_cfg[j].parameter1 == 3)//直接等于条件需要完全满足
				{
					if (m_param.new_mysterious_trial_info[i][j].schedule == (*cfg)[i].node_cfg[j].parameter2)
					{
						m_param.new_mysterious_trial_info[i][j].flag = 1;
					}
				}
				else
				{
					if (m_param.new_mysterious_trial_info[i][j].schedule >= (*cfg)[i].node_cfg[j].parameter2)
					{
						m_param.new_mysterious_trial_info[i][j].flag = 1;
					}
				}

				if ((*cfg)[i].node_cfg[j].parameter1 != 2 || m_param.open_day > i)
				{
					this->SendSingleInfo(i, j);
				}
			}
			add_num = num;
		}
	}*/
}