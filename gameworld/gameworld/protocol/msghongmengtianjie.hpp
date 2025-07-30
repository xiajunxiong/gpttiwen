#ifndef __MSG_HONGMENG_TIANJIE_HPP__
#define __MSG_HONGMENG_TIANJIE_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/hongmengtianjiedef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSHongMengTianJieReq			//!< 5100
	{
	public:
		enum REQ_TYPE
		{
			REQ_TYPE_TALK = 0,					// 广播对话
			REQ_TYPE_ROLE_INFO = 1,				// 请求个人信息
			REQ_TYPE_SEND_REWARD = 2,			// 请求副本奖励记录
			REQ_TYPE_SHOP = 3,					// 请求购买道具			p1：seq p2:num
			REQ_TYPE_WORLD_PASS = 4,			// 请求全服首通信息
			REQ_TYPE_PASS_TEAM = 5,				// 请求首通队伍			p1: fb_id
		};

		CSHongMengTianJieReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCHongMengTianJieSceneInfo		//!< 5101 
	{
	public:
		SCHongMengTianJieSceneInfo();
		MessageHeader header;

		short stage;
		short fb_id;
	};

	class SCHongMengTianJieRoleInfo			//!< 5102
	{
	public:
		SCHongMengTianJieRoleInfo();
		MessageHeader header;

		char help_times;															// 助战次数
		char reward_times[HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM];			// boss奖励次数

		char limit_buy_times[HongMengTianJie::MAX_LIMIT_SEQ_NUM];					// 限购物品购买次数
		char re_ch[2];

		BitMap<HongMengTianJie::MAX_FB_INDEX_NUM> fb_first_pass_mark;				// 副本首通标记 fb_id
	};

	class SCHongMengTianJieWorldInfo		//!< 5103 
	{
	public:
		SCHongMengTianJieWorldInfo();
		MessageHeader header;

		unsigned int first_pass_time[HongMengTianJie::MAX_FB_INDEX_NUM];
	};

	class SCHongMengTianJieTalkInfo		//!< 5104 
	{
	public:
		SCHongMengTianJieTalkInfo();
		MessageHeader header;

		int talk_id;
		int talk_seq;
	};

	class SCHongMengTianJieRewardInfo			//!< 5105
	{
	public:
		SCHongMengTianJieRewardInfo();
		MessageHeader header;

		int list_num;
		ItemPairCon item_list[50];
	};

	class SCHongMengTianJiePassTeamInfo		// 5106
	{
	public:
		struct TeamMemberInfo
		{
			TeamMemberInfo(): uid(0), level(0), profession(0) 
			{
				memset(member_name, 0, sizeof(GameName));
			}

			int uid;			//!< 角色uid
			int level;			//!< 等级		
			int profession;		//!< 职业
			GameName member_name;

			Appearance appearance; //!< 玩家外观信息
		};

		SCHongMengTianJiePassTeamInfo();
		MessageHeader header;

		int fb_id;
		TeamMemberInfo member_list[MAX_TEAM_MEMBER_NUM];
	};
}
#pragma pack(pop)

#endif