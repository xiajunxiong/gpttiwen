#include "baby.hpp"
#include "engineadapter.h"
#include "world.h"
#include "other/baby/babyconfig.hpp"
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


Baby::Baby() : m_role_module_mgr(NULL)
{
}

Baby::~Baby()
{
}

void Baby::Init(RoleModuleManager * role_module_manager, const RoleBabyParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;
}

void Baby::GetOtherInitParam(RoleBabyParam * param)
{
	*param = m_param;
}

void Baby::OnResetData(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.draw_times = 0;
	m_param.worship_times = 0;

	if (m_param.create_type == BABY_CREATE_TYPE_DUET)
	{
		BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.creating_baby_id);
		if (baby != NULL)
		{
			if (m_param.last_reset_time == baby->baby_data_param.last_reset_time)
			{
				unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
				baby->baby_data_param.add_jifen_times = 0;
				baby->baby_data_param.last_reset_time = now;
				m_param.last_reset_time = 0;
				BabyManager::Instance().SetBabyDataDirty(baby);
			}
			else
			{
				m_param.last_reset_time = baby->baby_data_param.last_reset_time;
			}
		}
	}
}

const AttributeList & Baby::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();

		for (int i = 0; i < MAX_BABY_NUM; ++i)
		{
			if (m_param.baby_id_list[i] != 0)
			{
				const BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.baby_id_list[i]);
				if (baby != NULL)
				{
					const BabyLevelCfg * lvl_cfg = LOGIC_CONFIG->GetBabyConfig()->GetBabyLevelCfg(baby->baby_data_param.level);
					if (lvl_cfg == NULL)
					{
						continue;
					}

					m_attrs_add.m_attrs[BATTLE_ATTR_MAX_HP] += static_cast<Attribute>(lvl_cfg->maxhp);
					m_attrs_add.m_attrs[BATTLE_ATTR_MAX_MP] += static_cast<Attribute>(lvl_cfg->maxmp);
					m_attrs_add.m_attrs[BATTLE_ATTR_ATTACK] += static_cast<Attribute>(lvl_cfg->gongji);
					m_attrs_add.m_attrs[BATTLE_ATTR_DEFENSE] += static_cast<Attribute>(lvl_cfg->fangyu);
					m_attrs_add.m_attrs[BATTLE_ATTR_AGILITY] += static_cast<Attribute>(lvl_cfg->minjie);
					m_attrs_add.m_attrs[BATTLE_ATTR_MENTAL] += static_cast<Attribute>(lvl_cfg->jingshen);
					m_attrs_add.m_attrs[BATTLE_ATTR_RECOVERY] += static_cast<Attribute>(lvl_cfg->huifu);
				}
			}
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void Baby::OnRoleLogin()
{
	if (m_param.create_type == BABY_CREATE_TYPE_DUET) //检查双人生产，可能被对面终止的情况
	{
		const BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.creating_baby_id);

		if (baby == NULL || baby->baby_data_param.baby_id == 0)
		{
			this->OnStopDuetCreate();
		}
	}
}

void Baby::ActiveBabySingle(char * baby_name)
{
	if (this->GetBabyNum() >= MAX_BABY_NUM)
	{
		return;
	}

	if (m_param.create_type != BABY_CREATE_TYPE_SINGLE)
	{
		return;
	}

	const BabyOtherCfg & other_cfg = LOGIC_CONFIG->GetBabyConfig()->GetOtherCfg();

	if (m_param.single_ji_fen < other_cfg.single_need_ji_fen)
	{
		return;
	}

	m_param.single_ji_fen = 0;
	m_param.create_type = 0;

	long long baby_id = BabyManager::Instance().AddNewBaby(m_role_module_mgr->GetRole()->GetUserId().db_index, baby_name,m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName());

	this->AddBaby(baby_id);

	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_BABY, __FUNCTION__);

	this->SendInfo();
}

void Baby::ActiveBabyDuet(char * baby_name)
{
	if (m_role_module_mgr->GetRole()->GetSex() != FEMALE)
	{
		return;
	}

	if (!m_role_module_mgr->GetMarriage()->IsOnlyLoverInTeam())
	{
		return;
	}

	Role * lover = m_role_module_mgr->GetMarriage()->GetLover();
	if (lover == NULL)
	{
		return;
	}

	if (lover->GetRoleModuleManager()->GetBaby()->GetBabyNum() >= MAX_BABY_NUM || this->GetBabyNum() >= MAX_BABY_NUM)
	{
		return;
	}

	BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.creating_baby_id);
	if (baby == NULL)
	{
		return;
	}

	if (baby->baby_data_param.create_stage != CREATE_BABY_STAGE_ATIVTE)
	{
		return;
	}

	if (m_param.create_type != BABY_CREATE_TYPE_DUET)
	{
		return;
	}

	const BabyOtherCfg & other_cfg = LOGIC_CONFIG->GetBabyConfig()->GetOtherCfg();
	if (!m_role_module_mgr->GetMoney()->UseCoinBind(other_cfg.stage_condition[CREATE_BABY_STAGE_ATIVTE], __FUNCTION__))
	{
		return;
	}

	baby->baby_data_param.level = 1;
	STRNCPY(baby->baby_data_param.baby_name, baby_name, sizeof(baby->baby_data_param.baby_name));
	BabyManager::Instance().SetBabyDataDirty(baby);

	long long add_baby_id = m_param.creating_baby_id;
	this->AddBaby(add_baby_id);
	lover->GetRoleModuleManager()->GetBaby()->AddBaby(add_baby_id);

	m_param.create_type = 0;
	m_param.creating_baby_id = 0;

	RoleBabyParam & lover_param = lover->GetRoleModuleManager()->GetBaby()->GetBabyParam();
	
	lover_param.create_type = 0;
	lover_param.creating_baby_id = 0;

	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_BABY, __FUNCTION__);
	lover->GetRoleModuleManager()->ReCalcAttr(AttributeList::RECALC_REASON_BABY, __FUNCTION__);
	
	this->SendInfo();
	lover->GetRoleModuleManager()->GetBaby()->SendInfo();

}

void Baby::DuetCreateRegStage1()
{
	if (!m_role_module_mgr->GetMarriage()->IsOnlyLoverInTeam())
	{
		return;
	}

	if (m_param.create_type != BABY_CREATE_TYPE_DUET)
	{
		return;
	}

	BabyDataNode * creating_baby = BabyManager::Instance().GetBabyData(m_param.creating_baby_id);	//获取正在生产的baby信息
	if (creating_baby == NULL)
	{
		return;
	}

	if (creating_baby->baby_data_param.create_stage != CREATE_BABY_STAGE_HUAI_YUN)
	{
		return;
	}

	const BabyOtherCfg & other_cfg = LOGIC_CONFIG->GetBabyConfig()->GetOtherCfg();
	if (!m_role_module_mgr->GetMoney()->UseCoinBind(other_cfg.stage_condition[CREATE_BABY_STAGE_HUAI_YUN], __FUNCTION__))
	{
		return;
	}

	creating_baby->baby_data_param.create_stage += 1;
	creating_baby->baby_data_param.duet_create_ji_fen = 0;
	BabyManager::Instance().SetBabyDataDirty(creating_baby);

	this->SendInfo();
	Role * lover = m_role_module_mgr->GetMarriage()->GetLover();
	if (lover != NULL)
	{
		lover->GetRoleModuleManager()->GetBaby()->SendInfo();
	}


	return;
}

void Baby::RemoveBaby(int baby_index)
{
	if (baby_index < 0 || baby_index >= MAX_BABY_NUM)
	{
		return;
	}

	BabyManager::Instance().RemoveBaby(m_param.baby_id_list[baby_index], m_role_module_mgr->GetRole()->GetUID());

	m_param.baby_id_list[baby_index] = 0;

	this->SendInfo();
}

void Baby::SendInfo()
{
	Protocol::SCBabyInfo protocol;
	protocol.draw_times = m_param.draw_times;
	protocol.worship_times = m_param.worship_times;
	protocol.single_ji_fen = m_param.single_ji_fen;
	protocol.duet_ji_fen = 0;
	protocol.duet_create_stage = 0;
	protocol.add_jifen_times = 0;
	protocol.create_type = m_param.create_type;
	
	const BabyDataNode * creating_baby = BabyManager::Instance().GetBabyData(m_param.creating_baby_id);	//获取正在生产的baby信息
	if (creating_baby != NULL)
	{
		protocol.duet_ji_fen = creating_baby->baby_data_param.duet_create_ji_fen;
		protocol.duet_create_stage = creating_baby->baby_data_param.create_stage;
		protocol.add_jifen_times = creating_baby->baby_data_param.add_jifen_times;
	}

		
	for (int i = 0; i < MAX_BABY_NUM; ++i)
	{
		protocol.baby[i].baby_id = 0;
		const BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.baby_id_list[i]); //获取已经生产的baby信息
		if (baby != NULL)
		{
			protocol.baby[i] = baby->baby_data_param;
		}		
	}


	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&protocol, sizeof(protocol));
}

void Baby::AddPlayJiFen(int add_ji_fen)
{
	if (m_param.creating_baby_id == 0)
	{
		return;
	}

	if (add_ji_fen <= 0)
	{
		return;
	}

	BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.creating_baby_id);
	if (baby == NULL)
	{
		return;
	}

	if (baby->baby_data_param.level != 0 || (baby->baby_data_param.create_stage != CREATE_BABY_STAGE_BEI_YUN && baby->baby_data_param.create_stage != CREATE_BABY_STAGE_TAI_JAIO))
	{
		return;
	}

	baby->baby_data_param.duet_create_ji_fen += add_ji_fen;
	baby->baby_data_param.add_jifen_times += 1;

	const BabyOtherCfg & other_cfg = LOGIC_CONFIG->GetBabyConfig()->GetOtherCfg();

	if (baby->baby_data_param.create_stage == CREATE_BABY_STAGE_BEI_YUN)
	{
		if (baby->baby_data_param.duet_create_ji_fen >= other_cfg.stage_condition[CREATE_BABY_STAGE_BEI_YUN])
		{
			baby->baby_data_param.duet_create_ji_fen = 0;
			baby->baby_data_param.create_stage += 1;
		}
	}
	else if (baby->baby_data_param.create_stage == CREATE_BABY_STAGE_TAI_JAIO)
	{
		if (baby->baby_data_param.duet_create_ji_fen >= other_cfg.stage_condition[CREATE_BABY_STAGE_TAI_JAIO])
		{
			baby->baby_data_param.duet_create_ji_fen = 0;
			baby->baby_data_param.create_stage += 1;
		}
	}

	this->SendInfo();

	BabyManager::Instance().SetBabyDataDirty(baby);
}

void Baby::UpLevel(int index)
{
	if (index < 0 || index >= MAX_BABY_NUM)
	{
		return;
	}

	if (m_param.baby_id_list[index] == 0)
	{
		return;
	}

	BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.baby_id_list[index]);
	if (baby == NULL)
	{
		return;
	}
	unsigned short & baby_lvl = baby->baby_data_param.level;
	if (baby_lvl >= LOGIC_CONFIG->GetBabyConfig()->GetBabyMaxLevel())
	{
		return;
	}
	const BabyLevelCfg * lvl_cfg = LOGIC_CONFIG->GetBabyConfig()->GetBabyLevelCfg(baby->baby_data_param.level);
	if (lvl_cfg == NULL)
	{
		return;
	}

	const BabyOtherCfg & other_cfg = LOGIC_CONFIG->GetBabyConfig()->GetOtherCfg();
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.uplvl_item_id, 1, __FUNCTION__))
	{
		return;
	}

	bool is_up_lvl = false;
	baby->baby_data_param.exp += other_cfg.add_exp;
	if (baby->baby_data_param.exp >= lvl_cfg->exp)
	{
		baby->baby_data_param.exp -= lvl_cfg->exp;
		baby_lvl += 1;
		is_up_lvl = true;
	}

	if (is_up_lvl)
	{
		m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_BABY, __FUNCTION__);
	}

	this->SendInfo();
	BabyManager::Instance().SetBabyDataDirty(baby);
}

void Baby::ReName(int index, char * new_name)
{
	if (index < 0 || index >= MAX_BABY_NUM)
	{
		return;
	}

	if (m_param.baby_id_list[index] == 0)
	{
		return;
	}

	BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.baby_id_list[index]);
	if (baby == NULL)
	{
		return;
	}
	
	const BabyOtherCfg & other_cfg = LOGIC_CONFIG->GetBabyConfig()->GetOtherCfg();
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.rename_item_id, 1, __FUNCTION__))
	{
		return;
	}

	STRNCPY(baby->baby_data_param.baby_name, new_name, sizeof(baby->baby_data_param.baby_name));
	
	this->SendInfo();

	BabyManager::Instance().SetBabyDataDirty(baby);
}

void Baby::Draw()
{
	if (m_param.create_type != 1)
	{
		return;
	}

	if (m_param.draw_times != 0)
	{
		return;
	}

	BabyDrawCfg::ItemCfg * item_cfg = LOGIC_CONFIG->GetBabyConfig()->GetRandDrawItem();
	if (item_cfg==NULL)
	{
		return;
	}
	m_param.draw_times += 1;
	m_param.single_ji_fen += item_cfg->add_ji_fen;

	this->SendInfo();
}

void Baby::Contribution(int type)
{
	if (m_param.create_type != 1)
	{
		return;
	}

	if (m_param.worship_times != 0)
	{
		return;
	}

	const BabyOtherCfg & other_cfg = LOGIC_CONFIG->GetBabyConfig()->GetOtherCfg();

	switch (type)
	{
		case 0:
			{
				if (!m_role_module_mgr->GetMoney()->UseCoinBind(other_cfg.contribute_coin_num, __FUNCTION__))
				{
					return;
				}
				m_param.single_ji_fen += other_cfg.contribute_coin_jifen;				
			}
			break;
		case 1:
			{
				if (!m_role_module_mgr->GetMoney()->UseImmortalCoin(other_cfg.contribute_xianyu_num, __FUNCTION__))
				{
					return;
				}
				m_param.single_ji_fen += other_cfg.contribute_xianyu_jifen;
			}
			break;
		case 2:
			{
				if (!m_role_module_mgr->GetMoney()->UseCoinBind(other_cfg.contribute_gold_num, __FUNCTION__))
				{
					return;
				}
				m_param.single_ji_fen += other_cfg.contribute_gold_jifen;
			}
			break;
		default:
			return;
	}

	m_param.worship_times += 1;
	this->SendInfo();	
}

void Baby::GiveUpCurrCreateType()
{
	if (m_param.create_type == 1)
	{
		m_param.single_ji_fen = 0;
		m_param.create_type = 0;
	}
	else if (m_param.create_type == 2)
	{
		this->GiveUpDuetCreate();
	}

	
	this->SendInfo();
}

void Baby::SelectCreateType(int create_type)
{
	if (create_type <= BABY_CREATE_TYPE_INVAILD || create_type > BABY_CREATE_TYPE_DUET)
	{
		return;
	}

	if (m_param.create_type != BABY_CREATE_TYPE_INVAILD)
	{
		return;
	}

	if (this->IsMaxBaby())
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_BABY_NUM_MAX);
		return;
	}

	const BabyOtherCfg & other_cfg = LOGIC_CONFIG->GetBabyConfig()->GetOtherCfg();

	if (m_role_module_mgr->GetRole()->GetLevel() < other_cfg.create_need_role_level)
	{
		return;
	}

	if (create_type == BABY_CREATE_TYPE_SINGLE)
	{
		m_param.create_type = create_type;
	}
	else if (create_type == BABY_CREATE_TYPE_DUET)
	{
		Role * lover = m_role_module_mgr->GetMarriage()->GetLover();
		if (lover == NULL)
		{
			return;
		}

		if (lover->GetRoleModuleManager()->GetBaby()->IsMaxBaby())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_LOVER_BABY_CREATING);
			return;
		}

		if (lover->GetLevel() < other_cfg.create_need_role_level)
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_LOVER_LEVEL_NOT_ENOUG);
			return;
		}

		RoleBabyParam & lover_param = lover->GetRoleModuleManager()->GetBaby()->GetBabyParam();
		if (lover_param.create_type != BABY_CREATE_TYPE_INVAILD)
		{
			return;
		}

		long long baby_id = 0;
		if (m_role_module_mgr->GetRole()->GetSex() == FEMALE)
		{
			baby_id = BabyManager::Instance().AddNewBaby(lover->GetUserId().db_index, NULL, lover->GetUID(), lover->GetName(), lover->GetUID(), lover->GetName(), true);
		}
		else
		{
			baby_id = BabyManager::Instance().AddNewBaby(m_role_module_mgr->GetRole()->GetUserId().db_index, NULL, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), lover->GetUID(), lover->GetName(), true);
		}

		m_param.create_type = create_type;
		m_param.creating_baby_id = baby_id;

		lover_param.create_type = create_type;
		lover_param.creating_baby_id = baby_id;

		lover->GetRoleModuleManager()->GetBaby()->SendInfo();
	}
	
	this->SendInfo();
}


void Baby::OnDivorce()
{
	if (m_param.create_type == BABY_CREATE_TYPE_DUET)
	{
		this->OnStopDuetCreate();
		//TODP:gfp 发邮件。终止生产
	}

	this->SendInfo();
}

int Baby::GetBabyNum()
{
	int sum = 0;

	for (int i = 0; i < MAX_BABY_NUM; ++i)
	{
		if (m_param.baby_id_list[i] != 0)
		{
			sum += 1;
		}
	}

	return sum;
}

void Baby::AddBaby(long long baby_uuid)
{
	int free_baby_index = 0;
	if (!this->GetFreeBabyItemIndex(free_baby_index))
	{
		return;
	}

	if (m_param.creating_baby_id == baby_uuid)	//如果增加babyid等于正在双人创建，顺便清0
	{
		m_param.creating_baby_id = 0;
	}


	m_param.baby_id_list[free_baby_index] = baby_uuid;
}

bool Baby::IsMaxBaby()
{
	if (this->GetBabyNum() >= MAX_BABY_NUM)
	{
		return true;
	}

	return false;
}

bool Baby::CanStartPlay()
{
	if (m_param.create_type != BABY_CREATE_TYPE_DUET)
	{
		return false;
	}

	const BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.creating_baby_id);

	if (baby == NULL)
	{
		return false;
	}

	if (baby->baby_data_param.add_jifen_times != 0)
	{
		return false;
	}

	return true;
}

void Baby::OnStopDuetCreate()
{
	m_param.creating_baby_id = 0;
	m_param.create_type = 0;

	this->SendInfo();
}

void Baby::GmOper(int type, int p1, int p2, int p3)
{
	switch (type)
	{
		case 0:
			{
				if (m_param.create_type == BABY_CREATE_TYPE_SINGLE)
				{
					m_param.single_ji_fen += p1;
				}
			}

		default:
			break;
	}

	this->SendInfo();
}

bool Baby::GetFreeBabyItemIndex(int & out_index)
{
	for (int i = 0; i < MAX_BABY_NUM; ++i)
	{
		if (m_param.baby_id_list[i] == 0)
		{
			out_index = i;
			return true;
		}
	}

	return false;
}

void Baby::GiveUpDuetCreate()
{
	BabyDataNode * baby = BabyManager::Instance().GetBabyData(m_param.creating_baby_id);
	if (baby != NULL)
	{
		baby->baby_data_param.baby_id = 0;
		BabyManager::Instance().SetBabyDataDirty(baby);
	}

	this->OnStopDuetCreate();
	
	Role * lover = m_role_module_mgr->GetMarriage()->GetLover();
	if (lover != NULL)
	{
		lover->GetRoleModuleManager()->GetBaby()->OnStopDuetCreate();
	}
}

RoleBabyParam & Baby::GetBabyParam()
{
	return m_param;
}

