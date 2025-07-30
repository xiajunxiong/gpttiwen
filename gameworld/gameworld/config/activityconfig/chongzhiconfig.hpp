#ifndef __CHONGZHI_CONFIG_HPP__
#define __CHONGZHI_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include <set>

static const int MAX_CHONGZHI_FANLI_REWARD_NUM = 5;
static const int CHONGZHI_REWARD_CONFIG_MAX_NUM = 32;
static const int DESCRIPTION_STR_LEN = 128;
static const int MAX_CHONG_ZHI_SHOW_ID_NUM = 7;

enum CHONGZHI_EXTRA_REWARD_TYPE
{
	CHONGZHI_EXTRA_REWARD_TYPE_INVALID = 0,
	CHONGZHI_EXTRA_REWARD_TYPE_GOLD_BIND = 1,			// ��Ԫ
	CHONGZHI_EXTRA_REWARD_TYPE_MONTH_CARD = 2,			// ����
	CHONGZHI_EXTRA_REWARD_TYPE_COIN_BIND = 3,			// ���
	CHONGZHI_EXTRA_REWARD_TYPE_GOLD = 4,				// Ԫ��
	CHONGZHI_EXTRA_REWARD_TYPE_NEW_MONTH_CARD = 5,		// �¿�

	CHONGZHI_EXTRA_REWARD_TYPE_MAX,
};

enum CHONGZHI_CURRENCY_TYPE
{
	CHONGZHI_CURRENCY_TYPE_RMB = 0,
	CHONGZHI_CURRENCY_TYPE_TWD,     // ̨��
	CHONGZHI_CURRENCY_TYPE_USD,		// ����
	CHONGZHI_CURRENCY_TYPE_KRW,		// ��Ԫ
	CHONGZHI_CURRENCY_TYPE_VND,		// Խ�϶�

	CHONGZHI_CURRENCY_TYPE_MAX,
};

enum CHONGZHI_PLAT_TYPE
{
	CHONGZHI_PLAT_TYPE_ANDROID,
	CHONGZHI_PLAT_TYPE_IOS,
};

struct ChongzhiFanliCfg
{
	ChongzhiFanliCfg() : seq(-1), min_level(0), max_level(0), gold(0), count(0) {}

	int seq;
	int min_level;
	int max_level;
	int gold;
	int count;
	ItemConfigData reward_list[MAX_CHONGZHI_FANLI_REWARD_NUM];
};

struct ChongzhiOtherConfig
{
	ChongzhiOtherConfig() :
		extra_reward_effective_day(0), first_recharge_grow_day(0), buy_money(0), cfg_ver(0)
	{

	}

	int extra_reward_effective_day;					// ����Ԫ��������Ч����������ǰn����Ч, ���������Ա���ƣ�

	//�׳�ר��
	int first_recharge_grow_day;					// �׳�ר��-�ɳ�������ʱ�䷶Χ
	int buy_money;
	int cfg_ver;
	std::vector<ItemConfigData> buy_reward_list;
};

// �µĳ�ֵ��
struct SpidChongzhiCfg
{
	SpidChongzhiCfg() : spid(0), show_count(0)
	{
		memset(show_id_list, 0, sizeof(show_id_list));
		memset(sheet, 0, sizeof(sheet));
	}

	//�ж����ⲿ��
	int GetShowBinary(int currency, int chong_zhi_flag)
	{
		int show_binary = show_id_list[currency][0];
		for (int i = 0; i < show_count && i < ARRAY_ITEM_COUNT(show_id_list[currency]); ++i)
		{
			show_binary = show_id_list[currency][i];
			//���Ҹõ�λ��Ч��߶�����λ
			bool is_conform = false;
			for (int k = sizeof(chong_zhi_flag) * 8 - 1; k >= 0; k--)
			{
				if (show_id_list[currency][i] & (1 << k))
				{
					if (chong_zhi_flag & (1 << k))
					{
						is_conform = true;
					}
					break;
				}
			}
			if (!is_conform)
			{
				break;
			}

			UNSTD_STATIC_CHECK(sizeof(show_id_list[currency][i]) == sizeof(chong_zhi_flag));
		}

		return show_binary;
	}

	int spid;
	int show_count;
	int show_id_list[CHONGZHI_CURRENCY_TYPE_MAX][MAX_CHONG_ZHI_SHOW_ID_NUM];
	int sheet[CHONGZHI_CURRENCY_TYPE_MAX];		// �����ű�  _1 _2 _3
	int double_spid;							// ˫�˵Ķ�Ӧspid, ���� spid=aeu, double_spid=ieu, ��֮��Ȼ
};

struct ChongzhiReward
{
	ChongzhiReward() : seq(0), chongzhi(0), reward_type(CHONGZHI_EXTRA_REWARD_TYPE_INVALID), reward_coin(0)
	{
		memset(money_show, 0, sizeof(money_show));
		memset(description_first_chongzhi, 0, sizeof(description_first_chongzhi));
	}

	int seq;
	int chongzhi;
	int reward_type;
	int reward_coin;

	// ���ͻ����·�������ʾ���ֶ�
	int money_show[CHONGZHI_CURRENCY_TYPE_MAX];
	char description_first_chongzhi[DESCRIPTION_STR_LEN];
};

struct SheetChongzhiCfg
{
	int reward_count;
	ChongzhiReward reward_list[CHONGZHI_REWARD_CONFIG_MAX_NUM];
};

static const int NEW_CHONGZHI_SHEET_COUNT = 7;

struct FristRechargeCfg
{
	FristRechargeCfg():seq(0),need_gold(0){}
	int seq;
	int need_gold;
	ItemConfigData reward;

	ItemConfigData day_reward_list[MAX_FIRST_RECHARGE_DAY_NUM - 1];	//Ĭ�ϵ�һ�첻��������
};

struct FirstRechargeExclusiveCfg
{
	FirstRechargeExclusiveCfg() : activity_type(0), param1(0)
	{}

	int activity_type;
	int param1;
	std::vector<ItemConfigData> reward_list;
};

class ChongzhiConfig : public ILogicConfig
{
public:
	ChongzhiConfig();
	virtual ~ChongzhiConfig();

	virtual bool Init(const std::string &path, std::string *err);
	const ChongzhiFanliCfg* GetFanliCfg(int seq, int level);
	void GetNewChongzhiConfig(int spid, int currency, unsigned int chong_zhi_flag, int* chongzhi_reward_count, 
		const ChongzhiReward* chongzhi_reward_list[CHONGZHI_REWARD_CONFIG_MAX_NUM]);
	const ChongzhiReward * GetExtraRewardByNum(int chongzhi, int currency_type, int spid);
	const ChongzhiReward * GetExtraRewardBySeq(int seq, int currency_type, int spid);
	int GetExtraRewardCfgCount(int currency_type, int spid);
	const ChongzhiOtherConfig & GetOtherCfg() { return m_other_cfg; }
	const FristRechargeCfg * GetFristRechargeCfg(int seq);
	const FirstRechargeExclusiveCfg * GetFirstRechargeExclusiveCfg(int type);
	bool IsFirstRechargeActivity(int activity_type);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitChongzhiCfg(TiXmlElement *RootElement, int* reward_count, ChongzhiReward* reward_list);
	int InitFanliCfg(TiXmlElement *RootElement);
	int InitSpidChongzhiCfg(TiXmlElement *RootElement);
	int InitFirstRechargeCfg(TiXmlElement *RootElement);
	int InitFirstRechargeExclusiveCfg(TiXmlElement *RootElement);
	int InitFirstRechargeActivityCfg(TiXmlElement *RootElement);

	std::vector<ChongzhiFanliCfg> m_fanli_cfg_list;	
	int m_new_chongzhi_sheet_count;
	SheetChongzhiCfg m_sheet_chongzhi_cfg_list[NEW_CHONGZHI_SHEET_COUNT];

	typedef std::map<int, SpidChongzhiCfg> SpidChongzhiCfgMap;
	SpidChongzhiCfgMap m_spid_chongzhi_cfg_map;
	ChongzhiOtherConfig m_other_cfg;
	int m_dev_spid;
	std::map<int, FristRechargeCfg> m_frist_recharge_cfg;
	std::map<int, FirstRechargeExclusiveCfg> m_first_recharge_exclusive_cfg;		// �׳�ר��	key:type 
	std::set<int> m_first_recharge_activity_list;			//�׳�ר��-�в�����(���ϵ���ʱ�ID)
};

#endif