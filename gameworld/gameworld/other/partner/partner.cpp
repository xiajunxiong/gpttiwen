#include "partner.hpp"
#include "partnerconfig.hpp"
#include "partnerequipmentconfig.hpp"
#include "gamelog.h"
#include "protocol/msgpartner.h"
#include "config/logicconfigmanager.hpp"
#include "other/train/train.hpp"
#include "other/train/trainconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "item/itemextern.hpp"
#include "item/knapsack.h"
#include "item/itempool.h"
#include "task/taskrecorder.h"
#include "global/team/teammanager.hpp"
#include "global/team/team.hpp"
#include "other/route/mailroute.hpp"
#include "other/event/eventhandler.hpp"
#include "item/partnernatalcard/partnernatalcard.hpp"
#include "servercommon/platformbattledef.hpp"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "global/platformbattle/platformbattle.h"
#include "item/expense/norexitem.hpp"
#include "item/elementcrystal/elementcrystal.hpp"
#include "equipment/equipmentmanager.h"
#include "battle/battle_manager_local.hpp"
#include "world.h"
#include "other/skin/skinconfig.hpp"
#include "config/otherconfig/roleattrpelletconfig.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "mysterystoreconfig.hpp"
#include "global/rank/rankmanager.hpp"
#include "protocol/msgrole.h"
#include "servercommon/performancecatch.hpp"
#include "item/money.h"
#include "skill/skillpool.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "other/roleattribute/roleexpconfig.hpp"
#include "servercommon/rolecommonsavedef.h"
#include "other/partner/partnergifttreeconfig.hpp"
#include "other/roleattrpellet/roleattrpellet.hpp"
#include "item/spiritpact/spiritpactitem.hpp"
#include "other/funopen/funopen.hpp"
#include "other/collection/collection.hpp"
#include "other/xiuwei/xiuwei.hpp"
#include "servercommon/protobufmanager.hpp"
#include "other/fuyu/fuyumanager.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/starchart/starchart.h"

//升品等级限制
#define CHECK_QUALITY_LEVEL(_role_level,_need_level,_notice)																													\
do																																										\
{																																										\
	if (_role_level < _need_level)																																		\
	{																																									\
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), _notice, _need_level);													\
		if (length > 0)																																					\
		{																																								\
			Protocol::SCSystemMsg sm;																																	\
			int sendlen = 0;																																			\
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))											\
			{																																							\
				EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*) &sm, sendlen);												\
			}																																							\
		}																																								\
			return false;																																				\
	}																																									\
} while (0)

Partner::Partner() : m_already_send_all_attr(false), m_partner_count(0), once_init(0), m_role_module_mgr(NULL), m_partner_jiban_num(0)
{
	memset(m_platform_battle_partner, 0, sizeof(m_platform_battle_partner));
}

Partner::~Partner()
{
}

void Partner::Init(RoleModuleManager * role_module_manager,
	const PartnerParam & p_partner,
	const NewPartnerParam & new_p_partner,
	const PartnerGiftTreeParam & partner_gift_tree,
	const PartnerSpiritPactParam & partner_pact_param)
{
	m_role_module_mgr = role_module_manager;
	m_partner = p_partner;
	m_new_partner = new_p_partner;
	m_partner_gift_tree_list = partner_gift_tree;
	m_pact_grid_param = partner_pact_param;

	if (m_partner.tranf_id == 0)
	{
		for (int i = 0; i < PARTNER_NUM_MAX; i++)
		{
			NewPartnerBaseInfo &new_param = m_new_partner.partner_base_info[i];
			PartnerBaseInfo &old_param = m_partner.partner_base_info[i];
			new_param.partner_list.partner_id = old_param.partner_list.partner_id;
			new_param.partner_list.is_unlocked = old_param.partner_list.is_unlocked;
			new_param.partner_list.fight_state = old_param.partner_list.fight_state;
			new_param.partner_list.upgrade_lv = old_param.partner_list.upgrade_lv;
			new_param.partner_list.attr_stage = old_param.partner_list.attr_stage;
			memcpy(new_param.partner_list.skill_list, old_param.partner_list.skill_list, sizeof(old_param.partner_list.skill_list));
			memcpy(new_param.partner_list.curr_fetter, old_param.partner_list.curr_fetter, sizeof(old_param.partner_list.curr_fetter));
			memcpy(new_param.partner_list.wash_fetter, old_param.partner_list.wash_fetter, sizeof(old_param.partner_list.wash_fetter));
			for (int j = 0; j < PE_TYPE_MAX; j++)
			{
				new_param.partner_list.equip_list[j].partner_equip_id = old_param.partner_list.equip_list[j].partner_equip_id;
				new_param.partner_list.equip_list[j].unlock_level = old_param.partner_list.equip_list[j].unlock_level;
				new_param.partner_list.equip_list[j].m_voc_list[0].attr_type = old_param.partner_list.equip_list[j].attr_type;
				new_param.partner_list.equip_list[j].m_voc_list[0].attr_value = old_param.partner_list.equip_list[j].attr_value;
			}
			new_param.partner_data = old_param.partner_data;
			new_param.partner_quality_type = old_param.partner_quality_type;
			new_param.old_quality = old_param.old_quality;
			new_param.new_quality = old_param.new_quality;
			new_param.is_init = old_param.is_init;
			new_param.partner_crytal = old_param.partner_crytal;
			new_param.skin_active_status = old_param.skin_active_status;
			new_param.cur_skin_id = old_param.cur_skin_id;
			new_param.capability = old_param.capability;
			new_param.battle_pos = old_param.battle_pos;
			new_param.quality_segment = old_param.quality_segment;
			new_param.is_set_partner_quality_segment_flag = old_param.is_set_partner_quality_segment_flag;
			new_param.biography_open_flag = old_param.biography_open_flag;
			new_param.level = old_param.level;
			new_param.biography_fetch_flag = old_param.biography_fetch_flag;
		}
		memcpy(m_new_partner.synfight_list, m_partner.synfight_list, sizeof(m_partner.synfight_list));
		m_new_partner.store_next_auto_refresh_time = m_partner.store_next_auto_refresh_time;
		for (int i = 0; i < MYSTERY_STORE_MAX_SEQ; i++)
		{
			NewStoreNatalCard &new_data = m_new_partner.store_natal_card[i];
			StoreNatalCard &old_data = m_partner.store_natal_card[i];
			new_data.buy_count = old_data.buy_count;
			new_data.group_id = old_data.group_id;
			new_data.seq = old_data.seq;
			new_data.m_voc_list[0].attr_type = old_data.attr_type;
			new_data.m_voc_list[0].attr_value = old_data.attr_value;
		}
		m_new_partner.new_partner_unlock_flag = m_partner.new_partner_unlock_flag;

		m_new_partner.is_recover_item_flag = m_partner.is_recover_item_flag;
		m_new_partner.is_refresh_store_flag = m_partner.is_refresh_store_flag;
		m_new_partner.store_refresh_num = m_partner.store_refresh_num;
		m_new_partner.store_ver = m_partner.store_ver;
		m_new_partner.next_free_refresh_time = m_partner.next_free_refresh_time;

		m_partner.tranf_id = 1;
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_partner_count = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerCount();
	for (int t_cur = PARTNER_ID_START; t_cur <= m_partner_count && t_cur < PARTNER_NUM_MAX; ++t_cur)
	{
		//增加了伙伴等级，不在跟随角色等级，已经存在线上玩家赋值成角色等级
		int cur_pid = t_cur;
		NewPartnerBaseInfo & partner_info = m_new_partner.partner_base_info[cur_pid];
		if (partner_info.level == 0)
		{
			if (IsPartnerAlreadyActive(cur_pid))
			{
				partner_info.level = m_role_module_mgr->GetRole()->GetLevel();
			}
			else
			{
				partner_info.level = 1;
			}			
		}
	}

	for (int t_cur = PARTNER_ID_START; t_cur <= m_partner_count && t_cur < PARTNER_NUM_MAX; ++t_cur)
	{
		int cur_pid = t_cur;
		NewPartnerBaseInfo &partner_info = m_new_partner.partner_base_info[cur_pid];
		NewPartnerCon & t_contain = partner_info.partner_list;
		if (cur_pid != t_contain.partner_id) t_contain.partner_id = cur_pid;

		int upgrade_lv = t_contain.upgrade_lv;
		while (upgrade_lv > 0)
		{
			if (NULL == LOGIC_CONFIG->GetPartnerConfig()->GetCurGradeAttrCfg(cur_pid, upgrade_lv))
			{
				upgrade_lv -= 1;
			}
			else
			{
				break;
			}
		}
		t_contain.upgrade_lv = upgrade_lv; // 主动降级，因为由于版本更替的原因，伙伴阶数可能超过配置的最大阶数

		// 检查
		if (m_partner.new_partner_unlock_flag == 0)//检查转换出还原奖励
		{
			/*std::vector<ItemConfigData> back_vec;
			for (int solt_idx = 0; solt_idx < PE_TYPE_MAX; solt_idx++)
			{
				PartnerEquipment & quote_equip = t_contain.equip_list[solt_idx];
				if (quote_equip.unlock_level <= 0)continue;
				const std::vector<PartnerEqLimitCfg> * info_cfg = LOGIC_CONFIG->GetPartnerEquipmentConfig()->GetPEUlockLimitVec(t_cur, solt_idx + 1);
				if (info_cfg != NULL)
				{
					//返还解锁奖励
					for (std::vector<PartnerEqLimitCfg>::const_iterator back_it = info_cfg->begin(); back_it != info_cfg->end(); ++back_it)
					{
						if(back_it == info_cfg->begin())continue;
						if (back_it->hole_level > quote_equip.unlock_level)continue;
						back_vec.push_back(ItemConfigData(back_it->hole_need, false, back_it->hole_num));
					}
				}
				const NewPartnerEqLimitCfg * new_info_cfg = LOGIC_CONFIG->GetPartnerEquipmentConfig()->GetNewPEUnlockLimitBegin(t_cur, solt_idx);
				if (new_info_cfg == NULL)continue;
				quote_equip.unlock_level = new_info_cfg->hole_level;//已经解锁的变成新解锁等级
			}
			if (back_vec.size() > 0)m_role_module_mgr->GetKnapsack()->PutList((short)back_vec.size(), &back_vec[0], PUT_REASON_PARTNER_BACK_REWARD);*/
			m_partner.new_partner_unlock_flag = 1;
		}

		for (int solt_idx = 0; solt_idx < PE_TYPE_MAX; solt_idx++)
		{
			NewPartnerEquipment & quote_equip = t_contain.equip_list[solt_idx];
			const ItemBase * item_base = ITEMPOOL->GetItem(quote_equip.partner_equip_id);
			if (item_base == NULL)continue;
			const PartnerNatalCard * natal_card = (const PartnerNatalCard *)item_base;
			//检查命格是否正确属性
			for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
			{
				if (quote_equip.m_voc_list[i].attr_value == 0)continue;
				if (!LOGIC_CONFIG->GetPartnerEquipmentConfig()->CheckAttrTypeAndValue(quote_equip.partner_equip_id, i, quote_equip.m_voc_list[i].attr_type, quote_equip.m_voc_list[i].attr_value))
				{
					//属性对不上
					NetValueItemParam new_param;
					natal_card->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &new_param, PUT_REASON_NO_NOTICE);//重新再随机一次属性
					natal_card->SetRandomNetValueFlag(&new_param);
					for (int voc_idx = 0; voc_idx < MAX_PARTNER_NATAL_CARD_VOC_NUM; voc_idx++)
					{
						quote_equip.m_voc_list[voc_idx].attr_type = new_param.partner_natal_card_param.m_voc_list[voc_idx].attr_type;
						quote_equip.m_voc_list[voc_idx].attr_value = new_param.partner_natal_card_param.m_voc_list[voc_idx].attr_value;
					}
					break;
				}
			}
			quote_equip.unlock_level = MAX_ROLE_LEVEL;
		}

		if (!partner_info.IsInitBitUse(NewPartnerBaseInfo::INIT_BIT_0))
		{
			// 觉醒 和 元素水晶
			const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_info.GetPartnerId());
			if (NULL == basic_cfg) continue;
			partner_info.old_quality = (char)basic_cfg->quality;
			partner_info.new_quality = partner_info.old_quality;
			if (this->IsPartnerAlreadyActive(partner_info.GetPartnerId()))
			{
				partner_info.partner_crytal.current_crytal_id = basic_cfg->crystal_item;

				const ItemBase* element_item = ITEMPOOL->GetItem(basic_cfg->crystal_item);
				if (NULL != element_item)
				{
					const ElementCrystal* element_crystal = (const ElementCrystal*)element_item;
					partner_info.partner_crytal.use_time = element_crystal->GetCrystalLimitTimes();
				}
			}
			else
			{
				partner_info.partner_crytal.current_crytal_id = 0;
				partner_info.partner_crytal.use_time = 0;
				for (int j = 0; j < PARTNER_ELE_MAX; ++j)
				{
					partner_info.partner_crytal.att_list[j] = PARTNER_CRYSTAL_NULL;
				}
			}

			// 技能	
			partner_info.quality_segment = 1;
			partner_info.partner_quality_type = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(partner_info.new_quality);
			NewPartnerCon & param = m_new_partner.partner_base_info[t_cur].partner_list;
			const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
				cur_pid, partner_info.partner_quality_type, partner_info.quality_segment));
			if (NULL == q_cfg) continue;

			for (int idx = 0; idx < PARTNER_COMMON_SKILL_MAX; ++idx)
			{
				if (q_cfg->common_skill_id_list[idx] > 0 && q_cfg->common_skill_lv_list[idx] > 0)
				{
					param.skill_list[idx] = 1;	// 启用技能
				}
				else
				{
					param.skill_list[idx] = 0;
				}
			}
		}
		else if (4 != partner_info.is_set_partner_quality_segment_flag)
		{
			gamelog::g_log_partner.printf(LL_INFO, "%s user[%d, %s] partner_id[%d] old_quality_segment[%d]  old_quality[%d] old_quality_type[%d] old_skill_list[%d, %d, %d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), t_contain.partner_id, partner_info.quality_segment, partner_info.new_quality,
				partner_info.partner_quality_type, t_contain.skill_list[0], t_contain.skill_list[1], t_contain.skill_list[2]);
			partner_info.quality_segment = (0 == partner_info.quality_segment) ? 1 : partner_info.quality_segment;
			partner_info.partner_quality_type = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(partner_info.new_quality);
			const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
				cur_pid, partner_info.partner_quality_type, partner_info.quality_segment));
			if (NULL == q_cfg) continue;

			for (int idx = 0; idx < PARTNER_COMMON_SKILL_MAX; ++idx)
			{
				if (q_cfg->common_skill_id_list[idx] > 0 && q_cfg->common_skill_lv_list[idx] > 0)
				{
					t_contain.skill_list[idx] = 1;	// 启用技能
				}
				else
				{
					t_contain.skill_list[idx] = 0;
				}
			}
			gamelog::g_log_partner.printf(LL_INFO, "%s user[%d, %s] partner_id[%d] new_quality_segment[%d]  new_quality[%d] new_quality_type[%d] new_skill_list[%d, %d, %d]", __FUNCTION__,
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), t_contain.partner_id, partner_info.quality_segment, partner_info.new_quality,
				partner_info.partner_quality_type, t_contain.skill_list[0], t_contain.skill_list[1], t_contain.skill_list[2]);
		}

		partner_info.is_set_partner_quality_segment_flag = 4;
		partner_info.InitBitSet(NewPartnerBaseInfo::INIT_BIT_0);
	}


	for (int t_cur = PARTNER_ID_START; t_cur <= m_partner_count && t_cur < PARTNER_NUM_MAX; ++t_cur)
	{
		int cur_pid = t_cur;
		NewPartnerBaseInfo &partner_info = m_new_partner.partner_base_info[cur_pid];
		NewPartnerCon & t_contain = partner_info.partner_list;
		const PartnerBasicCfg* cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(cur_pid);
		if (NULL != cfg && cfg->quality > partner_info.new_quality)
		{
			int old_quality = partner_info.new_quality;
			partner_info.new_quality = cfg->quality;
			gamelog::g_log_partner.printf(LL_INFO, "Partner::Init Change Quality role[%d,%s] old_quality[%d] new_quality[%d]",
				m_role_module_mgr->GetUid(),
				m_role_module_mgr->GetRoleName(),
				old_quality, partner_info.new_quality);
		}
	}
}

void Partner::InitEnd()
{
	for (int t_cur = PARTNER_ID_START; t_cur <= m_partner_count && t_cur < PARTNER_NUM_MAX; ++t_cur)
	{
		NewPartnerBaseInfo &partner_info = m_new_partner.partner_base_info[t_cur];
		this->ReCalcPartnerAttr(t_cur, false);		// 重算伙伴属性
		/*if (partner_info.cur_skin_id != 0)
		{
			this->PutOnSkin(partner_info.GetPartnerId(), partner_info.cur_skin_id, true);
		}*/
	}

	//重新附加  伙伴--元神契约的属性
	for (int t_cur = PARTNER_ID_START; t_cur <= m_partner_count && t_cur < PARTNER_NUM_MAX; ++t_cur)
	{
		this->ReCalcPactAttr(t_cur, true, false);
	}

	//TODO 临时处理
	RankManager::Instance().SyncPersonRankInfo(m_role_module_mgr->GetRole(), PERSON_RANK_TYPE_CAPABILITY);
	RankManager::Instance().SyncPersonRankInfo(m_role_module_mgr->GetRole(), PERSON_RANK_TYPE_FOUR_MAX_PARTNER);
}

void Partner::GetPartnerParam(PartnerParam * p_partner_param,
	NewPartnerParam * new_p_partner_param,
	PartnerGiftTreeParam * partner_gift_tree_param,
	PartnerSpiritPactParam * partner_pact_param)
{
	*p_partner_param = m_partner;
	*new_p_partner_param = m_new_partner;
	*partner_gift_tree_param = m_partner_gift_tree_list;
	*partner_pact_param = m_pact_grid_param;
}

bool Partner::IsPartnerValid(int partner_id) const
{
	if (partner_id < 0 || partner_id >= PARTNER_NUM_MAX)
	{
		return false;
	}

	return !m_new_partner.partner_base_info[partner_id].IsInvalid();
}

bool Partner::IsPartnerInvalid(int partner_id) const
{
	if (partner_id < 0 || partner_id >= PARTNER_NUM_MAX)
	{
		return true;
	}

	return m_new_partner.partner_base_info[partner_id].IsInvalid();
}

void Partner::Update(time_t now_second)
{
	/*if (now_second >= m_partner.store_next_auto_refresh_time)
	{
		this->OnStoreRefresh(true);
	}*/
}

void Partner::OnPartnerDayChange(unsigned int old_dayid, unsigned int new_dayid)
{
	if (old_dayid == new_dayid) return;

	m_new_partner.store_refresh_num = 0;
	this->SendStoreInfo();
	// 天赋 - 每日送道具 - 被砍掉了
	/*for (int check_id = 0; check_id < m_partner_count && check_id < PARTNER_NUM_MAX; ++check_id)	// 检查每个伙伴
	{
	const PartnerCon & param_con = m_partner.partner_list[check_id];
	if (LOCK_STATE_LOCKED == param_con.is_unlocked) continue;

	const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(param_con.partner_id);
	if (NULL == basic_cfg) continue;

	const PartnerUpGradeCfg * ug_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurGradeAttrCfg(param_con.partner_id, param_con.upgrade_lv);
	if (NULL == ug_cfg) continue;

	const PartnerTalentCfg * talent_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPTalentSkillCfg(ug_cfg->talent_skill_id, ug_cfg->talent_skill_lv);
	if (NULL == talent_cfg || TALENT_PLAYER_GAIN_ITEM_DAILY != talent_cfg->effect_type) continue;

	const ItemBase * item_base = ITEMPOOL->GetItem(talent_cfg->param1);
	if (NULL == item_base) continue;

	static MailContentParam contentparam; contentparam.Reset();
	ItemDataWrapper & mail_item = contentparam.item_list[0];
	mail_item.item_id = talent_cfg->param1;
	mail_item.is_bind = 1;		// 伙伴送的道具默认绑定
	mail_item.num = talent_cfg->param2;
	mail_item.invalid_time = item_base->CalInvalidTime();

	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_partner_talent_gift, basic_cfg->name);
	if (length > 0) MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}*/
}

void Partner::OnRoleLogin()
{
	if (3 != m_new_partner.is_recover_item_flag)
	{
		int num = m_role_module_mgr->GetKnapsack()->Count(26678, true, true);
		if (num > 0)
		{
			if (m_role_module_mgr->GetKnapsack()->ConsumeKnapsackItem(26678, num, "partner recover item [Partner::OnRoleLogin]"))
			{
				static MailContentParam contentparam; contentparam.Reset();
				contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = num * 1000;

				SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_system_item_return_subject);
				SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_partner_item_system_return_content);

				MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
		m_new_partner.is_recover_item_flag = 3;
	}
	if (4 != m_new_partner.is_refresh_store_flag)
	{
		const FunOpenCfg* funopen_cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_SHAN_MYSTERY_STORE);
		if (NULL != funopen_cfg)
		{
			if (funopen_cfg->limit_level <= m_role_module_mgr->GetRole()->GetLevel())
			{
				this->OnStoreRefresh(true);
				m_new_partner.is_refresh_store_flag = 4;

				// 立即获得第一次免费刷新
				m_new_partner.next_free_refresh_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
				m_partner.next_free_refresh_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
				this->SendStoreInfo();
			}
		}
	}
	const MysteryStoreOtherCfg * other_cfg = LOGIC_CONFIG->GetMysteryStoreConfig()->GetOtherCfg();
	if (NULL != other_cfg && m_new_partner.store_ver != other_cfg->mystery_store_ver)
	{
		const FunOpenCfg* funopen_cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_SHAN_MYSTERY_STORE);
		if (NULL != funopen_cfg)
		{
			if (funopen_cfg->limit_level <= m_role_module_mgr->GetRole()->GetLevel())
			{
				this->OnStoreRefresh(true);
				// 
				m_new_partner.next_free_refresh_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
				m_partner.next_free_refresh_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
				this->SendStoreInfo();
			}
		}
	}

	if (m_partner.partner_skill_tree_back_flag != 1)
	{
		m_partner.partner_skill_tree_back_flag = 1;
		std::vector<ItemConfigData> return_vec;
		for (int t_apid = PARTNER_ID_START; t_apid <= m_partner_count && t_apid < PARTNER_NUM_MAX; ++t_apid)
		{
			if (this->IsPartnerInvalid(t_apid)) continue;
			PartnerGiftTree &partner_tree = m_partner_gift_tree_list.partner_gift_tree_list[t_apid];
			for (int tree_point = 0; tree_point < MAX_PARTNER_GIFT_TREE_SKILL_NUM; tree_point++)
			{
				const PartnerGiftTreeReturnCfg * partner_return = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftTreeReturnCfg(tree_point + 1, partner_tree.gift_tree[tree_point]);
				if (NULL != partner_return)
				{
					return_vec.push_back(ItemConfigData(partner_return->return_item, 1, partner_return->return_num));
				}
			}
		}

		//道具累积
		std::vector<ItemConfigData> real_item_list;
		for (std::vector<ItemConfigData>::iterator it = return_vec.begin(); it != return_vec.end(); it++)
		{
			bool has_same = false;
			for (std::vector<ItemConfigData>::iterator it2 = real_item_list.begin(); it2 != real_item_list.end(); it2++)
			{
				if (it->item_id == it2->item_id)
				{
					has_same = true;
					it2->num += it->num;
					break;
				}
			}
			if (has_same)continue;
			real_item_list.push_back(*it);
		}
		return_vec.clear();
		if ((int)real_item_list.size() > 0)
		{
			static MailContentParam contentparam; contentparam.Reset();
			int real_item_num = 0;
			for (std::vector<ItemConfigData>::iterator it = real_item_list.begin(); it != real_item_list.end() && real_item_num < MAX_ATTACHMENT_ITEM_NUM; it++)
			{
				contentparam.item_list[real_item_num].num = it->num;
				contentparam.item_list[real_item_num].is_bind = it->is_bind ? 1 : 0;
				contentparam.item_list[real_item_num++].item_id = it->item_id;
				if (real_item_num >= MAX_ATTACHMENT_ITEM_NUM)//给满邮件了，立刻发一封邮件
				{
					int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_partner_tree_return_subject);
					int lencont = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_partner_tree_return_content);
					if (lensub > 0 && lencont > 0)
					{
						MailRoute::Instance().MailToUser(IntToUid(m_role_module_mgr->GetUid()), MAIL_REASON_DEFAULT, contentparam);
					}
					real_item_num = 0;
					contentparam.Reset();
				}
			}
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_partner_tree_return_subject);
			int lencont = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_partner_tree_return_content);

			if (lensub > 0 && lencont > 0)
			{
				MailRoute::Instance().MailToUser(IntToUid(m_role_module_mgr->GetUid()), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}

	this->SendPactGridList();
}

const AttributeList & Partner::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		m_partner_jiban_num = 0;
		for (int t_apid = PARTNER_ID_START; t_apid <= m_partner_count && t_apid < PARTNER_NUM_MAX; ++t_apid)
		{
			if (this->IsPartnerInvalid(t_apid)) continue;

			const NewPartnerCon & p_partner = m_new_partner.partner_base_info[t_apid].partner_list;
			if (LOCK_STATE_LOCKED == p_partner.is_unlocked) continue;

			// 伙伴羁绊
			const PartnerUpGradeCfg * upg_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurGradeAttrCfg(p_partner.partner_id, p_partner.upgrade_lv);
			if (NULL != upg_cfg)
			{
				const PartnerUpGradeRoleAttrCfg * upg_role_attr_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerUpGradeRoleAttrCfg(upg_cfg->ji_ban_seq);
				if (upg_role_attr_cfg != NULL)
				{
					m_attrs_add.Add(upg_role_attr_cfg->attrs);
					m_partner_jiban_num += 1;
				}
			}
		}
		EventHandler::Instance().OnRolePartnerJiBanNum(m_role_module_mgr->GetRole(), m_partner_jiban_num);
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void Partner::CheckPartnerCollection()
{
	for (int partner_id = PARTNER_ID_START; partner_id < PARTNER_NUM_MAX; partner_id++)
	{
		if (this->IsPartnerAlreadyActive(partner_id))
		{
			m_role_module_mgr->GetCollection()->OnPartnerCollection(partner_id);
		}
	}
}

bool Partner::IsPartnerAlreadyActive(int partner_id)
{
	/*int p_seq = m_partner.GetIdxByPartnerID(partner_id);
	if (PARTNER_ERROR_SEQ == p_seq || LOCK_STATE_UNLOCK != m_partner.partner_list[p_seq].is_unlocked) return false;*/

	if (this->IsPartnerInvalid(partner_id) || m_new_partner.partner_base_info[partner_id].partner_list.is_unlocked != LOCK_STATE_UNLOCK) return false;

	return true;
}

int Partner::GetCurFightPartnerNum()
{
	int num = 0;
	for (int i = 0; i < PARTNER_SYNFIGHT_NUM_MAX; ++i)
	{
		if (m_new_partner.synfight_list[i] > 0)
		{
			++num;
		}
	}

	return num;
}

void Partner::MaxPartnerHPOrMP(int partner_id, int max_type, bool is_notify)
{
	if (max_type > MAX_TYPE_BOTH_TWO || max_type < MAX_TYPE_ONLY_CHP) return;

	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerCon & p_con = m_new_partner.partner_base_info[partner_id].partner_list;
	NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];
	PartnerSql & p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
	
	const AttrCon & t_attr_list = m_partner_attr_list[partner_id];
	if (MAX_TYPE_ONLY_CHP == max_type || MAX_TYPE_BOTH_TWO == max_type)
	{
		p_sql.max_hp = t_attr_list.attr_list[BATTLE_ATTR_MAX_HP];
		p_sql.cur_hp = p_sql.max_hp;
	}

	if (MAX_TYPE_ONLY_CMP == max_type || MAX_TYPE_BOTH_TWO == max_type)
	{
		p_sql.max_mp = t_attr_list.attr_list[BATTLE_ATTR_MAX_MP];
		p_sql.cur_mp = p_sql.max_mp;
	}

	if (is_notify && m_already_send_all_attr)
	{
		std::vector<BattleAttrPair> attr_type_list;

		attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_HP, p_sql.cur_hp));
		attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_MP, p_sql.cur_mp));

		this->SendPartnerAttrChange(partner_id, attr_type_list, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_HPMP);
	}

	if (1 == p_con.fight_state)
	{
		this->SyncPartnerInfoToTeam(partner_id, p_sql.cur_hp, p_sql.max_hp, p_sql.cur_mp, p_sql.max_mp, this->GetCurPutOnSkin(partner_id), pbi.level);
	}
}

bool Partner::AlterPartnerHPOrMPValue(int partner_id, int r_type, int r_value, bool is_percent /* = false*/)
{
	if (r_type > MAX_TYPE_BOTH_TWO || r_type < MAX_TYPE_ONLY_CHP) return false;

	if (this->IsPartnerInvalid(partner_id)) return false;

	PartnerSql & p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
	NewPartnerCon & p_con = m_new_partner.partner_base_info[partner_id].partner_list;
	NewPartnerBaseInfo& p_base = m_new_partner.partner_base_info[partner_id];
	std::vector<BattleAttrPair> attr_type_list;

	if (r_type == MAX_TYPE_ONLY_CHP)
	{
		if (p_sql.max_hp <= p_sql.cur_hp)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
			return false;
		}
	}
	else if (r_type == MAX_TYPE_ONLY_CMP)
	{
		if (p_sql.max_mp <= p_sql.cur_mp)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
			return false;
		}
	}
	else
	{
		if (p_sql.max_mp <= p_sql.cur_mp && p_sql.max_hp <= p_sql.cur_hp)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
			return false;
		}
	}

	if (MAX_TYPE_ONLY_CHP == r_type || MAX_TYPE_BOTH_TWO == r_type)
	{
		if (is_percent)
		{
			r_value = (int)(1.0 * p_sql.max_hp * r_value / BATTLE_PERCENT_NUM);
		}
		IntHelpAdd(p_sql.cur_hp, p_sql.max_hp, r_value);
		attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_HP, p_sql.cur_hp));
	}

	if (MAX_TYPE_ONLY_CMP == r_type || MAX_TYPE_BOTH_TWO == r_type)
	{
		if (is_percent)
		{
			r_value = (int)(1.0 * p_sql.max_mp * r_value / BATTLE_PERCENT_NUM);
		}
		IntHelpAdd(p_sql.cur_mp, p_sql.max_mp, r_value);
		attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_MP, p_sql.cur_mp));
	}

	this->SendPartnerAttrChange(partner_id, attr_type_list, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_HPMP);

	if (1 == p_con.fight_state)
	{
		this->SyncPartnerInfoToTeam(partner_id, p_sql.cur_hp, p_sql.max_hp, p_sql.cur_mp, p_sql.max_mp, this->GetCurPutOnSkin(partner_id), p_base.level);
	}
	return true;
}

void Partner::AlterPartnerHPOrMPPerce(int partner_id, int r_type, double r_value)
{
	if (r_type > MAX_TYPE_BOTH_TWO || r_type < MAX_TYPE_ONLY_CHP) return;
	if (this->IsPartnerInvalid(partner_id)) return;

	PartnerSql & p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
	NewPartnerCon & p_con = m_new_partner.partner_base_info[partner_id].partner_list;
	NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];
	AttrCon & t_attr_list = m_partner_attr_list[partner_id];
	std::vector<BattleAttrPair> attr_type_list;

	if (MAX_TYPE_ONLY_CHP == r_type || MAX_TYPE_BOTH_TWO == r_type)
	{
		p_sql.cur_hp += (int)(t_attr_list.attr_list[BATTLE_ATTR_MAX_HP] * r_value);
		if (p_sql.cur_hp > t_attr_list.attr_list[BATTLE_ATTR_MAX_HP])
		{
			p_sql.cur_hp = t_attr_list.attr_list[BATTLE_ATTR_MAX_HP];
		}
		attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_HP, p_sql.cur_hp));
	}

	if (MAX_TYPE_ONLY_CMP == r_type || MAX_TYPE_BOTH_TWO == r_type)
	{
		p_sql.cur_mp += (int)(t_attr_list.attr_list[BATTLE_ATTR_MAX_MP] * r_value);
		if (p_sql.cur_mp > t_attr_list.attr_list[BATTLE_ATTR_MAX_MP])
		{
			p_sql.cur_mp = t_attr_list.attr_list[BATTLE_ATTR_MAX_MP];
		}
		attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_MP, p_sql.cur_mp));
	}

	this->SendPartnerAttrChange(partner_id, attr_type_list, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_HPMP);

	if (1 == p_con.fight_state)
	{
		this->SyncPartnerInfoToTeam(partner_id, p_sql.cur_hp, p_sql.max_hp, p_sql.cur_mp, p_sql.max_mp, this->GetCurPutOnSkin(partner_id), pbi.level);
	}
}

void Partner::SetPartnerHPOrMPValue(int partner_id, int cur_hp, int cur_mp)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	PartnerSql & p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
	NewPartnerCon & p_con = m_new_partner.partner_base_info[partner_id].partner_list;
	NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];
	std::vector<BattleAttrPair> attr_type_list;

	p_sql.cur_hp = cur_hp;
	p_sql.cur_hp = GetMin(p_sql.max_hp, p_sql.cur_hp);
	p_sql.cur_hp = GetMax(1, p_sql.cur_hp);
	attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_HP, p_sql.cur_hp));

	p_sql.cur_mp = cur_mp;
	p_sql.cur_mp = GetMin(p_sql.max_mp, p_sql.cur_mp);
	p_sql.cur_mp = GetMax(0, p_sql.cur_mp);
	attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_MP, p_sql.cur_mp));

	this->SendPartnerAttrChange(partner_id, attr_type_list, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_HPMP);

	if (1 == p_con.fight_state)
	{
		this->SyncPartnerInfoToTeam(partner_id, p_sql.cur_hp, p_sql.max_hp, p_sql.cur_mp, p_sql.max_mp, this->GetCurPutOnSkin(partner_id), pbi.level);
	}
}

void Partner::AddAllPartnerHpMp(double percent)
{
	for (int i = PARTNER_ID_START; i < PARTNER_NUM_MAX; ++i)
	{
		this->AlterPartnerHPOrMPPerce(m_new_partner.partner_base_info[i].GetPartnerId(), MAX_TYPE_BOTH_TWO, percent);
	}
}

void Partner::AddFightPartnerHpMp(double percent)
{
	for (int i = 0; i < PARTNER_SYNFIGHT_NUM_MAX; i++)
	{
		if(m_new_partner.synfight_list[i] != 0)
		{
			this->AlterPartnerHPOrMPPerce(m_new_partner.synfight_list[i], MAX_TYPE_BOTH_TWO, percent);
		}
	}
}

void Partner::SyncPartnerInfoToTeam(int partner_id, int cur_hp, int max_hp, int cur_mp, int max_mp, int skin_id, int level)
{
	Team* team = m_role_module_mgr->GetRole()->GetMyTeam();
	if (NULL != team && team->GetLeaderUID() == m_role_module_mgr->GetRole()->GetUID() && team->IsPartnerInTeam(partner_id))
	{
		Protocol::SCUpdateMemberInfo msg;
		msg.uid = partner_id;
		msg.is_partner = 1;
		msg.hp = cur_hp;
		msg.max_hp = max_hp;
		msg.mp = cur_mp;
		msg.max_mp = max_mp;
		msg.skin_id = skin_id;
		msg.level = level;

		team->SendToMember(&msg, sizeof(msg));
	}
}

Attribute Partner::GetAttrByPartnerID(int partner_id, int attr_type)
{
	if (attr_type < BATTLE_ATTR_MIN || attr_type >= BATTLE_ATTR_MAX) return 0;
	if (this->IsPartnerInvalid(partner_id)) return 0;

	if (BATTLE_ATTR_CUR_HP == attr_type)
	{
		return m_new_partner.partner_base_info[partner_id].partner_data.cur_hp;
	}
	else if (BATTLE_ATTR_CUR_MP == attr_type)
	{
		return m_new_partner.partner_base_info[partner_id].partner_data.cur_mp;
	}

	return m_partner_attr_list[partner_id].attr_list[attr_type];
}

Attribute* Partner::GetAttrListByPartnerID(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return NULL;
	return m_partner_attr_list[partner_id].attr_list;
}

const PartnerSql* Partner::GetPartnerDataById(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return NULL;
	return &m_new_partner.partner_base_info[partner_id].partner_data;
}

const NewPartnerBaseInfo* Partner::GetPartnerBaseInfoById(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return NULL;

	return &m_new_partner.partner_base_info[partner_id];
}

int Partner::GetPartnerAttrListReason(int partner_id)
{
#ifdef _DEBUG
	PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_TEST_PARTNER_RECALC_REAWSON_ATTR_LIST_TIME);
#endif

	if (this->IsPartnerInvalid(partner_id)) return -1;
	const NewPartnerBaseInfo & pbi = m_new_partner.partner_base_info[partner_id];
	NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	PartnerGiftTree & partner_gift_tree = m_partner_gift_tree_list.partner_gift_tree_list[partner_id];

	const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(p_param.partner_id);
	if (NULL == basic_cfg) return -2;

	//初始化
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		m_reason_list[i].Reset();
	}

	// 基础属性 - 等值 - 必须放最前
	const PartnerLevelCfg * p_attr_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerLevelCfg(p_param.partner_id, pbi.level);
	if (NULL != p_attr_cfg)
	{
		m_reason_list[PARTNER_REASON_LIST_TYPE_LEVEL].m_attrs[BATTLE_ATTR_MAX_HP] = p_attr_cfg->attr_list[PARTNER_ATTR_MHP];
		m_reason_list[PARTNER_REASON_LIST_TYPE_LEVEL].m_attrs[BATTLE_ATTR_MAX_MP] = p_attr_cfg->attr_list[PARTNER_ATTR_MMP];
		for (int t_cur = PARTNER_ATTR_ATK; t_cur < PARTNER_ATTR_MAX; ++t_cur)
		{
			if (t_cur - PARTNER_ATTR_ATK < 0) continue;
			m_reason_list[PARTNER_REASON_LIST_TYPE_LEVEL].m_attrs[t_cur - PARTNER_ATTR_ATK] = p_attr_cfg->attr_list[t_cur];
		}

		for (int t_cur = PARTNER_RESI_TOXIC; t_cur < PARTNER_RESI_MAX; ++t_cur)
		{
			m_reason_list[PARTNER_REASON_LIST_TYPE_LEVEL].m_attrs[t_cur + BATTLE_ATTR_ANTI_VALUE_BEGIN] = p_attr_cfg->resi_list[t_cur];
		}

		for (int t_cur = PARTNER_AMEN_CRI; t_cur < PARTNER_AMEN_TEN; ++t_cur)
		{
			m_reason_list[PARTNER_REASON_LIST_TYPE_LEVEL].m_attrs[t_cur + BATTLE_ATTR_CORRECTED_VALUE_BEGIN] = p_attr_cfg->amen_list[t_cur];
		}
		m_reason_list[PARTNER_REASON_LIST_TYPE_LEVEL].m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE] = p_attr_cfg->amen_list[PARTNER_AMEN_TEN];
	}

	Attribute tmp_base_attr_list[BATTLE_ATTR_MAX] = { 0 };			// 基础属性列表
	memcpy(tmp_base_attr_list, m_reason_list[PARTNER_REASON_LIST_TYPE_LEVEL].m_attrs, sizeof(tmp_base_attr_list));

	// 突破属性 - 加值
	for (int tmp_upgrade_lv = 0; tmp_upgrade_lv < p_param.upgrade_lv; ++tmp_upgrade_lv)
	{
		int tmp_break_lv = 0;
		const PartnerBreakThCfg * p_break_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, tmp_upgrade_lv, tmp_break_lv);
		while (NULL != p_break_cfg)
		{
			m_reason_list[PARTNER_REASON_LIST_TYPE_ADV].m_attrs[p_break_cfg->attr_type] += p_break_cfg->add_value;
			p_break_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, tmp_upgrade_lv, ++tmp_break_lv);
		}
	}

	for (int tmp_break_lv = 0; tmp_break_lv < p_param.attr_stage; ++tmp_break_lv)	// 当前进阶等级的突破属性
	{
		const PartnerBreakThCfg * p_break_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, p_param.upgrade_lv, tmp_break_lv);
		if (NULL != p_break_cfg) m_reason_list[PARTNER_REASON_LIST_TYPE_ADV].m_attrs[p_break_cfg->attr_type] += p_break_cfg->add_value;
	}

	// 装备属性 - 加值
	bool need_calc_suit = true; int calc_suit_id = 0, min_level = 0, min_color = 0;
	for (int slot_idx = 0; slot_idx < PE_TYPE_MAX; ++slot_idx)
	{
		const NewPartnerEquipment & equipment = p_param.equip_list[slot_idx];
		if (0 == equipment.partner_equip_id)
		{
			need_calc_suit = false;
			continue;
		}

		for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
		{
			if (equipment.m_voc_list[i].attr_value >= 0 && equipment.m_voc_list[i].attr_type < BATTLE_ATTR_MAX) m_reason_list[PARTNER_REASON_LIST_TYPE_EQUIP].m_attrs[equipment.m_voc_list[i].attr_type] += equipment.m_voc_list[i].attr_value;
		}

		if (need_calc_suit)
		{
			const PartnerEqSuitInform * suit_cfg = LOGIC_CONFIG->GetPartnerEquipmentConfig()->GetEquipmentSuitID(equipment.partner_equip_id);
			if (NULL == suit_cfg || (0 != calc_suit_id && suit_cfg->suit_id != calc_suit_id))
			{
				need_calc_suit = false;
				continue;
			}

			calc_suit_id = suit_cfg->suit_id;		// 比对，取较低阶的；
			if (0 == min_level || suit_cfg->suit_level < min_level)
			{
				min_level = suit_cfg->suit_level;
				min_color = suit_cfg->suit_color;
			}
			else if (suit_cfg->suit_level == min_level)
			{
				if (0 == min_color || suit_cfg->suit_color <= min_color)
				{
					min_color = suit_cfg->suit_color;
				}
			}
		}
	}

	//命格套装
	if (need_calc_suit)
	{
		const PartnerEqSuitEffect * suit_cfg = LOGIC_CONFIG->GetPartnerEquipmentConfig()->GetPartnerEquipmentAttrAdditionCfg(calc_suit_id, min_level, min_color);
		if (NULL != suit_cfg)
		{
			m_reason_list[PARTNER_REASON_LIST_TYPE_EQUIP].m_attrs[BATTLE_ATTR_MAX_HP] += suit_cfg->basic_attr[PARTNER_ATTR_MHP];
			m_reason_list[PARTNER_REASON_LIST_TYPE_EQUIP].m_attrs[BATTLE_ATTR_MAX_MP] += suit_cfg->basic_attr[PARTNER_ATTR_MMP];
			for (int t_cur = PARTNER_ATTR_ATK; t_cur < PARTNER_ATTR_MAX; ++t_cur)
			{
				if (t_cur - PARTNER_ATTR_ATK < 0) continue;
				m_reason_list[PARTNER_REASON_LIST_TYPE_EQUIP].m_attrs[t_cur - PARTNER_ATTR_ATK] += suit_cfg->basic_attr[t_cur];
			}
		
			for (int t_cur = PARTNER_RESI_TOXIC; t_cur < PARTNER_RESI_MAX; ++t_cur)
			{
				m_reason_list[PARTNER_REASON_LIST_TYPE_EQUIP].m_attrs[t_cur + BATTLE_ATTR_ANTI_VALUE_BEGIN] += suit_cfg->resis_attr[t_cur];
			}

			for (int t_cur = PARTNER_AMEN_CRI; t_cur < PARTNER_AMEN_TEN; ++t_cur)
			{
				m_reason_list[PARTNER_REASON_LIST_TYPE_EQUIP].m_attrs[t_cur + BATTLE_ATTR_CORRECTED_VALUE_BEGIN] += suit_cfg->amend_attr[t_cur];
			}
			m_reason_list[PARTNER_REASON_LIST_TYPE_EQUIP].m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE] += suit_cfg->amend_attr[PARTNER_AMEN_TEN];
		}
	}

	// 人物修炼导致的伙伴属性提升
	std::set<int> partner_train_type = LOGIC_CONFIG->GetTrainConfig()->GetAllPartnerTrainCfgType();
	for (std::set<int>::iterator iter = partner_train_type.begin(); iter != partner_train_type.end(); ++iter)
	{
		int train_type = *iter;
		RoleCommonSavePartnerTrainData * train_data = m_role_module_mgr->GetTrain()->GetRoleCommonSavePartnerTrainData(train_type);
		if (train_data == NULL)
		{
			continue;
		}
		const PartnerTrainCfg * train_cfg = LOGIC_CONFIG->GetTrainConfig()->GetPartnerTrainCfg(train_type, train_data->level);
		if (train_cfg == NULL)
		{
			continue;
		}

		for (std::set<int>::iterator iter2 = train_cfg->attr_type_list.begin(); iter2 != train_cfg->attr_type_list.end(); ++iter2)
		{
			int attr_type = *iter2;
			if (0 <= attr_type && attr_type < BATTLE_ATTR_MAX)
			{
				m_reason_list[PARTNER_REASON_LIST_TYPE_TRAIN].m_attrs[attr_type] += train_cfg->attribute_value;
			}
		}

		const PartnerTrainCfg * train_cfg_1 = LOGIC_CONFIG->GetTrainConfig()->GetPartnerTrainCfg(train_type, 0);
		if (NULL == train_cfg_1)
		{
			continue;
		}

		for (std::set<int>::iterator iter2 = train_cfg_1->attr_type_list.begin(); iter2 != train_cfg_1->attr_type_list.end(); ++iter2)
		{
			int attr_type = *iter2;
			if (0 <= attr_type && attr_type < BATTLE_ATTR_MAX)
			{
				m_reason_list[PARTNER_REASON_LIST_TYPE_TRAIN].m_attrs[attr_type] -= train_cfg_1->attribute_value;
			}
		}
	}

	// 修为
	m_role_module_mgr->GetXiuWei()->ReCalcPartnerAttr(m_reason_list[PARTNER_REASON_LIST_TYPE_XIU_WEI], pbi.level);
	// 星图
	m_role_module_mgr->GetStarChart()->ReCalcPartnerAttr(m_reason_list[PARTNER_REASON_LIST_TYPE_STAR_CHART]);

	//升品属性（觉醒）
	int init_quality_type = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(basic_cfg->quality);
	int quality_up_type = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(m_new_partner.partner_base_info[partner_id].new_quality);
	for (int type = QUALITY_TYPE_BLUE_TO_PURPLE; type <= quality_up_type && type < QUALITY_TYPE_MAX; ++type)
	{
		int quality_segment = 1;
		int max_quality_segment = PARTNER_QUALITY_SEGMENT_MAX_NUM + 1;
		if (type == quality_up_type)
		{
			max_quality_segment = m_new_partner.partner_base_info[partner_id].quality_segment;
		}
		for (; quality_segment < max_quality_segment; quality_segment++)
		{
			const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(partner_id, type, quality_segment));
			if (NULL != q_cfg)
			{
				//伙伴初始品质比蓝色高的部分添加到等级中
				if (init_quality_type > type)
				{
					this->AddAttrByQualityUp(m_reason_list[PARTNER_REASON_LIST_TYPE_LEVEL], q_cfg);
				}
				else
				{
					this->AddAttrByQualityUp(m_reason_list[PARTNER_REASON_LIST_TYPE_WAKE], q_cfg);
				}
			}
		}
	}

	AttributeList attr_pellet;
	m_role_module_mgr->GetRoleAttrPellet()->ReCalcAttrPartner(attr_pellet, partner_id, basic_cfg->profession);
	for (int i = 0; i < BATTLE_ATTR_MAX; i++)
	{
		m_reason_list[PARTNER_REASON_LIST_TYPE_PELLET].m_attrs[i] += attr_pellet.m_attrs[i];
	}

	// 伙伴皮肤 属性加值
	for (int list_idx = 0; list_idx < PARTNER_SKIN_ONCE; ++list_idx)
	{
		if (this->IsActive(partner_id, list_idx)) // 加上所有激活皮肤的属性
		{
			const PartnerSkinCfgInfo *skin_att = LOGIC_CONFIG->GetSkinConfig()->GetPartnerSkinCfgInfo(SkinKey(p_param.partner_id, list_idx));
			if (skin_att != NULL)
			{
				// 增加属性
				this->AttrChangeBySkin(m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER], skin_att);
			}
		}
	}

	// 被动技能加成
	const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
		partner_id, LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(pbi.new_quality), pbi.quality_segment));
	if (NULL != q_cfg)
	{
		for (int t_s_num = 0; t_s_num < PARTNER_COMMON_SKILL_MAX; ++t_s_num)
		{
			int skill_id = q_cfg->common_skill_id_list[t_s_num];
			int skill_lv = q_cfg->common_skill_lv_list[t_s_num];

			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
			if (NULL == passive_skill) continue;

			int spid = passive_skill->sp_id;
			if (passive_skill->is_level_spid && skill_lv >= 1 && skill_lv < ARRAY_LENGTH(passive_skill->level_spid_list))
			{
				spid = passive_skill->level_spid_list[skill_lv];
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
					m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
				}

				if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
					&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
				{
					BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_role_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
					m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += add_attr;
				}

				if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
					&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
				{
					BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
					m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += add_attr;
				}

				if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
					&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
					&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
					&& detail_cfg->param_list[2] > 0)
				{
					int attr_type0 = detail_cfg->param_list[0];
					int attr_type1 = detail_cfg->param_list[1];
					m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[attr_type0] += static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
				}
			}
		}
	}

	std::vector<SkillPro> skill_list;
	int guild_inborn_capbility = m_role_module_mgr->GetRoleGuildInborn()->GetSkillList(&skill_list, GUILD_INBORN_ADD_TARGET_TYPE_PARTNER);
	for (int i = 0; i < (int)skill_list.size(); i++)
	{
		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_list[i].skill_id);
		if (NULL == passive_skill) continue;

		int skill_level = skill_list[i].skill_level;
		int spid = passive_skill->sp_id;
		if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
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
				m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
			}

			if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_role_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
				m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
				m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[2] > 0)
			{
				int attr_type0 = detail_cfg->param_list[0];
				int attr_type1 = detail_cfg->param_list[1];
				m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[attr_type0] += static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
			}
		}
	}

	//家族灵脉可能会配5项基础属性
	AttributePlan tmp_ap;
	for (int i = 0, k = BATTLE_ADD_POINT_TYPE_BEGIN; i < ADD_POINT_TYPE_MAX && k <= BATTLE_ADD_POINT_TYPE_END; i++, k++)
	{
		tmp_ap.add_point_list[i] += m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[k];
	}
	LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER]);

	// 伙伴天赋
	{
		//伙伴天赋
		int tree_level_count = 0;
		int role_level = m_role_module_mgr->GetRole()->GetLevel();
		for (int tree_point = 0; tree_point < MAX_PARTNER_GIFT_TREE_SKILL_NUM; tree_point++)
		{
			int tree_level = partner_gift_tree.gift_tree[tree_point];
			tree_level_count += tree_level;
			const PartnerGiftUplevelCfg * tree_cfg = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftTreeUplevelByPartnerId(partner_id, tree_point + 1, tree_level);
			if (tree_cfg != NULL)
			{
				for (int attr_idx = 0; attr_idx < PartnerGiftUplevelCfg::MAX_PARTNER_TREE_MAX_ATTR_NUM; attr_idx++)
				{
					if (tree_cfg->attr_list[attr_idx].attr_type >= 0 && tree_cfg->attr_list[attr_idx].attr_type <= BATTLE_ATTR_MAX)
					{
						m_reason_list[PARTNER_REASON_LIST_TYPE_TALENT].m_attrs[tree_cfg->attr_list[attr_idx].attr_type] += tree_cfg->attr_list[attr_idx].attr_value;
					}
				}

				if (tree_cfg->sp_id > 0)
				{
					const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(tree_cfg->sp_id);
					if (NULL != spe_cfg)
					{
						for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
						{
							const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
							if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

							const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
							if (NULL == detail_cfg) continue;

							if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								m_reason_list[PARTNER_REASON_LIST_TYPE_TALENT].m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
							}

							if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(ceil(role_level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
								m_reason_list[PARTNER_REASON_LIST_TYPE_TALENT].m_attrs[detail_cfg->param_list[0]] += add_attr;
							}

							if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
								m_reason_list[PARTNER_REASON_LIST_TYPE_TALENT].m_attrs[detail_cfg->param_list[0]] += add_attr;
							}

							if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
								&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
								&& detail_cfg->param_list[2] > 0)
							{
								int attr_type0 = detail_cfg->param_list[0];
								int attr_type1 = detail_cfg->param_list[1];
								m_reason_list[PARTNER_REASON_LIST_TYPE_TALENT].m_attrs[attr_type0] += static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
							}
						}
					}
				}
			}
		}
		const PartnerGiftSpListCfg * sp_cfg = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftSpCfg(partner_id);
		if (sp_cfg != NULL)
		{
			for (int sp_idx = 0; sp_idx < PartnerGiftSpListCfg::MAX_SP_NUM; sp_idx++)
			{
				if (NULL != sp_cfg->sp_list[sp_idx] && sp_cfg->sp_list[sp_idx]->inborn_level_ask <= tree_level_count)
				{
					//特效生效
					const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_cfg->sp_list[sp_idx]->sp_id);
					if (NULL != spe_cfg)
					{
						for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
						{
							const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
							if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

							const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
							if (NULL == detail_cfg) continue;

							if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								m_reason_list[PARTNER_REASON_LIST_TYPE_TALENT].m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
							}

							if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(ceil(role_level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
								m_reason_list[PARTNER_REASON_LIST_TYPE_TALENT].m_attrs[detail_cfg->param_list[0]] += add_attr;
							}

							if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
								m_reason_list[PARTNER_REASON_LIST_TYPE_TALENT].m_attrs[detail_cfg->param_list[0]] += add_attr;
							}

							if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
								&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
								&& detail_cfg->param_list[2] > 0)
							{
								int attr_type0 = detail_cfg->param_list[0];
								int attr_type1 = detail_cfg->param_list[1];
								m_reason_list[PARTNER_REASON_LIST_TYPE_TALENT].m_attrs[attr_type0] += static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
							}
						}
					}
				}
			}
		}
	}

	//飞升武器
	if (pbi.soar_arms_level > 0)
	{
		const PartnerSoarArmsCfg * soar_arms_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarArmsCfg(basic_cfg->soar_arms_id, pbi.soar_arms_level);
		if (NULL != soar_arms_cfg)
		{
			for (int i = 0; i < ARRAY_LENGTH(soar_arms_cfg->attr_list) && i < ARRAY_LENGTH(m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs); i++)
			{
				m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[i] += soar_arms_cfg->attr_list[i];
			}
			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(soar_arms_cfg->arms_skill_id);
			if (NULL != passive_skill)
			{
				int spid = passive_skill->sp_id;
				if (passive_skill->is_level_spid && soar_arms_cfg->arms_skill_level >= 1 && soar_arms_cfg->arms_skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
				{
					spid = passive_skill->level_spid_list[soar_arms_cfg->arms_skill_level];
				}
				const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
				if (NULL != spe_cfg)
				{
					for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
					{
						const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
						if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

						const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
						if (NULL == detail_cfg) continue;

						if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
							&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
						{
							m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
						}

						if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
							&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
						{
							BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_role_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
							m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += add_attr;
						}

						if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
							&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
						{
							BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
							m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += add_attr;
						}

						if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
							&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
							&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
							&& detail_cfg->param_list[2] > 0)
						{
							int attr_type0 = detail_cfg->param_list[0];
							int attr_type1 = detail_cfg->param_list[1];
							m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[attr_type0] += static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
						}
					}
				}
			}
		}
	}

	// 符玉
	AttrCon fuyu_add; fuyu_add.Reset();
	m_role_module_mgr->GetFuYuManager()->ReCalcPartnerAttr(partner_id, fuyu_add);
	for (int i = 0; i < BATTLE_ATTR_MAX; i++)
	{
		m_reason_list[PARTNER_REASON_LIST_TYPE_EQUIP].m_attrs[i] += fuyu_add.attr_list[i];
	}

	//伙伴--元神契约
	m_partner_pact_list[partner_id].GetNowPactAttr(m_reason_list[PARTNER_REASON_LIST_TYPE_PACT]);

	//伙伴强化
	if (pbi.intensify_level > 0)
	{
		const PartnerIntensifyCfg *pi_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerIntensifyCfg(partner_id, pbi.intensify_level);
		if (NULL != pi_cfg)
		{
			for (int i = 0; i < (int)pi_cfg->attr_list.size(); i++)
			{
				const PartnerIntensifyCfg::AttrData &attr_data = pi_cfg->attr_list[i];

				m_reason_list[PARTNER_REASON_LIST_TYPE_INTENSIFY].m_attrs[attr_data.type] += attr_data.param;
			}
		}
	}

#ifdef _DEBUG
	long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_PARTNER_RECALC_REAWSON_ATTR_LIST_TIME);

	printf("%s partner_id[%d] elapse[%lld]us\n", __FUNCTION__, p_param.partner_id, elapse_us);
#endif

	return 0;
}

void Partner::SendPartnerAttrListReason(short param1)
{
	if (this->IsPartnerInvalid(param1)) return;
	if (this->GetPartnerAttrListReason(param1)) return;

	Protocol::SCAttrListReason info;
	info.reason_type = Protocol::SCAttrListReason::REASON_TYPE_PARTNER;
	info.param1 = param1;
	for (int i = 0; i < MAX_REASON_TYPE_LIST_NUM; i++)
	{
		info.reason_list[i] = m_reason_list[i];
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Partner::SendPartnerSingleInfo(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;
	const NewPartnerBaseInfo &partner_info = m_new_partner.partner_base_info[partner_id];
	const NewPartnerCon & p_pc = partner_info.partner_list;
	const PartnerUpGradeCfg * pucg = LOGIC_CONFIG->GetPartnerConfig()->GetCurGradeAttrCfg(p_pc.partner_id, p_pc.upgrade_lv);
	if (NULL == pucg) return;

	Protocol::SCPartnerSingleInfo info;
	info.partner_info.base_info = p_pc;
	info.partner_info.task_finish = 1;

	AttrCon & attr_con = m_partner_attr_list[partner_id];
	for (int t_idx = BATTLE_ATTR_MIN; t_idx < BATTLE_ATTR_MAX; ++t_idx)
	{
		if (BATTLE_ATTR_CUR_HP == t_idx || BATTLE_ATTR_CUR_MP == t_idx) continue;

		info.partner_info.attr_pair[t_idx] = attr_con.attr_list[t_idx];
	}
	// 当前血量和当前魔力特殊处理
	const PartnerSql & p_sd = partner_info.partner_data;

	info.partner_info.attr_pair[BATTLE_ATTR_CUR_HP] = p_sd.cur_hp;
	info.partner_info.attr_pair[BATTLE_ATTR_CUR_MP] = p_sd.cur_mp;
	info.partner_info.capability = attr_con.capability;
	info.partner_info.crytal_id = partner_info.partner_crytal.current_crytal_id;
	info.partner_info.use_time = partner_info.partner_crytal.use_time;
	info.partner_info.skin_id = partner_info.cur_skin_id;
	info.partner_info.level = partner_info.level;
	info.partner_info.quality_segment = partner_info.quality_segment;
	info.partner_info.quality = partner_info.new_quality;
	info.partner_info.intensify_level = partner_info.intensify_level;
	info.partner_info.biography_open_flag = partner_info.biography_open_flag;
	info.partner_info.biography_fetch_flag = partner_info.biography_fetch_flag;
	info.partner_info.soar_count = partner_info.soar_count;
	info.partner_info.soar_arms_level = partner_info.soar_arms_level;
	info.partner_info.rank = RankManager::Instance().GetPartnerTopRankShadow()
		->GetPartnerTopRank(m_role_module_mgr->GetUid(), partner_id);

	memcpy(info.partner_info.put_on_pact_id_list, partner_info.put_on_pact_id_list, sizeof(info.partner_info.put_on_pact_id_list));

	m_partner_pact_list[partner_id].GetBePactBaseAttr(info.partner_info.attach_base_attr_list);

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Partner::OnOpenBiography(int partner_id, int biography_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
		return;
	}
	const PartnerBiographyCfg * biography_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBiographyCfg(partner_id, biography_id);
	if (NULL == biography_cfg)
	{
		return;
	}
	NewPartnerBaseInfo & base_info = m_new_partner.partner_base_info[partner_id];
	base_info.biography_open_flag |= (1 << biography_id);
	this->SendPartnerSingleInfo(partner_id);
}

void Partner::OnFetchBiography(int partner_id, int biography_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
		return;
	}
	const PartnerBiographyCfg * biography_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBiographyCfg(partner_id, biography_id);
	if (NULL == biography_cfg)
	{
		return;
	}
	NewPartnerBaseInfo & base_info = m_new_partner.partner_base_info[partner_id];
	if (base_info.partner_list.upgrade_lv < biography_cfg->need_upgrade)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_BIOGRAPHY_UPGRADE_LV_NOT_WORTH);
		return;
	}
	if (base_info.biography_fetch_flag & (1 << biography_id))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	m_role_module_mgr->GetMoney()->AddCoinBind(biography_cfg->coin, __FUNCTION__);
	if (!biography_cfg->reward_list.empty())
	{	
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)biography_cfg->reward_list.size(), &biography_cfg->reward_list[0]))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)biography_cfg->reward_list.size(), &biography_cfg->reward_list[0], PUT_REASON_PARTNER_BIOGRAPHY))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
	base_info.biography_fetch_flag |= (1 << biography_id);
	this->SendPartnerSingleInfo(partner_id);
}

void Partner::GetSynFightPartnerIdList(int max_out_num, int * out_list, int * out_num)
{
	*out_num = GetMin(max_out_num, PARTNER_SYNFIGHT_NUM_MAX);

	for (int t_cur = 0; t_cur < *out_num; ++t_cur)
	{
		out_list[t_cur] = m_new_partner.synfight_list[t_cur];
	}
}

std::vector<int> Partner::GetSynFightPartnerIdList2() const
{
	std::vector<int> ret;
	for (int t_cur = 0; t_cur < ARRAY_LENGTH(m_new_partner.synfight_list); ++t_cur)
	{
		int partner_id = m_new_partner.synfight_list[t_cur];
		if (!this->IsPartnerInvalid(partner_id))
		{
			ret.push_back(partner_id);
		}
	}

	return ret;
}

std::set<int> Partner::GetSynFightPartnerIdList3() const
{
	std::set<int> ret;
	for (int t_cur = 0; t_cur < ARRAY_LENGTH(m_new_partner.synfight_list); ++t_cur)
	{
		int partner_id = m_new_partner.synfight_list[t_cur];
		if (!this->IsPartnerInvalid(partner_id))
		{
			ret.insert(partner_id);
		}
	}

	return ret;
}


bool Partner::GetBattleCharacterData(int partner_id, PartnerBattleData * out_data, int * fight_list, int fight_num, bool is_use_cur_hp_mp)
{
	if (NULL == out_data) return false;
	out_data->Reset();
	BattleCharacterData* bcd = &out_data->battle_data;

	if (this->IsPartnerInvalid(partner_id)) return false;

	const NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];
	const NewPartnerCon& p_pc = m_new_partner.partner_base_info[partner_id].partner_list;
	const PartnerSql& p_ps = m_new_partner.partner_base_info[partner_id].partner_data;
	const PartnerCrytal& p_crystal = m_new_partner.partner_base_info[partner_id].partner_crytal;
	const PartnerGiftTree & p_gift = m_partner_gift_tree_list.partner_gift_tree_list[partner_id];

	const PartnerBasicCfg * p_pbc = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (NULL == p_pbc) return false;

	bcd->character_id = partner_id;
	bcd->character_type = BATTLE_CHARACTER_TYPE_PARTNER;
	bcd->level = pbi.level;
	F_STRNCPY(bcd->name, p_pbc->name, sizeof(GameName));
	bcd->profession = p_pbc->profession;
	bcd->race = (char)p_pbc->race_type;
	bcd->quality = pbi.new_quality;
	bcd->weapon_type = WEAPON_TYPE_NONE;	// 伙伴一律空手
	bcd->weapon_id = 0;
	bcd->is_use_cur_hp_mp = (is_use_cur_hp_mp ? 1 : 0);
	bcd->is_long_range_unit = static_cast<char>(p_pbc->is_long_range_unit);

	memcpy(bcd->attr_list, m_partner_attr_list[partner_id].attr_list, sizeof(bcd->attr_list));

	// 伙伴水晶
	ItemID element_crystal_id = p_crystal.current_crytal_id;
	int crstal_remain_times = p_crystal.use_time;
	if (element_crystal_id > 0 && crstal_remain_times > 0)
	{
		const ItemBase* element_item = ITEMPOOL->GetItem(element_crystal_id);
		if (NULL != element_item)
		{
			const ElementCrystal* element_crystal = (const ElementCrystal*)element_item;
			bcd->attr_list[BATTLE_ATTR_ELEMENT_FIRE] = element_crystal->GetElementFire();
			bcd->attr_list[BATTLE_ATTR_ELEMENT_WATER] = element_crystal->GetElementWater();
			bcd->attr_list[BATTLE_ATTR_ELEMENT_WIND] = element_crystal->GetElementWind();
			bcd->attr_list[BATTLE_ATTR_ELEMENT_EARTH] = element_crystal->GetElementEarth();
			bcd->wear_element_type = element_crystal->GetElementType();
		}
		else
		{
			bcd->attr_list[BATTLE_ATTR_ELEMENT_FIRE] = 0;
			bcd->attr_list[BATTLE_ATTR_ELEMENT_WATER] = 0;
			bcd->attr_list[BATTLE_ATTR_ELEMENT_WIND] = 0;
			bcd->attr_list[BATTLE_ATTR_ELEMENT_EARTH] = 0;
		}
	}
	else
	{
		bcd->attr_list[BATTLE_ATTR_ELEMENT_FIRE] = 0;
		bcd->attr_list[BATTLE_ATTR_ELEMENT_WATER] = 0;
		bcd->attr_list[BATTLE_ATTR_ELEMENT_WIND] = 0;
		bcd->attr_list[BATTLE_ATTR_ELEMENT_EARTH] = 0;
	}

	// 伙伴技能
	bcd->skill_num = 0;
	const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
		partner_id, LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(pbi.new_quality), pbi.quality_segment));
	if (NULL != q_cfg)
	{
		for (int t_s_num = 0; t_s_num < PARTNER_COMMON_SKILL_MAX; ++t_s_num)
		{
			BattleCharacterData::SkillItem & od_sl = bcd->skill_list[bcd->skill_num];
			if (p_pc.skill_list[t_s_num] > 0) // 启用了技能
			{
				od_sl.skill_id = q_cfg->common_skill_id_list[t_s_num];
				od_sl.skill_level = q_cfg->common_skill_lv_list[t_s_num];
				bcd->skill_num += 1;
			}
			else if (NULL != SkillPool::Instance().GetPassiveSkillCfg(q_cfg->common_skill_id_list[t_s_num]))
			{
				od_sl.skill_id = q_cfg->common_skill_id_list[t_s_num];
				od_sl.skill_level = q_cfg->common_skill_lv_list[t_s_num];
				bcd->skill_num += 1;

				NewPartnerCon& t = m_new_partner.partner_base_info[partner_id].partner_list;
				t.skill_list[t_s_num] = 1;
			}
		}
	}

	const PartnerLevelSkillCfg* skill_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetLevelSkillCfg(partner_id, p_pc.upgrade_lv);
	if (NULL != skill_cfg)
	{
		const PartnerTalentCfg * p_ptc = LOGIC_CONFIG->GetPartnerConfig()->GetPTalentSkillCfg(skill_cfg->gift_skill_id, skill_cfg->gift_skill_level);
		if (NULL != p_ptc && TALENT_PLAYER_PASSIVE_SKILL == p_ptc->effect_type)
		{
			if (bcd->skill_num < MAX_BATTLE_SKILL_NUM)
			{
				bcd->skill_list[bcd->skill_num].skill_id = p_ptc->param1;
				bcd->skill_list[bcd->skill_num].skill_level = skill_cfg->gift_skill_level;
				bcd->skill_num += 1;
			}
		}
	}

	if (is_use_cur_hp_mp)
	{
		bcd->attr_list[BATTLE_ATTR_MAX_HP] = p_ps.max_hp;
		bcd->attr_list[BATTLE_ATTR_MAX_MP] = p_ps.max_mp;
		bcd->attr_list[BATTLE_ATTR_CUR_HP] = p_ps.cur_hp;
		bcd->attr_list[BATTLE_ATTR_CUR_MP] = p_ps.cur_mp;
	}
	else
	{
		bcd->attr_list[BATTLE_ATTR_MAX_HP] = p_ps.max_hp;
		bcd->attr_list[BATTLE_ATTR_MAX_MP] = p_ps.max_mp;
		bcd->attr_list[BATTLE_ATTR_CUR_HP] = p_ps.max_hp;
		bcd->attr_list[BATTLE_ATTR_CUR_MP] = p_ps.max_mp;
	}

	bcd->capability = m_partner_attr_list[partner_id].capability;
	bcd->fly_flag = pbi.soar_count;
	out_data->skin_id = this->GetCurPutOnSkin(partner_id);
	out_data->battle_row = pbi.battle_pos > 0 ? 1 : 0;
	out_data->battle_column = -1;
	out_data->str_lv = pbi.intensify_level;

	out_data->other_sp_num = 0;
	short other_sp_num = out_data->other_sp_num;
	//家族灵脉
	m_role_module_mgr->GetRoleGuildInborn()->GetPassiveSkillByType(GUILD_INBORN_ADD_TARGET_TYPE_PARTNER, BATTLE_PARTNER_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	
	//修为特效
	m_role_module_mgr->GetXiuWei()->OnGetBattleEffect(XIU_WEI_CHARACTER_TYPE_PARTNER, BATTLE_PARTNER_OTHER_EFFECT_NUM, &other_sp_num, out_data->other_sp_list);
	
	out_data->other_sp_num = other_sp_num;

	{
		//伙伴天赋
		int tree_level_count = 0;
		int role_level = m_role_module_mgr->GetRole()->GetLevel();
		for (int tree_point = 0; tree_point < MAX_PARTNER_GIFT_TREE_SKILL_NUM; tree_point++)
		{
			int tree_level = p_gift.gift_tree[tree_point];
			tree_level_count += tree_level;
		}
		const PartnerGiftSpListCfg * sp_cfg = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftSpCfg(partner_id);
		if (sp_cfg != NULL)
		{
			for (int sp_idx = 0; sp_idx < PartnerGiftSpListCfg::MAX_SP_NUM; sp_idx++)
			{
				if (sp_cfg->sp_list[sp_idx] != NULL && sp_cfg->sp_list[sp_idx]->inborn_level_ask <= tree_level_count)
				{
					const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_cfg->sp_list[sp_idx]->sp_id);
					if (NULL == spe_cfg) continue;

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
						continue;
					}

					if (out_data->other_sp_num < BATTLE_PARTNER_OTHER_EFFECT_NUM)
					{
						out_data->other_sp_list[out_data->other_sp_num++] = sp_cfg->sp_list[sp_idx]->sp_id;
					}
					else
					{
#ifdef _DEBUG
						assert(false);
#else
						gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_other_sp_num[%d] ", __FUNCTION__, BATTLE_PARTNER_OTHER_EFFECT_NUM);
#endif
					}
				}
			}
		}
	}

	//飞升武器特效
	this->GetSoarArmsPassiveSkillById(partner_id, BATTLE_PARTNER_OTHER_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);
	//收集符玉特效
	m_role_module_mgr->GetFuYuManager()->GetPassiveSkill(partner_id, BATTLE_PARTNER_OTHER_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);
	//收集星图特效
	m_role_module_mgr->GetStarChart()->GetPartnerBattleEffect(BATTLE_PARTNER_OTHER_EFFECT_NUM, &out_data->other_sp_num, out_data->other_sp_list);

	return true;
}

bool Partner::GetPlatformBattleCharacterData(int partner_id, PartnerBattleData ARG_OUT * out_data, PlatformBattlePartner ARG_OUT & platform_battle_partner, std::set<int> fight_list)
{
	if (this->IsPartnerInvalid(partner_id)) return false;

	PartnerBattleData partner_battle_data;
	bool b = GetBattleCharacterData(partner_id, &partner_battle_data, NULL, 0, false);
	if (!b)
	{
		return false;
	}

	if (out_data != NULL)
	{
		*out_data = partner_battle_data;
	}

	{
		platform_battle_partner.partner_id = partner_id;
		platform_battle_partner.level = partner_battle_data.battle_data.level;
		platform_battle_partner.capability = this->GetParterCapability(partner_id);
		platform_battle_partner.partner_battle_data.skin_id = this->GetCurPutOnSkin(partner_id);
		platform_battle_partner.partner_battle_data.battle_row = partner_battle_data.battle_row;
		platform_battle_partner.partner_battle_data.battle_data = partner_battle_data.battle_data;
		platform_battle_partner.partner_battle_data.other_sp_num = 0;
		platform_battle_partner.partner_battle_data.other_sp_list[platform_battle_partner.partner_battle_data.other_sp_num++] = m_role_module_mgr->GetProfAdvanceSpecialEffectId();
		platform_battle_partner.partner_battle_data.str_lv = partner_battle_data.str_lv;
	}

	return true;
}

void Partner::OnPartnerActive2(int partner_id)
{
	this->OnPartnerActive(partner_id);
}

int Partner::GetPartnerCurCount()
{
	int partner_count = 0;
	for (int t_apid = PARTNER_ID_START; t_apid <= m_partner_count && t_apid < PARTNER_NUM_MAX; ++t_apid)
	{
		const NewPartnerCon & p_partner = m_new_partner.partner_base_info[t_apid].partner_list;
		if (LOCK_STATE_LOCKED == p_partner.is_unlocked) continue;
		partner_count++;
	}
	return partner_count;
}

int Partner::GetPartnerCurCount(int quality)
{
	int partner_count = 0;
	for (int t_apid = PARTNER_ID_START; t_apid <= m_partner_count && t_apid < PARTNER_NUM_MAX; ++t_apid)
	{
		const NewPartnerCon & p_partner = m_new_partner.partner_base_info[t_apid].partner_list;
		if (LOCK_STATE_LOCKED == p_partner.is_unlocked) continue;

		if (p_partner.upgrade_lv >= quality)
		{
			partner_count++;
		}
	}
	return partner_count;

}

int Partner::GetMaxParnterGrade()
{
	int max_grade = 0;
	for (int t_apid = PARTNER_ID_START; t_apid <= m_partner_count && t_apid < PARTNER_NUM_MAX; ++t_apid)
	{
		const NewPartnerCon & p_partner = m_new_partner.partner_base_info[t_apid].partner_list;
		if (LOCK_STATE_LOCKED == p_partner.is_unlocked) continue;

		if (p_partner.upgrade_lv > max_grade)
		{
			max_grade = p_partner.upgrade_lv;
		}
	}

	return max_grade;
}

int Partner::GetMaxPartnerNumByGrade(int grade)
{
	int max_partner_num = 0;
	for (int t_apid = PARTNER_ID_START; t_apid <= m_partner_count && t_apid < PARTNER_NUM_MAX; ++t_apid)
	{
		const NewPartnerCon & p_partner = m_new_partner.partner_base_info[t_apid].partner_list;
		if (LOCK_STATE_LOCKED == p_partner.is_unlocked) continue;

		if (p_partner.upgrade_lv >= grade)
		{
			max_partner_num++;
		}
	}

	return max_partner_num;
}

int Partner::GetWearSetEquipNum()
{
	int wear_set_equip_num = 0;
	for (int t_apid = PARTNER_ID_START; t_apid <= m_partner_count && t_apid < PARTNER_NUM_MAX; ++t_apid)
	{
		const NewPartnerCon & p_partner = m_new_partner.partner_base_info[t_apid].partner_list;
		if (LOCK_STATE_LOCKED == p_partner.is_unlocked) continue;

		bool is_has_set_equip = true;
		for (int slot_idx = 0; slot_idx < PE_TYPE_MAX; ++slot_idx)
		{
			const NewPartnerEquipment & equipment = p_partner.equip_list[slot_idx];
			if (0 == equipment.partner_equip_id)
			{
				is_has_set_equip = false;
				break;
			}
		}
		if (is_has_set_equip)
		{
			wear_set_equip_num++;
		}
	}

	return wear_set_equip_num;
}

int Partner::GetPartnerNumByQualityNum(int quailty_num)
{
	int quality_partner_num = 0;
	for (int t_apid = PARTNER_ID_START; t_apid <= m_partner_count && t_apid < PARTNER_NUM_MAX; ++t_apid)
	{
		const NewPartnerCon & p_partner = m_new_partner.partner_base_info[t_apid].partner_list;
		if (LOCK_STATE_LOCKED == p_partner.is_unlocked) continue;

		//已解锁的伙伴默认添加(觉醒次数<=0)
		if (quailty_num <= 0)
		{
			quality_partner_num++;
			continue;
		}

		const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(p_partner.partner_id);
		if (NULL == basic_cfg) continue;

		int init_quality_up_type = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(basic_cfg->quality);
		int now_quality_up_type = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(m_new_partner.partner_base_info[t_apid].new_quality);
		int count = 0;
		for (int type = init_quality_up_type; type < now_quality_up_type && type < QUALITY_TYPE_MAX; ++type)
		{
			count += PARTNER_QUALITY_SEGMENT_MAX_NUM;
		}
		if (m_new_partner.partner_base_info[t_apid].quality_segment > 1)
		{
			count += (m_new_partner.partner_base_info[t_apid].quality_segment - 1);
		}
		if(count >= quailty_num)
		{
			quality_partner_num++;
		}
	}

	return quality_partner_num;
}


void Partner::ReCalAllPartnerAttr(int notice_type)
{
	for (int t_cur = PARTNER_ID_START; t_cur <= m_partner_count && t_cur < PARTNER_NUM_MAX; ++t_cur)
	{
		this->ReCalcPartnerAttr(t_cur, true, notice_type);
	}

	this->SyncPartnerDataPlatformBattle();
	this->SendAllPartnerInfo();
	if (4 != m_new_partner.is_refresh_store_flag)
	{
		const FunOpenCfg* funopen_cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_SHAN_MYSTERY_STORE);
		if (NULL != funopen_cfg)
		{
			if (funopen_cfg->limit_level <= m_role_module_mgr->GetRole()->GetLevel())
			{
				this->OnStoreRefresh(true);
				m_new_partner.is_refresh_store_flag = 4;

				// 立即获得第一次免费刷新
				m_new_partner.next_free_refresh_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
				m_partner.next_free_refresh_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
				this->SendStoreInfo();
			}
		}
	}
}

int Partner::GetMaxFourPartnerCap()
{
	int four_max_partner_count = 0;
	std::set<int> m_partner_cap_set;
	for (int id = PARTNER_ID_START; id <= m_partner_count && id < PARTNER_NUM_MAX; id++)
	{
		if (this->IsPartnerInvalid(id)) continue;
		if (m_new_partner.partner_base_info[id].partner_list.is_unlocked != LOCK_STATE_UNLOCK) continue;

		m_partner_cap_set.insert(m_partner_attr_list[id].capability);
	}
	if (!m_partner_cap_set.empty())
	{
		int i = 0;
		for (std::set<int>::reverse_iterator it = m_partner_cap_set.rbegin(); it != m_partner_cap_set.rend(); i++, it++)
		{
			if (i >= 4) break;

			four_max_partner_count += *it;
		}
	}

	return four_max_partner_count;
}

int Partner::GetFightParatnerCap()
{
	int capability_count = 0;
	for (int i = 0; i < PARTNER_SYNFIGHT_NUM_MAX; i++)
	{
		if (this->IsPartnerInvalid(m_new_partner.synfight_list[i]) || m_new_partner.partner_base_info[m_new_partner.synfight_list[i]].partner_list.is_unlocked == LOCK_STATE_LOCKED) continue;
		capability_count += m_new_partner.partner_base_info[m_new_partner.synfight_list[i]].capability;
	}

	return capability_count;
}

void Partner::OnRoleGuildInbornChange()
{
	for (int t_cur = PARTNER_ID_START; t_cur <= m_partner_count && t_cur < PARTNER_NUM_MAX; ++t_cur)
	{
		this->ReCalcPartnerAttr(t_cur, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_GUILD_INBORN);
	}

	this->SyncPartnerDataPlatformBattle();
	this->SendAllPartnerInfo();
}

void Partner::SyncFightListFromFormation(ARG_IN int* fight_partner_list, ARG_IN int* battle_row_list, int max_num)
{
	std::set<int> changed_partner_set;
	for (int i = 0; i < PARTNER_SYNFIGHT_NUM_MAX; ++i)
	{
		if (m_new_partner.synfight_list[i] > 0)
		{
			int partner_id = m_new_partner.synfight_list[i];
			if (this->IsPartnerValid(partner_id))
			{
				NewPartnerCon& pc = m_new_partner.partner_base_info[partner_id].partner_list;
				pc.fight_state = BATTLE_STATE_AWAIT;
				changed_partner_set.insert(partner_id);
				m_new_partner.synfight_list[i] = 0;
			}			
		}
	}

	for (int i = 0; i < max_num && i < PARTNER_SYNFIGHT_NUM_MAX; ++i)
	{
		int partner_id = fight_partner_list[i];
		if (partner_id > 0)
		{
			if (this->IsPartnerValid(partner_id))
			{
				NewPartnerCon& pc = m_new_partner.partner_base_info[partner_id].partner_list;
				NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];
				pc.fight_state = BATTLE_STATE_FIGHT;
				pbi.battle_pos = battle_row_list[i];
				m_new_partner.synfight_list[i] = partner_id;

				std::set<int>::iterator it = changed_partner_set.find(partner_id);
				if (it != changed_partner_set.end())
				{
					// 原本就有这个伙伴，则从set中删掉（表示没有被改变战斗状态）
					changed_partner_set.erase(it);
				}
				else
				{
					changed_partner_set.insert(partner_id);
				}
			}
		}
	}

	std::set<int>::iterator it = changed_partner_set.begin();
	for (; it != changed_partner_set.end(); ++it)
	{
		int partner_id = *it;
		this->SendPartnerFightState(partner_id);
	}
	this->SendSynfightInfoList();
}

int Partner::UpLevel(int partner_id, int num)
{
	if (this->IsPartnerInvalid(partner_id)) return -__LINE__;
	if (!this->IsPartnerAlreadyActive(partner_id)) return -__LINE__;
	if (num <= 0)return -__LINE__;

	NewPartnerBaseInfo & partner_base_info = m_new_partner.partner_base_info[partner_id];

	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	int old_level = partner_base_info.level;
	if (partner_base_info.level >= role_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_UP_LEVEL_OVER_ROLE_LEVEL);
		return -__LINE__;
	}

	int comsume_value = 0;
	//点击“提升10级”按钮时，如果该伙伴的等级会超过上限，那么消耗对应数量的元灵之髓并提升等级至上限
	int next_level = partner_base_info.level + num;
	if (next_level > role_level)
	{
		next_level = role_level;
	}

	for (int i = old_level; i < next_level; ++i)
	{
		const PartnerLevelCfg * level_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerLevelCfg(partner_id, i);
		if (level_cfg != NULL)
		{
			comsume_value += level_cfg->comsume_money_value;
		}
	}

	const PartnerLevelCfg * next_level_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerLevelCfg(partner_id, next_level);
	if (next_level_cfg == NULL) return -__LINE__;

	if (!m_role_module_mgr->GetMoney()->UseOtherCoin(comsume_value, next_level_cfg->comsume_money_type, __FUNCTION__))
	{	
		return -__LINE__;
	}

	partner_base_info.level = next_level;
	this->ReCalcPartnerAttr(partner_id, true);
	this->SendPartnerSingleInfo(partner_id);
	gamelog::g_log_partner.printf(LL_INFO, "%s %d | user[%d, %s] | partner_id:%d level:%d to %d,role_level%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		partner_id, old_level, next_level, role_level);

	if (this->IsInPlatformBattle(partner_id))
	{
		this->SyncPartnerDataPlatformBattle();
	}

	{
		PartnerSql & p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
		NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];

		this->SyncPartnerInfoToTeam(partner_id, p_sql.cur_hp, p_sql.max_hp, p_sql.cur_mp, p_sql.max_mp, this->GetCurPutOnSkin(partner_id), pbi.level);
	}
	EventHandler::Instance().OnPartnerUpLevel(m_role_module_mgr->GetRole(), partner_id, old_level, next_level);
	return 0;
}

int Partner::ResetLevel(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return -__LINE__;
	if (!this->IsPartnerAlreadyActive(partner_id)) return -__LINE__;

	NewPartnerBaseInfo & partner_base_info = m_new_partner.partner_base_info[partner_id];

	int old_level = partner_base_info.level;

	std::map<int, int> ret_value;
	for (int i = 1; i < partner_base_info.level; ++i)
	{
		const PartnerLevelCfg * level_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerLevelCfg(partner_id, i);
		if (level_cfg == NULL) return -__LINE__;

		ret_value[level_cfg->comsume_money_type] += level_cfg->comsume_money_value;
	}

	//消耗
	const PartnerOtherCfg & other_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg();
	if (!m_role_module_mgr->GetMoney()->UseCoinBind(other_cfg.reset_level_need_coint, __FUNCTION__))
	{
		return -__LINE__;
	}
	
	for (std::map<int, int>::iterator iter = ret_value.begin(); iter != ret_value.end(); ++iter)
	{
		int value = static_cast<int>(other_cfg.reset_level_ret_rate / 100.0 * iter->second);
		m_role_module_mgr->GetMoney()->AddOtherCoin(value, iter->first, __FUNCTION__);
	}

	partner_base_info.level = 1;
	this->ReCalcPartnerAttr(partner_id, true);
	this->SendPartnerSingleInfo(partner_id);

	if (this->IsInPlatformBattle(partner_id))
	{
		this->SyncPartnerDataPlatformBattle();
	}

	gamelog::g_log_partner.printf(LL_INFO, "%s %d | user[%d, %s] | partner_id:%d level:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
		partner_id, old_level);
	
	{
		PartnerSql & p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
		NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];

		this->SyncPartnerInfoToTeam(partner_id, p_sql.cur_hp, p_sql.max_hp, p_sql.cur_mp, p_sql.max_mp, this->GetCurPutOnSkin(partner_id), pbi.level);
	}

	return 0;
}

int Partner::GetCurrentQuality(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return -100;
	return m_new_partner.partner_base_info[partner_id].new_quality;
}

bool Partner::QualityUpgrade(int partner_id, int upgrade_type, bool is_p)
{
	if (this->IsPartnerInvalid(partner_id)) return false;

	const PartnerBasicCfg *pb_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (!pb_cfg) return false;

	if (!this->IsPartnerAlreadyActive(partner_id)) return false;

	const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
		partner_id, upgrade_type, m_new_partner.partner_base_info[partner_id].quality_segment));
	if (!q_cfg) return false;

	m_new_partner.partner_base_info[partner_id].partner_quality_type = upgrade_type;
	if (is_p)
	{
		if (!this->CanQualityUpgrade(partner_id, upgrade_type, m_new_partner.partner_base_info[partner_id].quality_segment)) return false;
	}

	AttrCon p_att = m_partner_attr_list[partner_id];
	Attribute tmp_attr_list[BATTLE_ATTR_MAX] = { 0 };			// 原本的属性值
	memcpy(tmp_attr_list, p_att.attr_list, sizeof(tmp_attr_list));

	//使用升品道具
	if (is_p)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(q_cfg->need_item, 1, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return false;
		}
	}

	int old_quality_segment = m_new_partner.partner_base_info[partner_id].quality_segment;
	int old_quality = m_new_partner.partner_base_info[partner_id].new_quality;

	if (is_p)
	{	
		this->AddAttrByQualityUp(p_att, q_cfg);
		this->OnPartnerQualityUp(partner_id);
	
		if (m_new_partner.partner_base_info[partner_id].new_quality >= PARTNERQUALITYT_PURPLE && m_new_partner.partner_base_info[partner_id].new_quality < PQT_MAX)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_system_post_partner,
				m_role_module_mgr->GetRole()->GetName(), partner_id, m_new_partner.partner_base_info[partner_id].old_quality, q_cfg->need_item,
				m_new_partner.partner_base_info[partner_id].new_quality, m_new_partner.partner_base_info[partner_id].quality_segment);
			if (length > 0)
			{
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				} 
			}
		}
	}

	once_init = 1;
	this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_QUALITY_UP);
	this->SendQualityUpInfo(partner_id, upgrade_type, is_p ? Protocol::SCPartnerQualityUp::NOTICETYPE_UP : Protocol::SCPartnerQualityUp::NOTICETYPE_ACTIVE);
	if (is_p)
	{
		EventHandler::Instance().OnPartnerQualityUp(m_role_module_mgr->GetRole(), partner_id);
		LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_PARTNER_AWAKENING, m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetRole()->GetPlatName(),
			q_cfg->need_item, partner_id, NULL, NULL, old_quality, old_quality_segment, m_new_partner.partner_base_info[partner_id].new_quality, m_new_partner.partner_base_info[partner_id].quality_segment, 0);
	}
	once_init = 0;

	gamelog::g_log_partner_qualityup.printf(LL_INFO, "Partner Quality Up:user[%d, %s] partner_id[%d],QualityUp Type[%d], Quality_segment[%d])", m_role_module_mgr->GetRole()->GetUID(),
		m_role_module_mgr->GetRole()->GetName(), partner_id, upgrade_type, m_new_partner.partner_base_info[partner_id].quality_segment);
	return true;
}

void Partner::AddAttrByQualityUp(AttrCon & _p_att, const QualityUpgradeCfg * _q_cfg)
{
	_p_att.attr_list[BATTLE_ATTR_ATTACK] += _q_cfg->add_attr[PARTNER_ATTR_ATK];
	_p_att.attr_list[BATTLE_ATTR_DEFENSE] += _q_cfg->add_attr[PARTNER_ATTR_DEF];
	_p_att.attr_list[BATTLE_ATTR_AGILITY] += _q_cfg->add_attr[PARTNER_ATTR_AGI];
	_p_att.attr_list[BATTLE_ATTR_RECOVERY] += _q_cfg->add_attr[PARTNER_ATTR_REC];
	_p_att.attr_list[BATTLE_ATTR_MENTAL] += _q_cfg->add_attr[PARTNER_ATTR_SPI];
	_p_att.attr_list[BATTLE_ATTR_MAX_HP] += _q_cfg->add_attr[PARTNER_ATTR_MHP];
	_p_att.attr_list[BATTLE_ATTR_MAX_MP] += _q_cfg->add_attr[PARTNER_ATTR_MMP];

	_p_att.attr_list[BATTLE_ATTR_CUR_HP] = _p_att.attr_list[BATTLE_ATTR_MAX_HP];
	_p_att.attr_list[BATTLE_ATTR_CUR_MP] = _p_att.attr_list[BATTLE_ATTR_MAX_MP];
}

void Partner::AddAttrByQualityUp(AttributeList & _p_att, const QualityUpgradeCfg * _q_cfg)
{
	_p_att.m_attrs[BATTLE_ATTR_ATTACK] += _q_cfg->add_attr[PARTNER_ATTR_ATK];
	_p_att.m_attrs[BATTLE_ATTR_DEFENSE] += _q_cfg->add_attr[PARTNER_ATTR_DEF];
	_p_att.m_attrs[BATTLE_ATTR_AGILITY] += _q_cfg->add_attr[PARTNER_ATTR_AGI];
	_p_att.m_attrs[BATTLE_ATTR_RECOVERY] += _q_cfg->add_attr[PARTNER_ATTR_REC];
	_p_att.m_attrs[BATTLE_ATTR_MENTAL] += _q_cfg->add_attr[PARTNER_ATTR_SPI];
	_p_att.m_attrs[BATTLE_ATTR_MAX_HP] += _q_cfg->add_attr[PARTNER_ATTR_MHP];
	_p_att.m_attrs[BATTLE_ATTR_MAX_MP] += _q_cfg->add_attr[PARTNER_ATTR_MMP];
}

void Partner::AddQualityByQualityUp(int partner_id)
{
	int up_temp = m_new_partner.partner_base_info[partner_id].new_quality + 1;
	if (up_temp < PQT_MAX)
	{
		m_new_partner.partner_base_info[partner_id].new_quality = up_temp;
		m_new_partner.partner_base_info[partner_id].quality_segment = 1;
		m_new_partner.partner_base_info[partner_id].partner_quality_type = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(m_new_partner.partner_base_info[partner_id].new_quality);
	}
}

bool Partner::CanQualityUpgrade(int _partner_id, int _upgrade_type, int quality_segment)
{
	if (_upgrade_type < QUALITY_TYPE_MIN || _upgrade_type >= QUALITY_TYPE_MAX || this->IsPartnerInvalid(_partner_id)) return false;

	const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(
		LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(_partner_id, _upgrade_type, quality_segment)
	);
	if (!q_cfg) return false;

	const NewPartnerBaseInfo & base_info = m_new_partner.partner_base_info[_partner_id];
	int max_quality_upgrade_limit = 0;
	int max_quality_segment_limit = 0;
	const PartnerSoarCfg * soar_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarCfg(base_info.soar_count);
	if (NULL == soar_cfg || base_info.soar_count <= 0)	//该伙伴尚未飞升
	{
		const PartnerOtherCfg & other_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg();
		max_quality_upgrade_limit = other_cfg.quality_upgrade_limit;
		max_quality_segment_limit = other_cfg.quality_segment_limit;
	}
	else
	{
		max_quality_upgrade_limit = soar_cfg->soar_quality_upgrade;
		max_quality_segment_limit = soar_cfg->soar_quality_segment;
	}
	int cur_quality_upgrade = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(base_info.new_quality);
	if (cur_quality_upgrade > max_quality_upgrade_limit || (cur_quality_upgrade == max_quality_upgrade_limit && base_info.quality_segment >= max_quality_segment_limit))
	{
		return false;
	}

	//查看背包是否有升品道具
	if (!m_role_module_mgr->GetKnapsack()->HasItem(q_cfg->need_item, 1)) return false;

	const ItemBase *itembase = ITEMPOOL->GetItem(q_cfg->need_item);
	if (!itembase) return false;
	if (itembase->GetItemType() != I_TYPE_EXPENSE) return false;

	const NorexItem *norex_item = (NorexItem *)itembase;
	if (NULL == norex_item) return false;
	if (norex_item->GetUseType() != NorexItem::I_NOREX_ITEM_PARTNER_QUALITYUP) return false;

	if (m_role_module_mgr->GetRole()->GetLevel() < q_cfg->level_limit)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_partner_quality_upgrade, q_cfg->level_limit);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
			}
		}
		return false;
	}

	if (q_cfg->need_num == 0) return false;
	int should_up = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(this->GetCurrentQuality(_partner_id));
	if (should_up != _upgrade_type) return false;

	return true;
}

void Partner::SendQualityUpInfo(int _partner_id, int _upgrade_type, char notice_type)
{
	Protocol::SCPartnerQualityUp info;
	info.partner_id = _partner_id;
	info.up_type = _upgrade_type;
	info.quality = m_new_partner.partner_base_info[_partner_id].new_quality;
	Attribute* _attr_list = this->GetAttrListByPartnerID(_partner_id);
	if (_attr_list == NULL)return;
	info.att_list[PARTNER_ATTR_MHP] = _attr_list[BATTLE_ATTR_MAX_HP];
	info.att_list[PARTNER_ATTR_MMP] = _attr_list[BATTLE_ATTR_MAX_MP];
	info.att_list[PARTNER_ATTR_ATK] = _attr_list[BATTLE_ATTR_ATTACK];
	info.att_list[PARTNER_ATTR_DEF] = _attr_list[BATTLE_ATTR_DEFENSE];
	info.att_list[PARTNER_ATTR_AGI] = _attr_list[BATTLE_ATTR_AGILITY];
	info.att_list[PARTNER_ATTR_REC] = _attr_list[BATTLE_ATTR_RECOVERY];
	info.att_list[PARTNER_ATTR_SPI] = _attr_list[BATTLE_ATTR_MENTAL];

	info.notic_type = notice_type;
	info.quality_segment = m_new_partner.partner_base_info[_partner_id].quality_segment;
	info.rew_2 = 0;

	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));

}

void Partner::PartnerEqUnlockEquipment(int partner_id, int slot_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
		return;
	}

	NewPartnerEquipment & quote_equip = p_param.equip_list[slot_id];
	//检查当前命格等级
	const NewPartnerEqLimitCfg * info_cfg = LOGIC_CONFIG->GetPartnerEquipmentConfig()->GetNewPEUnlockLimitBegin(partner_id, slot_id);
	if (info_cfg == NULL)return;

	if (info_cfg->hole_num > 0 && !m_role_module_mgr->GetKnapsack()->HasItem(info_cfg->hole_need, info_cfg->hole_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	if (quote_equip.unlock_level >= info_cfg->hole_level)
	{
		return;
	}

	if (info_cfg->price_num > 0)
	{
		m_role_module_mgr->GetMoney()->UseMoney(info_cfg->price_type, info_cfg->price_num, __FUNCTION__);
	}

	if (info_cfg->hole_num > 0)
	{
		m_role_module_mgr->GetKnapsack()->ConsumeItem(info_cfg->hole_need, info_cfg->hole_num, __FUNCTION__);
	}

	int old = quote_equip.unlock_level;
	quote_equip.unlock_level = info_cfg->hole_level;

	this->SendPartnerEquipsInfo(partner_id, slot_id);

	gamelog::g_log_partner.printf(LL_INFO, "%s line:%d | role[%d, %s] partner_id[%d] slot_id[%d] old_level:%d level:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		partner_id, slot_id, old, quote_equip.unlock_level);
}

void Partner::PartnerEqPullOnEquipment(int partner_id, int grid_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
		return;
	}

	const ItemGridData* grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, grid_id);
	if (NULL == grid_data)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(grid_data->item_id);
	if (NULL == item_base || I_TYPE_PARTNER_NATAL_CARD != item_base->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	PartnerNatalCard * natal_card = (PartnerNatalCard *)item_base;
	if (NULL == natal_card) return;

	int equip_type = natal_card->GetEquipType();
	if (equip_type < 0 || equip_type >= PE_TYPE_MAX) return;

	ItemID target_item_id = grid_data->item_id;

	NewPartnerEquipment target_attr; target_attr.Reset();

	//检查命格是否正确属性
	for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
	{
		if(grid_data->param->partner_natal_card_param.m_voc_list[i].attr_value == 0)continue;
		if (!LOGIC_CONFIG->GetPartnerEquipmentConfig()->CheckAttrTypeAndValue(grid_data->item_id, i, 
			grid_data->param->partner_natal_card_param.m_voc_list[i].attr_type, 
			grid_data->param->partner_natal_card_param.m_voc_list[i].attr_value))
		{
			//属性对不上
			natal_card->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), grid_data->param, PUT_REASON_NO_NOTICE);//重新再随机一次属性
			natal_card->SetRandomNetValueFlag(grid_data->param);
			break;
		}
	}

	for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
	{
		target_attr.m_voc_list[i].attr_type = grid_data->param->partner_natal_card_param.m_voc_list[i].attr_type;
		target_attr.m_voc_list[i].attr_value = grid_data->param->partner_natal_card_param.m_voc_list[i].attr_value;
	}

	NewPartnerEquipment & quote_equip = p_param.equip_list[equip_type];

	{
		if (quote_equip.unlock_level < item_base->GetLimitLevel())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_EQUIP_UNLOCK_TOO_LOW);
			return;
		}

		if (item_base->GetLimitLevel() > m_role_module_mgr->GetRole()->GetLevel())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return;
		}
	}

	ItemDataWrapper get_item(quote_equip.partner_equip_id, 1, 0);
	get_item.has_param = 1;
	static NetValueItemParam tmp_item_param; tmp_item_param.Reset();
	NewPartnerNatalCardParam & natal_param = tmp_item_param.partner_natal_card_param;
	natal_param.has_net_value = 1;

	for (int voc_idx = 0; voc_idx < MAX_PARTNER_NATAL_CARD_VOC_NUM; voc_idx++)
	{
		natal_param.m_voc_list[voc_idx] = quote_equip.m_voc_list[voc_idx];
	}
	
	tmp_item_param.GetData(get_item.param_data);

	if (m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, grid_id, 1, __FUNCTION__))
	{
		quote_equip.partner_equip_id = target_item_id;

		for (int voc_idx = 0; voc_idx < MAX_PARTNER_NATAL_CARD_VOC_NUM; voc_idx++)
		{
			quote_equip.m_voc_list[voc_idx] = target_attr.m_voc_list[voc_idx];
		}

		m_role_module_mgr->GetKnapsack()->Put(get_item, PUT_REASON_NO_NOTICE);

		gamelog::g_log_partner.printf(LL_INFO, "%s line:%d | role[%d, %s] partner_id[%d] slot_id[%d] item_id:%d attr_voc[%s] ,old_item_id:%d old_attr_voc[%s]", __FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), 
			m_role_module_mgr->GetRoleName(), partner_id, equip_type, target_item_id, quote_equip.GetAttrInfoLog(quote_equip.m_voc_list, MAX_PARTNER_NATAL_CARD_VOC_NUM), get_item.item_id, 
			natal_param.GetAttrInfoLog(natal_param.m_voc_list, MAX_PARTNER_NATAL_CARD_VOC_NUM));
	}

	this->ReCalcPartnerAttr(partner_id);
	this->SendPartnerEquipsInfo(partner_id, equip_type);	
	EventHandler::Instance().OnPartnerPutOnEquipment(m_role_module_mgr->GetRole(), partner_id);
}

void Partner::PartnerEqGetOffEquipment(int partner_id, int slot_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
		return;
	}

	NewPartnerEquipment & equip = p_param.equip_list[slot_id];
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(equip.partner_equip_id, 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return;
	}

	ItemDataWrapper get_item(equip.partner_equip_id, 1, 0);
	get_item.has_param = 1;
	static NetValueItemParam tmp_item_param; tmp_item_param.Reset();
	NewPartnerNatalCardParam & natal_param = tmp_item_param.partner_natal_card_param;
	natal_param.has_net_value = 1;
	for (int voc_idx = 0; voc_idx < MAX_PARTNER_NATAL_CARD_VOC_NUM; voc_idx++)
	{
		natal_param.m_voc_list[voc_idx] = equip.m_voc_list[voc_idx];
	}
	tmp_item_param.GetData(get_item.param_data);

	if (!m_role_module_mgr->GetKnapsack()->Put(get_item, PUT_REASON_NO_NOTICE)) return;

	equip.ResetByPutOff();
	this->ReCalcPartnerAttr(partner_id);
	this->SendPartnerEquipsInfo(partner_id, slot_id);
	EventHandler::Instance().OnPartnerTakeOffEquipment(m_role_module_mgr->GetRole(), partner_id);
	gamelog::g_log_partner.printf(LL_INFO, "%s line:%d | role[%d, %s] partner_id[%d] slot_id[%d] item_id:%d attr_voc[%s]", __FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		partner_id, slot_id, get_item.item_id, natal_param.GetAttrInfoLog(natal_param.m_voc_list, MAX_PARTNER_NATAL_CARD_VOC_NUM));
}

void Partner::PartnerEqResolveEquipment(int grid_idx)
{
	const ItemGridData* grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, grid_idx);
	if (NULL == grid_data)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(grid_data->item_id);
	if (NULL == item_base || I_TYPE_PARTNER_NATAL_CARD != item_base->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int limit_levle = item_base->GetLimitLevel();
	const int add_qiyun = LOGIC_CONFIG->GetPartnerEquipmentConfig()->GetPEResolveValue(limit_levle, item_base->GetColor());
	if (0 >= add_qiyun) return;

	if (m_role_module_mgr->GetMoney()->GetOtherCoin(MONEY_TYPE_LUCK_COIN) + add_qiyun > MAX_INT64)
	{
		return;
	}

	//分解掉命格卡
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, grid_idx, 1, __FUNCTION__))
	{
		return;
	}

	//加气运值
	int ret = m_role_module_mgr->GetMoney()->AddOtherCoin(add_qiyun, MONEY_TYPE_LUCK_COIN, __FUNCTION__);

	gamelog::g_log_partner_qualityup.printf(LL_INFO, "%s user[%s %d] resolve_grid_idx[%d] item_id[%d] add_qiyun[%d] add_succ[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetUID(), grid_idx, item_base->GetItemId(), add_qiyun, ret ? 1 : 0);
}

void Partner::SendElementCrystalInfo()
{
	Protocol::SCPartnerUseElementCrytalFight info;
	info.count = 0;

	for (int i = PARTNER_ID_START; i < PARTNER_NUM_MAX; ++i)
	{
		if (this->IsPartnerInvalid(i)) continue;
		if (m_new_partner.partner_base_info[i].partner_list.is_unlocked == LOCK_STATE_LOCKED) continue;

		PartnerUseElementCrytalFightInfo &pufc_info = info.once_info[info.count];
		pufc_info.partner_id = m_new_partner.partner_base_info[i].GetPartnerId();
		pufc_info.crytal_id = m_new_partner.partner_base_info[i].partner_crytal.current_crytal_id;
		pufc_info.use_time = m_new_partner.partner_base_info[i].partner_crytal.use_time;

		info.count += 1;

	}

	int info_len = sizeof(info) - (PARTNER_NUM_MAX - info.count) * sizeof(PartnerUseElementCrytalFightInfo);
	m_role_module_mgr->NetSend((const char *)&info, info_len);
}

int Partner::ClearMixedElementCrystal()
{
	int count = 0;
	for (int i = PARTNER_ID_START; i < PARTNER_NUM_MAX; ++i)
	{
		if (this->IsPartnerInvalid(i)) continue;

		NewPartnerBaseInfo & base_info = m_new_partner.partner_base_info[i];
		if (!IsMixedElementCrystalId(base_info.partner_crytal.current_crytal_id))
		{
			continue;
		}

		count += 1;
		this->RemoveElementCrystal(i, false, false);
	}

	this->SendElementCrystalInfo();

	return count;
}

void Partner::GMSetCountSub(int _partner_id, int _count)
{
	if (this->IsPartnerInvalid(_partner_id)) return;
	m_new_partner.partner_base_info[_partner_id].partner_crytal.use_time = _count;

	this->SendElementCrystalInfo();

}

void Partner::UseElementCrystalById(int _partner_id, ItemID _crystal_id)
{
	const ItemBase *item = ITEMPOOL->GetItem(_crystal_id);
	if (!item) return;
	if (item->GetItemType() != I_TYPE_ELEMENT_CRYSTAL) return;

	if (this->IsPartnerInvalid(_partner_id)) return;
	const PartnerBasicCfg *pb_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(_partner_id);
	if (!pb_cfg) return;

	if (!this->IsPartnerAlreadyActive(_partner_id)) return;

	CLEAR_PARTNER_CRYSTAL(m_partner_attr_list[_partner_id].attr_list);
	// 装备水晶
	PARTNER_CRYSTAL_PUT(_crystal_id, m_partner_attr_list[_partner_id].attr_list);

	const ElementCrystal* crystal = (const ElementCrystal*)item;

	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_WATER - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_WATER];
	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_FIRE - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_FIRE];
	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_WIND - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_WIND];
	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_EARTH - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_EARTH];

	m_new_partner.partner_base_info[_partner_id].partner_crytal.current_crytal_id = _crystal_id;
	m_new_partner.partner_base_info[_partner_id].partner_crytal.use_time = crystal->GetCrystalLimitTimes();

	static ItemDataWrapper wrapper;
	wrapper.Reset();
	wrapper.item_id = m_new_partner.partner_base_info[_partner_id].partner_crytal.current_crytal_id;
	wrapper.num = 1;
	wrapper.is_bind = true;

	this->SendElementCrystalInfo();
}

void Partner::UseElementCrystal(int _partner_id, int index, bool is_send)
{
	if (this->IsPartnerInvalid(_partner_id)) return;

	if (m_new_partner.partner_base_info[_partner_id].partner_crytal.current_crytal_id != 0)
	{
		this->RemoveElementCrystal(_partner_id);
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_partner_crytals_remove);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
			}
		}
	}

	const ItemGridData* item_data = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index);
	if (NULL == item_data) return;
	const ItemBase *item = NULL;

	if (item_data->num == 0) return;

	EquipmentOtherParam other_param;
	m_role_module_mgr->GetEquipmentManager()->GetInitParam(&other_param);

	item = ITEMPOOL->GetItem(item_data->item_id);
	if (item == NULL || NULL == item_data->param) return;
	if (item->GetItemType() != I_TYPE_ELEMENT_CRYSTAL) return;

	const PartnerBasicCfg *pb_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(_partner_id);
	if (!pb_cfg) return;

	if (item_data->item_id != 0 && item_data->param->element_crystal_param.used_times == 0) return;

	CLEAR_PARTNER_CRYSTAL(m_partner_attr_list[_partner_id].attr_list);
	// 装备水晶
	PARTNER_CRYSTAL_PUT(item_data->item_id, m_partner_attr_list[_partner_id].attr_list);

	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_WATER - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_WATER];
	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_FIRE - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_FIRE];
	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_WIND - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_WIND];
	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_EARTH - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_EARTH];

	m_new_partner.partner_base_info[_partner_id].partner_crytal.current_crytal_id = item_data->item_id;
	m_new_partner.partner_base_info[_partner_id].partner_crytal.use_time = item_data->param->element_crystal_param.used_times;

	m_role_module_mgr->GetKnapsack()->ClearGrid(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index);
	m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index, true, 0);

	this->ReCalcPartnerAttr(_partner_id);

	if (is_send)
	{
		this->SendElementCrystalInfo();
	}
}

void Partner::UseElementCrystalToFight(int index_1, int index_2, int index_3, int index_4)
{
	std::vector<int> fight_list = this->GetSynFightPartnerIdList2();

	std::vector<int> v_crytal_list;
	v_crytal_list.push_back(index_1);
	v_crytal_list.push_back(index_2);
	v_crytal_list.push_back(index_3);
	v_crytal_list.push_back(index_4);

	std::vector<int>::iterator cur_it = v_crytal_list.begin();

	for (int i = 0; i < (int)fight_list.size() && (int)v_crytal_list.size(); ++i)
	{
		int curr_partner_id = fight_list[i];
		if (m_new_partner.partner_base_info[curr_partner_id].partner_crytal.current_crytal_id != 0)
		{
			continue;
		}
		this->UseElementCrystal(curr_partner_id, *cur_it++, false);
	}

	this->SendElementCrystalInfo();
}

void Partner::RemoveElementCrystal(int _partner_id, bool is_send, bool is_put)
{
	if (this->IsPartnerInvalid(_partner_id)) return;

	CLEAR_PARTNER_CRYSTAL(m_partner_attr_list[_partner_id].attr_list);

	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_WATER - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_WATER];
	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_FIRE - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_FIRE];
	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_WIND - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_WIND];
	m_new_partner.partner_base_info[_partner_id].partner_crytal.att_list[BATTLE_ATTR_ELEMENT_EARTH - BATTLE_ATTR_ELEMENT_BEGIN] = m_partner_attr_list[_partner_id].attr_list[BATTLE_ATTR_ELEMENT_EARTH];

	if (is_put)
	{
		static NetValueItemParam tmp_item_param;
		tmp_item_param.Reset();

		tmp_item_param.element_crystal_param.used_times = m_new_partner.partner_base_info[_partner_id].partner_crytal.use_time;
		tmp_item_param.element_crystal_param.has_set_net_value = 1;

		static ItemDataWrapper wrapper;
		wrapper.Reset();
		wrapper.item_id = m_new_partner.partner_base_info[_partner_id].partner_crytal.current_crytal_id;
		wrapper.num = 1;
		wrapper.is_bind = true;
		tmp_item_param.GetData(wrapper.param_data);

		if (m_new_partner.partner_base_info[_partner_id].partner_crytal.use_time == 0)
		{
			m_role_module_mgr->GetKnapsack()->Put(wrapper, PUT_REASON_PARTNER_ELEMENT_TAKEOFF_BAD);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->Put(wrapper, PUT_REASON_PARTNER_ELEMENT_TAKEOFF_DEFAULT);
		}
	}

	m_new_partner.partner_base_info[_partner_id].partner_crytal.current_crytal_id = 0;
	m_new_partner.partner_base_info[_partner_id].partner_crytal.use_time = 0;

	this->ReCalcPartnerAttr(_partner_id);
	if (is_send)
	{
		this->SendElementCrystalInfo();
	}

}

void Partner::RemoveAllCurPartnerElementCrystal()
{
	std::vector<int> v_have_partner_id;
	this->GetPartnerList(&v_have_partner_id);

	for (int i = 0; i < (int)v_have_partner_id.size(); ++i)
	{
		this->RemoveElementCrystal(v_have_partner_id[i], false);
	}

	this->SendElementCrystalInfo();
}

/*void Partner::ElementCrystalCountSub(int _seq)
{
if (PARTNER_ERROR_SEQ == _seq) return;
if (m_partner.partner_crytal[_seq].partner_id == 0) return;
if (m_partner.partner_crytal[_seq].current_crytal_id == 0) return;

if (m_partner.partner_crytal[_seq].use_time - 1 == 0 && m_partner.partner_crytal[_seq].current_crytal_id != 0)
{
m_partner.partner_crytal[_seq].use_time -= 1;
this->RemoveElementCrystal(m_partner.partner_crytal[_seq].partner_id);

int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_partner_crytals_done, m_partner.partner_crytal[_seq].partner_id,
m_partner.partner_quality[_seq].new_quality);
if (length > 0)
{
Protocol::SCSystemMsg sm;
int sendlen = 0;
if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
{
EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*) &sm, sendlen);
}
}

return;
} else
{
if (m_partner.partner_crytal[_seq].current_crytal_id != 0)
{
m_partner.partner_crytal[_seq].use_time -= 1;
}
}
this->SendElementCrystalInfo();

}*/

void Partner::ElementCrystalCountSubByPartnerId(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;
	if (m_new_partner.partner_base_info[partner_id].partner_crytal.current_crytal_id == 0) return;

	if (m_new_partner.partner_base_info[partner_id].partner_crytal.use_time - 1 == 0 && m_new_partner.partner_base_info[partner_id].partner_crytal.current_crytal_id != 0)
	{
		m_new_partner.partner_base_info[partner_id].partner_crytal.use_time -= 1;
		this->RemoveElementCrystal(partner_id);

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_partner_crytals_done, partner_id,
			m_new_partner.partner_base_info[partner_id].new_quality);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
			}
		}

		return;
	}
	else
	{
		if (m_new_partner.partner_base_info[partner_id].partner_crytal.current_crytal_id != 0)
		{
			m_new_partner.partner_base_info[partner_id].partner_crytal.use_time -= 1;
		}
	}
	this->SendElementCrystalInfo();


}

bool Partner::ActiveSkin(int partner_id, short skin_id)
{
	if (this->IsPartnerInvalid(partner_id)) return false;

	const PartnerSkinCfgInfo *skin_conf = LOGIC_CONFIG->GetSkinConfig()->GetPartnerSkinCfgInfoBySkinId(skin_id);
	if (skin_conf == NULL)return false;

	// 查看该伙伴是否有皮肤
	if (partner_id != skin_conf->partner_id) return false;
	//查看背包是否有激活道具
	if (!m_role_module_mgr->GetKnapsack()->HasItem(skin_conf->active_stuff_id, 1)) return false;

	if (this->IsActive(partner_id, skin_conf->skin_seq))// 激活过
	{
		ItemConfigData item;
		item.item_id = skin_conf->decompose_item_id;
		item.is_bind = true;
		item.num = skin_conf->decompose_item_num;
		
		bool is_true = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, (short)item.num))
		{
			if (m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_DECOMPOSE_SURFACE))
			{
				is_true = true;
			}
		}

		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_PARTNER_SKIN_CONVERT);
		if (!is_true)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&item, 1, SNED_MAIL_TYPE_DEFAULT, true);
		}
		return true;
	}

	m_role_module_mgr->GetKnapsack()->ConsumeItem(skin_conf->active_stuff_id, 1, __FUNCTION__);

	m_new_partner.partner_base_info[partner_id].skin_active_status |= (MOVE_BYTE_BASE << skin_conf->skin_seq);

	this->SendSingleInfo(partner_id, skin_id, Protocol::SCParnerSkinSingleOper::SKIN_AVTIVE);
	this->SyncPartnerDataPlatformBattle();
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::partner_skin_notice_2, partner_id, m_new_partner.partner_base_info[partner_id].new_quality, skin_id);
	if (length > 0)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
		{
			EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
		}
	}

	ROLE_LOG_QUICK6(LOG_TYPE_PARTNER_SKIN_ACTIVE, m_role_module_mgr->GetRole(), partner_id, skin_id, __FUNCTION__, "");

	this->PutOnSkin(partner_id, skin_id);
	return true;
}

bool Partner::AutoActiveSkin(ItemID active_id, short num)
{
	const PartnerSkinCfgInfo *skin_conf = LOGIC_CONFIG->GetSkinConfig()->GetPartnerSkinCfgInfoByActiveId(active_id);
	if (skin_conf == NULL || num <= 0)return false;

	if (!this->IsActive(skin_conf->partner_id, skin_conf->skin_seq))
	{
		m_new_partner.partner_base_info[skin_conf->partner_id].skin_active_status |= (MOVE_BYTE_BASE << skin_conf->skin_seq);

		this->SendSingleInfo(skin_conf->partner_id, skin_conf->skin_id, Protocol::SCParnerSkinSingleOper::SKIN_AVTIVE);

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::partner_skin_notice_2, skin_conf->partner_id, m_new_partner.partner_base_info[skin_conf->partner_id].new_quality, skin_conf->skin_id);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
			}
		}

		ROLE_LOG_QUICK6(LOG_TYPE_PARTNER_SKIN_ACTIVE, m_role_module_mgr->GetRole(), skin_conf->partner_id, skin_conf->skin_id, __FUNCTION__, "");

		this->PutOnSkin(skin_conf->partner_id, skin_conf->skin_id);
		num--;
		this->SyncPartnerDataPlatformBattle();
	}
	if (num > 0)
	{
		ItemConfigData item;
		item.item_id = skin_conf->decompose_item_id;
		item.is_bind = true;
		item.num = skin_conf->decompose_item_num * num;

		bool is_true = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, (short)item.num))
		{
			if (m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_DECOMPOSE_SURFACE))
			{
				is_true = true;
			}
		}

		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_PARTNER_SKIN_CONVERT);
		if (!is_true)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&item, 1, SNED_MAIL_TYPE_DEFAULT, true);
		}
	}

	return true;

}

void Partner::AttrChangeBySkin(AttrCon & p_att, const PartnerSkinCfgInfo * skin_att, char add_or_sub)
{
	p_att.attr_list[BATTLE_ATTR_MAX_HP] += (add_or_sub == 1 ? skin_att->maxhp : -skin_att->maxhp); 
	p_att.attr_list[BATTLE_ATTR_MAX_MP] += (add_or_sub == 1 ? skin_att->maxmp : -skin_att->maxmp); 
	p_att.attr_list[BATTLE_ATTR_ATTACK] += (add_or_sub == 1 ? skin_att->gongji : -skin_att->gongji); 
	p_att.attr_list[BATTLE_ATTR_DEFENSE] += (add_or_sub == 1 ? skin_att->fangyu : -skin_att->fangyu); 
	p_att.attr_list[BATTLE_ATTR_AGILITY] += (add_or_sub == 1 ? skin_att->minjie : -skin_att->minjie); 
	p_att.attr_list[BATTLE_ATTR_MENTAL] += (add_or_sub == 1 ? skin_att->jingshen : -skin_att->jingshen); 
	p_att.attr_list[BATTLE_ATTR_RECOVERY] += (add_or_sub == 1 ? skin_att->huifu : -skin_att->huifu);
	p_att.attr_list[BATTLE_ATTR_CRITICAL] += (add_or_sub == 1 ? skin_att->bisha : -skin_att->bisha); 
	p_att.attr_list[BATTLE_ATTR_DODGE] += (add_or_sub == 1 ? skin_att->shanduo : -skin_att->shanduo); 
	p_att.attr_list[BATTLE_ATTR_HIT] += (add_or_sub == 1 ? skin_att->mingzhong : -skin_att->mingzhong); 
	p_att.attr_list[BATTLE_ATTR_COUNTER_ATTACK] += (add_or_sub == 1 ? skin_att->fanji : -skin_att->fanji); 
	p_att.attr_list[BATTLE_ATTR_MAGIC_DEFENSE] += (add_or_sub == 1 ? skin_att->mokang : -skin_att->mokang);
	p_att.attr_list[BATTLE_ATTR_MAGIC_ATTACK] += (add_or_sub == 1 ? skin_att->mogong : -skin_att->mogong);
	p_att.attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] += (add_or_sub == 1 ? skin_att->jianren : -skin_att->jianren); 
	p_att.attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] += (add_or_sub == 1 ? skin_att->bisha_dmg : -skin_att->bisha_dmg); 
	p_att.attr_list[BATTLE_ATTR_ANTI_POISON] += (add_or_sub == 1 ? skin_att->def_zhongdu : -skin_att->def_zhongdu); 
	p_att.attr_list[BATTLE_ATTR_ANTI_PETRIFY] += (add_or_sub == 1 ? skin_att->def_shihua : -skin_att->def_shihua); 
	p_att.attr_list[BATTLE_ATTR_ANTI_FREEZE] += (add_or_sub == 1 ? skin_att->def_hunshui : -skin_att->def_hunshui);
	p_att.attr_list[BATTLE_ATTR_ANTI_DRUNK] += (add_or_sub == 1 ? skin_att->def_jiuzui : -skin_att->def_jiuzui); 
	p_att.attr_list[BATTLE_ATTR_ANTI_CHAOS] += (add_or_sub == 1 ? skin_att->def_hunluan : -skin_att->def_hunluan); 

	p_att.attr_list[BATTLE_ATTR_CUR_HP] = p_att.attr_list[BATTLE_ATTR_MAX_HP];
	p_att.attr_list[BATTLE_ATTR_CUR_MP] = p_att.attr_list[BATTLE_ATTR_MAX_MP];
}

void Partner::AttrChangeBySkin(AttributeList & p_att, const PartnerSkinCfgInfo * skin_att)
{
	p_att.m_attrs[BATTLE_ATTR_MAX_HP] += skin_att->maxhp;
	p_att.m_attrs[BATTLE_ATTR_MAX_MP] += skin_att->maxmp;
	p_att.m_attrs[BATTLE_ATTR_ATTACK] += skin_att->gongji;
	p_att.m_attrs[BATTLE_ATTR_DEFENSE] += skin_att->fangyu;
	p_att.m_attrs[BATTLE_ATTR_AGILITY] += skin_att->minjie;
	p_att.m_attrs[BATTLE_ATTR_MENTAL] += skin_att->jingshen;
	p_att.m_attrs[BATTLE_ATTR_RECOVERY] += skin_att->huifu;
	p_att.m_attrs[BATTLE_ATTR_CRITICAL] += skin_att->bisha;
	p_att.m_attrs[BATTLE_ATTR_DODGE] += skin_att->shanduo;
	p_att.m_attrs[BATTLE_ATTR_HIT] += skin_att->mingzhong;
	p_att.m_attrs[BATTLE_ATTR_COUNTER_ATTACK] += skin_att->fanji;
	p_att.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE] += skin_att->mokang;
	p_att.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += skin_att->mogong;
	p_att.m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE] += skin_att->jianren;
	p_att.m_attrs[BATTLE_ATTR_CRITICAL_INC_VALUE] += skin_att->bisha_dmg;
	p_att.m_attrs[BATTLE_ATTR_ANTI_POISON] += skin_att->def_zhongdu;
	p_att.m_attrs[BATTLE_ATTR_ANTI_PETRIFY] += skin_att->def_shihua;
	p_att.m_attrs[BATTLE_ATTR_ANTI_FREEZE] += skin_att->def_hunshui;
	p_att.m_attrs[BATTLE_ATTR_ANTI_DRUNK] +=skin_att->def_jiuzui;
	p_att.m_attrs[BATTLE_ATTR_ANTI_CHAOS] += skin_att->def_hunluan;
}

void Partner::PutOnSkin(int partner_id,int skin_id, bool is_from_init /* = false */)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const PartnerSkinCfgInfo *skin_conf = LOGIC_CONFIG->GetSkinConfig()->GetPartnerSkinCfgInfoBySkinId(skin_id);
	if (skin_conf == NULL)return;

	if (this->IsPartnerAlreadyActive(partner_id))
	{
		// 该伙伴此编号皮肤是否激活或正在穿戴
		if (!this->IsActive(partner_id, skin_conf->skin_seq) || this->IsPutOn(partner_id, skin_id))return;
	}

	if (partner_id == skin_conf->partner_id)
	{
		this->PutOffSkin(partner_id, skin_id, is_from_init);

		m_new_partner.partner_base_info[partner_id].cur_skin_id = skin_conf->skin_id;

		if (this->IsPartnerAlreadyActive(skin_conf->partner_id))
		{
			this->ReCalcPartnerAttr(skin_conf->partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_SKIN, ADD_ATTR);
		} else
		{
			this->ReCalcPartnerAttr(skin_conf->partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_SKIN_NO_AC, ADD_ATTR);
		}

		this->SendSingleInfo(partner_id, skin_id, Protocol::SCParnerSkinSingleOper::SKIN_PUT_ON);

		PartnerSql & p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
		NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];
		//PartnerCon & p_con = m_partner.partner_base_info[partner_id].partner_list;

		if (!is_from_init)
		{
			this->SyncPartnerInfoToTeam(partner_id, p_sql.cur_hp, p_sql.max_hp, p_sql.cur_mp, p_sql.max_mp, this->GetCurPutOnSkin(partner_id), pbi.level);
		}
	}
}

void Partner::PutOffSkin(int partner_id, int skin_id, bool is_from_init /* = false */)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const PartnerSkinCfgInfo *skin_conf = LOGIC_CONFIG->GetSkinConfig()->GetPartnerSkinCfgInfoBySkinId(skin_id);
	if (skin_conf == NULL)return;

	// 该伙伴此编号皮肤是否激活或未穿戴
	if (!this->IsActive(partner_id, skin_conf->skin_seq) || !this->IsPutOn(partner_id, skin_id))return;

	if (partner_id == skin_conf->partner_id)
	{
		m_new_partner.partner_base_info[partner_id].cur_skin_id = 0;
		this->SendSingleInfo(partner_id, skin_id, Protocol::SCParnerSkinSingleOper::SKIN_PUT_OFF);
		PartnerSql & p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
		NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];
		//PartnerCon & p_con = m_partner.partner_base_info[partner_id].partner_list;

		if (!is_from_init)
		{
			this->SyncPartnerInfoToTeam(partner_id, p_sql.cur_hp, p_sql.max_hp, p_sql.cur_mp, p_sql.max_mp, this->GetCurPutOnSkin(partner_id), pbi.level);
		}	
	}
}

int Partner::GetSkinCount(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return 0;

	int count = 0;
	for (int step = 0; step < PARTNER_SKIN_ONCE; ++step)
	{
		if (this->IsActive(partner_id, step))
		{
			count += 1;
		}
	}

	return count;
}

bool Partner::IsActive(int partner_id, int skin_seq)
{
	if (m_new_partner.partner_base_info[partner_id].skin_active_status & (MOVE_BYTE_BASE << skin_seq))
	{
		return true;
	}
	return false;
}

void Partner::GetActiveSkinId(int partner_id, int ARG_OUT skin_list[PARTNER_SKIN_ONCE])
{
	//skin_list 在外部初始化

	int index = 0;
	for (int step = 0; step < PARTNER_SKIN_ONCE; ++step)
	{
		if (this->IsActive(partner_id, step))
		{
			skin_list[index] = step;
			index += 1;
		}
	}
}

int Partner::GetCurPutOnSkin(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return 0;
	return m_new_partner.partner_base_info[partner_id].cur_skin_id;
}

bool Partner::IsPutOn(int partner_id, int skin_id)
{
	if (m_new_partner.partner_base_info[partner_id].cur_skin_id == skin_id)
	{
		return true;
	}
	return false;
}

void Partner::SendSingleInfo(int partner_id, int skin_id, int reson)
{
	Protocol::SCParnerSkinSingleOper info;

	info.send_type = reson;
	info.partner_id = partner_id;
	info.skin_id = skin_id;

	m_role_module_mgr->NetSend((const void *)&info, sizeof(info));
}

void Partner::SendAllSkinInfo()
{
	Protocol::SCParnerSkinActive info;
	info.count = 0;
	for (int i = PARTNER_ID_START; i <= m_partner_count && i < PARTNER_NUM_MAX; ++i)
	{
		if (this->IsPartnerInvalid(i)) continue;

		info.skin_list[info.count].partner_id = m_new_partner.partner_base_info[i].partner_list.partner_id;
		info.skin_list[info.count].skin_active_status = m_new_partner.partner_base_info[i].skin_active_status;
		info.count++;
	}

	int send_len = sizeof(info) - (PARTNER_NUM_MAX - info.count) * sizeof(info.skin_list[0]);
	m_role_module_mgr->NetSend((const char *)&info, send_len);
}


int Partner::GetParterCapability(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return 0;

	return m_partner_attr_list[partner_id].capability;

}

void Partner::GetPartnerList(std::vector<int>* role_partner_vec)
{
	if (role_partner_vec == NULL)return;

	for (int idx = PARTNER_ID_START; idx < PARTNER_NUM_MAX; ++idx)
	{
		if (this->IsPartnerInvalid(idx)) continue;
		if (LOCK_STATE_UNLOCK == m_new_partner.partner_base_info[idx].partner_list.is_unlocked && m_new_partner.partner_base_info[idx].GetPartnerId() != 0)
		{
			role_partner_vec->push_back(m_new_partner.partner_base_info[idx].GetPartnerId());
		}
	}

}

void Partner::GetUnlockedPartnerSet(std::set<int>* unlocked_set)
{
	if (unlocked_set == NULL)return;

	for (int idx = PARTNER_ID_START; idx < PARTNER_NUM_MAX; ++idx)
	{
		if (this->IsPartnerInvalid(idx)) continue;
		if (LOCK_STATE_UNLOCK == m_new_partner.partner_base_info[idx].partner_list.is_unlocked && m_new_partner.partner_base_info[idx].GetPartnerId() != 0)
		{
			unlocked_set->insert(m_new_partner.partner_base_info[idx].GetPartnerId());
		}
	}

}

void Partner::ActivePartnerByConsumingItem(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;
	NewPartnerBaseInfo & partner_info = m_new_partner.partner_base_info[partner_id];

	NewPartnerCon & p_param = partner_info.partner_list;

	if (LOCK_STATE_UNLOCK == p_param.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_UNLOCK);
		return;
	}

	const PartnerBasicCfg * cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (NULL == cfg) return;

	// 消耗
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->unlock_item, cfg->need_num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	// 生效
	p_param.is_unlocked = LOCK_STATE_UNLOCK;
	partner_info.level = cfg->activation_level;
	this->SendPartnerFightState(partner_id, true);
	this->MaxPartnerHPOrMP(partner_id, MAX_TYPE_BOTH_TWO);
	EventHandler::Instance().OnActivePartner(m_role_module_mgr->GetRole(), partner_id);
	ROLE_LOG_QUICK10(LOG_TYPE_PARTNER_CHANGE_ADD, m_role_module_mgr->GetRole(), partner_id, 0, __FUNCTION__, NULL, 0, 0, 0, 0, 0);

	int cur_fight_num = this->GetCurFightPartnerNum();
	if (cur_fight_num < LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg().synfight_numb_max)
	{
		this->ChangePartnerFightState(partner_id, __FUNCTION__);
	}

	// 装备水晶id
	this->UseElementCrystalById(partner_id, cfg->crystal_item);

	// 品质
	int _temp_quality_type;
	switch (partner_info.old_quality)
	{
	case FETTER_BLUE:
		{
			_temp_quality_type = QUALITY_TYPE_BLUE_TO_PURPLE;
			this->QualityUpgrade(partner_info.GetPartnerId(), _temp_quality_type, false);
		}
		break;
	case FETTER_PURPLE:
		_temp_quality_type = QUALITY_TYPE_PURPLE_TO_ORANGE;
		this->QualityUpgrade(partner_info.GetPartnerId(), _temp_quality_type, false);
		break;

	case FETTER_ORANGE:
		//for (int type = QUALITY_TYPE_MIN; type <= QUALITY_TYPE_PURPLE_TO_ORANGE; ++type)
		{
			_temp_quality_type = QUALITY_TYPE_ORANGE_TO_RED;
			this->QualityUpgrade(partner_info.GetPartnerId(), _temp_quality_type, false);
		}
		break;

	case PETTER_RED:
		//for (int type = QUALITY_TYPE_MIN; type <= QUALITY_TYPE_ORANGE_TO_RED; ++type)
		{
			_temp_quality_type = QUALITY_TYPE_RED_TO_GOLD;
			this->QualityUpgrade(partner_info.GetPartnerId(), _temp_quality_type, false);
		}
		break;
	}
	this->OnPartnerActive(partner_id);
	this->SendPartnerSingleInfo(partner_id);
}

bool Partner::IsInPlatformBattle(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return false;

	return m_platform_battle_partner[partner_id];
}

bool Partner::CanJoinPlatformBattle(int partner_id)
{
	return true;		//出战中也能守擂
}

void Partner::ClearPartnerPlatformBattleState()
{
	for (int i = 0; i < ARRAY_LENGTH(m_platform_battle_partner); ++i)
	{
		m_platform_battle_partner[i] = 0;
	}
}

void Partner::SetPartnerPlatformBattleState(int partner_id, bool is_platform_battle)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	if (is_platform_battle)
	{
		m_platform_battle_partner[partner_id] = true;
	}
	else
	{
		m_platform_battle_partner[partner_id] = false;
	}
}

void Partner::SyncPartnerDataPlatformBattle()
{
	PlatformBattle::Instance().SyncPartnerData(m_role_module_mgr->GetRole());
}

void Partner::OnPartnerIntensify(int partner_id, Protocol::CSPartnerIntensifyReq * req)
{
	if (NULL == req) return;
	if (this->IsPartnerInvalid(partner_id)) return;
	if (!this->IsPartnerAlreadyActive(partner_id)) return;
	NewPartnerBaseInfo& pbi = m_new_partner.partner_base_info[partner_id];

	const PartnerIntensifyCfg *pi_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerIntensifyCfg(partner_id, pbi.intensify_level);
	if (NULL == pi_cfg) return;

	if (pi_cfg->expend_item_1 == 0) return;			// 强化满级 不能再强化了

	if (pi_cfg->levels > 0 && pbi.level < pi_cfg->levels)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_LEVEL_TOO_LOW);
		return;
	}
	if (pi_cfg->feisheng_num > 0 && pbi.soar_count < pi_cfg->feisheng_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_FEI_SHENG_TIMES_NOT_ENOUGH);
		return;
	}
	//检查金币消耗
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(pi_cfg->use_coin))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}

	//检查本体元神消耗
	{
		if (req->expend_item_1.use_item_id != pi_cfg->expend_item_1 || req->expend_item_1.use_num != pi_cfg->expend_num_1)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		int save_index = LOGIC_CONFIG->GetPartnerConfig()->GetPactSaveIndex(req->expend_item_1.use_item_id);
		if (0 > save_index || PARTNER_SPIRIT_PACT_MAX_GRID <= save_index)
			return;

		PartnerSpiritPactParam::PactGridInfo &grid_info = m_pact_grid_param.grid_list[save_index];
		if (req->expend_item_1.use_num > grid_info.item_num)	// 仓库数量不足
		{
			m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}
	}
	//检查通用元神消耗
	if (pi_cfg->expend_num_2 > 0)
	{
		if (req->expend_item_2_count <= 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		int use_coumt = 0;		// 通用元神消耗统计
		for (int i = 0; i < req->expend_item_2_count && i < ARRAY_LENGTH(req->expend_item_2_list); i++)
		{
			int save_index = LOGIC_CONFIG->GetPartnerConfig()->GetPactSaveIndex(req->expend_item_2_list[i].use_item_id);
			if (0 > save_index || PARTNER_SPIRIT_PACT_MAX_GRID <= save_index)
				return;

			PartnerSpiritPactParam::PactGridInfo &grid_info = m_pact_grid_param.grid_list[save_index];
			if (req->expend_item_2_list[i].use_num > grid_info.item_num)	// 仓库数量不足
			{
				m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}

			use_coumt += req->expend_item_2_list[i].use_num;
		}

		if (use_coumt != pi_cfg->expend_num_2)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		}
	}

	//检查指定元神消耗
	if (pi_cfg->expend_item_3 != 0)
	{
		if (req->expend_item_3.use_item_id != pi_cfg->expend_item_3 || req->expend_item_3.use_num != pi_cfg->expend_num_3)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		int save_index = LOGIC_CONFIG->GetPartnerConfig()->GetPactSaveIndex(req->expend_item_3.use_item_id);
		if (0 > save_index || PARTNER_SPIRIT_PACT_MAX_GRID <= save_index)
			return;

		PartnerSpiritPactParam::PactGridInfo &grid_info = m_pact_grid_param.grid_list[save_index];
		if (req->expend_item_3.use_num > grid_info.item_num)	// 仓库数量不足
		{
			m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}
	}

	//检查通过，消耗元神
	m_role_module_mgr->GetMoney()->UseCoinBind(pi_cfg->use_coin, __FUNCTION__);
	this->PactGridTakeOut(req->expend_item_1.use_item_id, req->expend_item_1.use_num, PUT_REASON_PARTNER_INTENSIFY);

	std::string expend_item_2_str;
	if (pi_cfg->expend_num_2 > 0)
	{
		for (int i = 0; i < req->expend_item_2_count && i < ARRAY_LENGTH(req->expend_item_2_list); i++)
		{
			this->PactGridTakeOut(req->expend_item_2_list[i].use_item_id, req->expend_item_2_list[i].use_num, PUT_REASON_PARTNER_INTENSIFY);

			expend_item_2_str = STRING_SPRINTF("{use_item_%d[%d:%d]}", i, req->expend_item_2_list[i].use_item_id, req->expend_item_2_list[i].use_num);
		}
	}
	if (pi_cfg->expend_item_3 != 0)
	{
		this->PactGridTakeOut(req->expend_item_3.use_item_id, req->expend_item_3.use_num, PUT_REASON_PARTNER_INTENSIFY);
	}

	pbi.intensify_level += 1;

	this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_INTENSIFY);
	{
		Protocol::SCPartnerIntensifyInfo info;
		info.partner_id = partner_id;
		info.intensify_level = pbi.intensify_level;

		m_role_module_mgr->NetSend(&info, sizeof(info));
	}

	if (pi_cfg->is_broadcast)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_partner_strengthen_broadcast,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), partner_id, pbi.new_quality, pbi.intensify_level + 1);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT)))
			{
				World::GetInstWorld()->SendToAllGateway((const char *)&sm, sendlen);		// 通知所有玩家
			}
		}
	}
	EventHandler::Instance().OnPartnerIntensify(m_role_module_mgr->GetRole(), partner_id, pbi.intensify_level, pbi.new_quality);
	gamelog::g_log_partner.printf(LL_INFO, "%s user[%d, %s] partner_id:%d new_intensify_level:%d expend_item_2_list:%s", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		partner_id, pbi.intensify_level, expend_item_2_str.c_str());
}

void Partner::GmPartnerFlyUp(int partner_id, int fly_up_level, bool modify_role_flyup)
{
	if (this->IsPartnerInvalid(partner_id)) return;
	if (fly_up_level <= FlyUpSystemParam::FLY_UP_STAG_NO || fly_up_level >= FlyUpSystemParam::FLY_UP_STAG_MAX) return;

	NewPartnerBaseInfo & pbi = m_new_partner.partner_base_info[partner_id];
	if (fly_up_level <= pbi.soar_count) return;

	if(modify_role_flyup) m_role_module_mgr->GetFlyUpSystem()->GmFlyUp(fly_up_level);

	pbi.soar_count = fly_up_level;
	if(pbi.soar_count > FlyUpSystemParam::FLY_UP_STAG_NO) pbi.soar_arms_level = 1;
	else pbi.soar_arms_level = 0;

	this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_SOAR);
	
	this->SendPartnerSoarNoitce(Protocol::SCPartnerSoarNoitce::NOTICE_TYPE_SOAR, partner_id);
	gamelog::g_log_partner.printf(LL_INFO, "%s user[%d, %s] partner_id:%d soar_count:%d soar_arms_level:%d", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		partner_id, pbi.soar_count, pbi.soar_arms_level);
}

void Partner::OnPartnerActive(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerBaseInfo & base_info = m_new_partner.partner_base_info[partner_id];
	const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
		partner_id, LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(base_info.new_quality), base_info.quality_segment));
	if (NULL == q_cfg) return;
	
	for (int idx = 0; idx < PARTNER_COMMON_SKILL_MAX; ++idx)
	{
		if (q_cfg->common_skill_id_list[idx] > 0 && q_cfg->common_skill_lv_list[idx] > 0)
		{
			base_info.partner_list.skill_list[idx] = 1;	// 启用技能
		}
	}

	this->SendPartnerSkillsInfo(partner_id);

}

void Partner::OnPartnerUpgrade(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	/*const PartnerLevelSkillCfg* cfg = LOGIC_CONFIG->GetPartnerConfig()->GetLevelSkillCfg(partner_id, m_partner.partner_base_info[partner_id].partner_list.upgrade_lv);
	if (NULL == cfg) return;

	const PartnerLevelSkillCfg* last_level_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetLevelSkillCfg(partner_id, m_partner.partner_base_info[partner_id].partner_list.upgrade_lv - 1);
	if (NULL == last_level_cfg) return;

	PartnerCon & param = m_partner.partner_base_info[partner_id].partner_list;
	for (int idx = 0; idx < PARTNER_COMMON_SKILL_MAX; ++idx)
	{
		if (cfg->skill_id_list[idx] > 0 && cfg->skill_level_list[idx] > 0
			&& last_level_cfg->skill_level_list[idx] == 0)
		{
			param.skill_list[idx] = 1;	// 启用技能
		}
	}
	*/
	this->SendPartnerSkillsInfo(partner_id);

}

void Partner::OnPartnerQualityUp(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	int old_quality_segment = m_new_partner.partner_base_info[partner_id].quality_segment;
	int old_quality = m_new_partner.partner_base_info[partner_id].new_quality;
	if (m_new_partner.partner_base_info[partner_id].quality_segment >= PARTNER_QUALITY_SEGMENT_MAX_NUM)
	{
		this->AddQualityByQualityUp(partner_id);
	}
	else
	{	
		++m_new_partner.partner_base_info[partner_id].quality_segment;
	}
	const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
		partner_id, LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(m_new_partner.partner_base_info[partner_id].new_quality), m_new_partner.partner_base_info[partner_id].quality_segment));
	if (!q_cfg) return;

	const QualityUpgradeCfg *last_q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
		partner_id, LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(old_quality), old_quality_segment));
	if (!last_q_cfg) return;

	NewPartnerCon & param = m_new_partner.partner_base_info[partner_id].partner_list;
	for (int idx = 0; idx < PARTNER_COMMON_SKILL_MAX; ++idx)
	{
		if (q_cfg->common_skill_id_list[idx] > 0 && q_cfg->common_skill_lv_list[idx] > 0
			&& last_q_cfg->common_skill_lv_list[idx] == 0)
		{
			param.skill_list[idx] = 1;	// 启用技能
		}
	}
	this->SendPartnerSkillsInfo(partner_id);
}

bool Partner::ActiveAppointedPartner(int param, int unlock_type, int *activity_partner_id, bool is_notice)	// 通过使用道具和完成任务触发
{
	if (unlock_type < UNLOCK_TYPE_TASK || unlock_type > UNLOCK_TYPE_ITEM_NOREX) return false;

	// 检测激活是否合法以及预计是否会成功

	int partner_id = LOGIC_CONFIG->GetPartnerConfig()->PartnerActiveCheck(unlock_type, param);
	NewPartnerBaseInfo & partnert_info = m_new_partner.partner_base_info[partner_id];

	const PartnerBasicCfg * cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partnert_info.GetPartnerId());
	if (NULL == cfg) return false; 

	if (this->IsPartnerInvalid(partner_id)) return false;

	NewPartnerCon & p_param = partnert_info.partner_list;
	if (LOCK_STATE_UNLOCK == p_param.is_unlocked)
	{
		if (is_notice)m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_UNLOCK);
		return false;
	}
	// 检测通过，激活
	if (activity_partner_id != NULL)*activity_partner_id = partner_id;

	p_param.is_unlocked = LOCK_STATE_UNLOCK;
	partnert_info.level = cfg->activation_level;
	this->SendPartnerFightState(partner_id, true);
	this->MaxPartnerHPOrMP(partner_id, MAX_TYPE_BOTH_TWO);

	EventHandler::Instance().OnActivePartner(m_role_module_mgr->GetRole(), partner_id);
	ROLE_LOG_QUICK10(LOG_TYPE_PARTNER_CHANGE_ADD, m_role_module_mgr->GetRole(), partner_id, 0, __FUNCTION__, NULL, unlock_type, param, 0, 0, 0);

	int cur_fight_num = this->GetCurFightPartnerNum();
	if (cur_fight_num < LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg().synfight_numb_max)
	{
		this->ChangePartnerFightState(partner_id, __FUNCTION__);
	}

	// 装备水晶id
	const PartnerBasicCfg *pb_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (!pb_cfg) return false;
	this->UseElementCrystalById(partner_id, pb_cfg->crystal_item);

	// 品质
	int _temp_quality_type;
	switch (partnert_info.old_quality)
	{
		case FETTER_BLUE:
		{
			_temp_quality_type = QUALITY_TYPE_BLUE_TO_PURPLE;
			this->QualityUpgrade(partnert_info.GetPartnerId(), _temp_quality_type, false);
		}
		break;
	case FETTER_PURPLE:
		_temp_quality_type = QUALITY_TYPE_PURPLE_TO_ORANGE;
		this->QualityUpgrade(partnert_info.GetPartnerId(), _temp_quality_type, false);
		break;

	case FETTER_ORANGE:
		//for (int type = QUALITY_TYPE_MIN; type <= QUALITY_TYPE_PURPLE_TO_ORANGE; ++type)
		{
			_temp_quality_type = QUALITY_TYPE_ORANGE_TO_RED;
			this->QualityUpgrade(partnert_info.GetPartnerId(), _temp_quality_type, false);
		}
		break;

	case PETTER_RED:
	//	for (int type = QUALITY_TYPE_MIN; type <= QUALITY_TYPE_ORANGE_TO_RED; ++type)
		{
			_temp_quality_type = QUALITY_TYPE_RED_TO_GOLD;
			this->QualityUpgrade(partnert_info.GetPartnerId(), _temp_quality_type, false);
		}
		break;
	}
	this->OnPartnerActive(partner_id);
	this->SendPartnerSingleInfo(partner_id);
	return true;
}

bool Partner::ActivePartnerByOther(int partner_id, const char * reason)
{
	if (this->IsPartnerInvalid(partner_id)) return false;
	NewPartnerBaseInfo & partner_info = m_new_partner.partner_base_info[partner_id];

	NewPartnerCon & p_param = partner_info.partner_list;

	if (LOCK_STATE_UNLOCK == p_param.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_UNLOCK);
		return false;
	}

	const PartnerBasicCfg * cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (NULL == cfg) return false;

	// 生效
	p_param.is_unlocked = LOCK_STATE_UNLOCK;
	partner_info.level = cfg->activation_level;
	this->SendPartnerFightState(partner_id, true);
	this->MaxPartnerHPOrMP(partner_id, MAX_TYPE_BOTH_TWO);
	EventHandler::Instance().OnActivePartner(m_role_module_mgr->GetRole(), partner_id);
	ROLE_LOG_QUICK10(LOG_TYPE_PARTNER_CHANGE_ADD, m_role_module_mgr->GetRole(), partner_id, 0, __FUNCTION__, reason, 0, 0, 0, 0, 0);

	int cur_fight_num = this->GetCurFightPartnerNum();
	if (cur_fight_num < LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg().synfight_numb_max)
	{
		this->ChangePartnerFightState(partner_id, __FUNCTION__);
	}

	// 装备水晶id
	this->UseElementCrystalById(partner_id, cfg->crystal_item);

	// 品质
	int _temp_quality_type;
	switch (partner_info.old_quality)
	{
		case FETTER_BLUE:
		{
			_temp_quality_type = QUALITY_TYPE_BLUE_TO_PURPLE;
			this->QualityUpgrade(partner_info.GetPartnerId(), _temp_quality_type, false);
		}
		break;
		case FETTER_PURPLE:
		_temp_quality_type = QUALITY_TYPE_PURPLE_TO_ORANGE;
		this->QualityUpgrade(partner_info.GetPartnerId(), _temp_quality_type, false);
		break;

		case FETTER_ORANGE:
		//for (int type = QUALITY_TYPE_MIN; type <= QUALITY_TYPE_PURPLE_TO_ORANGE; ++type)
		{
			_temp_quality_type = QUALITY_TYPE_ORANGE_TO_RED;
			this->QualityUpgrade(partner_info.GetPartnerId(), _temp_quality_type, false);
		}
		break;

		case PETTER_RED:
		//for (int type = QUALITY_TYPE_MIN; type <= QUALITY_TYPE_ORANGE_TO_RED; ++type)
		{
			_temp_quality_type = QUALITY_TYPE_RED_TO_GOLD;
			this->QualityUpgrade(partner_info.GetPartnerId(), _temp_quality_type, false);
		}
		break;
	}
	this->OnPartnerActive(partner_id);
	this->SendPartnerSingleInfo(partner_id);

	return true;
}

void Partner::SendAllPartnerInfo()
{
	static Protocol::SCPartnerAllDataInform padi;
	padi.partner_count = 0;
	for (int t_cur = PARTNER_ID_START; t_cur < PARTNER_NUM_MAX; ++t_cur)
	{
		if (this->IsPartnerInvalid(t_cur)) continue;

		// 该伙伴是否有激活皮肤
		int skin_count = this->GetSkinCount(t_cur);
		if (m_new_partner.partner_base_info[t_cur].partner_list.is_unlocked == LOCK_STATE_LOCKED && skin_count <= 0) continue;

		AttrCon & attr_con = m_partner_attr_list[t_cur];
		const NewPartnerBaseInfo &partner_info = m_new_partner.partner_base_info[t_cur];
		PartnerPro & pro_con = padi.partner[padi.partner_count];
		const NewPartnerCon & p_pc = partner_info.partner_list;

		pro_con.base_info = p_pc;

		const PartnerUpGradeCfg * pucg = LOGIC_CONFIG->GetPartnerConfig()->GetCurGradeAttrCfg(p_pc.partner_id, p_pc.upgrade_lv);
		if (NULL == pucg) continue;

		pro_con.quality_segment = partner_info.quality_segment;
		pro_con.quality = partner_info.new_quality;
		pro_con.intensify_level = partner_info.intensify_level;
		pro_con.task_finish = 1;

		for (int t_idx = BATTLE_ATTR_MIN; t_idx < BATTLE_ATTR_MAX; ++t_idx)
		{
			if (BATTLE_ATTR_CUR_HP == t_idx || BATTLE_ATTR_CUR_MP == t_idx) continue;

			pro_con.attr_pair[t_idx] = attr_con.attr_list[t_idx];
		}

		// 当前血量和当前魔力特殊处理
		const PartnerSql & p_sd = partner_info.partner_data;

		pro_con.attr_pair[BATTLE_ATTR_CUR_HP] = p_sd.cur_hp;
		pro_con.attr_pair[BATTLE_ATTR_CUR_MP] = p_sd.cur_mp;

		pro_con.capability = attr_con.capability;

		padi.partner_count += 1;

		pro_con.crytal_id = partner_info.partner_crytal.current_crytal_id;
		pro_con.use_time = partner_info.partner_crytal.use_time;

		pro_con.skin_id = partner_info.cur_skin_id;
		pro_con.level = partner_info.level;
		pro_con.biography_open_flag = partner_info.biography_open_flag;
		pro_con.biography_fetch_flag = partner_info.biography_fetch_flag;
		pro_con.soar_count = partner_info.soar_count;
		pro_con.soar_arms_level = partner_info.soar_arms_level;
		pro_con.rank = RankManager::Instance().GetPartnerTopRankShadow()
			->GetPartnerTopRank(m_role_module_mgr->GetUid(), pro_con.base_info.partner_id);

		memcpy(pro_con.put_on_pact_id_list, partner_info.put_on_pact_id_list, sizeof(pro_con.put_on_pact_id_list));

		m_partner_pact_list[t_cur].GetBePactBaseAttr(pro_con.attach_base_attr_list);

	}
	int send_len = sizeof(padi) - (PARTNER_NUM_MAX - padi.partner_count) * sizeof(PartnerPro);
	m_role_module_mgr->NetSend((const char*)&padi, send_len);
	m_already_send_all_attr = true;

	this->SendSynfightInfoList();
}

void Partner::SendSynfightInfoList()
{
	static Protocol::SCSynFightPartnerOrder sfpo;
	sfpo.fight_numb = 0;
	for (int i = 0; i < PARTNER_SYNFIGHT_NUM_MAX; ++i)
	{
		if (m_new_partner.synfight_list[i] > 0)
		{
			sfpo.fight_list[sfpo.fight_numb++] = m_new_partner.synfight_list[i];
		}
	}

	int send_len = sizeof(sfpo) - (PARTNER_SYNFIGHT_NUM_MAX - sfpo.fight_numb) * sizeof(sfpo.fight_list[0]);
	m_role_module_mgr->NetSend((const char*)&sfpo, send_len);
}

void Partner::ChangePartnerFightState(int partner_id, const char * reason)
{
	NewPartnerBaseInfo &partner_info = m_new_partner.partner_base_info[partner_id];
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerCon & aim_p = partner_info.partner_list;
	if (LOCK_STATE_LOCKED == aim_p.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
		return;
	}

	if (BATTLE_STATE_AWAIT == aim_p.fight_state)
	{
		int empty_pos_idx = -1;
		int cur_fight_num = 0;
		for (int pos_idx = 0; pos_idx < PARTNER_SYNFIGHT_NUM_MAX; ++pos_idx)
		{
			if (m_new_partner.synfight_list[pos_idx] > 0)
			{
				cur_fight_num += 1;
				continue;
			}

			if (-1 == empty_pos_idx)
			{
				empty_pos_idx = pos_idx;
			}
		}

		if (-1 == empty_pos_idx || cur_fight_num >= LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg().synfight_numb_max)
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_SYNCFIGHT_LIMITS);
			return;
		}

		aim_p.fight_state = BATTLE_STATE_FIGHT;
		m_new_partner.synfight_list[empty_pos_idx] = partner_id;
	}
	else
	{
		bool has_found = false;
		for (int pos_idx = 0; pos_idx < PARTNER_SYNFIGHT_NUM_MAX; ++pos_idx)
		{
			if (partner_id == m_new_partner.synfight_list[pos_idx])
			{
				partner_info.partner_list.fight_state = BATTLE_STATE_AWAIT;
				m_new_partner.synfight_list[pos_idx] = -1;
				has_found = true;
				break;
			}
		}

		if (!has_found)
		{
			return;
		}
	}

	// 改变伙伴的出战状态时下发一次队伍协议 
	EventHandler::Instance().OnPartnerStateChange(m_role_module_mgr->GetRole(), partner_id, BATTLE_STATE_AWAIT == aim_p.fight_state);
	this->SendPartnerFightState(partner_id);
	this->SendSynfightInfoList();

	RankManager::Instance().SyncPersonRankInfo(m_role_module_mgr->GetRole(), PERSON_RANK_TYPE_CAPABILITY);

	ColosseumManager::Instance().SyncUserData(m_role_module_mgr->GetRole());	

	gamelog::g_log_partner.printf(LL_INFO, "%s partner_state_change partner_id[%d] new_state[%d] reason[%s]", __FUNCTION__, partner_id, aim_p.fight_state, reason);
}

void Partner::PartnerBreakTAndUpGrade(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
		return;
	}

	if (NULL == LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, p_param.upgrade_lv, p_param.attr_stage))
	{
		this->PartnerUPGradeHandler(partner_id);
		this->OnPartnerUpgrade(partner_id);
	}
	else
	{
		this->PartnerAttrBreakThrou(partner_id);
	}

	if (this->IsInPlatformBattle(partner_id))
	{
		this->SyncPartnerDataPlatformBattle();
	}

}

void Partner::PartnerFettersBaptize(int partner_id)
{
	/*
	int p_seq = m_partner.GetIdxByPartnerID(partner_id);
	if (PARTNER_ERROR_SEQ == p_seq) return;

	PartnerCon & p_param = m_partner.partner_list[p_seq];
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
	m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
	return;
	}

	const PartnerOtherCfg & p_other = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg();
	Money * money = m_role_module_mgr->GetMoney();
	if (NULL == money) return;

	if (p_other.fetter_wash_price > 0 && !money->ImmortalCoinMoreThan(p_other.fetter_wash_price))
	{
	m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_MARKET_COIN_BIND_NOT_ENOUGH);
	return;
	}

	const PartnerBasicCfg * p_basic = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(p_param.partner_id);
	if (NULL == p_basic) return;

	if (p_other.fetter_wash_price > 0 && !money->UseCoinBind(p_other.fetter_wash_price, "PartnerFettersWash")) return;

	// 消耗完毕，注意不能中途返回
	p_param.wash_fetter[0] = LOGIC_CONFIG->GetPartnerConfig()->GainARandFetter(partner_id, p_basic->fetter_group);
	do { p_param.wash_fetter[1] = LOGIC_CONFIG->GetPartnerConfig()->GainARandFetter(partner_id, p_basic->fetter_group, p_param.wash_fetter[0]); } while (0 == p_param.wash_fetter[1]);

	this->SendPartnerFetterInfo(p_seq);
	*/
}

void Partner::PartnerFettersResultSave(int partner_id)
{
	/*
	int p_seq = m_partner.GetIdxByPartnerID(partner_id);
	if (PARTNER_ERROR_SEQ == p_seq) return;

	PartnerCon & p_param = m_partner.partner_list[p_seq];
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
	m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
	return;
	}

	for (int t_cur = 0; t_cur < PARTNER_FETTER_COUNT_MAX; ++t_cur)
	{
	p_param.curr_fetter[t_cur] = p_param.wash_fetter[t_cur];
	p_param.wash_fetter[t_cur] = 0;
	}

	this->SendPartnerFetterInfo(p_seq);
	*/
}

void Partner::PartnerSkillSeletorSave(int partner_id, int slot, int skill)
{
	/*
	if (slot < 0 || slot >= PARTNER_COMMON_SKILL_MAX) return;

	int p_seq = m_partner.GetIdxByPartnerID(partner_id);
	if (PARTNER_ERROR_SEQ == p_seq) return;

	PartnerCon & p_param = m_partner.partner_list[p_seq];
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
	m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
	return;
	}

	if (-1 == skill)
	{
	p_param.skill_list[slot] = skill;
	}
	else
	{
	if (0 == LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSkillLv(partner_id, p_param.upgrade_lv, skill))
	{
	m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_SKILLS_IS_LOCKED);
	return;
	}

	p_param.skill_list[slot] = skill;
	}

	//this->ReCalcPartnerAttr(p_seq);
	this->SendPartnerSkillsInfo(p_seq);
	*/
}

void Partner::PartnerSynFightListAlter(int insert_id, int target_id)
{
	if (this->IsPartnerInvalid(insert_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	NewPartnerCon& pc_insert = m_new_partner.partner_base_info[insert_id].partner_list;

	if (this->IsPartnerInvalid(target_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	NewPartnerCon& pc_target = m_new_partner.partner_base_info[target_id].partner_list;

	for (int t_cur = 0; t_cur < PARTNER_SYNFIGHT_NUM_MAX; ++t_cur)
	{
		if (target_id != m_new_partner.synfight_list[t_cur]) continue;

		m_new_partner.synfight_list[t_cur] = insert_id;
		pc_insert.fight_state = BATTLE_STATE_FIGHT;
		this->SendPartnerFightState(insert_id);

		// 查看是否有另外的空位给target_id
		bool has_empty_place_for_target = false;
		int cur_fight_num = this->GetCurFightPartnerNum();
		if (cur_fight_num < LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg().synfight_numb_max)
		{
			for (int k = 0; k < PARTNER_SYNFIGHT_NUM_MAX; ++k)
			{
				if (m_new_partner.synfight_list[k] > 0) continue;

				m_new_partner.synfight_list[k] = target_id;
				has_empty_place_for_target = true;
				break;
			}
		}
		if (!has_empty_place_for_target) // 没有另外的空位，转为待机
		{
			pc_target.fight_state = BATTLE_STATE_AWAIT;
			this->SendPartnerFightState(target_id);
		}

		this->SendSynfightInfoList();
		TeamManager::Instance().ChangePartnerFightState(m_role_module_mgr->GetRole());
		break;
	}
}

void Partner::PartnerSwitchFightPost(int partnerA, int partnerB)
{
	if (partnerB == partnerA) return;

	int* posA = NULL;
	int* posB = NULL;
	for (int t_cur = 0; t_cur < PARTNER_SYNFIGHT_NUM_MAX; ++t_cur)
	{
		if (m_new_partner.synfight_list[t_cur] == partnerA) posA = &m_new_partner.synfight_list[t_cur];
		if (m_new_partner.synfight_list[t_cur] == partnerB) posB = &m_new_partner.synfight_list[t_cur];
	}

	if (NULL != posA && NULL != posB)
	{
		std::swap(*posA, *posB);
		this->SendSynfightInfoList();
	}
}

void Partner::PartnerSkillActivate(int partner_id, short is_activate, short skill_idx)
{
	if (skill_idx < 0 || skill_idx >= PARTNER_COMMON_SKILL_MAX)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (this->IsPartnerInvalid(partner_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	NewPartnerCon & param = m_new_partner.partner_base_info[partner_id].partner_list;
	const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
		partner_id, LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(m_new_partner.partner_base_info[partner_id].new_quality), m_new_partner.partner_base_info[partner_id].quality_segment));
	if (NULL == q_cfg) return;

	if (q_cfg->common_skill_lv_list[skill_idx] > 0)
	{
		param.skill_list[skill_idx] = is_activate ? 1 : 0;
	}
	this->SendPartnerSkillsInfo(partner_id);
}

void Partner::ReCalcPartnerAttr(int partner_id, bool is_notify, char notic_type, char add_or_sub)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const NewPartnerBaseInfo & pbi = m_new_partner.partner_base_info[partner_id];
	NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	AttrCon & attr_con = m_partner_attr_list[partner_id];
	PartnerGiftTree & partner_gift_tree = m_partner_gift_tree_list.partner_gift_tree_list[partner_id];

	int old_partner_capability = attr_con.capability;
	Attribute tmp_attr_list[BATTLE_ATTR_MAX] = { 0 };			// 原本的属性值
	memcpy(tmp_attr_list, attr_con.attr_list, sizeof(tmp_attr_list));
	attr_con.Reset();

	// 元素属性 - 等值  [ 改为装备元素水晶 ]
	const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(p_param.partner_id);
	if (NULL == basic_cfg) return;

	// 先将元素初始化为初始元素
	CLEAR_PARTNER_CRYSTAL(attr_con.attr_list);
	if (this->IsPartnerAlreadyActive(p_param.partner_id))
	{
		PARTNER_CRYSTAL_PUT(basic_cfg->crystal_item, attr_con.attr_list);
	}


	// 基础属性 - 等值 - 必须放最前
	const PartnerLevelCfg * p_attr_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerLevelCfg(p_param.partner_id, pbi.level);
	if (NULL != p_attr_cfg)
	{
		attr_con.attr_list[BATTLE_ATTR_MAX_HP] = p_attr_cfg->attr_list[PARTNER_ATTR_MHP];
		attr_con.attr_list[BATTLE_ATTR_MAX_MP] = p_attr_cfg->attr_list[PARTNER_ATTR_MMP];
		for (int t_cur = PARTNER_ATTR_ATK; t_cur < PARTNER_ATTR_MAX; ++t_cur)
		{
			if (t_cur - PARTNER_ATTR_ATK < 0) continue;
			attr_con.attr_list[t_cur - PARTNER_ATTR_ATK] = p_attr_cfg->attr_list[t_cur];
		}

		for (int t_cur = PARTNER_RESI_TOXIC; t_cur < PARTNER_RESI_MAX; ++t_cur)
		{
			attr_con.attr_list[t_cur + BATTLE_ATTR_ANTI_VALUE_BEGIN] = p_attr_cfg->resi_list[t_cur];
		}

		for (int t_cur = PARTNER_AMEN_CRI; t_cur < PARTNER_AMEN_MAX; ++t_cur)
		{
			attr_con.attr_list[t_cur + BATTLE_ATTR_CORRECTED_VALUE_BEGIN] = p_attr_cfg->amen_list[t_cur];
		}
		attr_con.attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] = p_attr_cfg->amen_list[PARTNER_AMEN_TEN];
	}

	Attribute tmp_base_attr_list[BATTLE_ATTR_MAX] = { 0 };			// 基础属性列表
	memcpy(tmp_base_attr_list, attr_con.attr_list, sizeof(tmp_base_attr_list));

	// 突破属性 - 加值
	for (int tmp_upgrade_lv = 0; tmp_upgrade_lv < p_param.upgrade_lv; ++tmp_upgrade_lv)
	{
		int tmp_break_lv = 0;
		const PartnerBreakThCfg * p_break_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, tmp_upgrade_lv, tmp_break_lv);
		while (NULL != p_break_cfg)
		{
			attr_con.attr_list[p_break_cfg->attr_type] += p_break_cfg->add_value;
			p_break_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, tmp_upgrade_lv, ++tmp_break_lv);
		}
	}

	for (int tmp_break_lv = 0; tmp_break_lv < p_param.attr_stage; ++tmp_break_lv)	// 当前进阶等级的突破属性
	{
		const PartnerBreakThCfg * p_break_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, p_param.upgrade_lv, tmp_break_lv);
		if (NULL != p_break_cfg) attr_con.attr_list[p_break_cfg->attr_type] += p_break_cfg->add_value;
	}

	// 装备属性 - 加值
	bool need_calc_suit = true; int calc_suit_id = 0, min_level = 0, min_color = 0;
	for (int slot_idx = 0; slot_idx < PE_TYPE_MAX; ++slot_idx)
	{
		const NewPartnerEquipment & equipment = p_param.equip_list[slot_idx];
		if (0 == equipment.partner_equip_id)
		{
			need_calc_suit = false;
			continue;
		}

		for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
		{
			if (equipment.m_voc_list[i].attr_value >= 0 && equipment.m_voc_list[i].attr_type < BATTLE_ATTR_MAX) attr_con.attr_list[equipment.m_voc_list[i].attr_type] += equipment.m_voc_list[i].attr_value;
		}

		if (need_calc_suit)
		{
			const PartnerEqSuitInform * suit_cfg = LOGIC_CONFIG->GetPartnerEquipmentConfig()->GetEquipmentSuitID(equipment.partner_equip_id);
			if (NULL == suit_cfg || (0 != calc_suit_id && suit_cfg->suit_id != calc_suit_id))
			{
				need_calc_suit = false;
				continue;
			}

			calc_suit_id = suit_cfg->suit_id;		// 比对，取较低阶的；
			if (0 == min_level || suit_cfg->suit_level < min_level)
			{
				min_level = suit_cfg->suit_level;
				min_color = suit_cfg->suit_color;
			}
			else if (suit_cfg->suit_level == min_level)
			{
				if (0 == min_color || suit_cfg->suit_color <= min_color)
				{
					min_color = suit_cfg->suit_color;
				}
			}
		}
	}

	if (need_calc_suit)
	{
		const PartnerEqSuitEffect * suit_cfg = LOGIC_CONFIG->GetPartnerEquipmentConfig()->GetPartnerEquipmentAttrAdditionCfg(calc_suit_id, min_level, min_color);
		if (NULL != suit_cfg)
		{
			attr_con.attr_list[BATTLE_ATTR_MAX_HP] += suit_cfg->basic_attr[PARTNER_ATTR_MHP];
			attr_con.attr_list[BATTLE_ATTR_MAX_MP] += suit_cfg->basic_attr[PARTNER_ATTR_MMP];
			for (int t_cur = PARTNER_ATTR_ATK; t_cur < PARTNER_ATTR_MAX; ++t_cur)
			{
				if (t_cur - PARTNER_ATTR_ATK < 0) continue;
				attr_con.attr_list[t_cur - PARTNER_ATTR_ATK] += suit_cfg->basic_attr[t_cur];
			}

			for (int t_cur = PARTNER_RESI_TOXIC; t_cur < PARTNER_RESI_MAX; ++t_cur)
			{
				attr_con.attr_list[t_cur + BATTLE_ATTR_ANTI_VALUE_BEGIN] += suit_cfg->resis_attr[t_cur];
			}

			for (int t_cur = PARTNER_AMEN_CRI; t_cur < PARTNER_AMEN_MAX; ++t_cur)
			{
				attr_con.attr_list[t_cur + BATTLE_ATTR_CORRECTED_VALUE_BEGIN] += suit_cfg->amend_attr[t_cur];
			}
			attr_con.attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] += suit_cfg->amend_attr[PARTNER_AMEN_TEN];
		}
	}

	// 人物修炼导致的伙伴属性提升
	std::set<int> partner_train_type = LOGIC_CONFIG->GetTrainConfig()->GetAllPartnerTrainCfgType();
	for (std::set<int>::iterator iter = partner_train_type.begin(); iter != partner_train_type.end(); ++iter)
	{
		int train_type = *iter;
		RoleCommonSavePartnerTrainData * train_data = m_role_module_mgr->GetTrain()->GetRoleCommonSavePartnerTrainData(train_type);
		if (train_data == NULL)
		{
			continue;
		}
		const PartnerTrainCfg * train_cfg = LOGIC_CONFIG->GetTrainConfig()->GetPartnerTrainCfg(train_type, train_data->level);
		if (train_cfg == NULL)
		{
			continue;
		}

		for (std::set<int>::iterator iter2 = train_cfg->attr_type_list.begin(); iter2 != train_cfg->attr_type_list.end(); ++iter2)
		{
			int attr_type = *iter2;
			if (0 <= attr_type && attr_type < BATTLE_ATTR_MAX)
			{
				attr_con.attr_list[attr_type] += train_cfg->attribute_value;
			}
		}
	}

	// 修为
	{
		AttributeList temp_xiu_wei;
		m_role_module_mgr->GetXiuWei()->ReCalcPartnerAttr(temp_xiu_wei, pbi.level);
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			attr_con.attr_list[i] += temp_xiu_wei.m_attrs[i];
		}
	}

	// 星图
	{
		AttributeList temp_star_chart;
		m_role_module_mgr->GetStarChart()->ReCalcPartnerAttr(temp_star_chart);
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			attr_con.attr_list[i] += temp_star_chart.m_attrs[i];
		}
	}

	// 升品增加属性 当循环到当前品质+当前段位时,这个属性不加													
	int quality_up_type = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(m_new_partner.partner_base_info[partner_id].new_quality);
	for (int type = QUALITY_TYPE_BLUE_TO_PURPLE; type <= quality_up_type && type < QUALITY_TYPE_MAX; ++type)
	{
		int quality_segment = 1;
		int max_quality_segment = PARTNER_QUALITY_SEGMENT_MAX_NUM + 1;
		if (type == quality_up_type)
		{
			max_quality_segment = m_new_partner.partner_base_info[partner_id].quality_segment;
		}
		for (; quality_segment < max_quality_segment; quality_segment++)
		{
			const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(partner_id, type, quality_segment));
			if (NULL != q_cfg)
			{
				this->AddAttrByQualityUp(attr_con, q_cfg);
			}
		}
	}

	AttributeList attr_pellet;
	m_role_module_mgr->GetRoleAttrPellet()->ReCalcAttrPartner(attr_pellet, partner_id, basic_cfg->profession);
	for (int i = 0; i < BATTLE_ATTR_MAX; i++)
	{
		attr_con.attr_list[i] += attr_pellet.m_attrs[i];
	}

	// 元素水晶修正
	if (m_new_partner.partner_base_info[partner_id].partner_crytal.use_time > 0)
	{
		const ItemBase* element_item = ITEMPOOL->GetItem(m_new_partner.partner_base_info[partner_id].partner_crytal.current_crytal_id);
		if (NULL != element_item)
		{
			const ElementCrystal* element_crystal = (const ElementCrystal*)element_item;
			attr_con.attr_list[BATTLE_ATTR_ELEMENT_WATER] = element_crystal->GetElementWater();
			attr_con.attr_list[BATTLE_ATTR_ELEMENT_FIRE] = element_crystal->GetElementFire();
			attr_con.attr_list[BATTLE_ATTR_ELEMENT_WIND] = element_crystal->GetElementWind();
			attr_con.attr_list[BATTLE_ATTR_ELEMENT_EARTH] = element_crystal->GetElementEarth();
		}
		else
		{
			attr_con.attr_list[BATTLE_ATTR_ELEMENT_WATER] = 0;
			attr_con.attr_list[BATTLE_ATTR_ELEMENT_FIRE] = 0;
			attr_con.attr_list[BATTLE_ATTR_ELEMENT_WIND] = 0;
			attr_con.attr_list[BATTLE_ATTR_ELEMENT_EARTH] = 0;
		}
	}
	else
	{
		attr_con.attr_list[BATTLE_ATTR_ELEMENT_WATER] = 0;
		attr_con.attr_list[BATTLE_ATTR_ELEMENT_FIRE] = 0;
		attr_con.attr_list[BATTLE_ATTR_ELEMENT_WIND] = 0;
		attr_con.attr_list[BATTLE_ATTR_ELEMENT_EARTH] = 0;
	}

	// 伙伴皮肤 属性加值
	for (int list_idx = 0; list_idx < PARTNER_SKIN_ONCE; ++list_idx)
	{
		if (this->IsActive(partner_id, list_idx)) // 加上所有激活皮肤的属性
		{
			const PartnerSkinCfgInfo *skin_att = LOGIC_CONFIG->GetSkinConfig()->GetPartnerSkinCfgInfo(SkinKey(p_param.partner_id, list_idx));
			if (skin_att != NULL)
			{
				// 增加属性
				this->AttrChangeBySkin(attr_con, skin_att, add_or_sub);
			}
		}
	}

	// 被动技能加成
	const QualityUpgradeCfg *q_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetParnterQualityCfg(LOGIC_CONFIG->GetPartnerConfig()->___QuaKey__(
		partner_id, LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(pbi.new_quality), pbi.quality_segment));
	if (NULL != q_cfg)
	{
		for (int t_s_num = 0; t_s_num < PARTNER_COMMON_SKILL_MAX; ++t_s_num)
		{
			int skill_id = q_cfg->common_skill_id_list[t_s_num];
			int skill_lv = q_cfg->common_skill_lv_list[t_s_num];

			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(skill_id);
			if (NULL == passive_skill) continue;

			int spid = passive_skill->sp_id;
			if (passive_skill->is_level_spid && skill_lv >= 1 && skill_lv < ARRAY_LENGTH(passive_skill->level_spid_list))
			{
				spid = passive_skill->level_spid_list[skill_lv];
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
					attr_con.attr_list[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
				}

				if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
					&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
				{
					BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_role_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
					attr_con.attr_list[detail_cfg->param_list[0]] += add_attr;
				}

				if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
					&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
				{
					BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
					attr_con.attr_list[detail_cfg->param_list[0]] += add_attr;
				}

				if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
					&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
					&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
					&& detail_cfg->param_list[2] > 0)
				{
					int attr_type0 = detail_cfg->param_list[0];
					int attr_type1 = detail_cfg->param_list[1];
					attr_con.attr_list[attr_type0] += static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
				}
			}
		}
	}

	{
		//伙伴天赋
		int tree_level_count = 0;
		int role_level = m_role_module_mgr->GetRole()->GetLevel();
		for (int tree_point = 0; tree_point < MAX_PARTNER_GIFT_TREE_SKILL_NUM; tree_point++)
		{
			int tree_level = partner_gift_tree.gift_tree[tree_point];
			tree_level_count += tree_level;
			const PartnerGiftUplevelCfg * tree_cfg = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftTreeUplevelByPartnerId(partner_id, tree_point + 1, tree_level);
			if (tree_cfg != NULL)
			{
				for (int attr_idx = 0; attr_idx < PartnerGiftUplevelCfg::MAX_PARTNER_TREE_MAX_ATTR_NUM; attr_idx++)
				{
					if (tree_cfg->attr_list[attr_idx].attr_type >= 0 && tree_cfg->attr_list[attr_idx].attr_type < BATTLE_ATTR_MAX)
					{
						attr_con.attr_list[tree_cfg->attr_list[attr_idx].attr_type] += tree_cfg->attr_list[attr_idx].attr_value;
					}
				}

				if (tree_cfg->sp_id > 0)
				{
					const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(tree_cfg->sp_id);
					if (NULL != spe_cfg)
					{
						for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
						{
							const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
							if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

							const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
							if (NULL == detail_cfg) continue;

							if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								attr_con.attr_list[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
							}

							if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(ceil(role_level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
								attr_con.attr_list[detail_cfg->param_list[0]] += add_attr;
							}

							if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
								attr_con.attr_list[detail_cfg->param_list[0]] += add_attr;
							}

							if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
								&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
								&& detail_cfg->param_list[2] > 0)
							{
								int attr_type0 = detail_cfg->param_list[0];
								int attr_type1 = detail_cfg->param_list[1];
								int attr_value = static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
								attr_con.attr_list[attr_type0] += attr_value;
							}
						}
					}
				}
			}
		}
		const PartnerGiftSpListCfg * sp_cfg = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftSpCfg(partner_id);
		if (sp_cfg != NULL)
		{
			for (int sp_idx = 0; sp_idx < PartnerGiftSpListCfg::MAX_SP_NUM; sp_idx++)
			{
				if (NULL != sp_cfg->sp_list[sp_idx] && sp_cfg->sp_list[sp_idx]->inborn_level_ask <= tree_level_count)
				{
					//特效生效
					const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_cfg->sp_list[sp_idx]->sp_id);
					if (NULL != spe_cfg)
					{
						for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
						{
							const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
							if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

							const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
							if (NULL == detail_cfg) continue;

							if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								attr_con.attr_list[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
							}

							if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(ceil(role_level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
								attr_con.attr_list[detail_cfg->param_list[0]] += add_attr;
							}

							if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
							{
								BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
								attr_con.attr_list[detail_cfg->param_list[0]] += add_attr;
							}

							if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
								&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
								&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
								&& detail_cfg->param_list[2] > 0)
							{
								int attr_type0 = detail_cfg->param_list[0];
								int attr_type1 = detail_cfg->param_list[1];
								int attr_value = static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
								attr_con.attr_list[attr_type0] += attr_value;
							}
						}
					}
				}
			}
		}
	}

	//飞升武器
	if (pbi.soar_arms_level > 0)
	{
		const PartnerSoarArmsCfg * soar_arms_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarArmsCfg(basic_cfg->soar_arms_id, pbi.soar_arms_level);
		if (NULL != soar_arms_cfg)
		{
			for (int i = 0; i < ARRAY_LENGTH(soar_arms_cfg->attr_list) && i < ARRAY_LENGTH(attr_con.attr_list); i++)
			{
				attr_con.attr_list[i] += soar_arms_cfg->attr_list[i];
			}
			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(soar_arms_cfg->arms_skill_id);
			if (NULL != passive_skill)
			{
				int spid = passive_skill->sp_id;
				if (passive_skill->is_level_spid && soar_arms_cfg->arms_skill_level >= 1 && soar_arms_cfg->arms_skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
				{
					spid = passive_skill->level_spid_list[soar_arms_cfg->arms_skill_level];
				}
				const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
				if (NULL != spe_cfg)
				{
					for (int k = 0; k < spe_cfg->effect_num && k < EFFECT_DETAIL_NUM; ++k)
					{
						const SpecialEffectKey & key = spe_cfg->effect_detail_list[k];
						if (key.effect_type != SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

						const EffectDetailCfg * detail_cfg = BattleSpecialEffectPool::GetInstance().GetEffectDetailCfg(key);
						if (NULL == detail_cfg) continue;

						if (SPE_ATTR_ADD_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
							&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
						{
							attr_con.attr_list[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
						}

						if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
							&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
						{
							BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_role_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
							attr_con.attr_list[detail_cfg->param_list[0]] += add_attr;
						}

						if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
							&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
						{
							BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
							attr_con.attr_list[detail_cfg->param_list[0]] += add_attr;
						}

						if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
							&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
							&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
							&& detail_cfg->param_list[2] > 0)
						{
							int attr_type0 = detail_cfg->param_list[0];
							int attr_type1 = detail_cfg->param_list[1];
							attr_con.attr_list[attr_type0] += static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
						}
					}
				}
			}
		}
	}

	//伙伴强化
	if (pbi.intensify_level > 0)
	{
		const PartnerIntensifyCfg *pi_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerIntensifyCfg(partner_id, pbi.intensify_level);
		if (NULL != pi_cfg)
		{
			for (int i = 0; i < (int)pi_cfg->attr_list.size(); i++)
			{
				const PartnerIntensifyCfg::AttrData &attr_data = pi_cfg->attr_list[i];

				attr_con.attr_list[attr_data.type] += attr_data.param;
			}
		}
	}

	// 符玉加成
	m_role_module_mgr->GetFuYuManager()->ReCalcPartnerAttr(partner_id, attr_con);

	// 情缘属性 - 加值 - 只有出战伙伴匹配才加 - 属性计算移动到战斗数据传入
	AttrCon not_skill_attr_con = attr_con;		//没有技能加成属性(这里的技能指的是技能自己拥有评分)
	std::vector<SkillPro> sill_list;
	AttributeList guild_inborn_attr;
	int guild_inborn_capbility = m_role_module_mgr->GetRoleGuildInborn()->GetSkillList(&sill_list, GUILD_INBORN_ADD_TARGET_TYPE_PARTNER);
	for (int i = 0; i < (int)sill_list.size(); i++)
	{
		const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(sill_list[i].skill_id);
		if (NULL == passive_skill) continue;

		int skill_level = sill_list[i].skill_level;
		int spid = passive_skill->sp_id;
		if (passive_skill->is_level_spid && skill_level >= 1 && skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
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
				guild_inborn_attr.m_attrs[detail_cfg->param_list[0]] += detail_cfg->param_list[1];
			}

			if (SPE_ATTR_ADD_ATTR_BY_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(m_role_module_mgr->GetRole()->GetLevel() * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
				guild_inborn_attr.m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL == detail_cfg->effect_sub_type && detail_cfg->param_num >= 2
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX)
			{
				BattleAttr add_attr = static_cast<BattleAttr>(ceil(pbi.level * (double)detail_cfg->param_list[1] / BATTLE_PERCENT_NUM));
				m_reason_list[PARTNER_REASON_LIST_TYPE_OTHER].m_attrs[detail_cfg->param_list[0]] += add_attr;
			}

			if (SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR == detail_cfg->effect_sub_type && detail_cfg->param_num >= 3
				&& detail_cfg->param_list[0] >= BATTLE_ATTR_MIN && detail_cfg->param_list[0] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[1] >= BATTLE_ATTR_MIN && detail_cfg->param_list[1] < BATTLE_ATTR_MAX
				&& detail_cfg->param_list[2] > 0)
			{
				int attr_type0 = detail_cfg->param_list[0];
				int attr_type1 = detail_cfg->param_list[1];
				guild_inborn_attr.m_attrs[attr_type0] += static_cast<int>(1.0 * tmp_base_attr_list[attr_type1] / (1.0 * detail_cfg->param_list[2] / BATTLE_PERCENT_NUM));
			}
		}
	}
	//家族灵脉可能会配5项基础属性
	AttributePlan tmp_ap;
	for (int i = 0, k = BATTLE_ADD_POINT_TYPE_BEGIN; i < ADD_POINT_TYPE_MAX && k <= BATTLE_ADD_POINT_TYPE_END; i++, k++)
	{
		tmp_ap.add_point_list[i] += guild_inborn_attr.m_attrs[k];
	}
	LOGIC_CONFIG->GetRoleExpConfig()->ConvertPointToAttribute(tmp_ap, guild_inborn_attr);
	for (int i = 0; i < BATTLE_ATTR_MAX; i++)
	{
		attr_con.attr_list[i] += guild_inborn_attr.m_attrs[i];
	}

	// 伙伴评分 - 必须放在属性计算完毕之后 先算这个,后再加技能特效属性
	this->CalcPartnerCapability(not_skill_attr_con, basic_cfg->profession);
	attr_con.capability = not_skill_attr_con.capability + guild_inborn_capbility;

	//伙伴--元神契约  需求--给某伙伴附加该伙伴除了 元神契约属性外的千分比基础总属性 并将战力加入attr_con.capability
	//注传入的是 无契约 的基础属性
	this->OnReCalcAttr(partner_id, not_skill_attr_con, is_notify);

	m_new_partner.partner_base_info[partner_id].capability = attr_con.capability;
	if (once_init == 0)//修改下发顺序
	{
		this->SendQualityUpInfo(partner_id, m_new_partner.partner_base_info[partner_id].partner_quality_type, Protocol::SCPartnerQualityUp::NOTICETYPE_INIT);
	}
	if (m_role_module_mgr->GetRole()->GetSceneStatus()->IsInAutoRecoverScene())
	{
		PartnerSql& p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
		p_sql.max_hp = attr_con.attr_list[BATTLE_ATTR_MAX_HP];
		p_sql.max_mp = attr_con.attr_list[BATTLE_ATTR_MAX_MP];

		p_sql.cur_hp = p_sql.max_hp;
		p_sql.cur_mp = p_sql.max_mp;

		attr_con.attr_list[BATTLE_ATTR_CUR_HP] = p_sql.cur_hp;
		attr_con.attr_list[BATTLE_ATTR_CUR_MP] = p_sql.cur_mp;
	}
	else
	{
		PartnerSql& p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
		p_sql.max_hp = attr_con.attr_list[BATTLE_ATTR_MAX_HP];
		p_sql.max_mp = attr_con.attr_list[BATTLE_ATTR_MAX_MP];

		p_sql.cur_hp = GetMin(p_sql.cur_hp, attr_con.attr_list[BATTLE_ATTR_MAX_HP]);
		p_sql.cur_hp = GetMax(1, p_sql.cur_hp);

		p_sql.cur_mp = GetMin(p_sql.cur_mp, attr_con.attr_list[BATTLE_ATTR_MAX_MP]);
		p_sql.cur_mp = GetMax(0, p_sql.cur_mp);

		attr_con.attr_list[BATTLE_ATTR_CUR_HP] = p_sql.cur_hp;
		attr_con.attr_list[BATTLE_ATTR_CUR_MP] = p_sql.cur_mp;
	}

	// 发送协议通知客户端
	if (is_notify)
	{
		std::vector<BattleAttrPair> attr_type_list;
		for (int t_cur = BATTLE_ATTR_MIN; t_cur < BATTLE_ATTR_MAX; ++t_cur)		// 只发送改变了的数据
		{
			if (tmp_attr_list[t_cur] != attr_con.attr_list[t_cur])
				attr_type_list.push_back(BattleAttrPair(t_cur, attr_con.attr_list[t_cur]));
		}

		this->SendPartnerAttrChange(partner_id, attr_type_list, notic_type);
	}

	EventHandler::Instance().OnPartnerCapabilityChange(m_role_module_mgr->GetRole(), old_partner_capability, attr_con.capability);
	RankManager::Instance().SyncPartnerRankInfo(this->GetRoleModuleManager()->GetRole(), PARTNER_RANK_TYPE_CAPABILITY, partner_id);

	if (BATTLE_STATE_FIGHT == p_param.fight_state)
	{
		RankManager::Instance().SyncPersonRankInfo(m_role_module_mgr->GetRole(), PERSON_RANK_TYPE_CAPABILITY);
		ColosseumManager::Instance().SyncUserData(m_role_module_mgr->GetRole());
	}
}

void Partner::CalcPartnerCapability(AttrCon & attr_con, int prof_type)
{
	const CapabilityCalCfg * prof_cap_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfCapabilityCalCfg(prof_type);
	if (NULL == prof_cap_cfg) return;

	double capability = 0;
	double special_capability = 0;
	for (int i = 0; i < BATTLE_ATTR_ANTI_VALUE_END && i < ARRAY_LENGTH(prof_cap_cfg->cap_base) && i < ARRAY_LENGTH(attr_con.attr_list); i++)
	{
		if (prof_cap_cfg->cap_base[i] == 0) continue;

		if (LOGIC_CONFIG->GetScoreSystemConfig()->IsSpecialAttrType(i))
		{
			special_capability += attr_con.attr_list[i] * 1.0 * prof_cap_cfg->cap_base[i];
		}
		else
		{
			capability += attr_con.attr_list[i] * 1.0 * prof_cap_cfg->cap_base[i];
		}
	}

	attr_con.capability = static_cast<int>(capability * 1.0F * CAPABILITY_BASE_NUM / CAPABILITY_ATTR_BASE_NUM) + special_capability;
}

void Partner::OnPartnerReCalcEnd()
{
}

void Partner::PartnerAttrBreakThrou(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	const PartnerBreakThCfg * p_brk_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, p_param.upgrade_lv, p_param.attr_stage);
	if (NULL == p_brk_cfg) return;

	int stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (int i = 0; i < PARTNER_ATTR_BKTH_ITEM_TYPE && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++i)
	{
		ItemExtern::ItemConsumeConfig & stuff_con = stuff_list[stuff_count];
		const ItemPairCon & stuff_cfg = p_brk_cfg->need_item[i];

		stuff_con.item_id = stuff_cfg.item_id;
		stuff_con.num = stuff_cfg.item_num;
		stuff_con.buyable = false;

		if (stuff_con.item_id > 0 && stuff_con.num > 0)
		{
			stuff_count += 1;
		}
	}

	// 消耗
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "WorkshopCompoundItem")) return;
	}

	// 突破
	int old_stage = p_param.attr_stage;
	++p_param.attr_stage;
	ROLE_LOG_QUICK10(LOG_TYPE_PARTNER_CHANGE_ATTR, m_role_module_mgr->GetRole(), p_param.partner_id, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_BREAKE_THOUGH,
		__FUNCTION__, NULL, p_param.attr_stage, old_stage, 0, 0, 0);
	// 	if (NULL == LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, p_param.upgrade_lv, p_param.attr_stage))
	// 	{
	// 		this->PartnerUPGradeHandler(idx);
	// 	}
	// 	else

	{
		this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_BREAKE_THOUGH);
		this->SendPartnerGradeBreak(partner_id, POP_UP_WINDOWS_NONE);
	}
	EventHandler::Instance().OnPartnerBreak(m_role_module_mgr->GetRole(), partner_id, m_new_partner.partner_base_info[partner_id].new_quality);
}

void Partner::PartnerUPGradeHandler(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	NewPartnerBaseInfo & base_info = m_new_partner.partner_base_info[partner_id];
	if (LOCK_STATE_LOCKED == p_param.is_unlocked)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
		return;
	}

	if (NULL != LOGIC_CONFIG->GetPartnerConfig()->GetCurAttrBreakCfg(p_param.partner_id, p_param.upgrade_lv, p_param.attr_stage))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_ATTR_BREAKTH_UNFINISHED);
		return;
	}

	if (NULL == LOGIC_CONFIG->GetPartnerConfig()->GetCurGradeAttrCfg(p_param.partner_id, p_param.upgrade_lv + 1))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_GRADE_ALREAY_MAX);
		return;
	}

	const PartnerUpGradeCfg * cur_upg_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetCurGradeAttrCfg(p_param.partner_id, p_param.upgrade_lv);
	if (NULL == cur_upg_cfg) return;

	if (m_role_module_mgr->GetRole()->GetLevel() < cur_upg_cfg->player_lv_need)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH); 
		return;
	}

	int max_upgrade_level_limit = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg().upgrade_level_limit;
	const PartnerSoarCfg * soar_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarCfg(base_info.soar_count);
	if(NULL != soar_cfg)
	{
		max_upgrade_level_limit = soar_cfg->upgrade_level;
	}

	if(p_param.upgrade_lv >= max_upgrade_level_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_UP_GRADE_FAIL_BY_MAX_LIMIT);
		return;
	}

	p_param.attr_stage = 0;
	int old_upgrade = p_param.upgrade_lv;
	++p_param.upgrade_lv;
	ROLE_LOG_QUICK10(LOG_TYPE_PARTNER_CHANGE_ATTR, m_role_module_mgr->GetRole(), p_param.partner_id, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_UP_GARD,
		__FUNCTION__, NULL, p_param.upgrade_lv, old_upgrade, 0, 0, 0);
	EventHandler::Instance().OnPartnerUpgradeLevel(m_role_module_mgr->GetRole(), p_param.partner_id, p_param.upgrade_lv);
	this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_UP_GARD);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_PARTNER, __FUNCTION__);
	this->SendPartnerGradeBreak(partner_id, POP_UP_WINDOWS_TRUE);
}

void Partner::SendPartnerFightState(int partner_id, bool is_show)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	static Protocol::SCPartnerCombatsStatus pcs;
	pcs.partner = p_param.partner_id;
	pcs.is_fight = p_param.fight_state;
	pcs.is_show = is_show ? 1 : 0;

	m_role_module_mgr->NetSend((const char*)&pcs, sizeof(pcs));


}

void Partner::SendPartnerAttrChange(int partner_id, const std::vector<BattleAttrPair> & attr_chan_list, char notic_type)
{
	if (this->IsPartnerInvalid(partner_id) || attr_chan_list.empty()) return;

	static Protocol::SCPartnerAttrChangeRep pacr;
	pacr.partner = partner_id;
	pacr.list_count = 0;
	pacr.notic_type = notic_type;
	pacr.capability = m_partner_attr_list[partner_id].capability;

	for (std::vector<BattleAttrPair>::const_iterator v_it = attr_chan_list.begin(); v_it != attr_chan_list.end(); ++v_it)
	{
		if (v_it->attr_type < BATTLE_ATTR_MIN || v_it->attr_type >= BATTLE_ATTR_MAX) continue;

		pacr.attr_list[(int)pacr.list_count++] = *v_it;
	}

	int send_len = sizeof(pacr) - (BATTLE_ATTR_MAX - pacr.list_count) * sizeof(pacr.attr_list[0]);
	m_role_module_mgr->NetSend((const char*)&pacr, send_len);
}

void Partner::SendPartnerFetterInfo(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	static Protocol::SCPartnerFettersInfoma pfi;
	pfi.partner = p_param.partner_id;
	pfi.resver_sh = 0;

	for (int t_cur = 0; t_cur < PARTNER_FETTER_COUNT_MAX; ++t_cur)
	{
		pfi.curr_fetter[t_cur] = p_param.curr_fetter[t_cur];
		pfi.wash_fetter[t_cur] = p_param.wash_fetter[t_cur];
	}

	m_role_module_mgr->NetSend((const char*)&pfi, sizeof(pfi));
}

void Partner::SendPartnerGradeBreak(int partner_id, int need_pop)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	const PartnerUpGradeCfg * p_pugc = LOGIC_CONFIG->GetPartnerConfig()->GetCurGradeAttrCfg(p_param.partner_id, p_param.upgrade_lv);
	if (NULL == p_pugc) return;

	static Protocol::SCPartnerBaseGradeInfo pbgi;

	pbgi.partner = p_param.partner_id;
	pbgi.level = p_param.upgrade_lv;
	pbgi.stage = p_param.attr_stage;

	pbgi.upgrade_ret = need_pop;
	pbgi.task_finish = 1;

	pbgi.resetve_sh = 0;

	m_role_module_mgr->NetSend((const char*)&pbgi, sizeof(pbgi));

}

void Partner::SendPartnerSkillsInfo(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	const NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	static Protocol::SCSavePartnersSkillRep spsr;
	spsr.partner = p_param.partner_id;
	memcpy(spsr.fight_skill_list, p_param.skill_list, sizeof(spsr.fight_skill_list));
	UNSTD_STATIC_CHECK(sizeof(spsr.fight_skill_list) == sizeof(p_param.skill_list));
	m_role_module_mgr->NetSend((const char*)&spsr, sizeof(spsr));
}

void Partner::SendPartnerEquipsInfo(int partner_id, int slot_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerCon & p_param = m_new_partner.partner_base_info[partner_id].partner_list;
	const NewPartnerEquipment & p_e = p_param.equip_list[slot_id];
	Protocol::SCPartnerEqSingleSlots pess;
	pess.partner_id = p_param.partner_id;
	pess.equip_slot = slot_id;
	pess.equip_id = p_e.partner_equip_id;
	pess.unlock_level = p_e.unlock_level;
	for (int i = 0; i < MAX_PARTNER_NATAL_CARD_VOC_NUM; i++)
	{
		pess.attr_pair[i] = BattleAttrPair(p_e.m_voc_list[i].attr_type, p_e.m_voc_list[i].attr_value);
	}

	m_role_module_mgr->NetSend((const char*)&pess, sizeof(pess));
}


void Partner::OnStoreBuy(int seq)
{
	if (0 > seq || seq >= MYSTERY_STORE_MAX_SEQ)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	NewStoreNatalCard & node = m_new_partner.store_natal_card[seq];
	const MysteryStoreItemGroupCfg * cfg = LOGIC_CONFIG->GetMysteryStoreConfig()->GetItemCfg(node.group_id, node.seq);
	if (NULL == cfg)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num, false))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();
		//m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (node.buy_count >= cfg->limit_buy && 0 != cfg->limit_buy)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
		return;
	}

	switch (cfg->price_type)
	{
	case MONEY_TYPE_GOLD:
		{
			if (cfg->price > m_role_module_mgr->GetMoney()->GetGold())
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		{
			if (cfg->price > m_role_module_mgr->GetMoney()->GetImmortalCoin())
			{
				m_role_module_mgr->NoticeNum(errornum::EN_MARKET_COIN_BIND_NOT_ENOUGH);
				return;
			}
		}
		break;
	case MONEY_TYPE_COIN_BIND:
		{
			if (cfg->price > m_role_module_mgr->GetMoney()->GetCoinBind())
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
	default:
		{
			if (cfg->price > m_role_module_mgr->GetMoney()->GetOtherCoin(cfg->price_type))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_MONEY_LUCK_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
	}

	//消耗
	switch (cfg->price_type)
	{
	case MONEY_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(cfg->price, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->UseImmortalCoin(cfg->price, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_MARKET_COIN_BIND_NOT_ENOUGH);
				return;
			}
		}
		break;
	case MONEY_TYPE_COIN_BIND:
		{
			if (!m_role_module_mgr->GetMoney()->UseCoinBind(cfg->price, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
	default:
		{
			if (!m_role_module_mgr->GetMoney()->UseOtherCoin(cfg->price, cfg->price_type, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_MONEY_LUCK_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
	}

	NetValueItemParam p_item;
	p_item.partner_natal_card_param.has_net_value = 1;
	memcpy(p_item.partner_natal_card_param.m_voc_list, node.m_voc_list, sizeof(node.m_voc_list));

	ItemDataWrapper item;
	item.item_id = cfg->item.item_id;
	item.num = cfg->item.num;
	item.is_bind = cfg->item.is_bind;
	p_item.GetDataByItemType(I_TYPE_PARTNER_NATAL_CARD, item.param_data);

	if (!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_MYSTERY_STORE))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}
	
	node.buy_count++;
	this->SendStoreInfo();
}

void Partner::OnStoreRefresh(bool is_auto)
{
	const MysterStoreConsumeAddCfg * consume_add_cfg = LOGIC_CONFIG->GetMysteryStoreConfig()->GetConsumeAddCfg(m_new_partner.store_refresh_num);
	if (NULL == consume_add_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	const MysteryStoreOtherCfg * other_cfg = LOGIC_CONFIG->GetMysteryStoreConfig()->GetOtherCfg();
	if (NULL == other_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	bool is_has = false;
	if (!is_auto && !this->IsFreeRefresh())
	{
		if (0 != consume_add_cfg->is_priority_use_item)
		{
			if (m_role_module_mgr->GetKnapsack()->Count(consume_add_cfg->consume_item.item_id) >= consume_add_cfg->consume_item.num)
			{
				is_has = true;
			}
		}
		if (!is_has)
		{
			switch (consume_add_cfg->refresh_money_type)
			{
			case MONEY_TYPE_GOLD:
				{
					if (consume_add_cfg->refresh_price > m_role_module_mgr->GetMoney()->GetGold())
					{
						m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
						return;
					}
				}
				break;
			case MONEY_TYPE_IMMORTAL_COIN:
				{
					if (consume_add_cfg->refresh_price > m_role_module_mgr->GetMoney()->GetImmortalCoin())
					{
						m_role_module_mgr->NoticeNum(errornum::EN_MARKET_COIN_BIND_NOT_ENOUGH);
						return;
					}
				}
				break;
			case MONEY_TYPE_COIN_BIND:
				{
					if (consume_add_cfg->refresh_price > m_role_module_mgr->GetMoney()->GetCoinBind())
					{
						m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
						return;
					}
				}
				break;
			default:
				{
					if (consume_add_cfg->refresh_price > m_role_module_mgr->GetMoney()->GetOtherCoin(consume_add_cfg->refresh_money_type))
					{
						m_role_module_mgr->NoticeNum(errornum::EN_MONEY_LUCK_COIN_NOT_ENOUGH);
						return;
					}
				}
				break;
			}
		}			
	}

	int level = m_role_module_mgr->GetRole()->GetLevel();
	const std::vector<MysteryStoreStoreCfg> * group_id_list = LOGIC_CONFIG->GetMysteryStoreConfig()->GetStoreCfg(level);
	if (NULL == group_id_list)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int pass_level = m_role_module_mgr->GetDreamNotesChallenge()->GetPassLevel();

	int count = 0;
	gamelog::g_log_partner.buff_printf("%s user[%d %s] pass_level[%d] list[", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), pass_level);
	for (std::vector<MysteryStoreStoreCfg>::const_iterator it = group_id_list->begin(); it != group_id_list->end() && count < MYSTERY_STORE_MAX_SEQ; ++it)
	{
		const MysteryStoreItemGroupCfg * cfg = LOGIC_CONFIG->GetMysteryStoreConfig()->RandGetItemCfg(it->item_group, pass_level);
		if (NULL == cfg)
		{
			gamelog::g_log_partner.printf(LL_INFO, "%s user[%d %s] Error -1 item_group[%d] pass_level[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
				it->item_group, pass_level);
			continue;
		}
	
		const ItemBase *itembase = ITEMPOOL->GetItem(cfg->item.item_id);
		if (NULL == itembase) 
		{
			gamelog::g_log_partner.printf(LL_INFO, "%s user[%d %s] Error -2 item_id[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), cfg->item.item_id);
			continue;
		}

		NetValueItemParam item_param;
		itembase->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &item_param, PUT_REASON_MYSTERY_STORE);
		itembase->SetRandomNetValueFlag(&item_param);
		NewPartnerNatalCardParam & natal_param = item_param.partner_natal_card_param;

		NewStoreNatalCard & node = m_new_partner.store_natal_card[count++];
		node.Reset();
		node.buy_count = 0;
		node.group_id = it->item_group;
		node.seq = cfg->seq;

		memcpy(node.m_voc_list, natal_param.m_voc_list, sizeof(node.m_voc_list));

		gamelog::g_log_partner.buff_printf("|%d %d|", it->item_group, cfg->seq);
	}
	gamelog::g_log_partner.buff_printf("]");
	gamelog::g_log_partner.commit_buff(LL_INFO);

	if (!is_auto && !this->IsFreeRefresh())
	{
		if (is_has)
		{
			if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(consume_add_cfg->consume_item.item_id, consume_add_cfg->consume_item.num, __FUNCTION__))
			{
				is_has = false;
			}
		}
		if (!is_has)
		{
			switch (consume_add_cfg->refresh_money_type)
			{
			case MONEY_TYPE_GOLD:
			{
				if (!m_role_module_mgr->GetMoney()->UseGold(consume_add_cfg->refresh_price, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
					return;
				}
			}
			break;
			case MONEY_TYPE_IMMORTAL_COIN:
			{
				if (!m_role_module_mgr->GetMoney()->UseImmortalCoin(consume_add_cfg->refresh_price, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_MARKET_COIN_BIND_NOT_ENOUGH);
					return;
				}
			}
			break;
			case MONEY_TYPE_COIN_BIND:
			{
				if (!m_role_module_mgr->GetMoney()->UseCoinBind(consume_add_cfg->refresh_price, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return;
				}
			}
			break;
			default:
			{
				if (!m_role_module_mgr->GetMoney()->UseOtherCoin(consume_add_cfg->refresh_money_type, consume_add_cfg->refresh_price, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_MONEY_LUCK_COIN_NOT_ENOUGH);
					return;
				}
			}
			break;
			}
			
			m_new_partner.store_refresh_num++;
		}
	}

	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (is_auto && 0 != m_new_partner.store_next_auto_refresh_time)
	{
		int pass_time = (now_time - m_new_partner.store_next_auto_refresh_time) % other_cfg->auto_time;
		m_new_partner.store_next_auto_refresh_time = now_time + other_cfg->auto_time - pass_time;
	}
	else if (this->IsFreeRefresh())
	{
		m_new_partner.next_free_refresh_time = this->CalculateNextFreeRefreshTime();
	}
	else
	{
		m_new_partner.store_next_auto_refresh_time = now_time + other_cfg->auto_time;
	}
	m_new_partner.store_ver = other_cfg->mystery_store_ver;
	m_partner.store_ver = other_cfg->mystery_store_ver;
	this->SendStoreInfo();
}

void Partner::SendStoreInfo()
{
	Protocol::SCMysteryStoreInfo info;
	info.store_refresh_num = m_new_partner.store_refresh_num;
	info.count = 0;
	info.next_free_refresh_time = m_new_partner.next_free_refresh_time;
	info.next_refresh_time = m_new_partner.store_next_auto_refresh_time - static_cast<unsigned int>(EngineAdapter::Instance().Time());
	for (int i = 0; i < MYSTERY_STORE_MAX_SEQ; ++i)
	{
		if (0 == m_new_partner.store_natal_card[i].group_id || -1 == m_new_partner.store_natal_card[i].seq)
		{
			continue;
		}

		info.data[info.count++] = m_new_partner.store_natal_card[i];
	}

	int len = sizeof(info) - sizeof(info.data[0]) * (MYSTERY_STORE_MAX_SEQ - info.count);
	m_role_module_mgr->NetSend(&info, len);
}

unsigned int Partner::CalculateNextFreeRefreshTime()
{
	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	return now_time + EngineAdapter::Instance().NextDayInterval(12, 0, 0);
}

bool Partner::IsFreeRefresh()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	return m_new_partner.next_free_refresh_time <= now;
}

void Partner::OnPageCompose(int item_id)
{
	const PartnerComposePageCfg *page_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetComposePageCfg(item_id);
	if(NULL == page_cfg) return;

	if (page_cfg->num > m_role_module_mgr->GetKnapsack()->Count(item_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_PAGE_COMPOSE_ITEM_NOT_ENOUGH);
		return;
	}
	const PartnerComposeCfg *rand_compose_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetRandComposeRewardCfg(page_cfg->reward_group_id);
	if (NULL == rand_compose_cfg)
	{
		return;
	}
	const ItemBase* item_base = ITEMPOOL->GetItem(rand_compose_cfg->item.item_id);
	if (NULL == item_base || I_TYPE_EXPENSE != item_base->GetItemType())
	{
		return;
	}
	const NorexItem * norex_item = (const NorexItem *)item_base;
	if (norex_item->GetUseType() != NorexItem::I_NOREX_ITEM_PARTNER_LICENCE)
	{
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(item_id, page_cfg->num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_PAGE_COMPOSE_ITEM_NOT_ENOUGH);
		return;
	}
	int param1 = norex_item->GetParam1();
	int param2 = norex_item->GetParam2();
	int param3 = norex_item->GetParam3();
	std::vector<ItemConfigData> notice_list;
	for (int i = 0; i < rand_compose_cfg->item.num; i++)
	{
		if (!this->ActiveAppointedPartner(rand_compose_cfg->item.item_id, UNLOCK_TYPE_ITEM_NOREX))
		{
			notice_list.push_back(ItemConfigData(param1, param3 == 1 ? true : false, param2 * (rand_compose_cfg->item.num - i)));
			break;
		}
	}
	if (!notice_list.empty())
	{
		bool is_send_mail = true;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutList((short)notice_list.size(), &notice_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)notice_list.size(), &notice_list[0], PUT_REASON_NOREXITEM_ITEM))
			{
				is_send_mail = false;
			}
		}
		if (is_send_mail)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&notice_list[0], (int)notice_list.size());
		}
		m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)notice_list.size(), &notice_list[0], GET_REASON_PARTNER_LIENCE, norex_item->GetParam1(), norex_item->GetParam2());
	}
}

void Partner::GmSetTranf(char tranf_id)
{
	m_partner.tranf_id = tranf_id;
}

void Partner::UpLevelGiftTree(int partner_id, int tree_point)
{
	if (partner_id < 0 || partner_id >= PARTNER_NUM_MAX)return;
	if (tree_point - 1 < 0 || tree_point - 1 >= MAX_PARTNER_GIFT_TREE_SKILL_NUM)return;
	PartnerGiftTree & node = m_partner_gift_tree_list.partner_gift_tree_list[partner_id];
	int &tree_level = node.gift_tree[tree_point - 1];
	if (tree_level + 1 > MAX_PARTNER_GIFT_TREE_LEVEL)
	{
		return;
	}
	const PartnerGiftTreeNodeCfg* tree_cfg = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftTreeByPartnerId(partner_id, tree_point);
	if (tree_cfg == NULL)return;
	//下一等级节点返还计算
	const PartnerGiftUplevelCfg * level_cfg = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftTreeUplevelByPartnerId(partner_id, tree_point, tree_level + 1);
	if (level_cfg == NULL)return;

	if (tree_cfg->pre_hole_num)
	{
		bool uplevel_suc = true;
		for (int i = 0; i < tree_cfg->pos_hole_num && i < 2; i++)
		{
			if (tree_cfg->pre_hole[i].hole - 1 >= 0 && tree_cfg->pre_hole[i].hole - 1 < MAX_PARTNER_GIFT_TREE_SKILL_NUM)
			{
				int &pre_tree_level = node.gift_tree[tree_cfg->pre_hole[i].hole - 1];
				if (pre_tree_level < tree_cfg->pre_hole[i].level_limit)
				{
					uplevel_suc = true;
				}
			}
		}
		if (!uplevel_suc)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_GIFT_TREE_LEVEL_LIMIT);
			return;
		}
	}

	int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	if (level_cfg->consume_item_list.size() > 0)
	{
		for (std::vector<ItemConfigData>::const_iterator it = level_cfg->consume_item_list.begin(); it != level_cfg->consume_item_list.end(); it++)
		{
			stuff_list[stuff_count].buyable = false;
			stuff_list[stuff_count].num = it->num;
			stuff_list[stuff_count++].item_id = it->item_id;
		}
	}
	// 消耗
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}

		if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "Partner::UpLevelGiftTree")) return;
	}

	tree_level += 1;

	this->ReCalcPartnerAttr(partner_id);
	this->SendSingleGiftTreeInfo(partner_id);
}

void Partner::ResetGiftTree(int partner_id, int tree_point)
{
	if (partner_id < 0 || partner_id >= PARTNER_NUM_MAX)return;
	if (tree_point - 1 < 0 || tree_point - 1 >= MAX_PARTNER_GIFT_TREE_SKILL_NUM)return;
	PartnerGiftTree & node = m_partner_gift_tree_list.partner_gift_tree_list[partner_id];

	int &tree_level = node.gift_tree[tree_point - 1];
	if (tree_level <= 0)return;

	//获取这个天赋节点的后续节点，同样重置后续节点
	int back_coin = 0;
	std::vector<ItemConfigData> back_item;
	int round_num = 0;
	std::vector<int> recale_point;
	this->ResetTreePoint(&round_num, partner_id, tree_point, &back_coin, &back_item, &recale_point);

	if (back_coin > 0 && !m_role_module_mgr->GetMoney()->CoinBindMoreThan(back_coin))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	//道具累积
	std::vector<ItemConfigData> recale_list;
	for (std::vector<ItemConfigData>::iterator it = back_item.begin(); it != back_item.end(); it++)
	{
		bool recale_add = false;
		for (std::vector<ItemConfigData>::iterator recale_it = recale_list.begin(); recale_it != recale_list.end(); recale_it++)
		{
			if (recale_it->item_id == it->item_id)
			{
				recale_it->num += it->num;
				recale_add = true;
				break;
			}
		}
		if (!recale_add)
		{
			recale_list.push_back(*it);
		}
	}
	if (recale_list.size() <= 0)return;
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)recale_list.size(), &recale_list[0]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}
	if(back_coin > 0)m_role_module_mgr->GetMoney()->UseCoinBind(back_coin, __FUNCTION__);
	//允许重置
	if (recale_point.size() <= 0)return;
	for (std::vector<int>::iterator reset_point_it = recale_point.begin(); reset_point_it != recale_point.end(); reset_point_it++)
	{
		if ((*reset_point_it) - 1 < 0 || (*reset_point_it) - 1 >= MAX_PARTNER_GIFT_TREE_SKILL_NUM)continue;
		gamelog::g_log_partner.printf(LL_INFO, "Partner::ResetGiftTre user[%s,%d] reset_partner_id[%d] tree_point[%d] reset_point[%d]", m_role_module_mgr->GetRoleName(), m_role_module_mgr->GetRoleUID(),
			partner_id, tree_point, *reset_point_it);//记录天赋树重置
		node.gift_tree[(*reset_point_it) - 1] = 0;
	}

	m_role_module_mgr->GetKnapsack()->PutList((short)recale_list.size(), &recale_list[0], PUT_REASON_NOREXITEM_ITEM);
	this->ReCalcPartnerAttr(partner_id);
	this->SendSingleGiftTreeInfo(partner_id);
}

void Partner::ResetTreePoint(int *round_num, int partner_id, int tree_point, int *back_coin, std::vector<ItemConfigData>* back_item_vec, std::vector<int>* reset_point)
{
	if (NULL == round_num || *round_num + 1 > 50)return;//递归最大次数返回
	*round_num += 1;
	if (partner_id < 0 || partner_id >= PARTNER_NUM_MAX)return;
	if (tree_point - 1 < 0 || tree_point - 1 >= MAX_PARTNER_GIFT_TREE_SKILL_NUM)return;
	PartnerGiftTree & node = m_partner_gift_tree_list.partner_gift_tree_list[partner_id];

	int &tree_level = node.gift_tree[tree_point - 1];
	if (tree_level <= 0)return;

	const PartnerGiftTreeNodeCfg* tree_cfg = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftTreeByPartnerId(partner_id, tree_point);
	if (tree_cfg == NULL)return;
	
	//当前节点返还计算
	for (int tree_level_idx = 1; tree_level_idx <= tree_level; tree_level_idx++)
	{
		const PartnerGiftUplevelCfg * level_cfg = LOGIC_CONFIG->GetPartnerGiftTreeConfig()->GetPartnerGiftTreeUplevelByPartnerId(partner_id, tree_point, tree_level_idx);
		if (level_cfg == NULL)continue;
		if (level_cfg->consume_item_list.size() > 0)
		{
			for (std::vector<ItemConfigData>::const_iterator it = level_cfg->consume_item_list.begin(); it != level_cfg->consume_item_list.end(); it++)
			{
				back_item_vec->push_back(*it);
			}
		}
	}
	
	if (tree_cfg->reset_consume_coin > 0)*back_coin += tree_cfg->reset_consume_coin;
	reset_point->push_back(tree_point);
	for (int i = 0; i < tree_cfg->pos_hole_num && i < 3; i++)
	{
		if (tree_cfg->pos_hole[i] > 0)
		{
			bool has_reset_point = false;
			for (std::vector<int>::iterator it = reset_point->begin(); it != reset_point->end(); it++)
			{
				if ((*it) == tree_cfg->pos_hole[i])has_reset_point = true;//排除被重置过的节点
			}
			if (!has_reset_point)this->ResetTreePoint(round_num, partner_id, tree_cfg->pos_hole[i], back_coin, back_item_vec, reset_point);//继续往后找
			continue;
		}
	}
}

void Partner::SendGiftTreeInfo()
{
	Protocol::SCPartnerGiftTreeListInfo info;
	info.partner_num = 0;
	for (int partner_idx = 0; partner_idx < PARTNER_NUM_MAX; partner_idx++)
	{
		if (this->IsPartnerInvalid(partner_idx)) continue;
		info.partner_gift_tree_list[info.partner_num].partner_id = partner_idx;
		info.partner_gift_tree_list[info.partner_num++].gift_tree = m_partner_gift_tree_list.partner_gift_tree_list[partner_idx];
	}
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info) - ((PARTNER_NUM_MAX - info.partner_num) * sizeof(Protocol::PartnerGiftTreeNode)));
}

void Partner::SendSingleGiftTreeInfo(int partner_id)
{
	Protocol::SCPartnerSingleGiftTreeInfo info;
	info.partner_id = partner_id;
	info.partner_gift_tree = m_partner_gift_tree_list.partner_gift_tree_list[partner_id];
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void Partner::OnAttrPelletChange(int partner_id)
{
	this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_ATTR_PELLET);
}

void Partner::OnPartnerSoar(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerBaseInfo & pbi = m_new_partner.partner_base_info[partner_id];
	const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	const PartnerSoarCfg * soar_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarCfg(pbi.soar_count + 1);
	if (NULL == basic_cfg || NULL == soar_cfg)
	{
		return;
	}
	if (pbi.level < soar_cfg->soar_need_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_SOAR_FAIL_BY_LEVEL_NOT_ENOUGH);
		return;
	}
	if (!m_role_module_mgr->GetTaskRecorder()->HaveFinished(basic_cfg->soar_task_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_SOAR_FAIL_BY_SOAR_TASK_NOT_FINISH);	
		return;
	}
	if (pbi.capability < soar_cfg->soar_need_score)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_SOAR_FAIL_BY_SCORE_NOT_ENOUGH);
		return;
	}
	//伙伴飞升去除进阶限制
	/*if (pbi.partner_list.upgrade_lv < LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg().upgrade_level_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_SOAR_FAIL_BY_UPGRADE_LEVEL_LIMIT);
		return;
	}*/
	int quality_upgrade_type = LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(pbi.new_quality);
	if (quality_upgrade_type < soar_cfg->soar_need_quality_upgrade || (quality_upgrade_type == soar_cfg->soar_need_quality_upgrade && pbi.quality_segment < soar_cfg->soar_need_quality_segment))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_SOAR_FAIL_BY_QUALITY_LIMIT);
		return;
	}

	pbi.soar_count += 1;
// 	if (1 == pbi.soar_count)	//第一次飞升时激活飞升武器	(飞升不激活飞升武器了)
// 	{
// 		pbi.soar_arms_level = 1;
// 		this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_SOAR);
// 	}
	this->SendPartnerSoarNoitce(Protocol::SCPartnerSoarNoitce::NOTICE_TYPE_SOAR, partner_id);
	gamelog::g_log_partner.printf(LL_INFO, "%s user[%d, %s] partner_id:%d soar_count:%d soar_arms_level:%d", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
		partner_id, pbi.soar_count, pbi.soar_arms_level);
}

void Partner::OnPartnerSoarArmsLevelUp(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return;

	NewPartnerBaseInfo & pbi = m_new_partner.partner_base_info[partner_id];
	const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (NULL == basic_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

//  const PartnerSoarCfg * soar_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarCfg(pbi.soar_count);
//  if (NULL == soar_cfg)
// 	{
//  	m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_SOAR_ARMS_LEVEL_UP_FAIL_BY_NOT_SOAR);
// 		return;
// 	}

	if (LOGIC_CONFIG->GetPartnerConfig()->GetCanQualityUpType(pbi.new_quality) < LOGIC_CONFIG->GetPartnerConfig()->GetPartnerOtherCfg().soar_arms_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_SOAR_ACTIVATION_FAILED);
		return;
	}

	if (pbi.soar_arms_level >= MAX_PARTNER_ARMS_LEVEL)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_SOAR_ARMS_LEVEL_UP_FAIL_LEVEL_LIMIT);
		return;
	}

	//判断所需消耗的道具
	const PartnerSoarArmsCfg* soar_arms_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarArmsCfg(basic_cfg->soar_arms_id, pbi.soar_arms_level);
	const PartnerSoarArmsCfg * new_soar_arms_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarArmsCfg(basic_cfg->soar_arms_id, pbi.soar_arms_level + 1);
	if (NULL == soar_arms_cfg || NULL == new_soar_arms_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_SOAR_ARMS_LEVEL_UP_FAIL_LEVEL_LIMIT);
		return;
	}
	if (!soar_arms_cfg->break_consume_list.empty())
	{
		int stuff_count = 0;
		ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
		for (int i = 0; i < (int)soar_arms_cfg->break_consume_list.size() && i < ARRAY_LENGTH(stuff_list); i++)
		{
			stuff_list[stuff_count].item_id = soar_arms_cfg->break_consume_list[i].item_id;
			stuff_list[stuff_count].num = soar_arms_cfg->break_consume_list[i].num;
			stuff_list[stuff_count].buyable = false;
			stuff_count++;
		}
		if (stuff_count > 0)
		{
			static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
			if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}

			if (consume_list.count > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
					return;
				}
			}
		}
	}

	pbi.soar_arms_level += 1;
	this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_SOAR_ARMS_LEVEL_UP);
	this->SendPartnerSoarNoitce(Protocol::SCPartnerSoarNoitce::NOTICE_TYPE_SOAR_ARMS_LEVEL_UP, partner_id);
	gamelog::g_log_partner.printf(LL_INFO, "%s user[%d, %s] partner_id:%d soar_count:%d soar_arms_level:%d", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		partner_id, pbi.soar_count, pbi.soar_arms_level);
}

int Partner::GetUseMaxAttrPellet(int partner_id)
{
	int can_use_max_attr_pellet = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetOtherCfg().use_limit;
	if(this->IsPartnerInvalid(partner_id)) return can_use_max_attr_pellet;

	const PartnerSoarCfg * soar_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarCfg(m_new_partner.partner_base_info[partner_id].soar_count);
	if (NULL == soar_cfg || m_new_partner.partner_base_info[partner_id].soar_count <= 0)
	{
		return can_use_max_attr_pellet;
	}
	can_use_max_attr_pellet = soar_cfg->attr_pellet_limit;
	return can_use_max_attr_pellet;
}

void Partner::GetSoarArmsPassiveSkillById(int partner_id, int max_other_sp_num, int * out_other_sp_num, int * out_other_sp_list)
{
	if(this->IsPartnerInvalid(partner_id) || NULL == out_other_sp_num || NULL == out_other_sp_list) return;
	
	const short & soar_arms_level = m_new_partner.partner_base_info[partner_id].soar_arms_level;
	const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if(NULL == basic_cfg || soar_arms_level <= 0) return;

	const PartnerSoarArmsCfg * soar_arms_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerSoarArmsCfg(basic_cfg->soar_arms_id, soar_arms_level);
	if(NULL == soar_arms_cfg) return;

	const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(soar_arms_cfg->arms_skill_id);
	if (NULL == passive_skill) return;

	int spid = passive_skill->sp_id;
	if (passive_skill->is_level_spid && soar_arms_cfg->arms_skill_level >= 1 && soar_arms_cfg->arms_skill_level < ARRAY_LENGTH(passive_skill->level_spid_list))
	{
		spid = passive_skill->level_spid_list[soar_arms_cfg->arms_skill_level];
	}
	const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
	if (NULL == spe_cfg) return;

	// 加面板的特效不收集入战斗
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

void Partner::SendPartnerSoarNoitce(int notice_type, int partner_id)
{
	if (this->IsPartnerInvalid(partner_id) || notice_type < 0 || notice_type >= Protocol::SCPartnerSoarNoitce::NOTICE_TYPE_MAX) return;

	Protocol::SCPartnerSoarNoitce info;
	info.notice_type = notice_type;
	info.partner_id = partner_id;
	info.soar_count = m_new_partner.partner_base_info[partner_id].soar_count;
	info.soar_arms_level = m_new_partner.partner_base_info[partner_id].soar_arms_level;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

int Partner::GetPartnerSoarArmsLevel(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return 0;
	NewPartnerBaseInfo & pbi = m_new_partner.partner_base_info[partner_id];
	return pbi.soar_arms_level;
}

int Partner::GetPartnerSoarLevel(int partner_id)
{
	if (this->IsPartnerInvalid(partner_id)) return 0;
	NewPartnerBaseInfo & pbi = m_new_partner.partner_base_info[partner_id];
	return pbi.soar_count;
}

void Partner::ReCalcPartnerAttrByFuYu(int partner_id, bool is_notify, char notic_type, char add_or_sub)
{
	this->ReCalcPartnerAttr(partner_id, is_notify, notic_type, add_or_sub);
}

std::vector<int> Partner::GetActiveParnerList()
{
	std::vector<int> ret;
	ret.reserve(10);
	for (int id = 0; id < PARTNER_NUM_MAX; ++id)
	{
		if (this->IsPartnerAlreadyActive(id))
		{
			ret.push_back(id);
		}
	}
	return ret;
}

void Partner::GetRolePartnerInfoParam(RolePartnerInfoParam * role_partner_info_param)
{
	PB_RolePartnerInfoData* pb_data = (PB_RolePartnerInfoData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_PARTNER_INFO_DATA);
	if (NULL == pb_data)
		return;

	pb_data->mutable_pertner_data_list()->Clear();
	RolePartnerInfoData role_partner_info_data;
	for (int i = 0; i < PARTNER_NUM_MAX; i++)
	{
		if (this->IsPartnerInvalid(i))
			continue;

		NewPartnerBaseInfo & partner_info = m_new_partner.partner_base_info[i];
		NewPartnerCon & p_param = partner_info.partner_list;
		if (LOCK_STATE_LOCKED == p_param.is_unlocked)
			continue;

		role_partner_info_data.Reset();
		role_partner_info_data.partner_id = p_param.partner_id;
		role_partner_info_data.level = partner_info.level;
		role_partner_info_data.upgrade_lv = p_param.upgrade_lv;
		role_partner_info_data.quality = partner_info.new_quality;
		role_partner_info_data.quality_segment = partner_info.quality_segment;

		int pact_count = 0;
		for (int j = 0; j < 3 && j < PARTNER_SPIRIT_PACT_MAX; j++)
		{
			if(partner_info.put_on_pact_id_list[j] <= 0) continue;

			role_partner_info_data.pact_id_list[pact_count] = partner_info.put_on_pact_id_list[j];
			pact_count++;
		}

		for (int j = 0; i < 3 && j < PE_TYPE_MAX; j++)
		{
			role_partner_info_data.equip_list[j] = p_param.equip_list[j].partner_equip_id;
		}

		role_partner_info_data.capability = partner_info.capability;

		role_partner_info_data.GetPBData(pb_data->add_pertner_data_list());
	}

	if (!pb_data->SerializeToArray(role_partner_info_param->partner_data, ARRAY_LENGTH(role_partner_info_param->partner_data)))
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}
}

bool Partner::PactGridPutIn(ItemID item_id, int num, int reason_type)
{
	if (0 >= num)
	{
		return true;
	}
	int save_index = LOGIC_CONFIG->GetPartnerConfig()->GetPactSaveIndex(item_id);
	if (0 > save_index
		|| PARTNER_SPIRIT_PACT_MAX_GRID <= save_index)
	{
		return false;
	}

	PartnerSpiritPactParam::PactGridInfo &grid_info
		= m_pact_grid_param.grid_list[save_index];

	if (0 >= grid_info.item_num + num)
	{//溢出
		return false;
	}

	grid_info.item_num += num;

	this->SendPactGridChange(save_index, item_id,
		grid_info.item_num);

	switch (reason_type)
	{
	//需求 只有商店获得 要提示
	case PUT_REASON_CONVERT_SHOP:
		{
			static ItemConfigData temp_item;
			temp_item.item_id = item_id;
			temp_item.num = num;
			temp_item.is_bind = false;

			m_role_module_mgr->GetKnapsack()->NotifyGetItemList(1, &temp_item, GET_REASON_PARTNER_PACT);
		}
		break;
	default:
		break;
	}


	//log
	gamelog::g_log_partner.printf(LL_INFO,
		"%s %d | uid:%d  name:%s | save_index:%d, item_id:%d, num:%d, add_num:%d, reason_type%d",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		save_index, item_id, grid_info.item_num,
		num, reason_type);

	ROLE_LOG_QUICK10(LOG_TYPE_PARTNER_PACT, m_role_module_mgr->GetRole(), item_id, num, NULL, NULL, reason_type, 0, 0, 0, 0);

	return true;
}

bool Partner::PactGridTakeOut(ItemID item_id, int num, int reason_type)
{
	if (0 >= num)
	{//不允许移除 0 或 负数
		return false;
	}
	int save_index = LOGIC_CONFIG->GetPartnerConfig()->GetPactSaveIndex(item_id);
	if (0 > save_index
		|| PARTNER_SPIRIT_PACT_MAX_GRID <= save_index)
	{
		return false;
	}

	PartnerSpiritPactParam::PactGridInfo &grid_info
		= m_pact_grid_param.grid_list[save_index];

	if (grid_info.item_num < num)
	{//不足
		return false;
	}
	grid_info.item_num -= num;

	this->SendPactGridChange(save_index, item_id, grid_info.item_num);

	//log
	gamelog::g_log_partner.printf(LL_INFO,
		"%s %d | uid:%d  name:%s | save_index:%d, item_id:%d, num:%d, add_num:%d, reason_type%d",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		save_index, item_id, grid_info.item_num,
		num, reason_type);

	return true;
}

bool Partner::PactPutOn(int partner_id, int put_index, int put_on_save_index)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_CRYSTAL_FB, true))
	{
		return false;
	}
	

	if (!IsPartnerAlreadyActive(partner_id))
	{
		return false;
	}

	//校验--装备位置
	if (0 > put_index
		|| PARTNER_SPIRIT_PACT_MAX <= put_index)
	{
		return false;
	}

	//校验--装备当前储存位置
	ItemID put_on_item_id
		= LOGIC_CONFIG->GetPartnerConfig()->GetPactItemID(put_on_save_index);
	if (0 == put_on_item_id)
	{
		return false;
	}

	//校验物品数量
	if (1 > m_pact_grid_param.grid_list[put_on_save_index].item_num)
	{
		return false;
	}

	//校验物品类型
	const ItemBase * put_on_item_base = ITEMPOOL->GetItem(put_on_item_id);
	if (NULL == put_on_item_base)
	{
		return false;
	}
	if (I_TYPE_PARTNER_PACT != put_on_item_base->GetItemType())
	{
		return false;
	}
	const SpiritPactItem *put_on_pact_item = (SpiritPactItem *)put_on_item_base;
	int pact_partner_id = put_on_pact_item->GetPartnerID();
	if (!IsPartnerAlreadyActive(pact_partner_id))
	{
		return false;
	}

	//校验 伙伴类型不能相同
	if (pact_partner_id == partner_id)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_PACT_CANT_SELF);
		return false;
	}
	//有装备 卸下(清0)
	ItemID & now_put_item_id = m_new_partner.partner_base_info[partner_id].put_on_pact_id_list[put_index];
	if (0 != now_put_item_id)
	{
		if (!this->PactGetOff(partner_id, put_index))
		{
			return false;
		}
	}

	//已装备同伙伴契约
	if (m_partner_pact_list[partner_id].IsPact(pact_partner_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_PACT_CANT_SAME);
		return false;
	}

	if (PARTNER_SPIRIT_BE_PACT_MAX <= (int)m_partner_pact_list[pact_partner_id].GetBePactSet().size())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_PACT_MAX);
		return false;
	}


	//消耗
	if (!this->PactGridTakeOut(put_on_item_id, 1, PUT_REASON_PARTNER_PACT_PUT_ON))
	{
		return false;
	}

	//装备
	now_put_item_id = put_on_item_id;

	m_partner_pact_list[partner_id].PactAdd(pact_partner_id);	
	m_partner_pact_list[pact_partner_id].BePactAdd(partner_id);

	//重算
	this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_PACT_CHANGE);

	this->SendPactInfo(partner_id, Protocol::SCPartnerPactInfo::DEF, put_index);

	return true;
}

bool Partner::PactGetOff(int partner_id, int put_index)
{
	if (!IsPartnerAlreadyActive(partner_id))
	{
		return false;
	}

	//校验--装备位置
	if (0 > put_index
		|| PARTNER_SPIRIT_PACT_MAX <= put_index)
	{
		return false;
	}

	ItemID & now_put_item_id = m_new_partner.partner_base_info[partner_id].put_on_pact_id_list[put_index];
	if (0 == now_put_item_id)
	{
		return false;
	}

	const ItemBase * put_on_item_base = ITEMPOOL->GetItem(now_put_item_id);
	if (NULL == put_on_item_base)
	{
		return false;
	}
	if (I_TYPE_PARTNER_PACT != put_on_item_base->GetItemType())
	{
		return false;
	}
	const SpiritPactItem *put_on_pact_item = (SpiritPactItem *)put_on_item_base;

	//放入仓库
	if (!this->PactGridPutIn(now_put_item_id, 1, PUT_REASON_PARTNER_PACT_PUT_ON))
	{
		return false;
	}
	//卸下
	now_put_item_id = 0;

	int pact_partner_id = put_on_pact_item->GetPartnerID();

	m_partner_pact_list[partner_id].PactOff(pact_partner_id);
	if (this->IsPartnerValid(pact_partner_id))
	{
		m_partner_pact_list[pact_partner_id].BePactOff(partner_id);
	}

	//重算
	this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_PACT_CHANGE);

	this->SendPactInfo(partner_id, Protocol::SCPartnerPactInfo::DEF, put_index);

	return true;
}

bool Partner::PactSwap(int off_partner_id, int off_index, int put_partner_id, int put_index)
{
	// 先卸下 然后装到其他地方 非要这样合并写 难受
	if (!IsPartnerAlreadyActive(off_partner_id))
	{
		return false;
	}
	//暂存目标装备的信息
	ItemID target_item_id = m_new_partner.partner_base_info[off_partner_id].put_on_pact_id_list[off_index];
	int target_save_index
		= LOGIC_CONFIG->GetPartnerConfig()->GetPactSaveIndex(target_item_id);
	if (0 > target_save_index)
	{
		return false;
	}

	//卸下
	this->PactGetOff(off_partner_id, off_index);
	//穿戴
	this->PactPutOn(put_partner_id, put_index, target_save_index);

	return true;
}

void Partner::PactUpStar(int partner_id, int put_index)
{
	if (!IsPartnerAlreadyActive(partner_id))
	{
		return;
	}

	//校验--装备位置
	if (0 > put_index
		|| PARTNER_SPIRIT_PACT_MAX <= put_index)
	{
		return;
	}

	ItemID & now_put_item_id = m_new_partner.partner_base_info[partner_id].put_on_pact_id_list[put_index];
	if (0 == now_put_item_id)
	{
		return;
	}

	const ItemBase * put_on_item_base = ITEMPOOL->GetItem(now_put_item_id);
	if (NULL == put_on_item_base)
	{
		return;
	}
	if (I_TYPE_PARTNER_PACT != put_on_item_base->GetItemType())
	{
		return;
	}
	const SpiritPactItem *put_on_pact_item = (SpiritPactItem *)put_on_item_base;

	//升星产物
	ItemID m_up_star_out_id = put_on_pact_item->GetUpStarOutID();
	{
		const ItemBase * out_item_base = ITEMPOOL->GetItem(m_up_star_out_id);
		if (NULL == out_item_base)
		{
			return;
		}
		if (I_TYPE_PARTNER_PACT != out_item_base->GetItemType())
		{
			return;
		}
	}

	//消耗
	if (!this->PactGridTakeOut(put_on_pact_item->GetUpStarCostID(), put_on_pact_item->GetUpStarCostNum(), PUT_REASON_PARTNER_PACT_UP_STAR))
	{
		return;
	}

	ItemID old_put_item_id = now_put_item_id;
	//升星
	now_put_item_id = m_up_star_out_id;

	//log
	gamelog::g_log_partner.printf(LL_INFO,
		"%s %d | uid:%d  name:%s | put_index:%d, old_item_id:%d, new_item_id:%d, reason_type%d",
		__FUNCTION__, __LINE__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		put_index, old_put_item_id,
		now_put_item_id, PUT_REASON_PARTNER_PACT_UP_STAR);

	//重算
	this->ReCalcPartnerAttr(partner_id, true, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_PACT_CHANGE);

	this->SendPactInfo(partner_id, Protocol::SCPartnerPactInfo::UP_START, put_index);
}

void Partner::SendPactGridList()
{
	Protocol::SCPartnerSpiritPactList info;
	info.send_type = Protocol::SCPartnerSpiritPactList::SEND_TYPE_ALL;
	info.count = 0;

	for (int save_index = 0; save_index < PARTNER_SPIRIT_PACT_MAX_GRID; ++save_index)
	{
		PartnerSpiritPactParam::PactGridInfo &grid_info
			= m_pact_grid_param.grid_list[save_index];
		if (0 >= grid_info.item_num)
		{
			continue;
		}

		info.pact_list[info.count].item_id
			= LOGIC_CONFIG->GetPartnerConfig()->GetPactItemID(save_index);
		if (0 == info.pact_list[info.count].item_id)
		{
			continue;
		}

		info.pact_list[info.count].save_index = save_index;
		info.pact_list[info.count].num = grid_info.item_num;
		++info.count;
	}

	int send_len = sizeof(SCPartnerSpiritPactList)
		- (PARTNER_SPIRIT_PACT_MAX_GRID - info.count) * sizeof(SCPartnerSpiritPactList::PactInfo);

	m_role_module_mgr->NetSend((const char *)&info, send_len);
}

void Partner::SendPactGridChange(int save_index, ItemID item_id, int item_num)
{
	Protocol::SCPartnerSpiritPactList info;
	info.send_type = Protocol::SCPartnerSpiritPactList::SEND_TYPE_CHANGE;

	info.count = 1;

	info.pact_list[0].save_index = save_index;
	info.pact_list[0].item_id = item_id;
	info.pact_list[0].num = item_num;

	int send_len = sizeof(SCPartnerSpiritPactList)
		- (PARTNER_SPIRIT_PACT_MAX_GRID - 1) * sizeof(SCPartnerSpiritPactList::PactInfo);

	m_role_module_mgr->NetSend((const char *)&info, send_len);
}

void Partner::SendPactInfo(int partner_id, int type, short param)
{
	if (!IsPartnerAlreadyActive(partner_id))
	{
		return;
	}

	Protocol::SCPartnerPactInfo info;
	info.partner_id = partner_id;
	info.type = type;
	memcpy(info.put_on_pact_id_list,
		m_new_partner.partner_base_info[partner_id].put_on_pact_id_list,
		sizeof(info.put_on_pact_id_list));
	info.param = param;

	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void Partner::SendPactBaseAttr(int partner_id)
{
	if (!IsPartnerAlreadyActive(partner_id))
	{
		return;
	}

	Protocol::SCPartnerPactBaseAttr info;
	info.partner_id = partner_id;
	m_partner_pact_list[partner_id].GetBePactBaseAttr(info.attach_base_attr_list);

	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void Partner::OnReCalcAttr(int partner_id, AttrCon & partner_attr, bool is_notify, char notic_type)
{
	if (!IsPartnerAlreadyActive(partner_id))
	{
		return;
	}

	PartnerPactInfo & pact_info = m_partner_pact_list[partner_id];

	{//重设自身  被契约属性
		Attribute attach_base_attr_list[PARTNER_ATTR_MAX] = {0};
		for (int attr_has = 0; attr_has < PARTNER_ATTR_MAX; attr_has++)
		{
			attach_base_attr_list[attr_has] += partner_attr.attr_list[PARTNER_ATTR_BATTLE_HAS[attr_has]];
		}
		pact_info.SetBePactBase(attach_base_attr_list);
	}

	//清空自身  契约属性
	pact_info.ReSetPact();
	//重算自己 契约属性
	this->ReCalcPactAttr(partner_id, true);
	//传出 用于战力计算
	pact_info.AddNowPactTo(partner_attr);

	//被契约  重算伙伴 契约属性
	if (is_notify)
	{
		this->SendPactBaseAttr(partner_id);

		//bool need_notic = false;
		const std::set<int> & be_pact_set = pact_info.GetBePactSet();
		for (std::set<int>::const_iterator be_pact_it = be_pact_set.begin();
			be_pact_it != be_pact_set.end();
			be_pact_it++)
		{
			if (!IsPartnerAlreadyActive(*be_pact_it))
			{
				continue;
			}

			this->ReCalcPactAttr(*be_pact_it, is_notify, is_notify);
			//need_notic = true;
		}
		//if (need_notic 
		//	&& Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_PACT_CHANGE != notic_type)
		//{
		//	m_role_module_mgr->NoticeNum(errornum::EN_PARTNER_BE_PACT_UP);
		//}

	}
}

void Partner::ReCalcPactAttr(int partner_id, bool calc_cap /*= false*/, bool is_notify/* = false*/)
{
	if (!IsPartnerAlreadyActive(partner_id))
	{
		return;
	}

	PartnerPactInfo & pact_info = m_partner_pact_list[partner_id];
	Attribute pact_attr_list[PARTNER_ATTR_MAX] = { 0 };

	//获得契约属性
	for (int pact_index = 0;
		pact_index < PARTNER_SPIRIT_PACT_MAX;
		pact_index++)
	{
		const ItemBase * pact_item_base =
			ITEMPOOL->GetItem(m_new_partner.partner_base_info[partner_id].put_on_pact_id_list[pact_index]);
		if (NULL == pact_item_base)
		{
			continue;
		}
		if (I_TYPE_PARTNER_PACT != pact_item_base->GetItemType())
		{
			continue;
		}
		const SpiritPactItem *put_on_pact_item = (SpiritPactItem *)pact_item_base;

		PartnerPactInfo & now_pact_info = m_partner_pact_list[put_on_pact_item->GetPartnerID()];

		now_pact_info.AddBePactTo(pact_attr_list, put_on_pact_item->GetAttrUpPerMil());

		pact_info.PactAdd(put_on_pact_item->GetPartnerID());
		now_pact_info.BePactAdd(partner_id);
	}

	int old_add_cap = 0;
	int new_add_cap = 0;
	static AttrCon temp_attr;

	if (calc_cap)
	{//重算战力  存旧属性增加
		temp_attr.Reset();

		const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
		if (NULL == basic_cfg) return;
		 
		pact_info.GetNowPactAttr(temp_attr);

		this->CalcPartnerCapability(temp_attr, basic_cfg->profession);
		old_add_cap = temp_attr.capability;
	}

	//附加到总属性上
	pact_info.ReCalcPactAttr(m_partner_attr_list[partner_id], pact_attr_list);

	if (calc_cap)
	{//重算战力  减旧加新
		temp_attr.Reset();
		const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
		if (NULL == basic_cfg) return;

		pact_info.GetNowPactAttr(temp_attr);

		this->CalcPartnerCapability(temp_attr, basic_cfg->profession);
		new_add_cap = temp_attr.capability;

		m_partner_attr_list[partner_id].capability += (new_add_cap - old_add_cap);
		m_new_partner.partner_base_info[partner_id].capability = m_partner_attr_list[partner_id].capability;

		if (is_notify && old_add_cap != new_add_cap)
		{
			AttrCon &self_attr_con = m_partner_attr_list[partner_id];

			std::vector<BattleAttrPair> attr_type_list;
			for (int t_cur = 0; t_cur < PARTNER_ATTR_MAX; ++t_cur)
			{
				attr_type_list.push_back(BattleAttrPair(PARTNER_ATTR_BATTLE_HAS[t_cur],
					self_attr_con.attr_list[PARTNER_ATTR_BATTLE_HAS[t_cur]]));
			}

			//血量
			if (m_role_module_mgr->GetRole()->GetSceneStatus()->IsInAutoRecoverScene())
			{
				PartnerSql& p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
				p_sql.max_hp = self_attr_con.attr_list[BATTLE_ATTR_MAX_HP];
				p_sql.max_mp = self_attr_con.attr_list[BATTLE_ATTR_MAX_MP];

				p_sql.cur_hp = p_sql.max_hp;
				p_sql.cur_mp = p_sql.max_mp;

				self_attr_con.attr_list[BATTLE_ATTR_CUR_HP] = p_sql.cur_hp;
				self_attr_con.attr_list[BATTLE_ATTR_CUR_MP] = p_sql.cur_mp;

				attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_HP, p_sql.cur_hp));
				attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_MP, p_sql.cur_mp));
			}
			else
			{
				PartnerSql& p_sql = m_new_partner.partner_base_info[partner_id].partner_data;
				p_sql.max_hp = self_attr_con.attr_list[BATTLE_ATTR_MAX_HP];
				p_sql.max_mp = self_attr_con.attr_list[BATTLE_ATTR_MAX_MP];

				p_sql.cur_hp = GetMin(p_sql.cur_hp, self_attr_con.attr_list[BATTLE_ATTR_MAX_HP]);
				p_sql.cur_hp = GetMax(1, p_sql.cur_hp);

				p_sql.cur_mp = GetMin(p_sql.cur_mp, self_attr_con.attr_list[BATTLE_ATTR_MAX_MP]);
				p_sql.cur_mp = GetMax(0, p_sql.cur_mp);

				self_attr_con.attr_list[BATTLE_ATTR_CUR_HP] = p_sql.cur_hp;
				self_attr_con.attr_list[BATTLE_ATTR_CUR_MP] = p_sql.cur_mp;

				attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_HP, p_sql.cur_hp));
				attr_type_list.push_back(BattleAttrPair(BATTLE_ATTR_CUR_MP, p_sql.cur_mp));
			}

			this->SendPartnerAttrChange(partner_id, attr_type_list, Protocol::SCPartnerAttrChangeRep::NOTIC_TYPE_PACT);
		}
	}

	// 重算契约信息之后再算一次,否则好像算不到契约的信息
	RankManager::Instance().SyncPartnerRankInfo(this->GetRoleModuleManager()->GetRole(), PARTNER_RANK_TYPE_CAPABILITY, partner_id);
}

PartnerPactInfo::PartnerPactInfo()
{
	memset(m_now_pact_attr_list, 0, sizeof(m_now_pact_attr_list));
	memset(m_be_pact_base_attr_list, 0, sizeof(m_be_pact_base_attr_list));
}

PartnerPactInfo::~PartnerPactInfo()
{
}

void PartnerPactInfo::ReCalcPactAttr(AttrCon &total_attr, Attribute attach_attr_list[PARTNER_ATTR_MAX])
{
	this->PactAttrOff(total_attr);

	//储存新的附加
	memcpy(m_now_pact_attr_list, attach_attr_list, sizeof(m_now_pact_attr_list));

	//附加
	for (int attr_has = 0; attr_has < PARTNER_ATTR_MAX; attr_has++)
	{
		total_attr.attr_list[PARTNER_ATTR_BATTLE_HAS[attr_has]] += m_now_pact_attr_list[attr_has];
	}
}

void PartnerPactInfo::AddNowPactTo(AttrCon & total_attr)
{
	for (int attr_has = 0; attr_has < PARTNER_ATTR_MAX; attr_has++)
	{
		total_attr.attr_list[PARTNER_ATTR_BATTLE_HAS[attr_has]] += m_now_pact_attr_list[attr_has];
	}
}

void PartnerPactInfo::ReSetPact()
{
	memset(m_now_pact_attr_list, 0, sizeof(m_now_pact_attr_list));
}

void PartnerPactInfo::PactAttrOff(AttrCon &total_attr)
{
	//减去旧的
	for (int attr_has = 0; attr_has < PARTNER_ATTR_MAX; attr_has++)
	{
		total_attr.attr_list[PARTNER_ATTR_BATTLE_HAS[attr_has]] -= m_now_pact_attr_list[attr_has];
	}

	//清空
	memset(m_now_pact_attr_list, 0, sizeof(m_now_pact_attr_list));
}

void PartnerPactInfo::SetBePactBase(Attribute base_attr_list[PARTNER_ATTR_MAX])
{
	memcpy(m_be_pact_base_attr_list, base_attr_list, sizeof(m_be_pact_base_attr_list));
}

void PartnerPactInfo::AddBePactTo(Attribute attach_attr_list[PARTNER_ATTR_MAX], int attr_per_mil)
{
	for (int attr_has = 0; attr_has < PARTNER_ATTR_MAX; attr_has++)
	{
		attach_attr_list[attr_has] += (Attribute)(1.0 * m_be_pact_base_attr_list[attr_has]  / PER_MIL_BASE * attr_per_mil);
	}
}

bool PartnerPactInfo::IsPact(int partner_id)
{
	if (m_now_pact_partner_id_set.find(partner_id)
		!= m_now_pact_partner_id_set.end())
	{
		return true;
	}

	return false;
}

bool PartnerPactInfo::PactAdd(int partner_id)
{
	if (m_now_pact_partner_id_set.find(partner_id)
		!= m_now_pact_partner_id_set.end())
	{
		return false;
	}

	m_now_pact_partner_id_set.insert(partner_id);
	return true;
}

void PartnerPactInfo::PactOff(int partner_id)
{
	m_now_pact_partner_id_set.erase(partner_id);
}

void PartnerPactInfo::BePactAdd(int partner_id)
{
	m_be_pact_partner_id_set.insert(partner_id);
}

void PartnerPactInfo::BePactOff(int partner_id)
{
	m_be_pact_partner_id_set.erase(partner_id);
}

void PartnerPactInfo::GetNowPactAttr(AttrCon & now_pact_attr)
{
	for (int attr_has = 0; attr_has < PARTNER_ATTR_MAX; attr_has++)
	{
		now_pact_attr.attr_list[PARTNER_ATTR_BATTLE_HAS[attr_has]] = m_now_pact_attr_list[attr_has];
	}
}

void PartnerPactInfo::GetNowPactAttr(AttributeList & now_pact_attr)
{
	for (int attr_has = 0; attr_has < PARTNER_ATTR_MAX; attr_has++)
	{
		now_pact_attr.m_attrs[PARTNER_ATTR_BATTLE_HAS[attr_has]] += m_now_pact_attr_list[attr_has];
	}
}

void PartnerPactInfo::GetBePactBaseAttr(Attribute base_attr_list[PARTNER_ATTR_MAX])
{
	memcpy(base_attr_list, m_be_pact_base_attr_list, sizeof(m_be_pact_base_attr_list));
}
