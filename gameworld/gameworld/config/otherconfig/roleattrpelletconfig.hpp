#ifndef __ROLE_ATTR_PELLET_CONFIG_HPP__
#define __ROLE_ATTR_PELLET_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <set>

struct RoleAttrPelletOtherCfg
{
	RoleAttrPelletOtherCfg() : use_limit(0)
	{}

	int use_limit;
};

struct RoleAttrPelletAttrListCfg
{
	RoleAttrPelletAttrListCfg() : attr_pellet_id(0), role_attr_pellet_type(0), type_param(0), attr_type(0), attr_value(0)
	{}

	int attr_pellet_id;
	int role_attr_pellet_type;
	int type_param;
	int attr_type;
	int attr_value; 
};

union AttrPelletKey
{
	int param[2];
	long long unique_key;
};
static const int MAX_ROLE_ATTR_PELLET_TYPE_BASE_NUM = 10000;
static const int MAX_ROLE_TYPE_PARAM_BASE_NUM = 100;


class RoleAttrPelletConfig : public ILogicConfig
{
public:
	RoleAttrPelletConfig();
	virtual ~RoleAttrPelletConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const RoleAttrPelletOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const RoleAttrPelletAttrListCfg * GetPelletListCfg(int attr_pellet_id, int type_param);
	const RoleAttrPelletAttrListCfg * GetAttrPelletCfg(int role_attr_pellet_type, int type_param, int attr_type);

	const std::set<int> * GetRolePelletItemCfg(int gift_item_id);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitAttrListCfg(TiXmlElement *RootElement);
	int InitRoleItemCfg(TiXmlElement *RootElement);

	long long GetAttrPelletKey(int attr_pellet_id, int type_param);
	void GetAttrPelletParam(long long unique_key, int *attr_pellet_id, int * type_param);
	int GetAttrPelleTypeKey(int role_attr_pellet_type, int type_param, int attr_type);
	

	RoleAttrPelletOtherCfg m_other_cfg;
	std::map<long long, RoleAttrPelletAttrListCfg> m_attr_list_map;		// 属性丹属性列表 key:GetAttrPelletKey
	std::map<int, RoleAttrPelletAttrListCfg> m_attr_type_map;			// key:GetAttrPelleTypeKey
	std::set<int> m_attr_list_set;												
	std::map<int, std::set<int> >  m_role_item_cfg;		//角色属性丹随机
};


#endif