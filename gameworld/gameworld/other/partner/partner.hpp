#ifndef __PARTNER_HPP__
#define __PARTNER_HPP__

#include "servercommon/partnerdef.h"
#include "other/attributelist.hpp"
#include "protocol/msgpartner.h"
#include "servercommon/rolepetpartnerdef.hpp"

#include <vector>
#include <set>

struct AttrCon
{
	AttrCon() { this->Reset(); }

	void Reset()
	{
		capability = 0;
		memset(attr_list, 0, sizeof(attr_list));
	}

	int capability;		// 战斗力
	Attribute attr_list[BATTLE_ATTR_MAX];		//总属性
};

//元神契约信息
class PartnerPactInfo
{
	//pact		契约   给本伙伴增加属性
	//bepact	被契约 给契约对象增加属性
	//
	static const int PER_MIL_BASE = 1000;
public:
	PartnerPactInfo();
	~PartnerPactInfo();

	//--重算--附加属性  减旧增新
	//----传入:当前总属性 传出重算后总属性)
	//----传入:新附加属性
	void ReCalcPactAttr(AttrCon &total_attr, Attribute attach_attr_list[PARTNER_ATTR_MAX]);
	void AddNowPactTo(AttrCon &total_attr);

	//清空契约属性
	void ReSetPact();
	//移除契约属性
	void PactAttrOff(AttrCon &total_attr);

	//设置契约基础属性
	void SetBePactBase(Attribute base_attr_list[PARTNER_ATTR_MAX]);
	//添加契约基础属性到 (属性列, 千分比)
	void AddBePactTo(Attribute attach_attr_list[PARTNER_ATTR_MAX], int attr_per_mil);

	//已契约
	bool IsPact(int partner_id);
	//契约新伙伴
	bool PactAdd(int partner_id);
	//解除契约
	void PactOff(int partner_id);

	//被其他伙伴契约
	void BePactAdd(int partner_id);
	//被其他伙伴解除契约
	void BePactOff(int partner_id);

	const std::set<int> &GetNowPactSet() const { return m_now_pact_partner_id_set; }
	const std::set<int> &GetBePactSet() const { return m_be_pact_partner_id_set; }

	void GetNowPactAttr(AttrCon &now_pact_attr);
	void GetNowPactAttr(AttributeList &now_pact_attr);
	void GetBePactBaseAttr(Attribute base_attr_list[PARTNER_ATTR_MAX]);


private:
	//当前装备契约的<伙伴id>
	std::set<int> m_now_pact_partner_id_set;
	//该伙伴此时被增加的契约属性(已经附加到AttrCon)
	Attribute m_now_pact_attr_list[PARTNER_ATTR_MAX];

	//装备了该伙伴契约的<伙伴id>
	std::set<int> m_be_pact_partner_id_set;
	//该伙伴用于计算给其契约目标增加的属性(100%)
	Attribute m_be_pact_base_attr_list[PARTNER_ATTR_MAX];
};

class RoleModuleManager;
struct PlatformBattlePartner;
struct BattleCharacterData;
struct QualityUpgradeCfg;
struct PartnerBasicCfg;
struct PartnerUseElementCrytalFightInfo;
struct PartnerSkinCfgInfo;

class Partner
{
public:
	Partner();
	~Partner();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager * GetRoleModuleManager() { return m_role_module_mgr; }

	void Init(RoleModuleManager * role_module_manager,
		const PartnerParam & p_partner,
		const NewPartnerParam & new_p_partner,
		const PartnerGiftTreeParam & partner_gift_tree,
		const PartnerSpiritPactParam & partner_pact_param
		);
	void InitEnd();
	void GetPartnerParam(PartnerParam * p_partner_param,
		NewPartnerParam * new_p_partner_param,
		PartnerGiftTreeParam * partner_gift_tree_param,
		PartnerSpiritPactParam * partner_pact_param);
	NewPartnerParam GetPartnerParam() { return m_new_partner; }
	std::vector<int> GetActiveParnerList();

	void GetRolePartnerInfoParam(RolePartnerInfoParam * role_partner_info_param);

	bool IsPartnerValid(int partner_id) const;		// 该id是否有效
	bool IsPartnerInvalid(int partner_id) const;		// 该id是否无效

	void Update(time_t now_second);
	void OnPartnerDayChange(unsigned int old_dayid, unsigned int new_dayid);
	void OnRoleLogin();
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);
	void CheckPartnerCollection();


	bool IsPartnerAlreadyActive(int partner_id);								// 查询伙伴是否激活
	int GetCurFightPartnerNum();
	void ActivePartnerByConsumingItem(int partner_id);							// 用道具激活伙伴
	bool ActiveAppointedPartner(int param, int unlock_type, int *activity_partner_id = NULL, bool is_notice = true);	// 激活指定的伙伴
	bool ActivePartnerByOther(int partner_id, const char * reason);				// 由其他模块激活伙伴
	void SendAllPartnerInfo();													// 发送所有伙伴的信息
	void SendSynfightInfoList();												// 发送伙伴出战信息列表
	void ChangePartnerFightState(int partner_id, const char * reason);			// 改变伙伴出战状态
	void PartnerBreakTAndUpGrade(int partner_id);								// 伙伴能力突破进阶，晋升
	void PartnerFettersBaptize(int partner_id);									// 伙伴情缘洗练				// [注释]
	void PartnerFettersResultSave(int partner_id);								// 保存伙伴情缘洗练结果		// [注释]
	void PartnerSkillSeletorSave(int partner_id, int slot, int skill);			// 改变伙伴携带技能			// [注释]
	void PartnerSynFightListAlter(int insert_id, int target_id);				// 调整伙伴出战列表
	void PartnerSwitchFightPost(int partnerA, int partnerB);					// 互换两个伙伴的出战顺序
	void PartnerSkillActivate(int partner_id, short is_activate, short skill_idx); // 伙伴激活技能

	void MaxPartnerHPOrMP(int partner_id, int max_type, bool is_notify = true);	// 刷满当前 HP 或 MP
	bool AlterPartnerHPOrMPValue(int partner_id, int r_type, int r_value, bool is_percent = false);		// 按数值变动指定伙伴的血魔（比如用血药蓝药、特殊事件等，value 正为恢复，负为减损）
	void AlterPartnerHPOrMPPerce(int partner_id, int r_type, double r_value);	// 按比例变动指定伙伴的血魔（value 为 生命\魔力上限 占比）
	void SetPartnerHPOrMPValue(int partner_id, int cur_hp, int cur_mp);
	void AddAllPartnerHpMp(double percent);
	void AddFightPartnerHpMp(double percent);
	void SyncPartnerInfoToTeam(int partner_id, int cur_hp, int max_hp, int cur_mp, int max_mp, int skin_id, int level);

	Attribute GetAttrByPartnerID(int partner_id, int attr_type);				// 获取伙伴当前单种属性
	Attribute* GetAttrListByPartnerID(int partner_id);
	const PartnerSql* GetPartnerDataById(int partner_id);
	const NewPartnerBaseInfo* GetPartnerBaseInfoById(int partner_id);
	int GetPartnerAttrListReason(int partner_id);								// 获取伙伴属性来源列表
	void SendPartnerAttrListReason(short param1);								// 下发伙伴属性来源列表
	void SendPartnerSingleInfo(int partner_id);									// 下发单条伙伴信息

	//伙伴传记
	void OnOpenBiography(int partner_id, int biography_id);						// 打开传记
	void OnFetchBiography(int partner_id, int biography_id);					// 领取传记奖励

	void GetSynFightPartnerIdList(int max_out_num, int * out_list, int * out_num);// 获取出战的伙伴列表
	std::vector<int> GetSynFightPartnerIdList2() const;
	std::set<int> GetSynFightPartnerIdList3() const;
	bool GetBattleCharacterData(int partner_id, PartnerBattleData * out_data, int * fight_list, int fight_num, bool is_use_cur_hp_mp);// 获取对应伙伴的基本属性

	int GetPartnerCurCount();										// 获取已经激活的伙伴数量
	int GetPartnerCurCount(int quality);							// 获得达到等阶的伙伴数量
	int GetMaxParnterGrade();										// 获得当前伙伴中最高的进阶等级
	int GetMaxPartnerNumByGrade(int grade);							// 获得达到该进阶等级的伙伴数量
	int GetActiveJiBanNum() { return m_partner_jiban_num; }			// 获得此时激活羁绊的伙伴数量
	int GetWearSetEquipNum();										// 获得穿戴一套命格卡的伙伴数量(不限制是否相同套装效果)
	int GetPartnerNumByQualityNum(int quailty_num);					// 获得达到觉醒(升品)次数的伙伴数量(次数从伙伴本身初始品质开始计算)

	void ReCalAllPartnerAttr(int notice_type);		// 玩家等级提升后，重算伙伴属性
	int GetMaxFourPartnerCap();	// 获取4个伙伴最高战力总和
	int GetFightParatnerCap();	// 获取当前出战伙伴战力总和

	void OnRoleGuildInbornChange();

	void SyncFightListFromFormation(ARG_IN int* fight_partner_list, ARG_IN int* battle_row_list, int max_num);

	//----------------------------升级----------------------------
	int UpLevel(int partner_id, int num);
	int ResetLevel(int partner_id);

	/************************************************************************/
	/*                               伙伴升品                               */
	/************************************************************************/
	int GetCurrentQuality(int partner_id);																					// 获得当前伙伴的品质
	bool QualityUpgrade(int partner_id, int upgrade_type, bool is_p = true/*是否要增加判断能不能升品*/);						// 伙伴升品
	void AddAttrByQualityUp(AttrCon &_p_att, const QualityUpgradeCfg *_q_cfg);												// 提升属性
	void AddAttrByQualityUp(AttributeList &_p_att, const QualityUpgradeCfg *_q_cfg);										// 提升属性
	void AddQualityByQualityUp(int partner_id);																				// 提升品质
	bool CanQualityUpgrade(int _partner_id, int _upgrade_type, int quality_segment);										// 是否有当前阶，是否可以升品
	void SendQualityUpInfo(int _partner_id, int _upgrade_type, char notice_type);								// 升品信息下发

	// -------------------------------------------------------------- 伙伴装备 -------------------------------------------------------------- //
	void PartnerEqUnlockEquipment(int partnet_id, int slot_id);					// 解锁
	void PartnerEqPullOnEquipment(int partner_id, int grid_id);					// 穿上装备
	void PartnerEqGetOffEquipment(int partner_id, int slot_id);					// 脱下装备
	void PartnerEqResolveEquipment(int grid_idx);								// 分解装备

	/************************************************************************/
	/*                               元素水晶                               */
	/************************************************************************/
	void UseElementCrystalById(int _partner_id,ItemID _crystal_id);														// 使用伙伴水晶
	void UseElementCrystal(int _partner_id, int index /* 物品栏序号*/, bool is_send = true);							// 使用伙伴水晶
	void UseElementCrystalToFight(int index_1, int index_2, int index_3, int index_4);									// 出战伙伴使用伙伴水晶
	void RemoveElementCrystal(int _partner_id, bool is_send = true, bool is_put_knaspack = true);														// 卸下元素水晶
	void RemoveAllCurPartnerElementCrystal();																			// 一键卸下所有激活伙伴的元素水晶
	void ElementCrystalCountSubByPartnerId(int partner_id);																// 该伙伴的水晶次数-1
	void SendElementCrystalInfo();																						// 伙伴水晶信息下发
	int ClearMixedElementCrystal();				//线上老玩家所获得的混合元素水晶将自动回收，且需要按照每个混合元素水晶转换为5000铜币通过邮件返还 ;  需要检查的地方有背包,仓库,角色身上佩戴水晶,已激活伙伴身上佩戴水晶

	void GMSetCountSub(int _partner_id,int _count);																		// GM命令，设置水晶次数			
	

	/************************************************************************/
	/*                              伙伴皮肤                                */
	/************************************************************************/
	bool ActiveSkin(int partner_id,short skin_id);																						// 道具激活皮肤
	bool AutoActiveSkin(ItemID active_id, short num);																					// 自动激活	
	void AttrChangeBySkin(AttrCon &p_att, const PartnerSkinCfgInfo *skin_att, char add_or_sub /* 1- 增加属性  0-减少属性 */);				// 增加属性
	void AttrChangeBySkin(AttributeList &p_att, const PartnerSkinCfgInfo *skin_att);													// 增加属性
	void PutOnSkin(int partner_id, int skin_id, bool is_from_init = false);																// 穿戴皮肤
	void PutOffSkin(int partner_id, int skin_id, bool is_from_init = false);															// 卸下皮肤
	int GetSkinCount(int partner_id);																									// 获得当前伙伴拥有皮肤数量
	bool IsActive(int partner_id,int skin_sq);																							// 该皮肤ID是否拥有
	void GetActiveSkinId(int partner_id,int ARG_OUT skin_list[PARTNER_SKIN_ONCE]);														// 获取当前伙伴拥有皮肤
	int GetCurPutOnSkin(int partner_id);																								// 获取当前正在穿戴的皮肤
	bool IsPutOn(int partner_id, int skin_id);																							// 该皮肤是否正在穿戴
	void SendSingleInfo(int partner_id, int skin_id,int reson);																			// 单个信息下发
	void SendAllSkinInfo();																												// 下发总皮肤激活数量

	//------------------------------------------
	int GetParterCapability(int partner_id);
	void GetPartnerList(std::vector<int> * role_partner_vec);//获取玩家拥有伙伴的id
	void GetUnlockedPartnerSet(std::set<int>* unlocked_set);//获取玩家拥有伙伴的id
															 
	//-------------------擂台战函数
	bool IsInPlatformBattle(int partner_id);
	bool CanJoinPlatformBattle(int partner_id);
	void ClearPartnerPlatformBattleState();
	void SetPartnerPlatformBattleState(int partner_id, bool is_platform_battle);
	void SyncPartnerDataPlatformBattle();				//伙伴属性发生变化同步数据到擂台战
	bool GetPlatformBattleCharacterData(int partner_id, PartnerBattleData ARG_OUT * out_data, PlatformBattlePartner ARG_OUT & out_partner, std::set<int> fight_list);// 获取对应伙伴的基本属性


	void OnPartnerActive2(int partner_id);				//当伙伴激活时

	//神秘商店
	void OnStoreBuy(int seq);
	void OnStoreRefresh(bool is_auto);
	void SendStoreInfo();
	unsigned int CalculateNextFreeRefreshTime();
	bool IsFreeRefresh();

	//伙伴残卷合成
	void OnPageCompose(int item_id);
	void GmSetTranf(char tranf_id);

	//伙伴天赋系统
	void GmSetReturnFlag() { m_partner.partner_skill_tree_back_flag = 0; }
	void UpLevelGiftTree(int partner_id, int tree_point);
	void ResetGiftTree(int partner_id, int tree_point);
	void ResetTreePoint(int *round_num ,int partner_id, int tree_point, int *back_coin, std::vector<ItemConfigData>* back_item_vec, std::vector<int>* reset_point);//重置某伙伴某个天赋点--只重置这个点
	void SendGiftTreeInfo();
	void SendSingleGiftTreeInfo(int partner_id);

	//属性丹
	void OnAttrPelletChange(int partner_id);

	//伙伴飞升
	void OnPartnerSoar(int partner_id);
	void OnPartnerSoarArmsLevelUp(int partner_id);
	int GetUseMaxAttrPellet(int partner_id);
	void GetSoarArmsPassiveSkillById(int partner_id, int max_other_sp_num, int * out_other_sp_num, int * out_other_sp_list);	//收集飞升武器特效
	void SendPartnerSoarNoitce(int notice_type, int partner_id);

	int GetPartnerSoarArmsLevel(int partner_id);
	int GetPartnerSoarLevel(int partner_id);
	// 符玉重算属性
	void ReCalcPartnerAttrByFuYu(int partner_id, bool is_notify = true, char notic_type = Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_NO, char add_or_sub = ADD_ATTR);

	///////////////////////////////
	//////伙伴--元神契约
	///////////////////////////////
	//放入系统仓库
	bool PactGridPutIn(ItemID item_id, int num, int reason_type);
	//移除系统仓库(类似消耗)
	bool PactGridTakeOut(ItemID item_id, int num, int reason_type);

	//--装上
	bool PactPutOn(int partner_id, int put_index, int save_index);
	//--卸下
	bool PactGetOff(int partner_id, int put_index);
	//--替换
	bool PactSwap(int off_partner_id, int off_index, int put_partner_id, int put_index);
	//--升星
	void PactUpStar(int partner_id, int put_index);
	//--契约仓库通知
	void SendPactGridList();
	void SendPactGridChange(int save_index, ItemID item_id, int item_num);
	void SendPactInfo(int partner_id, int type, short param);
	void SendPactBaseAttr(int partner_id);

	//伙伴战力重算事件
	void OnReCalcAttr(int partner_id, AttrCon &partner_attr, bool is_notify,
		char notic_type = Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_NO);

	//重算契约属性
	//目前使用附加和移除的方式改变属性和战力 类似buff
	//不然直接调用重算伙伴战力效率极低且容易导致死循环
	//TODO:重写 属性计算逻辑
	void ReCalcPactAttr(int partner_id, bool calc_cap = false, bool is_notify = false);

	//////////伙伴--元神契约
	///////////////////////////////
	///////////////////////////////

	//伙伴强化
	void OnPartnerIntensify(int partner_id, Protocol::CSPartnerIntensifyReq * req);

	// gm伙伴飞升
	void GmPartnerFlyUp(int partner_id, int fly_up_level, bool modify_role_flyup);

private:
	void OnPartnerActive(int partner_id);				//当伙伴激活时
	void OnPartnerUpgrade(int partner_id);				//当伙伴升阶时
	void OnPartnerQualityUp(int partner_id);			//当伙伴升品时,提升段数
	
	void ReCalcPartnerAttr(int partner_id, bool is_notify = true, char notic_type = Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_NO, char add_or_sub = ADD_ATTR /* 1- 增加属性  0-减少属性 */);
	void CalcPartnerCapability(AttrCon &attr_con, int prof_type);		// 计算伙伴战力

	//属性战力重算结束
	void OnPartnerReCalcEnd();



	void PartnerAttrBreakThrou(int partner_id);						// 伙伴属性突破
	void PartnerUPGradeHandler(int partner_id);						// 伙伴等阶提升

	void SendPartnerFightState(int partner_id, bool is_show = false);	// 发送出战状态 - 激活、改变状态的时候
	void SendPartnerAttrChange(int partner_id, const std::vector<BattleAttrPair> & attr_chan_list, char notic_type = -1);	// 通知属性改变
	void SendPartnerFetterInfo(int partner_id);							// 通知情缘信息
	void SendPartnerGradeBreak(int partner_id, int need_pop);			// 发送属性突破和等阶信息
	void SendPartnerSkillsInfo(int partner_id);							// 发送携带的技能
	void SendPartnerEquipsInfo(int partner_id, int slot_id);			// 发送穿戴的装备

private:
	bool m_already_send_all_attr;
	int m_partner_count;										// 伙伴配置总数
	char once_init;

	RoleModuleManager * m_role_module_mgr;

	AttributeList	m_reason_list[MAX_REASON_TYPE_LIST_NUM];	//使用该属性来源前需先调用GetPartnerAttrListReason接口
	PartnerParam m_partner;
	NewPartnerParam m_new_partner;
	AttributeList m_attrs_add;									// 角色属性加值
	AttrCon m_partner_attr_list[PARTNER_NUM_MAX];				// 伙伴属性表
	PartnerPactInfo m_partner_pact_list[PARTNER_NUM_MAX];		// 元神契约信息
	
	bool m_platform_battle_partner[PARTNER_NUM_MAX];			// 擂台战守擂中伙伴 下标是partner_id

	int m_partner_jiban_num;		//激活羁绊的伙伴数量
	//伙伴天赋树
	PartnerGiftTreeParam m_partner_gift_tree_list;

	//伙伴--元神契约仓库
	PartnerSpiritPactParam m_pact_grid_param;
};

#endif