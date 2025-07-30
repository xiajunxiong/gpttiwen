#ifndef __MSG_SKILL_H__
#define __MSG_SKILL_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/skilllistparam.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	struct SkillItemInfo
	{
		short	index;			//!< 序号 (用于与服务端交互，标识某个技能）
		UInt16	skill_id;		//!< 技能ID
		short	skill_level;	//!< 技能等级
		short	reserve_sh;		//!< 保留字
		int		skill_exp;		//!< 当前经验
	};

	class SCSkillList : public IMessageSerializer	//!< 1300 技能列表协议
	{
	public:
		SCSkillList();
		virtual ~SCSkillList() {}

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteInt(skill_count, buffer, buffer_size, out_length);

			for (int i = 0; i < skill_count && i < MAX_SKILL_NUM; ++i)
			{
				result = result && WriteShort(skill_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteUShort(skill_list[i].skill_id, buffer, buffer_size, out_length);
				result = result && WriteShort(skill_list[i].skill_level, buffer, buffer_size, out_length);
				result = result && WriteShort(skill_list[i].reserve_sh, buffer, buffer_size, out_length); // 方便客户端用相同代码读SkillItemInfo
				result = result && WriteInt(skill_list[i].skill_exp, buffer, buffer_size, out_length);
			}

			result = result && WriteInt(common_use_skill_num, buffer, buffer_size, out_length);
			for (int i = 0; i < common_use_skill_num && i < MAX_SKILL_NUM; ++i)
			{
				result = result && WriteShort(common_use_skill_index_list[i], buffer, buffer_size, out_length);
			}

			return result;
		}

		MessageHeader	header;

		int				skill_count;				//!< 已学习的技能数量
		SkillItemInfo	skill_list[MAX_SKILL_NUM];	//!< 技能列表 

		int				common_use_skill_num;		//!< 常用技能数
		short			common_use_skill_index_list[MAX_SKILL_NUM]; //!< 常用技能列表（表内元素为技能序号 \see SkillItemInfo::index）
	};

	class SCSkillChangeNotice						//!< 1301 技能信息变更协议
	{
	public:
		SCSkillChangeNotice();
		MessageHeader header;

		SkillItemInfo	skill;						//!< 变更的技能		\see SkillItemInfo
	};

	class CSSkillOperateReq							//!< 1350 技能操作请求
	{
	public:
		CSSkillOperateReq();
		MessageHeader header;

		enum OPERATE_TYPE
		{
			OPERATE_TYPE_GET_LIST = 0,				//!< 获取技能列表 0
			OPERATE_TYPE_LEARN_SKILL = 1,			//!< 学习技能 1    \param param1 技能id
			OPERATE_TYPE_USE_SKILL_GRASS = 2,		//!< 使用技能草 2  \param param1 技能id  \param param2 技能草类型  \param param3 是否批量[0否 1是]
			OPERATE_TYPE_SET_COMMON_USE_SKILL = 3,	//!< 设置常用技能 3 \param param1 技能序号（index） \param param2 [0:添加常用  1:删除常用]
			OPERATE_TYPE_SKILL_FAST_UP = 4,			//!< 学习技能(一键) 4	\param param1 技能id	param2:目标等级
			OPERATE_TYPE_SKILL_UPLEVEL = 5,			//!< 升级技能 5	   \param param1 技能id  param2 是否勾选所有技能都升级
		};

		int operate_type;							//!< 请求类型  \see OPERATE_TYPE
		int param1;
		int param2;
		int param3;
	};

	struct SkillContainer
	{
		SkillContainer() : skill_id(0) {}

		int skill_id;
		LifeSkillInfo skill_data;
	};

	class SCLifeSkillInfo						//!< 1302 生活技能信息下发
	{
	public:
		SCLifeSkillInfo();
		MessageHeader header;

		enum REASON_TYPE
		{
			REASON_TYPE_NORMAL = 0,				//!< 0 正常情况下发		
			REASON_TYPE_CHANGE_NOTICE,			//!< 1 变更通知 （此情况下，只会下发单个技能信息，客户端替换单个技能的显示即可）
		};

		short reason_type;						//!< 下发协议的原因 \see SCLifeSkillInfo
		short count;							//!< 下发技能列表的元素数量
		SkillContainer life_skill_list[LIFE_SKILL_TYPE_MAX];  //!<  生活技能列表   \see LIFE_SKILL_TYPE_MAX
	};

	class SCCommonUseSkillChangeNotice			//!< 1303 常用技能变更通知
	{
	public:
		SCCommonUseSkillChangeNotice();
		MessageHeader header;

		int common_use_skill_num;						//!< 常用技能数量
		short common_use_skill_idx_list[MAX_SKILL_NUM];	//!< 常用技能序号列表 （根据common_use_skill_num的值来读取列表长度)
	};

	class SCSkillExpDeltaChangeNotify
	{
	public:
		SCSkillExpDeltaChangeNotify();			//!< 1304 技能经验改变通知
		MessageHeader header;

		int skill_id;				//!< 技能ID：有可能是战斗技能，也有可能是生活技能
		int exp_change;				//!< 经验值改变量：正数为增加，负数为减损
	};

	class CSSkillPositionModify		//!< 1351
	{
	public:
		CSSkillPositionModify();
		MessageHeader header;

		int skill_num;				//!< 请求列表中有多少个技能序号
		short common_use_skill_idx_list[MAX_SKILL_NUM]; //!< 技能序号列表
	};
}

#pragma pack(pop)
#endif