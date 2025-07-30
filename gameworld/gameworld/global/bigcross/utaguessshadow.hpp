#ifndef  __UTA_GUESS_SHADOW_HPP__
#define  __UTA_GUESS_SHADOW_HPP__

#include "servercommon/userprotocal/msgutaguess.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

class Role;

class UTAGuessShadow
{
public:
	static UTAGuessShadow & Instance();

	void GiveLotteryToRoleFromHidden(crossgameprotocal::HiddenGameGiveLotteryToRole * _msg);

	void OnClientReq(Role * _role, Protocol::CSUTAGuessClienReq * _req);

	void GiveLotteryToRole(int _uid, int _num, int _in_reason_type);

private:
	UTAGuessShadow();
	~UTAGuessShadow();

	void ClientReqToBigCross(Role * _role, Protocol::CSUTAGuessClienReq * _req);
	bool CanBet(Role * _role, Protocol::CSUTAGuessClienReq * _req);

};



#endif	// __UTA_GUESS_SHADOW_HPP__
