#ifndef __QING_YUAN_CONFIG_HPP__
#define __QING_YUAN_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"

#include "servercommon/configcommon.h"
#include "servercommon/weddingdef.hpp"
#include "gamedef.h"

#include <set>

namespace WeddingFB
{
	//��������
	struct BaseCfg
	{
		BaseCfg()
		{
			scene_id = 0;
			barrage_heat_add_times = 0;
			barrage_heat_add_num = 0;

			meal__interval_time_s = 0;
			meal__max_num = 0;

			base_invite_max = 0;
			buy_invite_max_add = 0;
			buy_invite_max_times = 0;

			buy_invite_max_cost_type = 0;
			buy_invite_max_cost_num = 0;

			weddding_suit_id = 0;
		}
		int scene_id;	//����id
		
		Posi role_born;//������
		
		int barrage_heat_add_times;	// ��Ļ�������ȶȵĴ���
		int barrage_heat_add_num;	// ��Ļ���ӵ��ȶ�

		ItemConfigData candies;		// ϲ��
		// �ò�
		unsigned int meal__interval_time_s;// �òͼ��
		int meal__max_num;// ����òʹ���
		std::vector<int> meal__group_id_list;	// �òͽ���

		//��������
		int base_invite_max;		//������������
		int buy_invite_max_add;		//������չ��
		int buy_invite_max_times;	//����չ����

		int buy_invite_max_cost_type;//������չ���Ļ�������
		int buy_invite_max_cost_num;//������չ����


		int weddding_suit_id;		//������װid
	};

	//���绷������
	struct LinkCfg
	{
		LinkCfg()
		{
			link_seq = 0;
			phase_type = 0;
			param_1 = 0;

			link_time = 0;

			memset(common_oper_cfg_, 0, sizeof(common_oper_cfg_));
		}
		int link_seq;	//����˳�� [1,]

		int phase_type;	//�׶�����
		int param_1;	//�׶β���

		int link_time;	//����ʱ��

		//������������[����]
		int common_oper_cfg_[COMMON_OPER_TYPE_MAX];
	};

	//ף����Ʒ
	struct BlessShopItemCfg
	{
		BlessShopItemCfg()
		{
			shop_item_index = 0;

			cost_type = 0;
			cost_num = 0;

			add_heatt = 0;
		}

		int shop_item_index;	//��Ʒindex [0, ]

		int cost_type;			//���Ļ�������
		long long cost_num;		//��������

		int add_heatt;			//�����ȶ�
	};

	//�ȶȽ���
	struct HeatRewardCfg
	{
		HeatRewardCfg()
		{
			reward_seq = 0;
			need_hot = 0;
		}

		int reward_seq;	//�������[1,30]
		int need_hot;	//�����ȶ�
		std::vector<ItemConfigData> reward_vec;//����
	};
	
	//////С��Ϸ
	// Ѱ���ٰ�
	struct SubActXubQingMiAi
	{
		SubActXubQingMiAi()
		{
		}
		std::vector<int> item_list;	//������id vec

		std::vector<Posi> gift_pos;
	};
	// ������ӡ
	struct SubActXinXinXiangYin
	{
		SubActXinXinXiangYin()
		{
		}
		std::vector<int> item_list;	//������id vec
	};
	// ��������
	struct SubActBaoWeiXiuQiu
	{
		SubActBaoWeiXiuQiu()
		{
			reward_index = 0;
			npc_id = 0;
			monster_group_id = 0;
		}
		int reward_index;
		int npc_id;
		int monster_group_id;
		std::vector<ItemConfigData> item_list;	//������id vec
	};

	// ��������
	struct SubActZaoShengGuiZi
	{
		SubActZaoShengGuiZi()
		{
			reward_index = 0;
		}
		int reward_index;
		std::vector<ItemConfigData> item_list;	//������id vec
	};

	//����--ԤԼʱ���(ÿ��)
	struct WeddingRegistCfg
	{
		WeddingRegistCfg()
		{
			index = 0;
			begin_day_second = 0;
		}
		int index;
		int begin_day_second;	//��ʼ�ĵ�������
	};
	//����--ԤԼʱ��map <��ʼ�ĵ�������, ����>
	typedef std::map<time_t, WeddingRegistCfg> WeddingRegistCfgMap;

	//Ѳ������
	struct XunYouCfg
	{
		XunYouCfg()
		{
			xun_you_car_type = 0;

			scene_id = 0;
			begin_dir = 0;
			speed = 0;

			cost_type = 0;
			cost_num = 0;

			gift_gather_id = 0;
			gift_valid_time = 0;
			creat_gift_num = 0;
		}
		int xun_you_car_type;	//��������[1,

		int scene_id;	//Ѳ�γ���id
		Dir begin_dir;	//��ʼ����
		int speed;		//�ƶ��ٶ�

		//����
		int cost_type;//��������
		int cost_num;//��������


		//��;��������(�ɼ���)
		int gift_gather_id;	//����ɼ���id
		int gift_valid_time;//������Чʱ��(s)
		int creat_gift_num;	//���α�������
		std::vector<int> gift_group_id_list;//���佱��

	};
	//Ѳ��·��
	struct XunYouWayPoint
	{
		XunYouWayPoint()
		{
			way_point_index = 0;
			stop_wait_time = 0;
		}
		int way_point_index;	//·����index
		Posi way_poi;			//·��������

		int stop_wait_time;//ԭ�صȴ�ʱ��

		//���䴴������
		std::vector<Posi> creat_gift_vec;
	};


	//��������Ʒ
	struct GroupItemCfg
	{
		GroupItemCfg()
		{
			group_id = 0;
			power = 0;

			coin_bind = 0;
		}
		int group_id;	//������id
		int power;		//��Ȩ��

		//����
		int coin_bind;	//ͭ��
		ItemConfigData reward_item;	//��Ʒ
	};

	//������ ����map<Ȩ��_begin, ��������Ʒ>
	typedef std::map<int, GroupItemCfg>  GroupItemPool;
	//������
	struct GroupCfg
	{
		GroupCfg()
		{
			group_id = 0;

			total_power = 0;
		}
		int group_id;

		int total_power;	//������Ȩ��
		GroupItemPool item_pool;
	};
	//������id map<������id, ������>
	typedef std::map<int, GroupCfg>  GroupIDMap;
}

struct QingYuanMarryCfg				//��Ե����
{
	int marry_type;
	int money_type;
	int money_value;
	std::vector<ItemConfigData> rewards;
};

struct QingYuanDurationRewardCfg			//��Եʱ������
{
	int need_marry_day;
	std::vector<ItemConfigData> rewards;
};

struct QingYuanTitleCfg
{
	int title_id;
	int need_intimacy;
	int need_day;
};

struct QingYuanSkillCfg			//��Ե��������
{
	int seq;
	int need_intimacy;
	int skill_id;
};

struct QingYuanOtherCfg			
{
	int qing_yuan_need_intimacy; // ��Ե��Ҫ�����ܶ�
	int qing_yuan_leave_wait_time;		// �����Ҫ�ȴ�ʱ��
	int qing_yuan_logout_duration_can_leave;		// ���߶���� ���������
	int qing_yuan_force_leave_comsume_gold;		// ǿ�������Ҫ��Ԫ��
};

struct QingYuanGiftCfg
{
	int qing_yuan_gift_type;
	int need_money_value;
	int need_money_type;
	int during_day;  //����������

	int active_reward_money_type;
	int active_reward_money_value;
	std::vector<ItemConfigData> day_rewards;
};

class QingYuanConfig : public ILogicConfig
{
public:
	QingYuanConfig();
	virtual ~QingYuanConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	//���ð汾
	int GetCfgVer() const { return 1; };

	//�����׼ʱ��
	bool CheckWeddingBeginTime(time_t begin_time);

	const WeddingFB::BaseCfg & GetFBBaseCfg()const { return m_fb_base_cfg; };
	const WeddingFB::LinkCfg * GetFBLinkCfg(int link_seq) const;
	//���繫����������
	int GetFBCommonOperParam(int link_seq, int common_oper) const;
	time_t GetFbPrePareTime() const { return m_fb_pre_pare_time_s; };
	time_t GetFbTotalTime() const { return m_fb_total_time_s; };
	//ף��
	const WeddingFB::BlessShopItemCfg * GetFBBlessShopItemCfg(int shop_index) const;
	const WeddingFB::HeatRewardCfg * GetFBHeatRewardCfg(int reward_seq) const;
	//С��Ϸ
	//Ѱ���ٰ�
	const WeddingFB::SubActXubQingMiAi &GetSubActXubQingMiAi() const;
	//������ӡ
	const WeddingFB::SubActXinXinXiangYin &GetSubActXinXinXiangYin() const;

	//��������
	const WeddingFB::SubActBaoWeiXiuQiu *GetSubActBaoWeiXiuQiu(int npc_id) const;
	const std::vector<int>& GetSubActBaoWeiXiuQiuNpcIDVec() const;
	// ��������
	const WeddingFB::SubActZaoShengGuiZi *GetSubActZaoShengGuiZi(int index) const;
	// С��Ϸ ���У��
	bool IsSubActFinish(int sub_act_type, int act_param);

	//Ѳ��
	const WeddingFB::XunYouCfg * GeXunYouCfg(int xun_you_type) const;
	const WeddingFB::XunYouWayPoint * GeXunYouWayPoint(int way_point_index) const;


	const WeddingFB::GroupItemCfg * GetGroupRandCfg(int group_id) const;

	const QingYuanMarryCfg * GetQingYuanMarryCfg(int marry_type);
	const QingYuanOtherCfg & GetOtherCfg();
	const QingYuanGiftCfg * GetQingYuanGiftCfg(int gift_type);
	std::vector<QingYuanDurationRewardCfg> GetQingYuanDurationCfg(int last_day,int now_day);
	std::set<int> GetQingYuanTitle(int intimacy, int day);
	std::vector<QingYuanSkillCfg> GetQingYuanSkillCfg(int intimacy);
public:
	int InitFBBaseCfg(TiXmlElement *RootElement);
	int InitFBLinkCfg(TiXmlElement *RootElement);
	int InitFBBlessShopCfg(TiXmlElement *RootElement);
	int InitFBHeatRewardCfg(TiXmlElement *RootElement);
	int InitWeddingRegistCfg(TiXmlElement *RootElement);

	int InitXubQingMiAiCfg(TiXmlElement *RootElement);
	int InitXinXinXiangYinCfg(TiXmlElement *RootElement);
	int InitBaoWeiXiuQiuCfg(TiXmlElement *RootElement);
	int IniZaoShengGuiZiCfg(TiXmlElement *RootElement);

	int InitXunYouCfg(TiXmlElement *RootElement);
	int InitXunYouWayPointCfg(TiXmlElement *RootElement);


	int InitGroupCfg(TiXmlElement *RootElement);

	int InitOtherCfg(TiXmlElement * RootElement);
	int InitQingYuanCfg(TiXmlElement * RootElement);
	int InitQingYuanTitleCfg(TiXmlElement * RootElement);
	int InitQingYuanDurationRewardCfg(TiXmlElement * RootElement);
	int InitQingYuanGiftCfg(TiXmlElement * RootElement);
	int InitQingYuanSkillCfg(TiXmlElement * RootElement);


	//����--������������
	WeddingFB::BaseCfg m_fb_base_cfg;
	//����--��ǰ׼��ʱ��
	time_t m_fb_pre_pare_time_s;
	//����--����ʱ��
	time_t m_fb_total_time_s;
	//����--�����׶�����vec[����˳��seq]
	std::vector<WeddingFB::LinkCfg> m_fb_phase_vec;
	//����--����ף����Ʒvec[��Ʒindex]
	std::vector<WeddingFB::BlessShopItemCfg> m_bless_shop_vec;
	//����--�ȶȽ���vec[�������seq]
	std::vector<WeddingFB::HeatRewardCfg> m_heat_reward_vec;
	//����--ԤԼʱ��map <��ʼ�ĵ�������, ����>
	WeddingFB::WeddingRegistCfgMap m_wedding_regist_cfg_map;

	//С��Ϸ
	// Ѱ���ٰ�
	WeddingFB::SubActXubQingMiAi m_xun_qing_mi_ai_cfg;
	// ������ӡ
	WeddingFB::SubActXinXinXiangYin m_xin_xin_xiang_yin_cfg;
	// �������� <npc_id, ����_index>
	std::map<int, WeddingFB::SubActBaoWeiXiuQiu> m_sub_act_bao_wei_xiu_qiu_map;
	// �������� npc_id[����index]
	std::vector<int> m_sub_act_bao_wei_xiu_qiu_npc_id;
	// ��������
	std::vector<WeddingFB::SubActZaoShengGuiZi> m_zao_sheng_gui_zi_vec;
	//С��Ϸ ��ɱ��
	int m_sub_act_finish_flag[WeddingFB::SUB_ACT_TYPE_MAX];


	//Ѳ������[Ѳ������]
	std::vector<WeddingFB::XunYouCfg> m_xun_you_cfg_vec;
	//Ѳ��·��[·����index]
	std::vector<WeddingFB::XunYouWayPoint> m_xun_you_way_point_vec;


	WeddingFB::GroupIDMap m_group_id_map;
	
	QingYuanOtherCfg m_other_cfg;			
	std::map<int, QingYuanMarryCfg> m_marry_cfg;			//�������
	std::map<int, QingYuanGiftCfg> m_qing_yuan_gift_cfg;			//��Ե����
	std::vector<QingYuanTitleCfg> m_qing_yuan_title_cfg;				//��Ե�ƺ�����
	std::vector<QingYuanDurationRewardCfg> m_qing_yuan_duration_reward_cfg;		//���ʱ������
	std::vector<QingYuanSkillCfg> m_qing_yuan_skill_cfg;		//��Ե����
};

#endif
