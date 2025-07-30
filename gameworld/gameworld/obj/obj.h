
#ifndef __OBJ_H__
#define __OBJ_H__

#include <vector>
#include "gamedef.h"

class Scene;
class Obj
{
public:
	enum _ObjType
	{
		// ��������ʱ��IsCharacter��������ͬʱ�޸�
		OBJ_TYPE_INVALID,
		OBJ_TYPE_ROLE,					//!< 1 ��ɫ
		OBJ_TYPE_NPC_MONSTER,			//!< 2 NPC - ����
		OBJ_TYPE_NPC_GATHERS,			//!< 3 NPC - �ɼ���
		OBJ_TYPE_SPECIAL_TRANSPORT,		//!< 4 ������
		OBJ_TYPE_TREASURE_CHEST,		//!< 5 ����
		OBJ_TYPE_NPC_FUNC,				//!< 6 NPC - ������NPC | �ɷ��������ɵ�NPC���ˡ��ֽ�ʦ�����ǹ�����NPC��
		OBJ_TYPE_ROBOT,					//!< 7 ������
		OBJ_TYPE_NPC_CHOSED,			//!< 8 NPC - ����NPC
		OBJ_TYPE_XUN_YOU_CAR,			//!< 9 Ѳ�λ���
		OBJ_TYPE_PEAK_TOKEN_OBJ,		//!< 10 ���������� ����
		OBJ_TYPE_TU_ZI_KAI_PAO_TU_ZI,		//!< 11 ���ӿ��ܻ�� ����

		OBJ_TYPE_COUNT
	};

	Obj(ObjType type);
	Obj(ObjType type, ObjID obj_id, const GameName _name);
	virtual	~Obj();

	void		SetId(ObjID gid) { m_obj_id = gid; }
	inline ObjID GetId() { return m_obj_id; }
	inline ObjType GetObjType() { return m_obj_type; }
	UInt32		GetSingleMgrIndex() { return m_single_mgr_index; }
	void		SetSingleMgrIndex(UInt32 index) { m_single_mgr_index = index; }
	void		SetPos(const Posi &pos) { m_posi = pos; m_posf.Init((float)pos.x, (float)pos.y); }
	inline const Posi& GetPos() const{ return m_posi; }
	inline const Posf& GetPosf() const { return m_posf; }
	void		GetName(GameName & name) const;
	inline const char* GetName() { return m_name; }
	void		SetName(GameName name);
	void		SetScene(Scene *scene) { m_scene = scene; }
	inline Scene* GetScene() { return m_scene; }

	inline Dir  GetDir() { return m_dir; }
	inline Scalar GetDirDistance() { return m_dir_distance; }
	inline UInt32 GetObserHandle() { return m_obser_handle; }

	virtual void OnEnterScene();
	virtual void OnLeaveScene();

	virtual void OnAOIEnter(ObjID obj_id){};
	virtual void OnAOILeave(ObjID obj_id){};

	virtual void Update(unsigned long interval, time_t now_second, unsigned int now_dayid) {}

	static bool IsCharacter(ObjType type);
	static bool IsRandPos(ObjType type);

	void		SetInvalid() { m_valid = false; }
	bool		IsValid() { return m_valid; }

protected:
	const ObjType	m_obj_type;
	ObjID		m_obj_id;

	GameName	m_name;					// ����
	Scene		*m_scene;				// ���ڳ�����id
	UInt32		m_single_mgr_index;
	Posf		m_posf;					// Ŀǰ�ľ�ȷλ�ã������˶�����
	Posi		m_posi;					// Ŀǰ��λ�ã������߼�����
	Dir			m_dir;					// ����
	Scalar		m_dir_distance;			// �������
	short		m_height;				// �߶ȣ����ã�

	UInt32		m_obser_handle;
	
	bool        m_valid;				// �����Ƿ���Ч 
};

#endif


