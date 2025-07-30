#include "droppool.hpp"
#include "servercommon/configcommon.h"
#include "globalconfig/globalconfig.h"
#include "obj/character/role.h"
#include "item/itempool.h"
#include "scene/map.h"
#include "scene/scene.h"
#include "gameworld/gamelog.h"
#include "gamecommon.h"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"
#include "other/roledrop/roledrop.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "other/activesystem/activesystem.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "config/otherconfig/sceneminemonsterconfig.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
DropPool *gs_drop_pool_config = NULL;

DropPool * DropPool::Instance()
{
	if (NULL == gs_drop_pool_config)
	{
		gs_drop_pool_config = new DropPool();
	}

	return gs_drop_pool_config;
}

DropPool::DropPool()
{
	memset(m_drop_list, 0, sizeof(m_drop_list));
}

DropPool::~DropPool()
{
	this->Release();
}

void DropPool::Release()
{
	for (int i = 0 ; i < MAX_DROP_ID; ++i)
	{
		if (NULL != m_drop_list[i])
		{
			delete m_drop_list[i];
		}
	}
	memset(m_drop_list, 0, sizeof(m_drop_list));
}

bool DropPool::Init(const std::string &dir, const std::string &configname, std::string *err)
{
	char errinfo[1024] = {0};

	TiXmlDocument document;

	if (configname == "" || !document.LoadFile(dir+configname))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load DropPool Config Error, %s.", (dir+configname).c_str(), document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	TiXmlElement *currentElement = NULL;

	if (NULL == RootElement)
	{
		*err = configname + ": xml root node error.";
		return false;
	}

	currentElement = RootElement->FirstChildElement("path");
	while (NULL != currentElement)
	{
		std::string path;

		if (!GetNodeValue(currentElement, path))
		{
			this->Release();
			return false;
		}

		DropConfig *dropconfig = new DropConfig();
		if (!dropconfig->LoadConfig((dir+path).c_str(),err))
		{
			delete dropconfig; dropconfig = NULL;
			this->Release();
			return false;
		}

		if (0 != m_drop_list[dropconfig->GetDropID()])
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: Load Drop Pool Config Error, drop id [%d] is repeat.", (dir+configname).c_str(), dropconfig->GetDropID());
			*err = errinfo;

			delete dropconfig; dropconfig = NULL;
			this->Release();
			return false;
		}

		m_drop_list[dropconfig->GetDropID()] = dropconfig;

		currentElement = currentElement->NextSiblingElement();
	}

	return true;
}

bool DropPool::Reload(const std::string &config_dir, const std::string &configname, std::string *err)
{
	DropPool *temp = new DropPool();

	if (!temp->Init(config_dir, configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != gs_drop_pool_config) delete gs_drop_pool_config;

	gs_drop_pool_config = temp;

	return true;
}

bool DropPool::IsUseActivityNotice(int get_reason_type)
{
	bool ret = false;
	switch (get_reason_type)
	{
		case GET_REASON_MIJIQIWEN:
		{
			ret = true;
		}
		break;
		default:
		break;
	}
	return ret;
}

DropConfig * DropPool::GetDropConfig(UInt16 drop_id)
{
	if (drop_id <= 0 || drop_id >= USHRT_MAX) return NULL;
	return m_drop_list[drop_id];
}

int DropPool::GetDropItemList(UInt16 dropid, ItemConfigData item_list[DropConfig::DROP_ITEM_PROB_MAX])
{
	if (NULL == item_list) return 0;

	DropConfig *drop_config = DROPPOOL->GetDropConfig(dropid);
	if (NULL == drop_config) return 0;

	DropItemCfg drop_list[DropConfig::DROP_ITEM_PROB_MAX];

	int item_count = drop_config->RandDropItem(drop_list);
	item_count = drop_config->GetFixDropItem(item_count, drop_list);

	if (item_count > DropConfig::DROP_ITEM_PROB_MAX)
	{
		item_count = DropConfig::DROP_ITEM_PROB_MAX;
	}

	for (int i = 0; i < item_count; ++i)
	{
		item_list[i] = drop_list[i].item_config_data;
	}

	return item_count;
}

bool DropPool::DropToRole(Role * role, UInt16 dropid, int monster_group_id, bool is_log)
{
	DropConfig *drop_config = DROPPOOL->GetDropConfig(dropid);
	if (NULL == drop_config) return false;

	DropItemCfg item_list[DropConfig::DROP_ITEM_PROB_MAX];

	int item_count = drop_config->RandDropItem(item_list);
	item_count = drop_config->GetFixDropItem(item_count, item_list);

	Int64 add_coin = 0;
	Int64 add_immortal_coin = 0;
	Int64 add_gold = 0;

	static ItemConfigData add_item_list[DropConfig::DROP_ITEM_PROB_MAX];
	int add_item_num = 0;

	if (item_count > 0 && item_count <= DropConfig::DROP_ITEM_PROB_MAX)
	{
		for (int i = 0; i < item_count; ++i)
		{
			ItemConfigData & drop_item = item_list[i].item_config_data;
			switch (drop_item.item_id)
			{
			case COIN_ITEM_ID:
				add_coin += drop_item.num;
				break;
			case GOLD_ITEM_ID:
				add_gold += drop_item.num;
				break;
			case IMMORTAL_COIN_ITEM_ID:
				add_immortal_coin += drop_item.num;
				break;
			default:
				{
					const ItemBase * item_base = ITEMPOOL->GetItem(drop_item.item_id);
					if (NULL != item_base && add_item_num < DropConfig::DROP_ITEM_PROB_MAX)
					{
						// 判断物品是否要限制掉落，且RoleDrop没有解除限制
						if (item_base->IsLimitDrop() &&
							(role->GetRoleModuleManager()->GetRoleDrop()->IsLockDrop(drop_item.item_id)))
						{
							continue;
						}

						if (item_base->HasDropCoolDown() &&
							role->GetRoleModuleManager()->GetRoleDrop()->IsItemDropInCoolDown(drop_item.item_id))
						{
							continue;
						}

						// 转换物品且为无职业时，不发放奖励
						if (I_TYPE_CONVERT_ITEM == item_base->GetItemType() && 0 == role->GetProfession())
						{
							continue;
						}

						if (item_list[i].is_active_limit && 
							SceneMineMonsterConfig::Instance().IsMeetActive(role->GetLevel(), role->GetRoleModuleManager()->GetActiveSystem()->GetActiveNum()))
						{
							drop_item.is_bind = false;
						}

						add_item_list[add_item_num++] = drop_item;

						if (item_list[i].broadcast)
						{
							int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_drop_broadcast, role->GetUID(), role->GetName(), drop_item.item_id);
							if (length > 0)
							{
								World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
							}
						}
					}
				}
				break;
			}
		}

		if (add_coin > 0 || add_gold > 0 || add_immortal_coin > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->Add(add_gold, add_immortal_coin, add_coin, __FUNCTION__);
		}
		
		if (add_item_num > 0)
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutList(add_item_num, add_item_list, PUT_REASON_DROP);
		}

		// 记录掉落了什么， 至于有否成功掉落到背包，就要查看knapsack的log了
		if (is_log)
		{
			gamelog::g_log_drop.buff_printf("DropPool::DropToRole role[%d,%s] drop_id[%d] from monster_group[%d], drop: coin[%lld] immortal_coin[%lld] gold[%lld] item_num[%d] item_list:{ ",
				role->GetUID(), role->GetName(), dropid, monster_group_id, add_coin, add_immortal_coin, add_gold, add_item_num);
			for (int i = 0; i < add_item_num; ++i)
			{
				gamelog::g_log_drop.buff_printf("[%d:%d:%d] ", add_item_list[i].item_id, add_item_list[i].num, add_item_list[i].is_bind ? 1 : 0);
			}
			gamelog::g_log_drop.commit_buff(LL_INFO);
		}
	}

	return true;
}

bool DropPool::DropListToRole(Role * role, const std::vector<int>& dropid_list, int monster_group_id, bool is_log, int notice_get_reson, int multipel, std::vector<ItemConfigData> *item_list, 
	int put_reason, int param1/* = 0*/, int param2/* = 0*/, int activity_type /* = 0*/, bool is_win/* = true*/)
{
	if (role == NULL)
	{
		return false;
	}

	std::vector<ItemConfigData> drop_item_list;
	for (int i = 0; i < (int)dropid_list.size(); ++i)
	{
		this->DropToRoleAndOutDropItem(role, dropid_list[i], monster_group_id, drop_item_list, multipel);
	}
	if (NULL != item_list)
	{
		if (!item_list->empty())
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutList(item_list->size(), &(*item_list)[0], put_reason);
			drop_item_list.insert(drop_item_list.end(), item_list->begin(), item_list->end());
		}
	}
	bool is_notice = false;
	if (this->IsUseActivityNotice(notice_get_reson))
	{
		RoleActivity * role_activity = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(activity_type);
		if (NULL != role_activity && !drop_item_list.empty())
		{	
			role_activity->OnSendResultNotice(is_win ? Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_WIN : Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_BATTLE_FAIL,
				&drop_item_list, NULL, param1, param2);
			is_notice = true;
		}
	}

	if (!is_notice && notice_get_reson != GET_REASON_INVALID)
	{
		role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)drop_item_list.size(), drop_item_list.empty() ? NULL : &drop_item_list[0], notice_get_reson, param1, param2);
	}

	return true;
}

bool DropPool::DropToRoleAndOutDropItem(Role * role, UInt16 dropid, int monster_group_id, std::vector<ItemConfigData>ARG_OUT & out_drop_item, int multipel)
{
	DropConfig *drop_config = DROPPOOL->GetDropConfig(dropid);
	if (NULL == drop_config) return false;

	DropItemCfg item_list[DropConfig::DROP_ITEM_PROB_MAX];

	int item_count = drop_config->RandDropItem(item_list);
	item_count = drop_config->GetFixDropItem(item_count, item_list);

	Int64 add_coin = 0;
	Int64 add_immortal_coin = 0;
	Int64 add_gold = 0;

	static ItemConfigData add_item_list[DropConfig::DROP_ITEM_PROB_MAX];
	int add_item_num = 0;

	if (item_count > 0 && item_count <= DropConfig::DROP_ITEM_PROB_MAX)
	{
		for (int i = 0; i < item_count; ++i)
		{
			const ItemConfigData & drop_item = item_list[i].item_config_data;
			switch (drop_item.item_id)
			{
				case COIN_ITEM_ID:
					add_coin += drop_item.num;
					break;
				case GOLD_ITEM_ID:
					add_gold += drop_item.num;
					break;
				case IMMORTAL_COIN_ITEM_ID:
					add_immortal_coin += drop_item.num;
					break;
				default:
					{
						const ItemBase * item_base = ITEMPOOL->GetItem(drop_item.item_id);
						if (NULL != item_base && add_item_num < DropConfig::DROP_ITEM_PROB_MAX)
						{
							// 判断物品是否要限制掉落，且RoleDrop没有解除限制
							if (item_base->IsLimitDrop() && 
								role->GetRoleModuleManager()->GetRoleDrop()->IsLockDrop(drop_item.item_id))
							{
								continue;
							}

							if (item_base->HasDropCoolDown() &&
								role->GetRoleModuleManager()->GetRoleDrop()->IsItemDropInCoolDown(drop_item.item_id))
							{
								continue;
							}

							// 转换物品且为无职业时，不发放奖励
							if (I_TYPE_CONVERT_ITEM == item_base->GetItemType() && 0 == role->GetProfession())
							{
								continue;
							}

							if (item_base->GetItemType() == I_TYPE_CONVERT_ITEM)
							{
								//自动转换的物品堆叠上限是99，转成成装备是1
								for (int index = 0; index < multipel; ++index)
								{
									add_item_list[add_item_num++] = drop_item;
								}
							}
							else
							{
								add_item_list[add_item_num] = drop_item;
								add_item_list[add_item_num++].num *= multipel;
							}
						}
					}
					break;
			}
		}

		if (add_coin > 0 || add_gold > 0 || add_immortal_coin > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->Add(add_gold * multipel, add_immortal_coin * multipel, add_coin * multipel, __FUNCTION__);
		}

		if (add_item_num > 0)
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutList(add_item_num, add_item_list, PUT_REASON_DROP);
			out_drop_item.insert(out_drop_item.end(), add_item_list, add_item_list + add_item_num);
		}

		// 记录掉落了什么， 至于有否成功掉落到背包，就要查看knapsack的log了
		gamelog::g_log_drop.buff_printf("DropPool::DropToRole role[%d,%s] drop_id[%d] from monster_group[%d], drop: coin[%lld] immortal_coin[%lld] gold[%lld] item_num[%d] item_list:{ ",
						role->GetUID(), role->GetName(), dropid, monster_group_id, add_coin* multipel, add_immortal_coin* multipel, add_gold* multipel, add_item_num);
		for (int i = 0; i < add_item_num; ++i)
		{
			gamelog::g_log_drop.buff_printf("[%d:%d:%d] ", add_item_list[i].item_id, add_item_list[i].num* multipel, add_item_list[i].is_bind ? 1 : 0);
		}
		gamelog::g_log_drop.commit_buff(LL_INFO);
		
	}

	return true;
}

bool DropPool::DropListToRoleAddReveal(Role * role, std::vector<ItemDataWrapper> add_show_list, int notice_get_reson, std::vector<ItemConfigData>* item_list, int put_reason, int param1, int param2)
{
	if(NULL == role) return false;

	std::vector<ItemConfigData> show_item_list;
	if (NULL != item_list && !item_list->empty())
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutList(item_list->size(), &(*item_list)[0], put_reason);
		show_item_list.insert(show_item_list.end(), item_list->begin(), item_list->end());
	}
	for (int i = 0; i < (int)add_show_list.size(); i++)
	{
		const ItemBase * item_base = ITEMPOOL->GetItem(add_show_list[i].item_id);
		if (NULL != item_base)
		{
			show_item_list.push_back(ItemConfigData(add_show_list[i].item_id, add_show_list[i].is_bind, add_show_list[i].num));
		}
	}

	if (notice_get_reson != GET_REASON_INVALID)
	{
		role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)show_item_list.size(), show_item_list.empty() ? NULL : &show_item_list[0], notice_get_reson, param1, param2);
	}

	return true;
}

bool DropPool::RandOutDropItem(Role* role, UInt16 dropid, int monster_group_id, std::vector<ItemConfigData>ARG_OUT & out_drop_item, int multipel, Int64 * out_add_coin/* = NULL*/, Int64 * out_add_immortal_coin/* = NULL*/, Int64 * out_add_gold/* = NULL*/)
{
	DropConfig *drop_config = DROPPOOL->GetDropConfig(dropid);
	if (NULL == drop_config) return false;

	DropItemCfg item_list[DropConfig::DROP_ITEM_PROB_MAX];

	int item_count = drop_config->RandDropItem(item_list);
	item_count = drop_config->GetFixDropItem(item_count, item_list);

	Int64 add_coin = 0;
	Int64 add_immortal_coin = 0;
	Int64 add_gold = 0;

	static ItemConfigData add_item_list[DropConfig::DROP_ITEM_PROB_MAX];
	int add_item_num = 0;

	if (item_count > 0 && item_count <= DropConfig::DROP_ITEM_PROB_MAX)
	{
		for (int i = 0; i < item_count; ++i)
		{
			const ItemConfigData & drop_item = item_list[i].item_config_data;
			switch (drop_item.item_id)
			{
			case COIN_ITEM_ID:
				add_coin += drop_item.num;
				break;
			case GOLD_ITEM_ID:
				add_gold += drop_item.num;
				break;
			case IMMORTAL_COIN_ITEM_ID:
				add_immortal_coin += drop_item.num;
				break;
			default:
				{
					const ItemBase * item_base = ITEMPOOL->GetItem(drop_item.item_id);
					if (NULL != item_base && add_item_num < DropConfig::DROP_ITEM_PROB_MAX)
					{
						// 判断物品是否要限制掉落，且RoleDrop没有解除限制
						if (item_base->IsLimitDrop() &&
							role->GetRoleModuleManager()->GetRoleDrop()->IsLockDrop(drop_item.item_id))
						{
							continue;
						}

						if (item_base->HasDropCoolDown() &&
							role->GetRoleModuleManager()->GetRoleDrop()->IsItemDropInCoolDown(drop_item.item_id))
						{
							continue;
						}

						// 转换物品且为无职业时，不发放奖励
						if (I_TYPE_CONVERT_ITEM == item_base->GetItemType() && 0 == role->GetProfession())
						{
							continue;
						}

						if (item_base->GetItemType() == I_TYPE_CONVERT_ITEM)
						{
							//自动转换的物品堆叠上限是99，转成成装备是1
							for (int index = 0; index < multipel; ++index)
							{
								add_item_list[add_item_num++] = drop_item;
							}
						}
						else
						{
							add_item_list[add_item_num] = drop_item;
							add_item_list[add_item_num++].num *= multipel;
						}
					}
				}
				break;
			}
		}

		if (add_coin > 0 || add_gold > 0 || add_immortal_coin > 0)
		{
			if (out_add_coin != NULL)*out_add_coin = add_coin;
			if (out_add_immortal_coin != NULL)*out_add_immortal_coin = add_immortal_coin;
			if (out_add_gold != NULL)*out_add_gold = add_gold;
		}

		if (add_item_num > 0)
		{
			out_drop_item.insert(out_drop_item.end(), add_item_list, add_item_list + add_item_num);
		}
	}

	return true;
}

bool DropPool::RandOutDropItemList(Role* role, const std::vector<int> & dropid_list, int monster_group_id, std::vector<ItemConfigData> *out_item_list, int multipel/* = 1*/, Int64 * out_add_coin/* = NULL*/, Int64 * out_add_immortal_coin/* = NULL*/, Int64 * out_add_gold/* = NULL*/)
{
	std::vector<ItemConfigData> drop_item_list;
	Int64 all_add_coin = 0;
	Int64 all_add_gold = 0;
	Int64 all_add_immortal_coin = 0;
	for (int i = 0; i < (int)dropid_list.size(); ++i)
	{
		Int64 add_coin = 0;
		Int64 add_gold = 0;
		Int64 add_immortal_coin = 0;
		this->RandOutDropItem(role, dropid_list[i], monster_group_id, drop_item_list, multipel, &add_coin, &add_gold, &add_immortal_coin);
		all_add_coin += add_coin;
		all_add_gold += add_gold;
		all_add_immortal_coin += add_immortal_coin;
	}
	if (NULL != out_item_list)
	{
		out_item_list->insert(out_item_list->end(), drop_item_list.begin(), drop_item_list.end());
	}
	if (out_add_coin != NULL)*out_add_coin = all_add_coin;
	if (out_add_gold != NULL)*out_add_gold = all_add_gold;
	if (out_add_immortal_coin != NULL)*out_add_immortal_coin = all_add_immortal_coin;
	return true;
}

bool DropPool::DropToRole(Role * role, UInt16 dropid)
{
	DropConfig *drop_config = DROPPOOL->GetDropConfig(dropid);
	if (NULL == drop_config) return false;

	DropItemCfg item_list[DropConfig::DROP_ITEM_PROB_MAX];

	int item_count = drop_config->RandDropItem(item_list);
	item_count = drop_config->GetFixDropItem(item_count, item_list);

	Int64 add_coin = 0;
	Int64 add_immortal_coin = 0;
	Int64 add_gold = 0;

	static ItemConfigData add_item_list[DropConfig::DROP_ITEM_PROB_MAX];
	int add_item_num = 0;

	if (item_count > 0 && item_count <= DropConfig::DROP_ITEM_PROB_MAX)
	{
		for (int i = 0; i < item_count; ++i)
		{
			ItemConfigData & drop_item = item_list[i].item_config_data;
			switch (drop_item.item_id)
			{
			case COIN_ITEM_ID:
				add_coin += drop_item.num;
				break;
			case GOLD_ITEM_ID:
				add_gold += drop_item.num;
				break;
			case IMMORTAL_COIN_ITEM_ID:
				add_immortal_coin += drop_item.num;
				break;
			default:
				{
					const ItemBase *item_base = ITEMPOOL->GetItem(drop_item.item_id);
					if (NULL != item_base && add_item_num < DropConfig::DROP_ITEM_PROB_MAX)
					{
						// 判断物品是否要限制掉落，且RoleDrop没有解除限制
						if (item_base->IsLimitDrop() &&
							role->GetRoleModuleManager()->GetRoleDrop()->IsLockDrop(drop_item.item_id))
						{
							continue;
						}

						if (item_base->HasDropCoolDown() &&
							role->GetRoleModuleManager()->GetRoleDrop()->IsItemDropInCoolDown(drop_item.item_id))
						{
							continue;
						}

						// 转换物品且为无职业时，不发放奖励
						if (I_TYPE_CONVERT_ITEM == item_base->GetItemType() && 0 == role->GetProfession())
						{
							continue;
						}

						if (item_list[i].is_active_limit &&
							SceneMineMonsterConfig::Instance().IsMeetActive(role->GetLevel(), role->GetRoleModuleManager()->GetActiveSystem()->GetActiveNum()))
						{
							drop_item.is_bind = false;
						}

						add_item_list[add_item_num++] = drop_item;
					}
				}
				break;
			}
		}

		if (add_coin > 0 || add_gold > 0 || add_immortal_coin > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->Add(add_gold, add_immortal_coin, add_coin, __FUNCTION__);
		}

		if (add_item_num > 0)
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutList(add_item_num, add_item_list, PUT_REASON_DROP);
		}
	}

	return true;
}

