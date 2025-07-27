#ifndef __BATTLE_PROTOCOL_H__
#define __BATTLE_PROTOCOL_H__

#include "servercommon/serverdef.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/msgserialize.hpp"

#pragma pack(push)
#pragma pack(4)

namespace battleloginprotocol
{
	enum _MsgType
	{
		MT_INVALID,
		MT_REGISTER_TO_LOGIN,		// battle��loginע���Լ� (Ȼ��login��battle��Ϣͬ����gateway��������battle)
	
		MT_STORY_BATTLE_INFO,		// battle��login���;���ս������������
		MT_STORY_ROUND_RESULT,		// battle��login���ͻغϽ��
	};

	struct MessageHeader
	{
		MessageHeader() : msg_type(MT_INVALID), reserve_sh(0) {}
		MessageHeader(int type) : msg_type(type) {}

		short msg_type;
		short reserve_sh;
	};

	struct BL_RegisterToLogin
	{
		BL_RegisterToLogin() : header(MT_REGISTER_TO_LOGIN) {}
		MessageHeader header;

		int index;
		char ip[16];
		unsigned short port_for_gateway;
		unsigned short port_for_gameworld;
	};

	struct BL_SyncStoryBattleInfoMsg
	{
		BL_SyncStoryBattleInfoMsg() : header(MT_STORY_BATTLE_INFO) {}
		MessageHeader header;

		// ������Protocol::SCBattleInfo������
	};

	struct BL_SyncStoryRoundResultMsg
	{
		BL_SyncStoryRoundResultMsg() : header(MT_STORY_ROUND_RESULT) {}
		MessageHeader header;

		int round;

		// ������Protocol::SCBattleRoundResult������
	};
}

/*
namespace battlegateprotocol
{
	enum _MsgType
	{
		MT_GB_BEGIN = 0,
		MT_REGISTER_TO_BATTLE,		// gateway��battleע���Լ�
		MT_NET_RECV_USER_MSG,		// gateway�յ��û�����ת����battle

		MT_BG_BEGIN = 20,
		MT_NET_SEND_USER_MSG,		// battle���û����ذ���ͨ��gatewayת��
	};

	struct MessageHeader
	{
		MessageHeader() : msg_type(MT_GB_BEGIN), user_netid(0) {}
		MessageHeader(int type) : msg_type(type) {}

		short msg_type;
		unsigned short user_netid;
	};

	struct GB_RegisterToBattle
	{
		GB_RegisterToBattle() : header(MT_REGISTER_TO_BATTLE) {}
		MessageHeader header;

		int gate_index;
	};

	struct GB_NetRecvUserMsg
	{
		GB_NetRecvUserMsg() : header(MT_NET_RECV_USER_MSG) {}
		MessageHeader header;
		// �õ�ַ����Ϊ��Ϣʵ��
	};

	struct BG_NetSendUserMsg
	{
		BG_NetSendUserMsg() : header(MT_NET_SEND_USER_MSG) {}
		MessageHeader header;
		// �õ�ַ����Ϊ��Ϣʵ��
	};
}
*/

namespace battlegameprotocol
{
	// GAME_BATTLE  -> ��Ϸ������ս����
	// BATTLE_GAME  -> ս����������Ϸ��
	enum _MsgType
	{
		MT_INVALID,

		MT_BATTLE_GAME_PROTOCOL_BEGIN = MT_INVALID,

		// ��Ϸ��Ϊ���ת������ս����������
		MT_GAME_BATTLE_USER_MSG_REQ,		

		// ս�������أ� ������Ϸ��ת������ң�Ӧ����ֱ����ս�����·���ֻ�е�ս�����Ҳ�����Ӧ��gatewayʱ�Ž�����Ϸ���·���
		MT_BATTLE_GAME_USER_MSG_RESP,

		// ս�������أ� ������Ϸ��ת�������(ֱ��ͨ��UID����Role)
		MT_BATTLE_GAME_USER_MSG_RESP_BY_UID,

		// ս����ʼ���󣨵���PVE)
		MT_GAME_BATTLE_START_SINGLE_PVE_REQ,

		// ս����ʼ����ȷ��
		MT_BATTLE_GAME_START_FIGHT_ACK, 

		// ս�����֪ͨ
		MT_BATTLE_GAME_FIGHT_RESULT_NOTICE,	

		// ս��DEMO��ʼ����
		MT_GAME_BATTLE_START_DEMO_REQ,

		// ����������
		MT_GAME_BATTLE_ANIMATION_FINISH_REQ,

		// �������
		MT_BATTLE_GAME_ROLE_RUN_AWAY,

		// ǿ�ƽ���ս��
		MT_GAME_BATTLE_FORCE_FINISH_FIGHT,

		// ǿ�ƽ���ս������
		MT_BATTLE_GAME_FORCE_FINISH_FIGHT,

		// ��������ս��
		MT_GAME_BATTLE_ROLE_LOGIN, 

		// ��Ҷ���֪ͨս����
		MT_GAME_BATTLE_ROLE_LOGOUT,

		// ���ս������
		MT_GAME_BATTLE_ROLE_OPERATE,

		// ��������Զ�
		MT_BATTLE_GAME_ROLE_SET_AUTO,

		// ��������Զ���Ϊ
		MT_BATTLE_GAME_ROLE_SET_AUTO_MOVE,

		// ������뵹��ʱ����
		MT_GAME_BATTLE_ROLE_FINISH_COUNTDOWN,

		// ���ս����ʹ�û�Ѫ��Ʒ
		MT_GAME_BATTLE_ROLE_USE_HEAL_ITEM,

		// ���ս����ʹ�û�����Ʒ
		MT_GAME_BATTLE_ROLE_USE_MP_ITEM,

		// ���ս����ʹ����Ʒ��ͬ������Ϸ��������
		MT_BATTLE_GAME_ROLE_USE_ITEM,

		// ���ս����ʹ��װ��
		MT_GAME_BATTLE_ROLE_USE_EQUIPMENT,

		// ���ս���з�ӡ����
		MT_GAME_BATTLE_ROLE_SEAL_PET,

		// ���ս���з�ӡ���ͬ������Ϸ��������
		MT_BATTLE_GAME_ROLE_SEAL_PET,

		// ���ս����ʹ��˲ɱ����ĵ���
		MT_GAME_BATTLE_ROLE_USE_INST_KILL_MONSTER_ITEM,

		// ���ս����ʹ��˲ɱ����ĵ��ߣ�ͬ������Ϸ��������
		MT_BATTLE_GAME_ROLE_INST_KILL_MONSTER,

		// ��ʼ���ս������
		MT_GAME_BATTLE_START_TEAM_PVE_REQ,

		// �������ض�����
		MT_GAME_BATTLE_RELOAD_CONFIG_REQ,

		// ֪ͨ��Ϸ��Battleʵ���ѱ��ͷ�
		MT_BATTLE_GAME_BATTLE_INSTANCE_RELEASED,

		// ����BOSSѪ��֪ͨ
		MT_BATTLE_GAME_WORLD_BOSS_HP_SYNC,
		// ����BOSS2 (����boss)Ѫ��֪ͨ
		MT_BATTLE_GAME_WORLD_BOSS_2_HP_SYNC,
		// ����BOSS3 (������)Ѫ��֪ͨ
		MT_BATTLE_GAME_WORLD_BOSS_3_HP_SYNC,

		// ����PVP
		MT_GAME_BATTLE_START_SINGLE_PVP_REQ,

		// ����ɫ�Ƿ��������ս��
		MT_GAME_BATTLE_ROLE_REALLY_IN_BATTLE_REQ,

		// ���ؽ�ɫ�Ƿ������ս��
		MT_BATTLE_GAME_ROLE_REALLY_IN_BATTLE_RESP,

		// ��ʼ����PVP����
		MT_GAME_BATTLE_MULTI_PVP_REQ,

		// ��ʼ���˴�Ӱ������
		MT_GAME_BATTLE_START_FIGHT_SINGLE_SHADOW_REQ,

		// ��ʼ���˴�Ӱ������
		MT_GAME_BATTLE_START_FIGHT_MULTI_SHADOW_REQ,

		// ��ʼ��ս
		MT_GAME_BATTLE_START_OB_REQ,

		// ��ʼ��ս����ĳ��Ϊ�ӽǣ�
		MT_GAME_BATTLE_START_OB_SIDE_REQ,

		// ��������ս ��game�������Ա
		MT_BATTLE_GAME_GUILD_HONOR_FIGHT_REQUEST_BACKUP,

		// ��������ս ��battle��ǲ����Ա
		MT_GAME_BATTLE_GUILD_HONOR_FIGHT_SEND_BACKUP,

		// ��������ս ֪ͨ��Ϸ��ĳ��������ѹҵ�
		MT_BATTLE_GAME_GUILD_HONOR_ROLE_DIE_LIST,

		// ��̨ս��ʼս��
		MT_GAME_BATTLE_START_PLATFORM_FIGHT_REQ,

		// ����ս��ָ��
		MT_GAME_BATTLE_SET_STRATEGY_REQ,

		// ���ս��ָ��
		MT_GAME_BATTLE_CLEAR_STRATEGY_REQ,

		// �뿪��ս���������ɹ�ս����ͨ�Ĺ�ս��
		MT_GAME_BATTLE_LEAVE_OB_REQ,

		// ǿ�ƽ���ָ��ģʽ��ս��
		MT_GAME_BATTLE_FORCE_FINISH_BATTLE_MODE_REQ,

		// ǿ�ƽ���ָ��ģʽ��ս������
		MT_BATTLE_GAME_FORCE_FINISH_BATTLE_MODE_RESP,

		// ս����֪ͨ��Ϸ������Ҵӹ�ս��Ϊս������Ԯ�߼���
		MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE,

		// ս����֪ͨ��Ϸ������Ҵ�ս����Ϊ��ս����Ԯ�߼���
		MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE_END,

		// �����ͣս��
		MT_GAME_BATTLE_PAUSE_BATTLE_REQ,

		// ���ս���з���Ļ
		MT_GAME_BATTLE_ROLE_CHAT_REQ,

		// �����ս�������
		MT_GAME_BATTLE_ROLE_LEAVE_TEAM_AFTER_BATTLE,

		// ������ս��׼��
		MT_GAME_BATTLE_ROLE_FINISH_PREPARE,

		// ���OB״̬ͬ��
		MT_BATTLE_GAME_ROLE_SYNC_OB_STATUS,

		// ս����֪ͨ��Ϸ�� ս����ʼʧ��
		MT_BATTLE_GAME_START_FIGHT_FAILED,

		// ��Ϸ����ս������ѯս���Ƿ�ʼʧ��
		MT_GAME_BATTLE_CHECK_BATTLE_START_FAILED,

		// ս��������Ϸ���ظ�ս���Ƿ�ʼʧ��
		MT_BATTLE_GAME_CHECK_BATTLE_START_FAILED_ACK,

		// ��ʼ����ս���淨
		MT_GAME_BATTLE_START_FIGHT_SPECIAL_REQ,

		// ����ս���淨����
		MT_GAME_BATTLE_START_FIGHT_SPECIAL_OPERATE_REQ,

		// �����淨����
		MT_GAME_BATTLE_SPECIAL_RESULT_NOTICE,

		// ս������
		MT_GAME_BATTLE_BATTLE_GIVEUP,

		// ����ؽ�ս��
		MT_GAME_BATTLE_ROLE_REJOIN_BATTLE_SCENE,

		// ǿ�ƽ�����ս֪ͨ����Ϸ��֪ͨս�������һЩ��ս����Ϣ��������Ҫ�ذ�����Ϊ��Ϸ�������Ѿ������ˣ�
		MT_GAME_BATTLE_FORCE_STOP_OB_NOTICE,

		// ��ҳ���һ��
		MT_GAME_BATTLE_ROLE_UNDO_REQ,

		// ��ҵ����ۿ�ս���ٶ�
		MT_GAME_BATTLE_MODIFY_ANIMATION_SPEED_REQ,

		// ��ҵ���ս���ٶȷ���
		MT_BATTLE_GAME_MODIFY_ANIMATION_SPEED_RESP,

		// ��������ս֪ͨ��Ա��ʼս����ս�����
		MT_BATTLE_GAME_GUILD_HONOR_ROLE_STATE_CHANGE_NOTICE,

		// ��������սÿ�غ�ͬ��һ�����˺�
		MT_BATTLE_GAME_GUILD_HONOR_SYNC_ROUND_TOTAL_DAMAGE,

		// ֪ͨս������ȡ�ȸ��´���
		MT_GAME_BATTLE_HOTFIX,

		// ս����ͬ������BOSS��һغ����ݵ���Ϸ��
		MT_BATTLE_GAME_WORLD_BOSS_SYNC_ROLE_STAT,
		MT_BATTLE_GAME_WORLD_BOSS_2_SYNC_ROLE_STAT,
		MT_BATTLE_GAME_WORLD_BOSS_3_SYNC_ROLE_STAT,

		//����BOSS��Ϸ����ս��������boss Ѫ��
		MT_GAME_BATTLE_WORLD_BOSS_CHANGE_HP,
		MT_GAME_BATTLE_WORLD_BOSS_2_CHANGE_HP,
		MT_GAME_BATTLE_WORLD_BOSS_3_CHANGE_HP,

		//�����Զ���ָ��
		MT_GAME_BATTLE_SET_CUSTOM_INSTRUCTION,

		// ֻͬ��������Զ�ָ��
		MT_BATTLE_GAME_ONLY_SET_PET_AUTO_MOVE,

		// ����ս��
		MT_GAME_BATTLE_SKIP_BATTLE_REQ,

		// ��ս
		MT_GAME_BATTLE_FAST_FORWARD_REQ,

		// ���PVE
		MT_GAME_BATTLE_START_PARTNER_PVE_REQ,

		// �ͷű�������,ͬ����Ϸ�����Ļû���
		MT_BATTLE_GAME_PERFORM_CHANGE_CARD_SKILL,

		// UTAÿ�غ�ͬ����ҵ��˺������˵�����
		MT_BATTLE_GAME_UTA_SYNC_ROLE_STAT,

		// UTAÿ�غϿ�ʼͬ��OB�غϳ�����
		MT_BATTLE_GAME_UTA_SYNC_ROUND_START_DATA,

		// UTAÿ�غ�ս������ͬ������OB
		MT_BATTLE_GAME_UTA_SYNC_ROUND_RESULT_DATA,

		// ս��Ͷ��
		MT_GAME_BATTLE_BATTLE_SURRENDER,

		MT_BATTLE_GAME_MAX,
	};

	static const int BATTLE_GAME_MSG_NUM_MAX = 1024;

	struct MessageHeader
	{
		explicit MessageHeader() : msg_type(MT_INVALID) {}
		explicit MessageHeader(int _msg_type) : msg_type(_msg_type) {}

		int msg_type;
	};

	bool WriteBegin(int msg_type, char* buffer, int max_len, int* out_len);
	bool ReadBegin(int& msg_type, const char* buffer, int max_len, int* out_len);

	struct GameBattleUserMsgReq
	{
		GameBattleUserMsgReq() : header(MT_GAME_BATTLE_USER_MSG_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;

		// ������Protocol������
	};

	struct BattleGameUserMsgResp
	{
		BattleGameUserMsgResp() : header(MT_BATTLE_GAME_USER_MSG_RESP) {}
		MessageHeader header;

		BGNetID bg_netid;

		// ������Protocol������
	};

	struct BattleGameUserMsgRespByUid
	{
		BattleGameUserMsgRespByUid() : header(MT_BATTLE_GAME_USER_MSG_RESP_BY_UID) {}
		MessageHeader header;

		int uid;

		// ������Protocol������
	};

	struct BattleGameSpecialBusinessData	// ��������ҵ������  ��ҵ�Battle����Game��Э���У�ʵ������ҵ������ݴ���
	{
		short battle_mode;
		short data_type;	//!< \enum BATTLE_BUSINESS_DATA_TYPE
		int data_length;
		char data[BATTLE_BUSINESS_DATA_BUFFER_LEN];
	};

	struct GameBattleStartSinglePVEReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartSinglePVEReq() : header(MT_GAME_BATTLE_START_SINGLE_PVE_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		BGNetID bg_netid;
		int uid;

		short battle_mode;		// ս��ģʽ��ʶ 
		short animation_speed_mode; // �����ٶ�
		int monster_group_id;	// ������ID
		int battle_id;	// ����ս��ID��ʶ

		BattleData_Business business_data; // ����ԭ�����ص�ҵ������

		RoleBattleData role_battle_data;
	};

	struct GameBattleStartSinglePVPReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartSinglePVPReq() : header(MT_GAME_BATTLE_START_SINGLE_PVP_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		BGNetID attacker_bgnetid;
		int attacker_uid;

		BGNetID defender_bgnetid;
		int defender_uid;

		short battle_mode;		// ս��ģʽ��ʶ 
		short animation_speed_mode; // �����ٶ�
		int battle_id;	// ս��ID��ʶ

		BattleData_Business business_data; // ����ԭ�����ص�ҵ������

		RoleBattleData attacker_battle_data;
		RoleBattleData defender_battle_data;
	};

	struct GameBattleStartTeamPVEReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartTeamPVEReq() : header(MT_GAME_BATTLE_START_TEAM_PVE_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			RoleBattleData role_battle_data;
		};

		int team_id;
		short battle_mode;		// ս��ģʽ��ʶ 
		short animation_speed_mode; // �����ٶ�
		int monster_group_id;	// ������ID
		int battle_id;	// ����ս��ID��ʶ

		BattleData_Business business_data; // ����ԭ�����ص�ҵ������

		short leader_idx;
		short role_num;
		MemberData member_battle_data[SIDE_MAX_ROLE_NUM];

		int pos_id_list[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleStartMultiPVPReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartMultiPVPReq() : header(MT_GAME_BATTLE_MULTI_PVP_REQ){}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			RoleBattleData role_battle_data;
		};

		struct ObserverData
		{
			BGNetID bg_netid;
			short side;
			int uid;
		};

		short battle_mode;		// ս��ģʽ��ʶ 
		short animation_speed_mode; // �����ٶ�
		int battle_id;	// ս��ID��ʶ
		BattleData_Business business_data; // ����ԭ�����ص�ҵ������

		short attacker_num;
		short defender_num;
		MemberData attacker_data_list[SIDE_MAX_ROLE_NUM];
		MemberData defender_data_list[SIDE_MAX_ROLE_NUM];

		int observer_num;
		ObserverData observer[BATTLE_OBSERVER_MAX_NUM];

		int attacker_pos_id_list[SIDE_MAX_ROLE_NUM];
		int defender_pos_id_list[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleStartSingleFightShadowReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartSingleFightShadowReq() : header(MT_GAME_BATTLE_START_FIGHT_SINGLE_SHADOW_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		BGNetID attacker_bgnetid;
		int attacker_uid;

		short battle_mode;		// ս��ģʽ��ʶ 
		short animation_speed_mode; // �����ٶ�
		int battle_id;	// ����ս��ID��ʶ

		BattleData_Business business_data; // ����ԭ�����ص�ҵ������

		RoleBattleData attacker_battle_data;
		RoleBattleData defender_battle_data;
	};

	struct GameBattleStartMultiFightShadowReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartMultiFightShadowReq() : header(MT_GAME_BATTLE_START_FIGHT_MULTI_SHADOW_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct AttackerInfo
		{
			BGNetID bgnetid;
			int uid;
		};

		short battle_mode;			// ս��ģʽ��ʶ 
		short animation_speed_mode; // �����ٶ�
		int battle_id;				// ����ս��ID��ʶ

		BattleData_Business business_data; // ����ԭ�����ص�ҵ������

		short attacker_num;
		short defender_num;

		AttackerInfo attacker_list[SIDE_MAX_ROLE_NUM];
		RoleBattleData attacker_data_list[SIDE_MAX_ROLE_NUM];
		RoleBattleData defender_data_list[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleStartSpecialReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartSpecialReq() : header(MT_GAME_BATTLE_START_FIGHT_SPECIAL_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			GameName			role_name;
			short				m_avatar_type;									// ������������
			short				reserve_sh;
			long long			m_avatar_timestamp;								// ������ϴ�ͷ��
		};

		short battle_mode;		// ս��ģʽ��ʶ 
		int battle_id;			// ս��ID��ʶ

		int attacker_count;
		MemberData attacker_data[SIDE_MAX_ROLE_NUM];
		int defender_count;
		MemberData defender_data[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleStartPlatformFightReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartPlatformFightReq() : header(MT_GAME_BATTLE_START_PLATFORM_FIGHT_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		int attacker_uid;
		BGNetID bg_netid;
		int battle_id;

		int animation_speed_mode;

		int atk_pet_num;
		PetBattleData atk_pet_list[SIDE_MAX_ROLE_NUM];
		int atk_pet_pos_list[SIDE_MAX_ROLE_NUM];

		int atk_partner_num;
		PartnerBattleData atk_partner_list[SIDE_MAX_ROLE_NUM];
		int atk_partner_pos_list[SIDE_MAX_ROLE_NUM];

		int defender_uid;
		int def_pet_num;
		PetBattleData def_pet_list[SIDE_MAX_ROLE_NUM];
		int def_pet_pos_list[SIDE_MAX_ROLE_NUM];

		int def_partner_num;
		PartnerBattleData def_partner_list[SIDE_MAX_ROLE_NUM];
		int def_partner_pos_list[SIDE_MAX_ROLE_NUM];

		BattleData_Business business_data;
	};

	struct GameBattleStartPartnerPVEReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartPartnerPVEReq() : header(MT_GAME_BATTLE_START_PARTNER_PVE_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		int attacker_uid;
		BGNetID bg_netid;
		int battle_id;

		short battle_mode;		// ս��ģʽ��ʶ 
		short animation_speed_mode; // �����ٶ�
		int monster_group_id;	// ������ID

		int atk_partner_num;
		PartnerBattleData atk_partner_list[SIDE_MAX_ROLE_NUM];
		int atk_partner_pos_list[SIDE_MAX_ROLE_NUM];

		BattleData_Business business_data;
	};

	struct BattleGameStartFightAck : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameStartFightAck() : header(MT_BATTLE_GAME_START_FIGHT_ACK) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct RoleInfo
		{
			int uid;
			short grid_idx;
			short reserve_sh;
		};

		int battle_id;
		int battle_mode;
		short is_pvp;
		short is_team_up;
		short animation_speed_type;
		short reserve_sh;
		int role_count;
		RoleInfo role_list[BATTLE_MAX_ROLE_NUM];

		BattleGameSpecialBusinessData special_business_data;	// �������Э�����
	};

	struct BattleGameFightResultAck : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameFightResultAck() : header(MT_BATTLE_GAME_FIGHT_RESULT_NOTICE) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct RoleInfo
		{
			int uid;
			int level;
			char is_flyaway;
			char is_runaway;
			char is_leader;
			char pet_num;
			int pet_id_list[ROLE_BATTLE_PET_NUM];
			int pet_level_list[ROLE_BATTLE_PET_NUM];
			long long pet_unique_id_list[ROLE_BATTLE_PET_NUM];
			BattleData_StatisticRole pet_stat_list[ROLE_BATTLE_PET_NUM];
			BattleData_StatisticRole role_statistic;
			BattleData_BusinessRole role_business;
		};

		struct RoleList
		{
			int role_count;
			RoleInfo role_list[SIDE_MAX_ROLE_NUM];
		};

		int battle_id;
		int battle_mode;
		int monster_group_id;
		int battle_result;							//!< \see RESULT_TYPE

		RoleList attacker_list;
		RoleList defender_list;

		BattleData_Business business_data;			// ��������˵�ҵ�����ݽṹ���ܲ�ͬ�� ���濴���󣬿��Դ�char[]
		BattleData_Statistic statistic_data;		// ͳ��ս������

		BattleGameSpecialBusinessData special_business_data; // �������Э�����  
	};

	struct GameBattleDemoStartReq
	{
		GameBattleDemoStartReq() : header(MT_GAME_BATTLE_START_DEMO_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		int battle_id;

		int demo_seq;	// ��Ϊ0����ʹ��Demo���ã�����ʹ��NpcDemo����
		int battle_mode;
		short avatar_type;

		// ����Զ�����
		short is_auto;
		short auto_first_move_type;
		short auto_first_skill_id;
		short auto_first_skill_level;
		short auto_second_move_type;
		short auto_second_skill_id;
		short auto_second_skill_level;

		BattleData_Business business_data;
	};

	struct GameBattlePrepareFinishReq
	{
		GameBattlePrepareFinishReq() : header(MT_GAME_BATTLE_ROLE_FINISH_PREPARE){}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct GameBattleAnimationFinishReq
	{
		GameBattleAnimationFinishReq() : header(MT_GAME_BATTLE_ANIMATION_FINISH_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct GameBattleCountDownFinishReq
	{
		GameBattleCountDownFinishReq() : header(MT_GAME_BATTLE_ROLE_FINISH_COUNTDOWN) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct BattleGameRoleRunAway : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameRoleRunAway() : header(MT_BATTLE_GAME_ROLE_RUN_AWAY) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		int battle_id;
		int battle_mode;
		int uid;
		short reserve_sh;
		short is_leave_team;
		BattleData_Business business_data;	// ҵ������
		BattleData_Statistic statistic_data;
		BattleGameSpecialBusinessData special_business_data; // �������Э�����
	};

	struct GameBattleForceFinishFight
	{
		GameBattleForceFinishFight() : header(MT_GAME_BATTLE_FORCE_FINISH_FIGHT) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct BattleGameForceFinishFight : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameForceFinishFight() : header(MT_BATTLE_GAME_FORCE_FINISH_FIGHT) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct RoleInfo
		{
			int uid;
			char is_flyaway;
			char is_runaway;
			char is_leader;
			char pet_num;
			int pet_id_list[ROLE_BATTLE_PET_NUM];
			int pet_lv_list[ROLE_BATTLE_PET_NUM];
			long long pet_unique_id_list[ROLE_BATTLE_PET_NUM];
			BattleData_StatisticRole pet_stat_list[ROLE_BATTLE_PET_NUM];
			BattleData_StatisticRole role_statistic;
			BattleData_BusinessRole role_business;
		};

		struct RoleList
		{
			int role_count;
			RoleInfo role_list[SIDE_MAX_ROLE_NUM];
		};

		int battle_id;
		int battle_mode;
		int monster_group_id;
		int result_type;

		RoleList attacker_list;
		RoleList defender_list;

		BattleData_Business business_data;			// ��������˵�ҵ�����ݽṹ���ܲ�ͬ�� ���濴���󣬿��Դ�char[]
		BattleData_Statistic statistic_data;		// ͳ��ս������

		BattleGameSpecialBusinessData special_business_data; // �������Э�����
	};

	struct GameBattleRoleOperate
	{
		GameBattleRoleOperate() : header(MT_GAME_BATTLE_ROLE_OPERATE) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int operate_type;
		int param1;
		int param2;
		int param3;
		int param4;
	};

	struct BattleGameRoleSetAuto
	{
		BattleGameRoleSetAuto() : header(MT_BATTLE_GAME_ROLE_SET_AUTO) {}
		MessageHeader header;

		int role_id;
		int is_set_auto;
	};

	struct GameBattleRoleLogin
	{
		GameBattleRoleLogin() : header(MT_GAME_BATTLE_ROLE_LOGIN) {}
		MessageHeader header;

		int role_id;
		BGNetID bg_netid;
		short is_ob;
		short reserve_sh;
	};

	struct GameBattleRoleLogout
	{
		GameBattleRoleLogout() : header(MT_GAME_BATTLE_ROLE_LOGOUT) {}
		MessageHeader header;

		int role_id;
		int battle_id;
	};

	struct BattleGameRoleSetAutoMove
	{
		BattleGameRoleSetAutoMove() : header(MT_BATTLE_GAME_ROLE_SET_AUTO_MOVE) {}
		MessageHeader header;

		int role_id;

		short first_move_type;
		short first_skill_id;
		short first_skill_level;

		short second_move_type;
		short second_skill_id;
		short second_skill_level;

		int second_move_pet_id;
		long long second_move_pet_unique_id;

		short reserve_sh;
		short pet_num;
		int pet_id_list[ROLE_BATTLE_PET_NUM];
		long long pet_unique_id_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_move_type_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_skill_id_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_skill_level_list[ROLE_BATTLE_PET_NUM];
		short reserve_sh1;
		
		UNSTD_STATIC_CHECK(5 == ROLE_BATTLE_PET_NUM);
	};

	struct BattleGameOnlySyncPetAutoMove
	{
		BattleGameOnlySyncPetAutoMove() : header(MT_BATTLE_GAME_ONLY_SET_PET_AUTO_MOVE) {}
		MessageHeader header;

		int role_id;
		int pet_num;
		int pet_id_list[ROLE_BATTLE_PET_NUM];	
		long long pet_unique_id_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_move_type_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_skill_id_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_skill_level_list[ROLE_BATTLE_PET_NUM];
		short reserve_sh;

		UNSTD_STATIC_CHECK(5 == ROLE_BATTLE_PET_NUM);
	};

	struct GameBattleRoleUseHealItem
	{
		GameBattleRoleUseHealItem() : header(MT_GAME_BATTLE_ROLE_USE_HEAL_ITEM) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short target_grid_idx;
		short item_column;
		short item_index;
		ItemID item_id;
		short item_count;
		int heal_num;
		short purify_percent;
		short is_realive;
		short limit_level;
		short reply_percent;		//�ظ������������ްٷֱ�
		short is_percent;			//�Ƿ��ǰٷֱȻظ�
		short reserve_sh;
	};

	struct GameBattleRoleUseMpItem
	{
		GameBattleRoleUseMpItem() : header(MT_GAME_BATTLE_ROLE_USE_MP_ITEM) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short target_grid_idx;
		short item_column;
		short item_index;
		ItemID item_id;
		short item_count;
		short limit_level;
		short mp_value;
		short is_percent;		//�Ƿ��ǰٷֱȻظ�
		short reserve_sh;
	};

	struct GameBattleRoleUseEquipment
	{
		GameBattleRoleUseEquipment() : header(MT_GAME_BATTLE_ROLE_USE_EQUIPMENT) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short item_column;
		short item_index;
		ItemID item_id;
		
		int equip_attr_num;
		AttrInfo equip_attr_list[BATTLE_WEAPON_ATTR_NUM];

		int equip_sp_num;
		int equip_sp_list[BATTLE_WEAPON_EFFECT_NUM];
	};

	struct BattleGameRoleUseItem
	{
		BattleGameRoleUseItem() : header(MT_BATTLE_GAME_ROLE_USE_ITEM) { }
		MessageHeader header;

		int role_id;

		short item_column;
		short item_index;
		ItemID item_id;
		short reserve_sh;
	};

	struct GameBattleRoleSealPet
	{
		GameBattleRoleSealPet() : header(MT_GAME_BATTLE_ROLE_SEAL_PET) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short target_grid_idx;
		ItemID seal_card_item_id;
		short succ_rate;
		int monster_id;
		int pet_id;
		short pr_up_on_fail;
		short fixed_times_must_succ;
	};

	struct BattleGameRoleSealPet
	{
		BattleGameRoleSealPet() : header(MT_BATTLE_GAME_ROLE_SEAL_PET) {}
		MessageHeader header;

		int role_id;

		int monster_id;
		int pet_id;
		ItemID seal_card_item_id;
		char is_succ;
		char is_sure_give_pet_on_succ;
		short is_auto_seal_pet;			// �Ƿ��Զ�ץ��
		short is_bind_pet;
		int monster_group_id;
		int battle_mode;				// ս���淨
	};

	struct GameBattleRoleUseInstKillMonsterItem
	{
		GameBattleRoleUseInstKillMonsterItem() : header(MT_GAME_BATTLE_ROLE_USE_INST_KILL_MONSTER_ITEM) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short target_grid_idx;
		ItemID instant_kill_item_id;
		short succ_rate;
		int monster_id;
	};

	struct BattleGameRoleInstantKillMonster
	{
		BattleGameRoleInstantKillMonster() : header(MT_BATTLE_GAME_ROLE_INST_KILL_MONSTER) {}
		MessageHeader header;

		int role_id;
		int monster_id;
		ItemID instant_kill_item_id;
		char is_succ;
		char reserve_ch;
	};

	struct GameBattleReloadConfig
	{
		GameBattleReloadConfig() : header(MT_GAME_BATTLE_RELOAD_CONFIG_REQ) {}
		MessageHeader header;

		int config_type;
		int param1;
		int param2;
	};

	struct BattleGameInstanceReleaseNotice
	{
		BattleGameInstanceReleaseNotice() : header(MT_BATTLE_GAME_BATTLE_INSTANCE_RELEASED) {}
		MessageHeader header;

		int battle_id;
	};

	struct BattleGameSyncWorldBossHp
	{
		BattleGameSyncWorldBossHp() : header(MT_BATTLE_GAME_WORLD_BOSS_HP_SYNC) {}
		MessageHeader header;

		int cur_hp;
		int round_modify_hp;
	};

	struct BattleGameSyncWorldBoss2Hp
	{
		BattleGameSyncWorldBoss2Hp() : header(MT_BATTLE_GAME_WORLD_BOSS_2_HP_SYNC) {}
		MessageHeader header;

		int cur_hp;
		int round_modify_hp;
	};

	struct BattleGameSyncWorld3BossHp
	{
		BattleGameSyncWorld3BossHp() : header(MT_BATTLE_GAME_WORLD_BOSS_3_HP_SYNC) {}
		MessageHeader header;

		int cur_hp;
		int round_modify_hp;
	};

	struct GameBattleCheckRoleReallyInBattleReq
	{
		GameBattleCheckRoleReallyInBattleReq() : header(MT_GAME_BATTLE_ROLE_REALLY_IN_BATTLE_REQ){}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct BattleGameCheckRoleReallyInBattleResp
	{
		BattleGameCheckRoleReallyInBattleResp() : header(MT_BATTLE_GAME_ROLE_REALLY_IN_BATTLE_RESP) {}
		MessageHeader header;

		int uid;
		short is_in_battle;
		short reserve_sh;
		int battle_id;
		int battle_server_idx;
	};

	struct GameBattleStartOBReq
	{
		GameBattleStartOBReq() : header(MT_GAME_BATTLE_START_OB_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int battle_id;
		int watch_uid;
		int forbid_quit_ob_flag;	// ���ò���Ϊ1ʱ����ֹ�˳���ս
	};

	struct GameBattleStartOBSideReq
	{
		GameBattleStartOBSideReq() : header(MT_GAME_BATTLE_START_OB_SIDE_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int battle_id;
		short watch_side;		// 0����  1�ط�
		short reserve_sh;
	};

	struct BattleGameGuildHonorFightRequestBackup
	{
		BattleGameGuildHonorFightRequestBackup() : header(MT_BATTLE_GAME_GUILD_HONOR_FIGHT_REQUEST_BACKUP) {}
		MessageHeader header;

		int battle_id;				//!< ս����ʶID
		short attacker_need_num;	//!< ������Ҫ������
		short defender_need_num;	//!< �ط���Ҫ������
		BattleData_Business business_data;	//!< ҵ������
	};

	struct GameBattleGuildHonorFightSendBackup : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleGuildHonorFightSendBackup() : header(MT_GAME_BATTLE_GUILD_HONOR_FIGHT_SEND_BACKUP) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			RoleBattleData role_battle_data;
		};

		int battle_id;
		short attacker_num;
		short defender_num;
		MemberData attacker_data_list[SIDE_MAX_ROLE_NUM];
		MemberData defender_data_list[SIDE_MAX_ROLE_NUM];
	};

	struct BattleGameGuildHonorRoleDieNotice
	{
		BattleGameGuildHonorRoleDieNotice() : header(MT_BATTLE_GAME_GUILD_HONOR_ROLE_DIE_LIST) {}
		MessageHeader header;

		int battle_id;
		int attacker_queue_num;
		int defender_queue_num;
		BattleData_Business business_data;	//!< ҵ������
	};

	struct GameBattleSetStrategyReq
	{
		GameBattleSetStrategyReq() : header(MT_GAME_BATTLE_SET_STRATEGY_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		short strategy_type;
		short target_grid_idx;
	};

	struct GameBattleClearStrategyReq
	{
		GameBattleClearStrategyReq() : header(MT_GAME_BATTLE_CLEAR_STRATEGY_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		short grid_idx;
		short is_clear_all;
	};

	struct GameBattleLeaveOBReq
	{
		GameBattleLeaveOBReq() : header(MT_GAME_BATTLE_LEAVE_OB_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		int battle_id;

		int is_leave_team;
	};

	struct GameBattleForceFinishBattleModeReq
	{
		GameBattleForceFinishBattleModeReq() : header(MT_GAME_BATTLE_FORCE_FINISH_BATTLE_MODE_REQ) {}
		MessageHeader header;

		int battle_mode;
		int param1;			// ����ԭ������
		int param2;			// ����ԭ������
		int param3;			// ����ԭ������
	};

	struct BattleGameForceFinishBattleModeResp
	{
		BattleGameForceFinishBattleModeResp() : header(MT_BATTLE_GAME_FORCE_FINISH_BATTLE_MODE_RESP) {}
		MessageHeader header;

		int battle_server_idx;
		int battle_mode;
		int param1;
		int param2;
		int param3;
	};

	struct BattleGameRoleReinforceBattle
	{
		BattleGameRoleReinforceBattle() : header(MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE) {}
		MessageHeader header;

		int uid;
		int battle_id;
	};

	struct BattleGameRoleReinforceBattleEnd
	{
		BattleGameRoleReinforceBattleEnd() : header(MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE_END) {}
		MessageHeader header;

		int uid;
		int battle_id;
	};

	struct GameBattleRolePauseBattle
	{
		GameBattleRolePauseBattle() : header(MT_GAME_BATTLE_PAUSE_BATTLE_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		int is_pause;
	};

	struct GameBattleRoleRealTimeComment
	{
		GameBattleRoleRealTimeComment() : header(MT_GAME_BATTLE_ROLE_CHAT_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		unsigned int msg_length;

		// ���½�����������
		char msg_content[REAL_TIME_COMMENT_LEN];
	};

	struct GameBattleRoleLeaveTeamAfterBattle
	{
		GameBattleRoleLeaveTeamAfterBattle() : header(MT_GAME_BATTLE_ROLE_LEAVE_TEAM_AFTER_BATTLE) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct BattleGameRoleSyncOBStatus
	{
		BattleGameRoleSyncOBStatus() : header(MT_BATTLE_GAME_ROLE_SYNC_OB_STATUS) {}
		MessageHeader header;

		enum OB_STATUS
		{
			OB_STATUS_STOP_OB = 0,
			OB_STATUS_START_OB = 1,
			OB_STATUS_STOP_FIGHT_START_OB = 2,
			OB_STATUS_START_FIGHT_STOP_OB = 3,
		};

		int uid;
		int battle_id;
		int ob_status;
	};

	struct BattleGameStartFightFailed
	{
		BattleGameStartFightFailed() : header(MT_BATTLE_GAME_START_FIGHT_FAILED) {}
		MessageHeader header;

		int battle_id;
		BattleData_Business business_data;
	};

	struct GameBattleCheckBattleHasFail
	{
		GameBattleCheckBattleHasFail() : header(MT_GAME_BATTLE_CHECK_BATTLE_START_FAILED) {}
		MessageHeader header;

		int battle_id;
	};

	struct BattleGameCheckBattleHasFailAck
	{
		BattleGameCheckBattleHasFailAck() : header(MT_BATTLE_GAME_CHECK_BATTLE_START_FAILED_ACK) {}
		MessageHeader header;

		int battle_id;
		int is_failed;
	};

	struct GameBattleJoySeeksJavorsOperate
	{
		GameBattleJoySeeksJavorsOperate() : header(MT_GAME_BATTLE_START_FIGHT_SPECIAL_OPERATE_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int operate_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
	};

	struct BattleGameSpecialFightResultAck : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameSpecialFightResultAck() : header(MT_GAME_BATTLE_SPECIAL_RESULT_NOTICE) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			GameName			role_name;
			short				m_avatar_type;									// ������������
			short				reserve_sh;
			long long			m_avatar_timestamp;								// ������ϴ�ͷ��
		};

		short battle_mode;		// ս��ģʽ��ʶ 
		int battle_id;			// ս��ID��ʶ
		int battle_result;

		int attacker_num;
		int defender_num;
		MemberData attacker_data[SIDE_MAX_ROLE_NUM];
		MemberData defender_data[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleBattleGiveup
	{
		GameBattleBattleGiveup() : header(MT_GAME_BATTLE_BATTLE_GIVEUP) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct GameBattleBattleSurrender
	{
		GameBattleBattleSurrender() : header(MT_GAME_BATTLE_BATTLE_SURRENDER) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct GameBattleRoleRejoinBattleScene
	{
		GameBattleRoleRejoinBattleScene() : header(MT_GAME_BATTLE_ROLE_REJOIN_BATTLE_SCENE) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
		GameName role_name;

		int battle_id;
	};

	struct GameBattleForceStopOBNotice
	{
		GameBattleForceStopOBNotice() : header(MT_GAME_BATTLE_FORCE_STOP_OB_NOTICE) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
		int battle_id;
	};

	struct GameBattleRoleUndoReq
	{
		GameBattleRoleUndoReq() : header(MT_GAME_BATTLE_ROLE_UNDO_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct GameBattleModifyAnimationSpeedReq
	{
		GameBattleModifyAnimationSpeedReq() : header(MT_GAME_BATTLE_MODIFY_ANIMATION_SPEED_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int speed_type;
	};

	struct BattleGameModifyAnimationSpeedResp
	{
		BattleGameModifyAnimationSpeedResp() : header(MT_BATTLE_GAME_MODIFY_ANIMATION_SPEED_RESP) {}
		MessageHeader header;

		int uid;
		short speed_type;
		short is_team;
		int battle_mode;
		int teammate_uid_list[4];
	};

	struct BattleGameGuildHonorRoleStateChangeNotice
	{
		BattleGameGuildHonorRoleStateChangeNotice() : header(MT_BATTLE_GAME_GUILD_HONOR_ROLE_STATE_CHANGE_NOTICE) {}
		MessageHeader header;

		int battle_id;
		int zone_id;
		int is_attacker_side;
		short start_role_num;
		short end_role_num;
		int start_role_list[SIDE_MAX_ROLE_NUM];
		int end_role_list[SIDE_MAX_ROLE_NUM];
	};

	struct BattleGameGuildHonorSyncTotalDamage
	{
		BattleGameGuildHonorSyncTotalDamage() : header(MT_BATTLE_GAME_GUILD_HONOR_SYNC_ROUND_TOTAL_DAMAGE) {}
		MessageHeader header;

		int battle_id;
		int zone_id;
		long long atk_total_damage;
		long long def_total_damage;
	};

	struct GameBattleHotfix
	{
		GameBattleHotfix() : header(MT_GAME_BATTLE_HOTFIX) {}
		MessageHeader header;

		char so_file_name[128];
		char symbol_str[512];
	};

	struct BattleGameWorldBossSyncRoleStat
	{
		BattleGameWorldBossSyncRoleStat() : header(MT_BATTLE_GAME_WORLD_BOSS_SYNC_ROLE_STAT) {}
		MessageHeader header;
	
		struct DeltaStat
		{
			int role_id;
			long long delta_damage;
			long long delta_injure;
			long long delta_heal;
		};

		int role_count;
		DeltaStat delta_stat_list[SIDE_MAX_ROLE_NUM];
	};

	struct BattleGameWorldBoss2SyncRoleStat
	{
		BattleGameWorldBoss2SyncRoleStat() : header(MT_BATTLE_GAME_WORLD_BOSS_2_SYNC_ROLE_STAT) {}
		MessageHeader header;

		struct DeltaStat
		{
			int role_id;
			long long delta_damage;
			long long delta_injure;
			long long delta_heal;
		};

		int role_count;
		DeltaStat delta_stat_list[SIDE_MAX_ROLE_NUM];
	};

	struct BattleGameWorldBoss3SyncRoleStat
	{
		BattleGameWorldBoss3SyncRoleStat() : header(MT_BATTLE_GAME_WORLD_BOSS_3_SYNC_ROLE_STAT) {}
		MessageHeader header;

		struct DeltaStat
		{
			int role_id;
			long long delta_damage;
			long long delta_injure;
			long long delta_heal;
		};

		int role_count;
		DeltaStat delta_stat_list[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleWorldBossChangeHp
	{
		GameBattleWorldBossChangeHp() : header(MT_GAME_BATTLE_WORLD_BOSS_CHANGE_HP) {}
		MessageHeader header;

		long long cur_hp;
		long long change_value;		
	};

	struct GameBattleWorldBoss2ChangeHp
	{
		GameBattleWorldBoss2ChangeHp() : header(MT_GAME_BATTLE_WORLD_BOSS_2_CHANGE_HP) {}
		MessageHeader header;

		long long cur_hp;
		long long change_value;
	};

	struct GameBattleWorldBoss3ChangeHp
	{
		GameBattleWorldBoss3ChangeHp() : header(MT_GAME_BATTLE_WORLD_BOSS_3_CHANGE_HP) {}
		MessageHeader header;

		long long cur_hp;
		long long change_value;
	};

	struct GameBattleSetCustomInstruction
	{
		GameBattleSetCustomInstruction() : header(MT_GAME_BATTLE_SET_CUSTOM_INSTRUCTION) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		short reserve_sh;
		short grid_idx;
		InstructionStr istr;
	};

	struct GameBattleSkipBattleReq
	{
		GameBattleSkipBattleReq() : header(MT_GAME_BATTLE_SKIP_BATTLE_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct GameBattleFastForwardBattleReq
	{
		GameBattleFastForwardBattleReq() : header(MT_GAME_BATTLE_FAST_FORWARD_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct BattleGamePerformChangeCardSkill
	{
		BattleGamePerformChangeCardSkill() : header(MT_BATTLE_GAME_PERFORM_CHANGE_CARD_SKILL) {}
		MessageHeader header;

		int role_id;
		int change_card_seq;
	};

	struct BattleGameSyncUTARoleStat
	{
		BattleGameSyncUTARoleStat() : header(MT_BATTLE_GAME_UTA_SYNC_ROLE_STAT) {}
		MessageHeader header;

		struct DeltaStat
		{
			short side;
			short reserve_sh;
			int role_id;
			long long delta_damage;
			long long delta_injure;
			long long delta_heal;
		};

		long long schedule_id;
		short round;
		short role_count;
		DeltaStat delta_stat_list[BATTLE_MAX_ROLE_NUM];
	};

	struct BattleGameSyncUTARoundStartData : public IMessageDeserializer, public IMessageSerializer
	{
		BattleGameSyncUTARoundStartData() : header(MT_BATTLE_GAME_UTA_SYNC_ROUND_START_DATA) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_LL(schedule_id)
				&& WRITE_INT(length_sc_battle_info)
				&& WRITE_INT(length_sc_buff_list)
				&& WRITE_INT(length_sc_special_effect_list)
				&& WRITE_INT(length_sc_halo_list);
			if (!ret) return false;

			ret = WRITE_STRN(sc_battle_info_data, length_sc_battle_info)
				&& WRITE_STRN(sc_buff_list_data, length_sc_buff_list)
				&& WRITE_STRN(sc_special_effect_list_data, length_sc_special_effect_list)
				&& WRITE_STRN(sc_halo_list_data, length_sc_halo_list);
			if (!ret) return false;

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_LL(schedule_id)
				&& READ_INT(length_sc_battle_info)
				&& READ_INT(length_sc_buff_list)
				&& READ_INT(length_sc_special_effect_list)
				&& READ_INT(length_sc_halo_list);
			if (!ret) return false;

			ret = READ_STRN(sc_battle_info_data, length_sc_battle_info, sizeof(sc_battle_info_data))
				&& READ_STRN(sc_buff_list_data, length_sc_buff_list, sizeof(sc_buff_list_data))
				&& READ_STRN(sc_special_effect_list_data, length_sc_special_effect_list, sizeof(sc_special_effect_list_data))
				&& READ_STRN(sc_halo_list_data, length_sc_halo_list, sizeof(sc_halo_list_data));
			if (!ret) return false;

			return ret;
		}

		long long schedule_id;

		int length_sc_battle_info;
		char sc_battle_info_data[5000];

		int length_sc_buff_list;
		char sc_buff_list_data[2000];

		int length_sc_special_effect_list;
		char sc_special_effect_list_data[2000];

		int length_sc_halo_list;
		char sc_halo_list_data[2000];
	};

	struct BattleGameSyncUTARoundResultData
	{
		BattleGameSyncUTARoundResultData() : header(MT_BATTLE_GAME_UTA_SYNC_ROUND_RESULT_DATA) {}
		MessageHeader header;

		long long schedule_id;
		int data_length;
		char data_buffer[100000];
	};
}


#pragma pack(pop)


#endif