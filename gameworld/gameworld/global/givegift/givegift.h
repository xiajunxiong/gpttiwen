#ifndef _GIVE_GIFT_HPP_
#define _GIVE_GIFT_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/commonsavedef.h"

#include <map>
#include <vector>
#include <list>
#include <set>

class Role;
struct CommonSaveGiveGiftData;
class CommonSave;
class GiveGift
{
public:
	static GiveGift & Instance();

	void OnDayChange();
	void OnUserLogin(Role *role);
	void OnUserLoginOriginGame(int role_id);	

	bool AddGiveGift(const CommonSaveGiveGiftData & data);
	int FetchItem(int role_id, const long long & give_gift_id_ll, bool call_from_game = false);
	void SendGiftInfo(int role_id);
	void SendGiftInfoUpdate(int role_id, const CommonSaveGiveGiftData & data);
	int SetFetchAck(int role_id, long long give_gift_id_ll, bool is_succ);

	int GmFetchItem(int role_id, bool call_from_game = false);
private:
	GiveGift();
	virtual ~GiveGift();
private:
	void ReturnItem();    //每天返回赠送没有领取的物品

	CommonSave * GetCommonSaveGiveGift() const;
	CommonSaveGiveGiftData * GetCommonSaveGiveGiftData(int role_id, int give_gift_id1, int give_gift_id2, bool is_auto_create = false);
	CommonSaveGiveGiftData * GetCommonSaveGiveGiftDataLL(int role_id, long long give_gift_id, bool is_auto_create = false);
};

#endif

