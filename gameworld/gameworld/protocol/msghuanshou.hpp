#ifndef __MSG_HUAN_SHOU_HPP__
#define __MSG_HUAN_SHOU_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/huanshoudef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/itemlistparam.h"

#pragma pack(push, 4) 

namespace Protocol
{

	class CSHuanShouReq			//!< 5360 幻兽请求
	{
	public:
		CSHuanShouReq();
		MessageHeader header;

		enum HUAN_SHOU_REQ_TYPE
		{
			HUAN_SHOU_REQ_TYPE_INFO = 0,				//!< 请求下发所有信息
			HUAN_SHOU_REQ_TYPE_OPEN_EGG = 1,			//!< 请求打开幻兽蛋 p1:bag_column p2:bag_index
			HUAN_SHOU_REQ_TYPE_STR_LEVEL = 2,			//!< 请求强化幻兽 p1:huanshou_id
			HUAN_SHOU_REQ_TYPE_LEARN_SKILL = 3,			//!< 请求学习技能 p1:huanshou_id p2:bag_index p3:lock_flag
			HUAN_SHOU_REQ_TYPE_SAVE_SKILL = 4,			//!< 请求保存技能 p1:huanshou_id p2:is_save(0:清除 非0:保存)
			HUAN_SHOU_REQ_TYPE_P_POINT_LEVEL_UP = 5,	//!< 请求学习潜能点位 p1:huanshou_id p2:point_index
			HUAN_SHOU_REQ_TYPE_LIAN_DAN = 6,			//!< 请求炼丹(潜能果)
			HUAN_SHOU_REQ_TYPE_FU_SHEN = 7,				//!< 请求附身 p1:huanshou_id
			HUAN_SHOU_REQ_TYPE_CANCEL_FU_SHEN = 8,		//!< 请求取消附身
			HUAN_SHOU_REQ_TYPE_ANNOUNCEMENT_INFO = 9,	//!< 请求公告信息
			HUAN_SHOU_REQ_TYPE_SINGLE_HUANSHOU = 10,	//!< 请求下发单个幻兽信息 p1.huanshou_id
			HUAN_SHOU_REQ_TYPE_USE_HUAN_SHOU_IMAGE = 11,//!< 请求是否使用幻兽形象 p1:(0.不使用 1.使用)
		};

		int op_type;
		int param1;
		int param2;
		int param3;
	};

	struct HuanShouSingleInfoPro
	{
		HuanShouSingleInfoPro() { this->Reset(); }
		HuanShouSingleInfoPro(const RoleHuanShouData & data)
		{
			huanshou_id = 0;
			str_level = data.str_level;
			new_skill_gird_idx = data.new_skill_gird_idx;
			new_skill_id = data.new_skill_id;
			point_level_count = 0;
			memcpy(skill_list, data.skill_list, sizeof(skill_list));
			memcpy(point_level_list, data.point_level_list, sizeof(point_level_list));
		}

		void Reset()
		{
			huanshou_id = 0;
			str_level = 0;
			new_skill_gird_idx = -1;
			new_skill_id = 0;
			reserve_ch = 0;
			point_level_count = 0;
			reserve_sh = 0;
			memset(skill_list, 0, sizeof(skill_list));
			memset(point_level_list, 0, sizeof(point_level_list));
		}

		bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteShort(huanshou_id, buffer, buffer_size, out_length);
			result = result && WriteUShort(str_level, buffer, buffer_size, out_length);
			result = result && WriteShort(new_skill_gird_idx, buffer, buffer_size, out_length);
			result = result && WriteUShort(new_skill_id, buffer, buffer_size, out_length);

			result = result && WriteStrN((const char *)skill_list, sizeof(skill_list), buffer, buffer_size, out_length);

			result = result && WriteUChar(point_level_count, buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)point_level_list, sizeof(point_level_list[0]) * point_level_count, buffer, buffer_size, out_length);

			return result;
		}

		short huanshou_id;
		unsigned short str_level;										//幻兽强化等级(激活则为1级)
		short new_skill_gird_idx;										//打书的数据暂存
		unsigned short new_skill_id;									//打书的数据暂存
		char reserve_ch;
		unsigned char point_level_count;								//幻兽潜能点位列表数量
		short reserve_sh;	
		unsigned short skill_list[MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM];	//幻兽打书技能列表
		unsigned char point_level_list[MAX_HUAN_SHOU_CUR_POTENTIAL_POINT_NUM];	//幻兽潜能点位等级列表
	};


	class SCHuanShouAllInfo : public IMessageSerializer		//!< 5361 所有幻兽信息下发
	{
	public:
		SCHuanShouAllInfo();
		virtual ~SCHuanShouAllInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			egg_count = GetMin(egg_count, (short)ARRAY_ITEM_COUNT(egg_cur_open_times_list));
			result = result && WriteShort(egg_count, buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)egg_cur_open_times_list, sizeof(egg_cur_open_times_list[0]) * egg_count, buffer, buffer_size, out_length);

			short count = GetMin(huanshou_count, (short)ARRAY_ITEM_COUNT(huanshou_list));
			result = result && WriteShort(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count; i++)
			{
				HuanShouSingleInfoPro & info = huanshou_list[i];
				result = result && info.Serialize(buffer, buffer_size, out_length);
			}

			return result;
		}

		short egg_count;
		short huanshou_count;
		int egg_cur_open_times_list[MAX_HUAN_SHOU_EGG_ID_NUM];	//幻兽保底次数		
		HuanShouSingleInfoPro huanshou_list[MAX_HUAN_SHOU_NUM];
	};

	class SCHuanShouSingleInfo	: public IMessageSerializer		//!< 5362 单个幻兽信息下发
	{
	public:
		SCHuanShouSingleInfo();
		virtual ~SCHuanShouSingleInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && huan_shou_info.Serialize(buffer, buffer_size, out_length);

			return result;
		}

		HuanShouSingleInfoPro huan_shou_info;
	};

	class SCHuanShouNoticeInfo		//!< 5363 幻兽信息通知下发
	{
	public:
		SCHuanShouNoticeInfo();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_ACTIVE = 0,				// 幻兽激活 p1.huanshou_id 
			NOTICE_TYPE_ACTIVE_CONVERT = 1,		// 幻兽激活转化 p1.huanshou_id
			NOTICE_TYPE_STR_LEVEL = 2,			// 幻兽强化(进化) p1.huanshou_id p2.str_level
			NOTICE_TYPE_LEARN_SKILL = 3,		// 幻兽学习技能 p1.huanshou_id p2.new_skill_grid_idx p3.new_skill_id
			NOTICE_TYPE_CLEAR_LEARN_SKILL = 4,	// 幻兽清除准备学习技能 p1.huanshou_id
			NOTICE_TYPE_SAVE_LEARN_SKILL = 5,	// 幻兽保存学习技能 p1.huanshou_id p2.new_skill_grid_idx p3.new_skill_id
			NOTICE_TYPE_LEARN_P_POINT = 6,		// 幻兽学习点位 p1.huanshou_id p2.point_index p3.point_level
			NOTICE_TYPE_RARE_COUNT = 7,			// 幻兽蛋稀有保底 p1:egg_id p2:cur_open_egg_times
		};

		short notice_type;
		short param1;
		int param2;
		int param3;
	};

	class CSHuanShouDecomposeReq		//!< 5364 幻兽灵魄分解请求
	{
	public:
		CSHuanShouDecomposeReq();
		MessageHeader header;

		int bag_column;
		short decompose_list[ItemNamespace::MAX_HUAN_SHOU_GRID_NUM];	//分解序号列表
	};

	class SCHuanShouOtherInfo			//!< 5365 幻兽其它信息下发
	{
	public:
		SCHuanShouOtherInfo();
		MessageHeader header;

		char fushen_id;								//此时附身的幻兽ID(id等同于序号)(-1:没附身)
		char is_use_huanshou;						//是否使用幻兽形象 0:不使用 1:使用幻兽形象
		short cur_lian_dan_count;					//丹炉内此时正在炼丹的次数
		unsigned int start_lian_dan_timestamp;		//开始炼丹时间戳
	};

	class SCHuanShouAnnouncementInfo	//!< 5366 幻兽公告信息下发
	{
	public:
		SCHuanShouAnnouncementInfo();
		MessageHeader header;

		short reserve_sh;
		short ann_count;															//当前公告数量
		HuanShouAnnouncementInfo ann_list[MAX_HUAN_SHOU_ANNOUNCEMENT_INFO_NUM];		//公告列表
	};


	class SCHuanShouBagInfoAck : public IMessageSerializer  //!< 5370 幻兽背包所有信息下发
	{
	public:
		SCHuanShouBagInfoAck();
		virtual ~SCHuanShouBagInfoAck() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);
			for (int i = 0; i < item_count && i < ARRAY_ITEM_COUNT(item_list); ++i)
			{
				result = result && WriteUShort(item_list[i].item_id, buffer, buffer_size, out_length);
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

		short		reserve_sh;
		short		item_count;											//!< 物品数量
		InfoType2	item_list[ItemNamespace::MAX_HUAN_SHOU_GRID_NUM];	//!< 物品列表  MAX_DRAGON_SOUL_BAG_NUM 200
	};

	class SCHuanShouBagSingleInfo : public IMessageSerializer  //!< 5371 幻兽背包单条信息下发
	{
	public:
		SCHuanShouBagSingleInfo();
		virtual ~SCHuanShouBagSingleInfo() {}
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(put_reason, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
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
	

}

#pragma pack(pop)


#endif //__MSG_HEART_SKILL_HPP__