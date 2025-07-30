#ifndef __MSG_HUAN_SHOU_HPP__
#define __MSG_HUAN_SHOU_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/huanshoudef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/itemlistparam.h"

#pragma pack(push, 4) 

namespace Protocol
{

	class CSHuanShouReq			//!< 5360 ��������
	{
	public:
		CSHuanShouReq();
		MessageHeader header;

		enum HUAN_SHOU_REQ_TYPE
		{
			HUAN_SHOU_REQ_TYPE_INFO = 0,				//!< �����·�������Ϣ
			HUAN_SHOU_REQ_TYPE_OPEN_EGG = 1,			//!< ����򿪻��޵� p1:bag_column p2:bag_index
			HUAN_SHOU_REQ_TYPE_STR_LEVEL = 2,			//!< ����ǿ������ p1:huanshou_id
			HUAN_SHOU_REQ_TYPE_LEARN_SKILL = 3,			//!< ����ѧϰ���� p1:huanshou_id p2:bag_index p3:lock_flag
			HUAN_SHOU_REQ_TYPE_SAVE_SKILL = 4,			//!< ���󱣴漼�� p1:huanshou_id p2:is_save(0:��� ��0:����)
			HUAN_SHOU_REQ_TYPE_P_POINT_LEVEL_UP = 5,	//!< ����ѧϰǱ�ܵ�λ p1:huanshou_id p2:point_index
			HUAN_SHOU_REQ_TYPE_LIAN_DAN = 6,			//!< ��������(Ǳ�ܹ�)
			HUAN_SHOU_REQ_TYPE_FU_SHEN = 7,				//!< ������ p1:huanshou_id
			HUAN_SHOU_REQ_TYPE_CANCEL_FU_SHEN = 8,		//!< ����ȡ������
			HUAN_SHOU_REQ_TYPE_ANNOUNCEMENT_INFO = 9,	//!< ���󹫸���Ϣ
			HUAN_SHOU_REQ_TYPE_SINGLE_HUANSHOU = 10,	//!< �����·�����������Ϣ p1.huanshou_id
			HUAN_SHOU_REQ_TYPE_USE_HUAN_SHOU_IMAGE = 11,//!< �����Ƿ�ʹ�û������� p1:(0.��ʹ�� 1.ʹ��)
		};

		int op_type;
		int param1;
		int param2;
		int param3;
	};

	struct HuanShouSingleInfoPro
	{
		HuanShouSingleInfoPro() { this->Reset(); }
		HuanShouSingleInfoPro(const RoleHuanShouData & data)
		{
			huanshou_id = 0;
			str_level = data.str_level;
			new_skill_gird_idx = data.new_skill_gird_idx;
			new_skill_id = data.new_skill_id;
			point_level_count = 0;
			memcpy(skill_list, data.skill_list, sizeof(skill_list));
			memcpy(point_level_list, data.point_level_list, sizeof(point_level_list));
		}

		void Reset()
		{
			huanshou_id = 0;
			str_level = 0;
			new_skill_gird_idx = -1;
			new_skill_id = 0;
			reserve_ch = 0;
			point_level_count = 0;
			reserve_sh = 0;
			memset(skill_list, 0, sizeof(skill_list));
			memset(point_level_list, 0, sizeof(point_level_list));
		}

		bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteShort(huanshou_id, buffer, buffer_size, out_length);
			result = result && WriteUShort(str_level, buffer, buffer_size, out_length);
			result = result && WriteShort(new_skill_gird_idx, buffer, buffer_size, out_length);
			result = result && WriteUShort(new_skill_id, buffer, buffer_size, out_length);

			result = result && WriteStrN((const char *)skill_list, sizeof(skill_list), buffer, buffer_size, out_length);

			result = result && WriteUChar(point_level_count, buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)point_level_list, sizeof(point_level_list[0]) * point_level_count, buffer, buffer_size, out_length);

			return result;
		}

		short huanshou_id;
		unsigned short str_level;										//����ǿ���ȼ�(������Ϊ1��)
		short new_skill_gird_idx;										//����������ݴ�
		unsigned short new_skill_id;									//����������ݴ�
		char reserve_ch;
		unsigned char point_level_count;								//����Ǳ�ܵ�λ�б�����
		short reserve_sh;	
		unsigned short skill_list[MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM];	//���޴��鼼���б�
		unsigned char point_level_list[MAX_HUAN_SHOU_CUR_POTENTIAL_POINT_NUM];	//����Ǳ�ܵ�λ�ȼ��б�
	};


	class SCHuanShouAllInfo : public IMessageSerializer		//!< 5361 ���л�����Ϣ�·�
	{
	public:
		SCHuanShouAllInfo();
		virtual ~SCHuanShouAllInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			egg_count = GetMin(egg_count, (short)ARRAY_ITEM_COUNT(egg_cur_open_times_list));
			result = result && WriteShort(egg_count, buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)egg_cur_open_times_list, sizeof(egg_cur_open_times_list[0]) * egg_count, buffer, buffer_size, out_length);

			short count = GetMin(huanshou_count, (short)ARRAY_ITEM_COUNT(huanshou_list));
			result = result && WriteShort(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count; i++)
			{
				HuanShouSingleInfoPro & info = huanshou_list[i];
				result = result && info.Serialize(buffer, buffer_size, out_length);
			}

			return result;
		}

		short egg_count;
		short huanshou_count;
		int egg_cur_open_times_list[MAX_HUAN_SHOU_EGG_ID_NUM];	//���ޱ��״���		
		HuanShouSingleInfoPro huanshou_list[MAX_HUAN_SHOU_NUM];
	};

	class SCHuanShouSingleInfo	: public IMessageSerializer		//!< 5362 ����������Ϣ�·�
	{
	public:
		SCHuanShouSingleInfo();
		virtual ~SCHuanShouSingleInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && huan_shou_info.Serialize(buffer, buffer_size, out_length);

			return result;
		}

		HuanShouSingleInfoPro huan_shou_info;
	};

	class SCHuanShouNoticeInfo		//!< 5363 ������Ϣ֪ͨ�·�
	{
	public:
		SCHuanShouNoticeInfo();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_ACTIVE = 0,				// ���޼��� p1.huanshou_id 
			NOTICE_TYPE_ACTIVE_CONVERT = 1,		// ���޼���ת�� p1.huanshou_id
			NOTICE_TYPE_STR_LEVEL = 2,			// ����ǿ��(����) p1.huanshou_id p2.str_level
			NOTICE_TYPE_LEARN_SKILL = 3,		// ����ѧϰ���� p1.huanshou_id p2.new_skill_grid_idx p3.new_skill_id
			NOTICE_TYPE_CLEAR_LEARN_SKILL = 4,	// �������׼��ѧϰ���� p1.huanshou_id
			NOTICE_TYPE_SAVE_LEARN_SKILL = 5,	// ���ޱ���ѧϰ���� p1.huanshou_id p2.new_skill_grid_idx p3.new_skill_id
			NOTICE_TYPE_LEARN_P_POINT = 6,		// ����ѧϰ��λ p1.huanshou_id p2.point_index p3.point_level
			NOTICE_TYPE_RARE_COUNT = 7,			// ���޵�ϡ�б��� p1:egg_id p2:cur_open_egg_times
		};

		short notice_type;
		short param1;
		int param2;
		int param3;
	};

	class CSHuanShouDecomposeReq		//!< 5364 �������Ƿֽ�����
	{
	public:
		CSHuanShouDecomposeReq();
		MessageHeader header;

		int bag_column;
		short decompose_list[ItemNamespace::MAX_HUAN_SHOU_GRID_NUM];	//�ֽ�����б�
	};

	class SCHuanShouOtherInfo			//!< 5365 ����������Ϣ�·�
	{
	public:
		SCHuanShouOtherInfo();
		MessageHeader header;

		char fushen_id;								//��ʱ����Ļ���ID(id��ͬ�����)(-1:û����)
		char is_use_huanshou;						//�Ƿ�ʹ�û������� 0:��ʹ�� 1:ʹ�û�������
		short cur_lian_dan_count;					//��¯�ڴ�ʱ���������Ĵ���
		unsigned int start_lian_dan_timestamp;		//��ʼ����ʱ���
	};

	class SCHuanShouAnnouncementInfo	//!< 5366 ���޹�����Ϣ�·�
	{
	public:
		SCHuanShouAnnouncementInfo();
		MessageHeader header;

		short reserve_sh;
		short ann_count;															//��ǰ��������
		HuanShouAnnouncementInfo ann_list[MAX_HUAN_SHOU_ANNOUNCEMENT_INFO_NUM];		//�����б�
	};


	class SCHuanShouBagInfoAck : public IMessageSerializer  //!< 5370 ���ޱ���������Ϣ�·�
	{
	public:
		SCHuanShouBagInfoAck();
		virtual ~SCHuanShouBagInfoAck() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);
			for (int i = 0; i < item_count && i < ARRAY_ITEM_COUNT(item_list); ++i)
			{
				result = result && WriteUShort(item_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_market_buy_flag, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].num, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_bind, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].has_param, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);

				if (1 == item_list[i].has_param)
				{
					result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
					result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);
				}
			}

			return result;
		}

		short		reserve_sh;
		short		item_count;											//!< ��Ʒ����
		InfoType2	item_list[ItemNamespace::MAX_HUAN_SHOU_GRID_NUM];	//!< ��Ʒ�б�  MAX_DRAGON_SOUL_BAG_NUM 200
	};

	class SCHuanShouBagSingleInfo : public IMessageSerializer  //!< 5371 ���ޱ���������Ϣ�·�
	{
	public:
		SCHuanShouBagSingleInfo();
		virtual ~SCHuanShouBagSingleInfo() {}
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(put_reason, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(index, buffer, buffer_size, out_length);
			result = result && WriteChar(is_market_buy_flag, buffer, buffer_size, out_length);
			result = result && WriteInt(num, buffer, buffer_size, out_length);
			result = result && WriteChar(is_bind, buffer, buffer_size, out_length);
			result = result && WriteChar(has_param, buffer, buffer_size, out_length);
			result = result && WriteUInt(invalid_time, buffer, buffer_size, out_length);
			result = result && WriteLL(item_unique_id, buffer, buffer_size, out_length);
			if (1 == has_param)
			{
				result = result && WriteShort(param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(param_data, param_length, buffer, buffer_size, out_length);
			}

			return result;
		}

		short				reason;					//!< �ı��ԭ��
		short				put_reason;				//!< �����Ʒ����Դ  \see PUT_REASON_TYPE
		ItemID				item_id;				//!< ��Ʒid
		short				index;					//!< ��Ʒ�ڱ����е���� 
		short				num;					//!< ����
		long long           item_unique_id;			//!< ����ΨһID
		char				is_market_buy_flag;		//!< �Ƿ���г������ʶ 0:�� 1:���г�����
		char				reserve_ch;
		char				is_bind;				//!< �Ƿ��
		char				has_param;				//!< �Ƿ��о�ֵ����
		UInt32				invalid_time;			//!< ����ʱ��
		short				param_length;			//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
		NetValueItemParamData param_data;			//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
	};
	

}

#pragma pack(pop)


#endif //__MSG_HEART_SKILL_HPP__