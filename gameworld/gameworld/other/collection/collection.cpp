#include "collection.hpp"
#include "collectionconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "gamelog.h"
#include "obj/character/role.h"
#include "protocol/msgcollection.h"
#include "monster/monsterpool.hpp"
#include "item/itempool.h"
#include "config/otherconfig/sundryconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "servercommon/errornum.h"
#include "other/route/mailroute.hpp"
#include "task/taskmanager.h"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "other/roleheartskill/roleheartskill.hpp"
#include "other/shengqi/holyequipmanager.hpp"

Collection::Collection()
{
}

Collection::~Collection()
{
}

void Collection::Init(RoleModuleManager * role_module_manager, const CollectionParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;
}

void Collection::GetOtherInitParam(CollectionParam * param)
{
	*param = m_param;
}

const AttributeList & Collection::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		int max_collection_num = LOGIC_CONFIG->GetCollectionConfig()->GetMaxSeq();
		for (int i = 0; i <= max_collection_num && i < COLLECTION_MAX_NUM; i++)
		{
			if (!m_param.collection_activate_flag.IsBitSet(i)) continue;
			const CollectionCfg *cfg = LOGIC_CONFIG->GetCollectionConfig()->GetCollectionCfg(i);
			if (NULL == cfg) continue;

			m_attrs_add.m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(cfg->maxhp);
			m_attrs_add.m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(cfg->maxmp);
			m_attrs_add.m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(cfg->gongji);
			m_attrs_add.m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(cfg->fangyu);
			m_attrs_add.m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(cfg->minjie);
			m_attrs_add.m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(cfg->jingshen);
			m_attrs_add.m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(cfg->hueifu);
		}
		int max_included_num = LOGIC_CONFIG->GetCollectionConfig()->GetIncludedMaxSeq();
		for (int i = 0; i <= max_included_num && i < COLLECTION_MAX_NUM; i++)
		{
			int id = i / INT_MAX_BIT_NUM;
			int index = i % INT_MAX_BIT_NUM;
			if ((m_param.collection_included_activate_flag[id] & (1 << index)) == 0) continue;
			const CollectionActivationCfg *cfg = LOGIC_CONFIG->GetCollectionConfig()->GetPetCollectBySeq(i);
			if (NULL == cfg) continue;

			m_attrs_add.m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(cfg->maxhp);
			m_attrs_add.m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(cfg->maxmp);
			m_attrs_add.m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(cfg->gongji);
			m_attrs_add.m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(cfg->fangyu);
			m_attrs_add.m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(cfg->minjie);
			m_attrs_add.m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(cfg->jingshen);
			m_attrs_add.m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(cfg->huifu);
			m_attrs_add.m_attrs[BATTLE_ATTR_CRITICAL] += static_cast<Attribute>(cfg->bisha);
			m_attrs_add.m_attrs[BATTLE_ATTR_CRITICAL_DEC_VALUE] += static_cast<Attribute>(cfg->jianren);
			m_attrs_add.m_attrs[BATTLE_ATTR_COUNTER_ATTACK] += static_cast<Attribute>(cfg->fanji);
			m_attrs_add.m_attrs[BATTLE_ATTR_HIT] += static_cast<Attribute>(cfg->mingzhong);
			m_attrs_add.m_attrs[BATTLE_ATTR_DODGE] += static_cast<Attribute>(cfg->shanduo);
			m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_POISON] += static_cast<Attribute>(cfg->def_zhongdu);
			m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_PETRIFY] += static_cast<Attribute>(cfg->def_shihua);
			m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_FREEZE] += static_cast<Attribute>(cfg->def_hunshui);
			m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_DRUNK] += static_cast<Attribute>(cfg->def_jiuzui);
			m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_CHAOS] += static_cast<Attribute>(cfg->def_hunluan);
			m_attrs_add.m_attrs[BATTLE_ATTR_MAGIC_ATTACK] += static_cast<Attribute>(cfg->mogong);
			m_attrs_add.m_attrs[BATTLE_ATTR_MAGIC_DEFENSE] += static_cast<Attribute>(cfg->mokang);
			m_attrs_add.m_attrs[BATTLE_ATTR_CRITICAL_INC_VALUE] += static_cast<Attribute>(cfg->bisha_dmg);
		}

		for (int i = 0; i < MAX_ATLAS_CARD_NUM; i++)
		{
			unsigned short & node_level = m_param.new_card_level[i];
			const CollectionTreasuresCfg * treasures_cfg = LOGIC_CONFIG->GetCollectionConfig()->GetTreasuresCfg(i, node_level);
			if (NULL != treasures_cfg)
			{
				if (treasures_cfg->attr_type_1 >= 0 && treasures_cfg->attr_type_1 < BATTLE_ATTR_MAX)m_attrs_add.m_attrs[treasures_cfg->attr_type_1] += treasures_cfg->attr_value_1;
				if (treasures_cfg->attr_type_2 >= 0 && treasures_cfg->attr_type_2 < BATTLE_ATTR_MAX)m_attrs_add.m_attrs[treasures_cfg->attr_type_2] += treasures_cfg->attr_value_2;
				if (treasures_cfg->attr_type_3 >= 0 && treasures_cfg->attr_type_3 < BATTLE_ATTR_MAX)m_attrs_add.m_attrs[treasures_cfg->attr_type_3] += treasures_cfg->attr_value_3;
				if (treasures_cfg->attr_type_4 >= 0 && treasures_cfg->attr_type_4 < BATTLE_ATTR_MAX)m_attrs_add.m_attrs[treasures_cfg->attr_type_4] += treasures_cfg->attr_value_4;
			}
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void Collection::OnChangeProfession(int old_prof, int new_prof)
{
	int max_collection_num = LOGIC_CONFIG->GetCollectionConfig()->GetMaxSeq();
	for (int i = 0; i <= max_collection_num && i < COLLECTION_MAX_NUM; i++)
	{
		if (!m_param.collection_activate_flag.IsBitSet(i)) continue;
		const CollectionCfg *cfg = LOGIC_CONFIG->GetCollectionConfig()->GetCollectionCfg(i);
		if (NULL == cfg)continue;
		if (0 == cfg->limit_prof) continue;
		// 如果不是同职业的就清掉
		if (old_prof != new_prof && (old_prof/100 != new_prof/100))
		{
			m_param.collection_activate_flag.UnSetBit(i);
			// 如果返还消耗就发个邮箱
			if (cfg->is_back_card)
			{
				static MailContentParam contentparam; contentparam.Reset();
				int real_count = 0;
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_mail_collection_content, i);
				int length2 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_mail_collection_main_content);
				for (int i = 0; i < cfg->fee_num && i < COLLECTION__FEE_MAX_NUM; i++)
				{
					const ItemBase * item_base = ITEMPOOL->GetItem(cfg->collection_itemfee_id[i]);
					if (NULL != item_base)
					{
						if (real_count >= MAX_ATTACHMENT_ITEM_NUM)//累积超过邮件上限，先行发放一次
						{
							if (length > 0)
							{
								MailRoute::MailToUser(IntToUid(m_role_module_mgr->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
							}
							memset(contentparam.item_list, 0, sizeof(contentparam.item_list));
							real_count = 0;
						}
						contentparam.item_list[real_count].item_id = cfg->collection_itemfee_id[i];
						contentparam.item_list[real_count].num = cfg->collection_itemfee_num[i];
						contentparam.item_list[real_count].is_bind = 1;
						contentparam.item_list[real_count].invalid_time = item_base->CalInvalidTime();
						real_count += 1;
					}
				}
				if (real_count > 0 && length > 0 && length2 > 0)
				{
					MailRoute::MailToUser(IntToUid(m_role_module_mgr->GetRole()->GetUID()), MAIL_REASON_DEFAULT, contentparam);
				}

			}
		}
	}
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_COLLECTION, __FUNCTION__);
	this->SendAllInfo();
}

void Collection::OnRoleLogin()
{
	switch (m_param.version_id)
	{//注 一般情况 不加 breck
	case 0:
	case 1:
	case 2:
		{
			m_param.version_id = 3;

			//遍历宠物 宠物仓库 魂器背包 激活当前拥有的魂器
			m_role_module_mgr->GetPet()->CheckPetSoulCollection();
			m_role_module_mgr->GetKnapsack()->CheckPetSoulCollection();

			//检测伙伴
			m_role_module_mgr->GetPartner()->CheckPartnerCollection();

			//检测心法
			m_role_module_mgr->GetRoleHeartSkill()->CheckHeartSkillCollection();

			//检测宠物
			m_role_module_mgr->GetPet()->CheckPetCollection();

			//检测圣器
			m_role_module_mgr->GetHolyEquipManager()->CheckHolyEquipCollection();
		}
	default:
		break;
	}

	this->SendIncludedAllInfo();
	this->SendAtlasCardInfo();
}

void Collection::OnActivateCollection(int seq)
{
	if (seq < 0 || seq >= COLLECTION_MAX_NUM)return;
	if (m_param.collection_activate_flag.IsBitSet(seq))return;
	const CollectionCfg *cfg = LOGIC_CONFIG->GetCollectionConfig()->GetCollectionCfg(seq);
	if (NULL == cfg)return;

	/////////////////////判断////////////////////////////
	// 如果有职业限制
	if (cfg->limit_prof != 0 && cfg->limit_prof != m_role_module_mgr->GetRole()->GetProfession() / PROFESSION_BASE) return;
	// 如果道具不够
	for (int i = 0; i < cfg->fee_num && i < COLLECTION__FEE_MAX_NUM; i++)
	{
		if (m_role_module_mgr->GetKnapsack()->Count(cfg->collection_itemfee_id[i], false) < cfg->collection_itemfee_num[i])
		{
			return;
		}
	}
	/////////////////////消费////////////////////////////
	for (int i = 0; i < cfg->fee_num && i < COLLECTION__FEE_MAX_NUM; i++)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->collection_itemfee_id[i], cfg->collection_itemfee_num[i], __FUNCTION__))
		{
			gamelog::g_log_collection.printf(LL_INFO, "%s user[%d %s] consume error item_id[%d] item_num[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
											 cfg->collection_itemfee_id[i], cfg->collection_itemfee_num[i]);
		}
	}
	/////////////////////激活////////////////////////////
	m_param.collection_activate_flag.SetBit(seq);
	ROLE_LOG_QUICK6(LOG_TYPE_COLLECTION, m_role_module_mgr->GetRole(), seq, 0, __FUNCTION__, "ILLUSTRATED");
	EventHandler::Instance().OnActivateCollection(m_role_module_mgr->GetRole(), cfg->seq, cfg->page, cfg->color);
	/////////////////////////////////////////////////////
	this->SendSingleInfo(seq);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_COLLECTION, __FUNCTION__);

	m_role_module_mgr->GetTaskManager()->OnCollectionTuJianActive();
}

void Collection::OnConvert(int seq)
{
	const CollectionConvertCfg *cfg = LOGIC_CONFIG->GetCollectionConfig()->GetConvertCfg(seq);
	if (NULL == cfg)return;
	/////////////////////消费////////////////////////////
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->convert_itemfee, cfg->convert_itemnum, __FUNCTION__, KNAPSACK_CONSUME_TYPE_CANNOT_BUY, true))
	{
		return;
	}
	/////////////////////给予////////////////////////////
	m_role_module_mgr->GetKnapsack()->Put(cfg->get_item, PUT_REASON_COLLECTION_ONCONVERT);
	/////////////////////////////////////////////////////

}

int Collection::GetActiveNumByColor(int color)
{
	int active_count = 0;
	for (int i = 0; i < COLLECTION_MAX_NUM; i++)
	{
		const CollectionCfg *cfg = LOGIC_CONFIG->GetCollectionConfig()->GetCollectionCfg(i);
		if (NULL == cfg) break;	//配置读取有检测顺序,当获取不到时,则后续没有配置了

		if(!m_param.collection_activate_flag.IsBitSet(i)) continue;		
		if(0 != color && cfg->color != color) continue;

		active_count += 1;
	}

	return active_count;
}

void Collection::OnPetCollection(int pet_id)
{
	const CollectionActivationCfg * cfg = LOGIC_CONFIG->GetCollectionConfig()->GetPetCollectByPetId(pet_id);
	if (cfg == NULL || cfg->page != CollectionConfig::COLLECTION_TYPE_PET)return;
	int id = cfg->seq / INT_MAX_BIT_NUM;
	int index = cfg->seq % INT_MAX_BIT_NUM;
	if ((m_param.collection_included_ready_flag[id] & ((int)1<<index) ) > 0)return;
	m_param.collection_included_ready_flag[id] |= ((int)1 << index);
	this->SendSignCollection(cfg->seq);
}

void Collection::OnPartnerCollection(int partner_id)
{
	const CollectionActivationCfg * cfg = LOGIC_CONFIG->GetCollectionConfig()->GetPartnerCollectByPartnerId(partner_id);
	if (cfg == NULL || cfg->page != CollectionConfig::COLLECTION_TYPE_PARTNER)return;
	int id = cfg->seq / INT_MAX_BIT_NUM;
	int index = cfg->seq % INT_MAX_BIT_NUM;
	if ((m_param.collection_included_ready_flag[id] & ((int)1 << index)) > 0)return;
	m_param.collection_included_ready_flag[id] |= ((int)1 << index);
	this->SendSignCollection(cfg->seq);
}

void Collection::OnFaBaoCollection(int fabao_id)
{
	const CollectionActivationCfg * cfg = LOGIC_CONFIG->GetCollectionConfig()->GetFabaoCollectByFabaoId(fabao_id);
	if (cfg == NULL || cfg->page != CollectionConfig::COLLECTION_TYPE_FABAO)return;
	int id = cfg->seq / INT_MAX_BIT_NUM;
	int index = cfg->seq % INT_MAX_BIT_NUM;
	if ((m_param.collection_included_ready_flag[id] & ((int)1 << index)) > 0)return;
	m_param.collection_included_ready_flag[id] |= ((int)1 << index);
	this->SendSignCollection(cfg->seq);
}

void Collection::OnPetSoulCollection(int pet_soul_item_id, bool need_sync)
{
	const CollectionActivationCfg * cfg = LOGIC_CONFIG->GetCollectionConfig()->GetPetSoulCollectByPetSoulId(pet_soul_item_id);
	if (cfg == NULL || cfg->page != CollectionConfig::COLLECTION_TYPE_PET_SOUL)
	{
		return;
	}

	int id = cfg->seq / INT_MAX_BIT_NUM;
	int index = cfg->seq % INT_MAX_BIT_NUM;
	if ((m_param.collection_included_ready_flag[id] & ((int)1 << index)) > 0)
	{
		return;
	}
	m_param.collection_included_ready_flag[id] |= ((int)1 << index);

	if (need_sync)
	{
		this->SendSignCollection(cfg->seq);
	}
}

void Collection::OnHolyEquipCollection(int shengqi_id)
{
	const CollectionActivationCfg * cfg = LOGIC_CONFIG->GetCollectionConfig()->GetHolyEquipCollectByHolyEquipId(shengqi_id);
	if (cfg == NULL || cfg->page != CollectionConfig::COLLECTION_TYPE_HOLY_EQUIP)return;
	int id = cfg->seq / INT_MAX_BIT_NUM;
	int index = cfg->seq % INT_MAX_BIT_NUM;
	if ((m_param.collection_included_ready_flag[id] & ((int)1 << index)) > 0)return;
	m_param.collection_included_ready_flag[id] |= ((int)1 << index);
	this->SendSignCollection(cfg->seq);
}

void Collection::OnReadyCollection(int type, int active_id)
{
	const CollectionActivationCfg * cfg = LOGIC_CONFIG->GetCollectionConfig()->GetCollectionActivationCfg(type, active_id);
	if(NULL == cfg || cfg->page != type) return;

	int id = cfg->seq / INT_MAX_BIT_NUM;
	int index = cfg->seq % INT_MAX_BIT_NUM;
	if ((m_param.collection_included_ready_flag[id] & ((int)1 << index)) > 0)return;
	m_param.collection_included_ready_flag[id] |= ((int)1 << index);
	this->SendSignCollection(cfg->seq);
}

void Collection::ActiveCollection(int seq)
{
	if (seq < 0 || seq >= COLLECTION_MAX_NUM)return;

	int id = seq / INT_MAX_BIT_NUM;
	int index = seq % INT_MAX_BIT_NUM;
	if ((m_param.collection_included_ready_flag[id] & ((int)1 << index)) == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COLLECTION_INCLUDED_FAIL);
		return;
	}

	if ((m_param.collection_included_activate_flag[id] & ((int)1 << index)) == 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COLLECTION_INCLUDED_AGAIN);
		return;
	}
	m_param.collection_included_activate_flag[id] |= ((int)1 << index);
	ROLE_LOG_QUICK6(LOG_TYPE_COLLECTION, m_role_module_mgr->GetRole(), seq, 0, __FUNCTION__, "INCLUDED");
	this->SendSignCollection(seq);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_COLLECTION, __FUNCTION__);
}

bool Collection::IsActiveCollection(int seq)
{
	if (seq < 0 || seq >= COLLECTION_MAX_NUM) return false;
	int id = seq / INT_MAX_BIT_NUM;
	int index = seq % INT_MAX_BIT_NUM;
	if ((m_param.collection_included_ready_flag[id] & ((int)1 << index)) == 0)
	{
		return false;
	}
	return true;
}

void Collection::SendIncludedAllInfo()
{
	Protocol::SCCollectionIncluded info;
	memcpy(info.collection_included_ready_flag, m_param.collection_included_ready_flag, sizeof(info.collection_included_ready_flag));
	memcpy(info.collection_included_activate_flag, m_param.collection_included_activate_flag, sizeof(info.collection_included_activate_flag));
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Collection::SendSignCollection(int seq)
{
	if (seq < 0 || seq >= COLLECTION_MAX_NUM)return;
	int id = seq / INT_MAX_BIT_NUM;
	int index = seq % INT_MAX_BIT_NUM;
	Protocol::SCCollectionIncludedSignle msg;
	msg.seq = (unsigned short)seq;
	msg.ready_flag = (m_param.collection_included_ready_flag[id] & (1 << index)) > 0 ? 1 : 0;
	msg.activate_flag = (m_param.collection_included_activate_flag[id] & (1 << index)) > 0 ? 1 : 0;
	m_role_module_mgr->NetSend((const char*)&msg, sizeof(msg));
}

void Collection::SendAllInfo()
{
	Protocol::SCCollectionAllInfo info;
	for (int i = 0; i < COLLECTION_MAX_NUM; i++)
	{
		info.collection_activate_flag[i] = m_param.collection_activate_flag.IsBitSet(i) ? 1 : 0;
	}
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Collection::SendSingleInfo(int seq)
{
	if (seq < 0 || seq >= COLLECTION_MAX_NUM) return;

	Protocol::SCCollectionSingleInfo info;
	info.seq = seq;
	info.is_activate = m_param.collection_activate_flag.IsBitSet(seq) ? 1 : 0;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Collection::OnKillMonsterDropCard(int monster_num, int* monster_id_list)
{
	int max_drop_card_num = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().drop_card_max_num;
	int total_drop_num = 0;
	for (int kind = 0; kind < monster_num; ++kind)
	{
		int monster_id = monster_id_list[kind];
		total_drop_num += this->DropCard(monster_id, 1, max_drop_card_num - total_drop_num);
		
		if (total_drop_num >= max_drop_card_num) break;
	}
}

int Collection::GetCollectionActiveTuJianNum()
{
	int num = 0;

	int max_collection_num = LOGIC_CONFIG->GetCollectionConfig()->GetMaxSeq();
	for (int i = 0; i <= max_collection_num && i < COLLECTION_MAX_NUM; i++)
	{
		if (m_param.collection_activate_flag.IsBitSet(i))
		{
			num += 1;
		}
	}

	return num;
}

void Collection::UpLevelAtlasCard(int seq)
{
	if (seq < 0 || seq >= MAX_ATLAS_CARD_NUM)return;

	unsigned short & node_level = m_param.new_card_level[seq];

	const CollectionTreasuresCfg * treasures_cfg = LOGIC_CONFIG->GetCollectionConfig()->GetTreasuresCfg(seq, node_level + 1);

	if (NULL == treasures_cfg)return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(treasures_cfg->id, treasures_cfg->consume_quantity, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_ITEM);
		return;
	}

	node_level += 1;
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_COLLECTION, __FUNCTION__);
	this->SendAtlasCardSingleInfo(seq);
}

void Collection::SendAtlasCardInfo()
{
	Protocol::SCCollectionAtlasCardInfo info;
	for (int i = 0; i < MAX_ATLAS_CARD_NUM; i++)
	{
		info.new_card_level[i] = m_param.new_card_level[i];
	}
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Collection::SendAtlasCardSingleInfo(int seq)
{
	if (seq < 0 || seq >= MAX_ATLAS_CARD_NUM)return;
	Protocol::SCCollectionAtlasCardSignleInfo info;

	info.seq = seq;
	info.level = m_param.new_card_level[seq];

	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

int Collection::DropCard(int monster_id, int monster_num, int max_drop_num)
{
	const MonsterCfg* cfg = MonsterPool::GetInstance().GetMonsterCfg(monster_id);
	if (NULL == cfg) return 0;

	const ItemBase* item_base = ITEMPOOL->GetItem(cfg->m_drop_card_id);
	if (NULL == item_base) return 0;

	int total_card_num = 0;
	for (int i = 0; i < monster_num && total_card_num < max_drop_num; ++i)
	{
		if (cfg->IsDropCard())
		{
			total_card_num += 1;

			break; // 每只怪最多掉一张
		}
	}

	if (total_card_num > 0)
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->m_drop_card_id, total_card_num))
		{
			static ItemConfigData item_add;
			item_add.item_id = cfg->m_drop_card_id;
			item_add.num = total_card_num;
			item_add.is_bind = false;
			m_role_module_mgr->GetKnapsack()->Put(item_add, PUT_REASON_DROP);
		}
	}

	return total_card_num;
}
