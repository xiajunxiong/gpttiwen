#ifndef __ROLE_GATHER_DREAM_HPP__
#define __ROLE_GATHER_DREAM_HPP__

#include "servercommon/gatherdreamdef.hpp"
#include <set>

class RoleModuleManager;
class RoleGatherDream
{
public:
	RoleGatherDream();
	~RoleGatherDream();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * role_module_manager, const GatherDreamParam & param);
	void GetInitParam(GatherDreamParam * param);

	void OnRoleLogin();
	void OndayChange(unsigned int old_dayid, unsigned int now_dayid);

	void OnAddGiftUid(int uid);				//赠予碎片后添加纪录
	void OnGetPet(int pet_id);				//获得宠物

	bool IsHaveGiftNum();					//判断今天是否还有剩余赠予次数
	bool IsCanTodayGiftUser(int uid);		//是否已经送过该玩家
	void SetPublishSendFlag(int is_send);
	bool IsPublishSend() { return m_param.gather_dream_publish_is_send_flag == 0 ? true : false; }
	void SetTodayPublishFlag(bool is_publish);
	bool IsTodayPublish() { return m_param.today_is_publish_gather_dream_flag > 0 ? true : false; }

	void SendGatherDreamList();
	void SendCanPublishList();

private:
	RoleModuleManager * m_module_mgr;
	
	GatherDreamParam m_param;
	std::map<int, bool> m_pet_id_map;				//以宠物ID为key	
	std::map<int, int> m_gift_role_uid_map;			//今天赠送玩家映射 key:role_uid
};




#endif