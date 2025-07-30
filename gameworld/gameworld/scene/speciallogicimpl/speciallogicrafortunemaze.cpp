#include "speciallogicrafortunemaze.hpp"
#include "world.h"
#include "obj/character/role.h"
#include "obj/otherobj/treasurechest.h"
#include "config/randactivityconfig/impl/randactivityfortunemazeconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "engineadapter.h"
#include "protocol/msgscene.h"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"

SpecialLogicFortuneMaze::SpecialLogicFortuneMaze(Scene * scene) : SpecialLogic(scene)
{
	kick_out_tamp = 0;
	box_num = 0;
}

SpecialLogicFortuneMaze::~SpecialLogicFortuneMaze()
{
}

void SpecialLogicFortuneMaze::OnRoleEnterScene(Role * role)
{
	this->SendBoxInfo(role);
}

void SpecialLogicFortuneMaze::Update(unsigned long interval, time_t now_second)
{
	if (kick_out_tamp > 0 && kick_out_tamp <= now_second)
	{
		this->DelayKickOutAllRole();
	}
}

bool SpecialLogicFortuneMaze::OnOpenTreasureChest(Role * role, TreasureChest * transport_obj)
{
	if (transport_obj == NULL)return false;
	int reward_group_id = 0;
	transport_obj->GetParam(&reward_group_id, NULL, NULL);
	const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL == config) return false;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
	if (NULL == ramgr) return false;

	const FortuneMazeRewardGroupCfg * reward_cfg = config->GetFortuneMazeRewardGroupConfig(reward_group_id, ramgr);
	if (reward_cfg == NULL)return false;

	if (!reward_cfg->reward_vec.empty())
	{
		if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporary((short)reward_cfg->reward_vec.size(),&reward_cfg->reward_vec[0]))
		{
			role->GetRoleModuleManager()->GetKnapsack()->PutList((short)reward_cfg->reward_vec.size(), &reward_cfg->reward_vec[0], PUT_REASON_RA_FORTUNE_MAZE);
		}
		else
		{
			// ·¢ÓÊ¼þ
			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, reward_cfg->reward_vec);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_fortune_maze_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(role->GetRoleModuleManager()->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}

	if(reward_cfg->hearsay == 1)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_randactivity_fortune_maze_content,
			role->GetName(), RAND_ACTIVITY_TYPE_FORTUNE_MAZE, transport_obj->GetTreasureChestid(), RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
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
	kick_out_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	box_num -= 1;
	this->SendBoxInfo(role);
	RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
	if (NULL != fortune_maze)
	{
		fortune_maze->OnRoleOpenTreasureChest();
	}
	return true;
}

void SpecialLogicFortuneMaze::CreateBox(int npc_id, int reward_group_id, Posi box_pos)
{
	TreasureChest  * treasure_chest_obj = new TreasureChest(npc_id);
	treasure_chest_obj->SetPos(box_pos);
	treasure_chest_obj->SetParam(&reward_group_id);
	m_scene->AddObj(treasure_chest_obj);
	box_num += 1;
}

void SpecialLogicFortuneMaze::SendBoxInfo(Role * role)
{
	Protocol::SCRAFortuneMazeBoxNum msg;
	msg.box_num = box_num;
	role->GetRoleModuleManager()->NetSend((const char *)&msg, sizeof(msg));
}
