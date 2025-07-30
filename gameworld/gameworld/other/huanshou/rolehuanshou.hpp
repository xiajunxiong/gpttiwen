#ifndef __ROLE_HUAN_SHOU_HPP__
#define __ROLE_HUAN_SHOU_HPP__

#include "servercommon/huanshoudef.hpp"
#include "other/attributelist.hpp"
#include "item/itemgriddata.h"

using namespace ItemNamespace;

class RoleModuleManager;
class RoleHuanShou
{
public:
	RoleHuanShou();
	~RoleHuanShou();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, int bag_list_num, const ItemListParam::ItemListData *bag_data_list[], const RoleHuanShouParam & param);
	void GetInitParam(RoleHuanShouParam * param);
	void GetChangeBagList(int *count, ItemListParam::ItemListData *save_list);				// 保存背包信息

	void Update(unsigned int now_second);
	void OnRoleLogin();

	//幻兽背包
	bool Put(const ItemDataWrapper & item_wrapper, int put_reason, int *out_index = NULL);
	bool ConsumeItem(ItemID item_id, int num, const char * reason);
	bool ConsumeItemByIndex(int index, int num, const char * reason);
	const ItemGridData * GetItemGridData2ByBag(int index) const;							// 获取背包格子信息
	bool GetItemGridWrapDataByBag(int index, ItemDataWrapper* out_wrapper) const;			// 获取背包格子信息
	int GetSpaceGridIdx()const;
	bool IsHasItemCount(int item_id, int num) const;
	bool HasBagNum(int grid_num) const;
	void SetBagGridDrity(int grid_index, int reason_type, bool to_notice_client = true, int put_reason = 0);
	void ClearBagGrid(int index);
	void ClearBagDirtyMark();


	void OnSendBagAllInfo();
	void OnSendBagSingleInfo(int index, short reason_type, short put_reason = 0);

	//属性计算
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc = true);

	//--幻兽操作
	//打开幻兽蛋
	void OnOpenHuanShouEgg(int bag_column, int bag_index);
	//使用主动消耗道具,获得指定幻兽
	bool OnOpenHuanShouEgg(int huanshou_id);
	//强化幻兽
	void OnStrHuanShouLevel(int huanshou_id);
	//学习技能
	void OnLearnSkill(int huanshou_id, int bag_index, int lock_flag);
	//保存技能
	void OnSaveSkill(int huanshou_id, bool is_save);
	//学习潜能点位
	void OnLearnPPointLevelUp(int huanshou_id, int point_index);
	//炼丹
	void OnLianDan();
	//附身
	void OnFuShen(int huanshou_id);
	//取消附身
	void OnCancelFuShen();
	//设置当前使用形象
	void OnSetCurUseImage(bool is_use_huanshou);

	//幻兽灵魄分解
	void OnHuanShouItemDecompose(std::set<short> decompose_index_list, int bag_column);


	void SendHuanShouAllInfo();
	void SendHuanShouSingleInfo(int huanshou_id);
	void SendOtherInfo();
	void OnSendNoticeInfo(short notice_type, short param1 = 0, int param2 = 0, int param3 = 0);

	//收集战斗特效
	void OnGetFuShenSkill(ARG_OUT BattleHuanShouSkillInfo * skill_info, ARG_OUT int * out_other_sp_list, int max_other_sp_num, short * out_other_sp_num);

	char GetFuShenID() const { return m_param.fushen_id; }
	char GetCurUseImage() const { return m_param.is_use_huanshou; }
	int GetFushenLevel() const;

	void GmActiveHuanShou(int huanshou_id);

private:
	void OnCheckLianDan(unsigned int now_second);

private:

	bool IsInvalidBagGrid(int index)const { return index < 0 || index >= ARRAY_ITEM_COUNT(m_grid_list); }
	bool IsInvalidId(int huanshou_id) const { return huanshou_id < 0 || huanshou_id >= ARRAY_ITEM_COUNT(m_param.huan_shou_list); }
	//此处仅限于获取打书列表
	void GetPassiveSkillGridList(const RoleHuanShouData & huanshou_data, std::vector<SkillPro> & skill_list, int skill_limit)const;

	RoleModuleManager * m_module_mgr;
	RoleHuanShouParam m_param;

	AttributeList m_attr_list;

	//幻兽背包信息
	bool m_old_state[MAX_HUAN_SHOU_GRID_NUM];
	bool m_dirty_mark[MAX_HUAN_SHOU_GRID_NUM];
	bool m_knapsack_is_change;
	ItemGridData* m_grid_list[MAX_HUAN_SHOU_GRID_NUM];

};

#endif