#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "obj/character/role.h"

#include "protocol/msgother.h"
#include "treasurechest.h"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "gameworld/npc/npcpool.h"
#include "servercommon/string/gameworldstr.h"
#include "scene/scenemanager.hpp"
#include "global/team/team.hpp"
#include "servercommon/errornum.h"

TreasureChest::TreasureChest() :Obj(OBJ_TYPE_TREASURE_CHEST), m_treasure_chest_id(0), m_is_open(false)
{
}

TreasureChest::TreasureChest(int treasure_chest_id) : Obj(OBJ_TYPE_TREASURE_CHEST), m_treasure_chest_id(treasure_chest_id), m_is_open(false)
{
}

TreasureChest::~TreasureChest()
{
}

void TreasureChest::Init(int treasure_chest_id)
{
	m_treasure_chest_id = treasure_chest_id;
}

void TreasureChest::Open(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	if ((m_posi - role->GetPos()).IsLongerThan(30))
	{
		return;
	}

	if (this->IsOpen())
	{
		role->NoticeNum(errornum::EN_TREASURE_CHEST_IS_OPEN);
		return;
	}

	SpecialLogic * special_lobgic = m_scene->GetSpecialLogic();
	if (special_lobgic == NULL)
	{
		return;
	}

	Team * team = role->GetMyTeam();//如果组队，则判断是否队长
	if (team != NULL)
	{
		if (!team->IsLeader(role))
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
			return;
		}
	}

	const NPCTreasureChestInfoCfg * cfg = NPCPOOL->GetTreasureChestCfg(m_treasure_chest_id);
	if (cfg != NULL)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg->consume_id, cfg->consume_num, "TreasureChest::Open", 0, true))
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_knapsack_on_consume_message2, cfg->consume_id, cfg->consume_num);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
			}
		}
	}

	

	if (special_lobgic->OnOpenTreasureChest(role, this))
	{
		m_is_open = true;

		static Protocol::SCNoticeNearbyOnOpenTreasureChest notice;
		notice.obj_id = this->GetId();
		notice.reserved = 0;

		if (NULL != m_scene)
		{
			m_scene->GetZoneMatrix()->NotifyAreaMsg(m_obser_handle, (void *)&notice, sizeof(notice));
		}
	}
}

bool TreasureChest::IsOpen()
{
	return m_is_open;
}

void TreasureChest::SetOpen(bool is_open)
{
	m_is_open = is_open;
}

int TreasureChest::GetTreasureChestid()
{
	return m_treasure_chest_id;
}

void TreasureChest::SetParam(const int * p1, const int * p2, const int * p3)
{
	if (p1 != NULL)
	{
		m_param1 = *p1;
	}

	if (p2 != NULL)
	{
		m_param2 = *p2;
	}

	if (p3 != NULL)
	{
		m_param3 = *p3;
	}
}

void TreasureChest::GetParam(int *ARG_OUT p1, int *ARG_OUT p2, int *ARG_OUT p3)
{
	if (p1 != NULL)
	{
		*p1 = m_param1;
	}

	if (p2 != NULL)
	{
		*p2 = m_param2;
	}

	if (p3 != NULL)
	{
		*p3 = m_param3;
	}
}

