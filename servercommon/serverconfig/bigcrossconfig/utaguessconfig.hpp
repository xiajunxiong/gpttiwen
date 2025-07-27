#ifndef __UTA_GUESS_CONFIG_H__
#define __UTA_GUESS_CONFIG_H__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"

struct UTAGuessOtherCfg
{
	UTAGuessOtherCfg():lottery_id(0) , exchange_rate(0) , min_odds(0) , max_odds(0), once_lottery_limit(0)
	{}

	int lottery_id;					// ���±ҵ���ID
	int exchange_rate;				// ����ת��
	int min_odds;					// ��������(Ĭ�����ʣ�
	int max_odds;					// ��������
	int once_lottery_limit;			// ������ע����
};


class UTAGuessConfig : public ILogicConfig
{
public:
	UTAGuessConfig();
	~UTAGuessConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const UTAGuessOtherCfg & GetOtherCfg()const  { return m_other_cfg; }

private:
	int InitOtherConfig(TiXmlElement *RootElement, std::string& errormsg);

	UTAGuessOtherCfg m_other_cfg;
};

#endif	// __UTA_GUESS_CONFIG_H__

