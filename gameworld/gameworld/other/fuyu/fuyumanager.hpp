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
	typedef std::pair<int, int> PartnerWear;	// first-���id(1- 40), second-װ������(0-2)
	typedef std::pair<int, int> FuYuGrid;		// <������λ:column, ��������:bag_index>

	struct SPTimeOut
	{
		SPTimeOut(uint32_t timeout, ItemID item, int64_t unique_id):timeout_timestamp(timeout), item_id(item), item_unique_id(unique_id)
		{}

		bool operator< (const SPTimeOut & _t) const
		{	// ʱ����������ν����Ҫ�����ж�
			return timeout_timestamp < _t.timeout_timestamp;
		}

		uint32_t timeout_timestamp;		// ��ʱʱ���
		ItemID	 item_id;				// ����id
		int64_t  item_unique_id;		// ��ƷΨһid
	};

	enum FU_YU_SEND_REASON
	{
		FU_YU_SEND_REASON_PUT_ON = 0,			// ��������
		FU_YU_SEND_REASON_TAKE_OFF = 1,			// ж�·���
		FU_YU_SEND_REASON_UPGRADE_SUCC = 2,		// ǿ������ɹ�
		FU_YU_SEND_REASON_UPGRADE_FAIL = 3,		// ǿ������ʧ��
		FU_YU_SEND_REASON_SP_TIMEOUT = 4,		// ��Ч��ʱʧЧ
		FU_YU_SEND_REASON_SUBLIME = 5,			// ��������
		FU_YU_SEND_REASON_SWITCH = 6,			// ����ת��
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

	// ��ȡ��Ч
	void GetPassiveSkill(int partner_id, int max_other_sp_num, int * out_other_sp_num, int * out_other_sp_list);

	void Update(unsigned long interval, time_t now_second);
	void OnRoleLogin();

	void OnClientReq(Protocol::CSFuYuReq* req);

	// ����
	void PutOn(PartnerWear partner_wear, int column, int bag_index);
	// ����
	void TakeOff(PartnerWear partner_wear);
	// �ֽ�
	void Decompose(int column, int bag_index);
	// ǿ������(����)
	void Upgrade(PartnerWear partner_wear, ItemID consume_id, bool is_protect);
	// ǿ������(����)
	void Upgrade(int column, int bag_index, ItemID consume_id, bool is_protect);

	// ����
	void SublimeToWear(PartnerWear target_fuyu, FuYuGrid consume_tianci_fuyu, FuYuGrid consume_fuyu);
	void SublimeToBag (FuYuGrid target_fuyu, FuYuGrid consume_tianci_fuyu, FuYuGrid consume_fuyu);

	// ת��
	void ConvertToWear(PartnerWear target_fuyu, int target_sp_id1, int target_sp_id2);
	void ConvertToBag (FuYuGrid target_fuyu, int target_sp_id1, int target_sp_id2);

	// ���·�����Ϣ
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

	// ��װ��WearInfoChange
	void SendSingleInfo(int wear_index, int reason);
	void SendSingleInfo(PartnerWear partner_wear, int reason);
private:
	RoleModuleManager * m_role_module_mgr;

	std::map<int, AttributeList> m_partner_attr_list;	// ����, key-���id
	std::queue<SPTimeOut> m_sp_timeout;			// ��Ч��ʱ����

	bool		 m_fuyu_wear_is_change;
	bool		 m_fuyu_old_state[ItemNamespace::MAX_FU_YU_GRID_NUM];
	ItemGridData m_wear_fuyu_grid[ItemNamespace::MAX_FU_YU_GRID_NUM];

	FuYuCommonParam m_param;
};
UNSTD_STATIC_CHECK((PARTNER_NUM_MAX * WEAR_FU_YU_WEAR_NUM) <= ItemNamespace::MAX_FU_YU_GRID_NUM)


#endif