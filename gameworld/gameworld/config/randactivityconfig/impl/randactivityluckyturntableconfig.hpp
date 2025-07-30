#ifndef __RAND_ACTIVITY_LUCKY_TURN_TABLE_CONFIG_HPP__
#define __RAND_ACTIVITY_LUCKY_TURN_TABLE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raluckyturntableparam.hpp"
#include "item/itembase.h"

struct LuckyTurntableParamCfg
{
	LuckyTurntableParamCfg() : param(0), frequency(0)
	{}

	//int type;				//�������� RA_LUCKY_TURNTABLE_TYPE
	int param;				//�ڴ˱�ʾΪ����������ʹ���
	int frequency;			//����ת�̴���
};

struct LuckyTurntableRewardCfg
{
	LuckyTurntableRewardCfg() :seq(0), rate(0), hearsay(0)
	{}

	int seq;							//��Ӧ���ñ��seq
	ItemConfigData reward_item;			//��������
	int rate;							//Ȩ��/ռ��
	int hearsay;						//�Ƿ��� 0 :��
};

struct LuckyTurntableRewardSectionCfg
{
	LuckyTurntableRewardSectionCfg() :section_start(0), section_end(0), rate_count(0)
	{}

	int section_start;						//��ʼʱ��
	int section_end;						//����ʱ��
	int rate_count;							//��Ȩ��
	LuckyTurntableRewardCfg node[MAX_RA_LUCKY_TURNTABLE_REWARD_NUM];
};

class RandActivityManager;

class RandActivityLuckyTurnTableConfig : public RandActivityConfig
{
public:
	RandActivityLuckyTurnTableConfig();
	virtual ~RandActivityLuckyTurnTableConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const LuckyTurntableParamCfg* GetParamCfg(int type)const;
	const LuckyTurntableRewardCfg* GetRandRewardCfg(RandActivityManager* ramgr)const;
	const LuckyTurntableRewardCfg* GetRewardCfgBySeq(int seq)const;

private:
	int InitTurntableParamCfg(TiXmlElement *RootElement);
	int InitTurntableRewardCfg(TiXmlElement *RootElement);

	LuckyTurntableParamCfg m_param_cfg[RA_LUCKY_TURNTABLE_TYPE_MAX];	// ת������
	std::vector<LuckyTurntableRewardSectionCfg> m_reward_section_cfg;	// ת�̽���
	std::vector<LuckyTurntableRewardCfg> m_reward_cfg;					// ת�̽���
};

#endif	//__RAND_ACTIVITY_LUCKY_TURN_TABLE_CONFIG_HPP__