#ifndef __APPEARANCE_CONFIG_HPP__
#define __APPEARANCE_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"

struct HeadshotCfg
{
	HeadshotCfg() : headshot_id(0), avatar_type(0), type(0),
		advance_times(0), surface_id(0)
	{

	}

	enum COND_TYPE
	{
		COND_TYPE_ADVANCE_TIMES = 1,
		COND_TYPE_SHIZHUANG = 2,
	};

	short headshot_id;
	short avatar_type;	// 绑定的角色模型[1,6]
	short type;			// 解锁类型 1 进阶  2 时装
	short advance_times;// 进阶次数
	int surface_id;		// 时装ID
};

class AppearanceConfig
{
public:
	typedef std::vector<int> ColorVec;
	typedef std::vector<int> JobVec;
	typedef std::vector<int> AvatarVec;
	typedef std::map<int, AvatarVec> ProfessionAvatarMap;
	typedef std::map<int, HeadshotCfg> HeadshotCfgMap;
public:
	static AppearanceConfig& Instance();

	bool Init(const std::string &path, const std::string &configname, std::string *err);
	bool Reload(const std::string &path, const std::string &configname, std::string * err);
	bool IsColorMatchAvatar(int avatar_type, int advance_times, int color);
	bool CheckAvatarAndColor(int avatar_type, int color, int job_id);

	JobVec * GetJobVec(int avatar_type,int advancement_level)
	{
		if (avatar_type <= AVATAR_TYPE_INVALID || avatar_type >= AVATAR_TYPE_MAX) return NULL;
		if (advancement_level <= 0 || advancement_level > PROFESSION_ADVANCE_TIMES)return NULL;
		return &m_avatar_job_list[avatar_type][advancement_level];
	}
	int RandProfessionByAvatarType(int avatar_type, int advance_level);
	int RandAvatarByProfession(int profession);

	const HeadshotCfg* GetHeadshotCfg(short headshot_id);
	
	int GetSexByAvatar(int avatar_type);
private:
	AppearanceConfig();
	~AppearanceConfig();
	AppearanceConfig(const AppearanceConfig&); 
	AppearanceConfig& operator= (const AppearanceConfig&);

	int InitAppearanceCfg(TiXmlElement* RootElement);
	int InitChangeImageJobCfg(TiXmlElement* RootElement);
	int InitHeadshotCfg(TiXmlElement* RootElement);
	int InitRoleCfg(TiXmlElement* RootElement);

	ColorVec m_avatar_color_list[AVATAR_TYPE_MAX][PROFESSION_ADVANCE_TIMES + 1];

	JobVec m_avatar_job_list[AVATAR_TYPE_MAX][PROFESSION_ADVANCE_TIMES + 1];

	ProfessionAvatarMap m_profession_avatar_map;
	int m_avatar_sex_list[AVATAR_TYPE_MAX];				//模型->性別 index:avatar_type  value:sex

	HeadshotCfgMap m_headshot_cfg_map;
};

#endif