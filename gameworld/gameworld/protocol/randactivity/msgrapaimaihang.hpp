#ifndef __MSG_RA_PAI_MAI_HANG_HPP__
#define __MSG_RA_PAI_MAI_HANG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rapaimaihangparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��3000
	enum RA_PAI_MAI_HANG_REQ_TYPE
	{
		RA_PAI_MAI_HANG_REQ_TYPE_ALL_INFO = 0,			//�����ɫ������Ϣ
		RA_PAI_MAI_HANG_REQ_TYPE_BID = 1,				//������ P1:seq P2:bid_price
		RA_PAI_MAI_HANG_REQ_TYPE_AUTO_BID = 2,			//�����Զ����� P1:seq P2:bid_price P3:add_price (�˴��̶����Զ�����)
		RA_PAI_MAI_HANG_REQ_TYPE_CANCEL_AUTO = 3,		//����ȡ���Զ����� P1:seq  (ȡ���Զ�������̷���ʣ��Ĵ�)
		RA_PAI_MAI_HANG_REQ_TYPE_RETRIEVE = 4,			//����ȡ�ؼĴ�Ԫ�� P1:seq P2:retrieve_gold P3:add_price
		RA_PAI_MAI_HANG_REQ_TYPE_SET_SHOW_FALG = 5,		//����������Ϣչʾ״̬ P1:seq P2: 0-��չʾ
		RA_PAI_MAI_HANG_REQ_TYPE_RECORD_INFO = 6,		//�����¼��Ϣ
		RA_PAI_MAI_HANG_REQ_TYPE_CHANGE_ADD_PRICE = 7,	//�����޸ļӼ۽��(ֻ��ӵ�мĴ�ſ��޸�) p1:seq p2:add_price
	};

	
	class SCRAPaiMaiHangNoticeInfo		//!< 3100 ����-������֪ͨ��Ϣ�·�
	{
	public:
		SCRAPaiMaiHangNoticeInfo();
		MessageHeader header;

		unsigned int pai_mai_start_timestamp;				//���Ŀ�ʼʱ���
		unsigned int pai_mai_end_timestamp;					//���Ľ���ʱ���
		unsigned int pai_mai_last_end_timestamp;			//�����������ʱ���

		RAPaiMaiHangSingleShopItemInfo shop_item_list[MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM];	//ȫ����Ʒ��Ϣ
	};

	class SCRAPaiMaiHangSingleNoticeInfo	//!< 3101 ����-�����е���֪ͨ��Ϣ�·�
	{
	public:
		SCRAPaiMaiHangSingleNoticeInfo();
		MessageHeader header;
		
		int seq;
		RAPaiMaiHangSingleShopItemInfo shop_item_info;
	};

	class SCRAPaiMaiHangUserInfo	//!< 3102 ����-�����н�ɫ��Ϣ�·�
	{
	public:
		SCRAPaiMaiHangUserInfo();
		MessageHeader header;

		RAPaiMaiHangUserSingleItemInfo shop_item_list[MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM];	//��ɫ������Ʒ��Ϣ
	};

	class SCRAPaiMaiHangUserSingleInfo	//!< 3103 ����-�����н�ɫ������Ϣ�·�
	{
	public:
		SCRAPaiMaiHangUserSingleInfo();
		MessageHeader header;

		int seq;
		RAPaiMaiHangUserSingleItemInfo shop_item_info;
	};

	class SCRAPaiMaiHangUserRecordInfo	//!< 3104 ����-�����н�ɫ���ļ�¼�·�
	{
	public:
		SCRAPaiMaiHangUserRecordInfo();
		MessageHeader header;

		int record_count;
		RAPaiMaiHangRecordParam record_list[MAX_PAI_MAI_HANG_RECORD_NUM];
	};


}

#pragma pack(pop)

#endif //__MSG_RA_PAI_MAI_HANG_HPP__