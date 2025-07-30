#ifndef __RAND_ACTIVITY_COMPENSATION_CONFIG_HPP__
#define __RAND_ACTIVITY_COMPENSATION_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <string>

struct RACompensationOtherCfg
{
	RACompensationOtherCfg() : reset_flag(0), no_compensate_day(0) {}

	int reset_flag;
	int no_compensate_day;
};

struct RACompensationServerCfg
{
	RACompensationServerCfg() : server_id(0), open_day(0), item_group(0) {}

	std::string spid_name;
	int server_id;
	int open_day;
	int item_group;	
};

struct RACompensationSeqCfg
{
	std::vector<ItemConfigData> item_list;
};

struct RACompensationItemGroupCfg
{
	static const int MAX_ITEM_NUM = 5;

	RACompensationItemGroupCfg() : item_group(0) {}

	const RACompensationSeqCfg* GetSeqCfg(int seq) const;

	int item_group;
	std::vector<RACompensationSeqCfg> seq_list;
};

struct RACompensationUser
{
	bool operator < (const RACompensationUser& other) const
	{
		if (role_id != other.role_id)
		{
			return role_id < other.role_id;
		}
		else
		{
			return role_name < other.role_name;
		}
	}

	int role_id;
	std::string role_name;
};

class RACompensationConfig : public ILogicConfig
{
public:
	typedef std::vector<RACompensationServerCfg> ServerCfgVec;
	typedef std::vector<RACompensationItemGroupCfg> ItemGroupCfgVec;
	typedef std::vector<int> SeqList;
	typedef std::map<RACompensationUser, SeqList> UserSeqListMap;
	typedef std::map<int, SeqList> UserSeqListMapOnErrorLog;
public:
	RACompensationConfig();
	virtual ~RACompensationConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const RACompensationServerCfg* GetServerCfg(const std::string& spid_name, int server_id);
	const RACompensationItemGroupCfg* GetItemGroupCfg(int item_group);
	const SeqList* GetUserSeqList(int role_id, const std::string& role_name);
	const SeqList* GetUserSeqListOnErrorLog(int role_id);	

	const RACompensationOtherCfg& GetOtherCfg() const { return m_other_cfg; }
	bool IsNoCompensate(const std::string& spid_name, int server_id);
	const RACompensationItemGroupCfg* GetItemGroupCfgByServerId(const std::string& spid_name, int server_id);
private:
	int InitOtherCfg(TiXmlElement* RootElement);
	int InitServerCfg(TiXmlElement* RootElement);
	int InitItemGroupCfg(TiXmlElement* RootElement);
	int InitUserListCfg(TiXmlElement* RootElement);
	RACompensationItemGroupCfg* MutableItemGroupCfg(int item_group);
	SeqList* MutableUserSeqList(int role_id, const std::string& role_name);
	SeqList* MutableUserSeqListOnErrorLog(int role_id);

	RACompensationOtherCfg m_other_cfg;
	ServerCfgVec m_server_cfg_list;
	ItemGroupCfgVec m_item_group_cfg_list;
	UserSeqListMap m_user_seq_list_map;
	UserSeqListMapOnErrorLog m_user_seq_list_map_on_error_log;
};

#endif