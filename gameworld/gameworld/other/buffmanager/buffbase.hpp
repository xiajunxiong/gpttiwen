#ifndef __BUFF_BASE_HPP__
#define __BUFF_BASE_HPP__

#include "servercommon/buffdef.hpp"

class RoleModuleManager;
class BuffBase
{
public:
	BuffBase(RoleModuleManager * role_module_mgr, unsigned int begin_time, unsigned int end_time, int origin = 0);
	virtual ~BuffBase();

	void* operator new(size_t c);
	void operator delete(void* m);

	// 获得buff类型
	virtual int GetBuffType() = 0;
	// 施加效果
	virtual bool Effect() = 0;
	// 移除buff
	virtual bool Move() = 0;
	// 是否保存buff
	virtual void SetSave(bool is_save) { m_is_save = is_save; }

	virtual void SetLeaveSceneClear(bool is_clear) { m_is_leave_clear = is_clear; }
	virtual void SetSceneBuffType(int fb_type) { m_fb_type = fb_type; };

	int GetSceneBuffType() { return m_fb_type; }
	bool IsSave() { return m_is_save; }
	bool IsClearScene() { return m_is_leave_clear; }
	bool IsOvertime(time_t now_second);
	bool GetParam(BuffNode* param);
	int GetOrigin() { return m_origin; }
protected:
	RoleModuleManager * m_role_module_mgr;

	unsigned int m_begin_time;
	unsigned int m_end_time;
	int m_origin;
	bool m_is_save;
	bool m_is_leave_clear;
	int m_fb_type;
	NetValueBuffParam m_data;
};

class AttributeList;
class BuffChangeAttr : public BuffBase
{
public:
	BuffChangeAttr(RoleModuleManager * role_module_mgr, unsigned int begin_time, unsigned int end_time, int origin = 0);
	~BuffChangeAttr();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual int GetBuffType() { return ROLE_BUFF_CHANGE_ATTR; }

	virtual bool Effect();
	virtual bool Move();

	void AddAttr(int type, int value);
	void ReCalcAttr(AttributeList &base_add);

protected:


};

class BuffExpUp : public BuffBase
{
public:
	BuffExpUp(RoleModuleManager * role_module_mgr, unsigned int begin_time, unsigned int end_time, int origin = 0);
	~BuffExpUp();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual int GetBuffType() { return ROLE_BUFF_EXP_UP; }

	virtual bool Effect();
	virtual bool Move();

	void SetExpUpPer(int per);
protected:


};
class BuffChangeMoveSpeed : public BuffBase
{
public:
	BuffChangeMoveSpeed(RoleModuleManager * role_module_mgr, unsigned int begin_time, unsigned int end_time, int origin = 0);
	~BuffChangeMoveSpeed();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual int GetBuffType() { return ROLE_BUFF_CHANGE_MOVE_SPEED; }

	virtual bool Effect();
	virtual bool Move();

	void SetMoveSpeed(int change_per);
protected:


};
#endif // !__BUFF_BASE_HPP__


