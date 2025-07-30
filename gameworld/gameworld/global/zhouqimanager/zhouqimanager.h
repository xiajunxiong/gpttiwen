#ifndef __ZHOU_QI_MANAGER_HPP__
#define __ZHOU_QI_MANAGER_HPP__

#include "servercommon/zhouqidef.hpp"

#include <map>
#include <vector>
#include <set>

class Activity;
class Role;

class ZhouQiManager
{
public:
	static ZhouQiManager & Instance();

	void OnServerStart();
	void Update(unsigned long interval, time_t now_second);
	void OnDayChange();
	void OnUserLogin(Role * role);

	int GetZhouQiNum();
	void GetZhouQiInfo(int * ARG_OUT zhou_qi_num, unsigned int * ARG_OUT end_time = NULL, unsigned int * ARG_OUT begin_time = NULL);
	bool IsInZhouQi(int play_type);
	bool IsDayGameOpen(int play_type);

	void SendZhouQiInfo(Role * role = NULL);
private:
	ZhouQiManager();
	~ZhouQiManager();
private:
	bool CalZhouQiTimeInfo();


	int m_zhou_qi_num;
	time_t m_begin_time;
	time_t m_end_time;
};

#endif

