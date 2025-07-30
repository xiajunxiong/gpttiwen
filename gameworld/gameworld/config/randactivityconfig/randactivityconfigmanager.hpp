#ifndef __RAND_ACTIVITY_CONFIG_MANAGER_HPP__
#define __RAND_ACTIVITY_CONFIG_MANAGER_HPP__

#include "servercommon/activitydef.hpp"
#include "randactivityconfig.hpp"
#include "servercommon/mysterioustrialdef.hpp"
#include "servercommon/loginsystemdef.hpp"

static int const MAX_RAND_ACTIVITY_CFG_COUNT = RAND_ACTIVITY_TYPE_MAX;

struct RandActivityOtherCfg
{
	RandActivityOtherCfg() :extra_turntable_time(0), kill_boss_num(0), monthly_top_up_ingot(0), spend_ingots_once(0), mysterious_cfg_ver(0), month_investment_cfg_ver(0)
	{}

	int extra_turntable_time;			//��һ�����ת�̵�ʱ��
	int kill_boss_num;					//���û�ɱ����ȡһ��ת�̻���
	int monthly_top_up_ingot;			//�¶�Ͷ�������ֵ��Ԫ��
	int spend_ingots_once;				//����ת��ת��һ�������Ԫ��

	int mysterious_cfg_ver;				//���������汾��
	int month_investment_cfg_ver;		//�¶�Ͷ�ʰ汾��

	int universal_card_item_id;			//���ܼ������ID
};


struct RAPatrolMiningCfg			//Ѳ����ɼ�
{
	RAPatrolMiningCfg() :activity_type(0), rate(0)
	{}

	int activity_type;			//�����
	int rate;					//Ȩ��
	ItemConfigData item;		//��û�����ĵ���
};

struct RAFieldBossCfg			//Ұ��BOSS
{
	RAFieldBossCfg() :activity_type(0), rate(0)
	{}

	int activity_type;			//�����
	int rate;					//Ȩ��
	ItemConfigData item;		//��û�����ĵ���
};

struct RACrystalBossCfg			//ˮ������BOSS
{
	RACrystalBossCfg() :activity_type(0), rate(0)
	{}

	int activity_type;			//�����
	int rate;					//Ȩ��
	ItemConfigData item;		//��û�����ĵ���
};

struct RAPatrolMiningMoreCfg			//����Ѳ����ɼ�
{
	RAPatrolMiningMoreCfg() :activity_type(0), prize_id(0) {}

	int activity_type;			//�����
	int prize_id;				//����ID
	int weight;
};

struct RAFieldCrystalFBMoreCfg			//����Ұ��ˮ������
{
	RAFieldCrystalFBMoreCfg() :activity_type(0), prize_id(0) {}

	int activity_type;			//�����
	int prize_id;				//����ID
	int weight;
};

struct RAMorePrizeCfg			//��Ӫ�������
{
	RAMorePrizeCfg() {}

	int weight;
	ItemConfigData item;
};

struct ScoreReward
{
	ScoreReward() :seq(0), score(0), reward_count(0) {}

	int seq;
	int score;									//��������
	int reward_count;
	ItemConfigData reward[10];					//���߽���
};

struct MonthInvestmentCfg
{
	MonthInvestmentCfg() :days(0), seq(0)
	{}

	int days;
	int seq;
	ItemConfigData item;
};

struct MonthPurchaseRewardCfg
{
	MonthPurchaseRewardCfg() :seq(0)
	{}

	int seq;
	ItemConfigData item;
};


struct MysteriousTrialCfg
{
	MysteriousTrialCfg() :start_time(0), seq(0), activity_type(0), parameter1(0), parameter2(0), parameter3(0), parameter4(0), reward_id(0)
	{}

	int start_time;							// ��һ��
	int seq;
	int activity_type;						// �����
	int parameter1;							// 0:ֱ���ж�   1:�ۼ�  2:���μ���	3:ֱ�ӵ���
	int parameter2;							// Ŀ��ֵ
	int parameter3;							// �����ж�
	int parameter4;							// �����ж�
	int reward_id;							// ����ID
};

struct MysteriousTrialRewardCfg
{
	MysteriousTrialRewardCfg() : create_timestamp(0)
	{}

	unsigned int create_timestamp;
	std::vector<ItemConfigData> reward_list;
};

struct MysteriousTrialConfig
{
	MysteriousTrialConfig() {}

	MysteriousTrialCfg node_cfg[RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM];
};

struct MysteriousTrialFinallyCfg
{
	MysteriousTrialFinallyCfg() :seq(0), request_score(0)
	{}

	int seq;
	int request_score;				// Ŀ��ֵ
	ItemConfigData item;			// ��������
};

struct MysteriousTraialBuyCfg			//��������ÿ���ۿ�
{
	MysteriousTraialBuyCfg() : start_time(0), price(0)
	{}

	int start_time;			// ��ʼʱ��(��һ��)
	int price;				// �۸�
	ItemConfigData item;
};

struct RALoginSystemRewardCfg
{
	RALoginSystemRewardCfg(): seq(0), login_days(0)
	{}

	int seq;
	int login_days;
	std::vector<ItemConfigData> reward_list;
};

struct RALoginSystemRewardGroupCfg
{
	RALoginSystemRewardGroupCfg() : open_server_timestamp(0)
	{}

	unsigned int open_server_timestamp;
	std::map<int, RALoginSystemRewardCfg> login_reward;
};


struct InvalidTimeActivityCfg					//������Ʒ��Ϣ
{
	InvalidTimeActivityCfg() :seq(-1), item_id(0), activity_id(0) {};

	int seq;
	ItemID item_id;
	int activity_id;

	static int count;		//��¼������Ʒ����
};

struct InvalidTimeItemCfg
{
	InvalidTimeItemCfg() :activity_id(), local_activity(false) {};

	int activity_id;
	bool local_activity;
};

class RandActivityConfigManager : public ILogicConfig
{
public:
	RandActivityConfigManager();
	virtual ~RandActivityConfigManager();

	virtual bool Init(const std::string &configname, std::string *err);
	bool InitSeoarateCfg(const std::string &configname, std::string *err);

	const RandActivityConfig * GetRandActivityConfig(int rand_activity_type);

	const RandActivityOtherCfg & GetRandActivityOtherCfg()const { return m_rand_activity_other_cfg; }

	int GetActOpenDay(RandActivityManager* ramgr, int activity_type) const;				//��ǰ�����ʼʱ������
	int GetActOpenDayByTime(RandActivityManager * ramgr, int activity_type,time_t time) const;	//����time�����ʼʱ������
	int GetActRealOpenDay(RandActivityManager* ramgr, int activity_type) const;				//��ǰ������ʱ�������

	// [��ȷ��|�޷���������]--------------------------------
	//---------------------------------������,���淨�и���һ����----------------------
	bool GetPatrolMiningCfg(RandActivityManager* ramgr, std::vector<ItemConfigData>* list);
	bool GetFieldBossCfgById(RandActivityManager* ramgr, int monster_id, std::vector<ItemConfigData>* list);
	bool GetCrystalBossCfgById(RandActivityManager* ramgr, int monster_id, std::vector<ItemConfigData>* list);
	//---------------------------------������,���淨�и���һ����(����)----------------------
	bool GetPatrolMiningMoreCfg(RandActivityManager* ramgr, std::vector<ItemConfigData>* list);
	bool GetFieldCrystalFBMoreCfgById(RandActivityManager* ramgr, int monster_id, std::vector<ItemConfigData>* list);
	const RAMorePrizeCfg* GetMorePrizeCfg(int prize_id);
	///////////////////////////////////////  �¶�Ͷ��  ///////////////////////////////////
	const MonthInvestmentCfg * GetMonthInvestment(int seq)const;
	const std::vector<MonthPurchaseRewardCfg> * GetMonthPurchaseReward()const { return &m_month_purchase_reward; }
	///////////////////////////////////////  ���ֻ  ///////////////////////////////////////
	const ScoreReward* GetScoreReward(int seq);
	////////////////////////////////////  ��������(����(ģ�鹦��))  /////////////////////////////////
	const MysteriousTrialCfg* GetMysteriousTrialCfg(int type, int day, int seq)const;
	const std::map<int, MysteriousTrialConfig >* GetMysteriousTrialCfg(int type)const;
	const MysteriousTrialRewardCfg * GetMysteriousTrialRewardCfg(int reward_id, unsigned int create_timestamp) const;
	const MysteriousTrialFinallyCfg * GetMysteriousTrialFinallyReward(int type, int seq)const;
	const MysteriousTraialBuyCfg * GetMysteriousTrialBuyCfg(int type, int day)const;
	////////////////////////////////////////// �ۼƵ�½  /////////////////////////////////////
	const RALoginSystemRewardCfg* GetLoginSystemRewardBySeq(int login_day, const UniqueServerID& usid)const;
	////////////////////////////////////////// ������Ʒ  /////////////////////////////////////
	const InvalidTimeActivityCfg* GetInvalidTimeActivity(int _seq);
	bool IsItemInvalid(ItemID _item_id);			// �����id�Ƿ��ڹ��ڽṹ��
	int GetActivityId(ItemID _item_id);				// ���������Ҫ���ڱ�����update�����ʱ�临�Ӷ�̫��
	const InvalidTimeItemCfg * GetGroupListActivityId(ItemID _item_id);
private:
	int InitRandActivityOtherCfg(TiXmlElement *RootElement);
	int InitPatrolMiningCfg(TiXmlElement *RootElement);
	int InitFieldBossCfg(TiXmlElement *RootElement);
	int InitCrystalBossCfg(TiXmlElement * RootElement);
	int InitPatrolMiningMoreCfg(TiXmlElement *RootElement);
	int InitFieldCrystalFBCfg(TiXmlElement * RootElement);
	int InitMorePrizeCfg(TiXmlElement * RootElement);
	int InitScoreRewardCfg(TiXmlElement * RootElement);
	int InitMonthInvestment(TiXmlElement * RootElement);
	int InitMonthPurchaseReward(TiXmlElement * RootElement);
	int InitMysteriousTrialRewardfg(TiXmlElement *RootElement);
	int InitMysteriousTrialCfg(TiXmlElement *RootElement);
	int InitMysteriousTrialFinallyCfg(TiXmlElement *RootElement);
	int InitMysteriousTrialBuyCfg(TiXmlElement *RootElement);
	int InitLoginSystemRewardCfg(TiXmlElement * RootElement);
	int InitInvalidTimeActivity(TiXmlElement * RootElement);

	void Release();

	inline bool CheckRandActivity(int activity_type)
	{
		return activity_type - RAND_ACTIVITY_TYPE_BEGIN >= MAX_RAND_ACTIVITY_CFG_COUNT || activity_type - RAND_ACTIVITY_TYPE_BEGIN < 0;
	}

	RandActivityOtherCfg m_rand_activity_other_cfg;

	RandActivityConfig * m_rand_activity_cfg_arr[MAX_RAND_ACTIVITY_CFG_COUNT];

	// ������ͨ�� ���� û�л�ŵ�

	std::vector<RAPatrolMiningCfg> m_patrol_mining_cfg;									// �������,�������(Ѳ����ɼ�)
	std::map<int, std::vector<RAFieldBossCfg> > m_field_boss_map;						// ��boss_idΪkey �������,�������(Ұ��BOSS)
	std::map<int, std::vector<RACrystalBossCfg> > m_crystal_boss_map;					// ��boss_idΪkey �������,�������(ˮ������boss)

	std::vector<RAPatrolMiningMoreCfg> m_patrol_mining_more_cfg;						// �������,�������(����Ѳ����ɼ�)
	std::map<int, std::vector<RAFieldCrystalFBMoreCfg> > m_field_crystal_fb_map;		// ��boss_idΪkey �������,�������(����Ұ��ˮ������)
	std::map<int, std::vector<RAMorePrizeCfg> > m_more_prize_map;						// ��prize_id(����ID)Ϊkey �������,�������

	std::vector<ScoreReward> m_score_reward_vec;										// ���ֻ

	MonthInvestmentCfg m_month_investment_config[RA_MONTH_INVESTMENT_MAX_NUM];			// �¶�Ͷ����������
	std::vector<MonthPurchaseRewardCfg> m_month_purchase_reward;						// �¶�Ͷ�ʹ�����

	std::map<int, std::map<int, MysteriousTrialConfig> > m_mysterious_cfg;								// ����������ͨ���� key1: type vec:��day(start_time)Ϊ��		
	std::map<int, std::map<int, MysteriousTrialFinallyCfg> > m_mysterious_trial_finally_cfg;			// �����������ս��� key1: type key2:��seqΪ��
	std::map<int, std::map<int, MysteriousTraialBuyCfg> > m_mysterious_trial_buy_cfg;					// ��������ÿ���ۿ� key1:type key2:start_time --7
	std::map<int, std::vector<MysteriousTrialRewardCfg> > m_mysterious_trial_reward_cfg;				// ���������������� key:reward_id

	std::vector<RALoginSystemRewardGroupCfg> m_login_system_vec;

	std::map<int, InvalidTimeActivityCfg> m_seq_to_id_info;									// ����       -->   ������Ʒ��Ϣ
	std::map<ItemID, bool> m_id_to_exsit;													// item_id    -->   �Ƿ����
	std::map<ItemID, int> m_itemid_to_actiid;												// item_id    -->   ���Ʒid
	InvalidTimeItemCfg item_activity[65535];																// item_id	  -->   ���Ʒid
};

#endif	//__RAND_ACTIVITY_CONFIG_MANAGER_HPP__


