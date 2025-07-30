#ifndef __MSG_PET_GOD_FB_H__
#define __MSG_PET_GOD_FB_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"

#include "servercommon/petgodfbdef.hpp"


#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	//神印之地 用户操作		3930
	class CSPetGodFBReq
	{
	public:
		CSPetGodFBReq();
		MessageHeader		header;
		enum REQ_TYPE
		{
			REQ_TYPE_BUY_DAY_TMES = 0,	//购买每日次数
			REQ_TYPE_BUY_AUTO_FB =1,	//扫荡		p_1:fb_seq
		};
		int req_type;		//操作类型
		int param_1;
	};

	//角色信息			3931
	class SCPetGodFBRoleInfo
	{
	public:
		SCPetGodFBRoleInfo();
		MessageHeader		header;

		//历史最高通关层数 [ 副本类型(fb_type) - 1 ]
		int max_history_pass_level[PET_GOD_FB_MAX_TYPE_NUM];
	};

	//副本场景信息		3932
	class SCPetGodFBSceneInfo
	{
	public:
		SCPetGodFBSceneInfo();
		MessageHeader		header;

		int m_fb_seq;		//副本seq
		int m_is_finish;	//副本完成标记
		int m_boss_npc_id;	//boss
	};


};

#pragma pack(pop)

#endif