#ifndef MSGPRESTIGE_H
#define MSGPRESTIGE_H

#include "gamedef.h"
#include "servercommon/prestigedef.hpp"
#include "servercommon/commondata.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct PrestigeVillageInfo
	{
		PrestigeVillageInfo(){}

		PrestigeVillageInfo& operator=(const PrestigeVillageNode & param)
		{
			this->prestige = param.prestige;
			this->level = param.level;
			return *this;
		}

		int prestige;
		int level;
	};

	class SCPrestigeAllVillageInfo			//!< 1800 ȫ�����ӵ���Ϣ
	{
	public:
		SCPrestigeAllVillageInfo();
		MessageHeader		header;

		int count;						//!< �ж��ٸ�����
		PrestigeVillageInfo prestige_village_list[PRESTIGE_VILLAGE_MAX_NUM];
	};

	class SCPrestigeSingleVillageInfo			//!< 1802  �������ӵ���Ϣ
	{
	public:
		SCPrestigeSingleVillageInfo();
		MessageHeader		header;

		int index;						//!< ���ӵ�id
		PrestigeVillageInfo prestige_village;
	};

	class SCPrestigeAllShopInfo			//!< 1801 ȫ����Ʒ����Ϣ
	{
	public:
		SCPrestigeAllShopInfo();
		MessageHeader		header;

		// �޹�����
		unsigned short buy_count[NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM];		//!< �������
	};

	class SCPrestigeSingleShopInfo			//!< 1803 ������Ʒ����Ϣ
	{
	public:
		SCPrestigeSingleShopInfo();
		MessageHeader		header;

		// �޹�����
		short index;				//!< ��Ʒ��index
		short buy_count;			//!< �������
	};

	class CSPrestigeShopingReq			//!< 1804 ��������
	{
	public:
		CSPrestigeShopingReq();
		MessageHeader		header;

		int index;				//!< ��Ʒ��index
		int num;					//!< ���������
	};

	class SCPrestigeDonateInfo			//!< 1805	������Ϣ
	{
	public:
		SCPrestigeDonateInfo();
		MessageHeader		header;

		int today_donate_count;			//!< ���վ����˶��ټ�
		int today_add_value;			//!< ���ջ���˶�������
	};

	class SCPrestigeDonateNotice			//!< 1807	����֪ͨ
	{
	public:
		SCPrestigeDonateNotice();
		MessageHeader		header;

		int today_donate_count;
	};


	class CSPrestigeDonateOp			//!< 1806	����  
	{
	public:
		CSPrestigeDonateOp();
		MessageHeader		header;

		int village_id;		//!<����id
		int column;			//!<��������
		int index;			//!<��������
	};

	class SCPrestigeChangeNotice			//!< 1808	�����䶯֪ͨ
	{
	public:
		SCPrestigeChangeNotice();
		MessageHeader		header;

		int village_id;		//!<����id
		int param;			//!<��������������ֵ/�����ȼ���
	};

	
}
#pragma pack(pop)

#endif // !MSGPRESTIGE_H
