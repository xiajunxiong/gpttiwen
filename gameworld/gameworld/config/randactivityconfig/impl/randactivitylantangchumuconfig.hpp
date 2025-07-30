#ifndef __RAND_ACTIVITY_LAN_TANG_CHU_MU_CONFIG_HPP__
#define __RAND_ACTIVITY_LAN_TANG_CHU_MU_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "item/itempool.h"

struct LTCMBasicCfg
{
	LTCMBasicCfg() :task_type(0), param(0)
	{}

	enum TASK_TYPE
	{
		TASK_TYPE_JING_JI_CHANG = 0,		//!< ������
		TASK_TYPE_QIONG_DING_ZHI_ZHENG = 1,	//!< 񷶥֮��
		TASK_TYPE_WU_FANG_LEI_TAI = 2,		//!< �巽��̨
		TASK_TYPE_XIAN_SHI_HUO_DONG = 3,	//!< ������ʱ�	[ħ������,�������,��һ�а�,�ٹ�ҹ��,������Ϯ,�Թ�����,��������ս,�ý�ս��,��ϯ��ѡ,������Ҷ�,�����Կ���,�´��ع�,˿��֮·,���黰��]
		TASK_TYPE_CONSUME_TI_LI = 4,		//!< ʹ������
		TASK_TYPE_JING_YING_SHI_LIAN = 5,	//!< ��Ӣ����
		TASK_TYPE_YE_WAI_SHOU_LING = 6,		//!< Ұ������
		TASK_TYPE_YUAN_ZHENG_MO_ZHONG = 7,	//!< Զ��ħڣ
		TASK_TYPE_BEI_DOU_QI_XING = 8,		//!< ��������
		TASK_TYPE_ONLINE_TIME = 9,			//!< ����ʱ��
		TASK_TYPE_WAN_LING_HUA_BEN = 10,	//!< ���黰��

		TASK_TYPE_MAX,
	};

	int task_type;
	int param;
	std::vector<ItemConfigData> reward_list;
};

struct LTCMBasicSectionCfg
{
	LTCMBasicSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<LTCMBasicCfg> task_list;
};

struct LTCMRewardCfg
{
	LTCMRewardCfg():rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct LTCMRewardSectionCfg
{
	LTCMRewardSectionCfg() :section_start(0), section_end(0), total_rate(0)
	{}

	int section_start;
	int section_end;
	int total_rate;
	std::vector<LTCMRewardCfg> reward_cfg;
};


class RandActivityManager;
class RandActivityLanTangChuMuConfig : public RandActivityConfig
{
public:
	RandActivityLanTangChuMuConfig();
	virtual ~RandActivityLanTangChuMuConfig();

	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const LTCMBasicCfg* GetTaskCfg(RandActivityManager* ramgr, int seq)const;
	int GetTaskNumCfg(RandActivityManager* ramgr)const;
	const std::vector<std::pair<ItemID, int> >& GetHerbsCfg()const { return m_consume_cfg; }
	const std::vector<ItemConfigData>* RandRewrdCfg(RandActivityManager* ramgr)const;

private:
	int InitTaskCfg(TiXmlElement *RootElement, std::string & errormsg);
	int InitHerbsCfg(TiXmlElement *RootElement, std::string & errormsg);
	int InitRewardCfg(TiXmlElement *RootElement, std::string & errormsg);

	std::vector<LTCMBasicSectionCfg> m_task_section_cfg;
	std::vector<std::pair<ItemID, int> > m_consume_cfg;		// ҩ������,��Ʒid-����
	std::vector<LTCMRewardSectionCfg> m_reward_section_cfg;
};


#endif