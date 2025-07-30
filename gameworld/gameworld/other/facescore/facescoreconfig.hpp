#ifndef __FACE_SCORE_CONFIG_HPP__
#define __FACE_SCORE_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/facescoredef.h"
#include "servercommon/struct/battle/battle_def.hpp"

#include <map>

struct FaceScoreOthers
{
	FaceScoreOthers() : gold_exchange_rate (0) {}

	int gold_exchange_rate;		// Ԫ���һ���ֵ�ҵı�������ʾ 1 Ԫ���ɶһ���ֵ�ҵ�������
};

struct FaceAttrAddInfo
{
	FaceAttrAddInfo() : need_score(0), max_hp_add(0), max_mp_add(0)
	{
		memset(base_add, 0, sizeof(base_add));
		memset(amen_add, 0, sizeof(amen_add));
		memset(resi_add, 0, sizeof(resi_add));
	}

	int need_score;				// ����������Լӳ��������ֵ��
	int max_hp_add;				// Ѫ���޼ӳ�
	int max_mp_add;				// ħ���޼ӳ�
	int base_add[BATTLE_BASE_ATTR_NUM];		// �������������ء�����
	int amen_add[BATTLE_AMEN_ATTR_NUM];		// ����ֵ
	int resi_add[BATTLE_RESI_ATTR_NUM];		// �ֿ�ֵ
};

struct FaceAttrAddInfoProGroup		//��ֵϵͳ�ȼ�����ְҵ��
{
	FaceAttrAddInfoProGroup()
	{}
	
	FaceAttrAddInfo node_cfg[PROFESSION_TYPE_NUM];		//�±�0Ϊ����
};

struct FaceResourceInfo
{
	FaceResourceInfo() : type(0), exp_add(0), title_id(0)
	{}

	int type;				// ���� \see FACE_SCORE_TYPE
	int exp_add;			// �������ֵ����
	int title_id;			// ��type���ǳƺ�ʱ, Ϊ0
	long time;				// ����ʱ�� ��λ s
};

class FaceScoreConfig : public ILogicConfig
{
public:
	FaceScoreConfig();
	virtual ~FaceScoreConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int GetMaxLevel() { return m_face_score_attr_add_map.rbegin()->first; }
	int GetSeqByTitleID(int title_id);
	int GetUplevelNeedExp(int level, int job);
	void InitLevelAndExpByTotalExp(int * out_level, int * out_exp, int job);

	const FaceAttrAddInfo * GetAttrAddCfgByLevel(int level, int job);
	const FaceResourceInfo * GetResInfoCfg(int seq);

private:
	int InitFacesScoresOthersCfg(TiXmlElement * RootElement);
	int InitLevelAttrAdditionCfg(TiXmlElement * RootElement);
	int InitFaceItemsBuyPriceCfg(TiXmlElement * RootElement);

	FaceScoreOthers m_face_others;
	std::map<int, FaceAttrAddInfoProGroup> m_face_score_attr_add_map;
	std::map<int, FaceResourceInfo> m_face_res_info_map;

	// Ԥ����
	std::map<int, int> m_title_to_res_map;			// �ƺ�ID ת�� ��ԴID ӳ�䣺�� - �ƺ�ID��ֵ - ��ԴID
};

#endif