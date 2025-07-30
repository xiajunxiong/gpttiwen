#ifndef __RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_CONFIG_HPP__
#define __RAND_ACTIVITY_YUN_ZE_SLOT_MACHINE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "item/itempool.h"
#include "config/randactivityconfig/randactivityconfig.hpp"

static const int RA_YZ_SLOT_MACHINE_MAX_REWARD_POOL_NUM = 9;		// ���ؽ�����������

enum YUN_ZE_SLOT_MACHINE_TYPE
{
	YUN_ZE_SLOT_MACHINE_TYPE_BEGIN = 0,
	YUN_ZE_SLOT_MACHINE_TYPE_PERFECT = 1,	// ��߽���ƥ�����ֻ�ɫ
	YUN_ZE_SLOT_MACHINE_TYPE_WONDERFUL = 2,	// �еȽ���ƥ����ֻ�ɫ
	YUN_ZE_SLOT_MACHINE_TYPE_GOOD = 3,		// ��ͨ����ƥ��һ�ֻ�ɫ
	YUN_ZE_SLOT_MACHINE_TYPE_COMFORT = 4,	// ��ο����ƥ�����ֻ�ɫ

	YUN_ZE_SLOT_MACHINE_TYPE_MAX
};

enum YUN_ZE_SLOT_MACHINE_MONEY_TYPE
{
	YUN_ZE_SLOT_MACHINE_MONEY_TYPE_GOLD = 0,	// Ԫ��
	YUN_ZE_SLOT_MACHINE_MONEY_TYPE_RMB = 1,		// ֱ��
	YUN_ZE_SLOT_MACHINE_MONEY_TYPE_JADE = 2,	// ����
};

enum YUN_ZE_SLOT_MACHINE_BUY_TYPE
{
	YUN_ZE_SLOT_MACHINE_BUY_TYPE_TOTAL = 1,	// ���޹�
	YUN_ZE_SLOT_MACHINE_BUY_TYPE_TODAY = 2,	// ÿ���޹�
};

struct RAYZSlotMachineBasicsCfg
{
	RAYZSlotMachineBasicsCfg():item_pool(0), rate(0)
	{}

	int item_pool;
	int rate;
};

struct RAYZSlotMachineRewardCfg
{
	RAYZSlotMachineRewardCfg():item_group(0), item_pool(0)
	{}

	int item_group;
	int item_pool;
	ItemConfigData item;
};

struct RAYZSlotMachineTaskRewardCfg
{
	RAYZSlotMachineTaskRewardCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAYZSlotMachineRewardCfg> reward_list;
};

struct RAYZSlotMachineOtherCfg
{
	RAYZSlotMachineOtherCfg():price_type(0),price(0),item_consume(0),limit_type(0),total_buy_times(0),today_buy_times(0)
	{}

	int price_type;			// ��������
	int price;				// ������
	int cfg_ver;			// ֱ��������֤
	int item_consume;		// �齱����
	int limit_type;			// �޹�����
	int total_buy_times;	// ���޹�����
	int today_buy_times;	// ���޹�����
	ItemConfigData reward_item;		// �齱����
};

class RandActivityManager;
class RandActivityYunZeSlotMachineConfig : public RandActivityConfig
{
public:
	RandActivityYunZeSlotMachineConfig();
	virtual ~RandActivityYunZeSlotMachineConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int GetRandRewardByRateCfg()const;		// ���ؽ��غ�
	const RAYZSlotMachineTaskRewardCfg* GetTaskRewardCfg(RandActivityManager* ramgr)const;
	const RAYZSlotMachineOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	virtual int GetConfigVer()const { return m_other_cfg.cfg_ver; }

private:
	int InitBasicsCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);


	int m_total_rate;				// ��Ȩ��
	std::vector<RAYZSlotMachineBasicsCfg> m_basics_cfg;
	std::vector<RAYZSlotMachineTaskRewardCfg> m_task_reward_cfg;
	RAYZSlotMachineOtherCfg m_other_cfg;
};

#endif