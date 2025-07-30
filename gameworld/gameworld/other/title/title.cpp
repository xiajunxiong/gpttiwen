#include "title.hpp"
#include "titleconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "gamelog.h"
#include "gameworld/engineadapter.h"
#include "protocol/msgtitle.h"
#include "config/activityconfig/chiefelection/activitychiefelectionconfig.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "obj/character/role.h"
#include "config/otherconfig/professionconfig.hpp"
#include "config/activityconfig/worldteamarena/activityworldteamarenaconfig.hpp"
#include "config/activityconfig/chiefpeakbattle/activitychiefpeakbattlenconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "scene/aoi/zonematrix.h"
#include "scene/scene.h"
#include "global/guild/guild.hpp"
#include "config/otherconfig/colosseumconfig.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "other/facescore/facescore.hpp"
#include "global/activity/activityimpl/activityonlyfight.hpp"
#include "global/activity/activitymanager.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/activity/activityimpl/activitybraveground.hpp"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"
#include "other/duel/duel.hpp"
#include "servercommon/errornum.h"
#include "other/advancecareer/advancecareer.hpp"
#include "global/qingyuan/qingyuanmanager.h"
#include "config/otherconfig/guildconfig.hpp"

Title::Title()
{
	m_is_auto_wear = true;
	m_title_count = 0;
}

Title::~Title()
{
}

void Title::Init(RoleModuleManager * role_module_manager, const TitleParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;
	m_param.wear_index = -1;
	for (int i = 0; i < TITLE_MAX_NUM; ++i)
	{
		if (m_param.title_list[i].title_id > 0)
		{
			++m_title_count;
		}
	}
}

void Title::GetOtherInitParam(TitleParam * param)
{
	param->Reset();	// 如果数据获取不是单纯地全数据覆盖，则必须要重置数据，因为传入的容器会有其它玩家的数据残留；
	param->wear_index = m_param.wear_index;
	param->wear_title_id = this->GetWearTitle();
	param->is_first_create_role = m_param.is_first_create_role;
	param->is_first_syn_title_to_facescore = m_param.is_first_syn_title_to_facescore;
	param->reserve_short = m_param.reserve_short;

	int saved_count = 0;
	for (int i = 0; i < TITLE_MAX_NUM; ++i)
	{
		TitleData & title_data = m_param.title_list[i];
		if (title_data.title_id <= 0) continue;

		const TitleCfg * title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(title_data.title_id);
		if (NULL != title_cfg && TitleConfig::IsNeedSaveTitleType(title_cfg->source_type))
		{
			param->title_list[saved_count++] = title_data;
		}
	}
}

void Title::OnRoleLogin()
{
	this->SetAutoWear(false);
	//特殊来源称号都是没有保存到数据库的,所以此时删除颜值系统即可
	this->CalChiefElectionTitle(m_role_module_mgr->GetRole()->GetBaseProfession(), TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalPeakBattleTitle(0, TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalColosseumTitle(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalOnlyFightTitle(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalGuildFightTitle(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalBraveGroundTitle(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalHuanJieZhanChangeTitle(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalGuildHonorTitle2(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalQingYuanTitle(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalWorldTeamArenaTitle(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalProfessionTitle(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->CalGuildMemberTitle(TITLE_NOTICE_TYPE_NO_NOTICE);
	this->SetAutoWear(true);

	// 玩家创建角色给予职业称号
	if (m_param.is_first_create_role == 0)
	{
		this->CreateRoleAddTitle();
	}
	//同步玩家当前称号到颜值资源
	if (m_param.is_first_syn_title_to_facescore == 0)
	{
		for (int i = 0; i < TITLE_MAX_NUM; ++i)
		{
			if (m_param.title_list[i].title_id > 0)
			{
				m_role_module_mgr->GetFaceScore()->OnGainTitle(m_param.title_list[i].title_id);
			}
		}
		m_param.is_first_syn_title_to_facescore = 1;
	}
	if (this->HasTitle(m_param.wear_title_id))
	{		
		if (this->GetWearTitle() != m_param.wear_title_id)
		{
			this->OnWearTitleById(m_param.wear_title_id);  //自动带上次离线时称号
		}		
	}
	else
	{
		m_param.wear_title_id = 0;
		m_param.wear_index = -1;
	}
}

const AttributeList & Title::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		
		for (int i = 0; i < m_title_count && i < TITLE_MAX_NUM; ++i)
		{
			TitleData title_data = m_param.title_list[i];
			const TitleCfg *title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(title_data.title_id);
			if (NULL == title_cfg) continue;
			int attr_index = 0;
			for (attr_index = BATTLE_ATTR_NORMAL_VALUE_BEGIN; attr_index < BATTLE_ATTR_NORMAL_VALUE_END; ++attr_index)
			{
				m_attrs_add.m_attrs[attr_index] += title_cfg->attrs[attr_index];
			}
			for (attr_index = BATTLE_ATTR_CORRECTED_VALUE_BEGIN; attr_index < BATTLE_ATTR_CORRECTED_VALUE_END; ++attr_index)
			{
				m_attrs_add.m_attrs[attr_index] += title_cfg->attrs[attr_index];
			}
			for (attr_index = BATTLE_ATTR_ANTI_VALUE_BEGIN; attr_index < BATTLE_ATTR_ANTI_VALUE_END; ++attr_index)
			{
				m_attrs_add.m_attrs[attr_index] += title_cfg->attrs[attr_index];
			}
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void Title::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	for (int i = 0; i < m_title_count; ++i)
	{
		if (m_param.title_list[i].due_time > 0 && m_param.title_list[i].due_time <= now_second)
		{
			this->MoveTitle(i, "DUE_TIME");
		}
	}
}

int Title::AddTitle(int title_id, const char * reason, unsigned int due_time, int notice_type)
{
	if (m_title_count >= TITLE_MAX_NUM || this->HasTitle(title_id)) return -1;

	const TitleCfg * title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(title_id);
	if (NULL == title_cfg) return -1;

	int add_index = m_title_count;
	TitleData & title_con = m_param.title_list[add_index];
	title_con.title_id = title_id;
	
	if (title_cfg->time > 0)	// 如果配置表里有配限制时间 则用配置表里面的时间
	{
		title_con.due_time = (unsigned int)(EngineAdapter::Instance().Time() + title_cfg->time);
		if (due_time > 0)
		{
			title_con.due_time = GetMin(title_con.due_time, due_time);
		}
	}
	else if (due_time > 0)		// 否则用传入参数
	{
		title_con.due_time = due_time;
	}

	++m_title_count;
	

	this->SendSingleInfo(add_index, notice_type);

	/*int wear_title = this->GetWearTitleIndex();
	if (wear_title < 0 && this->IsAutoWear())
	{
		this->OnWearTitle(add_index, true);
	}*/
	ROLE_LOG_QUICK10(LOG_TYPE_ADD_TITLE, m_role_module_mgr->GetRole(), title_id, m_title_count, __FUNCTION__, reason, title_con.due_time, 0, 0, 0, 0);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_TITLE, __FUNCTION__);
	EventHandler::Instance().OnAddTitle(m_role_module_mgr->GetRole(), title_id, m_title_count);
	if (notice_type == TITLE_NOTICE_TYPE_DEFAULT)
	{
		this->SendTitleNotice(add_index, Protocol::SCTitleNotice::NOTICE_TYPE_GET_TITLE);
	}

	return add_index;
}

int Title::AddTitleOrRenewDueTime(int title_id, const char * reason, unsigned int due_time /*= 0*/, int notice_type /*= TITLE_NOTICE_TYPE_DEFAULT*/)
{
	if (!this->HasTitle(title_id))
	{
		return this->AddTitle(title_id, reason, due_time, notice_type);
	}
	else
	{
		TitleData* title = this->MutableTitleData(title_id);
		if (NULL == title) return -1;

		const TitleCfg * title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(title_id);
		if (NULL == title_cfg) return -1;

		title->due_time = (unsigned int)(EngineAdapter::Instance().Time() + title_cfg->time);
		if (due_time > 0)
		{
			title->due_time = GetMin(title->due_time, due_time);
		}

		return 0;
	}
}

int Title::AddSameSourceTypeTitle(int title_id, int notice_type)
{
	const TitleCfg * title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(title_id);
	if (NULL == title_cfg)
	{
		return -__LINE__;
	}

	if (title_cfg->source_type <= TITLESOURCETYPE_DEFAULT) //只有要计算出来的称号才允许
	{
		return  -__LINE__;
	}

	int replace_index = -1;
	for (int i = 0; i < m_title_count && i < TITLE_MAX_NUM; ++i)
	{
		TitleData & title_data = m_param.title_list[i];

		if (title_data.title_id == title_id)
		{
			return -__LINE__;			//如果拥有了就返回
		}

		if (replace_index == -1)
		{
			const TitleCfg * cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(title_data.title_id);
			if (NULL == cfg)
			{
				continue;
			}

			if (cfg->source_type == title_cfg->source_type)
			{
				replace_index = i;
			}
		}
	}

	if (replace_index != -1)
	{
		TitleData & title_data = m_param.title_list[replace_index];
		title_data.Reset();
		title_data.title_id = title_id;

		this->SendSingleInfo(replace_index, notice_type);
		this->SendTitleChangeToScene();

		return 0;
	}

	return AddTitle(title_id, __FUNCTION__, 0, notice_type);
}

void Title::MoveTitle(int title_index, const char * reason, int notice_type)
{
	if (title_index < 0 || title_index >= TITLE_MAX_NUM) return;

	TitleData & delete_title = m_param.title_list[title_index];
	if (delete_title.title_id <= 0) return;

	int old_title_id = delete_title.title_id;
	int end_index = m_title_count - 1;
	if (end_index < 0 || end_index >= TITLE_MAX_NUM) return;
	
	
	//在删除重置前,先行通知客户端
	if (notice_type == TITLE_NOTICE_TYPE_DEFAULT)
	{
		this->SendTitleNotice(title_index, Protocol::SCTitleNotice::NOTICE_TYPE_LOSE_TITLE);
	}

	gamelog::g_log_title.printf(LL_INFO, "%s reason[%s] user[%d, %s] title[%d, %u]", __FUNCTION__, NULL != reason ? reason : "NULL",
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), delete_title.title_id, delete_title.due_time);

	TitleData & end_title = m_param.title_list[end_index];
	delete_title = end_title;
	end_title.Reset();

	--m_title_count;

	this->SendSingleInfo(title_index);
	this->SendSingleInfo(end_index);
	if (end_index == m_param.wear_index)
	{
		if (end_index == title_index)
		{
			m_param.wear_index = -1;
			m_param.wear_title_id = 0;
		}
		else
		{
			m_param.wear_index = title_index;
			m_param.wear_title_id = m_param.title_list[title_index].title_id;
		}
		this->SendOtherInfo();
	}
	else if (title_index == m_param.wear_index)
	{
		m_param.wear_index = -1;
		m_param.wear_title_id = 0;
		this->SendOtherInfo();
	}
	EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), old_title_id, m_title_count);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_TITLE, __FUNCTION__);
}

bool Title::RemoveTitle(int title_id, const char * reason, int notice_type)
{
	for (int i = 0; i < TITLE_MAX_NUM; ++i)
	{
		if (m_param.title_list[i].title_id == title_id)
		{
			this->MoveTitle(i, reason);
			return true;
		}
	}
	return false;
}

bool Title::RemoveTitleBySrcType(int src_type, const char * reason, int notice_type, std::set<int> & exclude_title_id)
{
	std::set<int> need_del;
	for (int i = 0; i < TITLE_MAX_NUM && i < m_title_count; ++i)
	{
		if (exclude_title_id.erase(m_param.title_list[i].title_id))
		{
			continue;
		}

		const TitleCfg * title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(m_param.title_list[i].title_id);
		if (NULL == title_cfg)
		{
			continue;
		}

		if (title_cfg->source_type == TITLESOURCETYPE_DEFAULT || title_cfg->source_type != src_type)
		{
			continue;
		}

		need_del.insert(m_param.title_list[i].title_id);
	}

	for (std::set<int>::iterator ite = need_del.begin(); ite != need_del.end(); ++ite)
	{
		this->RemoveTitle(*ite, __FUNCTION__);
	}

	return true;
}

void Title::CreateRoleAddTitle()
{
	const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_role_module_mgr->GetRole()->GetProfession(), m_role_module_mgr->GetAdvanceCareer()->GetSubLevel());
	if (NULL != cfg && cfg->reward_title > 0) 
	{
		if (!this->HasTitle(cfg->reward_title))
		{
			int index = this->AddTitle(cfg->reward_title, __FUNCTION__, 0, TITLE_NOTICE_TYPE_NO_NOTICE);
			if (index >= 0)
			{
				if (this->GetWearTitle() != cfg->reward_title)
				{
					this->OnWearTitle(index, true);
				}
				m_param.is_first_create_role = 1;
			}
		}
	}
}

int Title::OnWearTitle(int index, bool is_face_wear, bool is_duel)
{
	if (index < 0 || index >= TITLE_MAX_NUM) return -1;
	if (m_title_count <= index) return -2;
	if (!is_duel)
	{
		if (m_role_module_mgr->GetDuel()->IsRedName())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_TITLE_HAS_KILL_VALUE);
			return -3;
		}
	}
	if (m_param.wear_index == index)
	{
		if (!is_face_wear)
		{
			if (!EventHandler::Instance().OnWearOrGetOffTitle(m_role_module_mgr->GetRole(), m_param.wear_title_id)) return -3;
		}

		m_param.wear_index = -1;
		m_param.wear_title_id = 0;
	}
	else
	{
		if (!is_face_wear)
		{
			if (!EventHandler::Instance().OnWearOrGetOffTitle(m_role_module_mgr->GetRole(), m_param.title_list[index].title_id, true)) return -4;
		}

		m_param.wear_index = index;
		m_param.wear_title_id = m_param.title_list[index].title_id;
	}

	this->SendOtherInfo();
	this->SendTitleChangeToScene();
	return 0;
}

bool Title::OnWearTitleById(int title_id, bool is_face_wear)
{
	for (int i = 0; i < m_title_count && i < TITLE_MAX_NUM; ++i)
	{
		TitleData & title_data = m_param.title_list[i];
		if (title_data.title_id == title_id)
		{
			if (0 <= this->OnWearTitle(i, is_face_wear))
			{
				return true;
			}
		}
	}

	return false;
}

void Title::OnRoleChangeProf(int old_prof, int new_prof)
{
	int old_base_prof = old_prof / PROFESSION_BASE;
	int new_base_prof = new_prof / PROFESSION_BASE;
	if (old_base_prof == new_base_prof)		// 进阶，获取新称号；
	{
		const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(new_prof , m_role_module_mgr->GetAdvanceCareer()->GetSubLevel());
		if (NULL != cfg && cfg->reward_title > 0) this->AddTitle(cfg->reward_title, __FUNCTION__);
	}
	else		// 改变职业，回收旧职业的所有进阶称号，获取当前新职业的所有进阶称号；
	{
		std::vector<int> old_prof_title_list;	// 旧职业称号表
		std::vector<int> new_prof_title_list;	// 新职业称号表
		std::set<int> remove_title_list;

		LOGIC_CONFIG->GetProfessionConfig()->GetAllTitleByBaseProf(old_base_prof, old_prof_title_list);	// 旧职业称号
		LOGIC_CONFIG->GetProfessionConfig()->GetAllTitleByBaseProf(new_base_prof, new_prof_title_list);	// 新职业称号
		for (std::vector<int>::iterator old_it = old_prof_title_list.begin(); old_it != old_prof_title_list.end(); ++old_it)
		{
			remove_title_list.insert(*old_it);
		}

		for (std::vector<int>::iterator new_it = new_prof_title_list.begin(); new_it != new_prof_title_list.end(); ++new_it)
		{
			remove_title_list.insert(*new_it);
		}

		for (int i = m_title_count - 1; i >= 0 ; --i)
		{
			int title_id = m_param.title_list[i].title_id;
			if (remove_title_list.end() != remove_title_list.find(title_id))
			{
				if (i == m_param.wear_index)		// 如果穿着，就脱下来
				{
					m_param.wear_index = -1;
					m_param.wear_title_id = 0;
					this->SendOtherInfo();
				}
				this->MoveTitle(i, __FUNCTION__, TITLE_NOTICE_TYPE_NO_NOTICE);

				remove_title_list.erase(title_id);
				if (remove_title_list.empty()) break;
			}
		}
		
		int promote_times = new_prof % PROFESSION_BASE;
		for (int j = 0; j < promote_times && j < (int)new_prof_title_list.size(); ++j)
		{
			this->AddTitle(new_prof_title_list[j], __FUNCTION__, 0, TITLE_NOTICE_TYPE_NO_NOTICE);
		}
		int user_old_prof = old_base_prof * PROFESSION_BASE;
		this->CalChiefElectionTitle(user_old_prof);

		this->SendAllInfo();
		this->SendOtherInfo();
		/*int wear_title = this->GetWearTitleIndex();
		if (wear_title < 0)
		{
			this->OnWearTitle(m_title_count - 1);
		}*/
	}

	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_TITLE, __FUNCTION__);
}

bool Title::HasTitle(int title_id)
{
	for (int i = 0; i < m_title_count && i < TITLE_MAX_NUM; ++i)
	{
		if (title_id == m_param.title_list[i].title_id)
		{
			return true;
		}
	}

	return false;
}

int Title::GetWearTitle()
{
	if (m_param.wear_index >= 0 && m_param.wear_index < TITLE_MAX_NUM)
	{
		return m_param.title_list[m_param.wear_index].title_id;
	}
	return 0;
}

void Title::SendAllInfo()
{
	return; // 客户端要求不下发

	/*
	Protocol::SCTitleAllInfo info;
	info.count = m_title_count;
	for (int i = 0; i < m_title_count && i < TITLE_MAX_NUM; ++i)
	{
		info.title_list[i] = m_param.title_list[i];
	}

	int len = sizeof(info) - (TITLE_MAX_NUM - m_title_count) * sizeof(TitleData);
	m_role_module_mgr->NetSend((const char *)&info, len);
	*/
}

void Title::SendOtherInfo()
{
	Protocol::SCTitleOtherInfo info;
	info.wear_title_id = m_param.wear_title_id;
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void Title::SendSingleInfo(int index,int notice_type)
{
	return; // 客户端要求不下发

	/*
	if (index < 0 || index >= TITLE_MAX_NUM) return;
	Protocol::SCTitleSingleInfo info;
	info.notice_type = notice_type;
	info.index = index;
	info.title = m_param.title_list[index];
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
	*/
}

void Title::SendTitleNotice(int seq, int notice)
{
	Protocol::SCTitleNotice info;
	info.notice = notice;
	info.index = seq;
	info.title_id = m_param.title_list[seq].title_id;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void Title::CalChiefElectionTitle(int last_election, int notice_type)
{
	int remove_title_id = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetTitleId(last_election);
	int base_prof = m_role_module_mgr->GetRole()->GetBaseProfession();

	bool ret = WorldStatus::Instance().IsChiefElectionWinner(m_role_module_mgr->GetRole()->GetUID(), base_prof);
	if (ret)
	{
		int title_id = LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetTitleId(base_prof);
		if (remove_title_id > 0 && remove_title_id != title_id)
		{
			if (!this->RemoveTitle(remove_title_id, __FUNCTION__))
			{
				EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), remove_title_id, m_title_count);
			}
		}
		this->AddTitle(title_id, __FUNCTION__, 0, notice_type);
	}
	else
	{
		if (!this->RemoveTitle(remove_title_id, __FUNCTION__))
		{
			EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), remove_title_id, m_title_count);
		}
	}
}

void Title::CalPeakBattleTitle(int last_match_round, int notice_type)
{
	const ChiefPeakBattleRankRewardCfg * last_cfg = LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetChiefPeakBattleRewardCfg(last_match_round);
	const ChiefPeakBattleRankRewardCfg * cfg = LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetChiefPeakBattleRewardCfg(WorldStatus::Instance().GetChiefPeakBattleMatchRound(m_role_module_mgr->GetRole()->GetUID()));
	if (cfg != NULL)
	{
		if (last_cfg != NULL && last_cfg->title_id != cfg->title_id)
		{
			if (!this->RemoveTitle(last_cfg->title_id, __FUNCTION__))
			{
				EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), last_cfg->title_id, m_title_count);
			}
		}
		this->AddTitle(cfg->title_id, __FUNCTION__, 0, notice_type);
	}
	else
	{
		if (last_cfg != NULL)
		{
			if (!this->RemoveTitle(last_cfg->title_id, __FUNCTION__))
			{
				EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), last_cfg->title_id, m_title_count);
			}
		}
	}
	if (NULL == cfg && NULL == last_cfg)
	{
		const std::set<int> * title_list =  LOGIC_CONFIG->GetActivityChiefPeakBattleConfig()->GetChiefPeakBattleRankTitleList();
		if (NULL != title_list && !title_list->empty())
		{
			for (std::set<int>::const_iterator it = title_list->begin(); it != title_list->end(); it++)
			{
				if (!this->RemoveTitle(*it, __FUNCTION__))
				{
					EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), *it, m_title_count);
				}
			}
		}
	}

}

void Title::CalColosseumTitle(int notice_type)
{
	// 跨服不计算竞技场成就，因为跨服没有竞技场数据
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	int rank = ColosseumManager::Instance().GetRoleRank(m_role_module_mgr->GetUid());
	const ColosseumOtherCfg& other_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetOtherCfg();

	const TitleCfg * top_1_title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(other_cfg.top_1_title_id);
	if (NULL == top_1_title_cfg || top_1_title_cfg->source_type != TITLESOURCETYPE_COLOSSEUM)
	{
		return;
	}

	const TitleCfg * top_10_title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(other_cfg.top_10_title_id);
	if (NULL == top_10_title_cfg || top_10_title_cfg->source_type != TITLESOURCETYPE_COLOSSEUM)
	{
		return;
	}

	if (rank > 10)
	{	
		if (!this->RemoveTitle(other_cfg.top_1_title_id, __FUNCTION__))
		{
			EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), other_cfg.top_1_title_id, m_title_count);
		}
		if (!this->RemoveTitle(other_cfg.top_10_title_id, __FUNCTION__))
		{
			EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), other_cfg.top_10_title_id, m_title_count);
		}
	}
	else
	{
		if (rank == 1)
		{
			this->AddTitle(other_cfg.top_1_title_id, __FUNCTION__, 0, notice_type);
		}
		this->AddTitle(other_cfg.top_10_title_id, __FUNCTION__, 0, notice_type);
	}

	//this->AddSameSourceTypeTitle(title_id, notice_type);
}

void Title::CalOnlyFightTitle(int notice_type)
{
	int rank = WorldStatus::Instance().GetOnlyFightRoleRank(m_role_module_mgr->GetUid());
	if (rank == 0)
	{
		std::vector<int> title_vec;
		WorldStatus::Instance().GetOnlyFightRankTitleList(&title_vec);
		const std::vector<int> * new_title_list = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetRankTitleListCfg();
		if (NULL != new_title_list)
		{
			title_vec.insert(title_vec.end(), new_title_list->begin(), new_title_list->end());
		}
		for (int i = 0; i < (int)title_vec.size(); i++)
		{
			if (title_vec[i] > 0)
			{
				if (!this->RemoveTitle(title_vec[i], __FUNCTION__))
				{
					EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), title_vec[i], m_title_count);
				}
			}
		}
	}	
	else
	{
		int title_id = WorldStatus::Instance().GetOnlyFightRankTitleId(rank);
		const OnlyFightRankAward * cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightRankCfg(rank, m_role_module_mgr->GetRole()->GetLevel());
		if (NULL == cfg || title_id != cfg->title)
		{
			if (title_id > 0)
			{
				if (!this->RemoveTitle(title_id, __FUNCTION__))
				{
					EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), title_id, m_title_count);
				}
			}
		}
		if (NULL != cfg)
		{
			this->AddTitle(cfg->title, __FUNCTION__, 0, notice_type);
		}
	}

}

void Title::CalGuildFightTitle(int notice_type)
{
	int title_id = 0;
	Guild * guild = m_role_module_mgr->GetRole()->GetGuild();
	if (guild != NULL)
	{
		title_id = guild->GetGuildFightTitle();
	}
	int new_title_id = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightRankTitle();

	if (title_id > 0)
	{
		if (new_title_id <= 0 || new_title_id != title_id)
		{
			if (!this->RemoveTitle(title_id, __FUNCTION__))
			{
				EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), title_id, m_title_count);
			}
		}
		if (new_title_id > 0)
		{
			this->AddTitle(new_title_id, __FUNCTION__, 0, notice_type);
		}
	}
	else
	{
		if (new_title_id > 0) //清除旧的
		{
			if (!this->RemoveTitle(new_title_id, __FUNCTION__))
			{
				EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), new_title_id, m_title_count);
			}
		}
	}

}

void Title::CalBraveGroundTitle(int notice_type)
{
	ActivityBraveGround * activity = (ActivityBraveGround *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_BRAVE_GROUND);
	if (activity == NULL)
	{
		return;
	}

	std::set<int> add_title_id;
	add_title_id.insert(activity->GetTopRoleTitle(m_role_module_mgr->GetUid()));
	this->RemoveTitleBySrcType(TITLESOURCETYPE_BRAVE_GROUND_TOP_ROLE, __FUNCTION__, notice_type, add_title_id);
	for (std::set<int>::iterator iter = add_title_id.begin(); iter != add_title_id.end(); ++iter)
	{
		this->AddTitle(*iter, __FUNCTION__, 0, notice_type);
	}
}

void Title::CalHuanJieZhanChangeTitle(int notice_type)
{
	ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
	if (activity == NULL)
	{
		return;
	}

	std::set<int> add_title_id;
	int title_id = activity->GetTopRoleTitle(m_role_module_mgr->GetUid());
	if (title_id != 0)
	{
		add_title_id.insert(title_id);
	}
	
	this->RemoveTitleBySrcType(TITLESOURCETYPE_HUAN_JIE_ZHAN_CHANG_TOP_ROLE, __FUNCTION__, notice_type, add_title_id);
	for (std::set<int>::iterator iter = add_title_id.begin(); iter != add_title_id.end(); ++iter)
	{
		this->AddTitle(*iter, __FUNCTION__, 0, notice_type);
	}
}

void Title::CalGuildHonorTitle2(int notice_type)
{
	int guild_id = m_role_module_mgr->GetRole()->GetGuildID();
	std::set<int> add_title_id;
	int title_id = GuildHonorBattleManager::Instance().GetMatchTitle(guild_id);
	if (title_id != 0)
	{
		add_title_id.insert(title_id);
	}

	this->RemoveTitleBySrcType(TITLESOURCETYPE_GUILD_HONOR_BATTLE, __FUNCTION__, notice_type, add_title_id);
	for (std::set<int>::iterator iter = add_title_id.begin(); iter != add_title_id.end(); ++iter)
	{
		this->AddTitle(*iter, __FUNCTION__, 0, notice_type);
	}
}

void Title::CalQingYuanTitle(int notice_type)
{
	int role_id = m_role_module_mgr->GetUid();
	std::set<int> add_title_id;

	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(role_id);
	if (qy)
	{
		add_title_id = qy->GetQingYuanTitle();
	}

	this->RemoveTitleBySrcType(TITLESOURCETYPE_QING_YUAN, __FUNCTION__, notice_type, add_title_id);
	for (std::set<int>::iterator iter = add_title_id.begin(); iter != add_title_id.end(); ++iter)
	{
		this->AddTitle(*iter, __FUNCTION__, 0, notice_type);
	}
}

void Title::CalWorldTeamArenaTitle(int notice_type)
{
	const std::map<int, int> & title_map = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetRankTitleListCfg();
	WorldTeamArenaRoleInfo team_info = m_role_module_mgr->GetWorldTeamArenaRankInfo();
	
	//由排名找到最新称号
	int new_title_id = 0;
	std::set<int> title_list;
	for (std::map<int, int>::const_reverse_iterator it = title_map.rbegin(); it != title_map.rend(); ++it)
	{
		if (team_info.world_team_arena_rank > 0 && team_info.world_team_arena_rank <= it->first)
		{
			new_title_id = it->second;
		}
		title_list.insert(it->second);
	}

	if (0 != new_title_id)
	{
		//最新配置称号跟之前不一致则移除之前记录
		if (team_info.title_id > 0 && new_title_id != team_info.title_id)
		{
			if (!this->RemoveTitle(team_info.title_id, __FUNCTION__))
			{
				EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), team_info.title_id, m_title_count);
			}
		}
		
		this->AddTitle(new_title_id, __FUNCTION__, 0, notice_type);
	}
	else
	{
		//玩家没有符合称号则移除之前可能存在的称号
		title_list.insert(team_info.title_id);

		for (std::set<int>::const_iterator it = title_list.begin(); it != title_list.end(); ++it)
		{
			if ((*it) > 0 && !this->RemoveTitle(*it, __FUNCTION__))
			{
				EventHandler::Instance().OnLoseTitle(m_role_module_mgr->GetRole(), *it, m_title_count);
			}
		}
	}
	//重新设置玩家记录
	team_info.title_id = new_title_id;
	m_role_module_mgr->OnSetWorldTeamArenaInfo(team_info.world_team_arena_rank, EngineAdapter::Instance().Time(), &team_info.title_id);
}

void Title::CalProfessionTitle(int notice_type)
{
	const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg
	(m_role_module_mgr->GetRole()->GetProfession(), m_role_module_mgr->GetAdvanceCareer()->GetSubLevel());
	if (NULL == cfg || cfg->reward_title <= 0) return;

	std::vector<int> title_list;
	LOGIC_CONFIG->GetProfessionConfig()->GetAllTitleByBaseProf(m_role_module_mgr->GetRole()->GetAppearanceProf(), title_list);
	if (title_list.empty()) return;

	bool is_find = false;
	for (int i = (int)title_list.size() - 1; i >= 0; i--)
	{
		if (is_find)
		{
			this->AddTitle(title_list[i], __FUNCTION__, 0, notice_type);
		}

		if (title_list[i] == cfg->reward_title)
		{
			is_find = true;
		}
	}
}

void Title::CalGuildMemberTitle(int notice_type)
{
	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (NULL == other_cfg) return;

	Guild * guild = m_role_module_mgr->GetRole()->GetGuild();
	if (NULL == guild)
	{
		this->RemoveTitle(other_cfg->post_title_id, __FUNCTION__);
	}
	else
	{
		this->AddTitle(other_cfg->post_title_id, __FUNCTION__, 0, notice_type);
	}
}

bool Title::IsAutoWear()
{
	return m_is_auto_wear;
}

void Title::SetAutoWear(bool is_auto_wear)
{
	m_is_auto_wear = is_auto_wear;
}

const TitleData * Title::GetWearTitleInfo()
{
	if(m_param.wear_index < 0 || m_param.wear_index >=TITLE_MAX_NUM)
		return NULL;
	
	return &m_param.title_list[m_param.wear_index];
}

void Title::SendTitleChangeToScene()
{
	Protocol::SCTitleChange scmsg;
	if(m_param.wear_index== -1 )
		scmsg.title_id = 0;
	else
	scmsg.title_id = m_param.title_list[m_param.wear_index].title_id;
	scmsg.objid = m_role_module_mgr->GetRole()->GetId();
	scmsg.facescore_seq = m_role_module_mgr->GetFaceScore()->GetFaceEquipSeq(FACE_TITLENAME);
	m_role_module_mgr->GetRole()->GetScene()->GetZoneMatrix()->NotifyAreaMsg(m_role_module_mgr->GetRole()->GetObserHandle(), &scmsg, sizeof(scmsg));

}


void Title::GmPrintAllTitle()
{
	printf("\n%s  ---> TotalCount: %d", __FUNCTION__, m_title_count);
	for (int i = 0; i < TITLE_MAX_NUM; ++i)
	{
		const UInt16 & title_id = m_param.title_list[i].title_id;
		if (title_id > 0) printf("\n\tIndex: %d | TitleID: %d", i, title_id);
	}
	printf("\nEnd.\n");
}

TitleData* Title::MutableTitleData(int title_id)
{
	for (int i = 0; i < m_title_count && i < TITLE_MAX_NUM; ++i)
	{
		if (title_id == m_param.title_list[i].title_id)
		{
			return &m_param.title_list[i];
		}
	}

	return NULL;
}
