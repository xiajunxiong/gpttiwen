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

	//禁止拷贝复制（请不要公有化以下这两个函数， 除非做好深拷贝， 因为在析构时会delete掉某些成员变量）
	OriginBusinessRegister(const OriginBusinessRegister&);
	OriginBusinessRegister& operator=(const OriginBusinessRegister &);

	OriginBusinessInfoMap m_role_businiess_map;
};

#endif