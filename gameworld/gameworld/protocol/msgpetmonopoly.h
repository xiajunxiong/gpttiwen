#ifndef __MSG_PET_MONOPOLY_H__
#define __MSG_PET_MONOPOLY_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/petmonopolydef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct MsgPetMonopolyGrid
	{
		short reward_type;	//!< 格子奖励类型  \see PET_MONOPOLY_GRID_REWARD
		short grid_type;	//!< 格子类型	\see PET_MONOPOLY_GRID_TYPE
		int param1;			//!< 当格子是宠物时，表示宠物ID；为格子是物品或碎片时，表示物品或碎片ID; 当格子是机会点时，表示特殊物品id
		int param2;			//!< 当格子是物品或碎片时，表示物品或碎片数量; 当格子是机会点时，表示特殊物品数量
	};

	class SCPetMonopolyInfo				//!< 3660 灵宠秘境信息下发
	{
	public:
		SCPetMonopolyInfo();
		MessageHeader header;

		int expect_silver_pet_id;		//!< 银卡祈福  0表示没有选择银卡
		int expect_gold_pet_id;			//!< 金卡祈福  0表示没有选择金卡
		short cur_level;				//!< 当前层数
		short cur_grid_idx;				//!< 当前格子  [0,36)

		MsgPetMonopolyGrid grid_list[PET_MONOPOLY_GRID_NUM]; //!< 棋盘，共36个格子
	};

	class CSPetMonopolyOperate				//!< 3661 灵宠秘境操作请求
	{
	public:
		CSPetMonopolyOperate();
		MessageHeader header;

		enum OPER_TYPE
		{
			OPER_TYPE_INFO = 0,				//!< 请求棋盘信息
			OPER_TYPE_REWARD_RECORD = 1,	//!< 查看奖励
			OPER_TYPE_SET_EXPECT_PET = 2,	//!< 设置祈福宠物 param1 宠物ID
			OPER_TYPE_CLEAR_EXPECT_PET = 3,	//!< 清除祈福宠物 param1 是否金宠(1:是，0:否）
			OPER_TYPE_ROLL_DICE = 4,		//!< 摇骰子
			OPER_TYPE_FETCH_GRID_REWARD = 5,//!< 获取当前格子的奖励
			OPER_TYPE_START_PET_BATTLE = 6, //!< 开始抓宠战斗
			OPER_TYPE_TURN_TABLE = 7,		//!< 开始转盘
			OPER_TYPE_ENTER_NEXT_LEVEL = 8, //!< 进入下一层
			OPER_TYPE_SHOW_TRUNTABLE_REWARDS = 9,	//!< 查看转盘奖励
			OPER_TYPE_TURN_TABLE_END = 10,	//!< 转盘动画结束
			OPER_TYPE_ADD_EXP = 11,			//!< 遇到怪物组时,抵达格子回调加经验
			OPER_TYPE_SPECIAL_ITEM = 12,	//!< 使用特殊道具 param1 道具类型 param2 传入玩家选择的鲁班骰点数(1-6)或者心愿卡选择的物品的seq
			OPER_TYPE_SPECIAL_ITEM_NUM = 13,//!< 获取全部特殊道具数量
			OPER_TYPE_FAST_MOVE_REWARD =14,	//!< 使用神行符申请奖励 param1 神行移动后每格的index
		};

		short operate_type;
		short reserve_sh;
		int param1;
		int param2;
	};

	class SCPetMonopolyExpectPetChangeNotice	//!< 3662 灵宠秘境祈福返回
	{
	public:
		SCPetMonopolyExpectPetChangeNotice();
		MessageHeader header;

		int silver_expect_pet_id;
		int gold_expect_pet_id;
	};

	class SCPetMonopolyRollDice		//!< 3663 灵宠秘境投骰子结果返回
	{
	public:
		SCPetMonopolyRollDice();
		MessageHeader header;

		short dice;					//!< 摇骰子结果[1,6];  负数代表使用了特殊道具
		short cur_grid_idx;			//!< 新的格子位置
	};

	class SCPetMonopolyCurGridChangeNotice	//!< 3664 当前格点内容改变通知
	{
	public:
		SCPetMonopolyCurGridChangeNotice();
		MessageHeader header;

		int cur_grid_idx;					//!< 当前格点序号
		MsgPetMonopolyGrid grid;			//!< 格点内容
	};

	class SCPetMonopolyRewardRecord	: public IMessageSerializer	//!< 3665 灵宠秘境查看奖励返回
	{
	public:
		SCPetMonopolyRewardRecord();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT(reward_num);
			for (int i = 0; i < reward_num; ++i)
			{
				result = result && WRITE_INT(reward_list[i].item_id);
				result = result && WRITE_INT(reward_list[i].item_num);
				result = result && WRITE_SHORT(reward_list[i].is_bind);
				result = result && WRITE_SHORT(reward_list[i].param_length);
				if (reward_list[i].param_length > 0)
				{
					result = result && WRITE_STRN(reward_list[i].data, reward_list[i].param_length);
				}
				
				if (!result)
				{
					break;
				}
			}

			return result;
		}

		struct ItemInfo
		{
			int item_id;				//!< 物品ID 
			int item_num;				//!< 物品数量
			short is_bind;				//!< 是否绑定 1绑定  0非绑定
			short param_length;			//!< param长度
			NetValueItemParamData data;	//!< param数据
		};

		int reward_num;
		ItemInfo reward_list[PET_MONOPOLY_REWARD_RECORD_NUM];
	};

	class SCPetMonopolyEnterNextLevelNotice		//!< 3666 进入下一层通知 （随后会发3660）
	{
	public:
		SCPetMonopolyEnterNextLevelNotice();
		MessageHeader header;

		int next_level;
	};

	struct MsgPetMonopolyTurntableItem	
	{
		short reward_type;	//!< 格子奖励类型  \see PET_MONOPOLY_GRID_REWARD
		short reserve_sh;	//!< 保留字

		int param1;			//!< 当格子是宠物时，表示宠物ID；为格子是物品或碎片时，表示物品或碎片ID
		int param2;			//!< 当格子是物品或碎片时，表示物品或碎片数量
	};

	class SCPetMonopolyTurntable	//!< 3667 转盘下发
	{
	public:
		SCPetMonopolyTurntable();
		MessageHeader header;

		MsgPetMonopolyTurntableItem sector_list[PET_MONOPOLY_TURNTABLE_SECTOR_NUM]; //!< 扇区列表 一共8个扇区
	};

	class SCPetMonopolyTurntableResult	//!< 3668 转盘结果下发
	{
	public:
		SCPetMonopolyTurntableResult();
		MessageHeader header;

		int sector_idx;				//!< 指向的扇区idx
	};

	class SCPetMonopolySpecialItem		//!< 3669 特殊道具数量信息下发
	{
	public:
		SCPetMonopolySpecialItem();
		MessageHeader header;

		int double_surplus;
		int use_item_type;		//-1表示未使用道具
		int special_item_num[PET_MONOPOLY_SPECIAL_ITEM_MAX];
	};

	class SCPetMonopolyWishCardGrid		//!< 3670 灵宠秘境心愿卡替换格子位置
	{
	public:
		SCPetMonopolyWishCardGrid();
		MessageHeader header;

		int replace_num;
		int grid_idx[PET_MONOPOLY_MAX_REPLACE_GRID_NUM];
		MsgPetMonopolyGrid item_grid[PET_MONOPOLY_MAX_REPLACE_GRID_NUM];
	};
}

#pragma pack(pop)

#endif