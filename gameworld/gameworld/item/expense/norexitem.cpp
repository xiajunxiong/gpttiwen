#include "norexitem.hpp"
#include "obj/character/role.h"
#include "obj/character/robot.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "scene/scene.h"
#include "scene/map.h"
#include "world.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/noticenum.h"
#include "globalconfig/globalconfig.h"
#include "item/itempool.h"
#include "gamelog.h"
#include "limits.h"
#include "other/vip/vip.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/pet/pet.hpp"
#include "other/giftranditemconfig.hpp"
#include "other/partner/partner.hpp"
#include "other/workshop/workshop.hpp"
#include "other/surface/surface.hpp"
#include "item/knapsack.h"
#include "other/treasure_map/treasuremap.h"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "other/prestige/prestige.hpp"
#include "task/taskmanager.h"
#include "other/title/title.hpp"
#include "global/team/team.hpp"
#include "other/fabao/fabao.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "other/event/eventhandler.hpp"
#include "protocol/msgitem.h"
#include "other/roleattribute/roleexpconfig.hpp"
#include "other/robot/robotmanager.hpp"
#include "other/facescore/facescore.hpp"
#include "other/medal/medal.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityluckyturntable.hpp"
#include "other/roleactivity/impl/roleactivitysevencolorpicture.hpp"
#include "other/roleactivity/impl/roleactivitycowexchange.hpp"
#include "other/roleactivity/impl/roleactivitycowluckyexchange.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "other/roleactivity/impl/roleactivityjinyitianxiang.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/duel/duel.hpp"
#include "global/guild/guild.hpp"
#include "other/fuyu/fuyumanager.hpp"
#include "other/huanshou/rolehuanshou.hpp"

NorexItem::NorexItem() 
	: ItemBase(I_TYPE_EXPENSE), m_use_type(0), m_param1(0), m_param2(0)
{

}

NorexItem::~NorexItem()
{

}

int NorexItem::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	if (NULL == role || NULL == role->GetScene() || num <= 0 ) return -1;

	if (num > 1)
	{
		switch(m_use_type)
		{
		case I_NOREX_ITEM_JINGYAN_DAN:
		case I_NOREX_ITEM_IMMORTAL_COIN_CARD:
		case I_NOREX_ITEM_BIND_COIN_CARD:
		case I_NOREX_ITEM_GATHER_POWER:
		case I_NOREX_ITEM_GIFT:
		case I_NOREX_ITEM_VILLAGE_PRESTIGE:
		case I_NOREX_ITEM_FLOWERS:
		case I_NOREX_ITEM_GOLD_CHONGZHI_CARD:
		case I_NOREX_ITEM_TREASURE_MAP:
		case I_NOREX_LUCKYLOTTERY_TICKET:
		case I_NOREX_ITEM_EXP_PERSENT:
		case I_NOREX_ITEM_QIYUN:
		case I_NOREX_ITEM_JIFEN:
		case I_NOREX_ITEM_INTIMACY:
		case I_NOREX_ITEM_RECOVERY_POWER:
		case I_NOREX_ITEM_BUY_GIFT_ITEM:
		case I_NOREX_ITEM_DREAM_NOTES_CHALLENGE:
		case I_NOREX_ITEM_MONEY_AUTO_USE:
		case I_NOREX_ITEM_CONVERT_ITEM:
		case I_NOREX_ITEM_FORTUNE_MAZE:
		case I_NOREX_ITEM_ROLE_ATTR_PELEET:
		case I_NOREX_ITEM_CHANGE_CARD:
		case I_NOREX_ITEM_GUILD_BUILD_MATERIAL:
		case I_NOREX_ITEM_SILVER_COIN_CARD:
		case I_NOREX_ITEM_MONEY_BOX:
			break;

		default:
			role->NoticeNum(errornum::EN_ITEM_CANNOT_MULTI_USE);
			return -1;
		}
	}

	switch (m_use_type)
	{
	case I_NOREX_ITEM_HUANSHOU_EGG:
		{
			if (!role->GetRoleModuleManager()->GetRoleHuanShou()->OnOpenHuanShouEgg(m_param1))
			{
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_SMART_MOUNTS_SKILL:	
		{
			return -m_use_type;
		}
		break;
	case I_NOREX_ITEM_GUILD_BUILD_MATERIAL:
		{
			Guild *guild = role->GetGuild();
			if (NULL == guild)
			{
				role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_NOT_GUILD_CAN_NOT_USE);
				return -m_use_type;
			}

			guild->AddExp(m_param1*num);
			guild->SendAllUseBulidMaterialInfo(role, item_data.item_id, num);
		}
		break;
	case I_NOREX_ITEM_CHANGE_CARD:
		{
			if (!role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnUseChangeCard(m_param1, num, true,__FUNCTION__))
			{
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_NEW_LIFE_SKILL_PROPS:
		{
			if (!role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnUseProps(m_param1))
			{
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_FORTUNE_MAZE:
		{
			RoleActivityFortuneMaze * role_activity = static_cast<RoleActivityFortuneMaze * >
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
			if (NULL != role_activity)
			{
				role_activity->ItemAddTimes(num*m_param1);
				role_activity->OnParticipateRA(__FUNCTION__);
			}

			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_fortune_maze_notice,
								  num*m_param1, RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
				}
			}
		}
		break;

	case I_NOREX_ITEM_PET_GOD_PRINT:
		{
			if (NULL != role->GetRoleModuleManager()->GetPet()->AddPetGodPrintItem(item_data.item_id, m_param2, m_param3, m_param1, item_data.item_unique_id))
			{
				return true;
			}
		}
	case I_NOREX_ITEM_CONVERT_ITEM:
		{
			ItemConfigData item_data(m_param1, 1, m_param2 * num);
			if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(item_data.item_id, item_data.num))
			{
				role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return -m_use_type;
			}
			if (!role->GetRoleModuleManager()->GetKnapsack()->Put(item_data, PUT_REASON_NOREX_CONVERT_ITEM))
			{
				role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_CRESTAL_XIYOU_TIMES:
		{
			if (!role->GetRoleModuleManager()->GetRoleCrystalFb()->AddXiYouPlayTimesByNorexitem(m_param1))
			{
				return -m_use_type;
			}

			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_crystal_fb_xiyou_times_add_notice, m_param1);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
		}
		break;
	case I_NOREX_ITEM_MONEY_AUTO_USE:
		{
			if (!role->GetRoleModuleManager()->GetMoney()->OnAddMoney(m_param1, m_param2 * num, __FUNCTION__))
			{
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_DREAM_NOTES_CHALLENGE:
		{
			role->GetRoleModuleManager()->GetDreamNotesChallenge()->OnUseItemAddTimes(m_param1 * num);
		}
		break;
	case I_NOREX_ITEM_BUY_GIFT_ITEM:
		{
			int use_num = 0;
			int ret = 0;
			for (int i = 0; i < num; i++)
			{
				ret = role->GetRoleModuleManager()->OnBuyGiftItem(m_param1, m_param2, m_param3, m_item_id, __FUNCTION__, false);
				if (0 != ret)
				{	
					switch (ret)
					{
						case BUY_GIFT_ITEM_RET_TYPE_1:
						{
							int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_norex_item_use_item_invalid, m_item_id);
							if (length > 0)
							{
								World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
							}
						}
						break;
						case BUY_GIFT_ITEM_RET_TYPE_2:
						{
							int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_norex_item_use_item_num_limit, m_item_id);
							if (length > 0)
							{
								World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
							}
						}
						break;
						default:
							break;
					}
					break;
				}
				use_num += 1;
			}
			num = use_num;
		}
		break;
	case I_NOREX_ITEM_EQUIP:
		{
			if (NULL == ITEMPOOL->GetItem(m_param2))return -m_use_type;
			if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(m_param2, 1, false))
			{
				role->NoticeNum(errornum::EN_NO_CHECK_ITEM_NO_KNAPSACK);
				return -m_use_type;
			}
			
			role->GetRoleModuleManager()->GetKnapsack()->Put(ItemConfigData(m_param2, true, 1), PUT_REASON_NOREXITEM_EQUIP);
		}
		break;
	case I_NOREX_ITEM_RECOVERY_POWER:
		{
			if(m_param1 <= 0) return -m_use_type;

			int consume_num = 0;
			for (int i = 0; i < num; i++)
			{
				if (!role->GetRoleModuleManager()->GetRoleCrystalFb()->AddCrystalFbEnergyByItem(m_param1, __FUNCTION__))
				{
					if (0 == consume_num)
					{
						role->NoticeNum(errornum::EN_USE_CRYSTAL_FB_ENERGY_ITEM_UPPER_LIMIT);
						return -m_use_type;
					}
					break;
				}
				else
				{
					++consume_num;
				}
			}

			num = consume_num;
		}
		break;
	case I_NOREX_ITEM_INTIMACY:
		{
			if (m_param1 <= 0)return -m_use_type;
			RoleActivity * activity = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE);
			if (NULL == activity)return -m_use_type;

			RoleActivityCowLuckyExchange *mg_act = (RoleActivityCowLuckyExchange*)activity;
			mg_act->OnNoreItemUse(m_param1 * num);
		}
		break;
	case I_NOREX_ITEM_JIFEN:
		{
			if (m_param1 <= 0)return -m_use_type;
			RoleActivity * activity = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_COW_EXCHANGE);
			if (NULL == activity)return -m_use_type;
			
			RoleActivityCowExchange *mg_act = (RoleActivityCowExchange*)activity;
			mg_act->OnNoreItemUse(m_param1 * num);
		}
		break;
	case I_NOREX_ITEM_QIYUN:
		{
			if (m_param1 <= 0)return -m_use_type;
			long long real_add = m_param1 * num;
			if (!role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(real_add, MONEY_TYPE_LUCK_COIN, "NorexItem::Use"))
			{
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_EXP_PERSENT:
		{
			const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(role->GetLevel());
			if (cfg == NULL)return -m_use_type;
			long long add_exp = cfg->exp * m_param1 * num;
			if (add_exp % 100 > 0)
			{
				add_exp = add_exp / 100 + 1;
			}
			else
			{
				add_exp = add_exp / 100;
			}
			if (add_exp <= 0)return -m_use_type;
			role->AddExp(add_exp , ADD_EXP_TYPE_EXP_ITEM, __FUNCTION__);
		}
		break;
	case I_NOREX_ITEM_FACESROE_RES:
	case I_NOREX_ITEM_TITLE_CATD:
		{
			if (!role->GetRoleModuleManager()->GetFaceScore()->AutoOrUseActiveRes(m_param1, item_data.item_id, false))
			{
				return -m_use_type;
			}
		}
		break;

	case I_NOREX_ITEM_LIMITED_TIME_TITLE_CARD:
		{
			if (role->GetRoleModuleManager()->GetTitle()->AddTitleOrRenewDueTime(m_param2, __FUNCTION__, item_data.invalid_time) < 0)
			{
				return -m_use_type;
			}
		}
		break;

	case I_NOREX_LUCKYLOTTERY_TICKET:
		{	
			RoleActivityLuckyTurnTable * lucky = static_cast<RoleActivityLuckyTurnTable * >
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE));
			if (NULL != lucky)
			{
				if (!lucky->OnUseLuckLotteryTicket(m_param1 * num))
				{
					return -m_use_type;
				}
			}
		}
		break;
	case I_NOREX_ITEM_EXP:
		{
			int level = role->GetLevel();
			if (level <= m_param1)
			{
				const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(level);
				if (NULL == cfg) return  -m_use_type;
				long long up_level_exp = cfg->exp;
				role->AddExp(up_level_exp, ADD_EXP_TYPE_EXP_ITEM, __FUNCTION__);
			}
			else
			{
				if (!role->AddExp(m_param2, ADD_EXP_TYPE_EXP_ITEM, __FUNCTION__))
				{
					return -m_use_type;
				}
			}
		}
		break;
	case I_NOREX_ITEM_TREASURE_MAP:
		{
			int actual_use_num = 0;
			if (!role->GetRoleModuleManager()->GetTreasureMap()->UseTreasureMap(m_param1, num, &actual_use_num))
			{
				return -m_use_type;
			}
			num = actual_use_num;
		}
		break;
	case I_NOREX_ITEM_GOLD_CHONGZHI_CARD:
		{
			if (m_param1 <= 0 || m_param1 > 1000000)   // 安全检测，不可以负数，也不能太大（防止策划配错超大的数）
			{
				return -m_use_type;
			}
			
			if (!role->GetRoleModuleManager()->GetMoney()->AddGold(m_param1 * num, __FUNCTION__))
			{
				return -m_use_type;
			}
			EventHandler::Instance().OnAddChongzhi(role, m_param1 * num);
		}
		break;
	case I_NOREX_ITEM_FLOWERS:
		{
			RoleActivitySevenColorPicture* seven_color = static_cast<RoleActivitySevenColorPicture *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE));
			if (NULL != seven_color)
			{
				if (!seven_color->OnAddPigmentByItem(num))
				{
					return -m_use_type;
				}
			}
		}
		break;
	case I_NOREX_ITEM_FABAO:
		{
			int fabao_id = m_param1;
			if (role->GetRoleModuleManager()->GetFaBao()->IsActiveFaBao(fabao_id))
			{				
				const FaBaoInfoCfg * info_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(fabao_id);
				if (info_cfg == NULL)
				{
					return false;
				}

				ItemConfigData temp(info_cfg->active_item_id, m_param3 == 1 ? true : false, m_param2);

				//激活了直接转化成对应碎片
				if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut2(temp))
				{
					return -m_use_type;
				}
				
				role->GetRoleModuleManager()->GetKnapsack()->Put(temp, PUT_REASON_NOREXITEM_ITEM);
				std::vector<ItemConfigData> notice_list;
				notice_list.push_back(temp);
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)notice_list.size(), &notice_list[0], GET_REASON_ACTIVE_FA_BAO_GET_ITEM, m_param1, m_param2);
			}
			else
			{
				if (!role->GetRoleModuleManager()->GetFaBao()->ActiveFaBaoByItem(fabao_id))
				{
					return -m_use_type;
				}
			}	
		}
		break;
	case I_NOREX_ITEM_JINGYAN_DAN:
		{
			role->AddExp(m_param1 * num, ADD_EXP_TYPE_NOREX_ITEM, "ItemUse:JingYanDan");
		}
		break;

	case I_NOREX_ITEM_GIFT:
		{
			std::vector< BoxItemCfg> item_list;

			for (int i = 0; i < num; ++i)
			{
				if (0 == m_param3)
				{
					//默认只会获得一个物品
					//for (int i = 0; item_count < GIFT_ITEM_MAX_NUM && i < m_param1; ++i)
					{
						BoxItemCfg temp_cfg;
						if (!LOGIC_CONFIG->GetGiftRandItemconfig()->RandItem(m_param2, &temp_cfg))
						{
							return -m_use_type;
						}

						item_list.push_back(temp_cfg);
					}
				}
				else
				{
					const BoxItemCfg *box_cfg = LOGIC_CONFIG->GetGiftRandItemconfig()->GetBoxItemCfg(m_param2, param1);
					if (NULL != box_cfg)
					{
						item_list.push_back(*box_cfg);
					}
				}
			}

			if ((int)item_list.size() <= 0 || (int)item_list.size() >= GIFT_ITEM_MAX_NUM) return -m_use_type;

			std::vector< ItemDataWrapper> knapsack_item_list;
			std::vector< ItemConfigData> notice_list;
			std::vector< ItemConfigData> broadcast_list;

			for (int i = 0; i < (int)item_list.size(); i++)
			{
				const BoxItemCfg& item = item_list[i];
				ItemDataWrapper temp(item.item_data.item_id, item.item_data.num, item.item_data.is_bind);
				switch (item.item_type)
				{
				case ITEM_TYPE_PET:
					NetValueItemParam * param = (NetValueItemParam *)&temp.param_data;
					param->pet_param.str_level = m_param1;
					break;
				}

				knapsack_item_list.push_back(temp);

				//通知列表 (宠物不通知)
				if (item.item_type != ITEM_TYPE_PET)
				{
					ItemConfigData put_reward(temp.item_id, (0 != temp.is_bind), item.item_data.num);
					bool is_find = false;
					for (int index = 0; index < (int)notice_list.size(); index++)
					{
						if (notice_list[index].item_id == put_reward.item_id && notice_list[index].is_bind == put_reward.is_bind)
						{
							is_find = true;
							notice_list[index].num += put_reward.num;
						}
					}
					if (!is_find)
					{
						notice_list.push_back(put_reward);
					}
				}

				if (0 != item.is_broadcast)
				{
					broadcast_list.push_back(item.item_data);
				}
			}
			////////////////////////////判断/////////////////////////////////////////
			if (!knapsack_item_list.empty())
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList((short)knapsack_item_list.size(), &knapsack_item_list[0]))
				{
					return -m_use_type;
				}
			}

			////////////////////////////给予/////////////////////////////////////////
			if (!knapsack_item_list.empty())
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutList((short)knapsack_item_list.size(), &knapsack_item_list[0], PUT_REASON_GIFT_RAND_ITEM);
			
				if (!notice_list.empty())
				{//物品获得通知
					role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)notice_list.size(), &notice_list[0], GET_REASON_RAND_GIFT, m_item_id, num);
				}
			}	

			for (std::vector< ItemConfigData>::const_iterator it = broadcast_list.begin(); broadcast_list.end() != it; ++it)
			{
				static Protocol::SCSystemMsg sm;
				int sendlen = 0;
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_gift_randitem_is_broadcast_notice, role->GetName(), m_item_id, it->item_id);
				if (length > 0)
				{
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
					}
				}
			}
		}
		break;

	case I_NOREX_ITEM_IMMORTAL_COIN_CARD:
		{
			if (!role->GetRoleModuleManager()->GetMoney()->AddImmortalCoin(m_param1 * num, __FUNCTION__))
			{
				return -m_use_type;
			}
		}
		break;

	case I_NOREX_ITEM_PET_EGGS:
		{
			if (!role->GetRoleModuleManager()->GetPet()->UsePetEggItem(m_param1, item_data.is_bind))
			{
				return -m_use_type;
			}
		}
		break;

	case I_NOREX_ITEM_PET_SKILL:
		{
			int pet_index = param1;
			std::set<PetReplacePassiveSkillLockItem> lock_skill_index;
			int passive_skill_id = m_param1;
			int need_pet_quality = m_param2;
		
			for (int i = 0; i < BitCount(param2); ++i)
			{
				if (param2 & (1<<i))
				{
					PetReplacePassiveSkillLockItem tmp(PET_REPLACE_SKILL_INDEX_TYPE_DEFAULT, i);
					lock_skill_index.insert(tmp);
				}				
			}


			for (int i = 0; i < BitCount(param3); ++i)
			{
				if (param3 & (1 << i))
				{
					PetReplacePassiveSkillLockItem tmp(PET_REPLACE_SKILL_INDEX_TYPE_LIAN_YAO, i);
					lock_skill_index.insert(tmp);
				}
			}

			if (!role->GetRoleModuleManager()->GetPet()->ReplacePassiveSkill(pet_index, passive_skill_id, need_pet_quality, lock_skill_index))
			{
				return -m_use_type;
			}
		}
		break;

	case I_NOREX_ITEM_PARTNER_LICENCE:
		{
			if (!role->GetRoleModuleManager()->GetPartner()->ActiveAppointedPartner(item_data.item_id, UNLOCK_TYPE_ITEM_NOREX))
			{
				//放入失败直接转化成对应碎片
				if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(m_param1, m_param2))
				{
					return -m_use_type;
				}

				role->GetRoleModuleManager()->GetKnapsack()->Put(ItemConfigData(m_param1, m_param3 == 1 ? true : false, m_param2), PUT_REASON_NOREXITEM_ITEM);
				std::vector<ItemConfigData> notice_list;
				notice_list.push_back(ItemConfigData(m_param1, m_param3 == 1 ? true : false, m_param2));
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)notice_list.size(), &notice_list[0], GET_REASON_PARTNER_LIENCE, m_param1, m_param2);
			}
		}
		break;

	case I_NOREX_ITEM_WORKSHOP_RECIPE:
		{
			if (!role->GetRoleModuleManager()->GetWorkshop()->AutoActiveNewRecipe(m_param1, item_data.item_id, false))
			{
				return -m_use_type;
			}
		}
		break;

	case I_NOREX_ITEM_SURFACE:
		{
			int ret = role->GetRoleModuleManager()->GetSurface()->ActivateSurface(m_param1);
			if (ret < 0)
			{
				gamelog::g_log_surface.printf(LL_WARNING, "%s Error %d \n", "ActivateSurface", ret);

				return -m_use_type;
			}
		}
		break;

	case I_NOREX_ITEM_HP_RECOVER: // 非战斗状态吃药，暂未实现
	case I_NOREX_ITEM_HP_PERCENT_RECOVER:
		{
			if (role->GetRoleStatusManager()->IsInBattleStatus())
			{
				role->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
				return -m_use_type;
			}
			bool is_percent = (m_use_type == I_NOREX_ITEM_HP_PERCENT_RECOVER);
			Protocol::SCTeamMemberAddHpMpNotice help_add_notice_protocol;		//发给对方
			help_add_notice_protocol.is_to_use_pet = 0;
			help_add_notice_protocol.partner_id = 0;
			help_add_notice_protocol.item_id = item_data.item_id;

			switch (param1)
			{
				case 0:		//加自己血
					{
						if (!role->GetRoleModuleManager()->ChangeHp(m_param1, is_percent))
						{
							role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
							return -m_use_type;
						}
					}
					break;
				case 1:		//加自己血 p2:pet_index
					{
						if (!role->GetRoleModuleManager()->GetPet()->ChangeHP(param2, m_param1, true, is_percent))
						{
							return -m_use_type;
						}
					}
					break;
				case 2:		//加自己伙伴血  p2:partner_id
					{
						if (!role->GetRoleModuleManager()->GetPartner()->AlterPartnerHPOrMPValue(param2, MAX_TYPE_ONLY_CHP, m_param1, is_percent))
						{
							return -m_use_type;
						}
					}
					break;
				case 3:		//给队友加血,p2: team_member_index | p3:是否对宠物
					{
						Team * team = role->GetMyTeam();
						if (team == NULL)
						{
							return -m_use_type;
						}
						TeamMember * team_member = team->GetMemberInfo(param2);
						if (team_member == NULL)
						{
							return -m_use_type;
						}

						if (!team_member->IsRobot())
						{
							Role* member_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(team_member->uid));
							if (NULL != member_role)
							{
								if (param3 == 0)
								{
									if (!member_role->GetRoleModuleManager()->ChangeHp(m_param1, is_percent))
									{
										role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
										return -m_use_type;
									}
								}
								else
								{
									if (!member_role->GetRoleModuleManager()->GetPet()->ChangeHP(member_role->GetRoleModuleManager()->GetCommonData().new_pet_status_index[NEW_FIGHT_INDEX], m_param1, false, is_percent))
									{
										return -m_use_type;
									}
								}

								help_add_notice_protocol.from_team_member_index = team->GetMemberIndex(role);
								help_add_notice_protocol.is_to_use_pet = param3 != 0;
								EngineAdapter::Instance().NetSend(member_role->GetNetId(), &help_add_notice_protocol, sizeof(help_add_notice_protocol));
							}
							
						}
						else
						{
							Robot* robot = RobotManager::Instance().GetRobot(team_member->uid);
							if (NULL != robot)
							{
								if (param3 == 0)
								{
									if (!robot->ChangeHp(m_param1, is_percent))
									{
										role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
										return -m_use_type;
									}
								}
								else
								{
									if (!robot->ChangePetHp(m_param1, is_percent))
									{
										role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
										return -m_use_type;
									}
								}
							}	
						}	
					}
					break;
				case 4:		//给队长的伙伴加血,p2: 伙伴id
					{
						Team * team = role->GetMyTeam();
						if (team == NULL)
						{
							return -m_use_type;
						}

						Role * leader = team->GetLeaderRole();
						if (leader == NULL)
						{
							return -m_use_type;
						}

						if (!leader->GetRoleModuleManager()->GetPartner()->AlterPartnerHPOrMPValue(param2, MAX_TYPE_ONLY_CHP, m_param1, is_percent))
						{
							return -m_use_type;
						}

						help_add_notice_protocol.from_team_member_index = team->GetMemberIndex(role);
						help_add_notice_protocol.partner_id = param2;
						EngineAdapter::Instance().NetSend(leader->GetNetId(), &help_add_notice_protocol, sizeof(help_add_notice_protocol));
						
					}
					break;
				default:
					{
						return -m_use_type;
					}					
					break;
			}		

			Protocol::SCUseRecoveryNotice use_notice;		//发给使用者 飘字
			use_notice.item_id = item_data.item_id;
			use_notice.use_gold_type = 0;
			use_notice.param1 = param1;
			use_notice.param2 = param2;
			use_notice.param3 = param3;
			EngineAdapter::Instance().NetSend(role->GetNetId(), &use_notice, sizeof(use_notice));
		}
		break;

	case I_NOREX_ITEM_BIND_COIN_CARD:
		{
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(m_param1 * num, "UseNorexItemCoinCard");
		}
		break;

	case I_NOREX_ITEM_RECOVER_MP:
	case I_NOREX_ITEM_MP_PERCENT_RECOVER:
		{
			if (role->GetRoleStatusManager()->IsInBattleStatus())
			{
				role->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
				return -m_use_type;
			}
			bool is_percent = (m_use_type == I_NOREX_ITEM_MP_PERCENT_RECOVER);
			Protocol::SCTeamMemberAddHpMpNotice help_add_notice_protocol;  //发给对方
			help_add_notice_protocol.is_to_use_pet = 0;
			help_add_notice_protocol.partner_id = 0;
			help_add_notice_protocol.item_id = item_data.item_id;

			switch (param1)
			{
				case 0:		//加自己魔
					{
						if (!role->GetRoleModuleManager()->ChangeMp(m_param1, is_percent))
						{
							role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
							return -m_use_type;
						}
					}
					break;
				case 1:		//加自己宠物魔 p2:pet_index
					{
						if (!role->GetRoleModuleManager()->GetPet()->ChangeMp(param2, m_param1, false, is_percent))
						{
							return -m_use_type;
						}
					}
					break;
				case 2:		//加自己伙伴 p2:partner_id
					{
						if (!role->GetRoleModuleManager()->GetPartner()->AlterPartnerHPOrMPValue(param2, MAX_TYPE_ONLY_CMP, m_param1, is_percent))
						{
							return -m_use_type;
						}
					}
					break;
				case 3:		//给队友加魔,p2: team_member_index | p3:是否对宠物
					{
						Team * team = role->GetMyTeam();
						if (team == NULL)
						{
							return -m_use_type;
						}
						TeamMember *  team_member = team->GetMemberInfo(param2);

						if (team_member == NULL)
						{
							return -m_use_type;
						}

						if (!team_member->IsRobot())
						{
							Role* member_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(team_member->uid));
							if (NULL != member_role)
							{
								if (param3 == 0)
								{
									if (!member_role->GetRoleModuleManager()->ChangeMp(m_param1, is_percent))
									{
										role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
										return -m_use_type;
									}
								}
								else
								{
									if (!member_role->GetRoleModuleManager()->GetPet()->ChangeMp(member_role->GetRoleModuleManager()->GetCommonData().new_pet_status_index[NEW_FIGHT_INDEX], m_param1, is_percent))
									{
										return -m_use_type;
									}
								}

								help_add_notice_protocol.from_team_member_index = team->GetMemberIndex(role);
								help_add_notice_protocol.is_to_use_pet = param3 != 0;
								EngineAdapter::Instance().NetSend(member_role->GetNetId(), &help_add_notice_protocol, sizeof(help_add_notice_protocol));
							}
						}
						else
						{
							Robot* robot = RobotManager::Instance().GetRobot(team_member->uid);
							if (NULL != robot)
							{
								if (param3 == 0)
								{
									if (!robot->ChangeMp(m_param1, is_percent))
									{
										role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
										return -m_use_type;
									}
								}
								else
								{
									if (!robot->ChangePetMp(m_param1, is_percent))
									{
										role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
										return -m_use_type;
									}
								}
							}
						}
					}
					break;
				case 4:		//给队长的伙伴加魔 , p2: 伙伴id
					{
						Team * team = role->GetMyTeam();
						if (team == NULL)
						{
							return -m_use_type;
						}

						Role * leader = team->GetLeaderRole();
						if (leader == NULL)
						{
							return -m_use_type;
						}

						if (!leader->GetRoleModuleManager()->GetPartner()->AlterPartnerHPOrMPValue(param2, MAX_TYPE_ONLY_CMP, m_param1, is_percent))
						{
							return -m_use_type;
						}

						help_add_notice_protocol.from_team_member_index = team->GetMemberIndex(role);
						help_add_notice_protocol.partner_id = param2;
						EngineAdapter::Instance().NetSend(leader->GetNetId(), &help_add_notice_protocol, sizeof(help_add_notice_protocol));
						
					}
					break;
				default:
					{
						return -m_use_type;
					}
					break;
			}

			Protocol::SCUseRecoveryNotice use_notice;		//发给使用者 飘字
			use_notice.item_id = item_data.item_id;
			use_notice.use_gold_type = 0;
			use_notice.param1 = param1;
			use_notice.param2 = param2;
			use_notice.param3 = param3;
			EngineAdapter::Instance().NetSend(role->GetNetId(), &use_notice, sizeof(use_notice));
		}
		break;
	case I_NOREX_ITEM_GATHER_POWER:
		{
			int real_comsume_num = 0;
			for (int i = 0; i < num; ++i)
			{
				if (!role->GetRoleModuleManager()->GetRoleGatherFb()->AddGatherPower(m_param1))
				{
					if (real_comsume_num == 0)
					{
						role->NoticeNum(errornum::EN_GATHER_POWER_FULLY);
						return -m_use_type;
					}

					break;
				}
				else
				{
					real_comsume_num += 1;
				}
			}

			num = real_comsume_num;
		}
		break;
	case I_NOREX_ITEM_VILLAGE_PRESTIGE:
		{
			if (!role->GetRoleModuleManager()->GetPrestige()->AddPrestige(m_param1, m_param2, __FUNCTION__, &num, item_data.item_id))
			{
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_ADVTASK_TRIGGER:
		if (!role->GetRoleModuleManager()->GetTaskManager()->OnUseItemToTriggerAdventureTask(m_item_id))
		{
			return -m_use_type;
		}
		break;
	case I_NOREX_ITEM_OPTIONAL_ITEM:
		{
			if (ITEM_PARAM_TYPE_OPTIOAN_GIFT_2 != param_type)
			{
				return -m_use_type;
			}

			long long select_flag = *(long long *)param;

			std::vector< ItemConfigData> knapsack_item_list;
			for (int i = 0; i < num; ++i)
			{
				for (int select_index = 0; select_index < sizeof(select_flag) * 8; ++select_index)
				{
					if (select_flag & (1LL << select_index))
					{
						const BoxItemCfg *box_cfg = LOGIC_CONFIG->GetGiftRandItemconfig()->GetBoxItemCfg(m_param1, select_index);
						if (NULL == box_cfg)
						{
							return -m_use_type;
						}

						knapsack_item_list.push_back(box_cfg->item_data);
					}
				}
			}

			if (knapsack_item_list.empty() || (int)knapsack_item_list.size() != m_param2) return -m_use_type;
			if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList((short)knapsack_item_list.size(), &knapsack_item_list[0]))
			{
				return -m_use_type;
			}

			if (!role->GetRoleModuleManager()->GetKnapsack()->PutList((short)knapsack_item_list.size(), &knapsack_item_list[0], PUT_REASON_GIFT_OPTIONAL_ITEM))
			{
				return -m_use_type;
			}
			else
			{
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)knapsack_item_list.size(), &knapsack_item_list[0], GET_REASON_RAND_GIFT, m_item_id, num);
			}
		}
		break;

	case I_NOREX_ITEM_INST_KILL_MONSTER:
		{
			role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);
			return -1;
		}
		break;
	case I_NOREX_ITEM_MEDAL_SP:
		{
			if (!role->GetRoleModuleManager()->GetMedal()->UseAddSpItem(m_param1, param1, param2, (0 != param3)))
			{
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_PET_SKIN:
		{
			if (!role->GetRoleModuleManager()->GetPet()->ActivityPetSkin(m_param1, num))
			{
				return -m_use_type;
			}
		}
		break;

	case I_NOREX_ITEM_PARTNER_SKIN:
		{
			if (!role->GetRoleModuleManager()->GetPartner()->ActiveSkin(m_param1, m_param2))
			{
				return -m_use_type;
			}
		}
		break;

	case I_NOREX_ITEM_DUEL_REDUCE:
		{
			if (!role->GetRoleModuleManager()->GetDuel()->IsRedName())
			{
				role->NoticeNum(errornum::EN_DUEL_NOT_HAS_KILL_VALUE);
				return -m_use_type;
			}
			if (!role->GetRoleModuleManager()->GetDuel()->CanUseRedeceItem())
			{
				role->NoticeNum(errornum::EN_TODAY_USE_COUNT_LIMIT);
				return -m_use_type;
			}

			if (!role->GetRoleModuleManager()->GetDuel()->ChangeKillValue(-m_param1, __FUNCTION__))
			{
				return -m_use_type;
			}
			else
			{
				role->GetRoleModuleManager()->GetDuel()->AddUseRedeceItemTime();
			}
		}
		break;
	case I_NOREX_ITEM_ROLE_ATTR_PELEET:
		{
			std::vector<ItemConfigData> convert_item_list;
			if (!role->GetRoleModuleManager()->GetKnapsack()->OnRoleAttrPelletConvert(ItemConfigData(m_item_id, true, num), convert_item_list) || convert_item_list.empty())
			{
				return -m_use_type;
			}
			if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(convert_item_list))
			{
				role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return -m_use_type;
			}
			if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(convert_item_list, PUT_REASON_NOREX_CONVERT_ITEM))
			{
				role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_SILVER_COIN_CARD:
		{
			if (!role->GetRoleModuleManager()->GetMoney()->OnAddMoney(MONEY_TYPE_SILVER_COIN, ((long long)m_param1) * num, __FUNCTION__))
			{
				return -m_use_type;
			}
		}
		break;
	case I_NOREX_ITEM_MONEY_BOX:
		{
			Protocol::SCHongBaoUseRet reward_ret;
			reward_ret.count = 0;

			int use_num = 0;

			for (int i = 0; i < num; ++i)
			{
				const GiftRandItemconfig::RedBagCfg::ItemInfo * reward_cfg
					= LOGIC_CONFIG->GetGiftRandItemconfig()->GetRandRedBagReward(m_param1);
				if (NULL == reward_cfg)
				{
					continue;
				}

				if (!role->GetRoleModuleManager()->GetMoney()->OnAddMoney(reward_cfg->money_type, reward_cfg->num, __FUNCTION__))
				{
					continue;
				}

				if (reward_ret.count < ARRAY_LENGTH(reward_ret.item_list))
				{
					Protocol::SCHongBaoUseRet::Item & reward_item = reward_ret.item_list[reward_ret.count++];
					reward_item.money_type = reward_cfg->money_type;
					reward_item.num = reward_cfg->num;
				}
	
				use_num += 1;
			}

			if (use_num == 0)
			{
				return -m_use_type;
			}

			num = use_num;
			unsigned int send_len = sizeof(reward_ret) - (ARRAY_LENGTH(reward_ret.item_list) - reward_ret.count) * sizeof(reward_ret.item_list[0]);

			//奖励通知
			role->GetRoleModuleManager()->NetSend(&reward_ret, send_len);
		}
		break;
	case I_NOREX_ITEM_DAILY_CUMULATIVE_RECHARGE:
		{
			RoleActivityCommonParam & common_data = role->GetRoleModuleManager()->GetRoleActivityManager()->GetCommonParam();
			common_data.chongzhi_num += m_param1;

			role->GetRoleModuleManager()->GetRoleActivityManager()->SendChongZhiInfo();
		}
		break;	
	case I_NOREX_ITEM_JIN_YI_TIAN_XIANG_ADD_VALUE:
		{
			RoleActivityJinYiTianXiang * role_activity = static_cast<RoleActivityJinYiTianXiang *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG));

			if (role_activity)
			{
				if (!role_activity->OnUseItem(m_param1 * num))
				{
					return -m_use_type;
				}				
			}
		}
		break;
	default:
		return -100;
	}

	return 0;
}


