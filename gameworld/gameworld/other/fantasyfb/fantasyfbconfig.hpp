#ifndef __FANTASY_FB_CONFIG_HPP__
#define __FANTASY_FB_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/commondata.hpp"
#include "servercommon/struct/itemlistparam.h"

static const int TEAM_MEMBER_MAX_NUM = 5;
static const int RATE_COUNT_MAX_NUM = 10000;			//������ֵ

struct SpecialLogicFantasyObjInfo
{
	SpecialLogicFantasyObjInfo() :npc_id(0), obj_type(0), obj_id(0), pos_x(0), pos_y(0)
	{}

	int npc_id;
	short obj_type;
	ObjID obj_id;
	short pos_x;
	short pos_y;
};

struct FantasyFBOtherCfg
{
	FantasyFBOtherCfg():npc_id(0), min_team_num(0), min_level(0)
	{}

	int npc_id;
	int min_team_num;					//������С����
	int min_level;
};

struct FantasyFBSceneCfg
{
	FantasyFBSceneCfg():seq(0), scene_id(0), boss_id(0), monster_dir(0),
		event_num(0), event_group_id(0)
	{
		boss_pos = Posi(0, 0);
		birth_pos = Posi(0, 0);
		transport_pos = Posi(0, 0);
	}
	int seq;
	int scene_id;					//��ǰ����id
	int boss_id;					//�ò�boss_id
	Posi boss_pos;					//boss����
	Dir monster_dir;				//bossת��
	Posi birth_pos;					//����������
	Posi transport_pos;				//����������
	int event_num;					//�ɼ�������
	int event_group_id;				//�ɼ��������
};

struct FantasyFBRewardGroupCfg
{
	FantasyFBRewardGroupCfg():rate(0)
	{}
	
	int rate;
	ItemConfigData reward;
};

struct FantasyFBRewardCfg				//boss��ɱ����s
{
	FantasyFBRewardCfg(): boss_id(0), bind_coin(0), reward_exp(0)
	{}

	int boss_id;
	int bind_coin;
	int reward_exp;
	std::vector<ItemConfigData> reward;
};

struct FantasyFbKillBossRewardCfg			//boss��ɱ�ȼ��齱��
{
	FantasyFbKillBossRewardCfg(): count(0)
	{}

	int count;
	FantasyFBRewardCfg node[LAYER_MAX_NUM];
};

struct Event_NPC_FB_Cfg
{
	enum FANTASYFB_NPC_TYPE
	{
		FANTASYFB_NPC_TYPE_0 = 0,		//!< δ֪��Ʒ
		FANTASYFB_NPC_TYPE_1 = 1,		//!< ����
		FANTASYFB_NPC_TYPE_2 = 2,		//!< npc
		FANTASYFB_NPC_TYPE_3 = 3,		//!< Boss
		FANTASYFB_NPC_TYPE_4 = 4,		//!< ����
		FANTASYFB_NPC_TYPE_5 = 5,		//!< ͭ��
		FANTASYFB_NPC_TYPE_6 = 6,		//!< �ɼ��� - ľ��
		FANTASYFB_NPC_TYPE_7 = 7,		//!< �ɼ��� - ��ʯ
		FANTASYFB_NPC_TYPE_8 = 8,		//!< �ɼ��� - ��
		FANTASYFB_NPC_TYPE_9 = 9,		//!< ����
		FANTASYFB_NPC_TYPE_10 = 10,		//!< ��˵��
	};
	Event_NPC_FB_Cfg() : event_group_id(0), npc_id(0), rate(0), npc_type(0), box_type(0), consume_item(0), consume_num(0)
	{}

	int event_group_id;					//�¼���id
	int npc_id;	
	int rate;
	int npc_type;
	int box_type;
	int consume_item;					//�����¼��Ǳ���ʱ����Ҫ
	int consume_num;					
};

struct FantasyFBGatherRewardCfg
{
	FantasyFBGatherRewardCfg():npc_id(0), reward_type(0), reward_id(0)
	{}
	enum REWARD_TYPE
	{
		REWARD_TYPE_0 = 1,//buff
		REWARD_TYPE_1 = 2,//����id
		REWARD_TYPE_2 = 3,//ͭ��
		REWARD_TYPE_MAX,
	};

	int npc_id;
	int reward_type;
	int reward_id;				//������ID
};

struct FantasyFBGatherRewardlevelCfg
{
	FantasyFBGatherRewardlevelCfg()
	{}

	std::vector<FantasyFBGatherRewardCfg> node_cfg;
};

struct FantasyFBFindRewardCfg				//�һؽ���
{
	FantasyFBFindRewardCfg():find_type(0), exp(0), coin(0), family_gongxian(0), group_id(0), min_level(0), max_level(0)
	{}

	int find_type;
	int exp;
	int coin;
	int family_gongxian;
	int group_id;
	int min_level;
	int max_level;
	int seq;
};

struct FantasyFbFindLevelGroupCfg			//�һصȼ�������
{
	FantasyFbFindLevelGroupCfg(): count(0)
	{}

	int count;
	FantasyFBFindRewardCfg node[LAYER_MAX_NUM];
};


struct FantasyFBFindRewardGroupCfg
{
	FantasyFBFindRewardGroupCfg():rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

class FantasyFbConfig : public ILogicConfig
{
public:
	FantasyFbConfig();
	virtual ~FantasyFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const FantasyFBOtherCfg* GetOtherCfg() { return &m_other_cfg; }
	const FantasyFBSceneCfg* GetSceneCfgBySeq(int seq);
	const FantasyFBSceneCfg* GetFBSceneCfg(int level, std::map<int, int>* seq_by_layer);
	const int GetSeqByBossid(int _boss_id);

	const FantasyFBRewardGroupCfg* GetRewardGroupCfg(int reward_id);
	const FantasyFbKillBossRewardCfg* GetRewardCfg(int level);
	int GetEventNPCFBCfg(int event_group_id, int need_npc_num, std::vector<Event_NPC_FB_Cfg>* node_cfg, std::map<int, std::vector<SpecialLogicFantasyObjInfo> >* obj_info_list);
	const FantasyFBGatherRewardCfg* GetGatherRewardCfg(int level, int npc_id);
	const FantasyFbFindLevelGroupCfg* GetFindCfg(int level, int find_type);
	const FantasyFBFindRewardGroupCfg* GetFindRewardCfg(int group_id);
	const int GetFindType(int _role_level);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitEventNPCCfg(TiXmlElement *RootElement);
	int InitGatherRewardCfg(TiXmlElement *RootElement);
	int InitFindRewardGroupCfg(TiXmlElement *RootElement);
	int InitFindCfg(TiXmlElement *RootElement);
	
	FantasyFBOtherCfg m_other_cfg;			
	std::map<int, FantasyFBSceneCfg> m_scene_cfg;								//��seqΪkey
	std::map<int, std::vector<FantasyFBSceneCfg> > m_scene_by_level;			//��1000 * min_level + max_levelΪkey

	std::map<int, std::vector<FantasyFBRewardGroupCfg> > m_reward_group_cfg;	//��reward_idΪkey
	std::map<int, FantasyFbKillBossRewardCfg> m_reward_cfg;						//��1000 * min_level + max_levelΪkey
	std::map<int, std::vector<Event_NPC_FB_Cfg> > m_event_npc_cfg;				//��event_group_id Ϊkey
	std::map<int, FantasyFBGatherRewardlevelCfg> m_gather_reward_cfg;			//��1000 * min_level + max_levelΪkey

	// �����һ�
	std::map<int, std::vector<FantasyFBFindRewardGroupCfg> > m_find_reward_group_map;			// �һص����飺�� - ��ID
	std::map<int, std::map<int, FantasyFbFindLevelGroupCfg> > m_fantasy_find_cfg;	// �����һر��� - �ȼ����䡢�һ�����

	std::map<int, int> m_bossid_to_seq;				//boss_id  -->  seq
	std::map<int ,FantasyFBFindRewardCfg> m_seq_to_fanrwcfg;
	std::vector<FantasyFBFindRewardCfg> m_fanrwcfg_v;
};


#endif
