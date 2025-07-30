#ifndef __RAND_ACTIVITY_HAPPY_UP_CONFIG_HPP__
#define __RAND_ACTIVITY_HAPPY_UP_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rahappyupparam.hpp"
#include "item/itembase.h"

struct RAHappyUpItemCfg
{
	RAHappyUpItemCfg() :item_seq(0), consume_item_id(0), consume_item_count(0), today_times(0), buy_times(0), limit_type(0), hearsay(0)
	{}

	int item_seq;
	ItemConfigData exchange_item;			//�����ĵ���
	int consume_item_id;					//���ĵ��ߵ�id
	int consume_item_count;					//���ĵ��ߵ�����
	int today_times;						//ÿ���޶�
	int buy_times;							//���޶�
	int limit_type;							//�޶�����
	int hearsay;							//���� 1:������ 0:����
};

struct RAHappyUpSectionCfg
{
	RAHappyUpSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAHappyUpItemCfg> item;
};

class RandActivityManager;

class RandActivityHappyUpConfig : public RandActivityConfig
{
public:
	RandActivityHappyUpConfig();
	virtual ~RandActivityHappyUpConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAHappyUpItemCfg* GetHappyUpItemCfg(RandActivityManager* ramgr, int item_seq)const;
	int GetHappyUpSectionItemCount(RandActivityManager* ramgr)const;
	const RAHappyUpSectionCfg * GetHappyUpSectionCfg(RandActivityManager* ramgr)const;
private:
	int InitCfg(TiXmlElement *RootElement);

	std::vector<RAHappyUpSectionCfg> m_happy_up_section_cfg;
};

#endif	//__RAND_ACTIVITY_HAPPY_UP_CONFIG_HPP__