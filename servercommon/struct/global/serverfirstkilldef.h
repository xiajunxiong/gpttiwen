#ifndef __SERVER_FIRST_KILL_DEF_H__
#define __SERVER_FIRST_KILL_DEF_H__

#include "common/tlvprotocol.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MEMBER_NAME_MAX_NUM = 5;
static const int FIRST_KILL_BOSS_TYPE_MAX_NUM = 100;
static const int TYPE_LEVEL_GROUP_SEQ_MAX_NUM = 30;
static const int SERVER_FIRST_KILL_SAVE_TO_DB_TIME = 60;	//�´α��浽���ݿ�ļ��ʱ��
static const int FIRST_KILL_TYPE_BASE_NUM = 10000;			//���� id = type*��ֵ + type_seq

struct RewardItemData
{
	RewardItemData()
	{
		this->Reset();
	}
	void Reset()
	{
		coin = 0;
		data.Reset();
	}
	struct ItemConfigPack
	{
		ItemConfigPack()
		{
			this->Reset();
		}
		void Reset()
		{
			item_id = 0;
			is_bind = 0;
			item_num = 0;
		}
		ItemID item_id;
		short is_bind;
		int item_num;
	};

	int coin;							//������ͭ��
	ItemConfigPack data;				//�����ĵ���
};

typedef char RewardItemDataHex[sizeof(RewardItemData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RewardItemDataHex) < 64);

struct ServerFirstKillEntry
{
	ServerFirstKillEntry()
	{
		this->Reset();
	}
	void Reset(bool is_deletle_video_record = true)
	{
		if (is_deletle_video_record)
		{
			battle_video_record_file_id = 0;
		}
		server_id = 0;
		type = 0;
		level_group = 0;
		seq = 0;
		monster_group_id = 0;
		member_count = 0;
		kill_timestamp = 0;
		kill_game_timestamp = 0;
		round_num = 0;
		kill_flag = 0;
		memset(member_name, 0, sizeof(member_name));
		reward_data.Reset();
		for (int i = 0; i < MEMBER_NAME_MAX_NUM; i++)
		{
			uid[i] = 0;
		}
	}
	bool Invalid()
	{
		return kill_flag < 1;
	}

	long long battle_video_record_file_id;		//��Ӧս��¼���ļ�
	int server_id;								//������ID	//ע���������ɱ�Ͽ���ķ�����ID��һ��
	short type;									//�淨����
	short level_group;							//�ȼ���
	short seq;									//�淨��������һ��������
	short member_count;							//�����ɱ��ҵ�����
	short round_num;							//��ҽ��еĻغ���
	short kill_flag;							//��ɱ��ʶ
	int monster_group_id;						//������ID
	long long kill_timestamp;					//��ɱ��ʱ��
	long long kill_game_timestamp;													
	int uid[MEMBER_NAME_MAX_NUM];				//��ҵ�uid
	GameName member_name[MEMBER_NAME_MAX_NUM];	//�����ɱ����ҵ�����
	RewardItemData reward_data;					//��������		
};

#pragma pack(pop)

enum ServerFirstKillBossType
{
	TYPE_MIYI_BOSS = 1,					//����֮������
	TYPE_CHALLENGE = 2,					//��ս����BOSS
	TYPE_WILD_BOSS = 3,					//Ұ��BOSS
	TYPE_SHAN_HAI_BOSS = 4,				//ɽ������
	TYPE_BIG_DIPPER = 5,				//��������
	TYPE_FALLEN_GOD = 6,				//����֮��
	TYPE_HONGMENG_TIANJIE = 7,			//�������

	SERVER_FIRST_KILL_TYPE_MAX,
};

static const int FIRST_KILL_TYPE_MAX = SERVER_FIRST_KILL_TYPE_MAX - 1;

class ServerFirstKillParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;																																							
	bool Unserialize(TLVUnserializer &instream);

	struct ServerFirstKillSaveEntry
	{
		ServerFirstKillSaveEntry()
		{
			this->Reset();
		}
		void Reset(bool is_deletle_video_record = true)
		{
			first_kill_info.Reset(is_deletle_video_record);
		}
		char change_state;					//�ı��״̬
		int id;

		ServerFirstKillEntry first_kill_info;
	};

	ServerFirstKillSaveEntry data_list[FIRST_KILL_TYPE_MAX][FIRST_KILL_BOSS_TYPE_MAX_NUM];

};

#endif 