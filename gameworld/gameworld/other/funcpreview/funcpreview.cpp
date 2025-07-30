#include "other/funcpreview/funcpreview.hpp"
#include "protocol/msgother.h"
#include "config/logicconfigmanager.hpp"
#include "other/funcpreview/funcpreviewconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "item/knapsack.h"

FuncPreview::FuncPreview() : m_mgr(NULL)
{

}

FuncPreview::~FuncPreview()
{

}

void FuncPreview::Init(RoleModuleManager* mgr, const FuncPreviewParam& param)
{
	m_mgr = mgr;
	m_param = param;
}

void FuncPreview::GetInitParam(ARG_OUT FuncPreviewParam* param)
{
	*param = m_param;
}

void FuncPreview::ClaimReward(int notice_id)
{
	const FuncPreviewCfg* cfg = LOGIC_CONFIG->GetFuncPreviewConfig()->GetFuncPreviewCfg(notice_id);
	if (NULL == cfg)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int lv = m_mgr->GetRole()->GetLevel();
	if (lv < cfg->end_level)
	{
		m_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	if (FUNC_PREVIEW_ALREADY_CLAIMED == m_param.reward_info_list[notice_id].status)
	{
		this->SendInfo();
		m_mgr->NoticeNum(errornum::EN_FUNC_PREVIEW_ALREADY_CLAIMED);
		return;
	}

	if (!m_mgr->GetKnapsack()->CheckForPutList(cfg->reward_num, cfg->reward_list))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_mgr->GetKnapsack()->PutList(cfg->reward_num, cfg->reward_list, PUT_REASON_FUNC_PREVIEW))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.reward_info_list[notice_id].status = FUNC_PREVIEW_ALREADY_CLAIMED;
	this->SendInfo();
}

void FuncPreview::OnLogin()
{
	this->CheckClaimable(true);
}

void FuncPreview::OnLevelUp()
{
	this->CheckClaimable(true);
}

void FuncPreview::SendInfo()
{
	int max_num = LOGIC_CONFIG->GetFuncPreviewConfig()->GetFuncPreviewCfgNum();

	Protocol::SCFuncPreviewInfo info;
#ifdef _DEBUG
	assert(max_num <= ARRAY_ITEM_COUNT(info.notice_id_list));
	assert(max_num <= ARRAY_ITEM_COUNT(m_param.reward_info_list));
#endif
	
	info.notice_num = max_num;
	for (int i = 0; i < max_num && i < ARRAY_ITEM_COUNT(info.notice_id_list) && i < ARRAY_ITEM_COUNT(m_param.reward_info_list); ++i)
	{
		info.notice_id_list[i] = m_param.reward_info_list[i].status;
	}
	
	m_mgr->NetSend(&info, sizeof(info));
}

void FuncPreview::CheckClaimable(bool is_notice_client)
{
	int lv = m_mgr->GetRole()->GetLevel();

	// 重新遍历所有未领取的奖励，看哪些能领取的
	int max_num = LOGIC_CONFIG->GetFuncPreviewConfig()->GetFuncPreviewCfgNum();
	for (int notice_id = 0; notice_id < max_num; ++notice_id)
	{
		const FuncPreviewCfg* cfg = LOGIC_CONFIG->GetFuncPreviewConfig()->GetFuncPreviewCfg(notice_id);
		if (NULL == cfg) continue;

		if (m_param.reward_info_list[notice_id].version != cfg->version)
		{
			m_param.reward_info_list[notice_id].version = cfg->version;
			m_param.reward_info_list[notice_id].status = FUNC_PREVIEW_CAN_NOT_CLAIM;
		}

		if (FUNC_PREVIEW_CAN_NOT_CLAIM == m_param.reward_info_list[notice_id].status)
		{
			if (lv >= cfg->end_level) // 如果不可领取，且等级已达到，则设为可领取
			{
				m_param.reward_info_list[notice_id].status = FUNC_PREVIEW_OK_TO_CLAIM;
			}
		}
	}

	if (is_notice_client)
	{
		this->SendInfo();
	}
}

