#ifndef __ROLE_LEVEL_COMPLEMENT_CONFIG_HPP__
#define __ROLE_LEVEL_COMPLEMENT_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/levelcomplementdef.hpp"
#include <vector>
#include <map>

struct RoleLevelComplementCfg
{
	const static int MULTIPLE_PERSENT = 100;
	RoleLevelComplementCfg()
	{
		day_difference = 0;
		complement_day = 0;
		first_day_exp = 0;
		second_day_exp = 0;
		add_exp_num = 0;
		exp = 0;
		exp_limit = 0;
		multiple = 0;
		buy_exp = 0;
		price_type = 1;
		price = 0;
	}

	int day_difference;
	int complement_day;
	int first_day_exp;
	int second_day_exp;
	int add_exp_num;
	int exp;
	int exp_limit;
	int multiple;
	int buy_exp;
	int price_type;
	int price;
};

struct RoleNewLevelComplementCfg
{
	RoleNewLevelComplementCfg() :day_add_exp_coefficient(0), exp_limit_coefficient(0), buy_exp_coefficient(0), money_type(0), price(0)
	{
		memset(multiple_coefficient_list, 0, sizeof(multiple_coefficient_list));
	}

	int day_add_exp_coefficient;	//�賿���侭��ϵ��
	int exp_limit_coefficient;		//�ȼ�����洢����
	int multiple_coefficient_list[MAX_LEVEL_COMPLEMENT_MULTIPLE_NUM];	//�ճ������ȡ����
	int buy_exp_coefficient;		//���ι�����ϵ��
	int money_type;					//��������
	int price;						
};

struct RoleLevelComplementOtherCfg
{
	RoleLevelComplementOtherCfg() : rank_level_difference(0), ranking(0), day_buy_num(0)
	{}

	int rank_level_difference;		//�ȼ���
	int ranking;					//���� (�����ȼ����а�ǰ)
	int day_buy_num;				//���칺�����ƴ���
};

class RoleLevelComplementConfig : public ILogicConfig
{
public:
	RoleLevelComplementConfig();
	virtual ~RoleLevelComplementConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const RoleLevelComplementCfg * GetLevelCfg(int day_difference);

	const RoleLevelComplementOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const RoleNewLevelComplementCfg * GetNewLevelCfg(int level_difference) const;
private:
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitNewLevelCfg(TiXmlElement *RootElement);

	std::map<int, RoleLevelComplementCfg> m_role_level_map;

	RoleLevelComplementOtherCfg m_other_cfg; 
	int m_max_level_difference;
	RoleNewLevelComplementCfg m_new_level_complement_list[MAX_ROLE_LEVEL + 1];	
};

#endif