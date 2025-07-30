#ifndef __SHI_TU_CONFIG_HPP__
#define __SHI_TU_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/shitudef.h"
#include <map>

struct ShiTuOtherCfg
{
	ShiTuOtherCfg() 
	{
		graduation_level = 0;
		graduation_day = 0;
		shi_tu_fb_scene_id = 0;
		shi_tu_fb_role_num = 0;
		shi_tu_fb_create_monster_num = 0;
		shi_tu_fb_limit_time = 0;
		force_break_relation_time = 0;
		force_break_relation_offline_time = 0;
		force_break_relation_consume_gold = 0;
	}

	int graduation_level;			//出师等级
	int graduation_day;			//可以出师天数
	int shi_tu_fb_scene_id;			
	int shi_tu_fb_role_num;
	int shi_tu_fb_create_monster_num;
	int shi_tu_fb_limit_time;
	int force_break_relation_time;		//强制解除冷却时间
	int force_break_relation_offline_time;		//强制解除
	int force_break_relation_consume_gold;
};


struct ShiTuRewardConfig
{
	ShiTuRewardConfig() 
	{		
		seq = 0;
		need_day = 0;
	}
	
	int seq;
	int need_day;
	ItemConfigData apprentice_reward;
	ItemConfigData master_reward;
};

struct ShiTuFbMonsterItemCfg
{
	ShiTuFbMonsterItemCfg()
	{
		group = 0;
		index = 0;
		monster_id = 0;
	}

	int group;
	int index;
	int monster_id;
};

struct ShiTuFbMonsterCfg
{
	ShiTuFbMonsterCfg()
	{
		min_level = 0;
		max_level = 0;
	}

	int min_level;
	int max_level;

	std::map<int, std::vector<ShiTuFbMonsterItemCfg> > monster_cfg;
};

struct ShiTuFbRewardCfg
{
	ShiTuFbRewardCfg()
	{
		master_coin = 0;
		master_xia_yi_zhi = 0;
		tu_id_coin = 0;
	}

	int master_coin;
	int master_xia_yi_zhi;
	std::vector<ItemConfigData> master_reward;

	int tu_id_coin;
	std::vector<ItemConfigData> tu_di_reward;
};
//师徒-里程
struct ShiTuMileCfg
{
	ShiTuMileCfg()
	{
		index = 0;
		type = 0;
		param = 0;
	}

	int index;		//序号
	int type;
	int param;
	std::vector<ItemConfigData> apprentice_reward;
	std::vector<ItemConfigData> master_reward;
};
//师徒传功-其他
struct ShiTuChuanGongOtherCfg
{
	ShiTuChuanGongOtherCfg()
	{
		week_apprentice_exp_times = 0;
		scene_id = 0;

		game_ready_time = 0;
		game_time = 0;

		day_mile_fetch_num = 0;
	}

	int week_apprentice_exp_times; //徒弟 每周经验奖励次数

	int scene_id;		//场景id
	Posi born_p;		//初始点

	int game_ready_time;//游戏准备时间
	int game_time;		//游戏时间

	int day_mile_fetch_num;	//每日 里程奖励领取限制
};
//师徒传功-经验奖励
struct ShiTuChuanGongExpCfg
{
	ShiTuChuanGongExpCfg()
	{
		level = 0;
		reward_exp = 0;
	}

	int level;
	int reward_exp;
};

//师徒传功-游戏星级奖励
struct ShiTuChuanGongGradeCfg
{
	ShiTuChuanGongGradeCfg()
	{
		grade = 0;
		score_begin = 0;
	}

	int grade;		//星级
	int score_begin;//积分分段 起始
	std::vector<ItemConfigData> master_reward_vec;
	std::vector<ItemConfigData> apprentice_reward_vec;
};


// -----------------------------------------------------------------------------------
class ShiTuConfig : public ILogicConfig
{
public:
	ShiTuConfig();
	virtual ~ShiTuConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ShiTuOtherCfg & GetOtherCfg();
	const std::vector< ShiTuRewardConfig> & GetLevelRewardItem();		//获取徒弟升级奖励配置

	std::vector<ShiTuFbMonsterItemCfg> GetRandMonsterList(int role_level, int num);
	const ShiTuFbRewardCfg * GetRewardCfg(int num);

	const ShiTuMileCfg * GetShiTuMileCfg(int index) const;

	const ShiTuChuanGongOtherCfg & GetChuanGongOtherCfg() const;
	const ShiTuChuanGongExpCfg * GetChuanGongExpCfg(int role_level) const;

	int GetChuanGongGradeCount() const { return (int)m_chuan_gong_grade_vec.size(); };
	const ShiTuChuanGongGradeCfg * GetChuanGongGradeCfg(int grade) const;



private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitLevelRewardCfg(TiXmlElement *RootElement);
	int InitFbMonsterCfg(TiXmlElement * RootElement);
	int InitFbRewardCfg(TiXmlElement * RootElement);

	int InitMileCfg(TiXmlElement *RootElement);


	int InitChuanGongOtherCfg(TiXmlElement *RootElement);
	int InitChuanGongExpCfg(TiXmlElement *RootElement);
	int InitChuanGongGradeCfg(TiXmlElement *RootElement);


	ShiTuOtherCfg m_other_cfg;
	std::vector< ShiTuRewardConfig> m_reward_cfg;
	std::vector<ShiTuFbMonsterCfg> m_monster_cfg;    //全等级怪物配置
	std::vector<ShiTuFbRewardCfg> m_fb_reward_cfg;    //全等级怪物配置


	std::vector<ShiTuMileCfg> m_mile_cfg;			//里程[序号]
	
	ShiTuChuanGongOtherCfg m_chuan_gong_other;		////师徒传功-其他
	std::vector<ShiTuChuanGongExpCfg> m_chuan_gong_exp_vec;		//师徒传功-经验奖励[角色等级]
	//师徒传功-游戏星级奖励map<积分, 奖励>
	typedef std::vector<ShiTuChuanGongGradeCfg> ShiTuChuanGongGradeVec;
	ShiTuChuanGongGradeVec m_chuan_gong_grade_vec;
};

#endif