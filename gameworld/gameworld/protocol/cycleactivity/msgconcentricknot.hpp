#ifndef __MSG_CONCENTRIC_KNOT_HPP__
#define __MSG_CONCENTRIC_KNOT_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSConcentricKnotReq			//!< 4690
	{
	public:
		enum REQ_TYPE
		{
			REQ_TYPE_ENTER = 0,				// 准备
			REQ_TYPE_AGREE,					// 准备回复		p1:is_agree
			REQ_TYPE_START,					// 开始			p1:cards_grade
			REQ_TYPE_OPEN_CARD,				// 翻牌			p1:card_id
			REQ_TYPE_END,					// 游戏结束		p1:is_finish
			REQ_TYPE_INFO,					// 请求个人信息
		};

		CSConcentricKnotReq();
		MessageHeader header;

		int req_type;
		int p1;
	};

	class SCConcentricKnotAgreeNotice		//!< 4691 
	{
	public:
		SCConcentricKnotAgreeNotice();
		MessageHeader header;

		int uid;
		int is_agree;
	};

	class SCConcentricKnotStart		//!< 4692 
	{
	public:
		SCConcentricKnotStart();
		MessageHeader header;

		int cards_group;
		unsigned int begin_times;
		unsigned int end_times;
	};

	class SCConcentricKnotOpenCard		//!< 4693
	{
	public:
		SCConcentricKnotOpenCard();
		MessageHeader header;

		int uid;
		int card_id;
	};

	class SCConcentricKnotEndGame		//!< 4694
	{
	public:
		SCConcentricKnotEndGame();
		MessageHeader header;

		int is_finish;
	};

	class SCConcentricKnotInfo		//!< 4695
	{
	public:
		SCConcentricKnotInfo();
		MessageHeader header;

		int reward_times;
	};
}
#pragma pack(pop)

#endif