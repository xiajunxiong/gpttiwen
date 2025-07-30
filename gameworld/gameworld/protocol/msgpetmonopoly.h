#ifndef __MSG_PET_MONOPOLY_H__
#define __MSG_PET_MONOPOLY_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/petmonopolydef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct MsgPetMonopolyGrid
	{
		short reward_type;	//!< ���ӽ�������  \see PET_MONOPOLY_GRID_REWARD
		short grid_type;	//!< ��������	\see PET_MONOPOLY_GRID_TYPE
		int param1;			//!< �������ǳ���ʱ����ʾ����ID��Ϊ��������Ʒ����Ƭʱ����ʾ��Ʒ����ƬID; �������ǻ����ʱ����ʾ������Ʒid
		int param2;			//!< ����������Ʒ����Ƭʱ����ʾ��Ʒ����Ƭ����; �������ǻ����ʱ����ʾ������Ʒ����
	};

	class SCPetMonopolyInfo				//!< 3660 ����ؾ���Ϣ�·�
	{
	public:
		SCPetMonopolyInfo();
		MessageHeader header;

		int expect_silver_pet_id;		//!< ������  0��ʾû��ѡ������
		int expect_gold_pet_id;			//!< ����  0��ʾû��ѡ���
		short cur_level;				//!< ��ǰ����
		short cur_grid_idx;				//!< ��ǰ����  [0,36)

		MsgPetMonopolyGrid grid_list[PET_MONOPOLY_GRID_NUM]; //!< ���̣���36������
	};

	class CSPetMonopolyOperate				//!< 3661 ����ؾ���������
	{
	public:
		CSPetMonopolyOperate();
		MessageHeader header;

		enum OPER_TYPE
		{
			OPER_TYPE_INFO = 0,				//!< ����������Ϣ
			OPER_TYPE_REWARD_RECORD = 1,	//!< �鿴����
			OPER_TYPE_SET_EXPECT_PET = 2,	//!< ���������� param1 ����ID
			OPER_TYPE_CLEAR_EXPECT_PET = 3,	//!< ��������� param1 �Ƿ���(1:�ǣ�0:��
			OPER_TYPE_ROLL_DICE = 4,		//!< ҡ����
			OPER_TYPE_FETCH_GRID_REWARD = 5,//!< ��ȡ��ǰ���ӵĽ���
			OPER_TYPE_START_PET_BATTLE = 6, //!< ��ʼץ��ս��
			OPER_TYPE_TURN_TABLE = 7,		//!< ��ʼת��
			OPER_TYPE_ENTER_NEXT_LEVEL = 8, //!< ������һ��
			OPER_TYPE_SHOW_TRUNTABLE_REWARDS = 9,	//!< �鿴ת�̽���
			OPER_TYPE_TURN_TABLE_END = 10,	//!< ת�̶�������
			OPER_TYPE_ADD_EXP = 11,			//!< ����������ʱ,�ִ���ӻص��Ӿ���
			OPER_TYPE_SPECIAL_ITEM = 12,	//!< ʹ��������� param1 �������� param2 �������ѡ���³��������(1-6)������Ը��ѡ�����Ʒ��seq
			OPER_TYPE_SPECIAL_ITEM_NUM = 13,//!< ��ȡȫ�������������
			OPER_TYPE_FAST_MOVE_REWARD =14,	//!< ʹ�����з����뽱�� param1 �����ƶ���ÿ���index
		};

		short operate_type;
		short reserve_sh;
		int param1;
		int param2;
	};

	class SCPetMonopolyExpectPetChangeNotice	//!< 3662 ����ؾ�������
	{
	public:
		SCPetMonopolyExpectPetChangeNotice();
		MessageHeader header;

		int silver_expect_pet_id;
		int gold_expect_pet_id;
	};

	class SCPetMonopolyRollDice		//!< 3663 ����ؾ�Ͷ���ӽ������
	{
	public:
		SCPetMonopolyRollDice();
		MessageHeader header;

		short dice;					//!< ҡ���ӽ��[1,6];  ��������ʹ�����������
		short cur_grid_idx;			//!< �µĸ���λ��
	};

	class SCPetMonopolyCurGridChangeNotice	//!< 3664 ��ǰ������ݸı�֪ͨ
	{
	public:
		SCPetMonopolyCurGridChangeNotice();
		MessageHeader header;

		int cur_grid_idx;					//!< ��ǰ������
		MsgPetMonopolyGrid grid;			//!< �������
	};

	class SCPetMonopolyRewardRecord	: public IMessageSerializer	//!< 3665 ����ؾ��鿴��������
	{
	public:
		SCPetMonopolyRewardRecord();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT(reward_num);
			for (int i = 0; i < reward_num; ++i)
			{
				result = result && WRITE_INT(reward_list[i].item_id);
				result = result && WRITE_INT(reward_list[i].item_num);
				result = result && WRITE_SHORT(reward_list[i].is_bind);
				result = result && WRITE_SHORT(reward_list[i].param_length);
				if (reward_list[i].param_length > 0)
				{
					result = result && WRITE_STRN(reward_list[i].data, reward_list[i].param_length);
				}
				
				if (!result)
				{
					break;
				}
			}

			return result;
		}

		struct ItemInfo
		{
			int item_id;				//!< ��ƷID 
			int item_num;				//!< ��Ʒ����
			short is_bind;				//!< �Ƿ�� 1��  0�ǰ�
			short param_length;			//!< param����
			NetValueItemParamData data;	//!< param����
		};

		int reward_num;
		ItemInfo reward_list[PET_MONOPOLY_REWARD_RECORD_NUM];
	};

	class SCPetMonopolyEnterNextLevelNotice		//!< 3666 ������һ��֪ͨ �����ᷢ3660��
	{
	public:
		SCPetMonopolyEnterNextLevelNotice();
		MessageHeader header;

		int next_level;
	};

	struct MsgPetMonopolyTurntableItem	
	{
		short reward_type;	//!< ���ӽ�������  \see PET_MONOPOLY_GRID_REWARD
		short reserve_sh;	//!< ������

		int param1;			//!< �������ǳ���ʱ����ʾ����ID��Ϊ��������Ʒ����Ƭʱ����ʾ��Ʒ����ƬID
		int param2;			//!< ����������Ʒ����Ƭʱ����ʾ��Ʒ����Ƭ����
	};

	class SCPetMonopolyTurntable	//!< 3667 ת���·�
	{
	public:
		SCPetMonopolyTurntable();
		MessageHeader header;

		MsgPetMonopolyTurntableItem sector_list[PET_MONOPOLY_TURNTABLE_SECTOR_NUM]; //!< �����б� һ��8������
	};

	class SCPetMonopolyTurntableResult	//!< 3668 ת�̽���·�
	{
	public:
		SCPetMonopolyTurntableResult();
		MessageHeader header;

		int sector_idx;				//!< ָ�������idx
	};

	class SCPetMonopolySpecialItem		//!< 3669 �������������Ϣ�·�
	{
	public:
		SCPetMonopolySpecialItem();
		MessageHeader header;

		int double_surplus;
		int use_item_type;		//-1��ʾδʹ�õ���
		int special_item_num[PET_MONOPOLY_SPECIAL_ITEM_MAX];
	};

	class SCPetMonopolyWishCardGrid		//!< 3670 ����ؾ���Ը���滻����λ��
	{
	public:
		SCPetMonopolyWishCardGrid();
		MessageHeader header;

		int replace_num;
		int grid_idx[PET_MONOPOLY_MAX_REPLACE_GRID_NUM];
		MsgPetMonopolyGrid item_grid[PET_MONOPOLY_MAX_REPLACE_GRID_NUM];
	};
}

#pragma pack(pop)

#endif