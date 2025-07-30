#ifndef __MOVE_ASSIST_HPP__
#define __MOVE_ASSIST_HPP__

#include "gamedef.h"
#include "protocol/msgmoveassist.h"

static const int MOVE_ASSIXT_WAY_POINT_NUM = 120;		//�����ƶ� ·��������

class Character;

//�г�����ɾ��
class MoveAssist
{
	friend class Character;
public:
	enum MOVE_ASSIST_STATUS_TYPE
	{
		MOVE_ASSIST_STATUS_TYPE_NONE = 0,			//δ����
		MOVE_ASSIST_STATUS_TYPE_WAIT_WAY_RET = 1,	//�ȴ�·���㷵��
		MOVE_ASSIST_STATUS_TYPE_WAY_MOVE = 2,		//��·���ƶ�
		MOVE_ASSIST_STATUS_TYPE_WAIT_MOVE = 3,		//��ͣ��·���ƶ�

		MOVE_ASSIST_STATUS_TYPE_MAX,
	};

	enum MOVE_ASSIST_REQ_TYPE
	{
		MOVE_ASSIST_REQ_TYPE_NONE = 0,		//	��Ч
		MOVE_ASSIST_REQ_TYPE_MOVE_TO = 1,	//	�ƶ���Ŀ���
		MOVE_ASSIST_REQ_TYPE_AROND = 2,		//	������Ѳ��
		MOVE_ASSIST_REQ_TYPE_EMPLOY = 3,	//	����

		MOVE_ASSIST_REQ_TYPE_MAX,
	};
public:
	int GetMoveAssistStatus() { return m_move_assixt_status; };

	//�����߼�������Update���ӳ�ִ�� ��ȷ���������¼�������ѭ��
	void Update(unsigned long interval, time_t now_second);

	//����Ŀ�ĵ�  �����ƶ��������غ������һ��
	void SetDestination(Posi target_pos, int assist_type);
	//���Ŀ�ĵ�
	void ReSetDestination();
	//��ͣ
	void MovePause();
	//����
	void MoveContinue();

	//�¼�
	void OnMoveToOther();	//���ƶ��������ƶ�����

	//�����ƶ�����
	void OnMoveAssistRet(Protocol::CSMoveAssistRet & ret_info);
protected:
	//�����߼�������Update���ӳ�ִ�� ��ȷ���������¼�������ѭ��
	//�����ƶ���������
	void SendMoveAssisReq();
	//����·��
	void SetWayPointList(int way_point_num, const Posi * way_point_list);

	//TODO:��ʱ�¼�
	UNSTD_STATIC_CHECK(4 == MOVE_ASSIST_STATUS_TYPE_MAX);



	Character * m_this_cha;
	unsigned int m_move_assist_id;

	//�ƶ�����-״̬
	int m_move_assixt_status;		//��ǰ״̬
	//(TODO:���ö�Ӧ��ʱ�¼�)
	//time_t m_status_begin_time;	//��ǰ״̬��ʼʱ��
	//time_t m_status_end_time;		//��ǰ״̬��ʼʱ��

	int m_assist_type;				//��������
	Posi m_target_posi;				//Ŀ�ĵ�

	//·����
	int m_way_point_count;
	Posi m_way_point_list[MOVE_ASSIXT_WAY_POINT_NUM];

	//�¸�·����index
	int m_next_way_point_index;

	//״̬����
	//�ȴ�·���㷵��  �ٴη�������Э��ʱ��
	time_t m_next_move_assist_req_time;
	//�����ƶ� �ƶ�ʧ�� ����ʱ����
	time_t m_next_move_assist_move_time;


private:
	MoveAssist(Character* self_cha, unsigned int move_assist_id);
	~MoveAssist();

	void* operator new(size_t c);
	void operator delete(void* m);
};

#endif