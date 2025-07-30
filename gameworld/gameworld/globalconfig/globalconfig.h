#ifndef __GLOBALCONFIG_H__
#define __GLOBALCONFIG_H__

#include "keyconfig.h"
#include "otherglobalconfig.h"
#include "gamedef.h"
#include "dynamicconfig.hpp"
#include "globalconfig/spidconfig.h"

#define GLOBALCONFIG GlobalConfig::Instance()

class GlobalConfig
{
public:
	static GlobalConfig * Instance();

	bool Init(const std::string &path, const std::string &configname, std::string *err);
	static bool Reload(const std::string &path, const std::string &configname, std::string *err);

	const unsigned int GetKeyConfig_ZoneMatrixX() const;
	const unsigned int GetKeyConfig_ZoneMatrixY() const;
	const unsigned int GetKeyConfig_RoleAoiX() const;
	const unsigned int GetKeyConfig_RoleAoiY() const;
	const unsigned int GetKeyConfig_RoleAoiLeaveMoreThenEnter() const;
	const unsigned int GetKeyConfig_RoleAoiSensitiveDistance() const;
	const unsigned int GetKeyConfig_PetAoiSensitiveDistance() const;
	const unsigned int GetKeyConfig_RoleSaveTime() const;
	const unsigned int GetOtherConfig_SortInOrder_Interval() const;						// 获取背包物品整理的时间间隔

	const bool GetOtherConfig_IsCheckValid() const;										// 是否检查外挂移动
	const unsigned int GetOtherConfig_MaxMoveSpeed() const;								// 游戏中可能的最大速度
	const unsigned int GetOtherConfig_MoveCheck_Length() const;							// 获取允许的移动非法距离
	const unsigned int GetOtherConfig_MaxTransportLength() const;						// 获取传送点的最远可请求距离

	int GetGetCmdTimeInterval_S() { return m_other.get_cmd_time_interval_s; }

	bool IsGmOpen() const;
	bool IsOpenMsgCache() const;
	bool IsOpenBigCross() const;
	int GetCenterCityBornPos(Posi &born_pos);

	DynamicConfig & GetDynamicConfig() { return m_dynamicconfig; }
	bool GetWorldTalkIsLimit() { return 0 != m_other.world_talk_is_limit; }
	int GetWorldTalkEffectMsgMinLen() { return m_other.world_talk_effect_msg_min_len; }
	int GetWorldTalkTorelateSameMaxCount() { return m_other.world_talk_torelate_max_count; }
	int GetWorldTalkAccumulateCountResetInterval() { return m_other.world_talk_accumulate_count_reset_interval; }

	int GetTalkLevel() { return m_spidconfig.GetTalkLevel(); }
	int GetTalkLevelXianMeng() { return m_spidconfig.GetTalkLevelXianMeng(); }
	int GetTalkLevelSiLiao() { return m_spidconfig.GetTalkLevelSiLiao(); }
	int GetTalkLevelCross() { return m_spidconfig.GetTalkLevelCross(); }

	int GetSpeakerLevel() { return m_spidconfig.GetSpeakerLevel(); };
	int GetCrossSpeakerLevel() { return m_spidconfig.GetCrossSpeakerLevel(); };
	int GetServerMaxRoleNum() { return m_other.server_max_role_num; }
	int GetCreateChatGroupLevel() { return m_spidconfig.GetCreateChatGroupLevel(); }

	int GetSingleChatNumLevel() { return m_spidconfig.GetSingleChatNumLevel(); };
	int GetSingleChatNumTime() { return m_spidconfig.GetSingleChatNumTime(); };
	int GetSingleChatNumNum() { return m_spidconfig.GetSingleChatNumNum(); };

	int GetForbidTalkTipsLevel() { return m_spidconfig.GetForbidTalkTipsLevel(); }

	int GetBuyLimit() { return m_spidconfig.GetBuyLimit(); }
	bool IsCanSell() { return m_spidconfig.IsCanSell(); }
	const std::vector<ItemConfigData>* GetRealNameReward() { return m_spidconfig.GetRealNameReward(); }

public:
	GlobalConfig() {}
	~GlobalConfig() {}

	globalconfig::KeyConfig m_key;
	globalconfig::OtherGlobalConfig m_other;
	DynamicConfig m_dynamicconfig;
	globalconfig::SpidConfig m_spidconfig;
};

#endif
