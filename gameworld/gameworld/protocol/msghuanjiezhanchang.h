#ifndef __MSG_HUAN_JIE_ZHAN_CHANG_HPP__
#define __MSG_HUAN_JIE_ZHAN_CHANG_HPP__

#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "item/itembase.h"
#include "gameworld/equipment/jewelry.h"
#include "servercommon/fabaodef.hpp"
#include "other/attributelist.hpp"
#include "servercommon/huashendef.hpp"
#include "servercommon/rolehuanjiedef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum HUAN_JIE_ZHAN_CHANG_REQ_TYPE
	{
		HUAN_JIE_ZHAN_CHANG_REQ_TYPE_MATCH,		//ƥ��
		HUAN_JIE_ZHAN_CHANG_REQ_TYPE_CANCEL_MATCH,		//ȡ��ƥ��
		HUAN_JIE_ZHAN_CHANG_REQ_TYPE_RANK_INFO,		//���а���Ϣ p1 server_id(�ĸ������а�0ʱ���)
		HUAN_JIE_ZHAN_CHANG_REQ_QUERY_OTHER_INFO,		//��ѯ�������
		HUAN_JIE_ZHAN_CHANG_REQ_INFO,		// �Լ���Ϣ
		HUAN_JIE_ZHAN_CHANG_PROF_WIN_LOSE_INFO,		//    �������ְҵ��սʤ����Ϣ���·�3309��
		HUAN_JIE_ZHAN_CHANG_LEADER_VIEW,		//    �ӳ����� p1 :0 �ǹرգ�1�Ǵ�
		HUAN_JIE_ZHAN_CHANG_FETCH_TODAY_REWARD,		//    ��ȡ����ʤ������ p1 ������seq
	};

	class CSHuanJieZhanChangReq			//!< 3290
	{
	public:
		CSHuanJieZhanChangReq();
		MessageHeader header;

		int req_type;
		int param1;
	};


	class SCHuanJieZhanChangTeamInfo			//!< 3291
	{
	public:
		SCHuanJieZhanChangTeamInfo();
		MessageHeader header;

		int is_open;
	};

	enum HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE
	{
		HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_HUA_SHEN,			//û��ʼ������
		HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_ZHAN_LI,			//����
		HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_CANN_FIGHT,		//�����޷�ս��״̬
		HUAN_JIE_ZHAN_CHANG_ERR_RETURN_RESON_TYPE_IN_MATCHING,		//����ƥ��״̬
	};


	class SCHuanJieZhanChangErrReturn                                    //!< 3292
	{
	public:
		SCHuanJieZhanChangErrReturn();
		MessageHeader header;

		int role_id;
		int reson;
	};

	enum HUAN_JIE_ZHAN_CHANG_NOTICE_TYPE
	{
		HUAN_JIE_ZHAN_CHANG_NOTICE_TYPE_,
	};

	class SCHuanJieZhanChangNotice //!< 3292
	{
	public:
		SCHuanJieZhanChangNotice();
		MessageHeader header;

		int is_open;
	};

	class SCHuanJieZhanChangMatchState		//!< 		3301  ƥ��״̬
	{
	public:
		SCHuanJieZhanChangMatchState();
		MessageHeader header;
		
		int match_state;
	};

	class SCHuanJieZhanChangResult   //!< 		3302  ս�����
	{
	public:
		SCHuanJieZhanChangResult();
		MessageHeader header;

		int result;				//���
		int rank_score;			//��ǰ��
		int win_streak_times;   //��ʤ����
		int is_finish_ding_ji;	//�Ƿ���ɶ�����
	};

	struct HuanJieZhanChangOpponentInfoItem
	{
		int role_id;
		GameName role_name;
		int prof;
		int score;
		short avatar_type;
		short headshot_id;
		int is_leader;
		int is_finis_ding_ji;
	};

	class SCHuanJieZhanChangOpponentInfo		//!< 		3303  ������Ϣ
	{
	public:
		SCHuanJieZhanChangOpponentInfo();
		MessageHeader header;

		unsigned int start_time;			//����ʱ���
		HuanJieZhanChangOpponentInfoItem opponent_info_item1[3];
		HuanJieZhanChangOpponentInfoItem opponent_info_item2[3];
	};

	class SCHuanJieZhanChangRoleInfo   //!< 3304  ��ɫ��Ϣ
	{
	public:
		SCHuanJieZhanChangRoleInfo();
		MessageHeader header;

		int season_num;		//����
		unsigned int end_time;
		int rank_score;			//��ǰ��
		int max_rank_score;		//��ǰ�����ﵽ�����λ����
		int win_streak_times;   //��ʤ����
		unsigned int total_times;		//�ܳ���
		unsigned int win_times;		//ʤ������
		unsigned int draw_times;		//ƽ�ִ���
		int is_finish_ding_ji;	//�Ƿ���ɶ�����
		int today_get_huan_jie_coin;	//��ûý������
		int today_win_times;	//����ʤ��
		int today_win_reward_flag;	//����ʤ��������ȡ��־
	};

	class SCHuanJieZhanChangRankInfo   //!< 3305  ���а���Ϣ
	{
	public:
		SCHuanJieZhanChangRankInfo();
		MessageHeader header;

		int server_id;
		int count;
		ActivityHuanJieZhanChangRankItemCommon rank_list[ACTIVITY_HUAN_JIE_ZHAN_CHANG_RANK_LIST_MAX_NUM];
	};

	class CSHuanJieZhanChangRoleRankListReq			//!< 3306  ��ѯ������а�����
	{
	public:
		CSHuanJieZhanChangRoleRankListReq();
		MessageHeader header;

		int role_id[100];						//���ò���100���ж��پͶ���
	};

	class SCHuanJieZhanChangRoleRankListing   //!< 3307  ��ѯ������а����� ����
	{
	public:
		struct RoleInfoItem
		{
			RoleInfoItem()
			{
				role_id = 0;
				score = 0;
				rank_pos = 0;
				cross_rank_pos = 0;
				join_time = 0;
				is_finish_ding_ji = 0;
			}

			int role_id;				
			int score;				//����
			int rank_pos;			//��������
			int cross_rank_pos;		//�������
			unsigned int join_time;		// �ﵽ����ʱ��
			int is_finish_ding_ji;
		};

		SCHuanJieZhanChangRoleRankListing();
		MessageHeader header;

		int count;
		RoleInfoItem role_infos[100];
	};

	enum SC_HUANJIE_ZHAN_CHANG_TIAN_TI_RECORD_TYPE
	{
		SC_HUANJIE_ZHAN_CHANG_TIAN_TI_RECORD_TYPE_ALL,
		SC_HUANJIE_ZHAN_CHANG_TIAN_TI_RECORD_TYPE_ADD,
	};

	class SCHuanJieZhanChangTianTiRecord   //!< 3308  ��ս��¼
	{
	public:

		SCHuanJieZhanChangTianTiRecord();
		MessageHeader header;

		int type;
		int count;
		CommonSaveHuanJieTianTiReceordData record[HUAN_JIE_TIAN_TI_RECORD_MAX_NUM];
	};


	class SCHuaShenRoleInfoAck			//!< 3310 ���������Ϣ�·�
	{
	public:
		SCHuaShenRoleInfoAck();
		MessageHeader		header;

		int role_level;			//!< ����ȼ�
		Appearance appearance;	//!< ���
	};

	class SCHuaShenRoleAttrList			//!< 3311 ���������б�
	{
	public:
		SCHuaShenRoleAttrList();
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
		};

		short attr_count;							//!< �·�����������
		short notify_reason;						//!< �·������� \see NOTIFY_REASON
		int capability;								//!< ���ս��(����)
		AttrPair attr_list[BATTLE_ATTR_MAX];		//!< ����������attr_count����  \see BATTLE_ATTR_MAX 50
	};


	class CSHuaShenMedalReq					//!< 3312 ����ѫ������
	{
	public:
		CSHuaShenMedalReq();
		MessageHeader		header;

		enum HUASHEN_MEDAL_REQ_TYPE
		{
			HUASHEN_MEDAL_REQ_TYPE_INFO,			//!< ������Ϣ
			HUASHEN_MEDAL_REQ_TYPE_PUT_ON,			//!< ����װ�� p1 grid_index  p2 index
			HUASHEN_MEDAL_REQ_TYPE_TAKE_OFF,		//!< ����װ�� p1 grid_index
		};

		int req_type;
		int param1;
		int param2;
	};

	class SCHuaShenMedalInfo : public IMessageSerializer	//!<3313 �·���ʱ���ڴ�����ѫ��
	{
	public:
		SCHuaShenMedalInfo();
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(count, buffer, buffer_size, out_length);
			for (int i = 0; i < count && i < ARRAY_LENGTH(grid_info); ++i)
			{
				const GridItem & grid = grid_info[i];

				result = result && WriteInt(grid.grid_index, buffer, buffer_size, out_length);
				result = result && WriteUShort(grid.item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(grid.num, buffer, buffer_size, out_length);
				result = result && WriteChar(grid.is_bind, buffer, buffer_size, out_length);
				result = result && WriteChar(grid.has_param, buffer, buffer_size, out_length);
				result = result && WriteUInt(grid.invalid_time, buffer, buffer_size, out_length);
				if (0 != grid.has_param)
				{
					result = result && WriteShort(grid.param_length, buffer, buffer_size, out_length);
					result = result && WriteStrN(grid.param_data, grid.param_length, buffer, buffer_size, out_length);
				}
			}
			return result;
		}

		struct GridItem
		{
			GridItem()
			{
				grid_index = 0;
				item_id = 0;		//!< ��Ʒid
				num = 0;			//!< ����
				is_bind = 0;		//!< �Ƿ��
				has_param = 0;		//!< �Ƿ��о�ֵ����
				invalid_time = 0;	//!< ����ʱ��
				param_length = 0;	//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
				memset(param_data, 0, sizeof(param_data));
			}

			int					grid_index;
			ItemID				item_id;		//!< ��Ʒid
			short				num;			//!< ����
			char				is_bind;		//!< �Ƿ��
			char				has_param;		//!< �Ƿ��о�ֵ����
			UInt32				invalid_time;	//!< ����ʱ��
			short				param_length;	//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
			NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
		};

		int count;
		GridItem grid_info[MAX_MEDAL_WEAR_GRID];
	};

	class CSHuaShenEquipmentOperaReq						//!< 3314 ����װ����������
	{
	public:
		CSHuaShenEquipmentOperaReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_INFO_REQ = 0,			//!< 0 ������Ϣ
			OPERA_TYPE_PUTON = 1,				//!< 1 ����װ�� \param param1 0  \param param2 �����������  \param param3 �����Ĳ�λ \see EQUIPMENT_INDEX \ param param4 role_type 
			OPERA_TYPE_TAKEOFF = 2,				//!< 2 ����װ�� \param param1 Ҫ���µĲ�λ \see EQUIPMENT_INDEX \ param2 role_type
			OPERA_TYPE_TAKEOFF_CRYSTAL = 3,		//!< 3 ����Ԫ��ˮ��
			OPERA_TYPE_PUTON_CRYSTAL = 4,		//!< 5 װ��Ԫ��ˮ�� \param param1 �������������
			OPERA_TYPE_INORDER = 5,				//!< 6 ������
		};

		int msg_type;							//!< ��Ϣ����  \see OPERA_TYPE
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class SCHuaShenEquipmentInfoAck					//!< 3315 ����װ����Ϣ
	{
	public:
		SCHuaShenEquipmentInfoAck();
		MessageHeader header;

		ItemID element_crystal_id;				//!< Ԫ��ˮ������ƷID
		short used_times;						//!< Ԫ��ˮ����ʹ�ô���

		//����װ��
		short reserve_short;
		short huashen_count;
		EquipmentPro huashen_equipment_list[ItemNamespace::MAX_EQUIPMENT_GRID_NUM];	//����װ�� MAX_EQUIPMENT_GRID_NUM 5
	};

	class SCHuaShenEquipmentGridChange				//!< 3316 ����װ���ı�֪ͨ
	{
	public:
		SCHuaShenEquipmentGridChange();
		MessageHeader header;

		EquipmentPro equip_info;
	};

	class CSHuaShenJewelryHandleReq					//!< 3317�� ���� ϵͳ��������
	{
	public:
		CSHuaShenJewelryHandleReq();
		MessageHeader header;

		enum OPERA_TYPE
		{
			OPERA_TYPE_ALL_INFO_REQ = 0,		//!< �������� ���� ��Ϣ
			OPERA_TYPE_WEAR_JEWELRY = 1,		//!< ����װ������
			OPERA_TYPE_TAKE_OFF_ONE = 2,		//!< ������������
		};

		short op_type;		//!< ��������

		short param;		//!< ���������0 - �ޣ� 1 - ��������ID�� 2 - ����װ����ID��
	};

	class SCHuaShenJewelryAllInform					//!< 3318�� װ��������Ϣ����
	{
	public:
		SCHuaShenJewelryAllInform();
		MessageHeader header;

		int jewelry_count;		//!< ���� ����
		JewelryPro jewelry_list[Jewelry::J_TYPE_MAX];	// J_TYPE_MAX 3
	};

	class SCHuaShenJewelryInfoChange					//!< 3319�� ����������Ϣ����
	{
	public:
		SCHuaShenJewelryInfoChange();
		MessageHeader header;

		JewelryPro jewelry_info;
	};

	class CSHuaShenAttributePlanPointReq				//!< 3320 ����Ǳ�ܲ���
	{
	public:
		CSHuaShenAttributePlanPointReq();
		MessageHeader header;

		enum ATTRIBUTE_PLAN_TYPE
		{
			ATTRIBUTE_PLAN_TYPE_CLEAR_POINT = 0,	//ϴ��
			ATTRIBUTE_PLAN_TYPE_ADD_POINT = 1,		//�ӵ�
			ATTRIBUTE_PLAN_TYPE_SWITCH = 2,			//�л�����
		};

		short op_type;
		short delta_attr_list[ADD_POINT_TYPE_MAX];	//!< �ı�ĵ����� ��Ϊ����  \see ADD_POINT_TYPE_MAX 5
		int cur_plan_index;							//!< ѡ�еķ������
	};

	struct PlanDetail
	{
		char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< �ƻ���  \see ATTRIBUTE_PLAN_NAME_LENGTH
		short remain_point;							//!< ʣ�����
		short add_point_list[ADD_POINT_TYPE_MAX];   //!< �ӵ����ֵ \see ADD_POINT_TYPE_MAX
	};

	class SCHuaShenRoleAttributePlanInfo				//!< 3321 ����ӵ���Ϣ
	{
	public:
		SCHuaShenRoleAttributePlanInfo();
		MessageHeader header;

		int cur_plan_index;							//!< �÷������ [0,3)
		PlanDetail attr_plan_info;					//!< �÷�����Ϣ
	};

	class CSHuaShenPetOp						//!< 3322 ����������
	{
	public:
		CSHuaShenPetOp();
		MessageHeader		header;

		enum OP_TYPE
		{
			SetPetState = 0,				//!< ���ó���״̬		��indexΪp1�ĳ������ó�p2[1:��ս 2:���� 3:����1 4:����2 5:����3]��״̬
			RefreshPoints = 1,				//!< ϴ��			������indexΪp1�ĳ���ϴ��
			PetAttrsReq = 2,				//!< �����������		����indexΪp1�ĳ��������
			PetEquipmentPutOn = 3,			//!< ����װ������ p1 ����index p2 ��������
			PetEquipmentPutOff = 4,			//!< ����װ������ p1 ����index
		};

		int op_type;
		int p1;
		int p2;
	};

	class CSHuaShenPetAssignAttributePoints		//!< 3323 �����������������Ե� 
	{
	public:
		CSHuaShenPetAssignAttributePoints();
		MessageHeader		header;

		int index;
		int con;
		int str;
		int def;
		int agi;
		int magic;
	};

	class SCHuaShenSendPetAllInfo				//!< 3324 �������е���Ϣ 
	{
	public:
		SCHuaShenSendPetAllInfo();
		MessageHeader		header;

		enum HUASHEN_PET_ALL_INFO_REASON
		{
			HUASHEN_PET_ALL_INFO_REASON_DEFAULT = 0,		//Ĭ��
			HUASHEN_PET_ALL_INFO_REASON_TRAIN_SKILL,	//��������
		};

		int reason;
		int pet_count;		//!< ������������������list����
		PetPro pet_list[ItemNamespace::MAX_HUASHEN_PET_GRID_NUM];	//MAX_HUASHEN_PET_GRID_NUM 100
	};
	
	class SCHuaShenSendPetSingleInfo				//!< 3325 �����������Ϣ
	{
	public:
		enum PET_NOTIE_TYPE
		{
			PET_NOTIE_TYPE_NO_TYPE = 0,			//!< �ޱ���·�
			PET_NOTIE_TYPE_ASSIGN_POINTS,		//!< ���Ե�
			PET_NOTIE_TYPE_STATUS,				//!< ����״̬���
			PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE,//!< ����װ�����ѱ��

		};
		SCHuaShenSendPetSingleInfo();
		MessageHeader		header;

		int notie_type;		
		PetPro pet_info;
	};

	class SCHuaShenSendPetOtherInfo			//!< 3326 ����������Ϣ
	{
	public:
		SCHuaShenSendPetOtherInfo();
		MessageHeader		header;

		short		fight_index;		//!< ��ս����index
		short		standby_index1;		//!< ��������index
		short		standby_index2;		//!< ��������index
		short		standby_index3;		//!< ��������index
		short		standby_index4;		//!< ��������index
		short		standby_index5;		//!< ��������index
	};

	class SCHuaShenSendPetAttrListInfo			//!< 3327 ��������
	{
	public:
		SCHuaShenSendPetAttrListInfo();
		MessageHeader		header;

		int index;
		int capability;
		AttributeList attr_list;
	};

	class CSHuaShenInfoReq						//!< 3328 ����ģ����Ϣ����
	{
	public:
		CSHuaShenInfoReq();
		MessageHeader		header;

		enum HUASHEN_INFO_TYPE
		{
			HUASHEN_INFO_TYPE_ALL_INFO,			//������Ϣ
			HUASHEN_INFO_TYPE_RENWU,			//����������Ϣ
			HUASHEN_INFO_TYPE_POINT,			//����ǰ������Ϣ
			HUASHEN_INFO_TYPE_KNAPSACK,			//������Ϣ
			HUASHEN_INFO_TYPE_PET,				//���г���
			HUASHEN_INFO_TYPE_PET_POINT,		//����ӵ���Ϣ  p1 : pet_index
			HUASHEN_INFO_TYPE_FIRST_ENTER,		//�򿪻ý�ս������������߻�ͬ����Ϣ
			HUASHEN_INFO_TYPE_FINISH_GUIDE,		//��ɻ���ָ��
		};
			
		int op_type;
		int param1;
	};

	class SCHuaShenKnapsackInfoAck : public IMessageSerializer  //!< 3329 ������Ϣ����
	{
	public:
		enum HUASHEN_SEND_REASON
		{
			HUASHEN_SEND_REASON_NORMAL = 0,			//!< 0 �ͻ�������������Ϣ
			HUASHEN_SEND_REASON_IN_ORDER = 1,		//!< 1 ����
			HUASHEN_SEND_REASON_REQ = 2,			//!< 2 �ͻ��˵������󱳰���Ϣ
		};

	public:
		SCHuaShenKnapsackInfoAck();
		virtual ~SCHuaShenKnapsackInfoAck() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(valid_grid_num, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(item_count, buffer, buffer_size, out_length);

			for (int i = 0; i < item_count && i < ItemNamespace::HUASHEN_ITEM_GRID_NUM_MAX; ++i)
			{
				result = result && WriteUShort(item_list[i].item_id, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].column, buffer, buffer_size, out_length);
				result = result && WriteShort(item_list[i].index, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_market_buy_flag, buffer, buffer_size, out_length);
				result = result && WriteInt(item_list[i].num, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].is_bind, buffer, buffer_size, out_length);
				result = result && WriteChar(item_list[i].has_param, buffer, buffer_size, out_length);
				result = result && WriteUInt(item_list[i].invalid_time, buffer, buffer_size, out_length);
				result = result && WriteLL(item_list[i].item_unique_id, buffer, buffer_size, out_length);

				if (1 == item_list[i].has_param)
				{
					result = result && WriteShort(item_list[i].param_length, buffer, buffer_size, out_length);
					result = result && WriteStrN(item_list[i].param_data, item_list[i].param_length, buffer, buffer_size, out_length);
				}
			}

			return result;
		}

		int			valid_grid_num;											//!< ��λ��Ч�������б� \see ITEM_COLUMN_TYPE_MAX
		short		reason;													//!< �·�����
		short		item_count;												//!< ��Ʒ����
		InfoType	item_list[ItemNamespace::HUASHEN_ITEM_GRID_NUM_MAX];	//!< ��Ʒ�б�  HUASHEN_ITEM_GRID_NUM_MAX 400
	};


	class SCHuaShenKnapsackItemChange : public IMessageSerializer  //!< 3330 ������Ʒ�ı�֪ͨ
	{
	public:
		SCHuaShenKnapsackItemChange();
		virtual ~SCHuaShenKnapsackItemChange() {}
		MessageHeader		header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);
			result = result && WriteShort(reason, buffer, buffer_size, out_length);
			result = result && WriteShort(put_reason, buffer, buffer_size, out_length);
			result = result && WriteUShort(item_id, buffer, buffer_size, out_length);
			result = result && WriteShort(column, buffer, buffer_size, out_length);
			result = result && WriteShort(index, buffer, buffer_size, out_length);
			result = result && WriteChar(is_market_buy_flag, buffer, buffer_size, out_length);
			result = result && WriteInt(num, buffer, buffer_size, out_length);
			result = result && WriteChar(is_bind, buffer, buffer_size, out_length);
			result = result && WriteChar(has_param, buffer, buffer_size, out_length);
			result = result && WriteUInt(invalid_time, buffer, buffer_size, out_length);
			result = result && WriteLL(item_unique_id, buffer, buffer_size, out_length);
			if (1 == has_param)
			{
				result = result && WriteShort(param_length, buffer, buffer_size, out_length);
				result = result && WriteStrN(param_data, param_length, buffer, buffer_size, out_length);
			}

			return result;
		}

		short				reason;					//!< �ı��ԭ��
		short				put_reason;				//!< �����Ʒ����Դ  \see PUT_REASON_TYPE
		ItemID				item_id;				//!< ��Ʒid
		short				column;					//!< ��Ʒ���ĸ��������� [0,9)
		short				index;					//!< ��Ʒ�ڱ����е���� 
		short				num;					//!< ����
		long long           item_unique_id;			//!< ����ΨһID
		char				is_market_buy_flag;		//!< �Ƿ���г������ʶ 0:�� 1:���г�����
		char				reserve_ch;
		char				is_bind;				//!< �Ƿ��
		char				has_param;				//!< �Ƿ��о�ֵ����
		UInt32				invalid_time;			//!< ����ʱ��
		short				param_length;			//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
		NetValueItemParamData param_data;			//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���
	};

	class CSSetHuaShenBattlePosition		//!< 3331 ��������վλ   
	{
	public:
		CSSetHuaShenBattlePosition();
		MessageHeader header;

		int position;				//!< ����վλ  ��0������   1��ǰ�ţ�
	};

	class SCSetHuaShenBattlePosition		//!< 3332 ��������վλ���� 
	{
	public:
		SCSetHuaShenBattlePosition();
		MessageHeader header;

		int position;				//!< ����վλ  ��0������   1��ǰ�ţ�
	};

	class SCHuaShenSeasonInfo			//!< 3333 �·���ұ�������Ϣ
	{
	public:
		SCHuaShenSeasonInfo();
		MessageHeader header;

		char season_first_enter;			// 0:��������δ����   1:�������Ѿ������ 
		char is_finish_guide;				// �Ƿ����ָ�� 0:δ��� 1:�����
		short season_reset_first_dijing;	// 0:�������û�δ���� 1:���������ѽ���(��������ʾ)
	};

	class SCHuaShenElementCrystalChangeNotice			//!< 3334 ����Ԫ��ˮ�����֪ͨ
	{
	public:
		SCHuaShenElementCrystalChangeNotice();
		MessageHeader header;

		ItemID element_crystal_id;				//!< Ԫ��ˮ������ƷID
		short used_times;						//!< Ԫ��ˮ����ʹ�ô���
	};

	class SCHuaShenAttrPlanListInfo						//!< 3335 �������ӵ���Ϣ
	{
	public:
		SCHuaShenAttrPlanListInfo();
		MessageHeader header;

		int cur_plan_index;									// ��ǰʹ�õķ������ [0, 3)
		PlanDetail plan_list[MAX_HUASHEN_ATTR_PLAN_NUM];	// �����б� �̶�����
	};

	class CSHuaShenAttributePlanChangeName					//!< 3336 ��������
	{
	public:
		CSHuaShenAttributePlanChangeName();
		MessageHeader header;

		int plan_index;								//!< Ҫ�ĵķ������[0,3)
		char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< �·�������\see ATTRIBUTE_PLAN_NAME_LENGTH
	};

	class SCHuaShenMedalTakeOffInfo					//!< 3337 ����ѫ�����º��ѫ��λ���·�
	{
	public:
		SCHuaShenMedalTakeOffInfo();
		MessageHeader header;

		int out_index;				//�������±�
	};

	/*class CSHuaShenFabaoReq				//!< 3328 ������������
	{
	public:
		enum FABAO_REQ_TYPE
		{
			FABAO_REQ_TYPE_INFO = 0,			//!< 0 ������Ϣ��Ϣ
			FABAO_REQ_TYPE_TAKEUP = 1,			//!< 1 ���� p1 ����װ������Ӧ��λ��[0-3] p2 ����������Ӧ��λ��[0-17]
			FABAO_REQ_TYPE_TAKEOFF = 2,			//!< 2 ���� p1 ����װ������Ӧ��λ��[0-3]
		};

		CSHuaShenFabaoReq();
		MessageHeader		header;

		int req_type;
		int param1;
		int param2;
	};

	class SCHuaShenFabaoEquipBarInfo			//!< 3329  ������װ�������
	{
	public:
		SCHuaShenFabaoEquipBarInfo();
		MessageHeader		header;

		int m_fabao_equipment_bar[MAX_FABAO_EQUIPMENT_BAR];	//!< ����װ����-��Ӧװ���ķ���index
	};*/

}

#pragma pack(pop)

#endif