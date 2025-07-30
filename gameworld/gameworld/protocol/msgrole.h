#ifndef MSGROLE_H
#define MSGROLE_H

#include "gamedef.h"
#include "servercommon/struct/roleinitparam.h"
#include "servercommon/struct/rolequeryparam.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/moneyotherdef.h"
#include "servercommon/attributedef.hpp"
#include "servercommon/rolesettingdef.hpp"
#include "servercommon/facescoredef.h"
#include "servercommon/partnerdef.h"
#include "item/itembase.h"
#include "equipment/jewelry.h"
#include "equipment/equipment.h"
#include "protocol/msgpet.h"
#include "other/pet/petconfig.hpp"
#include "servercommon/petdef.h"
#include "servercommon/jieyidef.h"
#include "servercommon/smartmountssystemdef.hpp"
#include "other/shengqi/holyequipmanager.hpp"
#include "msgpet.h"

#pragma pack(push, 4)

namespace Protocol
{
	/*
		�����ɫ��Ϣ
	*/
	class SCRoleInfoAck			//!< 1400
	{
	public:
		SCRoleInfoAck();		
		MessageHeader		header;
		
		int role_id;			//!< ��ɫid
		GameName name;			//!< ��ɫ�� GameName = char[32]
		short level;			//!< �ȼ�
		short profession;		//!< ְҵ
		int top_level;			//!< �۷�ȼ�
		int title_id;			//!< �ƺ�
		int facescroe_id;		//!< ��ֵϵͳ | ͷ��seq
		long long cur_exp;		//!< ��ǰ����ֵ

		GuildName guild_name;	//!< ������ GuildName = char[32]
		int move_speed;

		Appearance appearance;	//!< ���

		unsigned int create_time;	//!< ����ʱ��
		int kill_value;			//!< ɱ¾ֵ
		char authority_type;	//!< Ȩ��
		char reserve_ch;
		short reserve_sh;
	};

	class SCRoleSpeedChangeNotice			//!< 1408 obj���ٸı�֪ͨ
	{
	public:
		SCRoleSpeedChangeNotice();
		MessageHeader		header;

		int obj_id;
		int move_speed;
	};

	class SCRoleStatusChangeNotice
	{
	public:
		SCRoleStatusChangeNotice();			//!< 1409 - obj ״̬�ı�֪ͨ
		MessageHeader header;

		ObjID obj_id;				//!< Ҫ���µ���ҵ� Obj ID
		short cur_status;			//!< ��ǰ״̬
		short cur_move_status;		//!< ��ǰ�ƶ�״̬
		short reserve_sh;
	};

	class SCRoleAutoTalkInfo		//!< 1410 ��ɫ�Զ��Ի���Ϣ�·�
	{
	public:
		SCRoleAutoTalkInfo();
		MessageHeader header;

		enum ROLE_AUTO_TALK_NOTICE_TYPE
		{	
			ROLE_AUTO_TALK_NOTICE_TYPE_INIT = 0,	//��ʼ��,�յ�½��ʱ��,����Ҫ����ҷ��Ի�
			ROLE_AUTO_TALK_NOTICE_TYPE_SEND = 1,	//�л��Ի�ID,����ҷ��ͶԻ�

			ROLE_AUTO_TALK_NOTICE_TYPE_MAX
		};

		int notice_type;
		int talk_groups_id;
		int talk_id;
	};

	class SCRoleAuthorityInfoChange		//!< 1411 ��ɫȨ����Ϣ�ı��·�
	{
	public:
		SCRoleAuthorityInfoChange();
		MessageHeader header;

		char authority_type;		// Ȩ��
		char reserve_ch;
		short reserve_sh;
	};

	class SCRoleHistoryMaxCapabilityChange		//!< 1412 ��ɫ��ʷ���ֱ仯�·�
	{
	public:
		SCRoleHistoryMaxCapabilityChange();
		MessageHeader header;

		int max_role_capability;			// ��ɫ��ʷ���ս��	
	};

	class CSAllInfoReq						//!< 1450 ��ɫ����ģ����Ϣ����
	{
	public:
		CSAllInfoReq();
		MessageHeader	header;
	};

	class SCMoney									//!< 1401 ��ɫ������Ϣ
	{
	public:

		struct MoneyInfo
		{
			short reserve_sh;						//!< ����ֵ
			short money_type;						//!< �������� \see MONEY_TYPE_MAX
			Int64 money_num;						//!< ��������
		};

	public:
		SCMoney();
		MessageHeader		header;
		
		short is_no_show;							//!< �Ƿ�Ĭ�ı� 0:�񣨼�Ҫ��ʾ���)  1:��(������ʾ���)
		short count;								//!< money_list������

		MoneyInfo money_list[MONEY_TYPE_MAX];		//!< �����б�  \see MoneyInfo  \see MONEY_TYPE_MAX
	};

	class SCRoleAttrList							//!< 1402 �����б�
	{
	public:
		SCRoleAttrList();
		MessageHeader header;

		struct AttrPair
		{
			int attr_type;							//!< �������ͺ� \see enum BATTLE_ATTR
			int attr_value;							//!< ���Ե�ֵ
		};

		enum NOTIFY_REASON
		{
			NOTIFY_REASON_NORMAL = 0,
			NOTIFY_REASON_SYNC_HPMP_IN_FB = 1,
			NOTIFY_REASON_FA_BAO_DRAW = 2,
			NOTIFY_REASON_HEART_SKILL_EQUIP_CHANGE = 3,
			NOTIFY_REASON_DIAN_HUA_SHI_CAP = 4,
		};

		short attr_count;							//!< �·�����������
		short notify_reason;						//!< �·������� \see NOTIFY_REASON
		int capability;								//!< ���ս��(����)
		AttrPair attr_list[BATTLE_ATTR_MAX];		//!< ����������attr_count����  \see BATTLE_ATTR_MAX
	};

	class SCRoleAttributePlanInfo					//!< 1403 �ӵ㷽����Ϣ
	{
	public:
		SCRoleAttributePlanInfo();
		MessageHeader header;

		struct PlanDetail
		{
			char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< �ƻ���  \see ATTRIBUTE_PLAN_NAME_LENGTH
			short remain_point;							//!< ʣ�����
			short add_point_list[ADD_POINT_TYPE_MAX];   //!< �ӵ����ֵ \see ADD_POINT_TYPE_MAX
			short custom_add_point_list[ADD_POINT_TYPE_MAX];
			short reserve_sh;							//!< ������
		};
		UNSTD_STATIC_CHECK(ADD_POINT_TYPE_MAX == 5);

		short plan_num;								//!< ��ǰ�ж��ٸ����� ��Χ[1,3]
		short cur_plan_index;						//!< ��ǰ������� ��Χ[0, plan_num)
		PlanDetail plan_list[MAX_ATTR_PLAN_NUM];	//!< ���鳤��Ϊ3  \see PlanDetail

		short reset_plan_times;						//!< ϴ�����
		short tui_jian_auto_add;							//!< ������
	};

	class SCRoleExpChange							//!< 1404 ����ı�֪ͨ
	{
	public:
		SCRoleExpChange();
		MessageHeader header;

		long long change_exp;						//!< �ı�ľ���
		long long cur_exp;							//!< �ı����
		short change_reason;						//!< �ı��ԭ��  \see ADD_EXP_TYPE
		short reserve_sh;							//!< ������
	};

	class SCRoleLevelChange							//!< 1405
	{
	public:
		SCRoleLevelChange();
		MessageHeader header;

		enum ROLE_LEVEL_CHANGE_TYPE
		{
			ROLE_LEVEL_CHANGE_TYPE_NORMAL = 0,		//��ͨ�ȼ�
			ROLE_LEVEL_CHANGE_TYPE_TOP = 1,			//�۷�ȼ�
		};

		int change_type;
		int level;									//!< �ȼ�
		long long exp;								//!< ��ǰ����
	};

	class CSAttributePlanCreate						//!< 1451 �����¼ӵ㷽��
	{
	public:
		CSAttributePlanCreate();
		MessageHeader header;

		char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< ��������\see ATTRIBUTE_PLAN_NAME_LENGTH
	};

	class CSAttributePlanChangeName					//!< 1452 ��������
	{
	public:
		CSAttributePlanChangeName();
		MessageHeader header;

		int plan_index;								//!< Ҫ�ĵķ������[0,3)
		char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< �·�������\see ATTRIBUTE_PLAN_NAME_LENGTH
	};

	class CSAttributePlanSwitchPlan					//!< 1453 �л�����
	{
	public:
		CSAttributePlanSwitchPlan();
		MessageHeader header;

		int switch_to_index;						//!< Ҫ�л��ķ������[0,3)
	};

	class CSAttributePlanAddPoint					//!< 1454 �ӵ�
	{
	public:
		CSAttributePlanAddPoint();
		MessageHeader header;

		short plan_index;							//!< Ҫ�ӵ�ķ�����
		short delta_attr_list[ADD_POINT_TYPE_MAX];	//!< �ı�ĵ����� ��Ϊ����  \see ADD_POINT_TYPE_MAX
	};

	class CSAttributePlanClearPoint					//!< 1455 ϴ��
	{
	public:
		CSAttributePlanClearPoint();
		MessageHeader header;

		int plan_index;								//!< Ҫϴ��ķ���
	};

	class SCRoleProfessionChangeNotice	//!< 1406 ְҵ���֪ͨ
	{
	public:
		SCRoleProfessionChangeNotice();
		MessageHeader header;

		int new_profession;			//!< �µ�ְҵ
	};

	class CSRoleCareerChangeReq		//!< 1457 ��ɫתְ
	{
	public:
		CSRoleCareerChangeReq();
		MessageHeader header;

		char	avartar_type;		//!< ������������
		char	color;				//!< ��ɫ
		short	reserve_sh;			//!< ������
		int		to_profession;		//!< תְ���ĸ�ְҵ
	};

	class CSProfessionExtraInfoReq	//!< 1458 ����ְҵ������Ϣ
	{
	public:
		CSProfessionExtraInfoReq();
		MessageHeader header;
	};

	class SCProfessionExtraInfo		//!< 1407 ����ְҵ������Ϣ
	{
	public:
		SCProfessionExtraInfo();
		MessageHeader header;

		int change_prof_times;		//!< ��ǰ��תְ����
	};

	class CSRoleSystemSetReq
	{
	public:
		CSRoleSystemSetReq();		// 1460
		MessageHeader header;
		struct system_set
		{
			short system_set_type;		// ��������
			short system_set_param;		// ����ֵ
		};
		int count;
		system_set system_set_list[RoleSetSystemParam::SYSTEM_SET_INFO_MAX];
	};

	class SCRoleSystemSetInfo
	{
	public:
		SCRoleSystemSetInfo();		// 1461
		MessageHeader header;

		int system_set_list[22];
	};

	class CSRoleSystemSetChatGroupReq	//!< 1462 �ͻ����Զ�������-Ⱥ������
	{
	public:
		CSRoleSystemSetChatGroupReq();
		MessageHeader header;

		long long chat_group_id;		//!< Ⱥ��ID
		int set_value;					//!< [0:����� 1:����]
	};

	class SCRoleSystemSetChatGroupInfo	//!< 1463 �ͻ����Զ�������-Ⱥ����Ϣ
	{
	public:
		SCRoleSystemSetChatGroupInfo();	
		MessageHeader header;

		struct GroupInfo
		{
			long long chat_group_id;	//!< Ⱥ��ID
			int value;					//!< [0:����� 1:����]
		};

		int chat_group_num;				//!< Ⱥ������ 
		GroupInfo system_set_group_list[MAX_ROLE_CROSS_GROUP]; //!< ��Ⱥ��������ȡ����
	};

	class CSSetEncounterMineMonsterFlag		//!< ������������ 2030
	{
	public:
		CSSetEncounterMineMonsterFlag();
		MessageHeader header;

		int flag;					//!< �Ƿ���������  0:��  1����
	};

	class SCEncounterMineMonsterFlag	//!< �������ַ��� 2005
	{
	public:
		SCEncounterMineMonsterFlag();
		MessageHeader header;

		int flag;					//!< �Ƿ���������  0:��  1����
	};

	class CSSetBattlePosition		//!< ��������վλ 2031  
	{
	public:
		CSSetBattlePosition();
		MessageHeader header;

		int position;				//!< ����վλ  ��0������   1��ǰ�ţ�
	};

	class SCSetBattlePosition		//!< ��������վλ���� 2006
	{
	public:
		SCSetBattlePosition();
		MessageHeader header;

		int position;				//!< ����վλ  ��0������   1��ǰ�ţ�
	};

	class CSSetBattleAuto			//!< �������������Ƿ��Զ�ս�� 2032
	{
	public:
		CSSetBattleAuto();
		MessageHeader header;

		int is_auto;
	};

	class CSSetBattleAutoMove		//!< �������������Զ�ս������Ϊ 2033
	{
	public:
		CSSetBattleAutoMove();
		MessageHeader header;

		short first_move_type;		//!< ��Ϊ����  \see BATTLE_MOVE_TYPE
		short first_skill_id;		//!< ����ID
		short first_skill_level;	//!< ���ܵȼ�

		short second_move_type;		//!< ��Ϊ����  \see BATTLE_MOVE_TYPE
		short second_skill_id;		//!< ����ID
		short second_skill_level;	//!< ���ܵȼ�
	};

	class SCBattleAutoInfo			//!< �����Զ�ս��������Ϣ�ذ� 2007
	{
	public:
		SCBattleAutoInfo();
		MessageHeader header;

		int is_auto;				//!< ��ǰ�Ƿ��Զ�
	
		short first_move_type;		//!< ��Ϊ����  \see BATTLE_MOVE_TYPE
		short first_skill_id;		//!< ����ID
		short first_skill_level;	//!< ���ܵȼ�

		short second_move_type;		//!< ��Ϊ����  \see BATTLE_MOVE_TYPE
		short second_skill_id;		//!< ����ID
		short second_skill_level;	//!< ���ܵȼ�
	};

	class CSRoleResetName			//!< 1054 ������������
	{
	public:
		CSRoleResetName();
		MessageHeader	header;

		GameName new_name;			//!< �µ����� �64���ֽ�
	};

	class SCRoleResetName			//!< 1004 �������ֻذ� (�㲥����ǰ�����������ˣ�
	{
	public:
		SCRoleResetName();
		MessageHeader header;
		
		ObjID obj_id;				//!< ���õĽ�ɫ��obj_id  (�ͻ���Ҫ�жϺ����ĸ���ɫ�ڸ�����
		short reserved;				//!< ������
		GameName new_name;			//!< �µ����� �64���ֽ�
	};

	class CSQueryRoleInfo
	{
	public:
		CSQueryRoleInfo();		//!< 2010 ��ѯ�����Ϣ
		MessageHeader header;

		enum QUERY_ROLE_TYPE
		{
			QUERY_ROLE_TYPE_DETAILED = 0,			// ��ɫ��ϸ��Ϣ�·�
			QUERY_ROLE_TYPE_SIMPLE = 1,				// ��ɫ������Ϣ�·�
		};

		int role_id;			//!< Ŀ����ҵ�UID
		int query_type;			
	};

	class SCQueryRoleInfoRet : public IMessageSerializer 				//!< 2011 ��ѯ�����Ϣ�·�
	{
	public:
		SCQueryRoleInfoRet();	
		virtual ~SCQueryRoleInfoRet() {}
		MessageHeader header;
		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(role_id, buffer, buffer_size, out_length);
			result = result && WriteStrN(role_name, sizeof(role_name), buffer, buffer_size, out_length);
			result = result && WriteStrN(guild_name, sizeof(guild_name), buffer, buffer_size, out_length);
			result = result && WriteUInt(avatar_type, buffer, buffer_size, out_length);
			result = result && WriteUInt(prof, buffer, buffer_size, out_length);
			result = result && WriteUInt(level, buffer, buffer_size, out_length);
			result = result && WriteInt(top_level, buffer, buffer_size, out_length);
			result = result && WriteInt(capability, buffer, buffer_size, out_length);
			result = result && WriteInt(rank_score, buffer, buffer_size, out_length);
			result = result && WriteShort(pet_count, buffer, buffer_size, out_length);
			result = result && WriteShort(partner_count, buffer, buffer_size, out_length);
			result = result && WriteShort(medal_count, buffer, buffer_size, out_length);
			result = result && WriteChar(is_finish_ding_ji, buffer, buffer_size, out_length);
			result = result && WriteChar(reserve_ch, buffer, buffer_size, out_length);
			result = result && WriteInt(kill_value, buffer, buffer_size, out_length);
			result = result && WriteInt(accessories_count, buffer, buffer_size, out_length);
			result = result && WriteShort(smart_mounts_count, buffer, buffer_size, out_length);
			result = result && WriteShort(holy_equip_count, buffer, buffer_size, out_length);	
			result = result && WriteInt(course_level, buffer, buffer_size, out_length);
			result = result && WriteInt(course_cur_fraction, buffer, buffer_size, out_length);
			result = result && WriteInt(face_score_level, buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&user_attribute, sizeof(user_attribute), buffer, buffer_size, out_length);
			result = result && WriteInt(qing_yuan_role_id, buffer, buffer_size, out_length);
			result = result && WriteStrN(qing_yuan_name, sizeof(qing_yuan_name), buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&jie_yi_name, sizeof(jie_yi_name), buffer, buffer_size, out_length);
			
			for (int i = 0; i < ARRAY_LENGTH(jie_yi_name_list); ++i)
			{
				result = result && WriteStrN(jie_yi_name_list[i], sizeof(jie_yi_name_list[i]), buffer, buffer_size, out_length);
			}

			result = result && WriteStrN((const char *)&appearance_info, sizeof(appearance_info), buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&equip_list, sizeof(equip_list), buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&jewelry_list, sizeof(jewelry_list), buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&face_equip_list, sizeof(face_equip_list), buffer, buffer_size, out_length);
			for (int i = 0; i < pet_count && i < MAX_FIGHT_AND_STAND_BY_PET_NUM; i++)
			{
				result = result && WriteStrN((const char *)&pet_list[i], sizeof(pet_list[i]), buffer, buffer_size, out_length);
				result = result && WriteStrN((const char *)&pet_attr_list[i], sizeof(QueryAttrList), buffer, buffer_size, out_length);
				result = result && WriteStrN((const char *)&pet_god_print_list[i], sizeof(GodPrintListPro), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < partner_count && i < PARTNER_SYNFIGHT_NUM_MAX; i++)
			{
				result = result && WriteStrN((const char *)&partner_list[i], sizeof(PartnerQueryPro), buffer, buffer_size, out_length);
			}
				
			for (int i = 0; i < medal_count && i < MAX_MEDAL_WEAR_GRID; i++)
			{
				result = result && WriteStrN((const char *)&medal_list[i], sizeof(MedalPro), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < accessories_count && i < ItemNamespace::MAX_ACCESSORIES_INDEX_NUM; i++)
			{
				result = result && WriteStrN((const char *)&accessories_list[i], sizeof(AccessoriesPro), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < smart_mounts_count && i < ARRAY_LENGTH(smart_mounts_list); i++)
			{
				result = result && WriteStrN((const char *)&smart_mounts_list[i], sizeof(SmartMountsSystemPro), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < holy_equip_count && i < ARRAY_LENGTH(holy_equip_list); i++)
			{
				result = result && WriteStrN((const char *)&holy_equip_list[i], sizeof(HolyEquipPro), buffer, buffer_size, out_length);
			}

			return result;
		}
		int				role_id;
		GameName		role_name;			// �������
		GuildName		guild_name;			// ��������
		UInt32			avatar_type;		// ��ɫ����
		UInt32			prof;				// ְҵ
		UInt32			level;				// ����ȼ�
		int				top_level;			// �۷�ȼ�
		int				capability;			// ���ս��
		int				rank_score;			// �ý�ս����λ��
		short			pet_count;			// ��ʱ��ս+��ս�ĳ�������
		short			partner_count;		// �������
		short			medal_count;		// ѫ������
		char 			is_finish_ding_ji;	// �Ƿ���ɶ�����(�ý�ս��)
		char			reserve_ch;			
		int				kill_value;			// ɱ¾ֵ
		int				accessories_count;	// ��������
		short			smart_mounts_count;	// ��������
		short			holy_equip_count;	// ʥ������	
		int				course_level;		// ��ǰ���̵ȼ�
		int				course_cur_fraction;// ��ǰ���̵�
		int				face_score_level;	// ��ֵ�ȼ�
		QueryAttrList	user_attribute;		// ��ɫ����
		int				qing_yuan_role_id;	// ��Ե��ɫID
		GameName		qing_yuan_name;		// ��Ե����
		JieYIZuName 	jie_yi_name;		// ����������
		GameName		jie_yi_name_list[MAX_JIE_YI_ZU_MAX_ROLE_ITEM_NUM];	// �����Ա����

		Appearance appearance_info;
		EquipmentPro equip_list[Equipment::E_INDEX_MAX];
		JewelryPro jewelry_list[Jewelry::J_TYPE_MAX];
		int face_equip_list[FACE_ADDITION_TYPE_MAX];					// ��װ���ģ���ԴID������鿴��ֵϵͳ���ñ�	FACE_ADDITION_TYPE_MAX -- 10
		MsgPetInfo pet_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];				// �������������		��ս+��ս				MAX_PET_FIGHT_AND_STANDBY_NUM - 5
		QueryAttrList pet_attr_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];	// ��������
		GodPrintListPro pet_god_print_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];	// ��������ӡ��װ
		PartnerQueryPro partner_list[PARTNER_SYNFIGHT_NUM_MAX];		// ��ս���  PARTNER_SYNFIGHT_NUM_MAX -- 5
		MedalPro medal_list[MAX_MEDAL_WEAR_GRID];				// �������� MAX_MEDAL_WEAR_GRID -- 3
		AccessoriesPro accessories_list[ItemNamespace::MAX_ACCESSORIES_INDEX_NUM];// �������� MAX_ACCESSORIES_INDEX_NUM -- 5
		SmartMountsSystemPro smart_mounts_list[MAX_SMART_MOUNTS_SYSTEM_NUM];	// ���������б� MAX_SMART_MOUNTS_SYSTEM_NUM -- 10
		HolyEquipPro holy_equip_list[MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM];			// ��ս�ʹ���ʥ���б� MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM -- 5
	};

	class CSChangeHeadshot				//!< 2035 �ı�ͷ��
	{
	public:
		CSChangeHeadshot();			
		MessageHeader header;
		
		short headshot_id;				//!< ͷ��ID
		short reserve_sh;				//!< ������
	};

	class SCAttrListReason 			//!< 2036 ������Դ�б��·�	
	{
	public:
		SCAttrListReason();
		MessageHeader header;

		enum REASON_TYPE
		{
			REASON_TYPE_ROLE = 0,			// ��ɫ
			REASON_TYPE_PET = 1,			// ����
			REASON_TYPE_PARTNER = 2,		// ���
			REASON_TYPE_HUASHEN_ROLE = 3,	// �����ɫ
			REASON_TYPE_HUASHEN_PET = 4,	// �������

			REASON_TYPE_MAX,
		};

		short reason_type;
		short param1;
		AttributeList reason_list[MAX_REASON_TYPE_LIST_NUM];	//������Դ�б�
	};

	class CSAttrListReasonReq			//!< 2037 ������Դ�����б��·�
	{
	public:
		CSAttrListReasonReq();
		MessageHeader header;

		short reason_type;				//REASON_TYPE_MAX
		short param1;					//��ɫʱĬ��0,������pet_index, �����partner_id
	};

	class CSRoleInfoNoticeReq     //!< 2850
	{
	public:
		CSRoleInfoNoticeReq();
		MessageHeader header;
	};

	class SCRoleInfoNotice		//!< 2851
	{
	public:
		SCRoleInfoNotice();
		MessageHeader header;
		int is_cross;
	};

	class CSRoleTopLevelReq		//!< 2860 �۷�ȼ�����
	{
	public:
		CSRoleTopLevelReq();
		MessageHeader header;

		enum ROLE_TOP_LEVEL_REQ_TYPE
		{
			ROLE_TOP_LEVEL_REQ_TYPE_NOTICE = 0,				//�������
			ROLE_TOP_LEVEL_REQ_TYPE_FETCH = 1,				//��ȡ���� p1 top_level
			ROLE_TOP_LEVEL_REQ_TYPE_FETCH_ACHIEVE = 2,		//��ȡ�ɾͽ��� p1 seq
		};

		short op_type;
		short param1;
	};

	class SCRoleTopLevelAllInfo	//!< 2861 �۷�ȼ�������Ϣ�·�
	{
	public:
		SCRoleTopLevelAllInfo();
		MessageHeader header;

		char is_refresh_first_notice_top_level_flag;			// ˢ�º��״ε��� 0:δ��  1:�ѵ�
		char reserve_ch;
		short reserve_sh;
		int top_level;
		unsigned int next_top_level_refresh_timestamp;			// �۷�ȼ�����ʱ���
		BitMap<MAX_ROLE_TOP_LEVEL_DB_NUM> role_top_level_reward_fetch_flag;
		BitMap<MAX_ROLE_TOP_LEVEL_ACHIEVE_NUM> top_level_achieve_flag;	//�۷�ȼ��ɾͽ�����ȡ��ʶ
	};

	class SCRoleTopLevelNoticce		//!< 2862 �۷�ȼ�֪ͨ
	{
	public:
		SCRoleTopLevelNoticce();
		MessageHeader header;

		enum ROLE_TOP_LEVEL_NOTICE_TYPE
		{
			ROLE_TOP_LEVEL_NOTICE_TYPE_NOITCE = 0,			// ����		P1 0:δ�� 1:�ѵ�
			ROLE_TOP_LEVEL_NOTICE_TYPE_FETCH = 1,			// ��ȡ�ȼ�����  p1 0:δ��ȡ 1:����ȡ P2 top_level
			ROLE_TOP_LEVEL_NOITCE_TYPE_FETCH_ACHIEVE = 2,	// ��ȡ�ɾͽ��� p1 0:δ��ȡ 1:����ȡ p2 seq
		};

		short notice_type;
		short param1;
		int param2;
	};

	class SCRoleTopLevelAOINotice	//!< 2863 �۷�ȼ�֪ͨ�ܱ����
	{
	public:
		SCRoleTopLevelAOINotice();
		MessageHeader header;

		ObjID obj_id;
		short reserve_sh;
		int top_level;		//�۷�ȼ�
	};

	////////////////////////////////////////////////////////////////////
	// ��Դ����

	class CSFetchRecourceDownloadReward			// 3955 ��ȡ��Դ����
	{
	public:
		CSFetchRecourceDownloadReward();
		MessageHeader		header;
	};

	class SCFetchRecourceDownloadReward			// 3956 ��ȡ��Դ�������
	{
	public:
		SCFetchRecourceDownloadReward();
		MessageHeader		header;

		int flag;								//�Ƿ���ȡ���
	};

	////////////////////////////////////////////////////////////////////
	// ��թƭ����
	class CSFangFraudRewardReq					// 3957 ��թƭ����
	{
	public:
		static const int MAX_REQ_NUM = 4;			// ��ѡ������

		CSFangFraudRewardReq();
		MessageHeader		header;

		int count;
		short list[MAX_REQ_NUM];
	};

	class SCFangFraudRewardInfo					// 3958 ��թƭ������Ǳ��
	{
	public:
		SCFangFraudRewardInfo();
		MessageHeader		header;

		int flag;								//�Ƿ���ȡ���
	};

	class SCQueryRoleSimpleInfo					//!< 2025 ���������Ϣ�·�
	{
	public:
		SCQueryRoleSimpleInfo();
		MessageHeader header;

		int role_id;
		GameName		role_name;			// �������
		UInt32			level;				// ����ȼ�
		int				top_level;			// �۷�ȼ�
		int				capability;			// ���ս��
		Appearance appearance_info;			// ��ɫ���
	};

	////////////////////////////////////////////////////////////////////
	// ��������

	class CSFetchPingJiaRewardReq			// 3968 ��������
	{
	public:
		CSFetchPingJiaRewardReq();
		MessageHeader		header;
	};

	class SCFetchPingJiaRewardInfo			// 3969 ����������Ϣ
	{
	public:
		SCFetchPingJiaRewardInfo();
		MessageHeader		header;

		int flag;								//�Ƿ���ȡ���
	};
}

#pragma pack(pop)
#endif