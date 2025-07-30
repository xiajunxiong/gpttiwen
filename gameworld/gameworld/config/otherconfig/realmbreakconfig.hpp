#ifndef __REALM_BREAK_CONFIG_HPP__
#define __REALM_BREAK_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/realmbreakdef.hpp"
#include "servercommon/struct/itemlistparam.h"

struct RealmBreakOtherCfg
{
	RealmBreakOtherCfg() :break_num(0), break_max_num(0), break_exp_s(0), player_exp_s(0), tuna_time_s(0), tuna_num(0), item_time_s(0), gaimingdan_item_s(0)
	{}

	int break_num;				//ÿ������Ľ���
	int break_max_num;			//������
	int break_exp_s;			//ÿ�����������������
	int player_exp_s;			//ÿ�ξ���/ͭ�������������
	int tuna_time_s;			//����ʱ��
	int tuna_num;				//ÿ�����ɴ���
	int item_time_s;			//ÿ��������߽����������
	int gaimingdan_item_s;		//ÿ�θ������������ļ������
};

struct RealmBreakCfg
{
	static const int MAX_RATE_COUNT_NUM = 10000;

	RealmBreakCfg():break_need_exp(0), break_exp_time_s(0), player_exp_time_s(0), player_coin_time_s(0), monster_group_id(0), tuna_exp(0), exp_double(0), exp_treble(0), exp_quintuple(0),
		tuna_num(0), break_player_exp_award(0), break_lose(0), break_exp_lixian_max(0), player_exp_lixian_max(0), time_max_s(0), item_group_id(0),
		time_interval_item_id(0), time_interval_item_num(0)
	{}

	int break_need_exp;			//����ͻ����������ֵ
	int break_exp_time_s;		//ʱ�����Զ����ӵ�����ֵ
	int player_exp_time_s;		//ʱ�����Զ����ӵľ���
	int player_coin_time_s;		// ʱ�����Զ����ӵ�ͭ��
	int monster_group_id;
	int tuna_exp;				//������������ֵ
	int exp_double;
	int exp_treble;
	int exp_quintuple;
	int tuna_num;				//�����ɴ��� -1:�����ƴ��� 
	int break_player_exp_award;
	std::vector<ItemConfigData> reward_list;
	int break_lose;
	int break_exp_lixian_max;	//��������ֵ�洢����
	int player_exp_lixian_max;	//���߾���洢����
	int time_max_s;				//�洢���ʱ��
	int item_group_id;			//������ID
	int time_interval_item_id;	//ÿ��һ��ʱ���������߽���
	int time_interval_item_num;	//ÿ��һ��ʱ���������߽��� \see other���gaimingdan_item_s
};

struct RealmBreakItemCfg
{
	static const int RAND_RATE_BASE_NUM = 10000;
	RealmBreakItemCfg() : rate(0)
	{}

	int rate;
	ItemConfigData item;
};

struct RealmBreakFightCfg
{
	RealmBreakFightCfg() :seq(0), monster_group_id(0), level(0), coin(0), break_exp(0), player_exp(0) {}

	int seq;
	int monster_group_id;
	int level;
	int coin;
	int break_exp;
	int player_exp;
	std::vector<ItemConfigData> item;
};

class RealmBreakConfig : public ILogicConfig
{
public:
	RealmBreakConfig();
	virtual ~RealmBreakConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const RealmBreakOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const RealmBreakCfg * GetBreakCfg(int seq);
	int GetBreakExpByItemId(ItemID item_id);
	bool GetRandItem(int item_group_id, int& rand_num, RealmBreakItem * item_list);
	const RealmBreakFightCfg* GetFightCfg(int seq)const;
	const RealmBreakFightCfg* MonsterGroupGetFightCft(int monster_group_id)const;
	int GetFightCanPetNum(int create_role_day)const;

	const std::vector<ItemConfigData>* GetGaiMingReissueCfg(int layer)const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitItemGroupCfg(TiXmlElement *RootElement);
	int InitBreakCfg(TiXmlElement *RootElement);
	int InitBreakItemCfg(TiXmlElement *RootElement);
	int InitBreakFightCfg(TiXmlElement *RootElement);
	int InitHelpFightCfg(TiXmlElement *RootElement);
	int InitGaiMingItemCfg(TiXmlElement *RootElement);	// ����������

	RealmBreakOtherCfg m_other_cfg;
	std::map<int, std::vector<RealmBreakItemCfg> > m_item_group_cfg;	//���������� 
	std::map<int, RealmBreakCfg> m_break_cfg;							//�������� key:����(����seq)
	std::map<ItemID, int> m_item_cfg;									//ҩƷ���� key:item_id second:add_break_exp
	std::vector<RealmBreakFightCfg> m_fight_cfg;
	std::map<int, int> m_help_fight_map;								//�������� key:��������

	std::vector<std::vector<ItemConfigData> > m_gaiming_cfg;			// ����������
};

#endif