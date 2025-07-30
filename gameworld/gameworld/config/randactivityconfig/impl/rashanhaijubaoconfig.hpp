#ifndef __RA_SHAN_HAI_JU_BAO_CONFIG_HPP__
#define __RA_SHAN_HAI_JU_BAO_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rashanhaijubaoparam.hpp"

class RandActivityManager;

namespace RAShanHaiJuBao
{
	struct OtherCfg
	{
		OtherCfg()
		{
			draw_cost_item_id = 0;
			cost_num = 0;
		}
		ItemID draw_cost_item_id;
		int cost_num;
	};
	//��Ʒ����
	struct DrawItemCfg
	{
		DrawItemCfg()
		{
			section_day_begin = 0;

			phase_seq = 0;
			phase_act_day_begin = 0;

			draw_power = 0;
			is_brocast = 0;

			draw_index = 0;
			show_type = 0;
		}
		int section_day_begin;	//�ֶο�ʼ�Ŀ�������

		int phase_seq;				//��׶�
		int phase_act_day_begin;	//��׶� ��ʼ�����

		int draw_power;				//Ȩ��
		int is_brocast;				//����

		int draw_index;				//�������
		ItemConfigData draw_item;	//����

		int show_type;				//�ͻ�����
	};
	//����Ȩ��map<Ȩ��begin, ��Ʒ����>
	typedef std::map<int, DrawItemCfg> DrawPoolPowerMap;
	//��������
	struct DrawPool
	{
		DrawPool()
		{
			phase_seq = 0;
			phase_act_day_begin = 0;

			power_count = 0;
		}
		int phase_seq;				//��׶�
		int phase_act_day_begin;	//��׶� ��ʼ�����

		int power_count;
		DrawPoolPowerMap draw_pool;
	};
	//��׶ν���map<��׶�, ����>
	typedef std::map<int, DrawPool> PhaseDrawPoolMap;
	//�׶�����
	struct SectionCfg
	{
		//����[��׶�]
		PhaseDrawPoolMap phase_draw_pool_map;
	};
	//�׶�����map<��������begin, ��Ʒ����>
	typedef std::map<int, SectionCfg> SectionCfgMap;

	//���⽱��
	struct ExtraRewardCfg
	{
		ExtraRewardCfg()
		{
			extra_reward_index = 0;
			need_draw_num = 0;
		}
		int extra_reward_index;	//���⽱��index
		int need_draw_num;		//����齱����

		//����
		std::vector<ItemConfigData> reward_item_vec;
	};
	//���⽱���׶�����
	struct ExtraPhaseCfg
	{
		ExtraPhaseCfg()
		{
			m_extra_reward_count = 0;
		}
		int m_extra_reward_count;
		ExtraRewardCfg m_extra_reward_cfg_[SHAN_HAI_JU_BAO_EXTRA_REWARD_MAX];
	};
	
};

class RAShanHaiJuBaoConfig : public RandActivityConfig
{
public:
	RAShanHaiJuBaoConfig();
	virtual ~RAShanHaiJuBaoConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAShanHaiJuBao::OtherCfg & GetOtherCfg() const { return m_other_cfg; };
	const RAShanHaiJuBao::DrawItemCfg * GetRandDrawCfg(RandActivityManager* ramgr, int phase_seq) const;
	const RAShanHaiJuBao::ExtraRewardCfg * GetExtraRewardCfg(int phase_seq, int extra_reward_index) const;
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitDrawCfg(TiXmlElement *RootElement);
	int InitExtraRewardCfg(TiXmlElement *RootElement);


	RAShanHaiJuBao::OtherCfg m_other_cfg;
	RAShanHaiJuBao::SectionCfgMap m_section_map;

	//���⽱���׶�����[��׶�]
	RAShanHaiJuBao::ExtraPhaseCfg m_extra_phase_cfg_[SHAN_HAI_JU_BAO_EXTRA_PHASE_MAX + 1];
};

#endif		//__RA_SHAN_HAI_JU_BAO_CONFIG_HPP__