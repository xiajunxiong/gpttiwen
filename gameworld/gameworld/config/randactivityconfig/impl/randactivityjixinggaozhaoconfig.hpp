#ifndef __RAND_ACTIVITY_JIXING_GAOZHAO_CONFIG_HPP__
#define __RAND_ACTIVITY_JIXING_GAOZHAO_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rajixinggaozhaoparam.hpp"

struct RAJiXingGaoZhaoBaseCfg
{
	RAJiXingGaoZhaoBaseCfg() :seq(0), reward_type(0), gold_num(0), reward_per(0), hearsay(0), guarantee_times(0) {}

	int seq;
	int reward_type;
	ItemConfigData reward_item;
	int gold_num;
	int reward_per;
	short hearsay;
	int guarantee_times;
};

struct RAJiXingGaoZhaoBaseMapCfg
{
	RAJiXingGaoZhaoBaseMapCfg() : rate_max(0) {}

	int rate_max;
	std::map<int, RAJiXingGaoZhaoBaseCfg> base_cfg;
};

struct RAJiXingGaoZhaoJackpotCfg
{
	RAJiXingGaoZhaoJackpotCfg() : jackpot_item_id(0), jackpot_item_num(0), jackpot_gold_num(0), all_draw_times(0), add_item_num(0), add_gold(0) {}

	ItemID jackpot_item_id;				// ���ص���
	int jackpot_item_num;				// ���س�ʼ��������
	int jackpot_gold_num;				// ���س�ʼԪ������

	int all_draw_times;					// ȫ���齱����
	int add_item_num;					// ���ӵ�������
	int add_gold;						// ����Ԫ������
};

struct RAJiXingGaoZhaoShopCfg
{
	RAJiXingGaoZhaoShopCfg() : limit_type(0), limit_times(0), price_type(0), buy_money(0) {}

	short limit_type;					// �޹�����
	short limit_times;					// �޹�����
	short price_type;					// ��������
	int buy_money;						// ������

	std::vector<ItemConfigData> reward_item;
};

struct RAJiXingGaoZhaoPhaseCfg
{
	RAJiXingGaoZhaoPhaseCfg() : lucky_draw_num(0) {}

	int lucky_draw_num;
	std::vector<ItemConfigData> reward_item;
};

struct RAJiXingGaoZhaoOtherCfg
{
	RAJiXingGaoZhaoOtherCfg() :item_id(0), item_num(0) {}

	ItemID item_id;
	int item_num;
};

class RandActivityManager;
class RandActivityJiXingGaoZhaoConfig : public RandActivityConfig
{
public:
	RandActivityJiXingGaoZhaoConfig();
	virtual ~RandActivityJiXingGaoZhaoConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	
	const RAJiXingGaoZhaoBaseCfg * GetBaseCfg(RandActivityManager * ramgr, int check_floor, const BitMap<MAX_JI_XING_GAO_ZHAO_REWARD_GROUP_NUM>* bit_map, bool *floor_back) const;
	const RAJiXingGaoZhaoJackpotCfg * GetJackpotCfg(RandActivityManager * ramgr) const;
	const RAJiXingGaoZhaoShopCfg * GetShopCfg(RandActivityManager * ramgr, int shop_index) const;
	const RAJiXingGaoZhaoPhaseCfg * GetPhaseCfg(RandActivityManager * ramgr, int phase_index) const;
	const RAJiXingGaoZhaoOtherCfg & GetOtherCfg()const { return m_other_cfg; }

	const std::vector<RAJiXingGaoZhaoShopCfg> * GetShopMapCfg(RandActivityManager * ramgr) const;

private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitJackpotCfg(TiXmlElement *RootElement);
	int InitShopCfg(TiXmlElement *RootElement);
	int InitPhaseCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::map<int, std::map<int, RAJiXingGaoZhaoBaseCfg*> > m_cfg_floor_map;
	std::map<int, RAJiXingGaoZhaoBaseMapCfg> m_base_map;
	std::map<int, RAJiXingGaoZhaoJackpotCfg> m_jackpot_cfg;
	std::map<int, std::vector<RAJiXingGaoZhaoShopCfg> > m_shop_cfg;
	std::map<int, std::vector<RAJiXingGaoZhaoPhaseCfg> > m_phase_cfg;
	RAJiXingGaoZhaoOtherCfg m_other_cfg;
};


#endif	// __RAND_ACTIVITY_JIXING_GAOZHAO_CONFIG_HPP__