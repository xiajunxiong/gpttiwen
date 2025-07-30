#ifndef __FUNC_PREVIEW_CONFIG_HPP__
#define __FUNC_PREVIEW_CONFIG_HPP__

#include "servercommon/funcpreviewdef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

struct FuncPreviewCfg
{
	FuncPreviewCfg() : notice_id(0), start_level(0), end_level(0),
		version(0), reward_num(0), not_use(0){}

	static const int REWARD_NUM = 3;

	int notice_id;
	int start_level;
	int end_level;
	int version;
	int reward_num;
	int not_use;
	ItemConfigData reward_list[REWARD_NUM];
};

class FuncPreviewConfig : public ILogicConfig
{
public:
	FuncPreviewConfig();
	virtual ~FuncPreviewConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int GetFuncPreviewCfgNum() const { return m_func_preview_cfg_num; }
	const FuncPreviewCfg* GetFuncPreviewCfg(int notice_id);
private:
	int InitFuncPreviewCfg(TiXmlElement *RootElement);

	int m_func_preview_cfg_num;
	FuncPreviewCfg m_func_preview_cfg_list[MAX_FUNC_PREVIEW_NUM];
};

#endif