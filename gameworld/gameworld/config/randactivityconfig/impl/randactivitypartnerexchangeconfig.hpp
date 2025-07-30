#ifndef __RAND_ACTIVITY_PARTNER_EXCHANGE_CONFIG_HPP__
#define __RAND_ACTIVITY_PARTNER_EXCHANGE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rapartnerexchangeparam.hpp"
#include "item/itembase.h"

struct RAPartnerExchangeCfg			//���һ�
{
	enum BUY_TEPY
	{
		BUY_TEPY_NO = 0,	// ��
		BUY_TEPY_ALL = 1,	// ���޶�
		BUY_TEPY_TODAY = 2,	// ÿ���޶�
	};
	RAPartnerExchangeCfg() :section_start(0), section_end(0), seq1(0), number(0), condition_id(0), item_id(0), condition_num(0), hearsay(0),
		buy_tepy(BUY_TEPY_NO), buy_seccond_day(0) {}

	int section_start;
	int section_end;
	int seq1;					//�����
	ItemConfigData item1;		//��û�����ĵ���
	ItemConfigData item2;		//��û�����ĵ���
	ItemConfigData item3;		//��û�����ĵ���

	int number;
	ItemConfigData reward_item;		//����

	int condition_id;				//��Ҫ�Ѽ�����id
	int item_id;					//��Ҫ��Ƭid
	int condition_num;				//��Ҫ��Ƭ����
	int hearsay;

	int buy_tepy;
	int buy_seccond_day;
};

class RandActivityManager;

class RandActivityPartnerExchangeConfig : public RandActivityConfig
{
public:
	RandActivityPartnerExchangeConfig();
	virtual ~RandActivityPartnerExchangeConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAPartnerExchangeCfg* GetPartnerExchangeBySeq(RandActivityManager* ramgr, int seq)const;
	int GetActOpenDay(RandActivityManager* ramgr, int activity_type) const;							//��ǰʱ��	    ��	�����ʱ������
	int GetActRealOpenDay(RandActivityManager* ramgr, int activity_type) const;						//�����ʱ��	��	����ʱ������

private:
	int InitPartnerExchangeCfg(TiXmlElement *RootElement);

	std::map<int, std::map<int, RAPartnerExchangeCfg> > m_partner_exchange_map;			// ���һ�
};

#endif	//__RAND_ACTIVITY_PARTNER_EXCHANGE_CONFIG_HPP__