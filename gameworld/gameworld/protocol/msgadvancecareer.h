#ifndef __MSG_ADVANCE_CAREER_H__
#define __MSG_ADVANCE_CAREER_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/rolecrystalfbdef.h"
#include "servercommon/commondata.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSRoleCareerAdvanceReq					//!< 1456 ��ɫ������������һ��תְ��
	{
	public:
		CSRoleCareerAdvanceReq();
		MessageHeader header;

		int to_profession;							//!< �������ĸ�ְҵ[С�׶�ʱû��]
		int to_color;								//!< ������ı���ɫ[С�׶�ʱû��]
	};

	class SCAdvanceCareerSubInfo					//!< 4156 ְҵ�����ӽ���Ϣ	
	{
	public:
		SCAdvanceCareerSubInfo();
		MessageHeader		header;

		int sub_level;							// ��ǰ�ӽ׶εĽ���
		char is_mission_complete;				// ��ǰ�׼��������Ƿ����(��ս�����͵ȼ�)
		char reserve_ch;						// ����_ch
		short reserve_sh;						// ����_sh
	};
	
	class CSRoleChangeColorReq					// 1459 ��ɫְҵ��ɫ�������
	{
	public:
		CSRoleChangeColorReq();
		MessageHeader		header;

		int to_color;							// Ŀ����ɫ
	};
}

#pragma pack(pop)

#endif	// __MSG_ADVANCE_CAREER_H__

