#include "taoquannaji.hpp"
#include "gameworld/gameworld/engineadapter.h"
#include "gameworld/config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "protocol/msgother.h"
#include "taoquannajiconfig.hpp"
#include "scene/fbmanager.hpp"
#include "global/zhouqimanager/zhouqimanager.h"
#include "servercommon/zhouqidef.hpp"

TaoQuanNaJi::TaoQuanNaJi()
{
}

TaoQuanNaJi::~TaoQuanNaJi()
{
}

void TaoQuanNaJi::Init(RoleModuleManager * module_mgr, const TaoQuanNaJiParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;
	later_send_msg_tamp = 0;
}

void TaoQuanNaJi::GetInitParam(TaoQuanNaJiParam * param)
{
	*param = m_param;
}

void TaoQuanNaJi::Update(unsigned int now_second)
{
	if (later_send_msg_tamp > 0 && later_send_msg_tamp < now_second)
	{
		this->SendInfo();
		later_send_msg_tamp = 0;
	}
}

void TaoQuanNaJi::OnDayChange()
{
	this->EndGame();
	m_param.DayReset();
	this->SendInfo();
	this->SendPlayInfo();
}

void TaoQuanNaJi::LeaveFb()
{
	this->EndGame();
}

void TaoQuanNaJi::Begain()
{
	if (m_param.map_init == 1)return;
	const NaJiOther & other_cfg = LOGIC_CONFIG->GetTaoQuanNaJiConfig()->GetOtherCfg();
	m_param.can_play_times = other_cfg.circle_num;
	this->RandMap();
	this->SendInfo();
	this->SendPlayInfo();
}

void TaoQuanNaJi::TaoQuan(int x, int y)
{
	if (m_param.map_init == 0)return;

	if (x >= 0 && x < TAO_QUAN_NA_JI_X_NUM && y >= 0 && y < TAO_QUAN_NA_JI_Y_NUM)
	{
		int &get_point = m_param.play_map[x][y];

		//检查效果
		const NaJiItem * item_cfg = LOGIC_CONFIG->GetTaoQuanNaJiConfig()->GetItemCfg(get_point);
		if (NULL != item_cfg)
		{
			//特殊效果
			switch (item_cfg->sp_type)
			{
			case NaJiItem::ITEM_EFFECT_TYPE_1:
				m_param.sroce += this->MapEffectDisapperSameType(item_cfg->item_type);
				break;
			case NaJiItem::ITEM_EFFECT_TYPE_2:
				m_param.sroce += this->MapEffectDisapperPointAround(x, y);
				break;
			default:
				switch (item_cfg->item_type)
				{
				case 1:
					m_param.item_1_count += 1;
					break;
				case 2:
					m_param.item_2_count += 1;
					break;
				case 3:
					m_param.item_3_count += 1;
					break;
				case 4:
					m_param.item_4_count += 1;
					break;
				default:
					break;
				}
				m_param.sroce += item_cfg->score;
				break;
			}
		}
		get_point = 0;
	}
	m_param.can_play_times -= 1;
	this->SendPlayInfo();
	//检查结算
	bool later_send_msg = false;
	if (this->CheckMapBeClear())
	{
		m_param.map_init = 0;
		this->SendInfo();//清空后立刻下发一次,才刷新地图
		this->RandMap();
		later_send_msg = true;
		later_send_msg_tamp = EngineAdapter::Instance().Time() + 3;//延迟3秒下发
	}
	else if (m_param.can_play_times <= 0)
	{
		//结束游戏
		this->EndGame();
	}
	if(!later_send_msg)this->SendInfo();
}

void TaoQuanNaJi::EnterFb()
{
	const NaJiOther & other_cfg = LOGIC_CONFIG->GetTaoQuanNaJiConfig()->GetOtherCfg();
	FBManager::Instance().EnterFB(m_module_mgr->GetRole(), FBManager::FB_TYPE_TAO_QUAN_NA_JI, other_cfg.scene_id, 0, false);
}

void TaoQuanNaJi::SendInfo()
{
	if (!ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_TAO_QUAN_NA_JI))
	{
		return;
	}
	Protocol::SCTaoQuanNaJiInfo info;
	info.map_init = m_param.map_init;
	info.can_play_times = m_param.can_play_times;
	info.today_play_times = m_param.today_play_times;
	info.flag_title = m_param.flag_title;
	for (int i = 0; i < TAO_QUAN_NA_JI_X_NUM; i++)
	{
		for (int j = 0; j < TAO_QUAN_NA_JI_Y_NUM; j++)
		{
			info.play_map[i][j] = m_param.play_map[i][j];
		}
	}

	m_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void TaoQuanNaJi::RandMap()
{
	//获取随机地图
	if (m_param.map_init == 1)return;
	
	const NaJiMapList * rand_map_cfg = LOGIC_CONFIG->GetTaoQuanNaJiConfig()->RandMapCfg();
	if (NULL != rand_map_cfg)
	{
		std::vector<int> item_vec;
		for (std::vector<NaJiMapNode>::const_iterator it = rand_map_cfg->m_map_list.begin(); it != rand_map_cfg->m_map_list.end(); it++)
		{
			for (int item_idx = 0; item_idx < it->item_num; item_idx++)
			{
				item_vec.push_back(it->item_id);
			}
		}
		if ((int)item_vec.size() <= 0)
		{
			printf("%s group[%d] map_point_num[%d] node_list_size[%d] ", __FUNCTION__, rand_map_cfg->group, rand_map_cfg->map_point_num, (int)rand_map_cfg->m_map_list.size());
		}
		for (int x = 0; x < TAO_QUAN_NA_JI_X_NUM; x++)
		{
			for (int y = 0; y < TAO_QUAN_NA_JI_Y_NUM; y++)
			{
				if (item_vec.size() > 0)
				{
					int item_idx = rand() % item_vec.size();
					m_param.play_map[x][y] = item_vec[item_idx];
					item_vec.erase(item_vec.begin() + item_idx);
				}
			}
		}

		m_param.map_init = 1;
	}
}

int TaoQuanNaJi::MapEffectDisapperSameType(int item_type)
{
	int count_score = 0;
	for (int x = 0; x < TAO_QUAN_NA_JI_X_NUM; x++)
	{
		for (int y = 0; y < TAO_QUAN_NA_JI_Y_NUM; y++)
		{
			int &get_point = m_param.play_map[x][y];
			if (get_point != 0)
			{
				const NaJiItem * item_cfg = LOGIC_CONFIG->GetTaoQuanNaJiConfig()->GetItemCfg(get_point);
				if (NULL != item_cfg && item_cfg->item_type == item_type)
				{
					count_score += item_cfg->score;
					switch (item_cfg->item_type)
					{
					case 1:
						m_param.item_1_count += 1;
						break;
					case 2:
						m_param.item_2_count += 1;
						break;
					case 3:
						m_param.item_3_count += 1;
						break;
					case 4:
						m_param.item_4_count += 1;
						break;
					default:
						break;
					}
					get_point = 0;
				}
			}
		}
	}
	return count_score;
}

int TaoQuanNaJi::MapEffectDisapperPointAround(int targe_x, int targe_y)
{
	int count_score = 0;
	for (int x = 0; x < TAO_QUAN_NA_JI_X_NUM; x++)
	{
		for (int y = 0; y < TAO_QUAN_NA_JI_Y_NUM; y++)
		{
			if (x >= targe_x - 1 && x <= targe_x + 1 &&
				y >= targe_y - 1 && y <= targe_y + 1)
			{//矩形范围
				int &get_point = m_param.play_map[x][y];
				const NaJiItem * item_cfg = LOGIC_CONFIG->GetTaoQuanNaJiConfig()->GetItemCfg(get_point);
				if (NULL != item_cfg)
				{
					count_score += item_cfg->score;
					switch (item_cfg->item_type)
					{
					case 1:
						m_param.item_1_count += 1;
						break;
					case 2:
						m_param.item_2_count += 1;
						break;
					case 3:
						m_param.item_3_count += 1;
						break;
					case 4:
						m_param.item_4_count += 1;
						break;
					default:
						break;
					}
				}
				if (get_point != 0)
				{
					get_point = 0;
				}
			}
		}
	}
	return count_score;
}

bool TaoQuanNaJi::CheckMapBeClear()
{
	for (int x = 0; x < TAO_QUAN_NA_JI_X_NUM; x++)
	{
		for (int y = 0; y < TAO_QUAN_NA_JI_Y_NUM; y++)
		{
			int get_point = m_param.play_map[x][y];
			if (get_point != 0)
			{
				return false;
			}
		}
	}
	return true;
}

void TaoQuanNaJi::EndGame()
{
	if (m_param.map_init != 1)return;
	const NaJiOther & other_cfg = LOGIC_CONFIG->GetTaoQuanNaJiConfig()->GetOtherCfg();
	const NaJiReward * reward_cfg = LOGIC_CONFIG->GetTaoQuanNaJiConfig()->GetRewardCfg(m_param.sroce);
	if (reward_cfg != NULL)
	{
		std::vector<ItemConfigData> real_vec;
		if (m_param.flag_title == 0 && m_param.sroce >= other_cfg.title_score)
		{
			real_vec.push_back(ItemConfigData(other_cfg.title_item_id, 1, 1));
			m_param.flag_title = 1;
		}

		if (m_param.today_play_times < other_cfg.reward_time)
		{
			if ((int)reward_cfg->m_reward_vec.size() > 0)
			{
				real_vec.insert(real_vec.begin(), reward_cfg->m_reward_vec.begin(), reward_cfg->m_reward_vec.end());
			}
		}

		if ((int)real_vec.size() > 0)
		{
			if (!m_module_mgr->GetKnapsack()->PutList((int)real_vec.size(), &real_vec[0], PUT_REASON_TAO_QUAN_NA_JI))
			{
				m_module_mgr->GetKnapsack()->SendMail(real_vec);
			}
			m_param.today_play_times += 1;
		}
	}
	this->SendPlayInfo();
	m_param.ResetEndGame();
}

void TaoQuanNaJi::SendPlayInfo()
{
	if (!ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_TAO_QUAN_NA_JI))
	{
		return;
	}
	Protocol::SCTaoQuanNaJiPlayEndInfo info;
	info.score = m_param.sroce;
	info.item_1_count = m_param.item_1_count;
	info.item_2_count = m_param.item_2_count;
	info.item_3_count = m_param.item_3_count;
	info.item_4_count = m_param.item_4_count;
	m_module_mgr->NetSend((const char *)&info, sizeof(info));
}
