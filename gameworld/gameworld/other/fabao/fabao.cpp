#include "fabao.hpp"
#include "engineadapter.h"
#include "world.h"
#include "other/fabao/fabaoconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "item/money.h"
#include "servercommon/errornum.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgbaby.hpp"
#include "servercommon/noticenum.h"
#include "other/marriage/marriage.hpp"
#include "global/team/team.hpp"
#include "other/baby/babymanager.hpp"
#include "item/itemextern.hpp"
#include "item/itempool.h"
#include "protocol/msgfabao.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/event/eventhandler.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "gamelog.h"
#include "servercommon/logdef.h"
#include "battle/battle_manager_local.hpp"
#include "protocol/msgrole.h"
#include "other/route/mailroute.hpp"
#include "task/taskmanager.h"


FaBao::FaBao() : m_role_module_mgr(NULL)
{
}

FaBao::~FaBao()
{
}

void FaBao::Init(RoleModuleManager * role_module_manager, const FaBaoParam & param, const FaBaoDrawParam & draw_param)
{
	m_role_module_mgr = role_module_manager;
	m_role_module_mgr = role_module_manager;
	m_param = param;
	m_draw_param = draw_param;
}

void FaBao::GetOtherInitParam(FaBaoParam * param, FaBaoDrawParam * draw_param)
{
	*param = m_param;
	*draw_param = m_draw_param;
}

const AttributeList & FaBao::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		for (int i = 0; i < MAX_FABAO_COUNT; ++i)
		{
			if (!this->IsActiveFaBao(i))continue;
			FaBaoEquip & equip = m_param.m_fabao_list[i];
			int fabao_id = equip.fabao_index;

			const FaBaoLevelItemCfg * level_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoLevelItemCfg(fabao_id, equip.fabao_level);
			if (level_cfg != NULL)
			{
				for (int attr_index = 0; attr_index < (int)level_cfg->attr_cfg.size(); ++attr_index)
				{
					const FaBaoLevelItemCfg::FaBaoAttrItem & attr_item = level_cfg->attr_cfg[attr_index];
					if (0 <= attr_item.attr_type && attr_item.attr_type < BATTLE_ATTR_MAX)
					{
						m_attrs_add.m_attrs[attr_item.attr_type] += attr_item.attr_value;
					}
				}
			}

			const FaBaoInfoCfg * cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(fabao_id);
			if (NULL == cfg || cfg->target != FABAO_TARGET_ROLE || cfg->fabao_type != FABAO_TYPE_PASSIVE)
			{
				continue;
			}

			const FaBaoJinJieItemCfg * jin_jie_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoJinJieItemCfg(fabao_id, equip.fabao_jie);
			if (jin_jie_cfg == NULL)
			{
				continue;
			}

			if (!jin_jie_cfg->is_percentage)
			{
				if (0 <= cfg->effect && cfg->effect < BATTLE_ATTR_MAX)
				{
					int add_value = 0;
					if (IsRatioAttrType(cfg->effect))
					{
						add_value = static_cast<Attribute>(jin_jie_cfg->advanced_addition / (double)FaBao::FA_BAO_ATTR_CAL_RATE * 10);
					}
					else
					{
						add_value = static_cast<Attribute>(jin_jie_cfg->advanced_addition / (double)FaBao::FA_BAO_ATTR_CAL_RATE);
					}
										
					m_attrs_add.m_attrs[cfg->effect] += add_value;
				}
			}
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

const AttributeList & FaBao::ReCalcAttrPer(AttributeList & base_add, const AttributeList & cpy_attr_list)
{
	m_attrs_add_per.Reset();
	for (int i = 0; i < MAX_FABAO_COUNT; ++i)
	{
		if (!this->IsActiveFaBao(i))continue;
		FaBaoEquip & equip = m_param.m_fabao_list[i];
		int fabao_id = equip.fabao_index;

		const FaBaoInfoCfg * cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(fabao_id);
		if (NULL == cfg || cfg->target != FABAO_TARGET_ROLE || cfg->fabao_type != FABAO_TYPE_PASSIVE)
		{
			continue;
		}

		const FaBaoJinJieItemCfg * jin_jie_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoJinJieItemCfg(fabao_id, equip.fabao_jie);
		if (jin_jie_cfg == NULL)
		{
			continue;
		}

		if (jin_jie_cfg->is_percentage)
		{
			if (0 <= cfg->effect && cfg->effect < BATTLE_ATTR_MAX)
			{
				m_attrs_add_per.m_attrs[cfg->effect] += static_cast<Attribute>(m_attrs_add_per.m_attrs[cfg->effect] * (1 + jin_jie_cfg->advanced_addition / (double)FaBao::FA_BAO_ATTR_CAL_RATE));
			}
		}
	}
	
	base_add.Add(m_attrs_add_per.m_attrs);
	return m_attrs_add_per;
}

void FaBao::OnRoleLogin()
{
	bool need_recale = false;
	if (m_role_module_mgr->GetCommonData().fabao_tranf <= 0)
	{
		m_role_module_mgr->GetCommonData().fabao_tranf = 1;
		//触发羁绊系统，返还超越羁绊最低级法宝的法宝消耗材料
		
		int group_max = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoMaxJiBanGroup();
		if (group_max > 0)
		{
			for (int group_id = 0; group_id < group_max && group_id < 50; group_id++)
			{
				const FaBaoJiBanGroupCfg * jiban_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoJiBanByGroupCfg(group_id);
				if (jiban_cfg != NULL)
				{
					bool init = false;
					int group_low_level = 1;//默认最低0阶1级
					int group_low_jie = 0;
					for (std::vector<FaBaoJiBanCfg>::const_iterator it = jiban_cfg->m_jiban_vec.begin(); it != jiban_cfg->m_jiban_vec.end(); it++)
					{
						if (it->id < 0 || it->id >= MAX_FABAO_COUNT)continue;
						FaBaoEquip &jiban_fabao = m_param.m_fabao_list[it->id];
						if (init || (group_low_level > jiban_fabao.fabao_level && jiban_fabao.fabao_level > 0))
						{
							group_low_jie = jiban_fabao.fabao_jie;
							group_low_level = jiban_fabao.fabao_level;
							if (group_low_level < 0)group_low_level = 1;
							init = true;
						}
					}
					static const int BUFFER_LEN = 1024;
					static char buffer[BUFFER_LEN];
					memset(buffer, 0, sizeof(buffer));
					int cur_len = 0;
					for (std::vector<FaBaoJiBanCfg>::const_iterator it = jiban_cfg->m_jiban_vec.begin(); it != jiban_cfg->m_jiban_vec.end(); it++)
					{
						if (it->id < 0 || it->id >= MAX_FABAO_COUNT)continue;
						FaBaoEquip &jiban_fabao = m_param.m_fabao_list[it->id];
						if (!this->IsActiveFaBao(it->id))continue;

						if (jiban_fabao.fabao_level > group_low_level)
						{
							std::vector<ItemConfigData> back_item_vec;
							int back_coin = 0;

							//返还新等级 -》 原等级的材料
							for (int level_idx = group_low_level; level_idx < jiban_fabao.fabao_level; level_idx++)
							{
								const FaBaoLevelItemCfg * old_level_cost_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetOldFaBaoLevelItemCfg(it->id, level_idx);
								if (old_level_cost_cfg != NULL)
								{
									back_coin += old_level_cost_cfg->consume_coin;
									if (NULL != ITEMPOOL->GetItem(old_level_cost_cfg->consume_item))back_item_vec.push_back(ItemConfigData(old_level_cost_cfg->consume_item, false, old_level_cost_cfg->consume_num));
								}
							}
							//返还新突破 -》 原突破的材料
							for (int jie_idx = group_low_jie; jie_idx < jiban_fabao.fabao_jie; jie_idx++)
							{
								const FaBaoJinJieItemCfg * old_jie_cost_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetOldFaBaoJinJieItemCfg(it->id, jie_idx);
								if (old_jie_cost_cfg != NULL)
								{
									back_coin += old_jie_cost_cfg->consume_coin;
									if (old_jie_cost_cfg->consume_item.size() > 0)
									{
										for (std::vector<FaBaoJinJieItemCfg::FaBaoJinJieConsumeItem>::const_iterator it2 = old_jie_cost_cfg->consume_item.begin(); it2 != old_jie_cost_cfg->consume_item.end(); it2++)
										{
											back_item_vec.push_back(ItemConfigData(it2->consume_item_id, false, it2->consume_item_num));
										}
									}
								}
							}

							//返还差值等级
							for (int level_idx = 1; level_idx < group_low_level; level_idx++)
							{
								const FaBaoLevelItemCfg *  level_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoLevelItemCfg(it->id, level_idx);
								const FaBaoLevelItemCfg *  level_cfg2 = LOGIC_CONFIG->GetFaBaoConfig()->GetOldFaBaoLevelItemCfg(it->id, level_idx);
								if (level_cfg != NULL && level_cfg2 != NULL)
								{
									if (level_cfg->consume_coin < level_cfg2->consume_coin)back_coin += level_cfg2->consume_coin - level_cfg->consume_coin;
									if (level_cfg->consume_item == level_cfg2->consume_item && level_cfg->consume_num < level_cfg2->consume_num)
									{
										back_item_vec.push_back(ItemConfigData(level_cfg2->consume_item, false, level_cfg2->consume_num - level_cfg->consume_num));
									}
								}
							}

							//返还差值阶
							for (int jie_idx = 0; jie_idx < group_low_jie; jie_idx++)
							{
								const FaBaoJinJieItemCfg * new_jie_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoJinJieItemCfg(it->id, jie_idx);
								const FaBaoJinJieItemCfg * old_jie_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetOldFaBaoJinJieItemCfg(it->id, jie_idx);
								if (new_jie_cfg != NULL && old_jie_cfg != NULL)
								{
									if (new_jie_cfg->consume_coin < old_jie_cfg->consume_coin)back_coin += old_jie_cfg->consume_coin - new_jie_cfg->consume_coin;

									if (new_jie_cfg->consume_item.size() == old_jie_cfg->consume_item.size())
									{
										for (int item_idx = 0; item_idx < new_jie_cfg->consume_item.size(); item_idx++)
										{
											if (new_jie_cfg->consume_item[item_idx].consume_item_id == old_jie_cfg->consume_item[item_idx].consume_item_id &&
												new_jie_cfg->consume_item[item_idx].consume_item_num < old_jie_cfg->consume_item[item_idx].consume_item_num)
											{
												back_item_vec.push_back(ItemConfigData(new_jie_cfg->consume_item[item_idx].consume_item_id, false, old_jie_cfg->consume_item[item_idx].consume_item_num - new_jie_cfg->consume_item[item_idx].consume_item_num));
											}
										}
									}
								}
							}

							static MailContentParam contentparam; contentparam.Reset();
							contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = back_coin;
							int index = 0;
							for (std::vector<ItemConfigData>::iterator back_it = back_item_vec.begin(); back_it != back_item_vec.end() && index < MAX_ATTACHMENT_ITEM_NUM; back_it++)
							{
								bool has_same = false;
								for (int item_idx = 0; item_idx < index && item_idx < MAX_ATTACHMENT_ITEM_NUM; item_idx++)
								{
									if (contentparam.item_list[item_idx].item_id == back_it->item_id)
									{
										contentparam.item_list[item_idx].num += back_it->num;
										has_same = true;
										break;
									}
								}
								if (has_same)continue;
								contentparam.item_list[index++] = ItemDataWrapper(back_it->item_id, back_it->num, back_it->is_bind ? 1 : 0);
							}

							int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_fabao_back_subject);
							int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_fabao_back_content);
							if (lensub > 0 && length > 0)
							{
								MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
							}

							jiban_fabao.fabao_level = group_low_level;
							jiban_fabao.fabao_jie = group_low_jie;
							jiban_fabao.cur_exp = 0;
							need_recale = true;
							for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < index; i++)
							{
								int len = SNPRINTF(buffer + cur_len, BUFFER_LEN - cur_len, "[item_id:%d,is_bind:%d,num:%d],",
									(int)contentparam.item_list[i].item_id, (int)contentparam.item_list[i].is_bind, contentparam.item_list[i].num);
								if (len > 0)
								{
									cur_len += len;
								}
							}
						}
					}
					gamelog::g_log_fa_bao.printf(LL_INFO, "back_first user[%s,%d] back_item_list[%s]", m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), buffer);
				}
			}
		}
	}

	if (m_role_module_mgr->GetCommonData().fabao_tranf <= 1)
	{
		//二次返还
		static MailContentParam contentparam; contentparam.Reset();
		const FaBaoSecondReturnBackListCfg * user_back = LOGIC_CONFIG->GetFaBaoConfig()->GetUserFaBaoSecondRetrunCfg(m_role_module_mgr->GetUid());
		if (user_back != NULL)
		{
			for (std::vector<FaBaoSecondReturnBackCfg>::const_iterator it = user_back->m_user_back_vec.begin(); it != user_back->m_user_back_vec.end(); it++)
			{
				const FaBaoSecondBackCfg * back_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoItemIdBackCfg(it->item_id);
				if (NULL != back_cfg)
				{
					int dis = back_cfg->original_price - back_cfg->present_price;
					if (dis > 0)
					{
						contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN] += dis * it->item_num;
						gamelog::g_log_fa_bao.printf(LL_INFO, "FaBaoBack2_deatil user[%d,%s] mail_id[%s] discount[%d] num[%d]",
							m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), it->mail_id, dis, it->item_num);
					}
				}
			}
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_fabao_back_subject2);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_fabao_back_content2);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
				gamelog::g_log_fa_bao.printf(LL_INFO, "FaBaoBack2 user[%d,%s] fabao_tranf[%d] add_money[virtual_type:%d,num:%d]",
					m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetCommonData().fabao_tranf,
					MAIL_VIRTUAL_SILVER_COIN, contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN]);
			}
		}
		m_role_module_mgr->GetCommonData().fabao_tranf = 2;
	}

	if (need_recale)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FABAO, __FUNCTION__);
		m_role_module_mgr->GetPet()->ReCalcAttrByFaBao();//法宝调用宠物属性修改
	}
	this->SendFaBaoAllInfo();
}

void FaBao::OnWeekChange()
{
	m_param.WeekReset();
}

void FaBao::GMsetTranf(int tranf)
{
	m_role_module_mgr->GetCommonData().fabao_tranf = tranf;
}

void FaBao::ActiveFaBao(short fabao_index, bool is_from_draw)
{
	if (fabao_index < 0 || fabao_index >= MAX_FABAO_COUNT)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_NO_THIS_FABAO);
		return;
	}

	if (IsActiveFaBao(fabao_index))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_HAS_ACTIVE_FABAO);
		return;
	}

	//检查消耗
	const FaBaoInfoCfg * info_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(fabao_index);
	if (info_cfg == NULL)
	{
		return;
	}

	if (!is_from_draw)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(info_cfg->active_item_id, info_cfg->active_item_num, __FUNCTION__))
		{
			return;
		}
	}

	m_param.m_fabao_list[fabao_index].fabao_index = fabao_index;
	m_param.m_fabao_list[fabao_index].fabao_level = 1;
	m_param.m_fabao_list[fabao_index].fabao_type = info_cfg->fabao_type;
	m_param.m_fabao_list[fabao_index].fabao_jie = 0;
	m_param.m_fabao_list[fabao_index].cur_exp = 0;

	m_param.m_fabao_list[fabao_index].level_effect = info_cfg->effect;
	EventHandler::Instance().OnAddFaBao(m_role_module_mgr->GetRole(), fabao_index);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FABAO, __FUNCTION__, false, is_from_draw ? Protocol::SCRoleAttrList::NOTIFY_REASON_FA_BAO_DRAW : Protocol::SCRoleAttrList::NOTIFY_REASON_NORMAL);
	m_role_module_mgr->GetPet()->ReCalcAttrByFaBao();//法宝调用宠物属性修改
	this->SendSignalFaBaoInfo(fabao_index, Protocol::SCFabaoSignInfo::SEND_INFO_TYPE_ACTIVE);
	ROLE_LOG_QUICK6(LOG_TYPE_FABAO_ACTIVE, m_role_module_mgr->GetRole(), fabao_index, m_param.m_fabao_list[fabao_index].fabao_level, __FUNCTION__, NULL);

	m_role_module_mgr->GetTaskManager()->OnFaBaoLevelChange();
}

bool FaBao::UpLevelFaBao(short fabao_index, bool is_notie)//--
{
	if (fabao_index < 0 || fabao_index >= MAX_FABAO_COUNT)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_NO_THIS_FABAO);
		return false;
	}

	if (!this->IsActiveFaBao(fabao_index))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_NO_ACTIVE_FABAO);
		return false;
	}

	FaBaoEquip & equip_info = m_param.m_fabao_list[fabao_index];
	//配置根据法宝等级获取法宝下一级的进阶等级
	const FaBaoLevelItemCfg * next_level_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoLevelItemCfg(fabao_index, equip_info.fabao_level + 1);
	if (next_level_cfg == NULL)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_LEVEL_MAX_ERR);
		return false;
	}

	const FaBaoLevelItemCfg * cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoLevelItemCfg(fabao_index, equip_info.fabao_level);
	if (cfg == NULL)
	{
		return false;
	}

	if (equip_info.fabao_jie < cfg->need_advanced_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_LEVEL_ERR);
		return false;
	}

	//检查羁绊
	const FaBaoJiBanGroupCfg * jiban_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoJiBanByIdCfg(fabao_index);
	if (jiban_cfg != NULL)
	{
		for (std::vector<FaBaoJiBanCfg>::const_iterator it = jiban_cfg->m_jiban_vec.begin(); it != jiban_cfg->m_jiban_vec.end(); it++)
		{
			if (it->id != fabao_index)
			{
				if (it->id < 0 || it->id >= MAX_FABAO_COUNT)continue;
				FaBaoEquip & jiban_fabao_equip = m_param.m_fabao_list[it->id];
				if (jiban_fabao_equip.fabao_jie < equip_info.fabao_jie)
				{
					//请先升级同一羁绊法宝
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_fabao_uplevel_limit_message);
					if (length > 0)
					{
						Protocol::SCSystemMsg sm;
						int sendlen = 0;
						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
						{
							m_role_module_mgr->NetSend((const char*)&sm, sendlen);
						}
					}
					return false;
				}
			}
		}
	}

	// 配置获取法宝对应等级上限
	const FaBaoOtherCfg & other_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoOtehrCfg();

	{
		Money * money = m_role_module_mgr->GetMoney();
		if (!money->CoinBindMoreThan(cfg->consume_coin))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return false;
		}

		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->consume_item, cfg->consume_num, __FUNCTION__))
		{
			return false;
		}

		if (!money->UseCoinBind(cfg->consume_coin, "FaBao::UpLevelFaBao"))
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return false;
		}
	}

	equip_info.fabao_level += 1;
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FABAO, __FUNCTION__);
	ROLE_LOG_QUICK6(LOG_TYPE_FABAO_UP, m_role_module_mgr->GetRole(), fabao_index, equip_info.fabao_level, __FUNCTION__, "uplevel");
	EventHandler::Instance().OnFaBaoLevelUp(m_role_module_mgr->GetRole(), fabao_index, equip_info.fabao_level);
	if (is_notie)
	{
		this->SendSignalFaBaoInfo(fabao_index, Protocol::SCFabaoSignInfo::SEND_INFO_TYPE_UPLEVEL);
	}

	m_role_module_mgr->GetTaskManager()->OnFaBaoLevelChange();

	return true;
}

int FaBao::AdvancedFaBao(short fabao_index)
{
	if (fabao_index < 0 || fabao_index >= MAX_FABAO_COUNT)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_NO_THIS_FABAO);
		return-__LINE__;
	}
	if (!this->IsActiveFaBao(fabao_index))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_NO_ACTIVE_FABAO);
		return-__LINE__;
	}
	FaBaoEquip & fabao_equip = m_param.m_fabao_list[fabao_index];

	const FaBaoLevelItemCfg * level_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoLevelItemCfg(fabao_index, fabao_equip.fabao_level);
	if (level_cfg == NULL)
	{
		return-__LINE__;
	}

	if (fabao_equip.fabao_jie >= level_cfg->need_advanced_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_LEVEL_NEED_ERR);
		return-__LINE__;
	}

	const FaBaoJinJieItemCfg * cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoJinJieItemCfg(fabao_index, fabao_equip.fabao_jie);
	if (cfg == NULL)
	{
		return-__LINE__;
	}

	const FaBaoJinJieItemCfg * next_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoJinJieItemCfg(fabao_index, fabao_equip.fabao_jie + 1);
	if (next_cfg == NULL)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_JIE_MAX_ERR);
		return-__LINE__;
	}

	std::vector<ItemExtern::ItemConsumeConfig> stuff_list;
	for (int i = 0; i < (int)cfg->consume_item.size(); ++i)
	{
		const FaBaoJinJieItemCfg::FaBaoJinJieConsumeItem & curr = cfg->consume_item[i];

		ItemExtern::ItemConsumeConfig  stuff_con;
		stuff_con.item_id = curr.consume_item_id;
		stuff_con.num = curr.consume_item_num;
		stuff_con.buyable = false;
		stuff_list.push_back(stuff_con);
	}

	ItemExtern::ItemConsumeList consume_list;
	if (!ItemExtern::GetConsumeListByOrder(m_role_module_mgr->GetRole(), false, (int)stuff_list.size(), &stuff_list[0], &consume_list, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -__LINE__;
	}

	if (consume_list.need_gold_num > 0 && !m_role_module_mgr->GetMoney()->GoldMoreThan(consume_list.need_gold_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return -__LINE__;
	}

	if (cfg->consume_coin > 0 && !m_role_module_mgr->GetMoney()->CoinBindMoreThan(cfg->consume_coin))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return false;
	}

	if (consume_list.count > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list,
												consume_list.index_list, consume_list.num_list, __FUNCTION__))
	{
		return -__LINE__;
	}

	if (consume_list.need_gold_num > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(consume_list.need_gold_num, __FUNCTION__))
		{
			return -__LINE__;
		}
	}

	if (cfg->consume_coin > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseCoinBind(cfg->consume_coin, __FUNCTION__))
		{
			return -__LINE__;
		}
	}

	++fabao_equip.fabao_jie;
	ROLE_LOG_QUICK6(LOG_TYPE_FABAO_UP, m_role_module_mgr->GetRole(), fabao_index, fabao_equip.fabao_jie, __FUNCTION__, "upjie");
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FABAO, __FUNCTION__);
	m_role_module_mgr->GetPet()->ReCalcAttrByFaBao();//法宝调用宠物属性修改
	this->SendSignalFaBaoInfo(fabao_index, Protocol::SCFabaoSignInfo::SEND_INFO_TYPE_ADVANCE);

	return 0;
}

bool FaBao::ActiveFaBaoByItem(int fabao_index)
{
	if (fabao_index < 0 || fabao_index >= MAX_FABAO_COUNT)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_NO_THIS_FABAO);
		return false;
	}

	if (IsActiveFaBao(fabao_index))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_FABAO_HAS_ACTIVE_FABAO);
		return false;
	}

	const FaBaoInfoCfg * info_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(fabao_index);
	if (info_cfg == NULL)
	{
		return false;
	}

	m_param.m_fabao_list[fabao_index].fabao_index = fabao_index;
	m_param.m_fabao_list[fabao_index].fabao_level = 1;
	m_param.m_fabao_list[fabao_index].fabao_type = info_cfg->fabao_type;
	m_param.m_fabao_list[fabao_index].fabao_jie = 0;
	m_param.m_fabao_list[fabao_index].cur_exp = 0;

	m_param.m_fabao_list[fabao_index].level_effect = info_cfg->effect;
	EventHandler::Instance().OnAddFaBao(m_role_module_mgr->GetRole(), fabao_index);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FABAO, __FUNCTION__);
	m_role_module_mgr->GetPet()->ReCalcAttrByFaBao();//法宝调用宠物属性修改
	this->SendSignalFaBaoInfo(fabao_index, Protocol::SCFabaoSignInfo::SEND_INFO_TYPE_ACTIVE);
	ROLE_LOG_QUICK6(LOG_TYPE_FABAO_ACTIVE, m_role_module_mgr->GetRole(), fabao_index, m_param.m_fabao_list[fabao_index].fabao_level, __FUNCTION__, NULL);
	return true;
}

int FaBao::GetFaBaoNumByColorAndLevel(int color, int level)
{
	int count = 0;
	for (int i = 0; i < MAX_FABAO_COUNT; ++i)
	{
		if (m_param.m_fabao_list[i].fabao_level <= 0 || m_param.m_fabao_list[i].fabao_level < level)continue;	//未解锁/未达标

		if (0 != color)
		{
			const FaBaoInfoCfg * info_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(i);
			if (info_cfg == NULL || color != info_cfg->color) continue;
		}
		count++;
	}
	return count;
}

void FaBao::FabaoGuideFight()
{
	if (NULL != m_role_module_mgr->GetRole()->GetMyTeam())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	int monster_group_id = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoOtehrCfg().guide_monster_group;
	BattleManagerLocal::GetInstance().StartMonsterFightReq(m_role_module_mgr->GetRole(), monster_group_id, BATTLE_MODE_NORMAL_MONSTER, 0);
}

void FaBao::GetFaBaoList(short * out_data_fabao_num, BattleFabao out_data_fabao_info[BATTLE_FABAO_NUM_MAX])
{
	*out_data_fabao_num = 0;
	for (int i = 0; i < MAX_FABAO_COUNT && *out_data_fabao_num < BATTLE_FABAO_NUM_MAX; i++)
	{
		if (!this->IsActiveFaBao(i)) continue;

		FaBaoEquip & fabao_info = m_param.m_fabao_list[i];
		int fa_bao_id = fabao_info.fabao_index;
		const FaBaoInfoCfg * cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(fa_bao_id);
		if (cfg == NULL)
		{
			continue;
		}

		const FaBaoJinJieItemCfg * jin_jie_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoJinJieItemCfg(fa_bao_id, fabao_info.fabao_jie);
		if (jin_jie_cfg == NULL)
		{
			continue;
		}

		double effect_value = jin_jie_cfg->advanced_addition;
		if (cfg->max_jinjie_level <= fabao_info.fabao_jie && cfg->max_advanced_addition != 0)
		{
			effect_value += cfg->max_advanced_addition;
		}

		double effect_value_2 = jin_jie_cfg->advanced_addition_2;
		if (cfg->max_jinjie_level <= fabao_info.fabao_jie && cfg->max_advanced_addition_2 != 0)
		{
			effect_value_2 += cfg->max_advanced_addition_2;
		}

		if ((FABAO_TYPE_BATTLE_PASSIVE == cfg->fabao_type || FABAO_TYPE_ACTIVE == cfg->fabao_type))
		{
			out_data_fabao_info[*out_data_fabao_num].fabao_id = fabao_info.fabao_index;
			out_data_fabao_info[*out_data_fabao_num].fabao_level = fabao_info.fabao_level;
			out_data_fabao_info[*out_data_fabao_num].fabao_skill_type = cfg->effect;
			out_data_fabao_info[*out_data_fabao_num].fabao_grade = fabao_info.fabao_jie;
			out_data_fabao_info[*out_data_fabao_num].fabao_effect_percent_num = static_cast<int>(effect_value);
			out_data_fabao_info[*out_data_fabao_num].fabao_effect_percent_num_2 = static_cast<int>(effect_value_2);
			++ * out_data_fabao_num;
		}
	}
}

int FaBao::GetFaBaoMaxLevel()
{
	int max_level = 0;
	for (int fabao_index = 0; fabao_index < MAX_FABAO_COUNT; ++fabao_index)
	{
		if (!this->IsActiveFaBao(fabao_index))
		{
			continue;
		}

		FaBaoEquip & equip_info = m_param.m_fabao_list[fabao_index];
		max_level = std::max(max_level, (int)equip_info.fabao_level);
	}

	return max_level;
}

bool FaBao::IsActiveFaBao(short fabao_index)
{
	if (fabao_index < 0 || fabao_index >= MAX_FABAO_COUNT)
	{
		return false;
	}
	return m_param.m_fabao_list[fabao_index].fabao_index != FABAO_INDEX_NO && m_param.m_fabao_list[fabao_index].fabao_level > 0;
}

void FaBao::SendFaBaoAllInfo()
{
	Protocol::SCFabaoInfo protocol;

	protocol.week_use_coin_times = m_param.week_use_coin_times;
	memcpy(protocol.m_fabao_equipment_bar, m_param.m_fabao_equipment_bar, sizeof(protocol.m_fabao_equipment_bar));
	memcpy(protocol.m_fabao_list, m_param.m_fabao_list, sizeof(protocol.m_fabao_list));

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char *)&protocol, sizeof(protocol));
}

void FaBao::SendSignalFaBaoInfo(int fabao_index, int reason_type)
{
	Protocol::SCFabaoSignInfo protocol;
	protocol.reason = reason_type;
	protocol.week_use_coin_times = m_param.week_use_coin_times;
	memcpy(protocol.m_fabao_equipment_bar, m_param.m_fabao_equipment_bar, sizeof(protocol.m_fabao_equipment_bar));
	memcpy(&protocol.m_fabao_info, &m_param.m_fabao_list[fabao_index], sizeof(protocol.m_fabao_info));
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char *)&protocol, sizeof(protocol));
}

void FaBao::SendEquipBarInfo()
{
	Protocol::SCFabaoEquipBarInfo protocol;
	memcpy(protocol.m_fabao_equipment_bar, m_param.m_fabao_equipment_bar, sizeof(protocol.m_fabao_equipment_bar));
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char *)&protocol, sizeof(protocol));
}



const FaBaoParam & FaBao::GetFaBaoParam()
{
	return m_param;
}

const FaBaoEquip * FaBao::GetFaBaoEquip(int fabao_index)
{
	if (fabao_index >= 0 && fabao_index < MAX_FABAO_COUNT)
	{
		return &m_param.m_fabao_list[fabao_index];
	}
	return NULL;
}

void FaBao::ReCalcAttrByRoleLevelUp()
{
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FABAO, __FUNCTION__);
}

void FaBao::GmActiveAllFabao()
{
	for (int i = 0; i < MAX_FABAO_COUNT; ++i)
	{
		if (this->IsActiveFaBao(i)) continue;

		const FaBaoInfoCfg * info_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(i);
		if (info_cfg == NULL || !info_cfg->is_open) continue;

		this->ActiveFaBao(i, true);
	}
}

void FaBao::OnLunHuiWangChuangChange(int seq)
{
	bool need_send = false;
	const FaBaoOtherCfg & other_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoOtehrCfg();
	for (std::map<int, int>::const_iterator iter = other_cfg.fa_bao_draw_active_index_to_seq.begin(); iter != other_cfg.fa_bao_draw_active_index_to_seq.end(); ++iter)
	{
		int luzi_index = iter->first;
		int need_level = iter->second;
		
		if (seq < need_level)
		{
			continue;
		}

		if (luzi_index < 0 || luzi_index >= ARRAY_LENGTH(m_draw_param.item_info))
		{
			continue;
		}

		if (m_draw_param.item_info[luzi_index].is_active)
		{
			continue;
		}

		need_send = true;
		m_draw_param.item_info[luzi_index].is_active = 1;
	}

	if (need_send)
	{
		this->SendFaBaoDrawInfo();
	}
}

int FaBao::FaBaoSetDrawMaterial(int index, int(&item_ids)[FA_BAO_DRAW_MATERIAL_MAX_NUM])
{
	if (index < 0 || index >= FA_BAO_DRAW_ITEM_MAX_NUM)
	{
		return -__LINE__;
	}

	FaBaoDrawItemInfo & info = m_draw_param.item_info[index];

	if (!info.is_active)
	{
		return -__LINE__;
	}

	if (info.end_time != 0)
	{
		return -__LINE__;
	}

	std::map<int, int> item_to_num;
	unsigned int time_consume = 0;
	for (int i = 0; i < ARRAY_LENGTH(item_ids); ++i)
	{
		int item_id = item_ids[i];
		const FaBaoDrawCfg * cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoDrawCfg(item_id);
		if (cfg == NULL)		
		{
			return -__LINE__;//包含了不是抽奖消耗物品，直接返回
		}
				
		item_to_num[item_id] += 1;
		time_consume += cfg->consume_time;		
	}
	
	for (std::map<int, int>::iterator iter = item_to_num.begin(); iter != item_to_num.end(); ++iter)
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(iter->first, iter->second))
		{
			return -__LINE__;
		}
	}

	for (std::map<int, int>::iterator iter = item_to_num.begin(); iter != item_to_num.end(); ++iter)
	{
		m_role_module_mgr->GetKnapsack()->ConsumeItem(iter->first, iter->second, __FUNCTION__);
	}

	info.ResetMaterial();
	info.end_time = (unsigned int)(EngineAdapter::Instance().Time() + time_consume);

	for (int i = 0; i < FA_BAO_DRAW_MATERIAL_MAX_NUM; ++i)
	{
		info.material_item_id[i] = item_ids[i];
		info.material_num += 1;
	}

	this->SendFaBaoDrawInfo();

	return 0;
}

void FaBao::FaBaoDrawAccelerate(int index,int times)
{
	if (index < 0 || index >= FA_BAO_DRAW_ITEM_MAX_NUM || times <= 0)
	{
		return;
	}

	FaBaoDrawItemInfo & info = m_draw_param.item_info[index];

	if (info.end_time == 0)
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();
	time_t remain_time = info.end_time - (now + info.speed_up_time);
	if (remain_time <= 0)
	{
		return;
	}

	const FaBaoOtherCfg & other_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoOtehrCfg();
	if (other_cfg.speed_up_time <= 0)
	{
		return;
	}

	int real_times = (int)std::min((remain_time + other_cfg.speed_up_time - 1) / other_cfg.speed_up_time, (time_t)times);
	if (real_times <= 0)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.speed_up_item,real_times, __FUNCTION__))
	{
		return;
	}

	info.speed_up_time += other_cfg.speed_up_time * real_times;

	this->SendFaBaoDrawInfo();
}

void FaBao::FaBaoDraw(int index)
{
	if (index < 0 || index >= FA_BAO_DRAW_ITEM_MAX_NUM)
	{
		return;
	}

	FaBaoDrawItemInfo & info = m_draw_param.item_info[index];
	if (info.end_time == 0)
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();
	if (now + info.speed_up_time < info.end_time)
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckEmptyGridNoLessThanInTemporary(FA_BAO_DRAW_MATERIAL_MAX_NUM)) //一个材料一个奖励
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TEMPORARY_KNAPSACK_NO_SPACE);
		return;
	}

	
	int r = RandomNum(FA_BAO_DRAW_MAX_RATE);
	
	int fa_bao_id = -1;
	std::vector<ItemConfigData> rewards;

	bool is_get_fa_bao_reward = false;			//一次抽奖只能获取一次法宝奖励
	int fabao_sui_pian_index = -1;
	for (int i = 0; i < info.material_num && i < ARRAY_LENGTH(info.material_item_id); ++i)//一个材料一个奖励
	{
		int item_id = info.material_item_id[i];
		const FaBaoDrawCfg * cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoDrawCfg(item_id);
		if (cfg == NULL)
		{
			return;
		}

		bool is_get_reward = false;
		if (!is_get_fa_bao_reward)
		{
			if (r < cfg->rate)
			{
				is_get_fa_bao_reward = true;
				is_get_reward = true;
				const FaBaoDrawRewardItemCfg * reward_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoDrawRewardCfg(item_id, FA_BAO_DRAW_REWARD_TYPE_FA_BAO);
				if (reward_cfg == NULL)
				{
					return;
				}


				gamelog::g_log_fa_bao.printf(LL_INFO, "%s line:%d |  role[%d %s] draw_get_fa_bao:%d", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), reward_cfg->reward_id);
				if (!this->IsActiveFaBao(reward_cfg->reward_id))
				{										
					fa_bao_id = reward_cfg->reward_id;
					this->ActiveFaBao(fa_bao_id, true);
				}
				else
				{
					fa_bao_id = reward_cfg->reward_id;
					const FaBaoInfoCfg * fabao_info = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoInfoCfg(reward_cfg->reward_id);
					if (fabao_info == NULL)
					{
						return;
					}

					fabao_sui_pian_index = i;
					rewards.push_back(ItemConfigData(fabao_info->active_item_id, true, cfg->transform_price_num));
				}
			}

			r -= cfg->rate;
		}

		if (is_get_reward)
		{
			continue;
		}

		const FaBaoDrawRewardItemCfg * reward_cfg = LOGIC_CONFIG->GetFaBaoConfig()->GetFaBaoDrawRewardCfg(item_id, FA_BAO_DRAW_REWARD_TYPE_ITEM);
		if (reward_cfg == NULL)
		{
			return;
		}

		rewards.push_back(ItemConfigData(reward_cfg->reward_id, true, reward_cfg->reward_num));
	}

	if (rewards.empty())
	{
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_FA_BAO_DRAW_REWARD))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	Protocol::SCFabaoDrawRewardInfo protocol;
	protocol.fa_bao_id = fa_bao_id;
	protocol.fa_bao_sui_pian_index = (short)fabao_sui_pian_index;
	memset(protocol.item_id, 0, sizeof(protocol.item_id));

	for (int i = 0; i < (int)rewards.size() && i < FA_BAO_DRAW_MATERIAL_MAX_NUM; ++i)
	{
		protocol.item_id[i] = rewards[i].item_id;
		protocol.item_num[i] = rewards[i].num;
	}
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));


	info.ResetMaterial();
	this->SendFaBaoDrawInfo();
}

void FaBao::SendFaBaoDrawInfo()
{
	Protocol::SCFabaoDrawInfo protocol;
	
	for (int i = 0;  i < FA_BAO_DRAW_ITEM_MAX_NUM; ++i)
	{
		protocol.item_info[i] = m_draw_param.item_info[i];
	}

	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
}

