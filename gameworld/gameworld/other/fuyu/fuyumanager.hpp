#ifndef __FUYU_MANAGER_H__
#define __FUYU_MANAGER_H__

#include "other/attributelist.hpp"
#include "item/itemgriddata.h"
#include "other/partner/partner.hpp"
#include "servercommon/fuyudef.h"
#include "servercommon/struct/itemlistparam.h"
#include "protocol/msgfuyu.h"
#include <map>
#include <queue>


class RoleModuleManager;
class Role;

class FuYuManager
{
	typedef std::pair<int, int> PartnerWear;	// first-伙伴id(1- 40), second-装备索引(0-2)
	typedef std::pair<int, int> FuYuGrid;		// <背包栏位:column, 背包格子:bag_index>

	struct SPTimeOut
	{
		SPTimeOut(uint32_t timeout, ItemID item, int64_t unique_id):timeout_timestamp(timeout), item_id(item), item_unique_id(unique_id)
		{}

		bool operator< (const SPTimeOut & _t) const
		{	// 时间戳相等无所谓不需要额外判断
			return timeout_timestamp < _t.timeout_timestamp;
		}

		uint32_t timeout_timestamp;		// 超时时间戳
		ItemID	 item_id;				// 符玉id
		int64_t  item_unique_id;		// 物品唯一id
	};

	enum FU_YU_SEND_REASON
	{
		FU_YU_SEND_REASON_PUT_ON = 0,			// 穿戴符玉
		FU_YU_SEND_REASON_TAKE_OFF = 1,			// 卸下符玉
		FU_YU_SEND_REASON_UPGRADE_SUCC = 2,		// 强化符玉成功
		FU_YU_SEND_REASON_UPGRADE_FAIL = 3,		// 强化符玉失败
		FU_YU_SEND_REASON_SP_TIMEOUT = 4,		// 特效超时失效
		FU_YU_SEND_REASON_SUBLIME = 5,			// 符玉升华
		FU_YU_SEND_REASON_SWITCH = 6,			// 符玉转换
	};

public:
	FuYuManager();
	~FuYuManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const FuYuCommonParam & param, const std::vector<const ItemListParam::ItemListData * >& fuyu_item_list);
	void GetInitParam(FuYuCommonParam * param);

	void GetChangeFuYuItemList(int * count, ItemListParam::ItemListData * save_list);
	void ClearDirtyMark();

	void ReCalcPartnerAttr(int partner_id, AttrCon &base_add, bool is_recalc = true);
	void ReCalcAllPartnerAttr();

	// 获取特效
	void GetPassiveSkill(int partner_id, int max_other_sp_num, int * out_other_sp_num, int * out_other_sp_list);

	void Update(unsigned long interval, time_t now_second);
	void OnRoleLogin();

	void OnClientReq(Protocol::CSFuYuReq* req);

	// 穿戴
	void PutOn(PartnerWear partner_wear, int column, int bag_index);
	// 脱下
	void TakeOff(PartnerWear partner_wear);
	// 分解
	void Decompose(int column, int bag_index);
	// 强化符玉(穿戴)
	void Upgrade(PartnerWear partner_wear, ItemID consume_id, bool is_protect);
	// 强化符玉(背包)
	void Upgrade(int column, int bag_index, ItemID consume_id, bool is_protect);

	// 升华
	void SublimeToWear(PartnerWear target_fuyu, FuYuGrid consume_tianci_fuyu, FuYuGrid consume_fuyu);
	void SublimeToBag (FuYuGrid target_fuyu, FuYuGrid consume_tianci_fuyu, FuYuGrid consume_fuyu);

	// 转化
	void ConvertToWear(PartnerWear target_fuyu, int target_sp_id1, int target_sp_id2);
	void ConvertToBag (FuYuGrid target_fuyu, int target_sp_id1, int target_sp_id2);

	// 更新符玉信息
	void WearInfoChange(int wear_index, int reason, bool is_notice = true);
	void WearInfoChange(PartnerWear partner_wear, int reason, bool is_notice = true);

	void GmPutFuYu(ItemID item_id, int sp_group);

private:	
	int			SwitchTo(PartnerWear partner_wear);
	PartnerWear SwitchTo(int index);

	ItemGridData* GetFuYuWearNode(int wear_idx);
	ItemGridData* GetFuYuWearNode(PartnerWear partner_wear);

	void UpdateTimeOut();

	void SendAllInfo();

	// 包装在WearInfoChange
	void SendSingleInfo(int wear_index, int reason);
	void SendSingleInfo(PartnerWear partner_wear, int reason);
private:
	RoleModuleManager * m_role_module_mgr;

	std::map<int, AttributeList> m_partner_attr_list;	// 属性, key-伙伴id
	std::queue<SPTimeOut> m_sp_timeout;			// 特效超时队列

	bool		 m_fuyu_wear_is_change;
	bool		 m_fuyu_old_state[ItemNamespace::MAX_FU_YU_GRID_NUM];
	ItemGridData m_wear_fuyu_grid[ItemNamespace::MAX_FU_YU_GRID_NUM];

	FuYuCommonParam m_param;
};
UNSTD_STATIC_CHECK((PARTNER_NUM_MAX * WEAR_FU_YU_WEAR_NUM) <= ItemNamespace::MAX_FU_YU_GRID_NUM)


#endif