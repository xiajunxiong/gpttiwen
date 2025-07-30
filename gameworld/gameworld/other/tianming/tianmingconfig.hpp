#ifndef __TIAN_MING_CONFIG_HPP__
#define __TIAN_MING_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/tianmingparam.h"

struct TianMingCfg
{
	TianMingCfg()
	{
		need_exp = 0;
		att_type = 0;
		att_per = 0;
		att_per_tol = 0;
	}

	int need_exp;			// �������õȼ�����ľ���
	int att_type;			// �ӳɵ�ϵͳ����
	int att_per;			// �õȼ��ļӳ� , ����ֵ (Ӧ���ò��� ,�ȶ���)
	int att_per_tol;		// �ܼӳ� , ָ��ϵͳ���͵ĸ����Լӳɵ��õȼ�ʱ���ܼӳ�ֵ
};

// -----------------------------------------------------------------------------------
class TianMingConfig : public ILogicConfig
{
public:
	TianMingConfig();
	virtual ~TianMingConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const TianMingCfg * GetTianMingCfg(int _level);

private:
	int InitTianMingCfg(TiXmlElement * RootElement);

	TianMingCfg  m_tian_ming_cfg_arr[MAX_TIAN_MING_LEVEL_COUNT]; 

};

#endif	// __TIAN_MING_CONFIG_HPP__
