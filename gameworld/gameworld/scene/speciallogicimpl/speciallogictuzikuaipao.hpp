#ifndef __SPECIAL_TU_ZI_KUAI_PAO_HPP__
#define __SPECIAL_TU_ZI_KUAI_PAO_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>

#include "servercommon/tuzikuaipaodef.hpp"

struct SpecialLogicTuZiKuaiPaoRoleInfo
{
	SpecialLogicTuZiKuaiPaoRoleInfo();

	bool IsSet(int times, bool cheak_ready = false) const;
	void SetDefault(int times);

	int role_id;
	int is_leader;		//没啥用，用生成为用哪个坐标点位置
	int use_carrot_times;
	int set_tu_zi[TU_ZI_KUAI_PAO_SET_TIMES][TU_ZI_KUAI_PAO_MAX_TRACK_NUM];
	int set_tu_zi_ready[TU_ZI_KUAI_PAO_SET_TIMES];
};

enum  TU_ZI_KUAI_PAO_PHASE_TYPE
{
	TU_ZI_KUAI_PAO_PHASE_TYPE_SET1 = 1,
	TU_ZI_KUAI_PAO_PHASE_TYPE_SET2,
	TU_ZI_KUAI_PAO_PHASE_TYPE_READY,
	TU_ZI_KUAI_PAO_PHASE_TYPE_START,
};

class SpecialLogicTuZiKuaiPao : public SpecialLogic
{
public:
	SpecialLogicTuZiKuaiPao(Scene* scene);
	virtual ~SpecialLogicTuZiKuaiPao();

	virtual void Update(unsigned long interval, time_t now_second);
	virtual void UpdateSetInfo(unsigned long interval, time_t now_second);
	virtual void UpdateMatch(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景
	virtual bool CanDestroy();
	
	void Set(Role * role, int(&tu_zi_type)[TU_ZI_KUAI_PAO_MAX_TRACK_NUM]);
	void SetReady(Role * role);
	void Encourage(Role * role,int obj_id,int encourage_type);
	void Leave(Role * role);

	void GmOp(int type, int p1, int p2);
public:
	bool IsFinish();
	bool IsStart();
	void SetTrackResult(int tack_id, int role_id);
	void SendSceneInfo(Role * role = NULL);
	void SendRoleInfo(Role * role);
private:
	void InitSceneInfo();
	void CreateTuZi();
	void Checkfinish();

	bool m_is_finsih;
	bool m_is_create_tu_zi;
	int m_phase;
	time_t m_next_phase_time;
	int m_result[TU_ZI_KUAI_PAO_MAX_TRACK_NUM];
	
	std::map<int, SpecialLogicTuZiKuaiPaoRoleInfo> m_role_info;
	std::map<int, int> m_tu_zi_kuai_pao_phase_time;
};

#endif