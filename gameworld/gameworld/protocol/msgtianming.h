#ifndef __MSG_TIAN_MING_HPP__
#define __MSG_TIAN_MING_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/struct/tianmingparam.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCTianMingInfo							//!< ����ϵͳ��Ϣ 4015
	{
	public:
		SCTianMingInfo();
		MessageHeader		header;

		int level;									// ����ϵͳ�ܵȼ�
		int exp;									// ����ϵͳ�ľ���

		int sys_type_level[TIAN_MING_ADD_ATTR_SYS_TYPE_MAX];	// ÿ������ӳɵ�ǰ��ߵȼ�
	};
}

#pragma pack(pop)

#endif // __MSG_TIAN_MING_HPP__

