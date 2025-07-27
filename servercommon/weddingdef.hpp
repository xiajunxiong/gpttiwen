#ifndef __WEDDING_DEF_HPP__
#define __WEDDING_DEF_HPP__

#include "servercommon/servercommon.h"

/////////////////////////////// ��Ե---���� /////////////////////////////
//����
//SCENE_TYPE_WEDDING_FB = 42,
//FB_TYPE_WEDDING_FB = 36,	
//BATTLE_MODE_WEDDING_FB,78	
//����id
typedef int WeddingID;

namespace WeddingFB
{


	//�׶�����
	enum PhaseType
	{
		PHASE_TYPE_INVALID = 0,		//				�����߼�
		PHASE_TYPE_BROADCAST = 1,	// �����׶�:	��������
		PHASE_TYPE_WARM = 2,		// Ԥ�Ƚ׶�:	��
		PHASE_TYPE_RITE = 3,		// ���ý׶�:	��
		PHASE_TYPE_MEAL = 4,		// �òͽ׶�:	��
		PHASE_TYPE_SUB_ACT = 5,		// ��׶�:	P_1:�����(С��Ϸ)

		PHASE_TYPE_MAX,
	};

	//////�����(С��Ϸ)
	enum SubActType
	{//													������ɫ����		��������
		SUB_ACT_TYPE_XUN_QING_MI_AI = 1,// Ѱ���ٰ�		p:������ȡ���		��
		SUB_ACT_TYPE_XIN_XIN_XIANG_YIN,	// ������ӡ		p:������ȡ���		��
		SUB_ACT_TYPE_BAO_WEI_XIU_QIU,	// ��������		p:������ȡ���[λ]	����index
		SUB_ACT_TYPE_ZAO_SHENG_GUI_ZI,	// ��������		p:������ȡ���[λ]	����index

		SUB_ACT_TYPE_MAX,
	};

	//������������
	enum CommonOperType
	{//												��������	��������		��������
		COMMON_OPER_TYPE_ENTER_SCENE,	//���볡��  cs:�ɽ���	�������		��Ч ��enterfb
		COMMON_OPER_TYPE_CANDIES,		//��ϲ��	cs:����ȡ	��ȡ����		��
		COMMON_OPER_TYPE_RITE,			//���ö���	c:����		�Ѳ���			����������ֵ
		COMMON_OPER_TYPE_MEAL,			//�ò�		cs:����ȡ	��ȡ����		��
		COMMON_OPER_TYPE_BLESS,			//ף��		cs:�ɹ���	��				��Ʒindex
		COMMON_OPER_TYPE_BARRAGE,		//��Ļ		cs:�ɷ���	��������		��Ч ��chat
		COMMON_OPER_TYPE_HEAT_REWARD,	//�ȶȽ���	������		��ȡ��־(λ)	�������	

		COMMON_OPER_TYPE_MAX,
	};

	enum WeddingRegistStatus
	{
		WEDDING_REGIST_STATUS_NULL = 0,		//δԤԼ
		WEDDING_REGIST_STATUS_NOW,			//��ԤԼ
		WEDDING_REGIST_STATUS_DONE,			//�Ѿٰ�
	};

	//����ԤԼ�����С
	static const int WEDDING_INFO_SIZE = 200;

	//�������б��С
	static const int HOST_NUM = 2;
	//���������б��С
	static const int MAX_INVITE_SIZE = 100;
	//�����б��С
	static const int MAX_APPLY_SIZE = 200;

	//���� С��Ϸ
	//�������� �ؿ���
	static const int SUB_ACT_BAO_WEI_XIU_QIU_MAX_CFG_SIZE = 4;

////// ���
#pragma pack(push, 4)
	//��������Ϣ
	struct HostInfo
	{	
		HostInfo()
		{
			this->Reset();
		}

		void Reset()
		{
			uid = 0;
			memset(name, 0, sizeof(name));
			memset(hold_list, 0, sizeof(hold_list));
			appearance.Reset();
		}

		int uid;		//uid
		
		//��һ����������Ϣ  ��ֹ ����δ����ʱ ȡ��������
		GameName name;	//��ɫ��
		int hold_list[10];
		UNSTD_STATIC_CHECK(52 == sizeof(Appearance));
		Appearance appearance;
	};
	typedef char HostInfoHex[sizeof(HostInfo) * 2 + 1];
	UNSTD_STATIC_CHECK(sizeof(HostInfoHex) < 512);
	//�����б�
	struct InviteInfo
	{
		InviteInfo()
		{
			this->Reset();
		}

		void Reset()
		{
			count = 0;
			memset(invite_uid_, 0, sizeof(invite_uid_));
		}
		int count;
		int invite_uid_[WeddingFB::MAX_INVITE_SIZE];
	};
	typedef char InviteInfoHex[sizeof(InviteInfo) * 2 + 1];
	UNSTD_STATIC_CHECK(sizeof(InviteInfoHex) < 1024);

	//������Ϣ
	struct CommonInfo
	{
		CommonInfo()
		{
			this->Reset();
		}

		void Reset()
		{
			qing_yuan_id = 0;
			buy_invite_times = 0;
			xun_you_type = 0;
		}
		long long qing_yuan_id;
		int buy_invite_times;	//����չ����
		int xun_you_type;		//Ѳ������
	};
	typedef char CommonInfoHex[sizeof(CommonInfo) * 2 + 1];
	UNSTD_STATIC_CHECK(sizeof(CommonInfoHex) < 256);

	//������Ϣ �ᱻ���л�
	struct WeddingInfoDBParam
	{
		WeddingInfoDBParam()
		{
			this->Reset();
		}

		void Reset()
		{
			change_state = 0;
			save_index = 0;

			wedding_id = 0;
			begin_time = 0;
			cfg_ver = 0;

			is_finsih = 0;

			for (int i = 0; i < HOST_NUM; i++)
			{
				host_info_[i].Reset();
			}
			
			invite_info.Reset();

			common_info.Reset();
		}
		bool IsValid() const { return 0 != wedding_id; };

		int change_state;
		int save_index;

		WeddingID wedding_id;		//���id
		unsigned int begin_time;	//��Ὺ��ʱ��
		int cfg_ver;				//���ð汾

		int is_finsih;				//��ɱ��
		//������
		HostInfo host_info_[HOST_NUM];

		//��������
		InviteInfo invite_info;

		CommonInfo common_info;
	};

	struct HiddenGlobalParam
	{
		HiddenGlobalParam()
		{
			this->Reset();
		}

		void Reset()
		{
			m_creat_wedding_end = 0;
		}

		WeddingID m_creat_wedding_end;
	};

#pragma pack(pop)

};

class WeddingInfoDBList
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	static const int MAX_SIZE = 30;

	int m_save_index_begin;
	int m_count;
	WeddingFB::WeddingInfoDBParam m_wedding_info_list[MAX_SIZE];
};


#endif