#ifndef __WORK_SHOP_HPP__
#define __WORK_SHOP_HPP__

#include "servercommon/workshopdef.h"
#include "servercommon/configcommon.h"
#include "gameworld/other/workshop/godequipupconfig.hpp"

class RoleModuleManager;
class Workshop				// 工坊系统
{
public:
	Workshop();
	~Workshop();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager * GetRoleModuleManager() { return m_role_module_mgr; }

	void Init(RoleModuleManager * role_module_manager, const WorkShopParam & p_workshop_param);
	void GetWorkshopParam(WorkShopParam * p_workshop_param);

	//精工功能
	void NormalUltimate(int stone_type, int equip_posi, int equip_grid, int role_type = ROLE_TYPE_BENZUN);//普通精工
	void StrongUltimate(int stone_type, int equip_posi, int equip_grid, int role_type = ROLE_TYPE_BENZUN);//强化精工

	//**************************************实际已迁移

	//装备升星
	void EquipUpStar(int seq, int equip_type, int equip_posi, int equip_grid, int cost_grid, int cost_grid_2);

	//特殊星耀，神装功能
	void UpEquipGrade(int stone_type, int equip_posi, int equip_grid, int role_type = ROLE_TYPE_BENZUN);
	void BreakThroughGarde(int stone_type, int equip_posi, int equip_grid, int role_type = ROLE_TYPE_BENZUN);

	void EquipGemMounting(int stone_type, int equip_posi, int equip_grid, int stone_grid);	// 宝石镶嵌
	void EquipGemDismount(int stone_type, int equip_posi, int equip_grid, int stone_slot);	// 宝石拆卸
	void EquipGemUpgrades(int stone_type, int equip_posi, int equip_grid, int stone_slot);	// 宝石升级
	void ConfirmGemUpgrade();												// 确认升级宝石

	//装备点化/吸附/附魔/分解
	void DianHuaEquip(int equip_posi, int equip_grid, int dianhuashi_coloumn, int dianhuashi_grid);
	void SaveDianHuaEquip(int equip_posi, int equip_grid);
	void XiFuEquip(int equip_posi, int equip_grid);
	void FuMoEquip(int equip_posi, int equip_grid, int *new_spid = NULL, int *new_level = NULL);
	void ComposeDianHuaShi(int main_stone_idx, int cost_stone_1, int cost_stone_2);

	//-----------------------------等待迁移
	//更换装备特殊词条 _1是接受词条装备 _2是被摘除词条装备
	void ChangeEquipVoc(int equip_posi_1, int equip_grid_1, int equip_posi_2, int equip_grid_2, int role_type = ROLE_TYPE_BENZUN);
	void ChangeJewelryVoc(int equip_posi_1, int equip_grid_1, int equip_posi_2, int equip_grid_2, int role_type = ROLE_TYPE_BENZUN);
	void VocUltimate(int item_type, int equip_posi_1, int equip_grid_1, int voc_posi_1, int role_type);

	void OnActiveEquipVocAttr(int item_type, int equip_posi_1, int equip_grid_1, int voc_posi_1, int role_type);//激活装备词条属性
	void OnRefreshEquipVocAttr(int item_type, int equip_posi_1, int equip_grid_1, int lock_voc_flag, int role_type, bool use_gold = false);//重置装备词条属性
	void OnSaveEquipVocAttr(int item_type, int equip_posi, int equip_grid, int role_type);//保留属性

	void RefreshPetEquipMent(int compound_seq, int bag_index1);								// 洗练宠物装备
	void SaveRefreshPetEquipment(int bag_index1);											// 保存宠物装备洗练信息

	void RefreshPetEquipMentInPet(int compound_seq, int pet_index);							// 洗练宠物装备(宠物身上的)
	void SaveRefreshPetEquipmentInPet(int pet_index);										// 保存宠物装备洗练信息(宠物身上的)
	//**************************************实际已迁移

	void DisComposeDianHuaShi(int dianhuashi_coloumn, int dianhuashi_grid);//分解点化石

	void CheckTurnBack();

	void GMRandEnd(int equip_posi, int equip_grid, int level);

	//特殊操作（神装/星耀）装备职业转换，更替装备
	bool ChnageGodEquip(ItemGridData * change_item);

	//分解合成出来的装备
	void DeComposeItem(int grid_num, int *grid_idx_list);

	void CompoundItem(int compound_seq, int compound_num, int is_craft, bool is_huashen_buy = false);		// 制作物品
	bool DiscompoundItem(int compound_seq, int compound_num,int backpace_index);		// 分解物品
	bool ActiveNewRecipe(int active_id);												// 激活配方
	bool AutoActiveNewRecipe(int active_id, int item_id, bool is_auto_use = true, bool is_notice = true);		// 自动激活配方
	void BatchCompoundItem(int compound_seq, int compound_num);							// 批量合成装备
	bool RecipeIsActive(int active_id);

	void ExchangePetSkillBook(int exc_seq, std::vector<ItemPairCon> & grid_list);	// 兑换宠物技能书
	void ExchangeTarget(int exc_seq/*序号*/, int exc_times/*合成次数*/);					// 兑换宠物封印卡 合成钥匙、洗髓露
	
	//void SynthesisTarget(int _exc_seq,ItemID _target_id, ItemID _need_id,int _synthesis_type);						//合成物品

	void CraftJewelry(int list_id);											// 灵饰打造
	void ReforgeJewelry(int mate_grid_1, int mate_grid_2);					// 灵饰重锻
	void ChooseReforgeRetain(int list_idx);									// 选择重锻结果

	void CompoundPetEquipMent(int compound_seq, int bag_index1, int bag_index2);				// 宠物装备合成
	void BreakPetEquipMent(int bag_index);

	void SendAllWorkshopInfo();												// 发送所有信息
	void OnRoleLogin();		// 玩家登陆时，若重锻信息不为空，发送重锻信息

	void GmUnlockAllRecipe(bool is_unlock);			// 解锁所有配方
	void GmSetGodEquipChange() { m_workshop_param.god_equip_flag = 0; }
	void GmSetGodEquipChangeLevelFlag() { m_workshop_param.god_equip_change_flag = 0; }
	void GmSetGodEquipFixFlag() { m_workshop_param.check_god_equip_base_voc_flag = 0; }
	void GodEquipReback(); //神装返还
	bool CheckGodEquipItemDataReturn(const ItemGridData * item_data);
	bool CheckGodEquipItemDataChangeLevel(const ItemGridData * item_data);
	bool CheckGodEquipItemDataReturnBreak(ItemGridData * item_data, const GodEquipTurnBackNode * cfg);
	void GodEquipChange();

	//神装校验
	void FixGodEquipBaseVoc();
private:
	RoleModuleManager * m_role_module_mgr;
	WorkShopParam m_workshop_param;

	void SendRecipeActiveFlag(int active_id, bool is_notice = true);		// 发送单个配方激活
	void SendJewelryTimesInfo();					// 下发灵饰合成失败次数
	void SendJewelryReforgeOutcome();
	void SendComposeResult(bool is_succ, const ItemDataWrapper & get_item);


};

#endif