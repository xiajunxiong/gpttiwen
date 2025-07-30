#ifndef __ROLE_ACTIVITY_SEVEN_COLOR_PICTURE_HPP__
#define __ROLE_ACTIVITY_SEVEN_COLOR_PICTURE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rasevencolorpictureparam.hpp"

class RoleActivitySevenColorPicture : public RoleActivity
{
public:
	RoleActivitySevenColorPicture(int type);
	virtual ~RoleActivitySevenColorPicture();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityMakeUpReq(Protocol::CSRandActivityMakeUpReq * req);

	void OnRASevenColorPictureOp(int type, int picture_index, char seven_color_picture_pigment[][SEVEN_COLOR_PIGMENT_TYPE_MAX], char * msg);
	bool OnAddPigmentByItem(int use_num);

	void SendRASevenColorTypeInfo(int color_index, int pigment_index);
public:
	void SendRASevenColorPicutreInfo();
	void OnRASevenColorPictureUsePigment(int picture_index, char seven_color_picture_pigment[][SEVEN_COLOR_PIGMENT_TYPE_MAX], char * msg);
	void OnRASevenColorPictureReward(int picture_index);
	void OnRASevenColorPictureTotalReward();

	RASevenColorPictureParam m_param;
};

#endif	//__ROLE_ACTIVITY_SEVEN_COLOR_PICTURE_HPP__
