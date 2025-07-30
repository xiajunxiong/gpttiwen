#ifndef __FACE_SCORE_HPP__
#define __FACE_SCORE_HPP__

#include "servercommon/facescoredef.h"
#include "other/attributelist.hpp"

class RoleModuleManager;

class FaceScore			// ��ֵϵͳ
{
public:
	FaceScore();
	~FaceScore();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager * GetRoleModuleManager() { return m_mgr; }

	void Init(RoleModuleManager * role_module_manager, const FaceScoreTimeParam &face_score_time_param,const FaceScoreParam & face_score_param, int buff_len, const char *buff_cur);
	void GetFaceScoreParam(FaceScoreTimeParam *out_time_param,FaceScoreParam * out_param, char * buff_cur, int * buff_len);
	void Update(time_t now_second);

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);

	bool OnWearOrGetOffTitle(int title_id, bool is_wear);		// ����������³ƺ��¼����ػص�,��ֵװ���豣��һ��

	bool AutoOrUseActiveRes(int seq, int item_id, bool is_auto_use = true, bool is_noitce = true);	// ��ȡ�����Զ�ʹ�û��Լ�ʹ��ʱ���øýӿ�
	bool RequestEquips(int seq, bool is_face_wear = true);			// ���� - װ����ֵ��Դ
	bool RequestRemove(int grid_id, bool is_face_getoff = true);	// ���� - ��ж��ֵ��Դ

	void OnGainTitle(int title_id);		// �¼����ػص� - ��óƺ�
	void OnLoseTitle(int title_id);		// ʱ�䴥�ػص� |���¼����ػص� - ʧȥ�ƺ�
	
	void GetFaceEquipList(int face_equip_list[FACE_ADDITION_TYPE_MAX]);
	int GetFaceEquipSeq(int res_type);
	int GetActiveCount(int face_type);
	int GetFaceScoreLevel() { return m_param.face_level; }
	const FaceScoreParam GetFaceScoreParam() { return m_param; }

	void AddFaceScore(int add_value);

	void SendFaceAllInfo();
	void SendFaceTitleChangeNotice();
	void SendResTimeAllInfo();

private:
	RoleModuleManager * m_mgr;
	FaceScoreParam		m_param;
	FaceScoreTimeParam	m_time_param;
	AttributeList		m_attrs_add;

	void OnGainRes(int seq);
	void OnLostRes(int seq, bool is_face_time = false);		//is_face_time: true:��ֵ��Դ��ʱ false:�ƺ��¼���ʱ�䴥�ػص�
	void SendFaceEquipGrid(int grid_id);
	void SendSingleResInfo(int res_id);
	void SendLevelAndExp();
	void SendResTimeInfo(int seq,int time);

	std::map<int, UInt32> m_timeout_res_map;		// ��ʱ�����Ƶ���Դ���� - ��ԴID��ֵ - ��ʱʱ��

	int m_max_level;		// ���ȼ�
	int m_current_exp;		// ��ǰ����
	int m_face_type_count_list[FACE_ADDITION_TYPE_MAX];			//�Ѽ���ĸ�����������
	int m_active_face_count;			// �Ѽ�������
};

#endif