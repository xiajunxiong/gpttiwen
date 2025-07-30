#ifndef __QUERY_HANDLER_HPP__
#define __QUERY_HANDLER_HPP__

#include "item/itembase.h"
#include "protocol/msgrole.h"
#include "protocol/msgpet.h"
#include "other/pet/petconfig.hpp"
#include "equipment/equipmentmanager.h"
#include "servercommon/struct/rolequeryparam.hpp"
#include "servercommon/partnerdef.h"
#include <map>

struct QueryCacheInfo
{
	QueryCacheInfo()
	{
		role_id = 0;
		memset(role_name, 0, sizeof(GameName));
		memset(guild_name, 0, sizeof(guild_name));
		avatar_type = 0;
		prof = 0;
		vip_level = 0;
		level = 0;
		top_level = 0;
		avatar_timestamp = 0;
		capability = 0;
		rank_score = 0;
		pet_count = 0;
		partner_count = 0;
		medal_count = 0;
		is_finish_ding_ji = 0;
		kill_value = 0;
		reserve_ch = 0;
		accessories_count = 0;
		smart_mounts_count = 0;
		holy_equip_count = 0;
		user_attribute.Reset();
		Invaild_timestamp = 0;
		course_level = 0;
		course_cur_fraction = 0;
		face_score_level = 0;
		qing_yuan_role_id = 0;
		memset(qing_yuan_name, 0, sizeof(qing_yuan_name));
		jie_yi_name.Reset();
		memset(jie_yi_name_list, 0, sizeof(jie_yi_name_list));
		memset(face_equip_list, -1, sizeof(face_equip_list));
	}

	void GetName(GameName name) const
	{
		if (NULL != name)
		{
			F_STRNCPY(name, role_name, sizeof(GameName));
		}
	}

	int				role_id;
	GameName		role_name;			// 玩家名字
	GuildName		guild_name;			// 家族名字
	int				avatar_type;		// 角色类型
	int				prof;				// 职业
	int				vip_level;			// vip 等级
	int				level;				// 人物等级
	int				top_level;			// 巅峰等级
	long long		avatar_timestamp;
	int				capability;			// 玩家战力
	int				rank_score;			// 段位分(幻界战场)
	short			pet_count;			// 宠物出战+待战数量
	short			partner_count;		// 伙伴出战数量	
	short			medal_count;		// 令牌数量(勋章)
	char			is_finish_ding_ji;	// 是否完成定级赛(幻界战场)
	char			reserve_ch;
	int				kill_value;			// 杀戮值
	int				accessories_count;	// 配饰数量
	short			smart_mounts_count;	// 灵骑数量
	short			holy_equip_count;	// 圣器出战+待战数量
	QueryAttrList	user_attribute;		// 默认显示7个属性 MAX_QUERY_ATTR_NUM - 7
	unsigned int    Invaild_timestamp;	// 该缓存失效时间
	int				course_level;		// 当前历程等级
	int				course_cur_fraction;// 当前历程点
	int				face_score_level;	// 颜值等级
	int				qing_yuan_role_id;	// 情缘角色ID
	GameName		qing_yuan_name;		// 情缘名称
	JieYIZuName 	jie_yi_name;		// 结义组名称
	GameName		jie_yi_name_list[MAX_JIE_YI_ZU_MAX_ROLE_ITEM_NUM];	// 结义成员名称

	Appearance appearance_info;
	EquipmentPro equip_list[Equipment::E_INDEX_MAX];
	JewelryPro jewelry_list[Jewelry::J_TYPE_MAX];
	int face_equip_list[FACE_ADDITION_TYPE_MAX];		// 已装备的（资源ID，详情查看颜值系统配置表）	FACE_ADDITION_TYPE_MAX -- 10
	Protocol::MsgPetInfo pet_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];		// 不包括跟随宠物		出战+待战		MAX_PET_FIGHT_AND_STANDBY_NUM - 5
	QueryAttrList pet_attr_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];	// 宠物属性
	GodPrintListPro pet_god_print_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];	// 各宠物神印套装
	PartnerQueryPro partner_list[PARTNER_SYNFIGHT_NUM_MAX];	// 出战伙伴  PARTNER_SYNFIGHT_NUM_MAX -- 5
	MedalPro medal_list[MAX_MEDAL_WEAR_GRID];				// 穿戴令牌 MAX_MEDAL_WEAR_GRID -- 3
	AccessoriesPro accessories_list[ItemNamespace::MAX_ACCESSORIES_INDEX_NUM];// 穿戴配饰 MAX_ACCESSORIES_INDEX_NUM -- 5
	SmartMountsSystemPro smart_mounts_list[MAX_SMART_MOUNTS_SYSTEM_NUM];	// 激活灵骑列表 MAX_SMART_MOUNTS_SYSTEM_NUM -- 10
	HolyEquipPro holy_equip_list[MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM];			// 出战和待机圣器列表 MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM -- 5
};

class Role;
class QueryHandler
{
public:
	static QueryHandler & Instance();

	void OnQueryBack(UserID viewer_userid, int target_uid, int ret, const RoleQueryParam &p, const QueryItemListParam &item_list_param, const GodPrintItemListParam &god_print_item_list_param);
	void OnQueryOther(int target_uid, int viewer_uid,bool is_check_from_game_to_hidden = false);
	void OnUserLogout(Role * role);
	void SendQueryFollowUpInfo();
	void ClearUserCache(int uid);
private:
	QueryHandler() {};
	~QueryHandler() {};

	void OnQueryOnline(int viewer_uid, Role * target);		// 查询在线玩家数据

	void GetInfoToCacheNode(Role * target, QueryCacheInfo * cache_info);
	void GetInfoToCacheNode(const RoleQueryParam &p, const QueryItemListParam &item_list_param, const GodPrintItemListParam &god_print_item_list_param, QueryCacheInfo * cache_info);
	void SendInfoToViewer(int viewer_uid, QueryCacheInfo *cache_node);

	std::map<int, QueryCacheInfo> m_cache_map;			// 离线缓存 - 键：玩家UID
};

#endif