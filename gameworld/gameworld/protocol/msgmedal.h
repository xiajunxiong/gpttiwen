#ifndef __MSG_MEDAL_HPP__
#define __MSG_MEDAL_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/medaldef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum MEDAL_REQ_TYPE
	{
		MEDAL_REQ_TYPE_INFO,					//!< 请求信息
		MEDAL_REQ_TYPE_PUT_ON,					//!< 穿戴装备 p1 grid_index,p2 column ,p3 index
		MEDAL_REQ_TYPE_TAKE_OFF,				//!< 脱下装备  p1 grid_index,
		MEDAL_REQ_TYPE_EXCHANGE_MATERIAL,		//!< 兑换材料 p1 seq
		MEDAL_REQ_TYPE_DECOMPOSE,				//!< 分解勋章 p1 column ,p2 index
		MEDAL_REQ_TYPE_LOTTREY,					//!< 抽奖 p1 seq p2 times
		MEDAL_REQ_TYPE_UPGRADE,					//!< 对背包进阶 p1 column,p2 index ,p3 consume_column,p4 consume_index
		MEDAL_REQ_TYPE_UPGRADE_2,				//!< 对穿戴进阶 p1 wear_index,		,p3 consume_column,p4 consume_index
		MEDAL_REQ_TYPE_SPE_EXCHANGE,			//!< 天赐令牌置换 p1:column p2:index p3:sp_id p4:second_sp_id p5:is_wear(0:背包 1:穿戴中(index))
	};

	class CSMedalReq					//!< 2160
	{
	public:
		CSMedalReq();
		MessageHeader		header;

		int req_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
	};

	class CSMedalUpGradeSpecialReq				//!< 2171
	{
	public:
		CSMedalUpGradeSpecialReq();
		MessageHeader		header;

		int is_wear_grid;//是否穿戴(0/1)
		int column;//穿戴不填
		int index;
		//天赐令牌
		int consume_column_spe;
		int consume_index_spe;
		//普通令牌
		int consume_column;
		int consume_index;
	};

	class SCLottreyInfo					//!< 2166 抽奖详细下发
	{
	public:
		SCLottreyInfo();
		MessageHeader		header;

		struct MedalReward
		{
			MedalReward():reward_bag_type(0), reward_bag_index(0) {}
			int reward_bag_type;
			int reward_bag_index;
		};
		int count;
		MedalReward reward_list[MEDAL_LOTTERY_TIMES_TYPE_10];//奖励信息
	};

	class SCMedalInfo : public IMessageSerializer 			//!<2161
	{
	public:
		SCMedalInfo();
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length);

		struct GridItem
		{
			GridItem();

			int grid_index;
			ItemID			item_id;		//!< 物品id
			short				num;			//!< 数量
			char				is_bind;		//!< 是否绑定
			char				has_param;		//!< 是否有净值参数
			UInt32			invalid_time;	//!< 过期时间
			short				param_length;	//!< 净值参数的数据长度 （当has_param为1时可读）
			NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
		};

		int count;
		GridItem grid_info[MAX_MEDAL_WEAR_GRID];
	};

	class SCMedalFbSceneInfo						//!< 水晶副本场景信息 2162
	{
	public:
		SCMedalFbSceneInfo();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<当前层数
		int is_has_monster;
	};

		
	class SCMedalFbKillBoss						//!< 勋章副本击杀boss 2163
	{
	public:
		SCMedalFbKillBoss();
		MessageHeader		header;

		int scene_id;
	};


	class SCMedalDecomposeNotice					//!< 勋章分解通知 2164
	{
	public:
		SCMedalDecomposeNotice();
		MessageHeader		header;

		int is_succ;
	};

	class CSMedalTaskPlayVideo	//!< 2165 勋章任务播放密笺动画
	{
	public:
		CSMedalTaskPlayVideo();
		MessageHeader header;

		int video_id;	//!< 动画ID
	};

	class SCMedalTakeOff	//!< 2167
	{
	public:
		SCMedalTakeOff();
		MessageHeader header;

		int out_column;
		int out_index;
	};

	class SCMedalUpgradeNotice	//!< 2168
	{
	public:
		SCMedalUpgradeNotice();
		MessageHeader header;

		short is_succ;
		short is_wear;
		int column;
		int index;
	};

	class SCMedalSpecialExchangeNotice		//!< 2169 天赐勋章置换通知
	{
	public:
		SCMedalSpecialExchangeNotice();
		MessageHeader header;

		short is_wear;			//1:穿戴中(index) 0:背包(column, index)
		short column;
		int index;
	};

	class CSMedalOneKeyGradeReq				//!< 2170
	{
	public:
		CSMedalOneKeyGradeReq();
		MessageHeader		header;

		short count;
		short column;
		BitMap<MEDAL_ONE_KEY_GRADE_MAXNUM> index_flag;	//MEDAL_ONE_KEY_GRADE_MAXNUM- 320 范围[0-299]
	};
}

#pragma pack(pop)

#endif // __MSG_BABY_HPP__

