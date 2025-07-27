
#ifndef NAME_CONFIG_HPP
#define NAME_CONFIG_HPP

#include <stdio.h>
#include <vector>
#include <map>
#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "engineadapter.h"
//////////////////////////////////////////////////////////////////////////
// 一：前中后缀分别最大999个，超过999需要将所有的str_id
// 重新调整位数的占用，重新沟通协议。至于性别,只能用女或男
//
// 二：namestrid的构成:（下述"位"表示十进制数左起第X位
// 1.前缀性别标识占第1位, 前缀名字id占第2~4位
// 2.中缀性别标识占第5位, 中缀名字id占第6~8位
// 3.后缀性别标识占第9位, 中缀名字id占第10~12位
// 三：计算namestrid的方法如下：
// 1.初始化：令变量namestrid = 0; 常量CFG_NUM= 1000 常量OFFSET_NUM = 10000
// 2.计算前缀: namestrid += first_sex * CFG_NUM + first_name_id;
// 3.计算中缀: namestrid *= OFFSET_NUM;
//             namestrid += middle_sex * CFG_NUM + middle_name_id;
// 4.计算后缀：namestrid *= OFFSET_NUM;
//             namestrid += last_sex * CFG_NUM + last_name_id;
//////////////////////////////////////////////////////////////////////////
static const int MAX_NAME_CFG_NUM = 1000;
static const int OFFSET_NUM = 10000;

enum FIRST_TYPE
{
	FEMALE_FIRST = 0,					// 女性的前缀
	MALE_FIRST = 1,						// 男性的前缀
	FIRST_TYPE_MAX,
};

enum MIDDLE_TYPE
{
	FEMALE_MIDDLE = 0,					// 女性的中缀
	MALE_MIDDLE = 1,					// 男性的中缀
	MIDDLE_TYPE_MAX,
};

enum LAST_TYPE
{
	FEMALE_LAST = 0,					// 女性的后缀
	MALE_LAST = 1,						// 男性的后缀
	LAST_TYPE_MAX,
};

static const int TYPE_COUNT = FIRST_TYPE_MAX + MIDDLE_TYPE_MAX + LAST_TYPE_MAX;
static const int NAME_TYPE = 3;
static const int CHAR_LEN = 32;

UNSTD_STATIC_CHECK(6 == TYPE_COUNT && 3 == NAME_TYPE);

struct NameCfg
{
	NameCfg() {}

	struct Name
	{
		int name_id;
		GameName name_str;
	};

	struct NameList
	{
		std::vector<Name> name_str_list;
	};

	NameList first_name[FIRST_TYPE_MAX];
	NameList middle_name[MIDDLE_TYPE_MAX];
	NameList last_name[LAST_TYPE_MAX];
};

class NameConfig
{
public:
	static NameConfig * Instance();
	bool Init(const std::string &path, const std::string &configname, std::string *err);
	int InitNameCfg(TiXmlElement* RootElement);
	const NameCfg & GetNameCfg() const{ return m_name_cfg; }

	bool GetRandomName(int sex, GameName out_name);

private:
	int InitNameCfgImpl(TiXmlElement* RootElement, NameCfg::NameList& name_list, const char* sheet_name);

	NameCfg m_name_cfg;
};

#endif