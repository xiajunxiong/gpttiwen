#ifndef __ROLE_QUERY_PARAM_HPP__
#define __ROLE_QUERY_PARAM_HPP__

#include <string>

#include "roleinitparam.h"
#include "battle/battle_def.hpp"
#include "servercommon/basedef.h"
#include "servercommon/petdef.h"
#include "servercommon/partnerdef.h"
#include "servercommon/commondata.hpp"
#include "servercommon/facescoredef.h"
#include "servercommon/smartmountssystemdef.hpp"

static const int MAX_QUERY_HANDLER_INVAILD_TIME = 300;
static const int MAX_JIE_YI_ZU_MAX_ROLE_ITEM_NUM = 4;

class RoleQueryParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int				role_id;								// ID
	GameName		role_name;								// 名字
	int				avatar_type;							// 角色类型
	int				headshot_id;							// 头像ID
	int				sex;									// 性别
	int				prof;									// 职业
	int				vip_level;
	int				rank_score;								// 排位分(幻界战场)
	char			is_finish_ding_ji;						// 是否完成了定级赛(幻界战场)
	short			level;
	int				top_level;								// 巅峰等级
	long long		avatar_timestamp;
	int				kill_value;								// 杀戮值
	int				course_fraction_count;					// 总历程点数

	RoleInitParam role_init_param;
	Appearance appearance_info;
	PetCommonParam pet_common_param;
	NewPartnerParam partner_param;
	CommonDataParam common_param;
	GodPrintBagParam god_print_param;
	FaceScoreParam face_score_param;
	SmartMountsSystemParam smart_mounts_param;
};

#endif // __ROLE_OTHER_INIT_PARAM_HPP__