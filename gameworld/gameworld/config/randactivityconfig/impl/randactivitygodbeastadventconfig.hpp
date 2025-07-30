#ifndef __RAND_ACTIVITY_GOD_BEAST_ADVENT_CONFIG_HPP__
#define __RAND_ACTIVITY_GOD_BEAST_ADVENT_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"

#include <vector>

struct YSLSTaskRewardCfg
{
	YSLSTaskRewardCfg():like(0)
	{}

	int like;		// �øж�
	std::vector<ItemConfigData> reward_list;
};

struct YSLSTaskCfg
{
	YSLSTaskCfg():task_type(0),task_param(0)
	{}

	enum TASK_TYPE
	{
		TASK_TYPE_JING_JI_CHANG = 0,		//!< ������
		TASK_TYPE_QIONG_DING_ZHI_ZHENG = 1,	//!< 񷶥֮��
		TASK_TYPE_WU_FANG_LEI_TAI = 2,		//!< �巽��̨
		TASK_TYPE_XIAN_SHI_HUO_DONG = 3,	//!< ������ʱ�	[ħ������,�������,��һ�а�,�ٹ�ҹ��,������Ϯ,�Թ�����,��������ս,�ý�ս��,��ϯ��ѡ,������Ҷ�,�����Կ���,�´��ع�,˿��֮·]
		TASK_TYPE_CONSUME_TI_LI = 4,		//!< ʹ������
		TASK_TYPE_JING_YING_SHI_LIAN = 5,	//!< ��Ӣ����
		TASK_TYPE_YE_WAI_SHOU_LING = 6,		//!< Ұ������
		TASK_TYPE_YUAN_ZHENG_MO_ZHONG = 7,	//!< Զ��ħڣ
		TASK_TYPE_BEI_DOU_QI_XING = 8,		//!< ��������
		TASK_TYPE_ONLINE_TIME = 9,			//!< ����ʱ��
		TASK_TYPE_WAN_LING_HUA_BEN = 10,	//!< ���黰��

		TASK_TYPE_MAX,
	};

	int task_type;		//!< ��������
	int task_param;		//!< �����������
};

struct YSLSTaskGroupCfg
{
	YSLSTaskGroupCfg():task_group(0)
	{}

	int task_group;
	std::map<int, YSLSTaskCfg> task_cfg;				//��������, key-seq
	std::map<int, YSLSTaskRewardCfg> reward_cfg;		//��������, key-seq 
};

struct YSLSTaskSectionCfg
{
	YSLSTaskSectionCfg():section_start(0),section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<YSLSTaskGroupCfg> task_group_cfg;
};

struct YSLSExchangeCfg
{
	YSLSExchangeCfg():unlock_integral(0),limit_type(0),limit_times(0),consume_item_id(0), consume_num(0)
	{}

	enum BUY_LIMIT
	{
		SERVER_LIMIT = 0,	// ȫ���޹�
		DAY_LIMIT = 1,		// ÿ���޹�
		ACTIVITY_LIMIT = 2,	// ��޹�
	};

	int unlock_integral;		// �����øж�����
	int limit_type;				// �޹�����
	int limit_times;			// �޹�����
	int consume_item_id;		// ���ĵ���id
	int consume_num;			// ���ĵ�������
	ItemConfigData reward;
};

struct YSLSExchangeSectionCfg
{
	YSLSExchangeSectionCfg():exchange_group(0)
	{}

	int exchange_group;
	std::map<int, YSLSExchangeCfg> exchange_cfg;		// �һ�����,key-seq
};

struct YSLSFbSectionCfg
{
	YSLSFbSectionCfg() :section_start(0), section_end(0), secret_area_type(0)
	{}

	int section_start;
	int section_end;
	int secret_area_type;		// ����
};

struct YSLSBuySectionCfg
{
	YSLSBuySectionCfg() :section_start(0), section_end(0),price(0)
	{}

	int section_start;
	int section_end;
	int price;				// ����۸�
};

struct YSLSRecycleCfg
{
	YSLSRecycleCfg():item_id(0),coin(0),gold(0),silver_coin(0),has_item(0)
	{}

	int item_id;		//Ҫ���յĵ���
	int coin;			//ͭ��
	int gold;			//Ԫ��
	int silver_coin;	//����
	int has_item;		//�ж��б��Ƿ��е���,0-item_listΪ��
	std::vector<ItemConfigData> item_list;	//����
};

struct YSLSCycleCfg
{
	YSLSCycleCfg():type(0),start_time(0),end_time(0),exchange_group(0)
	{}

	int type;
	unsigned int start_time;			// ��ʼʱ��
	unsigned int end_time;			// ����ʱ��
	int exchange_group;		// �һ���
};

struct YSLSOtherCfg
{
	YSLSOtherCfg() :is_more_change(0), integral_up(0)
	{}

	int is_more_change;		//ȫ���޹���������
	int integral_up;		//�øж�����
};


class RandActivityManager;
class RandActivityYiShouLinShiConfig : public RandActivityConfig
{
public:
	RandActivityYiShouLinShiConfig();
	virtual ~RandActivityYiShouLinShiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int GetTaskParamCfg(int task_type)const;
	int GetTaskNumCfg()const { return (int)m_task_param_cfg.size(); }
	const YSLSTaskCfg* GetTaskCfg(RandActivityManager* ramgr, int seq, int task_group)const;
	const YSLSTaskRewardCfg* GetTaskRewardCfg(RandActivityManager* ramgr, int seq, int task_group)const;
	const YSLSExchangeCfg* GetExchangeCfg(int exchange_group, int seq)const;
	int GetFbType(RandActivityManager* ramgr)const;
	int GetBuyCfg(RandActivityManager* ramgr)const;
	const YSLSRecycleCfg* GetRecycleCfg(int item_id)const;		// �жϵ����ǲ��ǻ�ڵĵ���
	const YSLSRecycleCfg* GetCurRecycleCfg(int exchange_group)const;	//��ȡ��Ӧ���������ڵĵ�������
	int GetStartTimeCfg(unsigned int& start_time, int& type)const;
	const YSLSOtherCfg& GetOtherCfg()const { return m_other_cfg; }
	const YSLSCycleCfg* GetCurCycleCfg(int seq)const;
	const std::map<int, YSLSRecycleCfg>& GetAllRecycleCfg()const { return m_recycle_cfg; }

private:
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitExchangeCfg(TiXmlElement *RootElement);
	int InitFbCfg(TiXmlElement *RootElement);
	int InitBuyCfg(TiXmlElement *RootElement);
	int InitRecycleCfg(TiXmlElement *RootElement);
	int InitCycleCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<YSLSTaskSectionCfg> m_task_section_cfg;				// ��������
	std::map<int, int> m_task_param_cfg;					// key-��������,value--�������
	std::vector<YSLSExchangeSectionCfg> m_exchange_section_cfg;		// �һ�����,key-seq
	std::vector<YSLSFbSectionCfg> m_fb_cfg;					// ��������
	std::vector<YSLSBuySectionCfg> m_buy_cfg;						// ��������
	std::map<int, YSLSRecycleCfg> m_recycle_cfg;			// ��������,key-�һ���
	std::vector<YSLSCycleCfg> m_cycle_time_cfg;						// ���������
	YSLSOtherCfg m_other_cfg;
};


#endif