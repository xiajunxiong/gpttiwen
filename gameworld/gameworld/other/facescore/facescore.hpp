#ifndef __FACE_SCORE_HPP__
#define __FACE_SCORE_HPP__

#include "servercommon/facescoredef.h"
#include "other/attributelist.hpp"

class RoleModuleManager;

class FaceScore			// 颜值系统
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

	bool OnWearOrGetOffTitle(int title_id, bool is_wear);		// 由佩戴或脱下称号事件触控回调,颜值装备需保持一致

	bool AutoOrUseActiveRes(int seq, int item_id, bool is_auto_use = true, bool is_noitce = true);	// 获取道具自动使用或自己使用时调用该接口
	bool RequestEquips(int seq, bool is_face_wear = true);			// 请求 - 装备颜值资源
	bool RequestRemove(int grid_id, bool is_face_getoff = true);	// 请求 - 拆卸颜值资源

	void OnGainTitle(int title_id);		// 事件触控回调 - 获得称号
	void OnLoseTitle(int title_id);		// 时间触控回调 |　事件触控回调 - 失去称号
	
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
	void OnLostRes(int seq, bool is_face_time = false);		//is_face_time: true:颜值资源超时 false:称号事件或时间触控回调
	void SendFaceEquipGrid(int grid_id);
	void SendSingleResInfo(int res_id);
	void SendLevelAndExp();
	void SendResTimeInfo(int seq,int time);

	std::map<int, UInt32> m_timeout_res_map;		// 有时间限制的资源：键 - 资源ID、值 - 超时时间

	int m_max_level;		// 最大等级
	int m_current_exp;		// 当前经验
	int m_face_type_count_list[FACE_ADDITION_TYPE_MAX];			//已激活的各种类型数量
	int m_active_face_count;			// 已激活总数
};

#endif