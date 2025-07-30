#ifndef __MSG_SHITU_HPP__
#define __MSG_SHITU_HPP__

#include "servercommon/shitudef.h"
#include "servercommon/userprotocal/msgheader.h"

#ifdef _MSC_VER
#include <type_traits>
#endif

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSShiTuMakeRelationReq					//!<2110			拜师或收徒 请求
	{
	public:
		CSShiTuMakeRelationReq();
		MessageHeader header;

		int type;			
		int uid;
	};

	class CSShiTuMakeRelationAck				//!<2111   回复拜师或收徒
	{
	public:
		CSShiTuMakeRelationAck();
		MessageHeader header;

		unsigned int type;

		int uid;
		int is_agree;
	};

	enum SHI_TU_BREAK_RELATION_REQ_TYPE
	{
		SHI_TU_BREAK_RELATION_REQ_TYPE_BREAK,			// 解除关系
		SHI_TU_BREAK_RELATION_REQ_TYPE_BREAK_FORCE,		// 强制解除关系
		SHI_TU_BREAK_RELATION_REQ_TYPE_BREAK_CANCEL,		// 冷静期中取消解除关系
	};

	class CSShiTuBreakRelationReq		//!<2112  毕业或出师 请求
	{
	public:
		CSShiTuBreakRelationReq();
		MessageHeader header;

		int req_type;
		int relation_type;
		int uid;
		int param1;	// 强制解除是否使用元宝
	};

	class SCShiTuMakeRelationRoute	//!<2113   拜师或收徒 请求转发
	{
	public:
		SCShiTuMakeRelationRoute();
		MessageHeader header;

		int type;						
	
		GameName name;
		int uid;
		int level;
		short sex;
		char prof;
		char reserve_ch;
		short avatar_type;
		short headshot_id;
	};

	class SCShiTuRelationInfo		//!<2114  师徒信息
	{
	public:
		SCShiTuRelationInfo();
		MessageHeader header;

		struct ShiTuItemInfo
		{
			ShiTuItemInfo()
			{				
#ifdef _MSC_VER
				using c = std::remove_reference<decltype(*this)>::type;
				static_assert(std::is_standard_layout<c>::value, "can't memset");
#endif

				memset(this, 0, sizeof(*this));
			}

			GuildName guild_name;
			GameName name;
			int uid;			
			int level;
			short sex;
			char hold_ch;
			char is_online;
			int prof;
			short avatar_type;
			short headshot_id;
			unsigned int shi_tu_time;	

			unsigned int login_time;	//上次登录时间

			int apprentice_old_cap;			//拜师时 徒弟的战力
			int apprentice_cap;				//当前徒弟战力
			int mile_reward_flag;			//里程奖励标志[序号]		//没用了，改成只能领取一次，不是每次师徒都能领取
			int relation_break_time_type;			// 1是强制解除，0是正常解除，对方不在线或拒绝
			unsigned int relation_break_time;		// 解除关系倒计时
		};

		ShiTuItemInfo master_info_list[SHITU_MAX_MASTER_COUNT];		//大小2
		int apprentice_count;
		ShiTuItemInfo apprentice_info_list[SHITU_MAX_APPRENTICE_COUNT]; //大小3
	};

	class SCShiTuBreakRelationRoute		//!< 2115   毕业或出师 请求转发
	{
	public:
		SCShiTuBreakRelationRoute();
		MessageHeader header;

		unsigned int type;

		GameName name;
		int uid;
		int level;
		short sex;
		char prof;
		char reserve_ch;
		long long	avatar_timestamp;
	};

	class CSShiTuBreakRelationAck			//!< 2116  回复 毕业或出师 
	{
	public:
		CSShiTuBreakRelationAck();
		MessageHeader header;

		unsigned int type;

		int uid;
		int is_agree;
	};

	enum SHI_TU_REQ_TYPE
	{
		SHI_TU_REQ_TYPE_RELATION_INFO,  //获取师徒信息（获取2114）
		SHI_TU_REQ_TYPE_INVITATION,	//邀请师徒副本
		SHI_TU_REQ_TYPE_FINISH_FB,		//结束副本	
	};

	class CSShiTuReq			//!< 2117		请求信息
	{
	public:
		CSShiTuReq();
		MessageHeader header;
		
		int req_type;
	};

	class SCShiTuFbSceneInfo				 //!< 2118		 场景怪物信息
	{
	public:
		SCShiTuFbSceneInfo();
		MessageHeader header;

		unsigned int end_time;
		int count;
		ShiTuFbMonsterInfoItem monster_info[100];  //数组
	};

	class SCShiTuFbCommonInfo				 //!< 2119  师徒其他信息
	{
	public:
		SCShiTuFbCommonInfo();
		MessageHeader header;

		int is_get_shi_tu_reward;		//是否已经获取过师徒副本奖励

		int day_mile_cap_fetch_num;			//今日战力理程 领取次数
		int day_mile_pass_day_fetch_num;	//今日拜师天数理程 领取次数

		int week_chuan_gong_flag;		//本周传功奖励标记
		int week_chuan_gong_exp_flag;	//本周传功经验标记
		int week_chuan_gong_score_max;	//本周传功最高得分
		long long mile_reward_flag;				//里程奖励标志[序号]
	};

	class SCShiTuFbInvitationNotice			//!< 2120 师徒副本组队邀请
	{
	public:
		SCShiTuFbInvitationNotice();
		MessageHeader		header;

		GameName name;
		int team_index;
		int uid;
		int is_cross;
	};

	class SCShiTuFbFinishNotice			//!< 2121 结束通知
	{
	public:
		SCShiTuFbFinishNotice();
		MessageHeader		header;

		int finish_group;
	};

	class SCShiTuFbBattleResult	//!< 2122 战斗通知
	{
	public:
		SCShiTuFbBattleResult();
		MessageHeader		header;

		int result;
		int group;
		int index;
	};

	class SCShiTuPreferencesInfo				 //!< 2124		
	{
	public:
		SCShiTuPreferencesInfo();
		MessageHeader header;

		char preferences[SHI_TU_PREFERENCES_NUM];		//4
	};

	class CSShiTuSeekingReq				//!< 2125			
	{
	public:
		enum SHI_TU_SEEKING_REQ_TYPE
		{
			SHI_TU_SEEKING_REQ_TYPE_INFO,			//请求信息， seeking_type;	//1 寻找徒弟， 0 寻找师傅
			SHI_TU_SEEKING_REQ_TYPE_INSERT,			//插入宣言 ， seeking_type;	//1 寻找徒弟， 0 寻找师傅
			SHI_TU_SEEKING_REQ_TYPE_REMOVE,		// 删除宣言
			SHI_TU_SEEKING_REQ_TYPE_PREFERENCES,		// 师徒偏好设置 preferences
		};

		CSShiTuSeekingReq();
		MessageHeader header;

		int req_type;		
		int seeking_type;	
		char preferences[SHI_TU_PREFERENCES_NUM];  //4

		ShiTuSeekingNotice seeking_notice;
	};

	struct ShiTuSeekingInfo
	{
		GuildName		guild_name;
		GameName		gamename;
		int				user_id;
		char			reserve_ch;
		signed char		sex;
		char			prof;
		char			is_online;
		short			level;
		short			headshot_id;
		unsigned int	timestamp;
		int				avatar_type;
		ShiTuSeekingNotice	seeking_notice;
	};

	class SCShiTuSeekingInfo				 //!< 2126		
	{
	public:
		SCShiTuSeekingInfo();
		MessageHeader header;

		ShiTuSeekingNotice	self_seeking;		//自己的宣言
		int seeking_type;
		int count;
		ShiTuSeekingInfo seeking_info[SHI_TU_SEEKING_MAX_COUNT];
	};


	//师徒传功
	//副本操作		4470
	class CSShiTuChuanGongFBReq
	{
	public:
		CSShiTuChuanGongFBReq();
		MessageHeader header;

		enum ReqType
		{
			REQ_TYPE_GAME_READY = 0,//就绪
			REQ_TYPE_GAME_INFO,		//游戏过程	p_1:基础得分  p_2: 加成后得分
			REQ_TYPE_GAME_END,		//(失效)结算游戏	p:结算积分
			REQ_TYPE_FETCH_MILE,	//领取里程奖励	p_1:目标uid p_2:领取序号
		};

		int req_type;
		int p_1;
		int p_2;
	};

	//游戏信息通知	4471
	class SCShiTuChuanGongGameInfo
	{
	public:
		SCShiTuChuanGongGameInfo();
		MessageHeader header;

		enum InfoType
		{
			INFO_TYPE_GAME_BEGIN = 0,//游戏开始准备	p:开始时间
			INFO_TYPE_GAME_SCORE,//对方游戏得分	p:对方基础得分
		};

		int type;
		unsigned int param;
	};

	//场景信息	4472
	class SCShiTuChuanGongSceneInfo
	{
	public:
		SCShiTuChuanGongSceneInfo();
		MessageHeader header;

		unsigned int fb_begin_time;//场景开启时间

		//角色信息
		struct RoleShowInfo
		{
			RoleShowInfo()
			{
				uid = 0;
				memset(role_name, 0, sizeof(role_name));
				appearance.Reset();
			}

			int uid;
			GameName role_name;		//角色名
			Appearance appearance;	//外观
		};
		//师徒信息
		int count;
		RoleShowInfo host_list[SHI_TU_SEEKING_TYPE_MAX];
	};

	//师徒通知  4473
	class SCShiTuSyncInfo
	{
	public:
		SCShiTuSyncInfo();
		MessageHeader header;

		enum SyncType
		{
			ROLE_ONLINE_DEF,	//师傅在线通知 初始化	p_1:师傅uid p_2:0全部未完成，1师徒副本未完成，2传功未完成
			ROLE_ONLINE_UPDATA,	//师傅在线通知 更新					||||按位写入 0:传功 1:师徒副本
		};

		int type;
		int param_1;
		int param_2;
	};

	enum SHI_TU_NOTICE_TYPE
	{
		SHI_TU_OP_NOTICE_TYPE_FORCE_BREAK,  //!<强制解除冷却期 p1 uid
		SHI_TU_OP_NOTICE_TYPE_FORCE_BREAK_CANCEL,  //!<取消强制解除冷却期 p1 uid
	};

	class SCShiTuOpNotice			//!< 5190
	{
	public:
		SCShiTuOpNotice();
		MessageHeader		header;
		
		int notice_type;
		int p1;
	};
}

#pragma pack(pop)

#endif