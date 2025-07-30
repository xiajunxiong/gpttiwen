#ifndef __MSG_JIE_YI_HPP__
#define __MSG_JIE_YI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commonsavedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum JIE_YI_REQ_RET_TYPE
	{
		JIE_YI_REQ_RET_TYPE_CREATE_CONFIRM,		//ȷ��
		JIE_YI_REQ_RET_TYPE_CREATE_CANCEL,			//ȡ��
		JIE_YI_REQ_RET_TYPE_CREATE_NAME_FINISH,		//�������
		JIE_YI_REQ_RET_TYPE_LEAVE,				//�뿪 p1 0��ȡ����1���뿪
		JIE_YI_REQ_RET_TYPE_KICK,				//����  p1 role_id
		JIE_YI_REQ_RET_TYPE_INVITE,				//����  p1 role_id
		JIE_YI_REQ_RET_TYPE_INVITE_APPLY,			//����ظ� p1 4211Э��zu_id  ,p2 �Ƿ�ͬ��
		JIE_YI_REQ_RET_TYPE_ZU_INFO,			//�����������Ϣ  p1 zu_id,����4207Э��
		JIE_YI_REQ_RET_TYPE_TAN_HE,			//����
		JIE_YI_REQ_RET_TYPE_TAN_HE_VOTE,			//����ͶƱ p1 target_role_id
	};


	class CSJieYiReq				//!< 4200
	{
	public:
		CSJieYiReq();
		MessageHeader header;

		int req_type;
		long long p1;
		int p2;
	}; 

	class CSJieYiCreate				//!< 4201 ����
	{
	public:
		CSJieYiCreate();
		MessageHeader header;

		JieYIZuName jie_yi_name;
	};

	class CSJieYiCreateName				//!< 4202  ����
	{
	public:
		CSJieYiCreateName();
		MessageHeader header;

		int key;
		JieYIZuName jie_yi_name;
	};

	class SCJieYiCreateInfo				//!< 4203   ���ڴ���������Ϣ
	{
	public:
		SCJieYiCreateInfo();
		MessageHeader header;
		
		int key;
		int is_name_finish;
		JieYIZuName jie_yi_name;
		int role_count;
		JieYiZuCreateRole role_info[JIE_YI_ZU_MAX_ROLE_ITEM_NUM];  //5
	};

	class SCJieYiCreateNameUpdate			//!< 4204  //�������ָ��ģ��������յ�
	{
	public:
		SCJieYiCreateNameUpdate();
		MessageHeader header;

		int key;
		JieYIZuName jie_yi_name;
	};

	class SCJieYiCreateConfirm			//!< 4205			//����ȷ�ϣ��������յ�
	{
	public:
		SCJieYiCreateConfirm();
		MessageHeader header;

		int key;
		int role_id[JIE_YI_ZU_MAX_ROLE_ITEM_NUM];		//!< �Ѿ�ȷ�ϵ����
	};

	class SCJieYiCreateCancel				//!< 4206
	{
	public:
		SCJieYiCreateCancel();			//����ȡ���������������յ�
		MessageHeader header;

		int type; //!< 0��ȡ����1�ǵ���
		int key;
		int role_id;
	};

	class SCJieYiZuInfo		//!< 4207			��������Ϣ
	{
	public:
		SCJieYiZuInfo();					//!< ��ɫ��������飬
		MessageHeader header;

		struct RoleInfo
		{
			JieYiZuRoleBaseInfo role_base_info;
			short is_zu_zhang;			//!< �Ƿ����鳤
			short has_fisrt_member_name;		//!< �Ƿ��Ѿ��Ĺ���Ա���֣���һ�����
			unsigned int  join_time;			//!< ����ʱ��
			unsigned int leave_end_time;		//!< ��ʱ����뿪�� �����뿪ʱ��, �ڵ�����۶��壬����ȷ�ϵ���������ҵ��ȷ�������24Сʱ�侲��
			JieYIZuMemberName member_name;
			unsigned int last_login_time;
			int online_state;
			short is_vote;			//�����Ա�Ƿ��Ѿ�ͶƱ��
			short tan_he_number_of_votes;			//���ͶƱ����
		};

		int info_type;  //1�ǲ�ѯ
		long long jie_yi_zu_id;
		long long create_time;
		JieYIZuName jie_yi_name;
		JieYIZuText jie_yi_text;
		unsigned int vote_end_time;

		int role_info_count;
		RoleInfo role_info[JIE_YI_ZU_MAX_ROLE_ITEM_NUM];
	};

	class CSJieYiZuMemberName  //!< 4208  ��Ա����
	{
	public:
		CSJieYiZuMemberName();
		MessageHeader header;

		JieYIZuMemberName member_name;
	};

	class CSJieYiZuText				//!< 4209 �鳤�޸�����
	{
	public:
		CSJieYiZuText();
		MessageHeader header;

		JieYIZuText text;
	};

	class CSJieYiZuName				//!< 4210 �鳤�޸Ľ�������
	{
	public:
		CSJieYiZuName();
		MessageHeader header;

		JieYIZuName jie_yi_name;
	};

	class SCJieYiInviteRoute				//!< 4211  ������ʱ����������˻��յ�
	{
	public:
		SCJieYiInviteRoute();
		MessageHeader header;
		
		long long jie_yi_zu_id;
		int role_id;
	};

	class SCJieYiRoleXuanYanList				//!< 4212  �Լ�����
	{
	public:
		SCJieYiRoleXuanYanList();
		MessageHeader header;

		struct XuanYanItem
		{
			JieYiZuRoleBaseInfo role_info;
			JieYIZuXuanYanText text;
		};

		int count;
		XuanYanItem xuan_yan_items[100];
	};

	class SCJieYiZuXuanYanList				//!< 4213  ��������
	{
	public:
		SCJieYiZuXuanYanList();
		MessageHeader header;

		struct XuanYanItem
		{
			long long jie_yi_zu_id;
			JieYIZuName zu_name;
			JieYIZuXuanYanText text;
		};

		int count;
		XuanYanItem xuan_yan_items[100];
	};

	class CSJieYiZuXuanYanReq				//!< 4214 ��������
	{
	public:
		CSJieYiZuXuanYanReq();
		MessageHeader header;
		
		short type;     //!< 0���Լ���1������
		short is_delete;     //!< 0��ӣ�1��ɾ��
		JieYIZuXuanYanText text;
	};

	class CSJieYiZuXuanYanReqList				//!< 4215 ���������б�
	{
	public:
		CSJieYiZuXuanYanReqList();
		MessageHeader header;

		int type;     //!< 0���Լ���1������
	};

	class SCJieYiZuLeaveNotice		//!< 4216  ������ �뿪֪ͨ
	{
	public:
		SCJieYiZuLeaveNotice();
		MessageHeader header;

		long long jie_yi_zu_id;
	};

	class SCJieYiZuRoleOnlineStatusNotice		//!< 4217  ������ ��Ա����
	{
	public:
		SCJieYiZuRoleOnlineStatusNotice();
		MessageHeader header;

		int role_id;
		unsigned int last_login_time;
		int online_status;
	};
}

#pragma pack(pop)

#endif
