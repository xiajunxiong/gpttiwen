#include "xiuwei.hpp"
#include "xiuweiconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/event/eventhandler.hpp"

#include "servercommon/errornum.h"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "engineadapter.h"
#include "gamelog.h"

XiuWei::XiuWei()
{

}

XiuWei::~XiuWei()
{

}

void XiuWei::Init(RoleModuleManager *role_module_manager, const XiuWeiParam &param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;
}

void XiuWei::GetOtherInitParam(XiuWeiParam *param)
{
	*param = m_param;
}

void XiuWei::OnLogin()
{
	this->SendInfo();
	this->SendDanYaoInfo();
}

void XiuWei::OnDayChange()
{
	m_param.normal_dan_day_times = 0;
	m_param.high_dan_day_times = 0;
	this->SendDanYaoInfo();
}

void XiuWei::OnClientReq(Protocol::CSXiuWeiReq * _req)
{
	if (NULL == _req)
		return;

	switch (_req->req_type)
	{
	case Protocol::XIU_WEI_REQ_TYPE_GET_INFO:
		{
			this->SendInfo();
			this->SendDanYaoInfo();
		}
		break;
	case Protocol::XIU_WEI_REQ_TYPE_UP_LEVEL:
		{
			this->UpLevel(_req->param1, _req->param2);
		}
		break;
	case Protocol::XIU_WEI_REQ_TYPE_ONE_KEY_UP:
		{
			this->OneKeyUpLevel(_req->param1, _req->param2);
		}
		break;
	default:
		break;
	}
}

const AttributeList &  XiuWei::ReCalcAttr(AttributeList &base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		// 攻击修炼
		this->ReCalcAttrByXiuWeiType(XIU_WEI_TYPE_ROLE_ATK, m_attrs_add, base_add);
		// 防御修炼
		this->ReCalcAttrByXiuWeiType(XIU_WEI_TYPE_ROLE_DEF, m_attrs_add, base_add);
	}
	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

const AttributeList &  XiuWei::ReCalcPetAttr(AttributeList &base_add, int pet_level)
{
	m_attrs_add_pet.Reset();
	// 攻击修炼
	this->ReCalcAttrByXiuWeiType(XIU_WEI_TYPE_PET_ATK, m_attrs_add_pet, base_add, pet_level);

	// 防御修炼
	this->ReCalcAttrByXiuWeiType(XIU_WEI_TYPE_PET_DEF, m_attrs_add_pet, base_add, pet_level);

	base_add.Add(m_attrs_add_pet.m_attrs);
	return m_attrs_add_pet;
}

const AttributeList &  XiuWei::ReCalcPartnerAttr(AttributeList &base_add, int partner_level)
{
	m_attrs_add_partner.Reset();
	// 攻击修炼
	this->ReCalcAttrByXiuWeiType(XIU_WEI_TYPE_PARTNER_ATK, m_attrs_add_partner, base_add, partner_level);

	// 防御修炼
	this->ReCalcAttrByXiuWeiType(XIU_WEI_TYPE_PARTNER_DEF, m_attrs_add_partner, base_add, partner_level);
	base_add.Add(m_attrs_add_partner.m_attrs);

	return m_attrs_add_partner;
}

void XiuWei::OnGetBattleEffect(int xiu_wei_character_type, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	if(NULL == out_other_sp_num || NULL == out_other_sp_list) return;

	switch (xiu_wei_character_type)
	{
		case XIU_WEI_CHARACTER_TYPE_ROLE:
		{
			this->GetBattleEffect(XIU_WEI_TYPE_ROLE_ATK, max_other_sp_num, out_other_sp_num, out_other_sp_list);
			this->GetBattleEffect(XIU_WEI_TYPE_ROLE_DEF, max_other_sp_num, out_other_sp_num, out_other_sp_list);
		}
		break;
		case XIU_WEI_CHARACTER_TYPE_PET:
		{
			this->GetBattleEffect(XIU_WEI_TYPE_PET_ATK, max_other_sp_num, out_other_sp_num, out_other_sp_list);
			this->GetBattleEffect(XIU_WEI_TYPE_PET_DEF, max_other_sp_num, out_other_sp_num, out_other_sp_list);
		}
		break;
		case XIU_WEI_CHARACTER_TYPE_PARTNER:
		{
			this->GetBattleEffect(XIU_WEI_TYPE_PARTNER_ATK, max_other_sp_num, out_other_sp_num, out_other_sp_list);
			this->GetBattleEffect(XIU_WEI_TYPE_PARTNER_DEF, max_other_sp_num, out_other_sp_num, out_other_sp_list);
		}
		break;
		default:
		break;
	}
}

void XiuWei::SendInfo()
{
	Protocol::SCXiuWeiInfo info;

	UNSTD_STATIC_CHECK(sizeof(info.info) == sizeof(m_param.info_arr));
	memcpy(info.info, m_param.info_arr, sizeof(info.info));

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char *)&info, sizeof(info));
}

void XiuWei::SendInfoChange(int _xiu_wei_type)
{
	if (_xiu_wei_type < 0 || _xiu_wei_type >= XIU_WEI_TYPE_MAX)
		return;

	Protocol::SCXiuWeiInfoChange change;

	change.type = _xiu_wei_type;
	UNSTD_STATIC_CHECK(sizeof(change.info) == sizeof(m_param.info_arr[0]));
	memcpy(&change.info, &m_param.info_arr[_xiu_wei_type], sizeof(change.info));

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char *)&change, sizeof(change));
}

void XiuWei::SendDanYaoInfo()
{
	Protocol::SCXiuWeiDanYaoLimitInfo info;

	info.high_dan_day_times = m_param.high_dan_day_times;
	info.normal_dan_day_times = m_param.normal_dan_day_times;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char *)&info, sizeof(info));
}

bool XiuWei::IsRoleType(int _xiu_wei_type)
{
	UNSTD_STATIC_CHECK(6 == XIU_WEI_TYPE_MAX);
	switch (_xiu_wei_type)
	{
	case XIU_WEI_TYPE_ROLE_ATK:
	case XIU_WEI_TYPE_ROLE_DEF:
		return true;
	}

	return false;
}

bool XiuWei::IsPetType(int _xiu_wei_type)
{
	UNSTD_STATIC_CHECK(6 == XIU_WEI_TYPE_MAX);
	switch (_xiu_wei_type)
	{
	case XIU_WEI_TYPE_PET_ATK:
	case XIU_WEI_TYPE_PET_DEF:
		return true;
	}

	return false;
}

bool XiuWei::IsParnerType(int _xiu_wei_type)
{
	UNSTD_STATIC_CHECK(6 == XIU_WEI_TYPE_MAX);
	switch (_xiu_wei_type)
	{
	case XIU_WEI_TYPE_PARTNER_ATK:
	case XIU_WEI_TYPE_PARTNER_DEF:
		return true;
	}

	return false;
}

void XiuWei::UpLevel(int _xiu_wei_type, int _dan_yao_type)
{
	if (_xiu_wei_type < XIU_WEI_TYPE_ROLE_ATK || _xiu_wei_type >= XIU_WEI_TYPE_MAX ||
		_dan_yao_type < XIU_WEI_DAN_TYPE_NORMAL || _dan_yao_type >= XIU_WEI_DAN_TYPE_MAX)
	{
		return;
	}

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	const XiuWeiOtherCfg & other_cfg = LOGIC_CONFIG->GetXiuWeiConfig()->GetOtherCfg();
	int fly_up_grade = m_role_module_mgr->GetFlyUpSystem()->GetFlyUpFlag();
	if (other_cfg.need_feisheng > fly_up_grade)
	{
		role->NoticeNum(errornum::EN_HAVE_NOT_REACH_ACTIVE_CONDITION);
		return; 
	}

	// 再检查是否满级了
	int max_level = LOGIC_CONFIG->GetXiuWeiConfig()->GetXiuWeiTypeMaxLevel(_xiu_wei_type);
	if (m_param.info_arr[_xiu_wei_type].level >= max_level)
	{
		role->NoticeNum(errornum::EN_LEVEL_MAX_LIMIT);
		return;
	}

	// 先计算一下使用后的等级和经验
	const XiuWeiAttrCfg * attr_cfg = LOGIC_CONFIG->GetXiuWeiConfig()->GetAttrCfg(_xiu_wei_type, m_param.info_arr[_xiu_wei_type].level + 1);
	if (NULL == attr_cfg)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 检查一下是非到达当前等级上限
	if (m_role_module_mgr->GetRole()->GetLevel() < attr_cfg->open_level)
	{
		role->NoticeNum(errornum::EN_CURR_LEVEL_LIMIT);
		return;
	}

	// 先检查是否有对应的道具
	int need_use_item_id = 0;
	int add_exp = 0;
	switch (_dan_yao_type)
	{
	case XIU_WEI_DAN_TYPE_NORMAL:
		{
			// 需要先判断次数是否足够
			if (m_param.normal_dan_day_times + 1 > other_cfg.normal_use_times)
			{
				role->NoticeNum(errornum::EN_TODAY_USE_COUNT_LIMIT);
				return;
			}
			need_use_item_id = other_cfg.cost1;
			add_exp = other_cfg.cost1_exp;
		}
		break;
	case XIU_WEI_DAN_TYPE_HIGH:
		{
			// 需要先判断次数是否足够
			if (m_param.high_dan_day_times + 1 > other_cfg.use_times)
			{
				role->NoticeNum(errornum::EN_TODAY_USE_COUNT_LIMIT);
				return;
			}
			need_use_item_id = other_cfg.cost2;
			add_exp = other_cfg.cost2_exp;
		}
		break;
	default:
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		}
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(need_use_item_id , 1))
	{
		role->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}

	// 消耗
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(need_use_item_id, 1, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	// 计算新的经验和等级
	short old_level = m_param.info_arr[_xiu_wei_type].level;
	int old_exp = m_param.info_arr[_xiu_wei_type].exp;
	short new_level = m_param.info_arr[_xiu_wei_type].level;
	int new_exp = m_param.info_arr[_xiu_wei_type].exp;

	while (add_exp > 0)
	{
		if (new_exp + add_exp >= attr_cfg->xiuwei_exp)
		{
			++new_level;
			new_exp = 0;
			add_exp -= attr_cfg->xiuwei_exp - new_exp;

			attr_cfg = LOGIC_CONFIG->GetXiuWeiConfig()->GetAttrCfg(_xiu_wei_type, new_level + 1);
			if (NULL == attr_cfg)
			{
				break;
			}
		}
		else
		{
			new_exp += add_exp;
			add_exp = 0;
		}
	}

	//生效
	m_param.info_arr[_xiu_wei_type].level = new_level;
	m_param.info_arr[_xiu_wei_type].exp = new_exp;
	if (need_use_item_id == other_cfg.cost2)
		++m_param.high_dan_day_times;
	else 
		++m_param.normal_dan_day_times;

	this->SendDanYaoInfo();


	bool level_up = false;
	if (old_level != new_level)
		level_up = true;

	// 重算战力
	if (IsRoleType(_xiu_wei_type) && level_up)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_XIU_WEI, __FUNCTION__);
	}
	else if (IsPetType(_xiu_wei_type) && level_up)
	{
		EventHandler::Instance().OnXiuWeiPetTypeLevelUp(role);
	}
	else if (IsParnerType(_xiu_wei_type) && level_up)
	{
		EventHandler::Instance().OnXiuWeiParnerTypeLevelUp(role);
	}

	this->SendInfoChange(_xiu_wei_type);

	// 日志
	gamelog::g_log_xiu_wei.printf(LL_INFO, "%s user[%d %s]  xiu_wei_type:%d dan_yao_type:%d  normal_day_times:%d  high_dan_day_times:%d  old_exp:%d old_level:%d  new_exp:%d new_level:%d",
		__FUNCTION__, role->GetUID(), role->GetName(), _xiu_wei_type, _dan_yao_type, m_param.normal_dan_day_times , m_param.high_dan_day_times,
		old_exp, old_level, new_exp, new_level);
}

void XiuWei::OneKeyUpLevel(int _xiu_wei_type, int _dan_yao_type)
{
	if (_xiu_wei_type < XIU_WEI_TYPE_ROLE_ATK || _xiu_wei_type >= XIU_WEI_TYPE_MAX ||
		_dan_yao_type < XIU_WEI_DAN_TYPE_NORMAL || _dan_yao_type >= XIU_WEI_DAN_TYPE_MAX)
	{
		return;
	}
	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
	{
		return;
	}

	const XiuWeiOtherCfg & other_cfg = LOGIC_CONFIG->GetXiuWeiConfig()->GetOtherCfg();
	int fly_up_grade = m_role_module_mgr->GetFlyUpSystem()->GetFlyUpFlag();
	if (other_cfg.need_feisheng > fly_up_grade)
	{
		role->NoticeNum(errornum::EN_HAVE_NOT_REACH_ACTIVE_CONDITION);
		return;
	}

	// 再检查是否满级了
	int max_level = LOGIC_CONFIG->GetXiuWeiConfig()->GetXiuWeiTypeMaxLevel(_xiu_wei_type);
	if (m_param.info_arr[_xiu_wei_type].level >= max_level)
	{
		role->NoticeNum(errornum::EN_LEVEL_MAX_LIMIT);
		return;
	}

	// 先计算一下使用后的等级和经验
	const XiuWeiAttrCfg * attr_cfg = LOGIC_CONFIG->GetXiuWeiConfig()->GetAttrCfg(_xiu_wei_type, m_param.info_arr[_xiu_wei_type].level + 1);
	if (NULL == attr_cfg)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 检查一下是非到达当前等级上限
	if (m_role_module_mgr->GetRole()->GetLevel() < attr_cfg->open_level)
	{
		role->NoticeNum(errornum::EN_CURR_LEVEL_LIMIT);
		return;
	}

	// 先检查是否有对应的道具
	int need_use_item_id = 0;
	int add_exp = 0;
	int last_use_num = 0;
	switch (_dan_yao_type)
	{
		case XIU_WEI_DAN_TYPE_NORMAL:
		{
			// 需要先判断次数是否足够
			if (m_param.normal_dan_day_times >= other_cfg.normal_use_times)
			{
				role->NoticeNum(errornum::EN_TODAY_USE_COUNT_LIMIT);
				return;
			}
			need_use_item_id = other_cfg.cost1;
			add_exp = other_cfg.cost1_exp;
			last_use_num = other_cfg.normal_use_times - m_param.normal_dan_day_times;
		}
		break;
		case XIU_WEI_DAN_TYPE_HIGH:
		{
			// 需要先判断次数是否足够
			if (m_param.high_dan_day_times >= other_cfg.use_times)
			{
				role->NoticeNum(errornum::EN_TODAY_USE_COUNT_LIMIT);
				return;
			}
			need_use_item_id = other_cfg.cost2;
			add_exp = other_cfg.cost2_exp;
			last_use_num = other_cfg.use_times - m_param.high_dan_day_times;
		}
		break;
		default:
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		}
		return;
	}

	if (add_exp <= 0)
	{
		return;
	}

	//升级所需经验
	int need_exp = attr_cfg->xiuwei_exp - m_param.info_arr[_xiu_wei_type].exp;
	//当前拥有的道具数量
	int has_item_count = m_role_module_mgr->GetKnapsack()->Count(need_use_item_id);
	if (has_item_count <= 0)
	{
		role->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}
	//当前能用的最大道具数量
	int can_use_item_count = GetMin(last_use_num, has_item_count);
	//升级所需的道具数量
	int up_level_need_item_count = need_exp / add_exp;
	if (0 != (need_exp % add_exp))
	{
		up_level_need_item_count += 1;
	}
	//实际消耗数量
	int real_consume_item_count = GetMin(can_use_item_count, up_level_need_item_count);
	if (real_consume_item_count <= 0)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(need_use_item_id, real_consume_item_count, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	// 计算新的经验和等级
	short old_level = m_param.info_arr[_xiu_wei_type].level;
	int old_exp = m_param.info_arr[_xiu_wei_type].exp;
	short new_level = m_param.info_arr[_xiu_wei_type].level;
	int new_exp = m_param.info_arr[_xiu_wei_type].exp;

	//当前需增加的经验
	int real_add_exp = real_consume_item_count * add_exp;
	while (real_add_exp > 0)
	{
		if (new_exp + real_add_exp >= attr_cfg->xiuwei_exp)
		{
			++new_level;
			new_exp = 0;
			real_add_exp -= attr_cfg->xiuwei_exp - new_exp;

			attr_cfg = LOGIC_CONFIG->GetXiuWeiConfig()->GetAttrCfg(_xiu_wei_type, new_level + 1);
			if (NULL == attr_cfg)
			{
				break;
			}
		}
		else
		{
			new_exp += real_add_exp;
			real_add_exp = 0;
		}
	}

	//生效
	m_param.info_arr[_xiu_wei_type].level = new_level;
	m_param.info_arr[_xiu_wei_type].exp = new_exp;
	if (need_use_item_id == other_cfg.cost2)
	{
		m_param.high_dan_day_times += real_consume_item_count;
	}
	else
	{
		m_param.normal_dan_day_times += real_consume_item_count;
	}

	this->SendDanYaoInfo();
	bool level_up = (old_level != new_level) ? true : false;

	// 重算战力
	if (IsRoleType(_xiu_wei_type) && level_up)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_XIU_WEI, __FUNCTION__);
	}
	else if (IsPetType(_xiu_wei_type) && level_up)
	{
		EventHandler::Instance().OnXiuWeiPetTypeLevelUp(role);
	}
	else if (IsParnerType(_xiu_wei_type) && level_up)
	{
		EventHandler::Instance().OnXiuWeiParnerTypeLevelUp(role);
	}

	this->SendInfoChange(_xiu_wei_type);

	// 日志
	gamelog::g_log_xiu_wei.printf(LL_INFO, "%s user[%d %s]  xiu_wei_type:%d dan_yao_type:%d  normal_day_times:%d  high_dan_day_times:%d  old_exp:%d old_level:%d  new_exp:%d new_level:%d",
		__FUNCTION__, role->GetUID(), role->GetName(), _xiu_wei_type, _dan_yao_type, m_param.normal_dan_day_times, m_param.high_dan_day_times,
		old_exp, old_level, new_exp, new_level);
}

void XiuWei::ReCalcAttrByXiuWeiType(int _xiu_wei_type, AttributeList &base_add, const AttributeList base_attr_list, int level_by_type)
{
	if (_xiu_wei_type < 0 || _xiu_wei_type >= XIU_WEI_TYPE_MAX)
		return;

	const XiuWeiAttrCfg * cfg = LOGIC_CONFIG->GetXiuWeiConfig()->GetAttrCfg(_xiu_wei_type, m_param.info_arr[_xiu_wei_type].level);
	if (NULL != cfg)
	{
		// 伤害加成
		base_add.m_attrs[BATTLE_ATTR_PHYSICAL_INC_DAMAGE] += cfg->dam_add;
		base_add.m_attrs[BATTLE_ATTR_MAGIC_INC_DAMAGE] += cfg->dam_add;
		// 效果命中
		base_add.m_attrs[BATTLE_ATTR_CONTROL_HIT] += cfg->xiaoguo_add;
		// 伤害减免
		base_add.m_attrs[BATTLE_ATTR_PHYSICAL_DEC_DAMAGE] += cfg->dedam_def;
		base_add.m_attrs[BATTLE_ATTR_MAGIC_DEC_DAMAGE] += cfg->dedam_def;
		// 效果抵抗
		base_add.m_attrs[BATTLE_ATTR_ANTI_CONTROL_RATE] += cfg->xiaoguo_def;
		// 生命加成
		base_add.m_attrs[BATTLE_ATTR_MAX_HP] += cfg->hp_add;

		if(cfg->sp_id > 0)
		{
			GetSpecialEffectAttr(cfg->sp_id, base_add, base_attr_list, m_role_module_mgr->GetRole()->GetLevel(), level_by_type);
		}
	}
}

void XiuWei::GetBattleEffect(int _xiu_wei_type, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)
{
	if (_xiu_wei_type < 0 || _xiu_wei_type >= XIU_WEI_TYPE_MAX)
		return;

	const XiuWeiAttrCfg * cfg = LOGIC_CONFIG->GetXiuWeiConfig()->GetAttrCfg(_xiu_wei_type, m_param.info_arr[_xiu_wei_type].level);
	if (NULL != cfg && cfg->sp_id > 0)
	{
		//加面板的特效不收集入战斗
		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(cfg->sp_id);
		if (NULL == spe_cfg) return;

		bool is_has_battle_effect = false;
		for (int j = 0; j < spe_cfg->effect_num && j < EFFECT_DETAIL_NUM; ++j)
		{
			const SpecialEffectKey & key = spe_cfg->effect_detail_list[j];
			if (key.effect_type == SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

			is_has_battle_effect = true;
			break;
		}

		if (!is_has_battle_effect)
		{
			return;
		}

		if (*out_other_sp_num < max_other_sp_num)
		{
			out_other_sp_list[(*out_other_sp_num)++] = cfg->sp_id;
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

