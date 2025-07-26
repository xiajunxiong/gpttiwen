#ifndef RAND_NAME_MANAGER_HPP
#define RAND_NAME_MANAGER_HPP

#include "irmimodule.h"
#include "servercommon/serverconfig/nameconfig.h"
#include "loginserver/server/loginlog.h"

#include <map>
#include <set>

typedef std::set<long long> NameStrIdList;

enum EXIST_TYPE
{
	VALUE_ERROR = 0,		// ����ֵ����
	NOT_EXIST = 1,			// ������
	EXIST = 2,				// ����
};

struct NameStringIdParam
{
	NameStringIdParam() : front_sex(0), front_id(0), middle_sex(0), middle_id(0), last_sex(0),
		last_id(0), name_str_id(0) {}

	short front_sex;
	short front_id;
	short middle_sex;
	short middle_id;
	short last_sex;
	short last_id;
	long long name_str_id;
};

class RandNameManager
{
public:
	static RandNameManager &Instance()
	{
		static RandNameManager st_rand_name;
		return st_rand_name;
	}

	void Init(NameStrIdList name_str_id_list);			// ��ʼ�����ֻ���
	int StrIdExist(long long find_str_id/*, int* index*/);	// ����Ƿ����
	bool AddNameStrId(long long add_str_id);			// ���
	bool DeleteNameStrId(long long del_str_id);			// ɾ��
	void OnGetRandNameId(NetID netid,int sex);
	
private:
	RandNameManager():m_next_interval(0){};
	~RandNameManager(){};

	long long GetRandNameId(int sex,NameStringIdParam* strid_param);					// �������������ӳ��id,severdef �Ա�enum
	unsigned int m_next_interval;

	NameStrIdList m_name_str_id_list;
};

#endif