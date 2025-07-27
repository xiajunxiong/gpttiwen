#ifndef __LOCAL_CONFIG_H__
#define __LOCAL_CONFIG_H__

#include <set>

#include "servercommon/servercommon.h"

union PlatTypeUnion
{
	int id;
	char spid[4];
};

class LocalConfig
{
public:
	static LocalConfig & Instance();
	static void GetPlatName(int plat_type, PlatName plat_name);

	bool Init(const std::string &path, std::string *err);

	inline int GetPlatType() { return m_plat_type; }
	int GetRActZhuanfuType() { return m_rand_act_zhuanfu_type; }
	const char * GetPlatName() { return m_plat_name; }
	void GetPlatName(PlatName plat_name);

	bool IsServerIdAllowed(int server_id);
	bool IsMergedServer();

	bool IsAllowedServerId(int server_id);

	int GetMergeServerId();
	inline const std::set<int> & GetAllowServerIdSet() const { return m_allow_server_id_set; }
	bool IsBelongServer(int role_id);

	bool IsIOSPlatForm() { return m_is_ios_platform; }
	bool IsTaiWanPlatform() { return m_is_taiwan_platform; }

	static const char* GetPlatNameBySpid(int spid);
	const UniqueServerID& GetUniqueServerID() const;

private:
	LocalConfig();
	~LocalConfig();

	static const int MAX_ALLOW_SERVER_ID_COUNT = 512;
	
	PlatName m_plat_name;							// ƽ̨��
	int m_plat_type;								// ƽ̨����
	int m_rand_act_zhuanfu_type;					// ר�����ͣ����Ǿ���ר����ֻ�����������������ã�
	bool m_is_check_server_id;						// �Ƿ����ID
	std::set<int> m_allow_server_id_set;			// ����ķ�ID

	bool m_is_ios_platform;							// �Ƿ���ƻ��ƽ̨
	bool m_is_taiwan_platform;						// �Ƿ�̨��ƽ̨

	int m_mother_server_id;
	UniqueServerID m_unique_sid;
};

#endif	// __LOCAL_CONFIG_H__

