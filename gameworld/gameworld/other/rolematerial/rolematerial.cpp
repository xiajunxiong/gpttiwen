#include "rolematerial.hpp"
#include "engineadapter.h"
#include "gamelog.h"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"

#include "scene/scene.h"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/funopen/funopen.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"

#include "protocol/msgmaterial.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/rolematerialconfig.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "global/worldstatus/worldstatus.hpp"


RoleMaterial::RoleMaterial() : m_module_mgr(NULL)
{
	m_level_data_map.clear();
	m_chapter_map.clear();
	memset(max_pass_level_seq, -1, sizeof(max_pass_level_seq));
}

RoleMaterial::~RoleMaterial()
{
}

void RoleMaterial::Init(RoleModuleManager * module_mgr, const MaterialParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;

	//如果热更增加了关卡,并且 其中有些关卡是玩家之前已经通过的,那么玩家需要重新登陆,才能获取到最新的章节积分
	for (int i = 0; i < m_param.level_count && i < MAX_MATERIAL_LEVEL_NUM; i++)
	{
		MaterialLevelNode & data_node = m_param.level_data_list[i];
		if(this->IsInValid(data_node.chapter_id, data_node.level_seq)) continue;

		//之前通关的数据中存在配置中不存在的关卡,也得加入映射,防止后面热更增加回来
		m_level_data_map[MaterialKey(data_node.chapter_id, data_node.level_seq)] = i;
		max_pass_level_seq[data_node.chapter_id] = max_pass_level_seq[data_node.chapter_id] > data_node.level_seq ? max_pass_level_seq[data_node.chapter_id] : data_node.level_seq;

		//配置中不存在的关卡不计算 
		if(NULL == LOGIC_CONFIG->GetMaterialConfig()->GetSceneCfg(data_node.chapter_id, data_node.level_seq)) continue;

		m_chapter_map[data_node.chapter_id] += this->GetLevelIntegralNum(i);
	}
}

void RoleMaterial::GetInitParam(MaterialParam * param)
{
	*param = m_param;
}

void RoleMaterial::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid || !FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_MATERIAL)) return;

	for (int i = 0; i < m_param.level_count && i < MAX_MATERIAL_LEVEL_NUM; i++)
	{
		m_param.level_data_list[i].day_pass_times = 0;
		m_param.level_data_list[i].buy_times = 0;
	}
	m_param.day_pass_times_count = 0;
	this->SendAllInfo();
	this->SendOtherInfo();
}

bool RoleMaterial::IsCanEnterFB(int fb_seq)
{
	const MaterialSceneCfg * scene_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetSceneCfg(fb_seq);
	if (NULL == scene_cfg)
	{
		return false;
	}
	return this->IsCanEnterFB(scene_cfg->chapter_id, scene_cfg->level_seq);
}

bool RoleMaterial::IsCanEnterFB(int chapter_id, int level_seq)
{
	if(this->IsInValid(chapter_id, level_seq)) return false;

	//玩家首次打玉虚境
	if(m_param.max_chapter_id < 0 && 0 == chapter_id && 0 == level_seq) return true;

	//不存在大于2章的情况
	if (chapter_id > m_param.max_chapter_id + 1)
	{
		return false;
	}
	else if (chapter_id == m_param.max_chapter_id + 1)
	{
		int level_num = LOGIC_CONFIG->GetMaterialConfig()->GetChapterLevelNum(m_param.max_chapter_id);
		if (level_num < 0 || 0 != level_seq) return false;

		if (m_param.max_level_seq + 1 < level_num)
		{
			return false;
		}
	}
	else if (chapter_id == m_param.max_chapter_id)
	{
		if (m_param.max_level_seq < level_seq - 1)
		{
			return false;
		}
	}
	else	//添加新关卡的时候,旧的通关章节也需要判断
	{
		if (max_pass_level_seq[chapter_id] < level_seq - 1)
		{
			return false;
		}
	}
	
	return true;
}

bool RoleMaterial::IsCanFastFb(int chapter_id, int level_seq)
{
	if (this->IsInValid(chapter_id, level_seq)) return false;
	MaterialKey level_key(chapter_id, level_seq);
	std::map<MaterialKey, short>::iterator it = m_level_data_map.find(level_key);
	if (it == m_level_data_map.end() || this->IsInValid(it->second))
	{
		return false;
	}
	
	for (int i = 0; i < MAX_MATERIAL_CONDITON_NUM; i++)
	{
		if (!(m_param.level_data_list[it->second].condition_flag & (1 << i)))
		{
			return false;
		}
	}
	
	return true;
}

bool RoleMaterial::IsCanEnterFBByTimes(int chapter_id, int level_seq)
{
	if (this->IsInValid(chapter_id, level_seq)) return false;
	MaterialKey level_key(chapter_id, level_seq);
	std::map<MaterialKey, short>::iterator it = m_level_data_map.find(level_key);
	if (it == m_level_data_map.end())
	{
		return true;	//没打过的关卡肯定有次数
	}
	const MaterialOtherCfg & other_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetOtherCfg();
	if (this->IsInValid(it->second))
	{
		return false;	//有映射,但是该映射值不规范
	}

	if (m_param.level_data_list[it->second].day_pass_times >= other_cfg.pass_time_max + m_param.level_data_list[it->second].buy_times)
	{
		return false;
	}

	return true;
}

int RoleMaterial::GetPassLevelByChapter(int chapter_id)
{
	if(chapter_id < 0 || chapter_id >= MAX_MATERIAL_CHAPTER_NUM) return 0;

	return max_pass_level_seq[chapter_id] + 1;
}

bool RoleMaterial::IsCanTips()
{
	if (m_module_mgr->GetRole()->GetLevel() >= 40 || m_param.tips_times >= 1) return false;

	RoleBattleData battle_data;
	m_module_mgr->CollectRoleBattleData(&battle_data, true);

	bool partner_status1 = false, partner_status2 = false;
	for (int i = 0; i < SIDE_MAX_ROLE_NUM - 1; ++i)
	{
		if (battle_data.partner_character_data[i].battle_data.character_id == 5)
		{
			partner_status1 = true;
		}

		if (battle_data.partner_character_data[i].battle_data.character_id == 22)
		{
			partner_status2 = true;
		}
	}

	if (!partner_status1 && !partner_status2)
	{
		m_module_mgr->NoticeNum(errornum::EN_LACK_AOE_AND_TREAT_PARTNER);
	}
	else if (!partner_status1 && partner_status2)
	{
		m_module_mgr->NoticeNum(errornum::EN_LACK_TREAT_PARTNER);
	}
	else if (partner_status1 && !partner_status2)
	{
		m_module_mgr->NoticeNum(errornum::EN_LACK_AOE_PARTNER);
	}

	m_param.tips_times++;
	
	return true;
}

void RoleMaterial::OnBuyTimes(int fb_seq)
{
	const MaterialSceneCfg * scene_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetSceneCfg(fb_seq);
	if (NULL == scene_cfg)
	{
		return;
	}
	MaterialKey level_key(scene_cfg->chapter_id, scene_cfg->level_seq);
	std::map<MaterialKey, short>::iterator it = m_level_data_map.find(level_key);
	if (it == m_level_data_map.end() || this->IsInValid(it->second))
	{
		m_module_mgr->NoticeNum(errornum::EN_MATERIAL_LEVEL_NOT_PASS_BUY_ERROR);
		return;
	}
	const MaterialOtherCfg & other_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetOtherCfg();
	//int price = LOGIC_CONFIG->GetMaterialConfig()->GetBuyCfg(m_param.level_data_list[it->second].buy_times + 1);
	if (0 >= other_cfg.extra_buy_consume || m_param.level_data_list[it->second].buy_times >= other_cfg.buy_times_max)
	{
		m_module_mgr->NoticeNum(errornum::EN_MATERIAL_LEVEL_BUY_NOT_ENOUGH);
		return;
	}
	if (!m_module_mgr->GetMoney()->UseGold(other_cfg.extra_buy_consume, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}
	int add_times = other_cfg.buy_times_max - m_param.level_data_list[it->second].buy_times;
	m_param.level_data_list[it->second].buy_times = other_cfg.buy_times_max;
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buy_extra_times_notice, add_times);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
	}
	this->SendSingleInfo(it->second);
	gamelog::g_log_role_material.printf(LL_INFO, "%s user[%d, %s] chapter_id[%d] level_seq[%d] level_count[%d] day_pass_times[%d] buy_times[%d] day_pass_times_count[%d]", __FUNCTION__, 
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), scene_cfg->chapter_id, scene_cfg->level_seq, m_param.level_count, m_param.level_data_list[it->second].day_pass_times, 
		m_param.level_data_list[it->second].buy_times, m_param.day_pass_times_count);
}

void RoleMaterial::SendPassReward(int chapter_id, int level_seq, unsigned char condition_flag)
{
	const MaterialSceneCfg * scene_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetSceneCfg(chapter_id, level_seq);
	if (NULL == scene_cfg)
	{
		return;
	}
	if (!this->IsCanEnterFB(chapter_id, level_seq))
	{
		return ;
	}

	bool is_first_kill = true;
	bool is_send_other_info = false;
	short index = -1;
	MaterialKey level_key(chapter_id, level_seq);
	std::map<MaterialKey, short>::iterator it = m_level_data_map.find(level_key);
	if (it == m_level_data_map.end())
	{
		if (m_param.level_count < 0 || m_param.level_count >= MAX_MATERIAL_LEVEL_NUM)
		{
			gamelog::g_log_role_material.printf(LL_WARNING, "%s user[%d, %s] chapter_id[%d] level_seq[%d] condition_flag[%u] level_count[%d] error!", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
				chapter_id, level_seq, condition_flag, m_param.level_count);
			printf("%s level_count error!", __FUNCTION__);
			return;
		}
		MaterialLevelNode node;
		node.chapter_id = level_key.chapter_id;
		node.level_seq = level_key.level_seq;
		index = m_param.level_count;
		m_param.level_data_list[m_param.level_count++] = node;
		m_level_data_map[level_key] = index;

		if (m_param.max_chapter_id < chapter_id)
		{
			m_param.max_chapter_id = chapter_id;
			m_param.max_level_seq = level_seq;
			is_send_other_info = true;
		}
		else if (m_param.max_chapter_id == chapter_id)
		{
			if (m_param.max_level_seq < level_seq)
			{
				m_param.max_level_seq = level_seq;
				is_send_other_info = true;
			}
		}

		max_pass_level_seq[chapter_id] = (max_pass_level_seq[chapter_id] >= level_seq) ? max_pass_level_seq[chapter_id] : level_seq;
	}
	else
	{
		if (this->IsInValid(it->second)) return;
		if (m_param.level_data_list[it->second].condition_flag & (1 << MATERIAL_FIRST_PASS_BIT_NUM))
		{
			is_first_kill = false;
		}
		index = it->second;
	}

	std::vector<ItemConfigData> reward_list;
	if (is_first_kill)
	{
		if (!scene_cfg->first_reward_list.empty())
		{
			reward_list.assign(scene_cfg->first_reward_list.begin(), scene_cfg->first_reward_list.end());
		}
		m_param.level_data_list[index].condition_flag |= (1 << MATERIAL_FIRST_PASS_BIT_NUM);
	}

	std::vector<ItemConfigData> common_reward_list;
	LOGIC_CONFIG->GetMaterialConfig()->GetPassReward(scene_cfg->common_reward_group_list, &common_reward_list);
	if (!common_reward_list.empty())
	{
		reward_list.insert(reward_list.end(), common_reward_list.begin(), common_reward_list.end());
	}
	if (!reward_list.empty())
	{
		if (!m_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_MATERIAL))
		{
			m_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
		reward_list.clear();
	}
	//即使是空表,也要下发,展示经验
	m_module_mgr->GetKnapsack()->NotifyGetItemList((short)common_reward_list.size(), common_reward_list.empty() ? NULL : &common_reward_list[0], GET_REASON_MATERIAL_FAST_PASS, 1);	//param1:1表示通关 列表下发普通奖励
	if (condition_flag > 0)
	{
		int add_integral_num = 0;
		for (int i = 0; i < MAX_MATERIAL_COND_COUNT_NUM; i++)
		{
			if (i < MAX_MATERIAL_CONDITON_NUM)
			{
				if ((m_param.level_data_list[index].condition_flag & (1 << i)) || (!(condition_flag & (1 << i))))
				{
					continue;
				}
				m_param.level_data_list[index].condition_flag |= (1 << i);
				add_integral_num++;
			}
			else
			{
				break;
				//策划说去除成就奖励
				//int idx = i - MAX_MATERIAL_CONDITON_NUM;
				//if ((m_param.level_data_list[index].achievenment_condition_flag & (1 << idx)) || (!(condition_flag & (1 << idx))))
				//{
				//	continue;
				//}
				//m_param.level_data_list[index].achievenment_condition_flag |= (1 << idx);
				//reward_list.push_back(scene_cfg->achievement_reward[idx]);
			}
		}
		m_chapter_map[chapter_id] += add_integral_num;
	}
	/*if (!reward_list.empty())
	{
		if (!m_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_MATERIAL_ACHIEVEMENT_REWARD))
		{
			m_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
	}*/
	
	//int add_exp = LOGIC_CONFIG->GetMaterialConfig()->GetExpCfg(m_module_mgr->GetRole()->GetLevel());
	const MaterialOtherCfg & other_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetOtherCfg();
	int add_exp = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetExpCfg(m_module_mgr->GetRole()->GetLevel()) * other_cfg.use_power;
	if (add_exp > 0)
	{
		long long real_add_exp = m_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_MATERIAL, __FUNCTION__);
		m_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
		EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_module_mgr->GetRole(), add_exp, __FUNCTION__);
	}
	m_module_mgr->GetRoleCrystalFb()->UseCrystalFbEnergy(other_cfg.use_power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_MATERIAL);
	m_param.level_data_list[index].day_pass_times++;
	m_param.day_pass_times_count++;
	this->SendSingleInfo(index);
	if (is_send_other_info)
	{
		this->SendOtherInfo();
	}
	EventHandler::Instance().OnPassMaterial(m_module_mgr->GetRole(), chapter_id, level_seq, is_first_kill);
	gamelog::g_log_role_material.printf(LL_INFO, "%s user[%d, %s] chapter_id[%d] level_seq[%d] condition_flag[%u] level_count[%d] day_pass_times[%d] day_pass_times_count[%d]", __FUNCTION__, 
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), chapter_id, level_seq, condition_flag, m_param.level_count, m_param.level_data_list[index].day_pass_times, 
		m_param.day_pass_times_count);
}

void RoleMaterial::OnFastPassFb(int fb_seq)
{
	if (!Scene::IsStaticScene(m_module_mgr->GetRole()->GetScene()->GetSceneType()))
	{
		m_module_mgr->NoticeNum(errornum::EN_SCENE_NOT_CAN_FAST_PASS_FB);
		return;
	}

	const MaterialOtherCfg & other_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetOtherCfg();
	if (m_module_mgr->GetRoleCrystalFb()->GetCrystalFbEnergy() < other_cfg.use_power)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_CRYSTAL_FB_ENERGY_NOT_ENOUGH);
		return;
	}

	const MaterialSceneCfg * scene_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetSceneCfg(fb_seq);
	if (NULL == scene_cfg)
	{
		return;
	}

	if (!this->IsCanFastFb(scene_cfg->chapter_id, scene_cfg->level_seq))
	{
		m_module_mgr->NoticeNum(errornum::EN_MATERIAL_CONDITION_NOT_ALL_FINISH);
		return;
	}
	std::map<MaterialKey, short>::iterator it = m_level_data_map.find(MaterialKey(scene_cfg->chapter_id, scene_cfg->level_seq));
	if (it == m_level_data_map.end() || this->IsInValid(it->second))
	{
		m_module_mgr->NoticeNum(errornum::EN_MATERIAL_CONDITION_NOT_ALL_FINISH);
		return;
	}

	if (m_param.level_data_list[it->second].day_pass_times >= other_cfg.pass_time_max + m_param.level_data_list[it->second].buy_times)
	{
		m_module_mgr->NoticeNum(errornum::EN_MATERIAL_PASS_TIME_NOT_ENOUGH);
		return;
	}

	std::vector<ItemConfigData> reward_list;
	LOGIC_CONFIG->GetMaterialConfig()->GetPassReward(scene_cfg->common_reward_group_list, &reward_list);
	if (!reward_list.empty())
	{
		if (!m_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_MATERIAL))
		{
			m_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
	}
	m_module_mgr->GetKnapsack()->NotifyGetItemList((short)reward_list.size(), reward_list.empty() ? NULL : &reward_list[0], GET_REASON_MATERIAL_FAST_PASS, 0);

	//int add_exp = LOGIC_CONFIG->GetMaterialConfig()->GetExpCfg(m_module_mgr->GetRole()->GetLevel());
	int add_exp = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetExpCfg(m_module_mgr->GetRole()->GetLevel()) * other_cfg.use_power;
	if (add_exp > 0)
	{
		long long real_add_exp = m_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_MATERIAL, __FUNCTION__);
		m_module_mgr->GetLevelComplement()->OnAddExp(real_add_exp);
		EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_module_mgr->GetRole(), add_exp, __FUNCTION__);
	}
	m_module_mgr->GetRoleCrystalFb()->UseCrystalFbEnergy(other_cfg.use_power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_MATERIAL);
	m_param.level_data_list[it->second].day_pass_times++;
	m_param.day_pass_times_count++;
	this->SendOtherInfo();
	this->SendSingleInfo(it->second);
	EventHandler::Instance().OnPassMaterial(m_module_mgr->GetRole(), scene_cfg->chapter_id, scene_cfg->level_seq, false);
	gamelog::g_log_role_material.printf(LL_INFO, "%s user[%d, %s] chapter_id[%d] level_seq[%d] level_count[%d] day_pass_times[%d] day_pass_times_count[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(),
		m_module_mgr->GetRole()->GetName(), scene_cfg->chapter_id, scene_cfg->level_seq, m_param.level_count, m_param.level_data_list[it->second].day_pass_times, m_param.day_pass_times_count);

	WorldStatus::Instance().LogFunctionStats(m_module_mgr->GetRole(), "RoleMaterial");
}

void RoleMaterial::OnFetchBoxReward(int seq)
{
	const MaterialBoxRewardCfg * box_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetBoxRewardCfg(seq);
	if (NULL == box_cfg)
	{
		return;
	}
	if (m_param.box_flag.IsBitSet(seq))
	{
		m_module_mgr->NoticeNum(errornum::EN_MATERIAL_BOX_REAWRD_FETCHED);
		return;
	}

	std::map<int, int>::iterator it = m_chapter_map.find(box_cfg->chapter_id);
	if (it == m_chapter_map.end() || it->second < box_cfg->max_score)
	{
		m_module_mgr->NoticeNum(errornum::EN_MATERIAL_INTEGRAL_NOT_ENOUGH);
		return;
	}
	if (!box_cfg->reward_list.empty())
	{
		if (!m_module_mgr->GetKnapsack()->PutList((short)box_cfg->reward_list.size(), &box_cfg->reward_list[0], PUT_REASON_MATERIAL))
		{
			m_module_mgr->GetKnapsack()->SendMail(&box_cfg->reward_list[0], (int)box_cfg->reward_list.size(), SNED_MAIL_TYPE_DEFAULT, true);
		}
	}
	m_module_mgr->GetMoney()->AddCoinBind(box_cfg->coin_reawrd, __FUNCTION__);

	m_param.box_flag.SetBit(seq);
	this->SendOtherInfo();
	gamelog::g_log_role_material.printf(LL_INFO, "%s user[%d, %s] chapter_id[%d] level_count[%d] day_pass_times_count[%d] box_seq[%d]", __FUNCTION__, m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),
		box_cfg->chapter_id, m_param.level_count, m_param.day_pass_times_count, seq);
}

void RoleMaterial::SendAllInfo()
{
	Protocol::SCMaterialAllInfo info;
	info.level_count = m_param.level_count;
	info.chapter_count = 0;
	for (int i = 0; i < info.level_count && i < MAX_MATERIAL_LEVEL_NUM; i++)
	{
		info.level_data_list[i] = m_param.level_data_list[i];
	}
	for (std::map<int, int>::iterator it = m_chapter_map.begin(); it != m_chapter_map.end() && info.chapter_count < MAX_MATERIAL_CHAPTER_NUM; it++)
	{
		info.intergral_list[info.chapter_count].chapter_id = it->first;
		info.intergral_list[info.chapter_count++].integral_num = it->second;
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

void RoleMaterial::SendSingleInfo(int level_data_index)
{
	if(this->IsInValid(level_data_index)) return;

	Protocol::SCMaterialSingleInfo info;
	info.reserve_sh = 0;
	info.integral_count = 0;
	info.data = m_param.level_data_list[level_data_index];

	std::map<int, int>::iterator it = m_chapter_map.find(info.data.chapter_id);
	if (it != m_chapter_map.end())
	{
		info.integral_count = it->second;
	}

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleMaterial::SendOtherInfo()
{
	Protocol::SCMaterialOtherInfo info;
	info.max_chapter_id = m_param.max_chapter_id;
	info.max_level_seq = m_param.max_level_seq;
	info.day_pass_time = m_param.day_pass_times_count;
	info.box_flag = m_param.box_flag;
	UNSTD_STATIC_CHECK(sizeof(info.box_flag) == sizeof(m_param.box_flag));

	m_module_mgr->NetSend(&info, sizeof(info));
}

bool RoleMaterial::IsInValid(int level_data_seq)
{
	return level_data_seq < 0 || level_data_seq >= MAX_MATERIAL_LEVEL_NUM || level_data_seq >= m_param.level_count;
}

bool RoleMaterial::IsInValid(int chapter_id, int level_seq)
{
	return chapter_id < 0 || chapter_id >= MAX_MATERIAL_CHAPTER_NUM || level_seq < 0 || level_seq >= MAX_MATERIAL_CHECKPOINT_NUM;
}

int RoleMaterial::GetLevelIntegralNum(int level_data_seq)
{
	if(this->IsInValid(level_data_seq)) return 0;

	int integral_num = 0;
	for (int i = 0; i < MAX_MATERIAL_CONDITON_NUM; i++)
	{
		if (m_param.level_data_list[level_data_seq].condition_flag & (1 << i))
		{
			integral_num++;
		}
	}

	return integral_num;
}
