#ifndef __DUEL_CONFIG_HPP__
#define __DUEL_CONFIG_HPP__

#include <vector>
#include <map>
#include <set>
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/dueldef.h"

struct DuelMapCfg
{
	DuelMapCfg()
	{
		revive_map = 0;
	}

	int revive_map;
	Posi pos;
};

struct DuelKillCfg
{
	DuelKillCfg()
	{
		kill_level = 0;
		min_value = 0;
		max_value = 0;
		title_id = 0;
		attr_down = 0;
	}

	int kill_level;
	int min_value;
	int max_value;
	int title_id;
	int attr_down;				// ȫ���Խ��ͣ��ٷֱȣ�
};

struct DuelOtherCfg
{
	DuelOtherCfg()
	{
		kill_value_base = 0;
		kill_value_multiple = 0;
		reduce_kill_value_time = 0;
		reduce_exp = 0;
		reduce_coin = 0;
		item_reduce_kill_value = 0;
		item_reduce_daily_times = 0;
		loss_reduce_kill_value = 0;
	}

	int kill_value_base;						// ����ʤ������ɱ¾ֵ����ֵ ��������ֵ+����*�ȼ��
	int kill_value_multiple;					// ����ʤ������ɱ¾ֵ������������ֵ+����*�ȼ��
	int reduce_kill_value_time;					// ÿx�����Զ�����һ��ɱ¾ֵ
	int reduce_exp;								// ����ʧ����ʧ����ֵ���ٷֱȣ�
	int reduce_coin;							// ����ʧ����ʧͭ��
	int item_reduce_kill_value;					// ���߼��ٵ�ɱ¾ֵ
	int item_reduce_daily_times;				// ÿ�տ�ʹ�õ��ߴ���
	int loss_reduce_kill_value;
	int duel_item_id;							// ������
};

// -----------------------------------------------------------------------------------
class DuelConfig : public ILogicConfig
{
public:
	DuelConfig();
	virtual ~DuelConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const DuelMapCfg* GetMapCfg(int scene_id)const;
	const DuelKillCfg* GetKillCfg(int kill_vaule)const;
	const DuelOtherCfg& GetOtherCfg()const { return m_other_cfg; }

	const bool SceneCanDuel(int scene_id)const;

private:
	int InitMapCfg(TiXmlElement * RootElement);
	int InitKillCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, DuelMapCfg> m_map_cfg;
	std::vector<DuelKillCfg> m_kill_cfg;
	DuelOtherCfg m_other_cfg;

};

#endif
