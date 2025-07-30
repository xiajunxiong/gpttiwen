#ifndef __MSG_QING_YUAN_H__
#define __MSG_QING_YUAN_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "servercommon/qingyuandef.h"

#include "servercommon/weddingdef.hpp"

#pragma pack(push) 
#pragma pack(4)
namespace Protocol
{
	//////����
	//��Ҳ���  4240
	enum WeddingFBReqType
	{
		WEDDING_FB_REQ_TYPE_FB_COMMON = 0,	//������������   p_1:��������  p_2:��������

		WEDDING_FB_REQ_TYPE_REGIST = 1,				//����--ԤԼ		p_1:ԤԼʱ���
		WEDDING_FB_REQ_TYPE_REGIST_OFF = 2,			//����--���ԤԼ
		WEDDING_FB_REQ_TYPE_INVITE = 3,				//����--����		p_1:Ŀ��uid
		WEDDING_FB_REQ_TYPE_APPLY = 4,				//����--����		p_1:����id
		WEDDING_FB_REQ_TYPE_MARK = 5,				//����--���봦��	p_1:Ŀ��uid	p_2:0:�ܾ�	1:ͬ��
	
		WEDDING_FB_REQ_TYPE_REQ_REGIST_LIST = 6,	//����--����ԤԼ�б�(������)
		WEDDING_FB_REQ_TYPE_REQ_SUB_REGIST_LIST = 7,//����--ԤԼ�б� ���� p_1:1:���� 0:ȡ������
		WEDDING_FB_REQ_TYPE_REQ_INVITE_LIST = 8,	//����--���������б�

		WEDDING_FB_REQ_TYPE_OTHER_INFO = 9,			//����--������Ϣ
		WEDDING_FB_REQ_TYPE_INVITE_SIZE_BUY = 10,	//����--��չ��������
		WEDDING_FB_REQ_TYPE_XUN_YOU = 11,			//Ԥ��Ѳ��			p_1:Ѳ������

		WEDDING_FB_REQ_TYPE_WEDDING_SHOW_INFO = 12,	//����--չʾ��Ϣ	p_1:����id
		WEDDING_FB_REQ_TYPE_WEDDING_OPEN_INFO = 13,	//����--������Ϣ

		WEDDING_FB_REQ_TYPE_REQ_SUB_ACT = 100,//����--С��Ϸ����	p_1:С��Ϸ����	p_2:��������
	};
	class CSWeddingFBReq
	{
	public:
		CSWeddingFBReq();
		MessageHeader		header;

		int req_type;
		int param_1;
		int param_2;
	};

	//������Ϣ  4241
	class SCWeddingFBSceneInfo
	{
	public:
		SCWeddingFBSceneInfo();
		MessageHeader		header;

		int m_is_finish;	//������ɱ��

		WeddingID m_wedding_id;	//����id

		unsigned int m_fb_begin_time;	//������ʼʱ��
		unsigned int m_fb_end_time;		//��������ʱ��

		int m_link_seq;		//��ǰ����
		unsigned int m_link_begin_time;	//������ʼʱ��
		unsigned int m_link_end_time;	//���ڽ���ʱ��

		int m_wedding_heat;	//�����ȶ�

		//��Ե��Ϣ δ��
		int xun_you_type;		//Ѳ������

		int host_uid[WeddingFB::HOST_NUM];			//������uid[2]
		GameName host_name[WeddingFB::HOST_NUM];	//����������[2]
		int host_avatar_type[WeddingFB::HOST_NUM];	//����������[2]
	};

	//������ɫ��Ϣ  4242
	class SCWeddingFBRoleInfo
	{
	public:
		SCWeddingFBRoleInfo();
		MessageHeader		header;

		unsigned int meal__last_fetch_time;//�ò�  �ϴ��ò�ʱ��

		//������������[����]
		int count;
		int commom_param[WeddingFB::COMMON_OPER_TYPE_MAX];
	};

	//�����㲥		4243
	enum WeddingFBSyncType
	{
		WEDDING_FB_SYNC_TYPE_BLESS_SHOP = 0,	//����ף����Ʒ֪ͨ p:��Ʒindex
		WEDDING_FB_SYNC_TYPE_HEAT,				//�ȶ�֪ͨ p:��ǰ�ȶ�
	};
	class SCWeddingFBSync
	{
	public:
		SCWeddingFBSync();
		MessageHeader		header;

		int sync_type;
		int param;
	};
	//���� С��Ϸ Ѱ���ٰ�   4251
	class SCWeddingSubAct_XubQingMiAi
	{
	public:
		SCWeddingSubAct_XubQingMiAi();
		MessageHeader		header;
	
		int poi_x;
		int poi_y;

		int fetch_flag;
	};

	//���� С��Ϸ ������ӡ	4252
	class SCWeddingSubAct_XinXinXiangYin
	{
	public:
		SCWeddingSubAct_XinXinXiangYin();
		MessageHeader		header;

		int fetch_flag;
	};
	//���� С��Ϸ ��������	4253
	class SCWeddingSubAct_BaoWeiXiuQing
	{
	public:
		SCWeddingSubAct_BaoWeiXiuQing();
		MessageHeader		header;
		
		struct RewardInfo
		{
			RewardInfo()
			{
				npc_id = 0;
				is_fetch = 0;
			}

			int npc_id;		//npc_id
			int is_fetch;	//��������ȡ(�ѻ���)
		};

		int count;
		RewardInfo info_list[WeddingFB::SUB_ACT_BAO_WEI_XIU_QIU_MAX_CFG_SIZE];
	};

	//���� С��Ϸ ��������	4254
	class SCWeddingSubAct_ZaoShengGuiZi
	{
	public:
		SCWeddingSubAct_ZaoShengGuiZi();
		MessageHeader		header;

		int fetch_flag;
	};

	//����--ԤԼ�б�		4245
	class SCWeddingRegistList
	{
	public:
		SCWeddingRegistList();
		MessageHeader		header;

		//��ԤԼʱ���  0: δԤԼ
		unsigned int regist_time;

		//ʱ���б�--�ѱ�ԤԼ
		int count;
		unsigned int be_regist_time_list[WeddingFB::WEDDING_INFO_SIZE];
	};

	//����--�����б�		4246
	class SCWeddingInviteList
	{
	public:
		SCWeddingInviteList();
		MessageHeader		header;

		struct InviteInfo
		{
			InviteInfo()
			{
				role_uid = 0;
				memset(role_name, 0, sizeof(role_name));
			}
			int role_uid;
			GameName role_name;
		};

		WeddingID wedding_id;	//����id
		int wedding_begin_time;	//����ʱ��
		int buy_invite_times;	//����չ����

		int host_uid_[WeddingFB::HOST_NUM];

		//�������б�
		int is_total;		//1:ȫ��  0:����
		int count;
		InviteInfo invite_list[WeddingFB::MAX_INVITE_SIZE];
	};

	//����--�����б�		4247
	class SCWeddingApplyList
	{
	public:
		SCWeddingApplyList();
		MessageHeader		header;

		struct ApplyInfo
		{
			ApplyInfo()
			{
				role_uid = 0;
				memset(role_name, 0, sizeof(role_name));
			}
			int role_uid;
			GameName role_name;
		};

		//�����б�
		int is_total;		//1:ȫ��  0:����
		int count;
		ApplyInfo apply_list[WeddingFB::MAX_APPLY_SIZE];
	};

	//����--֪ͨ		4248
	enum WeddingChangeType
	{//																����1
		WEDDING_CHANGE_TYPE_REGIST_ADD,			//ԤԼ�б�--����	�ѱ�ԤԼʱ��
		WEDDING_CHANGE_TYPE_REGIST_REMOVE,		//ԤԼ�б�--�Ƴ�	�ѱ�ԤԼʱ��
		WEDDING_CHANGE_TYPE_ROLE_REGIST_CHANGE,	//����ԤԼʱ��		ԤԼʱ��
		WEDDING_CHANGE_TYPE_APPLY_REMOVE,		//�����б�--�Ƴ�	������uid
	};
	class SCWeddingChange
	{
	public:
		SCWeddingChange();
		MessageHeader		header;

		int change_type;
		int param_1;
	};
	//����--������Ϣ	4249
	class SCWeddingBaseInfo
	{
	public:
		SCWeddingBaseInfo();
		MessageHeader		header;

		unsigned int begin_time;//����ʱ��

		int buy_invite_times;	//����չ����
	};
	//����--�����㲥	4250
	class SCWeddingStatusSync
	{
	public:
		SCWeddingStatusSync();
		MessageHeader		header;

		WeddingID wedding_id;			//����id
		unsigned int begin_time;		//����ʱ��
	};

	//����--չʾ��Ϣ(���뺯)	4244
	class SCWeddingShowInfo
	{
	public:
		enum StatusType
		{
			STATUS_TYPE_DEF = 0,//��
			STATUS_TYPE_APPLIED,//������
			STATUS_TYPE_INVITED,//�ѱ�����
		};
	public:
		SCWeddingShowInfo();
		MessageHeader		header;

		int wedding_id;			//����id
		unsigned int begin_time;//����ʱ��

		int status;

		//��������Ϣ
		struct HostShowInfo
		{
			HostShowInfo()
			{
				uid = 0;
				memset(role_name, 0, sizeof(role_name));
				appearance.Reset();
			}

			int uid;
			GameName role_name;		//��ɫ��
			Appearance appearance;	//���
		};
		int host_count;
		HostShowInfo host_list[WeddingFB::HOST_NUM];
	};

	//Ѳ�� ���� ״̬��Ϣͬ��  4450
	class SCXunYouCarStatusInfoSync
	{
	public:
		SCXunYouCarStatusInfoSync();
		MessageHeader		header;

		ObjID obj_id;	//����obj_id
		short is_stop;	//�ǵȴ�״̬
	};

	enum QING_YUAN_REQ_TYPE
	{
		QING_YUAN_REQ_TYPE_INVITE,		//!< ����  p1 role_id ��p2 ��Ե����
		QING_YUAN_REQ_TYPE_INVITE_REPLY,		//!< ����  p1 role_id,p2 �Ƿ�ͬ��
		QING_YUAN_REQ_TYPE_GIFT,		//!< ���ͶԷ����� p1 ��������
		QING_YUAN_REQ_TYPE_GIFT_FETCH,		//!< ��ȡ�����״ν���
		QING_YUAN_REQ_TYPE_GIFT_FETCH_DAY,		//!< ��ȡ����ÿ�ս���
		QING_YUAN_REQ_TYPE_SKILL_NOT_EFFECT_FLAG,		//!< p1 ���ü��ܲ���Ч��־�������ƣ���λ��1��Ӧ�ڼ������ܲ���Ч
		QING_YUAN_REQ_TYPE_LEAVE,		//!< p1 ����Է����
		QING_YUAN_REQ_TYPE_LEAVE_REPLY,		//!<�ظ�������� p1 �Ƿ�ͬ��
		QING_YUAN_REQ_TYPE_LEAVE_SINGLE,		//!< ���߳���7�죬������� 
		QING_YUAN_REQ_TYPE_LEAVE_FORCE,		//!< ǿ�����
		QING_YUAN_REQ_TYPE_LEAVE_CANCEL,		//!< ȡ�����
	};

	class CSQingYuanReq				//!< 4260
	{
	public:
		CSQingYuanReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCQingYuanInviteRoute				//!< 4261 ��Ե����ת��
	{
	public:
		SCQingYuanInviteRoute();
		MessageHeader header;

		int item_type;
		int inviter_role_id;
		int be_inviter_role_id;
	};

	class SCQingYuanInfo		//!< 4262			
	{
	public:
		SCQingYuanInfo();					
		MessageHeader header;

		struct RoleInfo
		{
			int role_id;
			unsigned int single_or_force_leave_end_time;   //ʱ�䵽���Զ������Ե
			int gift_type;			//��ͨ�¿�����
			int fetch_gift_flag;		// bit 0 �Ƿ���ȡ��ͨ������bit 1 �Ƿ���ȡ���콱��
			int gift_type_end_time;		//��ͨ�¿�����ʱ��
			unsigned int skill_no_effect_flag;
		};


		long long qing_yuan_id;
		unsigned int create_time;			//����ʱ��
		int has_hun_yan;			//�Ƿ�ٰ������
		unsigned int leave_end_time;   //ʱ�䵽���Զ������Ե
		RoleInfo role_info[QING_YUAN_MAX_ROLE_NUM];
	};


	class SCQingYuanLeaveRoute				//!< 4263 ��ԵЭ�����ת��
	{
	public:
		SCQingYuanLeaveRoute();
		MessageHeader header;
	};

	class SCQingYuanInviteRet				//!< 4264 ���뷵��
	{
	public:
		SCQingYuanInviteRet();
		MessageHeader header;

		int target_role_id;
		int is_agree;
	};

	class SCQingYuanLeaveNotice		//!< 4265  ��Ե�뿪֪ͨ
	{
	public:
		SCQingYuanLeaveNotice();
		MessageHeader header;

		long long qing_yuan_id;
	};

	class SCQingYuanCreateNotice		//!< 4266  ��Ե�����㲥
	{
	public:
		SCQingYuanCreateNotice();
		MessageHeader header;

		int role_id[QING_YUAN_MAX_ROLE_NUM];
	};
}

#pragma pack(pop)

#endif