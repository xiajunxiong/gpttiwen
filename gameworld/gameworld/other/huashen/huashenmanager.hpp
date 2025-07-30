#ifndef __HUASHEN_MANAGER_HPP__
#define __HUASHEN_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/huashendef.hpp"
#include "item/itemgriddata.h"
#include "servercommon/struct/itemlistparam.h"
#include "other/skill/roleskill.hpp"
#include "other/attributelist.hpp"
#include "protocol/msghuanjiezhanchang.h"
#include "other/pet/pet.hpp"
#include "other/medal/medal.hpp"
#include <set>
#include "servercommon/rankdef.hpp"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"

using namespace ItemNamespace;

enum HUASHEN_KNAPSACK_OP_TYPE
{
	HUASHEN_KNAPSACK_OP_TYPE_UPDATE = 0,		//更新背包数据
	HUASHEN_KNAPSACK_OP_TYPE_ADD    = 1,		//添加道具
	HUASHEN_KNAPSACK_OP_TYPE_CONSUME = 2,		//消耗道具
	HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN = 3,		//一键同步

	HUASHEN_KNAPSACK_OP_TYPE_MAX,
};
UNSTD_STATIC_CHECK(HUASHEN_KNAPSACK_OP_TYPE_MAX == 4);

//
static const int HUASHEN_GRID_INDEX_TYPE_BASE = 1000;		
enum HUASHEN_GRID_INDEX_TYPE
{
	HUASHEN_GRID_INDEX_TYPE_KNAPSACK = 0,		//背包
	HUASHEN_GRID_INDEX_TYPE_EQUIPMENT = 1,		//装备
	HUASHEN_GRID_INDEX_TYPE_JEWELRY = 2,		//灵饰
	HUASHEN_GRID_INDEX_TYPE_CRYSTAL = 3,		//元素水晶
	HUASHEN_GRID_INDEX_TYPE_MEDAL = 4,			//勋章
	HUASHEN_GRID_INDEX_TYPE_PET = 5,			//宠物
	HUASHEN_GRID_INDEX_TYPE_PET_EQUIPMENT= 6,	//宠物装备
};

class RoleModuleManager;
class HuaShenManager
{
public:
	HuaShenManager();
	~HuaShenManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, int param_list_num, const ItemListParam::ItemListData *param_data_list[], const HuaShenParam & param, 
				int pet_param_list_num, const ItemListParam::ItemListData *pet_param_data_list[], int medal_param_list_num, const ItemListParam::ItemListData *medal_param_data_list[]);	
	void GetInitParam(HuaShenParam * param);
	void InitEnd();

	void Update(time_t now_second);
	
	void OnSeasonFirstReset();			//本赛季选择战场后重置一些信息并同步信息
	void OnSeasonEndReset(bool is_reset_dingji_flag = true);			//赛季结束重置数据

	void OnMonthChange();				
	void OnEnterKeySynchronize(bool is_notice = true);		//进入幻界战场一键同步数据
	void OnSynKnapackData(bool is_element_crystal = false);	//化身界面购买道具后同步背包
	bool IsSeasonInTime();				//玩家是否已获得化身
	void OnRoleLogin();					
	void SetRoleInfo(int rank_score, int is_finis_ding_ji);	
	int GetRankScore() { return m_param.rank_score; }
	QueryHuanJieInfo GetQueryInfo();

	void ReCalcAttr(int recalc_reason, const char* recalc_reason_str, bool is_init = false);
	void SendRoleAttrReasonList(int param1);					//下发化身角色属性来源列表
	int GetPetAttrReasonList(int pet_index);					//获得宠物属性来源列表
	void SendPetAttrReasonList(int pet_index);					//下发宠物属性来源列表
	void OnRoleChangeProf();		//角色转职,化身洗点

	int GetHuaShenLevel() { return m_param.max_level; }

	bool SynBenZunItemData(ItemDataWrapper * item_grid_data, int op_type = HUASHEN_KNAPSACK_OP_TYPE_UPDATE, bool is_notice = false);		// 同步本尊道具信息
	bool SynBenZunItemData(ItemGridData * item_grid_data, int op_type = HUASHEN_KNAPSACK_OP_TYPE_UPDATE, bool is_notice = false);			// 同步本尊道具信息
	bool IsCheckItemIdentical(long long item_unique_id, int key_index);

	//化身背包操作(符合化身的道具都是不可堆叠)
	bool ConsumeItem(long long item_unique_id, bool is_notice = false);							
	bool ConsumeItem(int key_index, bool is_notice = false, bool is_key_syn_chronize = false);
	bool InOrder(bool ignore_time_interval = false);											// 整理背包
	bool Put(ItemDataWrapper & item_grid_data, bool is_notice = false, int *out_index = NULL);	// 添加物品	化身背包格子数量固定比本尊人物栏多100,此处就不做检测背包
	bool Consume(int index, bool is_notice = false);											// 消耗道具
	bool GetItemGridData(int index, ItemGridData* item_grid_data);								// 获取背包格子信息
	const ItemGridData * GetItemGridData2(int index) const;										// 获取背包格子信息
	void GetChangeItemList(int *count, ItemListParam::ItemListData *save_list);					// 保存背包信息
	void ClearDirtyMark();
	void SetItemDirty(int index, bool to_notice_client, short reason_type, short put_reason = 0);
	void SendKnapsackInfo(short reason = Protocol::SCHuaShenKnapsackInfoAck::HUASHEN_SEND_REASON_NORMAL);
	void SendKnapsackSingleItemChange(int index, short reason_type, short put_reason = 0);
	
	//化身装备操作(逻辑在本尊装备模块)
	bool PutOn(short index, long long unique_id, int grid_index_type);			//给准备佩戴的物品添加hash映射   先消耗道具,后添加映射
	bool TakeOff(long long unique_id);											//删除之前装备映射			   先删除映射,后添加道具

	//化身宠物操作
	void SynBenZunAllPetData();																			//赛季开始同步本尊宠物所有数据
	bool PetDataUpdate(int pet_index, ItemDataWrapper * item_data);										//宠物数据更新
	bool PetEquipmentUpdate(int pet_index, ItemDataWrapper * item_data);								//宠物装备数据更新
	bool PetEquipmentReset(int pet_index, bool is_notice = false);										//宠物装备重置 
	bool AddPet(ItemDataWrapper & pet_data);															//添加宠物
	bool DeletePet(int pet_index, bool is_notice = false, bool is_key_syn_chronize = false);			//删除宠物
	bool SetPetState(int pet_index, int state);															//设置宠物状态	
	bool AssignAttributePoints(int pet_index, short con, short str, short def, short agi, short magic);	//分配属性点		
	bool OnRefreshPoints(int pet_index, bool is_notice = true);											//洗点		
	bool PutOnPetEquipment(int pet_index, int equipment_bag_index);										//穿戴宠物装备
	bool PutOffPetEquipment(int pet_index);																//脱下宠物装备
	void AddPetBendiction(long long vice_item_unique_id);												//添加宠物祝福
	//void OnPetUp(int pet_index, int cur_level);														//宠物升级
	//void OnPetIntensi(int pet_index, int pet_level);													//宠物强化
	//void OnPetChangeName(int pet_index, GameName name);												//宠物改名
	//void OnUseSkillBook(int pet_index, int replace_gift_index, int skill_id);							//宠物使用技能书
	//void OnPetChangeTalent(int pet_index, PetParam * pet_param);										//宠物洗练天赋保存
	//void OnComprehendPassiveSkill(int pet_index, PetParam * pet_param, int f_index);					//宠物领悟技能
	bool OnPetGridInfoReset(int pet_index);																//跟宠物相关信息重置
	void ReCalcPetAttr(int pet_index);																	//重算宠物属性
	int GetPetAttributeList(int pet_index, AttributeList& attr);										//获得宠物属性列表
	const AttributeList * GetPetAttrList(long long item_unique_id);										//获取宠物属性列表
	int GetPetStrBuffLevel(int pet_index);																//用于共鸣(返回此时最低强化等级,并判断该宠物状态)
	bool IsWaitForFightPet(int pet_index);																//判断宠物是否为休息状态
	int GetPetCapability(int pet_index);																//获得某个宠物的战力
	int GetPetSkillCapability(int pet_index);															//获取宠物技能的战力
	static int ReCalcHuaShenPetCapability(const Attribute* attrs);										//获取宠物属性的战力
	bool PetInvalid(int index);
	void GetChangePetItemList(int *count, ItemListParam::ItemListData *save_list);						//保存宠物数据
	void SetPetItemDirty(int index, bool to_notice_client = true, int dirty_type = 0);
	void ClearPetDirtyMark();
	void SendPetAllInfo(int reason = Protocol::SCHuaShenSendPetAllInfo::HUASHEN_PET_ALL_INFO_REASON_DEFAULT);
	void SendPetSingleInfo(int pet_index, int notie_type = Protocol::SCHuaShenSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
	void SendPetOtherInfo();
	bool SendPetAttrInfo(int pet_index);

	bool GetBattleCharacterData(int pet_index, PetBattleData* out_data, bool is_use_cur_hp_mp);			//获得宠物战斗数据
	PetEffectSkillId GetPetEffectiveSkill(int pet_index);												//获取宠物生效的技能id，返回相同类型效果高的技能id

	//化身修炼操作
	const AttributeList & ReCalcTrainAttr(AttributeList &base_add, bool is_recalc);
	const AttributeList & ReCalcTrainAttrPer(AttributeList &base_add, const AttributeList & cpy_attr_list);
	void SynBenZunTrain();							//同步本尊的人物、宠物修炼数据
	void OnTrainUp(int type, int skill_level);		//本尊人物修炼技能升级
	void OnPetTrainUp(int skill_level);				//本尊宠物修炼技能升级

	//化身法宝操作 (化身法宝暂时砍掉)
//	void ReCalcFaBaoAttr(AttributeList &base_add, bool is_recalc);
//	void FaBaoPutOn(int equip_bar_index, int fabao_index);
//	void FabaoTakeOff(int equip_bar_index);

	//化身进阶操作
	const AttributeList & ReCalcAdvAttr(AttributeList &base_add, bool is_recalc);
	void CalcMaxAdvTimes();					//计算此时化身的进阶等级

	//化身技能操作
	void OnLoginSynBenZunSkillInfo(RoleSkill::SkillItem * skill_list);				//玩家登陆初始化时同步技能信息
	RoleSkill::SkillItem* GetSkill(short skill_index);
	void AddSkill(RoleSkill::SkillItem * skill_item);								//本尊学习新技能
	void SkillUp(int skill_id, int skill_level);									//本尊学习技能升级
	const AttributeList & ReCalcSkillAttr(AttributeList &base_add, bool is_recalc);

	//化身潜能操作
	void AddAttributePointReq(short vitality, short strength, short power, short speed, short magic, int cur_plan_index);	//分配属性点
	void ClearAttributePointReq(int cur_plan_index);																		//洗点
	void SwitchAttrPlan(int plan_index);																					//切换属性方案
	void AttrPlanChangeName(int plan_index, const char * change_name);														//方案改名
	const AttributeList & ReCalcAttrPlan(AttributeList &base_add, bool is_recalc);
	void SendPlanInfo(int cur_index);
	void SendPlanListInfo();

	//化身勋章操作
	const AttributeList & ReCalcMedalAttr(AttributeList &base_add, bool is_recalc);
	void MedalPutOn(int grid_index,int index);												//穿戴勋章
	bool MedalTakeOff(int grid_index, bool is_notice = true);								//脱下勋章
	bool MedalUpdate(int grid_index, ItemDataWrapper * item_data);							//更新勋章
	bool MedalReset(int grid_index, bool is_notice);										//重置勋章栏		
	void GetChangeMedalItemList(int *count, ItemListParam::ItemListData *save_list);		//保存勋章数据
	int GetMedalSpeScore();																	//获取令牌特效评分
	void GetMedalBattleData(RoleBattleData * role_data);									//收集化身勋章战斗数据
	std::set<int> GetEffectSp(std::set<MedalSpPriority> *sp_list = NULL);					//获取已排序的勋章列表					
	void SynBenZunMedalGrid(const ItemDataWrapper& item, int medal_index);
	void ClearMedalDirtyMark();	
	void SendMedalInfo();

	void OnSetBattlePosition(int position);
	void SendBattlePosition();

	void OnInfoReq(Protocol::CSHuaShenInfoReq  req);	
	void SendAllHuaShenInfo();			//发送跟化身有关的信息
	void SendRoleInfo();				//下发化身基本信息
	void SendRoleAttrList();			//下发化身属性列表
	void SendSeasonInfo();				//下发赛季信息


	//组装战斗数据
	void CollectRoleBattleData(RoleBattleData* out_data, bool need_partner,bool need_pet);
	bool GetRoleBattleData(BattleCharacterData* out_data, bool is_use_cur_hp_mp);
	bool GetPetBattleData(short max_pet_num, short* out_pet_num, PetBattleData* out_data_list,
		short* out_fight_pet_idx, bool is_use_cur_hp_mp);

	void GetPersonRankInfo(RoleBaseInfo & _role_info);
	void GetPetRankInfo(int _pet_index , RoleBaseInfo & _role_info, PetPro & _pet_param , AttributeList & _pet_attr);
private:
	void RankUpdate();

	void ClearGrid(int index);
	void RefreshValidGridIndexEnd(int change_index);
	void OnDeletePetAddPetEquipDelayHandle(int pet_index);		//一键同步时，删除的宠物此时正穿戴着宠物装备
	void CheckAllItemIdentical();			//检测此时本尊与化身穿戴的装备是否一致,不一致则进行处理
	void SeasonReset();						//赛季重置
	void ClearAllItemData();				//清除所有道具

	void Reset()
	{
		memset(m_role_attr_reason_list, 0, sizeof(m_role_attr_reason_list));
		memset(m_pet_attr_reason_list, 0, sizeof(m_pet_attr_reason_list));
		m_attr_list.Reset();
		m_attrs_plan_add.Reset();
		m_attrs_medal_add.Reset();
	//	m_attrs_fabao_add.Reset();
		m_attrs_train_add.Reset();
		m_attrs_train_per.Reset();
		m_base_attr_prof.Reset();
		m_attrs_skill_add.Reset();
		m_pet_count = 0;
		for (int i = 0; i < MAX_HUASHEN_PET_GRID_NUM; i++)
		{
			m_pet_base_attr[i].Reset();
			m_pet_attr_no_skill[i].Reset();
			m_pet_grid_list[i].Clear();
			m_pet_old_state[i] = false;
			m_pet_dirty_mark[i] = false;
		}
		m_pet_grid_is_change = false;

		for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
		{
			m_medal_old_state[i] = false;
			m_medal_grid_list[i].Clear();
		}

		m_valid_grid_index_end = 0;
		m_knapsack_last_inorder_time = 0;
		memset(m_old_state, 0, sizeof(m_old_state));
		memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
		m_knapsack_is_change = false;
		memset(m_grid_list, 0, sizeof(m_grid_list));

		m_item_map.clear();
		m_delay_handle_map.clear();
		for (int i = 0; i < MAX_SKILL_NUM; i++)
		{
			m_skill_list[i].Reset();
		}

		max_role_train_level = 0;
		max_pet_train_level = 0;
		max_advance_times = 0;
		role_advance_times = 0;
		m_pet_has_skin_id.clear();
		m_next_rank_update_time = 0;
	}

	RoleModuleManager *m_module_mgr;
	HuaShenParam m_param;

	AttributeList m_role_attr_reason_list[MAX_REASON_TYPE_LIST_NUM];	//化身角色属性来源列表
	AttributeList m_pet_attr_reason_list[MAX_REASON_TYPE_LIST_NUM];		//化身宠物属性来源列表 使用之前 需要先调用GetPetAttrReasonList接口

	AttributeList m_attr_list;					//化身总属性
	AttributeList m_attrs_plan_add;				//化身潜能点增加的属性
	AttributeList m_attrs_medal_add;			//化身勋章属性
	//AttributeList m_attrs_fabao_add;			//化身佩戴法宝属性
	AttributeList m_attrs_train_add;			//化身修炼属性
	AttributeList m_attrs_train_per;			//化身修炼属性
	AttributeList m_base_attr_prof;				//化身进阶属性
	AttributeList m_attrs_skill_add;			//化身被动技能属性

	int			m_pet_count;
	//宠物总属性(包含技能书)
	AttributeList	m_pet_base_attr[MAX_HUASHEN_PET_GRID_NUM];
	//宠物总属性(不包含技能书)
	AttributeList	m_pet_attr_no_skill[MAX_HUASHEN_PET_GRID_NUM];
	ItemGridData	m_pet_grid_list[MAX_HUASHEN_PET_GRID_NUM];
	bool			m_pet_old_state[MAX_HUASHEN_PET_GRID_NUM];
	bool			m_pet_dirty_mark[MAX_HUASHEN_PET_GRID_NUM];
	bool			m_pet_grid_is_change;		// 宠物信息是否发生改变

	//勋章
	bool			m_medal_old_state[MAX_MEDAL_WEAR_GRID];
	ItemGridData	m_medal_grid_list[MAX_MEDAL_WEAR_GRID];	

	short m_valid_grid_index_end;				// 当前背包中非空格子的再下一个格子序号
	short m_curr_valid_grid_num;				// 当前背包所有栏的有效格数
	UInt32 m_knapsack_last_inorder_time;		// 上次背包整理时间

	bool m_old_state[HUASHEN_ITEM_GRID_NUM_MAX];
	bool m_dirty_mark[HUASHEN_ITEM_GRID_NUM_MAX];
	bool m_knapsack_is_change;
	
	ItemGridData* m_grid_list[HUASHEN_ITEM_GRID_NUM_MAX];		//此时是new一份新的物品数据(拷贝本尊物品数据),而不是指向本尊背包的物品地址 
	std::map<long long, int> m_item_map;						//以unique_id为key, second为HUASHEN_GRID_INDEX_TYPE * HUASHEN_GRID_INDEX_TYPE_BASE + index
	std::map<long long, ItemDataWrapper> m_delay_handle_map;	//延迟道具处理

	RoleSkill::SkillItem m_skill_list[MAX_SKILL_NUM];		//技能信息

	int max_role_train_level;					//人物修炼限制最大等级
	int max_pet_train_level;					//宠物修炼限制最大等级

	int max_advance_times;						//符合化身等级限制的最大进阶次数[0,6]
	int role_advance_times;						//当前化身的进阶次数

	std::map<int, std::vector<int> >  m_pet_has_skin_id;

	unsigned int m_next_rank_update_time;				
};


#endif
