#ifndef __MSG_ROLE_JOIN_ACTIVITY_REWARD_HPP__
#define __MSG_ROLE_JOIN_ACTIVITY_REWARD_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSRoleJoinActivityRewardReq				//!< 3692 �ͻ�������
	{
	public:
		enum REQ_TYPE
		{
			REQ_TYPE_INFO = 0,						//!< ��ȡ��Ϣ (Ӧ���ò���)
			REQ_TYPE_GET_REWARD = 1,				//!< ��ȡ���� activity_type : �����
			REQ_TYPE_ONE_KEY_FINISH = 2,			//!< һ����ɹ���, activity_type: λ�����ȡ��Ҫһ����ɵ�ϵͳ
		};

		enum ONE_KEY_FINISH_TYPE
		{
			ONE_KEY_FINISH_TYPE_SHIMEN = 0,		// ʦ��(�ͽ�)����
			ONE_KEY_FINISH_TYPE_SANJIE = 1,		// �����ħ
			ONE_KEY_FINISH_TYPE_SHENGWANG = 2,	// ��������
			ONE_KEY_FINISH_TYPE_XUNXIAN = 3,	// Ѱ��(��������)
			ONE_KEY_FINISH_TYPE_JIUZHOU = 4,	// �����ر�
			ONE_KEY_FINISH_TYPE_HUABEN = 5,		// ���黰��

			ONE_KEY_FINISH_TYPE_MAX,
		};
		UNSTD_STATIC_CHECK(ONE_KEY_FINISH_TYPE_MAX < 32);

		CSRoleJoinActivityRewardReq();
		MessageHeader		header;

		int req_type;								// ����������� 
		int activity_type;
	};

	class SCRoleJoinActivityRewardInfo				//!< 3693 ��ɫ����������Ϣ
	{
	public:
		SCRoleJoinActivityRewardInfo();
		MessageHeader		header;

		int join_flag;							//!< ������[��λ] 
		int get_reward_flag;					//!< ����ȡ�������[��λ]
	};


	static const int ONE_KEY_FINISH_MAX_REWARDS_TYPE_NUM = 100;		// һ������������������
	static const int ONE_KEY_FINISH_MAX_MONEY_TYPE_NUM = 2;			// һ���������������

	

	class SCRoleOneKeyFinishRewardInfo
	{
	public:
		SCRoleOneKeyFinishRewardInfo();
		MessageHeader header;

		struct ItemInfo
		{
			ItemInfo() :item_id(0), is_bind(0), num(0)
			{}

			ItemID item_id;
			short is_bind;
			int num;
		};

		int activity_types;
		unsigned int exp;
		int money[ONE_KEY_FINISH_MAX_MONEY_TYPE_NUM];		// ��ʾ�ӵĻ���, [0]-ͭ��,[1]-����
		int count;
		ItemInfo rewards[ONE_KEY_FINISH_MAX_REWARDS_TYPE_NUM];
	};


}
#pragma pack(pop)

#endif // __MSG_ROLE_JOIN_ACTIVITY_REWARD_HPP__

