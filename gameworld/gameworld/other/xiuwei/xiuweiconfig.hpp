#ifndef __XIU_WEI_CONFIG_HPP__
#define __XIU_WEI_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/xiuweiparam.hpp"

struct XiuWeiOtherCfg			// ��Ϊ - ����
{
	XiuWeiOtherCfg():cost1(0), cost1_exp(0) , cost2(0) , cost2_exp(0) ,
		use_times(0) , normal_use_times(0), need_feisheng(0){}

	int cost1;					// ��ͨ��Ϊ��ID
	int cost1_exp;				// ��ͨ��Ϊ����������
	int cost2;					// �߼���Ϊ��ID
	int cost2_exp;				// �߼���Ϊ����������
	int use_times;				// �߼���Ϊ��ÿ��ʹ�ô�������
	int normal_use_times;		// �ͼ���Ϊ��ÿ��ʹ�ô�������
	int need_feisheng;			// ������Ҫ�����׶�
};

struct XiuWeiAttrCfg
{
	XiuWeiAttrCfg() :xiuwei_exp(0), dam_add(0), xiaoguo_add(0), dedam_def(0),
		xiaoguo_def(0), hp_add(0), sp_id(0), open_level(0){}

	int xiuwei_exp;				// ��Ϊ�������辭��
	int dam_add;				// �˺��ӳ� [ǧ�ֱ�]
	int xiaoguo_add;			// Ч������ [ǧ�ֱ�]
	int dedam_def;				// �˺����� [ǧ�ֱ�]
	int xiaoguo_def;			// Ч���ֿ� [ǧ�ֱ�]
	int hp_add;					// �����ӳ� [�̶�ֵ]
	int sp_id;

	int open_level;				// �����ȼ�
};

class XiuWeiConfig : public ILogicConfig
{
public:
	XiuWeiConfig();
	virtual ~XiuWeiConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const XiuWeiOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const XiuWeiAttrCfg * GetAttrCfg(int _xiu_wei_type, int _level);
	int GetXiuWeiTypeMaxLevel(int _xiu_wei_type);

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitAttrCfg(TiXmlElement * RootElement, std::string& errormsg);

	XiuWeiOtherCfg	m_other_cfg;
	std::map< int, std::map<int, XiuWeiAttrCfg > > m_attr_cfg_map;	//<��Ϊ���� < �ȼ� ,���� >>
};

#endif	// __XIU_WEI_CONFIG_HPP__
