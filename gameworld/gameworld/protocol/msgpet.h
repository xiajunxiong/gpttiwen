#ifndef __MSGPET_H__
#define __MSGPET_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "item/itembase.h"
#include "other/attributelist.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class CSPetOp						//!< 1700
	{
	public:
		CSPetOp();
		MessageHeader		header;

		enum OP_TYPE
		{
			AbandonmentPet = 1,				//!< ��������		����indexΪp1�ĳ���
			ConversionExp = 2,				//!< ת������		��indexΪp1�ĳ���ȼ�����p2��
			Rebirth = 3,					//!< ����			��indexΪp1�ĳ��ﷵ��
			Lock = 4,						//!< ����			��indexΪp1�ĳ������������
			Breakthroughs = 8,				//!< ǿ��			ǿ��indexΪp1�ĳ��� ����indexΪp2 p3 p4�ĳ���  p5 �Ƿ�����
			TransferBreakthroughs = 9,		//!< ǿ��ת��		��indexΪp2�ĳ����ǿ���ȼ�ת����indexΪp1�ĳ�����
			SetPetState = 10,				//!< ���ó���״̬		��indexΪp1�ĳ������ó�p2[1:��ս 2:���� 3:����1 4:����2 5:����3]��״̬
			PetAttrsReq = 11,				//!< �����������		����indexΪp1�ĳ��������
			AddExp = 12,					//!< ʹ�þ���ҩ		������indexΪp1�ĳ���ʹ��item_idΪp2�ĵ���p3��
			RefreshPoints = 13,				//!< ϴ��			������indexΪp1�ĳ���ϴ��
			Refined = 15,					//!< ϴ��			p1����index��p2 �Ƿ���Ҳһ��ϴ��
			ComprehendPassiveSkill = 16,	//!< ����			p1����index
			Compose = 17,					//!< �ϳ�			p1 ������seq
			RefinedSave = 18,				//!< ϴ������		p1����index
			Convert = 19,					//!< ���޶һ���		p1 ������seq�� 
			RefinedGiveUp = 20,				//!< ϴ������  p1����index
			MoveStorage = 21,				//!< �ƶ����ֿ�  p1 ��������index p2 �ֿ����index
			MovePetGrid = 22,				//!< �ƶ�������  p1 ��������index p2 �ֿ����index
			ExtendBag = 23,					//!< ��չ����
			PetEquipmentPutOn = 24,			//!< ����װ������ p1 ����index p2 ��������
			PetEquipmentPutOff = 25,		//!< ����װ������ p1 ����index
			PetEquipmentChange = 26,		//!< ����װ������ p1 ���װ���ĳ���index  p2 ʧȥװ���ĳ���index
			UsePetSkin = 27,		//!< ʹ�ó���Ƥ�� p1 ����index  p2 Ƥ��id
			UpLevelBreak = 28,		//!< ��������ͻ�� p1 ����index  
			UpLevelBreakAtrr = 29,		//!<  �������ͻ������ p1 ����index  
			GuideReplacePassiveSkill = 30,		//!<  ָ������		p1 ����index  
			GuideComprehendPassiveSkill = 31,		//!<  ָ������ p1 ����index  
			GuideRefined = 32,					//!< ָ��ϴ��			p1 ����index  p2 �Ƿ�ϴ������ 
			ReplacePassiveSkillSave = 33,					//!< ������鼼�� p1 ����index  p2 0���棬1�Ƿ���
			PutOnPetSoulEquipMent = 34,				//!< �������� p1�����±꣬p2����index
			PutOffPetSoulEquipMent = 35,			//!< ���»��� p1�����±�
			StepsPetSoulEquipMent = 36,				//!< ͻ�� p1 �Ƿ񴩴���0/1�� p2 �±꣨�����±�/����index��
			RefinePetSoulEquipMent = 37,			//!< ���� p1 �Ƿ񴩴���0/1�� p2 �±꣨�����±�/����index��
			LockPetSoulEquipMent = 38,				//!< �������� p1 �Ƿ񴩴���0/1����p2 �±꣨�����±�/����index��,p3 (0����/1����)
			PutOnPetGodPrintEquipMent = 39,			//!< ������ӡ p1 ����index��p2 ��ӡ����index[1 - 100]
			PutOffPetGodPrintEquipMent = 40,		//!< ������ӡ p1 ����index��p2 ��Ӧ����λ��posi[1 - 100]
			UpLevelPetGodPrintEquipMent = 41,		//!< ������ӡ p1 �����±�index[1 - 100] p2���辭��ֵ
			DisComplsePetGodPrintEquipMent = 42,	//!< �ֽ���ӡ p1 ������Ӧindex[1 - 100]
			PetGodPrintReqWish = 43,				//!< ���� p1 ʹ��ͭ��/Ԫ����0/1�� p2 �������� p3 �Ƿ��Զ��ֽ����
			PetGodPrintReqWishLogList = 44,			//!< ������־�б�
			PetGodPrintReqWishLogDetail = 45,		//!< ������־��ϸ p1 ��־�±�index
			PetGodPrintReqExchange = 46,			//!< ����һ� p1 seq
			PetGodPrintReqLock = 47,				//!< ��ӡ���� p1 �����±�[1-100] p2 �Ƿ�����[1/0]
			PetGodPrintReqUpLevelItem = 48,			//!< ��ӡ���ı����������� p1 ��ӡ�����±�[1-100] p2 �������ͣ�p3 �����±� p4 ��������
			PetGodPrintReqWearScheme = 49,			//!< ����������ӡ p1 ����index p2 ����idx
			PetGodPrintReqDiscomplse = 50,			//!< �ֽⱳ������ӡ���ϻ�ȡ���� p1 �������ͣ�p2 �����±� p3 ��������
			PET_OP_TYPE_KEY_LEVEL_UP = 51,			//!< ����һ���������� P1:pet_index
			PetSoulEquipExchange = 52,				//!< ��������һ� P1:�һ�seq
			PetSoulEquipChange = 53,				//!< �������ת�� p1 �Ƿ񴩴���0 = ����/1 = �� p2 �±� p3 ת������id
			PetMutant = 54,							//!< ����ʹ����Ʒ���� P1:pet_index
			SkillBookConvert = 55,					//!< ����������һ� p1 ����ID1  p2 ����ID2
			PetBenediction  = 56,					//!< ����ף�� P1:main_pet_index(����) P2:vice_pet_index(����)
			PetCancelBenediction = 57,				//!< ȡ������ף�� P1:vice_pet_index(����)
			StartPetFeiShengTask= 58,				//!<  p1: ��ʼ�����������pet_index
			UpLevelPassiveSkill = 59,				//!<  p1: pet_index  p2 ��������index p3 �Ƿ���������
			PetFeiSheng = 60,				//!<  p1: pet_index  p2 ѡ��ļ���index
			PetFeiShengSkillMod = 61,				//!<  p1: pet_index  p2 ѡ��ļ���index
			PetFeiShengRewardFetch = 62,				//!<  ��������¼ p1: mutant_id p2 �ڼ��η�������
			PetFeiShengSkillUplv = 63,				//!<  ������������ p1: pet_index
			PetGodPrintReqExchange2 = 64,			//!< ��ӡ�û� p1:����Ʒ1_idx p2:����Ʒ2_idx
			PetBadgePutOn = 65,				//!<  ���� p1: pet_index  p2,p3 ��������index
			PetBadgeTakeOff = 66,				//!<  ���� p1: pet_index 
			PetBadgeExChange = 67,				//!< �滻 p1: pet_index  p2 pet_index  
			PetBadgeCompose = 68,				//!< �ϳ� p1: ����seq
			PetLianYao = 69,					//!< ���� p1: ���� pet_index   p2 pet_index  
			PetLianYaoComprehendPassiveSkill = 70,			//!< ���������� p1:pet_index 
			PetUnlockTalent = 71,				//!< ���������츳 p1:pet_index p2:talent_type[1,20] p3:inborn_grid[1,3]
			PetTalentSkillLevelUp = 72,			//!< �����츳�������� p1:pet_index p2:talent_type[1,20]
			PetTalentSkillReplace = 73,			//!< �����츳�����滻 p1:pet_index p2:talent_type[1,20] p3:new_inborn_grid[1,3]
			PetTalentReset = 74,				//!< �����츳���� p1:pet_index
			PetStrengthenBuffSet = 75,				//!< ����ǿ��buff p1:�������� p2 ����index p3 pet_index
			PetStrengthenBuffUnSet = 76,				//!< ȡ������ǿ��buff p1:�������� p2 ����index 
			PetGodPrintDevour = 77,				//!< ��ӡ���� p1: ������ӡindex p2:��������ӡindex p3:���ɴ���index
			PetGodPrintDrop = 78,				//!< ������ӡ p1: ����index������ӡ����ǰ󣬷Ǵ����У�
			PetGaiMingUpLevel = 79,				//!< ���� p1:pet_index
			PetGaiMingAttrUpLevel = 80,				//!< ���������� p1: pet_index p2 ��ֵ������������ index
			PetGaiMingAttrReset = 81,				//!< ������������ p1: pet_index 
		};

		int op_type;
		int p1;
		int p2;
		int p3;
		int p4;
		int p5;
	};

	class CSPetRename						//!< ���� 1701
	{
	public:
		CSPetRename();
		MessageHeader		header;

		int index;
		GameName name;
	};

	class CSPetAssignAttributePoints		//!< �������Ե� 1702
	{
	public:
		CSPetAssignAttributePoints();
		MessageHeader		header;

		int index;
		int con;
		int str;
		int def;
		int agi;
		int magic;
	};

	enum PET_ALL_INFO_REASON
	{
		PET_ALL_INFO_REASON_DEFAULT,		//Ĭ��
		PET_ALL_INFO_REASON_TRAIN_SKILL,	//��������
		PET_ALL_INFO_REASON_GUILD_INBORN,	//��������
		PET_ALL_INFO_REASON_XIU_WEI,		//��Ϊ
		PET_ALL_INFO_REASON_STAR_CHART,		//��ͼ
	};
	const static int MAX_PET_SOUL_EQUIPMENT_COST = 10;

	struct MsgPetInfo
	{
		MsgPetInfo() : rank(-1) {}

		PetPro pet_pro;
		int rank;			// ��������0��ʼ��-1Ϊδ�ϰ�
	};

	class SCSendPetAllInfo				//!< 1703 �������е���Ϣ ֻ�ڵ�¼ʱ��һ�� 
	{
	public:
		SCSendPetAllInfo();
		MessageHeader		header;

		int reason;
		int pet_count;		//!< ������������������list����
		int pet_storage_count;
		MsgPetInfo pet_list[ItemNamespace::MAX_PET_GRID_NUM + ItemNamespace::MAX_PET_STORAGE_GRID_NUM];
	};

	class SCSendPetSingleInfo				//!< 1704 �����������Ϣ
	{
	public:
		enum PET_NOTIE_TYPE
		{
			PET_NOTIE_TYPE_NO_TYPE = 0,//!< �ޱ���·�
			PET_NOTIE_TYPE_EXP,//!< ���������·�
			PET_NOTIE_TYPE_REBIRTH,//!< ���ﷵ��
			PET_NOTIE_TYPE_RECOVER,//!< ״̬��ԭ����Ѫ������
			PET_NOTIE_TYPE_HPMP,//!< ��Ѫ����
			PET_NOTIE_TYPE_ASSIGN_POINTS,//!< ���Ե�
			PET_NOTIE_TYPE_SKILL,//!< ���ܱ��
			PET_NOTIE_TYPE_GIFT,//!< �츳���
			PET_NOTIE_TYPE_RENAME,//!< ������
			PET_NOTIE_TYPE_LOCK,//!< ����
			PET_NOTIE_TYPE_STATUS,//!< ����״̬���
			PET_NOTIE_TYPE_BREAK_THROUGHS,//!< ����ͻ��
			PET_NOTIE_TYPE_FABAO,//!< ����Ӱ���������
			PET_NOTIE_TYPE_REFINED,//!< ϴ���·�
			PET_NOTIE_TYPE_COMPREHEND_PASSIVE ,//!< ������
			PET_NOTIE_TYPE_REPLACE_PASSIVE,//!< ����
			PET_NOTIE_TYPE_REFINED_SAVE,//!< ϴ�������·�
			PET_NOTIE_TYPE_BATTLE_ADD_EXP,//!< ����ս���������Ӿ���
			PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE,//!< ����װ�����ѱ��
			PET_NOTIE_TYPE_PET_USE_SKIN,		//!< ʹ�ó���Ƥ��
			PET_NOTIE_TYPE_PET_UP_LEVEL_BREAK,		//!< ʹ�ó�������ͻ��
			PET_NOTIE_TYPE_AUTO_ASSIGN_POINTS,	//!< �Զ��������Ե�
			PET_NOTIE_TYPE_DOMINANCE_CHANGE,	//!< ����ͳ�����
			PET_NOTIE_TYPE_REPLACE_PASSIVE_SAVE,	//!< �������
			PET_NOTIE_TYPE_RESON,				//����
			PET_NOTIE_TYPE_SET_ADD_POINT_INFO,	// �����Զ��ӵ���Ϣ
			PET_NOTIE_TYPE_PET_SOUL_EQUIPMENT,	//!< ����������ѱ��
			PET_NOTIE_TYPE_PET_SOUL_LOCK,		//!< ��������
			PET_NOTIE_TYPE_PET_SOUL_DATA_CHANGE,//!< ������ֵ�仯
			PET_NOTIE_TYPE_PET_GOD_PRINT,		//!< ������ӡ�仯
			PET_NOTIE_TYPE_KEY_LEVEL_UP,		//!< ����һ������
			PET_NOTIE_TYPE_ATTR_PELLET,			//!< ���Ե�
			PET_NOTIE_TYPE_PET_SOUL_EXCHANGE,	//!< ����ת��
			PET_NOTIE_TYPE_BENEDICTION,			//!< ף�����
			PET_NOTIE_TYPE_FINISH_FEI_SHENG_TASK,			//!< ��ɷ����������
			PET_NOTIE_TYPE_FEI_SHENG,			//!< ��ɷ���
			PET_NOTIE_TYPE_FEI_SHENG_SKILL_MOD,			//!< ������������޸�
			PET_NOTIE_TYPE_UPGRADE_SKILL,			//!< ������������
			PET_NOTIE_TYPE_UPGRADE_FEI_SHENG_SKILL,			//!< ����������������
			PET_NOTIE_TYPE_BADGE_CHANGE,			//!< ������¸ı�
			PET_NOTIE_TYPE_LIAN_YAO,			//!< ��������
			PET_NOTIE_TYPE_LIAN_YAO_COMPREHEND_PASSIVE_SKILL, //!< ��������������
			PET_NOTIE_TYPE_UNLOCK_TALENT,			//!< 42.���������츳
			PET_NOTIE_TYPE_TALENT_SKILL_LEVEL_UP,	//!< 43.�����츳��������
			PET_NOTIE_TYPE_TALENT_SKILL_REPLACE,	//!< 44.�����츳�����滻
			PET_NOTIE_TYPE_TALENT_RESET,			//!< 45.�����츳����
			PET_NOTIE_TYPE_HOLY_BLESS,				//!< 46.ʥ��ʥ��
			PET_NOTIE_TYPE_STRENGTH_BUFF,				//!< 47.���ﹲ��
			PET_NOTIE_TYPE_GAI_MING,				//!< 48.�������
			PET_NOTIE_TYPE_GAI_MING_ATTR,				//!< 49.�����������
			PET_NOTIE_TYPE_GAI_MING_ATTR_RESET,			//!< 50.���������������
		};
		SCSendPetSingleInfo();
		MessageHeader		header;

		int notie_type;
		int add_exp;				// ս�������������ӵľ���
		MsgPetInfo pet_info;
	};

	class SCSendPetDeleteInfo				//!< 1706 ɾ������
	{
	public:
		SCSendPetDeleteInfo();
		MessageHeader		header;

		int pet_index;
	};

	class SCSendPetOtherInfo			//!< 1705 ����������Ϣ
	{
	public:
		SCSendPetOtherInfo();
		MessageHeader		header;

		short		fight_index;		//!< ��ս����index
		short		follow_index;		//!< �������index
		short		standby_index1;		//!< ��������index
		short		standby_index2;		//!< ��������index
		short		standby_index3;		//!< ��������index
		short		extend_grid_num;	//!< ����չ�ĸ���
		short		extend_times;		//!< ����չ�Ĵ���
		short		standby_index4;		//!< ��������index
		short		standby_index5;		//!< ��������index
	};

	class SCSendPetAttrListInfo			//!< 1707 ��������
	{
	public:
		SCSendPetAttrListInfo();
		MessageHeader		header;

		int index;
		int capability;
		AttributeList attr_list;
	};

	class SCSendPetBreakthroughsRet			//!< 1708 ǿ������
	{
	public:
		SCSendPetBreakthroughsRet();
		MessageHeader		header;

		int index;
		int is_succ;		/// < 0ʧ�� 1 �ɹ�
		int old_level;
		int is_liao_yao_pet;
	};

	class SCSendPetGetRet			//!< 1709 ��ó��ﷵ��
	{
	public:
		SCSendPetGetRet();
		MessageHeader		header;

		int index;
		int reason;
	};

	class SCPetFollowStatus
	{
	public:
		SCPetFollowStatus();		// 1690 - ���������Ϣ
		MessageHeader header;

		int role_obj_id;			// ���ObjID
		int follow_pet_id;			// ����ID
		GameName follow_pet_name;	// ��������
		int skin_id;
		int fei_sheng_times;		
	};

	class SCSealPetGetItemInfo
	{
	public:
		SCSealPetGetItemInfo();		// 1691 - ��ץ����õ�ʲô��Ʒ
		MessageHeader header;

		int pet_id;
		int item_id;
		short num;
		short is_get_pet;	//!< 0: �����Ƭ   1:��ó���
	};

	class SCPetIdRecordList  //!<1692 ����ͼ��
	{
	public:
		SCPetIdRecordList();		
		MessageHeader header;

		int count;
		unsigned short pet_id_list[PET_ID_MAX_COUNT];
	};

	class CSAbandonPetByList			//!< 1693 ������������
	{
	public:
		CSAbandonPetByList();
		MessageHeader		header;

		unsigned char del_pet_index_list[ItemNamespace::MAX_PET_GRID_NUM];
	};

	enum MOVE_PET_ITEM_NOTICE_TYPE
	{
		MOVE_PET_ITEM_NOTICE_TYPE_FAIL,			// ��ʧ��
		MOVE_PET_ITEM_NOTICE_TYPE_STORAGE,		//�ƶ����ֿ�
		MOVE_PET_ITEM_NOTICE_TYPE_GRID,			//�ֿ��ƶ�������
	};

	class SCMovePetGridNotice				//!< 1694 �ƶ���Ʒ֪ͨ
	{
	public:
		SCMovePetGridNotice();
		MessageHeader		header;

		int type;				//!< 0��ʧ�ܣ�1���ƶ����ֿ⣬2�ǲֿ��ƶ�������
		int pet_index;
		int pet_storage_index;
	};

	class SCPetSkinInfo				//!< 1695 ����Ƥ��
	{
	public:
		SCPetSkinInfo();
		MessageHeader		header;

		int count;
		int pet_skin_list[PET_SKIN_MAX_NUM];
	};

	class SCPetCalAttrRet				//!< 1696 �ͻ���������ʾͻ��Ԥ����ֵ����ֹ���
	{
	public:
		SCPetCalAttrRet();
		MessageHeader		header;

		int index;
		AttributeList attr_list;
	};

	class CSPetSoulUplevelReq			//!< 1697 ��������
	{
	public:
		CSPetSoulUplevelReq();
		MessageHeader		header;
		const static int MAX_USE_ITEM_NUM = 30;//������ĵ�����
		int is_put_on;
		short index;
		short pet_soul_index;
		int cost_num;
		short bag_colum[MAX_USE_ITEM_NUM];
		short bag_idx[MAX_USE_ITEM_NUM];
		short bag_num[MAX_USE_ITEM_NUM];
	};
	UNSTD_STATIC_CHECK(sizeof(CSPetSoulUplevelReq) % 4 == 0);

	struct GodPrintItemSendInfo
	{
		int bag_idx;//�����±�
		GodPrintItem item_info;//������Ϣ
	};

	class SCPetGodPrintBagInfo			//!< 1661 ��ӡ����
	{
	public:
		SCPetGodPrintBagInfo();
		MessageHeader		header;

		int round_seq;
		long long exp;//���еľ���
		int today_free_wish_times;//ʹ������������
		GodPrintWearScheme wear_scheme_list[MAX_PET_GOD_PRONT_SCHEME_NUM];	//Ԥ�淽��
		int bag_num;
		GodPrintItemSendInfo bag_list[NEW_MAX_PET_GOD_PRONT_BAG_NUM];			//��ӡ����
	};

	class SCPetGodPrintBagSingleInfo			//!< 1660 ��ӡ���������޸�
	{
	public:
		SCPetGodPrintBagSingleInfo();
		MessageHeader		header;
		int put_reason;
		int bag_idx;
		long long exp;
		GodPrintItem bag_list;				//��ӡ����
	};

	class SCPetGodPrintEXPInfo			//!< 1662 ��ӡ�����
	{
	public:
		SCPetGodPrintEXPInfo();
		MessageHeader		header;

		long long exp;
	};

	class SCPetGodPrintWearSchemeSingleInfo			//!< 1663 ��ӡ�������������·�
	{
	public:
		SCPetGodPrintWearSchemeSingleInfo();
		MessageHeader		header;

		int bag_idx;
		GodPrintWearScheme bag_list;				//��ӡ����
	};

	class CSPetGodPrintSetearSchemeReq			//!< 1664 ������ӡ��������
	{
	public:
		CSPetGodPrintSetearSchemeReq();
		MessageHeader		header;

		int bag_idx;
		GodPrintWearScheme bag_list;				//��ӡ����
	};

	class SCPetGodPrintWishLogListInfo			//!< 1665 ��ӡ��־�б��·�
	{
	public:
		SCPetGodPrintWishLogListInfo();
		MessageHeader		header;
		struct GodPrintLogSinple
		{
			GodPrintLogSinple() :log_tamp(0), item_id(0) {}
			unsigned int log_tamp;//��־ʱ���
			int item_id;//��ӡid
		};
		int log_num;	//��־����
		GodPrintLogSinple log_data[MAX_GOD_PRINT_WISH_LOG_NUM];
	};

	class SCPetGodPrintWishLogDetailInfo			//!< 1666 ��ӡ��־��ϸ�·�
	{
	public:
		SCPetGodPrintWishLogDetailInfo();
		MessageHeader		header;

		NewWishLogData log_detail;
	};
	const static int MAX_GOD_PRINT_WISH_REWARD_NUM = 80;//�������������
	class SCPetGodPrintWishRewardInfo			//!< 1667 �������·�
	{
	public:
		SCPetGodPrintWishRewardInfo();
		MessageHeader		header;

		struct WishInfo
		{
			WishInfo() {}
			int bag_idx;//�����±�
			int wish_idx;//����seq
			int item_group;//ͨ�ò���group
			int item_seq;//ͨ�ò���seq
		};
		int today_free_times;//ʹ������������
		int use_gold;//[0/1]�Ƿ�ʹ��Ԫ��
		int real_cost_coin;//ʵ�ʻ���
		int reward_num;
		int end_seq;//����ͣ���±�
		WishInfo reward_list[MAX_GOD_PRINT_WISH_REWARD_NUM];//��ӡ�����±�
	};

	class CSPetGodPrintUplevelReq			//!< 1668 ��ӡ������ӡ����
	{
	public:
		CSPetGodPrintUplevelReq();
		MessageHeader		header;
		
		int index;
		int bag_num;
		short bag_idx[MAX_USE_ITEM_NUM];
	};

	class SCPetBenedictionAllInfo		//!< 1698 ����ף���б���Ϣ�·�
	{
	public:
		SCPetBenedictionAllInfo();
		MessageHeader header;

		short benediction_pet_index_list[ItemNamespace::MAX_PET_GRID_NUM];		// ����ף���б�
	};

	class SCPetBenedictionSingleInfo	//!< 1699 ��������ף����Ϣ�·�
	{
	public:
		SCPetBenedictionSingleInfo();
		MessageHeader header;

		short pet_index;				// �ĸ�����
		short main_pet_index;			// �ܵ�ף���ĳ���
	};

	class SCPetFeiShengTaskInfo	//!<4300 �����������
	{
	public:
		SCPetFeiShengTaskInfo();
		MessageHeader header;

		long long pet_uuid;
		int today_is_finish;
		int task_id;

	};

	class SCPetFeiShengRewardInfo				//!< 4420 
	{
	public:
		SCPetFeiShengRewardInfo();
		MessageHeader		header;

		int count;
		PetYiWenLuRewardParam infos[100];
	};

	class CSPetComposeBatch		//!< 4430   �����ϳ�
	{
	public:
		CSPetComposeBatch();
		MessageHeader		header;

		struct Item
		{
			short seq;
			short use_shards_num;
		};
		
		Item batch_req[100];   //���ò���100
	};

	class SCPetComposeBatchRet		//!< 4431   �����ϳ� ����
	{
	public:
		SCPetComposeBatchRet();
		MessageHeader		header;

		int count;
		short pet_index_list[100];
	};

	class CSPetComposeAndDecomposeBatch		//!< 4432   �����ϳɺ󲢷ֽ⣬���Ƕһ�����
	{
	public:
		CSPetComposeAndDecomposeBatch();
		MessageHeader		header;

		struct Item
		{
			short seq;
			short num;
		};

		Item batch_req[100];   //���ò���100
	};

	class CSPetBadgeDecompose   //4433 ������·ֽ�
	{
	public:
		CSPetBadgeDecompose();
		MessageHeader header;

		KnapsackItemIndex decompose_list[100];
	};

	class SCPetStrengthenBuffInfo   //4434 ����ǿ��
	{
	public:
		SCPetStrengthenBuffInfo();
		MessageHeader header;

		PetStrengthenBuffParamItem strengthen_buff_item_list[PET_STRENGTHEN_BUFF_TYPE_NUM];
	};

	class SCPetLianYaoCommonInfo   //5230 ����������Ϣ
	{
	public:
		SCPetLianYaoCommonInfo();
		MessageHeader header;

		unsigned int is_unlock_lian_yao_furance_flag; 
	};

	enum PET_ADD_EXP_NOTICE_TYPE
	{
		PET_ADD_EXP_NOTICE_TYPE_GE_ROLE_LVEL=0, //���ڵ��ڽ�ɫ�ȼ�
	};

	class SCPetAddExpNotice   //5231 ������Ӿ���֪ͨ
	{
	public:
		SCPetAddExpNotice();
		MessageHeader header;

		int notice_type;
		int pet_index;
	};
}

#pragma pack(pop)
#endif