#ifndef __MSG_PARTNER_H__
#define __MSG_PARTNER_H__

#include "gamedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/partnerdef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSPartnerHandleRequest
	{
	public:
		CSPartnerHandleRequest();			//!< 1710 - ���ϵͳ����
		MessageHeader header;

		enum PARTNER_OP
		{
			PARTNER_INFO_REQUIRE = 0,		//!< �������л����Ϣ
			PARTNER_DISP_PARTNER = 1,		//!< ����ս״̬�ı�
			PARTNER_ATTR_BREAKTH = 2,		//!< ����ͻ�� / ��������
			PARTNER_WASH_FETTERS = 3,		//!< ��Եϴ��
			PARTNER_SAVE_FETTERS = 4,		//!< ������Եϴ�����
			PARTNER_ACTI_PARTNER = 5,		//!< ���߼�����
			PARTNER_SIGNLE_INFO  = 6,		//!< ���󵥸������Ϣ
			PARTNER_UP_LEVEL = 7,			//!< ����������  param1:����
			PARTNER_RESET_LEVEL = 8,		//!< ���������õȼ�
			PARTNER_OPEN_BIOGRAPHY = 9,		//!< �򿪴���	   param1:biography_id
			PARTNER_FETCH_BGP_REWARD = 10,	//!< ��ȡ���ǽ���   param1:biography_id
			PARTNER_PAGE_COMPOSE = 11,		//!< �������ҳ�ϳ� param1:item_id
			PARTNER_UPLEVEL_GIFT_TREE = 12,	//!< ���������츳�� param: �츳�±�
			PARTNER_RESET_GIFT_TREE = 13,	//!< ���������츳�� param: �츳�±�
			PARTNER_PACT_PUT_ON = 14,		//!< Ԫ����Լ װ�� p1:װ��λ��  p2:װ��save_index
			PARTNER_PACT_GET_OFF = 15,		//!< Ԫ����Լ ж�� p1:װ��λ��
			PARTNER_PACT_UP_STAR = 16,		//!< Ԫ����Լ ���� p1:װ��λ��
			PARTNER_PACT_SWAP = 17,			//!< Ԫ����Լ �滻 p1:װ��λ�� p2:ԭװ�����id p3:ԭװ��λ��
			PARTNER_SOAR = 18,				//!< ������� 
			PARTNER_SOAR_ARMS_LEVEL_UP = 19,//!< ���������������

			PARTNER_HANDLE_TYPE_MAX,
		};

		short op_type;		//!< ��������
		UInt16 partner;		//!< �����Ļ��� partner_id ��������ñ�
		int param1;
		int param2;
		int param3;
	};

	class SCPartnerAllDataInform
	{
	public:
		SCPartnerAllDataInform();			//!< 1711 - ���л�����Ϣ
		MessageHeader header;

		int partner_count;						//!< �������
		PartnerPro partner[PARTNER_NUM_MAX];	//!< ���л����Ϣ [0, 29]
	};

	class SCPartnerCombatsStatus
	{
	public:
		SCPartnerCombatsStatus();		//!< 1712 - �ı����ս״̬Э��ظ�
		MessageHeader header;

		UInt16 partner;		//!< ��Ӧ�Ļ�� ID
		char is_fight;		//!< ��ս״̬�� 0 - ������ 1 - ��ս��
		char is_show;		//!< �Ƿ������ 0 - ������ 1 - ������
	};

	class SCPartnerAttrChangeRep
	{
	public:
		enum NOTICE_TYPE
		{
			NOTICE_TYPE_NO = 0,				//!< �ޱ���·�
			NOTICE_TYPE_HPMP,				//!< HP,MP�仯
			NOTICE_TYPE_BREAKE_THOUGH,		//!< ���ͻ��
			NOTICE_TYPE_UP_GARD,			//!< ������
			NOTICE_TYPE_EQUIP,				//!< ���װ��
			NOTICE_TYPE_ROLE_UP_LEVEL,		//!< ��������������������
			NOTICE_TYPE_TRAIN_UP,			//!< ��������������������
			NOTIC_TYPE_QUALITY_UP,			//!< �����Ʒ��������
			NOTIC_TYPE_SKIN,				//!< ���Ƥ���������� (��鼤��ʱ)
			NOTIC_TYPE_SKIN_NO_AC,			//!< ���Ƥ���������� (���δ����)
			NOTIC_TYPE_GUILD_INBORN,		//!< ��������
			NOTIC_TYPE_ATTR_PELLET,			//!< ���Ե�
			NOTIC_TYPE_PACT,				//!< ��Լ����(����������Ա������)
			NOTIC_TYPE_PACT_CHANGE,			//!< ������Լ���
			NOTIC_TYPE_SOAR,				//!< ������
			NOTIC_TYPE_SOAR_ARMS_LEVEL_UP,	//!< ��������ǿ��
			NOTICE_TYPE_XIU_WEI,			//!< ������Ϊ������������
			NOTICE_TYPE_INTENSIFY,			//!< ���ǿ��
			NOTICE_TYPE_FU_YU,				//!< ����
			NOTICE_TYPE_STAR_CHART,			//!< ��ͼ��λ�仯
		};
		SCPartnerAttrChangeRep();		//!< 1713 - ������Ա仯֪ͨ
		MessageHeader header;

		UInt16 partner;								//!< ��Ӧ�Ļ�� ID
		char list_count;							//!< �ı����������

		char notic_type;

		int capability;								//!< �������
		BattleAttrPair attr_list[BATTLE_ATTR_MAX];	//!< ������ֵ - [0, 2147483647]
	};

	class SCPartnerFettersInfoma
	{
	public:
		SCPartnerFettersInfoma();		//!< 1714 - ������Ϣ�ظ�
		MessageHeader header;

		UInt16 partner;						//!< ��Ӧ�Ļ�� ID

		short resver_sh;

		unsigned char curr_fetter[PARTNER_FETTER_COUNT_MAX];		//!< ��ǰ���
		unsigned char wash_fetter[PARTNER_FETTER_COUNT_MAX];		//!< ϴ�����
	};

	class SCPartnerBaseGradeInfo
	{
	public:
		SCPartnerBaseGradeInfo();		//!< 1715 - �������׼���Ϣ
		MessageHeader header;

		UInt16 partner;				//!< ��� ID
		char level;					//!< ���׵ȼ�
		char stage;					//!< ͻ�ƽ׶� - [0, 7]

		char upgrade_ret;			//!< ���ױ�ǣ�0 - �ǽ��ף��������� 1 - �ǽ��ף�Ҫ����

		char task_finish;			//!< �Ƿ���ɽ�����������
		short resetve_sh;
	};

	class CSToSaveSkillSelection
	{
	public:
		CSToSaveSkillSelection();		//!< 1716 - ������ѡ�Ļ�鼼��
		MessageHeader header;

		UInt16 partner;		//!< ��� ID

		short index;		//!< ���ܲ� - 0��1��2
		short skill;		//!< ����ID    -1��ʾ�������ܲ�

		short reserve_sh;	//!< ������
	};

	class SCSavePartnersSkillRep
	{
	public:
		SCSavePartnersSkillRep();		//!< 1717 - ��鵱ǰ����֪ͨ
		MessageHeader header;

		UInt16 partner;
		short fight_skill_list[PARTNER_COMMON_SKILL_MAX];
	};
	UNSTD_STATIC_CHECK(3 == PARTNER_COMMON_SKILL_MAX);

	class SCSynFightPartnerOrder
	{
	public:
		SCSynFightPartnerOrder();		//!< 1718 - ��ս���˳����Ϣ
		MessageHeader header;

		int fight_numb;
		int fight_list[PARTNER_SYNFIGHT_NUM_MAX];
	};

	class CSChangeSyncFightOrder
	{
	public:
		CSChangeSyncFightOrder();		//!< 1719 - ��������ս˳��
		MessageHeader header;

		UInt16 insert_partner;			//!< ��Ҫ����Ļ��ID
		UInt16 target_partner;			//!< ��Ҫ��λ�Ļ��ID
	};

	class CSPartnerSwitchFightPos
	{
	public:
		CSPartnerSwitchFightPos();		//!< 2044 - ��������սλ��
		MessageHeader header;

		UInt16 partnerA_id;				//!< ���A id
		UInt16 partnerB_id;				//!< ���B id
	};

	class CSPartnerEqOperatorReq
	{
	public:
		CSPartnerEqOperatorReq();		//!< 1770 - ���װ����������
		MessageHeader header;

		enum PARTNER_EQUIP_OPERATOR_TYPE
		{
			PE_OP_PULL_ON_EQUIP = 0,		//!< ��װ��
			PE_OP_GET_OFF_EQUIP = 1,		//!< ��װ��
			PE_OP_UNLOCK_EQUIP = 2,			//!< ����
			PE_OP_RESOLVE_EQUIP = 3,		//!< �ֽ�
		};

		UInt16 op_type;			//!< ��������
		UInt16 partner_id;		//!< ���ID
		int param1;				//!< ����1		����ID
	};

	class SCPartnerEqSingleSlots
	{
	public:
		SCPartnerEqSingleSlots();		//!< 1771 - �������װ������Ϣ
		MessageHeader header;

		UInt16 partner_id;			//!< ���ID
		UInt16 equip_slot;			//!< װ����λ
		UInt16 equip_id;			//!< װ��ID
		UInt16 unlock_level;		//!< �����ȼ�
		BattleAttrPair attr_pair[MAX_PARTNER_NATAL_CARD_VOC_NUM];	//!< ����
	};

	struct SCPartnerQualityUp			//!< 1772 - �����Ʒ�·�
	{
		SCPartnerQualityUp();
		MessageHeader header;

		enum NoticeType
		{
			NOTICETYPE_INIT		= 0,					// ��ʼ���·�
			NOTICETYPE_UP		= 1,					// ��Ʒ�·�
			NOTICETYPE_ACTIVE	= 2,					// ������
		};

		int partner_id;								// ���id
		int up_type;								// ��Ʒ����
		int quality;								// Ʒ��
		int att_list[PARTNER_ATTR_MAX];				// ����ֵ
		char notic_type;							// �·�����
		char quality_segment;						// ����	
		short rew_2;								// ������

	};

	struct CSPartnerQualityUpReq		//!< 1773 - �����Ʒ����
	{
		CSPartnerQualityUpReq();
		MessageHeader header;

		enum QualityUpType			//��Ʒ����
		{
			QUALITY_TYPE_BLUE_TO_PURPLE		= 1,			//!< ��  --> ��
			QUALITY_TYPE_PURPLE_TO_ORANGE	= 2,			//!< ��  --> ��
			QUALITY_TYPE_ORANGE_TO_RED		= 3,			//!< ��  --> ��
			QUALITY_TYPE_RED				= 4,			//!< ��  --> ��
			QUALITY_TYPE_GOLD				= 5,			//!< ��ɫ
		};

		int use_type;	  // ��Ʒ������������
		int up_type;      // ��Ʒ����
		int partner_id;	  // ���id

	};

	//struct SCPartnerUseElementCrytal		//!< 1774 - ���Ԫ��ˮ����Ϣ�·�
	//{
	//	SCPartnerUseElementCrytal();
	//	MessageHeader header;

	//	int partner_id;								// ���id
	//	int crytal_id;								// װ����ˮ��id
	//	int use_time;								// ʣ��ʹ�ô���
	//	
	//};

	struct SCPartnerUseElementCrytalFight		//!< 1775 - ���Ԫ�ؿ���װ���·�
	{
		SCPartnerUseElementCrytalFight();
		MessageHeader header;

		int count;									// Ҫװ��ˮ���Ļ������
		PartnerUseElementCrytalFightInfo once_info[PARTNER_NUM_MAX];		
	};

	struct CSPartnerElementCrytalOper		//!< 1776 - ���Ԫ��ˮ������
	{
		CSPartnerElementCrytalOper();
		MessageHeader header;

		enum ElementCrytalUseType			
		{
			ELEMENT_CRYTALE_USE_TO_ONCE						= 1,			//!< Ϊĳ�����װ��Ԫ��ˮ��
			ELEMENT_CRAYTAL_REMOVE_TO_ONCE					= 2,			//!< ж��ĳ������Ԫ��ˮ��
			ELEMENT_CRAYTAL_USE_TO_FIGHT_PARTNER			= 3,			//!< ����װ��Ԫ��ˮ������ս���
			ELEMENT_CRAYTAL_REMOVE_FROM_EXIST_PARTNER		= 4,			//!< ж�����м������Ԫ��ˮ��
		};
		int oper_type;		// ��������
		int param_1;		// 1 - Ҫװ��Ԫ��ˮ���Ļ��id			2 - Ҫж��Ԫ��ˮ���Ļ��id		3 - ����װ������ս����Ԫ��ˮ��id_1id�ڱ�����index
		int param_2;		// 1 - ˮ��Ԫ����Ʒid�ڱ�����index										3 - ����װ������ս����Ԫ��ˮ��id_2id�ڱ�����index
		int param_3;		//																		3 - ����װ������ս����Ԫ��ˮ��id_3id�ڱ�����index
		int param_4;		//																		3 - ����װ������ս����Ԫ��ˮ��id_4id�ڱ�����index
		
	};

	struct SCParnerSkinActive				//!< 1777 - ���Ƥ����������Ϣ�·�
	{
		SCParnerSkinActive();
		MessageHeader header;

		struct PartnerSkinPro
		{
			PartnerSkinPro() { this->Reset(); }
			
			void Reset()
			{
				partner_id = 0;
				skin_active_status = 0;		//������, Ƥ������seq  1:�Ѽ���
			}

			short partner_id;
			short skin_active_status;
		};

		int count;									// ӵ��Ƥ������
		PartnerSkinPro skin_list[PARTNER_NUM_MAX];	// Ƥ���б�  PARTNER_NUM_MAX -- 40
	};

	struct SCParnerSkinSingleOper				//!< 1778 - ���Ƥ��������Ϣ�·�
	{
		SCParnerSkinSingleOper();
		MessageHeader header;

		enum SendType		// �·�����
		{
			SKIN_AVTIVE = 0,	// ����Ƥ��
			SKIN_PUT_ON = 1,	// Ƥ������
			SKIN_PUT_OFF = 2,	// Ƥ��ж��
		};

		int send_type;								// �·�����
		int partner_id;								// ���id
		int skin_id;								// Ƥ��id
	};

	struct CSPartnerSkinReq					//!< 1779 ���Ƥ������
	{
		CSPartnerSkinReq();
		MessageHeader header;

		enum ReqType
		{
			SKIN_AVTIVE			= 0,	// ����Ƥ��
			SKIN_PUT_ON			= 1,	// Ƥ������
			SKIN_PUT_OFF		= 2,	// Ƥ��ж��
		};

		int oper_type;	// ��������
		int param_1;	// ���id
		int param_2;	// Ƥ��id
	};

	class CSPartnerSkillActivate	//!< 2049 ��鼼������/����
	{
	public:
		CSPartnerSkillActivate();	
		MessageHeader header;

		int partner_id;		//!< ���ID
		short is_activate;	//!< 0������ 1������
		short skill_idx;	//!< ������� [0,3)
	};

	class SCMysteryStoreReq			//!< 3350 �����̵�����
	{
	public:
		SCMysteryStoreReq();
		MessageHeader header;

		enum ReqType
		{
			REQ_REFRESH = 0,// ˢ����Ʒ
			REQ_BUY = 1,	// ����
			REQ_INFO = 2,	// ȫ����Ϣ
		};

		short req_type;
		short param;
	};

	class SCMysteryStoreInfo		//!< 3351 �����̵���Ϣ
	{
	public:
		SCMysteryStoreInfo();
		MessageHeader header;

		unsigned int next_free_refresh_time;
		unsigned int next_refresh_time;
		short store_refresh_num;						//ˢ�´���
		short count;
		NewStoreNatalCard data[MYSTERY_STORE_MAX_SEQ];
	};

	class  SCPartnerSingleInfo					//!< 2022 �·���鵥����Ϣ
	{
	public:
		SCPartnerSingleInfo();
		MessageHeader header;

		PartnerPro partner_info;
	};

	struct PartnerGiftTreeNode
	{
		PartnerGiftTreeNode() :partner_id(0) {}
		int partner_id;
		PartnerGiftTree gift_tree;
	};

	class  SCPartnerGiftTreeListInfo					//!< 2023 �·�����츳����Ϣ
	{
	public:
		SCPartnerGiftTreeListInfo();
		MessageHeader header;

		int partner_num;
		PartnerGiftTreeNode partner_gift_tree_list[PARTNER_NUM_MAX];
	};

	class  SCPartnerSingleGiftTreeInfo					//!< 2024 �·���������츳����Ϣ
	{
	public:
		SCPartnerSingleGiftTreeInfo();
		MessageHeader header;

		int partner_id;
		PartnerGiftTree partner_gift_tree;
	};

	//4010 ���--Ԫ����Լ�ֿ�֪ͨ
	class SCPartnerSpiritPactList
	{
	public:
		SCPartnerSpiritPactList();
		MessageHeader header;
		
		enum SendType
		{
			SEND_TYPE_ALL = 0,			//ȫ��
			SEND_TYPE_CHANGE = 1,		//�ı�

		};

		struct PactInfo
		{
			PactInfo()
			{
				save_index = 0;
				item_id = 0;
				num = 0;
			}
			short save_index;	//����λ��
			ItemID item_id;		//��Ʒid
			int num;			//��Ʒ����
		};

		int send_type;

		int count;
		PactInfo pact_list[PARTNER_SPIRIT_PACT_MAX_GRID];
	};

	//4011 ���--Ԫ����Լ ��Լ�� ����֪ͨ
	class SCPartnerPactBaseAttr
	{
	public:
		SCPartnerPactBaseAttr();
		MessageHeader header;

		int partner_id;
		//�û�����ڸ�����ԼĿ�����ӵ�����
		Attribute attach_base_attr_list[PARTNER_ATTR_MAX];
	};

	//4012 ���--Ԫ����Լ ��Լ�ı�
	class SCPartnerPactInfo
	{
	public:
		SCPartnerPactInfo();
		MessageHeader header;

		enum Type
		{
			DEF = 0,		//Ĭ��
			UP_START,		//����	p:λ�� 0-2
		};

		int partner_id;
		int type;
		ItemID put_on_pact_id_list[PARTNER_SPIRIT_PACT_MAX];
		short param;

	};

	class SCPartnerSoarNoitce		//!< 2028 �·����������֪ͨ
	{
	public:
		SCPartnerSoarNoitce();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_SOAR = 0,						// ������
			NOTICE_TYPE_SOAR_ARMS_LEVEL_UP = 1,			// ����������ǿ��

			NOTICE_TYPE_MAX
		};

		int notice_type;
		int partner_id;
		short soar_count;
		short soar_arms_level;
	};

	class CSPartnerIntensifyReq			// 4465
	{
	public:
		CSPartnerIntensifyReq();
		MessageHeader header;

		struct UseItem
		{
			ItemID use_item_id;
			short use_num;
		};

		int partner_id;

		UseItem expend_item_1;
		UseItem expend_item_3;

		int expend_item_2_count;
		UseItem expend_item_2_list[20];
	};

	class SCPartnerIntensifyInfo		// 4466
	{
	public:
		SCPartnerIntensifyInfo();
		MessageHeader header;

		short partner_id;
		short intensify_level;
	};
}

#pragma pack(pop)
#endif