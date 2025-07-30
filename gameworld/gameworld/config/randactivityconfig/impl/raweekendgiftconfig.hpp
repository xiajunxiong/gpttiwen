#ifndef __RA_WEEKEND_GIFT_CONFIG_HPP__
#define __RA_WEEKEND_GIFT_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/raweekendgiftdef.hpp"

class RandActivityManager;

namespace RAWeekendGift
{
	//�����ר���������ͬһ����ҳ
	struct OtherCfg
	{
		OtherCfg()
		{
			duration_time = 0;
			cfg_ver = 0;
			is_weekend_gift_open = 0;
		}

		int duration_time;
		int cfg_ver;

		//(����-��ĩ���) -  �Ƿ���
		//0 �ر�
		//1 ���� �� (����-��ĩ���:������) �� ��ĩ: ÿ���������������
		int is_weekend_gift_open;
	};


	/*
	�ṹ ����ʱ�� map:	�������� 
						�ȼ��׶� map:	��ɫ�ȼ�
										�׶�����vec

	*/
	//�׶�����
	struct PhaseCfg
	{
		PhaseCfg()
		{
			day_section_begin = 0;
			role_level_section_begin = 0;

			chong_zhi_min = 0;
			chong_zhi_max = 0;

			gift_group_id = 0;
		}

		int day_section_begin;		//�׶ε���ʼ��������
		int role_level_section_begin;//��ɫ�ȼ��� ��ʼ
		
		//��ɫ��ֵ������
		int chong_zhi_min;
		int chong_zhi_max;

		//�����id
		int gift_group_id;
	};

	//�׶�����vec
	typedef std::vector<PhaseCfg> PhaseCfgVec;

	//�ȼ��ֶ�map<level_begin, �׶�����vec>
	typedef std::map<int, PhaseCfgVec> SectionLevelMap;

	//���������ֶ�map<��������, �ȼ��ֶ�map>
	typedef std::map<int, SectionLevelMap> SectionDayMap;

	//�������
	struct GiftCfg
	{
		GiftCfg()
		{
			gift_type_index = 0;
			limit_num = 0;
			buy_money = 0;
		}
		int gift_type_index;	//�������

		int limit_num;			//�޹�����

		int buy_money;			//�����ֵ(Ԫ��)

		//����
		std::vector<ItemConfigData> item_vec;	//��Ʒ
	};

	//����������
	struct GroupCfg
	{
		GroupCfg()
		{
			group_id = 0;
			total_power = 0;
		}
		int group_id;

		int total_power;

		//�����<Ȩ��end, �������>
		typedef std::map<int, int> RandPool;
		RandPool gift_index_rand_pool;
	};
	//������map <������id, ����������>
	typedef std::map<int, GroupCfg> GroupIDMap;
};

class RAWeekendGiftConfig : public RandActivityConfig
{
public:
	RAWeekendGiftConfig();
	virtual ~RAWeekendGiftConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAWeekendGift::OtherCfg & GetOtherCfg() const { return m_other_cfg; };

	//���������������������
	const int GetRandGiftIndex(RandActivityManager* ramgr, int role_level, int chong_zhi_value) const;

	const RAWeekendGift::GiftCfg * GetGiftCfg(int gift_type_index) const;

private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitGiftCfg(TiXmlElement * RootElement);
	int InitPhaseCfg(TiXmlElement * RootElement);

	RAWeekendGift::OtherCfg m_other_cfg;

	//�������vec[�������]
	std::vector<RAWeekendGift::GiftCfg> m_gift_cfg_vec;
	RAWeekendGift::GroupIDMap m_group_id_map;
	RAWeekendGift::SectionDayMap m_section_day_cfg;


};
#endif