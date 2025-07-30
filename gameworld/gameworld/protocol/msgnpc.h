#ifndef __MSG_NPC_H__
#define __MSG_NPC_H__

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSNPCHandleRequest
	{
	public:
		CSNPCHandleRequest();			//!< 1760: ���� NPC ����
		MessageHeader header;

		enum NPC_REQUEST_TYPE
		{
			NPC_REQUEST_STARTS_BATTLE = 0,		//!< ����ս��
			NPC_REQUEST_PURCHASE_ITEM = 1,		//!< ������Ʒ��ͨ�� NPC �̵꣩
			NPC_REQUEST_STARTS_GATHER = 2,		//!< ����ɼ�
			NPC_REQUEST_END_GATHER = 3,			//!< ��ɲɼ�
			NPC_REQUEST_CHOSED = 4,				//!< �Ի�NPC(ֻ����������NPC������NPC�ſ���ʹ��)
			NPC_REQUEST_START_BATTLE = 5,		//!< ����ս��(�����ж�)

			NPC_REQUEST_TYPE_MAX
		};

		short op_type;		//!< |	0			|	1		|	2		|	3		|	4		|	5 |
		ObjID npc_objid;	//!< ������->NPCobjid(����ֻ�жԻ�NPC���õ�)
		int npc_seq;		//!< |	   NPC �� SeqID												|

		int param1;			//!< |0����(��)		|			|		�ɼ���objid		|��������	|  ������ID
		int param2;			//!< | 0����ս����id|			|		�Ƿ��Զ��ɼ�	|��������	|
	};

	class SCNPCStartToGather
	{
	public:
		SCNPCStartToGather();			//!< 1761: �ɼ�����ʱ��
		MessageHeader header;

		unsigned int need_time;			//!< ����ɼ�ʱ��(��)
	};

	class SCNPCStopGathering
	{
	public:
		enum CANCEL_REASONS
		{
			CANCEL_REASON_FAIL = 0,		//!< �ɼ�ʧ��
			CANCEL_REASON_SUCC = 1,		//!< �ɼ��ɹ�

			CANCEL_REASON_MAX
		};

		SCNPCStopGathering();		//!< 1762: �����ɼ���Ϊ
		MessageHeader header;

		int role_obj_id;		//!< �ɼ����objid
		int gather_seq;			//!< �ɼ���� SeqID
		int reasons;			//!< ��������
	};

	class SCNPCGathersNotify
	{
	public:
		SCNPCGathersNotify();		//!< 1763: �ɼ��㲥(֪ͨ��Χ���)
		MessageHeader header;

		int role_obj_id;		//!< ��ɫ�� ObjID
		int gather_seq;			//!< �ɼ���� SeqID
		int npc_id;				//!< �ɼ���� NpcID
	};

	class SCNPCInfoNotify
	{
	public:
		SCNPCInfoNotify();		//!< 1765
		MessageHeader header;

		int obj_id;		//!< ObjID
		int param1;
		int param2;
		int param3;
	};

	class SCNPCCommitBack
	{
	public:
		SCNPCCommitBack();		//!< 1766  NPC��������
		MessageHeader header;
		struct option
		{
			option() : option_content(0), cost_coin(0), cost_item(0), cost_num(0)
			{}
			int option_content;	//!<ѡ����������seq
			int cost_coin;		//!<����ͭ��
			int cost_item;		//!<������Ʒid
			int cost_num;		//!<������Ʒ����
		};

		int obj_id;		//!< ObjID
		//����������seq
		int main_content;
		//ѡ��
		option option_list[2];
	};

	class SCNPCShopInfo
	{
	public:
		const static int MAX_SALE_ITEM_COUNT = 20;//�����������
		SCNPCShopInfo();		//!< 1767  NPC�̵귵��
		MessageHeader header;
		struct NpcShopItem
		{
			struct NpcShopItem_data
			{
				NpcShopItem_data() :item_id(0), num(0), is_bind(0), money_type(0), price(0), discount(0){}
				int item_id;			//!< ��Ʒid
				int num;				//!< ��Ʒ����
				short is_bind;			//!< �Ƿ��
				short money_type;		//!< �������� 0 Ԫ�� 1 �ɱ� 2 ͭ��
				int price;				//!< �۸�
				int discount;			//!< �ۿ�
			};
			NpcShopItem() :num(0) {}
			NpcShopItem_data item;		//!< ��Ʒ��Ϣ
			int num;					//!< �ɹ�������
			int max_num;				//!< �޹�����
		};
		int obj_id;						//!< ObjID

		int item_count;					//!< ��Ʒ����
		NpcShopItem item_list[MAX_SALE_ITEM_COUNT];
	};

	class CSNPCShopBuyReq
	{
	public:
		CSNPCShopBuyReq();		//!< 1768  ��NPC��������
		MessageHeader header;
		int obj_id;				//!< ObjID

		int buy_index;			//!< ��Ʒ���1��ʼ ����1767�·������
		int buy_num;			//!< ��������
	};
}

#pragma pack(pop)
#endif