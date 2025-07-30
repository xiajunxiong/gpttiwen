#ifndef __MSG_LEVEL_COMPLEMENT_H__
#define __MSG_LEVEL_COMPLEMENT_H__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class SCLevelComplementInfo					//!< 3562 - 所有信息
	{
	public:
		SCLevelComplementInfo();
		MessageHeader		header;

		long long accumulated_exp;					//!< 经验池
		unsigned int m_refresh_accumulated_tamp;	//!< 下次增加经验池时间戳
		unsigned int m_end_accumulate_tamp;			//!< 助力结束时间戳

		int m_day_difference;						//!< 助力差距天数
		int m_add_exp_times;						//!< 已刷新次数

		long long m_alread_get_exp;					//!< 累积获取经验
		long long m_save_exp;						//!< 累积获得存储经验
		long long m_today_accumulated_exp;			//!< 今天可获取经验值
	};

	class CSLevelComplementReq				//!< 3563 - 单个信息
	{
	public:
		enum LEVEL_COMPLEMENT_TYPE
		{
			LEVEL_COMPLEMENT_TYPE_0 = 0,//请求下发
			LEVEL_COMPLEMENT_TYPE_1 = 1,//购买经验
		};
		CSLevelComplementReq();
		MessageHeader		header;

		int type;				// !< 操作请求
		int param;				// !< 参数
	};

	class CSRoleNewLevelComplementReq		//!< 3566 新等级助力请求
	{
	public:
		CSRoleNewLevelComplementReq();
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,			// 请求角色助力信息下发
			REQ_TYPE_BUY_EXP = 1,		// 请求购买角色经验
		};

		int req_type;
	};

	class SCRoleNewLevelComplementInfo		//!< 3567 新等级助力角色信息下发	
	{
	public:
		SCRoleNewLevelComplementInfo();
		MessageHeader header;

		long long accumulation_exp;					//累积经验
		long long day_supplement_exp;				//当天凌晨补充的经验数值
		unsigned int last_get_daily_exp_timestamp;	//上次获得日常经验时间戳(不是当天)
		short day_buy_exp_count;					//当天购买经验次数
		short level_difference;						//等级差值
	};

	class SCNewLevelComplementSpeicalInfo	//!< 3568 新等级助力特殊信息下发
	{
	public:
		SCNewLevelComplementSpeicalInfo();
		MessageHeader header;

		int my_server_average_level;		// 配置前几名本服平均等级
	};
}

#pragma pack(pop)
#endif