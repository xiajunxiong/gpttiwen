#ifndef __CROSS_MSG_CODE_H__
#define __CROSS_MSG_CODE_H__

namespace Protocol
{
	// 注意 每种跨服玩法协议号单独一个段 global根据协议号转发给相应cross

	enum
	{
		CROSS_SERVER_MSG_BEGIN = 14000,
		CROSS_SERVER_MSG_END = 15000,
	};

	enum 
	{
		MT_RETURN_ORIGINAL_SERVER_SC = 14001,								// 通知玩家回到原服
		MT_CROSS_GET_PERSON_RANK_LIST_ACK_SC = 14002,						// 下发跨服排行榜列表
		MT_CROSS_PERSON_RANK_BASE_INFO_SC = 14003,							// 下发跨服排行榜基础信息（每个榜单的跨服第一，跨服所连接的服务器id列表）
		MT_CROSS_GET_PERSON_RANK_ALL_SERVER_LIST_ACK_SC = 14004,			// 跨服总榜
		MT_CROSS_GET_PET_RANK_LIST_ACK_SC = 14005,							// 下发跨服排行榜列表
		MT_CROSS_GET_PET_RANK_ALL_SERVER_LIST_ACK_SC = 14006,				// 跨服总榜

		MT_CROSS_GET_PERSON_RANK_LIST_CS = 14050,							// 获取跨服排行榜列表
		MT_CROSS_GET_PERSON_RANK_BASE_INFO_CS = 14051,						// 获取跨服排行榜基础信息
		MT_CROSS_GET_PERSON_RANK_ALL_SERER_LIST_CS = 14052,					// 请求总榜
		MT_CROSS_GET_PET_RANK_LIST_CS = 14053,								// 获取跨服排行榜列表
		MT_CROSS_GET_PET_RANK_ALL_SERER_LIST_CS = 14054,					// 请求总榜
		MT_CROSS_GET_PET_RANK_DETAILED_INFO_CS = 14055,						// 请求跨服宠物排行榜某一个宠物信息
		MT_CROSS_GET_PET_RANK_ALL_DETAILED_INFO_CS = 14056,					// 请求跨服宠物总榜某一个宠物信息

		MT_CROSS_ROLE_QUERY_FOLLOW_UP_INFO_SC = 14060,						// 人物查询后续信息追加下发		

		MT_CROSS_GUILD_RANK_LIST_REQ_SC = 14070,							// 请求跨服帮派排行榜信息
		MT_CROSS_GUILD_RANK_LIST_ACK_SC = 14071,							// 发送跨服帮派排行榜信息[单服]
		MT_CROSS_GUILD_RANK_LIST_ALL_SERVER_SC = 14072,						// 发送跨服帮派排行榜信息[总榜]
	};
}

#endif	// __CROSS_MSG_CODE_H__

