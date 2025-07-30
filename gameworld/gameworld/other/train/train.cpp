#include "train.hpp"
#include "trainconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "obj/character/role.h"
#include "protocol/msgtrain.h"
#include "item/money.h"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "other/pet/pet.hpp"
#include "gamelog.h"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"

Train::Train()
{
}

Train::~Train()
{
}

void Train::Init(RoleModuleManager * role_module_manager, const TrainParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;

}

void Train::GetOtherInitParam(TrainParam * param)
{
	*param = m_param;
}

int Train::GetTrainLevel(int type)
{
	if (type < 0 || type >= TARIN_TYPE_MAX) return 0;

	return m_param.train_list[type].level;
}

int Train::GetTrainSkillCapability()
{
	int ret_capability = 0;
	for (int type = TRAIN_BEGIN; type < TRAIN_ENG; ++type)
	{
		int level = this->GetTrainLevel(type);
		ret_capability += LOGIC_CONFIG->GetScoreSystemConfig()->GetTrainSkillScore(type, level);
	}

	return ret_capability;
}

const AttributeList & Train::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();

		int add_attr = 0;
		const TrainCfg * cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_PHYSICAL_DEC, m_param.train_list[TRAIN_PHYSICAL_DEC].level);
		if (NULL != cfg && 0 != cfg->attribute_value)
		{
			add_attr = (int)(base_add.m_attrs[BATTLE_ATTR_DEFENSE] * 10.0 / cfg->attribute_value);		//防御/参数得出的是百分比
			m_attrs_add.m_attrs[BATTLE_ATTR_PHYSICAL_DEC_DAMAGE] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
		}

		cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MAGIC_DEC, m_param.train_list[TRAIN_MAGIC_DEC].level);
		if (NULL != cfg && 0 != cfg->attribute_value)
		{
			add_attr = (int)(base_add.m_attrs[BATTLE_ATTR_DEFENSE] * 10 / cfg->attribute_value);
			m_attrs_add.m_attrs[BATTLE_ATTR_MAGIC_DEC_DAMAGE] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
		}

		for (int i = TRAIN_BEGIN; i < TRAIN_ENG; ++i)
		{
			const TrainAttrCfg * attr_cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainAttrCfg(m_role_module_mgr->GetRole()->GetBaseProfession(), i, m_param.train_list[i].level);
			if (NULL != attr_cfg && 0 <= attr_cfg->attribute_type && attr_cfg->attribute_type < BATTLE_ATTR_MAX)
			{
				m_attrs_add.m_attrs[attr_cfg->attribute_type] += attr_cfg->attribute_value;
			}
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

const AttributeList & Train::ReCalcAttrPer(AttributeList & base_add, const AttributeList & cpy_attr_list)
{
	m_attrs_add_per.Reset();

	int add_attr = 0;
	const TrainCfg *cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_ATTACK, m_param.train_list[TRAIN_ATTACK].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_ATTACK]);
		m_attrs_add_per.m_attrs[BATTLE_ATTR_ATTACK] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MENTAL, m_param.train_list[TRAIN_MENTAL].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_MENTAL]);
		m_attrs_add_per.m_attrs[BATTLE_ATTR_MENTAL] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MAX_HP, m_param.train_list[TRAIN_MAX_HP].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_MAX_HP]);
		m_attrs_add_per.m_attrs[BATTLE_ATTR_MAX_HP] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MAX_MP, m_param.train_list[TRAIN_MAX_MP].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_MAX_MP]);
		m_attrs_add_per.m_attrs[BATTLE_ATTR_MAX_MP] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAIN_MAIN_ATTR, m_param.train_list[TRAIN_MAIN_ATTR].level);
	if (NULL != cfg)
	{
		int attr_add_type = LOGIC_CONFIG->GetTrainConfig()->GetMainAttrType(m_role_module_mgr->GetRole()->GetProfession());
		if (attr_add_type >= 0 && attr_add_type < BATTLE_ATTR_MAX)
		{
			add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[attr_add_type]);
			m_attrs_add_per.m_attrs[attr_add_type] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
		}
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAN_DEFENSE, m_param.train_list[TRAN_DEFENSE].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_DEFENSE]);
		m_attrs_add_per.m_attrs[BATTLE_ATTR_DEFENSE] += (0 != cfg->level_limit && add_attr >= cfg->level_limit) ? cfg->level_limit : add_attr;
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAN_AGILITY, m_param.train_list[TRAN_AGILITY].level);
	if (NULL != cfg)
	{
		add_attr = (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_AGILITY]);
		m_attrs_add_per.m_attrs[BATTLE_ATTR_AGILITY] += (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_AGILITY]);
	}

	cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(TRAN_MAGIC_DEFENSE, m_param.train_list[TRAN_MAGIC_DEFENSE].level);
	if (NULL != cfg)
	{
		m_attrs_add_per.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE] += (int)(cfg->attribute_value / 10000.0 * cpy_attr_list.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE]);
	}

	base_add.Add(m_attrs_add_per.m_attrs);
	return m_attrs_add_per;
}

void Train::OnRoleLogin()
{
	//默认勾选
	if (1 != m_param.is_set_flag)
	{
		m_param.pet_train_red_dot_flag = 1;
		m_param.train_red_dot_flag |= ~0;
		m_param.is_set_flag = 1;
		this->SendRedDotFlagInfo();
	}
}

void Train::OnTrainUp(int type, int num)
{
	if (type < 0 || type >= TRAIN_ENG)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int max_level = LOGIC_CONFIG->GetTrainConfig()->GetMaxTrainLevel(type,m_role_module_mgr->GetRole()->GetLevel());
	const TrainOtherCfg & other_cfg = LOGIC_CONFIG->GetTrainConfig()->GetOtherCfg();

	TrainData &train = m_param.train_list[type];
	if (train.level >= max_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	const TrainCfg * cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(type, train.level + 1);
	if (NULL == cfg)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//////////////////////////////////////////////  消耗   //////////////////////////////////////////////
	int add_exp = 0;
	switch (cfg->consume_type)
	{
	case TRAIN_CONSUME_TYPE_BIND_COIN:
		{
			if (m_role_module_mgr->GetMoney()->UseCoinBind(cfg->bind_coin_consume * num, __FUNCTION__))
			{
				add_exp = cfg->get_exp * num;
			}
			else
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			}
		}
		break;
	case TRAIN_CONSUME_TYPE_ITEM:
		{
			if (m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->item_id, cfg->train_ball_consume * num, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, true))
			{
				add_exp = cfg->train_ball_exp * num;
				if (other_cfg.item_ret == cfg->item_id)
				{
					int old_num = m_role_module_mgr->GetCommonData().train_uplevel_comsume_item_num;
					m_role_module_mgr->GetCommonData().train_uplevel_comsume_item_num += cfg->train_ball_consume * num;

					gamelog::g_log_train.printf(LL_INFO, "%s line:%d | role[%d %s] old_num:%d curr_num:%d", __FUNCTION__, __LINE__,
								    m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), old_num, m_role_module_mgr->GetCommonData().train_uplevel_comsume_item_num);
				}
			}
		}
		break;
	default:
		{
			return;
		}
	}

	if (add_exp <= 0) return;

	//////////////////////////////////////////////  给予   //////////////////////////////////////////////

	int old_train_level = train.level;
	train.exp += add_exp;
	bool level_up = false;
	while (NULL != cfg && train.exp >= cfg->max_exp && train.level < max_level )
	{
		train.exp -= cfg->max_exp;
		++train.level;
		cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(type, train.level + 1);
		level_up = true;
	}
	
	if (level_up)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_TRAIN, __FUNCTION__);
		EventHandler::Instance().OnRoleTrainLevelUp(m_role_module_mgr->GetRole(), type, old_train_level, train.level);
	}

	this->SendAllInfo();
	this->SendPromotedInfo();
}

void Train::OnTrainUpFast(int type, int num)
{
	if (type < 0 || type >= TRAIN_ENG)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int max_level = LOGIC_CONFIG->GetTrainConfig()->GetMaxTrainLevel(type, m_role_module_mgr->GetRole()->GetLevel());
	const TrainOtherCfg & other_cfg = LOGIC_CONFIG->GetTrainConfig()->GetOtherCfg();

	TrainData &train = m_param.train_list[type];
	if (train.level >= max_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	int new_level = train.level;
	int new_exp = train.exp;

	//实际训练次数
	int real_train = 0;
	//实际增加经验
	long long add_exp = 0;

	for (int temp_level = new_level + 1;
		temp_level <= max_level;
		temp_level++)
	{
		const TrainCfg * cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(type, temp_level);
		if (NULL == cfg)
		{
			break;
		}

		long long need_exp = cfg->max_exp - new_exp;

		//退出循环
		bool need_break = false;

		switch (cfg->consume_type)
		{// 不同等级消耗 可能不同 分等级消耗
		case TRAIN_CONSUME_TYPE_BIND_COIN:
			{
				//升级需求训练次数
				int up_level_need_train = (need_exp - 1) / cfg->get_exp + 1;

				bool is_level_up = num >= real_train + up_level_need_train;

				//该等级升级次数    剩余次数足够升级 ? 升级所需次数 : 剩余次数
				int temp_num = is_level_up ? up_level_need_train : num - real_train;
				int temp_add_exp = cfg->get_exp * temp_num;

				if (m_role_module_mgr->GetMoney()->UseCoinBind(cfg->bind_coin_consume * temp_num, __FUNCTION__))
				{
					add_exp += temp_add_exp;
					real_train += temp_num;

					if (is_level_up)
					{
						new_level++;
						new_exp = new_exp + temp_add_exp - cfg->max_exp;
					}
					else
					{
						new_exp = new_exp + temp_add_exp;
						need_break = true;
					}
				}
				else
				{
					m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					need_break = true;
				}
			}
			break;
		case TRAIN_CONSUME_TYPE_ITEM:
			{
				//升级需求训练次数
				int up_level_need_train = (need_exp - 1) / cfg->train_ball_consume + 1;

				bool is_level_up = num >= real_train + up_level_need_train;

				//该等级升级次数    剩余次数足够升级 ? 升级所需次数 : 剩余次数
				int temp_num = is_level_up ? up_level_need_train : num - real_train;
				int temp_add_exp = cfg->train_ball_consume * temp_num;

				if (m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->item_id, cfg->train_ball_consume * temp_num, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, true))
				{
					add_exp += temp_add_exp;
					real_train += temp_num;

					if (other_cfg.item_ret == cfg->item_id)
					{
						int old_num = m_role_module_mgr->GetCommonData().train_uplevel_comsume_item_num;
						m_role_module_mgr->GetCommonData().train_uplevel_comsume_item_num += cfg->train_ball_consume * temp_num;

						gamelog::g_log_train.printf(LL_INFO, "%s line:%d | role[%d %s] old_num:%d curr_num:%d", __FUNCTION__, __LINE__,
							m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), old_num, m_role_module_mgr->GetCommonData().train_uplevel_comsume_item_num);
					}

					if (is_level_up)
					{
						new_level++;
						new_exp = new_exp + temp_add_exp - cfg->max_exp;
					}
					else
					{
						new_exp = new_exp + temp_add_exp;
						need_break = true;
					}
				}
				else
				{
					need_break = true;
				}
			}
			break;
		default:
			{
				need_break = true;
			}
		}

		if (need_break || real_train >= num)
		{
			break;
		}
	}

	if (add_exp <= 0) return;

	//////////////////////////////////////////////  给予   //////////////////////////////////////////////

	int old_train_level = train.level;
	int old_train_exp = train.exp;

	train.level = new_level;
	train.exp = new_exp;

	if (train.level != old_train_level)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_TRAIN, __FUNCTION__);
		EventHandler::Instance().OnRoleTrainLevelUp(m_role_module_mgr->GetRole(), type, old_train_level, train.level);
	}

	gamelog::g_log_train.printf(LL_INFO, "%s line:%d | role[%d %s] type[%d] old_level:%d old_exp:%d level:%d exp:%d real_train:%d",
		__FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), 
		type, old_train_level, old_train_exp, train.level, train.exp, real_train);


	this->SendAllInfo();
	this->SendPromotedInfo();
}

bool Train::OnPetTrainUp(int num)
{
	if (num <= 0) return false;

	TrainData& train = m_param.pet_train;


	int max_level = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainMaxLevel();
	if (train.level >= max_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_LEVEL_LIMIT);
		return false;
	}

	const PetTrainCfg * cfg = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainCfg(train.level + 1);
	if (NULL == cfg)
	{
		return false;
	}

	if (m_role_module_mgr->GetRole()->GetLevel() < cfg->need_role_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return false;
	}

	int price = cfg->need_coin * num;

	if (!m_role_module_mgr->GetMoney()->UseCoinBind(price, __FUNCTION__))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return false;
	}

	bool level_up = false;
	int add_exp = cfg->add_exp * num;
	train.exp += add_exp;

	while (train.exp >= cfg->need_exp)
	{
		train.level += 1;
		train.exp -= cfg->need_exp;
		level_up = true;

		cfg = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainCfg(train.level + 1);
		if (NULL == cfg)
		{
			break;
		}
	}

	this->SendAllInfo();

	if (level_up)
	{
		EventHandler::Instance().OnRoleTrainPetSkillLevelUp(m_role_module_mgr->GetRole(), TRAIN_TYPE_PET, train.level);
	}

	return true;
}

bool Train::ConvertItem(int seq, bool is_auto_buy)
{
	const TrainItemConvertCfg * cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainItemConvertCfg(seq);
	if (NULL == cfg)
	{
		return false;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->target_item_id, cfg->target_item_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	// 获取可消耗的背包材料列表
	short stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (int i = 0; i < cfg->comsume_size && i < TrainItemConvertCfg::MAX_COMSUME_SIZE && stuff_count < ARRAY_LENGTH(stuff_list); ++i)
	{
		stuff_list[stuff_count].item_id = cfg->comsume_item_id[i];
		stuff_list[stuff_count].num = cfg->comsume_num[i];
		stuff_list[stuff_count].buyable = is_auto_buy;
		stuff_count += 1;
	}

	ItemExtern::ItemConsumeList consume_list;
	consume_list.Reset();
	if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
	{
		return false;
	}

	if (consume_list.need_gold_num > 0 && !m_role_module_mgr->GetMoney()->GoldMoreThan(consume_list.need_gold_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return false;
	}

	//消耗
	if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		return false;
	}

	if (consume_list.need_gold_num > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(consume_list.need_gold_num, __FUNCTION__))
		{
			return false;
		}
	}

	ItemDataWrapper get_item_obj(cfg->target_item_id, cfg->target_item_num, 1);
	m_role_module_mgr->GetKnapsack()->Put(get_item_obj, PUT_REASON_LIU_LI_DING);

	return true;
}

void Train::SetRedDotFlag(int train_type)
{
	if (TRAIN_TYPE_PET == train_type)
	{
		m_param.pet_train_red_dot_flag = (0 != m_param.pet_train_red_dot_flag) ? 0 : 1;
	}
	else
	{
		if (train_type < 0 || train_type >= TRAIN_ENG)
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		if (m_param.train_red_dot_flag & (1 << train_type))
		{
			m_param.train_red_dot_flag &= ~(1 << train_type);
		}
		else
		{
			m_param.train_red_dot_flag |= (1 << train_type);
		}
	}
	this->SendRedDotFlagInfo();
}

int Train::OnPetTrainUp2(int type, int num)
{
	if (!LOGIC_CONFIG->GetTrainConfig()->IsPetTrainCfg2Type(type))
	{
		return -__LINE__;
	}
		
	RoleCommonSavePetTrainData2 * train_ptr = this->GetRoleCommonSavePetTrainData2(type);
	if (train_ptr == NULL)
	{
		return -__LINE__;
	}
	RoleCommonSavePetTrainData2 & train = *train_ptr;

	int max_level = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainCfgMaxLevel2(type);
	const PetTrainCfg2 * cfg = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainCfg2(type, train.level + 1);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	if (train.level>=max_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_LEVEL_LIMIT);
		return -__LINE__;
	}

	if (cfg->max_exp <= 0)
	{
		return -__LINE__;
	}

	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	if (role_level < cfg->need_role_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return -__LINE__;
	}

	//////////////////////////////////////////////  消耗   //////////////////////////////////////////////
	int add_exp = 0;
	switch (cfg->consume_type)
	{
		case TRAIN_CONSUME_TYPE_BIND_COIN:
			{
				if (m_role_module_mgr->GetMoney()->UseCoinBind(cfg->bind_coin_consume * num, __FUNCTION__))
				{
					add_exp = cfg->get_exp * num;
				}
				else
				{
					m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				}
			}
			break;
		case TRAIN_CONSUME_TYPE_ITEM:
			{
				if (m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->item_id, cfg->train_ball_consume * num, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, true))
				{
					add_exp = cfg->train_ball_exp * num;
				}
			}
			break;
		default:
			{
				return -__LINE__;
			}
	}

	if (add_exp <= 0)
	{
		return -__LINE__;
	}

	////////////////////////////////////////////  给予   //////////////////////////////////////////////

	int old_train_level = train.level;
	train.exp += add_exp;
	bool level_up = false;
	while (NULL != cfg && cfg->max_exp > 0 && train.exp >= cfg->max_exp && train.level < max_level && role_level >= cfg->need_role_level)
	{
		train.exp -= cfg->max_exp;
		++train.level;
		cfg = LOGIC_CONFIG->GetTrainConfig()->GetPetTrainCfg2(type, train.level + 1);
		level_up = true;
	}

	if (level_up)
	{
		EventHandler::Instance().OnRoleTrainPetSkillLevelUp(m_role_module_mgr->GetRole(), type, train.level);
	}

	this->SendAllInfo();

	return 0;
}

int Train::OnPartnerTrainUp(int type, int num)
{
	if (!LOGIC_CONFIG->GetTrainConfig()->IsPartnerTrainCfgType(type))
	{
		return -__LINE__;
	}

	RoleCommonSavePartnerTrainData * train_ptr = this->GetRoleCommonSavePartnerTrainData(type);
	if (train_ptr == NULL)
	{
		return -__LINE__;
	}
	RoleCommonSavePartnerTrainData & train = *train_ptr;

	int max_level = LOGIC_CONFIG->GetTrainConfig()->GetPartnerTrainCfgMaxLevel(type);
	const PartnerTrainCfg * cfg = LOGIC_CONFIG->GetTrainConfig()->GetPartnerTrainCfg(type, train.level + 1);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	if (train.level >= max_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SKILL_LEVEL_LIMIT);
		return -__LINE__;
	}

	if (cfg->max_exp <= 0)
	{
		return -__LINE__;
	}

	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	if (role_level < cfg->need_role_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return -__LINE__;
	}

	//////////////////////////////////////////////  消耗   //////////////////////////////////////////////
	int add_exp = 0;
	switch (cfg->consume_type)
	{
		case TRAIN_CONSUME_TYPE_BIND_COIN:
			{
				if (m_role_module_mgr->GetMoney()->UseCoinBind(cfg->bind_coin_consume * num, __FUNCTION__))
				{
					add_exp = cfg->get_exp * num;
				}
				else
				{
					m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				}
			}
			break;
		case TRAIN_CONSUME_TYPE_ITEM:
			{
				if (m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->item_id, cfg->train_ball_consume * num, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, true))
				{
					add_exp = cfg->train_ball_exp * num;
				}
			}
			break;
		default:
			{
				return -__LINE__;
			}
	}

	if (add_exp <= 0)
	{
		return -__LINE__;
	}

	////////////////////////////////////////////  给予   //////////////////////////////////////////////

	int old_train_level = train.level;
	train.exp += add_exp;
	bool level_up = false;
	while (NULL != cfg && cfg->max_exp > 0 && train.exp >= cfg->max_exp && train.level < max_level && role_level >= cfg->need_role_level)
	{
		train.exp -= cfg->max_exp;
		++train.level;
		cfg = LOGIC_CONFIG->GetTrainConfig()->GetPartnerTrainCfg(type, train.level + 1);
		level_up = true;
	}

	if (level_up)
	{
		EventHandler::Instance().OnRoleTrainPartnerSkillLevelUp(m_role_module_mgr->GetRole());
	}

	this->SendAllInfo();

	return 0;
}

std::map<int, int> Train::GetPetTrainUp2Map()
{
	std::map<int, int> ret;
	std::set<int> partner_train_type = LOGIC_CONFIG->GetTrainConfig()->GetAllPetTrainCfg2Type();
	for (std::set<int>::iterator iter = partner_train_type.begin(); iter != partner_train_type.end(); ++iter)
	{
		int train_type = *iter;
		RoleCommonSavePetTrainData2 * train_data = m_role_module_mgr->GetTrain()->GetRoleCommonSavePetTrainData2(train_type);
		if (train_data == NULL)
		{
			continue;
		}
		ret[train_type] = train_data->level;
	}

	return ret;
}

std::map<int, int> Train::GetPetTrainUp2MapByLevel(int role_level)
{
	std::map<int, int> ret;
	std::set<int> pet_train_type = LOGIC_CONFIG->GetTrainConfig()->GetAllPetTrainCfg2Type();
	for (std::set<int>::iterator iter = pet_train_type.begin(); iter != pet_train_type.end(); ++iter)
	{
		int train_type = *iter;
		RoleCommonSavePetTrainData2 * train_data = m_role_module_mgr->GetTrain()->GetRoleCommonSavePetTrainData2(train_type);
		if (train_data == NULL)
		{
			continue;
		}
		ret[train_type] = GetMin(static_cast<int>(train_data->level), LOGIC_CONFIG->GetTrainConfig()->GetPetTrain2MaxLevelByRoleLevel(train_type, role_level));
	}

	return ret;
}

RoleCommonSavePetTrainData2 * Train::GetRoleCommonSavePetTrainData2(int type, bool set_dirty)
{
	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_PET_TRAIN_2);
	if (rcs == NULL)
	{
		return NULL;
	}

	RoleCommonSavePetTrainData2 * xun_bao_info = rcs->GetTypeData<RoleCommonSavePetTrainData2>(type, true);
	if (xun_bao_info == NULL)
	{
		return NULL;
	}

	if (set_dirty)
	{
		rcs->SetDataChange(type);
	}

	return xun_bao_info;
}

RoleCommonSavePartnerTrainData * Train::GetRoleCommonSavePartnerTrainData(int type, bool set_dirty)
{
	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_PARTNER_TRAIN);
	if (rcs == NULL)
	{
		return NULL;
	}

	RoleCommonSavePartnerTrainData * xun_bao_info = rcs->GetTypeData<RoleCommonSavePartnerTrainData>(type, true);
	if (xun_bao_info == NULL)
	{
		return NULL;
	}

	if (set_dirty)
	{
		rcs->SetDataChange(type);
	}

	return xun_bao_info;
}

void Train::SendAllInfo()
{
	Protocol::SCTrainAllInfo info;

	info.pet_train.level = m_param.pet_train.level;
	info.pet_train.exp = m_param.pet_train.exp;
	info.pet_train.type = TRAIN_TYPE_PET;

	info.count = 0;
	for (int i = 0; info.count < ARRAY_LENGTH(info.train_list) && i < TARIN_TYPE_MAX; ++i)
	{
		Protocol::SCTrainAllInfo::TrainData & ptc_con = info.train_list[info.count++];
		ptc_con.type = i;
		ptc_con.level = m_param.train_list[i].level;
		ptc_con.exp = m_param.train_list[i].exp;
	}

	std::set<int> partner_train_type = LOGIC_CONFIG->GetTrainConfig()->GetAllPartnerTrainCfgType();
	for (std::set<int>::iterator iter = partner_train_type.begin(); info.count < ARRAY_LENGTH(info.train_list) && iter != partner_train_type.end(); ++iter)
	{
		int train_type = *iter;
		RoleCommonSavePartnerTrainData * train_data = m_role_module_mgr->GetTrain()->GetRoleCommonSavePartnerTrainData(train_type);
		if (train_data == NULL)
		{
			continue;
		}

		Protocol::SCTrainAllInfo::TrainData & ptc_con = info.train_list[info.count++];
		ptc_con.type = train_type;
		ptc_con.level = train_data->level;
		ptc_con.exp = train_data->exp;
	}

	std::set<int> pet_train_type = LOGIC_CONFIG->GetTrainConfig()->GetAllPetTrainCfg2Type();
	for (std::set<int>::iterator iter = pet_train_type.begin(); info.count < ARRAY_LENGTH(info.train_list) && iter != pet_train_type.end(); ++iter)
	{
		int train_type = *iter;
		RoleCommonSavePetTrainData2 * train_data = m_role_module_mgr->GetTrain()->GetRoleCommonSavePetTrainData2(train_type);
		if (train_data == NULL)
		{
			continue;
		}
		Protocol::SCTrainAllInfo::TrainData & ptc_con = info.train_list[info.count++];
		ptc_con.type = train_type;
		ptc_con.level = train_data->level;
		ptc_con.exp = train_data->exp;
	}


	EngineAdapter::Instance().NetSendSerializeMsg2(m_role_module_mgr->GetRole()->GetNetId(), &info);
}

void Train::SendPromotedInfo()
{
	Protocol::SCTrainInfoPromoted info;
	info.physical_dec_damage = m_attrs_add.m_attrs[BATTLE_ATTR_PHYSICAL_DEC_DAMAGE];
	info.magic_dec_damage = m_attrs_add.m_attrs[BATTLE_ATTR_MAGIC_DEC_DAMAGE];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Train::SendRedDotFlagInfo()
{
	Protocol::SCTrainRedDotFlagInfo info;
	info.pet_train_red_dot_flag = m_param.pet_train_red_dot_flag;
	info.train_red_dot_flag = m_param.train_red_dot_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

int Train::GetRoleTrainSkillLevelNum(int skill_level)
{
	if(skill_level <= 0) return 0;

	int count = 0;
	for (int i = 0; i < TARIN_TYPE_MAX; i++)
	{
		if (m_param.train_list[i].level >= skill_level)
		{
			count++;
		}
	}

	return count;
}

int Train::GetRolePetTrainSkillLevel()
{
	return m_param.pet_train.level;
}

int Train::GetMaxTrainLevel()
{
	int max_train_level = 0;
	for (int i = 0; i < TARIN_TYPE_MAX; i++)
	{
		if (max_train_level < m_param.train_list[i].level)
		{
			max_train_level = m_param.train_list[i].level;
		}
	}
	if (max_train_level < m_param.pet_train.level)
	{
		max_train_level = m_param.pet_train.level;
	}
	
	return max_train_level;
}

void Train::CheckUpLevelItem()
{
	if (m_role_module_mgr->GetCommonData().is_check_train_uplevel_item != 0)
	{
		return;
	}

	m_role_module_mgr->GetCommonData().is_check_train_uplevel_item = 1;

	const TrainOtherCfg & other_cfg = LOGIC_CONFIG->GetTrainConfig()->GetOtherCfg();
	std::set<int> xiu_lian_type_list = LOGIC_CONFIG->GetTrainConfig()->GetTrainTypeBySkillType(TRAIN_SKILL_TYPE_XIU_LIAN);
	
	int count = 0;
	for (std::set<int>::iterator iter = xiu_lian_type_list.begin(); iter != xiu_lian_type_list.end(); ++iter)
	{
		int type = *iter;
		if (type < 0 || type >= TRAIN_ENG)
		{
			continue;
		}

		TrainData & train = m_param.train_list[type];
		gamelog::g_log_train.printf(LL_INFO, "%s line:%d | role[%d %s] TrainData[type:%d level:%d exp:%d]", __FUNCTION__, __LINE__,
					    m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), type, train.level, train.exp);

		for (int i = 0; i <= train.level; ++i)
		{
			const TrainCfg * cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(type, i);
			if (NULL == cfg || cfg->train_ball_exp <= 0)
			{
				continue;
			}

			if (other_cfg.item_ret == cfg->item_id)
			{
				count += cfg->max_exp / cfg->train_ball_exp * cfg->train_ball_consume;
			}
		}

		const TrainCfg * cfg = LOGIC_CONFIG->GetTrainConfig()->GetTrainCfg(type, train.level + 1);
		if (cfg != NULL && cfg->train_ball_exp > 0)
		{
			count += (train.exp + cfg->train_ball_exp - 1) / cfg->train_ball_exp * cfg->train_ball_consume;
		}		
	}
	int old_num = m_role_module_mgr->GetCommonData().train_uplevel_comsume_item_num;	
	if (count > old_num)
	{
		m_role_module_mgr->GetCommonData().train_uplevel_comsume_item_num = count;
	}

	gamelog::g_log_train.printf(LL_INFO, "%s line:%d | role[%d %s] cal_num:%d old_num:%d curr_num:%d", __FUNCTION__, __LINE__,
				    m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), count, old_num, m_role_module_mgr->GetCommonData().train_uplevel_comsume_item_num);
}

void Train::OnRoleChangeProf()
{
	std::set<int> xiu_lian_type_list = LOGIC_CONFIG->GetTrainConfig()->GetTrainTypeBySkillType(TRAIN_SKILL_TYPE_XIU_LIAN);	
	for (std::set<int>::iterator iter = xiu_lian_type_list.begin(); iter != xiu_lian_type_list.end(); ++iter)
	{
		int type = *iter;
		if (type < 0 || type >= TRAIN_ENG)
		{
			continue;;
		}

		TrainData & train = m_param.train_list[type];
		gamelog::g_log_train.printf(LL_INFO, "%s line:%d | role[%d %s] TrainData[type:%d level:%d exp:%d]", __FUNCTION__, __LINE__,
					    m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), type, train.level, train.exp);

		train.Reset();
	}

	this->SendAllInfo();
}

void Train::SynHuaShenTrainData(TrainData * pet_train, TrainData * train_list, int count)
{
	if(NULL == pet_train || NULL == train_list || count <= 0) return;

	*pet_train = m_param.pet_train;
	memcpy(train_list, m_param.train_list, sizeof(TrainData) * count);
}
