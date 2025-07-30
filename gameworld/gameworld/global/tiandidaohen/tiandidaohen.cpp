#include "servercommon/serverconfig/crossconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "engineadapter.h"
#include "world.h"
#include "internalcomm.h"
#include "global/worldstatus/worldstatus.hpp"


#include "global/tiandidaohen/tiandidaohenconfig.hpp"
#include "protocol/msgtiandidaohen.h"
#include "tiandidaohen.h"

TianDiDaoHen & TianDiDaoHen::Instance()
{
	static  TianDiDaoHen r;
	return r;
}

void TianDiDaoHen::OnUserLogin(Role * role)
{
	this->SendTianDiDaoHenOpenInfo(role);
}

void TianDiDaoHen::OnUserLoginOriginGame(int role_id)
{
	this->SendTianDiDaoHenOpenInfo(NULL, role_id);
}

void TianDiDaoHen::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	const TianDiDaoHenOtherCfg & other_cfg = LOGIC_CONFIG->GetTianDiDaoHenConfig()->GetOtherCfg();
	WorldStatusCommonDataParam & common_data = WorldStatus::Instance().GetCommonData();
	if (common_data.tian_di_dao_hen_next_state_time == 0)
	{

		int open_day = World::GetInstWorld()->GetCurRealOpenDay();
		if (open_day < other_cfg.begin_day)
		{
			return;
		}
	}

	if (now_second >= common_data.tian_di_dao_hen_next_state_time)
	{
		if (common_data.tian_di_dao_hen_open_state == 0)
		{
			common_data.tian_di_dao_hen_next_state_time = (unsigned int)::GetZeroTime(now_second) + other_cfg.open_during_day * SECOND_PER_DAY;
			common_data.tian_di_dao_hen_open_state = 1;
		}
		else
		{
			common_data.tian_di_dao_hen_next_state_time = (unsigned int) ::GetZeroTime(now_second) + other_cfg.close_during_day * SECOND_PER_DAY;
			common_data.tian_di_dao_hen_open_state = 0;
		}

		this->SyncInfoToGame();
		this->SendTianDiDaoHenOpenInfo();		
	}
}

void TianDiDaoHen::OnGameServerConnect(std::set<int> server_id)
{
	if (server_id.empty())
	{
		return;
	}


	this->SyncInfoToGame(*server_id.begin());
}

bool TianDiDaoHen::IsOpen() const
{
	WorldStatusCommonDataParam & common_data = WorldStatus::Instance().GetCommonData();
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return common_data.tian_di_dao_hen_open_state != 0;
	}	
	else
	{
		return m_is_set_info_from_hidden && common_data.tian_di_dao_hen_open_state != 0;
	}
}

void TianDiDaoHen::SendTianDiDaoHenOpenInfo(Role * role, int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	WorldStatusCommonDataParam & common_data = WorldStatus::Instance().GetCommonData();
	Protocol::SCTianDiDaoHenOpenInfo protocol;
	protocol.is_open = common_data.tian_di_dao_hen_open_state;
	protocol.next_state_time = common_data.tian_di_dao_hen_next_state_time;

	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}

	if (role_id)
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
	}

	if (role == NULL && role_id == 0)
	{
		World::GetInstWorld()->SendToALLGameAllGateway(&protocol, sizeof(protocol));
	}
}

void TianDiDaoHen::SetInfo(int state, long long next_state_time)
{
	WorldStatusCommonDataParam & common_data = WorldStatus::Instance().GetCommonData();
	m_is_set_info_from_hidden = true;
	common_data.tian_di_dao_hen_open_state=state;
	common_data.tian_di_dao_hen_next_state_time = (unsigned int)next_state_time;
	//this->SendTianDiDaoHenOpenInfo();   
}

void TianDiDaoHen::SyncInfoToGame(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	WorldStatusCommonDataParam & common_data = WorldStatus::Instance().GetCommonData();
	crossgameprotocal::HiddenGameTianDiDaoHenSyncInfo  info;
	info.state = common_data.tian_di_dao_hen_open_state;
	info.next_state_time = common_data.tian_di_dao_hen_next_state_time;

	InternalComm::Instance().SendToGameThroughCross2(&info, sizeof(info), server_id);
}

void TianDiDaoHen::GmNextState()
{
	WorldStatusCommonDataParam & common_data = WorldStatus::Instance().GetCommonData();
	common_data.tian_di_dao_hen_next_state_time = (unsigned int)EngineAdapter::Instance().Time();
}

TianDiDaoHen::TianDiDaoHen()
{
	m_is_set_info_from_hidden = false;
}

TianDiDaoHen::~TianDiDaoHen()
{
}
