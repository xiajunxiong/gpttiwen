#ifndef __RAND_ACTIVITY_GIVE_SECRET_KEY_CONFIG_HPP__
#define __RAND_ACTIVITY_GIVE_SECRET_KEY_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/raexpelmonsterparam.hpp"
#include "item/itembase.h"

struct GiveSecretKeyCfg
{
	GiveSecretKeyCfg() : seq(0), type(0), parameter(0){}

	enum GIVE_SECRET_KEY_TYPE
	{
		GIVE_SECRET_KEY_TYPE_LEVEL = 1, //µÈ¼¶
		GIVE_SECRET_KEY_TYPE_FB = 2,	//ÃØ¾³id
	};
	int seq;
	int type;
	int parameter;
	ItemConfigData reward_item;
};

class RandActivityManager;

class RandActivityGiveSecretKeyConfig : public RandActivityConfig
{
public:
	RandActivityGiveSecretKeyConfig();
	virtual ~RandActivityGiveSecretKeyConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const GiveSecretKeyCfg * GetRewardCfg(int seq) const;
private:
	int InitGiveSecretKeyCfg(TiXmlElement *RootElement);

	std::map<int, GiveSecretKeyCfg> m_cfg_map;
};

#endif	//__RAND_ACTIVITY_GIVE_SECRET_KEY_CONFIG_HPP__