#ifndef __MSG_ROLE_JOIN_ACTIVITY_REWARD_HPP__
#define __MSG_ROLE_JOIN_ACTIVITY_REWARD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSRoleJoinActivityRewardReq				//!< 3692 客户端请求
	{
	public:
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,						//!< 获取信息 (应该用不上)
			REQ_TYPE_GET_REWARD = 1,				//!< 获取奖励 activity_type : 活动类型
			REQ_TYPE_ONE_KEY_FINISH = 2,			//!< 一键完成功能, activity_type: 位运算获取需要一键完成的系统
		};

		enum ONE_KEY_FINISH_TYPE
		{
			ONE_KEY_FINISH_TYPE_SHIMEN = 0,		// 师门(赏金)任务
			ONE_KEY_FINISH_TYPE_SANJIE = 1,		// 三界除魔
			ONE_KEY_FINISH_TYPE_SHENGWANG = 2,	// 声望任务
			ONE_KEY_FINISH_TYPE_XUNXIAN = 3,	// 寻仙(飞升任务)
			ONE_KEY_FINISH_TYPE_JIUZHOU = 4,	// 九州秘宝
			ONE_KEY_FINISH_TYPE_HUABEN = 5,		// 万灵话本

			ONE_KEY_FINISH_TYPE_MAX,
		};
		UNSTD_STATIC_CHECK(ONE_KEY_FINISH_TYPE_MAX < 32);

		CSRoleJoinActivityRewardReq();
		MessageHeader		header;

		int req_type;								// 请求操作类型 
		int activity_type;
	};

	class SCRoleJoinActivityRewardInfo				//!< 3693 角色参与活动奖励信息
	{
	public:
		SCRoleJoinActivityRewardInfo();
		MessageHeader		header;

		int join_flag;							//!< 参与标记[按位] 
		int get_reward_flag;					//!< 已领取奖励标记[按位]
	};


	static const int ONE_KEY_FINISH_MAX_REWARDS_TYPE_NUM = 100;		// 一键完成最大奖励类型数量
	static const int ONE_KEY_FINISH_MAX_MONEY_TYPE_NUM = 2;			// 一键完成最大货币类型

	

	class SCRoleOneKeyFinishRewardInfo
	{
	public:
		SCRoleOneKeyFinishRewardInfo();
		MessageHeader header;

		struct ItemInfo
		{
			ItemInfo() :item_id(0), is_bind(0), num(0)
			{}

			ItemID item_id;
			short is_bind;
			int num;
		};

		int activity_types;
		unsigned int exp;
		int money[ONE_KEY_FINISH_MAX_MONEY_TYPE_NUM];		// 表示加的货币, [0]-铜币,[1]-灵玉
		int count;
		ItemInfo rewards[ONE_KEY_FINISH_MAX_REWARDS_TYPE_NUM];
	};


}
#pragma pack(pop)

#endif // __MSG_ROLE_JOIN_ACTIVITY_REWARD_HPP__

