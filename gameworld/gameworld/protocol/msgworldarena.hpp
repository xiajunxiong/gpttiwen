#ifndef __MSG_WORLD_ARENA_HPP__
#define __MSG_WORLD_ARENA_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/worldarenaparam.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class SCWorldArenaInfo							//!< 4400 ������Ϣ
	{
	public:
		SCWorldArenaInfo();
		MessageHeader header;

		int already_signup_num;						//!< �ѱ�������
		short cur_status;							//!< ��ǰ״̬
		short reserve_sh;							//!< ������
		unsigned int next_status_timestamp;			//!< �´�״̬�仯ʱ���
		short season_index;							//!< ������  ��1��ʼ
		short total_group_num;						//!< �ܷ�����
	};

	class SCWorldArenaUserSignupInfo				//!< 4401 ��ұ�����Ϣ
	{
	public:
		SCWorldArenaUserSignupInfo();
		MessageHeader header;

		short has_signup;
		short reserve_sh;
	};

	class CSWorldArenaOperate						//!< 4402 ��Ҳ�������
	{
	public:
		CSWorldArenaOperate();
		MessageHeader header;

		enum OPER_TYPE
		{
			OPER_TYPE_INFO = 0,
			OPER_TYPE_SIGNUP = 1,			//!< ���� 
			OPER_TYPE_SCHEDULE_INFO = 2,	//!< �鿴������Ϣ
			OPER_TYPE_USER_BASE_INFO = 3,	//!< �鿴���ǵĻ�����Ϣ
			OPER_TYPE_ENTER_SCENE = 4,		//!< ǰ������
			OPER_TYPE_FETCH_WIN_BOX = 5,	//!< ��ȡʤ������ param1=sn (�����е����sn�ֶ�)
			OPER_TYPE_GROUP_RANK_LIST = 6,	//!< ��ѯ������а� param1=group_idx (������[0, �ܷ�����])
			OPER_TYPE_STANDBY_FOR_FIGHT = 7,//!< ��������׼�� param1=[1:׼�� 0:ȡ��׼��] ��������
			OPER_TYPE_FETCH_SIGNUP_REWARD = 8,	//!< ��ȡ��������
			OPER_TYPE_FETCH_DAILY_REWARD = 9,	//!< ��ȡÿ�ս���
		};

		short oper_type;
		short reserve_sh;

		int param1;
		int param2;
	};

	struct MsgWorldArenaOneSchedule
	{
		void Reset()
		{
			plat_type = 0;
			uid = 0;
			memset(name, 0, sizeof(name));
			avatar_type = 0;
			headshot_id = 0;
			profession = 0;
			memset(guild_name, 0, sizeof(guild_name));
			rank = 0;
			round_idx = 0;
		}

		void SetFightResult(int real_result, bool is_attacker)
		{
			switch (real_result)
			{
			case WORLD_ARENA_RESULT_ATTACKER_WIN:
				{
					if (is_attacker) result = 1;
					else result = 2;
				}
				break;

			case WORLD_ARENA_RESULT_DEFENDER_WIN:
				{
					if (is_attacker) result = 2;
					else result = 1;
				}
				break;

			case WORLD_ARENA_RESULT_DRAW:
				{
					result = 3;
				}
				break;

			case WORLD_ARENA_RESULT_BOTH_GIVEUP:
				{
					result = 2;
				}
				break;

			default:
				{
					result = 0;
				}
				break;
			}
		}

		int plat_type;							//!< spid   (���ڲ������ϸ���ݣ�
		int uid;								//!< ��ɫID (���ڲ������ϸ���ݣ�
		GameName name;							//!< ��ɫ����
		short level;							//!< �ȼ�
		short avatar_type;						//!< ģ��
		int headshot_id;						//!< ͷ��ID
		int profession;							//!< ְҵ (����ְҵ����״�����ϣ�
		GuildName guild_name;					//!< ������
		int rank;								//!< ȫ������
		int round_idx;							//!< ���� ȡֵ��Χ[0, 19)   
		short result;							//!< ��� [0:δ�����  1:ʤ  2:��  3:ƽ]
		short reserve_sh;						//!< ������
	};

	class SCWorldArenaUserScheduleInfo			//!< 4403 ���������Ϣ
	{
	public:
		SCWorldArenaUserScheduleInfo();
		MessageHeader header;

		short has_schedule;						//!< �Ƿ��ѷ��飨1:��  0:��) 
		short group_idx;						//!< ������ţ�ȡֵ��Χ��[0, �ܷ�����)

		MsgWorldArenaOneSchedule schedule_list[WORLD_ARENA_ROUND_NUM];	//!< \see MsgWorldArenaOneSchedule
	};

	class SCWorldArenaUserBaseInfo		//!< 4404 ��һ�����Ϣ
	{
	public:
		SCWorldArenaUserBaseInfo();
		MessageHeader header;

		char has_signup;				//!< �Ƿ���
		char has_fetch_signup_reward;	//!< �Ƿ�����ȡ��������
		short cur_rank;					//!< ��ǰ���� ��0��ʼ
		short group_index;				//!< ���  [0, �ܷ�����)
		short win_num;					//!< ʤ����
		short score;					//!< ����
		char daily_reward_fetch_flag;	//!< ÿ�ս�����ȡ���( 0:������ȡ 1:����ȡ 2:����ȡ )
		char reserve_ch;				//!< ������
		int win_box_reward_flag;		//!< ʤ��������ȡ��� (�����ƣ�
	};

	struct MsgRankListUserInfo
	{
		int plat_type;
		int uid;
		GameName name;			
		int profession;			//!< ְҵ
		short win_num;			//!< ʤ����
		short lose_num;			//!< ������
		short draw_num;			//!< ƽ����
		short total_num;		//!< �ܳ��� (�ܳ��� = ʤ + ƽ + �� + ��Ȩ����
		int score;				//!< ����
	};

	struct MsgTopThreeAdditionInfo
	{		
		Appearance appearance;
	};

	class SCWorldArenaGroupRankList		//!< 4405 �������а���Ϣ�������򣬰�˳����ʾ���ɣ�
	{
	public:
		SCWorldArenaGroupRankList();
		MessageHeader header;

		int group_idx;	//!< ���  [0, �ܷ�����)
		int user_num;	//!< ��ɫ��
		MsgTopThreeAdditionInfo top3_info_list[3];	//!< ����ض���3��
		MsgRankListUserInfo user_list[WORLD_ARENA_FIGHTER_NUM_PER_GROUP];   //!< ���20�ˣ���user_num��
	};

	class SCWorldArenaCountDownToFight	//!< 4406 ����ʱ����ս��
	{
	public:
		SCWorldArenaCountDownToFight();
		MessageHeader header;

		int count_down_s;			//!< ��������
	};

	class SCWorldArenaStartEnterSceneNotice		//!< 4407 ��ʼ����֪ͨ
	{
	public:
		SCWorldArenaStartEnterSceneNotice();
		MessageHeader header;

		unsigned int enter_scene_end_timestamp;	//!< ��������ʱ���
		short start_round_index;				//!< ��ʼ���ǵڼ���ս��
		short has_finish_this_round;			//!< ����ս���Ƿ������
	};

	struct MsgWorldArenaShowUpRoleInfo
	{	
		MsgWorldArenaShowUpRoleInfo() : plat_type(0), uid(0), level(0), profession(0),
			avatar_type(0), headshot_id(0)
		{
			memset(name, 0, sizeof(name));
		}

		int plat_type;				//!< ������
		int uid;					//!< ����id  �����������Ųſ�Ψһ�ж����ĸ���ң�
		int level;					//!< �ȼ�
		int profession;				//!< ְҵ
		short avatar_type;			//!< ģ��
		short headshot_id;			//!< ͷ��
		GameName name;				//!< ����		
	};

	class SCWorldArenaShowUpInfo		//!< 4408 �۽�������Ϣ
	{
	public:
		SCWorldArenaShowUpInfo();
		MessageHeader header;

		short is_attacker_show_up;		//!< �����Ƿ񵽳� 0:�� 1:��
		short is_defender_show_up;		//!< �ط��Ƿ񵽳�

		MsgWorldArenaShowUpRoleInfo attacker_info;
		MsgWorldArenaShowUpRoleInfo defender_info;
	};
}

#pragma pack(pop)


#endif