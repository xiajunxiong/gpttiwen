#ifndef __RAND_ACTIVITY_DUAN_YANG_XIANG_NANG_CONFIG_HPP__
#define __RAND_ACTIVITY_DUAN_YANG_XIANG_NANG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "item/itempool.h"

struct DYXNBasicCfg
{
	DYXNBasicCfg():task_type(0), param(0)
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

struct DYXNBasicSectionCfg
{
	DYXNBasicSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<DYXNBasicCfg> task_list;
};

struct DYXNRewardCfg
{
	DYXNRewardCfg():rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct DYXNRewardSectionCfg
{
	DYXNRewardSectionCfg() :section_start(0), section_end(0),total_rate(0)
	{}

	int section_start;
	int section_end;
	int total_rate;
	std::vector<DYXNRewardCfg> reward_cfg;
};


class RandActivityManager;
class RandActivityDuanYangXiangNangConfig : public RandActivityConfig
{
public:
	RandActivityDuanYangXiangNangConfig();
	virtual ~RandActivityDuanYangXiangNangConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const DYXNBasicCfg* GetTaskCfg(RandActivityManager* ramgr, int seq)const;
	int GetTaskNumCfg(RandActivityManager* ramgr)const;
	const DYXNRewardCfg* RandRewardCfg(RandActivityManager* ramgr)const;
	const std::vector<std::pair<ItemID, int> >& GetMakeCfg()const { return m_make_cfg; }

private:
	int InitBasicCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitMakeCfg(TiXmlElement *RootElement);

	std::vector<DYXNBasicSectionCfg> m_task_section_cfg;		// ��������
	std::vector<DYXNRewardSectionCfg> m_reward_section_cfg;		// ������ɽ�������
	std::vector<std::pair<ItemID, int> > m_make_cfg;	// ��������, ����id������
};


#endif
