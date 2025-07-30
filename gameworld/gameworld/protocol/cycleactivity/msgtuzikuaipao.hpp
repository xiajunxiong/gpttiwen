#ifndef __MSG_TU_ZI_KUAI_PAO_HPP__
#define __MSG_TU_ZI_KUAI_PAO_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/tuzikuaipaodef.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	enum TU_ZI_KUAI_PAO_REQ_TYPE
	{
		TU_ZI_KUAI_PAO_REQ_TYPE_SET,			//放置兔子 p1 p2 p3 赛道的兔子类型
		TU_ZI_KUAI_PAO_REQ_TYPE_ENCOURAGE,		//鼓励 p1 objid
		TU_ZI_KUAI_PAO_REQ_TYPE_SET_OK,		// 布局就绪
		TU_ZI_KUAI_PAO_LEAVE,		// 离开
	};

	class CSTuZiKuaiPaoReq			//!< 4770 
	{
	public:
		CSTuZiKuaiPaoReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
		int p3;
	};

	struct TuZiKuaiPaoSetInfo
	{
		int role_id;
		int set_tu_zi[TU_ZI_KUAI_PAO_SET_TIMES][TU_ZI_KUAI_PAO_MAX_TRACK_NUM];
		int is_ready[TU_ZI_KUAI_PAO_SET_TIMES];
	};

	class SCTuZiKuaiPaoSceneInfo			//!< 4771
	{
	public:
		SCTuZiKuaiPaoSceneInfo();
		MessageHeader header;
		
		int is_finsih;
		int phase;			//阶段123
		unsigned int next_phase_time;		//下一阶段时间
		int result[TU_ZI_KUAI_PAO_MAX_TRACK_NUM];	//结果 3个赛道胜利的人的role_id
		TuZiKuaiPaoSetInfo role_set_info[TU_ZI_KUAI_PAO_ROLE_NUM];	//设置兔子信息
	};

	class SCTuZiKuaiPaoInfo			//!< 4772 
	{
	public:
		SCTuZiKuaiPaoInfo();
		MessageHeader header;

		int reward_times;		//领取奖励次数
	};

	class SCTuZiKuaiPaoTuZiRest			//!< 4773 兔子休息
	{
	public:
		SCTuZiKuaiPaoTuZiRest();
		MessageHeader header;

		int track_type;
		int obj_id;
		int role_id;
	};


	class SCTuZiKuaiPaoEncourageResult			//!< 4774 鼓励结果 
	{
	public:
		SCTuZiKuaiPaoEncourageResult();
		MessageHeader header;

		int encourage_type;
		int is_succ;
		int succ_times;
		int fail_times;
		int obj_id;
	};

	class SCTuZiKuaiPaoTuZiRun			//!< 4775 兔子休息
	{
	public:
		SCTuZiKuaiPaoTuZiRun();
		MessageHeader header;

		int track_type;
		int obj_id;
		int role_id;
	};

	class SCTuZiKuaiPaoRoleInfo			//!< 4776
	{
	public:
		SCTuZiKuaiPaoRoleInfo();
		MessageHeader header;

		int use_carrot_times;
	};

	class SCTuZiKuaiPaoFinishNotice		//!< 4777
	{
	public:
		SCTuZiKuaiPaoFinishNotice();
		MessageHeader header;

		int win_role_id;
	};

	class SCTuZiKuaiPaoPosInfo		//!< 4778
	{
	public:
		SCTuZiKuaiPaoPosInfo();
		MessageHeader header;

		int obj_id;
		int x;
		int y;
	};
}

#pragma pack(pop)

#endif