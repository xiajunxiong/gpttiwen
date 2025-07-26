
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/userprotocal/loginmsgcode.h"
#include "servercommon/userprotocal/systemmsgcode.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "msglogin.h"
#include "msgregister.h"

namespace Protocol
{
	CSLoginToAccount::CSLoginToAccount():header(MT_LOGIN_TO_ACCOUNT_CS){}
	SCLoginToAccount::SCLoginToAccount():header(MT_LOGIN_TO_ACCOUNT_SC){}
	CSLoginToRole::CSLoginToRole():header(MT_LOGIN_TO_ROLE_CS){}
	SCLoginToRole::SCLoginToRole():header(MT_LOGIN_TO_ROLE_SC){}
	CSCreateRole::CSCreateRole() : header(MT_CREATE_ROLE_REQ_CS) {}
	SCCreateRoleAck::SCCreateRoleAck() : header(MT_CREATE_ROLE_ACK_SC) {}
	CSDestroyRole::CSDestroyRole() : header(MT_DESTROY_ROLE_REQ_CS) {}
	SCDestroyRoleAck::SCDestroyRoleAck() : header(MT_DESTROY_ROLE_ACK_SC) {}
	CSRandomNameReq::CSRandomNameReq() : header(MT_GET_RANDOM_NAME_CS) {}
	SCRandomNameRet::SCRandomNameRet() : header(MT_GET_RANDOM_NAME_SC) {}

	CSGetThreadInfo::CSGetThreadInfo() : header(MT_GET_TREAD_CS) {}
	SCGetThreadAck::SCGetThreadAck():header(MT_GET_THREAD_ACK_SC){}
	CSPassAntiWallow::CSPassAntiWallow():header(MT_ANTI_WALLOW_CS){}
	CSPassAntiWallowEx::CSPassAntiWallowEx():header(MT_ANTI_WALLOW_EX_CS){}
	SCAntiWallowRet::SCAntiWallowRet():header(MT_ANTI_WALLOW_EX_RET_SC){}
	SCAccountKeyError::SCAccountKeyError():header(MT_ACCOUNT_KEY_ERROR_SC){}
	SCProfNumInfo::SCProfNumInfo() : header(MT_PROF_NUM_INFO_SC) {}
	
	CSChangePlatName::CSChangePlatName():header(MT_CHANGE_PLATNAME_REQ_CS) {}
	SCChangePlatNameAck::SCChangePlatNameAck():header(MT_CHANGE_PLATNAME_ACK_SC) {}

	CSLHeartBeat::CSLHeartBeat():header(MT_L_HEARTBEAT_CS){};
	SCLHeartBeat::SCLHeartBeat() : header(MT_L_HEARTBEAT_SC) {}

	CSStoryBattleInfo::CSStoryBattleInfo() : header(MT_STORY_BATTLE_INFO_CS) {}
	CSStoryRoundResult::CSStoryRoundResult() : header(MT_STORY_ROUND_RESULT_CS) {}
	SCStoryNotAvaliable::SCStoryNotAvaliable() : header(MT_STORY_NOT_AVALIABLE_SC) {}

}

