#include "auto_catch_pet.hpp"
#include "servercommon/servercommon.h"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "other/pet/petconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "gamelog.h"

AutoCatchPet::AutoCatchPet() : m_mgr(NULL)
{

}

AutoCatchPet::~AutoCatchPet()
{

}

void AutoCatchPet::Init(RoleModuleManager* mgr, const AutoCatchPetParam& param)
{
	m_mgr = mgr;
	m_param = param;
}

void AutoCatchPet::GetInitParam(ARG_OUT AutoCatchPetParam* out_param)
{
	if (NULL == out_param) return;

	*out_param = m_param;
}

void AutoCatchPet::SetAutoCatchPetInfo(Protocol::CSSetAutoCatchPet* req)
{
	m_param.scene_id = req->scene_id;
	m_param.max_seal_card_common_num = req->max_seal_card_common;
	m_param.max_seal_card_silver_num = req->max_seal_card_silver;
	m_param.max_seal_card_gold_num = req->max_seal_card_gold;

	m_param.cur_seal_card_common_num = 0;
	m_param.cur_seal_card_silver_num = 0;
	m_param.cur_seal_card_gold_num = 0;

	// 清空之前的信息
	for (int i = 0; i < MAX_AUTO_CATCH_NUM; ++i)
	{
		m_param.auto_catch_list[i].Reset();
	}

	for (int i = 0; i < req->auto_catch_info_num && i < MAX_AUTO_CATCH_NUM; ++i)
	{
		m_param.auto_catch_list[i].monster_id = req->setting_list[i].monster_id;
		m_param.auto_catch_list[i].max_catch_num = req->setting_list[i].max_catch_num;
		m_param.auto_catch_list[i].cur_catch_num = 0;
	}

	this->SendAutoCatchInfo();

	gamelog::g_log_auto_catch_pet.buff_printf("SetAutoCatchPetInfo role[%d,%s] scene_id[%d] max_common_card[%d] max_silver_card[%d] max_gold_card[%d]\n",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), m_param.scene_id, 
		m_param.max_seal_card_common_num, m_param.max_seal_card_silver_num, m_param.max_seal_card_gold_num);

	for (int i = 0; i < req->auto_catch_info_num && i < MAX_AUTO_CATCH_NUM; ++i)
	{
		gamelog::g_log_auto_catch_pet.buff_printf("catch_monster_id[%d] catch_num[%d]\n",
			m_param.auto_catch_list[i].monster_id, m_param.auto_catch_list[i].max_catch_num);
	}
	gamelog::g_log_auto_catch_pet.commit_buff(LL_INFO);
}

void AutoCatchPet::SendAutoCatchInfo()
{
	Protocol::SCAutoCatchPetInfo info;
	info.scene_id = m_param.scene_id;
	info.max_seal_card_common = m_param.max_seal_card_common_num;
	info.max_seal_card_silver = m_param.max_seal_card_silver_num;
	info.max_seal_card_gold = m_param.max_seal_card_gold_num;

	info.cur_seal_card_common = m_param.cur_seal_card_common_num;
	info.cur_seal_card_silver = m_param.cur_seal_card_silver_num;
	info.cur_seal_card_gold = m_param.cur_seal_card_gold_num;

	info.auto_catch_info_num = 0;
	for (int i = 0; i < MAX_AUTO_CATCH_NUM; ++i)
	{
		if (!m_param.auto_catch_list[i].IsValid())
		{
			break;
		}

		info.info_list[info.auto_catch_info_num].monster_id = m_param.auto_catch_list[i].monster_id;
		info.info_list[info.auto_catch_info_num].cur_catch_num = m_param.auto_catch_list[i].cur_catch_num;
		info.info_list[info.auto_catch_info_num].max_catch_num = m_param.auto_catch_list[i].max_catch_num;

		info.auto_catch_info_num += 1;
	}

	int len = sizeof(info) - sizeof(info.info_list) + info.auto_catch_info_num * sizeof(info.info_list[0]);
	m_mgr->NetSend(&info, len);
}

void AutoCatchPet::OnAutoCatchPet(int monster_id, ItemID seal_card_item_id, bool is_succ)
{
	for (int i = 0; i < MAX_AUTO_CATCH_NUM; ++i)
	{
		if (!m_param.auto_catch_list[i].IsValid()) continue;

		if (monster_id == m_param.auto_catch_list[i].monster_id)
		{
			if (is_succ)
			{
				m_param.auto_catch_list[i].cur_catch_num += 1;

				gamelog::g_log_auto_catch_pet.printf(LL_INFO, "OnAutoCatchPet succ, role[%d,%s] monster_id[%d] cur_num[%d] max_num[%d]",
					m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), monster_id, 
					m_param.auto_catch_list[i].cur_catch_num,
					m_param.auto_catch_list[i].max_catch_num);
			}

			break;
		}
	}

	const PetOtherCfg* cfg = LOGIC_CONFIG->GetPetConfig()->GetOtherCfg();
	if (NULL == cfg) return;

	if (seal_card_item_id == cfg->common_sealcard)
	{
		m_param.cur_seal_card_common_num += 1;

		gamelog::g_log_auto_catch_pet.printf(LL_INFO, "OnAutoCatchPet USE_COMMON_CARD role[%d,%s] monster_id[%d] card_item[%d] is_succ[%s] cur_common_card[%d] max_common_card[%d]",
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(),
			monster_id, seal_card_item_id, is_succ ? "True" : "False",
			m_param.cur_seal_card_common_num, m_param.max_seal_card_common_num);
	}

	if (seal_card_item_id == cfg->silver_sealcard)
	{
		m_param.cur_seal_card_silver_num += 1;

		gamelog::g_log_auto_catch_pet.printf(LL_INFO, "OnAutoCatchPet USE_SILVER_CARD role[%d,%s] monster_id[%d] card_item[%d] is_succ[%s] cur_silver_card[%d] max_silver_card[%d]",
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(),
			monster_id, seal_card_item_id, is_succ ? "True" : "False",
			m_param.cur_seal_card_silver_num, m_param.max_seal_card_silver_num);
	}

	if (seal_card_item_id == cfg->gold_sealcard)
	{
		m_param.cur_seal_card_gold_num += 1;

		gamelog::g_log_auto_catch_pet.printf(LL_INFO, "OnAutoCatchPet USE_GOLD_CARD role[%d,%s] monster_id[%d] card_item[%d] is_succ[%s] cur_gold_card[%d] max_gold_card[%d]",
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(),
			monster_id, seal_card_item_id, is_succ ? "True" : "False",
			m_param.cur_seal_card_gold_num, m_param.max_seal_card_gold_num);
	}

	this->SendAutoCatchInfo();
}

