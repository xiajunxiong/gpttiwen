#ifndef __ACTIVITY_CONFIG_HPP__
#define __ACTIVITY_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"



struct ActivitySignUpCfg					//��ʱ�����
{
	ActivitySignUpCfg():seq(0), activity_id(0), open(0)
	{}

	int seq;								//����
	int activity_id;						//�ID
	int open;								//0:�ر� 1:����
	std::vector<ItemConfigData> item_list;	//���������б�
};

struct ActivityJoinRewardCfg						//��ʱ����뽱��
{
	ActivityJoinRewardCfg() :min_level(0), max_level(0), exp(0), coin(0)
	{}

	int min_level;									// ��͵ȼ�
	int max_level;									// ���ȼ�

	int exp;										// ���齱��
	int coin;										// ͭ�ҽ���
	std::vector<ItemConfigData> reward_vec;			// ���߽���
};

class ActivityConfig : public ILogicConfig
{
public:
	ActivityConfig();
	virtual ~ActivityConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActivitySignUpCfg* GetSignUpCfg(int seq);
	const ActivitySignUpCfg* GetSignUpCfgById(int activity_id);
	const ActivityJoinRewardCfg * GetJoinRewardCfg(int _activity_type, int _level);

private:
	int InitActivitySignUpCfg(TiXmlElement *RootElement);
	int InitActivityJoinRewardCfg(TiXmlElement *RootElement);

	std::vector<ActivitySignUpCfg> m_sign_up_cfg;					//��ʱ�����
	std::map<int, ActivitySignUpCfg> m_sign_up_by_id_map;			//��ʱ����� �ԻidΪkey

	std::map<int, std::vector<ActivityJoinRewardCfg> >	m_join_reward_map;	// < ��� , ������Ϣ>
};

#endif
