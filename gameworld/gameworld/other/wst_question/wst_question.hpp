#ifndef __WST_QUESTION_HPP__
#define __WST_QUESTION_HPP__
#include "other/attributelist.hpp"
#include "servercommon/struct/moneyotherdef.h"

class RoleModuleManager;
class WstQuestion
{
public:
	WstQuestion();
	~WstQuestion();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager );
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	bool OnWstQuestionFindReward(int consume_type, int count);
	void OnFindDataReset();
	void OnBuyExp(int buy_type);
	void OnQuestion(int answer_id,bool _is_onekey_finish = false);

	void OneKeyFinishWstQuation();		//Ò»¼üÍê³É

	void SendAllInfo();
	void SendWstInfo();
private:
	RoleModuleManager *m_role_module_mgr;
};


#endif // !__WST_QUESTION_HPP__
