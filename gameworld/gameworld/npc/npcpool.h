#ifndef __NPCPOOL_HPP__
#define __NPCPOOL_HPP__

#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>
#include <vector>

static const int GATHER_ITEM_TYPE_PER_GROUP = 50;
static const int NPC_MINE_MONSTER_NPC_ID = 65534;		// �԰����߼������֣�д��NPC_ID  

struct NPCInformaCfg;		// NPC ������Ϣ��ֵ�� - ֵ // �� �� ���õ� seq - ID

enum SERVER_NPC_TYPE
{
	SERVER_NPC_TYPE_INVALID = 0,//��
	SERVER_NPC_TYPE_MONSTER = 1,//����
	SERVER_NPC_TYPE_GATHERS = 2,//�ɼ���
	SERVER_NPC_TYPE_CHOSED = 3,//����npc

	SERVER_NPC_TYPE_MAX
};

struct ServerNPCMonsterParam
{
	int monster_group_id;
};

struct ServerNPCGatherParam		// �ɼ���
{
	void Reset()
	{
		gather_id = 0;
		gather_time = 0;
		gather_type = SERVER_NPC_TYPE_INVALID;

		award_item_count = 0;
		memset(gather_item_group_list, 0, sizeof(gather_item_group_list));
	}

	int	gather_id;				// �ɼ���ID
	int gather_time;			// �ɼ�����ʱ��
	int gather_type;			// �ɼ�������

	int award_item_count;		// ��Ʒ����
	int gather_item_group_list[GATHER_ITEM_TYPE_PER_GROUP];	// ������
};

struct ServerNPCConsumeParam		// �����б�
{
	int	consume_id;				// ����ƷID
	int consume_num;			// ����Ʒ����
	int consume_coin;			// ����ͭ��
	int consume_gold;			// ����Ԫ��
};

union ServerNPCParam
{
	ServerNPCMonsterParam monster_param;
	ServerNPCGatherParam gather_param;
	ServerNPCConsumeParam consume_param;
};

struct NPCDynamicInformaCfg		// NPC ������Ϣ��ֵ�� - ֵ // �� �� ���õ� seq - ID
{
	NPCDynamicInformaCfg(int p_npc_id = 0) : npc_id(p_npc_id), scene_id(0), npc_posi(0, 0) {}

	int npc_id;				// NPC ID

	int scene_id;			// ����
	Posi npc_posi;			// λ��
};

struct NPCItemGetCfg		// �ɼ��ｱ������
{
	NPCItemGetCfg() : prob(0) {};

	int prob;
	ItemConfigData award_item;
};

struct ServerNPCInfoCfg
{
	ServerNPCInfoCfg() : npc_id(0), server_npc_type(0) {}

	int npc_id;
	int server_npc_type;	// ������NPC����  \see SERVER_NPC_TYPE

	ServerNPCParam server_npc_param;
};

enum OPTION_EFFECT_TYPE//ѡ��Ч��
{
	OPTION_EFFECT_TYPE_NO = 0,		//!< �Ի�
	OPTION_EFFECT_TYPE_BATTLE = 1,	//!< ս��
	OPTION_EFFECT_TYPE_CONSUME = 2,	//!< ����
	OPTION_EFFECT_TYPE_SHOP = 3		//!< �̵�
};

struct option_cfg
{
	option_cfg() :operation_id(0),option_content(0), option_effect_type(OPTION_EFFECT_TYPE_NO), cost_coin(0), monster_group_id(0){}
	int operation_id;					//!< ѡ��id
	int option_content;					//!< ѡ����������seq
	int option_effect_type;				//!< ѡ��Ч��
	int cost_coin;						//!< ����ͭ��
	int monster_group_id;
	ItemConfigData need_item;
};

struct NPCChosedTalkInfoCfg
{
	NPCChosedTalkInfoCfg(){}
	const static int MAX_OPTION_NUM = 2;// ���ѡ����
	int npc_id;
	int main_context;
	option_cfg option_list[MAX_OPTION_NUM];
};

struct NPCTreasureChestInfoCfg
{
	NPCTreasureChestInfoCfg() {}
	int box_id;
	int consume_id;
	int consume_num;
};

// ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������ //

class Role;

#define NPCPOOL NpcPool::Instance()

class NpcPool
{
public:
	typedef std::map<int, ServerNPCInfoCfg> ServerNpcCfgMap;

public:
	NpcPool();
	~NpcPool();

	static NpcPool * Instance();

	bool Init(const std::string & dir, std::string * err);
	static bool Reload(const std::string& dir, std::string* err);

	const NPCDynamicInformaCfg * GetNPCInformaCfg(int seq_id);
	const ServerNPCInfoCfg * GetServerNPCInfo(int npc_id);
	const ItemConfigData * RandGatherAwardItem(int award_pool_id);
	const std::vector<Posi>* GetSceneClientNpcList(int scene_id);
	const NPCChosedTalkInfoCfg * GetChosedNpcCfg(int npc_id);
	const NPCTreasureChestInfoCfg * GetTreasureChestCfg(int treasurechest_id);
private:
	int InitNpcInformaCfg(TiXmlElement * RootElement);
	int InitGatherListCfg(TiXmlElement * RootElement);
	int InitRewardPoolCfg(TiXmlElement * RootElement);
	int InitServerNPCCfg(TiXmlElement * RootElement);
	int InitOperationCfg(TiXmlElement *RootElement);//�ȼ���ѡ��
	int InitChosedNPCCfg(TiXmlElement *RootElement);//д�����õ�ʱ���ѡ����ؽ�ChosedNPC����
	int InitTreasureChestCfg(TiXmlElement *RootElement);

	std::map<int, NPCDynamicInformaCfg> m_npc_info_cfg;
	ServerNpcCfgMap m_server_npc_map;

	std::map<int, std::vector<NPCItemGetCfg> > m_gather_item_cfg;	// �ɼ���������

	std::map<int, std::vector<Posi> > m_scene_npc_posi_map;

	std::map<int, NPCChosedTalkInfoCfg> m_chosed_npc_cfg_map;//ѡ��NPC����

	std::map<int, option_cfg> m_option_cfg_map;//ѡ���������

	std::map<int, NPCTreasureChestInfoCfg> m_treasure_chest_cfg_map;//ѡ������������
};

#endif