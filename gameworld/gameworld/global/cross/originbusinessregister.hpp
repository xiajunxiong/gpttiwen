#ifndef __ORIGIN_BUSINESS_REGISTER_HPP__
#define __ORIGIN_BUSINESS_REGISTER_HPP__

#include "servercommon/crossdef.hpp"

struct OriginBusinessInfo
{
	int role_id;
	unsigned int register_timestamp;
	ReturnOriginData_Business business;
};

class OriginBusinessRegister
{
public:
	typedef std::map<int, OriginBusinessInfo> OriginBusinessInfoMap;

public:
	static OriginBusinessRegister& Instance();

	void Update(unsigned long interval, unsigned int now);
	void OnUserLogin(const UserID& user_id);

	void RegisterBusiness(int role_id, const ReturnOriginData_Business& business);
private:
	OriginBusinessRegister();
	~OriginBusinessRegister();

	//��ֹ�������ƣ��벻Ҫ���л����������������� ������������� ��Ϊ������ʱ��delete��ĳЩ��Ա������
	OriginBusinessRegister(const OriginBusinessRegister&);
	OriginBusinessRegister& operator=(const OriginBusinessRegister &);

	OriginBusinessInfoMap m_role_businiess_map;
};

#endif