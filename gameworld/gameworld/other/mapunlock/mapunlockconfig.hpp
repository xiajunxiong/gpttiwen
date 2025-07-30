#ifndef __MAP_UNLOCK_CONFIG_HPP__
#define __MAP_UNLOCK_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/mapunlockdef.h"

#include <set>
#include <map>
#include <vector>

enum UNLOCK_TYPE		// ��������
{
	MAP_UNLOCK_TYPE_INVALID 	= 0,		// ��Ч������	����1
	MAP_UNLOCK_TYPE_TASK		= 1,		// �������		����ID
	MAP_UNLOCK_TYPE_LEVEL		= 2,		// �ﵽ�ȼ�		����ȼ�
	MAP_UNLOCK_TYPE_PASS_FB		= 3,		// ͨ�ظ���		��������
	MAP_UNLOCK_TYPE_BIG_DIPPER	= 4,		// ͨ�ر������� �Ǿ����
	MAP_UNLOCK_TYPE_FEISHEN		= 5,		// ��ɫ����		�����׶�

	MAP_UNLOCK_TYPE_MAX
};

struct UnlockSceneInfo		// - ��������ID
{
	UnlockSceneInfo() : active_id(0), irregular_id(0), condition_id(0) {}

	int active_id;
	int irregular_id;
	int condition_id;
};

struct UnlockCondition		// �������� - ������������
{
	UnlockCondition() : unlock_type(0), unlock_param(0) {}

	int unlock_type;		// ��������
	int unlock_param;		// ��������
};

class MapUnlockConfig : public ILogicConfig
{
public:
	MapUnlockConfig();
	virtual ~MapUnlockConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const UnlockCondition * GetUnlockCondition(int condition_id);

	void GetConditionIDListByIrregularID(int ir_id, std::set<int> & out_list);
	void GetUnlockInfoBySceneID(int scene_id, std::vector<UnlockSceneInfo> & out_list);
	bool IsCanEnterScene(int scene_id, int role_level);
	int GetActiveID(int scene_id);
	int GetIrregularID(int scene_id);
	int GetMaxIrregularID() { return m_irregular_id_to_condition_id.rbegin()->first; }

private:
	int InitWorldAreasInfo(TiXmlElement * RootElement);
	int InitSceneUnlockCon(TiXmlElement * RootElement);

	std::map<int, int> m_scene_id_to_active_id;							// ����ID ��Ӧ ����ID Ψһӳ���
	std::map<int, int> m_scene_id_to_irregular;							// ����ID ��Ӧ ����ID Ψһӳ���
	std::map<int, std::vector<UnlockSceneInfo> > m_unlock_scene_map;	// ����ID ��Ӧ ������Ϣ ӳ���
	std::map<int, UnlockCondition> m_unlock_condi_map;					// ����ID ��Ӧ �������� ӳ���
	std::map<int, std::set<int> > m_irregular_id_to_condition_id;		// ������ID ��Ӧ ����ID ӳ���
};

#endif