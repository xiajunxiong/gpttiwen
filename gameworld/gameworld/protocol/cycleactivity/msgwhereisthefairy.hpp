#ifndef __MSG_WHERE_IS_THE_FAIRY_HPP__
#define __MSG_WHERE_IS_THE_FAIRY_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSRoleWhereIsTheFairyOperaReq			//!< 4730 仙子去哪儿操作请求
	{
	public:
		CSRoleWhereIsTheFairyOperaReq();
		MessageHeader header;

		int opera_req_type;
	};

	enum CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE
	{
		CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE_INFO = 0,			//玩家信息请求
		CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE_FINISH_GAME = 1,	//完成游戏请求
		CA_WHERE_IS_THE_FAIRY_OPERA_REQ_TYPE_MANAGER_INFO = 2,	//请求仙子信息
	};

	class SCRoleWhereIsTheFairyInfo		//!< 4731 仙子去哪儿玩家信息下发
	{
	public:
		SCRoleWhereIsTheFairyInfo();
		MessageHeader header;

		char is_day_fetch_where_is_the_fairy_flag;	// 当天是否领取过仙子去哪儿的奖励 0:未领取
		char reserve_ch;
		short reserve_sh;
	};

	class SCWhereIsTheFairyInfo			//!< 4732 仙子去哪儿仙子信息下发
	{
	public:
		SCWhereIsTheFairyInfo();
		MessageHeader header;

		int scene_id;							//仙子在哪个场景
		unsigned int next_refresh_timestamp;	//仙子下次刷新时间戳
	};

}

#pragma pack(pop)

#endif