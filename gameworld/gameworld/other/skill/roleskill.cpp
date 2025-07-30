#include "roleskill.hpp"
#include "gamelog.h"
#include "skill/skillpool.hpp"
#include "servercommon/struct/structcommon.h"
#include "servercommon/string/gameworldstr.h"
#include "protocol/msgskill.h"
#include "engineadapter.h"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "other/route/mailroute.hpp"
#include "servercommon/errornum.h"
#include "item/money.h"
#include "item/itemextern.hpp"
#include "item/knapsack.h"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "config/otherconfig/professionconfig.hpp"
#include "item/itempool.h"
#include "equipment/equipment.h"
#include "other/event/eventhandler.hpp"
#include "other/skill/roleskill.hpp"
#include "other/funopen/funopen.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/workshop/workshopconfig.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "other/advancecareer/advancecareer.hpp"

RoleSkill::RoleSkill() : m_module_mgr(NULL), m_is_change(false)
{
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_old_state, 0, sizeof(m_old_state));
}

RoleSkill::~RoleSkill()
{

}

void RoleSkill::Init(RoleModuleManager* module_mgr, const SkillListParam& skill_list, const SkillOtherParam& other_param)
{
	m_module_mgr = module_mgr;

	int count = GetMin(skill_list.count, MAX_SKILL_NUM);

	for (int i = 0; i < count; ++i)
	{
		int index = skill_list.skill_list[i].index;
		if (index < 0 || index >= MAX_SKILL_NUM)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[RoleSkill::Init] [skill index is invalid:%d]", index);
			continue;
		}

		if (!m_skill_list[index].Invalid())
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[SkillManager::Init] [skill index duplicate:%d]", index);
			continue;
		}

		const SkillCfg* skill = SkillPool::Instance().GetSkillCfg(skill_list.skill_list[i].skill_id);
		const GamePassiveSkillCfg* passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_list.skill_list[i].skill_id);
		if (NULL == skill && NULL == passive_skill)
		{
			// 若主动和被动技能都找不到，就记个log
			gamelog::g_log_serious_error.printf(LL_WARNING, "[SkillManager::Init] skill id not found, skill_id[%d] skill_level[%d] skill_exp[%d]", 
				skill_list.skill_list[i].skill_id, skill_list.skill_list[i].skill_level, skill_list.skill_list[i].skill_exp);
			continue;
		}

		m_skill_list[index].index = index;
		m_skill_list[index].skill_id = skill_list.skill_list[i].skill_id;
		m_skill_list[index].skill_level = skill_list.skill_list[i].skill_level;
		m_skill_list[index].skill_exp = skill_list.skill_list[i].skill_exp;

		m_old_state[index] = true;

		// 每次上线重新获取skill_show_level
		int prof_max_skill_level = LOGIC_CONFIG->GetProfessionConfig()->GetMaxLearnSkillLevel(m_module_mgr->GetRole(), m_skill_list[index].skill_id);
		m_skill_list[index].skill_show_level = GetMin(prof_max_skill_level, (int)m_skill_list[index].skill_level);
	}

	m_other_param = other_param;

	if (0 == m_other_param.common_skill_reset_flag)
	{
		m_other_param.common_skill_reset_flag = 1;
		this->ResetCommonUseSkill();
	}
}

void RoleSkill::OnInitEnd()
{
	this->GetSkillPosition();
}

void RoleSkill::GetSkillOtherParam(SkillOtherParam* out_other_param)
{
	if (NULL != out_other_param)
	{
		*out_other_param = m_other_param;
	}
}

void RoleSkill::GetSkillList(SkillListParam* p)
{
	p->count = 0;

	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;

		p->skill_list[p->count].change_state = structcommon::CS_NONE;
		p->skill_list[p->count].index = i;

		p->skill_list[p->count].skill_level = m_skill_list[i].skill_level;
		p->skill_list[p->count].skill_id = m_skill_list[i].skill_id;
		p->skill_list[p->count].skill_exp = m_skill_list[i].skill_exp;

		++p->count;
	}
}

void RoleSkill::GetChangeSkillList(SkillListParam* p)
{
	if (!m_is_change)
	{
		p->count = 0;
		return;
	}

	int count = 0;
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_dirty_mark[i])
		{
			if (m_skill_list[i].Invalid())
			{
				if (!m_old_state[i]) continue;

				p->skill_list[count].change_state = structcommon::CS_DELETE;
				p->skill_list[count].index = i;
			}
			else
			{
				p->skill_list[count].change_state = m_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				p->skill_list[count].index = i;

				p->skill_list[count].skill_id = m_skill_list[i].skill_id;
				p->skill_list[count].skill_level = m_skill_list[i].skill_level;
				p->skill_list[count].skill_exp = m_skill_list[i].skill_exp;
			}

			++count;
		}
	}

	p->count = count;
}

RoleSkill::SkillItem* RoleSkill::GetSkill(short skill_index)
{
	if (skill_index < 0 || skill_index >= MAX_SKILL_NUM)
	{
		return NULL;
	}

	return &m_skill_list[skill_index];
}

RoleSkill::SkillItem* RoleSkill::GetSkillBySkillId(UInt16 skill_id)
{
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;

		if (m_skill_list[i].skill_id == skill_id) return &m_skill_list[i];
	}

	return NULL;
}

RoleSkill::SkillItem * RoleSkill::GetRandSkill()
{
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;

		return &m_skill_list[i];
	}
	return NULL;
}

void RoleSkill::ClearDirtyMark()
{
	if (!m_is_change) return;

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));

	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		m_old_state[i] = !m_skill_list[i].Invalid();
	}

	m_is_change = false;
}

void RoleSkill::OnRoleLogin()
{
	//修改后不要的技能给予返还
	this->CheckOldSkillReturn();

	//返还技能升级消耗
	this->CheckSkillUplevelConsumeCoinReturn();
	
	//检测已有技能是否符合该职业
	this->CheckSkillConformProf();

	{
		//初始化结束之后检测玩家职业和职业技能是否满足初始化
		int play_skill_num = this->TheNumOfLevelSkill(0);
		if (play_skill_num == 0)//一个技能都没有的检查角色初始职业技能
		{
			this->GiveInitialSkillOnRoleCreate();
		}
		else
		{
			this->CheckAnyInitialSkillMissing();
		}
	}
}

void RoleSkill::OnRoleOperateReq(int oper_type, int param1, int param2, int param3)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ROLE_SKILL_UPLEVEL, true))
	{
		return;
	}

	switch (oper_type)
	{
	case Protocol::CSSkillOperateReq::OPERATE_TYPE_GET_LIST:
		{
			this->SendSkillList();
			this->SendLifeSkillInfo();
		}
		break;

	case Protocol::CSSkillOperateReq::OPERATE_TYPE_LEARN_SKILL:
		{
			this->LearnSkill(param1);
		}
		break;

	//case Protocol::CSSkillOperateReq::OPERATE_TYPE_SET_COMMON_USE_SKILL:
	//	{
	//		this->SetCommonUseSkill(param1, 1 == param2);
	//	}
	//	break;
	case Protocol::CSSkillOperateReq::OPERATE_TYPE_SKILL_FAST_UP:
		{
			//this->LearnSkillFast(param1, param2);
		}
		break;

	case Protocol::CSSkillOperateReq::OPERATE_TYPE_SKILL_UPLEVEL:
		{
			if (param2 > 0)
			{
				this->UplevelAllSkill();
			}
			else
			{
				this->UplevelSkill(param1);
			}
		}
		break;

	default:
		break;
	}
}

void RoleSkill::OnRoleModifySkillPositionReq(int skill_num, short* skill_idx_list)
{
	short tmp_skill_num = 0;
	for (int i = 0; i < skill_num && tmp_skill_num < ARRAY_ITEM_COUNT(m_other_param.common_use_skill_idx_list); ++i)
	{
		if (NULL == this->GetSkill(skill_idx_list[i])) continue;

		m_other_param.common_use_skill_idx_list[tmp_skill_num++] = skill_idx_list[i];
	}
	m_other_param.common_use_skill_num = tmp_skill_num;

	this->SendCommonUseSkillChangeNotice();
}

int RoleSkill::GetMaxSkillLevel()
{	
	int max_skill_level = 0;
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;
			
		const GamePassiveSkillCfg* cfg = SkillPool::Instance().GetPassiveSkillCfg(m_skill_list[i].skill_id);
		if (NULL != cfg) continue;

		if (max_skill_level < m_skill_list[i].skill_show_level)
		{
			max_skill_level = m_skill_list[i].skill_show_level;
		}
	}
	return max_skill_level;
}

void RoleSkill::SendSkillList()
{
	static Protocol::SCSkillList msg;
	msg.common_use_skill_num = 0;
	msg.skill_count = 0;

	for (int i = 0; i < MAX_SKILL_NUM && msg.skill_count < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;

		const GamePassiveSkillCfg* cfg = SkillPool::Instance().GetPassiveSkillCfg(m_skill_list[i].skill_id);
		if (NULL != cfg)
		{
			continue; // 被动技能不下发给客户端看
		}

		msg.skill_list[msg.skill_count].index = i;
		msg.skill_list[msg.skill_count].skill_id = m_skill_list[i].skill_id;
		msg.skill_list[msg.skill_count].skill_level = m_skill_list[i].skill_show_level;
		msg.skill_list[msg.skill_count].reserve_sh = 0;
		msg.skill_list[msg.skill_count].skill_exp = m_skill_list[i].skill_exp;
		msg.skill_count += 1;
	}

	msg.common_use_skill_num = 0;
	for (int i = 0; i < m_other_param.common_use_skill_num && i < MAX_SKILL_NUM; ++i)
	{
		msg.common_use_skill_index_list[msg.common_use_skill_num++] = m_other_param.common_use_skill_idx_list[i];
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &msg);
}

void RoleSkill::SendSingleSkillInfo(short skill_index)
{
	SkillItem* skill_item = this->GetSkill(skill_index);
	if (NULL == skill_item || skill_item->Invalid()) return;

	const GamePassiveSkillCfg* cfg = SkillPool::Instance().GetPassiveSkillCfg(skill_item->skill_id);
	if (NULL != cfg)
	{
		return; // 被动技能不下发给客户端看
	}

	const SkillCfg* skill_cfg = SkillPool::Instance().GetSkillCfg(skill_item->skill_id);
	if (NULL == skill_cfg) return;

	static Protocol::SCSkillChangeNotice notice;
	notice.skill.index = skill_index;
	notice.skill.skill_id = skill_item->skill_id;
	notice.skill.skill_level = skill_item->skill_show_level;
	notice.skill.skill_exp = skill_item->skill_exp;

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&notice, sizeof(notice));
}

void RoleSkill::SendLifeSkillInfo(short life_skill_type /* = -1 */, short reason_type /* = 0 */)
{
	/*if (life_skill_type != -1 && (life_skill_type < LIFE_SKILL_TYPE_MIN || life_skill_type >= LIFE_SKILL_TYPE_MAX))
	{
		return;
	}

	UNSTD_STATIC_CHECK(0 == Protocol::SCLifeSkillInfo::REASON_TYPE_NORMAL);

	static Protocol::SCLifeSkillInfo msg;
	msg.count = 0;
	msg.reason_type = reason_type;

	if (-1 == life_skill_type)
	{
		for (int i = LIFE_SKILL_TYPE_MIN; i < LIFE_SKILL_TYPE_MAX; ++i)
		{
			const LifeSkillCfg * cfg_life_skill = SkillPool::Instance().GetLifeSkillCfg(i);
			if (NULL == cfg_life_skill) continue;

			Protocol::SkillContainer & ptc_con = msg.life_skill_list[msg.count++];
			const LifeSkillInfo & skill_data = m_other_param.life_skill_list[i];
			
			ptc_con.skill_id = cfg_life_skill->life_skill_id;
			ptc_con.skill_data.skill_level = skill_data.skill_level;
			ptc_con.skill_data.skill_exp = skill_data.skill_exp;
		}
	}
	else
	{
		UNSTD_STATIC_CHECK(LIFE_SKILL_TYPE_MAX - LIFE_SKILL_TYPE_MIN > 0);
		const LifeSkillCfg * cfg_life_skill = SkillPool::Instance().GetLifeSkillCfg(life_skill_type);
		if (NULL == cfg_life_skill) return;

		Protocol::SkillContainer & ptc_con = msg.life_skill_list[msg.count++];
		const LifeSkillInfo & skill_data = m_other_param.life_skill_list[life_skill_type];

		ptc_con.skill_id = cfg_life_skill->life_skill_id;
		ptc_con.skill_data.skill_level = skill_data.skill_level;
		ptc_con.skill_data.skill_exp = skill_data.skill_exp;
	}

	static const int LIST_SIZE = ARRAY_ITEM_COUNT(msg.life_skill_list);
	int send_len = sizeof(msg) - (LIST_SIZE - msg.count) * sizeof(msg.life_skill_list[0]);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&msg, send_len);*/
}

void RoleSkill::SendCommonUseSkillChangeNotice()
{
	static Protocol::SCCommonUseSkillChangeNotice msg;
	msg.common_use_skill_num = 0;

	for (int i = 0; i < m_other_param.common_use_skill_num && i < MAX_SKILL_NUM && msg.common_use_skill_num < MAX_SKILL_NUM; ++i)
	{
		msg.common_use_skill_idx_list[msg.common_use_skill_num++] = m_other_param.common_use_skill_idx_list[i];
	}

	int send_len = sizeof(msg) - (MAX_SKILL_NUM - msg.common_use_skill_num) * sizeof(msg.common_use_skill_idx_list[0]);
	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&msg, send_len);
}

bool RoleSkill::LearnSkill(UInt16 skill_id)
{
	const SkillCfg* skill_cfg = SkillPool::Instance().GetSkillCfg(skill_id);
	if (NULL == skill_cfg)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (!LOGIC_CONFIG->GetProfessionConfig()->CanLearnSkill(m_module_mgr->GetRole(), skill_id))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_CANNOT_LEARN);
		return false;
	}

	SkillItem* skill_item = this->GetSkillBySkillId(skill_id);
	if (NULL != skill_item)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_HAS_LEARNED);
		return false;
	}
	
	int skill_index = 0;
	if (!this->AddSkill(skill_id, &skill_index))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_FULL);
		return false;
	}

	skill_item = this->GetSkill(skill_index);
	if (NULL == skill_item)
	{
		// 不该出现这种情况， 如出现则是代码有BUG！
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		assert(0);
		return false;
	}
	
	EventHandler::Instance().OnLearnSkill(m_module_mgr->GetRole(), skill_id);
	return true;
}

bool RoleSkill::LearnSkillFast(UInt16 skill_id, int target_level)
{
	const SkillCfg* skill_cfg = SkillPool::Instance().GetSkillCfg(skill_id);
	if (NULL == skill_cfg)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (!LOGIC_CONFIG->GetProfessionConfig()->CanLearnSkill(m_module_mgr->GetRole(), skill_id))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_CANNOT_LEARN);
		return false;
	}

	SkillItem* skill_item = this->GetSkillBySkillId(skill_id);
	if (NULL == skill_item)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_NOT_LEARN);
		return false;
	}
	int old_level = skill_item->skill_level;
	int old_exp = skill_item->skill_exp;


	int prof_max_skill_level = LOGIC_CONFIG->GetProfessionConfig()->GetMaxLearnSkillLevel(m_module_mgr->GetRole(), skill_id);
	int skill_level_limit = skill_cfg->GetMaxLevelForRoleLevel(m_module_mgr->GetRole()->GetLevel());
	if (old_level >= target_level)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (prof_max_skill_level < target_level)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_LEVEL_LIMIT);
		return false;
	}

	if (skill_level_limit < target_level)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return false;
	}

	long long cost_coin = 0;
	long long add_exp = 0;

	int new_level = old_level;
	int new_exp = skill_item->skill_exp;

	for (int temp_level = new_level + 1; 
		temp_level <= prof_max_skill_level 
		&& temp_level <= skill_level_limit
		&& temp_level <= target_level;
		temp_level++)
	{
		long long need_exp = skill_cfg->GetUplevelNeedExp(temp_level) - new_exp;
		int per_up_add = skill_cfg->GetAddExp(temp_level);

		int up_times = (need_exp - 1) / per_up_add + 1;

		new_level++;
		new_exp = per_up_add - need_exp % per_up_add;

		add_exp += need_exp + new_exp;
		cost_coin += skill_cfg->GetBindCoinConsume(temp_level) * up_times;
	}

	if (add_exp > MAX_INT)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (!m_module_mgr->GetMoney()->UseAllCoin(cost_coin, __FUNCTION__))
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return false;
	}

	this->AddSkillExp(skill_item, add_exp, skill_cfg, &prof_max_skill_level);
	EventHandler::Instance().OnLearnSkill(m_module_mgr->GetRole(), skill_id);

	return true;
}

bool RoleSkill::UplevelSkill(UInt16 skill_id, bool * is_coin_not_enough /* = NULL*/, bool is_uplevel_all /* = false */)
{
	const SkillCfg* skill_cfg = SkillPool::Instance().GetSkillCfg(skill_id);
	if (NULL == skill_cfg)
	{
		if(!is_uplevel_all) m_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	SkillItem* skill_item = this->GetSkillBySkillId(skill_id);
	if (NULL == skill_item || skill_item->Invalid())
	{
		if(!is_uplevel_all) m_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_NOT_LEARN);
		return false;
	}

	int can_learn_level = GetMax(1, this->GetSkillLevel());
	if (skill_item->skill_level >= can_learn_level)
	{
		if(!is_uplevel_all) m_module_mgr->NoticeNum(errornum::EN_SKILL_LEVEL_LIMIT);
		return true;
	}

	bool is_coin_not_enough_flag = false;
	int new_level = skill_item->skill_level;
	for (int i = skill_item->skill_level + 1; i <= can_learn_level; ++i)
	{
		int need_coin_num = skill_cfg->GetCoinConsume(i);
		if (!m_module_mgr->GetMoney()->CoinBindMoreThan(need_coin_num))
		{
			is_coin_not_enough_flag = true;
			break;
		}

		if (!m_module_mgr->GetMoney()->UseCoinBind(need_coin_num, __FUNCTION__))
		{
			is_coin_not_enough_flag = true;
			break;
		}

		new_level = i;
	}

	int old_level = skill_item->skill_level;
	if (old_level == new_level)
	{
		if (NULL != is_coin_not_enough)
		{
			*is_coin_not_enough = is_coin_not_enough_flag;
		}
		if(!is_uplevel_all) m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return false;
	}

	skill_item->skill_level = new_level;
	skill_item->skill_show_level = skill_item->skill_level;
	this->SetDirty(skill_item->index);

	EventHandler::Instance().OnSkillLevelUp(m_module_mgr->GetRole(), skill_item->skill_id, old_level, skill_item->skill_level);
	
	if (!is_uplevel_all)
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ROLE_SKILL, __FUNCTION__); // 加技能时要重算属性
	}
	if (is_coin_not_enough_flag)
	{
		if (NULL != is_coin_not_enough)
		{
			*is_coin_not_enough = is_coin_not_enough_flag;
		}
		if(!is_uplevel_all) m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return false;
	}
	return true;
}

bool RoleSkill::UplevelAllSkill()
{
	std::vector<int> skill_list;
	for (int i = 0; i < m_other_param.common_use_skill_num && i < MAX_SKILL_NUM; ++i)
	{
		RoleSkill::SkillItem* skill_item = this->GetSkill(m_other_param.common_use_skill_idx_list[i]);
		if (NULL == skill_item) continue;

		const GamePassiveSkillCfg* passive_cfg = SkillPool::Instance().GetPassiveSkillCfg(skill_item->skill_id);
		if (NULL != passive_cfg) continue;  // 被动技能不用升级

		skill_list.push_back(skill_item->skill_id);
	}

	//按照顺序升级,如前序因某些条件无法继续升级,后序也需跑一遍
	bool is_coin_not_enough = false;
	for (size_t i = 0; i < skill_list.size(); ++i)
	{
		this->UplevelSkill(skill_list[i], &is_coin_not_enough, true);
	}

	if (is_coin_not_enough)
	{
		m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
	}

	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ROLE_SKILL, __FUNCTION__); // 加技能时要重算属性

	return true;
}

bool RoleSkill::SetCommonUseSkill(short skill_idx, bool is_unset)
{
	SkillItem* skill_item = this->GetSkill(skill_idx);
	if (NULL == skill_item)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_NOT_LEARN);
		return false;
	}

	if (is_unset)
	{
		bool is_found = false;
		short list_idx = -1;
		for (int i = 0; i < m_other_param.common_use_skill_num && i < MAX_SKILL_NUM; ++i)
		{
			if (m_other_param.common_use_skill_idx_list[i] == skill_idx)
			{
				is_found = true;
				list_idx = i;
				break;
			}
		}

		if (is_found)
		{
			for (int i = list_idx + 1; i < m_other_param.common_use_skill_num; ++i)
			{
				m_other_param.common_use_skill_idx_list[i - 1] = m_other_param.common_use_skill_idx_list[i];
			}
			m_other_param.common_use_skill_num -= 1;
		}
	}
	else
	{
		if (m_other_param.common_use_skill_num >= MAX_SKILL_NUM)
		{
			return false;
		}

		bool is_found = false;
		for (int i = 0; i < m_other_param.common_use_skill_num && i < MAX_SKILL_NUM; ++i)
		{
			if (m_other_param.common_use_skill_idx_list[i] == skill_idx)
			{
				is_found = true;
				break;
			}
		}
		if (is_found)
		{
			return false;
		}

		m_other_param.common_use_skill_idx_list[m_other_param.common_use_skill_num++] = skill_idx;
		//for (int i = m_other_param.common_use_skill_num - 1; i >= 0; --i)
		//{
		//	m_other_param.common_use_skill_idx_list[i + 1] = m_other_param.common_use_skill_idx_list[i];
		//}
		//m_other_param.common_use_skill_idx_list[0] = skill_idx;
		//m_other_param.common_use_skill_num += 1;
	}

	this->SendCommonUseSkillChangeNotice();

	return true;
}

void RoleSkill::ResetCommonUseSkill()
{
	int common_count = 0;
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;

		if (m_skill_list[i].skill_show_level <= 0) continue;

		m_other_param.common_use_skill_idx_list[common_count++] = i;
	}
	m_other_param.common_use_skill_num = common_count;
}

void RoleSkill::OnChangeProfession(int old_prof, int new_prof)
{	
	// 转其他职业，则删掉旧的技能
	if (GetBaseProf(old_prof) != GetBaseProf(new_prof))
	{
		this->RemoveSkillByProfession(old_prof);
	}

	std::vector<int> unlock_skill_list = LOGIC_CONFIG->GetProfessionConfig()->GetUnlockSkillList(GetBaseProf(new_prof), m_module_mgr->GetRole()->GetLevel());
	for (int i = 0; i < unlock_skill_list.size(); i++)
	{
		if (NULL == this->GetSkillBySkillId(unlock_skill_list[i]))
		{
			this->GiveInitialSkill(unlock_skill_list[i]);
		}
	}

	//ps.现在升级技能需要消耗铜币
	// 转去其他职业时，自动升满技能
	/*if (GetBaseProf(old_prof) != GetBaseProf(new_prof))
	{
		this->CheckSkillLevel();
	}*/
	
	// 转去其他职业时，重新调整技能排序
	if (GetBaseProf(old_prof) != GetBaseProf(new_prof))
	{
		this->ResetCommonUseSkill();
	}

	this->SendCommonUseSkillChangeNotice();
	this->SendSkillList();
}

const LifeSkillInfo * RoleSkill::GetLifeSkillInfoByLifeSkillType(int life_skill_type)
{
	if (life_skill_type < LIFE_SKILL_TYPE_MIN || life_skill_type >= LIFE_SKILL_TYPE_MAX) return NULL;
	
	const LifeSkillCfg * cfg = SkillPool::Instance().GetLifeSkillCfg(life_skill_type);
	if (NULL == cfg) return NULL;

	return & m_other_param.life_skill_list[life_skill_type];
}

void RoleSkill::OnDig()
{
	//this->AddLifeSkillExp(LIFE_SKILL_TYPE_DIG, SkillPool::Instance().GetLifeSkillOtherCfg().dig_add_exp);
}

void RoleSkill::OnLumbering()
{
	//this->AddLifeSkillExp(LIFE_SKILL_TYPE_LUMBERING, SkillPool::Instance().GetLifeSkillOtherCfg().lumber_add_exp);
}

void RoleSkill::OnPickFlower()
{
	//this->AddLifeSkillExp(LIFE_SKILL_TYPE_PICK_FLOWER, SkillPool::Instance().GetLifeSkillOtherCfg().pick_flower_add_exp);
}

void RoleSkill::OnMakeEquipment(ItemID equip_id, short num)
{
	/*const ItemBase* item = ITEMPOOL->GetItem(equip_id);
	if (NULL == item || I_TYPE_EQUIPMENT != item->GetItemType()) return;
	const Equipment* equip = (const Equipment*)item;

	int add_exp = SkillPool::Instance().GetLifeSkillOtherCfg().MakeEquipAddExp(equip->GetEquipLevel()) * num;
	int life_skill_type = this->GetLifeSkillTypeByEquipClass(equip->GetEquipClass());


	if (life_skill_type != LIFE_SKILL_TYPE_INVALID && add_exp > 0)
	{
		this->AddLifeSkillExp(life_skill_type, add_exp);
	}

	const LifeSkillCfg *ls_sfg = SkillPool::Instance().GetLifeSkillCfg(life_skill_type); if (!ls_sfg) return;
	const WorkshopItemCompound *wic_cfg = LOGIC_CONFIG->GetWorkshopConfig()->GetItemCompoundCfgByItemId(equip_id); if (!wic_cfg) return;

	LifeSkillInfo & info = m_other_param.life_skill_list[life_skill_type];
	int upgrade_need_exp = ls_sfg->GetUpgradeNeedExp(info.skill_level);

	const LifeSkillInfo *ls_info = this->GetLifeSkillInfoByLifeSkillType(life_skill_type); if (!ls_info) return;

	LifeSkillCfg::SkillParam sp = ls_sfg->skill_param[ls_info->skill_level];

	ItemDataWrapper wrappers[COMPOUND_MATE_KIND_MAX];// 返还材料
	memset(wrappers, 0, sizeof(wrappers));
	for (int count = 0; count < COMPOUND_MATE_KIND_MAX; ++count)
	{
		if (wic_cfg->need_mate[count].mate_id != 0 && wic_cfg->need_mate[count].can_return)
		{
			const ItemBase * item = ITEMPOOL->GetItem(wic_cfg->need_mate[count].mate_id);
			if (item != NULL)
			{
				wrappers[count].item_id = item->GetItemId();
				wrappers[count].is_bind = item->IsBind();
				wrappers[count].num = wic_cfg->need_mate[count].need_num * (sp.param2 / 100.0) * num;
			}
		}
	}
		
	int return_rate = rand() % 100;
	if (return_rate <=  sp.param1)
	{
		for (int idx = 0;idx < COMPOUND_MATE_KIND_MAX; ++idx)
		{
			if (wrappers[idx].item_id == 0 || wrappers[idx].num == 0) continue;
			//if (!m_module_mgr->GetKnapsack()->CheckForPut(wrappers[idx].item_id, wrappers[idx].num))
			//{
			//	m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			//	continue;
			//}
			m_module_mgr->GetKnapsack()->Put(wrappers[idx], PUT_REASON_WORKSHOP_COMPOUND);
		}
	}*/
}
/*
int RoleSkill::GetLifeSkillTypeByEquipClass(int equip_class)
{
	int life_skill_type = LIFE_SKILL_TYPE_INVALID;
	switch (equip_class)
	{
	case Equipment::E_CLASS_LIGHT_ARMOR:
		{
			life_skill_type = LIFE_SKILL_TYPE_LIGHT_ARMOUR;
		}
		break;

	case Equipment::E_CLASS_HEAVY_ARMOR:
		{
			life_skill_type = LIFE_SKILL_TYPE_HEAVY_ARMOUR;
		}
		break;

	case Equipment::E_CLASS_CLOTH_ARMOR:
		{
			life_skill_type = LIFE_SKILL_TYPE_QUILTED_ARMOUR;
		}
		break;

	case Equipment::E_CLASS_FISTS_GLOVE:
		{
			life_skill_type = LIFE_SKILL_TYPE_KNUCKLES;
		}
		break;

	case Equipment::E_CLASS_LARGE_SWORD:
		{
			life_skill_type = LIFE_SKILL_TYPE_SWORD;
		}
		break;

	case Equipment::E_CLASS_BATTLE_AXES:
		{
			life_skill_type = LIFE_SKILL_TYPE_AXE;
		}
		break;

	case Equipment::E_CLASS_SHARP_SPEAR:
		{
			life_skill_type = LIFE_SKILL_TYPE_SPEAR;
		}
		break;

	case Equipment::E_CLASS_WAR_LONGBOW:
		{
			life_skill_type = LIFE_SKILL_TYPE_BOW;
		}
		break;

	case Equipment::E_CLASS_MAGIC_STAFF:
		{
			life_skill_type = LIFE_SKILL_TYPE_MAGICIAN_WAND;
		}
		break;

	case Equipment::E_CLASS_CLERIC_WAND:
		{
			life_skill_type = LIFE_SKILL_TYPE_MISSIONARY_WAND;
		}
		break;

	case Equipment::E_CLASS_WIZARD_WAND:
		{
			life_skill_type = LIFE_SKILL_TYPE_WIZARD_WAND;
		}
		break;

	case Equipment::E_CLASS_WARD_SHIELD:
		{
			life_skill_type = LIFE_SKILL_TYPE_SHIELD;
		}
		break;

	case Equipment::E_CLASS_HEAVY_ARMET:
		{
			life_skill_type = LIFE_SKILL_TYPE_HELMET;
		}
		break;

	case Equipment::E_CLASS_LIGHTLY_CAP:
		{
			life_skill_type = LIFE_SKILL_TYPE_LIGHT_CAP;
		}
		break;

	case Equipment::E_CLASS_CLOTH_HOODS:
		{
			life_skill_type = LIFE_SKILL_TYPE_CLOTH_HOODS;
		}
		break;

	case Equipment::E_CLASS_HEAVY_BOOTS:
		{
			life_skill_type = LIFE_SKILL_TYPE_HEAVY_BOOTS;
		}
		break;

	case Equipment::E_CLASS_LIGHT_SHOES:
		{
			life_skill_type = LIFE_SKILL_TYPE_LIGHT_SHOES;
		}
		break;

	case Equipment::E_CLASS_CLOTH_SHOES:
		{
			life_skill_type = LIFE_SKILL_TYPE_CLOTH_SHOES;
		}
		break;
	}

	return life_skill_type;
}*/

void RoleSkill::GiveInitialSkill(int skill_id)
{
	const SkillCfg* skill_cfg = SkillPool::Instance().GetSkillCfg(skill_id);
	if (NULL == skill_cfg)
	{
		const GamePassiveSkillCfg* passive_cfg = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (NULL == passive_cfg)
		{
			m_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}	
	}

	this->AddSkill(skill_id, NULL);
}

int RoleSkill::GetGatherMultiple(int life_skill_type)
{
	int multiple = 1;
	/*if (life_skill_type < LIFE_SKILL_TYPE_MIN || life_skill_type >= LIFE_SKILL_TYPE_MAX) return multiple;

	const LifeSkillCfg * cfg = SkillPool::Instance().GetLifeSkillCfg(life_skill_type);
	if (NULL == cfg) return multiple;

	LifeSkillInfo & info = m_other_param.life_skill_list[life_skill_type];
	const LifeSkillCfg::SkillParam & skill_info = cfg->skill_param[info.skill_level];
	if (skill_info.param1 != 0)
	{
		int rand_num = rand() % 100;
		if (rand_num < skill_info.param1)
		{
			multiple = skill_info.param2;
		}
		else if(skill_info.param2 != 0)
		{
			if (rand_num < skill_info.param3 + skill_info.param1)
			{
				multiple = skill_info.param4;
			}
		}
	}*/
	return multiple;
}

int RoleSkill::CheckIsGather(int item_sub_type, int reason_type)
{
	int multiple = 1;
	/*if (reason_type == PUT_REASON_GATHER_OBJECT)
	{
		switch (item_sub_type)
		{
		case ItemNamespace::ITEM_SUB_TYPE_STONE:
			multiple = this->GetGatherMultiple(LIFE_SKILL_TYPE_DIG);
			break;
		case ItemNamespace::ITEM_SUB_TYPE_WOOD:
			multiple = this->GetGatherMultiple(LIFE_SKILL_TYPE_LUMBERING);
			break;
		case ItemNamespace::ITEM_SUB_TYPE_HERB:
			multiple = this->GetGatherMultiple(LIFE_SKILL_TYPE_PICK_FLOWER);
			break;
		default:
			break;
		}
	}*/

	return multiple;
}

int RoleSkill::TheNumOfLevelSkill(int level)
{
	int num = 0;
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		const SkillItem & skill_data = m_skill_list[i];
		if (skill_data.Invalid()) continue;

		if (skill_data.skill_show_level >= level) ++num;
	}

	return num;
}

void RoleSkill::GetSkillPosition()
{
	m_module_mgr->GetHuaShenManager()->OnLoginSynBenZunSkillInfo(m_skill_list);
}

bool RoleSkill::AddSkill(UInt16 skill_id, int* out_index)
{
	// 这里不检测skill_id了，应由外部检查

	int empty_index = -1;
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid() && empty_index == -1)
		{
			empty_index = i;
			break;
		}

		if (!m_skill_list[i].Invalid() && m_skill_list[i].skill_id == skill_id)
		{
			return false;
		}
	}

	if (-1 == empty_index)
	{
		return false;
	}

	//ps.技能需要使用铜币升级,不再白送,激活技能不需消耗
	int skill_level = 1;
	/*int skill_level = GetMax(1, this->GetSkillLevel());
	if (NULL != SkillPool::Instance().GetPassiveSkillCfg(skill_id))
	{
		skill_level = 1;
	}*/

	m_skill_list[empty_index].skill_id = skill_id;
	m_skill_list[empty_index].skill_exp = 0;
	m_skill_list[empty_index].skill_level = skill_level;
	m_skill_list[empty_index].index = empty_index;
	m_skill_list[empty_index].skill_show_level = skill_level;
	
	if (NULL != out_index)
	{
		*out_index = empty_index;
	}

	EventHandler::Instance().OnAddSkill(m_module_mgr->GetRole(), &m_skill_list[empty_index]);
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ROLE_SKILL, __FUNCTION__); // 加技能时要重算属性
	this->SetDirty(empty_index);

	this->SetCommonUseSkill(empty_index, false);
	return true;
}

bool RoleSkill::AddSkillExp(SkillItem* skill_item, int add_exp, const SkillCfg* skill_cfg /* = NULL */, int* prof_max_skill_level /* = NULL */)
{
	if (NULL == skill_cfg)
	{
		skill_cfg = SkillPool::Instance().GetSkillCfg(skill_item->skill_id);
		if (NULL == skill_cfg) return false;
	}

	int old_level = skill_item->skill_level;
	short max_skill_level = (NULL == prof_max_skill_level) ? 
		LOGIC_CONFIG->GetProfessionConfig()->GetMaxLearnSkillLevel(m_module_mgr->GetRole(), skill_item->skill_id) : 
		*prof_max_skill_level;

	short role_level_limit_skill_level = skill_cfg->GetMaxLevelForRoleLevel(m_module_mgr->GetRole()->GetLevel()); // 当前的角色等级可以学到的最大技能等级
	short fin_lv_limit = GetMin(max_skill_level, role_level_limit_skill_level);
	int uplevel_need_exp = skill_cfg->GetUplevelNeedExp(skill_item->skill_level + 1);
	
	skill_item->skill_exp += add_exp;

	while (skill_item->skill_exp >= uplevel_need_exp && skill_item->skill_level < fin_lv_limit)
	{
		++skill_item->skill_level;
		skill_item->skill_exp = (skill_item->skill_level >= max_skill_level) ? 0 : skill_item->skill_exp - uplevel_need_exp;
		uplevel_need_exp = skill_cfg->GetUplevelNeedExp(skill_item->skill_level + 1);
	}
	
	skill_item->skill_show_level = GetMin(max_skill_level, skill_item->skill_level);		// 同步一次实际等级
	if (old_level != skill_item->skill_level)
	{
		EventHandler::Instance().OnSkillLevelUp(m_module_mgr->GetRole(), skill_item->skill_id, old_level, skill_item->skill_level);
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ROLE_SKILL, __FUNCTION__); // 升级时要重算属性
	}
	this->SetDirty(skill_item->index);

	return true;
}

void RoleSkill::SetDirty(short skill_index, bool is_notice_client /*= true*/)
{
	if (skill_index < 0 || skill_index >= MAX_SKILL_NUM) return;

	m_is_change = true;
	m_dirty_mark[skill_index] = true;

	if (is_notice_client)
	{
		this->SendSingleSkillInfo(skill_index);
	}
}

const AttributeList & RoleSkill::ReCalcAttr(AttributeList &base_add, bool is_recalc)
{
	// 不管是否recalc都要计算，因为技能里有被动技能是按某些属性的值加其他属性的值
	m_attrs_add.Reset();
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;
		if (m_skill_list[i].skill_show_level <= 0) continue;		

		int skill_id = m_skill_list[i].skill_id;
		int skill_level = m_skill_list[i].skill_show_level;
		if (skill_level > MAX_BATTLE_SKILL_LEVEL) continue;

		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
		if (NULL == passive_skill) continue;

		int spid = passive_skill->sp_id;
		if (passive_skill->is_level_spid)
		{
			spid = passive_skill->level_spid_list[skill_level];
		}

		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
		if (NULL == spe_cfg) continue;

		for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
		{
			const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
			if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

			const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
			if (NULL == detail_cfg) continue;

			if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				m_attrs_add.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
				base_add.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
			}

			if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
				m_attrs_add.m_attrs[detail_cfg->param_list[0]] += add_attr;
				base_add.m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[2] > 0)
			{
				int attr_type0 = detail_cfg->param_list[0];
				int attr_type1 = detail_cfg->param_list[1];
				m_attrs_add.m_attrs[attr_type0] += static_cast<int>(1.0 * base_add.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
				base_add.m_attrs[attr_type0] += static_cast<int>(1.0 * base_add.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
			}
		}
	}
	return m_attrs_add;
}

void RoleSkill::OnRoleAdvanceCareer()
{
	//this->CheckSkillLevel();
}

void RoleSkill::AddLifeSkillExp(int life_skill_type, int add_exp)
{
	if (life_skill_type < LIFE_SKILL_TYPE_MIN || life_skill_type >= LIFE_SKILL_TYPE_MAX) return;
	/*
	const LifeSkillCfg * cfg = SkillPool::Instance().GetLifeSkillCfg(life_skill_type);
	if (NULL == cfg) return;

	LifeSkillInfo & info = m_other_param.life_skill_list[life_skill_type];
	if (info.skill_level >= cfg->max_level) return;
	int old_level = info.skill_level;
	info.skill_exp += add_exp;
	int upgrade_need_exp = cfg->GetUpgradeNeedExp(info.skill_level);
	while (info.skill_exp >= upgrade_need_exp && info.skill_level < cfg->max_level)
	{
		++info.skill_level;
		info.skill_exp = (info.skill_level >= cfg->max_level) ? 0 : info.skill_exp - upgrade_need_exp;
		upgrade_need_exp = cfg->GetUpgradeNeedExp(info.skill_level);
		EventHandler::Instance().OnLifeSkillLevelUp(m_module_mgr->GetRole(), cfg->life_skill_id, old_level, info.skill_level);
	}

	this->SendSkillExpChangeNotify(cfg->life_skill_id, add_exp);
	this->SendLifeSkillInfo(life_skill_type);*/
}

void RoleSkill::SendSkillExpChangeNotify(int skill_id, int exp_change)
{
	if (0 == exp_change) return;

	Protocol::SCSkillExpDeltaChangeNotify secn;
	secn.skill_id = skill_id;
	secn.exp_change = exp_change;
	m_module_mgr->NetSend((const void *)&secn, sizeof(secn));
}

void RoleSkill::CheckAnyInitialSkillMissing()
{
	// 检查是否有老玩家没有新加的初始职业技能 并将技能等级拉平
	const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_module_mgr->GetRole()->GetProfession(), m_module_mgr->GetAdvanceCareer()->GetSubLevel());
	if (NULL != cfg)
	{
		for (int i = 0; i < cfg->init_skill_num && i < ARRAY_ITEM_COUNT(cfg->init_skill_id); i++)
		{
			if (NULL == this->GetSkillBySkillId(cfg->init_skill_id[i]))
			{
				this->GiveInitialSkill(cfg->init_skill_id[i]);
			}
		}
	}
	//this->CheckSkillLevel();
}

void RoleSkill::CheckOldSkillReturn()
{
	int return_coin_count = 0;
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid() || !LOGIC_CONFIG->GetProfessionConfig()->IsNeedReturn(m_skill_list[i].skill_id)) continue;
		
		return_coin_count += LOGIC_CONFIG->GetProfessionConfig()->GetSkillChangeReturn(m_skill_list[i].skill_id, m_skill_list[i].skill_level, m_skill_list[i].skill_exp);
		m_skill_list[i].Reset();
		this->SetDirty(i, false);
	}
	if (return_coin_count > 0)
	{
		static MailContentParam contentparam; contentparam.Reset();

		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = return_coin_count;
		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_role_skill_chagne_return_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_role_skill_change_return_content);
		if (length1 > 0 && length2 > 0)
		{
			MailRoute::MailToUser(IntToUid(m_module_mgr->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void RoleSkill::CheckSkillUplevelConsumeCoinReturn()
{
	if (m_other_param.skill_uplevel_consume_coin_return_flag != 0) return;

	m_other_param.skill_uplevel_consume_coin_return_flag = 1;

	static MailContentParam contentparam; 
	contentparam.Reset();

	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_role_skill_chagne_return_subject_2);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_role_skill_change_return_content_2);
	if (length1 <= 0 || length2 <= 0) return;

	// 返还玩家已升级技能的所有铜币
	Int64 back_money = 0;
	for (int idx = 0; idx < ARRAY_ITEM_COUNT(m_skill_list); ++idx)
	{
		RoleSkill::SkillItem *skill = this->GetSkill(idx);
		if (skill == NULL || skill->Invalid()) continue;

		if (skill->skill_level > 13)
		{
			assert(0);// 出现这种情况可能是重复返还了，需要查查
			continue; // 13是旧的技能的最等级，如果大于13，则不应该返还
		}

		long long add_coin = SkillPool::Instance().GetSkillGoBackRetCoin(skill->skill_id,
			skill->skill_level, skill->skill_exp, 
			m_module_mgr->GetRole()->GetProfession(), 
			m_module_mgr->GetAdvanceCareer()->GetSubLevel());
		if (add_coin > 0) back_money += add_coin;
	}

	if (back_money > 0)
	{
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = back_money;
		
		MailRoute::MailToUser(IntToUid(m_module_mgr->GetUid()), MAIL_REASON_DEFAULT, contentparam);
	}

	gamelog::g_log_role_data.printf(LL_INFO, "CheckSkillUplevelConsumeCoinReturn role[%d,%s] back_coin[%lld] flag[%d]",
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), back_money, m_other_param.skill_uplevel_consume_coin_return_flag);
}

void RoleSkill::CheckSkillLevel()
{
	int skill_level = this->GetSkillLevel();
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;
	
		if (skill_level > m_skill_list[i].skill_level)
		{
			m_skill_list[i].skill_level = skill_level;
		}

		if (skill_level > m_skill_list[i].skill_show_level)
		{
			m_skill_list[i].skill_show_level = skill_level;
		}

		this->SetDirty(i, true);
	}
}

void RoleSkill::CheckSkillConformProf()
{
	std::set<int> skill_set = LOGIC_CONFIG->GetProfessionConfig()->GetProfSkillSet(GetBaseProf(m_module_mgr->GetRole()->GetProfession()));
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;

		if (skill_set.end() == skill_set.find(m_skill_list[i].skill_id))
		{
			m_skill_list[i].Reset();
			this->SetDirty(i);
		}
	}
}

int RoleSkill::GetSkillLevel()
{
	const CareerAdvanceCfg*  node = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_module_mgr->GetRole()->GetProfession(), m_module_mgr->GetAdvanceCareer()->GetSubLevel());
	if (NULL != node)
	{
		return node->max_skill_level;
	}
	return 0;
}

void RoleSkill::GiveInitialSkillOnRoleCreate()
{
	const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_module_mgr->GetRole()->GetProfession(), m_module_mgr->GetAdvanceCareer()->GetSubLevel());
	if (NULL != cfg)
	{
		for (int i = 0; i < cfg->init_skill_num && i < ARRAY_ITEM_COUNT(cfg->init_skill_id); i++)
		{
			if (NULL == this->GetSkillBySkillId(cfg->init_skill_id[i]))
			{
				this->GiveInitialSkill(cfg->init_skill_id[i]);
			}
		}
	}
	//this->CheckSkillLevel();
}

void RoleSkill::RemoveSkillByProfession(int profession)
{
	std::vector<int> skill_list = LOGIC_CONFIG->GetProfessionConfig()->GetProfSkillList(GetBaseProf(profession));
	for (size_t i = 0; i < skill_list.size(); ++i)
	{
		this->RemoveSkillById(skill_list[i]);
	}
}

void RoleSkill::RemoveSkillById(int skill_id)
{
	for (int i = 0; i < MAX_SKILL_NUM; ++i)
	{
		if (m_skill_list[i].Invalid()) continue;

		if (m_skill_list[i].skill_id == skill_id)
		{
			m_skill_list[i].Reset();
			this->SetDirty(i);
			break;
		}
	}
}
