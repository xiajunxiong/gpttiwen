#ifndef CHARAINITPARAM_H
#define CHARAINITPARAM_H

#include <string>

#include "common/tlvprotocol.h"

#include "servercommon/basedef.h"
#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

// 这个结构绝对不允许包含string这种东西 因为这个结构会memset的
// 这个结构体中所有要保存的字段都要显式地保存到Role或RoleAttrDetail表，加字段要记得加数据表
// 新增字段如果是有初始值，需要在roleinitattr和RMIRole的CreateRole中加逻辑
class RoleInitParam 
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	int				role_id;								// ID
	GameName		role_name;								// 名字
	
	char			authority_type;							// 权限 
	char			vip_level;								// VIP等级
	char			reserve_ch;								// 保留字
	char			color;									// 颜色

	short			avatar_type;							// 主角形象
	short			headshot_id;							// 头像ID
	int				top_level;								// 巅峰等级
	int				profession;								// 职业
	unsigned int    next_top_level_refresh_timestamp;		// 巅峰等级重置时间戳

	short			reserve_sh;
	short			role_fly_flag;							// 角色飞升阶段

	int				level;									// 等级
	int				scene_id;								// 场景ID
	int				scene_x;								// X
	int				scene_y;								// Y
	int				last_scene_id;							// 上次普通场景ID
	int				last_scene_x;							// X
	int				last_scene_y;							// Y

	int				online_time;							// 在线时长
	int				today_online_time;						// 今日在线时长
	int				lastday_online_time;					// 昨日在线时长
	long long		create_time;							// 创建时间
	
	Int64			coin_bind;								// 绑定金币
	Int64			immortal_coin;							// 仙币
	Int64			gold;									// 元宝（钻石）

	Int64           history_recharge_gold;					// 历史充值数

	long long		forbid_talk_time;						// 禁言时间
	int				forbid_talk_type;							// 禁言模式 \see FORBID_TYPE_MAX

	unsigned int	last_online_dayid;						// 上次更新的dayid
	unsigned int    last_reset_module_timestamp;			// 上次重置模块的时间戳

	long long		exp;									// 经验
	long long		max_exp;								// 最大经验

	int				capability;								// 战斗力
	int				max_capability;							// 历史最高战斗力

	long long 		avatar_timestamp;						// 头像时间戳， 实际用于客户端存放自定义上传的头像参数

	int				plat_spid;								// 平台渠道id

	int				weapon_id;								// 武器ID

	int				account_user_num;						// 该账号下第几个角色

	int				mini_game_id;							// 小游戏ID
private:

	
};

#endif

