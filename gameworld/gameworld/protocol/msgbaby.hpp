#ifndef __MSG_BABY_HPP__
#define __MSG_BABY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/babydef.hpp"
#include "servercommon/struct/global/babydataparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCBabyInfo				//!< 2060
	{
	public:
		SCBabyInfo();
		MessageHeader		header;

		unsigned short single_ji_fen;		//!<单人生产的积分
		unsigned short duet_ji_fen;		//!<双人生产的积分
		unsigned char draw_times;		//!< 单人求子 每天抽签次数
		unsigned char worship_times;		 //!< 单人求子 每天供奉次数		
		unsigned char duet_create_stage;	//!< 双人生产 阶段
		unsigned char add_jifen_times;		//!<小游戏是否能加积分，不等0就说明今天玩过

		short create_type;				//!<正在进行求子类型 1是单人，2是双人
		short reserved_sh;			

		BabyDataParam baby[MAX_BABY_NUM];		// baby 信息,大小2
	};

	class CSBabyReq				//!< 2061
	{
	public:
		enum BABY_REQ_TYPE
		{
			BABY_REQ_TYPE_INFO,					//!<信息
			BABY_REQ_TYPE_DRAW_LOTS,				//!< 抽签 
			BABY_REQ_TYPE_CONTRIBUTION,			//!< 贡献 p1 上供类型 [0~2 金币上供、魔币上供、钻石上供]
			BABY_REQ_TYPE_REMOVE,				//!< 遗弃 p1 baby的index
			BABY_REQ_TYPE_UP_LEVEL,				//!< 升级 p1 baby的index
			BABY_REQ_TYPE_PLAY_REQ,				//!< 询问对方开始小游戏（0，2阶段）
			BABY_REQ_TYPE_PLAY_RET,				//!< 回答是否开始小游戏 p1 是否接受，0是否
			BABY_REQ_TYPE_ADD_PLAY_JI_FEN,			//!< 添加小游戏积分 p1 要添加的积分
			BABY_REQ_TYPE_DUET_CREATE_REQ,		//!< 请求进行双人生产
			BABY_REQ_TYPE_DUET_CREATE_RET,			//!< 回答是否接受双人生产,p1 是否接受								
			BABY_REQ_TYPE_SELECT_SINGLE_CREATE,		//!<  选择单人生产模式
			BABY_REQ_TYPE_DUET_CREATE_STAGE_1,		//!< 选择双人生产  1阶段操作
			BABY_REQ_TYPE_GIVE_UP_CREATE,			//!< 放弃当前生产
		};

		CSBabyReq();
		MessageHeader		header;	

		int req_type;
		int param1;
	};

	class CSBabyRename						//!< 改名 2062
	{
	public:
		CSBabyRename();
		MessageHeader		header;

		int index;
		GameName name;
	};

	class SCBabyReqRoute				//!<  2063 询问请求转发
	{
	public:
		SCBabyReqRoute();
		MessageHeader		header;

		int req_type;				// 请求类型
	};

	class SCBabyStartPlay			//!<  2064 开始小游戏
	{
	public:
		SCBabyStartPlay();
		MessageHeader		header;

		unsigned int start_time;
	};

	class CSBabyActivte					//!< 激活子女 2065
	{
	public:
		enum REQ_TYPE
		{
			BABY_REQ_TYPE_DUET_ACTIVE,				//!< 激活双人生产（3阶段）
			BABY_REQ_TYPE_SINGLE_ACTIVE,			//!< 激活单人生产（积分够）
		};

		CSBabyActivte();
		MessageHeader		header;
		
		int req_type;
		GameName name;
	};
}

#pragma pack(pop)

#endif // __MSG_BABY_HPP__

