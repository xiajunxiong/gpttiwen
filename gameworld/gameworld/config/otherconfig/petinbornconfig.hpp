#ifndef __PET_INBORN_CONFIG_HPP__
#define __PET_INBORN_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/petdef.h"

struct PetInbornOtherCfg
{
	PetInbornOtherCfg() : replace_consume(0), return_consume(0)
	{}

	int replace_consume;		//替换天赋消耗
	int return_consume;			//重置天赋消耗
};

struct PetInbornBaseCfg
{
	PetInbornBaseCfg() : inborn_type(0), inborn_group_type(0), unlock_group_str_level(0)
	{
		memset(inborn_id_list, 0, sizeof(inborn_id_list));
	}

	int inborn_type;			//宠物天赋类型
	int inborn_group_type;		//天赋组类型 -- MAX_PET_TALENT_GROUP_NUM
	int unlock_group_str_level;	//解锁该组所需强化等级
	int inborn_id_list[MAX_PET_TALENT_GROUP_SKILL_NUM];		//天赋ID
};

struct PetInbornLevelCfg
{
	PetInbornLevelCfg() : sp_id(0), inborn_score(0)
	{}

	int sp_id;
	int inborn_score;
	std::vector<ItemConfigData> consume_list;
};

class PetInbornConfig : public ILogicConfig
{
public:
	PetInbornConfig();
	virtual ~PetInbornConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const PetInbornOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const PetInbornLevelCfg * GetLevelCfg(int inborn_id, int inborn_level);
	void GetLevelConsumeByReset(int inborn_id, int inborn_level, std::vector<ItemConfigData> & return_list) const;
	const PetInbornBaseCfg * GetBaseCfg(int inborn_type, int inborn_group_type);

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitPetInbornBaseCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitPetInbornLevelCfg(TiXmlElement * RootElement, std::string& errormsg);

	
	PetInbornOtherCfg m_other_cfg;
	std::map<int, std::map<int, PetInbornLevelCfg> > m_level_cfg;	//天赋等级配置  key1: inborn_id key2: inborn_level
	std::map<int, PetInbornBaseCfg> m_inborn_cfg;					//天赋基础配置  key:inborn_type * 1000 + inborn_group_type
};


#endif