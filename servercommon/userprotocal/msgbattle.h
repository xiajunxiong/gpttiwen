#ifndef __MSG_BATTLE_H__
#define __MSG_BATTLE_H__

#include "battlemsgcode.h"
#include "msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/battle/battle_ai_def.hpp"
#include "servercommon/struct/battle/battle_event_def.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class SCBattleStart			//!< 11001 ս����ʼ֪ͨ
	{
	public:
		SCBattleStart() : header(MT_BATTLE_START_SC) {}
		MessageHeader header;

		struct PvpBuff
		{
			short buff_id;
			short buff_level;
		};

		short is_boss_fight;	//!< 1:��bossս   0:����bossս
		short is_mine_monster;	//!< 1:�ǰ���     0:���ǰ���
		int monster_group_id;	//!< ������ID
		short can_fast_forward;	//!< �ܷ���ս (1:������ս 0:������ս)
		short reserve_sh_2;		//!< ������
		short max_round;		//!< ���غ���
		short pvp_buff_num;		//!< pvp_buff����
		PvpBuff pvp_buff_list[10];	//!< pvp_buff�б� ��ʵ����������
	};

	class SCBattleResult		//!< 11002 ս�����֪ͨ
	{
	public:
		SCBattleResult() : header(MT_BATTLE_RESULT_SC) {}
		MessageHeader header;

		char is_win;			//!< [0:��  1:ʤ]
		char result_type;		//!< ������� \see RESULT_TYPE
		char battle_mode;		//!< ս��ģʽ
		char reserve_ch;		//!< ������
	};

	class CSBattleDemoStart		 //!< 11052 ��ʼս��demo
	{
	public:
		CSBattleDemoStart() : header(MT_BATTLE_DEMO_START_CS) {}
		MessageHeader header;
	};

	class CSBattleFightMonsterReq //!< 11057 �����ĳ������
	{
	public:
		CSBattleFightMonsterReq() : header(MT_BATTLE_FIGHT_MONSTER_CS) {}
		MessageHeader header;

		int monster_group_id;		//!< ������id
	};

	struct MsgCharacterBuffInfo
	{
		short buff_id;			//!< buffid
		short buff_level;		//!< buff�ȼ�
		short sustain_num;		//!< �����Ļغ������ж���
		short grid_idx;			//!< ��ɫ�ĸ��
		unsigned int buff_key;	//!< buff�ı�ʶ
	};

	struct MsgCharacterSpecialEffect
	{
		short spe_id;			//!< ��ЧID
		short grid_idx;			//!< ��ɫ�ĸ��
	};

	struct MsgCharacterHalo
	{
		short grid_id;			//!< �⻷���������ڸ��
		short halo_id;			//!< �⻷ID
		char halo_cover_grid_list[MAX_CHARACTER_NUM]; //!< �⻷���ǵĸ��
	};

	struct MsgHallowInfo
	{
		short grid_idx;
		short reserve_sh;
		int hallow_id;
		int owner_uid;
	};

	class SCBattleInfo			//!< 11003 ս����Ϣ�·�
	{
	public:
		SCBattleInfo() : header(MT_BATTLE_INFO_SC) {}
		MessageHeader header;

		static const UInt16 VERSION = 3;

		void* operator new(size_t c);
		void operator delete(void* m);

		short battle_mode;					//!< ս���淨����  \see BATTLE_MODE
		char character_num;					//!< ��ɫ����
		char can_modify_speed;				//!< �Ƿ����޸Ĺۿ�ս���ٶ� ��0����  1:��)
		char is_ob;							//!< �Ƿ��ս(0:�ǹ�ս  1:���ɹ�ս  2:һֱ����ս��
		char ob_side;						//!< ��ս���Ӿ���0������  1���ط���
		char is_pvp;						//!< �Ƿ�PVP��0����  1:��)
		char is_team_up;					//!< �Ƿ���ӣ�0����  1:��)
		unsigned int limit_timestamp;		//!< ����ʱ�䣨0:������  >0:ս��ǿ�ƽ���ʱ�����
		int monster_group_id;				//!< ������ID
		BattleCharacterInfo character_info_list[MAX_CHARACTER_NUM];  //!< ��ɫ��Ϣ�б�  ��character_num����ѭ����ȡ
	};

	class SCBattleRoundPhase				//!< 11005 ս���׶���Ϣ�·�
	{
	public:
		SCBattleRoundPhase() : header(MT_BATTLE_ROUND_PHASE_SC) {}
		MessageHeader header;

		short round;						//!< �غ���
		short round_phase;					//!< �غϽ׶� [1:�ȴ��������ָ��׶�(����ʱ3���30��ȡ��������ǲ����Զ���)  2:����������׶�  3:���Ŷ����׶�]
		unsigned int next_round_phase_time; //!< �����¸��׶ε�ʱ��� (���ڵ���ʱ)

		char is_second_move_avaliable[MAX_CHARACTER_NUM]; //!< �±���grid_idx  \see  MAX_CHARACTER_NUM
	};

	class SCBattleRoundResult				//!< 11004 ���غ�ս�������·�
	{
	public:
		SCBattleRoundResult() : header(MT_BATTLE_ROUND_RESULT_SC) {}
		MessageHeader header;

		static const UInt16 VERSION = 0;

		void* operator new(size_t c);
		void operator delete(void* m);

		int round;							//!< ��ǰ�غ�
		int tree_num;						//!< �¼�������
		int total_data_len;					//!< �����ܳ���
		char result_buffer[MAX_UINT16 + 1];	//!< ���ݻ�����
	};

	class SCBattleRoleInfo : public IMessageSerializer	//!< 11006 �����ս���е�����
	{
	public:
		SCBattleRoleInfo() : header(MT_BATTLE_ROLE_INFO_SC) {}
		virtual ~SCBattleRoleInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(role_skill_num, buffer, buffer_size, out_length);

			for (int i = 0; i < role_skill_num; ++i)
			{
				result = result && WriteShort(role_skill_list[i].skill_id, buffer, buffer_size, out_length);
				result = result && WriteShort(role_skill_list[i].skill_level, buffer, buffer_size, out_length);
				result = result && WriteShort(role_skill_list[i].skill_index, buffer, buffer_size, out_length);
				result = result && WriteShort(role_skill_list[i].cooldown_num, buffer, buffer_size, out_length);
				result = result && WriteShort(role_skill_list[i].remain_cast_times, buffer, buffer_size, out_length);
			}

			result = result && WriteShort(pet_num, buffer, buffer_size, out_length);

			for (int i = 0; i < pet_num; ++i)
			{
				result = result && WriteInt(pet_list[i].pet_id, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].pet_level, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].pet_str_level, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].pet_skin_id, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].maxhp, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].maxmp, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].curhp, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].curmp, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].element_water, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].element_fire, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].element_wind, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].element_earth, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].status, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].skill_num, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].dec_reamin_hp_shield, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].fly_flag, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].lianyao_pet_id, buffer, buffer_size, out_length);

				for (int k = 0; k < pet_list[i].skill_num; ++k)
				{
					result = result && WriteShort(pet_list[i].pet_skill_list[k].skill_id, buffer, buffer_size, out_length);
					result = result && WriteShort(pet_list[i].pet_skill_list[k].skill_level, buffer, buffer_size, out_length);
					result = result && WriteShort(pet_list[i].pet_skill_list[k].skill_index, buffer, buffer_size, out_length);
					result = result && WriteShort(pet_list[i].pet_skill_list[k].cooldown_num, buffer, buffer_size, out_length);
					result = result && WriteShort(pet_list[i].pet_skill_list[k].remain_cast_times, buffer, buffer_size, out_length);
				}
			}

			result = result && WriteChar(is_auto, buffer, buffer_size, out_length);
			result = result && WriteChar(role_side, buffer, buffer_size, out_length);
			result = result && WriteShort(first_move_type, buffer, buffer_size, out_length);
			result = result && WriteShort(first_skill_id, buffer, buffer_size, out_length);
			result = result && WriteShort(first_skill_level, buffer, buffer_size, out_length);
			result = result && WriteShort(second_move_type, buffer, buffer_size, out_length);
			result = result && WriteShort(second_skill_id, buffer, buffer_size, out_length);
			result = result && WriteShort(second_skill_level, buffer, buffer_size, out_length);
			result = result && WriteShort(animation_speed, buffer, buffer_size, out_length);

			result = result && WriteShort(fabao_cd, buffer, buffer_size, out_length);
			result = result && WriteShort(fabao_remain_times, buffer, buffer_size, out_length);
			result = result && WriteShort(fabao_num, buffer, buffer_size, out_length);

			for (int i = 0; i < fabao_num; ++i)
			{
				result = result && WriteShort(fabao_list[i].fabao_id, buffer, buffer_size, out_length);
				result = result && WriteShort(fabao_list[i].fabao_level, buffer, buffer_size, out_length);
				result = result && WriteShort(fabao_list[i].fabao_skill_type, buffer, buffer_size, out_length);
				result = result && WriteShort(fabao_list[i].fabao_grade, buffer, buffer_size, out_length);
			}

			result = result && WRITE_SHORT(used_mp_item_times);
			result = result && WRITE_SHORT(max_mp_item_times);

			result = result && WRITE_INT(fake_potion_num);
			for (int i = 0; i < fake_potion_num; ++i)
			{
				result = result && WRITE_USHORT(fake_potion_list[i].fake_potion_item_id);
				result = result && WRITE_SHORT(fake_potion_list[i].potion_num);
			}

			result = result && WRITE_SHORT(guild_buff_id_1);
			result = result && WRITE_SHORT(guild_buff_id_2);

			result = result && WRITE_CHAR(heart_skill_is_vaild);
			result = result && WRITE_CHAR(heart_skill_count);
			for (int i = 0; i < heart_skill_count && i < ARRAY_LENGTH(heart_skill_list); ++i)
			{
				result = result && WRITE_SHORT(heart_skill_list[i].skill_id);
				result = result && WRITE_SHORT(heart_skill_list[i].level);
				result = result && WRITE_SHORT(heart_skill_list[i].remain_times);
			}

			result = result && WRITE_INT(dec_reamin_hp_shield);
			result = result && WRITE_USHORT(soaring_skill_id);
			result = result && WRITE_SHORT(soaring_skill_lv);
			result = result && WRITE_INT(change_card_skill_id);
			result = result && WRITE_INT(change_card_seq);
			result = result && WRITE_USHORT(huanshou_skill_info.skill_id);
			result = result && WRITE_SHORT(huanshou_skill_info.skill_level);
			result = result && WRITE_SHORT(huanshou_skill_info.cd_perform);

			return result;
		}

		struct SkillInfo
		{
			short skill_id;			//!< ����id
			short skill_level;		//!< ���ܵȼ�
			short skill_index;		//!< �������
			short cooldown_num;		//!< ����CD
			short remain_cast_times;//!< ʣ���ͷŴ��� -1Ϊ���޴���
		};

		struct PetInfo
		{
			int pet_id;				//!< ����ID
			short pet_level;		//!< ����ȼ�
			short pet_str_level;	//!< ����ǿ���ȼ�	
			int pet_skin_id;		//!< ����Ƥ��

			Attribute maxhp;		//!< ���Ѫ��
			Attribute maxmp;		//!< ���ħ��
			Attribute curhp;		//!< ��ǰѪ��
			Attribute curmp;		//!< ��ǰħ��

			short element_water;	//!< ˮԪ��ֵ
			short element_fire;		//!< ��Ԫ��ֵ
			short element_wind;		//!< ��Ԫ��ֵ
			short element_earth;	//!< ��Ԫ��ֵ

			short status;			//!< ״̬  [0:��ս 1:����]
			short skill_num;		//!< ������
			SkillInfo pet_skill_list[MAX_BATTLE_SKILL_NUM];	//!< �����б�
			int dec_reamin_hp_shield;						//!< ʣ�໤��ֵ
			int fly_flag;			//!< �����׶�/����
			int lianyao_pet_id;		//!< ����ID
		};

		struct FakePotionInfo
		{
			ItemID fake_potion_item_id;
			short potion_num;
		};

		short role_skill_num;								//!< ��ɫ��������
		SkillInfo role_skill_list[MAX_BATTLE_SKILL_NUM];	//!< ��ɫ�����б�

		short pet_num;										//!< ��������
		PetInfo pet_list[ROLE_BATTLE_PET_NUM];				//!< �����б�
		UNSTD_STATIC_CHECK(5 == ROLE_BATTLE_PET_NUM);

		char is_auto;										//!< [0:�� 1:��]
		char role_side;										//!< [0:���� 1:�ط�]
		short first_move_type;								//!< ��һ����Ϊ������
		short first_skill_id;								//!< ��һ����Ϊ�ļ������ (��Ϊ�Ǽ���ʱ��Ч��
		short first_skill_level;							//!< ��һ����Ϊ�ļ��ܵȼ� (��Ϊ�Ǽ���ʱ��Ч��
		short second_move_type;								//!< �ڶ�����Ϊ������
		short second_skill_id;								//!< �ڶ�����Ϊ�ļ������ (��Ϊ�Ǽ���ʱ��Ч��
		short second_skill_level;							//!< �ڶ�����Ϊ�ļ��ܵȼ� (��Ϊ�Ǽ���ʱ��Ч��
		short animation_speed;								//!< �����ٶ� \see ANIMATION_SPEED

		short fabao_cd;										//!< ����ʣ��CD
		short fabao_remain_times;							//!< ����ʣ���ͷŴ���
		short fabao_num;									//!< ��������
		BattleFabao fabao_list[BATTLE_FABAO_NUM_MAX];		//!< �����б�

		short used_mp_item_times;							//!< ʹ�ù��˶��ٴ���ҩ
		short max_mp_item_times;							//!< ���ʹ����ҩ����

		int fake_potion_num;
		FakePotionInfo fake_potion_list[BATTLE_NPC_DEMO_POTION_NUM]; //!< ��ҩ�б�

		short guild_buff_id_1;								//!< ����BUFF1
		short guild_buff_id_2;								//!< ����BUFF2
		short reserve_sh1;
		char heart_skill_is_vaild;											// �ķ������Ƿ���Ч 0:ʧЧ 1:��Ч
		char heart_skill_count;
		BattleHeartSkill heart_skill_list[BATTLE_HEART_SKILL_NUM];			//!< �ķ������б�
		int dec_reamin_hp_shield;							//!< ʣ�໤��ֵ

		UInt16 soaring_skill_id;							//!< ��������ID
		short soaring_skill_lv;								//!< �������ܵȼ�

		int change_card_skill_id;							//!< ��������ID
		int change_card_seq;								//!< ����seq
		BattleHuanShouSkillInfo huanshou_skill_info;		//!< ���޼���
	};

	class CSAnimationFinish
	{
	public:
		CSAnimationFinish() : header(MT_BATTLE_ANIMATION_FINISH_CS) {}
		MessageHeader header;
	};

	class CSBattleCountDownFinish				//!< 11056 
	{
	public:
		CSBattleCountDownFinish() : header(MT_BATTLE_COUNTDOWN_FINISH_CS) {}
		MessageHeader header;
	};

	class CSBattleRoleOperate			//!< 11054 ս����������
	{
	public:
		CSBattleRoleOperate() : header(MT_BATTLE_ROLE_OPERATE_CS) {}
		MessageHeader header;

		int operate_type;				//!< ������������ \see BATTLE_ROLE_OPERATE_TYPE_SET_AUTO
		int param1;						//!< ����1
		int param2;						//!< ����2
		int param3;						//!< ����3
		int param4;						//!< ����4
	};

	class CSBattleForceFinishFight
	{
	public:
		CSBattleForceFinishFight() : header(MT_BATTLE_FORCE_FINISH_FIGHT_CS) {}
		MessageHeader header;
	};

	class SCBattleSetAutoResp		//!< 11007 �����Զ��ķ���
	{
	public:
		SCBattleSetAutoResp() : header(MT_BATTLE_SET_AUTO_RESP_SC) {}
		MessageHeader header;

		int is_auto;				//!< [0:�� 1:��]
	};

	class SCBattleForceFinishFight	//!< 11008 ǿ�ƽ���ս���ķ��ػ�֪ͨ
	{
	public:
		SCBattleForceFinishFight() : header(MT_BATTLE_FORCE_FINISH_FIGHT_SC) {}
		MessageHeader header;
	};

	class SCBattleSetAutoMoveResp	//!< 11009 �����Զ���Ϊ�Ļذ�
	{
	public:
		SCBattleSetAutoMoveResp() : header(MT_BATTLE_SET_AUTO_MOVE_SC), notify_reason(REASON_TYPE_NORMAL){}
		MessageHeader header;

		enum REASON_TYPE
		{
			REASON_TYPE_NORMAL = 0,
			REASON_TYPE_EMPTY_MP_RESET_MOVE = 1, // ���������Զ���Ϊ
			REASON_TYPE_AI_SKILL = 2,			 // ����ʩ��	
		};

		short first_move_type;		//!< ��һ����Ϊ������
		short first_skill_id;		//!< ��һ����Ϊ�ļ���ID (��Ϊ�Ǽ���ʱ��Ч��
		short first_skill_level;	//!< ��һ����Ϊ�ļ��ܵȼ� (��Ϊ�Ǽ���ʱ��Ч��
		short second_move_type;		//!< �ڶ�����Ϊ������
		short second_skill_id;		//!< �ڶ�����Ϊ�ļ���ID (��Ϊ�Ǽ���ʱ��Ч��
		short second_skill_level;	//!< �ڶ�����Ϊ�ļ��ܵȼ� (��Ϊ�Ǽ���ʱ��Ч��
		int notify_reason;			//!< �·�ԭ�� \see REASON_TYPE
	};

	class CSBattleModeFightReq		//!< 11058  ��ս��ģʽ��ʼս�� 
	{
	public:
		CSBattleModeFightReq() : header(MT_BATTLE_MODE_FIGHT_CS) {}
		MessageHeader header;

		short battle_mode;			//!< ս��ģʽ \see  BATTLE_MODE
		short reserve_sh;
		int param1;
		int param2;
		int param3;
	};

	class CSChallengeOthersReq		//!< 11059 ǿ�� PK �������(ǿP������)
	{
	public:
		CSChallengeOthersReq() : header(MT_BATTLE_FIGHT_OTHERS_CS) {}
		MessageHeader header;

		int battle_mode;		//!< ս��ģʽ \see  BATTLE_MODE
		int role_id;			//!< ��ɫID
	};

	class CSBattleQieCuoWithOthers		//!< 11060 ��������������д�
	{
	public:
		CSBattleQieCuoWithOthers() : header(MT_BATTLE_QIE_CUO_REQ_CS) {}
		MessageHeader header;

		int role_id;		//!< Ŀ���ɫID
	};

	class SCOthersReqQieCuoWithYou		//!< 11061 ����������������д�֪ͨ
	{
	public:
		SCOthersReqQieCuoWithYou() : header(MT_BATTLE_QIE_CUO_REQ_SC) {}
		MessageHeader header;

		int role_id;				//!< ������id
		GameName sponsor_name;		//!< ����������
	};

	class CSBattleQieCuoMessageRep		//!< 11062 �д�����ظ�
	{
	public:
		CSBattleQieCuoMessageRep() : header(MT_BATTLE_QIE_CUO_REP_CS) {}
		MessageHeader header;

		int reply_role_id;		//!< �ظ������id
		int rep_result;			//!< �ظ��Ľ���� Ϊ0 - �ܾ�����0 - ͬ��
	};

	class SCBattleDemonCaveWave		//!< 11010 ħ�ߵ�ǰ������Ϣ
	{
	public:
		SCBattleDemonCaveWave() : header(MT_BATTLE_DEMON_CAVE_WAVE_SC) {}
		MessageHeader header;

		int cur_wave;				//!< ��ǰ����
	};

	enum ROLE_OPER_REASON
	{
		ROLE_OPER_REASON_SUCC = 0,						//!< 0 �ɹ�
		ROLE_OPER_REASON_BATTLE_SERVER_NOT_CONNECT = 1, //!< 1 ս����δ����
		ROLE_OPER_REASON_NOT_IN_BATTLE = 2,				//!< 2 ��ɫ����ս����
		ROLE_OPER_REASON_INPUT_PARAM_ERROR = 3,			//!< 3 �����������
		ROLE_OPER_REASON_KNAPSACK_LOAD_ERROR = 4,		//!< 4 ��ȡ��Ʒ����
		ROLE_OPER_REASON_ITEM_USE_ERROR = 5,			//!< 5 ʹ����Ʒʧ��
		ROLE_OPER_REASON_UNKNOWN_ERROR = 6,				//!< 6 δ֪����
		ROLE_OPER_REASON_MOVE_INVALID = 7,				//!< 7 �ж����Ϸ�
	};

	class SCBattleRoleOperate		//!< 11011 ��Ҳ������ؽ��
	{
	public:
		SCBattleRoleOperate() : header(MT_BATTLE_ROLE_OPERATE_SC) {}
		MessageHeader header;

		short is_succ;				//!< �Ƿ�����ɹ�
		short reserve_sh;			//!< ������
	};

	class SCBattleRoleOBSwitch		//!< 11012 ս���� ��սģʽ��ս��ģʽ���л�
	{
	public:
		SCBattleRoleOBSwitch() : header(MT_BATTLE_ROLE_OB_SWITCH_SC) {}
		MessageHeader header;

		short is_ob;				//!< �Ƿ��սģʽ��0:ս��ģʽ 1:���ɹ�սģʽ  2:һֱ��սģʽ��
		short reserve_sh;			//!< ������
	};

	class CSWatchBattleReq		 //!< 11200 ��ʼ��ս
	{
	public:
		CSWatchBattleReq() : header(MT_BATTLE_WATCH_BATTLE_CS) {}
		MessageHeader header;

		short watch_side;	//�ۿ� 0����ߣ�1���ұ�
		short id_type;		//0 uid ��1��battle-id
		int uid;			//<! �ۿ��Ľ�ɫid
	};

	class CSBattleSetStrategy	//!< 11071 ����ս��ָ��
	{
	public:
		CSBattleSetStrategy() : header(MT_BATTLE_SET_STRATEGY_CS) {}
		MessageHeader header;

		short strategy_type;	//!< \see  BATTLE_STRATEGY
		short target_grid_idx;	//!< ���[0, 20)
	};

	class CSBattleClearStrategy	//!< 11072 ���ս��ָ��
	{
	public:
		CSBattleClearStrategy() : header(MT_BATTLE_CLEAR_STRATEGY_CS) {}
		MessageHeader header;

		short grid_idx;			//!< ��� [0,20)
		short is_clear_all;		//!< �Ƿ����ȫ�� [0:�� 1:��]
	};

	class SCBattleStrategyInfo : public IMessageSerializer	//!< 11073 ս��ָ����Ϣ
	{
	public:
		SCBattleStrategyInfo() : header(MT_BATTLE_STRATEGY_INFO_SC) {}
		MessageHeader header;

		enum REASON_TYPE
		{
			REASON_TYPE_NORMAL = 0,		//!< 0 �б�
			REASON_TYPE_ADD = 1,		//!< 1 ��һ��
			REASON_TYPE_REMOVE = 2,		//!< 2 ȥ��һ��
		};

		struct StrategyInfo
		{
			short strategy_type;		//!< \see  BATTLE_STRATEGY
			short target_grid_idx;		//!< ���[0, 20)
			GameName role_name;
			InstructionStr istr;
		};

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = true;
			ret = ret && WRITE_BEGIN(header.msg_type);

			ret = ret && WRITE_SHORT(count);
			ret = ret && WRITE_SHORT(reason);
			for (int i = 0; i < count; ++i)
			{
				ret = ret && WRITE_SHORT(info_list[i].strategy_type);
				ret = ret && WRITE_SHORT(info_list[i].target_grid_idx);
				ret = ret && WRITE_STRN(info_list[i].role_name, sizeof(GameName));
				if (0 == info_list[i].strategy_type)
				{
					ret = ret && WRITE_STRN(info_list[i].istr, sizeof(InstructionStr));
				}
			}
			
			return ret;
		}

		short count;					//!< ����
		short reason;					//!< ԭ�� \see REASON_TYPE
		StrategyInfo info_list[MAX_CHARACTER_NUM];	//!< ս���б�
	};

	class SCBattleSyncWorldBossCurHp
	{
	public:
		SCBattleSyncWorldBossCurHp() : header(MT_SYNC_WORLD_BOSS_CUR_HP_SC) {}
		MessageHeader header;

		int cur_hp;
		int monster_id;
	};

	class SCBattleRoleFinishInputNotice	// 11075 ����������֪ͨ
	{
	public:
		SCBattleRoleFinishInputNotice() : header(MT_BATTLE_ROLE_FINISH_INPUT_NOTICE_SC) {}
		MessageHeader header;

		int role_id;
		short grid_idx;
		short reserve_sh;
	};

	class SCBattleRoleStartInput		// 11076 ���ǿ�ʼ����֪ͨ
	{
	public:
		SCBattleRoleStartInput() : header(MT_BATTLE_ROLE_START_INPUT_SC) {}
		MessageHeader header;

		short is_first_input_finish;	// һ����������Ƿ����
		short is_second_input_finish;	// ����/����һ����������Ƿ����
	};

	class CSBattleLeaveOB	// 11077 �뿪��ս��������һ���ͨ��ս��ң�
	{
	public:
		CSBattleLeaveOB() : header(MT_BATTLE_LEAVE_OB_CS) {}
		MessageHeader header;

		int is_leave_team;				// [0:���� 1:�˶�]
	};

	class SCBattleEndForAudience	// 11078 ����ս������
	{
	public:
		SCBattleEndForAudience() : header(MT_BATTLE_END_FOR_AUDIENCE_SC) {}
		MessageHeader header;
	};

	class CSBattlePause			//!< 11079 ������ͣ/ȡ����ͣս��
	{
	public:
		CSBattlePause() : header(MT_BATTLE_PAUSE_CS) {}
		MessageHeader header;

		int is_pause;			//!< 0:ȡ����ͣ  1:��ͣ
	};

	class SCBattlePause			//!< 11080 ��ͣ�������
	{
	public:
		SCBattlePause() : header(MT_BATTLE_PAUSE_SC) {}
		MessageHeader header;

		int is_succ;			//!< 0:ʧ��  1:�ɹ�
	};

	class CSBattleChat			//!< 11201 ���͵�Ļ
	{
	public:
		CSBattleChat() : header(MT_BATTLE_CHAT_CS) {}
		MessageHeader header;

		unsigned int msg_length;
		// ������������

		char msg_content[REAL_TIME_COMMENT_LEN];
	};

	class SCBattleChat			//!< 11202 ���͵�Ļ�ذ�
	{
	public:
		SCBattleChat() : header(MT_BATTLE_CHAT_SC) {}
		MessageHeader header;

		GameName sender_name;	//!< ����������
		int sender_uid;			//!< ������uid

		unsigned int msg_length;
		// ������������

		char msg_content[REAL_TIME_COMMENT_LEN];
	};

	class CSBattlePrepareFinish	 //!< 11081 ս��׼�����
	{
	public:
		CSBattlePrepareFinish() : header(MT_BATTLE_PREPARE_FINISH_CS) {}
		MessageHeader header;
	};

	class SCBattleSpecialInfo	 //!< 11013 ����ս���·�
	{
	public:
		SCBattleSpecialInfo() : header(MT_BATTLE_SPECIAL_START_SC) {}
		MessageHeader header;

		long long ready_tamp;				//!< ���ý���ʱ���

		JoySeeksFavorsMap myself_map;		//!< �Լ��ĵ�ͼ��Ϣ
		JoySeeksFavorsMap enemy_map;		//!< ���ֵĵ�ͼ��Ϣ
	};

	class SCBattleSpecialOneMapChangeInfo	 //!< 11014 ����ͼ�ı�
	{
	public:
		SCBattleSpecialOneMapChangeInfo() : header(MT_BATTLE_SPECIAL_ONE_MAP_CHANGE_SC) {}
		MessageHeader header;

		long long show_tamp;				//!< չʾʱ���
		long long next_move_tamp;			//!< ���غϽ���ʱ���

		char has_find;						//!< [0-2] 0:�ޣ�1�ҵ��ˣ�2���ҵ���
		char reserve_ch;
		short joy_seq;						//!< [0-4] 0:�ޣ�1С����2�е���3�� 
		JoySeeksFavorsMap map_info;
	};

	class SCBattleSpecialFinishInfo	 //!< 11015 ����Ѱ��ս�����
	{
	public:
		SCBattleSpecialFinishInfo() : header(MT_BATTLE_SPECIAL_FINISH_SC) {}
		MessageHeader header;

		int winer_id;
	};

	class CSBattleSpecialJoyReqInfo	 //!< 11016 ����Ѱ���������
	{
	public:
		CSBattleSpecialJoyReqInfo() : header(MT_BATTLE_SPECIAL_JOY_REQ_CS) {}
		MessageHeader header;

		int operate_type;				//!< ������������ \see BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_OPEN
		int param1;						//!< ����1
		int param2;						//!< ����2
		int param3;						//!< ����3
		int param4;						//!< ����4
		int param5;						//!< ����5
	};

	class CSBattleGiveup	//!< 11082 ����ս��
	{
	public:
		CSBattleGiveup() : header(MT_BATTLE_GIVEUP_CS) {}
		MessageHeader header;
	};

	class CSBattleSurrender	//!< 11102 Ͷ��
	{
	public:
		CSBattleSurrender() : header(MT_BATTLE_SURRENDER_CS) {}
		MessageHeader header;
	};

	class CSBattleReportSimpleInfo	//!< 11083 ����ս��������Ϣ
	{
	public:
		CSBattleReportSimpleInfo() : header(MT_BATTLE_REPORT_SIMPLE_INFO_CS) {}
		MessageHeader header;

		int monster_group_id;		//!< ������ID
		int param1;					//!< ԭ������ 
		int param2;					//!< ԭ������
		int param3;					//!< ԭ������
		int is_server_first_kill;	//!< �Ƿ���ɱ
	};

	class SCBattleReportSimpleInfo	//!< 11084 ����ս��������Ϣ
	{
	public:
		SCBattleReportSimpleInfo() : header(MT_BATTLE_REPORT_SIMPLE_INFO_SC) {}
		MessageHeader header;

		int monster_group_id;		//!< ������ID
		int param1;					//!< ԭ������ 
		int param2;					//!< ԭ������
		int param3;					//!< ԭ������
		long long report_file_id;	//!< ս��ID   0����û��
		int is_server_first_kill;	//!< �Ƿ���ɱ
	};

	class CSBattleReportDetailInfo //!< 11085 �ۿ�¼������
	{
	public:
		CSBattleReportDetailInfo() : header(MT_BATTLE_REPORT_DETAIL_INFO_CS) {}
		MessageHeader header;

		int monster_group_id;
		int is_server_first_kill;
	};

	class CSBattleReportParseFailed	//!< 11097 ����ս��¼��ʧ���ϱ�
	{
	public:
		CSBattleReportParseFailed() : header(MT_BATTLE_REPORT_PARSE_FAILED){}
		MessageHeader header;

		int monster_group_id;
		int is_server_first_kill;
	};

	class CSBattleReportStopWatching //!< 11086 ��ͣ�ۿ�ս�����ӳ�������or����ʱ������
	{
	public:
		CSBattleReportStopWatching() : header(MT_BATTLE_REPORT_STOP_WATCHING_CS) {}
		MessageHeader header;
	};

	class SCBattleReportStopWatching //!< 11087 ��ͣ�ۿ�ս������ ���ӳ��Ͷ�Ա�����յ���
	{
	public:
		SCBattleReportStopWatching() : header(MT_BATTLE_REPORT_STOP_WATCHING_SC) {}
		MessageHeader header;
	};

	class SCBattleAutoInfoList	// 11088 �Զ���Ϣ�б�
	{
	public:
		SCBattleAutoInfoList() : header(MT_BATTLE_AUTO_INFO_LIST_SC) {}
		MessageHeader header;

		struct AutoInfo
		{
			int role_id;		// ��ɫID
			short is_auto;		// 0:�ֶ� 1:�Զ�
			short reserve_sh;	// ������
		};

		int num;
		AutoInfo info_list[SIDE_MAX_ROLE_NUM];
	};

	class SCBattleAutoInfoChangeNotice	// 11089 �Զ���Ϣ�ı�֪ͨ
	{
	public:
		SCBattleAutoInfoChangeNotice() : header(MT_BATTLE_AUTO_INFO_CHANGE_NOTICE_SC) {}
		MessageHeader header;

		int role_id;	// ��ɫID
		int is_auto;	// 0:�ֶ� 1:�Զ�
	};

	class CSBattleStartNpcDemoFight	// 11090 ��ʼ������������ս��
	{
	public:
		CSBattleStartNpcDemoFight() : header(MT_BATTLE_START_NPC_DEMO_FIGHT_CS) {}
		MessageHeader header;

		int npc_demo_seq;	
		int is_initial_fight;		//!< �Ƿ��ʼս��
	};

	class SCBattleRoleInBattleNotice // 11091 �յ���֪ͨ������11092
	{
	public:
		SCBattleRoleInBattleNotice() : header(MT_BATTLE_ROLE_IN_BATTLE_NOTICE_SC) {}
		MessageHeader header;

		int battle_server_idx;		//!< ս������ţ�����11092����
		int battle_id;				//!< ս��ʵ��ID������11092����
	};

	class CSRoleRejoinBattleScene	// 11092 �ͻ��������ؽ�ս��
	{
	public:
		CSRoleRejoinBattleScene() : header(MT_BATTLE_ROLE_REJOIN_BATTLE_SCENE_CS) {}
		MessageHeader header;

		int battle_server_idx;		//!< ս�������
		int battle_id;				//!< ս��ʵ��ID
	};

	class SCBattleStatisticResult : public IMessageSerializer // 11093 ���ս��ͳ������
	{
	public:
		SCBattleStatisticResult() : header(MT_BATTLE_STATISTIC_RESULT_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = true;
			ret = ret && WRITE_BEGIN(header.msg_type);

			ret = ret && WRITE_INT(role_num);
			for (int i = 0; i < role_num; ++i)
			{
				ret = ret && WRITE_SHORT(role_stat_list[i].avatar_type);
				ret = ret && WRITE_SHORT(role_stat_list[i].head_id);
				ret = ret && WRITE_STRN(role_stat_list[i].role_name, sizeof(GameName));
				ret = ret && WRITE_SHORT(role_stat_list[i].reserve_sh);
				ret = ret && WRITE_SHORT(role_stat_list[i].special_appearance_type);
				ret = ret && WRITE_INT(role_stat_list[i].special_appearance_param);
				ret = ret && WRITE_LL(role_stat_list[i].total_damage_to_other);
				ret = ret && WRITE_LL(role_stat_list[i].total_suffer_injure);
				ret = ret && WRITE_LL(role_stat_list[i].total_heal_num);
				ret = ret && WRITE_INT(role_stat_list[i].fly_flag);
			}
			if (!ret) return false;

			ret = ret && WRITE_INT(partner_num);
			for (int i = 0; i < partner_num; ++i)
			{
				ret = ret && WRITE_INT(partner_stat_list[i].partner_id);
				ret = ret && WRITE_INT(partner_stat_list[i].skin_id);
				ret = ret && WRITE_LL(partner_stat_list[i].total_damage_to_other);
				ret = ret && WRITE_LL(partner_stat_list[i].total_suffer_injure);
				ret = ret && WRITE_LL(partner_stat_list[i].total_heal_num);
				ret = ret && WRITE_INT(partner_stat_list[i].fly_flag);
			}
			if (!ret) return false;

			ret = ret && WRITE_INT(pet_num);
			for (int i = 0; i < pet_num; ++i)
			{
				ret = ret && WRITE_INT(pet_stat_list[i].pet_id);
				ret = ret && WRITE_INT(pet_stat_list[i].skin_id);
				ret = ret && WRITE_SHORT(pet_stat_list[i].level);
				ret = ret && WRITE_CHAR(pet_stat_list[i].strength_level);
				ret = ret && WRITE_CHAR(pet_stat_list[i].role_stat_list_idx);
				ret = ret && WRITE_STRN(pet_stat_list[i].pet_name, sizeof(GameName));
				ret = ret && WRITE_LL(pet_stat_list[i].total_damage_to_other);
				ret = ret && WRITE_LL(pet_stat_list[i].total_suffer_injure);
				ret = ret && WRITE_LL(pet_stat_list[i].total_heal_num);
				ret = ret && WRITE_INT(pet_stat_list[i].fly_flag);
			}

			return true;
		}

		struct RoleStatistic
		{
			short avatar_type;		//!< ģ��
			short head_id;			//!< ͷ��id
			GameName role_name;		//!< �������

			short reserve_sh;		//!< ������
			short special_appearance_type;	//!< ������������
			int special_appearance_param;	//!< �����������

			long long total_damage_to_other; //!< ���˺�
			long long total_suffer_injure;	 //!< �ܳ���
			long long total_heal_num;		 //!< ������
			int fly_flag;					 //!< �������
		};

		struct PartnerStatistic
		{
			int partner_id;
			int skin_id;					 //!< Ƥ��ID
			long long total_damage_to_other; //!< ���˺�
			long long total_suffer_injure;	 //!< �ܳ���
			long long total_heal_num;		 //!< ������
			int fly_flag;					 //!< �������
		};

		struct PetStatistic
		{
			int pet_id;							//!< ����ID
			int skin_id;						//!< Ƥ��ID
			short level;						//!< ����ȼ�
			char strength_level;				//!< ǿ���ȼ�
			char role_stat_list_idx;			//!< �����Ӧ�����role_stat_list���±�
			GameName pet_name;					//!< ������
			long long total_damage_to_other;	//!< ���˺�
			long long total_suffer_injure;		//!< �ܳ���
			long long total_heal_num;			//!< ������
			int fly_flag;						//!< �������
		};

		int role_num;
		RoleStatistic role_stat_list[SIDE_MAX_ROLE_NUM];

		int partner_num;
		PartnerStatistic partner_stat_list[SIDE_MAX_ROLE_NUM];

		int pet_num;
		PetStatistic pet_stat_list[SIDE_MAX_ROLE_NUM * ROLE_BATTLE_PET_NUM]; //!< ����pet_num������pet_stat_list����
	};

	class CSRoleUndo		//!< 11203 ����һ������
	{
	public:
		CSRoleUndo() : header(MT_BATTLE_UNDO_CS) {}
		MessageHeader header;
	};

	class SCBattleChiefElectFieldBuffShow	// 11094 ��ϯ��ѡս��BUFF��ʾ
	{
	public:
		SCBattleChiefElectFieldBuffShow() : header(MT_BATTLE_CHIEF_ELECT_FIELD_BUFF_SHOW) {}
		MessageHeader header;

		short buff_id;
		short buff_value;
	};

	class CSBattleAnimationSpeedModify	// 11204 ս�����޸ı�������
	{
	public:
		CSBattleAnimationSpeedModify() : header(MT_BATTLE_ANIMATION_SPEED_MODIFY_CS) {}
		MessageHeader header;

		int speed_type;	// Ҫ�ĵ��ٶ�  [0:һ���� 1:������ 2:�뱶��]
	};

	class SCBattleAnimationSpeedModify	// 11205 ս�����޸ı��ٷ���
	{
	public:
		SCBattleAnimationSpeedModify() : header(MT_BATTLE_ANIMATION_SPEED_MODIFY_SC) {}
		MessageHeader header;

		int speed_type;	// �ĺ���ٶ� [0:һ���� 1:������ 2:�뱶��]
	};

	class SCBattleAISkillResult		// 11095 ����ʩ�����
	{
	public:
		SCBattleAISkillResult() : header(MT_BATTLE_AI_SKILL_RESULT) {}
		MessageHeader header;

		short move_type;			// ��Ϊ���� ��Ŀǰ��1:�ͷż��� �� 2:�չ���
		UInt16 skill_id;			// ����ID
		short skill_level;			// ���ܵȼ�
		short reserve_sh;			// ������
	};

	class CSBattleSetCustomInstruction	//!< 11096 �����Զ���ָ��
	{
	public:
		CSBattleSetCustomInstruction() : header(MT_BATTLE_SET_CUSTOM_INSTRUCTION_CS) {}
		MessageHeader header;

		short index;					//!< �Զ���ָ�����[0,8) 
		short grid_idx;					//!< ����[0,20)
	};

	class CSBattleSkipReq	//!< 11098 ����ս��
	{
	public:
		CSBattleSkipReq() : header(MT_BATTLE_SKIP_CS) {}
		MessageHeader header;
	};

	class CSBattleFastForwardReq	//!< 11099 ��ս
	{
	public:
		CSBattleFastForwardReq() : header(MT_BATTLE_FAST_FORWARD_CS) {}
		MessageHeader header;
	};

	class SCBattleFastForwardResp  //!< 11100 ��ս����·�
	{
	public:
		SCBattleFastForwardResp() : header(MT_BATTLE_FAST_FORWARD_SC) {}
		MessageHeader header;

		int result;	// 0:ʧ��  1:�ɹ�
	};

	class SCBattleGuildBossTotalDamage	// 11101 ��ս���·��Լ���BOSS���˺�
	{
	public:
		SCBattleGuildBossTotalDamage() : header(MT_BATTLE_GUILD_BOSS_DAMAGE_SC) {}
		MessageHeader header;

		long long total_damage;
	};

	class SCBattleMoveBoardcast	// 11206 ս��ָ��㲥
	{
	public:
		SCBattleMoveBoardcast() : header(MT_BATTLE_MOVE_BOARDCAST_SC) {}
		MessageHeader header;

		short grid_idx;			// �ĸ�����ϵĽ�ɫ����Ϊ
		short target_grid_idx;	// Ŀ��ĸ��
		short move_type;		// ��Ϊ����
		short param1;			// ����Ϊ�Ƿż���ʱ��Ϊ����ID������Ϊ�Ƿ���ʱ��Ϊ�������[0,fabao_num)
		short param2;			// ����Ϊ�Ƿż���ʱ��Ϊ���ܵȼ�  
		short param3;
	};
}

#pragma pack(pop)


namespace Protocol
{
	class SCBattleCharacterBuffList : public IMessageSerializer	//!< 11017 ��ɫbuff�б�
	{
	public:
		SCBattleCharacterBuffList() : header(MT_BATTLE_CHARACTER_BUFF_LIST_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT((int)character_buff_list.size());
			for (size_t i = 0; i < character_buff_list.size(); ++i)
			{
				MsgCharacterBuffInfo& info = character_buff_list[i];
				result = result && WRITE_SHORT(info.buff_id);
				result = result && WRITE_SHORT(info.buff_level);
				result = result && WRITE_SHORT(info.sustain_num);
				result = result && WRITE_SHORT(info.grid_idx);
				result = result && WRITE_INT(info.buff_key);
			}

			return result;
		}

		std::vector<MsgCharacterBuffInfo> character_buff_list;

		//�൱��һ��������һ�����飬���£�
		//int count;
		//CharacterBuffInfo buff_list[];
	};

	class SCBattleCharacterSpecialEffectList : public IMessageSerializer	//!< 11019 ��ɫ��Ч�б�
	{
	public:
		SCBattleCharacterSpecialEffectList() : header(MT_BATTLE_CHARACTER_SPECIAL_EFFECT_LIST_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT((int)spe_list.size());
			for (size_t i = 0; i < spe_list.size(); ++i)
			{
				MsgCharacterSpecialEffect& info = spe_list[i];
				result = result && WRITE_SHORT(info.spe_id);
				result = result && WRITE_SHORT(info.grid_idx);
			}

			return result;
		}

		std::vector<MsgCharacterSpecialEffect> spe_list;
	};

	class SCBattleCharacterHaloList : public IMessageSerializer	//!< 11020 ��ɫ�⻷�б�
	{
	public:
		SCBattleCharacterHaloList() : header(MT_BATTLE_CHARACTER_HALO_LIST_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT((int)halo_list.size());
			for (size_t i = 0; i < halo_list.size(); ++i)
			{
				MsgCharacterHalo& info = halo_list[i];
				result = result && WRITE_SHORT(info.grid_id);
				result = result && WRITE_SHORT(info.halo_id);
				for (int k = 0; k < ARRAY_ITEM_COUNT(info.halo_cover_grid_list); ++k)
				{
					result = result && WRITE_CHAR(info.halo_cover_grid_list[k]);
				}
			}

			return result;
		}

		std::vector<MsgCharacterHalo> halo_list;
	};

	class SCBattleHallowList : public IMessageSerializer, public IMessageDeserializer	//!< 11021 ʥ���б�
	{
	public:
		SCBattleHallowList() : header(MT_BATTLE_HALLOW_LIST_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT((int)hallow_list.size()); // ����
			for (size_t i = 0; i < hallow_list.size(); ++i)
			{
				MsgHallowInfo& info = hallow_list[i];
				result = result && WRITE_SHORT(info.grid_idx); // ʥ�����[20,30)
				result = result && WRITE_INT(info.hallow_id);  // ʥ��ID
				result = result && WRITE_INT(info.owner_uid);  // ӵ����uid
			}

			return result;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool result = true;
			int count = 0;
			result = result && READ_BEGIN(header.msg_type);
			result = result && READ_INT(count); // ����
			if (!result) return false;

			for (int i = 0; i < count; ++i)
			{
				MsgHallowInfo info;
				result = result && READ_SHORT(info.grid_idx); // ʥ�����[20,30)
				result = result && READ_INT(info.hallow_id);  // ʥ��ID
				result = result && READ_INT(info.owner_uid);  // ӵ����uid
				hallow_list.push_back(info);
				if (!result) return false;
			}

			return result;
		}

		std::vector<MsgHallowInfo> hallow_list;
	};

	class SCBattleReport : public IMessageSerializer		//!< 11018 ս��
	{
	public:
		SCBattleReport() : header(MT_BATTLE_REPORT_SC), battle_info(NULL) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			// �������SCBattleInfo��SCBattleRoundResult��Ҫ�޸�version
			UNSTD_STATIC_CHECK(sizeof(SCBattleInfo) == 3940);
			UNSTD_STATIC_CHECK(sizeof(SCBattleRoundResult) == 65552);
			UNSTD_STATIC_CHECK(SCBattleInfo::VERSION == 3);
			UNSTD_STATIC_CHECK(SCBattleRoundResult::VERSION == 0);

			bool ret = true;
			ret = ret && WRITE_BEGIN(header.msg_type);

			// EventVersion
			ret = ret && WRITE_USHORT(sc_battleinfo_version);
			ret = ret && WRITE_USHORT(sc_battleroundresult_version);
			ret = ret && WRITE_UINT(event_version_list.size());
			for (size_t i = 0; i < event_version_list.size(); ++i)
			{
				ret = ret && WRITE_SHORT(event_version_list[i].event_type);
				ret = ret && WRITE_USHORT(event_version_list[i].event_version);
			}
			if (!ret)
			{
				return false;
			}

			// battle info
			ret = ret && WRITE_BEGIN(battle_info->header.msg_type);
			ret = ret && WRITE_SHORT(battle_info->battle_mode);
			ret = ret && WRITE_CHAR(battle_info->character_num);
			ret = ret && WRITE_CHAR(battle_info->can_modify_speed);
			ret = ret && WRITE_CHAR(battle_info->is_ob);
			ret = ret && WRITE_CHAR(battle_info->ob_side);
			ret = ret && WRITE_CHAR(battle_info->is_pvp);
			ret = ret && WRITE_CHAR(battle_info->is_team_up);
			ret = ret && WRITE_UINT(battle_info->limit_timestamp);
			ret = ret && WRITE_INT(battle_info->monster_group_id);
			for (int i = 0; i < battle_info->character_num; ++i)
			{
				ret = ret && WRITE_STRN((const char*)&battle_info->character_info_list[i], sizeof(battle_info->character_info_list[i]));
			}
			if (!ret)
			{
				return false;
			}

			if (!hallow_list.Serialize(buffer, buffer_size, out_length))
			{
				return false;
			}

			ret = ret && WRITE_INT(static_cast<int>(round_result_list.size()));
			for (size_t i = 0; i < round_result_list.size(); ++i)
			{
				RoundResult& rr = round_result_list[i];

				ret = ret && WRITE_SHORT(rr.round);
				ret = ret && WRITE_CHAR(rr.has_prepare_result);
				ret = ret && WRITE_CHAR(rr.has_formal_result);
				if (1 == rr.has_prepare_result)
				{
					ret = ret && WRITE_BEGIN(rr.prepare_result->header.msg_type);
					ret = ret && WRITE_INT(rr.prepare_result->round);
					ret = ret && WRITE_INT(rr.prepare_result->tree_num);
					ret = ret && WRITE_INT(rr.prepare_result->total_data_len);
					ret = ret && WRITE_STRN(rr.prepare_result->result_buffer, rr.prepare_result->total_data_len);
					if (!ret)
					{
						return false;
					}
				}
				if (1 == rr.has_formal_result)
				{
					ret = ret && WRITE_BEGIN(rr.formal_result->header.msg_type);
					ret = ret && WRITE_INT(rr.formal_result->round);
					ret = ret && WRITE_INT(rr.formal_result->tree_num);
					ret = ret && WRITE_INT(rr.formal_result->total_data_len);
					ret = ret && WRITE_STRN(rr.formal_result->result_buffer, rr.formal_result->total_data_len);
					if (!ret)
					{
						return false;
					}
				}
			}

			return true;
		}

		bool CheckDeserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = true;
			ret = ret && READ_BEGIN(header.msg_type);

			// EventVersion
			ret = ret && READ_USHORT(sc_battleinfo_version);
			ret = ret && READ_USHORT(sc_battleroundresult_version);

			unsigned int event_version_num = 0;
			ret = ret && READ_UINT(event_version_num);
			event_version_num = GetMin(static_cast<unsigned int>(BATTLE_EVENT_MAX), event_version_num);
			event_version_list.reserve(event_version_num);
			for (unsigned int i = 0; i < event_version_num; ++i)
			{
				static EventVersion version;
				ret = ret && READ_SHORT(version.event_type);
				ret = ret && READ_USHORT(version.event_version);
				event_version_list.push_back(version);
			}
			if (!ret)
			{
				return false;
			}

			// battle info
			static Protocol::SCBattleInfo battle_info;
			ret = ret && READ_BEGIN(battle_info.header.msg_type);
			ret = ret && READ_SHORT(battle_info.battle_mode);
			ret = ret && READ_CHAR(battle_info.character_num);
			ret = ret && READ_CHAR(battle_info.can_modify_speed);
			ret = ret && READ_CHAR(battle_info.is_ob);
			ret = ret && READ_CHAR(battle_info.ob_side);
			ret = ret && READ_CHAR(battle_info.is_pvp);
			ret = ret && READ_CHAR(battle_info.is_team_up);
			ret = ret && READ_UINT(battle_info.limit_timestamp);
			ret = ret && READ_INT(battle_info.monster_group_id);
			
			if (battle_info.character_num <= 0 || battle_info.character_num > ARRAY_ITEM_COUNT(battle_info.character_info_list))
			{
				return false;
			}

			for (int i = 0; i < battle_info.character_num && i < ARRAY_ITEM_COUNT(battle_info.character_info_list); ++i)
			{
				ret = ret && READ_STRN((char*)&battle_info.character_info_list[i], sizeof(battle_info.character_info_list[i]), sizeof(battle_info.character_info_list[i]));
			}
			if (!ret)
			{
				return false;
			}

			if (!hallow_list.Deserialize(data, data_length, out_offset))
			{
				return false;
			}

			int round_result_num = 0;
			ret = ret && READ_INT(round_result_num);
			if (round_result_num > 10000)
			{
				return false;
			}
			for (int i = 0; i < round_result_num; ++i)
			{
				short round = 0;
				char has_prepare_result = 0;
				char has_formal_result = 0;
				ret = ret && READ_SHORT(round);
				ret = ret && READ_CHAR(has_prepare_result);
				ret = ret && READ_CHAR(has_formal_result);
				if (1 == has_prepare_result)
				{
					static Protocol::SCBattleRoundResult prepare_result;
					ret = ret && READ_BEGIN(prepare_result.header.msg_type);
					ret = ret && READ_INT(prepare_result.round);
					ret = ret && READ_INT(prepare_result.tree_num);
					ret = ret && READ_INT(prepare_result.total_data_len);
					ret = ret && READ_STRN(prepare_result.result_buffer, prepare_result.total_data_len, sizeof(prepare_result.result_buffer));
					if (!ret)
					{
						return false;
					}
				}

				if (1 == has_formal_result)
				{
					static Protocol::SCBattleRoundResult formal_result;
					ret = ret && READ_BEGIN(formal_result.header.msg_type);
					ret = ret && READ_INT(formal_result.round);
					ret = ret && READ_INT(formal_result.tree_num);
					ret = ret && READ_INT(formal_result.total_data_len);
					ret = ret && READ_STRN(formal_result.result_buffer, formal_result.total_data_len, sizeof(formal_result.result_buffer));
					if (!ret)
					{
						return false;
					}
				}
			}

			return true;
		}

		struct RoundResult
		{
			RoundResult() : round(0), has_prepare_result(0),
				has_formal_result(0), prepare_result(NULL),
				formal_result(NULL) {}

			short round;
			char has_prepare_result;
			char has_formal_result;
			SCBattleRoundResult* prepare_result;
			SCBattleRoundResult* formal_result;
		};

		struct EventVersion
		{
			short event_type;
			UInt16 event_version;
		};

		UInt16 sc_battleinfo_version;
		UInt16 sc_battleroundresult_version;		
		std::vector<EventVersion> event_version_list;

		SCBattleInfo* battle_info;
		SCBattleHallowList hallow_list;
		std::vector<RoundResult> round_result_list;

		// ���vector�൱�ڣ�
		// int round_num;
		// RoundResult round_list[];
	};
}

#endif