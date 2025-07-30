#include "bigdippermanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/activitybigdipper/activitybigdipperconfig.hpp"
#include "protocol/msgother.h"
#include "gameworld/world.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gameworld/internalcomm.h"
#include "battle/battle_manager_local.hpp"

BigDipperManager::BigDipperManager()
{
	
}

BigDipperManager::~BigDipperManager()
{

}

BigDipperManager & BigDipperManager::Instance()
{
	static BigDipperManager umm;
	return umm;
}

void BigDipperManager::Init(const BigDipperMonsterParam & param)
{
	data = param;
}

void BigDipperManager::GetInitParam(BigDipperMonsterParam * param)
{
	*param = data;
}

void BigDipperManager::Update(unsigned long interval, time_t now_second)
{
	if (data.week_refresh_tamp <= 0)
	{
		//首次启动
		this->OnWeekChange();
		data.week_refresh_tamp = now_second;
	}
}

void BigDipperManager::OnHideInfoBack(BigDipperMonsterParam * param)
{
	data = *param;
}

void BigDipperManager::OnUserLogin(Role * role)
{
	this->SendInfo(role);
}

void BigDipperManager::OnUserLogout(Role * role)
{
	
}

void BigDipperManager::OnWeekChange()
{
	BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_FALLEN_GOD);//殒神是北斗的拓展玩法，刷新时间一致，刷新时间还在战斗的玩家全员退出战斗
	if (!CrossConfig::Instance().IsHiddenServer())return;//只允许跨服刷新
	data.Reset();
	const BigDipperOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityBigDipperConfig()->GetOtherCfg();
	if (other_cfg.open_num > 0)
	{
		std::vector<BigDipperCfg> monster_list;
		LOGIC_CONFIG->GetActivityBigDipperConfig()->GetRandMonsterCfg(other_cfg.open_num, &monster_list);

		for (std::vector<BigDipperCfg>::iterator it = monster_list.begin(); data.open_monster_num < MAX_BIG_DIPPER_MONSTER_NUM &&
			it != monster_list.end(); it++)
		{
			data.monster_list[data.open_monster_num++] = it->sn;
		}
	}
	crossgameprotocal::HiddenGameBigDipperSynRet big_dipper_ret;
	big_dipper_ret.info = data;
	InternalComm::Instance().SendToAllGameThroughCross((char *)&big_dipper_ret, sizeof(big_dipper_ret));
	this->SendInfo();
}

bool BigDipperManager::CanBattleMonsterBySn(int sn)
{
	//return data.HasMonsterSn(sn);
	return true;
}

void BigDipperManager::SendInfo(Role * role)
{
	Protocol::SCBigDipperInfo info;
	info.monster_num = 0;
	memset(info.open_monster_list, 0, sizeof(info.open_monster_list));
	for (int i = 0; info.monster_num < MAX_BIG_DIPPER_MONSTER_NUM && i < data.open_monster_num; i++)
	{
		info.open_monster_list[info.monster_num++] = data.monster_list[i];
	}
	if (role == NULL)
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&info, sizeof(info) - ((MAX_BIG_DIPPER_MONSTER_NUM - info.monster_num) * sizeof(info.open_monster_list[0])));
	}
	else
	{
		role->GetRoleModuleManager()->NetSend((const char*)&info, sizeof(info) - ((MAX_BIG_DIPPER_MONSTER_NUM - info.monster_num) * sizeof(info.open_monster_list[0])));
	}
}
