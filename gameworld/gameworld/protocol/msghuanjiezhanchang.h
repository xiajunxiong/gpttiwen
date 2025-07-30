#ifndef __MSG_HUAN_JIE_ZHAN_CHANG_HPP__
#define __MSG_HUAN_JIE_ZHAN_CHANG_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "item/itembase.h"
#include "gameworld/equipment/jewelry.h"
#include "servercommon/fabaodef.hpp"
#include "other/attributelist.hpp"
#include "servercommon/huashendef.hpp"
#include "servercommon/rolehuanjiedef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum HUAN_JIE_ZHAN_CHANG_REQ_TYPE
	{
		HUAN_JIE_ZHAN_CHANG_REQ_TYPE_MATCH,		//匹配
		HUAN_JIE_ZHAN_CHANG_REQ_TYPE_CANCEL_MATCH,		//取消匹配
		HUAN_JIE_ZHAN_CHANG_REQ_TYPE_RANK_INFO,		//排行榜信息 p1 server_id(哪个服排行榜，0时跨服)
		HUAN_JIE_ZHAN_CHANG_REQ_QUERY_OTHER_INFO,		//查询其他玩家
		HUAN_JIE_ZHAN_CHANG_REQ_INFO,		// 自己信息
		HUAN_JIE_ZHAN_CHANG_PROF_WIN_LOSE_INFO,		//    请求与各职业对战胜率信息（下发3309）
		HUAN_JIE_ZHAN_CHANG_LEADER_VIEW,		//    队长界面 p1 :0 是关闭，1是打开
		HUAN_JIE_ZHAN_CHANG_FETCH_TODAY_REWARD,		//    领取今日胜出宝箱 p1 ：配置seq
	};

	class CSHuanJieZhanChangReq			//!< 3290
	{
	public:
		CSHuanJieZhanChangReq();
		MessageHeader header;

		int req_type;
		int param1;
	};


	class SCHuanJieZhanChangTeamInfo			//!< 3291
	{
	public:
		SCHuanJieZhanChangTeamInfo();
		MessageHeader header;

		int is_open;
	};

	enum HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE
	{
		HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_HUA_SHEN,			//没初始化化身
		HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_ZHAN_LI,			//暂离
		HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_CANN_FIGHT,		//处于无法战斗状态
		HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_IN_MATCHING,		//处于匹配状态
	};


	class SCHuanJieZhanChangErrReturn                                    //!< 3292
	{
	public:
		SCHuanJieZhanChangErrReturn();
		MessageHeader header;

		int role_id;
		int reson;
	};

	enum HUAN_JIE_ZHAN_CHANG_NOTICE_TYPE
	{
		HUAN_JIE_ZHAN_CHANG_NOTICE_TYPE_,
	};

	class SCHuanJieZhanChangNotice //!< 3292
	{
	public:
		SCHuanJieZhanChangNotice();
		MessageHeader header;

		int is_open;
	};

	class SCHuanJieZhanChangMatchState		//!< 		3301  匹配状态
	{
	public:
		SCHuanJieZhanChangMatchState();
		MessageHeader header;
		
		int match_state;
	};

	class SCHuanJieZhanChangResult   //!< 		3302  战斗结果
	{
	public:
		SCHuanJieZhanChangResult();
		MessageHeader header;

		int result;				//结果
		int rank_score;			//当前分
		int win_streak_times;   //连胜次数
		int is_finish_ding_ji;	//是否完成定级赛
	};

	struct HuanJieZhanChangOpponentInfoItem
	{
		int role_id;
		GameName role_name;
		int prof;
		int score;
		short avatar_type;
		short headshot_id;
		int is_leader;
		int is_finis_ding_ji;
	};

	class SCHuanJieZhanChangOpponentInfo		//!< 		3303  对手信息
	{
	public:
		SCHuanJieZhanChangOpponentInfo();
		MessageHeader header;

		unsigned int start_time;			//开打时间戳
		HuanJieZhanChangOpponentInfoItem opponent_info_item1[3];
		HuanJieZhanChangOpponentInfoItem opponent_info_item2[3];
	};

	class SCHuanJieZhanChangRoleInfo   //!< 3304  角色信息
	{
	public:
		SCHuanJieZhanChangRoleInfo();
		MessageHeader header;

		int season_num;		//赛季
		unsigned int end_time;
		int rank_score;			//当前分
		int max_rank_score;		//当前赛季达到最高排位积分
		int win_streak_times;   //连胜次数
		unsigned int total_times;		//总场数
		unsigned int win_times;		//胜利次数
		unsigned int draw_times;		//平手次数
		int is_finish_ding_ji;	//是否完成定级赛
		int today_get_huan_jie_coin;	//获得幻界币数量
		int today_win_times;	//今日胜场
		int today_win_reward_flag;	//今日胜场奖励领取标志
	};

	class SCHuanJieZhanChangRankInfo   //!< 3305  排行榜信息
	{
	public:
		SCHuanJieZhanChangRankInfo();
		MessageHeader header;

		int server_id;
		int count;
		ActivityHuanJieZhanChangRankItemCommon rank_list[ACTIVITY_HUAN_JIE_ZHAN_CHANG_RANK_LIST_MAX_NUM];
	};

	class CSHuanJieZhanChangRoleRankListReq			//!< 3306  查询玩家排行榜排名
	{
	public:
		CSHuanJieZhanChangRoleRankListReq();
		MessageHeader header;

		int role_id[100];						//不用补齐100，有多少就多少
	};

	class SCHuanJieZhanChangRoleRankListing   //!< 3307  查询玩家排行榜排名 返回
	{
	public:
		struct RoleInfoItem
		{
			RoleInfoItem()
			{
				role_id = 0;
				score = 0;
				rank_pos = 0;
				cross_rank_pos = 0;
				join_time = 0;
				is_finish_ding_ji = 0;
			}

			int role_id;				
			int score;				//积分
			int rank_pos;			//本服排名
			int cross_rank_pos;		//跨服排名
			unsigned int join_time;		// 达到分数时间
			int is_finish_ding_ji;
		};

		SCHuanJieZhanChangRoleRankListing();
		MessageHeader header;

		int count;
		RoleInfoItem role_infos[100];
	};

	enum SC_HUANJIE_ZHAN_CHANG_TIAN_TI_RECORD_TYPE
	{
		SC_HUANJIE_ZHAN_CHANG_TIAN_TI_RECORD_TYPE_ALL,
		SC_HUANJIE_ZHAN_CHANG_TIAN_TI_RECORD_TYPE_ADD,
	};

	class SCHuanJieZhanChangTianTiRecord   //!< 3308  对战记录
	{
	public:

		SCHuanJieZhanChangTianTiRecord();
		MessageHeader header;

		int type;
		int count;
		CommonSaveHuanJieTianTiReceordData record[HUAN_JIE_TIAN_TI_RECORD_MAX_NUM];
	};


	class SCHuaShenRoleInfoAck			//!< 3310 化身基本信息下发
	{
	public:
		SCHuaShenRoleInfoAck();
		MessageHeader		header;

		int role_level;			//!< 化身等级
		Appearance appearance;	//!< 外观
	};

	class SCHuaShenRoleAttrList			//!< 3311 化身属性列表
	{
	public:
		SCHuaShenRoleAttrList();
		MessageHeader header;

		struct AttrPair
		{
			int attr_type;							//!< 属性类型号 \see enum BATTLE_ATTR
			int attr_value;							//!< 属性的值
		};

		enum NOTIFY_REASON
		{
			NOTIFY_REASON_NORMAL = 0,
			NOTIFY_REASON_SYNC_HPMP_IN_FB = 1,
		};

		short attr_count;							//!< 下发的属性条数
		short notify_reason;						//!< 下发的理由 \see NOTIFY_REASON
		int capability;								//!< 玩家战力(评分)
		AttrPair attr_list[BATTLE_ATTR_MAX];		//!< 具体数量由attr_count决定  \see BATTLE_ATTR_MAX 50
	};


	class CSHuaShenMedalReq					//!< 3312 化身勋章请求
	{
	public:
		CSHuaShenMedalReq();
		MessageHeader		header;

		enum HUASHEN_MEDAL_REQ_TYPE
		{
			HUASHEN_MEDAL_REQ_TYPE_INFO,			//!< 请求信息
			HUASHEN_MEDAL_REQ_TYPE_PUT_ON,			//!< 穿戴装备 p1 grid_index  p2 index
			HUASHEN_MEDAL_REQ_TYPE_TAKE_OFF,		//!< 脱下装备 p1 grid_index
		};

		int req_type;
		int param1;
		int param2;
	};

	class SCHuaShenMedalInfo : public IMessageSerializer	//!<3313 下发此时正在穿戴的勋章
	{
	public:
		SCHuaShenMedalInfo();
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count && i < ARRAY_LENGTH(grid_info); ++i)
			{
				const GridItem & grid = grid_info[i];

				result = result && WriteInt(grid.grid_index, buffer, buffer_size, out_length);
				result = result && WriteUShort(grid.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(grid.num, buffer, buffer_size, out_length);
				result = result && WriteChar(grid.is_bind, buffer, buffer_size, out_length);
				result = result && WriteChar(grid.has_param, buffer, buffer_size, out_length);
				result = result && WriteUInt(grid.invalid_time, buffer, buffer_size, out_length);
				if (0 != grid.has_param)
				{
					result = result && WriteShort(grid.param_length, buffer, buffer_size, out_length);
					result = result && WriteStrN(grid.param_data, grid.param_length, buffer, buffer_size, out_length);
				}
			}
			return result;
		}

		struct GridItem
		{
			GridItem()
			{
				grid_index = 0;
				item_id = 0;		//!< 物品id
				num = 0;			//!< 数量
				is_bind = 0;		//!< 是否绑定
				has_param = 0;		//!< 是否有净值参数
				invalid_time = 0;	//!< 过期时间
				param_length = 0;	//!< 净值参数的数据长度 （当has_param为1时可读）
				memset(param_data, 0, sizeof(param_data));
			}

			int					grid_index;
			ItemID				item_id;		//!< 物品id
			short				num;			//!< 数量
			char				is_bind;		//!< 是否绑定
			char				has_param;		//!< 是否有净值参数
			UInt32				invalid_time;	//!< 过期时间
			short				param_length;	//!< 净值参数的数据长度 （当has_param为1时可读）
			NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
		};

		int count;
		GridItem grid_info[MAX_MEDAL_WEAR_GRID];
	};

	class CSHuaShenEquipmentOperaReq						//!< 3314 化身装备操作请求
	{
	public:
		CSHuaShenEquipmentOperaReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_INFO_REQ = 0,			//!< 0 请求信息
			OPERA_TYPE_PUTON = 1,				//!< 1 穿戴装备 \param param1 0  \param param2 背包格子序号  \param param3 穿戴的部位 \see EQUIPMENT_INDEX \ param param4 role_type 
			OPERA_TYPE_TAKEOFF = 2,				//!< 2 脱下装备 \param param1 要脱下的部位 \see EQUIPMENT_INDEX \ param2 role_type
			OPERA_TYPE_TAKEOFF_CRYSTAL = 3,		//!< 3 脱下元素水晶
			OPERA_TYPE_PUTON_CRYSTAL = 4,		//!< 5 装备元素水晶 \param param1 化身背包格子序号
			OPERA_TYPE_INORDER = 5,				//!< 6 整理背包
		};

		int msg_type;							//!< 消息类型  \see OPERA_TYPE
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class SCHuaShenEquipmentInfoAck					//!< 3315 化身装备信息
	{
	public:
		SCHuaShenEquipmentInfoAck();
		MessageHeader header;

		ItemID element_crystal_id;				//!< 元素水晶的物品ID
		short used_times;						//!< 元素水晶已使用次数

		//化身装备
		short reserve_short;
		short huashen_count;
		EquipmentPro huashen_equipment_list[ItemNamespace::MAX_EQUIPMENT_GRID_NUM];	//化身装备 MAX_EQUIPMENT_GRID_NUM 5
	};

	class SCHuaShenEquipmentGridChange				//!< 3316 化身装备改变通知
	{
	public:
		SCHuaShenEquipmentGridChange();
		MessageHeader header;

		EquipmentPro equip_info;
	};

	class CSHuaShenJewelryHandleReq					//!< 3317： 灵饰 系统操作请求
	{
	public:
		CSHuaShenJewelryHandleReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_ALL_INFO_REQ = 0,		//!< 请求所有 灵饰 信息
			OPERA_TYPE_WEAR_JEWELRY = 1,		//!< 请求装备灵饰
			OPERA_TYPE_TAKE_OFF_ONE = 2,		//!< 请求脱下灵饰
		};

		short op_type;		//!< 操作类型

		short param;		//!< 额外参数：0 - 无； 1 - 背包格子ID； 2 - 灵饰装备格ID；
	};

	class SCHuaShenJewelryAllInform					//!< 3318： 装备灵饰信息发送
	{
	public:
		SCHuaShenJewelryAllInform();
		MessageHeader header;

		int jewelry_count;		//!< 灵饰 总数
		JewelryPro jewelry_list[Jewelry::J_TYPE_MAX];	// J_TYPE_MAX 3
	};

	class SCHuaShenJewelryInfoChange					//!< 3319： 单个灵饰信息发送
	{
	public:
		SCHuaShenJewelryInfoChange();
		MessageHeader header;

		JewelryPro jewelry_info;
	};

	class CSHuaShenAttributePlanPointReq				//!< 3320 化身潜能操作
	{
	public:
		CSHuaShenAttributePlanPointReq();
		MessageHeader header;

		enum ATTRIBUTE_PLAN_TYPE
		{
			ATTRIBUTE_PLAN_TYPE_CLEAR_POINT = 0,	//洗点
			ATTRIBUTE_PLAN_TYPE_ADD_POINT = 1,		//加点
			ATTRIBUTE_PLAN_TYPE_SWITCH = 2,			//切换方案
		};

		short op_type;
		short delta_attr_list[ADD_POINT_TYPE_MAX];	//!< 改变的点数， 可为负数  \see ADD_POINT_TYPE_MAX 5
		int cur_plan_index;							//!< 选中的方案序号
	};

	struct PlanDetail
	{
		char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< 计划名  \see ATTRIBUTE_PLAN_NAME_LENGTH
		short remain_point;							//!< 剩余点数
		short add_point_list[ADD_POINT_TYPE_MAX];   //!< 加点的数值 \see ADD_POINT_TYPE_MAX
	};

	class SCHuaShenRoleAttributePlanInfo				//!< 3321 化身加点信息
	{
	public:
		SCHuaShenRoleAttributePlanInfo();
		MessageHeader header;

		int cur_plan_index;							//!< 该方案序号 [0,3)
		PlanDetail attr_plan_info;					//!< 该方案信息
	};

	class CSHuaShenPetOp						//!< 3322 化身宠物操作
	{
	public:
		CSHuaShenPetOp();
		MessageHeader		header;

		enum OP_TYPE
		{
			SetPetState = 0,				//!< 设置宠物状态		将index为p1的宠物设置成p2[1:出战 2:跟随 3:待机1 4:待机2 5:待机3]等状态
			RefreshPoints = 1,				//!< 洗点			给宠物index为p1的宠物洗点
			PetAttrsReq = 2,				//!< 请求宠物属性		请求index为p1的宠物的属性
			PetEquipmentPutOn = 3,			//!< 宠物装备穿戴 p1 宠物index p2 背包格子
			PetEquipmentPutOff = 4,			//!< 宠物装备穿戴 p1 宠物index
		};

		int op_type;
		int p1;
		int p2;
	};

	class CSHuaShenPetAssignAttributePoints		//!< 3323 化身请求分配宠物属性点 
	{
	public:
		CSHuaShenPetAssignAttributePoints();
		MessageHeader		header;

		int index;
		int con;
		int str;
		int def;
		int agi;
		int magic;
	};

	class SCHuaShenSendPetAllInfo				//!< 3324 宠物所有的信息 
	{
	public:
		SCHuaShenSendPetAllInfo();
		MessageHeader		header;

		enum HUASHEN_PET_ALL_INFO_REASON
		{
			HUASHEN_PET_ALL_INFO_REASON_DEFAULT = 0,		//默认
			HUASHEN_PET_ALL_INFO_REASON_TRAIN_SKILL,	//修炼技能
		};

		int reason;
		int pet_count;		//!< 根据这个参数读后面的list个数
		PetPro pet_list[ItemNamespace::MAX_HUASHEN_PET_GRID_NUM];	//MAX_HUASHEN_PET_GRID_NUM 100
	};
	
	class SCHuaShenSendPetSingleInfo				//!< 3325 单个宠物的信息
	{
	public:
		enum PET_NOTIE_TYPE
		{
			PET_NOTIE_TYPE_NO_TYPE = 0,			//!< 无标记下发
			PET_NOTIE_TYPE_ASSIGN_POINTS,		//!< 属性点
			PET_NOTIE_TYPE_STATUS,				//!< 宠物状态变更
			PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE,//!< 宠物装备穿脱变更

		};
		SCHuaShenSendPetSingleInfo();
		MessageHeader		header;

		int notie_type;		
		PetPro pet_info;
	};

	class SCHuaShenSendPetOtherInfo			//!< 3326 宠物其他信息
	{
	public:
		SCHuaShenSendPetOtherInfo();
		MessageHeader		header;

		short		fight_index;		//!< 出战宠物index
		short		standby_index1;		//!< 待机宠物index
		short		standby_index2;		//!< 待机宠物index
		short		standby_index3;		//!< 待机宠物index
		short		standby_index4;		//!< 待机宠物index
		short		standby_index5;		//!< 待机宠物index
	};

	class SCHuaShenSendPetAttrListInfo			//!< 3327 宠物属性
	{
	public:
		SCHuaShenSendPetAttrListInfo();
		MessageHeader		header;

		int index;
		int capability;
		AttributeList attr_list;
	};

	class CSHuaShenInfoReq						//!< 3328 化身模块信息请求
	{
	public:
		CSHuaShenInfoReq();
		MessageHeader		header;

		enum HUASHEN_INFO_TYPE
		{
			HUASHEN_INFO_TYPE_ALL_INFO,			//所有信息
			HUASHEN_INFO_TYPE_RENWU,			//化身人物信息
			HUASHEN_INFO_TYPE_POINT,			//化身当前方案信息
			HUASHEN_INFO_TYPE_KNAPSACK,			//背包信息
			HUASHEN_INFO_TYPE_PET,				//所有宠物
			HUASHEN_INFO_TYPE_PET_POINT,		//宠物加点信息  p1 : pet_index
			HUASHEN_INFO_TYPE_FIRST_ENTER,		//打开幻界战场界面请求，这边会同步信息
			HUASHEN_INFO_TYPE_FINISH_GUIDE,		//完成化身指引
		};
			
		int op_type;
		int param1;
	};

	class SCHuaShenKnapsackInfoAck : public IMessageSerializer  //!< 3329 背包信息返回
	{
	public:
		enum HUASHEN_SEND_REASON
		{
			HUASHEN_SEND_REASON_NORMAL = 0,			//!< 0 客户端请求所有信息
			HUASHEN_SEND_REASON_IN_ORDER = 1,		//!< 1 整理
			HUASHEN_SEND_REASON_REQ = 2,			//!< 2 客户端单独请求背包信息
		};

	public:
		SCHuaShenKnapsackInfoAck();
		virtual ~SCHuaShenKnapsackInfoAck() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(valid_grid_num, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);

			for (int i = 0; i < item_count && i < ItemNamespace::HUASHEN_ITEM_GRID_NUM_MAX; ++i)
			{
				result = result && WriteUShort(item_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].column, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_market_buy_flag, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].num, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_bind, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].has_param, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);

				if (1 == item_list[i].has_param)
				{
					result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
					result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);
				}
			}

			return result;
		}

		int			valid_grid_num;											//!< 栏位有效格子数列表 \see ITEM_COLUMN_TYPE_MAX
		short		reason;													//!< 下发理由
		short		item_count;												//!< 物品数量
		InfoType	item_list[ItemNamespace::HUASHEN_ITEM_GRID_NUM_MAX];	//!< 物品列表  HUASHEN_ITEM_GRID_NUM_MAX 400
	};


	class SCHuaShenKnapsackItemChange : public IMessageSerializer  //!< 3330 单个物品改变通知
	{
	public:
		SCHuaShenKnapsackItemChange();
		virtual ~SCHuaShenKnapsackItemChange() {}
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(put_reason, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(column, buffer, buffer_size, out_length);
			result = result && WriteShort(index, buffer, buffer_size, out_length);
			result = result && WriteChar(is_market_buy_flag, buffer, buffer_size, out_length);
			result = result && WriteInt(num, buffer, buffer_size, out_length);
			result = result && WriteChar(is_bind, buffer, buffer_size, out_length);
			result = result && WriteChar(has_param, buffer, buffer_size, out_length);
			result = result && WriteUInt(invalid_time, buffer, buffer_size, out_length);
			result = result && WriteLL(item_unique_id, buffer, buffer_size, out_length);
			if (1 == has_param)
			{
				result = result && WriteShort(param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(param_data, param_length, buffer, buffer_size, out_length);
			}

			return result;
		}

		short				reason;					//!< 改变的原因
		short				put_reason;				//!< 获得物品的来源  \see PUT_REASON_TYPE
		ItemID				item_id;				//!< 物品id
		short				column;					//!< 物品在哪个背包栏中 [0,9)
		short				index;					//!< 物品在背包中的序号 
		short				num;					//!< 数量
		long long           item_unique_id;			//!< 道具唯一ID
		char				is_market_buy_flag;		//!< 是否从市场购买标识 0:否 1:从市场购买
		char				reserve_ch;
		char				is_bind;				//!< 是否绑定
		char				has_param;				//!< 是否有净值参数
		UInt32				invalid_time;			//!< 过期时间
		short				param_length;			//!< 净值参数的数据长度 （当has_param为1时可读）
		NetValueItemParamData param_data;			//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
	};

	class CSSetHuaShenBattlePosition		//!< 3331 设置主角站位   
	{
	public:
		CSSetHuaShenBattlePosition();
		MessageHeader header;

		int position;				//!< 主角站位  （0：后排   1：前排）
	};

	class SCSetHuaShenBattlePosition		//!< 3332 设置主角站位返回 
	{
	public:
		SCSetHuaShenBattlePosition();
		MessageHeader header;

		int position;				//!< 主角站位  （0：后排   1：前排）
	};

	class SCHuaShenSeasonInfo			//!< 3333 下发玩家本赛季信息
	{
	public:
		SCHuaShenSeasonInfo();
		MessageHeader header;

		char season_first_enter;			// 0:本赛季还未进入   1:本赛季已经进入过 
		char is_finish_guide;				// 是否完成指引 0:未完成 1:已完成
		short season_reset_first_dijing;	// 0:赛季重置还未进入 1:赛季重置已进入(定级赛提示)
	};

	class SCHuaShenElementCrystalChangeNotice			//!< 3334 化身元素水晶变更通知
	{
	public:
		SCHuaShenElementCrystalChangeNotice();
		MessageHeader header;

		ItemID element_crystal_id;				//!< 元素水晶的物品ID
		short used_times;						//!< 元素水晶已使用次数
	};

	class SCHuaShenAttrPlanListInfo						//!< 3335 化身方案加点信息
	{
	public:
		SCHuaShenAttrPlanListInfo();
		MessageHeader header;

		int cur_plan_index;									// 当前使用的方案序号 [0, 3)
		PlanDetail plan_list[MAX_HUASHEN_ATTR_PLAN_NUM];	// 方案列表 固定三套
	};

	class CSHuaShenAttributePlanChangeName					//!< 3336 方案改名
	{
	public:
		CSHuaShenAttributePlanChangeName();
		MessageHeader header;

		int plan_index;								//!< 要改的方案序号[0,3)
		char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< 新方案名，\see ATTRIBUTE_PLAN_NAME_LENGTH
	};

	class SCHuaShenMedalTakeOffInfo					//!< 3337 化身勋章脱下后该勋章位置下发
	{
	public:
		SCHuaShenMedalTakeOffInfo();
		MessageHeader header;

		int out_index;				//背包栏下标
	};

	/*class CSHuaShenFabaoReq				//!< 3328 请求化身法宝操作
	{
	public:
		enum FABAO_REQ_TYPE
		{
			FABAO_REQ_TYPE_INFO = 0,			//!< 0 穿戴信息信息
			FABAO_REQ_TYPE_TAKEUP = 1,			//!< 1 穿戴 p1 法宝装备栏对应的位置[0-3] p2 法宝背包对应的位置[0-17]
			FABAO_REQ_TYPE_TAKEOFF = 2,			//!< 2 脱下 p1 法宝装备栏对应的位置[0-3]
		};

		CSHuaShenFabaoReq();
		MessageHeader		header;

		int req_type;
		int param1;
		int param2;
	};

	class SCHuaShenFabaoEquipBarInfo			//!< 3329  化身法宝装备栏变更
	{
	public:
		SCHuaShenFabaoEquipBarInfo();
		MessageHeader		header;

		int m_fabao_equipment_bar[MAX_FABAO_EQUIPMENT_BAR];	//!< 法宝装备栏-对应装备的法宝index
	};*/

}

#pragma pack(pop)

#endif