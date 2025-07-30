#include "roleactivitysevencolorpicture.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitysevencolorpictureconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "protocol/randactivity/msgrasevencolorpicture.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "other/roleactivity/impl/roleactivitywanlinggongying.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

RoleActivitySevenColorPicture::RoleActivitySevenColorPicture(int type)
	:RoleActivity(type)
{

}

RoleActivitySevenColorPicture::~RoleActivitySevenColorPicture()
{

}

void RoleActivitySevenColorPicture::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RASevenColorPictureParam));
}

void RoleActivitySevenColorPicture::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RASevenColorPictureParam));
}

void RoleActivitySevenColorPicture::OnLoginSendInfo()
{
	this->SendRASevenColorPicutreInfo();
}

void RoleActivitySevenColorPicture::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE != rand_act_type)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_seven_color_picture_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.ra_seven_color_picture_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		memset(m_param.ra_seven_color_picture_pigment, 0, sizeof(m_param.ra_seven_color_picture_pigment));
		memset(m_param.ra_seven_color_picture_score, 0, sizeof(m_param.ra_seven_color_picture_score));
		memset(m_param.ra_seven_color_picture_apply, 0, sizeof(m_param.ra_seven_color_picture_apply));
		m_param.seven_color_picture_flag.Reset();
		memset(m_param.ra_seven_color_area_data, 0, sizeof(m_param.ra_seven_color_area_data));
		memset(m_param.reserve_ch, 0, sizeof(m_param.reserve_ch));
		m_param.ra_seven_color_picutre_role_level = m_role_module_mgr->GetRole()->GetLevel();

		this->SetRoleDataChange();
	}
}

void RoleActivitySevenColorPicture::OnRandActivityMakeUpReq(Protocol::CSRandActivityMakeUpReq * req)
{
	if (m_param.seven_color_picture_flag.IsBitSet(SEVEN_COLOR_PICTURE_SCORE_MAX_NUM))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_MAKE_UP_REWARD_FINISHED);
		return;
	}

	int count = 0;
	for (int i = 0; i < SEVEN_COLOR_PICTURE_SCORE_MAX_NUM; i++)
	{
		if (!m_param.seven_color_picture_flag.IsBitSet(i))
		{
			count += SEVEN_COLOR_PICTURE_APPLY_MAX_NUM - m_param.ra_seven_color_picture_apply[i];
		}
	}
	if (count <= 0)
	{
		//保险加个日志
		gamelog::g_log_rand_activity.printf(LL_ERROR, "%s user[%d, %s], level[%d], type[%d] count[%d]", __FUNCTION__,m_role_module_mgr->GetRole()->GetUID(), 
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE, count);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivitySevenColorPictureConfig * config = static_cast<const RandActivitySevenColorPictureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE));
	if (NULL == config)
	{
		return;
	}
	const RASevenColorPictureOtherCfg & other_cfg = config->GetOtherCfg(ramgr);
	if (0 == other_cfg.is_complement)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_MAKE_UP_CUR_TIME_NOT_USE);
		return;
	}

	if (0 == other_cfg.is_complement)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_MAKE_UP_CUR_TIME_NOT_USE);
		return;
	}

	int end_day_time = config->GetCurRealEndDay(ramgr, RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
	if (end_day_time > other_cfg.end_day_time)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_MAKE_UP_CUR_TIME_NOT_USE);
		return;
	}

	switch (req->make_up_type)
	{
		case RAND_ACTIVITY_MAKE_UP_TYPE_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->UseCoinBind(other_cfg.coin_num * count, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		case RAND_ACTIVITY_MAKE_UP_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.gold_num * count, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary(1, &other_cfg.seven_color_collection_reward))
	{
		m_role_module_mgr->GetKnapsack()->PutList(1, &other_cfg.seven_color_collection_reward, PUT_REASON_SEVEN_COLOR_PICTURE);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&other_cfg.seven_color_collection_reward, 1, SEND_MAIL_TYPE_MAKE_UP_REWARD, true, RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
	}


	for (int i = 0; i < SEVEN_COLOR_PICTURE_SCORE_MAX_NUM; i++)
	{
		if (!m_param.seven_color_picture_flag.IsBitSet(i))
		{
			m_param.ra_seven_color_picture_apply[i] = SEVEN_COLOR_PICTURE_APPLY_MAX_NUM;
			m_param.seven_color_picture_flag.SetBit(i);
		}
	}

	m_param.seven_color_picture_flag.SetBit(SEVEN_COLOR_PICTURE_SCORE_MAX_NUM);
	this->SendRASevenColorPicutreInfo();
	this->SetRoleDataChange();
}

void RoleActivitySevenColorPicture::OnRASevenColorPictureOp(int type, int picture_index, char seven_color_picture_pigment[][SEVEN_COLOR_PIGMENT_TYPE_MAX], char * msg) 
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);

	switch (type)
	{
	case Protocol::SEVEN_COLOR_PICTURE_REQ_TYPE_INFO:
		{
			this->SendRASevenColorPicutreInfo();
		}
		break;
	case Protocol::SEVEN_COLOR_PICTURE_REQ_TYPE_USE_PIGMENT:
		{
			this->OnRASevenColorPictureUsePigment(picture_index, seven_color_picture_pigment, msg);  // param1 - picture_index  param2 - color_index param3 - pigment_index
		}
		break;
	}
	return;
}

bool RoleActivitySevenColorPicture::OnAddPigmentByItem(int use_num)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return false;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_OPEN_ITEM_NOT_USE);
		return false;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);

	int rand_num1 = 0;
	int rand_num2 = 0;
	for (int i = 0; i < use_num; i++)
	{
		rand_num1 = rand() % SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX;
		rand_num2 = rand() % SEVEN_COLOR_PIGMENT_TYPE_MAX;

		m_param.ra_seven_color_picture_pigment[rand_num1][rand_num2]++;
	}
	m_role_module_mgr->NoticeNum(noticenum::NT_RA_SEVEN_PICTURE_USE_SUCC);

	this->SendRASevenColorPicutreInfo();
	this->SetRoleDataChange();

	return true;
}

void RoleActivitySevenColorPicture::SendRASevenColorTypeInfo(int color_index, int pigment_index)
{
	if (color_index < 0 || color_index >= SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX || pigment_index < 0 || pigment_index >= SEVEN_COLOR_PIGMENT_TYPE_MAX) return;

	Protocol::SCRASevenColorTypeInfo info;
	info.seq = color_index;
	info.quality = pigment_index;
	info.num = m_param.ra_seven_color_picture_pigment[color_index][pigment_index];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySevenColorPicture::SendRASevenColorPicutreInfo()
{
	Protocol::SCRASevenColorPictureInfo info;
	memcpy(info.seven_color_picture_pigment, m_param.ra_seven_color_picture_pigment, sizeof(info.seven_color_picture_pigment));
	info.role_level = m_param.ra_seven_color_picutre_role_level;
	info.is_fetch_make_up_flag = m_param.seven_color_picture_flag.IsBitSet(SEVEN_COLOR_PICTURE_SCORE_MAX_NUM) ? 1 : 0;
	memcpy(info.ra_seven_color_area_data, m_param.ra_seven_color_area_data, sizeof(info.ra_seven_color_area_data));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivitySevenColorPicture::OnRASevenColorPictureUsePigment(int picture_index, char seven_color_picture_pigment[][SEVEN_COLOR_PIGMENT_TYPE_MAX], char * msg)
{
	if (picture_index < 0 || picture_index >= SEVEN_COLOR_PICTURE_SCORE_MAX_NUM) return;

	const RandActivitySevenColorPictureConfig * config = static_cast<const RandActivitySevenColorPictureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE));
	if (NULL == config)
	{
		return;
	}

	const RASevenColorPicturePigmentCfg* cfg = config->GetSevenColorPicturePigmentCfg();
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int count = 0;
	int type_count = 0;					//使用了多少不同的颜料
	int type_list[20];					//记录是哪种颜料
	for (int i = 0; i < SEVEN_COLOR_PIGMENT_COLOR_TYPE_MAX; i++)
	{
		for (int j = 0; j < SEVEN_COLOR_PIGMENT_TYPE_MAX; j++)
		{
			if (seven_color_picture_pigment[i][j] > 0)
			{
				count += seven_color_picture_pigment[i][j];
				if (seven_color_picture_pigment[i][j] > m_param.ra_seven_color_picture_pigment[i][j])
				{
					m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PIGMENT_NOT_ENOUGH);
					this->SendRASevenColorPicutreInfo();
					return;
				}
				if (count + m_param.ra_seven_color_picture_apply[picture_index] > SEVEN_COLOR_PICTURE_APPLY_MAX_NUM)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PICTURE_REACHED_THE_LIMIT);
					this->SendRASevenColorPicutreInfo();
					return;
				}
				type_list[type_count++] = (i + 1) * 1000 + j;		//颜色*1000 + 品质
			}
		}
	}

	for (int i = 0; i < type_count; i++)
	{
		m_param.ra_seven_color_picture_score[picture_index] += cfg[type_list[i] % 1000].is_special * seven_color_picture_pigment[type_list[i] / 1000 - 1][type_list[i] % 1000];
		m_param.ra_seven_color_picture_pigment[type_list[i] / 1000 - 1][type_list[i] % 1000] -= seven_color_picture_pigment[type_list[i] / 1000 - 1][type_list[i] % 1000];
	}

	m_param.ra_seven_color_picture_apply[picture_index] += count;
	memcpy(m_param.ra_seven_color_area_data, msg, sizeof(m_param.ra_seven_color_area_data));
	m_role_module_mgr->NoticeNum(noticenum::NT_RA_SEVEN_PICTURE_SUCC);

	if (m_param.ra_seven_color_picture_apply[picture_index] >= SEVEN_COLOR_PICTURE_APPLY_MAX_NUM)
	{
		if (!m_param.seven_color_picture_flag.IsBitSet(picture_index))
		{
			this->OnRASevenColorPictureReward(picture_index);

			{
				RoleActivityWanLingGongYing * role_activity = static_cast<RoleActivityWanLingGongYing *>
					(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING));
				if (NULL != role_activity)
				{
					role_activity->OnCompleteTask(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
				}
			}
		}
	}
	this->SendRASevenColorPicutreInfo();

	this->SetRoleDataChange();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivitySevenColorPicture::OnRASevenColorPictureReward(int picture_index)
{
	if (picture_index < 0 || picture_index >= SEVEN_COLOR_PICTURE_SCORE_MAX_NUM) return;

	const RandActivitySevenColorPictureConfig * config = static_cast<const RandActivitySevenColorPictureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE));
	if (NULL == config)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RASevenColorPictureAwardCfg * cfg = config->GetSevenColorPictureAwardCfg(m_param.ra_seven_color_picutre_role_level, m_param.ra_seven_color_picture_score[picture_index], ramgr);
	if (NULL == cfg)
	{
		return;
	}
	bool is_knapsack_space = true;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_num, cfg->reward))
	{
		if (m_role_module_mgr->GetKnapsack()->PutList(cfg->reward_num, cfg->reward, PUT_REASON_SEVEN_COLOR_PICTURE))
		{
			is_knapsack_space = false;
		}
	}

	if (is_knapsack_space)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward, cfg->reward_num, SEND_MAIL_TYPE_SEVEN_COLOR, true, m_param.ra_seven_color_picture_score[picture_index]);
	}
	m_param.seven_color_picture_flag.SetBit(picture_index);

	if (cfg->hearsay == 1 && !CrossConfig::Instance().IsHiddenServer())
	{
		for (int i = 0; i < cfg->reward_num; ++i)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_seven_color_picture_hearsay_content,
								  m_role_module_mgr->GetRole()->GetName(), RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE, cfg->reward[i].item_id, RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}
	}

	gamelog::g_log_rand_activity.buff_printf("%s user[%d, %s], level[%d], type[%d]", __FUNCTION__,
											 m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
											 m_role_module_mgr->GetRole()->GetLevel(), RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
	for (int i = 0; i < cfg->reward_num; ++i)
	{
		gamelog::g_log_rand_activity.buff_printf("item[%d ,%d]", cfg->reward[i].item_id, cfg->reward[i].num);
	}

	gamelog::g_log_rand_activity.commit_buff(LL_INFO);

	bool is_found = true;
	for (int i = 0; i < SEVEN_COLOR_PICTURE_SCORE_MAX_NUM; i++)
	{
		if (!m_param.seven_color_picture_flag.IsBitSet(i))
		{
			is_found = false;
		}
	}
	if (is_found)
	{
		this->OnRASevenColorPictureTotalReward();
	}

	this->SetRoleDataChange();
}

void RoleActivitySevenColorPicture::OnRASevenColorPictureTotalReward()
{
	if (m_param.seven_color_picture_flag.IsBitSet(SEVEN_COLOR_PICTURE_SCORE_MAX_NUM))
	{
		return;
	}

	const RandActivitySevenColorPictureConfig * config = static_cast<const RandActivitySevenColorPictureConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE));
	if (NULL == config)
	{
		return;
	}
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RASevenColorPictureOtherCfg & other_cfg = config->GetOtherCfg(ramgr);

	m_role_module_mgr->GetKnapsack()->SendMail(&other_cfg.seven_color_collection_reward, 1, SEND_MAIL_TYPE_SEVEN_COLOR_COUNT);

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], type[%d] item[id:%d , num:%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE, other_cfg.seven_color_collection_reward.item_id, other_cfg.seven_color_collection_reward.num);

	m_param.seven_color_picture_flag.SetBit(SEVEN_COLOR_PICTURE_SCORE_MAX_NUM);

	this->SetRoleDataChange();
}

