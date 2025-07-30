#ifndef __MSG_DU_CHUANG_ZEI_YING_HPP__
#define __MSG_DU_CHUANG_ZEI_YING_HPP__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)
namespace Protocol
{
	struct SpecialLogicDuChuangZeiYingObjInfo
	{
		SpecialLogicDuChuangZeiYingObjInfo() :npc_id(0), status(0)
		{}

		int npc_id;
		int status;			//0:���� 1:���
	};

	class SCDuChuangZeiYingSceneInfo				//!< 3401 �·�������Ϣ
	{
	public:
		SCDuChuangZeiYingSceneInfo();
		MessageHeader header;

		static const int MAX_SCENE_OBJ_NUM = 4;

		int count;
		SpecialLogicDuChuangZeiYingObjInfo obj_info[MAX_SCENE_OBJ_NUM];
	};

	class SCDuChuangZeiYingNotice				//!> 3402 ֪ͨѰ·
	{
	public:
		SCDuChuangZeiYingNotice();
		MessageHeader header;

		int npc_id;
	};


}
#pragma pack(pop)

#endif