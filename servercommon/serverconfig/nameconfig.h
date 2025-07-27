
#ifndef NAME_CONFIG_HPP
#define NAME_CONFIG_HPP

#include <stdio.h>
#include <vector>
#include <map>
#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "engineadapter.h"
//////////////////////////////////////////////////////////////////////////
// һ��ǰ�к�׺�ֱ����999��������999��Ҫ�����е�str_id
// ���µ���λ����ռ�ã����¹�ͨЭ�顣�����Ա�,ֻ����Ů����
//
// ����namestrid�Ĺ���:������"λ"��ʾʮ�����������Xλ
// 1.ǰ׺�Ա��ʶռ��1λ, ǰ׺����idռ��2~4λ
// 2.��׺�Ա��ʶռ��5λ, ��׺����idռ��6~8λ
// 3.��׺�Ա��ʶռ��9λ, ��׺����idռ��10~12λ
// ��������namestrid�ķ������£�
// 1.��ʼ���������namestrid = 0; ����CFG_NUM= 1000 ����OFFSET_NUM = 10000
// 2.����ǰ׺: namestrid += first_sex * CFG_NUM + first_name_id;
// 3.������׺: namestrid *= OFFSET_NUM;
//             namestrid += middle_sex * CFG_NUM + middle_name_id;
// 4.�����׺��namestrid *= OFFSET_NUM;
//             namestrid += last_sex * CFG_NUM + last_name_id;
//////////////////////////////////////////////////////////////////////////
static const int MAX_NAME_CFG_NUM = 1000;
static const int OFFSET_NUM = 10000;

enum FIRST_TYPE
{
	FEMALE_FIRST = 0,					// Ů�Ե�ǰ׺
	MALE_FIRST = 1,						// ���Ե�ǰ׺
	FIRST_TYPE_MAX,
};

enum MIDDLE_TYPE
{
	FEMALE_MIDDLE = 0,					// Ů�Ե���׺
	MALE_MIDDLE = 1,					// ���Ե���׺
	MIDDLE_TYPE_MAX,
};

enum LAST_TYPE
{
	FEMALE_LAST = 0,					// Ů�Եĺ�׺
	MALE_LAST = 1,						// ���Եĺ�׺
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