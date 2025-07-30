#include "funopen.hpp"
#include "obj/character/role.h"
#include "funopendef.hpp"
#include "config/logicconfigmanager.hpp"
#include "funopenconfig.hpp"
#include "servercommon/errornum.h"
#include "task/taskrecorder.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "global/zhouqimanager/zhouqiconfig.hpp"

bool FunOpen::IsFunOpen(Role* role, int fun_type, bool is_notice /* = false */)
{
	if (fun_type == FUN_TYPE_ZHOU_QI)
	{
		time_t now = EngineAdapter::Instance().Time();

		const ZhouQiOpenLimitCfg * open_limit_cfg = LOGIC_CONFIG->GetZhouQiConfig()->GetZhouQiOpenLimitCfg(now);
		if (!open_limit_cfg)
		{
			return false;
		}

		if (role->GetLevel() < open_limit_cfg->need_role_level)
		{
			if (is_notice)
			{
				role->NoticeNum(errornum::EN_FUN_OPEN_ROLE_LEVEL_LIMIT);
			}			
			return false;
		}

		return true;
	}

	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(fun_type);
	if (NULL == cfg) return false;

	if (role->GetLevel() < cfg->limit_level)
	{
		if (is_notice)
		{
			role->NoticeNum(errornum::EN_FUN_OPEN_ROLE_LEVEL_LIMIT);
		}
		return false;
	}
	int role_create_day = role->GetRoleModuleManager()->GetCreateDays();
	if (cfg->limit_open_day > role_create_day)
	{
		if (is_notice)
		{
			role->NoticeNum(errornum::EN_FUNOPEN_SERVER_OPEN_ERR);
		}
		return false;
	}

	if (cfg->limit_task > 0 && !role->GetRoleModuleManager()->GetTaskRecorder()->HaveFinished(cfg->limit_task))
	{
		if (is_notice)
		{
#ifdef _DEBUG
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
				gamestring::g_funopen_task_limit, fun_type, cfg->limit_task);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUID(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}	
#else
			role->NoticeNum(errornum::EN_FUN_OPEN_ROLE_TASK_LIMIT);
#endif
		}
		return false;
	}

	return true;
}

int FunOpen::GetFunLimitLevel(int fun_type)
{
	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(fun_type);
	if (NULL == cfg) return 0;

	return cfg->limit_level;
}

