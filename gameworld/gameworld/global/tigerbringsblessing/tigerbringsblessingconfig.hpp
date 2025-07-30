#ifndef  __TIGER_BRINGS_BLESSING_CONFIG_HPP__
#define  __TIGER_BRINGS_BLESSING_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/maildef.hpp"

struct TigerBringsBlessingOtherCfg
{
	TigerBringsBlessingOtherCfg() :current_price(0), reward_count(0), return_count(0),
		start_time(0), until_time(0), known_time(0), show_time(0), timestamp(0) {}

	int current_price;												// ����۸�

	int reward_count;												// ��
	ItemConfigData reward_item[MAX_ATTACHMENT_ITEM_NUM];			// �깺����

	int return_count;												// �깺ʧ�ܷ�������������

	int start_time;													// ����ʱ�� [��ʾ������һ���0�����ٸ�Сʱ�����] [��λ:Сʱ]
	int until_time;													// �������ʱ��,�ӿ���ʱ����� [��λ:Сʱ]
	int known_time;													// չʾ�ڿ���ʱ��,�ӿ���ʱ����� [��λ:Сʱ]
	int show_time;													// չʾ��ʱ�� , ��չʾ�ڿ�������� [��λ:Сʱ]

	unsigned int timestamp;											// ʱ��� ����ʱ�������ʱ��֮��ĲŻῪ���
};


// �����ɸ�
class TigerBringsBlessingConfig : public ILogicConfig
{
public:
	TigerBringsBlessingConfig();
	virtual ~TigerBringsBlessingConfig();

	virtual bool Init(const std::string & configname, std::string * err);
	const TigerBringsBlessingOtherCfg & GetOtherCfg() { return m_other_cfg; }

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);

	TigerBringsBlessingOtherCfg	m_other_cfg;
};

#endif

