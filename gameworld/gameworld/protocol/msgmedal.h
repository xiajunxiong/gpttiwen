#ifndef __MSG_MEDAL_HPP__
#define __MSG_MEDAL_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/medaldef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum MEDAL_REQ_TYPE
	{
		MEDAL_REQ_TYPE_INFO,					//!< ������Ϣ
		MEDAL_REQ_TYPE_PUT_ON,					//!< ����װ�� p1 grid_index,p2 column ,p3 index
		MEDAL_REQ_TYPE_TAKE_OFF,				//!< ����װ��  p1 grid_index,
		MEDAL_REQ_TYPE_EXCHANGE_MATERIAL,		//!< �һ����� p1 seq
		MEDAL_REQ_TYPE_DECOMPOSE,				//!< �ֽ�ѫ�� p1 column ,p2 index
		MEDAL_REQ_TYPE_LOTTREY,					//!< �齱 p1 seq p2 times
		MEDAL_REQ_TYPE_UPGRADE,					//!< �Ա������� p1 column,p2 index ,p3 consume_column,p4 consume_index
		MEDAL_REQ_TYPE_UPGRADE_2,				//!< �Դ������� p1 wear_index,		,p3 consume_column,p4 consume_index
		MEDAL_REQ_TYPE_SPE_EXCHANGE,			//!< ��������û� p1:column p2:index p3:sp_id p4:second_sp_id p5:is_wear(0:���� 1:������(index))
	};

	class CSMedalReq					//!< 2160
	{
	public:
		CSMedalReq();
		MessageHeader		header;

		int req_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
	};

	class CSMedalUpGradeSpecialReq				//!< 2171
	{
	public:
		CSMedalUpGradeSpecialReq();
		MessageHeader		header;

		int is_wear_grid;//�Ƿ񴩴�(0/1)
		int column;//��������
		int index;
		//�������
		int consume_column_spe;
		int consume_index_spe;
		//��ͨ����
		int consume_column;
		int consume_index;
	};

	class SCLottreyInfo					//!< 2166 �齱��ϸ�·�
	{
	public:
		SCLottreyInfo();
		MessageHeader		header;

		struct MedalReward
		{
			MedalReward():reward_bag_type(0), reward_bag_index(0) {}
			int reward_bag_type;
			int reward_bag_index;
		};
		int count;
		MedalReward reward_list[MEDAL_LOTTERY_TIMES_TYPE_10];//������Ϣ
	};

	class SCMedalInfo : public IMessageSerializer 			//!<2161
	{
	public:
		SCMedalInfo();
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length);

		struct GridItem
		{
			GridItem();

			int grid_index;
			ItemID			item_id;		//!< ��Ʒid
			short				num;			//!< ����
			char				is_bind;		//!< �Ƿ��
			char				has_param;		//!< �Ƿ��о�ֵ����
			UInt32			invalid_time;	//!< ����ʱ��
			short				param_length;	//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
			NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
		};

		int count;
		GridItem grid_info[MAX_MEDAL_WEAR_GRID];
	};

	class SCMedalFbSceneInfo						//!< ˮ������������Ϣ 2162
	{
	public:
		SCMedalFbSceneInfo();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<��ǰ����
		int is_has_monster;
	};

		
	class SCMedalFbKillBoss						//!< ѫ�¸�����ɱboss 2163
	{
	public:
		SCMedalFbKillBoss();
		MessageHeader		header;

		int scene_id;
	};


	class SCMedalDecomposeNotice					//!< ѫ�·ֽ�֪ͨ 2164
	{
	public:
		SCMedalDecomposeNotice();
		MessageHeader		header;

		int is_succ;
	};

	class CSMedalTaskPlayVideo	//!< 2165 ѫ�����񲥷��ܼ㶯��
	{
	public:
		CSMedalTaskPlayVideo();
		MessageHeader header;

		int video_id;	//!< ����ID
	};

	class SCMedalTakeOff	//!< 2167
	{
	public:
		SCMedalTakeOff();
		MessageHeader header;

		int out_column;
		int out_index;
	};

	class SCMedalUpgradeNotice	//!< 2168
	{
	public:
		SCMedalUpgradeNotice();
		MessageHeader header;

		short is_succ;
		short is_wear;
		int column;
		int index;
	};

	class SCMedalSpecialExchangeNotice		//!< 2169 ���ѫ���û�֪ͨ
	{
	public:
		SCMedalSpecialExchangeNotice();
		MessageHeader header;

		short is_wear;			//1:������(index) 0:����(column, index)
		short column;
		int index;
	};

	class CSMedalOneKeyGradeReq				//!< 2170
	{
	public:
		CSMedalOneKeyGradeReq();
		MessageHeader		header;

		short count;
		short column;
		BitMap<MEDAL_ONE_KEY_GRADE_MAXNUM> index_flag;	//MEDAL_ONE_KEY_GRADE_MAXNUM- 320 ��Χ[0-299]
	};
}

#pragma pack(pop)

#endif // __MSG_BABY_HPP__

