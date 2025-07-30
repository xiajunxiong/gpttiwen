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

	static const double EXPECT_JOIN_PERCENT = 0.5; // ����50%���˻����μӻ
#ifndef _DEBUG
	static const int ROOM_ROLE_NUM = 50; // ÿ�䷿�޶�50��
#else
	static const int ROOM_ROLE_NUM = 1;	 // DEBUGģʽ���޶�1�� �������
#endif
	static const int START_ROOM_KEY = 2; // ������ʼkey
	
	const GuildAnswerOthers & ga_other = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetGuildAnswerOtherConfig();
	
	switch (to_status)
	{
	case ACTIVITY_STATUS_STANDY:
		{
			// û��׼��ʱ��
		}		
		break;

	case ACTIVITY_STATUS_OPEN:
		{
			// ˢ�������б�
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