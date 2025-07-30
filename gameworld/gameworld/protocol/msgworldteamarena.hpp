#ifndef __MSG_WORLD_TEAM_ARENA_HPP__
#define __MSG_WORLD_TEAM_ARENA_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/worldteamarenaparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class CSWorldTeamArenaReq		//!< 4600 英雄会武请求
	{
	public:
		CSWorldTeamArenaReq();
		MessageHeader header;

		enum OP_TYPE
		{	
			OP_TYPE_SIGN_UP_ALL_INFO = 0,		//请求报名信息列表
			OP_TYPE_MEMBER_SIGN_UP = 1,			//成员确认报名
			OP_TYPE_CANCEL_SIGN_UP = 2,			//取消报名
			OP_TYPE_SIGN_UP_DETAILED_INFO = 3,	//请求查询某个战队详细信息(从报名) P1:team_unique_id P2:type(0:战队报名 1:比赛)
			OP_TYPE_RANK_DETAILED_INFO = 4,		//请求查询某个战队详细信息(从最终排名) P1:team_unique_id
			OP_TYPE_TEAM_SPECIAL_INFO = 5,		//请求队伍特殊信息
			OP_TYPE_OTHER_INFO = 6,				//请求其它信息
		};

		int op_type;
		long long param1;
		int param2;
	};

	class  SCWorldTeamArenaAllSimpleInfo : public IMessageSerializer	//!< 4601 所有报名战队信息下发
	{
	public:
		SCWorldTeamArenaAllSimpleInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(my_team_index, buffer, buffer_size, out_length);
			result = result && WriteShort(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count && i < ARRAY_LENGTH(team_list); i++)
			{
				const WorldTeamArenaTeamSignUpSimpleInfo & info = team_list[i];
				result = result && WriteLL(info.team_unique_id, buffer, buffer_size, out_length);
				result = result && WriteStrN(info.team_name, sizeof(info.team_name), buffer, buffer_size, out_length);
				result = result && WriteLL(info.total_capablity, buffer, buffer_size, out_length);

				result = result && WriteShort(info.level, buffer, buffer_size, out_length);
				result = result && WriteShort(info.top_level, buffer, buffer_size, out_length);
				result = result && WriteShort(info.avatar_type, buffer, buffer_size, out_length);
				result = result && WriteShort(info.headshot_id, buffer, buffer_size, out_length);
				result = result && WriteShort(info.fly_flag, buffer, buffer_size, out_length);

				for (int k = 0; k < ARRAY_LENGTH(info.member_list); ++k)
				{
					result = result && WriteInt(info.member_list[k].role_id, buffer, buffer_size, out_length);
					result = result && WriteInt(info.member_list[k].profession, buffer, buffer_size, out_length);
				}
			}
			result = result && WriteShort(know_count, buffer, buffer_size, out_length);
			for (int i = 0; i < know_count && i < ARRAY_LENGTH(know_team_index); i++)
			{
				result = result && WriteShort(know_team_index[i], buffer, buffer_size, out_length);
			}

			return result;
		}

		int my_team_index;
		short count;
		short know_count;
		WorldTeamArenaTeamSignUpSimpleInfo team_list[MAX_WORLD_TEAM_ARENA_SIGN_UP_TEAM_INFO_NUM];
		short know_team_index[MAX_WORLD_TEAM_ARENA_SIGN_UP_TEAM_INFO_NUM];				//熟识战队  数组值为上面数组下标
	};

	//修改为查找/报名
	class CSWorldTeamArenaSignUpReq		//!< 4602 英雄会武报名请求
	{
	public:
		CSWorldTeamArenaSignUpReq();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_SIGN_UP = 0,		//报名
			OP_TYPE_QUERY_TEAM = 1,		//查询战队		//战队名/角色名(不支持模糊搜索)
		};

		int op_type;
		GameName name;
	};

	class SCWorldTeamArenaPreTeamSignUpInfo		//!< 4603 英雄会武预备报名信息下发
	{
	public:
		SCWorldTeamArenaPreTeamSignUpInfo();
		MessageHeader header;

		PreTeamSignUpInfo pre_team_sign_up_info;
	};

	class SCWorldTeamArenaNoticeInfo		//!< 4604 英雄会武通知信息下发
	{
	public:
		SCWorldTeamArenaNoticeInfo();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_MEMBER_SIGN_UP = 0,				//成员确认报名 param1:member_index param2:determine_flag
			NOTICE_TYPE_SIGN_UP_FAILED_CLOSE = 1,		//报名失败关闭报名界面

			NOTICE_TYPE_MAX
		};

		int notice_type;
		int param1;
		int param2;
	};

	class SCWorldTeamArenaTeamSignUpDetailedInfo	//!< 4605 英雄会武战队详细信息下发(已拥有战队)
	{
	public:
		SCWorldTeamArenaTeamSignUpDetailedInfo();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_SIGN_UP = 0,		//报名界面
			NOTICE_TYPE_GAME = 1,			//比赛界面
			NOTICE_TYPE_RANK_END = 2,		//排名结算界面
		};

		int notice_type;
		WorldTeamArenaTeamSignUpDetailedInfo team_detailed_info;	
	};
	
	class SCActivityWorldTeamArenaInfo		//!< 4606 英雄会武活动信息下发
	{
	public:
		SCActivityWorldTeamArenaInfo();
		MessageHeader header;

		int season_num;								//赛季
		unsigned int start_sign_up_timestamp;		//开始报名时间戳
		unsigned int end_sign_up_timestamp;			//停止报名时间戳
		unsigned int activity_open_next_zero_timestamp;	//活动开始当晚0点时间戳
	};

	class CSActivityWorldTeamArenaSceneReq	//!< 4607 英雄会武场景操作请求
	{
	public:
		CSActivityWorldTeamArenaSceneReq();
		MessageHeader header;

		enum WORLD_TEAM_ARENA_SCENE_OP_TYPE
		{
			WTA_OP_TYPE_REQ_ALL_RANK_INFO = 0,		//!< 请求所有排名信息(打开排行榜)
			WTA_OP_TYPE_REQ_CLOSE_RANK = 1,			//!< 请求关闭排行榜 
			WTA_OP_TYPE_REQ_FIGHT_OTHER = 2,		//!< 请求战斗 P1:被动战斗玩家ID
		};

		int op_type;
		int param1;
	};

	class SCActivityWorldTeamArenaRankAllInfo			//!< 4608 英雄会武排行榜信息
	{
	public:
		SCActivityWorldTeamArenaRankAllInfo();
		MessageHeader header;

		int resi_team_num;			//!< 剩余战队数量
		int list_num;
		WorldTeamArenaRankInfo rank_list[ACTIVITY_RANK_DEFAULT_NUM_MAX];
	};

	class SCActivityWorldTeamArenaMyInfo					//!< 4609 英雄会武自身战队排行榜信息
	{
	public:
		SCActivityWorldTeamArenaMyInfo();
		MessageHeader header;

		enum WORLD_TEAM_ARENA_MY_INFO_TYPE
		{
			WORLD_TEAM_ARENA_MY_INFO_TYPE_SEND = 0,			//发送我的排行信息
			WORLD_TEAM_ARENA_MY_INFO_TYPE_NOTICE = 1,		//通知显示排行榜结果及下发我的排行信息		
		};

		int type;
		int rank_posi;							//!< 排名位置 - 0 表示未上榜
		WorldTeamArenaRankInfo m_info;			//!< 我的排行信息
	};

	class SCActivityWorldTeamArenaSpecialInfo		//!< 4610 英雄会武特殊信息下发
	{
	public:
		SCActivityWorldTeamArenaSpecialInfo();
		MessageHeader header;

		int resi_team_num;			//!< 剩余战队数量
	};

	class SCActivityWorldTeamArenaTeamSpecialInfo		//!< 4611 英雄会武队伍特殊信息下发
	{
	public:
		SCActivityWorldTeamArenaTeamSpecialInfo();
		MessageHeader header;

		WorldTeamArenaTeamSpecialInfo member_list[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];
	};

	class SCActivityWorldTeamArenaFightInfo : public IMessageSerializer 	//!< 4612 英雄会武发起战斗信息下发
	{
	public:
		SCActivityWorldTeamArenaFightInfo();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteUInt(start_fight_timestamp, buffer, buffer_size, out_length);
			for (int team_index = 0; team_index < 2; team_index++)
			{
				ActivityWorldTeamArenaTeamFightInfo team_info;
				if (0 == team_index)
				{
					team_info = fight_info.attacker_team;
				}
				else
				{
					team_info = fight_info.defender_team;
				}
				result = result && WriteInt(team_info.rank_posi, buffer, buffer_size, out_length);
				result = result && WriteStrN(team_info.team_name, sizeof(team_info.team_name), buffer, buffer_size, out_length);
				result = result && WriteInt(team_info.member_count, buffer, buffer_size, out_length);
				for (int i = 0; i < team_info.member_count && i < ARRAY_LENGTH(team_info.member_list); ++i)
				{
					result = result && WriteStrN((const char *)&team_info.member_list[i], sizeof(team_info.member_list[i]), buffer, buffer_size, out_length);
				}
			}

			return result;
		}

		unsigned int start_fight_timestamp;				//开始战斗时间戳
		ActivityWorldTeamArenaFightInfo fight_info;
	};

	class CSActivityWorldTeamArenaRankReq			//!< 4613 英雄会武最终榜单请求
	{
	public:
		CSActivityWorldTeamArenaRankReq();
		MessageHeader header;

	};

	class SCActivityWorldTeamArenaFinallyRankInfo		//!< 4614 英雄会武最终榜单信息下发
	{
	public:
		SCActivityWorldTeamArenaFinallyRankInfo();
		MessageHeader header;

		int rank_count;
		int my_rank_posi;						//!< 排名位置 - 0 表示未上榜
		WorldTeamArenaRankInfo my_rank_info;
		WorldTeamArenaRankInfo rank_list[ACTIVITY_RANK_DEFAULT_NUM_MAX];
	};

	class SCActivityWorldTeamArenaOtherInfo			//!< 4615 英雄会武其它信息下发
	{
	public:
		SCActivityWorldTeamArenaOtherInfo();
		MessageHeader header;

		int is_sign_up;				//!< 0:没报名 1:已报名
	};

}

#pragma pack(pop)
#endif	// __MSG_WORLD_TEAM_ARENA_HPP__