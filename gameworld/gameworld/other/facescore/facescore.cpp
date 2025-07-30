#include "facescore.hpp"
#include "facescoreconfig.hpp"
#include "engineadapter.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/title/title.hpp"
#include "other/title/titleconfig.hpp"
#include "servercommon/errornum.h"
#include "item/money.h"
#include "protocol/msgfacescore.hpp"
#include "protocol/msgtitle.h"
#include "protocol/msgother.h"
#include "obj/character/role.h"
#include "scene/scene.h"
#include "gamelog.h"
#include "other/event/eventhandler.hpp"
#include "scene/scenemanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgsystem.h"

FaceScore::FaceScore() : m_max_level(0), m_current_exp(0), m_active_face_count(0)
{
	memset(m_face_type_count_list, 0, sizeof(m_face_type_count_list));
}

FaceScore::~FaceScore()
{
}

void FaceScore::Init(RoleModuleManager * role_module_manager, const FaceScoreTimeParam &face_score_time_param, const FaceScoreParam & face_score_param, int buff_len, const char *buff_cur)
{
	m_mgr = role_module_manager;
	m_param = face_score_param;
	m_time_param = face_score_time_param;

	// 录入资源超时映射
	if (NULL != buff_cur && buff_len >= (int)sizeof(short))
	{
		short total_count = *(short *)buff_cur;
		buff_cur += sizeof(short);
		buff_len -= sizeof(short);

		for (int i = 0; i < total_count; ++i)
		{
			if (buff_len <= 0) break;

			int key = *(int *)buff_cur;
			buff_cur += sizeof(int);
			buff_len -= sizeof(int);

			UInt32 val = *(UInt32 *)buff_cur;
			buff_cur += sizeof(UInt32);
			buff_len -= sizeof(UInt32);

			m_timeout_res_map[key] = val;
		}
	}

	// 初始化参数
	{
		std::set<int> title_list = LOGIC_CONFIG->GetTitleConfig()->GetNotSaveTitle();
		m_max_level = LOGIC_CONFIG->GetFaceScoreConfig()->GetMaxLevel();	// 最大等级
		// 重算颜值等级
		for (int seq = 0; seq < FACE_RESOURCE_NUM_MAX; ++seq)
		{
			const FaceResourceInfo * res_cfg = LOGIC_CONFIG->GetFaceScoreConfig()->GetResInfoCfg(seq);
			if (NULL != res_cfg && m_param.active_flag.IsBitSet(seq)) 
			{
				if (title_list.find(res_cfg->title_id) != title_list.end())
				{
					m_param.active_flag.UnSetBit(seq);
					continue;
				}

				if (res_cfg->type >= FACE_CHATFRAME && res_cfg->type < FACE_TYPE_MAX)
				{
					m_face_type_count_list[res_cfg->type]++;
				}

				if(res_cfg->exp_add > 0)
				{
					m_current_exp += res_cfg->exp_add;
				}
			}
		}
		for (int i = 0; i < ARRAY_ITEM_COUNT(m_face_type_count_list); i++)
		{
			m_active_face_count += m_face_type_count_list[i];
		}

		LOGIC_CONFIG->GetFaceScoreConfig()->InitLevelAndExpByTotalExp(&m_param.face_level, &m_current_exp, m_mgr->GetRole()->GetAppearanceProf());
		if (m_param.face_level < 0)
		{
			m_param.face_level = 0;
		}
	}
	if (m_param.is_first_change_equip_list_reset != 2)
	{
		memset(m_param.face_equip_list, -1, sizeof(m_param.face_equip_list));
		m_param.is_first_change_equip_list_reset = 2;
	}
}

void FaceScore::GetFaceScoreParam(FaceScoreTimeParam *out_time_param, FaceScoreParam * out_param, char * buff_cur, int * buff_len)
{
	*out_param = m_param;
	*out_time_param = m_time_param;

	if (NULL == buff_cur || NULL == buff_len) return;

	*buff_len = 0;

	short total_count = (short)(m_timeout_res_map.size());
	if (total_count > FACE_RESOURCE_NUM_MAX) total_count = FACE_RESOURCE_NUM_MAX;

	*(short *)buff_cur = total_count;
	buff_cur += sizeof(short);
	*buff_len += sizeof(short);

	int idx = 0;
	for (std::map<int, UInt32>::iterator loop = m_timeout_res_map.begin(); loop != m_timeout_res_map.end(); ++loop)
	{
		*(int *)buff_cur = loop->first;
		buff_cur += sizeof(int);
		*buff_len += sizeof(int);

		*(UInt32 *)buff_cur = loop->second;
		buff_cur += sizeof(UInt32);
		*buff_len += sizeof(UInt32);

		if (++idx >= FACE_RESOURCE_NUM_MAX) break;
	}
}

void FaceScore::Update(time_t now_second)
{
	for (std::map<int, UInt32>::iterator check_loop = m_timeout_res_map.begin(); check_loop != m_timeout_res_map.end();)
	{
		if (now_second > check_loop->second)		// 超时了
		{
			this->OnLostRes(check_loop->first, true);
			m_timeout_res_map.erase(check_loop++);
		}
		else
		{
			++check_loop;
		}
	}

	for (int seq = 0; seq < FACE_RESOURCE_NUM_MAX; ++seq)
	{
		if (m_param.active_flag.IsBitSet(seq))
		{
			if (now_second >= m_time_param.times[seq] && m_time_param.times[seq] != 0)
			{
				this->OnLostRes(seq, true);
				this->SendResTimeInfo(seq, m_time_param.times[seq]);
			}

		}
	}


}

const AttributeList & FaceScore::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();

		const FaceAttrAddInfo * attr_fg = LOGIC_CONFIG->GetFaceScoreConfig()->GetAttrAddCfgByLevel(m_param.face_level, m_mgr->GetRole()->GetAppearanceProf());
		if (NULL != attr_fg)
		{
			// 基本属性
			m_attrs_add.m_attrs[BATTLE_ATTR_MAX_HP] += attr_fg->max_hp_add;
			m_attrs_add.m_attrs[BATTLE_ATTR_MAX_MP] += attr_fg->max_mp_add;
			for (int base_type = 0; base_type < BATTLE_BASE_ATTR_NUM; ++base_type)
			{
				m_attrs_add.m_attrs[BATTLE_ATTR_NORMAL_VALUE_BEGIN + base_type] += attr_fg->base_add[base_type];
			}

			// 修正属性
			for (int amen_type = 0; amen_type < BATTLE_AMEN_ATTR_NUM; ++amen_type)
			{
				m_attrs_add.m_attrs[BATTLE_ATTR_CORRECTED_VALUE_BEGIN + amen_type] += attr_fg->amen_add[amen_type];
			}

			// 抵抗值
			for (int resi_type = 0; resi_type < BATTLE_RESI_ATTR_NUM; ++resi_type)
			{
				m_attrs_add.m_attrs[BATTLE_ATTR_ANTI_VALUE_BEGIN + resi_type] += attr_fg->resi_add[resi_type];
			}
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

bool FaceScore::OnWearOrGetOffTitle(int title_id, bool is_wear)
{
	const int seq = LOGIC_CONFIG->GetFaceScoreConfig()->GetSeqByTitleID(title_id);
	bool ret = true;
	if (is_wear)
	{
		//判断最新穿戴的称号是否属于颜值系统
		if (seq >= 0 && seq < FACE_RESOURCE_NUM_MAX)
		{
			ret = this->RequestEquips(seq, false);
		}
		else
		{
			ret = this->RequestRemove(FACE_TITLENAME, false);		//佩戴新称号时,如果新称号不是颜值系统的,就需要拆卸颜值系统的称号 拆卸接口应该返回true
		}

	}
	else
	{
		ret = this->RequestRemove(FACE_TITLENAME, false);			//脱下称号的时候颜值系统应该同步拆卸   拆卸接口应该返回true
	}

	return ret;
}

bool FaceScore::AutoOrUseActiveRes(int seq, int item_id, bool is_auto_use, bool is_notice)
{
	const FaceResourceInfo * cfg_info = LOGIC_CONFIG->GetFaceScoreConfig()->GetResInfoCfg(seq);
	if (NULL == cfg_info)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	if (m_param.active_flag.IsBitSet(seq))
	{
		if (is_auto_use)
		{
			if (is_notice)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_face_score_is_inactive, item_id);
				if (length > 0)
				{
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_FLOAT)))
					{
						m_mgr->NetSend((const void *)&sm, sendlen);
					}
				}
			}
			else
			{
				m_mgr->SendHearsayInfo(Protocol::SCHearsayInfo::HEARSAY_TYPE_ACTIVE_FACE_FAILD, item_id);
			}
		}
		else
		{
			m_mgr->NoticeNum(errornum::EN_FACE_SCORE_RES_NOT_ACTIVE);
		}
		gamelog::g_log_surface.printf(LL_INFO, "%s seq[%d] item[%d] not enough active", __FUNCTION__, seq, item_id);
		return false;
	}
	 
	m_param.active_flag.SetBit(seq);
	this->SendSingleResInfo(seq);
	this->AddFaceScore(cfg_info->exp_add);

	const FaceResourceInfo * res_cfg = LOGIC_CONFIG->GetFaceScoreConfig()->GetResInfoCfg(seq);
	if (NULL == res_cfg) return false;
	if (res_cfg->time == 0)
	{
		this->SendResTimeInfo(seq, res_cfg->time);
	}
	else
	{
		unsigned int sec = (unsigned int) EngineAdapter::Instance().Time();
		m_time_param.times[seq] = sec + res_cfg->time;
		this->SendResTimeInfo(seq, m_time_param.times[seq]);
	}

	if (is_notice)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_face_score_active_succ, item_id);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_FLOAT)))
			{
				m_mgr->NetSend((const void *)&sm, sendlen);
			}
		}
	}
	else
	{
		m_mgr->SendHearsayInfo(Protocol::SCHearsayInfo::HEARSAY_TYPE_ACTIVE_FACE_SUCC, item_id);
	}
	if (cfg_info->type == FACE_TITLENAME)
	{
		m_mgr->GetTitle()->AddTitle(cfg_info->title_id, __FUNCTION__);
	}
	if (res_cfg->type >= FACE_CHATFRAME && res_cfg->type < FACE_TYPE_MAX)
	{
		m_face_type_count_list[res_cfg->type]++;
		m_active_face_count++;
	}
	EventHandler::Instance().OnAddFace(m_mgr->GetRole(), res_cfg->type);
	gamelog::g_log_surface.printf(LL_INFO, "%s seq[%d] item[%d] succ", __FUNCTION__, seq, item_id);
	ROLE_LOG_QUICK6(LOG_TYPE_FACESCORE_ACTIVE, m_mgr->GetRole(), seq, 0, __FUNCTION__, "");
	return true;
}

bool FaceScore::RequestEquips(int seq, bool is_face_wear)
{
	if (seq < 0 || seq >= FACE_RESOURCE_NUM_MAX)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (!m_param.active_flag.IsBitSet(seq))
	{
		m_mgr->NoticeNum(errornum::EN_FACE_SCORE_RES_IS_INACTIVE);
		return false;
	}

	const FaceResourceInfo * cfg_info = LOGIC_CONFIG->GetFaceScoreConfig()->GetResInfoCfg(seq);
	if (NULL == cfg_info || cfg_info->type < 0 || cfg_info->type >= FACE_ADDITION_TYPE_MAX) return false;
	
	// 防止穿戴后客户端还是使用穿戴接口(而不是脱下接口),导致称号模块与颜值头衔不一致
	bool is_repeat = true;
	if (m_param.face_equip_list[cfg_info->type] == seq) 
	{
//#ifdef _DEBUG
//		assert(0);
//#endif
		is_repeat = false;
	}
	bool ret = true;
	if (is_repeat && is_face_wear && cfg_info->type == FACE_TITLENAME)
	{
		ret = m_mgr->GetTitle()->OnWearTitleById(cfg_info->title_id, true);
	}
	if(!ret) return ret;

	m_param.face_equip_list[cfg_info->type] = seq;
	this->SendFaceEquipGrid(cfg_info->type);
	if (cfg_info->type == FACE_TITLENAME)
	{
		this->SendFaceTitleChangeNotice();
	}
	return true;
}

bool FaceScore::RequestRemove(int grid_id, bool is_face_getoff)
{
	if (grid_id < 0 || grid_id >= FACE_ADDITION_TYPE_MAX) return false;

	if (is_face_getoff && grid_id == FACE_TITLENAME)
	{
		const FaceResourceInfo * cfg = LOGIC_CONFIG->GetFaceScoreConfig()->GetResInfoCfg(m_param.face_equip_list[grid_id]);
		if (cfg == NULL) return false;

		m_mgr->GetTitle()->OnWearTitleById(cfg->title_id, true);
	}

	m_param.face_equip_list[grid_id] = -1;
	this->SendFaceEquipGrid(grid_id);
	if (grid_id == FACE_TITLENAME)
	{
		this->SendFaceTitleChangeNotice();
	}

	return true;
}

void FaceScore::OnGainTitle(int title_id)
{
	const int seq = LOGIC_CONFIG->GetFaceScoreConfig()->GetSeqByTitleID(title_id);
	if (seq >= 0 && seq < FACE_RESOURCE_NUM_MAX) 
	{
		this->OnGainRes(seq);
		/*if(m_mgr->GetTitle()->GetWearTitle() == title_id)	//称号模块取消自动穿戴,这边也需取消
		{
			this->RequestEquips(seq, false);		//此处使用false,表示是在获得称号后调用,不该再回调称号模块
		}*/
	}
}

void FaceScore::OnLoseTitle(int title_id)
{
	const int seq = LOGIC_CONFIG->GetFaceScoreConfig()->GetSeqByTitleID(title_id);
	if (seq < 0 || seq >= FACE_RESOURCE_NUM_MAX) return;

	this->OnLostRes(seq);
}

void FaceScore::GetFaceEquipList(int face_equip_list[FACE_ADDITION_TYPE_MAX])
{
	memcpy(face_equip_list, m_param.face_equip_list, sizeof(int) * FACE_ADDITION_TYPE_MAX);
}

int FaceScore::GetFaceEquipSeq(int res_type)
{
	return m_param.face_equip_list[res_type];
}

int FaceScore::GetActiveCount(int face_type)
{
	if (face_type < FACE_CHATFRAME || face_type >= FACE_TYPE_MAX)
	{
		return m_active_face_count;
	}

	return m_face_type_count_list[face_type];
}

void FaceScore::SendFaceAllInfo()
{
	Protocol::SCFaceScoreAllInfo fsai;
	fsai.face_level = m_param.face_level;
	fsai.cur_exp = m_current_exp;
	memcpy(fsai.equip_list, m_param.face_equip_list, sizeof(fsai.equip_list));
	fsai.active_flag = m_param.active_flag;

	m_mgr->NetSend((const void *)&fsai, sizeof(fsai));
}

void FaceScore::SendFaceTitleChangeNotice()
{
	Protocol::SCTitleChange scmsg;
	scmsg.title_id = m_mgr->GetTitle()->GetWearTitle();
	scmsg.objid = m_mgr->GetRole()->GetId();
	scmsg.facescore_seq = m_param.face_equip_list[FACE_TITLENAME];
	m_mgr->GetRole()->GetScene()->GetZoneMatrix()->NotifyAreaMsg(m_mgr->GetRole()->GetObserHandle(), &scmsg, sizeof(scmsg));
}

void FaceScore::SendResTimeAllInfo()
{
	Protocol::SCFaceTimeAllInfo info;
	info.count = 0;
	for (int seq = 0;seq < FACE_RESOURCE_NUM_MAX;++seq)
	{
		if (m_param.active_flag.IsBitSet(seq))
		{
			if (m_time_param.times[seq] != 0)
			{
				info.active_seq[info.count][0] = seq;
				info.active_seq[info.count][1] = m_time_param.times[seq];
				info.count += 1;
			}
			
		}
	}

	int send_len = sizeof(info) - (FACE_RESOURCE_NUM_MAX - info.count) * (sizeof(int) * 2);
	m_mgr->NetSend((const void *) &info, send_len);
}

void FaceScore::OnGainRes(int seq)
{
	if (m_param.active_flag.IsBitSet(seq)) return;	// 如果已经有了，返回

	const FaceResourceInfo * res_cfg = LOGIC_CONFIG->GetFaceScoreConfig()->GetResInfoCfg(seq);
	if (NULL == res_cfg) return;

	m_param.active_flag.SetBit(seq);
	this->SendSingleResInfo(seq);
	if (res_cfg->time == 0)
	{
		this->SendResTimeInfo(seq, res_cfg->time);
	} else
	{
		unsigned int sec = (unsigned int) EngineAdapter::Instance().Time();
		m_time_param.times[seq] = sec + res_cfg->time;
		this->SendResTimeInfo(seq, m_time_param.times[seq]);
	}
	if (res_cfg->type >= FACE_CHATFRAME && res_cfg->type < FACE_TYPE_MAX)
	{
		m_face_type_count_list[res_cfg->type]++;
		m_active_face_count++;
	}
	this->AddFaceScore(res_cfg->exp_add);
	EventHandler::Instance().OnAddFace(m_mgr->GetRole(), res_cfg->type);
}

void FaceScore::OnLostRes(int seq, bool is_face_time)
{
	const FaceResourceInfo * res_cfg = LOGIC_CONFIG->GetFaceScoreConfig()->GetResInfoCfg(seq);
	if (NULL == res_cfg) return;

	if (m_param.face_equip_list[res_cfg->type] == seq)
	{
		this->RequestRemove(res_cfg->type);
	}
	//由于一些称号不保存,与init计算冲突,所以此时判断激活放这边
	if (!m_param.active_flag.IsBitSet(seq))
	{
		return;		// 如果没有这个资源，返回
	}
	if (is_face_time && FACE_TITLENAME == res_cfg->type)
	{
		m_mgr->GetTitle()->RemoveTitle(res_cfg->title_id, __FUNCTION__);
	}

	m_param.active_flag.UnSetBit(seq);
	if (res_cfg->type >= FACE_CHATFRAME && res_cfg->type < FACE_TYPE_MAX)
	{
		m_face_type_count_list[res_cfg->type]--;
		m_active_face_count--;
		if(m_face_type_count_list[res_cfg->type] < 0)
		{
			m_face_type_count_list[res_cfg->type] = 0;
		}
		if (m_active_face_count < 0)
		{
			m_active_face_count = 0;
		}
	}
	this->SendSingleResInfo(seq);
	this->AddFaceScore(-res_cfg->exp_add);
	EventHandler::Instance().OnLostFace(m_mgr->GetRole(), res_cfg->type);
}

void FaceScore::AddFaceScore(int add_value)
{
	if (0 == add_value) return;

	m_current_exp = m_current_exp + add_value;

	// 判断等级改变
	int old_level = m_param.face_level;
	if (m_current_exp > 0)
	{
		int need_exp = LOGIC_CONFIG->GetFaceScoreConfig()->GetUplevelNeedExp(m_param.face_level, m_mgr->GetRole()->GetAppearanceProf());
		while (m_param.face_level < m_max_level && m_current_exp >= need_exp)
		{
			++m_param.face_level;
			m_current_exp -= need_exp;
			need_exp = LOGIC_CONFIG->GetFaceScoreConfig()->GetUplevelNeedExp(m_param.face_level, m_mgr->GetRole()->GetAppearanceProf());
		}
	}
	else
	{
		while (m_param.face_level > 0 && m_current_exp < 0)
		{
			--m_param.face_level;
			m_current_exp += LOGIC_CONFIG->GetFaceScoreConfig()->GetUplevelNeedExp(m_param.face_level, m_mgr->GetRole()->GetAppearanceProf());
		}
	}

	if (old_level != m_param.face_level) m_mgr->ReCalcAttr(AttributeList::RECALC_REASON_FACE_SCORE, __FUNCTION__);
	this->SendLevelAndExp();
}

void FaceScore::SendFaceEquipGrid(int grid_id)
{
	if (grid_id < 0 || grid_id >= FACE_ADDITION_TYPE_MAX) return;

	Protocol::SCFaceListGridInfo flgi;
	flgi.grid_id = grid_id;
	flgi.seq = m_param.face_equip_list[grid_id];

	m_mgr->NetSend((const void *)&flgi, sizeof(flgi));
}

void FaceScore::SendSingleResInfo(int seq)
{
	if (seq < 0 || seq >= FACE_RESOURCE_NUM_MAX) return;

	Protocol::SCFaceResStateInfo frsi;
	frsi.seq = seq;
	frsi.status = m_param.active_flag.IsBitSet(seq) ? 1 : 0;

	m_mgr->NetSend((const void *)&frsi, sizeof(frsi));
}

void FaceScore::SendLevelAndExp()
{
	Protocol::SCFaceLevelAndExps flae;
	flae.face_level = m_param.face_level;
	flae.cur_exp = m_current_exp;

	m_mgr->NetSend((const void *)&flae, sizeof(flae));
}

void FaceScore::SendResTimeInfo(int seq,int time)
{
	Protocol::SCFaceTimeSingleInfo info;
	info.seq = seq;
	info.time = time;

	m_mgr->NetSend((const void *) &info, sizeof(info));
}
