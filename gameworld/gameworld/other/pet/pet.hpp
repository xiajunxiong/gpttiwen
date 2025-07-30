#ifndef __PET_HPP__ 
#define __PET_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"
#include "gameworld/item/itembase.h"
#include "gameworld/item/itemgriddata.h"
#include "other/attributelist.hpp"
#include "protocol/msgpet.h"
#include "item/knapsack.h"
#include "protocol/msgpet.h"
#include "servercommon/rolepetpartnerdef.hpp"

#include <vector>
#include <set>

enum PET_STATE
{
	SET_FIGHT_INDEX = 1,			// 出战
	SET_FOLLOW_INDEX = 2,			// 跟随
	SET_STANDBY_INDEX1 = 3,			// 待机1
	SET_STANDBY_INDEX2 = 4,			// 待机2
	SET_STANDBY_INDEX3 = 5,			// 待机3
	SET_REST = 6,					// 休息
	SET_REST_FOLLOW = 7,			// 取消跟随
	SET_STANDBY_INDEX4 = 8,			// 待机4
	SET_STANDBY_INDEX5 = 9,			// 待机5
};

enum GET_PET_REASON_TYPE
{
	GET_PET_REASON_TYPE_INVALID = -1,
	GET_PET_REASON_TYPE_DEFAULT = 0,		// 默认
	GET_PET_REASON_TYPE_PET_EGG,			// 宠物蛋
	GET_PET_REASON_TYPE_SPECIAL_PET,		// 特殊宠物
	GET_PET_REASON_TYPE_COMPOSE_PET,		// 合成宠物
	GET_PET_REASON_TYPE_CONVERT_PET,		// 兑换宠物
	GET_PET_REASON_TYPE_REFINED_MUTANT_PET,		// 洗练获得变异宠物
	GET_PET_REASON_TYPE_SHEN_SHOU_JIANG_LIN,		// 神兽降临
	GET_PET_REASON_TYPE_XUN_BAO_EXCHANGE,		// 寻宝兑换
	GET_PET_REASON_TYPE_USE_ITEM_MUTANT_PET,		// 使用物品变异宠物
	GET_PET_REASON_TYPE_BATCH_COMPOSE_PET,		// 批量合成宠物
	GET_PET_REASON_TYPE_REALM_BREAK_FIGHT,		// 境界-心灵之境
};

static const double PET_ATTR_BASE_UP = 0.25 + 1e-6;
static const double PET_ATTR_LEVEL_UP = 0.042 + 1e-6;

class RoleModuleManager;
struct PlatformBattlePet;

enum PET_SKILL_SRC_TYPE
{
	PET_SKILL_SRC_TYPE_PASSIVE,
	PET_SKILL_SRC_TYPE_FEI_SHENG,
	PET_SKILL_SRC_TYPE_BADGE,
	PET_SKILL_SRC_TYPE_LIAN_YAO,
};

struct PetEffectSkillItem
{
	PetEffectSkillItem(int skill_id, int skill_level, bool skill_src_type)
	{
		this->skill_id = skill_id;
		this->skill_level = skill_level;
		this->skill_src_type = skill_src_type;
	}

	int skill_id;
	int skill_level;
	int skill_src_type;
};

struct PetEffectSkillId
{
	PetEffectSkillId()
	{
	}

	std::vector<PetEffectSkillItem>	passive_list;
};

struct PetSkillPriority
{
	PetSkillPriority()
	{
		skill_id = 0;
		skill_level = 0;
		skill_type = 0;
		priority_num = 0;
		priority_num_2 = 0;
		pet_skill_src_type = PET_SKILL_SRC_TYPE_PASSIVE;
	}

	bool operator<(const PetSkillPriority & other)const
	{
		if (skill_type < other.skill_type)
			return true;
		if (skill_type > other.skill_type)
			return false;

		if (priority_num > other.priority_num)
			return true;
		if (priority_num < other.priority_num)
			return false;

		if (skill_level > other.skill_level)
			return true;
		if (skill_level < other.skill_level)
			return false;

		if (priority_num_2 > other.priority_num_2)
			return true;
		if (priority_num_2 < other.priority_num_2)
			return false;

		return false;
	}

	int skill_id;
	int skill_level;
	int skill_type;
	int priority_num;
	int priority_num_2; 
	int pet_skill_src_type;
};

struct PetSpPriority
{
	PetSpPriority()
	{
		sp_id = 0;
		sp_type = 0;
		priority_num = 0;
	}

	bool operator<(const PetSpPriority & other)const
	{
		if (sp_type < other.sp_type)
			return true;
		if (sp_type > other.sp_type)
			return false;

		return false;
	}

	int sp_id;
	int sp_type;
	int priority_num;
};

struct PetReplacePassiveSkillLockItem
{
	bool operator<(const PetReplacePassiveSkillLockItem & other) const 
	{
		if (skill_type < other.skill_type)
			return true;
		if (skill_type > other.skill_type)
			return false;

		if (skill_index < other.skill_index)
			return true;
		if (skill_index > other.skill_index)
			return false;

		return false;
	}

	PetReplacePassiveSkillLockItem(int skill_type,int skill_index)
	{
		this->skill_type = skill_type;
		this->skill_index = skill_index;
	}

	int skill_type;
	int skill_index;
};

class Pet
{
public: 
	Pet();
	~Pet();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager* GetRoleModuleManager() { return m_role_module_mgr; }
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLogin();

	void Init(RoleModuleManager * role_module_manager, const PetCommonParam & param, const PetSkinParam & pet_skin_param, const PetAutoMoveParam & auto_move_param, const PetStrengthenBuffParam & pet_strengthen_buff_param, int param_list_num, const ItemListParam::ItemListData * param_data_list[]);
	void InitPetAutoMoveMap(const PetAutoMoveParam& param);
	void InitGodPrintBag(const GodPrintBagParam& param, const GodPrintBagWishLogParam & param2 , const NewGodPrintBagWishLogParam & param3);
	void InitNewGodPrintBag(const GodPrintItemListParam& god_print_item_list);
	void GetInitParam(PetCommonParam * param, PetSkinParam * pet_skin_param, PetAutoMoveParam* auto_move_param, PetStrengthenBuffParam * pet_strengthen_buff_param,
		GodPrintBagParam* god_print_param, GodPrintBagWishLogParam *god_print_log_param, GodPrintItemListParam * new_god_print_item_list , NewGodPrintBagWishLogParam * new_god_print_log_param);

	void GetRolePetInfoParam(RolePetInfoParam * role_pet_info_param);

	void GetGodPrintChangeItemList(GodPrintItemListParam * new_god_print_item_list);
	void ClearGodPrintBagDirtyMark();
	void GetChangeItemList(int *count, ItemListParam::ItemListData *save_list);
	void CheckDataIsIllegal();	// 检查数据的合法性
	void InitEnd();
	void ClearDirtyMark();
	void RankUpdate();

	void SetItemDirty(int index, bool to_notice_client = true,int dirty_type = Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);

	// 宠物请求协议操作
	int OnPetOp(Protocol::CSPetOp *op);

	// 扩展背包
	int ExtendPetBag();

	//角色升级
	void OnRoleLevelUp(int cur_level, int * pet_index = NULL);

	//
	bool FillSpecialPetData(int item_id, ItemDataWrapper ARG_OUT & item, int pet_level = 1, bool is_guilde_pet = true);		//新手宠物固定数据
	// 获得宠物
	int AcquirePet(int pet_id, const char* reason, int str_level = 1, bool is_max = false, int notice_reason = GET_PET_REASON_TYPE_DEFAULT, bool is_bind = false, bool is_xunbao = false, long long item_unique_id = 0LL);
	bool AcquirePet2(int pet_id, const char * reason, int put_reson, bool is_bind = false, int str_level = 1, bool is_max = false, long long item_unique_id = 0LL);
	// 获得指定数值的宠物
	int AcquireSpecialPet(const ItemBase* special_pet, const char* reason, bool is_bind, bool is_xunbao = false, long long item_unique_id = 0LL);   
	// 遗弃宠物
	int AbandonmentPet(int pet_index);																
	// 获得经验
	int AddExp(int pet_index, long long exp, const char* reason,int notice_reason);	
	// 一键升级
	int OnKeyLevelUp(int pet_index);
	//突破
	int PetBreakLevel(int pet_index);
	int GetPetBreakLevelAttr(int pet_index,int level);  //没什么用，客户端要求加
	// 转换经验
	int ConversionExp(int pet_index, int to_level, const char* reason, int put_reason = PUT_PET_CONVERSION_EXP);
	int ConversionExpByList(std::set<int> pet_index_set, int to_level, const char * reason, int put_reason = PUT_PET_CONVERSION_EXP);
	// 返生
	int Rebirth(int pet_index);																	
	// 回血回蓝
	int Recover(int pet_index);	
	void AllPetRecoverHpMpPercent(double percent);
	
	//出售前清理
	void ClearBeforeSale(int pet_index);

	// 系统回收宠物--用于宠物提交之类的系统回收
	bool SystemTakeBackPet(int pet_index,int pet_id);

	bool IsPetTraning(int pet_index);

	// 设置血量魔量
	void SetCurHpMp(int pet_id, int pet_index, int cur_hp, int cur_mp);

	// 调整血量
	bool ChangeHP(int pet_index, int ch_hp,bool is_notic = false, bool is_percent = false);
	bool ChangeMp(int pet_index, int ch_mp, bool is_notic = false, bool is_percent = false);

	// 获取宠物的AttributeList
	int GetAttributeList(int pet_index, AttributeList& attr);	
	int GetBagAttributeList(int pet_index, AttributeList& attr);
	int GetAttributeListHelper(int pet_index, bool is_storage, AttributeList & attr);
	int GetAttributeListHelper2(const PetParam & pet, AttributeList & attr, AttributeList & no_skill_attr, int role_level, unsigned long long item_unique_id, int str_buff_level = 0, bool is_wait = false, 
		int pet_train_level = 0, const std::map<int, int> * pet_train_2_map = NULL, bool is_huashen = false) const;
	int GetAttrListReason(int pet_index);				//获取宠物属性来源列表
	void SendPetAttrListReason(short pet_index);		//发送宠物属性来源列表
	void ReCalcAttr(int pet_index , bool _is_force = false, bool is_init = false);		//is_force 表示在计算战斗力的时候宠物排行榜是否强制刷新
	void ReCalcAttrByItemUniqueId(long long item_uuid, bool send_attr = true, int notie_type = Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_STRENGTH_BUFF);
	int GetPetAttr(int pet_index, int attr_type);
	void OnPetAttrChange(int pet_index, bool _is_force, bool is_init);
	void GetQueryAttrList(int pet_index, QueryAttrList & get_attr_list);
	
	// 如果有"出战"宠物 fight_index >= 0    standby_num 为有多少"待机"宠物
	void GetFightIndex(int* fight_index, int* standby_index1, int* standby_index2, int* standby_index3, int* standby_index4, int* standby_index5);
	int GetFightIndex();
	bool IsFightPet(int pet_index);
	// 分配属性点
	int AssignAttributePoints(int pet_index, unsigned short con, unsigned short str, unsigned short def, unsigned short agi, unsigned short magic);	
	int AutoAssignAttributePoints(int pet_index, bool is_notice = false);
	void UpdateAutoAssignAttributePoints();
	// 改名
	int ReName(int pet_index, GameName name);														
	// 强化
	int Breakthroughs(int pet_index,const int(&consume_pet_index_arr)[3], bool is_liao_yao_pet);
	int GmBreakthroughs(const int type, int param1, int param2);
	// 转移强化
	int TransferBreakthroughs(int pet_index,int consume_index);										
	// 上锁
	int Lock(int pet_index);
	// 判断是否上锁
	bool IsLock(int pet_index);
	// 设置宠物状态
	int SetPetState(int pet_index, int state);		
	// 使用经验药
	int OnUseExp(int pet_index, int item_id, int num);
	// 洗点
	int OnRefreshPoints(int pet_index);
	// 是否为备战宠物
	bool IsWaitForFightPet(int pet_index, int * out_index = NULL);
	//洗练---刷新被动技能和资质	
	int Refined(int pet_index, bool is_refined_skill,bool is_auto_nuy);
	//指引洗练---刷新被动技能和资质	
	int GuideRefined(int pet_index, bool is_refined_skill,bool is_auto_buy);
	//保存洗练出来被动技能和资质	
	int RefinedSave(int pet_index);
	//放弃洗练出来被动技能和资质	
	int RefinedGiveUp(int pet_index);
	//领悟技能---习的某个被动技能
	int ComprehendPassiveSkill(int pet_index);
	// 打书---替换某个被动技能
	bool ReplacePassiveSkill(int pet_index, int passive_skill_id,int need_pet_quality,std::set<PetReplacePassiveSkillLockItem> lock_index);
	int ReplacePassiveSkillSave(int pet_index,bool is_save);
	// 合成---消耗材料获取物品
	int Compose(int seq, int use_shards_num, int get_pet_reason_type = GET_PET_REASON_TYPE_CONVERT_PET, int * ARG_OUT pet_index_ptr = NULL);
	// 神兽兑换---消耗材料获取物品
	int Convert(int seq);
	//宠物学习技能时，固定替换第二个技能 , 服务器-宠物领悟技能指引和学技能指引都需要固定下来
	int GuideReplacePassiveSkill(int pet_index);
	//领悟技能指引时，固定领悟一个技能 , 服务器-宠物领悟技能指引和学技能指引都需要固定下来
	int GuideComprehendPassiveSkill(int pet_index);
	//消耗道具使宠物变异
	int Mutant(int pet_index);
	//宠物祝福
	int OnPetBenediction(int main_pet_index, int vice_pet_index);
	int OnCancelBenediction(int vice_pet_index);

	bool PutOnPetEquipment(int pet_index,int equipment_bag_index);//穿戴宠物装备
	bool PutOffPetEquipment(int pet_index);//脱下宠物装备
	bool ChangePetEquipmentToOtherPet(int pet_index_get,int pet_index_off);//把一个宠物的装备替换给另一个

	//获取宠物当前穿戴护符达到指定品质的数量
	int GetPetEquipmentByColor(int color, bool is_find_storage = true);
	//获取当前大于等于该品质的宠物数量
	int GetPetNumByQuality(int quality, bool is_find_storage = true);
	//获取宠物当前穿戴魂器达到指定品质的数量(目前魂器无法进仓库)
	int GetPetSoulEquipmentByColor(int color, bool is_find_storage = false);



	//灵骑统御改变
	void OnDominanceChange(int pet_index);

	// 获取宠物信息
	const ItemGridData * GetPetGridData(int pet_index) const;
	const ItemGridData * GetPetGridDataFromStorage(int pet_index) const;
	const PetParam* GetPetParam(int pet_index);
	const PetParam* GetFightPetParam();
	const PetParam* GetPetParamFromStorage(int pet_index);
	const PetParam* GetPetParam(int pet_id, long long unique_item_id, ARG_OUT int* pet_idx);

	void CheckPetCollection();

	bool GetBattleCharacterData(int pet_index, PetBattleData* out_data, bool is_use_cur_hp_mp);
	bool GetPetAutoMoveSetting(int pet_id, ARG_OUT short* auto_move_type, ARG_OUT short* auto_skill_id, ARG_OUT short* auto_skill_lv);

	bool GetPetGridData(int pet_index, ItemDataWrapper* out_wrapper);
	bool IsHavePet(int pet_id);
	int TheNumOfLevelPet(int level);

	bool HaveSpace(int count, bool is_notic = true);
	void SendPetOtherInfo();
	void SendAllInfo(int reason = Protocol::PET_ALL_INFO_REASON_DEFAULT);
	void SendSingleInfo(int pet_index,int notie_type = Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
	int  SendPetAttrInfo(int pet_index);
	void SendPetIdRecordList();
	void SendPetBenedictionAllInfo();
	void SendPetBenedictionSingleInfo(int pet_index);
	int GetStrBuffLevel(int pet_index, bool * is_in_buff_list = NULL);
	int GetPetCount() { return m_pet_count; }
	int GetPetAndStorageCount() { return m_pet_count + m_pet_storage_grid_use_num; }
	int GetReachingGrowthCount(int growth);
	int GetPetNumByQualityAndStr(int quality, int str = 1);
	int GetHasMaxPassiveSkillNum();									//获得宠物已学习的最高技能数量
	int GetPetDifferentCount();										//获得已拥有的宠物数量(相同不算)
	int GetPetCapabilityMax();										//获得当前宠物的最高战力	
	int GetPetIntensifyMax(int quality = 0);						//获得当前宠物的最高强化
	int GetPetCapability(int pet_index)const;						//获得某个宠物的战力
	int GetPetCapability_1(int pet_index)const;						//获得某个宠物的战力
	int GetFightPetCapability();									//获得某个宠物的战力
	int GetPetSkillCapability(int pet_index)const;					//获取宠物技能的战力
	int GetAllFightCapability()const;								//获取出战的宠物战力和
	static int ReCalcPetCapability(const Attribute* attrs);			//获取宠物属性的战力
	void SendGetPetNotice(int pet_index,int notice_reason);

	bool OnTradeConsumePet(int index);
	void ReCalcAttrByFaBao();				// 法宝信息修改，促使宠物属性变更
	int AddPet(NetValueItemParam* pet, const char* reason, bool is_bind = false, bool is_xunbao = false, long long item_unique_id = 0LL, bool is_add_guild_eventhander = true);
	bool AcquirePetWithParam(NetValueItemParam* pet, const char* reason, bool is_bind = false, bool is_xunbao = false, long long item_unique_id = 0LL, bool is_add_guild_eventhander = true);
	bool IsBind(int index);
	bool PetInvalid(int index) const;
	bool ModifyAutoMove(int pet_index, short auto_move_type, short auto_skill_id, short auto_skill_level, bool ignore_skill_check = false);
	bool ModifySpecifyPetAutoMove(int pet_id, long long pet_unique_id, short auto_move_type, short auto_skill_id, short auto_skill_level, bool is_main_fight_pet);
	void GmAddMaxAttrPet(int pet_id, int gift_type, int attr_point_val);

	void SendPetFollowInfo(Role * role = NULL);

	//发送出战宠物信息给队员
	void SendFightPetInfoToTeamMember();

	//使用宠物蛋
	bool UsePetEggItem(int pet_id, bool is_bind);
	//-------------------------------------------------------------------
	bool GetPetItemGridWrapData(int pet_index, ItemDataWrapper & ARG_OUT out_wrapper, AttributeList & attrs);
	//--------------------擂台战函数----------------------
	bool IsInPlatfformBattle(unsigned long long pet_unique_id, bool is_notice = false);
	void ClearPetPlatformBattleState();
	void SetPetPlatformBattleState(unsigned long long item_unique_id, bool is_platform_battle);
	void SyncPetDataPlatformBattle(int pet_index = -1);				//宠物属性发生变化同步数据到擂台战
	long long GetPetUniqueId(int pet_index) const;
	long long GetPetStorageUniqueId(int pet_index) const;
	bool HasPet(std::set<unsigned long long>  pet_unique_ids);
	bool GetPlatformBattleCharacterData(int pet_index, PetBattleData ARG_OUT * pet_battle_data, PlatformBattlePet ARG_OUT & platform_battle_pet_data);
	//---------------------------------------------------------------------------------------------------------
	void OnRolePetTrainSkillLevelUp();
	void OnRoleGuildInbornSkillChange();
	void OnXiuWeiPetTypeLevelUp();
	void OnStarChartPetAttrChange();

	PetEffectSkillId GetPetEffectiveSkill(int pet_index, bool is_storage = false)const;			//获取宠物生效的技能id，返回相同类型效果高的技能id
	static PetEffectSkillId GetPetEffectiveSkillHelper(const PetParam & pet);			//获取宠物生效的技能id，返回相同类型效果高的技能id
	void RecordBattlePetIndex();			//开始战斗调用，记录出战以及待机的宠物。防止 （战斗中对宠物进行出战待机等操作后战斗经验加到新出战的宠物上而非旧的出战宠物上的问题）
	void OnBattleFinishAddExp(long long add_exp);	

	//------------------------宠物仓库----------------------------
	bool PetStorageInvalid(int pet_storage_index) const;
	int MoveToPetStorage(int pet_index,int pet_storage_index);		//背包移动到仓库 | pet_index 是背包index | pet_storage_index 是仓库index
	int MoveToPetGrid(int pet_index, int pet_storage_index);			//仓库移动到背包  | pet_index 是背包index | pet_storage_index 是仓库index
	void SetPetStorageItemDirty(int pet_storage_index);				
	void SendMoveNotice(int notice_type = Protocol::MOVE_PET_ITEM_NOTICE_TYPE_FAIL, int pet_index = 0, int pet_storage_index = 0);

	//化身
	void SynHuaShenPetData(ItemGridData * pet_grid_list, int count, short* pet_status_list, int status_count);	//赛季开始,拷贝本尊数据到化身
	void HuaShenKeySyn(std::set<long long> * item_set);			//化身一键同步
	void HuaShenSkinSyn(std::map<int, std::vector<int> > * pet_has_skin_id = NULL);		//同步本尊宠物皮肤

	//宠物装备
	void RefreshPetEquipment(int seq,int pet_index, PetEquipMentParam * out_data);
	void SaveRefreshPetEquipment(int pet_index, PetEquipMentParam * out_data);

	void ChangePetSoulEquip(int is_put_on, int index, int new_item_id,int pet_soul_equipment_index);									// 变更魂器

	//宠物魂器
	void ExchangePetSoulEquip(int seq);
	bool PutOnPetSoulEquip(int pet_index, int equipment_bag_index,int pet_soul_equipment_index);							//穿戴
	bool PutOffAllPetSoulEquip(int pet_index);
	bool PutOffPetSoulEquip(int pet_index, int pet_soul_equipment_index);													//脱下
	void UpLevelPetSoulEquip(int is_put_on, int index,int cost_num,
		short cost_column_list[Protocol::CSPetSoulUplevelReq::MAX_USE_ITEM_NUM],
		short cost_item_list[Protocol::CSPetSoulUplevelReq::MAX_USE_ITEM_NUM], 
		short cost_item_num[Protocol::CSPetSoulUplevelReq::MAX_USE_ITEM_NUM], int pet_soul_equipment_index);			//升级
	void StepsPetSoulEquip(int is_put_on, int index,int pet_soul_equipment_index);									//突破
	void RefinePetSoulEquip(int is_put_on, int index, int equipment_bag_index,int pet_soul_equipment_index);			//强化
	void LockPetSoulEquip(int is_put_on, int index, bool is_lock,int pet_soul_equipment_index);
	void GetSoulEffectList(const PetParam & pet_param, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)const;	//收集魂器特效
	
	//检测宠物身上魂器的收藏是否激活
	void CheckPetSoulCollection(bool need_sync = false);

	//神印
	bool GetPetGodPrintList(int pet_index, GodPrintListPro * list);
	void ExchangeGodPrint(int exchange_idx_1, int exchange_idx_2);
	const GodPrintItem* AddPetGodPrintItem(ItemID item_id,int seq, int quality,int suit_id, long long union_item_id, int put_reason = 0, int *out_idx = NULL);
	const GodPrintItem* AddPetGodPrintByUse(const ItemGridData * item_data, int *out_idx = NULL);
	const GodPrintItem* AddPetGodPrintByUse(const ItemDataWrapper * item_data, int *out_idx = NULL);
	bool HasPetGodPrintBagNum(int gird_num) { return this->GetBagEmptyNum() >= gird_num; }
	void GetGodPrintEffectList(const PetParam & pet_param, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)const;	//收集神印特效
	int GetGodPrintEffectScore(const PetParam & pet_param)const;

	bool GetPetGodPrintData(int bag_idx, GodPrintItem * item_data);
	void PutOnPetGodPrintEquip(int pet_index,int index, bool is_from_client = true);					//穿戴
	bool PutOffPetGodPrintEquip(int pet_index,int posi_index, bool is_from_client = true);				//脱下
	bool PutOffAllPetGodPrintEquip(int pet_index, bool is_notice = false);
	bool LockPetGodPrintEquip(int bag_id, bool is_lock);
	void WearShcheme(int pet_index, int scheme_id);
	void PutOffPetGodPrintScheme(int pet_idx);//检查方案

	void UpLevelPetGodPrintEquip(int posi_index, int add_exp);				//升级
	void UpLevelPetGodPrintEquip2(int posi_index, int bag_num, short cost_index[MAX_USE_ITEM_NUM]);//消耗神印道具
	void UpLevelPetGodPrintEquip3(int posi_index, short cost_column_list, short cost_item_list, short cost_item_num);			//升级

	void DiscomplsePetGodPrintEquip(int posi_index);						//分解
	void DiscomplsePetGodPrintItem(short cost_column_list, short cost_item_list, short cost_item_num);//分解材料
	void ClearGodPrintFromDiscomplse(int posi_index);
	void SetPetGodPrintScheme(Protocol::CSPetGodPrintSetearSchemeReq *msg);
	void SendSingleGodPrintScheme(int scheme_id);
	void SendSingleGodPrintBagInfo(int posi_index, int put_reason);
	void SendExpInfo();
	void SendGodPrintBagInfo();

	//神印吞噬
	bool DevourGodPrint(int main_indnx, int bedevour_idx, int voc_idx);

	/*特殊操作-丢弃神印*/
	bool DropGodprintEquip(int posi_index);

	//神印祈神
	void ReqWishGodPrint(bool use_gold, int cost_coin, bool is_discomplse = false);
	void ExchangeGodPrint(int seq);
	short GetWishGodPrintTimes() { return m_god_print_param.wish_time; }

	void AddWishLog(const GodPrintItem * item_data);
	void SendGodPrintWishLogInfo();//日志列表
	void SendGodPrintWishDetailInfo(int log_idx);//单个日志详细

	//宠物皮肤
	bool HasPetSkin(int skin_id);
	bool ActivityPetSkin(int skin_id, short num, bool is_noticec = true);
	void SendPetSkinInfo();
	void UsePetSkin(int pet_index, int skin_id);

	//删除宠物
	void DeletePetByList(std::set<int> pet_index, const char * reason);		//调用先判断好能不能删

	//属性丹
	void OnUsePetAttrPellet(int pellet_id, int num, int pet_index);
	void OnAttrPelletRet(int pet_index);

	//宠物祝福
	void OnPetBenedictionReset(int pet_index);
	bool IsPetBenedictionVice(int pet_index);					//判断宠物是否处于祝福副宠
	bool IsPetBenedictionVice(long long item_unique_id);		//判断宠物是否处于祝福副宠
	void OnCalcPetBenedictioncAttr(long long item_unique_id, AttributeList & base_add, bool is_huashen)const;
	int GetPetBenedictionSpId(const PetParam & pet, long long item_unique_id, ARG_OUT int * sp_capability = NULL)const;	//化身也使用本尊的副宠评分来获取对应祝福特效
	void OnMainReCalcAttr(int main_pet_index);			//由于副宠属性变化,此时主宠也需要重新变化

	//宠物飞升
	void OnFinishFeiShengTask(int task_id);
	bool CanAcceptFeiShengTask(int task_id);
	bool CanCommitFeiShengTask(int task_id,bool notify);
	int ReqStartFeiShengTask(int pet_index);
	int ReqFeiSheng(int pet_index, int select_skill_index);
	int ReqFeiShengSkillMod(int pet_index, int select_skill_index);
	int OnUpgradePetSkill(int pet_index, int passive_skill_index,int is_lian_yao_skill);
	int SendFeiShengInfo();
	int GetFeiShengTaskPetIndex();
	int FeiShengSkillUpLevel(int pet_index);
	PetParam * GetFeiShengTaskPet(int ARG_OUT * get_index = NULL);
	void ResetPetFeiShengOnDayChange();
	void CheckPetFeiShengTask();
	void GmFeiSheng(int op_type, int p1,int p2);
	//宠物异闻录
	void CheckAllPetYiWenLuReward();
	int FetchYiWenLuReward(int mutant_id, int reward_type);
	void SendYiWenLuRewardInfo();
	void ResetPetFeiShengRewardFlag(); //需求 55915 要求重置 传记二奖励 领取标志
	void CheckPetYiWenLuRewardByIndex(int pet_index, bool need_notice = true);
	void CheckPetYiWenLuReward(const PetParam & pet_param, bool need_notice = true);
	void SetPetYiWenLuRewardFinishFlag(int mutant_id, int reward_seq, bool * has_change = NULL);
	void SetPetYiWenLuRewardFinishFlag(PetYiWenLuRewardParam * ptr, int reward_seq, bool * has_change = NULL);
	PetYiWenLuRewardParam * GetPetYiWenLuRewardParam(int mutant_id, bool get_empty = false);
	//宠物徽章
	int PetBadgePutOn(int pet_indx, int col, int indx);
	int PetBadgeTakeOff(int pet_index);
	int PetBadgeExchange(int pet_index_1, int pet_index_2);
	int PetBadgeCompose(int item_id);
	int PetBadgeDeCompose(const std::set<KnapsackItemIndex> & list);

	//炼妖
	int PetLianYao(int pet_indx_1, int pet_indx_2,int furnace_id);
	void PetLianYaoAddPassiveSkill(PetParam & pet_1,const PetParam & pet_2);
	int PetLianYaoPassiveNum(const PetParam & pet_param, int * passive_skill_free_index = NULL);
	int PetLianYaoComprehendPassiveSkill(int pet_indx);
	PetPassiveItemInfo * GetPetPassiveItemInfo(PetParam & pet_param,int passive_skill_type, int passive_skill_index);
	void BuildMapPetQualityToStrLevel();
	void SetMapPetQualityToStrLevel(const PetParam * pet_param);
	void CheckUnlockLianYaoFurance();  //检测是否能解锁炼妖炉子
	void SendLianYaoCommonInfo();
	//宠物强化奖励
	void CheckPetBagGetReward();

	//--宠物天赋
	//解锁宠物天赋
	int OnPetUnlockTalent(int pet_index, int talent_type, int inborn_grid);
	//宠物天赋技能升级
	int OnPetTalentSkillLevelUp(int pet_index, int talent_type);
	//宠物天赋技能替换
	int OnPetTalentSkillReplace(int pet_index, int talent_type, int new_inborn_grid);
	//宠物天赋重置
	int OnPetTalentReset(int pet_index);
	bool OnPetTalentReset(PetParam * pet_param);
	//收集宠物天赋战斗特效
	void GetPetTalentBattleSpecialEffectList(const PetParam & pet_param, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list)const;
	int GetPetTalentSpIdList(const PetParam & pet_param, std::vector<int> & sp_list) const;

	//-----------------------------
	bool IsPassiveSkillEffect();

	bool RetrievePet(int pet_id, long long unique_item_id);
	bool QueryPetInfo(int pet_id, std::string& out_msg);

	void CheckGodPrintAttr();//背包神印属性检查
	bool CheckGodPrintByIdx(int bag_idx, int * pet_idx);
	void GmSetFlag() { m_common_param.god_print_check_flag = 0; }
	//------
	void SetAutoAddInfo(int pet_index, int add_flag);

	void CheckModVer(); 
	void CheckModVer1();
	void CheckModVer2();

	// 生成随机洗练数据广播
	static void RandRefinedBroadcast();

	//宠物强化buff
	const static int STRENGTHEN_BUFF_NEED_PET_NUM = 3;		//选出强化等级最高3个，共鸣
	int SetStrengthenBuffPet(int buff_grid_index, int pet_index);
	int UnSetStrengthenBuffPet(int type_index, int index);
	int UnSetStrengthenBuffPetByItemUniqueId(long long item_unique_id);
	void SendInfoStrengthenBuff();
	int GetStrengthenBuffLevel(int type_index);
	int CalStrengthenBuffLevel(int type_index);
	int GetPetStrengthenLevelByItemUniqueId(long long item_unique_id);
	int  GetStrengthenBuffTypeIndex(int pet_id);
	void CheckStrengthenBuff(int pet_index);
	void GetStrengthenBuffSpId(int pet_index, int max_other_sp_num, short * out_other_sp_num, int * out_other_sp_list);	//收集宠物共鸣特效

	//宠物改命
	int PetGaiMingUpLevel(int pet_index,int times, bool is_break);
	int PetGaiMingUpLevelHelper(int pet_index, bool is_break);
	int PetGaiMingAttrUpLevel(int pet_index,int attr_index);
	int PetGaiMingAttrReset(int pet_index);
	void PetGaiMingReturn(int pet_index,const char * reason);

	int GetPetIndexByItemUniqueId(long long item_unique_id);
	const PetParam * GetPetParamByItemUniqueId(long long item_unique_id, int * pet_index = NULL, bool * is_in_storge = NULL);
private:
	bool CanDeletePet(int pet_index, const char * reson, unsigned int exclude_flag = 0);
	void CheckPetParamOffsetBeforeInit(int pet_id, long long pet_unique_id, NetValueItemParam* param);
	bool IsPetParamDataNormal(int pet_id, NetValueItemParam* param);
	bool IsInOffsetErrorUserList(int plat_type, int role_id);

	void OnPetGridInfoReset(int pet_index);			//宠物格子删除，移动到仓库调用，对使用到这个宠物其他功能操作
	void DeletePet(int pet_index, const char* reason);
	void DeleteStoragePet(int pet_index, const char * reason);
	int GetPetSkillMaxCount(int pet_index);
	PetParam* __GetPetParam(int pet_index);
	PetParam * GetPetParamFromStorageNoConst(int pet_index);
	void RecordGetPet(unsigned short pet_id);	//图鉴功能	记录获取的宠物idmeiy
	void BuildPetSkinIdMap(bool is_re = false);   //构建宠物能穿那些皮肤id，方便战力计算
	void ModifyAutoMoveOnAddPet(PetParam& pet_param);
	void ModifyAutoMoveMap(int pet_id, short auto_move_type, short auto_skill_id, short auto_skill_level);
	bool CheckDuplicateUniqueID(long long unique_id, int exclude_index = -1);
	void OnSetQueryAttrList(QueryAttrList & pet_query_attr, const AttributeList & pet_attr);

	//重算魂器等级
	void RecalePetSoul(long long add_exp , short *color, short *level, short *steps, int *m_exp, int *m_max_exp, unsigned short *attr_num, AttrInfo * attr_list[MAX_PET_SOUL_EQUIPMENT_ATTR_NUM]);

	PetParam* MutablePetParam(int pet_id, long long unique_item_id, ARG_OUT int* pet_idx);

	RoleModuleManager *m_role_module_mgr;

	int			m_pet_count;

	AttributeList	m_reason_list[MAX_REASON_TYPE_LIST_NUM];		//使用该属性来源需先调用GetAttrListReason接口
	//宠物总属性(包含技能书)
	AttributeList	m_pet_base_attr[ItemNamespace::MAX_PET_GRID_NUM];
	//宠物总属性(不包含技能书),用来计算战力，宠物战力有点特别，技能跟据其他配置加战力
	AttributeList	m_pet_attr_no_skill[ItemNamespace::MAX_PET_GRID_NUM];
	int				m_pet_capability_list[ItemNamespace::MAX_PET_GRID_NUM];
	ItemGridData	m_grid_list[ItemNamespace::MAX_PET_GRID_NUM];
	bool			m_old_state[ItemNamespace::MAX_PET_GRID_NUM];
	bool			m_dirty_mark[ItemNamespace::MAX_PET_GRID_NUM];


	int m_pet_storage_grid_use_num;				
	ItemGridData	m_storage_grid_list[ItemNamespace::MAX_PET_STORAGE_GRID_NUM];
	bool			m_storage_old_state[ItemNamespace::MAX_PET_STORAGE_GRID_NUM];
	bool			m_storage_dirty_mark[ItemNamespace::MAX_PET_STORAGE_GRID_NUM];

	bool			m_pet_grid_is_change;


	std::set<unsigned long long> m_is_platform_battle;

	PetCommonParam m_common_param;
	PetSkinParam m_pet_skin_param;
	GodPrintBagParam m_god_print_param;
	GodPrintBagWishLogParam m_god_print_log_param;
	NewGodPrintBagWishLogParam m_new_god_print_log_param;
	bool m_is_build_skin_id_map;
	std::map<int, std::vector<int> > m_pet_has_skin_id;

	std::map<long long, std::set<int> > m_pet_benediction_map;			// 宠物祝福映射 key:unique_id(主宠) second:pet_index(副宠)
	std::set<long long> m_pet_benediction_vice_set;						// 宠物祝福副宠 value:unique_id		

	bool m_is_update;

	typedef std::map<int, PetAutoMove> PetAutoMoveMap;
	PetAutoMoveMap m_auto_move_map;

	char m_init_flag;

	bool m_god_print_item_old_state[NEW_MAX_PET_GOD_PRONT_BAG_NUM];
	bool m_god_print_item_dirty_mark[NEW_MAX_PET_GOD_PRONT_BAG_NUM];
	bool m_god_print_item_bag_is_change;
	GodPrintItem * m_new_god_print_bag[NEW_MAX_PET_GOD_PRONT_BAG_NUM];//新神印背包
	int GetEmptyBag();
	int GetBagEmptyNum();
	void ClearGodPrintItem(int index,int clear_reason = 0);
	void SetGodPrintDirtyMark(int index, int put_reason = 0);

	int m_pet_fei_sheng_index;

	std::map<int, int> m_strengthen_buff_type_index_to_strengthen_buff_level;
	PetStrengthenBuffParam m_strengthen_buff_param;


	std::map<long long, int> unique_item_id_to_pet_index;
	std::map<long long, int> unique_item_id_to_pet_storage_index;

	std::map<int, int>	m_pet_quality_to_str_level;		//当前拥有宠物品质最高强化等级，检测解锁炼妖炉子用到，不用每次遍历宠物
};

enum PET_CAN_DELETE_EXCLUDE_FLAG
{
	PET_CAN_DELETE_EXCLUDE_FLAG_PET_SOUL =  0, //不包含魂器判断
	PET_CAN_DELETE_EXCLUDE_PLATFORM_BATTLE = 1, //不包含擂台站判断
};

#endif // __PET_HPP__