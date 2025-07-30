#ifndef __ANIMAL_RACE_MANAGER_HPP__
#define __ANIMAL_RACE_MANAGER_HPP__

#include "servercommon/struct/global/animalracedef.hpp"

class AnimalRaceManager
{
	friend class RMIInitAnimalRaceBackObjectImpl;

public:
	static AnimalRaceManager & Instance();
	
	void OnServerStart();
	void OnServerStop();
	bool LoadAnimalRaceData(long long id_from);

	void Update(time_t now_second);

	bool ISLoadFinish() { return m_load_flag; }

	void Init(const AnimalRaceParam &param);
	void GetInitParam(AnimalRaceParam * param);
	void ClearDirtyMark(AnimalRaceParam *param);

	bool GetChangeFlag() { return m_change_flag; }

	bool GetWinNumberAndResponse(int server_id, int date_id, int *win_number, int *response);

	void OnAddAnimalRaceRecord(AnimalRaceEntry & param);
	void OnRoleAddBetUpdateRecord(AnimalRaceEntry & param);

	int GetNewDateId(int server_id);
	AnimalRaceEntry * GetCurIsNew(time_t now_second);

	void ClearAnimalRaceItem(int index);
	void ClearAllAnimalRaceRecord();
private:
	AnimalRaceManager();
	~AnimalRaceManager();

	void Save(const AnimalRaceParam * param = NULL);
	
	void LoadAnimalRaceSucc();

	AnimalRaceParam m_param; 

	time_t m_animal_race_next_save_time;

	std::map<int, bool> m_dirty_mark;
	std::map<int, bool> m_old_state;

	bool m_change_flag;
	bool m_load_flag;



};




#endif