#ifndef __TITLE_CONFIG_HPP__
#define __TITLE_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/titledef.hpp"
#include "other/attributelist.hpp"
#include <map>
#include <vector>
#include <set>


struct TitleCfg
{
	TitleCfg()
	{
		source_type = 0;
		title_id = 0;
		time = 0;
		memset(attrs, 0, sizeof(attrs));
	}

	int source_type;
	int title_id;
	int time;		// ����������
	Attribute attrs[BATTLE_ATTR_MAX];
};

enum AUTO_TITLE_TYPE
{
	AUTO_TITLE_TYPE_INVALID = 0,
	AUTO_TITLE_TYPE_1 = 1,				//�ﵽָ����������				����1����������
	AUTO_TITLE_TYPE_2 = 2,				//ͨ������֮�ز���				����1������      ����2: �ò�����boss���   ͨ�صĻ�����2����BOSS���������1
	AUTO_TITLE_TYPE_3 = 3,				//�μӵ����׽쾺���Կ���		����1������
	AUTO_TITLE_TYPE_4 = 4,				//�����ɹ�����					����1����������
	AUTO_TITLE_TYPE_5 = 5,				//�������ս������				����1������ս������
	AUTO_TITLE_TYPE_6 = 6,				//һ��֮�����̳�ʹ��Ԫ������	����1��ʹ��Ԫ������
	AUTO_TITLE_TYPE_7 = 7,				//�ۼƳ�ֵԪ������				����1����ֵԪ������
	AUTO_TITLE_TYPE_8 = 8,				//����3�ֱ����ۼƵ���ָ������	����1�������������
	AUTO_TITLE_TYPE_9 = 9,				//�ۼ�Ѱ������					����1��Ѱ������
	AUTO_TITLE_TYPE_10 = 10,			//����ָ���ļ��ܵȼ�			����1������ID  ����2�����ܵȼ�

	AUTO_TITLE_TYPE_MAX,
};

struct AutoTitleCfg
{
	AutoTitleCfg()
	{
		cond_type = AUTO_TITLE_TYPE_INVALID;
		title_id = 0;
		cond_param1 = 0;
		cond_param2 = 0;
		cond_param3 = 0;
		cond_param4 = 0;
	}
	int cond_type;
	int title_id;
	int cond_param1;
	int cond_param2;
	int cond_param3;
	int cond_param4;
};

class TitleConfig : public ILogicConfig
{
public:
	TitleConfig();
	virtual ~TitleConfig();
	virtual bool Init(const std::string &configname, std::string *err);

	const TitleCfg* GetTitleCfg(int seq);
	const AutoTitleCfg* GetAutoTitleCfg(int seq);
	const AutoTitleCfg* CheckAutoTitleFirst(int *seq,int conditon_type,int param1 = 0,int param2 = 0,int param3 = 0,int param4 = 0);//��鷵�ص�һ���ƺŵ�seq
	std::set<int> GetNotSaveTitle();
	static bool IsNeedSaveTitleType(int title_type);
private:
	int InitTitleListCfg(TiXmlElement *RootElement);
	int InitAutoTitleListCfg(TiXmlElement *RootElement);

	std::map<int, TitleCfg> m_title_map;
	std::map<int, AutoTitleCfg> m_auto_title_map;
	std::set<int> m_not_save_title_id;
};

#endif // !__COLLECTION_CONFIG_HPP__
