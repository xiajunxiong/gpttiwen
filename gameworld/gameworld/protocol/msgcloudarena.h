#ifndef __MSG_CLOUD_ARENA_H__
#define __MSG_CLOUD_ARENA_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/cloudarenadef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSCloudArenaOperate	//!< 3675 �ƶ˾�����������
	{
	public:
		enum OPER_TYPE
		{
			OPER_TYPE_SELF_INFO = 0,		//!< ��ȡ������Ϣ
			OPER_TYPE_TARGET_INFO = 1,		//!< ��ȡĿ����Ϣ
			OPER_TYPE_REFRESH_TARGET = 2,	//!< ˢ��Ŀ���б�
			OPER_TYPE_START_FIGHT = 3,		//!< ��ʼ��ս  param1:��ս��X��Ŀ�� X=[0, 4)
			OPER_TYPE_BUY_FIGHT_TIMES = 4,	//!< ������ս�������ѷ�����
			OPER_TYPE_RECORD_LIST = 5,		//!< ����ս���б�  param1:�ӵڼ���ս����ʼ  param2:���������ս��
			OPER_TYPE_RANK_LIST = 6,		//!< �������а�  param1:���а�����  (0:�ܰ�  1:������  2:��λ��)
			OPER_TYPE_FETCH_RANKING_REWARD = 7,	//!< ��ȡ��λ����  param1:��λid (��λ����ranking�ֶ�)
			OPER_TYPE_OPEN_UI = 8,			//!< �򿪽���UI
			OPER_TYPE_ACHIEVEMENT_INFO = 9,	//!< ��ȡ�ɾ���Ϣ
			OPER_TYPE_FETCH_ACHIEVE = 10,	//!< ��ȡ�ɾͽ��� param1:�ɾ�����[0,10)  \see CLOUD_ARENA_ACHIEVEMENT
			OPER_TYPE_FETCH_DAY_REWARD = 11,//!< ��ȡ�ս���
			OPER_TYPE_QUERY_FORMATION = 12,	//!< ��ѯ����
		};

	public:
		CSCloudArenaOperate();
		MessageHeader header;

		int operate_type;
		int param1;
		int param2;
	};

	class SCCloudArenaSelfInfo	//!< 3676 ������Ϣ����
	{
	public:
		SCCloudArenaSelfInfo();
		MessageHeader header;

		int cur_rank;							//!< ��ǰ����
		int cur_score;							//!< ��ǰ����
		int capability;							//!< ����
		short remain_fight_times;				//!< ʣ����ս����
		char day_reward_status;					//!< ÿ�ս���״̬ 0:������ȡ  1:����ȡ  2:����ȡ
		char has_today_fight;					//!< �����Ƿ���ս�� 0:�� 1:��
		unsigned int season_finish_timestamp;	//!< ��������ʱ���
		unsigned int next_recover_fighttimes_timestamp; //!< �´���ս�����ָ�ʱ���
		int reach_ranking_flag;					//!< ��ɶ�λ��� (�Զ����Ƶ���ʽ��ʾÿ����λ�Ƿ��ɣ�
		int fetch_ranking_reward_flag;			//!< ��ȡ��λ������ǣ��Զ����Ƶ���ʽ��ʾÿ����λ��ɽ����Ƿ�����ȡ��
		int season_idx;							//!< ��ǰ�ڼ����� ��1��ʼ	
		int today_buy_times;					//!< �����ѹ�����ս����Ĵ���
		int last_day_score;						//!< ǰһ��Ļ���
	};

	class SCCloudArenaTargetInfo	//!< 3677 ������Ϣ����
	{
	public:
		SCCloudArenaTargetInfo();
		MessageHeader header;

		struct TargetInfo
		{
			int rank;				//!< ����
			int score;				//!< ����
			int capability;			//!< �ۺ�����
			int top_level;			//!< �۷�ȼ�
			short level;			//!< �ȼ�
			short is_robot;			//!< �Ƿ������(1:�� 0:��)
			GameName name;			//!< ����
			Appearance appearance;	//!< ���
		};

		unsigned int next_manual_refresh_timestamp;	//!< �´ο��ֶ�ˢ��ʱ���
		int today_already_refresh_times;			//!< �������ֶ�ˢ�´���
		int count;
		TargetInfo target_list[CLOUD_ARENA_OPPONENT_NUM];
	};

	class SCCloudArenaFightRecordList	//!< 3679 ���ս���·�
	{
	public:
		SCCloudArenaFightRecordList();
		MessageHeader header;

		int offset;			//!< �ӵڼ���ս����ʼ
		int count;			//!< �·��˶�����ս��
		CloudArenaFightRecord record_list[CLOUD_ARENA_USER_FIGHT_RECORD_NUM];
	};

	class SCCloudArenaNewFightRecordNotice	//!< 3678 �����ս��֪ͨ
	{
	public:
		SCCloudArenaNewFightRecordNotice();
		MessageHeader header;
	};

	struct RankInfo
	{
		RankInfo() : rank(0), uid(0), level(0), is_robot(0), profession(0), score(0)
		{
			memset(name, 0, sizeof(name));
		}

		int rank;
		int uid;
		int top_level;			//�۷�ȼ�
		short level;
		short is_robot;
		int profession;
		int score;
		GameName name;
	};

	class SCCloudArenaRankList				//!< 3680 ���а�����
	{
	public:
		static const int MAX_RANK_NUM = 100;

	public:
		SCCloudArenaRankList();
		MessageHeader header;

		short rank_type;					//!< rank����
		short rank_num;						//!< rank_list����
		RankInfo my_rank;					//!< �ҵ�����
		RankInfo rank_list[MAX_RANK_NUM];	//!< �����б�
	};

	class SCCloudArenaNoticeSeasonInitialScore	//!< 3681 ֪ͨ������ʼ��λ
	{
	public:
		SCCloudArenaNoticeSeasonInitialScore();
		MessageHeader header;

		int season_initial_score;
	};

	class SCCloudArenaNoticeDanChange	//!< 3682 ֪ͨ���λ���
	{
	public:
		SCCloudArenaNoticeDanChange();
		MessageHeader header;

		short old_dan;		//!< �ɵĶ�λ
		short new_dan;		//!< �µĶ�λ
	};

	class SCCloudArenaAchievementInfo	//!< 3683 �ɾ���Ϣ�·�
	{
	public:
		SCCloudArenaAchievementInfo();
		MessageHeader header;

		struct AchievementItem
		{
			int progress_num;
			short cur_level;
			short reward_status;
		};

		AchievementItem achieve_list[CLOUD_ARENA_ACHIEVEMENT_MAX];
	};

	class SCCloudArenaAchievementUpdateNotice	//!< 3684 �����ɾ���Ϣ����
	{
	public:
		SCCloudArenaAchievementUpdateNotice();
		MessageHeader header;

		int achieve_type;
		int progress_num;
		short cur_level;
		short reward_status;
	};

	class SCCloudArenaRoleSeasonHighestData		//!< 3685 ���������������·�
	{
	public:
		SCCloudArenaRoleSeasonHighestData();
		MessageHeader header;

		int season_highest_rank;		//!< ����������� ������0��ʼ��-1��ʾû������
		short season_highest_dan_id;	//!< ������߶�λid  
		short season_highest_score;		//!< ������߷���
		int is_show_new_shop;			//!< �Ƿ���ʾ�µ�񷶥�̵�
	};

	class SCCloudArenaRealTimeInfo	//!< 3686 ����ʵʱ������������
	{
	public:
		SCCloudArenaRealTimeInfo();
		MessageHeader header;

		int cur_rank;			//!< ��ǰ�������ͻ���Ҫ+1
		int cur_score;			//!< ��ǰ����
	};

	class CSCloudArenaRealTimeInfo //!< 3687 ����ʵʱ��������
	{
	public:
		CSCloudArenaRealTimeInfo();
		MessageHeader header;
	};

	class CSCloudArenaSetFormation	//!< 3688 ��������
	{
	public:
		CSCloudArenaSetFormation();
		MessageHeader header;

		struct GridInfo
		{
			int id;					// ����uid �� ����id �� ���id 
			short type;				// 0Ϊ��  1Ϊ����  2Ϊ����  3Ϊ���
			short reserve_sh;
			long long unique_id;    // Ϊ����ʱ�������ΨһID
		};

		struct PetStandbyInfo
		{
			int pet_id;				// ��������id ֵΪ0��û��
			long long unique_id;	// ��������ΨһID
		};

		enum FORMATION_GRID_TYPE
		{
			FORMATION_GRID_INVALID = 0,
			FORMATION_GRID_ROLE = 1,
			FORMATION_GRID_PET = 2,
			FORMATION_GRID_PARTNER = 3,
			FORMATION_GRID_MAX
		};

		bool CheckStanbyPetNum()
		{
			int standby_num = 0;
			for (int i = 0; i < ARRAY_ITEM_COUNT(standby_list); ++i)
			{
				if (standby_list[i].pet_id > 0) standby_num += 1;
			}

			if (standby_num >= CLOUD_ARENA_STANDBY_PET_MAX_NUM)
			{
				if (grid_list[2].type == FORMATION_GRID_PET ||
					grid_list[7].type == FORMATION_GRID_PET)
				{
					return false;
				}
			}

			return true;
		}

		bool CheckDuplicate()
		{
			for (int i = 0; i < ARRAY_ITEM_COUNT(grid_list) - 1; ++i)
			{
				if (FORMATION_GRID_INVALID == grid_list[i].type)
				{
					continue;
				}

				for (int k = i + 1; k < ARRAY_ITEM_COUNT(grid_list); ++k)
				{
					if (grid_list[i].type == grid_list[k].type &&
						grid_list[i].id == grid_list[k].id)
					{
						if (FORMATION_GRID_PET == grid_list[i].type)
						{
							if (grid_list[i].unique_id == grid_list[k].unique_id)
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}
				}

				if (FORMATION_GRID_PET == grid_list[i].type)
				{
					for (int k = 0; k < ARRAY_ITEM_COUNT(standby_list); ++k)
					{
						if (grid_list[i].id == standby_list[k].pet_id &&
							grid_list[i].unique_id == standby_list[k].unique_id)
						{
							return false;
						}
					}
				}
			}

			for (int i = 0; i < ARRAY_ITEM_COUNT(standby_list) - 1; ++i)
			{
				if (0 == standby_list[i].pet_id)
				{
					continue;
				}

				for (int k = i + 1; k < ARRAY_ITEM_COUNT(standby_list); ++k)
				{
					if (standby_list[i].pet_id == standby_list[k].pet_id &&
						standby_list[i].unique_id == standby_list[k].unique_id)
					{
						return false;
					}
				}
			}

			return true;
		}

		GridInfo grid_list[SIDE_CHARACTER_NUM]; 
		PetStandbyInfo standby_list[CLOUD_ARENA_STANDBY_PET_MAX_NUM];
	};

	class SCCloudArenaFormationInfo	//!< 3689 �����·�
	{
	public:
		SCCloudArenaFormationInfo();
		MessageHeader header;

		struct GridInfo
		{
			GridInfo() { this->Reset(); }

			void Reset()
			{
				id = 0;
				type = 0;
				reserve_sh = 0;
				unique_id = 0;
			}

			int id;					// ����uid �� ����id �� ���id 
			short type;				// 0Ϊ��  1Ϊ����  2Ϊ����  3Ϊ���
			short reserve_sh;
			long long unique_id;    // Ϊ����ʱ�������ΨһID
		};

		struct PetStandbyInfo
		{
			PetStandbyInfo() { this->Reset(); }
			void Reset()
			{
				pet_id = 0;
				unique_id = 0;
			}

			int pet_id;				// ��������id ֵΪ0��û��
			long long unique_id;	// ��������ΨһID
		};

		void Reset()
		{
			for (int i = 0; i < ARRAY_LENGTH(grid_list); ++i)
			{
				grid_list[i].Reset();
			}

			for (int i = 0; i < ARRAY_LENGTH(standby_list); ++i)
			{
				standby_list[i].Reset();
			}
		}

		GridInfo grid_list[SIDE_CHARACTER_NUM]; 
		PetStandbyInfo standby_list[CLOUD_ARENA_STANDBY_PET_MAX_NUM];
	};
}

#pragma pack(pop)

#endif