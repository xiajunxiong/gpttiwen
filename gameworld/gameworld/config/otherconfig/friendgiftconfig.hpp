#ifndef __FRIEND_GIFT_CONFIG_HPP__
#define __FRIEND_GIFT_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include <map>
#include <set>

struct FriendGiftOtherCfg
{
	FriendGiftOtherCfg() : hook_add(0), first_talk_add(0), intimacy_max(0) {}

	int hook_add;					// 组队增加亲密度  挂机和PVP不加
	int first_talk_add;				// 每日首日私聊亲密度增加
	int intimacy_max;				// 亲密度上限值
};

enum SEND_FLOWER_TYPE
{
	SEND_FLOWER_TYPE_INVALID = 0,		 // 无效
	SEND_FLOWER_TYPE_NORMAL = 1,		 // 普通类型 赠送会发聊天信息
	SEND_FLOWER_TYPE_HIGH = 2,			 // 高级类型 赠送会发聊天信息 + 传闻 + 特效
	SEND_FLOWER_TYPE_MAX,
};

struct FriendGiftSendFlowerCfg
{
	FriendGiftSendFlowerCfg() :intimacy_add(0), type(0), is_fast_buy(false), is_batch(false) {}

	int intimacy_add;
	int type;
	bool is_fast_buy;
	bool is_batch;
};

class FriendGiftConfig : public ILogicConfig
{
public:
	FriendGiftConfig();
	virtual ~FriendGiftConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const FriendGiftOtherCfg & GetOtherCfg() const { return m_other_cfg; }
	const FriendGiftSendFlowerCfg * GetSendFlowerCfg(int _item_id) const;

	bool IsExcludeFightType(int _type);

private:
	int InitOtherCfg(TiXmlElement* RootElement, std::string& errormsg);
	int InitSendFlowersCfg(TiXmlElement* RootElement, std::string& errormsg);
	int InitExcludeFightTypeCfg(TiXmlElement* RootElement, std::string& errormsg);

	FriendGiftOtherCfg							m_other_cfg;
	std::map<int, FriendGiftSendFlowerCfg >		m_send_flowers_cfg_map;
	std::set<int >								m_exclude_fight_type_set;		// 好友组队战斗不会加亲密的战斗类型列表
};

#endif		// __FRIEND_GIFT_CONFIG_HPP__