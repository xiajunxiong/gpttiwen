#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "global/tiandidaohen/tiandidaohen.h"
#include "global/cross/crossmanager.h"
#include "other/funopen/funopen.hpp"

#include "protocol/msgtiandidaohen.h"
#include "global/tiandidaohen/tiandidaohenconfig.hpp"
#include "roletiandidaohen.hpp"

int RoleTianDiDaoHen::ActiveItem(int item_index)
{
	const TianDiDaoHenCollectionSeqCfg * cfg = LOGIC_CONFIG->GetTianDiDaoHenConfig()->GetTianDiDaoHenCollectionSeqCfg(m_param.day_seq);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	if (item_index < 0 || item_index >= (int)cfg->item_cfg.size())
	{
		return -__LINE__;
	}

	if (m_param.active_flag & (1 << item_index))
	{
		return -__LINE__;
	}

	const TianDiDaoHenCollectionSeqCfg::ItemCfg & item_cfg = cfg->item_cfg[item_index];

	const TianDiDaoHenRewardGroupCfg::ItemCfg * reward_cfg = LOGIC_CONFIG->GetTianDiDaoHenConfig()->GetRandReward(item_cfg.reward_group_id);
	if (reward_cfg == NULL)
	{
		return -__LINE__;
	}

	if (!m_module_mgr->GetKnapsack()->CheckForPutList2(reward_cfg->rewards))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -__LINE__;
	}

	//ÏûºÄ
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(item_cfg.need_item_id, item_cfg.need_item_num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return -__LINE__;
	}

	//¸øÓè
	m_module_mgr->GetKnapsack()->PutVec(reward_cfg->rewards, PUT_REASON_TIAN_DI_DAO_HEN_ACTIVE);
	m_param.active_flag |= (1 << item_index);
	m_module_mgr->GetKnapsack()->NotifyGetItemListVec(reward_cfg->rewards, GET_REASON_TIAN_TI_DAO_HEN);

	this->SendInfo();

	return 0;
}

int RoleTianDiDaoHen::FetchFinalReward()
{
	if (m_param.is_fetch_reward)
	{
		return -__LINE__;
	}

	const TianDiDaoHenCollectionSeqCfg * cfg = LOGIC_CONFIG->GetTianDiDaoHenConfig()->GetTianDiDaoHenCollectionSeqCfg(m_param.day_seq);
	if (cfg == NULL)
	{
		return -__LINE__;
	}

	std::vector<ItemConfigData> rewards;
	const TianDiDaoHenRewardGroupCfg::ItemCfg * reward_cfg = LOGIC_CONFIG->GetTianDiDaoHenConfig()->GetRandReward(cfg->final_reward_group_id);
	if (reward_cfg == NULL)
	{
		return -__LINE__;
	}

	const TianDiDaoHenRewardGroupCfg::ItemCfg * reward_cfg_2 = LOGIC_CONFIG->GetTianDiDaoHenConfig()->GetRandReward(cfg->final_reward_group_id_2);
	if (reward_cfg_2 == NULL)
	{
		return -__LINE__;
	}

	rewards.insert(rewards.end(), reward_cfg->rewards.begin(), reward_cfg->rewards.end());
	rewards.insert(rewards.end(), reward_cfg_2->rewards.begin(), reward_cfg_2->rewards.end());

	//¸øÓè
	if (!m_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_TIAN_DI_DAO_HEN_FINAL))
	{
		return -__LINE__;
	}
	m_module_mgr->GetKnapsack()->NotifyGetItemListVec(rewards, GET_REASON_TIAN_TI_DAO_HEN);


	m_param.is_fetch_reward = 1;
	this->SendInfo();

	return 0;
}

void RoleTianDiDaoHen::SendInfo()
{
	Protocol::SCTianDiDaoHenRoleInfo protocol;
	protocol.seq = m_param.day_seq;
	protocol.active_flag = m_param.active_flag;
	protocol.is_fetch_reward = m_param.is_fetch_reward;
	m_module_mgr->NetSend(&protocol, sizeof(protocol));
}

RoleTianDiDaoHen::RoleTianDiDaoHen()
{
	m_module_mgr = NULL;
}

RoleTianDiDaoHen::~RoleTianDiDaoHen()
{
}

void RoleTianDiDaoHen::Init(RoleModuleManager * mgr, const RoleTianDiDaoHenParam & param)
{
	m_module_mgr = mgr;
	m_param = param;
}

void RoleTianDiDaoHen::GetInitParam(RoleTianDiDaoHenParam * param)
{
	*param = m_param;
}

void RoleTianDiDaoHen::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.day_seq = LOGIC_CONFIG->GetTianDiDaoHenConfig()->GetRandSeq();
	m_param.active_flag = 0;
	m_param.is_fetch_reward = 0;

	this->SendInfo();
}

void RoleTianDiDaoHen::RandDropItem()
{
	if (!TianDiDaoHen::Instance().IsOpen())
	{
		return;
	}

	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_TIAN_DI_DAO_HEN))
	{
		return;
	}

	ItemConfigData * item = LOGIC_CONFIG->GetTianDiDaoHenConfig()->GetRandDropItem();
	if (item)
	{
		m_module_mgr->GetKnapsack()->Put(*item, PUT_REASON_TIAN_DI_DAO_HEN_DROP);
	}
}

void RoleTianDiDaoHen::UseCrystalEnergy(int num)
{
	CommonDataParam & data = m_module_mgr->GetCommonData();
	data.use_crystal_energy_num += num;

	while (data.use_crystal_energy_num >= 10)
	{
		data.use_crystal_energy_num -= 10;
		this->RandDropItem();
	}	
}
