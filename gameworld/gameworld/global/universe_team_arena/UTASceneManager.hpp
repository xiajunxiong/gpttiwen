#ifndef __UTA_SCENE_MANAGER_HPP__
#define __UTA_SCENE_MANAGER_HPP__

struct UTAScheduleShadow;
class Scene;
class SpecialLogicUTA;
class UTAShadow;
class UTASceneManager
{
public:
	UTASceneManager(UTAShadow* uta_shadow);
	~UTASceneManager();

	Scene* CreateSceneForAttacker(const UTAScheduleShadow& schedule, unsigned int start_battle_timestamp, unsigned int scene_timeout_timestamp);
	Scene* CreateSceneForDefender(const UTAScheduleShadow& schedule, unsigned int start_battle_timestamp, unsigned int scene_timeout_timestamp);

	bool HasCreateSceneForAttacker(const UTAScheduleShadow& schedule);
	bool HasCreateSceneForDefender(const UTAScheduleShadow& schedule);

	SpecialLogicUTA* GetSpecialLogic(int scene_key);
	SpecialLogicUTA* GetSpecialLogic(int plat_type, int uid, const UTAScheduleShadow& schedule);
	SpecialLogicUTA* GetSpecialLogic(long long schedule_id, long long unique_team_id);
private:
	bool HasCreateScene(int scene_key);
	Scene* CreateScene(int scene_key);

	UTAShadow* m_shadow;
};

#endif