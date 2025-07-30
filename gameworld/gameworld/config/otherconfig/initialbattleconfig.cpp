#include "initialbattleconfig.hpp"
#include "servercommon/servercommon.h"

InitialBattleConfig::InitialBattleConfig()
{
	memset(m_job_seq_hash, 0, sizeof(m_job_seq_hash));
}

InitialBattleConfig::~InitialBattleConfig()
{

}

bool InitialBattleConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("job_seq", InitJobSeqCfg);

	return true;
}

int InitialBattleConfig::GetInitialBattleSeq(int profession)
{
	int base_prof = GetBaseProf(profession);
	if (base_prof <= 0 || base_prof >= PROFESSION_TYPE_NUM) return 0;

	return m_job_seq_hash[base_prof];
}

int InitialBattleConfig::InitJobSeqCfg(TiXmlElement *RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (dataElement)
	{
		int job = 0;
		if (!GetSubNodeValue(dataElement, "job", job))
		{
			return -1;
		}
		job = GetBaseProf(job);
		if (job <= 0 || job >= PROFESSION_TYPE_NUM)
		{
			return -111;
		}

		int seq = 0;
		if (!GetSubNodeValue(dataElement, "group", seq) || seq <= 0)
		{
			return -2;
		}

		m_job_seq_hash[job] = seq;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

