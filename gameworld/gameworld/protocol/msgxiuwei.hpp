#ifndef  __MSG_XIU_WEI_HPP__
#define  __MSG_XIU_WEI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/xiuweiparam.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{	
	enum XIU_WEI_REQ_TYPE
	{
		XIU_WEI_REQ_TYPE_GET_INFO = 0,
		XIU_WEI_REQ_TYPE_UP_LEVEL = 1,			// param1:�������� , param2:����������
		XIU_WEI_REQ_TYPE_ONE_KEY_UP = 2,		// һ������ param1:�������� , param2:����������
	};

	class CSXiuWeiReq							//!< 4345 ��Ϊ �ͻ��˲�������
	{
	public:
		CSXiuWeiReq();
		MessageHeader header;

		int req_type;
		int param1;
		int param2;
	};

	class SCXiuWeiInfo							//!< 4346 ��Ϊϵͳ��Ϣ	
	{
	public:
		SCXiuWeiInfo();
		MessageHeader		header;

		XiuWeiInfo info[XIU_WEI_TYPE_MAX];		// ��Ϊϵͳȫ��Ϣ
	};

	class SCXiuWeiInfoChange					// 4347 ��Ϊϵͳ��Ϣ���
	{
	public:
		SCXiuWeiInfoChange();
		MessageHeader		header;

		int type;								// ��Ϊ�ļ�������
		XiuWeiInfo info;						// ��Ӧ����Ϣ
	};

	class SCXiuWeiDanYaoLimitInfo				// 4348 �߼���ҩʹ�ô�����Ϣ 
	{
	public:
		SCXiuWeiDanYaoLimitInfo();
		MessageHeader		header;

		int normal_dan_day_times;				// �ͼ���ҩÿ����ʹ�ô���
		int high_dan_day_times;					// �߼���ҩÿ����ʹ�ô���
	};
}

#pragma pack(pop)

#endif	// __MSG_XIU_WEI_HPP__
