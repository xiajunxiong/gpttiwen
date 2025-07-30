#ifndef __MSG_MOVE_ASSIST_H__
#define __MSG_MOVE_ASSIST_H__

#include "gamedef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	//�ƶ�Э������
	class SCMoveAssistReq
	{
	public:
		SCMoveAssistReq();
		MessageHeader header;

		unsigned int move_assist_id;	//Ѱ·����ģ��id
		int scene_id;			//����id
		int scene_key;			//����key

		ObjID obj_id;			//������obj_id
		short assist_type;		//��������  Ѱ·  ����Ѳ��  ����

		int speed;				//�����ߵ�ǰ�ٶ�

		short cur_pos_x;		//��ʼλ��
		short cur_pos_y;		//��ʼλ��
		short target_pos_x;		//Ŀ��λ��
		short target_pos_y;		//Ŀ��λ��
	};

	//�ƶ�Э������
	class CSMoveAssistRet
	{
	public:
		CSMoveAssistRet();
		MessageHeader header;

		unsigned int move_assist_id;
		int scene_id;
		int scene_key;

		ObjID obj_id;
		short assist_type;		//��������  Ѱ·  ����Ѳ��  ����

		short cur_pos_x;
		short cur_pos_y;
		short target_pos_x;
		short target_pos_y;

		int way_point_count;	//·��������

		struct WayPoint
		{
			static const int WAY_POINT_MAX_NUM = 120;
			short pos_x;
			short pos_y;
		};

		WayPoint way_point_list[WayPoint::WAY_POINT_MAX_NUM];
	};
}
#pragma pack(pop)
#endif