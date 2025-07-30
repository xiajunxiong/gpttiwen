#ifndef __RA_CHAO_ZHI_GIFT_CONFIG_HPP__
#define __RA_CHAO_ZHI_GIFT_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rachaozhigiftdef.hpp"

class RandActivityManager;

namespace RAChaoZhiGift
{
	//����
	struct OtherCfg
	{
		OtherCfg()
		{
			cfg_ver = 0;
			can_buy_day = 0;
		}
		int cfg_ver;
		int can_buy_day;		//������  �����ǰN�� 
	};

	//ֱ��
	struct CmdBuyCfg
	{
		CmdBuyCfg()
		{
			buy_money = 0;
		}

		int buy_money;		//����۸�(Ԫ��)
	};

	//��������
	struct RewardPoolItem
	{
		RewardPoolItem()
		{
			power = 0;

			reward_min = 0;
			reward_max = 0;
		}

		int power;	//����Ȩ��

		//�������� ������(����)
		int reward_min;
		int reward_max;
	};


	//�������
	struct GiftCfg
	{
		GiftCfg()
		{
			gift_index = 0;
			can_fetch_act_day = 0;

			reward_id = 0;
			is_bind = 0;

			reward_min = 0;
			reward_max = 0;

			total_power = 0;
		}

		const ItemConfigData RandRewardItem() const;

		int gift_index;
		int can_fetch_act_day;		//�����ȡʱ����Ļ��������

		ItemID reward_id;	//������Ʒid
		int is_bind;		//��Ʒ��

		//�������� ������(��֤��)
		int reward_min;
		int reward_max;

		//����<Ȩ��_begin, ����>
		int total_power;
		std::map<int, RewardPoolItem> reward_pool;
	};
	//���map<���index, �������>
	typedef std::map<int, GiftCfg> GiftIndexMap;
};

class RAChaoZhiGiftConfig : public RandActivityConfig
{
public:
	RAChaoZhiGiftConfig();
	virtual ~RAChaoZhiGiftConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAChaoZhiGift::OtherCfg &GetOtherCfg() const { return m_other_cfg; };
	const RAChaoZhiGift::CmdBuyCfg * GetCmdBuyCfg(RandActivityManager* ramgr) const;
	const RAChaoZhiGift::GiftCfg * GetGiftCfg(RandActivityManager* ramgr, unsigned int act_begin_time, int gift_index) const;

private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitCmdBuyCfg(TiXmlElement * RootElement);
	int InitGiftCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);

	RAChaoZhiGift::OtherCfg m_other_cfg;

	//ֱ���׶�����<�����ʱ�Ŀ�������, ֱ������>
	std::map<int, RAChaoZhiGift::CmdBuyCfg> m_cmd_buy_day_section_cfg;

	//����׶�����<�����ʱ�Ŀ�������, ���map>
	std::map<int, RAChaoZhiGift::GiftIndexMap> m_gift_day_section_cfg;

	
};

#endif