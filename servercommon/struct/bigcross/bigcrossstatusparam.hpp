#ifndef __BIG_CROSS_STATUS_PARAM_HPP__
#define __BIG_CROSS_STATUS_PARAM_HPP__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/basedef.h"

class BigCrossStatusParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	void Reset();

	unsigned int last_update_timestamp;		// ���������� , ʵ��ʹ��ʱ�����Ƴ�[ͬʱɾ����Ӧ���ݿ��ֶ�]

};

#endif // __BIG_CROSS_STATUS_PARAM_HPP__

