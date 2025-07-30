#ifndef __ROLE_GIVE_GIFT_HPP__
#define __ROLE_GIVE_GIFT_HPP__

#include <string>

struct RoleCommonSaveRoleGiveGiftData;
class RoleModuleManager;
class RoleGiveGift
{
public:
	RoleGiveGift();
	~RoleGiveGift();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	int GiveItem(int to_role_id, int item_id, int num, char(&str)[COMMON_SAVE_GIVE_GIFT_STR_NUM]);
	int GiveFlower(int _to_role_id, int _item_id, int _num, bool _is_auto_buy , char(&_str)[COMMON_SAVE_GIVE_GIFT_STR_NUM]);
	void SendRoleInfo();
private:
	RoleCommonSaveRoleGiveGiftData * GetRoleCommonSaveRoleGiveGift(int to_role_id, int times_group_id);

	bool CanGiveFlower(int _to_role_id, int _item_id, int _num , bool _is_auto_buy);
	void GiveFlowerHelp(int _to_role_id, int _item_id, int _num, char(&_str)[COMMON_SAVE_GIVE_GIFT_STR_NUM]);
	bool ConsumeSendFlowerItem(int _item_id, int _num, bool _is_auto_buy);
	void GiveFlowerChange(int _to_role_id);


	RoleModuleManager * m_module_mgr;
};

#endif