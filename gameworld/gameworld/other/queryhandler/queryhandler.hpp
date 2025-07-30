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
	GameName		role_name;			// �������
	GuildName		guild_name;			// ��������
	int				avatar_type;		// ��ɫ����
	int				prof;				// ְҵ
	int				vip_level;			// vip �ȼ�
	int				level;				// ����ȼ�
	int				top_level;			// �۷�ȼ�
	long long		avatar_timestamp;
	int				capability;			// ���ս��
	int				rank_score;			// ��λ��(�ý�ս��)
	short			pet_count;			// �����ս+��ս����
	short			partner_count;		// ����ս����	
	short			medal_count;		// ��������(ѫ��)
	char			is_finish_ding_ji;	// �Ƿ���ɶ�����(�ý�ս��)
	char			reserve_ch;
	int				kill_value;			// ɱ¾ֵ
	int				accessories_count;	// ��������
	short			smart_mounts_count;	// ��������
	short			holy_equip_count;	// ʥ����ս+��ս����
	QueryAttrList	user_attribute;		// Ĭ����ʾ7������ MAX_QUERY_ATTR_NUM - 7
	unsigned int    Invaild_timestamp;	// �û���ʧЧʱ��
	int				course_level;		// ��ǰ���̵ȼ�
	int				course_cur_fraction;// ��ǰ���̵�
	int				face_score_level;	// ��ֵ�ȼ�
	int				qing_yuan_role_id;	// ��Ե��ɫID
	GameName		qing_yuan_name;		// ��Ե����
	JieYIZuName 	jie_yi_name;		// ����������
	GameName		jie_yi_name_list[MAX_JIE_YI_ZU_MAX_ROLE_ITEM_NUM];	// �����Ա����

	Appearance appearance_info;
	EquipmentPro equip_list[Equipment::E_INDEX_MAX];
	JewelryPro jewelry_list[Jewelry::J_TYPE_MAX];
	int face_equip_list[FACE_ADDITION_TYPE_MAX];		// ��װ���ģ���ԴID������鿴��ֵϵͳ���ñ�	FACE_ADDITION_TYPE_MAX -- 10
	Protocol::MsgPetInfo pet_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];		// �������������		��ս+��ս		MAX_PET_FIGHT_AND_STANDBY_NUM - 5
	QueryAttrList pet_attr_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];	// ��������
	GodPrintListPro pet_god_print_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];	// ��������ӡ��װ
	PartnerQueryPro partner_list[PARTNER_SYNFIGHT_NUM_MAX];	// ��ս���  PARTNER_SYNFIGHT_NUM_MAX -- 5
	MedalPro medal_list[MAX_MEDAL_WEAR_GRID];				// �������� MAX_MEDAL_WEAR_GRID -- 3
	AccessoriesPro accessories_list[ItemNamespace::MAX_ACCESSORIES_INDEX_NUM];// �������� MAX_ACCESSORIES_INDEX_NUM -- 5
	SmartMountsSystemPro smart_mounts_list[MAX_SMART_MOUNTS_SYSTEM_NUM];	// ���������б� MAX_SMART_MOUNTS_SYSTEM_NUM -- 10
	HolyEquipPro holy_equip_list[MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM];			// ��ս�ʹ���ʥ���б� MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM -- 5
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

	void OnQueryOnline(int viewer_uid, Role * target);		// ��ѯ�����������

	void GetInfoToCacheNode(Role * target, QueryCacheInfo * cache_info);
	void GetInfoToCacheNode(const RoleQueryParam &p, const QueryItemListParam &item_list_param, const GodPrintItemListParam &god_print_item_list_param, QueryCacheInfo * cache_info);
	void SendInfoToViewer(int viewer_uid, QueryCacheInfo *cache_node);

	std::map<int, QueryCacheInfo> m_cache_map;			// ���߻��� - �������UID
};

#endif