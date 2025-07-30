#include "roleguildinborn.hpp"
#include "world.h"
#include "gamelog.h"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/guildconfig.hpp"

#include "obj/character/role.h"
#include "skill/skillpool.hpp"
#include "global/guild/guild.hpp"
#include "protocol/msgguild.hpp"

#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/roleattribute/roleexpconfig.hpp"

RoleGuildInborn::RoleGuildInborn() : m_module_mgr(NULL)
{
}

RoleGuildInborn::~RoleGuildInborn()
{
}

void RoleGuildInborn::Init(RoleModuleManager * module_mgr, const RoleGuildInbornParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;

	if (0 == m_param.is_check_old_skill_flag)
	{
		for (int i = 0; i < MAX_GUILD_INBORN_SKILL_TYPE_NUM; i++)
		{
			for (int k = 0; k < MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM; k++)
			{
				if(0 == m_param.inborn_learn_list[i][k].is_learn_flag) continue;

				m_param.inborn_learn_list[i][k].skill_level = 4;
			}
		}

		m_param.is_check_old_skill_flag = 1;
	}
}

void RoleGuildInborn::GetInitParam(RoleGuildInbornParam * param)
{
	*param = m_param;
}

void RoleGuildInborn::OnRoleLogin()
{
}

const AttributeList & RoleGuildInborn::ReCalcAttr(AttributeList & base_add, bool is_recalc, AttributeList & skill_attr_list)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		for (int i = 0; i < MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM; i++)
		{
			for (int k = 0; k < MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM; k++)
			{
				if(m_param.inborn_learn_list[i][k].IsInvaild()) continue;

				const GuildInbornSkill & skill_info = m_param.inborn_learn_list[i][k];
				const GuildInbornLevelCfg * inborn_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildInbornCfg(i, k, skill_info.skill_level);
				if(NULL == inborn_cfg || !(inborn_cfg->gain_target & (1 << GUILD_INBORN_ADD_TARGET_TYPE_ROLE)))continue;

				const GamePassiveSkillCfg * passive_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(inborn_cfg->inborn_skill_id);
				if(NULL == passive_skill_cfg) continue;

				int spid = passive_skill_cfg->sp_id;
				if (passive_skill_cfg->is_level_spid)
				{
					if (skill_info.skill_level >= 1 && skill_info.skill_level < ARRAY_LENGTH(passive_skill_cfg->level_spid_list))
					{
						spid = passive_skill_cfg->level_spid_list[skill_info.skill_level];
					}
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
					}

					if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
						&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
					{
						BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
						m_attrs_add.m_attrs[detail_cfg->param_list[0]] += add_attr;
					}

					if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
						&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
						&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
						&& detail_cfg->param_list[2] > 0)
					{
						int attr_type0 = detail_cfg->param_list[0];
						int attr_type1 = detail_cfg->param_list[1];
						m_attrs_add.m_attrs[attr_type0] += static_cast<int>(1.0 * base_add.m_attrs[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
					}
				}
			}
		}
		//家族灵脉可能会配5项基础属性
		AttributePlan tmp_ap;
		for (int i = 0, k = BATTLE_ADD_POINT_TYPE_BEGIN; i < ADD_POINT_TYPE_MAX && k <= BATTLE_ADD_POINT_TYPE_END; i++, k++)
		{
			tmp_ap.add_point_list[i] += m_attrs_add.m_attrs[k];
		}
		LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, m_attrs_add);
	}

	base_add.Add(m_attrs_add.m_attrs);
	skill_attr_list.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

int RoleGuildInborn::GetSkillList(std::vector<SkillPro>* skill_list, int add_target_type)
{
	if(NULL == skill_list || add_target_type < 0 || add_target_type >= GUILD_INBORN_ADD_TARGET_TYPE_MAX) return 0;

	int skill_capability = 0;
	for (int i = 0; i < MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM; i++)
	{
		for (int k = 0; k < MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM; k++)
		{
			if (m_param.inborn_learn_list[i][k].IsInvaild()) continue;

			const GuildInbornSkill & skill_info = m_param.inborn_learn_list[i][k];
			const GuildInbornLevelCfg * inborn_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildInbornCfg(i, k, skill_info.skill_level);
			if (NULL == inborn_cfg || !(inborn_cfg->gain_target & (1 << add_target_type)))continue;

			skill_list->push_back(SkillPro(inborn_cfg->inborn_skill_id, skill_info.skill_level));
			skill_capability += inborn_cfg->skill_grade;
		}
	}
	return skill_capability;
}

int RoleGuildInborn::GetSkillScroe(int add_target_type)
{
	if(add_target_type < 0 || add_target_type >= GUILD_INBORN_ADD_TARGET_TYPE_MAX) return 0;

	int skill_capability = 0;
	for (int i = 0; i < MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM; i++)
	{
		for (int k = 0; k < MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM; k++)
		{
			if (m_param.inborn_learn_list[i][k].IsInvaild()) continue;

			const GuildInbornSkill & skill_info = m_param.inborn_learn_list[i][k];
			const GuildInbornLevelCfg * inborn_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildInbornCfg(i, k, skill_info.skill_level);
			if (NULL == inborn_cfg || !(inborn_cfg->gain_target & (1 << add_target_type)))continue;

			skill_capability += inborn_cfg->skill_grade;
		}
	}
	return skill_capability;
}

void RoleGuildInborn::OnLearnSkill(int inborn_type, int type_seq)
{
	if(this->IsInvaild(inborn_type, type_seq)) return;
	
	GuildInbornSkill & inborn_learn_info = m_param.inborn_learn_list[inborn_type][type_seq];
	/*if (!inborn_learn_info.IsInvaild())
	{
		m_module_mgr->NoticeNum(errornum::EN_SKILL_HAS_LEARNED);
		return;
	}*/
	int max_skill_level = LOGIC_CONFIG->GetGuildConfig()->GetMaxGuildInbornLevel(inborn_type, type_seq);
	const GuildInbornLevelCfg * level_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildInbornCfg(inborn_type, type_seq, inborn_learn_info.skill_level + 1);
	if (NULL == level_cfg || inborn_learn_info.skill_level >= max_skill_level)
	{
		return;
	}

	Guild * guild = m_module_mgr->GetRole()->GetGuild();
	if (NULL == guild)
	{
		m_module_mgr->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return;
	}

	if (m_module_mgr->GetRole()->GetLevel() < level_cfg->level_limit)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_role_guild_inborn_learn_skill_level_not_enough, level_cfg->level_limit);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
		return;
	}
	if(guild->GetGuildLevel() < level_cfg->guild_level_limit)
	{
		m_module_mgr->NoticeNum(errornum::EN_GUILD_LEVEL_NOT_ENOUGH);
		return;
	}

	if (-1 != level_cfg->prepose_seq)
	{
		if(this->IsInvaild(inborn_type, level_cfg->prepose_seq)) return;

		if (m_param.inborn_learn_list[inborn_type][level_cfg->prepose_seq].IsInvaild())
		{
			m_module_mgr->NoticeNum(errornum::EN_GUILD_INBORN_PREPOSEL_SKILL_NOT_LEARN);
			return;
		}
	}

	if(!level_cfg->consume_list.empty())
	{
		int stuff_count = 0;
		ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
		for (int i = 0; i < (int)level_cfg->consume_list.size() && stuff_count < ARRAY_ITEM_COUNT(stuff_list); ++i)
		{
			stuff_list[stuff_count].item_id = level_cfg->consume_list[i].item_id;
			stuff_list[stuff_count].num = level_cfg->consume_list[i].num;
			stuff_list[stuff_count].buyable = false;
			stuff_count += 1;
		}

		if (stuff_count > 0)
		{
			static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
			if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
			{
				m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}

			if (consume_list.count > 0)
			{
				if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
				{
					m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
					return;
				}
			}
		}
	}

	/*const GuildRoomCfg* room_cfg = LOGIC_CONFIG->GetGuildConfig()->GetRoomCfgByLevel(guild->GetGuildLevel());
	if (NULL == room_cfg)
	{
		return;
	}*/

	/*if (inborn_cfg->consume_skill_points + m_param.use_skill_points > room_cfg->skill_points)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_role_guild_inborn_skill_points_not_enough, inborn_cfg->consume_skill_points);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
		return;
	}*/

	inborn_learn_info.is_learn_flag = 1;
	inborn_learn_info.skill_level += 1;
//	m_param.use_skill_points += inborn_cfg->consume_skill_points;
	this->SendSingleInfo(inborn_type, type_seq, inborn_learn_info.skill_level);
	this->OnSkillChange(inborn_type, level_cfg->gain_target);
	gamelog::g_log_role_guild_inborn.printf(LL_INFO, "%s role[%d, %s] inborn_type[%d] type_seq[%d] level[%d]", __FUNCTION__, 
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), inborn_type, type_seq, inborn_learn_info.skill_level);
}

void RoleGuildInborn::OnResetSkill(int inborn_type, int type_seq)
{
	if (this->IsInvaild(inborn_type, type_seq)) return;

	if (m_param.inborn_learn_list[inborn_type][type_seq].IsInvaild())
	{
		m_module_mgr->NoticeNum(errornum::EN_SKILL_NOT_LEARN);
		return;
	}
	 GuildInbornSkill & skill_info = m_param.inborn_learn_list[inborn_type][type_seq];
	const GuildInbornLevelCfg * inborn_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildInbornCfg(inborn_type, type_seq, skill_info.skill_level);
	if (NULL == inborn_cfg)
	{
		return;
	}

	if (!m_module_mgr->GetMoney()->UseOtherCoin(inborn_cfg->reset_consume, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_GUILD_GONGXIAN_NOT_ENOUGH);
	}

	std::vector<ItemConfigData> return_item_list;
	LOGIC_CONFIG->GetGuildConfig()->GetGuildInbornResetSkill(inborn_type, type_seq, m_param.inborn_learn_list[inborn_type], MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_NUM, return_item_list);

	if(!return_item_list.empty())
	{
		m_module_mgr->GetKnapsack()->SendMail(return_item_list, SEND_MAIL_TYPE_GUILD_INBORN_SKILL_RESET);
	}

	
	this->SendSingleInfoByType(inborn_type);
	this->OnSkillChange(inborn_type, inborn_cfg->gain_target);
	gamelog::g_log_role_guild_inborn.printf(LL_INFO, "%s role[%d, %s] inborn_type[%d] type_seq[%d]", __FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(),
		inborn_type, type_seq);
}

void RoleGuildInborn::OnSetGuildLevel(int guild_level)
{
	//已不再需要最高等级了
	return;
	if (m_param.max_guild_level < guild_level)
	{
		m_param.max_guild_level = guild_level;
		gamelog::g_log_role_guild_inborn.printf(LL_INFO, "%s role[%d, %s] max_guild_level[%d] use_skill_points[%d]", __FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(),
			m_param.max_guild_level, m_param.use_skill_points);
	}
}

void RoleGuildInborn::GetPassiveSkillByType(int add_target_type, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	if(NULL == out_other_sp_num || NULL == out_other_sp_list || add_target_type < 0 || add_target_type >= GUILD_INBORN_ADD_TARGET_TYPE_MAX) return;

	for (int i = 0; i < MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM; i++)
	{
		for (int k = 0; k < MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM; k++)
		{
			if (m_param.inborn_learn_list[i][k].IsInvaild()) continue;

			const GuildInbornSkill & skill_info = m_param.inborn_learn_list[i][k];
			const GuildInbornLevelCfg * inborn_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildInbornCfg(i, k, skill_info.skill_level);
			if (NULL == inborn_cfg || !(inborn_cfg->gain_target & (1 << add_target_type)))continue;

			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(inborn_cfg->inborn_skill_id);
			if (NULL == passive_skill) continue;
			
			int skill_level = skill_info.skill_level;
			int spid = passive_skill->sp_id;
			if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
			{
				spid = passive_skill->level_spid_list[skill_level];
			}
			// 加面板的特效不收集入战斗
			const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
			if (NULL == spe_cfg) continue;

			bool is_has_battle_effect = false;
			for (int j = 0; j < spe_cfg->effect_num && j < EFFECT_DETAIL_NUM; ++j)
			{
				const SpecialEffectKey & key = spe_cfg->effect_detail_list[j];
				if (key.effect_type == SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

				is_has_battle_effect = true;
				break;
			}

			if(!is_has_battle_effect)
			{
				continue;
			}

			if (*out_other_sp_num < max_other_sp_num)
			{
				out_other_sp_list[(*out_other_sp_num)++] = spid;
			}
			else
			{
#ifdef _DEBUG
				assert(false);
#else
				gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_other_sp_num[%d] ", __FUNCTION__, max_other_sp_num);
#endif
				return;
			}
		}
	}
}

void RoleGuildInborn::SendAllInfo()
{
	Protocol::SCGuildInbornAllInfo info;
	info.reserve_sh = 0;
	info.learn_count = 0;
	for (int i = 0; i < MAX_GUILD_INBORN_SKILL_TYPE_CUR_NUM; i++)
	{
		for (int k = 0; k < MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM; k++)
		{
			if(m_param.inborn_learn_list[i][k].IsInvaild()) continue;

			info.learn_list[info.learn_count++] = Protocol::GuildInbornSkillInfo(i, k, m_param.inborn_learn_list[i][k].skill_level);
		}
	}

	int length = sizeof(info) - (ARRAY_LENGTH(info.learn_list) - info.learn_count) * sizeof(info.learn_list[0]);
	m_module_mgr->NetSend(&info, length);
}

void RoleGuildInborn::SendSingleInfo(int inborn_type, int type_seq, int skill_level)
{
	if(this->IsInvaild(inborn_type, type_seq)) return;

	Protocol::SCGuildInbornSingleInfo info;
	info.learn_info = Protocol::GuildInbornSkillInfo(inborn_type, type_seq, skill_level);

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleGuildInborn::SendSingleInfoByType(int inborn_type)
{
	if(this->IsInvaild(inborn_type)) return;

	Protocol::SCGuildInbornSingleTypeInfo info;
	info.reserve_sh = 0;
	info.guild_inborn_type = inborn_type;
	for (int i = 0; i < MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM; i++)
	{
		info.learn_level_list[i] = m_param.inborn_learn_list[inborn_type][i].skill_level;
	}

	m_module_mgr->NetSend(&info, sizeof(info));
}

bool RoleGuildInborn::IsInvaild(int inborn_type, int type_seq)
{
	if(inborn_type < 0 || inborn_type >= GUILD_INBORN_SKILL_TYPE_MAX) return true;

	return type_seq < 0 || type_seq >= MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM;
}

void RoleGuildInborn::OnSkillChange(int inborn_type, int add_target_type)
{
	if (add_target_type & (1 << GUILD_INBORN_ADD_TARGET_TYPE_ROLE))
	{
		EventHandler::Instance().OnRoleGuildInbornRoleChange(m_module_mgr->GetRole(), inborn_type);
	}
	if (add_target_type & (1 << GUILD_INBORN_ADD_TARGET_TYPE_PET))
	{
		EventHandler::Instance().OnRoleGuildInbornPetChange(m_module_mgr->GetRole(), inborn_type);
	}
	if (add_target_type & (1 << GUILD_INBORN_ADD_TARGET_TYPE_PARTNER))
	{
		EventHandler::Instance().OnRoleGuildInbornPartnerChange(m_module_mgr->GetRole(), inborn_type);
	}
}
