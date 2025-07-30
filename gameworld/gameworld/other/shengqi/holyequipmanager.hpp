#ifndef __HOLY_EUQIP_HPP__
#define __HOLY_EUQIP_HPP__

#include "servercommon/holyequipdef.h"
#include "protocol/msgholyequip.h"
#include "other/attributelist.hpp"

enum HOLY_EQUIP_SEND_REASON
{
	HOLY_EQUIP_SEND_REASON_NORMAL = 0,			//!< 0 正常下发（上线或客户端请求），不飘字
	HOLY_EQUIP_SEND_REASON_IN = 1,				//!< 1 外部获得飘字
	HOLY_EQUIP_SEND_REASON_HOLY_LVUP = 2,		//!< 2 圣器升级
	HOLY_EQUIP_SEND_REASON_MOVE = 3,			//!< 3 道具数据改变	符石/圣器
	HOLY_EQUIP_SEND_REASON_STATUS = 4,			//!< 4 圣器状态改变
	HOLY_EQUIP_SEND_REASON_FUSHI_FUSING = 5,	//!< 5 符石合成
	HOLY_EQUIP_SEND_REASON_FUSHI_REACST = 6,	//!< 6 符石重铸
	HOLY_EQUIP_SEND_REASON_FUSHI_INLAY = 7,		//!< 7 符石镶嵌
	HOLY_EQUIP_SEND_REASON_XUN_BAO = 8,			//!< 8 寻宝获得，不飘字(寻宝已经被干掉了)
	HOLY_EQUIP_SEND_REASON_BLESS_PET = 9,		//!< 9 圣佑宠物
	HOLY_EQUIP_SEND_REASON_SKILL_STUDY = 10,	//!< 10 技能书学习
	HOLY_EQUIP_SEND_REASON_SALE_FU_SHI = 11,	//!< 11 市场出售符石
};

static const int MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM = 5;		// 最高出战和待机圣器数量
UNSTD_STATIC_CHECK(MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM == HOLY_EQUIP_STATUS_MAX);	// 不可以变,改变的话角色基本信息里的holy_equip_status_index也需要重新设计

class Role;
class RoleModuleManager;

class HolyEquipManager
{
	enum LOG_REASON
	{
		LOG_REASON_STATE_CHANGE = 0,	// 圣器出战/待机/休息状态改变
		LOG_REASON_LEVEL_UP = 1,
		LOG_REASON_PUT_ON_FU_SHI = 2,
		LOG_REASON_TAKE_OFF_FU_SHI = 3,
	};

public:
	HolyEquipManager();
	~HolyEquipManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const HolyEquipCommonParam & param, int shengqi_param_list_num, const ItemListParam::ItemListData * shengqi_data_list[],
																		   int fushi_param_list_num, const ItemListParam::ItemListData * fushi_data_list[]);
	void GetInitParam(HolyEquipCommonParam * param);
	// 保存圣器背包数据
	void GetChangeHolyEquipBagList(int *count, ItemListParam::ItemListData *save_list);
	// 保存符石背包数据
	void GetChangeFuShiBagList(int *count, ItemListParam::ItemListData *save_list);

	void GetHolyEquipList(int *count, QueryItemListParam::ItemListData *save_list);

	void ClearHolyDirtyMark();
	void ClearFuShiDirtyMark();
	
	// 转为ItemDataWrapper, 聊天用
	bool GetHolyEquipGridData(short index, ItemDataWrapper & carrier_data);

	// 是否有圣佑宠物
	bool HasBlessPet(int index);

	// 重算圣器属性
	void ReCalHolyBaseAttr(int index, bool is_send = false);
	void ReCalAllHolyBaseAttr();

	// 计算给角色增加的属性
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);

	// 转职
	void ChangeProfession(int old_prof, int new_prof);

	// 计算给宠物增加的属性
	void ReCalcPetAttr(unsigned long long pet_unique_id, AttributeList &base_add);
	void RefreshPetAttr();

	// 客户端请求
	void ClientReq(Protocol::CSHolyEquipOpReq * msg);


	void OnRoleLogin();

	// 获取出战圣器, out_index返回的是角色通用信息holy_equip_stat_index的下标
	const ItemGridData* GetFightHolyEquip(int * out_index = NULL);

	// 获取可设置为待机/出战状态圣器位置的下标,-1表示没有
	int GetEmptyStandbytStatus();
	// 查找圣器状态,返回圣器出战/待机/休息状态, return < 0 数据错误
	int FindStatusByIndex(int index);

	// 获取圣器数据
	const ItemGridData* GetHolyEquipByIndex(int index);

	// 圣器升级
	void HolyItemLevelUp(Protocol::CSHolyEquipLevelUpReq * req);
	// 圣器增加经验
	bool HolyAddExp(ItemGridData& item_data, int add_exp);
	// 计算圣器评分
	void CalculateScore(int index);

	// 设置圣器状态 出战/待机/休息
	void SetHolyEquipStatus(int index, int status);
	int SetFightingStatus(int index);
	int SetStandbyStatus(int index);
	int SetRestStatus(int index);
	void StatusChange(int index, int status_type = -1);
	// 解锁主动技能孔位
	void UnlockSkillHole(int index, int hole);
	// 领悟新的主动技能 
	void StudySkill(int index, int consume_item_id);
	// 保存领悟的主动技能
	void SaveSkill(int index, int is_save);

	void GmStudySkill(int skill_id, int hole);
	void GmSetGift(int gift_id, int hole);

	// 解锁符石孔
	void UnlockFuShiHole(int index, int fushi_hole);
	// 镶嵌符石
	void PutOnFuShi(int sheng_qi_index, int sheng_qi_hole, int fu_shi_index);
	// 取下符石
	void TakeOffFuShi(int sheng_qi_index, int sheng_qi_hole);
	// 取下全部符石
	void TakeOffAllFuShi(int index);
	bool IsCanTakeOffAll(int index);

	// 合成符石 fushi_level-符石目标等级 [2-5]
	void FusionFuShi(int fushi_level, int fushi_index1, int fushi_index2, int fushi_index3, int fushi_index4, int fushi_index5);
	// 重铸符石
	void RecastFuShi(int fushi_index, int consume_index);
	// 重铸圣器上的符石
	void RecastFuShiOnHolyEquip(int index, int hole, int consume_fushi_index);

	// 圣佑一只宠物, pet_index = -1表示取消宠物的圣佑
	void BlessPet(int holy_index, int pet_index);
	void DeleteBlessPet(long long pet_unique_id);

	// 圣器跟随
	void HolyFollow(int index = -1);

	// 圣器技能书兑换
	void HallowsSkillConvert(int skill_1, int skill_2);

public:
	/*圣器背包操作*/
	int GetHolyItemEmptyBag();//背包空闲的格子
	bool HasHolyItemBagNum(int grid_num);
	bool AddNewHolyItem(const ItemDataWrapper * new_item, int outside_reason, int *out_idx = NULL, int reason_type = HOLY_EQUIP_SEND_REASON_NORMAL);//放入一个圣器（已存在的）
	bool AddNewHolyItem(const ItemGridData * new_item, int outside_reason, int *out_idx = NULL);//放入一个圣器（已存在的）
	const ItemGridData * AddHolyItem(ItemID new_id, int outside_reason, int *out_idx = NULL, int reason_type = HOLY_EQUIP_SEND_REASON_IN);
	ItemGridData * GetHolyItemGridDataKnapsack(int index);

	/*符石背包操作*/
	int GetFuShiEmptyBag();//背包空闲的格子
	bool HasFuShiBagNum(int grid_num);
	bool AddNewFuShiItem(const ItemDataWrapper * new_item, int outside_reason, int *out_idx = NULL);//放入一个符石（已存在的）, outside_reason: PUT_REASON
	bool AddNewFuShiItem(const ItemGridData * new_item, int outside_reason, int *out_idx = NULL);//放入一个符石（已存在的）
	const ItemGridData * AddFuShiItem(ItemID new_id, int outside_reason, int *out_idx = NULL);
	ItemGridData * GetFuShiGridDataKnapsack(int index);

	// 清空背包 圣器背包&符石背包
	void ClearAll();
	void ClearHolyGrid(int index);
	void ClearFuShiGrid(int index);

	bool ConsumeHolyGrid(int consume_idx, int reason_type, const char * reason, bool is_notice = true);		// 消耗圣器
	bool ConsumeFuShiGrid(int consume_idx, int reason_type, const char * reason, bool is_notice = true);	// 消耗符石

	void SetHolyBagDirty(short index, bool is_recalculate, short reason = HOLY_EQUIP_SEND_REASON_NORMAL, bool is_notice = true);
	void SetFuShiBagDirty(short index, short reason = HOLY_EQUIP_SEND_REASON_NORMAL, bool is_notice = true);

	void GmClearBag(int op_type);

	void CheckHolyEquipCollection();
	
	void GetBattleHallows(ARG_OUT short* hallow_num, ARG_OUT BattleHallowInfo* hallows, ARG_OUT short* fight_hallow_idx, int max_hallow_num);

private:
	// ALL IN!
	void SendAllInfo();
	void SendFirstGainFlagInfo();

	// 圣器/背包 单条数据
	void SendSingleHolyBagInfo(int index, short reason_type);
	void SendSingleFuShiBagInfo(int index, short reason_type);

	// 圣器/符石 背包全部数据
	void SendHolyBagListInfo();
	void SendFuShiBagListInfo();

	void SendHolyFollowInfo();

	ItemGridData* MutableFightHolyEquip(int * out_index = NULL, int* out_index_in_bag = NULL);
private:
	RoleModuleManager * m_role_module_mgr;
	AttributeList m_role_attrs_add;			// 符石附带角色属性
	AttributeList m_attrs_add[MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM];	// 圣器附带宠物属性

	ItemGridData* GetHolyBagNode(int bag_idx);
	ItemGridData* GetFuShiBagNode(int bag_idx);

	// 圣器背包
	int				m_holy_grid_num;
	bool			m_holy_bag_is_change;
	bool			m_holy_old_state[ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM];
	bool			m_holy_dirty_mark[ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM];
	ItemGridData*	m_holy_grid_list[ItemNamespace::MAX_HOLY_EQUIP_BAG_NUM];

	// 符石背包
	int				m_fushi_grid_num;
	bool			m_fushi_bag_is_change;
	bool			m_fushi_old_state[ItemNamespace::MAX_RUNE_STONE_BAG_NUM];
	bool			m_fushi_dirty_mark[ItemNamespace::MAX_RUNE_STONE_BAG_NUM];
	ItemGridData*	m_fushi_grid_list[ItemNamespace::MAX_RUNE_STONE_BAG_NUM];

	HolyEquipCommonParam m_param;
};

#endif
