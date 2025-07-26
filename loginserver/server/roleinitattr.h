#ifndef ROLEINITATTR_H
#define ROLEINITATTR_H

#include "servercommon/struct/roleinitparam.h"

#include <string.h>

class RoleInitAttr
{
public:
	static RoleInitAttr & Instance();

	static const int NEWBIE_SCENE_NUM = 16;

	struct RoleInitScene
	{
		RoleInitScene() : scene_id(0), scene_x(0), scene_y(0), range(0), last_scene_id(0), last_scene_x(0), last_scene_y(0), is_newbie_plot_open(0), is_auto_get_shizhuang(0){}
		int scene_id;
		int scene_x;
		int scene_y;
		int range;
		int last_scene_id;
		int last_scene_x;
		int last_scene_y;
		int is_newbie_plot_open;
		int is_auto_get_shizhuang;
	};

	bool Init(const std::string& path, const std::string& configname, std::string* err);

	const RoleInitParam & GetInitParam(char avatar_type, char color, int plat_spid);
	bool SetNewbiePlotOpenClose(int is_open);
	bool SetNewbieAutoGetShizhuang(int flag);

private:
	RoleInitAttr();
	~RoleInitAttr();

	RoleInitParam m_role_init_attr;

	RoleInitScene m_newbie_scene[NEWBIE_SCENE_NUM];
	unsigned short m_scenecount;
};

#endif
