#include "activityguildanswer.hpp"
#include "world.h"
#include "global/guild/guildmanager.hpp"
#include "scene/speciallogicimpl/speciallogicguildanswer.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/guildanswer/activityguildanswerconfig.hpp"
#include "other/monitor/systemmonitor.hpp"
#include "global/worldstatus/worldstatus.hpp"

ActivityGuildAnswer::ActivityGuildAnswer(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_GUILD_ANSWER)
{
}

ActivityGuildAnswer::~ActivityGuildAnswer()
{
}

void ActivityGuildAnswer::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	static const double EXPECT_JOIN_PERCENT = 0.5; // 假设50%的人会来参加活动
#ifndef _DEBUG
	static const int ROOM_ROLE_NUM = 50; // 每间房限定50人
#else
	static const int ROOM_ROLE_NUM = 1;	 // DEBUG模式下限定1人 方便测试
#endif
	static const int START_ROOM_KEY = 2; // 房间起始key
	
	const GuildAnswerOthers & ga_other = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetGuildAnswerOtherConfig();
	
	switch (to_status)
	{
	case ACTIVITY_STATUS_STANDY:
		{
			// 没有准备时间
		}		
		break;

	case ACTIVITY_STATUS_OPEN:
		{
			// 刷出任务列表
			std::vector<int> question_vec;
			LOGIC_CONFIG->GetActivityGuildAnswerConfig()->RandQuestionList(question_vec);

			m_room_key_vec.clear();
			int total_online_role_num = SystemMonitor::Instance().GetTotalOnlineRoleNum();
			int room_num = (int)(EXPECT_JOIN_PERCENT * total_online_role_num) / ROOM_ROLE_NUM + 1;
			for (int i = 0; i < room_num; ++i)
			{
				int room_key = START_ROOM_KEY + i;
				m_room_key_vec.push_back(room_key);

				SpecialLogicGuildAnswer * sp_ga = World::GetInstWorld()->GetSceneManager()
					->GetSpecialLogicGuildAnswer(ga_other.scene_id, room_key);
				if (NULL == sp_ga) continue;

				sp_ga->OnGuildAnswerBegin(question_vec);
			}
		}	
		break;

	case ACTIVITY_STATUS_CLOSE:
		{
			for (size_t room_idx = 0; room_idx < m_room_key_vec.size(); ++room_idx)
			{
				SpecialLogicGuildAnswer * sp_ga = World::GetInstWorld()->GetSceneManager()->
					GetSpecialLogicGuildAnswer(ga_other.scene_id, m_room_key_vec[room_idx]);
				if (NULL == sp_ga) continue;

				sp_ga->OnGuildAnswerOver();
			}
			m_room_key_vec.clear();
			WorldStatus::Instance().SetFindEndTimestamp(DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_ANSWER);
		}
		break;
	}
}