#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/itempool.h"
#include "engineadapter.h"
#include "item/knapsack.h"
#include "config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "item/itemextern.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "gamelog.h"

#include "other/huashen/huashenmanager.hpp"
#include "other/medal/medalconfig.hpp"
#include "item/medalitem/medalitem.hpp"
#include "other/medal/medalfbconfig.hpp"
#include "protocol/msgmedal.h"
#include "medal.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/event/eventhandler.hpp"

Medal::Medal() :m_role_module_mgr(NULL)
{
	for (int i = 0; i < ARRAY_LENGTH(m_medal_old_state); ++i)
	{
		m_medal_old_state[i] = false;
	}
}

Medal::~Medal()
{
	for(int i = 0; i < ARRAY_ITEM_COUNT(m_wear_medal_grid); ++i)
	{
		m_wear_medal_grid[i].Clear();
	}
}

void Medal::Init(RoleModuleManager * mgr, const MedalCommonParam & param, const std::vector<const ItemListParam::ItemListData * >& medal_item_list)
{
	m_role_module_mgr = mgr;
	m_param = param;

	for (int i = 0; i < (int)medal_item_list.size(); ++i)
	{
		const ItemListParam::ItemListData * item_data = medal_item_list[i];
		int index = item_data->index - ItemNamespace::MEDAL_INDEX_BEGIN;
		if (index < 0 || index >= MAX_MEDAL_WEAR_GRID)
		{
			continue;
		}

		ItemGridData & grid = m_wear_medal_grid[index];
		grid.item_id = item_data->item_wrapper.item_id;
		grid.num = 1;
		grid.is_bind = (0 != item_data->item_wrapper.is_bind);
		grid.item_unique_id = item_data->item_wrapper.item_unique_id;
		grid.is_market_buy_flag = item_data->item_wrapper.is_market_buy_flag;
		grid.param = ItemBase::CreateParamFromParamData(item_data->item_wrapper.param_data);

		MedalItemParam & medal_param = grid.param->medal_param;
		medal_param.CheckModifyVersion();

		gamelog::g_log_medal.printf(LL_INFO, "%s index[%d] item_id[%d] param[%s]", __FUNCTION__, index, grid.item_id, grid.param->GetParamLog(I_TYPE_MEDAL));

		if (0 == grid.item_unique_id)
		{
			long long item_unique_id = 0;
			UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
			if (0 != item_unique_id)
			{
				grid.item_unique_id = item_unique_id;

#ifdef TRACK_ITEM_FLAG
				gamelog::g_log_item_track.printf(LL_INFO, "%s assign unique_id[%lld] to role[%d,%s] item[%d] num[%d] from index[%d]", __FUNCTION__,
					item_unique_id, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					grid.item_id, grid.num, index);
#endif

			}
		}


		m_medal_old_state[index] = true;
	}
}

void Medal::GetInitParam(MedalCommonParam * param)
{
	*param = m_param;
}

void Medal::GetMedalList(int * count, QueryItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
	{
		const ItemGridData & curr_medal = m_wear_medal_grid[i];
		if (curr_medal.Invalid() || NULL == curr_medal.param) continue;

		save_list[tmp_count].index = i + ItemNamespace::MEDAL_INDEX_BEGIN;
		curr_medal.WrapTo(&save_list[tmp_count].item_wrapper);
		save_list[tmp_count].change_state_item = structcommon::CS_NONE;

		gamelog::g_log_medal.printf(LL_INFO, "%s index[%d] item_id[%d] param[%s]", __FUNCTION__, i, curr_medal.item_id, curr_medal.param->GetParamLog(I_TYPE_MEDAL));
		if(++tmp_count >= *count) break;
	}
	*count = tmp_count;
}

void Medal::GetChangeMedalItemList(int * count, ItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int wear_index = 0; wear_index < ARRAY_LENGTH(m_wear_medal_grid); ++wear_index)
	{
		const ItemGridData & curr_medal = m_wear_medal_grid[wear_index];
		if (curr_medal.Invalid())
		{
			if (!m_medal_old_state[wear_index]) continue;

			save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
		}
		else
		{

			m_medal_old_state[wear_index] ? save_list[tmp_count].change_state_item = structcommon::CS_UPDATE :
				save_list[tmp_count].change_state_item = structcommon::CS_INSERT;
			
		}

		save_list[tmp_count].index = wear_index + ItemNamespace::MEDAL_INDEX_BEGIN;
		ItemDataWrapper & item_data_wrapper = save_list[tmp_count].item_wrapper;
		const ItemGridData & grid_data = m_wear_medal_grid[wear_index];
		grid_data.WrapTo(&item_data_wrapper);

		if (!curr_medal.Invalid() && NULL != curr_medal.param)
		{
			gamelog::g_log_medal.printf(LL_INFO, "%s index[%d] item_id[%d] param[%s]", __FUNCTION__, wear_index, curr_medal.item_id, curr_medal.param->GetParamLog(I_TYPE_MEDAL));
		}
		
		++tmp_count;
	}

	*count = tmp_count;
}

void Medal::ClearDirtyMark()
{
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(m_medal_old_state) == ARRAY_ITEM_COUNT(m_wear_medal_grid));
	for (int i = 0; i < ARRAY_LENGTH(m_medal_old_state); ++i)
	{
		m_medal_old_state[i] = !m_wear_medal_grid[i].Invalid();
	}	
}

const AttributeList & Medal::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();

		for (int wear_index = 0; wear_index < ARRAY_LENGTH(m_wear_medal_grid); ++wear_index)
		{
			const ItemGridData & curr_medal = m_wear_medal_grid[wear_index];
			if (curr_medal.Invalid())
			{
				continue;
			}

			const MedalItemParam & medal_item_param = curr_medal.param->medal_param;
			const MedalUpgradeCfg * cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(medal_item_param.grade);
			if (cfg == NULL)
			{
				continue;
			}

			m_attrs_add.Add(cfg->attrs);
		}

		std::set<int> sp_id = this->GetEffectSp();
		for (std::set<int>::iterator iter = sp_id.begin(); iter != sp_id.end(); ++iter)
		{
			GetSpecialEffectAttr(*iter, m_attrs_add, base_add, m_role_module_mgr->GetRole()->GetLevel(), m_role_module_mgr->GetRole()->GetLevel());
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void Medal::Update(unsigned long interval, time_t now_second)
{




}

void Medal::SendInfo()
{
	Protocol::SCMedalInfo protocol;
	protocol.count = 0;

	for (int i = 0; protocol.count < ARRAY_LENGTH(protocol.grid_info) && i < ARRAY_LENGTH(m_wear_medal_grid); ++i)
	{
		Protocol::SCMedalInfo::GridItem & protocol_item = protocol.grid_info[protocol.count];
		const ItemGridData & item_grid = m_wear_medal_grid[i];
		if (item_grid.Invalid())
		{
			continue;
		}

		protocol_item.grid_index = i;
		protocol_item.item_id = item_grid.item_id;
		protocol_item.is_bind = (char)item_grid.is_bind;
		protocol_item.num = item_grid.num;
		protocol_item.has_param = false;
		protocol_item.invalid_time = item_grid.invalid_time;

		const ItemBase * item_base = ITEMPOOL->GetItem(item_grid.item_id);
		if (NULL != item_grid.param && NULL != item_base) // 只发送有参数的
		{
			protocol_item.has_param = true;
			protocol_item.param_length = item_grid.param->GetDataByItemType(item_base->GetItemType(), protocol_item.param_data);
		}

		protocol.count += 1;
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &protocol);
}

void Medal::PutOn(int wear_index, int column, int index)
{
	if (wear_index < 0 || wear_index >= ARRAY_LENGTH(m_wear_medal_grid))
	{
		return;
	}
	ItemGridData & grid = m_wear_medal_grid[wear_index];


	ItemDataWrapper item_grid_data;
	m_role_module_mgr->GetKnapsack()->GetItemGridWrapData(column, index, &item_grid_data);
	const ItemBase * item = ITEMPOOL->GetItem(item_grid_data.item_id);
	if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, item_grid_data.num, __FUNCTION__))
	{
		return;
	}

	if (!grid.Invalid())
	{
		ItemDataWrapper tmp_item;
		grid.WrapTo(&tmp_item);
		
		int out_column = 0;
		int out_index = 0;
		m_role_module_mgr->GetKnapsack()->Put(tmp_item, PUT_REASON_MEDAL_PUT_ON, &out_column, &out_index);
		//有装备替换下发给客户端
		Protocol::SCMedalTakeOff msg;
		msg.out_column = out_column;
		msg.out_index = out_index;
		m_role_module_mgr->NetSend((const char *)&msg, sizeof(msg));
	}
	grid.Clear();
	grid.Set(item_grid_data);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_MEDAL, __FUNCTION__);
	if (NULL != grid.param)
	{
		gamelog::g_log_medal.printf(LL_INFO, "%s role[%d, %s]  item_id[%d] sp_id[%d] grade[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		item_grid_data.item_id, grid.param->medal_param.sp_id, grid.param->medal_param.grade);

		const MedalUpgradeCfg * cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(grid.param->medal_param.grade);

		if (cfg != NULL)
		{
			EventHandler::Instance().OnRolePutOnMedal(m_role_module_mgr->GetRole(), this->GetMedalNum(), this->GetLowMedalColorOnWear());//触发数量
		}
	}

	this->SendInfo();
}

void Medal::TakeOff(int wear_index)
{
	if (wear_index < 0 || wear_index >= ARRAY_LENGTH(m_wear_medal_grid))
	{
		return;
	}

	ItemGridData & item_grid = m_wear_medal_grid[wear_index];
	if (item_grid.Invalid())
	{
		return;
	}

	ItemDataWrapper temp;
	item_grid.WrapTo(&temp);
	if (!m_role_module_mgr->GetKnapsack()->Put(temp, PUT_REASON_MEDAL_PUT_ON))
	{
		return;
	}

	item_grid.Clear();

	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_MEDAL, __FUNCTION__);
	
	EventHandler::Instance().OnRolePutOffMedal(m_role_module_mgr->GetRole());
	this->SendInfo();
}

bool Medal::GetMedalGridData(short put_on_index, ItemDataWrapper & carrier_data)
{
	if (put_on_index < 0 || put_on_index >= MAX_MEDAL_WEAR_GRID)
	{
		return false;
	}

	if (m_wear_medal_grid[put_on_index].Invalid()) return false;

	m_wear_medal_grid[put_on_index].WrapTo(&carrier_data);
	return true;
}

int Medal::GetSpeScore()
{
	int capability = 0;
	std::set<MedalSpPriority> m_sp_id_set;
	this->GetEffectSp(&m_sp_id_set);
	if (!m_sp_id_set.empty())
	{
		const MedalSpeScoreCfg * spe_score_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetMedalSpeScroeCfg(m_role_module_mgr->GetRole()->GetLevel());
		if(NULL == spe_score_cfg) return capability;
		
		int prof_type = m_role_module_mgr->GetRole()->GetAppearanceProf();
		for (std::set<MedalSpPriority>::iterator it = m_sp_id_set.begin(); it != m_sp_id_set.end(); it++)
		{
			if(it->sp_priority < 0 || it->sp_priority >= MAX_MEDAL_MAX_GRADE) continue;

			capability += spe_score_cfg->score_priority[it->sp_priority];
		}
	}

	return capability;
}

void Medal::ExcahngFbMaterial(int seq)
{
	const MedalMaterialItemCfg * cfg = LOGIC_CONFIG->GetMedalFbConfig()->GetMedalMaterialItemCfg(seq);
	if (cfg == NULL)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->target_item.item_id, cfg->target_item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
	for (int i = 0; i < (int)cfg->need_item_list.size(); ++i)
	{
		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = cfg->need_item_list[i].item_id;
		stuff_con.num = cfg->need_item_list[i].num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}

	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
												consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		return;
	}

	m_role_module_mgr->GetKnapsack()->Put(cfg->target_item, PUT_REASON_MEDAL_EXCAHNG_MATERIAL);
}

void Medal::DecomposeMedal(int column, int index)
{
	bool is_succ = false;
	const ItemGridData * item_grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
	if (item_grid_data == NULL || item_grid_data->param == NULL)
	{
		Protocol::SCMedalDecomposeNotice protocol;
		protocol.is_succ = is_succ;
		m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
		return;
	}

	const ItemBase * item = ITEMPOOL->GetItem(item_grid_data->item_id);
	if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
	{
		Protocol::SCMedalDecomposeNotice protocol;
		protocol.is_succ = is_succ;
		m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
		return;
	}

	const MedalDecomposeCfg * cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalDecomposeCfg(item_grid_data->param->medal_param.grade);
	if (cfg == NULL)
	{
		Protocol::SCMedalDecomposeNotice protocol;
		protocol.is_succ = is_succ;
		m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
		return;
	}


	if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(cfg->get_item))
	{
		Protocol::SCMedalDecomposeNotice protocol;
		protocol.is_succ = is_succ;
		m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
		return;
	}

	std::string medal_log = STRING_SPRINTF("%s role[%d, %s]  item_id[%d] sp_id[%d] grade[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		item_grid_data->item_id, item_grid_data->param->medal_param.sp_id, item_grid_data->param->medal_param.grade);

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, item_grid_data->num, __FUNCTION__))//item_grid_data 后面不能再使用
	{
		Protocol::SCMedalDecomposeNotice protocol;
		protocol.is_succ = is_succ;
		m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
		return;
	}
	
	is_succ = true;
	m_role_module_mgr->GetKnapsack()->Put(cfg->get_item, PUT_REASON_DECOMPOSE_MEDAL);

	Protocol::SCMedalDecomposeNotice protocol;
	protocol.is_succ = is_succ;
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));

	gamelog::g_log_medal.printf(LL_INFO, "%s", medal_log.c_str());
}

bool Medal::UseAddSpItem(int sp_id, int column, int index, bool is_wear_grid)
{
	const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
	if (NULL == spe_cfg)
	{
		return false;
	}

	if (!is_wear_grid)
	{
		const ItemGridData * item_grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
		if (item_grid_data == NULL || item_grid_data->param == NULL)
		{
			return false;
		}

		const ItemBase * item = ITEMPOOL->GetItem(item_grid_data->item_id);
		if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
		{
			return false;
		}

		item_grid_data->param->medal_param.sp_id = sp_id;
		item_grid_data->param->medal_param.grade = spe_cfg->medal_grade;
		m_role_module_mgr->GetKnapsack()->SetItemDirty(column, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
	else
	{
		if (index < 0 || index >= ARRAY_LENGTH(m_wear_medal_grid))
		{
			return false;
		}
		const ItemGridData & curr_medal = m_wear_medal_grid[index];
		if (curr_medal.Invalid())
		{
			return false;
		}

		curr_medal.param->medal_param.sp_id = sp_id;
		curr_medal.param->medal_param.grade = spe_cfg->medal_grade;
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_MEDAL, __FUNCTION__);
		this->SendInfo();
	}

	return true;
}

void Medal::OnlottreyMedal(int seq,int times)
{
	const MedalExchangeCfg * cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalExchangeCfg(seq);
	if (cfg == NULL)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (times != MEDAL_LOTTERY_TIMES_TYPE_1 && times != MEDAL_LOTTERY_TIMES_TYPE_10)//必须单抽或者10抽其中一个
	{
		return;
	}

	int cost_num = cfg->one_num;
	if (times == MEDAL_LOTTERY_TIMES_TYPE_10)
	{
		cost_num = cfg->ten_num;
	}
	
	std::vector<ItemConfigData> real_reward_list;
	std::vector<ItemConfigData> broadcast_list;		//需要公告列表
	for (int i = 0; i < times && i < MEDAL_LOTTERY_TIMES_TYPE_10; i++)
	{
		const MedalExchangeRewardCfg * reward_cfg = LOGIC_CONFIG->GetMedalConfig()->GetRandRewardInfoByRandGroup(cfg->reward_group);
		if (reward_cfg == NULL)
		{
			gamelog::g_log_medal.printf(LL_INFO, "Medal::OnlottreyMedal NULL reward_cfg user[%s,%d] reward_group[%d] times[%d] times_index[%d]",
				m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), cfg->reward_group, times, i);
			continue;
		}
		real_reward_list.push_back(reward_cfg->item);
		if (reward_cfg->is_broadcast)
		{
			broadcast_list.push_back(reward_cfg->item);
		}
	}

	if (real_reward_list.size() <= 0)return;

	//检查
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)real_reward_list.size(), &real_reward_list[0]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(cfg->item_id, cost_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	//奖励信息
	Protocol::SCLottreyInfo reward_msg;
	reward_msg.count = 0;

	std::vector< std::pair<int, int> > column_index_list;
	//消耗
	m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->item_id, cost_num, __FUNCTION__);

	if (m_role_module_mgr->GetKnapsack()->PutList3((short)real_reward_list.size(), &real_reward_list[0], PUT_REASON_NO_NOTICE, &column_index_list))
	{
		for (int i = 0; i < (int)column_index_list.size() && i < MEDAL_LOTTERY_TIMES_TYPE_10; i++)
		{
			reward_msg.reward_list[i].reward_bag_type = column_index_list[i].first;
			reward_msg.reward_list[reward_msg.count++].reward_bag_index = column_index_list[i].second;
		}
		m_role_module_mgr->NetSend((const char*)&reward_msg, sizeof(reward_msg) - ((MEDAL_LOTTERY_TIMES_TYPE_10 - reward_msg.count) * sizeof(Protocol::SCLottreyInfo::MedalReward)));

		if (!broadcast_list.empty())
		{
			for (int i = 0; i < (int)broadcast_list.size(); i++)
			{	
				const ItemBase * item = ITEMPOOL->GetItem(broadcast_list[i].item_id);
				if (item == NULL || item->GetItemType() != I_TYPE_MEDAL) continue;

				const MedalItem * medal_item = (const MedalItem *)item;
				NetValueItemParam item_param;
				LOGIC_CONFIG->GetMedalConfig()->RandMedalParam(&item_param, medal_item->GetAttrGroup(), medal_item->GetSpGroup(), medal_item->GetInitialRank());
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_medal_lottery_broadcast, m_role_module_mgr->GetUid(), 
					m_role_module_mgr->GetRole()->GetName(), broadcast_list[i].item_id, item_param.medal_param.sp_id);
				if (length > 0)
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE)))
					{
						World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
					}
				}
			}
		}
	}
}

int Medal::OnMedalUpGrade(int column, int index, int consume_column, int consume_index,  bool is_wear_grid)
{
	const ItemGridData * item_grid_data = NULL;
	if (!is_wear_grid)
	{
		item_grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
		if (item_grid_data == NULL || item_grid_data->param == NULL)
		{
			return -__LINE__;
		}

		const ItemBase * item = ITEMPOOL->GetItem(item_grid_data->item_id);
		if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
		{
			return -__LINE__;
		}

		if (column == consume_column && index == consume_index)
		{
			return -__LINE__;
		}
	}
	else
	{
		if (index < 0 || index >= ARRAY_LENGTH(m_wear_medal_grid))
		{
			return -__LINE__;
		}

		item_grid_data = &m_wear_medal_grid[index];

		if (item_grid_data->Invalid())
		{
			return -__LINE__;
		}
	}

	const ItemGridData * consume_item = m_role_module_mgr->GetKnapsack()->GetItemGridData2(consume_column, consume_index);
	
	{
		if (consume_item == NULL || consume_item->param == NULL)
		{
			return -__LINE__;
		}

		const ItemBase * item = ITEMPOOL->GetItem(consume_item->item_id);
		if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
		{
			return -__LINE__;
		}
	}

	MedalItemParam & up_param = item_grid_data->param->medal_param;
	const MedalItemParam & consume_param = consume_item->param->medal_param;

	int grade = up_param.grade;
	int consume_grade = consume_param.grade;

	if (up_param.sp_id != 0 || consume_param.sp_id != 0)
	{
		const SpecialEffectCfg * up_spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(up_param.sp_id);
		if (NULL == up_spe_cfg)return -__LINE__;

		const SpecialEffectCfg * consume_spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(consume_param.sp_id);
		if (NULL == consume_spe_cfg)return -__LINE__;

		//策划要求去除相同特效限定
		/*if (up_spe_cfg->sp_type != consume_spe_cfg->sp_type)
		{
			return -__LINE__;
		}*/
	}

	const MedalUpgradeCfg * cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(up_param.grade);
	const MedalUpgradeCfg * consume_cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(consume_param.grade);
	if (cfg == NULL || consume_cfg == NULL)
	{
		return -__LINE__;
	}

	if (cfg->color != consume_cfg->color)
	{
		return -__LINE__;
	}

	const MedalUpgradeCfg * next_cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(up_param.grade + 1);
	if (next_cfg == NULL)
	{
		return -__LINE__;
	}
	std::string medal_log = STRING_SPRINTF("%s role[%d, %s]  consume_item_id[%d] sp_id[%d] grade[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		consume_item->item_id, consume_item->param->medal_param.sp_id, consume_item->param->medal_param.grade);

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(consume_column, consume_index, 1, __FUNCTION__))  //consume_param后面不能再用
	{
		return -__LINE__;
	}
	gamelog::g_log_medal.printf(LL_INFO, "%s", medal_log.c_str());
	//////////////////////////// 检查 ///////////////////////////////////
	bool is_succ = false;
	int old_grade = up_param.grade;
	if (grade < consume_grade)
	{
		is_succ = true;
		up_param.grade = consume_grade;
	}
	else
	{
		// 计算成功率
		int succ_rate = LOGIC_CONFIG->GetMedalConfig()->GetUpgradeRate(grade, consume_grade);
		int rand_rate = RandomNum(10000);
		if (rand_rate < succ_rate) is_succ = true;

		if (is_succ)
		{
			up_param.grade += 1;
		}
		else
		{
			up_param.grade = LOGIC_CONFIG->GetMedalConfig()->GetMedalMinGradeByColor(cfg->color);
		}
	}
	EventHandler::Instance().OnRoleMedalUpGrade(m_role_module_mgr->GetRole(), cfg->color, up_param.grade);

	int new_sp_id = 0;
	const MedalOtherCfg & other_cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalOtherCfg();
	if (up_param.sp_id == 0 && up_param.grade >= other_cfg.rand_sp_id_grade)
	{
		const MedalSpGroupCfg::ItemCfg * item_cfg = LOGIC_CONFIG->GetMedalConfig()->GetRandMedalItemSpItemCfg(other_cfg.rand_sp_id_group);
		if (item_cfg != NULL)
		{
			new_sp_id = item_cfg->sp_id;
		}		
	}

	if (up_param.sp_id != 0)
	{
		new_sp_id = BattleSpecialEffectPool::GetInstance().GetMedalSpid(up_param.sp_id, up_param.grade);
	}

	int old_spid = up_param.sp_id;

	up_param.sp_id = new_sp_id;

	//传闻
	if (is_succ && next_cfg->hearsay == 1)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_medal_up_grade,
			m_role_module_mgr->GetRole()->GetName(), item_grid_data->item_id, old_grade, old_spid, item_grid_data->item_id, up_param.grade, up_param.sp_id);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
			{
				World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
			}
		}
	}

	if (!is_wear_grid)
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(column, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
	else
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_MEDAL, __FUNCTION__);
		this->SendInfo();
	}

	Protocol::SCMedalUpgradeNotice protocol;
	protocol.is_succ = is_succ;
	protocol.is_wear = is_wear_grid;
	protocol.column = column;
	protocol.index = index;
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));

	return 0;
}

void Medal::OnSpeMedalUpGrade(bool is_wear_grid, int column, int index, int consume_column_spe, int consume_index_spe, int consume_column, int consume_index)
{
	const ItemGridData * item_grid_data = NULL;
	const ItemGridData * item_grid_spe_data = NULL;//天赐令牌
	const ItemGridData * item_grid_common_data = NULL;//普通令牌
	if (is_wear_grid)
	{
		if (index < 0 || index >= ARRAY_LENGTH(m_wear_medal_grid))
		{
			return;
		}

		item_grid_data = &m_wear_medal_grid[index];

		if (item_grid_data->Invalid())
		{
			return;
		}
	}
	else
	{
		if (column == consume_column_spe && index == consume_index_spe)
		{
			return;
		}
		item_grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
		if (item_grid_data == NULL || item_grid_data->param == NULL)
		{
			return;
		}
	}

	const ItemBase * item = ITEMPOOL->GetItem(item_grid_data->item_id);
	if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_MEDAL_SPECIAL_EXCHANGE_NOT_SPECIAL);
		return;
	}
	MedalItemParam * medal_param = (MedalItemParam *)item_grid_data->param;
	if (!LOGIC_CONFIG->GetMedalConfig()->IsSpeMedalItem(medal_param->sp_id) || !LOGIC_CONFIG->GetMedalConfig()->IsSpeMedalItem(medal_param->sp_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_MEDAL_SPECIAL_EXCHANGE_NOT_SPECIAL);
		return;
	}
	//检查天赐令牌
	{
		//天赐令牌
		item_grid_spe_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(consume_column_spe, consume_index_spe);
		if (item_grid_spe_data == NULL || item_grid_spe_data->param == NULL)
		{
			return;
		}
		const ItemBase * item_spe = ITEMPOOL->GetItem(item_grid_spe_data->item_id);
		if (item_spe == NULL || item_spe->GetItemType() != I_TYPE_MEDAL)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_MEDAL_SPECIAL_EXCHANGE_NOT_SPECIAL);
			return;
		}
		MedalItemParam * spe_medal_param = (MedalItemParam *)item_grid_spe_data->param;
		if (!LOGIC_CONFIG->GetMedalConfig()->IsSpeMedalItem(spe_medal_param->sp_id) || !LOGIC_CONFIG->GetMedalConfig()->IsSpeMedalItem(spe_medal_param->sp_id))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_MEDAL_SPECIAL_EXCHANGE_NOT_SPECIAL);
			return;
		}

		if (spe_medal_param->sp_id != medal_param->sp_id)
		{
			return;
		}
	}
	int second_sp_id = 0;//升华特效
	//检查普通令牌
	{
		//普通令牌
		item_grid_common_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(consume_column, consume_index);
		if (item_grid_common_data == NULL || item_grid_common_data->param == NULL)
		{
			return;
		}
		const ItemBase * item = ITEMPOOL->GetItem(item_grid_common_data->item_id);
		if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		MedalItemParam * common_medal_param = (MedalItemParam *)item_grid_common_data->param;
		int change_level = 0;
		if (0 == medal_param->special_medal_gread)
		{
			change_level = MAX_SPECIAL_MEDAL_FIRST_UPGREADE_LEVEL;
		}
		else if(1 == medal_param->special_medal_gread)
		{
			change_level = MAX_SPECIAL_MEDAL_SECOND_UPGREADE_LEVEL;
		}
		else
		{
			return;
		}

		second_sp_id = BattleSpecialEffectPool::GetInstance().GetMedalSpid(common_medal_param->sp_id, change_level);
		if (second_sp_id == 0)return;
	}

	//消耗
	m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(consume_column_spe, consume_index_spe, 1, __FUNCTION__);
	m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(consume_column, consume_index, 1, __FUNCTION__);

	medal_param->second_sp_id = second_sp_id;
	medal_param->special_medal_gread += 1;
	if (is_wear_grid)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_MEDAL, __FUNCTION__);
		this->SendInfo();
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(column, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
}

void Medal::OnSpeMedalExchange(int column, int index, int sp_id, int second_sp_id, bool is_wear)
{
	const ItemGridData * item_grid_data = NULL;
	if (is_wear)
	{
		if (index < 0 || index >= ARRAY_LENGTH(m_wear_medal_grid))
		{
			return;
		}

		item_grid_data = &m_wear_medal_grid[index];

		if (item_grid_data->Invalid())
		{
			return;
		}
	}
	else
	{
		item_grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
		if (item_grid_data == NULL || item_grid_data->param == NULL)
		{
			return;
		}
	}
	if(NULL == item_grid_data) return;

	const ItemBase * item = ITEMPOOL->GetItem(item_grid_data->item_id);
	if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_MEDAL_SPECIAL_EXCHANGE_NOT_SPECIAL);
		return;
	}
	bool is_set_second_sp = false;
	MedalItemParam * medal_param = (MedalItemParam *)item_grid_data->param;
	if (LOGIC_CONFIG->GetMedalConfig()->IsSpeMedalItem(medal_param->sp_id))		// 道具本身是天赐令牌
	{	
		if (!LOGIC_CONFIG->GetMedalConfig()->IsSpeMedalItem(sp_id))		// 天赐令牌
		{
			m_role_module_mgr->NoticeNum(errornum::EN_MEDAL_SPECIAL_EXCHANGE_NOT_SPECIAL);
			return;
		}
		if (medal_param->second_sp_id > 0)		//只有天赐令牌才有第二个特效
		{
			if(medal_param->sp_id == sp_id && medal_param->second_sp_id == second_sp_id)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_MEDAL_EXCHANGE_NOT_SAME_SP);
				return;
			}
			const SpecialEffectCfg * old_second_spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(medal_param->second_sp_id);
			if (NULL == old_second_spe_cfg)
			{
				return;
			}
			const SpecialEffectCfg * new_second_spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(second_sp_id);
			if (NULL == new_second_spe_cfg)
			{
				return;
			}
			if (old_second_spe_cfg->medal_grade != new_second_spe_cfg->medal_grade || LOGIC_CONFIG->GetMedalConfig()->IsSpeMedalItem(second_sp_id))		//第二个特效不能是天赐
			{
				return;
			}
			is_set_second_sp = true;
		}
		else
		{
			if (medal_param->sp_id == sp_id)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_MEDAL_EXCHANGE_NOT_SAME_SP);
				return;
			}
		}
	}
	else
	{
		if (medal_param->sp_id == sp_id)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_MEDAL_EXCHANGE_NOT_SAME_SP);
			return;
		}
		if (LOGIC_CONFIG->GetMedalConfig()->IsSpeMedalItem(sp_id))
		{
			return;
		}
		const SpecialEffectCfg * old_spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(medal_param->sp_id);
		if (NULL == old_spe_cfg)
		{
			return;
		}
		const SpecialEffectCfg * new_spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(sp_id);
		if (NULL == new_spe_cfg)
		{
			return;
		}
		if (old_spe_cfg->medal_grade != new_spe_cfg->medal_grade)
		{
			return;
		}
	}
	
	const MedalOtherCfg & other_cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalOtherCfg();
	/*if (m_role_module_mgr->GetKnapsack()->Count(other_cfg.medal_exchange_item) <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}*/
	/*if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(ItemConfigData(target_item_id, true, 1)))
	{
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, 1, __FUNCTION__))
	{
		return;
	}*/
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.medal_exchange_item, 1, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}
	std::string medal_log = STRING_SPRINTF("%s role[%d, %s] consume_item_id[%d] before_item[%d,%d,%d,%d,%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		other_cfg.medal_exchange_item, item_grid_data->item_id, item_grid_data->param->medal_param.sp_id, item_grid_data->param->medal_param.grade,
		item_grid_data->param->medal_param.second_sp_id, item_grid_data->param->medal_param.special_medal_gread);
	medal_param->sp_id = sp_id;
	if(is_set_second_sp)
	{
		medal_param->second_sp_id = second_sp_id;
	}

	if (is_wear)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_MEDAL, __FUNCTION__);
		this->SendInfo();
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SetItemDirty(column, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
	}
	Protocol::SCMedalSpecialExchangeNotice info;
	info.is_wear = is_wear ? 1: 0;
	info.column = column;
	info.index = index;
	m_role_module_mgr->NetSend(&info, sizeof(info));

	gamelog::g_log_medal.printf(LL_INFO, "%s after_item[%d,%d,%d,%d,%d]", medal_log.c_str(), item_grid_data->item_id, item_grid_data->param->medal_param.sp_id, item_grid_data->param->medal_param.grade,
		item_grid_data->param->medal_param.second_sp_id, item_grid_data->param->medal_param.special_medal_gread);
}

void Medal::OnOneKeyMedalUpGrade(int count, int column, BitMap<MEDAL_ONE_KEY_GRADE_MAXNUM> index_flag)
{
	if (ITEM_COLUMN_TYPE_ITEM != column || 0 >= count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	std::vector<int> m_idx_vec;
	for (int i = 0; i < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; ++i)
	{
		if(!index_flag.IsBitSet(i)) continue;

		m_idx_vec.push_back(i);
	}

	if (count != (int)m_idx_vec.size())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	std::map<int, std::vector<ItemDataWrapper> > m_item_map;	// key-grade

	for (std::vector<int>::const_iterator it = m_idx_vec.begin(); it != m_idx_vec.end(); ++it)
	{
		const ItemGridData * item_grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, *it);
		if (item_grid_data == NULL || item_grid_data->param == NULL || item_grid_data->Invalid())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		const ItemBase * item = ITEMPOOL->GetItem(item_grid_data->item_id);
		if (item == NULL || item->GetItemType() != I_TYPE_MEDAL)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}

		ItemDataWrapper item_wrap_data;
		item_grid_data->WrapTo(&item_wrap_data);

		int grade = item_grid_data->param->medal_param.grade;
		m_item_map[grade].push_back(item_wrap_data);
	}

	int max_grade = LOGIC_CONFIG->GetMedalConfig()->GetMaxGrade();
	if (0 >= max_grade)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const MedalOtherCfg & other_cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalOtherCfg();

	std::vector<std::pair<int, int> > event_handle_list;
	for (int i = 0; i < max_grade; ++i)
	{
		std::map<int, std::vector<ItemDataWrapper> >::iterator it = m_item_map.find(i);
		if (m_item_map.end() == it) continue;

		const MedalUpgradeCfg * cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(i + 1);
		if(NULL == cfg) continue;

		while (2 <= it->second.size())
		{
			static NetValueItemParam param;
			param.SetData(it->second[0].param_data);

			param.medal_param.grade++;

			if (0 == param.medal_param.sp_id && param.medal_param.grade >= other_cfg.rand_sp_id_grade)
			{
				const MedalSpGroupCfg::ItemCfg * item_cfg = LOGIC_CONFIG->GetMedalConfig()->GetRandMedalItemSpItemCfg(other_cfg.rand_sp_id_group);
				if (item_cfg != NULL)
				{
					param.medal_param.sp_id = item_cfg->sp_id;
				}
			}

			if (param.medal_param.sp_id != 0)
			{
				param.medal_param.sp_id = BattleSpecialEffectPool::GetInstance().GetMedalSpid(param.medal_param.sp_id, param.medal_param.grade);
			}

			param.GetData(it->second[0].param_data);

			m_item_map[i + 1].push_back(it->second[0]);

			it->second.erase(it->second.begin(), it->second.begin() + 2);
			event_handle_list.push_back(std::pair<int, int>(cfg->color, param.medal_param.grade));
		}
	}

	//消耗
	for (std::vector<int>::const_iterator it = m_idx_vec.begin(); it != m_idx_vec.end(); ++it)
	{
		m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, *it, 1, __FUNCTION__);
	}

	//给与
	int end_count = 0;
	std::vector<ItemDataWrapper> m_reward_item;
	for (std::map<int, std::vector<ItemDataWrapper> >::const_iterator it = m_item_map.begin(); it != m_item_map.end(); ++it)
	{
		for (std::vector<ItemDataWrapper>::const_iterator iter = it->second.begin(); iter != it->second.end(); ++iter)
		{
			//m_role_module_mgr->GetKnapsack()->Put(*iter, PUT_REASON_ONE_KEY_GRADE_MEDAL);
			m_reward_item.push_back(*iter);
			end_count++;
		}
	}

	if (end_count > 0)
	{
		m_role_module_mgr->GetKnapsack()->PutList(end_count, &m_reward_item[0], PUT_REASON_ONE_KEY_GRADE_MEDAL);
	}

	if (0 != end_count && end_count != count)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_MEDAL, __FUNCTION__);
	}
	for(int i = 0; i < (int)event_handle_list.size(); ++i)
	{
		EventHandler::Instance().OnRoleMedalUpGrade(m_role_module_mgr->GetRole(), event_handle_list[i].first, event_handle_list[i].second);
	}
}

int Medal::GetMedalNum()
{
	int wear_num = 0;
	for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
	{
		ItemGridData &node = m_wear_medal_grid[i];
		if (!node.Invalid())wear_num += 1;
	}
	return wear_num;
}

int Medal::GetLowMedalColorOnWear()
{
	int low_color = 0;
	for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
	{
		ItemGridData &node = m_wear_medal_grid[i];
		NetValueItemParam * param = node.param;
		if (NULL == param) continue;
		const MedalUpgradeCfg * cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(param->medal_param.grade);
		if (cfg != NULL)
		{
			if (low_color == 0 || low_color > cfg->color)
			{
				low_color = cfg->color;
			}
		}
	}
	return low_color;
}

int Medal::GetCurWearNumByColor(int color)
{
	int wear_color_num = 0;
	for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
	{
		ItemGridData &node = m_wear_medal_grid[i];
		NetValueItemParam * param = node.param;
		if (NULL == param) continue;
		const MedalUpgradeCfg * cfg = LOGIC_CONFIG->GetMedalConfig()->GetMedalUpgradeCfg(param->medal_param.grade);
		if (cfg != NULL)
		{
			if (cfg->color >= color)
			{
				wear_color_num++;
			}
		}
	}

	return wear_color_num;
}

int Medal::GetCurWearNumByGrade(int grade)
{
	int wear_grade_num = 0;
	for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
	{
		ItemGridData &node = m_wear_medal_grid[i];
		NetValueItemParam * param = node.param;
		if (NULL == param) continue;
	
		if (param->medal_param.grade >= grade)
		{
			wear_grade_num++;
		}
	}

	return wear_grade_num;
}

void Medal::SynHuaShenMedal(std::set<long long> * item_set)
{
	for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
	{
		if(m_wear_medal_grid[i].Invalid()) continue;

		m_role_module_mgr->GetHuaShenManager()->SynBenZunItemData(&m_wear_medal_grid[i], HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN);
		if (NULL != item_set)
		{
			item_set->insert(m_wear_medal_grid[i].item_unique_id);
		}
	}
}

void Medal::SynHuaShenMedalGrid(bool is_check)
{
	if (is_check)
	{
		for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
		{
			if (m_wear_medal_grid[i].Invalid())
			{
				m_role_module_mgr->GetHuaShenManager()->MedalTakeOff(i, false);
				continue;
			}
			int key_index = HUASHEN_GRID_INDEX_TYPE_MEDAL * HUASHEN_GRID_INDEX_TYPE_BASE + i;
			if (!m_role_module_mgr->GetHuaShenManager()->IsCheckItemIdentical(m_wear_medal_grid[i].item_unique_id, key_index))
			{
				m_role_module_mgr->GetHuaShenManager()->ConsumeItem(m_wear_medal_grid[i].item_unique_id, false);
				m_role_module_mgr->GetHuaShenManager()->MedalTakeOff(i, false);
				ItemDataWrapper temp_item_wrapper;
				m_wear_medal_grid[i].WrapTo(&temp_item_wrapper);
				m_role_module_mgr->GetHuaShenManager()->SynBenZunMedalGrid(temp_item_wrapper, i);
			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
		{
			if (m_wear_medal_grid[i].Invalid()) continue;

			ItemDataWrapper item;
			m_wear_medal_grid[i].WrapTo(&item);
			m_role_module_mgr->GetHuaShenManager()->SynBenZunMedalGrid(item, i);
		}
	}
}

void Medal::GetBattleDate(RoleBattleData * role_data)
{
	if (role_data == NULL)
	{
		return;
	}
	std::set<int> sp_id = this->GetEffectSp();

	for (std::set<int>::iterator iter = sp_id.begin(); iter != sp_id.end() && role_data->other_sp_num < ARRAY_LENGTH(role_data->other_sp_list); ++iter)
	{
		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(*iter);
		if (NULL == spe_cfg)
		{
			continue;
		}
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
			continue;
		}
	
		if (role_data->other_sp_num < ARRAY_LENGTH(role_data->other_sp_list))
		{
			role_data->other_sp_list[role_data->other_sp_num++] = *iter;
		}
		else
		{
#ifdef _DEBUG
			assert(false);
#else
			gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_other_sp_num[%d] ", __FUNCTION__, ARRAY_LENGTH(role_data->other_sp_list));
#endif
			return;
		}
	}
}

std::set<int> Medal::GetEffectSp(std::set<MedalSpPriority> * out_sp_id_set)
{
	std::set<int> ret;
	std::set<MedalSpPriority> priority;

	for (int wear_index = 0; wear_index < ARRAY_LENGTH(m_wear_medal_grid); ++wear_index)
	{
		const ItemGridData & curr_medal = m_wear_medal_grid[wear_index];
		if (curr_medal.Invalid())
		{
			continue;
		}
		const MedalItemParam & medal_item_param = curr_medal.param->medal_param;

		{
			const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(medal_item_param.sp_id);
			if (NULL != spe_cfg && (spe_cfg->prof_type == 0 || m_role_module_mgr->GetRole()->GetAppearanceProf() == spe_cfg->prof_type))
			{
				MedalSpPriority  tmp;
				tmp.sp_id = medal_item_param.sp_id;
				tmp.sp_type = spe_cfg->sp_type;
				tmp.sp_priority = spe_cfg->sp_priority;
				tmp.prof_type = spe_cfg->prof_type;
				priority.insert(tmp);
			}
		}

		{
			const SpecialEffectCfg * spe_cfg_2 = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(medal_item_param.second_sp_id);
			if (NULL != spe_cfg_2 && (spe_cfg_2->prof_type == 0 || m_role_module_mgr->GetRole()->GetAppearanceProf() == spe_cfg_2->prof_type))
			{
				MedalSpPriority  tmp2;
				tmp2.sp_id = medal_item_param.second_sp_id;
				tmp2.sp_type = spe_cfg_2->sp_type;
				tmp2.sp_priority = spe_cfg_2->sp_priority;
				tmp2.prof_type = spe_cfg_2->prof_type;
				priority.insert(tmp2);
			}
		}
	}

	int last_sp_type = -1;
	for (std::set<MedalSpPriority>::iterator iter = priority.begin(); iter != priority.end(); ++iter)
	{
		if (last_sp_type == iter->sp_type)
		{
			continue;
		}

		last_sp_type = iter->sp_type;
		ret.insert(iter->sp_id);
		if (NULL != out_sp_id_set)
		{
			out_sp_id_set->insert(*iter);
		}
	}

	return ret;
}

bool Protocol::SCMedalInfo::Serialize(char * buffer, int buffer_size, int * out_length)
{
	bool result = true;
	result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

	result = result && WriteInt(count, buffer, buffer_size, out_length);
	for (int i = 0; i < count && i < ARRAY_LENGTH(grid_info); ++i)
	{
		const Protocol::SCMedalInfo::GridItem & grid = grid_info[i];
	
		result = result && WriteInt(grid.grid_index, buffer, buffer_size, out_length);
		result = result && WriteUShort(grid.item_id, buffer, buffer_size, out_length);
		result = result && WriteShort(grid.num, buffer, buffer_size, out_length);
		result = result && WriteChar(grid.is_bind, buffer, buffer_size, out_length);
		result = result && WriteChar(grid.has_param, buffer, buffer_size, out_length);
		result = result && WriteUInt(grid.invalid_time, buffer, buffer_size, out_length);
		if (0 != grid.has_param)
		{
			result = result && WriteShort(grid.param_length, buffer, buffer_size, out_length);
			result = result && WriteStrN(grid.param_data, grid.param_length, buffer, buffer_size, out_length);
		}
	}

	return result;
}

Protocol::SCMedalInfo::GridItem::GridItem()
{
	grid_index = 0;
	item_id = 0;		//!< 物品id
	num = 0;			//!< 数量
	is_bind = 0;		//!< 是否绑定
	has_param = 0;		//!< 是否有净值参数
	invalid_time = 0;	//!< 过期时间
	param_length = 0;	//!< 净值参数的数据长度 （当has_param为1时可读）
	memset(param_data, 0, sizeof(param_data));
}

