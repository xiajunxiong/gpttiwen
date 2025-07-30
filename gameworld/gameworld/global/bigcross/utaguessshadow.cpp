#include "utaguessshadow.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/bigcrossconfig/utaguessconfig.hpp"
#include "servercommon/internalprotocal/bigcrosshgameprotocal.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/string/gameworldstr.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "config/logicconfigmanager.hpp"
#include "global/bigcross/bigcrossmanager.hpp"
#include "gamelog.h"
#include "internalcomm.h"
#include "servercommon/errornum.h"
#include "other/route/mailroute.hpp"
#include "global/cross/crossmanager.h"

UTAGuessShadow & UTAGuessShadow::Instance()
{
	static UTAGuessShadow st_instance;
	return st_instance;
}

void UTAGuessShadow::GiveLotteryToRoleFromHidden(crossgameprotocal::HiddenGameGiveLotteryToRole * _msg)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (NULL == _msg)
		return;

	const UTAGuessOtherCfg & cfg = LOGIC_CONFIG->GetUTAGuessConfig()->GetOtherCfg();

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(_msg->uid);
	if (NULL != role)
	{
		int reason_type = _msg->reason_type;
		switch (reason_type)
		{
		case bigchgameprotocol::GIVE_LOTTERY_REASON_TYPE_BET_FAILD:
			{
				reason_type = PUT_REASON_GUESS_BET_FAILD_RETURN;
			}
			break;
		case bigchgameprotocol::GIVE_LOTTERY_REASON_TYPE_GET_GUESS_REWARD:
			{
				reason_type = PUT_REASON_GUESS_BET_REWARD;
			}
			break;
		}

		ItemConfigData  item_data;
		item_data.item_id = cfg.lottery_id;
		item_data.num = _msg->num;
		item_data.is_bind = true;

		bool put_ret = false;
		put_ret = role->GetRoleModuleManager()->GetKnapsack()->Put(item_data, reason_type);
		bool mail_ret = false;
		if (!put_ret)
		{
			mail_ret = role->GetRoleModuleManager()->GetKnapsack()->SendMail(&item_data, 1);
		}

		gamelog::g_log_other.printf(LL_INFO, "%s  put_ret:%d  mail_ret:%d  user[%d] in_reason_type:%d  item_id:%d num:%d ",
			__FUNCTION__, put_ret, mail_ret , _msg->uid, _msg->reason_type, cfg.lottery_id, _msg->num);
	}
	else
	{
		static MailContentParam contentparam;
		contentparam.Reset();
		int lensub = 0, length = 0;
		// 如果也不在主服 , 就发邮件
		int reason_type = _msg->reason_type;
		switch (reason_type)
		{
		case bigchgameprotocol::GIVE_LOTTERY_REASON_TYPE_BET_FAILD:
			{
				lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_uta_guess_reward_bet_retroactiv_subject);
				length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_uta_guess_reward_bet_retroactiv_content);
			}
			break;
		case bigchgameprotocol::GIVE_LOTTERY_REASON_TYPE_GET_GUESS_REWARD:
			{
				lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_uta_guess_reward_retroactiv_subject);
				length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_uta_guess_reward_retroactiv_content);
			}
			break;
		}

		if (lensub > 0 && length > 0)
		{
			contentparam.item_list[0].item_id = _msg->lottery_id;
			contentparam.item_list[0].num = _msg->num;
			contentparam.item_list[0].is_bind = 1;

			MailRoute::Instance().MailToUser(IntToUid(_msg->uid), MAIL_REASON_DEFAULT, contentparam);

			// 记录一下发送的日志
			gamelog::g_log_other.printf(LL_INFO ,"UTAGuessShadow::GiveLotteryToRoleFromHidden  . "
				"send mail . in_reason:%d  uid:%d  item_id:%d  item_num:%d",
				_msg->reason_type , _msg->uid , _msg->lottery_id , _msg->num);
		}
	}
}

void UTAGuessShadow::OnClientReq(Role * _role, Protocol::CSUTAGuessClienReq * _req)
{
	if (NULL == _role || NULL == _req)
		return;

	// 下注需要先判断一下自身的货币够不够 , 其他的去大跨服再做处理
	switch (_req->req_type)
	{
	case Protocol::UTA_GUESS_CLIENT_REQ_TYPE_BET:
		{
			if (!this->CanBet(_role, _req))
			{
				return;
			}
		}
		break;
	}

	switch (_req->req_type)
	{
	case Protocol::UTA_GUESS_CLIENT_REQ_TYPE_GET_INFO:
	case Protocol::UTA_GUESS_CLIENT_REQ_TYPE_BET:
	case Protocol::UTA_GUESS_CLIENT_REQ_TYPE_GET_RECORD:
	case Protocol::UTA_GUESS_CLIENT_REQ_TYPE_GET_BET_REWARD:
	case Protocol::UTA_GUESS_CLIENT_REQ_TYPE_GET_HISTORY_EARNING:
		{
			this->ClientReqToBigCross(_role, _req);
		}
		break;
	}
}

void UTAGuessShadow::GiveLotteryToRole(int _uid, int _num, int _in_reason_type)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	const UTAGuessOtherCfg & cfg = LOGIC_CONFIG->GetUTAGuessConfig()->GetOtherCfg();
	// 从bigcross到隐藏服 , 如果玩家当前在隐藏服就直接发给玩家
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(_uid);
	if (NULL != role)
	{
		int reason_type = _in_reason_type;
		switch (reason_type)
		{
		case bigchgameprotocol::GIVE_LOTTERY_REASON_TYPE_BET_FAILD:
			{
				reason_type = PUT_REASON_GUESS_BET_FAILD_RETURN;
			}
			break;
		case bigchgameprotocol::GIVE_LOTTERY_REASON_TYPE_GET_GUESS_REWARD:
			{
				reason_type = PUT_REASON_GUESS_BET_REWARD;
			}
			break;
		}

		ItemConfigData  item_data;
		item_data.item_id = cfg.lottery_id;
		item_data.num = _num;
		item_data.is_bind = true;


		bool put_ret = false;
		put_ret = role->GetRoleModuleManager()->GetKnapsack()->Put(item_data, reason_type);
		bool mail_ret = false;
		if (!put_ret)
		{
			mail_ret = role->GetRoleModuleManager()->GetKnapsack()->SendMail(&item_data, 1);
		}

		gamelog::g_log_other.printf(LL_INFO, "%s  put_ret:%d  mail_ret:%d  user[%d] in_reason_type:%d  item_id:%d num:%d ",
			__FUNCTION__, put_ret, mail_ret, _uid, _in_reason_type, cfg.lottery_id, _num);

	}
	else
	{
		// 如果玩家当前不在隐藏服 , 就转发到主服去发给玩家
		crossgameprotocal::HiddenGameGiveLotteryToRole hg_lottery;
		hg_lottery.uid = _uid;
		hg_lottery.lottery_id = cfg.lottery_id;
		hg_lottery.num = _num;
		hg_lottery.reason_type = _in_reason_type;
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(_uid).db_index, (char *)&hg_lottery, sizeof(hg_lottery));
	}
}

UTAGuessShadow::UTAGuessShadow()
{

}

UTAGuessShadow::~UTAGuessShadow()
{

}

void UTAGuessShadow::ClientReqToBigCross(Role * _role, Protocol::CSUTAGuessClienReq * _req)
{
	if (NULL == _role || NULL == _req)
		return;

	bigchgameprotocol::HGameBigCUTAGuessReq  hb_req;
	hb_req.uuid = _role->GetUniqueUserID();
	hb_req.unique_hidden_server_id = UniqueHiddenServerID(LocalConfig::Instance().GetPlatType(), CrossManager::Instance().GetHiddenServerIDX());

	hb_req.req_type = _req->req_type;
	hb_req.param_1 = _req->param_1;
	hb_req.param_2 = _req->param_2;
	hb_req.param_3 = _req->param_3;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!BigCrossManager::Instance().IsBigCrossServerConnected())
		{
			_role->NoticeNum(errornum::EN_BIGCROSS_SERVER_LOST);
			return;
		}

		InternalComm::Instance().SendToBigCrossServer((const char*)&hb_req, sizeof(hb_req));
	}
	else
	{
		InternalComm::Instance().SendToHiddenToBigCross((const char*)&hb_req, sizeof(hb_req));
	}
}

bool UTAGuessShadow::CanBet(Role * _role, Protocol::CSUTAGuessClienReq * _req)
{
	if (NULL == _role || NULL == _req)
		return false;
	const UTAGuessOtherCfg & cfg = LOGIC_CONFIG->GetUTAGuessConfig()->GetOtherCfg();

	if (_req->param_3 <= 0)
	{
		_role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (_req->param_3 > LOGIC_CONFIG->GetUTAGuessConfig()->GetOtherCfg().once_lottery_limit)
	{
		_req->param_3 = LOGIC_CONFIG->GetUTAGuessConfig()->GetOtherCfg().once_lottery_limit;
		_role->NoticeNum(errornum::EN_UTA_ONCE_LOTTERY_LIMIT);
	}

	if (!_role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg.lottery_id, _req->param_3, __FUNCTION__))
	{
		_role->NoticeNum(errornum::EN_LOTTERY_NUM_NOT_ENOUGH);
		return false;
	}

	return true;
}
