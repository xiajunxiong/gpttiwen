#ifndef __MSG_WORKSHOP_H__
#define __MSG_WORKSHOP_H__

#include "gamedef.h"
#include "servercommon/workshopdef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSWorkshopOperateRequire
	{
	public:
		CSWorkshopOperateRequire();			//!< 1730 - ����ϵͳ��������
		MessageHeader header;

		enum WORLSHOP_OP
		{
			WORKSHOP_OP_COMPOUND_REQ = 0,				//!< ����ϳ���Ʒ
			WORKSHOP_OP_GEM_MOUNTING = 1,				//!< װ����ʯ��Ƕ
			WORKSHOP_OP_GEM_DISMOUNT = 2,				//!< װ����ʯ��ж
			WORKSHOP_OP_MAKE_JEWELTY = 3,				//!< �����������
			WORKSHOP_OP_JEWE_REFORGE = 4,				//!< ���������ض�
			WORKSHOP_OP_GAIN_REFORGE = 5,				//!< ѡ���ضͽ��
			WORKSHOP_OP_GEM_UPGRADES = 6,				//!< װ����ʯ����
			WORKSHOP_OP_GEM_CONF_UGD = 7,				//!< ȷ�ϱ�ʯ����
			WORKSHOP_OP_SYNTHESIS = 8,					//!< ��Ʒ�һ����ϳ�
			WORKSHOP_OP_DISCOMPOUND_REQ = 9,			//!< ����һ���Ʒ
			WORKSHOP_OP_BATCH_COMP_REQ = 10,			//!< ���������ϳ�װ��
			WORKSHOP_OP_AVTIVE_EQUIP = 11,				//!< ���󼤻��λ����
			WORKSHOP_OP_REFRESH_EQUIP = 12,				//!< ����ϴ��δ������λ����
			WORKSHOP_OP_SAVE_EQUIP_REFRESH = 13,		//!< ���󱣴�ϴ���������
			WORKSHOP_OP_ULTIMATE_WORK = 14,				//!< ���󾫹�
			WORKSHOP_OP_GOD_STRONGTHEN = 15,			//!< ��װǿ������
			WORKSHOP_OP_DIAN_HUA = 16,					//!< װ���㻯 param1 �Ƿ񴩴��� param2 �±�idx param3 �㻯ʯ���ڱ����� param4 �㻯ʯ�±�
			WORKSHOP_OP_SAVE_DIAN_HUA = 17,				//!< ����㻯 param1 �Ƿ񴩴��� param2 �±�idx
			WORKSHOP_OP_FU_MO = 18,						//!< װ����ħ param1 �Ƿ񴩴��� param2 �±�idx
			WORKSHOP_OP_XI_FU = 19,						//!< װ������ param1 �Ƿ񴩴��� param2 �±�idx
			WORKSHOP_OP_DISCOMPOSE = 20,				//!< �ֽ�㻯ʯ param1 �������� param2 �����±�
			WORKSHOP_OP_UP_STAR = 21,					//!< װ������ param1 seq param2:�Ƿ�����	param3:װ������λ�� param4:װ����ռ���� param5:���ĵ���λ��  param6 ���ĵ���2λ��
			WORKSHOP_OP_UP_LEVEL_DIANHUA = 22,			//!< �㻯ʯ���� param1 ���㻯ʯ param2 ���ĵ㻯ʯ1 param2 ���ĵ㻯ʯ2
		};

		short op_type;			//!< ��������
		short param1;			//!< ��������� 0 - �ϳɱ� seq��	1 - �Ƿ�����		2 - �Ƿ�����		 3 - �б�id��	4 - ���ϸ��� 1��	5 - ѡ��id��6 - װ������λ�ã� 7 - null��			8 - �һ�Seq;		9 - �ϳɱ���� seq;		10:�ϳɱ�seq	11:�Ƿ�����			12:�Ƿ�����			13:�Ƿ�����			14��1/2������/ǿ��������	15:1/2������/ͻ�ƣ�
		short param2;			//!< ��������� 0 - �ϳ�������		1 - װ������λ��	2 - װ������λ��					4 - ���ϸ��� 2��				6 - װ����ռ���ӣ�						8 - �ϳ�����		9 - �ϳ�Ŀ����Ʒid;		10:�ϳ�����		11:װ������λ��		12:װ������λ��		13:װ������λ��		14���Ƿ�����				15���Ƿ�����
		short param3;			//!< ��������� 0 - �Ƿ�ǿ�����죻	1 - װ����ռ����	2 - װ����ռ����													6 - ��ʯ��Ƕ��λ�� 7 - ���������Ʒid																11:װ����ռ����		12:װ����ռ����		13:װ����ռ����		14��װ������λ��			15��װ������λ��
		short param4;			//!< ������� : 0 - 0:���ǻ����  1 - ��ʯ��ռ����	2 - ��ʯ��Ƕ��λ																																						11:��������λ��		12:��������(����)	13:�Ƿ���			14��װ����ռ����			15��װ����ռ����
		short param5;			//!<																																																							11:�Ƿ���					12:�Ƿ���								14���Ƿ���				15���Ƿ���
		short param6;			//!<																																																												12:�Ƿ�ʹ��Ԫ�����
		short reserve;
	};

	class CSWorkshopDecomposeReq
	{
	public:
		const static int MAX_DECOMPOSE_NUM = 20;
		CSWorkshopDecomposeReq();		//!< 3600 - װ���ֽ�����
		MessageHeader header;

		int grid_num;
		int decompose_grid[MAX_DECOMPOSE_NUM];
	};

	class SCWorkshopAllInfoRespond
	{
	public:
		SCWorkshopAllInfoRespond();		//!< 1731 - ��������������Ϣ
		MessageHeader header;

		int voc_refresh_times;								//!< ����װ����������
		BitMap<SPECIAL_RECIPE_TOTAL_NUM> sp_active_flag;	//!< ���пɼ�����䷽�ļ���״̬
		BitMap<SPECIAL_RECIPE_TOTAL_NUM> sp_created_flag;	//!< �����䷽������״̬
	};

	class SCSingleRecipeActiveFlag
	{
	public:
		SCSingleRecipeActiveFlag();		//!< 1732 - �������������Ϣ
		MessageHeader header;

		unsigned short active_id;		//!< ��������
		char is_active;					//!< �Ƿ񼤻 0 - δ��� 1 - ����
		char is_notice;					//!< �Ƿ񵯴�: 0:���� 1:������ʾ
	};

	class SCJewelryReforgeOutcomes
	{
	public:
		SCJewelryReforgeOutcomes();		//!< 1733 - �����ضͽ��֪ͨ
		MessageHeader header;

		JewelryProReserve reforge_list[JEWELTY_REFORGE_LIST_MAX];
	};

	class SCGemOneKeyUpgradesNeeds
	{
	public:
		SCGemOneKeyUpgradesNeeds();		//!< 1734 - һ��������ʯ���ĵĲ���
		MessageHeader header;

		GemUpgradedata mate_data;
	};

	class CSConvertPetSkillBookReq
	{
	public:
		CSConvertPetSkillBookReq();		//!< 1735 - ���＼����һ�
		MessageHeader header;

		UInt16 exc_seq;
		UInt16 list_num;
		ItemPairCon grid_list[PET_SKILL_BOOK_NEED_ITEM_TYPE_NUM];
	};

	class SCWorkshopComposeResults : public IMessageSerializer
	{
	public:
		SCWorkshopComposeResults();		//!< 1736 - �����ϳɽ���ظ�
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteChar(com_result, buffer, buffer_size, out_length);
			result = result && WriteChar(has_param, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteUShort(number, buffer, buffer_size, out_length);

			if (0 != has_param)
			{
				result = result && WriteUShort(param_len, buffer, buffer_size, out_length);
				result = result && WriteStrN(param_data, param_len, buffer, buffer_size, out_length);
			}

			return result;
		}

		char com_result;		//!< �ϳɽ��: 0 - ʧ�ܣ�1 - �ɹ���
		char has_param;			//!< �Ƿ���о�ֵ
		ItemID item_id;			//!< ����ID
		UInt16 number;			//!< ����
		UInt16 param_len;		//!< ���ݳ���
		NetValueItemParamData param_data;	//!< ������Ϣ
	};

	class CSChangeEquipVocReq
	{
	public:
		enum VOC_CHANGE_TYPE
		{
			VOC_CHANGE_TYPE_EQUIP = 0,//װ��
			VOC_CHANGE_TYPE_JEWELRY = 1,//����
			VOC_CHANGE_TYPE_ULTMATE_EQUIP = 2,//���� - װ��
			VOC_CHANGE_TYPE_ULTMATE_JEWELRY = 3,//���� - ����
		};
		CSChangeEquipVocReq();		//!< 1737 - װ�������滻����
		MessageHeader header;
		int change_voc_type;//������������
		int role_type;		//0:����  1:����
		//��ô���װ��
		int replacer_posi;
		int replacer_grid;
		int replacer_voc_index;
		//ժȡ����װ��
		int bereplacer_posi;
		int bereplacer_grid;
		int bereplacer_voc_index;
	};

	class CSWorkShopJewelryInfo
	{
	public:
		CSWorkShopJewelryInfo();		//!< 1738 - ���κϳ�ʧ�ܴ���
		MessageHeader header;
		int jewelry_times[INTJEWELRY_SUCC_MAX_GROUP];
	};

	class CSPetEquipMentReq
	{
	public:
		enum REQ_TYPR
		{
			REQ_TYPR_COMPOSE = 0,
			REQ_TYPR_REFRESH = 1,
			REQ_TYPR_SAVE_REFRESH = 2,
			REQ_TYPR_BREAK = 3,
		};
		CSPetEquipMentReq();
		MessageHeader header;
		int req_type;
		int param1;
		int param2;
		int param3;
	};
}

#pragma pack(pop)
#endif