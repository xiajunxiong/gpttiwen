#include "workshop.hpp"
#include "workshopconfig.hpp"
#include "gemstoneconfig.hpp"
#include "jewelryconfig.hpp"
#include "gamelog.h"
#include "engineadapter.h"
#include "protocol/msgworkshop.h"
#include "protocol/msgjewelry.h"
#include "protocol/msgitem.h"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/skill/roleskill.hpp"
#include "other/shop/shopconfig.h"
#include "other/roleactivity/roleactivity.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"
#include "item/itempool.h"
#include "item/itemextern.hpp"
#include "equipment/equipmentmanager.h"
#include "skill/skillpool.hpp"
#include "gameworld/config/otherconfig/entryconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "gameworld/world.h"
#include "servercommon/logdef.h"
#include "equipment/petequipment.h"
#include "other/workshop/petequipmentconfig.hpp"
#include "other/pet/pet.hpp"
#include "servercommon/noticenum.h"
#include "chat/chatmanager.h"

#include "other/huashen/huashenmanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "other/workshop/suitlistconfig.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "other/route/mailroute.hpp"
#include "item/dianhuashiitem/dianhuashiitem.hpp"
#include "other/dianhuashi/dianhuashiconfig.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "other/equipstar/equipstarconfig.hpp"

Workshop::Workshop()
{
}

Workshop::~Workshop()
{
}

void Workshop::Init(RoleModuleManager * role_module_manager, const WorkShopParam & p_workshop_param)
{
	m_role_module_mgr = role_module_manager;
	m_workshop_param = p_workshop_param;
}

void Workshop::GetWorkshopParam(WorkShopParam * p_workshop_param)
{
	*p_workshop_param = m_workshop_param;
}

void Workshop::NormalUltimate(int equip_type, int equip_posi, int equip_grid, int role_type)
{
	return;//精工屏蔽
}

void Workshop::StrongUltimate(int equip_type, int equip_posi, int equip_grid, int role_type)
{
	return;//精工屏蔽
}

void Workshop::EquipUpStar(int seq, int equip_type, int equip_posi, int equip_grid, int cost_grid, int cost_grid_2)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->EquipUpStar(seq, equip_type, equip_grid, cost_grid, cost_grid_2);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->EquipUpStar(seq, equip_type, equip_grid, cost_grid, cost_grid_2);
	}
}

void Workshop::UpEquipGrade(int equip_type, int equip_posi, int equip_grid, int role_type)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->UpEquipGrade(equip_type, equip_grid, role_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->UpEquipGrade(equip_type, equip_grid, role_type);
	}
}

void Workshop::BreakThroughGarde(int equip_type, int equip_posi, int equip_grid, int role_type)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->BreakThroughGarde(equip_type, equip_grid, role_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->BreakThroughGarde(equip_type, equip_grid, role_type);
	}
}

void Workshop::CheckTurnBack()
{
	if (m_role_module_mgr->GetCommonData().god_turn_bag_flag == 0)
	{
		m_role_module_mgr->GetCommonData().god_turn_bag_flag = 1;
		const GodEquipTurnBackListCfg * turn_back_cfg = GODEQUIP_CONFIG->GetTurnBackUserId(m_role_module_mgr->GetUid());
		if (NULL != turn_back_cfg)
		{
			m_role_module_mgr->GetKnapsack()->ReturnGodEquip();
			m_role_module_mgr->GetEquipmentManager()->ReturnGodEquip();
		}
	}

	if (m_role_module_mgr->GetCommonData().god_turn_bag_flag == 1)
	{
		m_role_module_mgr->GetCommonData().god_turn_bag_flag = 2;
		const GodEquipTurnBackListCfg * turn_back_cfg = GODEQUIP_CONFIG->GetTurnBackUserId2(m_role_module_mgr->GetUid());
		if (NULL != turn_back_cfg)
		{
			m_role_module_mgr->GetKnapsack()->ReturnGodEquip2();
			m_role_module_mgr->GetEquipmentManager()->ReturnGodEquip2();
		}
	}
}

void Workshop::DianHuaEquip(int equip_posi, int equip_grid, int dianhuashi_coloumn, int dianhuashi_grid)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->DianHuaEquip(equip_grid, dianhuashi_coloumn, dianhuashi_grid);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->DianHuaEquip(equip_grid, dianhuashi_coloumn, dianhuashi_grid);
	}
}

void Workshop::SaveDianHuaEquip(int equip_posi, int equip_grid)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->SaveDianHuaEquip(equip_grid);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SaveDianHuaEquip(equip_grid);
	}
}

void Workshop::DisComposeDianHuaShi(int dianhuashi_coloumn, int dianhuashi_grid)
{
	ItemGridData equip_grid;
	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(dianhuashi_coloumn, dianhuashi_grid, &equip_grid))
	{
		return;
	}
	const ItemBase * ib = ITEMPOOL->GetItem(equip_grid.item_id);
	if (NULL == ib || ib->GetItemType() != I_TYPE_DIAN_HUA_SHI)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	DianHuaShiParam &dianhuashi_param = equip_grid.param->dian_hua_shi_param;
	const DianHuaSpecialEffectDetailCfg * skill_detail_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetEffectDetailCfg(dianhuashi_param.effect_id, dianhuashi_param.effect_level);
	if (NULL != skill_detail_cfg)
	{
		if ((short)skill_detail_cfg->resolve_vec.size() > 0 && m_role_module_mgr->GetKnapsack()->PutList((short)skill_detail_cfg->resolve_vec.size(), &skill_detail_cfg->resolve_vec[0], PUT_REASON_DIAN_HUA_SHI))
		{
			//消耗
			if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(dianhuashi_coloumn, dianhuashi_grid, 1, __FUNCTION__))
			{
				gamelog::g_log_workshop.printf(LL_INFO, "%s err! no consume item_id[%d] param[%s] user[%d,%s]", __FUNCTION__, equip_grid.item_id, equip_grid.param->GetParamLog(I_TYPE_DIAN_HUA_SHI), m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName());
				return;
			}
		}
	}
}

void Workshop::GMRandEnd(int equip_posi, int equip_grid, int level)
{
	for (int i = 0; i < 500; i++)//最高500次循环
	{
		const DianHuaShiOtherCfg & other_cfg = LOGIC_CONFIG->GetDianHuaShiConfig()->GetOtherCfg();
		if (!m_role_module_mgr->GetKnapsack()->HasItem(other_cfg.fumo_id, 1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
		int new_spid = 0;
		int new_level = 0;
		this->FuMoEquip(equip_posi, equip_grid, &new_spid, &new_level);
		const ItemGridData * tmp_equip = NULL;
		if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
		{
			tmp_equip = m_role_module_mgr->GetEquipmentManager()->GetEquipListType(equip_grid);
		}
		else
		{
			tmp_equip = m_role_module_mgr->GetKnapsack()->GetItemGridData2(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid);
		}

		if (NULL == tmp_equip || NULL == tmp_equip->param) continue;

		EquipmentParam & equip_param = tmp_equip->param->equip_param;
		if (new_level > equip_param.effect_level)
		{
			this->SaveDianHuaEquip(equip_posi, equip_grid);
		}
		if (new_level >= level)return;
	}
}

void Workshop::XiFuEquip(int equip_posi, int equip_grid)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->XiFuEquip(equip_grid);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->XiFuEquip(equip_grid);
	}
}

void Workshop::FuMoEquip(int equip_posi, int equip_grid, int *new_spid, int *new_level)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->FuMoEquip(equip_grid, new_spid, new_level);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->FuMoEquip(equip_grid, new_spid, new_level);
	}
}

void Workshop::ComposeDianHuaShi(int main_stone_idx, int cost_stone_1, int cost_stone_2)
{
	m_role_module_mgr->GetKnapsack()->ComposeDianHuaShi(main_stone_idx, cost_stone_1, cost_stone_2);
}

bool Workshop::ChnageGodEquip(ItemGridData * grid)
{
	if (NULL == grid) return false;
	const ItemBase * i_b = ITEMPOOL->GetItem(grid->item_id);
	if (i_b == NULL)return false;
	if (i_b->GetItemType() == I_TYPE_EQUIPMENT)
	{
		const Equipment * i_e = (const Equipment *)i_b;
		if (i_e->GetColor() >= ItemBase::I_COLOR_STAR && i_e->GetColor() <= ItemBase::I_COLOR_GOD)
		{
			//星耀，神装需要自动变更装备
			const StrengthenCfg *change_cfg = GODEQUIP_CONFIG->GetStrengthenByJob(m_role_module_mgr->GetRole()->GetBaseProfession(), grid->param->equip_param.strengthen_level, i_b->GetColor(), i_e->GetEquipIndex());
			if (NULL == change_cfg)return false;

			const ItemBase * new_item_base = ITEMPOOL->GetItem(change_cfg->id);
			if (new_item_base == NULL)return false;
			if (new_item_base->GetItemType() != i_b->GetItemType())return false;

			EquipmentParam & p_ep = grid->param->equip_param;
			p_ep.attr_num = 0;
			for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX; i++)
			{
				p_ep.attr_list[i].attr_type = change_cfg->attr_type_list[i];
				p_ep.attr_list[i].attr_value = change_cfg->attr_add_list[i];
				if (change_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
				p_ep.attr_num++;
			}
			p_ep.new_suit_effect_id = change_cfg->suit_id;
			grid->item_id = change_cfg->id;
			return true;
		}
	}
	else if (i_b->GetItemType() == I_TYPE_JEWELRY)
	{
		const Jewelry * i_e = (const Jewelry *)i_b;
		if (i_e->GetColor() >= ItemBase::I_COLOR_STAR && i_e->GetColor() <= ItemBase::I_COLOR_GOD)
		{
			int real_equip_type = Equipment::E_INDEX_MAX + i_e->GetJewelryType();
			//星耀，神装需要自动变更装备
			const StrengthenCfg *change_cfg = GODEQUIP_CONFIG->GetStrengthenByJob(m_role_module_mgr->GetRole()->GetBaseProfession(), grid->param->jewelry_param.strengthen_level, i_b->GetColor(), real_equip_type);
			if (NULL == change_cfg)return false;

			const ItemBase * new_item_base = ITEMPOOL->GetItem(change_cfg->id);
			if (new_item_base == NULL)return false;
			if (new_item_base->GetItemType() != i_b->GetItemType())return false;

			JewelryParam & p_ep = grid->param->jewelry_param;
			p_ep.attr_num = 0;
			for (int i = 0; i < JewelryParam::ATTR_NUM_MAX; i++)
			{
				p_ep.attr_list[i].attr_type = change_cfg->attr_type_list[i];
				p_ep.attr_list[i].attr_value = change_cfg->attr_add_list[i];
				if (change_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
				p_ep.attr_num++;
			}
			p_ep.new_suit_effect_id = change_cfg->suit_id;
			grid->item_id = change_cfg->id;
			return true;
		}
	}
	return false;
}

void Workshop::DeComposeItem(int grid_num, int *grid_idx_list)
{
	ItemGridData tmp_equip;
	std::vector<ItemConfigData> real_reward_vec;

	for (int i = 0; i < grid_num && i < Protocol::CSWorkshopDecomposeReq::MAX_DECOMPOSE_NUM; i++)
	{
		tmp_equip.Reset();
		m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, grid_idx_list[i], &tmp_equip);

		if (tmp_equip.Invalid())return;

		const ItemBase * itembase = ITEMPOOL->GetItem(tmp_equip.item_id);
		int star_level = 0;
		if (itembase != NULL)
		{
			if (itembase->GetItemType() == I_TYPE_EQUIPMENT)
			{
				if (tmp_equip.param->equip_param.HasStone())
				{
					m_role_module_mgr->NoticeNum(errornum::EN_EQUIP_HAS_STONE_ERR);
					continue;
				}

				if (tmp_equip.param->equip_param.HasZhuLing())
				{
					m_role_module_mgr->NoticeNum(errornum::EN_EQUIP_HAS_ZHULING_ERR);
					continue;
				}
				if (tmp_equip.param->equip_param.star_level > star_level)
				{
					star_level = tmp_equip.param->equip_param.star_level;
				}
			}
			else if (itembase->GetItemType() == I_TYPE_JEWELRY)
			{
				if (tmp_equip.param->jewelry_param.HasStone())
				{
					m_role_module_mgr->NoticeNum(errornum::EN_JEWERLY_HAS_STONE_ERR);
					continue;
				}

				if (tmp_equip.param->jewelry_param.HasZhuLing())
				{
					m_role_module_mgr->NoticeNum(errornum::EN_JEWERLY_HAS_ZHULING_ERR);
					continue;
				}

				if (tmp_equip.param->jewelry_param.star_level > star_level)
				{
					star_level = tmp_equip.param->jewelry_param.star_level;
				}
			}
		}

		const WorkshopItemCompound * decompose_cfg = LOGIC_CONFIG->GetWorkshopConfig()->GetItemCompoundCfgByItemId(tmp_equip.item_id);
		if (NULL == decompose_cfg) return;

		//检查消耗是否正确
		if (tmp_equip.item_id != decompose_cfg->composition)
		{
			return;//这个道具分解不对劲
		}

		//分解返回
		int decompose_return_num = 0; ItemConfigData decompose_return_list[COMPOUND_MATE_KIND_MAX];
		for (int t_cur = 0; t_cur < COMPOUND_MATE_KIND_MAX; ++t_cur)
		{
			const MaterialCon & p_ipc = decompose_cfg->need_mate[t_cur];
			const ItemBase * return_ib = ITEMPOOL->GetItem(p_ipc.mate_id);
			if (NULL == return_ib) continue;
			if (!p_ipc.can_decompose)continue;
			if ((p_ipc.max_value - p_ipc.min_value + 1) == 0)continue;
			int real_return_num = (rand() % (p_ipc.max_value - p_ipc.min_value + 1)) + p_ipc.min_value;
			if (real_return_num > 0)decompose_return_list[decompose_return_num++] = ItemConfigData(p_ipc.mate_id, true, real_return_num);
		}

		//完全没有东西，不进行分解,背包空间不够也不分解
		if (decompose_cfg->resolve_return_lingyu <= 0 && (decompose_return_num < 0 || !m_role_module_mgr->GetKnapsack()->CheckForPutList(decompose_return_num, decompose_return_list)))break;

		//消耗道具
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, grid_idx_list[i], 1, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
			return;//消耗失败返回
		}

		for (int d_t = 0; d_t < decompose_return_num && d_t < COMPOUND_MATE_KIND_MAX; ++d_t)
		{
			bool already_has = false;
			for (std::vector<ItemConfigData>::iterator it = real_reward_vec.begin(); it != real_reward_vec.end(); ++it)
			{
				//检查是否有可以堆叠的道具
				if (it->item_id == decompose_return_list[d_t].item_id)
				{
					it->num += decompose_return_list[d_t].num;
					already_has = true;
					break;
				}
			}
			if (!already_has)real_reward_vec.push_back(decompose_return_list[d_t]);
		}

		if (decompose_cfg->resolve_return_lingyu > 0)
		{
			//检查是否星级装备
			double extra_add = 1.0;
			if (star_level > 0)
			{
				const EquipStarCfg * star_level_cfg = LOGIC_CONFIG->GetEquipStarConfig()->GetEquipStarAttrCfg(tmp_equip.item_id, star_level);
				if (NULL != star_level_cfg)
				{
					extra_add = (double)star_level_cfg->return_times / EquipStarCfg::PRICE_RATE;
				}
			}
			int real_return_money = (int)(decompose_cfg->resolve_return_lingyu * extra_add);
			if (!m_role_module_mgr->GetMoney()->AddOtherCoin(real_return_money, MONEY_TYPE_SILVER_COIN, __FUNCTION__))
			{
				gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::DiscompoundItem | Materials Consume Succeed, But Composition AddOtherCoin Failed. -> User: [ %d | %s ] | money[type:%d,price:%d] | OccurTime: %lld ]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), MONEY_TYPE_SILVER_COIN, decompose_cfg->resolve_return_lingyu, (long long)EngineAdapter::Instance().Time());
				return;
			}
		}

		if (decompose_return_num > 0 && !m_role_module_mgr->GetKnapsack()->PutList(decompose_return_num, decompose_return_list, PUT_REASON_WORKSHOP_COMPOUND))
		{
			gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::DiscompoundItem | Materials Consume Succeed, But Composition Put Failed. -> User: [ %d | %s ] | item_id: %d | OccurTime: %lld ]",
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), decompose_cfg->composition, (long long)EngineAdapter::Instance().Time());
			return;
		}
	}

	if ((short)real_reward_vec.size() > 0)m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)real_reward_vec.size(), &real_reward_vec[0], GET_REASON_EQUIP_DECOMPOSE);
}

void Workshop::CompoundItem(int compound_seq, int compound_num, int is_craft, bool is_huashen_buy)
{
	if (compound_num <= 0) return;

	const WorkshopItemCompound * p_wic = LOGIC_CONFIG->GetWorkshopConfig()->GetItemCompoundCfg(compound_seq);
	if (NULL == p_wic) return;

	if (0 != p_wic->active_sign && !m_workshop_param.sp_recipe_active_flag.IsBitSet(p_wic->active_sign))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_RECIPE_IS_NOT_ACTIVED);
		return;
	}

	if (m_role_module_mgr->GetRole()->GetLevel() < p_wic->level_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_TOO_LOW);
		return;
	}

	const ItemBase * p_ib = ITEMPOOL->GetItem(p_wic->composition);
	if (NULL == p_ib) return;

	bool set_god_create = false;
	if (I_TYPE_EQUIPMENT == p_ib->GetItemType() || I_TYPE_JEWELRY == p_ib->GetItemType())
	{
		if (1 != compound_num) compound_num = 1;	// 装备必须一件一件进行打造

		if (p_ib->GetColor() >= ItemBase::I_COLOR_STAR && p_ib->GetColor() <= ItemBase::I_COLOR_GOD)
		{
			if (m_workshop_param.sp_recipe_created_flag.IsBitSet(compound_seq))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_HAS_BE_CREATED_ERR);
				return;
			}
			set_god_create = true;
		}
	}

	// 检查铜币
	if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(p_wic->need_coin *compound_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	// 检测合成失败后的返还列表
	int fail_return_num = 0; ItemConfigData fail_return_list[FAILURE_RETURN_NUM_MAX];
	for (int t_cur = 0; t_cur < FAILURE_RETURN_NUM_MAX; ++t_cur)
	{
		const ItemPairCon & p_ipc = p_wic->fail_return_item[t_cur];
		const ItemBase * return_ib = ITEMPOOL->GetItem(p_ipc.item_id);
		if (NULL == return_ib) continue;

		fail_return_list[fail_return_num++] = ItemConfigData(p_ipc.item_id, true, p_ipc.item_num);
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(p_wic->composition, compound_num)
		 || (fail_return_num > 0 && !m_role_module_mgr->GetKnapsack()->CheckForPutList(fail_return_num, fail_return_list)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	// 获取材料列表
	int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	short back_bool_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM] = { 0 };

	if (1 == is_craft)		// 强化打造 - 消耗强化打造石
	{
		if (NULL == ITEMPOOL->GetItem(p_wic->fortifier_id))
		{
			return;
		}
		stuff_list[stuff_count++] = ItemExtern::ItemConsumeConfig(p_wic->fortifier_id, false, p_wic->craft_need);
	}

	for (int t_cur = 0; t_cur < COMPOUND_MATE_KIND_MAX && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++t_cur)
	{
		const MaterialCon & p_mc = p_wic->need_mate[t_cur];
		if (p_mc.need_num <= 0) continue;

		const ItemBase * mate_ib = ITEMPOOL->GetItem(p_mc.mate_id);
		if (NULL == mate_ib) continue;

		back_bool_list[stuff_count] = p_mc.can_return;
		stuff_list[stuff_count] = ItemExtern::ItemConsumeConfig(p_mc.mate_id, false, p_mc.need_num * compound_num);
		++stuff_count;
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

		if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "WorkshopCompoundItem")) return;
	}

	if (p_wic->need_coin > 0 && !m_role_module_mgr->GetMoney()->UseCoinBind(p_wic->need_coin * compound_num, "WorkshopCompoundItem")) return;

	// 道具消耗成功，注意不能中途返回 - 给予道具
	int succ_prob_r = rand() % 100;
	if (succ_prob_r < p_wic->succ_prob)		// 合成成功
	{
		ItemDataWrapper compound_equip; compound_equip.Reset();
		compound_equip.item_id = p_wic->composition;
		compound_equip.num = compound_num;
		compound_equip.is_bind = 0;
		static NetValueItemParam p_nvip; p_nvip.Reset();
		if (I_TYPE_EQUIPMENT == p_ib->GetItemType())
		{
			Equipment * c_equip = (Equipment *)p_ib;
			if (NULL == c_equip)
			{
				gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::Compound Equipment | Materials Consume Succeed, But Composition Convert To Equipment Failed. -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld ]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, compound_num, (long long)EngineAdapter::Instance().Time());

				return;
			}

			/*if (1 == is_craft)		// 强化打造
			{
				const WorkshopCraftAttrAdd * p_wcaa = LOGIC_CONFIG->GetWorkshopConfig()->GetCraftAttrAddCfg(p_wic->composition);
				if (NULL == p_wcaa) return;		// 这里前面已经检查过了，正常情况下，若前面没有返回，这里也不会返回；

				c_equip->RandomAttrByInsteadMin(&p_nvip, p_wcaa->min_val_instead);
			}
			else
			{
				c_equip->RandomEquipmentAttr(&p_nvip);
			}*/
			c_equip->RandomEquipmentAttr(&p_nvip);

			c_equip->RandomSpecialEffect(&p_nvip);
			m_role_module_mgr->GetRole()->GetName(p_nvip.equip_param.creator_name);		// 写入打造者
			p_nvip.equip_param.has_random_net_value = 1;
			if (1 == is_craft)		// 新-强化打造
			{
				//新增精工效果
				int ultimate_param = 0;
				EquipmentParam & equip_param = p_nvip.equip_param;
				for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX && i < p_nvip.equip_param.attr_num; i++)
				{
					//检查强化精工值是否已经达到最大值
					int max_ultimate_param = 0;
					int min_ultimate_param = 0;
					if (!c_equip->GetUltimateMaxValue(equip_param.attr_list[i].attr_type, &max_ultimate_param, &min_ultimate_param))
					{
						//获取失败,检查下一个精工
						continue;
					}

					if (max_ultimate_param - min_ultimate_param + 1 != 0)//是否有随机范围
					{
						ultimate_param = (rand() % (max_ultimate_param - min_ultimate_param + 1)) + min_ultimate_param;
					}
					else
					{
						ultimate_param = max_ultimate_param;
					}
					short &node_attr = equip_param.ultimate_attr_list[i];
					if (ultimate_param + node_attr > max_ultimate_param)
					{
						node_attr = max_ultimate_param;
					}
					else
					{
						if (ultimate_param + node_attr < 0)
						{
							node_attr = 0;
						}
						else
						{
							node_attr += ultimate_param;
						}
					}
				}
				p_nvip.equip_param.is_strong_create = 1;
			}

			//生活技能加成
			/*int life_skill_type = m_role_module_mgr->GetRoleSkill()->GetLifeSkillTypeByEquipClass(c_equip->GetEquipClass());
			const LifeSkillCfg * life_skill_cfg = SkillPool::Instance().GetLifeSkillCfg(life_skill_type);
			if (NULL != life_skill_cfg)
			{
				const LifeSkillInfo * role_skill = m_role_module_mgr->GetRoleSkill()->GetLifeSkillInfoByLifeSkillType(life_skill_type);
				if (NULL != role_skill && role_skill->skill_level <= MAX_LIFE_SKILL_LEVEL && role_skill->skill_level > 0)
				{
					const LifeSkillCfg::SkillParam & cur_skill_param = life_skill_cfg->skill_param[role_skill->skill_level];

					// 技能触发，属性提升 - 因生活技能导致的装备属性提升必定触发
					std::vector<AttrInfo> tmp_attr_inc;
					SkillPool::Instance().RandAttr(life_skill_type, role_skill->skill_level, tmp_attr_inc);

					for (std::vector<AttrInfo>::iterator loop = tmp_attr_inc.begin(); loop != tmp_attr_inc.end(); ++loop)
					{
						if (p_nvip.equip_param.inc_num >= EquipmentParam::ATTR_NUM_MAX) break;

						AttrVocInfo & attr_inc = p_nvip.equip_param.attr_extra_inc[p_nvip.equip_param.inc_num++];
						attr_inc.attr_type = loop->attr_type;
						attr_inc.attr_value = loop->attr_value;
					}

					for (int back_cur = 0; back_cur < stuff_count; ++back_cur)		// 计算材料返还
					{
						if (back_bool_list[back_cur] <= 0) continue;

						int skill_trigger_prob = rand() % 100;		// 每种材料单独随机是否返还
						if (skill_trigger_prob >= cur_skill_param.param1) continue;

						const ItemExtern::ItemConsumeConfig & stuff_con = stuff_list[back_cur];
						int tmp_num = (int)(cur_skill_param.param2 / 100.0 * stuff_con.num);
						if (tmp_num > 0) m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(stuff_con.item_id, true, tmp_num), PUT_REASON_WORKSHOP_COMPOUND);// 静默放入，若背包空间不足，此材料返还默认作废
					}
				}
			}*/
			//额外词条
			p_nvip.equip_param.inc_num = 0;
			int entry_num = 0;// LOGIC_CONFIG->GetEntryConfig()->GetEntryNumConfig(c_equip->GetEntryNum());
			if (c_equip->IsNeedReCalSmeltEntryNum())
			{
				const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_nvip.equip_param.strengthen_level, c_equip->GetEquipColor());
				if (max_num_cfg != NULL && max_num_cfg->max_num <= EquipmentParam::EXA_ATTR_NUM_MAX)p_nvip.equip_param.inc_num = p_nvip.equip_param.inc_num >= max_num_cfg->max_num ? p_nvip.equip_param.inc_num : max_num_cfg->max_num;
			}
			else
			{
				//词条解锁数
				const MaxAttrNumCfg *attr_max_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(c_equip->GetEquipLevel(), c_equip->GetEquipColor());
				if (attr_max_info != NULL)
				{
					entry_num = attr_max_info->max_num;
				}
			}
			for (int voc_idx = 0; voc_idx < entry_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
			{
				p_nvip.equip_param.attr_extra_inc[voc_idx].un_lock = 0;//所有词条修改为不开放，等玩家自己解锁
			}

			if (entry_num > 0)
			{
				RandGroupCfg entry_type_list[EquipmentParam::EXA_ATTR_NUM_MAX];
				int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(c_equip->GetEntryGroup(), entry_num, entry_type_list);
				if (real_entry_num > 0)
				{
					for (int attr_type_index = 0; attr_type_index < EquipmentParam::EXA_ATTR_NUM_MAX; attr_type_index++)
					{
						if (entry_type_list[attr_type_index].att_type < 0)continue;
						{
							const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(c_equip->GetEquipLevel(), entry_type_list[attr_type_index].quality, entry_type_list[attr_type_index].att_type);
							if (attr_info != NULL)
							{
								if (p_nvip.equip_param.inc_num >= EquipmentParam::EXA_ATTR_NUM_MAX) break;
								int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
								if (attr_value <= 0)
								{
									continue;
								}
								EquipmentParam & attr_param = p_nvip.equip_param;
								AttrVocInfo & attr_inc = attr_param.attr_extra_inc[attr_type_index];
								if (attr_inc.un_lock != 1)continue;
								attr_inc.attr_type = entry_type_list[attr_type_index].att_type;
								attr_inc.attr_value = attr_value;
								//p_nvip.equip_param.inc_num++;
							}
						}
					}
					p_nvip.equip_param.inc_num = entry_num;
					/*//检查是否保底
					//保底，颜色以装备本身品质颜色
					int voc_top_color = 0;
					for (int color_idx = 0; color_idx < 6;color_idx++)
					{
						if (entry_type_list[color_idx].quality > voc_top_color)voc_top_color = entry_type_list[color_idx].quality;
					}

					if(voc_top_color < (c_equip->GetEquipColor() > 5 ? 5 : c_equip->GetEquipColor()) && p_nvip.equip_param.inc_num > 0)//如果没有一个词条的颜色大于等于装备颜色，则从已随机词条抽取一个，边做保底词条
					{
						int rand_voc_idx = rand() % p_nvip.equip_param.inc_num;
					if (rand_voc_idx >= 0 && rand_voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX)
						{
							AttrVocInfo &top_voc = p_nvip.equip_param.attr_extra_inc[rand_voc_idx];
							const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig(c_equip->GetEquipLevel(), c_equip->GetEquipColor() > 5 ? 5 : c_equip->GetEquipColor(), top_voc.attr_type);
							if (attr_info != NULL)
							{
								int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
								if(top_voc.un_lock == 1 && attr_value > 0)
								{
									top_voc.attr_value = attr_value;
								}
							}
						}
					}*/
				}

				if (entry_num != real_entry_num)
				{
					gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::CompoundItem lost attr_voc -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld  | Equip_voc_num :%d | has_num:%d]",
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, compound_num, (long long)EngineAdapter::Instance().Time(), real_entry_num, entry_num);
				}
			}
			//检查是否有重复词条
			for (int same_index = 0; same_index < EquipmentParam::EXA_ATTR_NUM_MAX; same_index++)
			{
				AttrVocInfo &voc_info = p_nvip.equip_param.attr_extra_inc[same_index];
				if (voc_info.attr_type >= 0 && voc_info.attr_value > 0)
				{
					for (int same_index2 = 0; same_index2 < EquipmentParam::EXA_ATTR_NUM_MAX; same_index2++)
					{
						AttrVocInfo &voc_info2 = p_nvip.equip_param.attr_extra_inc[same_index2];
						if (same_index2 != same_index &&
							voc_info2.attr_type > 0 &&
							voc_info2.attr_type == voc_info.attr_type)
						{
							gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::CompoundItem same voc -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld  | Equip_voc_num :%d | Same_attr_type: %d]",
								m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, compound_num, (long long)EngineAdapter::Instance().Time(), p_nvip.equip_param.inc_num, voc_info.attr_type);
						}
					}
				}
			}

			//套装
			const SuitListGroup * suit_cfg = LOGIC_CONFIG->GetSuitListConfig()->GetSuitListGroup(c_equip->GetSuitRandomGroup());
			if (suit_cfg != NULL)
			{
				int suit_rad = rand() % suit_cfg->rate_weight;
				for (std::vector<SuitGroup>::const_iterator suit_it = suit_cfg->m_suit_group.begin(); suit_it != suit_cfg->m_suit_group.end(); suit_it++)
				{
					if (suit_it->rate > suit_rad)
					{
						p_nvip.equip_param.new_suit_effect_id = suit_it->id;
						break;
					}
				}
			}

			p_nvip.equip_param.is_treasure = (char)c_equip->CheckEquipmentIsTreasure(p_nvip.equip_param);
			if (c_equip->GetColor() >= ItemBase::I_COLOR_STAR && c_equip->GetColor() <= ItemBase::I_COLOR_GOD)p_nvip.equip_param.strengthen_level += 1;
			p_nvip.GetData(compound_equip.param_data);
			compound_equip.has_param = 1;
			int out_column = 0;
			int out_index = 0;
			if (!m_role_module_mgr->GetKnapsack()->Put(compound_equip, set_god_create ? PUT_REASON_GOD_EQUIP_WORK_SHOP : PUT_REASON_WORKSHOP_COMPOUND, &out_column, &out_index))		// 如果道具放入失败，记录 log
			{
				gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::CompoundItem | Materials Consume Succeed, But Composition Put Failed. -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld ]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, compound_num, (long long)EngineAdapter::Instance().Time());
			}
			if (ITEM_SCORE_LEVEL_S == p_nvip.equip_param.is_treasure)
			{
				m_role_module_mgr->OnAddGuildEvent(compound_equip.item_id, 0, GUILD_EVENTHANDLER_TYPE_WORK_SHOP_COMPOUND);
			}
			if (c_equip->GetColor() >= ItemBase::I_COLOR_PURPLE)
			{
				if (c_equip->GetColor() >= ItemBase::I_COLOR_STAR && c_equip->GetColor() <= ItemBase::I_COLOR_GOD)
				{
					int sign_id1 = 0;
					int sign_id2 = 0;
					ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, compound_equip);

					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_god_equip_create_notice, m_role_module_mgr->GetRole()->GetName(), sign_id1, sign_id2, compound_equip.item_id);
					if (length > 0)
					{
						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
						{
							World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
						}
					}
				}
				else if (c_equip->CheckEquipmentAttrMax(&p_nvip))
				{
					int sign_id1 = 0;
					int sign_id2 = 0;
					ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, compound_equip);

					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_system_post_equipment_max, m_role_module_mgr->GetRole()->GetName(), sign_id1, sign_id2, compound_equip.item_id);
					if (length > 0)
					{
						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
						{
							World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
						}
					}
				}
			}
			if (set_god_create)m_workshop_param.sp_recipe_created_flag.SetBit(compound_seq);
			EventHandler::Instance().OnMakeEquipment(m_role_module_mgr->GetRole(), p_ib->GetItemId(), 1);
			if (ItemBase::I_COLOR_STAR > c_equip->GetEquipColor() || c_equip->GetEquipColor() > ItemBase::I_COLOR_GOD)this->SendComposeResult(true, compound_equip);
			short equip_type = c_equip->GetEquipType();
			if (c_equip->GetEquipColor() >= ItemBase::I_COLOR_STAR && c_equip->GetEquipColor() <= ItemBase::I_COLOR_GOD)m_role_module_mgr->GetEquipmentManager()->PutOn(out_column, out_index, equip_type - Equipment::E_TYPE_MIN);
			ROLE_LOG_QUICK6(LOG_TYPE_WORK_SHOP_EQUIPMENT, m_role_module_mgr->GetRole(), compound_equip.item_id, 0, p_nvip.GetParamLog(p_ib->GetItemType()), __FUNCTION__);
		}
		else if (I_TYPE_JEWELRY == p_ib->GetItemType())
		{
			Jewelry * c_jewelry = (Jewelry *)p_ib;
			if (NULL == c_jewelry)
			{
				gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::Compound Jewelry | Materials Consume Succeed, But Composition Convert To Jewelry Failed. -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld ]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, compound_num, (long long)EngineAdapter::Instance().Time());

				return;
			}

			c_jewelry->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &p_nvip, PUT_REASON_WORKSHOP_COMPOUND);
			p_nvip.jewelry_param.has_random_net_value = 1;

			if (1 == is_craft)		// 新-强化打造
			{
				//新增精工效果
				int ultimate_param = 0;
				JewelryParam &jewelry_param = p_nvip.jewelry_param;
				for (int i = 0; i < JewelryParam::ATTR_NUM_MAX && i < p_nvip.jewelry_param.attr_num; i++)
				{
					//检查强化精工值是否已经达到最大值
					int max_ultimate_param = 0;
					int min_ultimate_param = 0;
					if (!c_jewelry->GetUltimateMaxValue(jewelry_param.attr_list[i].attr_type, &max_ultimate_param, &min_ultimate_param))
					{
						//获取失败,检查下一个精工
						continue;
					}

					if ((max_ultimate_param - min_ultimate_param + 1) != 0)//是否有随机范围
					{
						ultimate_param = (rand() % (max_ultimate_param - min_ultimate_param + 1)) + min_ultimate_param;
					}
					else
					{
						ultimate_param = max_ultimate_param;
					}
					short &node_attr = jewelry_param.ultimate_attr_list[i];
					if (ultimate_param + node_attr > max_ultimate_param)
					{
						node_attr = max_ultimate_param;
					}
					else
					{
						if (ultimate_param + node_attr < 0)
						{
							node_attr = 0;
						}
						else
						{
							node_attr += ultimate_param;
						}
					}
				}
				p_nvip.jewelry_param.is_strong_create = 1;
			}

			p_nvip.GetData(compound_equip.param_data);
			compound_equip.has_param = 1;
			int out_column = 0;
			int out_index = 0;
			m_role_module_mgr->GetKnapsack()->Put(compound_equip, set_god_create ? PUT_REASON_GOD_EQUIP_WORK_SHOP : PUT_REASON_WORKSHOP_COMPOUND, &out_column, &out_index);
			if (ITEM_SCORE_LEVEL_S == p_nvip.jewelry_param.is_treasure)
			{
				m_role_module_mgr->OnAddGuildEvent(compound_equip.item_id, 0, GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY);
			}

			if (c_jewelry->GetColor() >= ItemBase::I_COLOR_PURPLE)
			{
				if (c_jewelry->GetColor() >= ItemBase::I_COLOR_STAR && c_jewelry->GetColor() <= ItemBase::I_COLOR_GOD)
				{
					int sign_id1 = 0;
					int sign_id2 = 0;
					ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, compound_equip);
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_god_equip_create_notice, m_role_module_mgr->GetRole()->GetName(), sign_id1, sign_id2, compound_equip.item_id);
					if (length > 0)
					{
						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
						{
							World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
						}
					}
				}
				else if (c_jewelry->CheckJewelryAttrMax(&p_nvip.jewelry_param))
				{
					int sign_id1 = 0;
					int sign_id2 = 0;
					ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, compound_equip);

					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_system_post_equipment_max, m_role_module_mgr->GetRole()->GetName(), sign_id1, sign_id2, compound_equip.item_id);
					if (length > 0)
					{
						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
						{
							World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
						}
					}
				}
			}
			if (set_god_create)m_workshop_param.sp_recipe_created_flag.SetBit(compound_seq);
			RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
				(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
			if (NULL != god_token)
			{
				god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_MAKE_JEWELTY);
			}
			if (ItemBase::I_COLOR_STAR > c_jewelry->GetEquipColor() || c_jewelry->GetColor() > ItemBase::I_COLOR_GOD)this->SendComposeResult(true, compound_equip);
			EventHandler::Instance().OnMakeEquipment(m_role_module_mgr->GetRole(), p_ib->GetItemId(), 1);
			ROLE_LOG_QUICK6(LOG_TYPE_WORK_SHOP_JEWELRY, m_role_module_mgr->GetRole(), compound_equip.item_id, 0, p_nvip.GetParamLog(p_ib->GetItemType()), __FUNCTION__);
			if (c_jewelry->GetEquipColor() >= ItemBase::I_COLOR_STAR && c_jewelry->GetEquipColor() <= ItemBase::I_COLOR_GOD)m_role_module_mgr->GetEquipmentManager()->WearJewelry(out_index);
		}
		else
		{
			ItemDataWrapper compound_target; compound_target.Reset();
			compound_target.item_id = p_wic->composition;
			compound_target.num = compound_num;
			compound_target.is_bind = 0;
			compound_target.has_param = 1;
			int out_column = -1;
			int out_index = -1;
			if (!m_role_module_mgr->GetKnapsack()->Put(compound_target, PUT_REASON_WORKSHOP_COMPOUND, &out_column, &out_index))// 如果道具放入失败，记录 log
			{
				gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::CompoundItem | Materials Consume Succeed, But Composition Put Failed. -> User: [ %d | %s ] | ItemID: %d | Num: %d | OccurTime: %lld ]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, compound_num, (long long)EngineAdapter::Instance().Time());
			}
			compound_target.Reset();//清除这个没用的道具实例，用来获取对应位置道具实际信息
			if (!m_role_module_mgr->GetKnapsack()->GetItemGridWrapData(out_column, out_index, &compound_target))
			{
				gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::CompoundItem | Materials Consume Succeed, But Composition GetItemGridWrapData Failed. -> User: [ %d | %s ] | ItemID: %d | Num: %d | OccurTime: %lld ]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, compound_num, (long long)EngineAdapter::Instance().Time());
			}
			this->SendComposeResult(true, compound_target);
			if (p_ib->GetItemSubType() == ItemNamespace::ITEM_SUB_TYPE_ELEMENT_CRYSTAL)
			{
				if (is_huashen_buy && m_role_module_mgr->GetHuaShenManager()->IsSeasonInTime())
				{
					m_role_module_mgr->GetHuaShenManager()->OnSynKnapackData(true);
				}
			}
		}
		EventHandler::Instance().OnCompoundItem(m_role_module_mgr->GetRole(), p_wic->composition, compound_num);
	}
	else	// 合成失败
	{
		if (fail_return_num > 0 && !m_role_module_mgr->GetKnapsack()->PutList(fail_return_num, fail_return_list, PUT_REASON_WORKSHOP_COMPOUND))
		{
			gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::CompoundItem | Compound Failed, Item Return Failed. -> User: [ %d | %s ] | CoumpoundSeq: %d | OcurrTime: %lld ]",
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), compound_seq, (long long)EngineAdapter::Instance().Time());
		}

		m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_ITEM_COMPOUND_FAILURE);
	}
	if (set_god_create)
	{
		this->SendAllWorkshopInfo();
	}
}

bool Workshop::DiscompoundItem(int compound_seq, int compound_num, int backpace_index)
{
	if (compound_num <= 0 || backpace_index >= 300) return false;

	const WorkshopItemExchange * p_wic = LOGIC_CONFIG->GetWorkshopConfig()->GetItemExchangeCfg(compound_seq);
	if (NULL == p_wic) return false;

	if (p_wic->exchange_type < 2)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_CANNOT_DISCOMPOSE);
		return false;
	}

	const ItemBase * c_ib = ITEMPOOL->GetItem(p_wic->param1);//消耗物品
	const ItemBase * p_ib = ITEMPOOL->GetItem(p_wic->param1);//获得物品

	if (NULL == c_ib || NULL == p_ib)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_CANNOT_DISCOMPOSE);
		return false;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(p_ib->GetItemId(), compound_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	// 获取材料列表
	int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];


	stuff_list[stuff_count] = ItemExtern::ItemConsumeConfig(c_ib->GetItemId(), false, compound_num);
	++stuff_count;


	// 消耗
	if (stuff_count > 0)
	{
		static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
		if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, true))
		{
			//m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return false;
		}

		if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, backpace_index, compound_num, "DiscompoundItem"))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return false;
		}
	}

	// 道具消耗成功，注意不能中途返回 - 给予道具
	{
		if (I_TYPE_EQUIPMENT == p_ib->GetItemType())
		{
			Equipment * c_equip = (Equipment *)p_ib;
			if (NULL == c_equip)
			{
				gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::DiscompoundItem Equipment | Materials Consume Succeed, But Composition Convert To Equipment Failed. -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld ]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->param2, p_wic->give_num * compound_num, (long long)EngineAdapter::Instance().Time());

				return false;
			}

			static NetValueItemParam p_nvip; p_nvip.Reset();

			c_equip->RandomEquipmentAttr(&p_nvip);

			c_equip->RandomSpecialEffect(&p_nvip);

			m_role_module_mgr->GetRole()->GetName(p_nvip.equip_param.creator_name);		// 写入打造者
			p_nvip.equip_param.has_random_net_value = 1;
			p_nvip.equip_param.inc_num = 0;

			// 生活技能加成
			/*int life_skill_type = m_role_module_mgr->GetRoleSkill()->GetLifeSkillTypeByEquipClass(c_equip->GetEquipClass());
			const LifeSkillCfg * life_skill_cfg = SkillPool::Instance().GetLifeSkillCfg(life_skill_type);
			if (NULL != life_skill_cfg)
			{
				//分解暂无
			}*/

			p_nvip.equip_param.is_treasure = (char)c_equip->CheckEquipmentIsTreasure(p_nvip.equip_param);

			ItemDataWrapper compound_equip; compound_equip.Reset();
			p_nvip.GetData(compound_equip.param_data);

			compound_equip.item_id = p_wic->param2;
			compound_equip.num = p_wic->give_num * compound_num;
			compound_equip.is_bind = 0;
			compound_equip.has_param = 1;

			if (!m_role_module_mgr->GetKnapsack()->Put(compound_equip, PUT_REASON_WORKSHOP_COMPOUND))		// 如果道具放入失败，记录 log
			{
				gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::DiscompoundItem | Materials Consume Succeed, But Composition Put Failed. -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld ]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->param2, p_wic->give_num * compound_num, (long long)EngineAdapter::Instance().Time());
			}
		}
		else
		{
			ItemConfigData compound_target(p_wic->param2, true, p_wic->give_num * compound_num);
			if (!m_role_module_mgr->GetKnapsack()->Put(compound_target, PUT_REASON_WORKSHOP_COMPOUND))		// 如果道具放入失败，记录 log
			{
				gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::DiscompoundItem | Materials Consume Succeed, But Composition Put Failed. -> User: [ %d | %s ] | ItemID: %d | Num: %d | OccurTime: %lld ]",
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->param2, p_wic->give_num * compound_num, (long long)EngineAdapter::Instance().Time());
			}
		}
	}
	return true;
}

bool Workshop::ActiveNewRecipe(int active_id)
{
	if (active_id < 0 || active_id >= SPECIAL_RECIPE_TOTAL_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (m_workshop_param.sp_recipe_active_flag.IsBitSet(active_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_RECIPE_ALREADY_ACTIVE);
		return false;
	}

	m_workshop_param.sp_recipe_active_flag.SetBit(active_id);
	this->SendRecipeActiveFlag(active_id);
	m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_RECIPE_ACTIVE_SUCCEED);
	return true;
}

bool Workshop::AutoActiveNewRecipe(int active_id, int item_id, bool is_auto_use, bool is_notice)
{
	if (active_id < 0 || active_id >= SPECIAL_RECIPE_TOTAL_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	if (m_workshop_param.sp_recipe_active_flag.IsBitSet(active_id))
	{
		if (is_auto_use)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_face_score_is_inactive, item_id);
			if (length > 0)
			{
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_FLOAT)))
				{
					m_role_module_mgr->NetSend((const void *)&sm, sendlen);
				}
			}
		}
		else
		{
			m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_RECIPE_ALREADY_ACTIVE);
		}
		return false;
	}
	if (is_notice)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_face_score_active_succ, item_id);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_FLOAT)))
			{
				m_role_module_mgr->NetSend((const void *)&sm, sendlen);
			}
		}
	}

	gamelog::g_log_workshop.printf(LL_INFO, "%s Success! role[%d,%s] active_id:%d item_id:%d", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(),
		m_role_module_mgr->GetRole()->GetName(), active_id, item_id);
	m_workshop_param.sp_recipe_active_flag.SetBit(active_id);
	this->SendRecipeActiveFlag(active_id, is_notice);
	return true;
}

void Workshop::BatchCompoundItem(int compound_seq, int compound_num)
{
	if (compound_num <= 0) return;

	const WorkshopItemCompound * p_wic = LOGIC_CONFIG->GetWorkshopConfig()->GetItemCompoundCfg(compound_seq);
	if (NULL == p_wic)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 != p_wic->active_sign && !m_workshop_param.sp_recipe_active_flag.IsBitSet(p_wic->active_sign))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_RECIPE_IS_NOT_ACTIVED);
		return;
	}

	if (m_role_module_mgr->GetRole()->GetLevel() < p_wic->level_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_TOO_LOW);
		return;
	}
	const ItemBase * p_ib = ITEMPOOL->GetItem(p_wic->composition);
	if (NULL == p_ib) return;

	if (I_TYPE_EQUIPMENT != p_ib->GetItemType() && I_TYPE_JEWELRY != p_ib->GetItemType())
	{
		return;
	}
	if (p_ib->GetColor() > ItemBase::I_COLOR_BLUE)
	{
		return;
	}

	// 检查铜币
	if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(p_wic->need_coin *compound_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	// 检测合成失败后的返还列表
	int fail_return_num = 0; ItemConfigData fail_return_list[FAILURE_RETURN_NUM_MAX];
	for (int t_cur = 0; t_cur < FAILURE_RETURN_NUM_MAX; ++t_cur)
	{
		const ItemPairCon & p_ipc = p_wic->fail_return_item[t_cur];
		const ItemBase * return_ib = ITEMPOOL->GetItem(p_ipc.item_id);
		if (NULL == return_ib) continue;

		fail_return_list[fail_return_num++] = ItemConfigData(p_ipc.item_id, true, p_ipc.item_num * compound_num);
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(p_wic->composition, compound_num)
		|| (fail_return_num > 0 && !m_role_module_mgr->GetKnapsack()->CheckForPutList(fail_return_num, fail_return_list)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	// 获取材料列表
	int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	short back_bool_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM] = { 0 };

	for (int t_cur = 0; t_cur < COMPOUND_MATE_KIND_MAX && stuff_count < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM; ++t_cur)
	{
		const MaterialCon & p_mc = p_wic->need_mate[t_cur];
		if (p_mc.need_num <= 0) continue;

		const ItemBase * mate_ib = ITEMPOOL->GetItem(p_mc.mate_id);
		if (NULL == mate_ib) continue;

		back_bool_list[stuff_count] = p_mc.can_return;
		stuff_list[stuff_count] = ItemExtern::ItemConsumeConfig(p_mc.mate_id, false, p_mc.need_num * compound_num);
		++stuff_count;
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

		if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "WorkshopCompoundItem")) return;
	}
	if (p_wic->need_coin > 0 && !m_role_module_mgr->GetMoney()->UseCoinBind(p_wic->need_coin * compound_num, __FUNCTION__)) return;

	std::vector<ItemDataWrapper> compound_equip_list;
	int faild_num = 0;
	for (int i = 0; i < compound_num; i++)
	{
		int succ_prob_r = rand() % 100;
		if (succ_prob_r < p_wic->succ_prob)		// 合成成功
		{
			static ItemDataWrapper compound_equip; compound_equip.Reset();
			compound_equip.item_id = p_wic->composition;
			compound_equip.num = 1;
			compound_equip.is_bind = 0;
			compound_equip.has_param = 1;
			static NetValueItemParam p_nvip; p_nvip.Reset();
			if (I_TYPE_EQUIPMENT == p_ib->GetItemType())
			{
				Equipment * c_equip = (Equipment *)p_ib;
				if (NULL == c_equip)
				{
					gamelog::g_log_workshop.printf(LL_INFO, "[ %s Equipment | Materials Consume Succeed, But Composition Convert To Equipment Failed. -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld ]",
						__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, compound_num, (long long)EngineAdapter::Instance().Time());

					return;
				}
				c_equip->RandomEquipmentAttr(&p_nvip);
				c_equip->RandomSpecialEffect(&p_nvip);
				m_role_module_mgr->GetRole()->GetName(p_nvip.equip_param.creator_name);		// 写入打造者
				p_nvip.equip_param.has_random_net_value = 1;
				p_nvip.equip_param.inc_num = 0;
				//额外词条
				int entry_num = 0;// LOGIC_CONFIG->GetEntryConfig()->GetEntryNumConfig(c_equip->GetEntryNum());
								  //词条解锁数
				const MaxAttrNumCfg *attr_max_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(c_equip->GetEquipLevel(), c_equip->GetEquipColor());
				if (attr_max_info != NULL)
				{
					for (int voc_idx = 0; voc_idx < attr_max_info->max_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
					{
						p_nvip.equip_param.attr_extra_inc[voc_idx].un_lock = 0;//所有词条修改为不开放，等玩家自己解锁
					}
					entry_num = attr_max_info->max_num;
				}
				if (entry_num > 0)
				{
					RandGroupCfg entry_type_list[EquipmentParam::EXA_ATTR_NUM_MAX];
					int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(c_equip->GetEntryGroup(), entry_num, entry_type_list);
					if (real_entry_num > 0)
					{
						for (int attr_type_index = 0; attr_type_index < EquipmentParam::EXA_ATTR_NUM_MAX; attr_type_index++)
						{
							if (entry_type_list[attr_type_index].att_type < 0)continue;
							{
								const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(c_equip->GetEquipLevel(), entry_type_list[attr_type_index].quality, entry_type_list[attr_type_index].att_type);
								if (attr_info != NULL)
								{
									//if (p_nvip.equip_param.inc_num >= EquipmentParam::EXA_ATTR_NUM_MAX) break;
									int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
									if (attr_value <= 0)
									{
										continue;
									}
									EquipmentParam & attr_param = p_nvip.equip_param;
									AttrVocInfo & attr_inc = attr_param.attr_extra_inc[attr_type_index];
									if (attr_inc.un_lock != 1)continue;
									attr_inc.attr_type = entry_type_list[attr_type_index].att_type;
									attr_inc.attr_value = attr_value;
									//p_nvip.equip_param.inc_num++;
								}
							}
						}
						p_nvip.equip_param.inc_num = entry_num;
						/*//检查是否保底
						//保底，颜色以装备本身品质颜色
						int voc_top_color = 0;
						for (int color_idx = 0; color_idx < 6;color_idx++)
						{
							if (entry_type_list[color_idx].quality > voc_top_color)voc_top_color = entry_type_list[color_idx].quality;
						}

						if (voc_top_color < (c_equip->GetEquipColor() > 5 ? 5 : c_equip->GetEquipColor()) && p_nvip.equip_param.inc_num > 0)//如果没有一个词条的颜色大于等于装备颜色，则从已随机词条抽取一个，边做保底词条
						{
							int rand_voc_idx = rand() % p_nvip.equip_param.inc_num;
							if (rand_voc_idx >= 0 && rand_voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX)
							{
								AttrVocInfo &top_voc = p_nvip.equip_param.attr_extra_inc[rand_voc_idx];
								const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig(c_equip->GetEquipLevel(), c_equip->GetEquipColor() > 5 ? 5 : c_equip->GetEquipColor(), top_voc.attr_type);
								if (attr_info != NULL)
								{
									int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
									if (attr_value > 0)
									{
										top_voc.attr_value = attr_value;
									}
								}
							}
						}*/
					}

					/*if (entry_num != real_entry_num)
					{
					printf("%s lost_entry!\n", __FUNCTION__);
					}*/
				}
				//检查是否有重复词条
				for (int same_index = 0; same_index < 6; same_index++)
				{
					AttrVocInfo &voc_info = p_nvip.equip_param.attr_extra_inc[same_index];
					if (voc_info.attr_type >= 0 && voc_info.attr_value > 0)
					{
						for (int same_index2 = 0; same_index2 < 6; same_index2++)
						{
							AttrVocInfo &voc_info2 = p_nvip.equip_param.attr_extra_inc[same_index2];
							if (same_index2 != same_index &&
								voc_info2.attr_type > 0 &&
								voc_info2.attr_type == voc_info.attr_type)
							{
								//printf("%s has_same!!!!", __FUNCTION__);
							}
						}
					}
				}

				p_nvip.equip_param.is_treasure = (char)c_equip->CheckEquipmentIsTreasure(p_nvip.equip_param);
				p_nvip.GetData(compound_equip.param_data);
				ROLE_LOG_QUICK6(LOG_TYPE_WORK_SHOP_EQUIPMENT, m_role_module_mgr->GetRole(), compound_equip.item_id, 0, p_nvip.GetParamLog(p_ib->GetItemType()), __FUNCTION__);
			}
			else if (I_TYPE_JEWELRY == p_ib->GetItemType())
			{
				Jewelry * c_jewelry = (Jewelry *)p_ib;
				if (NULL == c_jewelry)
				{
					gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::Compound Jewelry | Materials Consume Succeed, But Composition Convert To Jewelry Failed. -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld ]",
						m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, compound_num, (long long)EngineAdapter::Instance().Time());

					return;
				}
				c_jewelry->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &p_nvip, PUT_REASON_WORKSHOP_COMPOUND);
				p_nvip.jewelry_param.has_random_net_value = 1;
				p_nvip.GetData(compound_equip.param_data);
				ROLE_LOG_QUICK6(LOG_TYPE_WORK_SHOP_JEWELRY, m_role_module_mgr->GetRole(), compound_equip.item_id, 0, p_nvip.GetParamLog(p_ib->GetItemType()), __FUNCTION__);
			}
			compound_equip_list.push_back(compound_equip);
		}
		else //合成失败返还道具
		{
			faild_num++;
		}
	}

	if (!compound_equip_list.empty())
	{
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)compound_equip_list.size(), &compound_equip_list[0], PUT_REASON_WORKSHOP_BATCH_COMPOUND))
		{
			gamelog::g_log_workshop.printf(LL_INFO, "[ %s | Materials Consume Succeed, But Composition Put Failed. -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld ]",
				__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), p_wic->composition, (int)compound_equip_list.size(), (long long)EngineAdapter::Instance().Time());
		}
		EventHandler::Instance().OnMakeEquipment(m_role_module_mgr->GetRole(), p_wic->composition, (short)compound_equip_list.size());
		m_role_module_mgr->GetKnapsack()->SendGetItemNotice(Protocol::SCKnapsackGetItemNotice::NOTICE_TYPE_BATCH_COMPOUND, p_wic->composition, (short)compound_equip_list.size());
		this->SendComposeResult(true, compound_equip_list[0]);
	}
	//返还失败合成
	if (faild_num > 0 && fail_return_num > 0)
	{
		for (int i = 0; i < fail_return_num && i < FAILURE_RETURN_NUM_MAX; i++)
		{
			fail_return_list[i].num = fail_return_list[i].num / compound_num * faild_num;
		}
		if (!m_role_module_mgr->GetKnapsack()->PutList(fail_return_num, fail_return_list, PUT_REASON_WORKSHOP_COMPOUND))
		{
			gamelog::g_log_workshop.printf(LL_INFO, "[ %s | Compound Failed, Item Return Failed. -> User: [ %d | %s ] | CoumpoundSeq: %d faild_num %d| OcurrTime: %lld ]",
				__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), compound_seq, faild_num, (long long)EngineAdapter::Instance().Time());
		}
		m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_ITEM_COMPOUND_FAILURE);
	}
}

bool Workshop::RecipeIsActive(int active_id)
{
	if (m_workshop_param.sp_recipe_active_flag.IsBitSet(active_id))
	{
		//m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_RECIPE_ALREADY_ACTIVE);
		return true;
	}

	return false;
}

void Workshop::ChangeEquipVoc(int equip_posi_1, int equip_grid_1, int equip_posi_2, int equip_grid_2, int role_type)
{
	if (role_type < 0 || role_type >= ROLE_TYPE_MAX) return;
	const ItemGridData * tmp_equip1 = NULL;
	const ItemGridData * tmp_equip2 = NULL;

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
	{
		tmp_equip1 = m_role_module_mgr->GetEquipmentManager()->GetEquipmentGridData2(equip_grid_1, role_type);
		if (NULL == tmp_equip1)
		{
			return;
		}
	}
	else
	{
		tmp_equip1 = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1);
		if (NULL == tmp_equip1)
		{
			return;
		}
	}

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_2)
	{
		tmp_equip2 = m_role_module_mgr->GetEquipmentManager()->GetEquipmentGridData2(equip_grid_2, role_type);
		if (NULL == tmp_equip2)
		{
			return;
		}
	}
	else
	{
		tmp_equip2 = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_2);
		if (NULL == tmp_equip2)
		{
			return;
		}
	}

	const ItemBase * equip_ib1 = ITEMPOOL->GetItem(tmp_equip1->item_id);
	const ItemBase * equip_ib2 = ITEMPOOL->GetItem(tmp_equip2->item_id);

	if (NULL == equip_ib1 || I_TYPE_EQUIPMENT != equip_ib1->GetItemType() || NULL == equip_ib2 || I_TYPE_EQUIPMENT != equip_ib2->GetItemType() ||
		NULL == tmp_equip1->param || NULL == tmp_equip2->param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	/*if (equip_ib1->GetLimitLevel() < equip_ib2->GetLimitLevel())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_EQUIPMENT_VOC_CHANGE_LEVEL_LIMIT);
		return;
	}*/

	if (equip_ib1->GetColor() < equip_ib2->GetColor())
	{
		return;
	}

	EquipmentParam & p_ep1 = tmp_equip1->param->equip_param;
	EquipmentParam & p_ep2 = tmp_equip2->param->equip_param;
	bool has_inc = false;
	for (int p_ep_idx = 0; p_ep_idx < EquipmentParam::EXA_ATTR_NUM_MAX && p_ep_idx < p_ep2.inc_num; p_ep_idx++)
	{
		if (p_ep2.attr_extra_inc[p_ep_idx].un_lock == 1 && p_ep2.attr_extra_inc[p_ep_idx].attr_value > 0)
		{
			has_inc = true;
		}
	}

	if (!has_inc)
	{
		return;
	}

	for (int i = 0; i < EquipmentParam::EXA_ATTR_NUM_MAX && i < p_ep1.inc_num; i++)
	{
		AttrVocInfo & voc_info1 = p_ep1.attr_extra_inc[i];
		AttrVocInfo & voc_info2 = p_ep2.attr_extra_inc[i];
		if (voc_info2.un_lock == 0)
		{
			voc_info1.un_lock = 0;
			voc_info1.attr_type = 0;
			voc_info1.attr_value = 0;
		}
		else if (voc_info1.un_lock == 1)
		{
			voc_info1.attr_type = voc_info2.attr_type;
			voc_info1.attr_value = voc_info2.attr_value;
		}
	}

	for (int i = 0; i < EquipmentParam::EXA_ATTR_NUM_MAX && i < p_ep2.inc_num; i++)
	{
		AttrVocInfo & voc_info2 = p_ep2.attr_extra_inc[i];
		AttrVocInfo & voc_info3 = p_ep2.refresh_attr_extra_inc[i];
		voc_info2.un_lock = 0;
		voc_info2.attr_type = 0;
		voc_info2.attr_value = 0;

		voc_info3.un_lock = 0;
		voc_info3.attr_type = 0;
		voc_info3.attr_value = 0;
	}

	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::ChangeEquipVoc user[%d,%s] weapon_id_1{[%d] param: [%s]} weapon_id_2{[%d] param: [%s]}",
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
		tmp_equip1->item_id, tmp_equip1->param->GetParamLog(equip_ib1->GetItemType()),
		tmp_equip2->item_id, tmp_equip2->param->GetParamLog(equip_ib2->GetItemType()));
	// 重算属性、发送信息
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
	{
		m_role_module_mgr->GetEquipmentManager()->SetEquipGridDirty(equip_grid_1, true, role_type);
		m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_EQUIPMENT, equip_grid_1, tmp_equip1, role_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_2)
	{
		m_role_module_mgr->GetEquipmentManager()->SetEquipGridDirty(equip_grid_2, true, role_type);
		m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_EQUIPMENT, equip_grid_2, tmp_equip2, role_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_2, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}

	if ((WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1 || WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_2) && ROLE_TYPE_BENZUN == role_type)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken * >
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_EQUIP_VOC_CHANGE);
	}
}

void Workshop::ChangeJewelryVoc(int equip_posi_1, int equip_grid_1, int equip_posi_2, int equip_grid_2, int role_type)
{
	if (role_type < 0 || role_type >= ROLE_TYPE_MAX) return;
	const ItemGridData * tmp_equip1 = NULL;
	const ItemGridData * tmp_equip2 = NULL;

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
	{
		tmp_equip1 = m_role_module_mgr->GetEquipmentManager()->GetJewelryGridData2(equip_grid_1, role_type);
		if (NULL == tmp_equip1)
		{
			return;
		}
	}
	else
	{
		tmp_equip1 = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1);
		if (NULL == tmp_equip1)
		{
			return;
		}
	}

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_2)
	{
		tmp_equip2 = m_role_module_mgr->GetEquipmentManager()->GetJewelryGridData2(equip_grid_2, role_type);
		if (NULL == tmp_equip2)
		{
			return;
		}
	}
	else
	{
		tmp_equip2 = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_2);
		if (NULL == tmp_equip2)
		{
			return;
		}
	}

	const ItemBase * equip_ib1 = ITEMPOOL->GetItem(tmp_equip1->item_id);
	const ItemBase * equip_ib2 = ITEMPOOL->GetItem(tmp_equip2->item_id);

	if (NULL == equip_ib1 || I_TYPE_JEWELRY != equip_ib1->GetItemType() || NULL == equip_ib2 || I_TYPE_JEWELRY != equip_ib2->GetItemType() ||
		NULL == tmp_equip1->param || NULL == tmp_equip2->param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	/*if (equip_ib1->GetLimitLevel() < equip_ib2->GetLimitLevel())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_EQUIPMENT_VOC_CHANGE_LEVEL_LIMIT);
		return;
	}*/
	JewelryParam & p_ep1 = tmp_equip1->param->jewelry_param;
	JewelryParam & p_ep2 = tmp_equip2->param->jewelry_param;

	bool has_inc = false;
	for (int p_ep_idx = 0; p_ep_idx < JewelryParam::EXA_ATTR_NUM_MAX && p_ep_idx < p_ep2.inc_num; p_ep_idx++)
	{
		if (p_ep2.attr_extra_inc[p_ep_idx].un_lock == 1 && p_ep2.attr_extra_inc[p_ep_idx].attr_value > 0)
		{
			has_inc = true;
		}
	}

	if (!has_inc)
	{
		return;
	}

	for (int i = 0; i < JewelryParam::EXA_ATTR_NUM_MAX; i++)
	{
		AttrVocInfo & voc_info1 = p_ep1.attr_extra_inc[i];
		AttrVocInfo & voc_info2 = p_ep2.attr_extra_inc[i];
		if (voc_info2.un_lock == 0)
		{
			voc_info1.un_lock = 0;
			voc_info1.attr_type = 0;
			voc_info1.attr_value = 0;
		}
		else if (voc_info1.un_lock == 1)
		{
			voc_info1.attr_type = voc_info2.attr_type;
			voc_info1.attr_value = voc_info2.attr_value;
		}
	}

	for (int i = 0; i < EquipmentParam::EXA_ATTR_NUM_MAX && i < p_ep2.inc_num; i++)
	{
		AttrVocInfo & voc_info2 = p_ep2.attr_extra_inc[i];
		AttrVocInfo & voc_info3 = p_ep2.refresh_attr_extra_inc[i];
		voc_info2.un_lock = 0;
		voc_info2.attr_type = 0;
		voc_info2.attr_value = 0;

		voc_info3.un_lock = 0;
		voc_info3.attr_type = 0;
		voc_info3.attr_value = 0;
	}

	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::ChangeJewelryVoc user[%d,%s] weapon_id_1{[%d] param: [%s]} weapon_id_2{[%d] param: [%s]}",
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
		tmp_equip1->item_id, tmp_equip1->param->GetParamLog(equip_ib1->GetItemType()),
		tmp_equip2->item_id, tmp_equip2->param->GetParamLog(equip_ib2->GetItemType()));

	// 重算属性、发送信息
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
	{
		m_role_module_mgr->GetEquipmentManager()->SetJewelryGridDirty(equip_grid_1, true, role_type);
		m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_JEWELRY, equip_grid_1, tmp_equip1, role_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_2)
	{
		m_role_module_mgr->GetEquipmentManager()->SetJewelryGridDirty(equip_grid_2, true, role_type);
		m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_JEWELRY, equip_grid_2, tmp_equip2, role_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_2, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}

	if ((WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1 || WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_2) && ROLE_TYPE_BENZUN == role_type)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_EQUIP_VOC_CHANGE);
	}
}

void Workshop::VocUltimate(int item_type, int equip_posi_1, int equip_grid_1, int voc_posi_1, int role_type)
{
	//屏蔽
	return;
	if (role_type < 0 || role_type >= ROLE_TYPE_MAX) return;
	if (item_type < 0 || item_type >= EQUIP_TYPE_MAX)return;
	if (voc_posi_1 < 0 || voc_posi_1 >= EquipmentParam::EXA_ATTR_NUM_MAX)return;
	const ItemGridData * tmp_equip = NULL;

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
	{
		if (item_type == EQUIP_TYPE_EQUIPMENT)//装备
		{
			tmp_equip = m_role_module_mgr->GetEquipmentManager()->GetEquipmentGridData2(equip_grid_1, role_type);
			if (NULL == tmp_equip)
			{
				return;
			}
		}
		else
		{
			tmp_equip = m_role_module_mgr->GetEquipmentManager()->GetJewelryGridData2(equip_grid_1, role_type);
			if (NULL == tmp_equip)
			{
				return;
			}
		}
	}
	else
	{
		tmp_equip = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1);
		if (NULL == tmp_equip)
		{
			return;
		}
	}

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip->item_id);

	if (NULL == equip_ib || (equip_ib->GetItemType() != I_TYPE_EQUIPMENT && I_TYPE_JEWELRY != equip_ib->GetItemType()) || NULL == tmp_equip->param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//检查对应孔位满足条件
	AttrVocInfo * node_voc = NULL;
	const QuenChingAddCfg * add_cfg = NULL;
	const AttValueCfg * attr_cfg = NULL;
	const AttValueCfg * max_attr_cfg = NULL;
	{
		if (equip_ib->GetItemType() == I_TYPE_EQUIPMENT)
		{
			const Equipment* equip_ment = (const Equipment*)equip_ib;
			node_voc = &tmp_equip->param->equip_param.attr_extra_inc[voc_posi_1];
			attr_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3ByLevelAndTypeAndValue(equip_ment->GetEquipLevel(), node_voc->attr_type, node_voc->attr_value);
			if (NULL == attr_cfg)return;
			add_cfg = LOGIC_CONFIG->GetEntryConfig()->GetQuenChingAddCfg(attr_cfg->quality, attr_cfg->att_type);
			if (NULL == add_cfg)return;
			max_attr_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(equip_ment->GetEquipLevel(), equip_ment->GetColor(), node_voc->attr_type);
			if (NULL == max_attr_cfg)return;
		}
		else if (equip_ib->GetItemType() == I_TYPE_JEWELRY)
		{
			const Jewelry* jewelry_ment = (const Jewelry*)equip_ib;
			node_voc = &tmp_equip->param->jewelry_param.attr_extra_inc[voc_posi_1];
			attr_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig4ByLevelAndTypeAndValue(jewelry_ment->GetJewelryLevel(), node_voc->attr_type, node_voc->attr_value);
			if (NULL == attr_cfg)return;
			add_cfg = LOGIC_CONFIG->GetEntryConfig()->GetQuenChingAddCfg(attr_cfg->quality, attr_cfg->att_type);
			if (NULL == add_cfg)return;
			max_attr_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig4(jewelry_ment->GetJewelryLevel(), jewelry_ment->GetColor(), node_voc->attr_type);
			if (NULL == max_attr_cfg)return;
		}
	}
	if (attr_cfg == NULL || add_cfg == NULL)return;
	//消耗
	const QuenChingExpendCfg * expend_cfg = LOGIC_CONFIG->GetEntryConfig()->GetQuenChingExpendCfg(attr_cfg->quality);
	if (expend_cfg == NULL)return;

	if (!m_role_module_mgr->GetKnapsack()->HasItem(expend_cfg->item_id, expend_cfg->consume_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	m_role_module_mgr->GetKnapsack()->ConsumeItem(expend_cfg->item_id, expend_cfg->consume_num, __FUNCTION__);

	int voc_color = attr_cfg->quality;

	if (node_voc->attr_value + add_cfg->once_add <= max_attr_cfg->max)node_voc->attr_value += add_cfg->once_add;
	else node_voc->attr_value = max_attr_cfg->max;

	{
		const AttValueCfg * attr_cfg2 = NULL;

		if (equip_ib->GetItemType() == I_TYPE_EQUIPMENT)
		{
			const Equipment* equip_ment = (const Equipment*)equip_ib;
			node_voc = &tmp_equip->param->equip_param.attr_extra_inc[voc_posi_1];
			attr_cfg2 = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfigByLevelAndTypeAndValue(equip_ment->GetEquipLevel(), node_voc->attr_type, node_voc->attr_value);
			if (NULL == attr_cfg2)return;
		}
		else if (equip_ib->GetItemType() == I_TYPE_JEWELRY)
		{
			const Jewelry* jewelry_ment = (const Jewelry*)equip_ib;
			node_voc = &tmp_equip->param->jewelry_param.attr_extra_inc[voc_posi_1];
			attr_cfg2 = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfigByLevelAndTypeAndValue(jewelry_ment->GetJewelryLevel(), node_voc->attr_type, node_voc->attr_value);
			if (NULL == attr_cfg2)return;
		}

		int cur_voc_color = attr_cfg2->quality > ItemBase::I_COLOR_RED ? ItemBase::I_COLOR_RED : attr_cfg2->quality;

		if (voc_color >= ItemBase::I_COLOR_PURPLE && cur_voc_color > voc_color)
		{
			EventHandler::Instance().OnEquipmentSmeltHigh(m_role_module_mgr->GetRole(), cur_voc_color, 1);
		}
	}

	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::OnActiveEquipVocAttr user[%d,%s] weapon_id{[%d] param: [%s]}",
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
		tmp_equip->item_id, tmp_equip->param->GetParamLog(equip_ib->GetItemType()));

	// 重算属性、发送信息
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
	{
		if (item_type == EQUIP_TYPE_EQUIPMENT)
		{
			m_role_module_mgr->GetEquipmentManager()->SetEquipGridDirty(equip_grid_1, true, role_type);
		}
		else
		{
			m_role_module_mgr->GetEquipmentManager()->SetJewelryGridDirty(equip_grid_1, true, role_type);
		}
		m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(item_type == EQUIP_TYPE_EQUIPMENT ? I_TYPE_EQUIPMENT : I_TYPE_JEWELRY, equip_grid_1, tmp_equip, role_type);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1 && ROLE_TYPE_BENZUN == role_type)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}
}

void Workshop::EquipGemMounting(int stone_type, int equip_posi, int equip_grid, int stone_grid)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->EquipGemMounting(stone_type, equip_grid, stone_grid);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->EquipGemMounting(stone_type, equip_grid, stone_grid);
	}
}

void Workshop::EquipGemDismount(int stone_type, int equip_posi, int equip_grid, int stone_slot)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->EquipGemDismount(stone_type, equip_grid, stone_slot);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->EquipGemDismount(stone_type, equip_grid, stone_slot);
	}
}

void Workshop::EquipGemUpgrades(int stone_type, int equip_posi, int equip_grid, int stone_slot)
{
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		m_role_module_mgr->GetEquipmentManager()->EquipGemUpgrades(stone_type, equip_grid, stone_slot);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->EquipGemUpgrades(stone_type, equip_grid, stone_slot);
	}
}

void Workshop::ConfirmGemUpgrade()
{
	m_role_module_mgr->GetEquipmentManager()->ConfirmGemUpgrade();
	m_role_module_mgr->GetKnapsack()->ConfirmGemUpgrade();
}

void Workshop::ExchangePetSkillBook(int exc_seq, std::vector<ItemPairCon> & grid_list)
{
	const WorkshopItemExchange * exc_cfg = LOGIC_CONFIG->GetWorkshopConfig()->GetItemExchangeCfg(exc_seq);
	if (NULL == exc_cfg || WORKSHOP_EXCHANGE_PET_SKILL_BOOK != exc_cfg->exchange_type) return;

	Knapsack * knapsack = m_role_module_mgr->GetKnapsack();
	if (NULL == knapsack) return;

	std::vector<ItemID> tmp_back_item_id_vec; int mate_num = 0;
	for (std::vector<ItemPairCon>::iterator loop = grid_list.begin(); loop != grid_list.end(); ++loop)
	{
		if (!LOGIC_CONFIG->GetWorkshopConfig()->IsItemBelongsToGroup(exc_cfg->param1, loop->item_id))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
			return;
		}

		mate_num += loop->item_num;
		tmp_back_item_id_vec.push_back(loop->item_id);
	}

	if (exc_cfg->need_num != mate_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	// 先把道具随机出来
	ItemID get_item = LOGIC_CONFIG->GetWorkshopConfig()->RandAExchangeItem(exc_cfg->param2);
	if (NULL == ITEMPOOL->GetItem(get_item))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_WITHOUT_LEGAL_RESULTS);
		return;
	}

	if (!knapsack->CheckForPut(get_item, exc_cfg->give_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	// 检查通过，消耗
	int stuff_count_1 = 0, stuff_count_2 = 0;
	ItemExtern::ItemConsumeConfig stuff_list_1[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	ItemExtern::ItemConsumeConfig stuff_list_2[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (std::vector<ItemPairCon>::iterator mate_loop = grid_list.begin(); mate_loop != grid_list.end(); ++mate_loop)
	{
		if (stuff_count_1 >= ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM)
		{
			if (stuff_count_2 >= ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return;
			}

			ItemExtern::ItemConsumeConfig & stuff_obj_2 = stuff_list_2[stuff_count_2++];
			stuff_obj_2.item_id = mate_loop->item_id;
			stuff_obj_2.num = mate_loop->item_num;
			stuff_obj_2.buyable = false;
			continue;
		}

		ItemExtern::ItemConsumeConfig & stuff_obj_1 = stuff_list_1[stuff_count_1++];
		stuff_obj_1.item_id = mate_loop->item_id;
		stuff_obj_1.num = mate_loop->item_num;
		stuff_obj_1.buyable = false;
	}

	ItemExtern::ItemConsumeList consume_list_1, consume_list_2;
	if (stuff_count_1 > 0 && !ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count_1, stuff_list_1, &consume_list_1, true))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}

	if (stuff_count_2 > 0 && !ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, stuff_count_2, stuff_list_2, &consume_list_2, true))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}

	// 消耗
	if (consume_list_1.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list_1.count, consume_list_1.column_list, consume_list_1.index_list, consume_list_1.num_list, "WorkshopConvertPetSkillBook")) return;
	if (consume_list_2.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list_2.count, consume_list_2.column_list, consume_list_2.index_list, consume_list_2.num_list, "WorkshopConvertPetSkillBook")) return;

	int prob_rate = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	if (prob_rate < exc_cfg->succ_rate)		// 成功
	{
		ItemDataWrapper get_item_obj(get_item, 1, 1);
		knapsack->Put(get_item_obj, PUT_REASON_WORKSHOP_COMPOUND);
		this->SendComposeResult(true, get_item_obj);
		EventHandler::Instance().OnCompoundBook(m_role_module_mgr->GetRole());
	}
	else	// 合成失败，返还其中的一本
	{
		int back_idx = rand() % (int)tmp_back_item_id_vec.size();
		ItemDataWrapper back_item_obj(tmp_back_item_id_vec[back_idx], 1, 1);
		knapsack->Put(back_item_obj, PUT_REASON_WORKSHOP_COMPOUND);
		this->SendComposeResult(false, back_item_obj);
	}
}

void Workshop::ExchangeTarget(int exc_seq, int exc_times)
{
	const WorkshopItemExchange * exc_cfg = LOGIC_CONFIG->GetWorkshopConfig()->GetItemExchangeCfg(exc_seq);

	if (NULL == exc_cfg) return;
	if (exc_cfg->exchange_type == WORKSHOP_EXCHANGE_PET_SKILL_BOOK) return;
	if (exc_cfg->exchange_type < WORKSHOP_EXCHANGE_PET_SEALS_CARD || exc_cfg->exchange_type > WORKSHOP_SYNTHESIS_SUPER_MARROW || exc_cfg->need_num <= 0) return;


	Knapsack * knapsack = m_role_module_mgr->GetKnapsack();
	if (NULL == knapsack) return;

	int comp_need = exc_times * exc_cfg->need_num;
	if (comp_need <= 0 || knapsack->Count(exc_cfg->param1, false) < comp_need)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}

	if (exc_cfg->need_coin != 0 && !m_role_module_mgr->GetMoney()->CoinBindMoreThan(exc_cfg->need_coin*exc_times))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	if (!knapsack->CheckForPut(exc_cfg->param2, exc_cfg->give_num * exc_times))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (exc_cfg->need_coin != 0 && !m_role_module_mgr->GetMoney()->UseCoinBind(exc_cfg->need_coin*exc_times, "WorkshopCompoundConsumeCoin"))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}
	int reality_num = 0;
	int prob_rate = 0;
	int faid_num = 0;
	if (knapsack->ConsumeItem(exc_cfg->param1, comp_need, __FUNCTION__))
	{
		for (int i = 0; i< exc_times; i++)
		{
			prob_rate = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			if (prob_rate < exc_cfg->succ_rate)		// 成功
			{
				reality_num++;
				gamelog::g_log_workshop.printf(LL_INFO, "%s Success! once_time:%d target_id:count->[%d]:[%d]", __FUNCTION__, i, exc_cfg->param2, exc_times);
			}
			else
			{
				faid_num++;
				gamelog::g_log_workshop.printf(LL_INFO, "%s Fail! once_time:%d target_id:count->[%d]:[%d]", __FUNCTION__, i, exc_cfg->param1, exc_times);
			}
		}
		if (reality_num > 0)
		{
			if (knapsack->Put(ItemConfigData(exc_cfg->param2, true, exc_cfg->give_num * reality_num), PUT_REASON_COMPUOUND_WORK_SHOP))
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_synthesis_info_5, exc_cfg->param2, exc_cfg->give_num * reality_num);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(m_role_module_mgr->GetRole()->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
				}
			}
		}
		if (faid_num > 0)
		{
			if (knapsack->Put(ItemConfigData(exc_cfg->fail_return_item, true, exc_cfg->fail_return_num * faid_num), PUT_REASON_COMPUOUND_WORK_SHOP))
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_synthesis_info_6, exc_cfg->param1, exc_cfg->fail_return_num * faid_num);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(m_role_module_mgr->GetRole()->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
				}
			}
		}
	}
	EventHandler::Instance().OnCompoundCard(m_role_module_mgr->GetRole(), reality_num);
}

//void Workshop::SynthesisTarget(int _exc_seq, ItemID _target_id,ItemID _need_id, int _synthesis_type)
//{
//	const WorkshopItemExchange * exc_cfg = LOGIC_CONFIG->GetWorkshopConfig()->GetItemExchangeCfg(_exc_seq);
//	if (NULL == exc_cfg || _synthesis_type != exc_cfg->exchange_type || exc_cfg->need_num <= 0) return;
//
//	Knapsack * knapsack = m_role_module_mgr->GetKnapsack();
//	if (NULL == knapsack) return;
//	if (!knapsack->HasItem(_need_id, exc_cfg->need_num))//合成材料不足
//	{
//		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_synthesis_info_1,ITEMPOOL->GetItem(_need_id)->GetItemName());
//		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
//		return;
//	}
//
//	if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(exc_cfg->need_coin))//铜币不足
//	{
//		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_synthesis_info_2);
//		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
//		return;
//	}
//
//	bool use_coin_ok = m_role_module_mgr->GetMoney()->UseCoinBind(exc_cfg->need_coin,__FUNCTION__);		//消耗铜币
//	bool consume_item_ok = knapsack->ConsumeItem(_need_id, exc_cfg->need_num, __FUNCTION__);			//消耗物品
//
//	if (!use_coin_ok || !consume_item_ok) return;
//
//	int prob_rate = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
//	if (prob_rate < exc_cfg->succ_rate)		// 成功
//	{
//		ItemConfigData icd = ItemConfigData(_target_id,1, 1);
//		knapsack->Put(icd, PUT_REASON_WORKSHOP_COMPOUND);
//
//		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_synthesis_info_3);
//		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
//
//		m_is_synthesis = true;
//
//	} else
//	{
//		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_synthesis_info_4);
//		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
//
//		m_is_synthesis = false;
//	}
//	
//	
//}

void Workshop::CraftJewelry(int list_id)
{
	const CompoundCont * p_cc = LOGIC_CONFIG->GetJewelryConfig()->GetCompoundInfo(list_id);
	if (NULL == p_cc) return;
	if (p_cc->random_group < 0 || p_cc->random_group >= INTJEWELRY_SUCC_MAX_GROUP)return;
	const JewelryColorInfoList * jc_c = LOGIC_CONFIG->GetJewelryConfig()->GetJewelryColorListCfgByGroup(p_cc->random_group);
	if (NULL == jc_c) return;

	//随机装备组
	int item_group_id = 0;
	{
		std::vector<JewelryColorInfo> m_color_vec;//实际随机权重表
		int weight = 0;
		int &times = m_workshop_param.jewelry_create_group[p_cc->random_group];
		for (int i = 0; i < INTJEWELRY_SUCC_MAX_LIST; i++)
		{
			if (jc_c->jewelry_color_list[i].rate > 0)
			{
				JewelryColorInfo node_new;
				node_new.color = jc_c->jewelry_color_list[i].color;
				node_new.fine_tuning_value = jc_c->jewelry_color_list[i].fine_tuning_value;

				node_new.id = jc_c->jewelry_color_list[i].id;
				node_new.quality_id = jc_c->jewelry_color_list[i].quality_id;
				int count = jc_c->jewelry_color_list[i].rate + (times * jc_c->jewelry_color_list[i].fine_tuning_value);
				node_new.rate = count >= 0 ? count : 0;
				m_color_vec.push_back(node_new);
				weight += node_new.rate;
			}
		}

		if (weight > 0)
		{
			int rate_num = rand() % weight;
			for (std::vector<JewelryColorInfo>::iterator it = m_color_vec.begin(); it != m_color_vec.end(); it++)
			{
				if (rate_num < it->rate)
				{
					item_group_id = it->id;
					if (it->color < Equipment::EQUIP_COLOR_IDX_PURPLE)
					{
						times++;
					}
					else
					{
						times = 0;
					}
					break;
				}

				rate_num -= it->rate;
			}
		}
	}

	int jewelry_id = LOGIC_CONFIG->GetJewelryConfig()->RandomAJewelryID(item_group_id);
	const ItemBase * p_ib = ITEMPOOL->GetItem(jewelry_id);
	if (NULL == p_ib || I_TYPE_JEWELRY != p_ib->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(jewelry_id, 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	Jewelry * j_item = (Jewelry *)p_ib;
	if (NULL == j_item) return;

	// 检查铜币
	if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(p_cc->compound_pay))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	// 获取材料列表
	int stuff_count = 0; ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	for (int t_cur = 0; t_cur < CRAFT_MATE_NEED_MAX; ++t_cur)
	{
		const ItemPairCon & p_mc = p_cc->mate_list[t_cur];
		if (p_mc.item_num <= 0) continue;

		const ItemBase * mate_ib = ITEMPOOL->GetItem(p_mc.item_id);
		if (NULL == mate_ib) continue;

		ItemExtern::ItemConsumeConfig & stuff_obj = stuff_list[stuff_count++];
		stuff_obj.item_id = p_mc.item_id;
		stuff_obj.num = p_mc.item_num;
		stuff_obj.buyable = false;
	}

	static NetValueItemParam p_nvip; p_nvip.Reset();
	if (!j_item->RandomNetValueParam(&p_nvip.jewelry_param))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_WITHOUT_LEGAL_RESULTS);
		return;
	}

	//额外词条 
	int entry_num = 0;// LOGIC_CONFIG->GetEntryConfig()->GetEntryNumConfig(j_item->GetEntryNum());
	//词条解锁数
	const MaxAttrNumCfg *attr_max_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(j_item->GetJewelryLevel(), j_item->GetEquipColor());
	if (attr_max_info != NULL)
	{
		for (int voc_idx = 0; voc_idx < attr_max_info->max_num && voc_idx < JewelryParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			p_nvip.jewelry_param.attr_extra_inc[voc_idx].un_lock = 0;
		}
		if (entry_num < attr_max_info->max_num)entry_num = attr_max_info->max_num;
	}
	if (entry_num > 0)//是否有词条
	{
		RandGroupCfg entry_type_list[JewelryParam::EXA_ATTR_NUM_MAX];
		int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(j_item->GetEntryGroup(), entry_num, entry_type_list);
		if (real_entry_num > 0)
		{
			for (int attr_type_index = 0; attr_type_index < JewelryParam::EXA_ATTR_NUM_MAX; attr_type_index++)
			{
				if (entry_type_list[attr_type_index].att_type < 0)continue;
				{
					const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig4(j_item->GetJewelryLevel(), entry_type_list[attr_type_index].quality, entry_type_list[attr_type_index].att_type);
					if (attr_info != NULL)
					{
						if (p_nvip.jewelry_param.inc_num >= JewelryParam::EXA_ATTR_NUM_MAX) break;
						int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
						if (attr_value <= 0)continue;
						JewelryParam & attr_param = p_nvip.jewelry_param;
						AttrVocInfo & attr_inc = attr_param.attr_extra_inc[attr_type_index];
						if (attr_inc.un_lock != 1)continue;
						attr_inc.attr_type = entry_type_list[attr_type_index].att_type;
						attr_inc.attr_value = attr_value;
						//p_nvip.jewelry_param.inc_num++;
					}
				}
			}
			p_nvip.jewelry_param.inc_num = entry_num;
			/*//检查是否保底
			//保底，颜色以装备本身品质颜色
			int voc_top_color = 0;
			for (int color_idx = 0; color_idx < 6; color_idx++)
			{
				if (entry_type_list[color_idx].quality > voc_top_color)voc_top_color = entry_type_list[color_idx].quality;
			}
			if (voc_top_color < (j_item->GetEquipColor() > 5 ? 5 : j_item->GetEquipColor()) && p_nvip.jewelry_param.inc_num > 0)//如果没有一个词条的颜色大于等于装备颜色，则从已随机词条抽取一个，边做保底词条
			{
				int rand_voc_idx = rand() % p_nvip.jewelry_param.inc_num;
				if (rand_voc_idx >= 0 && rand_voc_idx < JewelryParam::EXA_ATTR_NUM_MAX)
				{
					AttrVocInfo &top_voc = p_nvip.jewelry_param.attr_extra_inc[rand_voc_idx];
					const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig2(j_item->GetJewelryLevel(), j_item->GetEquipColor() > 5 ? 5 : j_item->GetEquipColor(), top_voc.attr_type);
					if (attr_info != NULL)
					{
						int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
						if (attr_value > 0)
						{
							top_voc.attr_value = attr_value;
						}
					}
				}
			}*/
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

		// 消耗 - 铜币 + 道具
		if (p_cc->compound_pay > 0 && !m_role_module_mgr->GetMoney()->UseCoinBind(p_cc->compound_pay, "WorkshopCraftJewelry")) return;
		if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, "WorkshopCompoundItem")) return;
	}

	// 道具消耗成功，给予道具，注意不能中途返回

	m_role_module_mgr->GetRole()->GetName(p_nvip.jewelry_param.creator_name);				// 打造者

	p_nvip.jewelry_param.is_treasure = (char)j_item->CheckEquipmentIsTreasure(p_nvip.jewelry_param);

	ItemDataWrapper compound_equip; compound_equip.Reset();
	p_nvip.GetData(compound_equip.param_data);

	compound_equip.item_id = jewelry_id;
	compound_equip.num = 1;
	compound_equip.is_bind = 0;
	compound_equip.has_param = 1;

	m_role_module_mgr->GetKnapsack()->Put(compound_equip, PUT_REASON_WORKSHOP_COMPOUND);
	if (ITEM_SCORE_LEVEL_S == p_nvip.jewelry_param.is_treasure)
	{
		m_role_module_mgr->OnAddGuildEvent(jewelry_id, 0, GUILD_EVENTHANDLER_TYPE_WORK_SHOP_CRAFT_JEWELRY);
	}
	if (j_item->GetColor() >= ItemBase::I_COLOR_PURPLE)
	{
		if (j_item->GetColor() >= ItemBase::I_COLOR_STAR && j_item->GetColor() <= ItemBase::I_COLOR_GOD)
		{
			int sign_id1 = 0;
			int sign_id2 = 0;
			ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, compound_equip);

			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_god_equip_create_notice, m_role_module_mgr->GetRole()->GetName(), sign_id1, sign_id2, compound_equip.item_id);
			if (length > 0)
			{
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}
		else if (j_item->CheckJewelryAttrMax(&p_nvip.jewelry_param))
		{
			int sign_id1 = 0;
			int sign_id2 = 0;
			ChatManager::Instance().AddChatItemInfo(&sign_id1, &sign_id2, compound_equip);

			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_system_post_equipment_max, m_role_module_mgr->GetRole()->GetName(), sign_id1, sign_id2, compound_equip.item_id);
			if (length > 0)
			{
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
				{
					World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
				}
			}
		}
	}

	if (ItemBase::I_COLOR_STAR > j_item->GetEquipColor() || j_item->GetColor() > ItemBase::I_COLOR_GOD)this->SendComposeResult(true, compound_equip);
	this->SendJewelryTimesInfo();

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_MAKE_JEWELTY);
	}

	ROLE_LOG_QUICK6(LOG_TYPE_WORK_SHOP_JEWELRY, m_role_module_mgr->GetRole(), compound_equip.item_id, 0, p_nvip.GetParamLog(p_ib->GetItemType()), __FUNCTION__);
}

void Workshop::ReforgeJewelry(int mate_grid_1, int mate_grid_2)
{
	// 	{
	// 		char * info = new char[2048];
	// 		sprintf(info, "Reforge, Grid: %d | %d", mate_grid_1, mate_grid_2);
	// 		m_role_module_mgr->GetRole()->PrintToScreen(info);
	// 		delete[] info;
	// 	}
	// 	for (int t_cur = 0; t_cur < JEWELTY_REFORGE_LIST_MAX; ++t_cur)
	// 	{
	// 		if (0 != m_workshop_param.jewelry_reforge_list[t_cur].jewelry_item)
	// 		{
	// 			m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_REFORGE_RESULTS_EXIST);
	// 			this->SendJewelryReforgeOutcome();
	// 		}
	// 	}
	// 
	// 	static ItemGridData mate_1, mate_2; mate_1.Reset(); mate_2.Reset();
	// 
	// 	m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, mate_grid_1, &mate_1);
	// 	m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, mate_grid_2, &mate_2);
	// 	if (mate_1.Invalid() || mate_2.Invalid())
	// 	{
	// 		m_role_module_mgr->GetRole()->PrintToScreen("Can't Get Item! Return.");
	// 		return;
	// 	}
	// 
	// 	const ItemBase * jewelry_ib_1 = ITEMPOOL->GetItem(mate_1.item_id);
	// 	if (NULL == jewelry_ib_1 || I_TYPE_JEWELRY != jewelry_ib_1->GetItemType())
	// 	{
	// 		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
	// 		return;
	// 	}
	// 
	// 	const ItemBase * jewelry_ib_2 = ITEMPOOL->GetItem(mate_2.item_id);
	// 	if (NULL == jewelry_ib_2 || I_TYPE_JEWELRY != jewelry_ib_2->GetItemType())
	// 	{
	// 		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
	// 		return;
	// 	}
	// 
	// 	Jewelry * j_item_1 = (Jewelry *)jewelry_ib_1;
	// 	Jewelry * j_item_2 = (Jewelry *)jewelry_ib_2;
	// 	if (NULL == j_item_1 || NULL == j_item_2)
	// 	{
	// 		m_role_module_mgr->GetRole()->PrintToScreen("Cast Error, Return.");
	// 		return;
	// 	}
	// 	
	// 	JewelryBasic j_head;
	// 	// 灵饰类型
	// 	j_head.jewelry_type = GetRand(j_item_1->GetJewelryType(), j_item_2->GetJewelryType());
	// 
	// 	// 灵饰等级
	// 	j_head.jewelry_level = (j_item_1->GetLimitLevel() + j_item_2->GetLimitLevel()) >> 1;
	// 	if (0 != j_head.jewelry_level % 10) j_head.jewelry_level += GetRand(5, -5);
	// 
	// 	// 灵饰品质
	// 	j_head.jewelry_color = j_item_1->GetColor() + j_item_2->GetColor();
	// 	if (0 != j_head.jewelry_color % 2) j_head.jewelry_color += GetRand(1, -1);
	// 	j_head.jewelry_color >>= 1;
	// 
	// 	{
	// 		char * info = new char[2048];
	// 		sprintf(info, "Type: %d | Level: %d | Color: %d", j_head.jewelry_type, j_head.jewelry_level, j_head.jewelry_color);
	// 		m_role_module_mgr->GetRole()->PrintToScreen(info);
	// 		delete[] info;
	// 	}
	// 
	// 	const int jewelry_id = LOGIC_CONFIG->GetJewelryConfig()->SearchLegalJewelry(j_head, j_item_1->GetAttrAddType(), j_item_2->GetAttrAddType());
	// 	const ItemBase * j_item = ITEMPOOL->GetItem(jewelry_id);
	// 	if (NULL == j_item || I_TYPE_JEWELRY != j_item->GetItemType())
	// 	{
	// 		m_role_module_mgr->GetRole()->PrintToScreen("Error, JewelryID Invaild.");
	// 		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
	// 		return;
	// 	}
	// 
	// 	Jewelry * j_target = (Jewelry *)j_item;
	// 	if (NULL == j_target) return;
	// 
	// 	static NetValueItemParam tmp_ip; tmp_ip.Reset();
	// 	if (!j_target->RandomNetValueParamReforge(&tmp_ip.jewelry_param, mate_1.param->jewelry_param.lyrics_id, mate_2.param->jewelry_param.lyrics_id, 
	// 		mate_1.param->jewelry_param.anima_val, mate_2.param->jewelry_param.anima_val))
	// 	{
	// 		m_role_module_mgr->NoticeNum(errornum::EN_WORKSHOP_WITHOUT_LEGAL_RESULTS);
	// 		return;
	// 	}
	// 
	// 	// 制作者署名
	// 	m_role_module_mgr->GetRole()->GetName(tmp_ip.jewelry_param.creator_name);
	// 
	// 	// 消耗材料
	// 	short column_list[2] = { 0, 0 }, grid_list[2] = { (short)mate_grid_1, (short)mate_grid_2 }, num_list[2] = { 1, 1 };
	// 	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(2, column_list, grid_list, num_list, "OnWorkshopJewelryReforge"))
	// 	{
	// 		m_role_module_mgr->GetRole()->PrintToScreen("Error, Consume Failed.");
	// 		return;
	// 	}
	// 
	// 	// 存入数据库
	// 	JewelryPro save_param; save_param.Reset();
	// 
	// 	save_param.jewelry_item = mate_1.item_id;
	// 	save_param.jewelry_type = j_item_1->GetJewelryType();
	// 	save_param.j_param = mate_1.param->jewelry_param;
	// 	m_workshop_param.jewelry_reforge_list[JEWELRY_REFORGE_MATE_1] = save_param;
	// 
	// 	save_param.jewelry_item = mate_2.item_id;
	// 	save_param.jewelry_type = j_item_2->GetJewelryType();
	// 	save_param.j_param = mate_2.param->jewelry_param;
	// 	m_workshop_param.jewelry_reforge_list[JEWELRY_REFORGE_MATE_2] = save_param;
	// 
	// 	save_param.jewelry_item = jewelry_id;
	// 	save_param.jewelry_type = j_target->GetJewelryType();
	// 	save_param.j_param = tmp_ip.jewelry_param;
	// 	m_workshop_param.jewelry_reforge_list[JEWELRY_REFORGE_RESULT] = save_param;
	// 	
	// 	// 发送给客户端，让玩家选择留下其中一个
	// 	this->SendJewelryReforgeOutcome();
	// 	m_role_module_mgr->GetRole()->PrintToScreen("Succ.");
}

void Workshop::ChooseReforgeRetain(int list_idx)
{
	// 	if (list_idx < 0 || list_idx >= JEWELTY_REFORGE_LIST_MAX) return;
	// 	
	// 	const JewelryPro & obtain = m_workshop_param.jewelry_reforge_list[list_idx];
	// 
	// 	const ItemBase * o_ib = ITEMPOOL->GetItem(obtain.jewelry_item);
	// 	if (NULL == o_ib || I_TYPE_JEWELRY != o_ib->GetItemType())
	// 	{
	// 		for (int t_cur = 0; t_cur < JEWELTY_REFORGE_LIST_MAX; ++t_cur)
	// 		{
	// 			m_workshop_param.jewelry_reforge_list[t_cur].Reset();
	// 		}
	// 
	// 		return;
	// 	}
	// 
	// 	ItemDataWrapper obtain_item; obtain_item.Reset();
	// 	NetValueItemParam tmp_ip; tmp_ip.Reset();
	// 	tmp_ip.jewelry_param = obtain.j_param;
	// 	tmp_ip.GetData(obtain_item.param_data);
	// 
	// 	obtain_item.item_id = obtain.jewelry_item;
	// 	obtain_item.num = 1;
	// 	obtain_item.is_bind = 0;
	// 	obtain_item.has_param = 1;
	// 
	// 	if (!m_role_module_mgr->GetKnapsack()->Put(obtain_item, PUT_REASON_WORKSHOP_COMPOUND)) return;
	// 
	// 	for (int t_cur = 0; t_cur < JEWELTY_REFORGE_LIST_MAX; ++t_cur)	// 重置
	// 	{
	// 		m_workshop_param.jewelry_reforge_list[t_cur].Reset();
	// 	}
	// 
	// 	this->SendJewelryReforgeOutcome();
}

void Workshop::RefreshPetEquipMent(int compound_seq, int bag_index1)
{
	const ItemGridData * tmp_equip1 = NULL;
	tmp_equip1 = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index1);
	if (NULL == tmp_equip1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
		return;
	}

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(tmp_equip1->item_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}
	const PetEquipment * pet_equip_ib = (const PetEquipment *)tmp_itembase;
	const PetEquipMentRefiningGroup * refresh_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetPetEquipMentRefiningCfg(compound_seq);
	if (refresh_cfg == NULL)return;

	if (refresh_cfg->id != tmp_equip1->item_id)return;

	PetEquipMentParam &pet_equip_param = tmp_equip1->param->pet_equipment_param;
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(refresh_cfg->item_id, refresh_cfg->number, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	std::string old_param = tmp_equip1->param->GetParamLog(I_TYPE_PET_EQUIPMENT);
	//获取随机配置

	if (pet_equip_param.equipment_skill_num == 2)
	{
		//第一个技能
		const PetEquipMentSkillGroup * skill_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetSkillRandMap(pet_equip_ib->GetSkillGroup());
		if (skill_cfg != NULL)
		{
			if (skill_cfg->weight_count > 0)
			{
				int new_rand_num = skill_cfg->weight_count;
				int rand_num = rand() % new_rand_num;
				for (std::vector<PetEquipMentSkill>::const_iterator itor = skill_cfg->rand_vec.begin(); itor != skill_cfg->rand_vec.end(); itor++)
				{
					if (itor->weight >= rand_num)
					{
						pet_equip_param.equipment_skill_refresh_list[0] = itor->gift_id;
						new_rand_num -= itor->weight;
						break;
					}
					else
					{
						rand_num -= itor->weight;
					}
				}
			}
		}
		else
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "Workshop::RefreshPetEquipMent err [PetEquipMentSkill] m_skill_group[%d]", pet_equip_ib->GetSkillGroup());
		}

		const PetEquipMentSkillGroup * skill_cfg2 = LOGIC_CONFIG->GetPetEquipMentConfig()->GetSkillRandMap(pet_equip_ib->GetSkillGroup2());
		if (skill_cfg2 != NULL)
		{
			if (skill_cfg2->weight_count > 0)
			{
				int new_rand_num = skill_cfg2->weight_count;
				int rand_num = rand() % new_rand_num;
				for (std::vector<PetEquipMentSkill>::const_iterator itor = skill_cfg2->rand_vec.begin(); itor != skill_cfg2->rand_vec.end(); itor++)
				{
					if (itor->weight >= rand_num)
					{
						pet_equip_param.equipment_skill_refresh_list[1] = itor->gift_id;
						new_rand_num -= itor->weight;
						break;
					}
					else
					{
						rand_num -= itor->weight;
					}
				}
			}
		}
		else
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "Workshop::RefreshPetEquipMent err [PetEquipMentSkill] m_skill_group2[%d]", pet_equip_ib->GetSkillGroup2());
		}
	}
	else
	{
		const PetEquipMentSkillGroup * skill_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetSkillRandMap(pet_equip_ib->GetSkillGroup());
		if (skill_cfg == NULL)
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "Workshop::RefreshPetEquipMent err [PetEquipMentSkill] m_skill_group[%d]", pet_equip_ib->GetSkillGroup());
			return;
		}

		std::vector<PetEquipMentSkill> new_rand_vec;
		new_rand_vec.assign(skill_cfg->rand_vec.begin(), skill_cfg->rand_vec.end());
		if (skill_cfg->weight_count > 0)
		{
			int new_rand_num = skill_cfg->weight_count;
			for (int i = 0; i < pet_equip_param.equipment_skill_num && i < MAX_PET_EQUIPMENT_SKILL_NUM; i++)
			{
				int rand_num = rand() % new_rand_num;
				for (std::vector<PetEquipMentSkill>::iterator itor = new_rand_vec.begin(); itor != new_rand_vec.end(); itor++)
				{
					if (itor->weight >= rand_num)
					{
						pet_equip_param.equipment_skill_refresh_list[i] = itor->gift_id;
						new_rand_num -= itor->weight;
						new_rand_vec.erase(itor);
						break;
					}
					else
					{
						rand_num -= itor->weight;
					}
				}
			}
		}
	}

	m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index1, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::RefreshPetEquipMent refresh_id[%d] bag_idx[%d] old_param[%s] new_param[%s]", tmp_equip1->item_id, bag_index1, old_param.c_str(), tmp_equip1->param->GetParamLog(I_TYPE_PET_EQUIPMENT));
}

void Workshop::SaveRefreshPetEquipment(int bag_index1)
{
	const ItemGridData * tmp_equip1 = NULL;
	tmp_equip1 = m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index1);
	if (NULL == tmp_equip1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
		return;
	}

	const ItemBase * tmp_itembase = ITEMPOOL->GetItem(tmp_equip1->item_id);
	if (tmp_itembase == NULL || tmp_itembase->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	PetEquipMentParam &pet_equip_param = tmp_equip1->param->pet_equipment_param;

	if (pet_equip_param.equipment_skill_refresh_list[0] == 0 && pet_equip_param.equipment_skill_refresh_list[1] == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	std::string old_param = tmp_equip1->param->GetParamLog(I_TYPE_PET_EQUIPMENT);

	for (int i = 0; i < pet_equip_param.equipment_skill_num; i++)
	{
		pet_equip_param.equipment_skill_list[i] = pet_equip_param.equipment_skill_refresh_list[i];
		pet_equip_param.equipment_skill_refresh_list[i] = 0;//保存了之后就清理掉
	}

	m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index1, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
}

void Workshop::RefreshPetEquipMentInPet(int compound_seq, int pet_index)
{
	PetEquipMentParam out_data;
	m_role_module_mgr->GetPet()->RefreshPetEquipment(compound_seq, pet_index, &out_data);

}

void Workshop::SaveRefreshPetEquipmentInPet(int pet_index)
{
	PetEquipMentParam out_data;
	m_role_module_mgr->GetPet()->SaveRefreshPetEquipment(pet_index, &out_data);
}

void Workshop::CompoundPetEquipMent(int compound_seq, int bag_index1, int bag_index2)
{
	const PetEquipMentCompoundItemGroup * p_cc = LOGIC_CONFIG->GetPetEquipMentConfig()->GetPetEquipMentCompound(compound_seq);
	if (NULL == p_cc) return;
	const  PetEquipMentCompoundItemRand * target_cfg = LOGIC_CONFIG->GetPetEquipMentConfig()->GetPetEquipMentRandCompoundItem(p_cc->target);
	if (NULL == target_cfg)return;

	ItemGridData tmp_equip_list[PetEquipMentCompoundItemGroup::MAX_PET_EQUIPMENT_COMPOUND_NUM]; tmp_equip_list[0].Reset(); tmp_equip_list[1].Reset();

	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index1, &tmp_equip_list[0]))return;

	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index2, &tmp_equip_list[1]))return;

	const ItemBase * item_base_list_1 = ITEMPOOL->GetItem(tmp_equip_list[0].item_id);
	const ItemBase * item_base_list_2 = ITEMPOOL->GetItem(tmp_equip_list[1].item_id);
	if (item_base_list_1 == NULL || item_base_list_1->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		return;
	}

	if (item_base_list_2 == NULL || item_base_list_2->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		return;
	}

	// 获取材料列表
	bool need_item_list[PetEquipMentCompoundItemGroup::MAX_PET_EQUIPMENT_COMPOUND_NUM] = { false,false };
	bool bag_item_list[PetEquipMentCompoundItemGroup::MAX_PET_EQUIPMENT_COMPOUND_NUM] = { false,false };//被标记，已经符合了被占用的
	for (int i = 0; i < PetEquipMentCompoundItemGroup::MAX_PET_EQUIPMENT_COMPOUND_NUM; ++i)
	{
		if (p_cc->stuff_id_list[i] != 0)
		{
			for (int j = 0; j < PetEquipMentCompoundItemGroup::MAX_PET_EQUIPMENT_COMPOUND_NUM; ++j)
			{
				if (!bag_item_list[j] && p_cc->stuff_id_list[i] == tmp_equip_list[j].item_id)
				{
					need_item_list[i] = true;
					bag_item_list[j] = true;
					break;
				}
			}
		}
		else
		{
			need_item_list[i] = true;
		}
	}

	if (!need_item_list[0] || !need_item_list[1])
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}
	//检测放入
	const ItemBase * p_ib = ITEMPOOL->GetItem(target_cfg->target);
	if (NULL == p_ib || I_TYPE_PET_EQUIPMENT != p_ib->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(target_cfg->target, 1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	//消耗
	m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index1, 1, __FUNCTION__);
	m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index2, 1, __FUNCTION__);

	ItemDataWrapper node_item;
	node_item.item_id = target_cfg->target;
	node_item.num = 1;
	node_item.is_bind = 1;

	NetValueItemParam tmp_item_param;
	tmp_item_param.SetData(node_item.param_data);
	if (!p_ib->HasRandomNetValue(&tmp_item_param))
	{
		p_ib->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &tmp_item_param, PUT_REASON_WORKSHOP_COMPOUND);
		p_ib->SetRandomNetValueFlag(&tmp_item_param);
		tmp_item_param.GetData(node_item.param_data);
	}
	node_item.has_param = 1;

	if (!m_role_module_mgr->GetKnapsack()->Put(node_item, PUT_REASON_NO_NOTICE))
	{
		gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::CompoundPetEquipMent | Materials Consume Succeed, But Composition Put Failed. -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld ]",
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), node_item.item_id, node_item.num, (long long)EngineAdapter::Instance().Time());
	}

	this->SendComposeResult(true, node_item);

	if (p_ib->GetColor() >= ItemBase::I_COLOR_RED)
	{
		int get_item_id = p_ib->GetItemId();
		bool is_in_stuff = false;
		for (int i = 0; i < ARRAY_LENGTH(p_cc->stuff_id_list); ++i)
		{
			if (get_item_id == p_cc->stuff_id_list[i])
			{
				is_in_stuff = true;
				break;
			}
		}

		if (!is_in_stuff)
		{
			// 传闻
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_compound_pet_equipment_broadcast, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), get_item_id);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
			}
		}
	}
}

void Workshop::BreakPetEquipMent(int bag_index)
{
	ItemGridData tmp_equip; tmp_equip.Reset();

	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index, &tmp_equip))return;

	const ItemBase * item_base = ITEMPOOL->GetItem(tmp_equip.item_id);

	if (item_base == NULL || item_base->GetItemType() != I_TYPE_PET_EQUIPMENT)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	PetEquipment * pet_equip = (PetEquipment*)item_base;
	int break_item = pet_equip->GetBreakItem();
	int break_num = pet_equip->GetBreakNum();
	bool break_isbind = tmp_equip.is_bind;
	if (break_item == 0 || break_num == 0)
	{

		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//消耗
	m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, bag_index, 1, __FUNCTION__);

	m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(break_item, break_isbind, break_num), PUT_REASON_WORKSHOP_COMPOUND);

	m_role_module_mgr->NoticeNum(noticenum::NT_WORKSHOP_PET_BREAK_SUCC);
}

void Workshop::SendAllWorkshopInfo()
{
	static Protocol::SCWorkshopAllInfoRespond wair;

	wair.voc_refresh_times = m_role_module_mgr->GetCommonData().equip_refresh_voc_times;
	wair.sp_active_flag = m_workshop_param.sp_recipe_active_flag;
	wair.sp_created_flag = m_workshop_param.sp_recipe_created_flag;

	m_role_module_mgr->NetSend((const char *)&wair, sizeof(wair));
	this->SendJewelryTimesInfo();
}

void Workshop::OnRoleLogin()
{
	this->GodEquipReback();
	this->GodEquipChange();
	this->FixGodEquipBaseVoc();
	for (int t_cur = 0; t_cur < JEWELTY_REFORGE_LIST_MAX; ++t_cur)
	{
		if (0 == m_workshop_param.jewelry_reforge_list[t_cur].jewelry_item) return;
	}
	this->SendJewelryReforgeOutcome();
}

void Workshop::OnActiveEquipVocAttr(int item_type, int equip_posi_1, int equip_grid_1, int voc_posi_1, int role_type)
{
	if (role_type < 0 || role_type >= ROLE_TYPE_MAX) return;
	if (item_type < 0 || item_type >= EQUIP_TYPE_MAX)return;
	if (voc_posi_1 < 0 || voc_posi_1 >= EquipmentParam::EXA_ATTR_NUM_MAX)return;
	ItemGridData tmp_equip; tmp_equip.Reset();

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
	{
		if (item_type == EQUIP_TYPE_EQUIPMENT)//装备
		{
			if (!m_role_module_mgr->GetEquipmentManager()->GetEquipmentGridData(equip_grid_1, &tmp_equip, role_type)) return;
		}
		else
		{
			if (!m_role_module_mgr->GetEquipmentManager()->GetJewelryGridData(equip_grid_1, &tmp_equip, role_type)) return;
		}
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1, &tmp_equip)) return;
	}

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip.item_id);

	if (NULL == equip_ib || (equip_ib->GetItemType() != I_TYPE_EQUIPMENT && I_TYPE_JEWELRY != equip_ib->GetItemType()) || NULL == tmp_equip.param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	//检查对应孔位满足条件
	if (equip_ib->GetColor() >= ItemBase::I_COLOR_STAR && equip_ib->GetColor() <= ItemBase::I_COLOR_GOD)//神装
	{

		if (equip_ib->GetItemType() == I_TYPE_EQUIPMENT)
		{
			const Equipment* equip_ment = (const Equipment*)equip_ib;
			const GodEquipActiveAttrNumConfig* attr_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentAttrVocCfg(equip_ment->GetEquipColor(), voc_posi_1);
			if (NULL != attr_num_cfg)
			{
				if (m_role_module_mgr->GetRole()->GetLevel() < attr_num_cfg->role_level)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_FUN_OPEN_ROLE_LEVEL_LIMIT);
					return;
				}
				if (tmp_equip.param->equip_param.strengthen_level < attr_num_cfg->godequip_level)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GOD_EQUIP_NO_LEVEL);
					return;
				}

				const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(tmp_equip.param->equip_param.strengthen_level, equip_ment->GetEquipColor());
				if (max_num_cfg != NULL && max_num_cfg->max_num <= EquipmentParam::EXA_ATTR_NUM_MAX)tmp_equip.param->equip_param.inc_num = tmp_equip.param->equip_param.inc_num >= max_num_cfg->max_num ? tmp_equip.param->equip_param.inc_num : max_num_cfg->max_num;
			}
		}
		else if (equip_ib->GetItemType() == I_TYPE_JEWELRY)
		{
			const Jewelry* equip_ment = (const Jewelry*)equip_ib;
			const GodEquipActiveAttrNumConfig* attr_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentAttrVocCfg(equip_ment->GetEquipColor(), voc_posi_1);
			if (NULL != attr_num_cfg)
			{
				if (m_role_module_mgr->GetRole()->GetLevel() < attr_num_cfg->role_level)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_FUN_OPEN_ROLE_LEVEL_LIMIT);
					return;
				}
				if (tmp_equip.param->jewelry_param.strengthen_level < attr_num_cfg->godequip_level)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GOD_EQUIP_NO_LEVEL);
					return;
				}

				const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(tmp_equip.param->jewelry_param.strengthen_level, equip_ment->GetEquipColor());
				if (max_num_cfg != NULL && max_num_cfg->max_num <= JewelryParam::EXA_ATTR_NUM_MAX)tmp_equip.param->jewelry_param.inc_num = tmp_equip.param->jewelry_param.inc_num >= max_num_cfg->max_num ? tmp_equip.param->jewelry_param.inc_num : max_num_cfg->max_num;
			}
		}
	}
	else
	{
		if (equip_ib->GetItemType() == I_TYPE_EQUIPMENT)
		{
			const Equipment* equip_ment = (const Equipment*)equip_ib;
			const MaxAttrNumCfg * entry_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(equip_ment->GetEquipLevel(), equip_ment->GetEquipColor());
			if (entry_cfg == NULL)return;
			if (m_role_module_mgr->GetRole()->GetLevel() < entry_cfg->role_level)
			{
				return;
			}
			
			const MaxAttrNumCfg *attr_max_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(equip_ment->GetEquipLevel(), equip_ment->GetEquipColor());
			if (attr_max_info != NULL)
			{
				tmp_equip.param->equip_param.inc_num = attr_max_info->max_num;
			}
		}
		else if (equip_ib->GetItemType() == I_TYPE_JEWELRY)
		{
			const Jewelry* equip_ment = (const Jewelry*)equip_ib;
			const MaxAttrNumCfg * entry_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(equip_ment->GetJewelryLevel(), equip_ment->GetEquipColor());
			if (entry_cfg == NULL)return;
			if (m_role_module_mgr->GetRole()->GetLevel() < entry_cfg->role_level)
			{
				return;
			}

			const MaxAttrNumCfg *attr_max_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrMaxNumConfig(equip_ment->GetJewelryLevel(), equip_ment->GetEquipColor());
			if (attr_max_info != NULL)
			{
				tmp_equip.param->jewelry_param.inc_num = attr_max_info->max_num;
			}
		}
	}

	//消耗
	const WorkshopSmeltPrice * smelt_cfg = LOGIC_CONFIG->GetEntryConfig()->GetSmeltPriceCfg();
	if (smelt_cfg == NULL)return;

	if (smelt_cfg->price_type == CURRENCY_TYPE_COIN_BIND)
	{
		if (smelt_cfg->smelt_price > 0 && !m_role_module_mgr->GetMoney()->CoinBindMoreThan(smelt_cfg->smelt_price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}
	else if (smelt_cfg->price_type == CURRENCY_TYPE_GOLD)
	{
		if (smelt_cfg->smelt_price > 0 && !m_role_module_mgr->GetMoney()->GoldMoreThan(smelt_cfg->smelt_price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}
	else
	{
		return;
	}

	//消耗
	if (smelt_cfg->price_type == CURRENCY_TYPE_COIN_BIND)
	{
		if (smelt_cfg->smelt_price > 0)m_role_module_mgr->GetMoney()->UseCoinBind(smelt_cfg->smelt_price, __FUNCTION__);
	}
	else if (smelt_cfg->price_type == CURRENCY_TYPE_GOLD)
	{
		if (smelt_cfg->smelt_price > 0)m_role_module_mgr->GetMoney()->UseGold(smelt_cfg->smelt_price, __FUNCTION__);
	}
	else
	{
		return;
	}

	int att_quality = 0;		// 获取属性品质

	if (equip_ib->GetItemType() == I_TYPE_EQUIPMENT)
	{
		EquipmentParam & p_ep1 = tmp_equip.param->equip_param;

		//检查是否已经被解锁了
		if (voc_posi_1 > 0)
		{
			if (p_ep1.attr_extra_inc[voc_posi_1 - 1].un_lock == 0)//前一个未解锁
			{
				return;
			}
		}

		if (p_ep1.attr_extra_inc[voc_posi_1].un_lock != 0 && p_ep1.attr_extra_inc[voc_posi_1].attr_value > 0)
		{
			return;
		}

		if (p_ep1.inc_num < voc_posi_1 + 1)return;

		p_ep1.attr_extra_inc[voc_posi_1].un_lock = 1;
		const Equipment * equip_item = (const Equipment *)equip_ib;

		int entry_aw_list[EquipmentParam::EXA_ATTR_NUM_MAX] = { -1,-1,-1,-1,-1,-1 };//装备已有词条
		for (int i = 0; i < p_ep1.inc_num && i < EquipmentParam::EXA_ATTR_NUM_MAX; i++)
		{
			if (i != voc_posi_1 && p_ep1.attr_extra_inc[i].attr_value > 0)
			{
				entry_aw_list[i] = p_ep1.attr_extra_inc[i].attr_type;
			}
		}
		if (equip_item->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep1.strengthen_level, equip_item->GetEquipColor());
			if (max_num_cfg != NULL)p_ep1.inc_num = p_ep1.inc_num >= max_num_cfg->max_num ? p_ep1.inc_num : max_num_cfg->max_num;
		}

		if (p_ep1.inc_num > 0)
		{
			RandGroupCfg entry_type_list[EquipmentParam::EXA_ATTR_NUM_MAX];
			int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(equip_item->GetEntryGroup(), p_ep1.inc_num, entry_type_list, entry_aw_list);
			if (real_entry_num > 0)
			{
				const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(equip_item->GetEquipLevel(), entry_type_list[0].quality, entry_type_list[0].att_type);
				if (attr_info != NULL)
				{
					int attr_value = 0;
					if (((attr_info->max - attr_info->min) + 1) > 0)attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
					else attr_value = attr_info->min;
					if (attr_value > 0)
					{
						AttrVocInfo & attr_inc = p_ep1.attr_extra_inc[voc_posi_1];
						if (attr_inc.un_lock == 1)
						{
							attr_inc.attr_type = entry_type_list[0].att_type;
							attr_inc.attr_value = attr_value;
						}
					}
				}

				att_quality = attr_info->quality;		// 获取属性品质
			}
			if (p_ep1.inc_num > EquipmentParam::EXA_ATTR_NUM_MAX)p_ep1.inc_num = EquipmentParam::EXA_ATTR_NUM_MAX;
		}
		else
		{
			m_role_module_mgr->NoticeNum(errornum::EN_NO_INS_NUM);
			return;
		}

		//检查是否有重复词条
		for (int same_index = 0; same_index < 6; same_index++)
		{
			AttrVocInfo &voc_info = p_ep1.attr_extra_inc[same_index];
			if (voc_info.attr_type >= 0 && voc_info.attr_value > 0)
			{
				for (int same_index2 = 0; same_index2 < 6; same_index2++)
				{
					AttrVocInfo &voc_info2 = p_ep1.attr_extra_inc[same_index2];
					if (same_index2 != same_index &&
						voc_info2.attr_type > 0 &&
						voc_info2.attr_type == voc_info.attr_type)
					{
						gamelog::g_log_workshop.printf(LL_INFO, "[ Workshop::OnActiveEquipVocAttr same voc -> User: [ %d | %s ] | EquipID: %d | Num: %d | OccurTime: %lld  | Equip_voc_num :%d | Same_attr_type: %d]",
							m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), tmp_equip.item_id, 1, (long long)EngineAdapter::Instance().Time(), p_ep1.inc_num, voc_info.attr_type);
					}
				}
			}
		}

		// 重算属性、发送信息
		if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
		{
			m_role_module_mgr->GetEquipmentManager()->SetEquipGridDirty(equip_grid_1, true, role_type);
			m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_EQUIPMENT, equip_grid_1, &tmp_equip, role_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}
	else if (I_TYPE_JEWELRY == equip_ib->GetItemType())
	{
		JewelryParam & p_ep1 = tmp_equip.param->jewelry_param;
		//检查是否已经被解锁了
		if (voc_posi_1 > 0)
		{
			if (p_ep1.attr_extra_inc[voc_posi_1 - 1].un_lock == 0)//前一个未解锁
			{
				return;
			}
		}

		if (p_ep1.attr_extra_inc[voc_posi_1].un_lock != 0)
		{
			return;
		}

		if (p_ep1.inc_num < voc_posi_1 + 1)return;

		p_ep1.attr_extra_inc[voc_posi_1].un_lock = 1;

		int entry_aw_list[JewelryParam::EXA_ATTR_NUM_MAX] = { -1,-1,-1,-1,-1,-1 };//装备已有词条
		for (int i = 0; i < p_ep1.inc_num && i < JewelryParam::EXA_ATTR_NUM_MAX; i++)
		{
			if (i != voc_posi_1 && p_ep1.attr_extra_inc[i].attr_value > 0)
			{
				entry_aw_list[i] = p_ep1.attr_extra_inc[i].attr_type;
			}
		}

		p_ep1.attr_extra_inc[voc_posi_1].un_lock = 1;
		const Jewelry * j_item = (const Jewelry *)equip_ib;
		if (j_item->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep1.strengthen_level, j_item->GetColor());
			if (max_num_cfg != NULL)p_ep1.inc_num = p_ep1.inc_num >= max_num_cfg->max_num ? p_ep1.inc_num : max_num_cfg->max_num;
		}

		if (p_ep1.inc_num > 0)//是否有词条
		{
			RandGroupCfg entry_type_list[JewelryParam::EXA_ATTR_NUM_MAX];
			int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(j_item->GetEntryGroup(), p_ep1.inc_num, entry_type_list, entry_aw_list);
			if (real_entry_num > 0)
			{
				const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(j_item->GetJewelryLevel(), entry_type_list[0].quality, entry_type_list[0].att_type);
				if (attr_info != NULL)
				{
					int attr_value = 0;
					if (((attr_info->max - attr_info->min) + 1) > 0)attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
					else attr_value = attr_info->min;
					if (attr_value > 0)
					{
						AttrVocInfo & attr_inc = p_ep1.attr_extra_inc[voc_posi_1];
						if (attr_inc.un_lock == 1)
						{
							attr_inc.attr_type = entry_type_list[0].att_type;
							attr_inc.attr_value = attr_value;
						}
					}
				}

				att_quality = attr_info->quality;		// 获取属性品质
			}

			if (p_ep1.inc_num > JewelryParam::EXA_ATTR_NUM_MAX)p_ep1.inc_num = JewelryParam::EXA_ATTR_NUM_MAX;
		}
		else
		{
			m_role_module_mgr->NoticeNum(errornum::EN_NO_INS_NUM);
			return;
		}

		// 重算属性、发送信息
		if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
		{
			m_role_module_mgr->GetEquipmentManager()->SetJewelryGridDirty(equip_grid_1, true, role_type);
			m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_JEWELRY, equip_grid_1, &tmp_equip, role_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

// 	int p1 = 0, p2 = 0, p3 = 0;
// 	if (att_quality == ItemBase::I_COLOR_PURPLE )
// 	{
// 		p1++;
// 	}
// 	else if (att_quality == ItemBase::I_COLOR_ORANGE)
// 	{
// 		p2++;
// 	}
// 	else if (att_quality >= ItemBase::I_COLOR_RED < ItemBase::I_COLOR_MAX)
// 	{
// 		p3++;
// 	}
	if (att_quality >= ItemBase::I_COLOR_PURPLE)
	{
		EventHandler::Instance().OnEquipmentSmeltHigh(m_role_module_mgr->GetRole(), att_quality, 1);
	}

	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::OnActiveEquipVocAttr user[%d,%s] weapon_id{[%d] param: [%s]}",
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
		tmp_equip.item_id, tmp_equip.param->GetParamLog(equip_ib->GetItemType()));
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1 && ROLE_TYPE_BENZUN == role_type)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}
}

void Workshop::OnRefreshEquipVocAttr(int item_type, int equip_posi_1, int equip_grid_1, int lock_voc_flag, int role_type, bool use_gold)
{
	if (role_type < 0 || role_type >= ROLE_TYPE_MAX) return;
	if (item_type < 0 || item_type >= EQUIP_TYPE_MAX)return;
	if (lock_voc_flag == 63)return;//6个位置都锁上了还洗什么

	int lock_num = 0;
	for (int i = 0; i < MAX_EQUIP_LOCK_NUM; i++)
	{
		if ((lock_voc_flag & (1 << i)) > 0)
		{
			lock_num += 1;
		}
	}

	const WorkshopLockPrice * lock_cfg = LOGIC_CONFIG->GetEntryConfig()->GetLockPriceCfg(lock_num);
	if (lock_cfg == NULL)return;

	if (lock_cfg->level != lock_num)return;

	int real_use_coin = 0;
	int real_use_gold = 0;

	if (use_gold)
	{
		if (lock_cfg->ex_coin > 0)
		{
			real_use_gold += lock_cfg->ex_coin;
		}
	}
	else
	{
		if (lock_cfg->coin > 0)
		{
			real_use_coin += lock_cfg->coin;
		}

		const EntryOtherConfig & other_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryOtherCfg();
		if (m_role_module_mgr->GetCommonData().equip_refresh_voc_times + 1 > other_cfg.coin_smelt_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ENTRY_COIN_USE_TIMES_NOT_ENOUGH);
			return;
		}
	}

	if (lock_cfg->gold > 0)
	{
		real_use_gold += lock_cfg->gold;
	}

	//检查
	if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(real_use_coin))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}
	if (!m_role_module_mgr->GetMoney()->GoldMoreThan(real_use_gold))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	ItemGridData tmp_equip; tmp_equip.Reset();

	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
	{
		if (item_type == EQUIP_TYPE_EQUIPMENT)//装备
		{
			if (!m_role_module_mgr->GetEquipmentManager()->GetEquipmentGridData(equip_grid_1, &tmp_equip, role_type)) return;
		}
		else
		{
			if (!m_role_module_mgr->GetEquipmentManager()->GetJewelryGridData(equip_grid_1, &tmp_equip, role_type)) return;
		}
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1, &tmp_equip)) return;
	}

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip.item_id);

	if (NULL == equip_ib || (equip_ib->GetItemType() != I_TYPE_JEWELRY && I_TYPE_EQUIPMENT != equip_ib->GetItemType()) || NULL == tmp_equip.param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (equip_ib->GetItemType() == I_TYPE_EQUIPMENT)
	{
		int entry_aw_list[EquipmentParam::EXA_ATTR_NUM_MAX] = { -1,-1,-1,-1,-1,-1 };//装备已有词条
		EquipmentParam & p_ep1 = tmp_equip.param->equip_param;

		if (p_ep1.attr_extra_inc[0].un_lock != 1)
		{
			//第一个都未解锁，没激活的
			return;
		}
		const Equipment * equip_item = (const Equipment *)equip_ib;
		if (equip_item->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep1.strengthen_level, equip_item->GetEquipColor());
			if (max_num_cfg != NULL)p_ep1.inc_num = p_ep1.inc_num >= max_num_cfg->max_num ? p_ep1.inc_num : max_num_cfg->max_num;
		}
		for (int i = 0; i < p_ep1.inc_num && i < EquipmentParam::EXA_ATTR_NUM_MAX; i++)
		{
			p_ep1.refresh_attr_extra_inc[i].un_lock = 1;
			if ((lock_voc_flag & (1 << i)) > 0)
			{
				entry_aw_list[i] = p_ep1.attr_extra_inc[i].attr_type;
			}
		}

		if (p_ep1.inc_num > 0)
		{
			RandGroupCfg entry_type_list[EquipmentParam::EXA_ATTR_NUM_MAX];
			int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(equip_item->GetEntryGroup(), p_ep1.inc_num, entry_type_list, entry_aw_list);
			if (real_entry_num > 0)//获得可随机词条
			{
				int real_entry_index = 0;
				for (int attr_type_index = 0; real_entry_index < real_entry_num && attr_type_index < EquipmentParam::EXA_ATTR_NUM_MAX; attr_type_index++)
				{
					AttrVocInfo & attr_inc = p_ep1.refresh_attr_extra_inc[attr_type_index];
					if ((lock_voc_flag & (1 << attr_type_index)) > 0 || p_ep1.attr_extra_inc[attr_type_index].un_lock != 1)//不满足的清空
					{
						attr_inc.un_lock = 0;
						attr_inc.attr_type = 0;
						attr_inc.attr_value = 0;
						continue;
					}

					if (entry_type_list[real_entry_index].att_type < 0)continue;

					const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3(equip_item->GetEquipLevel(), entry_type_list[real_entry_index].quality, entry_type_list[real_entry_index].att_type);
					if (attr_info != NULL)
					{
						//if (p_ep1.inc_num >= EquipmentParam::EXA_ATTR_NUM_MAX) break;
						if (((attr_info->max - attr_info->min) + 1) <= 0)continue;
						int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
						if (attr_value <= 0)
						{
							continue;
						}
						if (attr_inc.un_lock != 1)continue;
						attr_inc.attr_type = entry_type_list[real_entry_index].att_type;
						attr_inc.attr_value = attr_value;
						real_entry_index++;
					}
					else
					{
						attr_inc.un_lock = 0;
						attr_inc.attr_type = 0;
						attr_inc.attr_value = 0;
						continue;
					}
				}
			}
		}

		// 重算属性、发送信息
		if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
		{
			m_role_module_mgr->GetEquipmentManager()->SetEquipGridDirty(equip_grid_1, true, role_type);
			m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_EQUIPMENT, equip_grid_1, &tmp_equip, role_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}
	else if (I_TYPE_JEWELRY == equip_ib->GetItemType())
	{
		int entry_aw_list[EquipmentParam::EXA_ATTR_NUM_MAX] = { -1,-1,-1,-1,-1,-1 };//装备已有词条
		JewelryParam & p_ep1 = tmp_equip.param->jewelry_param;
		if (p_ep1.attr_extra_inc[0].un_lock != 1)
		{
			//第一个都未解锁，没激活的
			return;
		}

		const Jewelry * j_item = (const Jewelry *)equip_ib;
		if (j_item->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep1.strengthen_level, j_item->GetEquipColor());
			if (max_num_cfg != NULL)p_ep1.inc_num = p_ep1.inc_num >= max_num_cfg->max_num ? p_ep1.inc_num : max_num_cfg->max_num;
		}
		for (int i = 0; i < p_ep1.inc_num && i < EquipmentParam::EXA_ATTR_NUM_MAX; i++)
		{
			p_ep1.refresh_attr_extra_inc[i].un_lock = 1;
			if ((lock_voc_flag & (1 << i)) > 0)
			{
				entry_aw_list[i] = p_ep1.attr_extra_inc[i].attr_type;
			}
		}
		if (p_ep1.inc_num > 0)//是否有词条
		{
			RandGroupCfg entry_type_list[JewelryParam::EXA_ATTR_NUM_MAX];
			int real_entry_num = LOGIC_CONFIG->GetEntryConfig()->GetEntryRandConfig(j_item->GetEntryGroup(), p_ep1.inc_num, entry_type_list, entry_aw_list);
			if (real_entry_num > 0)
			{
				int real_entry_index = 0;
				for (int attr_type_index = 0; real_entry_index < real_entry_num && attr_type_index < JewelryParam::EXA_ATTR_NUM_MAX; attr_type_index++)
				{
					if (entry_type_list[real_entry_index].att_type < 0)continue;

					AttrVocInfo & attr_inc = p_ep1.refresh_attr_extra_inc[attr_type_index];
					if ((lock_voc_flag & (1 << attr_type_index)) > 0 || p_ep1.attr_extra_inc[attr_type_index].un_lock != 1)//不满足的清空
					{
						attr_inc.un_lock = 0;
						attr_inc.attr_type = 0;
						attr_inc.attr_value = 0;
						continue;
					}
					const AttValueCfg *attr_info = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig4(j_item->GetJewelryLevel(), entry_type_list[real_entry_index].quality, entry_type_list[real_entry_index].att_type);
					if (attr_info != NULL)
					{
						//if (p_ep1.inc_num >= JewelryParam::EXA_ATTR_NUM_MAX) break;
						int attr_value = rand() % ((attr_info->max - attr_info->min) + 1) + attr_info->min;
						if (attr_value <= 0)continue;

						if (attr_inc.un_lock != 1)continue;

						attr_inc.attr_type = entry_type_list[real_entry_index].att_type;
						attr_inc.attr_value = attr_value;
						real_entry_index++;
					}
					else
					{
						attr_inc.un_lock = 0;
						attr_inc.attr_type = 0;
						attr_inc.attr_value = 0;
						continue;
					}
				}
			}
		}

		// 重算属性、发送信息
		if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi_1)
		{
			m_role_module_mgr->GetEquipmentManager()->SetJewelryGridDirty(equip_grid_1, true, role_type);
			m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_JEWELRY, equip_grid_1, &tmp_equip, role_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid_1, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (real_use_coin > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(real_use_coin, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		m_role_module_mgr->GetCommonData().equip_refresh_voc_times += 1;
		this->SendAllWorkshopInfo();
	}

	if (real_use_gold > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(real_use_gold, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}

	EventHandler::Instance().OnEquipmentSmelt(m_role_module_mgr->GetRole());
	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::OnRefreshEquipVocAttr user[%d,%s] weapon_id{[%d] param: [%s]}",
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
		tmp_equip.item_id, tmp_equip.param->GetParamLog(equip_ib->GetItemType()));
	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_EQUIP_VOC_CHANGE);
	}
}

void Workshop::OnSaveEquipVocAttr(int item_type, int equip_posi, int equip_grid, int role_type)
{
	if (role_type < 0 || role_type >= ROLE_TYPE_MAX) return;
	if (item_type < 0 || item_type >= EQUIP_TYPE_MAX)return;
	ItemGridData tmp_equip; tmp_equip.Reset();
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
	{
		if (item_type == EQUIP_TYPE_EQUIPMENT)//装备
		{
			if (!m_role_module_mgr->GetEquipmentManager()->GetEquipmentGridData(equip_grid, &tmp_equip, role_type)) return;
		}
		else if (item_type == EQUIP_TYPE_JEWELRY)//灵饰
		{
			if (!m_role_module_mgr->GetEquipmentManager()->GetJewelryGridData(equip_grid, &tmp_equip, role_type)) return;
		}
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, &tmp_equip)) return;
	}

	const ItemBase * equip_ib = ITEMPOOL->GetItem(tmp_equip.item_id);
	if (NULL == equip_ib || (equip_ib->GetItemType() != I_TYPE_EQUIPMENT && I_TYPE_JEWELRY != equip_ib->GetItemType()) || NULL == tmp_equip.param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (equip_ib->GetItemType() == I_TYPE_EQUIPMENT)
	{
		EquipmentParam & p_ep1 = tmp_equip.param->equip_param;

		if (p_ep1.attr_extra_inc[0].un_lock != 1)
		{
			//第一个都未解锁，没激活的
			return;
		}

		bool has_refresh = false;
		for (int refresh_idx = 0; refresh_idx < EquipmentParam::EXA_ATTR_NUM_MAX; refresh_idx++)
		{
			if (p_ep1.refresh_attr_extra_inc[refresh_idx].un_lock != 0)
			{
				has_refresh = true;
			}
		}

		if (!has_refresh)
		{
			return;
		}

		//const Equipment * equip_item = (const Equipment *)equip_ib;

		for (int attr_type_index = 0; attr_type_index < EquipmentParam::EXA_ATTR_NUM_MAX; attr_type_index++)
		{
			AttrVocInfo & attr_inc = p_ep1.attr_extra_inc[attr_type_index];
			AttrVocInfo & attr_inc2 = p_ep1.refresh_attr_extra_inc[attr_type_index];
			if (attr_inc2.attr_type <0 || attr_inc2.attr_value <= 0 || attr_inc2.un_lock != 1)continue;
			attr_inc.attr_type = attr_inc2.attr_type;
			attr_inc.attr_value = attr_inc2.attr_value;
			attr_inc2.attr_value = 0;
			attr_inc2.attr_type = 0;
			attr_inc2.un_lock = 0;

			{	// 熔炼有礼
				const AttValueCfg * attr_cfg = NULL;
				const Equipment* equip_ment = (const Equipment*)equip_ib;
				attr_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3ByLevelAndTypeAndValue(equip_ment->GetEquipLevel(), attr_inc.attr_type, attr_inc.attr_value);
				if (NULL == attr_cfg) return;
				if (attr_cfg->quality >= ItemBase::I_COLOR_PURPLE)
				{
					EventHandler::Instance().OnEquipmentSmeltHigh(m_role_module_mgr->GetRole(), attr_cfg->quality, 1);
				}
			}
		}

		// 重算属性、发送信息
		if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
		{
			m_role_module_mgr->GetEquipmentManager()->SetEquipGridDirty(equip_grid, true, role_type);
			m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_EQUIPMENT, equip_grid, &tmp_equip, role_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}
	else if (I_TYPE_JEWELRY == equip_ib->GetItemType())
	{
		JewelryParam & p_ep1 = tmp_equip.param->jewelry_param;
		if (p_ep1.attr_extra_inc[0].un_lock != 1)
		{
			//第一个都未解锁，没激活的
			return;
		}
		bool has_refresh = false;
		for (int refresh_idx = 0; refresh_idx < JewelryParam::EXA_ATTR_NUM_MAX; refresh_idx++)
		{
			if (p_ep1.refresh_attr_extra_inc[refresh_idx].un_lock != 0)
			{
				has_refresh = true;
			}
		}

		if (!has_refresh)
		{
			return;
		}

		//const Jewelry * j_item = (const Jewelry *)equip_ib;
		for (int attr_type_index = 0; attr_type_index < JewelryParam::EXA_ATTR_NUM_MAX; attr_type_index++)
		{
			AttrVocInfo & attr_inc = p_ep1.attr_extra_inc[attr_type_index];
			AttrVocInfo & attr_inc2 = p_ep1.refresh_attr_extra_inc[attr_type_index];
			if (attr_inc2.attr_type <0 || attr_inc2.attr_value <= 0 || attr_inc2.un_lock != 1)continue;
			attr_inc.attr_type = attr_inc2.attr_type;
			attr_inc.attr_value = attr_inc2.attr_value;
			attr_inc2.attr_value = 0;
			attr_inc2.attr_type = 0;
			attr_inc2.un_lock = 0;

			{	// 熔炼有礼
				const AttValueCfg * attr_cfg = NULL;
				const Jewelry* jewelry_ment = (const Jewelry*)equip_ib;
				attr_cfg = LOGIC_CONFIG->GetEntryConfig()->GetEntryAttrValueConfig3ByLevelAndTypeAndValue(jewelry_ment->GetJewelryLevel(), attr_inc.attr_type, attr_inc.attr_value);
				if (NULL == attr_cfg) return;
				if(attr_cfg->quality >= ItemBase::I_COLOR_PURPLE)
				{
					EventHandler::Instance().OnEquipmentSmeltHigh(m_role_module_mgr->GetRole(), attr_cfg->quality, 1);
				}
			}
		}

		// 重算属性、发送信息
		if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi)
		{
			m_role_module_mgr->GetEquipmentManager()->SetJewelryGridDirty(equip_grid, true, role_type);
			m_role_module_mgr->GetEquipmentManager()->SynItemDataPut(I_TYPE_JEWELRY, equip_grid, &tmp_equip, role_type);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, equip_grid, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
		}
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::OnSaveEquipVocAttr user[%d,%s] weapon_id{[%d] param: [%s]}",
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(),
		tmp_equip.item_id, tmp_equip.param->GetParamLog(equip_ib->GetItemType()));
	if (WORKSHOP_EQUIP_POSI_EQUIP == equip_posi && ROLE_TYPE_BENZUN == role_type)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_EQUIPMENT, __FUNCTION__);
	}
}

void Workshop::GmUnlockAllRecipe(bool is_unlock)
{
	int param = is_unlock ? (-1) : 0;
	memset(m_workshop_param.sp_recipe_active_flag.byte_list, param, sizeof(m_workshop_param.sp_recipe_active_flag.byte_list));

	this->SendAllWorkshopInfo();
}

void Workshop::GodEquipReback()
{
	int now_version = LOGIC_CONFIG->GetWorkshopConfig()->GetNowGodEquipReturnVersion();
	if (m_workshop_param.version_god_equip < now_version)
	{
		m_workshop_param.version_god_equip = now_version;
		//遍历玩家装备栏，背包栏
		m_role_module_mgr->GetEquipmentManager()->GodEquipReBack();
		m_role_module_mgr->GetKnapsack()->GodEquipReBack();
	}

	if (m_role_module_mgr->GetCommonData().god_equip_err_flag == 0)
	{
		m_role_module_mgr->GetCommonData().god_equip_err_flag = 1;
		m_role_module_mgr->GetEquipmentManager()->GodEquipBackErrUplevel();
		m_role_module_mgr->GetKnapsack()->CheckGodEquipErrUplevel();
	}
}

bool Workshop::CheckGodEquipItemDataReturn(const ItemGridData * grid)
{
	if (NULL == grid || grid->Invalid()) return false;

	const ItemBase * item_base = ITEMPOOL->GetItem(grid->item_id);
	if (NULL == item_base || item_base->GetColor() < ItemBase::I_COLOR_STAR || item_base->GetColor() > ItemBase::I_COLOR_GOD) return false;
	int level = 0;
	if (item_base->GetItemType() == I_TYPE_EQUIPMENT)
	{
		EquipmentParam & equip_param = grid->param->equip_param;
		level = equip_param.strengthen_level;
	}
	else if (item_base->GetItemType() == I_TYPE_JEWELRY)
	{
		JewelryParam & jewelry_param = grid->param->jewelry_param;
		level = jewelry_param.strengthen_level;
	}

	//神装道具返还
	const WorlshopGodEquipReturn * god_equip_return_cfg = LOGIC_CONFIG->GetWorkshopConfig()->GetGodEquipReturnCfg(item_base->GetColor(), level);
	if (god_equip_return_cfg != NULL)
	{
		if (god_equip_return_cfg->turn_back_reward_vec.size() > 0)
		{
			static MailContentParam contentparam; contentparam.Reset();
			int real_item_num = 0;
			for (std::vector<ItemConfigData>::const_iterator it = god_equip_return_cfg->turn_back_reward_vec.begin(); it != god_equip_return_cfg->turn_back_reward_vec.end() && real_item_num < MAX_ATTACHMENT_ITEM_NUM; it++)
			{
				contentparam.item_list[real_item_num].num = it->num;
				contentparam.item_list[real_item_num].is_bind = it->is_bind ? 1 : 0;
				contentparam.item_list[real_item_num++].item_id = it->item_id;
			}
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_god_equip_return_subject);
			int lencont = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_god_equip_return_content, grid->item_id);

			if (lensub > 0 && lencont > 0)
			{
				MailRoute::Instance().MailToUser(IntToUid(m_role_module_mgr->GetUid()), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
	return true;
}

bool Workshop::CheckGodEquipItemDataChangeLevel(const ItemGridData * grid)
{
	if (NULL == grid || grid->Invalid()) return false;

	const ItemBase * item_base = ITEMPOOL->GetItem(grid->item_id);
	if (NULL == item_base || item_base->GetColor() != ItemBase::I_COLOR_GOD) return false;	//暂时神装就这2个品质之后如果有新增就改判断
	if (item_base->GetItemType() == I_TYPE_EQUIPMENT)
	{
		EquipmentParam & equip_param = grid->param->equip_param;
		if (equip_param.strengthen_level + 60 > MAX_GOD_EQUIP_MAX_LEVEL)
		{
			equip_param.strengthen_level = MAX_GOD_EQUIP_MAX_LEVEL;
		}
		else
		{
			equip_param.strengthen_level += 60;
		}
	}
	else if (item_base->GetItemType() == I_TYPE_JEWELRY)
	{
		JewelryParam & jewelry_param = grid->param->jewelry_param;
		if (jewelry_param.strengthen_level + 60 > MAX_GOD_EQUIP_MAX_LEVEL)
		{
			jewelry_param.strengthen_level = MAX_GOD_EQUIP_MAX_LEVEL;
		}
		else
		{
			jewelry_param.strengthen_level += 60;
		}
	}

	return true;
}

bool Workshop::CheckGodEquipItemDataReturnBreak(ItemGridData * item_data, const GodEquipTurnBackNode * cfg)
{
	if (item_data == NULL)return false;
	const ItemBase * i_b = ITEMPOOL->GetItem(item_data->item_id);
	if (NULL == i_b || (i_b->GetItemType() != I_TYPE_JEWELRY && i_b->GetItemType() != I_TYPE_EQUIPMENT) || i_b->GetColor() != ItemBase::I_COLOR_GOD)return false;
	const StrengthenCfg * ori_cfg = NULL;
	const StrengthenCfg * new_item_cfg = NULL;
	int level = 0;
	if (i_b->GetItemType() == I_TYPE_EQUIPMENT)
	{
		level = item_data->param->equip_param.strengthen_level;
	}
	else
	{
		level = item_data->param->jewelry_param.strengthen_level;
	}

	ori_cfg = GODEQUIP_CONFIG->GetStrengBackCfg(item_data->item_id, level);//原配置
	if (ori_cfg == NULL)return false;
	new_item_cfg = GODEQUIP_CONFIG->GetStrengthenCfg(ori_cfg->id, level);
	if (new_item_cfg == NULL)return false;

	if (i_b->GetItemType() == I_TYPE_EQUIPMENT)
	{
		item_data->item_id = ori_cfg->id;
		EquipmentParam & p_ep = item_data->param->equip_param;
		p_ep.attr_num = 0;
		for (int i = 0; i < EquipmentParam::ATTR_NUM_MAX; i++)
		{
			p_ep.attr_list[i].attr_type = new_item_cfg->attr_type_list[i];
			p_ep.attr_list[i].attr_value = new_item_cfg->attr_add_list[i];
			if (new_item_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
			p_ep.attr_num++;
		}
		p_ep.new_suit_effect_id = ori_cfg->suit_id;
		//重算熔炼词条数量(调高词条,新增为空词条,调低则保留之前的)

		const Equipment *new_equip = (const Equipment*)i_b;
		if (new_equip->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep.strengthen_level, new_equip->GetEquipColor());
			if (max_num_cfg != NULL && max_num_cfg->max_num <= EquipmentParam::EXA_ATTR_NUM_MAX)p_ep.inc_num = p_ep.inc_num >= max_num_cfg->max_num ? p_ep.inc_num : max_num_cfg->max_num;
		}
		for (int voc_idx = 0; voc_idx < p_ep.inc_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			//p_ep.attr_extra_inc[voc_idx].un_lock = 1;
		}
	}
	else
	{
		item_data->item_id = ori_cfg->id;
		JewelryParam & p_ep = item_data->param->jewelry_param;
		p_ep.attr_num = 0;
		for (int i = 0; i < JewelryParam::ATTR_NUM_MAX; i++)
		{
			p_ep.attr_list[i].attr_type = new_item_cfg->attr_type_list[i];
			p_ep.attr_list[i].attr_value = new_item_cfg->attr_add_list[i];
			if (new_item_cfg->attr_add_list[i] <= 0)continue;//值为0则词条为假不增加词条数量
			p_ep.attr_num++;
		}
		p_ep.new_suit_effect_id = ori_cfg->suit_id;
		//重算熔炼词条数量(调高词条,新增为空词条,调低则保留之前的)
		const Jewelry *new_equip = (const Jewelry*)i_b;
		if (new_equip->IsNeedReCalSmeltEntryNum())
		{
			const GodEquipAttrNumConfig* max_num_cfg = LOGIC_CONFIG->GetEntryConfig()->GetGodEquipmentNumCfg(p_ep.strengthen_level, new_equip->GetEquipColor());
			if (max_num_cfg != NULL)p_ep.inc_num = p_ep.inc_num >= max_num_cfg->max_num ? p_ep.inc_num : max_num_cfg->max_num;
		}
		for (int voc_idx = 0; voc_idx < p_ep.inc_num && voc_idx < EquipmentParam::EXA_ATTR_NUM_MAX; voc_idx++)
		{
			//p_ep.attr_extra_inc[voc_idx].un_lock = 1;
		}
	}

	if (cfg->return_item_list.size() > 0)
	{
		static MailContentParam contentparam; contentparam.Reset();
		int real_item_num = 0;
		for (std::vector<ItemConfigData>::const_iterator it = cfg->return_item_list.begin(); it != cfg->return_item_list.end() && real_item_num < MAX_ATTACHMENT_ITEM_NUM; it++)
		{
			contentparam.item_list[real_item_num].num = it->num;
			contentparam.item_list[real_item_num].is_bind = it->is_bind ? 1 : 0;
			contentparam.item_list[real_item_num++].item_id = it->item_id;
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_god_equip_back_break_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_god_equip_back_break_content);

		if (lensub > 0 && length > 0)
		{
			MailRoute::Instance().MailToUser(IntToUid(m_role_module_mgr->GetUid()), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	gamelog::g_log_workshop.printf(LL_INFO, "Workshop::CheckGodEquipItemDataReturnBreak user[%d,%s] weapon_id[%d] param: [%s]", m_role_module_mgr->GetUid(),
		m_role_module_mgr->GetRole()->GetName(), item_data->item_id, item_data->param->GetParamLog(i_b->GetItemType()));

	return true;
}

void Workshop::GodEquipChange()
{
	if (m_workshop_param.god_equip_change_flag < 1)
	{
		m_role_module_mgr->GetEquipmentManager()->GodEquipChangeLevel();
		m_role_module_mgr->GetKnapsack()->CheckGodEquipChangeLevel();

		m_workshop_param.god_equip_change_flag = 1;
	}
}

void Workshop::FixGodEquipBaseVoc()
{
	if (m_workshop_param.check_god_equip_base_voc_flag < 2)
	{
		m_role_module_mgr->GetEquipmentManager()->FixGodEquipBaseVoc();
		m_role_module_mgr->GetKnapsack()->FixGodEquipBaseVoc();

		m_workshop_param.check_god_equip_base_voc_flag = 2;
	}
}

void Workshop::SendRecipeActiveFlag(int active_id, bool is_notice)
{
	if (active_id < 0 || active_id >= SPECIAL_RECIPE_TOTAL_NUM) return;

	static Protocol::SCSingleRecipeActiveFlag sraf;
	sraf.active_id = active_id;
	sraf.is_active = (m_workshop_param.sp_recipe_active_flag.IsBitSet(active_id) ? 1 : 0);
	sraf.is_notice = is_notice ? 1 : 0;

	m_role_module_mgr->NetSend((const char *)&sraf, sizeof(sraf));
}

void Workshop::SendJewelryTimesInfo()
{
	static Protocol::CSWorkShopJewelryInfo wair;

	memcpy(wair.jewelry_times, m_workshop_param.jewelry_create_group, sizeof(wair.jewelry_times));

	m_role_module_mgr->NetSend((const char *)&wair, sizeof(wair));
}

void Workshop::SendJewelryReforgeOutcome()
{
	static Protocol::SCJewelryReforgeOutcomes jro;

	for (int t_cur = 0; t_cur < JEWELTY_REFORGE_LIST_MAX; ++t_cur)
	{
		jro.reforge_list[t_cur] = m_workshop_param.jewelry_reforge_list[t_cur];
	}

	m_role_module_mgr->NetSend((const char *)&jro, sizeof(jro));
}

void Workshop::SendComposeResult(bool is_succ, const ItemDataWrapper & get_item)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(get_item.item_id);
	if (NULL == item_base) return;

	Protocol::SCWorkshopComposeResults wcr;
	wcr.com_result = is_succ ? 1 : 0;
	wcr.has_param = (char)get_item.has_param;
	wcr.item_id = get_item.item_id;
	wcr.number = get_item.num;

	wcr.param_len = NetValueItemParam::GetDataLengthByItemType(item_base->GetItemType());
	memcpy(wcr.param_data, get_item.param_data, wcr.param_len);

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &wcr);
}
