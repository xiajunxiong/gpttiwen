#ifndef __COLLECTION_HPP__
#define __COLLECTION_HPP__

#include "servercommon/collectiondef.hpp"
#include "servercommon/attributedef.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Collection
{
public:
	Collection();
	~Collection();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const CollectionParam &param);
	void GetOtherInitParam(CollectionParam *param);

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);
	void OnChangeProfession(int old_prof, int new_prof);

	void OnRoleLogin();

	void OnActivateCollection(int seq);
	void OnConvert(int seq);

	int GetActiveNumByColor(int color);		//获取该品质激活的图鉴数量

	void OnPetCollection(int pet_id);
	void OnPartnerCollection(int partner_id);
	void OnFaBaoCollection(int fabao_id);
	void OnPetSoulCollection(int pet_soul_item_id, bool need_sync = true);		//抽完魂器收藏标记
	void OnHolyEquipCollection(int shengqi_id);

	void OnReadyCollection(int type, int active_id);		//激活前允许收录接口
	void ActiveCollection(int seq);
	bool IsActiveCollection(int seq);

	void SendIncludedAllInfo();
	void SendSignCollection(int seq);

	void SendAllInfo();
	void SendSingleInfo(int seq);

	void OnKillMonsterDropCard(int monster_num, int* monster_id_list);

	int GetCollectionActiveTuJianNum();

	void UpLevelAtlasCard(int seq);
	void SendAtlasCardInfo();
	void SendAtlasCardSingleInfo(int seq);
private:

	int DropCard(int monster_id, int monster_num, int max_drop_num);

	RoleModuleManager *m_role_module_mgr;

	CollectionParam m_param;
	AttributeList m_attrs_add;
};


#endif // !__COLLECTION_HPP__
