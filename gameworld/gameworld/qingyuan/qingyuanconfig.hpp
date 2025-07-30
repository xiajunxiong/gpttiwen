#ifndef __QING_YUAN_CONFIG_HPP__
#define __QING_YUAN_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"

#include "servercommon/configcommon.h"
#include "servercommon/weddingdef.hpp"
#include "gamedef.h"

#include <set>

namespace WeddingFB
{
	//其它配置
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
		int scene_id;	//场景id
		
		Posi role_born;//出生点
		
		int barrage_heat_add_times;	// 弹幕可增加热度的次数
		int barrage_heat_add_num;	// 弹幕增加的热度

		ItemConfigData candies;		// 喜糖
		// 用餐
		unsigned int meal__interval_time_s;// 用餐间隔
		int meal__max_num;// 最大用餐次数
		std::vector<int> meal__group_id_list;	// 用餐奖励

		//婚宴邀请
		int base_invite_max;		//基础邀请人数
		int buy_invite_max_add;		//单次拓展数
		int buy_invite_max_times;	//可拓展次数

		int buy_invite_max_cost_type;//单次拓展消耗货币类型
		int buy_invite_max_cost_num;//单次拓展消耗


		int weddding_suit_id;		//婚宴套装id
	};

	//婚宴环节配置
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
		int link_seq;	//环节顺序 [1,]

		int phase_type;	//阶段类型
		int param_1;	//阶段参数

		int link_time;	//持续时间

		//公共操作配置[类型]
		int common_oper_cfg_[COMMON_OPER_TYPE_MAX];
	};

	//祝福商品
	struct BlessShopItemCfg
	{
		BlessShopItemCfg()
		{
			shop_item_index = 0;

			cost_type = 0;
			cost_num = 0;

			add_heatt = 0;
		}

		int shop_item_index;	//商品index [0, ]

		int cost_type;			//消耗货币类型
		long long cost_num;		//消耗数量

		int add_heatt;			//增加热度
	};

	//热度奖励
	struct HeatRewardCfg
	{
		HeatRewardCfg()
		{
			reward_seq = 0;
			need_hot = 0;
		}

		int reward_seq;	//奖励序号[1,30]
		int need_hot;	//需求热度
		std::vector<ItemConfigData> reward_vec;//奖励
	};
	
	//////小游戏
	// 寻情觅爱
	struct SubActXubQingMiAi
	{
		SubActXubQingMiAi()
		{
		}
		std::vector<int> item_list;	//奖励组id vec

		std::vector<Posi> gift_pos;
	};
	// 心心相印
	struct SubActXinXinXiangYin
	{
		SubActXinXinXiangYin()
		{
		}
		std::vector<int> item_list;	//奖励组id vec
	};
	// 保卫绣球
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
		std::vector<ItemConfigData> item_list;	//奖励组id vec
	};

	// 枣生桂子
	struct SubActZaoShengGuiZi
	{
		SubActZaoShengGuiZi()
		{
			reward_index = 0;
		}
		int reward_index;
		std::vector<ItemConfigData> item_list;	//奖励组id vec
	};

	//婚宴--预约时间段(每日)
	struct WeddingRegistCfg
	{
		WeddingRegistCfg()
		{
			index = 0;
			begin_day_second = 0;
		}
		int index;
		int begin_day_second;	//开始的当日秒数
	};
	//婚宴--预约时间map <开始的当日秒数, 配置>
	typedef std::map<time_t, WeddingRegistCfg> WeddingRegistCfgMap;

	//巡游配置
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
		int xun_you_car_type;	//花车类型[1,

		int scene_id;	//巡游场景id
		Dir begin_dir;	//初始朝向
		int speed;		//移动速度

		//花费
		int cost_type;//货币类型
		int cost_num;//货币数量


		//中途创建宝箱(采集物)
		int gift_gather_id;	//宝箱采集物id
		int gift_valid_time;//宝箱有效时间(s)
		int creat_gift_num;	//单次宝箱数量
		std::vector<int> gift_group_id_list;//宝箱奖励

	};
	//巡游路径
	struct XunYouWayPoint
	{
		XunYouWayPoint()
		{
			way_point_index = 0;
			stop_wait_time = 0;
		}
		int way_point_index;	//路径点index
		Posi way_poi;			//路径点坐标

		int stop_wait_time;//原地等待时间

		//宝箱创建坐标
		std::vector<Posi> creat_gift_vec;
	};


	//奖励组物品
	struct GroupItemCfg
	{
		GroupItemCfg()
		{
			group_id = 0;
			power = 0;

			coin_bind = 0;
		}
		int group_id;	//奖励组id
		int power;		//分权重

		//奖励
		int coin_bind;	//铜币
		ItemConfigData reward_item;	//物品
	};

	//奖励组 奖池map<权重_begin, 奖励组物品>
	typedef std::map<int, GroupItemCfg>  GroupItemPool;
	//奖励组
	struct GroupCfg
	{
		GroupCfg()
		{
			group_id = 0;

			total_power = 0;
		}
		int group_id;

		int total_power;	//奖池总权重
		GroupItemPool item_pool;
	};
	//奖励组id map<奖励组id, 奖励组>
	typedef std::map<int, GroupCfg>  GroupIDMap;
}

struct QingYuanMarryCfg				//结缘配置
{
	int marry_type;
	int money_type;
	int money_value;
	std::vector<ItemConfigData> rewards;
};

struct QingYuanDurationRewardCfg			//情缘时长奖励
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

struct QingYuanSkillCfg			//情缘技能配置
{
	int seq;
	int need_intimacy;
	int skill_id;
};

struct QingYuanOtherCfg			
{
	int qing_yuan_need_intimacy; // 结缘需要的亲密度
	int qing_yuan_leave_wait_time;		// 离婚需要等待时间
	int qing_yuan_logout_duration_can_leave;		// 离线多久能 单方面离婚
	int qing_yuan_force_leave_comsume_gold;		// 强制离婚需要的元宝
};

struct QingYuanGiftCfg
{
	int qing_yuan_gift_type;
	int need_money_value;
	int need_money_type;
	int during_day;  //持续多少天

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

	//配置版本
	int GetCfgVer() const { return 1; };

	//婚宴标准时间
	bool CheckWeddingBeginTime(time_t begin_time);

	const WeddingFB::BaseCfg & GetFBBaseCfg()const { return m_fb_base_cfg; };
	const WeddingFB::LinkCfg * GetFBLinkCfg(int link_seq) const;
	//婚宴公共操作参数
	int GetFBCommonOperParam(int link_seq, int common_oper) const;
	time_t GetFbPrePareTime() const { return m_fb_pre_pare_time_s; };
	time_t GetFbTotalTime() const { return m_fb_total_time_s; };
	//祝福
	const WeddingFB::BlessShopItemCfg * GetFBBlessShopItemCfg(int shop_index) const;
	const WeddingFB::HeatRewardCfg * GetFBHeatRewardCfg(int reward_seq) const;
	//小游戏
	//寻情觅爱
	const WeddingFB::SubActXubQingMiAi &GetSubActXubQingMiAi() const;
	//心心相印
	const WeddingFB::SubActXinXinXiangYin &GetSubActXinXinXiangYin() const;

	//保卫绣球
	const WeddingFB::SubActBaoWeiXiuQiu *GetSubActBaoWeiXiuQiu(int npc_id) const;
	const std::vector<int>& GetSubActBaoWeiXiuQiuNpcIDVec() const;
	// 枣生桂子
	const WeddingFB::SubActZaoShengGuiZi *GetSubActZaoShengGuiZi(int index) const;
	// 小游戏 完成校验
	bool IsSubActFinish(int sub_act_type, int act_param);

	//巡游
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


	//婚宴--副本基础配置
	WeddingFB::BaseCfg m_fb_base_cfg;
	//婚宴--提前准备时长
	time_t m_fb_pre_pare_time_s;
	//婚宴--副本时长
	time_t m_fb_total_time_s;
	//婚宴--副本阶段配置vec[环节顺序seq]
	std::vector<WeddingFB::LinkCfg> m_fb_phase_vec;
	//婚宴--副本祝福商品vec[商品index]
	std::vector<WeddingFB::BlessShopItemCfg> m_bless_shop_vec;
	//婚宴--热度奖励vec[奖励序号seq]
	std::vector<WeddingFB::HeatRewardCfg> m_heat_reward_vec;
	//婚宴--预约时间map <开始的当日秒数, 配置>
	WeddingFB::WeddingRegistCfgMap m_wedding_regist_cfg_map;

	//小游戏
	// 寻情觅爱
	WeddingFB::SubActXubQingMiAi m_xun_qing_mi_ai_cfg;
	// 心心相印
	WeddingFB::SubActXinXinXiangYin m_xin_xin_xiang_yin_cfg;
	// 保卫绣球 <npc_id, 配置_index>
	std::map<int, WeddingFB::SubActBaoWeiXiuQiu> m_sub_act_bao_wei_xiu_qiu_map;
	// 保卫绣球 npc_id[配置index]
	std::vector<int> m_sub_act_bao_wei_xiu_qiu_npc_id;
	// 枣生桂子
	std::vector<WeddingFB::SubActZaoShengGuiZi> m_zao_sheng_gui_zi_vec;
	//小游戏 完成标记
	int m_sub_act_finish_flag[WeddingFB::SUB_ACT_TYPE_MAX];


	//巡游配置[巡游类型]
	std::vector<WeddingFB::XunYouCfg> m_xun_you_cfg_vec;
	//巡游路径[路径点index]
	std::vector<WeddingFB::XunYouWayPoint> m_xun_you_way_point_vec;


	WeddingFB::GroupIDMap m_group_id_map;
	
	QingYuanOtherCfg m_other_cfg;			
	std::map<int, QingYuanMarryCfg> m_marry_cfg;			//结婚配置
	std::map<int, QingYuanGiftCfg> m_qing_yuan_gift_cfg;			//情缘送礼
	std::vector<QingYuanTitleCfg> m_qing_yuan_title_cfg;				//情缘称号配置
	std::vector<QingYuanDurationRewardCfg> m_qing_yuan_duration_reward_cfg;		//结婚时长配置
	std::vector<QingYuanSkillCfg> m_qing_yuan_skill_cfg;		//情缘技能
};

#endif
