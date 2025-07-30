#include "sanxianguidong.hpp"
#include "gameworld/gameworld/engineadapter.h"
#include "gameworld/config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "other/route/mailroute.hpp"
#include "gamelog.h"
#include "protocol/msgother.h"
#include "global/team/team.hpp"
#include "servercommon/noticenum.h"
#include "sanxianguidongconfig.hpp"
#include "scene/fbmanager.hpp"
#include "other/title/title.hpp"
#include "global/zhouqimanager/zhouqimanager.h"
#include "servercommon/zhouqidef.hpp"

SanXianGuiDong::SanXianGuiDong()
{
}

SanXianGuiDong::~SanXianGuiDong()
{
}

void SanXianGuiDong::Init(RoleModuleManager * module_mgr, const int & param, const int & help_param, const unsigned int & tamp)
{
	m_module_mgr = module_mgr;
	today_play_times = param;
	pass_idx = help_param;
	choose_tamp = tamp;
}

void SanXianGuiDong::GetInitParam(int * param, int * help_param, unsigned int * tamp)
{
	*param = today_play_times;
	*help_param = pass_idx;
	*tamp = choose_tamp;
	
}

void SanXianGuiDong::OnLogout()
{
	choose_tamp = 0;
}

void SanXianGuiDong::OnDayChange()
{
	today_play_times = 0;
	pass_idx = 0;
	choose_tamp = 0;
	this->SendInfo();
}

void SanXianGuiDong::UpDate(unsigned int now_second)
{
	if (choose_tamp > 0 && choose_tamp <= now_second)
	{
		this->EndPlay(Protocol::SCSanXianGuiDongPlayEndInfo::GAME_TYPE_LOSE);
		choose_tamp = 0;
	}
}

void SanXianGuiDong::StarChoose()
{
	const SanXianOtherCfg & other_cfg = LOGIC_CONFIG->GetSanXianConfig()->GetOtherCfg();
	choose_tamp = (unsigned int)EngineAdapter::Instance().Time() + other_cfg.chose_time;
	this->SendEndPlayInfo(Protocol::SCSanXianGuiDongPlayEndInfo::GAME_TYPE_PLAYING);
}

void SanXianGuiDong::Choose(bool is_true)
{
	if (choose_tamp == 0)return;
	this->EndPlay(is_true ? Protocol::SCSanXianGuiDongPlayEndInfo::GAME_TYPE_WIN : Protocol::SCSanXianGuiDongPlayEndInfo::GAME_TYPE_LOSE);
	choose_tamp = 0;
	this->SendInfo();
}

void SanXianGuiDong::SendInfo()
{
	if (!ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_SAN_XIAN_GUI_DONG))
	{
		return;
	}
	Protocol::SCSanXianGuiDongInfo info;
	info.pass_idx = pass_idx;
	info.today_play_times = today_play_times;
	m_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void SanXianGuiDong::EnterFb()
{
	const SanXianOtherCfg & other_cfg = LOGIC_CONFIG->GetSanXianConfig()->GetOtherCfg();
	FBManager::Instance().EnterFB(m_module_mgr->GetRole(), FBManager::FB_TYPE_SAN_XIAN_GUI_DONG, other_cfg.scene_id, 0, false);
}

void SanXianGuiDong::EndPlay(int game_type)
{
	const SanXianOtherCfg & other_cfg = LOGIC_CONFIG->GetSanXianConfig()->GetOtherCfg();
	bool check_title = false;
	if (game_type == Protocol::SCSanXianGuiDongPlayEndInfo::GAME_TYPE_WIN)
	{
		pass_idx += 1;
		const SanXianLevelCfg * level_cfg = LOGIC_CONFIG->GetSanXianConfig()->GetLevelCfg(pass_idx);
		if (NULL != level_cfg)
		{
			const SanXianLevelCfg * pass_cfg = LOGIC_CONFIG->GetSanXianConfig()->GetLevelCfg(pass_idx + 1);//检查下一关
			if (NULL == pass_cfg)
			{
				std::vector<ItemConfigData> real_item_vec;
				if (pass_idx >= other_cfg.title_level)//称号奖励无需获奖次数
				{
					if (m_module_mgr->GetCommonData().sanxian_title_flag == 0)
					{
						ItemConfigData title;
						title.item_id = other_cfg.title_item_id;
						title.num = 1;
						title.is_bind = true;
						real_item_vec.push_back(title);
						m_module_mgr->GetCommonData().sanxian_title_flag = 1;
					}
					check_title = true;
				}
				//完全通关--通关奖励必须要有次数
				if (today_play_times < other_cfg.reward_time)
				{
					if ((int)level_cfg->reward_vec.size() > 0)
					{
						real_item_vec.insert(real_item_vec.end(), level_cfg->reward_vec.begin(), level_cfg->reward_vec.end());
					}
				}

				if ((int)real_item_vec.size() > 0 && !m_module_mgr->GetKnapsack()->PutList((short)real_item_vec.size(), &real_item_vec[0], PUT_REASON_SAN_XIAN_GUI_DONG_REWARD))
				{
					m_module_mgr->GetKnapsack()->SendMail(real_item_vec, SEND_MAIL_TYPE_SAN_XIAN_GUI_DONG_REWARD);
				}

				pass_idx = 0;
				today_play_times += 1;
			}
		}
	}
	else
	{
		//发放奖励
		{
			std::vector<ItemConfigData> real_item_vec;
			if (pass_idx >= other_cfg.title_level)
			{
				if (m_module_mgr->GetCommonData().sanxian_title_flag == 0)
				{
					ItemConfigData title;
					title.item_id = other_cfg.title_item_id;
					title.num = 1;
					title.is_bind = true;
					real_item_vec.push_back(title);
					m_module_mgr->GetCommonData().sanxian_title_flag = 1;
				}
				check_title = true;
			}

			const SanXianLevelCfg * reward_cfg = LOGIC_CONFIG->GetSanXianConfig()->GetLevelCfg(pass_idx);
			if (NULL != reward_cfg)
			{
				if (today_play_times < other_cfg.reward_time && (int)reward_cfg->reward_vec.size() > 0)
				{
					real_item_vec.insert(real_item_vec.end(), reward_cfg->reward_vec.begin(), reward_cfg->reward_vec.end());
				}
			}

			if (pass_idx == 0)
			{
				if (today_play_times < other_cfg.reward_time && (int)other_cfg.reward_vec.size() > 0)
				{
					real_item_vec.insert(real_item_vec.end(), other_cfg.reward_vec.begin(), other_cfg.reward_vec.end());
				}
			}

			if ((int)real_item_vec.size() > 0)
			{
				if (!m_module_mgr->GetKnapsack()->PutList((short)real_item_vec.size(), &real_item_vec[0], PUT_REASON_SAN_XIAN_GUI_DONG_REWARD))
				{
					m_module_mgr->GetKnapsack()->SendMail(real_item_vec, SEND_MAIL_TYPE_SAN_XIAN_GUI_DONG_REWARD);
				}
				today_play_times += 1;
			}
		}
		pass_idx = 0;
	}

	if (check_title)
	{
		if (!m_module_mgr->GetTitle()->HasTitle(other_cfg.title_id))//补偿一个称号
		{
			m_module_mgr->GetTitle()->AddTitle(other_cfg.title_id, __FUNCTION__);
		}
	}

	this->SendEndPlayInfo(game_type);
}

void SanXianGuiDong::SendEndPlayInfo(int game_type)
{
	Protocol::SCSanXianGuiDongPlayEndInfo info;
	info.game_type = game_type;
	info.pass_idx = pass_idx;
	info.choose_tamp = choose_tamp;
	info.sanxian_title_flag = m_module_mgr->GetCommonData().sanxian_title_flag;
	m_module_mgr->NetSend((const char *)&info, sizeof(info));
}
