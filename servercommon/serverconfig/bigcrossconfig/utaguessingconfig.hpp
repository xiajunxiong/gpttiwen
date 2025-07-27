#ifndef __UTA_GUESSING_CONFIG_H__
#define __UTA_GUESSING_CONFIG_H__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"

struct UTAGuessingOtherCfg
{
	UTAGuessingOtherCfg():exchange_rate(0) , min_odds(0) , max_odds(0){}

	int exchange_rate;				// ����ת��
	int min_odds;					// ��������(Ĭ�����ʣ�
	int max_odds;					// ��������
};


class UTAGuessingConfig : public ILogicConfig
{
public:
	UTAGuessingConfig();
	~UTAGuessingConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const UTAGuessingOtherCfg & GetOtherCfg() { return m_other_cfg; }

private:
	int InitOtherConfig(TiXmlElement *RootElement, std::string& errormsg);

	UTAGuessingOtherCfg m_other_cfg;
};

#endif	// __UTA_GUESSING_CONFIG_H__

