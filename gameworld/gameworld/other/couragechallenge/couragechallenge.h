#ifndef _COURAGE_CHALLENGE_HPP_
#define _COURAGE_CHALLENGE_HPP_

#include "servercommon/couragechallengedef.hpp"

class Role;
class RoleModuleManager;

struct CourageChallengeKey
{
	CourageChallengeKey(): layer_level(0), level_seq(0) {}
	CourageChallengeKey(short _layer_level, char _seq): layer_level(_layer_level), level_seq(_seq) {}

	bool operator< (const CourageChallengeKey& index) const
	{
		if (layer_level != index.layer_level)
		{
			return layer_level < index.layer_level;
		}
		else
		{
			return level_seq < index.level_seq;
		}
	}

	short layer_level;		// �ֲ�ȼ���
	char level_seq;			// �÷ֲ�ȼ����е�seq
};

class CourageChallenge
{
public:
	typedef std::map<CourageChallengeKey, unsigned char> LevelInfoMap;
	typedef std::map<CourageChallengeKey, char> DirtyGroupMap;

public:
	CourageChallenge();
	~CourageChallenge();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const CourageChallengeParam & param, const RoleCourageChallengeLevelParamList & param_list);					//��ó�ʼ��Param
	void GetInitParam(CourageChallengeParam * param, RoleCourageChallengeLevelParamList * list);
	void ClearDirtyMark();
	void OnResetData(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLogin();

	void SendPassReward(int monster_group_id, unsigned char flag);
	void SetLayerLevelCondition(unsigned char flag, CourageChallengeKey level_key);

	void OnRoleOpenUI();
	void FetchEveryDayReward(int layer_level, int layer_level_seq);
	void FetchMaxScoreReward(int layer_level);		//��ȡ�ò����Ǵ�
	int GetLayerScoreNum(int layer_level);			//��øò��ѻ�õ���������
	int GetReachStarScoreCount();					//��ô�ʱ�Ѵ�ɵ�����������
	int GetPassMaxLayerLevel();						//�����Ҵ�ʱ�������ߵĵȼ���
	int GetPassMaxSeq();							//��ȡ�����ͨ�ص���ߵĹؿ����seq
	bool IsFinishCurLayer(int layer_level);			

	void UpdateRoleRank(bool is_change = false);							//���������ħ֮��������Ϣ

	void SendAllInfo();
	void SendSingleInfo(CourageChallengeKey level_key);
	void SendScoreSingleInfo(int layer_level);
	inline int GetPetHelperNum() const { return m_param.pet_helper_num; }
private:
	bool IsInvalid(CourageChallengeKey level_key);
	bool IsInvalid(int layer_level, int level_seq);
	bool IsScoreInvalid(int layer_level);
	ScoreLayerLevelNode & GetScoreNode(int score_index);
	void CalculatePetHelperNum();
	bool UpdatePetHelperNum(int pet_helper_num);
	void SendPetHelperNumUpdateNotice();

	RoleModuleManager * m_role_module_mgr;
	CourageChallengeParam m_param;

	//std::map<CourageChallengeKey, short> m_data_node_map;			// second��Ӧ��ʱparam��data_node�����±�
	//std::map<int, short> m_layer_level_star_map;					// keyΪ�����ȼ��� second��Ӧ��ʱparam��score_data_node�����±�

	ScoreLayerLevelNode score_data[MAX_COURAGE_CHALLENGE_SCORE_NUM];		// ��½�������������
	LevelInfoMap m_level_data_map;											// �ؿ�����
	DirtyGroupMap m_dirty_group_map;
	DirtyGroupMap m_snap_dirty_group_map;
};


#endif 
